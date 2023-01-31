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

#include <brush-impl.hh>
#include <glyph-impl.hh>
#include <pen-impl.hh>
#include "font-win.hh"
#include "display-win.hh"
#include "painter-win.hh"
#include "pixmap-win.hh"
#include "winface-win.hh"
#include <cmath>
#include <cstring>
#include <map>
#include <iostream>

namespace tau {

static const std::map<Oper, DWORD> rops_ = {
    { OPER_COPY,        SRCCOPY         },
    { OPER_CLEAR,       BLACKNESS       },
    { OPER_SET,         WHITENESS       }
};

Painter_win::Painter_win():
    Painter_impl()
{
    select_font_priv(Font::normal());
}

Painter_win::Painter_win(Winface_win * wf):
    Painter_impl(),
    hwnd_(wf->handle())
{
    hdc_ = GetDC(hwnd_);
    dpi_ = wf->wdp()->dpi();
    SetBkMode(hdc_, TRANSPARENT);
    select_font_priv(Font::normal());
    wstate().wclip.set(wf->self()->size());
    wf->self()->signal_destroy().connect(fun(this, &Painter_win::on_destroy));
    update_clip_priv();
}

Painter_win::Painter_win(Winface_win * wf, PAINTSTRUCT * pstruct):
    Painter_impl()
{
    dpi_ = wf->wdp()->dpi();
    select_font_priv(Font::normal());
    begin_paint(wf, pstruct);
}

Painter_win::~Painter_win() {
    done_dc();
}

void  Painter_win::done_dc() {
    if (hwnd_) {
        if (pstruct_) {
            EndPaint(hwnd_, pstruct_);
            pstruct_ = nullptr;
        }

        else if (hdc_) {
            ReleaseDC(hwnd_, hdc_);
        }

        hdc_ = NULL;
        hwnd_ = NULL;
    }
}

void Painter_win::begin_paint(Winface_win * wf, PAINTSTRUCT * pstruct) {
    done_dc();
    update_clip_priv();
    hdc_ = BeginPaint(wf->handle(), pstruct);

    if (hdc_) {
        pstruct_ = pstruct;
        hwnd_ = wf->handle();
        SetBkMode(hdc_, TRANSPARENT);
        wstate().wclip.set(wf->self()->size());
    }
}

void Painter_win::end_paint() {
    done_dc();
}

DWORD Painter_win::rop(Oper op) {
    auto iter = rops_.find(op);
    return iter != rops_.end() ? iter->second : SRCCOPY;
}

void Painter_win::set_font(Font_ptr font) {
    if (hdc_ && font) {
        if (auto fms = std::dynamic_pointer_cast<Font_win>(font)) {
            if (HFONT hfont = fms->handle()) {
                state().font_spec = font->spec();
                SelectFont(hdc_, hfont);
                state().font = font;
                return;
            }
        }

        select_font_priv(font->spec());
    }
}

Font_ptr Painter_win::select_font_priv(const ustring & font_spec) {
    if (font_spec != state().font_spec) {
        auto wfp = std::make_shared<Font_win>(hdc_, dpi_, font_spec);
        signal_invalidate_.connect(fun(wfp, &Font_win::invalidate));
        state().font_spec = font_spec;
        state().font = wfp;
        SelectFont(hdc_, wfp->handle());
    }

    return state().font;
}

// Overrides pure Painter_impl.
Vector Painter_win::text_size(const ustring & s) {
    if (Font_ptr f = font()) {
        HFONT hfont = std::static_pointer_cast<Font_win>(f)->handle();

        if (hfont) {
            HFONT old_font = SelectFont(hdc_, hfont);
            std::wstring ws = str_to_wstring(s);
            SIZE wsize;
            GetTextExtentPoint32W(hdc_, ws.c_str(), ws.size(), &wsize);
            SelectFont(hdc_, old_font);
            return Vector(wsize.cx, wsize.cy);
        }
    }

    return Vector();
}

// Overrides pure Painter_impl.
Vector Painter_win::text_size(const std::u32string & s) {
    if (Font_ptr f = font()) {
        HFONT hfont = std::static_pointer_cast<Font_win>(f)->handle();

        if (hfont) {
            HFONT old_font = SelectFont(hdc_, hfont);
            std::wstring ws = str32_to_wstring(s);
            SIZE wsize;
            GetTextExtentPoint32W(hdc_, ws.c_str(), ws.size(), &wsize);
            SelectFont(hdc_, old_font);
            return Vector(wsize.cx, wsize.cy);
        }
    }

    return Vector();
}

// protected
// Overrides Painter_impl.
void Painter_win::stroke_prim_text(const Prim_text & p) {
    if (!hdc_) { return; }
    Point pt(matrix()*p.pos);
    pt -= woffset();

    if (Font_ptr f = font()) {
        HFONT hfont = std::static_pointer_cast<Font_win>(f)->handle();

        if (hfont) {
            SelectFont(hdc_, hfont);
            SetTextColor(hdc_, p.color.bgr24());

            static const std::size_t bs = 512;
            wchar_t b[bs];
            std::size_t i, j = 0;

            for (i = 0; i < p.str.size() && j < bs-1; i++) {
                char32_t wc = p.str[i];

                if (wc >= 0x00110000 || (wc >= 0x0000d800 && wc < 0x0000e000)) {  // illegal sequence.
                    continue;
                }

                if (wc < 0x00010000) {
                    b[j++] = wc;
                }

                else {
                    b[j++] = (wc-0x00010000)/0x400+0xd800;
                    b[j++] = (wc-0x00010000)%0x400+0xdc00;
                }
            }

            TextOutW(hdc_, pt.x(), pt.y()-f->ascent(), b, j);
            //std::wstring ws = str_to_wstring(p.str);
            //TextOutW(hdc_, pt.x(), pt.y()-f->ascent(), ws.c_str(), ws.size());
        }
    }
}

void Painter_win::set_clip() {
    if (hdc_) {
        HRGN rgn = CreateRectRgn(cr_.left, cr_.top, cr_.right, cr_.bottom);
        SelectClipRgn(hdc_, rgn);
        DeleteObject(rgn);
    }
}

void Painter_win::update_clip_priv() {
    cr_ = to_winrect(wstate().wclip);
    set_clip();
}

void Painter_win::stroke_polyline(const Point * pts, std::size_t npts) {
    if (!hdc_ || npts < 2) { return; }
    double lw = state().pen->line_width;
    HPEN pen = CreatePen(PS_SOLID, lw > 0.0 ? lw : 1, state().pen->color.bgr24());
    SelectObject(hdc_, pen);
    SetROP2(hdc_, winrop(state().op));
    POINT xp = to_winpoint(*pts++);
    --npts;
    MoveToEx(hdc_, xp.x, xp.y, NULL);
    while (npts--) { xp = to_winpoint(*pts++); LineTo(hdc_, xp.x, xp.y); }
    DeleteObject(pen);
}

void Painter_win::stroke_prim_arc(const Prim_arc & obj) {
    if (!hdc_) { return; }

    if (state().mat.is_identity()) {
        double lw = state().pen->line_width;
        HPEN hpen = CreatePen(PS_SOLID, lw > 0.0 ? lw : 1, state().pen->color.bgr24());
        SelectObject(hdc_, hpen);
        SetROP2(hdc_, winrop(state().op));
        Point c(obj.center.x(), obj.center.y());
        c -= woffset();
        double r = obj.radius, a1 = obj.angle1, a2 = obj.angle2;
        SetArcDirection(hdc_, a2 < a1 ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE);
        Arc(hdc_, c.x()-r, c.y()-r, c.x()+r, c.y()+r,
            c.x()+r*std::cos(a1), c.y()+r*std::sin(a1),
            c.x()-r*std::cos(a2), c.y()-r*std::sin(a2));
        DeleteObject(hpen);
    }

    else {
        Painter_impl::stroke_prim_arc(obj);
    }
}

void Painter_win::fill_prim_arc(const Prim_arc & obj) {
    Painter_impl::fill_prim_arc(obj);
}

// protected
// Overrides Painter_impl.
void Painter_win::stroke_prim_rect(const Prim_rect * po, std::size_t no) {
    if (!hdc_) { return; }

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
            stroke_rectangle(r);
        }

        else {
            stroke_polyline(pts, 5);
        }

        ++po;
    }
}

// protected
// Overrides Painter_impl.
void Painter_win::fill_prim_rect(const Prim_rect * po, std::size_t no) {
    if (!hdc_) { return; }

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
            fill_polygon(pts, 5, state().brush->color);
        }

        ++po;
    }
}

void Painter_win::stroke_rectangle(const Rect & r) {
    if (!hdc_) { return; }

    double lw = state().pen->line_width;
    HPEN hpen = CreatePen(PS_SOLID, lw > 0.0 ? lw : 1, state().pen->color.bgr24());
    SelectObject(hdc_, hpen);
    SetROP2(hdc_, winrop(state().op));
    RECT wr = to_winrect(r);
    Rectangle(hdc_, wr.left, wr.top, wr.right, wr.bottom);
    DeleteObject(hpen);
}

void Painter_win::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    if (!hdc_) { return; }

    HBRUSH hbr = CreateSolidBrush(c.bgr24());
    SetROP2(hdc_, winrop(state().op));

    while (nrs--) {
        RECT rect = to_winrect(Rect(rs->origin(), rs->size()));

        if (OPER_COPY == state().op) {
            FillRect(hdc_, &rect, hbr);
        }

        else {
            HRGN rgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
            FillRgn(hdc_, rgn, hbr);
            DeleteObject(rgn);
        }

        ++rs;
    }

    DeleteObject(hbr);
}

// protected
void Painter_win::fill_polygon(const Point * pts, std::size_t npts, const Color & color) {
    if (!hdc_ || npts < 2) { return; }

    HBRUSH hbrush = CreateSolidBrush(color.bgr24());
    HPEN hpen = (HPEN)GetStockObject(NULL_PEN);
    SelectObject(hdc_, hbrush);
    SelectObject(hdc_, hpen);
    SetROP2(hdc_, winrop(state().op));
    POINT xps[npts];
    for (std::size_t n = 0; n < npts; ++n) { xps[n] = to_winpoint(pts[n]); }
    Polygon(hdc_, xps, npts);
    DeleteObject(hbrush);
    DeleteObject(hpen);
}

// protected
// Overrides Painter_impl.
void Painter_win::fill_prim_contour(const Prim_contour & o) {
    if (!hdc_) { return; }

    if (1 == o.ctrs.size()) {
        const Contour & ctr = o.ctrs.back();

        if (1 == ctr.order()) {
            std::size_t pos = 1, npts = 1+ctr.size();
            Point pts[npts];
            pts[0] = matrix()*ctr.start(); pts[0] -= woffset();
            for (const Curve & cv: ctr) { pts[pos] = matrix()*cv.end(); pts[pos++] -= woffset(); }
            if (const Rect r = is_rect(pts, npts)) { fill_rectangles(&r, 1, state().brush->color); }
            else { fill_polygon(pts, npts, state().brush->color); }
            return;
        }
    }

    Painter_impl::fill_prim_contour(o);
}

// protected
void Painter_win::draw_pixmap(Pixmap_cptr pix, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) {
    if (!hdc_) { return; }

    if (auto pix_ms = std::dynamic_pointer_cast<const Pixmap_win>(pix)) {
        if (HDC cdc = CreateCompatibleDC(hdc_)) {
            HBITMAP cbm = pix_ms->create_bitmap(hdc_);

            if (cbm) {
                SelectObject(cdc, cbm);

                if (transparent && 32 == pix_ms->depth()) {
                    BLENDFUNCTION bf { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                    AlphaBlend(hdc_, pt.x(), pt.y(), pix_size.width(), pix_size.height(), cdc, pix_origin.x(), pix_origin.y(), pix_size.width(), pix_size.height(), bf);
                }

                else {
                    BitBlt(hdc_, pt.x(), pt.y(), pix_size.width(), pix_size.height(), cdc, pix_origin.x(), pix_origin.y(), rop(state().op));
                }
            }

            // Delete HDC before HBITMAP to prevent GDI resource leak!
            DeleteDC(cdc);
            if (cbm) { DeleteObject(cbm); }
        }
    }
}

void Painter_win::on_destroy() {
    signal_invalidate_();
    state().font.reset();
    state().font_spec.clear();
    hdc_ = NULL;
    hwnd_ = NULL;
}

} // namespace tau

//END
