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

#ifndef TAU_PAINTER_IMPL_HH
#define TAU_PAINTER_IMPL_HH

#include <tau/brush.hh>
#include <tau/color.hh>
#include <tau/contour.hh>
#include <tau/matrix.hh>
#include <tau/pen.hh>
#include <tau/signal.hh>
#include <sys-impl.hh>
#include <array>
#include <forward_list>
#include <vector>

namespace tau {

class Widget_impl;
struct Point64 { int64_t x, y; };

class Painter_impl: public trackable {
protected:

    Painter_impl();
    virtual ~Painter_impl() {}

public:

    Painter_impl(const Painter_impl & other) = delete;
    Painter_impl & operator=(const Painter_impl & other) = delete;

    void capture(Widget_impl * wi);
    void set_obscured_area(const Rect & oa);
    void poffset(const Point & pt) { wstate().woffset_ = pt; }
    void wpush();
    void wpop();
    void wreset();
    void reserve_stack(std::size_t n);

    void push();
    void pop();
    void clear();

    void text(const ustring & text, const Color & c);
    void text(const std::u32string & text, const Color & c);
    void text(std::u32string && text, const Color & c);
    void glyph(Glyph_ptr glyph);

    Font_ptr font() { return state().font_; }
    Matrix & matrix() { return state().mat_; }

    // Overriden by Pixmap_painter.
    virtual void paint();

    void fill();
    void fill_preserve();

    void stroke();
    void stroke_preserve();

    void set_pen(Pen pen) { state().pen_ = pen.impl; }
    Pen pen() const { return Pen(state().pen_); }

    void  set_brush(Brush brush) { state().brush_ = brush.impl; }
    Brush brush() const { return Brush(state().brush_); }

    void pixmap(Pixmap_cptr pix, const Point pix_origin, const Size & pix_size, bool transparent);
    void pixmap(Pixmap_cptr pix, bool transparent);

    void set_oper(Oper op) { state().op_ = op; }
    Oper oper() const { return state().op_; }

    void move_to(double x, double y) { move_to(Vector(x, y)); }
    void move_to(const Vector & vec);
    void move_rel(double x, double y) { move_rel(Vector(x, y)); }
    void move_rel(const Vector & vec);

    const Vector & position() const { return state().pos_; }
    Vector & position() { return state().pos_; }

    void rectangle(const Vector & v1, const Vector & v2);
    void rectangle(double x1, double y1, double x2, double y2) { rectangle(Vector(x1, y1), Vector(x2, y2)); }

    void line_to(const Vector & vec);
    void line_to(double x, double y) { line_to(Vector(x, y)); }

    void line_rel(const Vector & vec);
    void line_rel(double x, double y) { line_rel(Vector(x, y)); }

    void conic_to(const Vector & cp, const Vector & end);
    void conic_to(double cx, double cy, double ex, double ey) { conic_to(Vector(cx, cy), Vector(ex, ey)); }

    void cubic_to(const Vector & cp1, const Vector & cp2, const Vector & end);

    void cubic_to(double cx1, double cy1, double cx2, double cy2, double ex, double ey) {
        cubic_to(Vector(cx1, cy1), Vector(cx2, cy2), Vector(ex, ey));
    }

    void arc(const Vector & center, double radius, double angle1, double angle2, bool pie=false);

    void arc(double cx, double cy, double radius, double angle1, double angle2, bool pie=false) {
        arc(Vector(cx, cy), radius, angle1, angle2, pie);
    }

    void circle(const Vector & center, double radius);
    void circle(double cx, double cy, double radius) { circle(Vector(cx, cy), radius); }

    void contour(const Contour & ctr);
    void contour(Contour && ctr);

    virtual void set_font(Font_ptr fp) = 0;
    virtual Font_ptr select_font(const ustring & spec) = 0;
    virtual Vector text_size(const ustring & s) = 0;
    virtual Vector text_size(const std::u32string & s) = 0;

protected:

    struct Raster_profile {
        int64_t         x_ = 0;             // current coordinate during sweep
        bool            ascend_ = false;
        bool            overtop_ = false;
        bool            overbot_ = false;
        int             height_ = 0;        // profile's height in scanlines
        int             start_ = 0;         // profile's starting scanline
        std::size_t     count_ = 0;         // number of lines to step before this profile becomes drawable
        std::size_t     ix_ = 0;
    };

#if TAU_HAS_VALLOCATOR
    using Turns = std::forward_list<int, v_allocator<int>>;
    using RP_list = std::forward_list<Raster_profile *, v_allocator<Raster_profile *>>;
#else
    using Turns = std::forward_list<int>;
    using RP_list = std::forward_list<Raster_profile *>;
#endif
    using Raster_profiles = std::vector<Raster_profile>;
    using Arcs = std::vector<Point64>;
    using Points = std::vector<int64_t>;

    struct Raster {
        int64_t         x_;
        int64_t         y_;
        Color           color_;
        bool            fresh_ = false;
        bool            joint_ = false;
        int             rstate_ = 0;
        Turns           turns_;
        Arcs            arc_ { 32 };
        Points          xs_;
        Raster_profiles pros_;
    };

    struct Prim {
        bool            heap = false;
        int             ref = 0;

        virtual ~Prim() {}
        virtual void free() {}
    };

    struct Prim_contour: Prim {
        std::vector<Contour> ctrs;
        void free() override { ctrs.clear(); }
    };

    struct Prim_arc: Prim {
        Vector          center;
        double          radius;
        double          angle1;
        double          angle2;
        bool            pie;
    };

    struct Prim_rect: Prim {
        Vector v1, v2;
    };

    struct Prim_text: Prim {
        Vector          pos;
        std::u32string  str;
        Color           color;

        void free() override { str.clear(); }
    };

    struct Prim_pixmap: Prim {
        Vector          pos;
        Pixmap_cptr     pix;
        Point           origin;
        Size            size;
        bool            transparent;

        void free() override { pix.reset(); }
    };

    struct State {
        Vector          pos_;
        Matrix          mat_;
        Oper            op_ = OPER_COPY;
        ustring         fontspec_;
        Font_ptr        font_;
        Brush_ptr       brush_;
        Pen_ptr         pen_;
    };

    struct Wstate {
        bool            visible_ = true;
        Rect            obscured_;
        Point           woffset_;
    };

private:

    using Stack = std::vector<State>;
    using Wstack = std::vector<Wstate>;
    using Prims = std::vector<Prim *>;

    Stack       stack_;
    Wstack      wstack_;
    Prims       prims_;
    Prim *      last_ = nullptr;

    std::array<Prim_contour, 64> contours_;
    std::array<Prim_arc,     16> arcs_;
    std::array<Prim_rect,    64> rects_;
    std::array<Prim_text,    32> texts_;
    std::array<Prim_pixmap,   8> pixmaps_;

protected:

    Prim_contour * new_prim_contour(const Contour * pctr, std::size_t nctrs);
    Prim_contour * new_prim_contour(Contour * pctr, std::size_t nctrs);
    Prim_contour * new_prim_contour(Contour && ctr);
    Prim_arc * new_prim_arc(const Vector & center, double radius, double angle1, double angle2, bool pie);
    Prim_rect * new_prim_rect(const Vector & v1, const Vector & v2);
    Prim_text * new_prim_text(const Vector & pos, const ustring & str, const Color & color);
    Prim_text * new_prim_text(const Vector & pos, const std::u32string & str, const Color & color);
    Prim_text * new_prim_text(const Vector & pos, std::u32string && str, const Color & color);
    Prim_pixmap * new_prim_pixmap(const Vector & pos, Pixmap_cptr pix, const Point & origin, const Size & size, bool transparent);

    Prim_contour * alloc_contour();
    Prim_text * alloc_text();
    void free_prim(Prim * p);

    virtual void update_clip() = 0;
    virtual void fill_rectangles(const Rect * r, std::size_t nrs, const Color & color) = 0;
    virtual void stroke_polyline(const Point * pts, std::size_t npts) = 0;

    virtual void stroke_rectangle(const Rect & r) = 0;
    virtual void fill_polygon(const Point * pts, std::size_t npts, const Color & color) = 0;
    virtual void draw_pixmap(Pixmap_cptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) = 0;

    // Overriden by Painter_xcb.
    // Overriden by Painter_win.
    virtual void stroke_prim_text(const Prim_text & o);

    // Overriden by Painter_xcb.
    // Overriden by Painter_win.
    virtual void stroke_prim_rect(const Prim_rect * po, std::size_t no);

    // Overriden by Painter_xcb.
    // Overriden by Painter_win.
    virtual void fill_prim_rect(const Prim_rect * po, std::size_t no);

    // Overriden by Painter_xcb.
    // Overriden by Painter_win.
    virtual void fill_prim_contour(const Prim_contour & o);

    virtual void stroke_prim_arc(const Prim_arc & o);
    virtual void fill_prim_arc(const Prim_arc & o);

    State & state() { return stack_.back(); }
    const State & state() const { return stack_.back(); }

    Wstate & wstate() { return wstack_.back(); }
    const Wstate & wstate() const { return wstack_.back(); }

    const Point & woffset() const { return wstate().woffset_; }
    bool visible() const { return wstate().visible_; }

    Rect is_rect(const Point * pts, std::size_t npts);
    void arc_segment(Contour & ctr, double xc, double yc, double radius, double angle1, double angle2);
    Contour contour_from_arc(const Vector & center, double radius, double angle1, double angle2);

    void stroke_contour(const Contour & ctr);
    void stroke_conic(const Vector & start, const Vector & cp, const Vector & end);
    void stroke_cubic(const Vector & start, const Vector & cp1, const Vector & cp2, const Vector & end);

private:

    // ----- Raster stuff -----
    void new_raster_profile(Raster & ras, int state, bool overshoot);
    void end_raster_profile(Raster & ras, bool overshoot);
    void raster_line_up(Raster & ras, int64_t x1, int64_t y1, int64_t x2, int64_t y2);
    void raster_line_to(Raster & ras, int64_t ex, int64_t ey);
    void raster_bezier_up(Raster & ras, int order);
    void raster_conic_to(Raster & ras, int64_t cx, int64_t cy, int64_t ex, int64_t ey);
    void raster_cubic_to(Raster & ras, int64_t cx1, int64_t cy1, int64_t cx2, int64_t cy2, int64_t ex, int64_t ey);
    void sort_raster_profiles(Raster & ras, RP_list & v);
    void raster_sweep(Raster & ras, bool horz);
    void raster_add_contour(Raster & ras, const Contour & ctr, bool horz);
    void raster_pass(Raster & ras, const Contour * ctrs, std::size_t nctrs, bool horz);
    void raster_fill_rectangle(Raster & ras, int x1, int y1, int x2, int y2, const Color & c);
    void raster_contours(const Contour * ctrs, std::size_t nctrs, const Color & color);
    // ----- Raster stuff -----

    void flush_object();
    Prim_contour * get_contour();
};

} // namespace tau

#endif // TAU_PAINTER_IMPL_HH
