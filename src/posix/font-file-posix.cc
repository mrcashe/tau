// ----------------------------------------------------------------------------
// Copyright © 2014-2022 Konstantin Shmelkov <mrcashe@gmail.com>.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include <posix/font-face-posix.hh>
#include <posix/font-file-posix.hh>
#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/locale.hh>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>

namespace {

inline uint32_t u32(const char * b) {
    return uint8_t(b[3])|(uint32_t(uint8_t(b[2])) << 8)|(uint32_t(uint8_t(b[1])) << 16)|(uint32_t(uint8_t(b[0])) << 24);
}

inline uint16_t u16(const char * b) {
    return uint8_t(b[1])|(uint16_t(uint8_t(b[0])) << 8);
}

uint32_t checksum(const char * b, uint32_t size) {
    uint32_t cs = 0;

    while (size) {
        cs += u32(b);
        b += 4;
        size = size > 4 ? size-4 : 0;
    }

    return cs;
}

} // anonymous namespace

namespace tau {

class Font_file_impl: public Font_file {
    struct Entry {
        uint32_t        ofs;
        uint32_t        len;
        uint32_t        cs;
    };

    struct Horz_header {
        int16_t         ascent = 0;
        int16_t         descent = 0;
        int16_t         linegap = 0;
        int16_t         max_advance = 0;
        int16_t         min_lsb = 0;
        int16_t         min_rsb = 0;
        int16_t         max_x_extent = 0;
        bool            caret_slope_rise = false;
        bool            caret_slope_run = false;
        uint16_t        rcount = 0;
    };

    struct Horz_metrics {
        uint16_t        adv = 0;
        int16_t         lsb = 0;
    };

    struct Loca {
        uint32_t        ofs;
        std::streamsize len;
    };

    using Char_map = std::map<char32_t, uint16_t>;
    using Loca_table = std::vector<Loca>;
    using Horz_table = std::vector<Horz_metrics>;
    using Entries = std::map<ustring, Entry>;

    Entries             entries_;
    ustring             path_;
    ustring             family_;
    ustring             facename_;
    ustring             fontname_;
    ustring             psname_;
    unsigned            upm_ = 0;               // Units per EM.
    bool                baseline0_ = false;     // Baseline at Y = 0.
    bool                lsb_ = false;           // Left sidebearing at X = 0.
    bool                idepend_ = false;       // Instructions may depend on point size.
    bool                pforce_= false;         // Force PPEM...
    bool                ialter_ = false;        // Instructions may alter advance.
    bool                loca32_ = false;        // Long LOCA str_format.
    unsigned            gcount_ = 0;            // Glyph count.
    Rect                bbox_;                  // Font bounding box.
    Char_map            cmap_;                  // Char code -> glyph index.
    Loca_table          loca_;                  // Glyph location table.
    Horz_header         hhea_;                  // Horizontal header.
    Horz_table          hmtx_;                  // Horizontal metrics.

public:

    Font_file_impl(const ustring & fp):
        path_(fp)
    {
        std::ifstream is(Locale().encode_filename(path_), std::ios::binary);

        char hdr[12];
        is.read(hdr, 12);

        if (12 != is.gcount() || 0x00010000 != u32(hdr)) {
            throw bad_font(str_format(path_notdir(path_), ": bad header"));
        }

        uint16_t ntables = u16(hdr+4);
        if (0 != ntables) {
            std::size_t nbytes = 16*ntables;
            char b[nbytes];
            is.read(b, nbytes);

            if (nbytes != std::size_t(is.gcount())) {
                throw bad_font(str_format(path_notdir(path_), ": corrupted header"));
            }

            for (uint16_t n = 0; n < ntables; ++n) {
                std::size_t index = 16*n;
                ustring tag = str_toupper(str_trimright(ustring(b+index, 4)));
                Entry ent;
                ent.cs = u32(b+index+4);
                ent.ofs = u32(b+index+8);
                ent.len = ~3 & (3+u32(b+index+12));
                entries_[tag] = ent;
            }
        }

        load_name(is);
        load_head(is);
    }

    ustring file_path() const override {
        return path_;
    }

    std::vector<ustring> list_families() const override {
        std::vector<ustring> v;
        if (!family_.empty()) { v.push_back(family_); }
        return v;
    }

    std::vector<ustring> list_faces(const ustring & family) const override {
        std::vector<ustring> v;
        if (!family_.empty() && !facename_.empty() && family == family_) { v.push_back(facename_); }
        return v;
    }

    Font_face_ptr face(Font_file_ptr file, const ustring & family, const ustring & face) override {
        std::ifstream is(Locale().encode_filename(path_), std::ios::binary);
        preload(is);
        auto zero = std::make_shared<Master_glyph>();
        load_glyph(is, 0, zero);

        Font_face_ptr ff = std::make_shared<Font_face>(file, zero);
        ff->set_family(family_);
        ff->set_facename(facename_);
        ff->set_fontname(fontname_);
        ff->set_psname(psname_);
        ff->set_bounds(bbox_);
        ff->set_upm(upm_);
        ff->set_ascent(hhea_.ascent);
        ff->set_descent(hhea_.descent);
        ff->set_linegap(hhea_.linegap);
        ff->set_max_advance(hhea_.max_advance);
        ff->set_min_lsb(hhea_.min_lsb);
        ff->set_min_rsb(hhea_.min_rsb);
        ff->set_max_x_extent(hhea_.max_x_extent);
        ff->set_caret_slope_rise(hhea_.caret_slope_rise);
        ff->set_caret_slope_run(hhea_.caret_slope_run);
        return ff;
    }

    std::vector<Master_glyph_ptr> glyphs(const ustring & family, const ustring & face, const std::u32string & str) override {
        std::vector<Master_glyph_ptr> gs;

        if (family == family_ && face == facename_ && !str.empty()) {
            std::ifstream is(Locale().encode_filename(path_), std::ios::binary);
            preload(is);

            for (char32_t wc: str) {
                uint16_t gindex = glyph_index(wc);

                if (0 != gindex) {
                    auto master = std::make_shared<Master_glyph>();
                    load_glyph(is, gindex, master);
                    gs.push_back(master);
                }

                else {
                    gs.push_back(Master_glyph_ptr());
                }
            }
        }

        return gs;
    }

    Master_glyph_ptr glyph(const ustring & family, const ustring & face, char32_t wc) override {
        if (family == family_ && face == facename_) {
            std::ifstream is(Locale().encode_filename(path_), std::ios::binary);
            preload(is);
            uint16_t gindex = glyph_index(wc);

            if (0 != gindex) {
                auto master = std::make_shared<Master_glyph>();
                load_glyph(is, gindex, master);
                return master;
            }
        }

        return nullptr;
    }

private:

    double conv_2_dot_14(int32_t u) {
        return (static_cast<double>(u & 0xc000)/16384)+(static_cast<double>(u & 0x3fff)/16384);
    }

    // ----------------------------------------------------------------------------
    // http://stackoverflow.com/questions/20733790/truetype-fonts-glyph-are-made-of-quadratic-bezier-why-do-more-than-one-consecu
    //
    // TTF parsing requires applying http://www.microsoft.com/typography/otspec/glyf.htm
    // as well as the tech docs about the TTF str_format from the microsoft site. These tell
    // us that there are two types of points for a curve: on-curve and off-curve points.
    // on-curve points are "real" points, through which a curve passes, and off-curve points
    // are control points that guide the bezier curvature.
    // Now, what you describe as "a bezier curve" is correct: a single bezier curve goes
    // from 1 real point, guided by 1 control point, to 1 real point. However, quadratic
    // curves are generally crap for design work because they are really bad at approximating
    // circular arcs, but they're also much cheaper to work with than cubic curves, so we're
    // stuck with them for fonts with truetype outlines. To get around this, TTF outlines
    // generally use sequences of bezier curves to get decent looking uniform curves, and
    // those sequences tend to have a nice property: the on- and off-curve points are spaced
    // in a very particular pattern.
    //
    // Consider this Bezier sequence:
    // P1 - C1 - P2 - C2 - P3 - C3 - P4
    //
    // If we add the on instr_formation, we'd encode it in TTF as:
    // P1 - C1 - P2 - C2 - P3 - C3 - P4
    // 1  -  0 -  1 -  0 -  1 -  0 -  1
    //
    // Now for the trick: if each Pn is an on-curve point, and each Cn is a control point,
    // and P2 lies exactly midway between C1 and C2, P3 lies between C2 and C3, and so on,
    // then this is a compactable curve: if we know C1 and C2, we know P2, so we don't have
    // to list it explicitly, we can just leave that up to whatever parses the glyph outline.
    // So TTF will encode long bezier sequences for uniform curves as:
    // P1 - C1 - C2 - C3 - P4
    // 1 -  0 -  0 -  0 -  1
    //
    // saving considerable space, without loss of precision. If you look at your TTX dump, you'll
    // see this reflected in the on values for each point. To get the P2, P3, etc, all we do is this:
    // foreach(array as point):
    // do we have an implied on-curve point?
    //      if(mask_for_point == 0 && mask_for_previous_point === 0):
    //              missing_point = midpoint(point, previous_point)
    //              points.push(missing_point)
    //
    // add the explicitly encoded point
    //      points.push(point)
    //
    // after running this procedure, the points array will have alternating on-curve and off-curve points,
    // and the beziers are constructed as
    //      for(i=0, i<arr.length, i+=2):
    //      curve(array[i], array[i+1], array[i+2])
    //
    // edit after a bit of searching, http://chanae.walon.org/pub/ttf/ttf_glyphs.htm covers how to work with
    // the glyf table data in pretty good detail (the ascii graphics are a bit silly, but still legible enough).
    // ----------------------------------------------------------------------------

    void load_glyph(std::istream & is, uint16_t gindex, Master_glyph_ptr master) {
        if (gindex >= gcount_) {
            throw bad_font(str_format(path_notdir(path_), ": missing required glyph index ", gindex));
        }

        // First, collect horizontal glyph metrics.
        if (gindex < hmtx_.size()) {
            Horz_metrics & hm = hmtx_[gindex];
            master->set_advance(hm.adv);
            master->set_lbearing(hm.lsb);
        }

        else {
            std::cerr << "** " << path_notdir(path_) << ": missing HMTX entry for glyph index " << gindex << std::endl;
        }

        // Bounds will be collected later in case glyph has outlines.
        int16_t xmin = 0, xmax = 0, ymin = 0, ymax = 0;

        // loca.ofs = offset to glyph from the beginning of GLYF table.
        // loca.len = GLYF table element size for this glyph.
        Loca & loca = loca_[gindex];

        // Some glyphs, like space (U+0020) has no outlines at all.
        // In that case loca.len will be == 0.
        if (0 != loca.len) {
            auto ei = entries_.find("GLYF");

            if (ei == entries_.end()) {
                throw bad_font(str_format(path_notdir(path_), ": missing GLYF table"));
            }

            Entry & ent = ei->second;
            uint32_t fofs = ent.ofs+loca.ofs;
            is.seekg(fofs, std::ios::beg);

            if (is.tellg() != fofs) {
                throw bad_font(str_format(path_notdir(path_), ": seek() to GLYF table failed"));
            }

            char b[loca.len];
            is.read(b, loca.len);

            if (loca.len != is.gcount()) {
                throw bad_font(str_format(path_notdir(path_), ": read() failed on GLYF table"));
            }

            xmin = u16(b+2), ymin = u16(b+4), xmax = u16(b+6), ymax = u16(b+8);
            master->set_bounds(Point(xmin, ymin), Point(xmax, ymax));

            // Number of contours.
            // If negative, we have a compound glyph.
            int16_t nc = u16(b);

            // Simple glyph.
            if (nc > 0) {
                uint16_t ofs = 10+nc+nc;

                if (ofs >= loca.len) {
                    std::cerr << "** " << path_notdir(path_) << ": n_contours = " << nc << ", LOCA32 = " << loca32_ << std::endl;
                    throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for header: ", ofs, " >= ", loca.len));
                }

                uint16_t ilen = u16(b+ofs);             // Instruction length, in bytes.
                uint16_t npts = 1+u16(b+ofs-2);         // Number of points.
                uint8_t  flags[npts];
                int16_t  x[npts], y[npts];

                // Extract flags[] array.
                ofs = 12+nc+nc+ilen;

                for (uint16_t fi = 0; fi < npts; ) {
                    if (ofs >= loca.len) {
                        std::cerr << "** " << path_notdir(path_) << ": n_contours = " << nc << ", ilen = " << ilen << ", LOCA32 = " << loca32_ << std::endl;
                        throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for flags: ", ofs, " >= ", loca.len));
                    }

                    uint8_t fb = b[ofs++];
                    uint8_t nrep = 1;

                    if (0x08 & fb) {
                        if (ofs >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for flag rep: ",  ofs, " >= ", loca.len)); }
                        nrep += b[ofs++];
                    }

                    while (fi < npts && nrep--) {
                        flags[fi++] = fb;
                    }
                }

                // Extract x[] array.
                int16_t vv = 0;

                for (uint16_t cpi = 0; cpi < npts; ++cpi) {
                    int16_t v = 0;

                    // 0x02 -> the corresponding x-coordinate is 1 byte long.
                    if (0x02 & flags[cpi]) {
                        if (ofs >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for x-byte: ",  ofs, " >= ", loca.len)); }
                        uint8_t c = b[ofs++];
                        if (0x10 & flags[cpi]) { v += c; }
                        else { v -= c; }
                        vv += v;
                    }

                    else if (!(0x10 & flags[cpi])) {
                        if (ofs+1 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for x-word: ",  ofs+1, " >= ", loca.len)); }
                        v = u16(b+ofs);
                        ofs += 2;
                        vv += v;
                    }

                    x[cpi] = vv;
                }

                // Extract y[] array.
                vv = 0;
                for (uint16_t cpi = 0; cpi < npts; ++cpi) {
                    int16_t v = 0;

                    // 0x04 -> the corresponding y-coordinate is 1 byte long.
                    if (0x04 & flags[cpi]) {
                        if (ofs >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for y-byte: ",  ofs, " >= ", loca.len)); }
                        uint8_t c = b[ofs++];
                        if (0x20 & flags[cpi]) { v += c; }
                        else { v -= c; }
                        vv += v;
                    }

                    else {
                        if (!(0x20 & flags[cpi])) {
                            if (ofs+1 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": GLYF: insufficient space for y-word: ",  ofs+1, " >= ", loca.len)); }
                            v = u16(b+ofs);
                            ofs += 2;
                            vv += v;
                        }
                    }

                    y[cpi] = vv;
                }

                // Bezier generating loop.
                if (0x01 & flags[0]) {                      // 1st point must be ON contour.
                    for (uint16_t cpi = 0; cpi < npts;) {   // cpi - current point index.

                        // Loop over contours.
                        for (int16_t ic = 0; ic < nc; ++ic) {
                            bool on = true;                 // Last point state (ON, OFF).
                            uint16_t lpi = u16(b+10+ic+ic); // Index of the last point within contour.

                            if (lpi >= npts) {
                                throw bad_font(str_format(path_notdir(path_), ": last point index exceeds total point count: ", lpi, " >= ", npts));
                            }

                            Point first(x[cpi], y[cpi]);    // Fetch first point.
                            Point cur = first;              // Current point.
                            Point cp;                       // Control point for conic curves.
                            Contour gctr(first);

                            // Loop over points starting with [1] index.
                            for (++cpi; cpi <= lpi; ++cpi) {
                                Point xy(x[cpi], y[cpi]);

                                // ON to ON -> emit linear curve.
                                // OFF to ON -> emit conic curve.
                                // OFF to OFF -> emit compactable curve.
                                // ON to OFF -> fix control point.
                                if (0x01 & flags[cpi]) {
                                    if (on) { gctr.line_to(xy); }
                                    else { gctr.conic_to(cp, xy); }
                                    cur = xy;
                                    on = true;
                                }

                                else {
                                    if (!on) {
                                        Point e( (cp.x()+xy.x())/2, (cp.y()+xy.y())/2 );
                                        gctr.conic_to(cp, e);
                                        cur = e;
                                    }

                                    cp = xy;
                                    on = false;
                                }
                            }

                            if (cur != first) { gctr.line_to(first); }
                            master->add_contour(gctr);
                        }
                    }
                }
            }

            // Compound glyph.
            else if (nc < 0) {
                uint16_t ofs = 10;
                uint16_t flags;

                do {
                    if (ofs+3 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": Compound GLYF: insufficient space for header: ", ofs+3, " >= ", loca.len)); }
                    flags = u16(b+ofs); ofs += 2;
                    uint16_t sub_index = u16(b+ofs); ofs += 2;
                    bool enable_merge = true;

                    // Recursive call!
                    auto sub_master = std::make_shared<Master_glyph>();
                    load_glyph(is, sub_index, sub_master);

                    uint16_t arg0, arg1;

                    // Arguments 1 and 2 are words.
                    if (0x0001 & flags) {
                        if (ofs+3 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": Compound GLYF: insufficient space for word-args: ", ofs+3, " >= ", loca.len)); }
                        arg0 = u16(b+ofs); ofs += 2;
                        arg1 = u16(b+ofs); ofs += 2;
                    }

                    // Arguments 1 and 2 are bytes.
                    else {
                        if (ofs+1 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": Compound GLYF: insufficient space for byte-args: ", ofs+1, " >= ", loca.len)); }
                        arg0 = b[ofs++];
                        arg1 = b[ofs++];
                    }

                    // Arguments are XY values.
                    if (0x0002 & flags) {
                        int16_t dx = arg0, dy = arg1;

                        if (0 != dx || 0 != dy) {
                            // ROUND_XY_TO_GRID.
                            // If set, round the xy values to grid.
                            // If not set do not round xy values to grid (relevant only to bit 1 is set)
                            //if (0x0004 & flags) {
                            //     NOT IMPLEMENTED!
                            //}

                            sub_master->translate(Vector(dx, dy));
                        }
                    }

                    // Otherwise, they are points.
                    else {
                        std::cerr << "!! FIXME: load_glyph(): " << path_ << "\n";
                        std::cerr << "!! FIXME: load_glyph(): Subglyph positioning by points not yet implemented.\n";
                        std::cerr << "!! FIXME: load_glyph(): Subglyph #" << gindex << " will not be merged.\n";
                        enable_merge = false;
                    }

                    // We have a scale.
                    // If set, there is a simple scale for the component in 2.14 str_format.
                    // If not set, scale is 1.0.
                    if (0x0008 & flags) {
                        if (ofs+1 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": Compound GLYF: insufficient space for 2.14 scale: ", ofs+1, " >= ", loca.len)); }
                        double s = conv_2_dot_14(u16(b+ofs)); ofs += 2;

                        if (enable_merge) {
                            std::cerr << "!! FIXME: load_glyph(): " << path_ << "\n";
                            std::cerr << "!! FIXME: load_glyph(): Simple subglyph scaling is untested.\n";
                            std::cerr << "!! FIXME: load_glyph(): Subglyph #" << gindex << " has scale of " << s << ".\n";
                            sub_master->scale(Vector(s, s));
                        }
                    }

                    // We have X and Y scale.
                    // If set the x direction will use a different scale than the y direction in 2.14 str_format.
                    else if (0x0040 & flags) {
                        if (ofs+3 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": Compound GLYF: insufficient space for XY scale: ", ofs+3, " >= ", loca.len)); }
                        double sx = conv_2_dot_14(u16(b+ofs)); ofs += 2;
                        double sy = conv_2_dot_14(u16(b+ofs)); ofs += 2;

                        if (enable_merge) {
                            std::cerr << "!! FIXME: load_glyph(): " << path_ << "\n";
                            std::cerr << "!! FIXME: load_glyph(): Subglyph scaling is untested.\n";
                            std::cerr << "!! FIXME: load_glyph(): Subglyph #" << gindex << " has scale of " << sx << ":" << sy << ".\n";
                            sub_master->scale(Vector(sx, sy));
                        }
                    }

                    // We have 2x2.
                    // If set there is a 2-by-2 transstr_formation that will be used to scale the component in 2.14 str_format.
                    else if (0x0080 & flags) {
                        if (ofs+7 >= loca.len) { throw bad_font(str_format(path_notdir(path_), ": Compound GLYF: insufficient space for 2x2 scale: ", ofs+7, " >= ", loca.len)); }
                        double xx = conv_2_dot_14(u16(b+ofs)); ofs += 2;
                        double xy = conv_2_dot_14(u16(b+ofs)); ofs += 2;
                        double yx = conv_2_dot_14(u16(b+ofs)); ofs += 2;
                        double yy = conv_2_dot_14(u16(b+ofs)); ofs += 2;

                        if (enable_merge) {
                            std::cerr << "!! FIXME: load_glyph(): " << path_ << "\n";
                            std::cerr << "!! FIXME: load_glyph(): 2x2 subglyph scaling is untested.\n";
                            std::cerr << "!! FIXME: load_glyph(): Subglyph #" << gindex << " has scale of " << xx << ":" << xy << ":" << yx << ":" << yy << ".\n";
                            std::cerr << "!! FIXME: load_glyph(): Subglyph #" << gindex << " will not be merged.\n";
                        }

                        enable_merge = false;
                    }

                    if (enable_merge) {
                        // -------------------------------------------------------------------------------
                        // https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
                        //
                        // Use metrics from this component for the compound glyph.
                        // The purpose of USE_MY_METRICS is to force the lsb and rsb to take on a
                        // desired value. For example, an i-circumflex (ï, U+00ef) is often composed
                        // of the circumflex and a dotless-i. In order to force the composite to have the
                        // same metrics as the dotless-i, set USE_MY_METRICS for the dotless-i
                        // component of the composite. Without this bit, the rsb and lsb would be
                        // calculated from the HMTX entry for the composite (or would need to be
                        // explicitly set with TrueType instructions).
                        // Note that the behavior of the USE_MY_METRICS operation is undefined for
                        // rotated composite components.
                        // --------------------------------------------------------------------------------
                        // FIXME About rsb to use.

//                         if (0x0200 & flags) {
//                             int adv = sub_master->advance();
//                             int lsb = sub_master->lbearing();
//                             Rect bounds = sub_master->bounds();
//                             master->set_advance(adv);
//                             master->set_lbearing(lsb);
//                             master->set_bounds(bounds.top_left(), bounds.bottom_right());
//                         }

                        master->merge(*sub_master);
                    }

                } while (flags & 0x0020);
            }
        }
    }

    uint16_t glyph_index(char32_t wc) {
        auto iter = cmap_.find(wc);
        return iter != cmap_.end() ? iter->second : 0;
    }

    void preload(std::ifstream & is) {
        if (0 == gcount_) {
            load_maxp(is);
            hmtx_.resize(gcount_);
        }

        if (0 == hhea_.rcount) {
            load_hhea(is);
            load_hmtx(is);
        }

        if (cmap_.empty()) {
            load_cmap(is);

            if (cmap_.empty()) {
                throw bad_font(str_format(path_notdir(path_), ": unicode character table not found"));
            }
        }

        if (loca_.empty()) {
            load_loca(is);
        }
    }

    void load_maxp(std::istream & is) {
        auto ei = entries_.find("MAXP");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing MAXP table"));
        }

        Entry & ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to MAXP table"));
        }

        char b[ent.len];
        is.read(b, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read MAXP table"));
        }

        // Table version (+0).
        uint32_t ww = u32(b);
        if (0x00010000 != ww) {
            throw bad_font(str_format("MAXP table version ", std::hex, std::setw(8), std::setfill('0'), ww, " unsupported"));
        }

        gcount_ = u16(b+4);
        //uint16_t max_points = u16(b+6);
        //uint16_t max_contours = u16(b+8);
        //uint16_t max_composite_points = u16(b+10);
        //uint16_t max_composite_contours = u16(b+12);
        //uint16_t max_zones = u16(b+14);
        //uint16_t max_twilight_points = u16(b+16);
        //uint16_t max_storage = u16(b+18);
        //uint16_t max_function_defs = u16(b+20);
        //uint16_t max_inst_defs = u16(b+22);
        //uint16_t max_stack = u16(b+24);
        //uint16_t max_sizeof = u16(b+26);          // Maximum byte count for glyph instruction.
        //uint16_t max_cmp_elements = u16(b+28);    // Maximum number of components referenced at top level for any composite glyph.
        //uint16_t max_cmp_depth = u16(b+30);       // Maximum level of recursion: 1 for simple component.
    }

    // Horizontal metrics table.
    void load_hmtx(std::istream & is) {
        auto ei = entries_.find("HMTX");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing HMTX table"));
        }

        auto ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to HMTX table"));
        }

        char b[ent.len];
        is.read(b, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read HMTX table"));
        }

        uint32_t hmto = 0;              // horz long metrics current offset, bytes.
        uint32_t hmtl;                  // horz long metrics limit, bytes.
        uint32_t lsbo = 0;              // left side bearing current offset, bytes.
        uint32_t lsbl = 0;              // left side bearing limit, bytes.
        uint16_t adv = 0;
        int16_t  lsb = 0;
        uint16_t n_lsb = gcount_-hhea_.rcount;
        hmtl = 4*static_cast<uint32_t>(hhea_.rcount);

        for (uint16_t gindex = 0; gindex < gcount_; ++gindex) {
            if (hmto < hmtl) {
                adv = u16(b+hmto);
                lsb = u16(b+hmto+2);
                hmto += 4;
            }

            else if (0 != n_lsb) {
                if (0 == lsbo) {
                    lsbo = hmtl;
                    lsbl = lsbo+2*static_cast<uint32_t>(n_lsb);
                }

                if (lsbo < lsbl) {
                    lsb = u16(b+lsbo);
                    lsbo += 2;
                }
            }

            Horz_metrics & hm = hmtx_[gindex];
            hm.adv = adv;
            hm.lsb = lsb;
        }
    }

    // Horizontal header table.
    void load_hhea(std::istream & is) {
        auto ei = entries_.find("HHEA");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing HHEA table"));
        }

        auto ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to HHEA table"));
        }

        char hhea[ent.len];
        is.read(hhea, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read HHEA table"));
        }

        // Table version (+0).
        uint32_t ww = u32(hhea);

        if (0x00010000 != ww) {
            throw bad_font(str_format(path_notdir(path_), ": HHEA table version ", std::hex, std::setw(8), std::setfill('0'), ww, " unsupported"));
        }

        // HMTX metric data str_format (0 for current str_format) (+32).
        int16_t i = u16(hhea+32);

        if (0 != i) {
            throw bad_font(str_format(path_notdir(path_), ": HHEA metric data str_format ", std::hex, std::setw(4), std::setfill('0'), i, " unsupported"));
        }

        hhea_.ascent = u16(hhea+4);                        // Ascender (+4)
        hhea_.descent = u16(hhea+6);                       // Descender (+6)
        hhea_.linegap = u16(hhea+8);                       // Line gap (+8).
        hhea_.max_advance = u16(hhea+10);                  // Advance width max (+10).
        hhea_.min_lsb = u16(hhea+12);                      // Min left side bearing (+12).
        hhea_.min_rsb = u16(hhea+14);                      // Min right side bearing (+14).
        hhea_.max_x_extent = u16(hhea+16);                 // Max x extent (lsb+xmax-xmin) (+16).
        hhea_.caret_slope_rise = 0 != u16(hhea+18);        // Caret slope rise (1 for vertical) (+18).
        hhea_.caret_slope_run = 0 != u16(hhea+20);         // Caret slope run (0 for vertical) (+20).
        hhea_.rcount = u16(hhea+34);                       // Number of horizontal metrics (HMTX table #records) (+34).
    }

    // Glyph location table.
    void load_loca(std::istream & is) {
        auto ei = entries_.find("LOCA");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing LOCA table"));
        }

        Entry & ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to LOCA table"));
        }

        char b[ent.len];
        is.read(b, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read LOCA table"));
        }

        uint32_t cs = checksum(b, ent.len);

        if (cs != ent.cs) {
            throw bad_font(str_format(path_notdir(path_), ": checksum mismatch for LOCA table, (0x",
                                      std::hex, std::setw(8), std::setfill('0'), cs, " != 0x", std::setw(8), ent.cs, ")"));
        }

        ei = entries_.find("GLYF");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing GLYF table"));
        }

        uint32_t glyf_len = ei->second.len;
        uint32_t len = loca32_ ? 4*gcount_ : 2*gcount_; // LOCA table size.
        uint32_t top, ofs = 0;
        Loca loca;

        for (uint16_t index = 0; index < gcount_; ++index) {
            if (loca32_) {
                loca.ofs = u32(b+ofs);
                ofs += 4;
                top = ofs < len ? u32(b+ofs) : glyf_len;
            }

            else {
                uint32_t o = u16(b+ofs);
                loca.ofs = o << 1;
                ofs += 2;

                if (ofs < len) {
                    o = u16(b+ofs);
                    top = o << 1;
                }

                else {
                    top = glyf_len;
                }
            }

            loca.len = top-loca.ofs;
            loca_.push_back(loca);
        }
    }

    // Character map table.
    void load_cmap(std::istream & is) {
        auto ei = entries_.find("CMAP");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing CMAP table"));
        }

        Entry & ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to CMAP table"));
        }

        char b[ent.len];
        is.read(b, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read CMAP table"));
        }

        if (0 != u16(b)) {
            throw bad_font(str_format(path_notdir(path_), ": CMAP table version ",
                                      std::hex, std::setw(4), std::setfill('0'), u16(b), " unsupported"));
        }

        uint16_t n_cmap = u16(b+2);

        if (0 == n_cmap) {
            throw bad_font(str_format(path_notdir(path_), ": no character tables found"));
        }

        uint32_t ofs = 4;

        // Using str_format 4 cmap.
        for (uint16_t n = 0; n < n_cmap; ++n, ofs += 8) {
            uint32_t subt = u32(b+ofs+4);
            uint16_t str_format = u16(b+subt);

            if (4 == str_format) {
                //uint16_t len = u16(b+subt+2);
                uint16_t nsegs = u16(b+subt+6)/2;
                uint32_t iend = subt+14;
                uint32_t istart = iend+2+nsegs+nsegs;
                uint32_t idelta = istart+nsegs+nsegs;
                uint32_t ioffset = idelta+nsegs+nsegs;
                uint32_t iarray = ioffset+nsegs+nsegs;

                for (uint16_t seg = 0; seg < nsegs; ++seg) {
                    uint16_t endc = u16(b+iend+seg+seg);
                    uint16_t startc = u16(b+istart+seg+seg);
                    int16_t delta = u16(b+idelta+seg+seg);
                    int16_t offset = u16(b+ioffset+seg+seg);

                    for (uint16_t wc = startc; wc < 0xffff && wc <= endc; ++wc) {
                        uint16_t gindex;

                        if (0 == offset) {
                            gindex = wc+delta;
                        }

                        else {
                            uint16_t i = wc+offset-startc;
                            gindex = u16(b+iarray+i+i);
                        }

                        if (gindex <= gcount_) {
                            cmap_[wc] = gindex;
                        }
                    }
                }

                break;
            }
        }
    }

    void load_head(std::ifstream & is) {
        auto ei = entries_.find("HEAD");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing HEAD table"));
        }

        Entry & ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to HEAD table"));
        }

        char b[ent.len];
        is.read(b, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read HEAD table"));
        }

        // Table version number.
        uint32_t ww = u32(b);

        if (0x00010000 != ww) {
            throw bad_font(str_format(path_notdir(path_), ": HEAD tabel version 0x",
                                      std::hex, std::setw(8), std::setfill('0'), ww, " unsupported"));
        }

        // Magic number (+12).
        ww = u32(b+12);
        if (0x5f0f3cf5 != ww) {
            throw bad_font(str_format(path_notdir(path_), ": HEAD table magic number mismatch (0x",
                                      std::hex, std::setw(8), std::setfill('0'), ww, " provided)"));
        }

        // Glyph data str_format (+52), must be 0.
        int16_t gstr_format = u16(b+52);
        if (0x0000 != gstr_format) {
            throw bad_font(str_format(path_notdir(path_), ": HEAD table glyph data str_format 0x",
                                      std::hex, std::setw(8), std::setfill('0'), gstr_format, " unsupported"));
        }

        // Font revision (+4, set by font manufacturer).
        //uint32_t font_rev = u32(b+4);

        // Check sum adjustment (+8)
        //uint32_t cs_adj = u32(b+8);

        // Flags (+16).
        uint16_t flags = u16(b+16);
        baseline0_ = flags & 0x01;  // 0x01 -> baseline for font at y = 0.
        lsb_ = flags & 0x02;   // 0x02 -> left sidebearing at x = 0.
        idepend_ = flags & 0x04;    // 0x04 -> instructions may depend on point size.
        pforce_ = flags & 0x08;     // 0x08 -> force ppem for interger values...
        ialter_ = flags & 0x10;     // 0x10 -> instructions may alter advance...

        // Units per EM (+18).
        upm_ = u16(b+18);

        if (upm_ < 16 || upm_ > 16384) {
            throw bad_font(str_format(path_notdir(path_), ": Units per EM (", upm_, ") is out of range"));
        }

        // Created (+20, 8bytes).
        // Modified (+28, 8bytes).

        // xmin, ymin, xmax, ymax (+36).
        int16_t xmin = u16(b+36);
        int16_t ymin = u16(b+38);
        int16_t xmax = u16(b+40);
        int16_t ymax = u16(b+42);
        bbox_.set(Point(xmin, ymin), Point(xmax, ymax));

        // Mac style: 0x01 -> BOLD, 0x02 -> ITALIC (+44).
        //uint16_t mac_style = u16(b+44);

        // Smallest readable size in pixels (+46).
        //uint16_t lowest_ppem = u16(b+46);

        // Font direction hint (+48): 0 -> mixed, 1 -> LTR, 2 ->LTR(+Neutrals), -1 -> RTL, -2 -> RTL(+Neutrals).
        //int16_t direction = u16(b+48);

        // Short (0) or long (1) LOCA str_format (+50).
        int16_t locaf = u16(b+50);
        loca32_ = (0 != locaf);
    }

    void load_name(std::ifstream & is) {
        auto ei = entries_.find("NAME");

        if (ei == entries_.end()) {
            throw bad_font(str_format(path_notdir(path_), ": missing NAME table"));
        }

        Entry & ent = ei->second;
        is.seekg(ent.ofs, std::ios::beg);

        if (is.tellg() != ent.ofs) {
            throw bad_font(str_format(path_notdir(path_), ": failed to seek to NAME table"));
        }

        char b[ent.len];
        is.read(b, ent.len);

        if (ent.len != std::size_t(is.gcount())) {
            throw bad_font(str_format(path_notdir(path_), ": failed to read NAME table"));
        }

        ustring fam8,  fam16;
        ustring face8, face16;
        ustring name8, name16;

        // Format selector must be always 0.
        uint32_t ww = u16(b);
        if (0 != ww) {
            throw bad_font(str_format(path_notdir(path_), ": NAME table str_format selector 0x",
                                      std::hex, std::setw(8), std::setfill('0'), ww, " unsupported"));
        }

        // Number of name records.
        uint16_t nrec = u16(b+2);

        // Offset to start of string storage (from start of table).
        uint32_t storage = u16(b+4);

        // Name records from offset+6, nrec total, 12 bytes size each.
        uint32_t index = 6;
        for (uint16_t nr = 0; nr < nrec; ++nr, index += 12) {
            // Platform ID.
            uint16_t plat_id = u16(b+index);

            // Plaform specific encoding ID.
            uint16_t pse = u16(b+index+2);

            // Language ID.
            //uint16_t lang_id = u16(b+index+4);

            // Name ID.
            uint16_t name_id  = u16(b+index+6);

            // String offset from start of storage area (in bytes).
            uint16_t ofs = storage+u16(b+index+10);

            // String length (in bytes).
            uint16_t len = u16(b+index+8);

            // The platform identifier
            // The supported platform identifier (platformID) codes are given in the following table.
            // PlatformID codes have been assigned for Unicode, Macintosh, and Microsoft.
            // PlatformID codes 240 through 255 have been reserved for user-defined
            // platforms and are not available for registration. Names with platformIDs
            // other than 0, 1, and 2 are ignored on OS X and iOS.
            //
            // Platform ID      Platform        Platform-specific ID
            // ---------------------------------------------------------------
            //  0               Unicode         Indicates Unicode version.
            //  1               Macintosh       QuickDraw Script Manager code.
            //  2               (reserved; do not use)
            //  3               Microsoft       Microsoft encoding.
            //
            // The platform ID 2 was originally to use with ISO/IEC 10646, but that use is now
            // deprecated, as ISO/IEC 10646 and Unicode have identical character code assignments.

            // Macintosh.
            if (1 == plat_id && 0 == pse) {
                std::size_t slen = strnlen(b+ofs, ent.len-ofs);

                if (slen < len) {
                    //std::cerr << path_ << "\n: len=" << len << " ent.len=" << ent.len << " " << slen << " " << (const char *)(b+ofs) << "\n";
                    // std::cerr << "** " << path_ << ": Macintosh: invalid name record at 0x" << std::hex <<
                    //     ent.ofs+ofs << std::dec << " given length is " << len << ", available " << slen << ", skip this record" << std::endl;
                }

                else {
                    ustring s(b+ofs, len);

                    switch (name_id) {
                        case 1: fam8 = s; break;
                        case 2: face8 = s; break;
                        case 4: name8 = s; break;
                        case 6: psname_ = s; break;
                    }
                }
            }

            // Microsoft unicode (UTF-16).
            // Microsoft platform-specific encoding ID’s (platform ID = 3)
            // Code     Description
            // 0        Undefined character set or indexing scheme
            // 1        UGL character set with Unicode indexing scheme
            else if (3 == plat_id && (0 == pse || 1 == pse)) {
                ustring s = read_utf16(b+ofs, len);

                switch (name_id) {
                    case 1: fam16 = s; break;
                    case 2: face16 = s; break;
                    case 4: name16 = s; break;
                }
            }
        }

        if (!fam8.empty()) {
            family_ = str_trim(fam8);
        }

        else if (!fam16.empty()) {
            family_ = str_trim(fam16);
        }

        else {
            throw bad_font(str_format(path_notdir(path_), ": Family name not found"));
        }

        if (!face8.empty()) {
            facename_ = str_trim(face8);
        }

        else if (!face16.empty()) {
            facename_ = str_trim(face16);
        }

        else {
            facename_ = "Regular";
        }

        if (!name8.empty()) {
            fontname_ = name8;
        }

        else if (!name16.empty()) {
            fontname_ = name16;
        }

        else {
            fontname_ = "Regular";
        }
    }

    ustring read_utf16(const char * p, std::size_t nbytes) {
        char16_t sur = 0;
        ustring  s;

        while (nbytes > 1) {
            char16_t wc = u16(p);
            p += 2;
            nbytes = nbytes > 1 ? nbytes-2 : 0;

            if (0 == sur) {
                if (char16_is_surrogate(wc)) {
                    sur = wc;
                }

                else {
                    s += static_cast<char32_t>(wc);
                }
            }

            else {
                if (char16_is_surrogate(wc)) {
                    s += char32_from_surrogate(sur, wc);
                }

                sur = 0;
            }
        }

        return s;
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Font_file_ptr Font_file::create(const ustring & fp) {
    return std::make_shared<Font_file_impl>(fp);
}

} // namespace tau

//END
