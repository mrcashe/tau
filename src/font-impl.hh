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

#ifndef TAU_FONT_IMPL_HH
#define TAU_FONT_IMPL_HH

#include <tau/font.hh>
#include <types-impl.hh>

namespace tau {

class Font_impl {
protected:

    Font_impl() = default;
    Font_impl(const Font_impl &) = delete;
    Font_impl & operator=(const Font_impl &) = delete;

public:

    virtual ~Font_impl() {}

    static Font wrap(Font_ptr fp) { return Font(fp); }

    static Font_ptr strip(Font f) { return f.impl; }

    virtual ustring spec() const = 0;

    virtual ustring psname() const = 0;

    virtual unsigned dpi() const = 0;

    virtual double ascent() const = 0;

    virtual double descent() const = 0;

    virtual double linegap() const = 0;

    virtual Vector min() const = 0;

    virtual Vector max() const = 0;

    virtual Glyph_ptr glyph(char32_t wc) = 0;
};

} // namespace tau

#endif // TAU_FONT_IMPL_HH
