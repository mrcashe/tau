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

#ifndef TAU_WINFACE_WIN_HH
#define TAU_WINFACE_WIN_HH

#include "types-win.hh"
#include <dialog-impl.hh>
#include <popup-impl.hh>
#include "display-win.hh"

namespace tau {

class Winface_win: public Winface {
public:

    Winface_win(Display_win_ptr dp, HWND hwnd): dp_(dp), hwnd_(hwnd) {}
   ~Winface_win() { CloseHandle(hwnd_); }

    // Overrides pure Winface.
    Display_ptr display() override;

    // Overrides pure Winface.
    Display_cptr display() const override;

   // Overrides pure Winface.
   void update() override;

    // Overrides pure Winface.
    void invalidate(const Rect & inval) override;

    // Overrides pure Winface.
    Painter_ptr painter() override;

    // Overrides pure Winface.
    void move(const Point & pt) override;

    // Overrides pure Winface.
    void move(const Rect & r) override;

    // Overrides pure Winface.
    void resize(const Size & size) override;

    // Overrides pure Winface.
    void minimize() override;

    // Overrides pure Winface.
    void maximize() override;

    // Overrides pure Winface.
    void restore() override;

    // Overrides pure Winface.
    void set_fullscreen(bool yes) override;

    // Overrides pure Winface.
    void set_cursor(Cursor_ptr cursor) override;

    // Overrides pure Winface.
    void unset_cursor() override;

    // Overrides pure Winface.
    void show_cursor(bool show) override;

    // Overrides pure Winface.
    void set_title(const ustring & title) override;

    // Overrides pure Winface.
    void set_icon(Pixmap_ptr icon) override;

    // Overrides pure Winface.
    void show_frame(bool show) override;

    // Overrides pure Winface.
    void enable_maximize(bool enable) override;

    // Overrides pure Winface.
    bool maximize_enabled() const override;

    // Overrides pure Winface.
    void enable_minimize(bool enable) override;

    // Overrides pure Winface.
    bool minimize_enabled() const override;

    // Overrides pure Winface.
    bool visible() const override { return visible_; }

    Display_win_ptr wdp() { return dp_; }
    Display_win_cptr wdp() const { return dp_; }

    void init(Window_impl * wii);

    Window_impl * self() { return self_; }
    const Window_impl * self() const { return self_; }
    HWND handle() const { return hwnd_; }

    void handle_maximize();
    void handle_minimize();
    void handle_restore();
    void handle_visibility(bool visible);

    void track_mouse_event();
    void untrack_mouse_event();

private:

    Display_win_ptr     dp_;
    HWND                hwnd_;
    RECT                keep_rect_;
    unsigned            keep_style_ = 0;
    bool                tme_ = false;  // TrackMouseEvent() called.
    bool                display_handled_ = false;
    bool                want_fullscreen_ = false;
    bool                want_maximize_ = false;
    bool                want_minimize_ = false;
    bool                visible_ = false;
    Window_impl *       self_ = nullptr;
    Toplevel_impl *     tpl_ = nullptr;
    Popup_impl *        popup_ = nullptr;
    HCURSOR             icursor_ = NULL;    // hCursor from class.

private:

    void on_show();
    void on_toplevel_show();
    void on_hide();
    void change_style_bits(LONG set, LONG reset);
};

#define WINFACE_WIN(wip) std::static_pointer_cast<Winface_win>(wip->winface())

} // namespace tau

#endif // TAU_WINFACE_WIN_HH
