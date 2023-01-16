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

#include <tau/exception.hh>
#include <tau/string.hh>
#include <brush-impl.hh>
#include <pixmap-impl.hh>
#include <posix/theme-posix.hh>
#include "pixmap-painter-xcb.hh"
#include "font-xcb.hh"

namespace tau {

Pixmap_painter_xcb::Pixmap_painter_xcb(Pixmap_impl * pixmap):
    Painter_impl(),
    pixmap_(pixmap)
{
    if (pixmap) {
        pixmap->signal_destroy().connect(fun(this, &Pixmap_painter_xcb::on_pixmap_destroy));
        wstate().wclip.set(pixmap->size());
    }
}

// Overrides pure Painter_impl.
void Pixmap_painter_xcb::set_font(Font_ptr font) {
    if (font) {
        state().font = font;
        state().font_spec = font->spec();
    }
}

// Overrides pure Painter_impl.
Font_ptr Pixmap_painter_xcb::select_font(const ustring & font_spec) {
    if (state().font_spec != font_spec) {
        double font_size = font_size_from_spec(font_spec);
        state().font_spec = font_spec;
        auto theme = Theme_posix::root_posix();
        Font_face_ptr ffp = theme->create_font_face(font_spec);

        if (!ffp) {
            state().font_spec = Font::normal();
            ffp = theme->create_font_face(state().font_spec);
        }

        if (!ffp) {
            throw graphics_error(str_format("Unable to create font face ", font_spec));
        }

        Vector dpi(72, 72);
        if (pixmap_) { dpi = pixmap_->ppi(); }
        state().font = std::make_shared<Font_posix>(ffp, font_size >= 1.0 ? font_size : 10.0, std::max(dpi.x(), dpi.y()));
    }

    return state().font;
}

// Overrides pure Painter_impl.
Vector Pixmap_painter_xcb::text_size(const ustring & s) {
    int w = 0, h = 0;

    if (Font_ptr fp = font()) {
        h = std::ceil(fp->ascent()-fp->descent()+fp->linegap());

        for (char32_t wc: s) {
            if (!char32_is_zerowidth(wc)) {
                if (Glyph_ptr g = fp->glyph(wc)) {
                    w += std::ceil(g->advance().x());
                }
            }
        }
    }

    return Vector(w, h);
}

// Overrides pure Painter_impl.
Vector Pixmap_painter_xcb::text_size(const std::u32string & s) {
    int w = 0, h = 0;

    if (Font_ptr fp = font()) {
        h = std::ceil(fp->ascent()-fp->descent()+fp->linegap());

        for (char32_t wc: s) {
            if (!char32_is_zerowidth(wc)) {
                if (Glyph_ptr g = fp->glyph(wc)) {
                    w += std::ceil(g->advance().x());
                }
            }
        }
    }

    return Vector(w, h);
}

// Overrides Painter_impl.
void Pixmap_painter_xcb::paint() {
    if (pixmap_) {
        Rect r(0, 0, pixmap_->size());
        fill_rectangles(&r, 1, state().brush->color);
    }
}

// Overrides pure Painter_impl.
void Pixmap_painter_xcb::stroke_rectangle(const Rect & r) {
}

// Overrides pure Painter_impl.
void Pixmap_painter_xcb::stroke_polyline(const Point * pts, std::size_t npts) {
}

// Overrides pure Painter_impl.
void Pixmap_painter_xcb::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    if (pixmap_) {
        pixmap_->fill_rectangles(rs, nrs, c);
    }
}

// Overrides pure Painter_impl.
void Pixmap_painter_xcb::fill_polygon(const Point * pts, std::size_t npts, const Color & color) {
}

// Overrides pure Painter_impl.
void Pixmap_painter_xcb::draw_pixmap(Pixmap_ptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) {
}

} // namespace tau

//END
