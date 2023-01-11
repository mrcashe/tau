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

#ifndef TAU_PAINTER_WIN_HH
#define TAU_PAINTER_WIN_HH

#include "types-win.hh"
#include <painter-impl.hh>

namespace tau {

class Painter_win: public Painter_impl {
public:

    Painter_win();
    Painter_win(Winface_win * wf);
    Painter_win(Winface_win * wf, PAINTSTRUCT * pstruct);
   ~Painter_win();

   void begin_paint(Winface_win * wf, PAINTSTRUCT * pstruct);
   void end_paint();

    // Overrides pure Painter_impl.
    Vector text_size(const ustring & s) override;

    // Overrides pure Painter_impl.
    Vector text_size(const std::u32string & s) override;

protected:

    // Overrides pure Painter.
    void set_font(Font_ptr font) override;

    // Overrides pure Painter_impl.
    Font_ptr select_font(const ustring & font_spec) override;

    // Overrides pure Painter_impl.
    void update_clip() override;

    // Overrides pure Painter_impl.
    void stroke_rectangle(const tau::Rect & r) override;

    // Overrides pure Painter_impl.
    void stroke_polyline(const Point * pts, std::size_t npts) override;

    // Overrides pure Painter_impl.
    void fill_rectangles(const Rect * rs, std::size_t nrs, const Color & color) override;

    // Overrides pure Painter_impl.
    void fill_polygon(const Point * pts, std::size_t npts, const Color & color) override;

    // Overrides pure Painter_impl.
    void draw_pixmap(Pixmap_ptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) override;

    // Overrides Painter_impl.
    void stroke_prim_text(const Prim_text & o) override;

    // Overrides Painter_impl.
    void fill_prim_contour(const Prim_contour & o);

    // Overrides Painter_impl.
    void stroke_prim_arc(const Prim_arc & obj) override;

    // Overrides Painter_impl.
    void fill_prim_arc(const Prim_arc & obj) override;

    // Overrides Painter_impl.
    void stroke_prim_rect(const Prim_rect * po, std::size_t no) override;

    // Overrides Painter_impl.
    void fill_prim_rect(const Prim_rect * po, std::size_t no) override;

private:

    void  done_dc();
    DWORD rop(Oper op);
    void  on_destroy();
    void  set_clip();

private:

    HWND            hwnd_ = NULL;
    HDC             hdc_ = NULL;
    PAINTSTRUCT *   pstruct_ = nullptr;
    RECT            cr_ { 0, 0, 0, 0 };
    unsigned        dpi_ = 96;
    signal<void()>  signal_invalidate_;
};

} // namespace tau

#endif // TAU_PAINTER_WIN_HH
