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

#ifndef TAU_FONT_POSIX_HH
#define TAU_FONT_POSIX_HH

#include "types-posix.hh"
#include <font-impl.hh>
#include <glyph-impl.hh>
#include <map>

namespace tau {

class Font_posix: public Font_impl {
public:

    Font_posix(Font_face_ptr fface, double size_pt, unsigned dpi);

    // Overrides pure Font_impl.
    ustring family_name() const override;

    // Overrides pure Font_impl.
    ustring face_name() const override;

    // Overrides pure Font_impl.
    ustring spec() const override {
        return font_spec_build(family_name(), face_name(), sz_);
    }

    // Overrides pure Font_impl.
    ustring psname() const override;

    // Overrides pure Font_impl.
    double size() const override {
        return sz_;
    }

    // Overrides pure Font_impl.
    unsigned dpi() const override {
        return dpi_;
    }

    // Overrides pure Font_impl.
    double ascent() const override {
        return ascent_;
    }

    // Overrides pure Font_impl.
    double descent() const override {
        return descent_;
    }

    // Overrides pure Font_impl.
    double linegap() const override {
        return linegap_;
    }

    // Overrides pure Font_impl.
    Vector min() const override {
        return min_;
    }

    // Overrides pure Font_impl.
    Vector max() const override {
        return max_;
    }

    // Overrides pure Font_impl.
    Glyph_ptr glyph(char32_t wc) override;

protected:

    using Glyph_map = std::map<char32_t, Glyph_ptr>;

    Font_face_ptr       face_;
    Glyph_map           gmap_;
    Vector              scale_;
    Vector              min_;
    Vector              max_;
    unsigned            dpi_ = 0;
    double              sz_ = 0.0;
    double              ascent_ = 0.0;
    double              descent_ = 0.0;
    double              linegap_ = 0.0;
};

} // namespace tau

#endif // TAU_FONT_POSIX_HH
