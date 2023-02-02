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
#include <theme-impl.hh>
#include "cursor-win.hh"
#include "display-win.hh"
#include "painter-win.hh"
#include "pixmap-win.hh"
#include "window-win.hh"
#include <iostream>

namespace tau {

void Winface_win::init(Window_impl * wii) {
    self_ = wii;
    tpl_ = dynamic_cast<Toplevel_impl *>(wii);
    popup_ = dynamic_cast<Popup_impl *>(wii);
    self_->signal_hide().connect(fun(this, &Winface_win::on_hide));
    if (tpl_) { tpl_->signal_show().connect(fun(this, &Winface_win::on_toplevel_show)); }
    else { self_->signal_show().connect(fun(this, &Winface_win::on_show)); }
    int cursor_size = std::max(GetSystemMetrics(SM_CXCURSOR), GetSystemMetrics(SM_CYCURSOR));

    if (auto cursor = Theme_impl::root()->find_cursor("arrow:left_ptr:default", cursor_size)) {
        if (auto wcursor = std::dynamic_pointer_cast<Cursor_win>(cursor)) {
            icursor_ = wcursor->handle();
        }
    }

    if (!icursor_) { icursor_ = LoadCursor(NULL, IDC_ARROW); }
    SetCursor(icursor_);
}

// Overrides pure Winface.
Display_ptr Winface_win::display() {
    return dp_->loop()->alive() ? dp_ : nullptr;
}

// Overrides pure Winface.
Display_cptr Winface_win::display() const {
    return dp_->loop()->alive() ? dp_ : nullptr;
}

void Winface_win::track_mouse_event() {
    if (!tme_) {
        tme_ = true;
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd_;
        tme.dwHoverTime = 100;
        TrackMouseEvent(&tme);
    }
}

void Winface_win::untrack_mouse_event() {
    tme_ = false;
}

void Winface_win::on_toplevel_show() {
    if (!visible_) {
        DWORD show = SW_SHOWNORMAL;

        if (want_fullscreen_) {
            ShowWindow(hwnd_, SW_SHOW);
            set_fullscreen(true);
            return;
        }

        else if (want_minimize_) {
            show = SW_MINIMIZE;
        }

        else if (want_maximize_) {
            show = SW_MAXIMIZE;
        }

        ShowWindow(hwnd_, show);
    }
}

void Winface_win::on_show() {
    ShowWindow(hwnd_, SW_SHOWNORMAL);
}

// Overrides pure Winface.
void Winface_win::on_hide() {
    ShowWindow(hwnd_, SW_HIDE);
}

// Overrides pure Winface.
void Winface_win::update() {
    UpdateWindow(hwnd_);
}

// Overrides pure Winface.
void Winface_win::invalidate(const Rect & r) {
    RECT wr = to_winrect(r);
    InvalidateRect(hwnd_, &wr, FALSE);
}

// Overrides pure Winface.
Painter_ptr Winface_win::painter() {
    return std::make_shared<Painter_win>(this);
}

// Overrides pure Winface.
void Winface_win::move(const Point & pt) {
    SetWindowPos(hwnd_, 0, pt.x(), pt.y(), 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

// Overrides pure Winface.
void Winface_win::move(const Rect & r) {
    SetWindowPos(hwnd_, 0, r.left(), r.top(), r.width(), r.height(), SWP_NOZORDER);
}

// Overrides pure Winface.
void Winface_win::resize(const Size & size) {
    unsigned w = size.width(), h = size.height();

    if (!popup_) {
        RECT wr, cr;
        GetWindowRect(hwnd_, &wr);
        GetClientRect(hwnd_, &cr);
        w += (wr.right-wr.left)-(cr.right-cr.left);
        h += (wr.bottom-wr.top)-(cr.bottom-cr.top);
    }

    SetWindowPos(hwnd_, 0, 0, 0, w, h, SWP_NOZORDER|SWP_NOMOVE);
}

// Overrides pure Winface.
void Winface_win::minimize() {
    want_fullscreen_ = false;
    want_minimize_ = true;
    want_maximize_ = false;
    ShowWindow(hwnd_, SW_MINIMIZE);
}

// Overrides pure Winface.
void Winface_win::maximize() {
    want_fullscreen_ = false;
    want_minimize_ = false;
    want_maximize_ = true;
    ShowWindow(hwnd_, SW_MAXIMIZE);
}

// Overrides pure Winface.
void Winface_win::restore() {
    want_fullscreen_ = false;
    want_minimize_ = false;
    want_maximize_ = false;
    ShowWindow(hwnd_, SW_RESTORE);
}

// Overrides pure Winface.
void Winface_win::set_fullscreen(bool yes) {
    want_fullscreen_ = yes;
    want_minimize_ = false;
    want_maximize_ = false;

    if (tpl_) {
        if (!tpl_->hidden()) {
            if (yes && !tpl_->full_screened()) {
                if (GetWindowRect(hwnd_, &keep_rect_)) {
                    MONITORINFO mi;
                    HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
                    mi.cbSize = sizeof(mi);
                    int x, y, width, height;

                    if (monitor && GetMonitorInfo(monitor, &mi)) {
                        x = mi.rcMonitor.left;
                        y = mi.rcMonitor.top;
                        width = mi.rcMonitor.right-x;
                        height = mi.rcMonitor.bottom-y;
                    }

                    else {
                        x = y = 0;
                        width = GetSystemMetrics(SM_CXSCREEN);
                        height = GetSystemMetrics(SM_CYSCREEN);
                    }

                    keep_style_ = GetWindowLong(hwnd_, GWL_STYLE);
                    SetWindowLong(hwnd_, GWL_STYLE, (keep_style_ & ~WS_OVERLAPPEDWINDOW) | WS_POPUP);
                    SetWindowPos(hwnd_, HWND_TOP, x, y, width, height, SWP_NOCOPYBITS|SWP_SHOWWINDOW);
                    tpl_->handle_fullscreen(true);
                }
            }

            else if (!yes && tpl_->full_screened()) {
                SetWindowLong(hwnd_, GWL_STYLE, keep_style_);
                SetWindowPos(hwnd_, HWND_NOTOPMOST, keep_rect_.left, keep_rect_.top, keep_rect_.right-keep_rect_.left, keep_rect_.bottom-keep_rect_.top, SWP_NOCOPYBITS|SWP_SHOWWINDOW);
                tpl_->handle_fullscreen(false);
            }
        }
    }
}

// Overrides pure Winface.
void Winface_win::set_cursor(Cursor_ptr cursor) {
    if (auto wcursor = std::dynamic_pointer_cast<Cursor_win>(cursor)) {
        HCURSOR hc = wcursor->handle();
        if (hc) { SetCursor(hc); }
    }
}

// Overrides pure Winface.
void Winface_win::unset_cursor() {
    HCURSOR hcursor = icursor_;
    if (!hcursor) { hcursor = LoadCursor(NULL, IDC_ARROW); }
    if (hcursor) { SetCursor(hcursor); }
}

// Overrides pure Winface.
void Winface_win::show_cursor(bool show) {
    ShowCursor(show);
}

// Overrides pure Winface.
void Winface_win::set_title(const ustring & title) {
    SetWindowTextW(hwnd_, str_to_wstring(title).c_str());
}

// Overrides pure Winface.
void Winface_win::set_icon(Pixmap_ptr pix) {
    if (auto pix_ms = std::dynamic_pointer_cast<Pixmap_win>(pix)) {
        Size sz = pix_ms->size();

        if (sz) {
            auto mask = new Pixmap_win(1, sz);

            for (std::size_t y = 0; y < sz.height(); ++y) {
                for (std::size_t x = 0; x < sz.width(); ++x) {
                    Color c = pix_ms->get_pixel(Point(x, y));
                    mask->put_pixel(x, y, c);
                }
            }

            if (HDC dc = GetDC(NULL)) {
                if (HBITMAP hpix = pix_ms->create_bitmap(dc)) {
                    if (HBITMAP hmask = mask->create_bitmap(dc)) {
                        ICONINFO ii;
                        ii.fIcon = true;
                        ii.xHotspot = 0;
                        ii.yHotspot = 0;
                        ii.hbmColor = hpix;
                        ii.hbmMask = hmask;

                        if (HICON ico = CreateIconIndirect(&ii)) {
                            SendMessageW(hwnd_, WM_SETICON, ICON_BIG, LPARAM(ico));
                            SendMessageW(hwnd_, WM_SETICON, ICON_SMALL, LPARAM(ico));
                            DestroyIcon(ico);
                        }

                        DeleteObject(hmask);
                    }

                    DeleteObject(hpix);
                }

                ReleaseDC(NULL, dc);
            }

            delete mask;
        }
    }
}

void Winface_win::change_style_bits(LONG set, LONG reset) {
    LONG ls = GetWindowLong(hwnd_, GWL_STYLE);
    ls &= ~reset; ls |= set;
    SetWindowLong(hwnd_, GWL_STYLE, ls);
    SetWindowPos(hwnd_, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER);
}

// Overrides pure Winface.
void Winface_win::show_frame(bool show) {
    LONG bits = WS_CAPTION|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU;
    change_style_bits(show ? bits : 0, show ? 0 : bits);
}

void Winface_win::handle_maximize() {
    want_maximize_ = false;
    if (tpl_) { tpl_->handle_maximize(); }
}

void Winface_win::handle_minimize() {
    want_minimize_ = false;
    if (tpl_) { tpl_->handle_minimize(); }
}

void Winface_win::handle_restore() {
    want_minimize_ = false;
    want_maximize_ = false;
    if (tpl_) { tpl_->handle_restore(); }
}

void Winface_win::handle_visibility(bool visible) {
    if (visible_ != visible) {
        visible_ = visible;
        self_->on_owner_show(visible);
    }
}

// Overrides pure Winface.
void Winface_win::enable_maximize(bool enable) {
    LONG bits = WS_MAXIMIZEBOX|WS_SYSMENU;
    change_style_bits(enable ? bits : 0, enable ? 0 : bits);
}

// Overrides pure Winface.
bool Winface_win::maximize_enabled() const {
    return WS_MAXIMIZEBOX & GetWindowLong(hwnd_, GWL_STYLE);
}

// Overrides pure Winface.
void Winface_win::enable_minimize(bool enable) {
    LONG bits = WS_MINIMIZEBOX|WS_SYSMENU;
    change_style_bits(enable ? bits : 0, enable ? 0 : bits);
}

// Overrides pure Winface.
bool Winface_win::minimize_enabled() const {
    return WS_MINIMIZEBOX & GetWindowLong(hwnd_, GWL_STYLE);
}

} // namespace tau

//END
