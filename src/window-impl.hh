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

#ifndef TAU_WINDOW_IMPL_HH
#define TAU_WINDOW_IMPL_HH

#include <tau/enums.hh>
#include <bin-impl.hh>

namespace tau {

class Winface: public trackable {
public:

    virtual ~Winface() {}

    virtual Display_ptr display() = 0;
    virtual Display_cptr display() const = 0;

    virtual void update() = 0;
    virtual void invalidate(const Rect & inval) = 0;
    virtual Painter_ptr painter() = 0;

    virtual void move(const Point & pt) = 0;
    virtual void move(const Rect & r) = 0;
    virtual void resize(const Size & size) = 0;

    virtual void minimize() = 0;
    virtual void maximize() = 0;
    virtual void restore() = 0;
    virtual void set_fullscreen(bool yes) = 0;
    virtual bool visible() const = 0;

    virtual void set_cursor(Cursor_ptr cursor) = 0;
    virtual void unset_cursor() = 0;
    virtual void show_cursor(bool show) = 0;
    virtual void set_title(const ustring & title) = 0;
    virtual void set_icon(Pixmap_ptr icon) = 0;

    virtual void enable_maximize(bool enable) = 0;
    virtual bool maximize_enabled() const = 0;

    virtual void enable_minimize(bool enable) = 0;
    virtual bool minimize_enabled() const = 0;

    virtual void show_frame(bool show) = 0;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Window_impl: public Bin_impl {
public:

    Winface_ptr winface() { return winface_; }
    Winface_cptr winface() const { return winface_; }

    // Overrides Widget_impl.
    Display_ptr display() override { return winface_->display(); }

    // Overrides Widget_impl.
    Display_cptr display() const override { return winface_->display(); }

    // Overrides Widget_impl.
    bool cursor_visible() const override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    bool grab_modal_up(Widget_impl * caller) override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    bool end_modal_up(Widget_impl * caller) override;

    // Overrides Widget_impl.
    bool has_modal() const override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    int  grab_focus_up(Widget_impl * caller) override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    void drop_focus_up(Widget_impl * caller) override;

    // Overrides Widget_impl.
    bool has_window() const override;

    // Overrides Widget_impl.
    void set_cursor_up(Cursor_ptr cursor) override;

    // Overrides Widget_impl.
    void unset_cursor_up() override;

    // Overrides Container_impl.
    // Overrides Window_impl.
    void show_cursor_up() override;

    // Overrides Widget_impl.
    void hide_cursor_up() override;

    // Overrides Widget_impl.
    Window_impl * window() override { return this; }

    // Overrides Widget_impl.
    const Window_impl * window() const override { return this; }

    // Overrides Widget_impl.
    Point to_screen(const Point & pt=Point()) const override;

    // Overrides Widget_impl.
    Point to_window(const Point & pt=Point()) const override;

    // Overrides Widget_impl.
    // Overriden by Toplevel_impl.
    // Overriden by Popup_impl.
    Point to_root(const Point & pt=Point()) const override;

    // Overrides Widget_impl.
    Point where_mouse() const override;

    // Overrides Widget_impl.
    bool hover() const override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    bool grab_mouse_up(Widget_impl * wi) override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    bool ungrab_mouse_up(Widget_impl * caller) override;

    // Overrides Container_impl.
    // Overrides Widget_impl.
    bool grabs_mouse() const override;

    // Overrides Widget_impl.
    void invalidate(const Rect & inval=Rect()) override;

    // Overrides Widget_impl.
    Painter painter() override;

    // Overriden by Dialog_impl.
    virtual void close();

    void move(const Point & pt);
    void move(int x, int y);
    void move(const Rect & r);
    void move(int x, int y, unsigned width, unsigned height);
    void resize(const Size & sz);
    void resize(unsigned width, unsigned height);
    void update();

    /// @return Pointer to the created tooltip window.
    Window_ptr open_tooltip(Widget_impl * caller, Widget_ptr tooltip);
    Window_ptr open_tooltip(Widget_impl * caller, Widget_ptr tooltip, const Point & pt, Gravity gravity, unsigned time_ms);
    void close_tooltip(Widget_impl * caller);

    bool update_position(const Point & position);
    Point position() const { return position_; }

    void handle_client_area(const Rect & r);
    Rect client_area() const { return client_area_; }

    Window_ptr parent_window() { return wpp_; }
    Window_cptr parent_window() const { return wpp_; }

    signal<void()> & signal_close() { return signal_close_; }
    signal<void()> & signal_position_changed() { return signal_position_changed_; }

protected:

    Winface_ptr         winface_;
    Point               position_;              // Position within the screen.
    Rect                client_area_;
    Window_ptr          wpp_;                   // An optional parent window.

    signal<void()>      signal_close_;
    signal<void()>      signal_position_changed_;

protected:

    Window_impl();
   ~Window_impl() { signal_destroy_(); }

private:

    connection          tooltip_cx_;            // Tooltip signal_alarm connection.
    Window_ptr          tooltip_;               // Tooltip window.
    Widget_impl *       tooltip_widget_ = nullptr; // Widget called show_tooltip.

private:

    void on_tooltip_close();
    void on_tooltip_mouse_motion(int mm, const Point & pt);
    bool on_tooltip_mouse_down(int mbt, int mm, const Point & pt);
    bool on_tooltip_mouse_wheel(int d, int mm, const Point & pt);
};

} // namespace tau

#endif // TAU_WINDOW_IMPL_HH
