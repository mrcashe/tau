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

#ifndef TAU_DISPLAY_IMPL_HH
#define TAU_DISPLAY_IMPL_HH

#include <tau/enums.hh>
#include <tau/display.hh>
#include <tau/geometry.hh>
#include <tau/style.hh>
#include <tau/timer.hh>
#include <tau/timeval.hh>
#include <timer-impl.hh>
#include <window-impl.hh>
#include <thread>

namespace tau {

class Display_impl: public trackable {
public:

    Display_impl(const Display_impl & other) = delete;
    Display_impl(Display_impl && other) = delete;
    Display_impl & operator=(const Display_impl & other) = delete;
    Display_impl & operator=(Display_impl && other) = delete;
    virtual ~Display_impl() {}

    static Display_ptr this_display();
    static Display_ptr open(const ustring & args);
    static Display wrap(Display_ptr dp) { return Display(dp); }

    int id() const { return dpid_; }
    Size size_px() const;
    Size size_mm() const;
    unsigned dpi() const;
    bool screensaver_allowed() const { return 0 == screensaver_counter_; }

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual int depth() const = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual bool can_paste_text() const = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual void paste_text() = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual void copy_text(const ustring & s) = 0;

    // Overriden by Display_xcb.
    virtual void allow_screensaver();

    // Overriden by Display_xcb.
    virtual void disallow_screensaver();

    // Overriden by Display_freebsd.
    // Overriden by Display_linux.
    // Overriden by Display_win.
    virtual Loop_ptr loop() = 0;

    // Overriden by Display_freebsd.
    // Overriden by Display_linux.
    // Overriden by Display_win.
    virtual Loop_cptr loop() const = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual void grab_mouse(Window_impl * wii) = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual void ungrab_mouse() = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual bool grab_modal(Window_impl * wii) = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual bool end_modal(Window_impl * wii) = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual void grab_window_focus(Window_impl * wii) = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual Point where_mouse() const = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual Rect cursor_area() const = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual Toplevel_ptr create_toplevel(Display_ptr dp, const Rect & bounds) = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual Dialog_ptr create_dialog(Window_impl * tpl, const Rect & bounds=Rect()) = 0;

    // Overriden by Display_xcb.
    // Overriden by Display_win.
    virtual Popup_ptr create_popup(Display_ptr dp, Widget_impl * wpp, const Point & origin, Gravity gravity) = 0;

    Window_ptr winptr(Widget_impl * wi);
    Window_cptr winptr(const Widget_impl * wi) const;
    const Window_impl * modal_window() const { return modal_window_; }
    const Window_impl * focused_window() const { return focused_; }
    const Window_impl * mouse_grabber() const { return mouse_grabber_; }
    const Window_impl * mouse_owner() const { return mouse_owner_; }

    signal<void()> & signal_can_paste() { return signal_can_paste_; }
    signal<void(ustring)> & signal_paste_text() { return signal_paste_text_; }

protected:

    Size                    size_px_;
    Size                    size_mm_;
    unsigned                dpi_ = 96;
    unsigned                screensaver_counter_ = 0;
    std::thread::id         tid_;
    int                     dpid_ = -1;

    Window_impl *           mouse_grabber_ = nullptr;
    Window_impl *           mouse_owner_ = nullptr;
    Window_impl *           modal_window_ = nullptr;
    Window_impl *           focused_ = nullptr;

    signal<void()>          signal_can_paste_;
    signal<void(ustring)>   signal_paste_text_;

protected:

    Display_impl() = default;

    // Overriden by Display_freebsd.
    // Overriden by Display_linux.
    // Overriden by Display_win.
    virtual void done() = 0;

    void add_window(Window_ptr wip);
    void remove_window(Window_impl * wii);
    Window_impl * set_mouse_owner(Window_impl * wii, const Point & pt);
    void reset_mouse_owner();
    void focus_window(Window_impl * wii);

    /// @return true if focus was removed.
    bool unfocus_window(Window_impl * wii);

private:

    using Windows = std::list<Window_ptr>;

    Windows     windows_;
    Windows     woff_;
    Timer       woff_timer_ { fun(this, &Display_impl::on_woff_timer) };
    Timeval     woff_tv_;

private:

    void on_woff_timer();
    void on_window_sensitivity(Window_impl * wii);
};

} // namespace tau

#endif // TAU_DISPLAY_IMPL_HH
