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

#ifndef TAU_PIXMAP_PAINTER_WIN_HH
#define TAU_PIXMAP_PAINTER_WIN_HH

#include "types-win.hh"
#include <tau/color.hh>
#include <painter-impl.hh>

namespace tau {

class Pixmap_painter_win: public Painter_impl {
public:

    explicit Pixmap_painter_win(Pixmap_impl * pixmap);

    // Overrides pure Painter_impl.
    Vector text_size(const ustring & s) override;

    // Overrides pure Painter_impl.
    Vector text_size(const std::u32string & s) override;

protected:

    // Overrides Painter_impl.
    void paint() override {}

    // Overrides pure Painter_impl.
    void stroke_rectangle(const Rect & r) override;

    // Overrides pure Painter_impl.
    void stroke_polyline(const Point * pts, std::size_t npts) override;

    // Overrides pure Painter_impl.
    void fill_rectangles(const Rect * rs, std::size_t nrs, const Color & color) override;

    // Overrides pure Painter_impl.
    void fill_polygon(const Point * pts, std::size_t npts, const Color & color) override;

    // Overrides pure Painter_impl.
    void draw_pixmap(Pixmap_ptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) override;

    // Overrides pure Painter_impl.
    void set_font(Font_ptr font) override;

    // Overrides pure Painter_impl.
    Font_ptr select_font(const ustring & spec) override;

    // Overrides pure Painter_impl.
    void update_clip() override {}

    void on_pixmap_destroy() { pixmap_ = nullptr; }

protected:

    Pixmap_impl * pixmap_ = nullptr;
};

} // namespace tau

#endif // TAU_PIXMAP_PAINTER_WIN_HH
