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

#include <tau/exception.hh>
#include <brush-impl.hh>
#include <pen-impl.hh>
#include <posix/theme-posix.hh>
#include "font-xcb.hh"
#include "painter-xcb.hh"
#include "pixmap-xcb.hh"
#include "winface-xcb.hh"

#include <iostream>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

Painter_xcb::Painter_xcb(Winface_xcb * wf):
    Painter_impl(),
    dp_(wf->xdp()),
    cx_(wf->conn()),
    xid_(wf->wid()),
    xpicture_(wf->xpicture()),
    gc_(cx_, wf->wid())
{
    wstate().obscured_.set(wf->self()->size());
    wf->self()->signal_destroy().connect(fun(this, &Painter_xcb::on_destroy));
    select_font(Font::normal());
    update_clip();
}

void Painter_xcb::draw_pixmap(Pixmap_cptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) {
    if (XCB_NONE == xpicture_ || XCB_NONE == xid_) { return; }
    auto xpix = std::dynamic_pointer_cast<const Pixmap_xcb>(pix);
    if (!xpix || !xpix->size()) { return; }
    xpix->set_display(dp_);
    xpix->draw(xid_, xpicture_, state().op_, pix_origin, pix_size, pt, transparent);
}

void Painter_xcb::set_clip() {
    if (XCB_NONE != xpicture_) {
        gc_.flush();
        xcb_set_clip_rectangles(cx_, XCB_CLIP_ORDERING_UNSORTED, gc_.xid(), 0, 0, 1, &cr_);
        xcb_render_set_picture_clip_rectangles(cx_, xpicture_, 0, 0, 1, &cr_);
    }
}

void Painter_xcb::update_clip() {
    cr_ = to_xcb_rectangle(wstate().obscured_);
    set_clip();
}

// Overrides Painter_impl.
void Painter_xcb::fill_polygon(const Point * pts, std::size_t npts, const Color & color) {
    if (XCB_NONE == xid_) { return; }
    xcb_point_t xpts[npts];
    for (std::size_t n = 0; n < npts; ++n) { xpts[n] = to_xcb_point(pts[n]); }
    gc_.set_foreground(color);
    gc_.set_func(gx_oper(state().op_));
    gc_.flush();
    xcb_fill_poly(cx_, xid_, gc_.xid(), XCB_POLY_SHAPE_COMPLEX, XCB_COORD_MODE_ORIGIN, npts, xpts);
    xcb_flush(cx_);
}

// protected
// Overrides Painter_impl.
void Painter_xcb::fill_prim_contour(const Prim_contour & o) {
    if (1 == o.ctrs.size()) {
        const Contour & ctr = o.ctrs.back();

        if (1 == ctr.order()) {
            std::size_t pos = 1, npts = 1+ctr.size();
            Point pts[npts];
            pts[0] = matrix()*ctr.start(); pts[0] -= woffset();
            for (const Curve & cv: ctr) { pts[pos] = matrix()*cv.end(); pts[pos++] -= woffset(); }
            if (const Rect r = is_rect(pts, npts)) { fill_rectangles(&r, 1, state().brush_->color); }
            else { fill_polygon(pts, npts, state().brush_->color); }
            return;
        }
    }

    Painter_impl::fill_prim_contour(o);
}

// protected
// Overrides Painter_impl.
void Painter_xcb::stroke_prim_rect(const Prim_rect * po, std::size_t no) {
    if (XCB_NONE == xid_) { return; }

    xcb_rectangle_t rs[no];
    std::size_t nrs = 0;
    Vector wo = woffset();

    while (no--) {
        Point pts[5];
        pts[0] = matrix()*po->v1-wo;
        pts[1] = matrix()*Vector(po->v2.x(), po->v1.y())-wo;
        pts[2] = matrix()*po->v2-wo;
        pts[3] = matrix()*Vector(po->v1.x(), po->v2.y())-wo;
        pts[4] = pts[0];

        if (pts[0].x() == pts[3].x() && pts[1].y() == pts[2].y()) {
            rs[nrs].x = pts[0].x();
            rs[nrs].y = pts[0].y();
            rs[nrs].width = pts[1].x()-pts[0].x();
            rs[nrs++].height = pts[2].y()-pts[1].y();
        }

        else {
            stroke_polyline(pts, 5);
        }

        ++po;
    }

    if (nrs) {
        load_stroke_gc();
        xcb_poly_rectangle(cx_, xid_, gc_.xid(), nrs, rs);
        xcb_flush(cx_);
    }
}

// protected
// Overrides Painter_impl.
void Painter_xcb::fill_prim_rect(const Prim_rect * po, std::size_t no) {
    if (XCB_NONE == xid_) { return; }

    xcb_rectangle_t rs[no];
    std::size_t nrs = 0;
    Vector wo = woffset();

    while (no--) {
        Point pts[5];
        pts[0] = matrix()*po->v1-wo;
        pts[1] = matrix()*Vector(po->v2.x(), po->v1.y())-wo;
        pts[2] = matrix()*po->v2-wo;
        pts[3] = matrix()*Vector(po->v1.x(), po->v2.y())-wo;
        pts[4] = pts[0];

        if (pts[0].x() == pts[3].x() && pts[0].y() == pts[1].y()) {
            rs[nrs].x = pts[0].x();
            rs[nrs].y = pts[0].y();
            rs[nrs].width = pts[1].x()-pts[0].x();
            rs[nrs++].height = pts[2].y()-pts[1].y();
        }

        else {
            fill_polygon(pts, 5, state().brush_->color);
        }

        ++po;
    }

    if (nrs) {
        gc_.set_foreground(state().brush_->color);
        gc_.set_func(gx_oper(state().op_));
        gc_.flush();
        xcb_poly_fill_rectangle(cx_, xid_, gc_.xid(), nrs, rs);
        xcb_flush(cx_);
    }
}

// protected
// Overrides pure Painter_impl.
void Painter_xcb::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    if (XCB_NONE == xid_ || 0 == nrs) { return; }
    xcb_rectangle_t xr[nrs];
    std::size_t n = nrs;
    for (xcb_rectangle_t * p = xr; n; n--) { *p++ = to_xcb_rectangle(*rs++); }
    gc_.set_foreground(c);
    gc_.set_func(gx_oper(state().op_));
    gc_.flush();
    xcb_poly_fill_rectangle(cx_, xid_, gc_.xid(), nrs, xr);
    xcb_flush(cx_);
}

// protected
// Overrides Painter_impl.
void Painter_xcb::stroke_prim_text(const Prim_text & o) {
    if (XCB_NONE == xpicture_ || wstate().obscured_.empty()) { return; }
    auto fp = std::dynamic_pointer_cast<Font_xcb>(state().font_);
    if (!fp) { return; }
    Point pt(matrix()*o.pos);
    xcb_render_picture_t src = dp_->solid_fill(o.color);
    uint8_t op = xrender_oper(state().op_);
    set_clip();
    fp->render_glyphs(o.str, pt-woffset(), op, src, xpicture_);
}

void Painter_xcb::stroke_rectangle(const Rect & r) {
    if (XCB_NONE == xid_) { return; }
    xcb_rectangle_t xr = to_xcb_rectangle(r);
    load_stroke_gc();
    xcb_poly_rectangle(cx_, xid_, gc_.xid(), 1, &xr);
    xcb_flush(cx_);
}

void Painter_xcb::stroke_polyline(const Point * pts, std::size_t npts) {
    if (XCB_NONE == xid_ || npts < 2) { return; }
    std::size_t n = npts;
    xcb_point_t xpts[npts];
    for (xcb_point_t * p = xpts; n; n--) { *p++ = to_xcb_point(*pts++); }
    load_stroke_gc();
    xcb_poly_line(cx_, XCB_COORD_MODE_ORIGIN, xid_, gc_.xid(), npts, xpts);
    xcb_flush(cx_);
}

void Painter_xcb::stroke_prim_arc(const Prim_arc & obj) {
    if (XCB_NONE == xid_) { return; }

    if (matrix().is_identity()) {
        xcb_arc_t arc;
        arc.x = obj.center.x()-woffset().x()-obj.radius;
        arc.y = obj.center.y()-woffset().y()-obj.radius;
        arc.width = 2*obj.radius;
        arc.height = 2*obj.radius;
        arc.angle1 = 3666.93*obj.angle1;
        arc.angle2 = 3666.93*(obj.angle2-obj.angle1);
        load_stroke_gc();
        xcb_poly_arc(cx_, xid_, gc_.xid(), 1, &arc);
        xcb_flush(cx_);
    }

    else {
        Painter_impl::stroke_prim_arc(obj);
    }
}

void Painter_xcb::load_stroke_gc() {
    gc_.set_foreground(state().pen_->color);
    double lw = state().pen_->line_width;
    gc_.set_line_width(lw > 0.0 ? lw : 1);
    gc_.set_line_style(xcb_line_style(state().pen_->line_style));
    gc_.set_cap_style(xcb_cap_style(state().pen_->cap_style));
    gc_.set_join_style(xcb_join_style(state().pen_->join_style));
    gc_.set_func(gx_oper(state().op_));
    gc_.flush();
}

void Painter_xcb::fill_prim_arc(const Prim_arc & obj) {
    Painter_impl::fill_prim_arc(obj);
}

// Overrides pure Painter.
void Painter_xcb::set_font(Font_ptr font) {
    if (font) {
        state().font_ = font;
        state().fontspec_ = font->spec();
    }
}

// Overrides pure Painter.
Font_ptr Painter_xcb::select_font(const ustring & font_spec) {
    if (state().fontspec_ != font_spec) {
        auto theme = Theme_posix::root_posix();

        if (Font_ptr font = theme->uncache_font(font_spec, dp_->dpi())) {
            state().font_ = font;
            state().fontspec_ = font_spec;
        }

        else {
            double font_size = font_size_from_spec(font_spec);
            state().fontspec_ = font_spec;
            Font_face_ptr ffp = theme->create_font_face(font_spec);

            if (!ffp) {
                state().fontspec_ = Font::normal();
                ffp = theme->create_font_face(state().fontspec_);
            }

            if (!ffp) {
                throw graphics_error(str_format("Painter_xcb: Unable to create font face ", font_spec));
            }

            state().font_ = std::make_shared<Font_xcb>(ffp, font_spec, font_size >= 1.0 ? font_size : 10.0, dp_);
            theme->cache_font(state().font_, font_spec);
        }
    }

    return state().font_;
}

// Overrides pure Painter.
Vector Painter_xcb::text_size(const ustring & s) {
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

// Overrides pure Painter.
Vector Painter_xcb::text_size(const std::u32string & s) {
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

void Painter_xcb::on_destroy() {
    xpicture_ = XCB_NONE;
    xid_ = XCB_NONE;
}

} // namespace tau

//END
