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

#ifndef TAU_PAINTER_IMPL_HH
#define TAU_PAINTER_IMPL_HH

#include <tau/brush.hh>
#include <tau/color.hh>
#include <tau/contour.hh>
#include <tau/matrix.hh>
#include <tau/pen.hh>
#include <tau/signal.hh>
#include <array>
#include <list>
#include <map>
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
    void pclip(const Rect & sclip);
    void poffset(const Point & pt) { wstate().woffset = pt; }
    void wpush();
    void wpop();
    void wreset();
    void push();
    void pop();
    void clear();

    void text(const ustring & text, const Color & c);
    void text(const std::u32string & text, const Color & c);
    void text(std::u32string && text, const Color & c);
    void glyph(Glyph_ptr glyph);

    Font_ptr font() { return state().font; }
    Matrix & matrix() { return state().mat; }

    // Overriden by Pixmap_painter.
    virtual void paint();

    void fill();
    void fill_preserve();

    void stroke();
    void stroke_preserve();

    void set_pen(Pen pen) { state().pen = pen.impl; }
    Pen pen() const { return Pen(state().pen); }

    void  set_brush(Brush brush) { state().brush = brush.impl; }
    Brush brush() const { return Brush(state().brush); }

    void pixmap(Pixmap_ptr pix, const Point pix_origin, const Size & pix_size, bool transparent);
    void pixmap(Pixmap_ptr pix, bool transparent);

    void set_oper(Oper op) { state().op = op; }
    Oper oper() const { return state().op; }

    void move_to(double x, double y) { move_to(Vector(x, y)); }
    void move_to(const Vector & vec);
    void move_rel(double x, double y) { move_rel(Vector(x, y)); }
    void move_rel(const Vector & vec);

    const Vector & position() const { return state().pos; }
    Vector & position() { return state().pos; }

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
        int64_t         x;          // current coordinate during sweep
        bool            ascend;
        bool            overtop;
        bool            overbot;
        int             iself;
        int             inext;
        int             height;     // profile's height in scanlines
        int             start;      // profile's starting scanline
        std::size_t     count;      // number of lines to step before this profile becomes drawable
        std::size_t     ix;
    };

    using Turns = std::map<int64_t, int64_t>;
    using Raster_profiles = std::vector<Raster_profile>;
    using Arcs = std::vector<Point64>;
    using Points = std::vector<int64_t>;

    struct Raster {
        int64_t         x;
        int64_t         y;
        Color           color;
        bool            fresh = false;
        bool            joint = false;
        int             rstate = 0;
        Turns           turns;
        Arcs            arc { 32 };
        Points          xs;
        Raster_profiles pros;
        double          bri_thres = 0.0;
    };

    struct Prim {
        bool            heap = false;
        int             ref = 0;

        virtual ~Prim() {}
        virtual void free() {}
    };

    struct Prim_contour: Prim {
        std::vector<Contour> ctrs;
        double bri_thres = 0.0;

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
        Pixmap_ptr      pix;
        Point           origin;
        Size            size;
        bool            transparent;

        void free() override { pix.reset(); }
    };

    struct State {
        Vector          pos;
        Matrix          mat;
        Oper            op = OPER_COPY;
        ustring         font_spec;
        Font_ptr        font;
        Brush_ptr       brush;
        Pen_ptr         pen;
    };

    struct Wstate {
        bool            visible = true;
        Rect            wclip;
        Point           woffset;
    };

private:

    using Stack = std::list<State>;
    using Wstack = std::list<Wstate>;
    using Prims = std::vector<Prim *>;

    Stack       stack_;
    Wstack      wstack_;
    Prims       prims_;
    Prim *      last_ = nullptr;

    std::array<Prim_contour, 32> contours_;
    std::array<Prim_arc,     64> arcs_;
    std::array<Prim_rect,    64> rects_;
    std::array<Prim_text,    64> texts_;
    std::array<Prim_pixmap,  32> pixmaps_;

protected:

    Prim_contour * new_prim_contour(const Contour * pctr, std::size_t nctrs, double bri_thres=0.0);
    Prim_contour * new_prim_contour(Contour * pctr, std::size_t nctrs, double bri_thres=0.0);
    Prim_contour * new_prim_contour(Contour && ctr, double bri_thres=0.0);
    Prim_arc * new_prim_arc(const Vector & center, double radius, double angle1, double angle2, bool pie);
    Prim_rect * new_prim_rect(const Vector & v1, const Vector & v2);
    Prim_text * new_prim_text(const Vector & pos, const ustring & str, const Color & color);
    Prim_text * new_prim_text(const Vector & pos, const std::u32string & str, const Color & color);
    Prim_text * new_prim_text(const Vector & pos, std::u32string && str, const Color & color);
    Prim_pixmap * new_prim_pixmap(const Vector & pos, Pixmap_ptr pix, const Point & origin, const Size & size, bool transparent);

    Prim_contour * alloc_contour();
    Prim_text * alloc_text();
    void free_prim(Prim * p);

    virtual void update_clip() = 0;
    virtual void fill_rectangles(const Rect * r, std::size_t nrs, const Color & color) = 0;
    virtual void stroke_polyline(const Point * pts, std::size_t npts) = 0;

    virtual void stroke_rectangle(const Rect & r) = 0;
    virtual void fill_polygon(const Point * pts, std::size_t npts, const Color & color) = 0;
    virtual void draw_pixmap(Pixmap_ptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) = 0;

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

    const Point & woffset() const { return wstate().woffset; }
    bool visible() const { return wstate().visible; }

    Rect is_rect(const Point * pts, std::size_t npts);
    void arc_segment(Contour & ctr, double xc, double yc, double radius, double angle1, double angle2);
    Contour contour_from_arc(const Vector & center, double radius, double angle1, double angle2);
    Point64 fixed(const Vector & vec, bool swap);

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
    void sort_raster_profiles(Raster & ras, std::vector<int> & v);
    void raster_sweep(Raster & ras, bool horz);
    void raster_add_contour(Raster & ras, const Contour & ctr, bool horz);
    void raster_pass(Raster & ras, const Contour * ctrs, std::size_t nctrs, bool horz);
    void raster_fill_rectangle(Raster & ras, int x1, int y1, int x2, int y2, const Color & c);
    void raster_contours(const Contour * ctrs, std::size_t nctrs, const Color & color, double bri_thres);
    // ----- Raster stuff -----

    void flush_object();
    Prim_contour * get_contour();
};

} // namespace tau

#endif // TAU_PAINTER_IMPL_HH
