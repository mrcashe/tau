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

#include <tau/timeval.hh>
#include <glyph-impl.hh>
#include "font-win.hh"
#include <cstring>
#include <iostream>
#include <mutex>

namespace {

using Mutex = std::recursive_mutex;
using Lock = std::lock_guard<Mutex>;

Mutex                     mx_;
std::vector<tau::ustring> families_;
tau::ustring              normal_;
double                    normal_pts_ = 10.0;

inline uint32_t u32(const char * b) {
    return uint8_t(b[3])|(uint32_t(uint8_t(b[2])) << 8)|(uint32_t(uint8_t(b[1])) << 16)|(uint32_t(uint8_t(b[0])) << 24);
}

inline uint16_t u16(const char * b) {
    return uint8_t(b[1])|(uint16_t(uint8_t(b[0])) << 8);
}

tau::ustring read_utf16(const char * p, std::size_t nbytes) {
    char16_t sur = 0;
    tau::ustring  s;

    while (nbytes > 1) {
        char16_t wc = u16(p);
        p += 2;
        nbytes -= 2;

        if (0 == sur) {
            if (tau::char16_is_surrogate(wc)) {
                sur = wc;
            }

            else {
                s += static_cast<char32_t>(wc);
            }
        }

        else {
            if (tau::char16_is_surrogate(wc)) {
                s += tau::char32_from_surrogate(sur, wc);
            }

            sur = 0;
        }
    }

    return s;
}

int CALLBACK lister(const LOGFONTW * lf, const TEXTMETRICW * tm, DWORD ftype, LPARAM lp) {
    if (TRUETYPE_FONTTYPE == ftype) {
        tau::ustring fam = tau::str_from_wstring(std::wstring(lf->lfFaceName));
        auto * v = reinterpret_cast<std::vector<tau::ustring> *>(lp);
        if (v->end() == std::find(v->begin(), v->end(), fam)) { v->push_back(fam); }
    }

    return 1;
}

void load_families() {
    LOGFONTW lf;
    std::memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;

    if (HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL)) {
        EnumFontFamiliesExW(hdc, &lf, lister, LPARAM(&families_), 0);
        DeleteDC(hdc);
    }
}

tau::ustring font_spec_from_logfont(const LOGFONT & lf) {
    tau::ustring spec;

    if (HFONT hfont = CreateFontIndirect(&lf)) {
        if (HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL)) {
            HFONT old_font = SelectFont(hdc, hfont);
            TEXTMETRIC tm;

            if (GetTextMetrics(hdc, &tm)) {
                int pt = 0;
                if (lf.lfHeight < 0) { pt = -MulDiv(lf.lfHeight+tm.tmInternalLeading, 72, GetDeviceCaps(hdc, LOGPIXELSY)); }
                else if (lf.lfHeight > 0) { pt = MulDiv(lf.lfHeight, 72, GetDeviceCaps(hdc, LOGPIXELSY)); }
                spec = tau::font_spec_build(lf.lfFaceName, "Regular", pt);
            }

            SelectFont(hdc, old_font);
            DeleteDC(hdc);
        }

        DeleteFont(hfont);
    }

    return spec;
}

} // anonymous namespace

namespace tau {

// ----------------------------------------------------------------------------
// Font class static methods.
// ----------------------------------------------------------------------------

ustring Font::normal() {
    Lock lk(mx_);

    if (normal_.empty()) {
        if (families_.empty()) { load_families(); }
        ustring good_fonts = "Trebuchet MS:Tahoma:Times New Roman:Arial:Noto Sans:Droid Sans:DejaVu Sans";

        NONCLIENTMETRICS ncm;
        std::memset(&ncm, 0, sizeof ncm);
        ncm.cbSize = sizeof ncm;

        if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof ncm, &ncm, FALSE)) {
            ustring spec = font_spec_from_logfont(ncm.lfMessageFont);

            if (!spec.empty()) {
                ustring fam = font_family_from_spec(spec);

                if (str_similar(fam, families_)) {
                    good_fonts.insert(0, fam+":");
                    double pts = font_size_from_spec(spec);
                    if (pts > 10.0) { normal_pts_ = pts; }
                }
            }
        }

        for (const ustring & s: str_explode(good_fonts, ':')) {
            if (str_similar(s, families_)) {
                normal_ = s;
                break;
            }
        }

        if (normal_.empty() && !families_.empty()) {
            normal_ = families_.front();
        }
    }

    return font_spec_build(normal_, "Regular", normal_pts_);
}

std::vector<ustring> Font::list_families() {
    Lock lk(mx_);
    if (families_.empty()) { load_families(); }
    return families_;
}

// ----------------------------------------------------------------------------
// Font_win class methods.
// ----------------------------------------------------------------------------

Font_win::Font_win(HDC hdc, unsigned dpi, const ustring & spec):
    hdc_(hdc),
    dpi_(dpi)
{
    sz_ = font_size_from_spec(spec);
    if (sz_ < 1.0) { sz_ = 10; }
    spec_ = font_size_change(spec, sz_);

    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = -((sz_*dpi)/72);
    lf.lfOutPrecision = OUT_OUTLINE_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
    ustring fam = font_family_from_spec(spec);
    std::size_t len = std::min(std::size_t(LF_FACESIZE), fam.bytes());
    memcpy(lf.lfFaceName, fam.c_str(), len);
    lf.lfFaceName[len < LF_FACESIZE ? len : LF_FACESIZE-1] = '\0';

    ustring face = str_tolower(font_face_from_spec(spec));
    lf.lfItalic = (ustring::npos != face.find("italic") || ustring::npos != face.find("oblique"));

    if (ustring::npos != face.find("thin")) {
        lf.lfWeight = FW_THIN;
    }

    else if (ustring::npos != face.find("extralight") || ustring::npos != face.find("ultralight")) {
        lf.lfWeight = FW_EXTRALIGHT;
    }

    else if (ustring::npos != face.find("light")) {
        lf.lfWeight = FW_LIGHT;
    }

    else if (ustring::npos != face.find("medium")) {
        lf.lfWeight = FW_MEDIUM;
    }

    else if (ustring::npos != face.find("semibold") || ustring::npos != face.find("demibold")) {
        lf.lfWeight = FW_SEMIBOLD;
    }

    else if (ustring::npos != face.find("extrabold") || ustring::npos != face.find("ultrabold") || ustring::npos != face.find("superbold")) {
        lf.lfWeight = FW_EXTRABOLD;
    }

    else if (ustring::npos != face.find("bold")) {
        lf.lfWeight = FW_BOLD;
    }

    else if (ustring::npos != face.find("heavy") || ustring::npos != face.find("black")) {
        lf.lfWeight = FW_HEAVY;
    }

    else {
        lf.lfWeight = FW_NORMAL;
    }

    hfont_ = CreateFontIndirectA(&lf);
    HFONT old_font = SelectFont(hdc, hfont_);
    OUTLINETEXTMETRICW otm;

    if (GetOutlineTextMetricsW(hdc, sizeof otm, &otm)) {
        ascent_  = otm.otmTextMetrics.tmAscent;
        descent_ = -otm.otmTextMetrics.tmDescent;
        linegap_ = otm.otmLineGap;
        min_.set(otm.otmrcFontBox.left, otm.otmrcFontBox.top);
        max_.set(otm.otmrcFontBox.right, otm.otmrcFontBox.bottom);
    }

    const DWORD table = 0x656d616e; // 'name'
    DWORD nbytes = GetFontData(hdc, table, 0, NULL, 0);

    if (GDI_ERROR != nbytes) {
        char b[nbytes];

        if (nbytes == GetFontData(hdc, table, 0, b, nbytes)) {
            uint32_t format_selector = u16(b);

            if (0 == format_selector) {
                uint16_t nrec = u16(b+2);
                uint32_t storage = u16(b+4);
                uint32_t index = 6;

                for (uint16_t nr = 0; nr < nrec; ++nr, index += 12) {
                    uint16_t plat_id = u16(b+index);
                    uint16_t pse = u16(b+index+2);
                    uint16_t name_id = u16(b+index+6);
                    uint16_t ofs = storage+u16(b+index+10);
                    uint16_t pslen = u16(b+index+8);

                    if (6 == name_id) {
                        if (1 == plat_id && 0 == pse) {
                            psname_.assign(b+ofs, pslen);
                            break;
                        }

                        else if (3 == plat_id && (0 == pse || 1 == pse)) {
                            psname_ = read_utf16(b+ofs, pslen);
                            break;
                        }
                    }
                }
            }
        }
    }

    SelectFont(hdc, old_font);
}

Font_win::~Font_win() {
    invalidate();
}

void Font_win::invalidate() {
    hdc_ = NULL;
    if (hfont_) { DeleteFont(hfont_); }
    hfont_ = NULL;
}

// Overrides pure Font_impl.
std::vector<ustring> Font::list_faces(const ustring & font_family) {
    std::vector<ustring> v;

    v.push_back("Regular");
    v.push_back("Italic");
    v.push_back("Bold");
    v.push_back("Bold Italic");

    return v;
}

Glyph_ptr Font_win::glyph(char32_t wc) {
    auto glyph = std::make_shared<Glyph_impl>();
    HFONT old_font = SelectFont(hdc_, hfont_);
    GLYPHMETRICS gm;
    MAT2 mat { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
    DWORD nbytes = GetGlyphOutlineW(hdc_, wc, GGO_NATIVE, &gm, 0, NULL, &mat);

    if (nbytes <= 32768) {
        uint8_t buffer[nbytes];
        const uint8_t * bmax = buffer+nbytes;

        if (0 < GetGlyphOutlineW(hdc_, wc, GGO_NATIVE, &gm, nbytes, buffer, &mat)) {
            int ox = gm.gmptGlyphOrigin.x;
            int oy = gm.gmptGlyphOrigin.y;
            int sx = gm.gmBlackBoxX;
            int sy = gm.gmBlackBoxY;

            glyph->set_min(Vector(ox, oy-sy));
            glyph->set_max(Vector(ox+sx, oy));
            glyph->set_advance(Vector(gm.gmCellIncX, gm.gmCellIncY));

            for (const uint8_t * p = buffer; p < bmax; ) {
                const TTPOLYGONHEADER * hdr = reinterpret_cast<const TTPOLYGONHEADER *>(p);
                const uint8_t * pmax = p+(hdr->cb);

                if (pmax > buffer+nbytes || TT_POLYGON_TYPE != hdr->dwType) {
                    std::cerr << "** Font_win::glyph(U+" << std::hex << wc << std::dec << "): currupted glyph header" << std::endl;
                    break;
                }

                Vector vstart = pointfx_to_vector(hdr->pfxStart), vcur = vstart;
                Contour ctr(vstart);
                const uint8_t * cp = p+sizeof(*hdr);
                const POINTFX * pfx = nullptr;

                while (cp < pmax) {
                    const TTPOLYCURVE * cv = reinterpret_cast<const TTPOLYCURVE *>(cp);
                    const POINTFX * fxs = reinterpret_cast<const POINTFX *>(cp+4);
                    const unsigned cpfx = cv->cpfx;
                    cp += 4;

                    // http://www.drdobbs.com/truetype-font-secrets/184403680
                    // Polyline records consist of a short (two-byte) integer n followed by n points.
                    // The last point of the previous record connects by a straight line to the first point,
                    // then straight lines connect subsequent points.
                    if (TT_PRIM_LINE == cv->wType) {
                        for (unsigned n = 0; n < cpfx; ++n) {
                            cp += sizeof(POINTFX);
                            pfx = fxs++;
                            vcur = pointfx_to_vector(*pfx);
                            ctr.line_to(vcur);
                        }
                    }

                    // http://www.drdobbs.com/truetype-font-secrets/184403680
                    // QSpline records also consist of a short integer n followed by n points,
                    // but only the last point lies on the glyph itself.
                    // These points define a connected series of n-1 Bezier curves.
                    else if (TT_PRIM_QSPLINE == cv->wType) {
                        Vector cpt;

                        for (unsigned n = 0; n < cpfx-1; ++n, ++fxs) {
                            cp += sizeof(POINTFX);
                            cpt = pointfx_to_vector(fxs[0]);

                            if (n < cpfx-2) {
                                Vector ncp = pointfx_to_vector(fxs[1]);
                                vcur.set( (cpt.x()+ncp.x())/2, (cpt.y()+ncp.y())/2 );
                                ctr.conic_to(cpt, vcur);
                            }

                        }

                        pfx = fxs;
                        vcur = pointfx_to_vector(fxs[0]);
                        ctr.conic_to(cpt, vcur);
                        cp += sizeof(POINTFX);
                    }

                    else if (TT_PRIM_CSPLINE == cv->wType) {
                        std::cerr << "** Font_win::glyph(U+" << std::hex << wc << std::dec << "): cubic spline no supported yet" << std::endl;
                        break;
                    }

                    else {
                        std::cerr << "** Font_win::glyph(U+" << std::hex << wc << std::dec << "): currupted glyph data" << std::endl;
                        break;
                    }
                }

                if (pfx && hdr->pfxStart != *pfx) { ctr.line_to(vstart); }
                glyph->add_contour(ctr);
                p += hdr->cb;
            }
        }
    }

    SelectFont(hdc_, old_font);
    return glyph;
}

} // namespace tau

//END
