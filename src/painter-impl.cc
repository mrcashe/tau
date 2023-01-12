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

#include <brush-impl.hh>
#include <container-impl.hh>
#include <glyph-impl.hh>
#include <painter-impl.hh>
#include <pixmap-impl.hh>
#include <climits>

#include <iostream>

namespace {

const double PI = 3.14159265358979323846;
const double tolerance_ = 1e-5;

int64_t ifloor(int64_t x) {
    return x & -65536;
}

int64_t iceil(int64_t x) {
    return (x+65536-1) & -65536;
}

int itrunc(int64_t x) {
    return x >> 16;
}

int ifrac(int64_t x) {
    return x & 65535;
}

bool is_top_overshoot(int64_t x) {
    return x-ifloor(x) >= 32768;
}

bool is_bottom_overshoot(int64_t x) {
    return x-iceil(x) >= 32768;
}

int smuldiv(int64_t a, int64_t b, int64_t c) {
    int64_t s = 1;
    if (a < 0) { a = -a; s = -1; }
    if (b < 0) { b = -b; s = -s; }
    if (c < 0) { c = -c; s = -s; }
    int64_t d = c > 0 ? (a*b+c/2)/c : INT_MAX;
    return s > 0 ? d : -d;
}

int smuldiv_no_round(int64_t a, int64_t b, int64_t c) {
    int64_t s = 1;
    if (a < 0) { a = -a; s = -1; }
    if (b < 0) { b = -b; s = -s; }
    if (c < 0) { c = -c; s = -s; }
    int64_t d =  c > 0 ? a*b/c : INT_MAX;
    return s > 0 ? d : -d;
}

// Borrowed from Cairo Graphics project, see cairo-arc.c.
double arc_max_angle_for_tolerance_normalized(double tolerance) {
    // Use table lookup to reduce search time in most cases.
    static const struct { double angle, error; } table[] = {
        { PI/1.0,   0.0185185185185185036127   },
        { PI/2.0,   0.000272567143730179811158 },
        { PI/3.0,   2.38647043651461047433e-05 },
        { PI/4.0,   4.2455377443222443279e-06  },
        { PI/5.0,   1.11281001494389081528e-06 },
        { PI/6.0,   3.72662000942734705475e-07 },
        { PI/7.0,   1.47783685574284411325e-07 },
        { PI/8.0,   6.63240432022601149057e-08 },
        { PI/9.0,   3.2715520137536980553e-08  },
        { PI/10.0,  1.73863223499021216974e-08 },
        { PI/11.0,  9.81410988043554039085e-09 },
    };

    double angle, error;
    int i, table_size = sizeof(table)/sizeof(*table);

    for (i = 0; i < table_size; ++i) {
        if (table[i].error < tolerance) {
            return table[i].angle;
        }
    }

    ++i;

    do {
        angle = PI/i++;
        error = 2.0/27.0*std::pow(std::sin(angle/4), 6)/std::pow(std::cos(angle/4), 2);
    } while (error > tolerance);

    return angle;
}

// Borrowed from Cairo Graphics project, see cairo-matrix.c.
double transformed_circle_major_axis(const tau::Matrix & mat, double radius) {
    if (mat.has_unity_scale()) { return radius; }

    double a = mat.xx(), b = mat.yx(), c = mat.xy(), d = mat.yy();
    double i = a*a + b*b, j = c*c + d*d;
    double f = 0.5*(i+j), g = 0.5*(i-j);
    double h = a*c + b*d;

    return radius*std::sqrt(f+std::hypot(g, h));
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

Painter_impl::Painter_impl():
    trackable()
{
    stack_.emplace_back();
    wstack_.emplace_back();
    prims_.reserve(128);
}

void Painter_impl::wreset() {
    clear();
    stack_.clear();
    wstack_.clear();
    stack_.emplace_back();
    wstack_.emplace_back();
}

// public
void Painter_impl::capture(Widget_impl * wi) {
    wstate().visible = wi->obscured();
    poffset(wi->poffset());
    pclip(wi->pclip());
}

// public
void Painter_impl::pclip(const Rect & r) {
    if (wstate().wclip != r) {
        wstate().wclip = r;
        update_clip();
    }
}

// public
void Painter_impl::push() {
    State s(stack_.back());
    stack_.push_back(s);
}

// public
void Painter_impl::pop() {
    if (stack_.size() > 1) {
        stack_.pop_back();
    }
}

// public
void Painter_impl::wpush() {
    Wstate ws(wstack_.back());
    wstack_.push_back(ws);
}

// public
void Painter_impl::wpop() {
    if (wstack_.size() > 1) {
        Rect wc = wstate().wclip;
        wstack_.pop_back();
        if (wc != wstate().wclip) { update_clip(); }
    }
}

// public
void Painter_impl::glyph(Glyph_ptr glyph) {
    flush_object();
    auto ctrs = glyph->contours();
    for (Contour & ctr: ctrs) { ctr.scale(Vector(1.0, -1.0, 1.0)); ctr.translate(position()); }

    if (!ctrs.empty()) {
        auto * o = new_prim_contour(ctrs.data(), ctrs.size());
        prims_.push_back(o);
    }
}

// public
void Painter_impl::text(const ustring & str, const Color & c) {
    flush_object();
    prims_.push_back(new_prim_text(position(), str, c));
}

// public
void Painter_impl::text(const std::u32string & str, const Color & c) {
    flush_object();
    prims_.push_back(new_prim_text(position(), str, c));
}

// public
void Painter_impl::text(std::u32string && str, const Color & c) {
    flush_object();
    prims_.push_back(new_prim_text(position(), std::move(str), c));
}

// public
void Painter_impl::pixmap(Pixmap_ptr pix, const Point pix_origin, const Size & pix_size, bool transparent) {
    flush_object();
    prims_.push_back(new_prim_pixmap(position(), pix, pix_origin, pix_size, transparent));
}

// public
void Painter_impl::pixmap(Pixmap_ptr pix, bool transparent) {
    pixmap(pix, Point(), pix->size(), transparent);
}

// public
void Painter_impl::move_to(const Vector & vec) {
    flush_object();
    position() = vec;
}

// public
void Painter_impl::move_rel(const Vector & vec) {
    flush_object();
    position() += vec;
}

// public
void Painter_impl::line_to(const Vector & end) {
    Prim_contour * c = get_contour();
    c->ctrs.back().line_to(end);
    position() = end;
}

// public
void Painter_impl::line_rel(const Vector & end) {
    Prim_contour * c = get_contour();
    c->ctrs.back().line_to(position()+end);
    position() += end;
}

// public
void Painter_impl::conic_to(const Vector & cp, const Vector & end) {
    Prim_contour * c = get_contour();
    c->ctrs.back().conic_to(cp, end);
    position() = end;
}

// public
void Painter_impl::cubic_to(const Vector & cp1, const Vector & cp2, const Vector & end) {
    Prim_contour * c = get_contour();
    c->ctrs.back().cubic_to(cp1, cp2, end);
    position() = end;
}

// public
void Painter_impl::arc(const Vector & center, double radius, double angle1, double angle2, bool pie) {
    flush_object();
    prims_.push_back(new_prim_arc(center, radius, angle1, angle2, pie));
}

// public
void Painter_impl::circle(const Vector & center, double radius) {
    arc(center, radius, 0.0, 2*3.141592654);
}

// public
void Painter_impl::rectangle(const Vector & v1, const Vector & v2) {
    flush_object();
    prims_.push_back(new_prim_rect(v1, v2));
}

// public
void Painter_impl::contour(const Contour & ctr) {
    flush_object();
    prims_.push_back(new_prim_contour(&ctr, 1, 0.5));
}

// public
void Painter_impl::contour(Contour && ctr) {
    flush_object();
    prims_.push_back(new_prim_contour(std::move(ctr), 0.5));
}

// public
// Overriden by Pixmap_painter.
void Painter_impl::paint() {
    if (visible()) {
        fill_rectangles(&wstate().wclip, 1, state().brush->color);
    }
}

void Painter_impl::stroke_preserve() {
    flush_object();

    if (visible()) {
        Prim ** pp = prims_.data(), **pe = prims_.data()+prims_.size();

        while (pp != pe) {
            if (auto * p_ctr = dynamic_cast<Prim_contour *>(*pp)) {
                for (auto & ctr: p_ctr->ctrs) { stroke_contour(ctr); }
                ++pp;
            }

            else if (auto * p_rect = dynamic_cast<Prim_rect *>(*pp)) {
                auto q = p_rect;
                std::size_t n = 1;

                while (++pp != pe) {
                    if (!dynamic_cast<Prim_rect *>(*pp)) { break; }
                    ++n;
                }

                stroke_prim_rect(q, n);
            }

            else if (auto * p_arc = dynamic_cast<Prim_arc *>(*pp)) {
                stroke_prim_arc(*p_arc);
                ++pp;
            }

            else if (auto * p_text = dynamic_cast<Prim_text *>(*pp)) {
                stroke_prim_text(*p_text);
                ++pp;
            }

            else if (auto * p_pix = dynamic_cast<Prim_pixmap *>(*pp)) {
                draw_pixmap(p_pix->pix, p_pix->origin, p_pix->size, Point(matrix()*p_pix->pos)-woffset(), p_pix->transparent);
                ++pp;
            }

            else {
                ++pp;
            }
        }
    }
}

// public
void Painter_impl::stroke() {
    stroke_preserve();
    clear();
}

// public
void Painter_impl::fill_preserve() {
    flush_object();

    if (visible()) {
        Prim ** pp = prims_.data(), **pe = prims_.data()+prims_.size();

        while (pp != pe) {
            if (auto * p_ctr = dynamic_cast<Prim_contour *>(*pp)) {
                fill_prim_contour(*p_ctr);
                ++pp;
            }

            else if (auto * p_rect = dynamic_cast<Prim_rect *>(*pp)) {
                auto q = p_rect;
                std::size_t n = 1;

                while (++pp != pe) {
                    if (!dynamic_cast<Prim_rect *>(*pp)) { break; }
                    ++n;
                }

                fill_prim_rect(q, n);
            }

            else if (auto * p_arc = dynamic_cast<Prim_arc *>(*pp)) {
                fill_prim_arc(*p_arc);
                ++pp;
            }

            else if (auto * p_text = dynamic_cast<Prim_text *>(*pp)) {
                stroke_prim_text(*p_text);
                ++pp;
            }

            else if (auto * p_pix = dynamic_cast<Prim_pixmap *>(*pp)) {
                draw_pixmap(p_pix->pix, p_pix->origin, p_pix->size, Point(matrix()*p_pix->pos)-woffset(), p_pix->transparent);
                ++pp;
            }

            else {
                ++pp;
            }
        }
    }
}

// public
void Painter_impl::fill() {
    fill_preserve();
    clear();
}

// protected
void Painter_impl::stroke_contour(const Contour & ctr) {
    if (!visible()) { return; }

    Contour tctr(ctr);
    tctr *= matrix();
    tctr.translate(-woffset());
    Point start = tctr.start();

    if (1 == tctr.order()) { // contour is linear?
        std::size_t pos = 1, npts = 1+tctr.size();
        Point pts[npts];
        pts[0] = start;
        for (const Curve & cv: tctr) { pts[pos++] = cv.end(); }

        if (const Rect r = is_rect(pts, npts)) {
            stroke_rectangle(r);
        }

        else {
            stroke_polyline(pts, npts);
        }
    }

    else {
        for (const Curve & cv: tctr) {
            if (3 == cv.order()) {
                stroke_cubic(start, cv.cp1(), cv.cp2(), cv.end());
            }

            else if (2 == cv.order()) {
                stroke_conic(start, cv.cp1(), cv.end());
            }

            else {
                Point pts[2];
                pts[0] = start;
                pts[1] = cv.end();
                stroke_polyline(pts, 1);
            }

            start = cv.end();
        }
    }
}

// protected
void Painter_impl::stroke_conic(const Vector & start, const Vector & cp, const Vector & end) {
    if (!visible()) { return; }

    std::vector<Point> pts;
    pts.push_back(start);

    Vector d = end-start;
    int dmax = 2+2*std::max(std::fabs(d.x()), std::fabs(d.y()));
    const double ts = 1.0/dmax;

    for (double t = ts; t < 1.0; t += ts) {
        Vector p0 = start+t*(cp-start);
        Vector p1 = cp+t*(end-cp);
        Point  pt = p0+t*(p1-p0);
        if (pts.back() != pt) { pts.push_back(pt); }
    }

    pts.push_back(end);
    stroke_polyline(pts.data(), pts.size());
}

// protected
void Painter_impl::stroke_cubic(const Vector & start, const Vector & cp1, const Vector & cp2, const Vector & end) {
    if (!visible()) { return; }

    std::vector<Point> pts;
    pts.push_back(start);
    Vector d = end-start;
    int dmax = 2+2*std::max(std::fabs(d.x()), std::fabs(d.y()));
    const double ts = 1.0/dmax;

    for (double t = ts; t < 1.0; t += ts) {
        Vector p0 = start+t*(cp1-start);
        Vector p1 = cp1+t*(cp2-cp1);
        Vector p2 = cp2+t*(end-cp2);
        Vector r0 = p0+t*(p1-p0);
        Vector r1 = p1+t*(p2-p1);
        Point  pt = r0+t*(r1-r0);
        if (pts.back() != pt) { pts.push_back(pt); }
    }

    pts.push_back(end);
    stroke_polyline(pts.data(), pts.size());
}

// protected
void Painter_impl::stroke_prim_arc(const Prim_arc & o) {
    stroke_contour(contour_from_arc(o.center, o.radius, o.angle1, o.angle2));
}

// protected
void Painter_impl::stroke_prim_text(const Prim_text & o) {
}

// protected
void Painter_impl::fill_prim_arc(const Prim_arc & o) {
    if (!visible()) { return; }
    Contour ctr(contour_from_arc(o.center, o.radius, o.angle1, o.angle2));
    if (o.pie) { ctr.line_to(o.center); ctr.line_to(ctr.start()); }
    ctr *= matrix();
    ctr.translate(-woffset());
    raster_contours(&ctr, 1, state().brush->color, 0.5);
}

// protected
// Overriden by Painter_xcb.
// Overriden by Painter_win.
void Painter_impl::fill_prim_contour(const Prim_contour & o) {
    if (!visible()) { return; }
    std::vector<Contour> ctrs;

    for (const Contour & ctr: o.ctrs) {
        ctrs.emplace_back(ctr);
        ctrs.back() *= matrix();
        ctrs.back().translate(-woffset());
    }

    raster_contours(ctrs.data(), ctrs.size(), state().brush->color, o.bri_thres);
}

// protected
// Overriden by Painter_xcb.
// Overriden by Painter_win.
void Painter_impl::stroke_prim_rect(const Prim_rect * po, std::size_t no) {
    while (no--) {
        Point pts[5];

        pts[0] = matrix()*po->v1;
        pts[1] = matrix()*Vector(po->v2.x(), po->v1.y());
        pts[2] = matrix()*po->v2;
        pts[3] = matrix()*Vector(po->v1.x(), po->v2.y());
        pts[4] = pts[0];

        for (int i = 0; i < 5; ++i) { pts[i] -= woffset(); }

        if (pts[0].x() == pts[3].x() && pts[1].y() == pts[2].y()) {
            Rect r(pts[0], Size(pts[1].x()-pts[0].x(), pts[2].y()-pts[1].y()));
            //stroke_rectangle(r);
        }

        else {
            //stroke_polyline(pts, 5);
        }

        ++po;
    }
}

// protected
// Overriden by Painter_xcb.
// Overriden by Painter_win.
void Painter_impl::fill_prim_rect(const Prim_rect * po, std::size_t no) {
    while (no--) {
        Point pts[5];

        pts[0] = matrix()*po->v1;
        pts[1] = matrix()*Vector(po->v2.x(), po->v1.y());
        pts[2] = matrix()*po->v2;
        pts[3] = matrix()*Vector(po->v1.x(), po->v2.y());
        pts[4] = pts[0];

        for (int i = 0; i < 5; ++i) { pts[i] -= woffset(); }

        if (pts[0].x() == pts[3].x() && pts[0].y() == pts[1].y()) {
            Rect r(pts[0], Size(pts[1].x()-pts[0].x(), pts[2].y()-pts[1].y()));
            fill_rectangles(&r, 1, state().brush->color);
        }

        else {
            //fill_polygon(pts, 5, state().brush->color);
        }

        ++po;
    }
}

// protected
Point64 Painter_impl::fixed(const Vector & vec, bool swap) {
    Point64 pt;
    pt.x = swap ? 65536*vec.y() : 65536*vec.x(); pt.x -= 32768;
    pt.y = swap ? 65536*vec.x() : 65536*vec.y(); pt.y -= 32768;
    return pt;
}

// protected
Rect Painter_impl::is_rect(const Point * pts, std::size_t npts) {
    if (4 == npts || (5 == npts && pts[0] == pts[4])) {
        int xmin = std::min(pts[0].x(), std::min(pts[1].x(), std::min(pts[2].x(), pts[3].x())));
        int ymin = std::min(pts[0].y(), std::min(pts[1].y(), std::min(pts[2].y(), pts[3].y())));
        int xmax = std::max(pts[0].x(), std::max(pts[1].x(), std::max(pts[2].x(), pts[3].x())));
        int ymax = std::max(pts[0].y(), std::max(pts[1].y(), std::max(pts[2].y(), pts[3].y())));

        for (int n = 0; n < 4; ++n) {
            if (pts[n].x() != xmin && pts[n].x() != xmax) { return Rect(); }
            if (pts[n].y() != ymin && pts[n].y() != ymax) { return Rect(); }
        }

        return Rect(xmin, ymin, Size(xmax-xmin, ymax-ymin));
    }

    return Rect();
}

// protected
void Painter_impl::arc_segment(Contour & ctr, double xc, double yc, double radius, double angle1, double angle2) {
    double rsin1 = radius*std::sin(angle1);
    double rcos1 = radius*std::cos(angle1);
    double rsin2 = radius*std::sin(angle2);
    double rcos2 = radius*std::cos(angle2);
    double h = 4.0/3.0*std::tan((angle2-angle1)/4.0);

    ctr.cubic_to(Vector(xc+rcos1-h*rsin1, yc+rsin1+h*rcos1),
                 Vector(xc+rcos2+h*rsin2, yc+rsin2-h*rcos2),
                 Vector(xc+rcos2, yc+rsin2));
}

// protected
Contour Painter_impl::contour_from_arc(const Vector & center, double radius, double angle1, double angle2) {
    radius = std::fabs(radius);
    angle1 = -angle1;
    angle2 = -angle2;
    Contour ctr(Vector(center.x()+radius*std::cos(angle1), center.y()+radius*std::sin(angle1)));
    double major_axis = transformed_circle_major_axis(matrix(), radius);
    double max_angle = arc_max_angle_for_tolerance_normalized(tolerance_/major_axis);
    int nseg = std::ceil(std::fabs(angle2-angle1)/max_angle);

    if (nseg > 0) {
        double step = (angle2-angle1)/nseg;

        for (; nseg > 1; --nseg, angle1 += step) {
            arc_segment(ctr, center.x(), center.y(), radius, angle1, angle1+step);
        }

        arc_segment(ctr, center.x(), center.y(), radius, angle1, angle2);
    }

    return ctr;
}

// private
void Painter_impl::free_prim(Prim * p) {
    if (p->ref > 0 && 0 == --p->ref) {
        if (p->heap) { delete p; return; }
        p->free();
    }
}

// public
void Painter_impl::clear() {
    Prim ** p = prims_.data();
    for (std::size_t n = prims_.size(); n; p++, n--) { free_prim(*p); }
    prims_.clear();
    if (last_) { free_prim(last_); last_ = nullptr; }
}

// private
void Painter_impl::flush_object() {
    if (last_) {
        prims_.push_back(last_);
        last_ = nullptr;
    }
}

// private
Painter_impl::Prim_contour * Painter_impl::get_contour() {
    if (auto * pc = dynamic_cast<Prim_contour *>(last_)) { return pc; }
    flush_object();
    auto * p = new_prim_contour(Contour(position()));
    last_ = p;
    return p;
}

Painter_impl::Prim_contour * Painter_impl::alloc_contour() {
    Prim_contour * p = contours_.data();

    for (std::size_t n = contours_.size(); n; n--, p++) {
        if (p->ref < 1) {
            p->ref = 1;
            return p;
        }
    }

    p = new Prim_contour;
    p->heap = true;
    return p;
}

Painter_impl::Prim_text * Painter_impl::alloc_text() {
    Prim_text * p = texts_.data();

    for (std::size_t n = texts_.size(); n; n--, p++) {
        if (p->ref < 1) {
            p->ref = 1;
            return p;
        }
    }

    p = new Prim_text;
    p->heap = true;
    return p;
}

Painter_impl::Prim_contour * Painter_impl::new_prim_contour(const Contour * pctr, std::size_t nctrs, double bri_thres) {
    Prim_contour * p = alloc_contour();
    while (nctrs--) { p->ctrs.push_back(*pctr); pctr++; }
    p->bri_thres = bri_thres;
    return p;
}

Painter_impl::Prim_contour * Painter_impl::new_prim_contour(Contour * pctr, std::size_t nctrs, double bri_thres) {
    Prim_contour * p = alloc_contour();
    while (nctrs--) { p->ctrs.emplace_back(std::move(*pctr)); pctr++; }
    p->bri_thres = bri_thres;
    return p;
}

Painter_impl::Prim_contour * Painter_impl::new_prim_contour(Contour && ctr, double bri_thres) {
    Prim_contour * p = alloc_contour();
    p->ctrs.emplace_back(std::move(ctr));
    p->bri_thres = bri_thres;
    return p;
}

Painter_impl::Prim_arc * Painter_impl::new_prim_arc(const Vector & center, double radius, double angle1, double angle2, bool pie) {
    Prim_arc * p = nullptr, * pa = arcs_.data();

    for (std::size_t n = arcs_.size(); n; n--, pa++) {
        if (pa->ref < 1) {
            pa->ref = 1;
            p = pa;
            break;
        }
    }

    if (!p) {
        p = new Prim_arc;
        p->heap = true;
    }

    p->center = center;
    p->radius = radius;
    p->angle1 = angle1;
    p->angle2 = angle2;
    p->pie = pie;
    return p;
}

Painter_impl::Prim_rect * Painter_impl::new_prim_rect(const Vector & v1, const Vector & v2) {
    Prim_rect * p = nullptr, * pa = rects_.data();

    for (std::size_t n = rects_.size(); n; n--, pa++) {
        if (pa->ref < 1) {
            pa->ref = 1;
            p = pa;
            break;
        }
    }

    if (!p) {
        p = new Prim_rect;
        p->heap = true;
    }

    double x1 = std::min(v1.x(), v2.x());
    double y1 = std::min(v1.y(), v2.y());
    double x2 = std::max(v1.x(), v2.x());
    double y2 = std::max(v1.y(), v2.y());
    p->v1.set(x1, y1);
    p->v2.set(x2, y2);

    return p;
}

Painter_impl::Prim_text * Painter_impl::new_prim_text(const Vector & pos, const ustring & str, const Color & color) {
    Prim_text * p = alloc_text();
    p->pos = pos;
    p->str.assign(str.size(), ' ');
    std::size_t i = 0;
    for (char32_t wc: str) { p->str[i++] = wc; }
    p->color = color;
    return p;
}

Painter_impl::Prim_text * Painter_impl::new_prim_text(const Vector & pos, const std::u32string & str, const Color & color) {
    Prim_text * p = alloc_text();
    p->pos = pos;
    p->str = str;
    p->color = color;
    return p;
}

Painter_impl::Prim_text * Painter_impl::new_prim_text(const Vector & pos, std::u32string && str, const Color & color) {
    Prim_text * p = alloc_text();
    p->pos = pos;
    p->str = std::move(str);
    p->color = color;
    return p;
}

Painter_impl::Prim_pixmap * Painter_impl::new_prim_pixmap(const Vector & pos, Pixmap_ptr pix, const Point & origin, const Size & size, bool transparent) {
    Prim_pixmap * p = nullptr, * pa = pixmaps_.data();

    for (std::size_t n = pixmaps_.size(); n; n--, pa++) {
        if (pa->ref < 1) {
            pa->ref = 1;
            p = pa;
            break;
        }
    }

    if (!p) {
        p = new Prim_pixmap;
        p->heap = true;
    }

    p->pos = pos;
    p->pix = pix;
    p->origin = origin;
    p->size = size;
    p->transparent = transparent;
    return p;
}

// ----------------------------------------------------------------------------
// Raster stuff.
// ----------------------------------------------------------------------------

// private
void Painter_impl::new_raster_profile(Raster & ras, int state, bool overshoot) {
    ras.rstate = state;
    ras.fresh = true;
    ras.joint = false;

    if (!ras.pros.empty()) { ras.pros.back().inext = ras.pros.size(); }
    ras.pros.emplace_back();
    Raster_profile & p = ras.pros.back();

    p.start  = 0;
    p.iself = ras.pros.size()-1;
    p.inext = ras.pros.size()-1;
    p.ix = ras.xs.size();
    p.height = 0;

    if (-1 == state) {
        p.ascend = false;
        p.overtop = overshoot;
        p.overbot = false;
    }

    else {
        p.ascend = true;
        p.overbot = overshoot;
        p.overtop = false;
    }
}

// private
void Painter_impl::end_raster_profile(Raster & ras, bool overshoot) {
    ras.joint = false;

    if (!ras.pros.empty()) {
        auto & p = ras.pros.back();
        p.height = ras.xs.size()-p.ix;

        if (p.height > 0) {
            if (p.ascend) {
                p.overtop = overshoot;
                p.overbot = false;
            }

            else {
                p.overbot = overshoot;
                p.overtop = false;
            }

            p.inext = ras.pros.size();
        }

        else {
            ras.pros.pop_back();
            if (!ras.pros.empty()) { ras.pros.back().inext--; }
        }
    }
}

// private
void Painter_impl::raster_line_up(Raster & ras, int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
    int64_t dx = x2-x1, dy = y2-y1;

    if (dy > 0) {
        int e1 = itrunc(y1), e2 = itrunc(y2);
        int f1 = ifrac(y1),  f2 = ifrac(y2);

        if (f1 > 0) {
            if (e1 == e2) { return; }
            else { x1 += smuldiv(dx, 65536-f1, dy); ++e1; }
        }

        else {
            if (ras.joint) {
                ras.joint = false;
                ras.xs.pop_back();
            }
        }

        ras.joint = 0 == f2;

        if (ras.fresh) {
            ras.pros.back().start = e1;
            ras.fresh = false;
        }

        int ix, rx;

        if (dx > 0) {
            ix = smuldiv_no_round(65536, dx, dy);
            rx = 65536*dx%dy;
            dx = 1;
        }

        else {
            ix = -smuldiv_no_round(65536, -dx, dy);
            rx = -65536*dx%dy;
            dx = -1;
        }

        int64_t ax = -dy;

        for (int size = e2-e1+1; size > 0; --size) {
            ras.xs.push_back(x1);
            x1 += ix;
            dx += rx;
            if (ax >= 0) { ax -= dy; x1 += dx; }
        }
    }
}

// private
void Painter_impl::raster_line_to(Raster & ras, int64_t ex, int64_t ey) {
    if (1 == ras.rstate) {
        if (ey < ras.y) {
            end_raster_profile(ras, is_top_overshoot(ras.y));
            new_raster_profile(ras, -1, is_top_overshoot(ras.y));
        }
    }

    else if (-1 == ras.rstate) {
        if (ey > ras.y) {
            end_raster_profile(ras, is_bottom_overshoot(ras.y));
            new_raster_profile(ras, 1, is_bottom_overshoot(ras.y));
        }
    }

    else {
        if (ey > ras.y) {
            new_raster_profile(ras, 1, is_bottom_overshoot(ras.y));
        }

        else if (ey < ras.y) {
            new_raster_profile(ras, -1, is_top_overshoot(ras.y));
        }
    }

    if (1 == ras.rstate) {
        raster_line_up(ras, ras.x, ras.y, ex, ey);
    }

    else {
        bool fresh = ras.fresh;
        raster_line_up(ras, ras.x, -ras.y, ex, -ey);
        if (fresh && !ras.fresh) { ras.pros.back().start = -ras.pros.back().start; }
    }

    ras.x = ex;
    ras.y = ey;
}

// private
void Painter_impl::raster_bezier_up(Raster & ras, int order) {
    int64_t y1 = ras.arc[order].y;
    int64_t y2 = ras.arc[0].y;
    int e2 = ifloor(y2);
    int e  = iceil(y1);

    if (0 == ifrac(y1)) {
        if (ras.joint) ras.xs.pop_back(), ras.joint = false;
        ras.xs.push_back(ras.arc[order].x);
        e += 65536;
    }

    if (ras.fresh) {
        ras.pros.back().start = itrunc(e);
        ras.fresh = false;
    }

    if (e <= e2) {
        int ai = 0;

        while (e <= e2) {
            ras.joint = false;
            y2 = ras.arc[ai+0].y;

            if (e < y2) {
                y1 = ras.arc[ai+order].y;

                if (y2-y1 > 255) {
                    if (std::size_t(ai+order+4) > ras.arc.size()) {
                        ras.arc.resize(ai+order+4);
                    }

                    if (3 == order) {
                        int64_t a, b, c, d;

                        ras.arc[ai+6].x = ras.arc[ai+3].x;
                        c = ras.arc[ai+1].x;
                        d = ras.arc[ai+2].x;
                        ras.arc[ai+1].x = a = (ras.arc[ai+0].x+c+1) >> 1;
                        ras.arc[ai+5].x = b = (ras.arc[ai+3].x+d+1) >> 1;
                        c = (c+d+1) >> 1;
                        ras.arc[ai+2].x = a = (a+c+1) >> 1;
                        ras.arc[ai+4].x = b = (b+c+1) >> 1;
                        ras.arc[ai+3].x = (a+b+1) >> 1;

                        ras.arc[ai+6].y = ras.arc[ai+3].y;
                        c = ras.arc[ai+1].y;
                        d = ras.arc[ai+2].y;
                        ras.arc[ai+1].y = a = (ras.arc[ai+0].y+c+1) >> 1;
                        ras.arc[ai+5].y = b = (ras.arc[ai+3].y+d+1) >> 1;
                        c = (c+d+1) >> 1;
                        ras.arc[ai+2].y = a = (a+c+1) >> 1;
                        ras.arc[ai+4].y = b = (b+c+1) >> 1;
                        ras.arc[ai+3].y = (a+b+1) >> 1;
                    }

                    else {
                        int64_t a, b;

                        ras.arc[ai+4].x = ras.arc[ai+2].x;
                        b = ras.arc[ai+1].x;
                        a = ras.arc[ai+3].x = (ras.arc[ai+2].x+b)/2;
                        b = ras.arc[ai+1].x = (ras.arc[ai+0].x+b)/2;
                        ras.arc[ai+2].x = (a+b)/2;

                        ras.arc[ai+4].y = ras.arc[ai+2].y;
                        b = ras.arc[ai+1].y;
                        a = ras.arc[ai+3].y = (ras.arc[ai+2].y+b)/2;
                        b = ras.arc[ai+1].y = (ras.arc[ai+0].y+b)/2;
                        ras.arc[ai+2].y = (a+b)/2;
                    }

                    ai += order;
                    continue;
                }

                if (y1 != y2) {
                    ras.xs.push_back(ras.arc[ai+order].x+(ras.arc[ai+0].x-ras.arc[ai+order].x)*(e-y1)/(y2-y1));
                    e += 65536;
                }
            }

            else if (e == y2) {
                ras.joint = true;
                ras.xs.push_back(ras.arc[ai+0].x);
                e += 65536;
            }

            ai -= order;
            if (ai < 0) break;
        }
    }
}

// private
void Painter_impl::raster_conic_to(Raster & ras, int64_t cx, int64_t cy, int64_t ex, int64_t ey) {
    if (ras.y != ey) {
        int st = ras.y < ey ? 1 : -1;

        if (ras.rstate != st) {
            bool o = st == 1 ? is_bottom_overshoot(ras.y) : is_top_overshoot(ras.y);
            if (0 != ras.rstate) { end_raster_profile(ras, o); }
            new_raster_profile(ras, st, o);
        }

        if (1 == st) {
            ras.arc[0].x = ex;
            ras.arc[0].y = ey;
            ras.arc[1].x = cx;
            ras.arc[1].y = cy;
            ras.arc[2].x = ras.x;
            ras.arc[2].y = ras.y;
            raster_bezier_up(ras, 2);
        }

        else {
            ras.arc[0].x = ex;
            ras.arc[0].y = -ey;
            ras.arc[1].x = cx;
            ras.arc[1].y = -cy;
            ras.arc[2].x = ras.x;
            ras.arc[2].y = -ras.y;
            bool fresh = ras.fresh;
            raster_bezier_up(ras, 2);
            if (fresh && !ras.fresh) { ras.pros.back().start = -ras.pros.back().start; }
        }
    }

    ras.x = ex;
    ras.y = ey;
}

// private
void Painter_impl::raster_cubic_to(Raster & ras, int64_t cx1, int64_t cy1, int64_t cx2, int64_t cy2, int64_t ex, int64_t ey) {
    if (ras.y != ey) {
        int state_bez = ras.y <= ey ? 1 : -1;

        if (ras.rstate != state_bez) {
            bool o = state_bez == 1 ? is_bottom_overshoot(ras.y) : is_top_overshoot(ras.y);
            if (ras.rstate != 0) { end_raster_profile(ras, o); }
            new_raster_profile(ras, state_bez, o);
        }

        if (state_bez == 1) {
            ras.arc[0].x = ex;
            ras.arc[0].y = ey;
            ras.arc[1].x = cx2;
            ras.arc[1].y = cy2;
            ras.arc[2].x = cx1;
            ras.arc[2].y = cy1;
            ras.arc[3].x = ras.x;
            ras.arc[3].y = ras.y;
            raster_bezier_up(ras, 3);
        }

        else {
            ras.arc[0].x = ex;
            ras.arc[0].y = -ey;
            ras.arc[1].x = cx2;
            ras.arc[1].y = -cy2;
            ras.arc[2].x = cx1;
            ras.arc[2].y = -cy1;
            ras.arc[3].x = ras.x;
            ras.arc[3].y = -ras.y;
            bool fresh = ras.fresh;
            raster_bezier_up(ras, 3);
            if (fresh && !ras.fresh) { ras.pros.back().start = -ras.pros.back().start; }
        }
    }

    ras.x = ex;
    ras.y = ey;
}

// private
void Painter_impl::sort_raster_profiles(Raster & ras, std::vector<int> & v) {
    for (int n: v) {
        Raster_profile & p = ras.pros[n];
        p.x = ras.xs[p.ix];
        if (p.ascend) { p.ix++; }
        else  { p.ix--; }
        p.height--;
    }

    std::sort(v.begin(), v.end(), [ras](int i, int j) { return ras.pros[i].x < ras.pros[j].x; });
}

// private
void Painter_impl::raster_sweep(Raster & ras, bool horz) {
    std::vector<int> wt, dl, dr;

    // first, compute min and max Y
    int ymax = INT_MIN;
    int ymin = INT_MAX;

    for (unsigned n = 0; n < ras.pros.size(); ++n) {
        Raster_profile & p = ras.pros[n];
        int bottom = p.start;
        int top = p.start+p.height-1;
        ymin = std::min(ymin, bottom);
        ymax = std::max(ymax, top);
        p.x = 0;
        wt.push_back(n);
    }

    // compute the distance of each profile from ymin
    for (Raster_profile & p: ras.pros) {
        p.count = p.start-ymin;
    }

    // let's go
    int y = ymin, y_height = 0;

    for (auto iter: ras.turns) {
        int y_change = iter.second;

        if (y_change != ymin) {
            for (int n: std::vector<int>(wt)) {
                Raster_profile & p = ras.pros[n];
                p.count -= y_height;

                if (0 == p.count) {
                    auto j = std::find(wt.begin(), wt.end(), n);
                    if (j != wt.end()) { wt.erase(j); }
                    if (p.ascend) { dl.push_back(p.iself); }
                    else { dr.push_back(p.iself); }
                }
            }

            // sort the drawing lists
            sort_raster_profiles(ras, dl);
            sort_raster_profiles(ras, dr);
            y_height = y_change-y;

            while (y < y_change) {
                //std::cout << "y= " << y << " y_change= " << y_change << " turns= " << ras.turns.size() << "\n";
                auto li = dl.begin(), ri = dr.begin();

                for (; li != dl.end() && ri != dr.end(); ++li, ++ri) {
                    Raster_profile & left = ras.pros[*li];
                    Raster_profile & right = ras.pros[*ri];
                    int x1 = 32768+std::min(left.x, right.x);
                    int x2 = 32768+std::max(left.x, right.x);
                    int e1 = itrunc(x1), e2 = itrunc(x2);
                    double mul = 1.0/65536;

                    if (horz) {
                        if (e1 < e2) {
                            double bri = double(65536-ifrac(x1))*mul;
                            if (bri >= ras.bri_thres) { raster_fill_rectangle(ras, y, e1, y, e1, ras.color.darken(1.0-bri)); }
                            //bri = double(ifrac(x2))*mul;
                            //if (bri >= ras.bri_thres) { raster_fill_rectangle(ras, y, e2, y, e2, ras.color.darken(1.0-bri)); }
                        }

                        else {
                            double bri = double(ifrac(x2-x1))*mul;
                            if (bri >= ras.bri_thres) { raster_fill_rectangle(ras, y, e1, y, e1, ras.color.darken(1.0-bri)); }
                        }
                    }

                    else {
                        if (e1 < e2) {
                            double bri = double(65536-ifrac(x1))*mul;
                            if (bri >= ras.bri_thres) { raster_fill_rectangle(ras, e1, y, e1, y, ras.color.darken(1.0-bri)); }
                            bri = double(ifrac(x2))*mul;
                            if (bri >= ras.bri_thres) { raster_fill_rectangle(ras, e2, y, e2, y, ras.color.darken(1.0-bri)); }
                            if (e2-e1 > 1) { raster_fill_rectangle(ras, e1+1, y, e2-1, y, ras.color); }
                        }

                        else {
                            double bri = double(ifrac(x2-x1))*mul;
                            if (bri >= ras.bri_thres) { raster_fill_rectangle(ras, e1, y, e1, y, ras.color.darken(1.0-bri)); }
                        }
                    }
                }

                if (++y < y_change) {
                    sort_raster_profiles(ras, dl);
                    sort_raster_profiles(ras, dr);
                }
            }

            for (int n: std::vector<int>(dl)) {
                if (0 == ras.pros[n].height) {
                    auto j = std::find(dl.begin(), dl.end(), n);
                    if (j != dl.end()) dl.erase(j);
                }
            }

            for (int n: std::vector<int>(dr)) {
                if (0 == ras.pros[n].height) {
                    auto j = std::find(dr.begin(), dr.end(), n);
                    if (j != dr.end()) dr.erase(j);
                }
            }
        }
    }
}

// private
void Painter_impl::raster_add_contour(Raster & ras, const Contour & ctr, bool horz) {
    if (!ctr.empty()) {
        ras.rstate = 0;
        std::size_t ifirst = ras.pros.size();
        Point64 start = fixed(ctr.start(), horz);
        ras.x = start.x, ras.y = start.y;

        for (const Curve & cv : ctr) {
            Point64 end = fixed(cv.end(), horz);

            if (3 == cv.order()) {
                //std::cout << "curve start=" << ctr.start().x() << ":" << ctr.start().y() << " end= " << cv.end().x() << ":" << cv.end().y() << "\n";
                Point64 cp1 = fixed(cv.cp1(), horz), cp2 = fixed(cv.cp2(), horz);
                raster_cubic_to(ras, cp1.x, cp1.y, cp2.x, cp2.y, end.x, end.y);
            }

            else if (2 == cv.order()) {
                Point64 cp = fixed(cv.cp1(), horz);
                raster_conic_to(ras, cp.x, cp.y, end.x, end.y);
            }

            else {
                raster_line_to(ras, end.x, end.y);
            }
        }

        raster_line_to(ras, start.x, start.y);

        if (!ras.pros.empty()) {
            bool o = ras.pros.back().ascend ? is_top_overshoot(ras.y) : is_bottom_overshoot(ras.y);
            end_raster_profile(ras, o);
            if (ifirst < ras.pros.size()) { ras.pros.back().inext = ifirst; }
        }
    }
}

// private
void Painter_impl::raster_pass(Raster & ras, const Contour * ctrs, std::size_t nctrs, bool horz) {
    while (nctrs--) { raster_add_contour(ras, *ctrs++, horz); }

    // finalize.
    for (Raster_profile & p: ras.pros) {
        int bottom, top;

        if (p.ascend) {
            bottom = p.start;
            top = p.start+p.height-1;
            //std::cout << "ascend, bottom= " << bottom << ", top= " << top << "\n";
        }

        else {
            bottom = p.start-p.height+1;
            top = p.start;
            p.start = bottom;
            p.ix += p.height-1;
            //std::cout << "descend, bottom= " << bottom << ", top= " << top << "\n";
        }

        ras.turns[bottom] = bottom;
        ras.turns[top+1] = top+1;
    }

    raster_sweep(ras, horz);
    ras.pros.clear();
    ras.turns.clear();
    ras.xs.clear();
    ras.joint = false;
    ras.fresh = false;
}

// private
void Painter_impl::raster_fill_rectangle(Raster & ras, int x1, int y1, int x2, int y2, const Color & c) {
    Rect r(x1, y1, x2, y2);
    r &= wstate().wclip;
    if (r) { fill_rectangles(&r, 1, c); }
}

// private
void Painter_impl::raster_contours(const Contour * ctrs, std::size_t nctrs, const Color & color, double bri_thres) {
    Raster ras;
    ras.color = color;
    ras.bri_thres = bri_thres;
    raster_pass(ras, ctrs, nctrs, false);
    raster_pass(ras, ctrs, nctrs, true);
}

} // namespace tau

//END
