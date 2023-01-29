// ----------------------------------------------------------------------------
// Copyright © 2014-2023 Konstantin Shmelkov <mrcashe@gmail.com>.
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
#include "theme-win.hh"
#include <cstring>
#include <iostream>

namespace {

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

} // anonymous namespace

namespace tau {

// ----------------------------------------------------------------------------
// Font_win class methods.
// ----------------------------------------------------------------------------

Font_win::Font_win(HDC hdc, unsigned dpi, const ustring & spec):
    hdc_(hdc),
    dpi_(dpi),
    spec_(spec)
{
    if (font_size_from_spec(spec_) < 1.0) { spec_ = font_size_at_least(spec_, 10); }
    auto specv = font_spec_explode(spec_);
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = -((font_size_from_spec(spec_)*dpi)/72);
    lf.lfOutPrecision = OUT_OUTLINE_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
    lf.lfWeight = FW_NORMAL;
    ustring fam = font_family_from_spec(specv);
    std::size_t len = std::min(std::size_t(LF_FACESIZE), fam.bytes());
    memcpy(lf.lfFaceName, fam.c_str(), len);
    lf.lfFaceName[len < LF_FACESIZE ? len : LF_FACESIZE-1] = '\0';
    lf.lfItalic = str_similar("Italic", specv) || str_similar("Oblique", specv);
    if (str_similar("Mono", specv)) { lf.lfPitchAndFamily |= FIXED_PITCH; }
    if (str_similar("Thin", specv)) { lf.lfWeight = FW_THIN; }
    else if (str_similar("ExtraLight", specv) || str_similar("UltraLight", specv)) { lf.lfWeight = FW_EXTRALIGHT; }
    else if (str_similar("Light", specv)) { lf.lfWeight = FW_LIGHT; }
    else if (str_similar("Medium", specv)) { lf.lfWeight = FW_MEDIUM; }
    else if (str_similar("SemiBold", specv) || str_similar("DemiBold", specv)) { lf.lfWeight = FW_SEMIBOLD; }
    else if (str_similar("ExtraBold", specv) || str_similar("UltraBold", specv) || str_similar("SuperBold", specv)) { lf.lfWeight = FW_EXTRABOLD; }
    else if (str_similar("Bold", specv)) { lf.lfWeight = FW_BOLD; }
    else if (str_similar("Heavy", specv) || str_similar("Black", specv)) { lf.lfWeight = FW_HEAVY; }

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

// static
std::vector<ustring> Font::list_families() {
    return Theme_win::root_win()->font_families();
}

// static
std::vector<ustring> Font::list_faces(const ustring & font_family) {
    static const char * weights = "ExtraLight:Light:Medium:SemiBold:ExtraBold:Bold:Heavy";
    std::vector<ustring> v;

    for (const ustring & s: str_explode(weights, ':')) {
        v.push_back(s);
        v.push_back("Mono "+s);
        v.push_back(s+" Italic");
        v.push_back("Mono "+s+" Italic");
    }

    return v;
}

} // namespace tau

//END
