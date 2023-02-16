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

#include <posix/font-face-posix.hh>
#include <posix/font-file-posix.hh>

namespace tau {

Font_face::Font_face(Font_file_ptr file, Master_glyph_ptr zero):
    file_(file),
    zero_(zero)
{
}

void Font_face::set_family(const ustring & family) {
    family_ = family;
}

void Font_face::set_facename(const ustring & facename) {
    facename_ = facename;
}

void Font_face::set_fontname(const ustring & name) {
    fontname_ = name;
}

void Font_face::set_psname(const ustring & psname) {
    psname_ = psname;
}

void Font_face::set_bounds(const Rect & bbox) {
    bbox_ = bbox;
}

void Font_face::set_upm(unsigned upm) {
    upm_ = upm;
}

void Font_face::set_ascent(int16_t asc) {
    ascent_ = asc;
}

void Font_face::set_descent(int16_t desc) {
    descent_ = desc;
}

void Font_face::set_linegap(int16_t linegap) {
    linegap_ = linegap;
}

void Font_face::set_max_advance(uint16_t adv) {
    max_advance_ = adv;
}

void Font_face::set_min_lsb(int16_t lsb) {
    min_lsb_ = lsb;
}

void Font_face::set_min_rsb(int16_t rsb) {
    min_rsb_ = rsb;
}

void Font_face::set_max_x_extent(int16_t extent) {
    max_x_extent_ = extent;
}

void Font_face::set_caret_slope_rise(bool rise) {
    caret_slope_rise_ = rise;
}

void Font_face::set_caret_slope_run(bool run) {
    caret_slope_run_ = run;
}

void Font_face::preload(char32_t first, char32_t last) {
    std::u32string s(last-first+1, U' ');
    std::size_t i = 0;
    for (char32_t c = first; c <= last; ++c) { s[i++] = c; }
    i = 0;

    for (auto g: file_->glyphs(family_, facename_, s)) {
        if (g) {
            glyphs_[s[i++]] = g;
        }
    }
}

Master_glyph_ptr Font_face::glyph(char32_t wc) {
    Lock lock(mx_);

    // Preload ASCII glyphs.
    if (glyphs_.empty()) {
        preload(0x0020, 0x007e);
    }

    // Try to uncache.
    auto iter = glyphs_.find(wc);
    if (iter != glyphs_.end()) { return iter->second; }

    // Test if it needed to preload Latin1 extended page.
    if (wc >= 0x00a0 && wc <= 0x00ff && !xlatin_) {
        preload(0x00a0, 0x00ff);
        xlatin_ = true;
    }

    // Test if it needed to preload Unicode pages 01...ff.
    if (wc >= 0x0100 && wc <= 0xffff) {
        if (std::string::npos == preloaded_.find_first_of(wc>>8)) {
            preloaded_ += (wc>>8);
            preload(wc&0xff00, wc|0x00ff);
        }
    }

    // Not found, load glyph from file.
    auto glyph = file_->glyph(family_, facename_, wc);

    // Return fallback glyph.
    if (!glyph) { return zero_; }

    // Cache it.
    glyphs_[wc] = glyph;
    return glyph;
}

} // namespace tau

//END
