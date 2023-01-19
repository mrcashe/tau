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

#include <tau/font.hh>
#include "theme-win.hh"
#include <cstring>
#include <mutex>

namespace {

using Mutex = std::recursive_mutex;
using Lock = std::lock_guard<Mutex>;

Mutex                       mx_;
tau::Theme_win_ptr          root_;
std::vector<tau::ustring>   families_;

int CALLBACK lister(const LOGFONTW * lf, const TEXTMETRICW * tm, DWORD ftype, LPARAM lp) {
    if (TRUETYPE_FONTTYPE == ftype) {
        tau::ustring fam = tau::str_from_wstring(std::wstring(lf->lfFaceName));
        auto * v = reinterpret_cast<std::vector<tau::ustring> *>(lp);
        if (v->end() == std::find(v->begin(), v->end(), fam)) { v->push_back(fam); }
    }

    return 1;
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

// static
Theme_win_ptr Theme_win::root_win() {
    Lock lk(mx_);

    if (!root_) {
        root_ = std::make_shared<Theme_win>();
        root_->boot();
    }

    return root_;
}

// static
Theme_ptr Theme_impl::root() {
    return Theme_win::root_win();
}

void Theme_win::boot() {
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

    ustring nice_fonts = "Trebuchet MS:Tahoma:Times New Roman:Arial:Noto Sans:Droid Sans:DejaVu Sans";
    double pts = 10.0;

    NONCLIENTMETRICS ncm;
    std::memset(&ncm, 0, sizeof ncm);
    ncm.cbSize = sizeof ncm;

    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof ncm, &ncm, FALSE)) {
        ustring spec = font_spec_from_logfont(ncm.lfMessageFont);

        if (!spec.empty()) {
            ustring fam = font_family_from_spec(spec);

            if (str_similar(fam, families_)) {
                nice_fonts.insert(0, fam+":");
                pts = std::max(pts, font_size_from_spec(spec));
            }
        }
    }

    for (const ustring & s: str_explode(nice_fonts, ':')) {
        if (font_mono_.empty()) {
            ustring mono = font_face_set(s, "Mono");
            if (str_similar(mono, families_)) { font_mono_ = font_size_at_least(mono, pts); }
        }

        if (str_similar(s, families_)) {
            font_normal_ = font_size_at_least(s, pts);
            break;
        }
    }

    if (font_normal_.empty() && !families_.empty()) {
        font_normal_ = font_size_at_least(families_.front(), pts);
    }

    if (font_mono_.empty() && !font_normal_.empty()) {
        font_mono_ = font_face_set(font_normal_, "Mono");
    }

    Theme_impl::boot();
    boot_icon_themes("Tau");
}

std::vector<ustring> Theme_win::font_families() const {
    Lock lk(mx_);
    return families_;
}

} // namespace tau

//END
