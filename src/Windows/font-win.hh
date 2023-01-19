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

#ifndef TAU_FONT_WIN_HH
#define TAU_FONT_WIN_HH

#include "types-win.hh"
#include <tau/signal.hh>
#include <font-impl.hh>

namespace tau {

class Font_win: public Font_impl, public trackable {
public:

    Font_win(HDC hdc, unsigned dpi, const ustring & font_spec);
    Font_win(const Font_win & other) = delete;
    Font_win & operator=(const Font_win & other) = delete;

   ~Font_win();

    // Overrides Font_impl.
    ustring spec() const override { return spec_; }

    // Overrides Font_impl.
    ustring psname() const override { return psname_; }

    // Overrides Font_impl.
    unsigned dpi() const override { return dpi_; }

    // Overrides Font_impl.
    double ascent() const override { return ascent_; }

    // Overrides Font_impl.
    double descent() const override { return descent_; }

    // Overrides Font_impl.
    double linegap() const override { return linegap_; }

    // Overrides Font_impl.
    Vector min() const override { return min_; }

    // Overrides Font_impl.
    Vector max() const override { return max_; }

    // Overrides Font_impl.
    Glyph_ptr glyph(char32_t wc) override;

    HFONT handle() const { return hfont_; }

    void invalidate();

private:

    HDC         hdc_ = NULL;
    HFONT       hfont_ = NULL;
    unsigned    dpi_;
    ustring     spec_;
    ustring     psname_;
    Vector      min_;
    Vector      max_;
    double      ascent_ = 0.0;
    double      descent_ = 0.0;
    double      linegap_ = 0.0;
};

} // namespace tau

#endif // TAU_FONT_WIN_HH
