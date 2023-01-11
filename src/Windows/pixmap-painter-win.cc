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

#include <glyph-impl.hh>
#include <pixmap-impl.hh>
#include "pixmap-painter-win.hh"

namespace tau {

Pixmap_painter_win::Pixmap_painter_win(Pixmap_impl * pixmap):
    Painter_impl(),
    pixmap_(pixmap)
{
    if (pixmap) {
        pixmap->signal_destroy().connect(fun(this, &Pixmap_painter_win::on_pixmap_destroy));
        wstate().wclip.set(pixmap->size());
    }
}

// Overrides pure Painter_impl.
void Pixmap_painter_win::set_font(Font_ptr font) {
}

// Overrides pure Painter_impl.
Font_ptr Pixmap_painter_win::select_font(const ustring & spec) {
    return Font_ptr();
}

// Overrides pure Painter_impl.
Vector Pixmap_painter_win::text_size(const ustring & s) {
    return Vector();
}

// Overrides pure Painter_impl.
Vector Pixmap_painter_win::text_size(const std::u32string & s) {
    return Vector();
}

// Overrides pure Painter_impl.
void Pixmap_painter_win::stroke_rectangle(const Rect & r) {
}

// Overrides pure Painter_impl.
void Pixmap_painter_win::stroke_polyline(const Point * pts, std::size_t npts) {
}

// Overrides pure Painter_impl.
void Pixmap_painter_win::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    if (pixmap_) {
        pixmap_->fill_rectangles(rs, nrs, c);
    }
}

// Overrides pure Painter_impl.
void Pixmap_painter_win::fill_polygon(const Point * pts, std::size_t npts, const Color & color) {
}

// Overrides pure Painter_impl.
void Pixmap_painter_win::draw_pixmap(Pixmap_ptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) {
}

} // namespace tau

//END
