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

#include <tau/brush.hh>
#include <tau/painter.hh>
#include <tau/pen.hh>
#include <tau/pixmap.hh>
#include <font-impl.hh>
#include <glyph-impl.hh>
#include <painter-impl.hh>
#include <iostream>

namespace {

tau::Matrix dummy_matrix;

void log(const char * s) {
    std::cerr << "tau::Painter::" << s << "() called on pure Painter" << std::endl;
}

} // anonymous namespace

namespace tau {

Painter::Painter() {}

Painter::Painter(Painter_ptr pp):
    impl(pp)
{
}

Painter::operator bool() const {
    return nullptr != impl;
}

void Painter::set_font(Font font) {
    if (impl) {
        impl->set_font(Font_impl::strip(font));
    }

    else {
        log("set_font");
    }
}

Font Painter::select_font(const ustring & font_spec) {
    if (impl) {
        return Font_impl::wrap(impl->select_font(font_spec));
    }

    else {
        log("select_font");
        return Font();
    }
}

Font Painter::font() {
    if (impl) {
        return Font_impl::wrap(impl->font());
    }

    else {
        log("font");
        return Font();
    }
}

Vector Painter::text_size(const ustring & s, Orientation orient) {
    if (impl) {
        return impl->text_size(s);
    }

    else {
        log("text_size");
        return Vector();
    }
}

Vector Painter::text_size(const std::u32string & s, Orientation orient) {
    if (impl) {
        return impl->text_size(s);
    }

    else {
        log("text_size");
        return Vector();
    }
}

void Painter::text(const ustring & s, const Color & c, Orientation orient) {
    if (impl) {
        impl->text(s, c);
    }

    else {
        log("text");
    }
}

void Painter::text(const std::u32string & s, const Color & c, Orientation orient) {
    if (impl) {
        impl->text(s, c);
    }

    else {
        log("text");
    }
}

void Painter::text(std::u32string && s, const Color & c, Orientation orient) {
    if (impl) {
        impl->text(std::move(s), c);
    }

    else {
        log("text");
    }
}

void Painter::glyph(Glyph glyph) {
    if (impl) {
        impl->glyph(Glyph_impl::strip(glyph));
    }

    else {
        log("glyph");
    }
}

Matrix & Painter::matrix() const {
    if (impl) {
        return impl->matrix();
    }

    else {
        log("matrix");
        return dummy_matrix;
    }
}

void Painter::set_pen(Pen pen) {
    if (impl) {
        impl->set_pen(pen);
    }

    else {
        log("set_pen");
    }
}

Pen Painter::pen() const {
    if (impl) {
        return impl->pen();
    }

    else {
        log("pen");
        return Pen();
    }
}

void Painter::set_brush(Brush brush) {
    if (impl) {
        impl->set_brush(brush);
    }

    else {
        log("set_brush");
    }
}

Brush Painter::brush() const {
    if (impl) {
        return impl->brush();
    }

    else {
        log("brush");
        return Brush();
    }
}

void Painter::paint() {
    if (impl) {
        impl->paint();
    }

    else {
        log("paint");
    }
}

void Painter::fill() {
    if (impl) {
        impl->fill();
    }

    else {
        log("fill");
    }
}

void Painter::fill_preserve() {
    if (impl) {
        impl->fill_preserve();
    }

    else {
        log("fill_preserve");
    }
}

void Painter::stroke() {
    if (impl) {
        impl->stroke();
    }

    else {
        log("stroke");
    }
}

void Painter::stroke_preserve() {
    if (impl) {
        impl->stroke_preserve();
    }

    else {
        log("stroke_preserve");
    }
}

void Painter::push() {
    if (impl) {
        impl->push();
    }

    else {
        log("push");
    }
}

void Painter::pop() {
    if (impl) {
        impl->pop();
    }

    else {
        log("pop");
    }
}

void Painter::clear() {
    if (impl) {
        impl->clear();
    }

    else {
        log("clear");
    }
}

void Painter::set_oper(Oper op) {
    if (impl) {
        impl->set_oper(op);
    }

    else {
        log("set_oper");
    }
}

Oper Painter::oper() const {
    if (impl) {
        return impl->oper();
    }

    else {
        log("oper");
        return OPER_COPY;
    }
}

void Painter::pixmap(Pixmap_ptr pix, const Point pix_origin, const Size & pix_size, bool transparent) {
    if (impl) {
        impl->pixmap(pix, pix_origin, pix_size, transparent);
    }

    else {
        log("pixmap");
    }
}

void Painter::pixmap(Pixmap_ptr pix, bool transparent) {
    if (impl) {
        impl->pixmap(pix, transparent);
    }

    else {
        log("pixmap");
    }
}

Vector Painter::position() const {
    if (impl) {
        return impl->position();
    }

    else {
        log("position");
        return Vector();
    }
}

void Painter::move_to(double x, double y) {
    if (impl) {
        impl->move_to(x, y);
    }

    else {
        log("move_to");
    }
}

void Painter::move_to(const Vector & v) {
    if (impl) {
        impl->move_to(v);
    }

    else {
        log("move_to");
    }
}

void Painter::move_rel(double x, double y) {
    if (impl) {
        impl->move_rel(x, y);
    }

    else {
        log("move_rel");
    }
}

void Painter::move_rel(const Vector & v) {
    if (impl) {
        impl->move_rel(v);
    }

    else {
        log("move_rel");
    }
}

void Painter::rectangle(double x1, double y1, double x2, double y2, double radius) {
    if (impl) {
        impl->rectangle(x1, y1, x2, y2);
    }

    else {
        log("rectangle");
    }
}

void Painter::rectangle(const Vector & v1, const Vector & v2, double radius) {
    if (impl) {
        impl->rectangle(v1, v2);
    }

    else {
        log("rectangle");
    }
}

void Painter::line_to(double x, double y) {
    if (impl) {
        impl->line_to(x, y);
    }

    else {
        log("line_to");
    }
}

void Painter::line_to(const Vector & end) {
    if (impl) {
        impl->line_to(end);
    }

    else {
        log("line_to");
    }
}

void Painter::line_rel(double dx, double dy) {
    if (impl) {
        impl->line_rel(dx, dy);
    }

    else {
        log("line_rel");
    }
}

void Painter::line_rel(const Vector & dv) {
    if (impl) {
        impl->line_rel(dv);
    }

    else {
        log("line_rel");
    }
}

void Painter::conic_to(double cx, double cy, double ex, double ey) {
    if (impl) {
        impl->conic_to(cx, cy, ex, ey);
    }

    else {
        log("conic_to");
    }
}

void Painter::conic_to(const Vector & cp, const Vector & end) {
    if (impl) {
        impl->conic_to(cp, end);
    }

    else {
        log("conic_to");
    }
}

void Painter::cubic_to(double cx1, double cy1, double cx2, double cy2, double ex, double ey) {
    if (impl) {
        impl->cubic_to(cx1, cy1, cx2, cy2, ex, ey);
    }

    else {
        log("cubic_to");
    }
}

void Painter::cubic_to(const Vector & cp1, const Vector & cp2, const Vector & end) {
    if (impl) {
        impl->cubic_to(cp1, cp2, end);
    }

    else {
        log("cubic_to");
    }
}

void Painter::arc(const Vector & center, double radius, double angle1, double angle2, bool pie) {
    if (impl) {
        impl->arc(center, radius, angle1, angle2, pie);
    }

    else {
        log("arc");
    }
}

void Painter::arc(double cx, double cy, double radius, double angle1, double angle2, bool pie) {
    if (impl) {
        impl->arc(cx, cy, radius, angle1, angle2, pie);
    }

    else {
        log("arc");
    }
}

void Painter::circle(const Vector & center, double radius) {
    if (impl) {
        impl->circle(center, radius);
    }

    else {
        log("circle");
    }
}

void Painter::circle(double cx, double cy, double radius) {
    if (impl) {
        impl->circle(cx, cy, radius);
    }

    else {
        log("circle");
    }
}

void Painter::contour(const Contour & ctr) {
    if (impl) {
        impl->contour(ctr);
    }

    else {
        log("contour");
    }
}

void Painter::contour(Contour && ctr) {
    if (impl) {
        impl->contour(std::move(ctr));
    }

    else {
        log("contour");
    }
}

} // namespace tau

//END
