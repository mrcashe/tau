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

#ifndef TAU_WIDGET_IMPL_HH
#define TAU_WIDGET_IMPL_HH

#include <types-impl.hh>
#include <tau/enums.hh>
#include <tau/geometry.hh>
#include <tau/style.hh>
#include <tau/timer.hh>
#include <tau/ustring.hh>
#include <climits>

namespace tau {

// The base class of whole graphics object hierarchy.
// Almost every descendant derives this class to extend it's capabilities.
class Widget_impl: public trackable {
    friend class Widget;

public:

    Widget_impl();
    Widget_impl(const Widget_impl & other) = delete;
    Widget_impl & operator=(const Widget_impl & other) = delete;
    Widget_impl(Widget_impl && other) = delete;
    Widget_impl & operator=(Widget_impl && other) = delete;

    virtual ~Widget_impl();

    // Overriden by Window_impl.
    virtual bool cursor_visible() const;

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual bool grab_modal_up(Widget_impl * caller);

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual bool end_modal_up(Widget_impl * caller);

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual int  grab_focus_up(Widget_impl * caller);

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual void drop_focus_up(Widget_impl * caller);

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual bool grab_mouse_up(Widget_impl * wi);

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual bool ungrab_mouse_up(Widget_impl * caller);

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual bool grabs_mouse() const;

    // Overriden by Container_impl.
    virtual bool handle_accel(char32_t kc, int km);

    // Overriden by Container_impl.
    virtual bool handle_key_down(char32_t kc, int km);

    // Overriden by Container_impl.
    virtual bool handle_key_up(char32_t kc, int km);

    // Overriden by Container_impl.
    virtual bool handle_input(const ustring & s);

    // Overriden by Container_impl.
    virtual bool handle_mouse_down(int mbt, int mm, const Point & pt);

    // Overriden by Container_impl.
    virtual bool handle_mouse_up(int mbt, int mm, const Point & pt);

    // Overriden by Container_impl.
    virtual bool handle_mouse_double_click(int mbt, int mm, const Point & pt);

    virtual void handle_mouse_motion(int mm, const Point & pt);
    virtual void handle_mouse_enter(const Point & pt);
    virtual void handle_mouse_leave();
    virtual bool handle_mouse_wheel(int delta, int mm, const Point & pt);

    // Overriden by Container_impl.
    virtual void handle_paint(Painter pr, const Rect & inval);

    // Overriden by Container_impl.
    virtual void handle_backpaint(Painter pr, const Rect & inval);

    // Overriden by Container_impl.
    virtual void handle_display();

    // Overriden by Container_impl.
    virtual void handle_parent();

    // Overriden by Container_impl.
    virtual void handle_unparent();

    // Overriden by Window_impl.
    virtual bool has_modal() const;

    // Overriden by Window_impl.
    virtual bool has_window() const;

    // Overriden by Window_impl.
    virtual bool hover() const;

    // Overriden by Window_impl.
    virtual void invalidate(const Rect & r=Rect());

    // Overriden by Window_impl.
    virtual Painter painter();

    // Overriden by Dialog_impl.
    virtual void quit_dialog();

    // Overriden by Window_impl.
    // Overriden by Container_impl.
    virtual void set_cursor_up(Cursor_ptr cursor);

    // Overriden by Popup_impl.
    // Overriden by Toplevel_impl.
    virtual void set_parent(Container_impl * parent);

    // Overriden by Window_impl.
    // Overriden by Container_impl.
    virtual void unset_cursor_up();

    // Overriden by Container_impl.
    // Overriden by Window_impl.
    virtual void show_cursor_up();

    // Overriden by Window_impl.
    virtual void hide_cursor_up();

    // Overriden by Container_impl.
    virtual void resume_focus();

    // Overriden by Container_impl.
    virtual void suspend_focus();

    // Overriden by Container_impl.
    virtual void clear_focus();

    // Overriden by Window_impl.
    virtual Point to_screen(const Point & pt=Point()) const;

    // Overriden by Window_impl.
    virtual Point to_window(const Point & pt=Point()) const;

    // Overriden by Window_impl.
    // Overriden by Toplevel_impl.
    // Overriden by Popup_impl.
    virtual Point to_root(const Point & pt=Point()) const;

    // Overriden by Window_impl.
    virtual Point where_mouse() const;

    // Overriden by Window_impl.
    virtual Window_impl * window();

    // Overriden by Window_impl.
    virtual const Window_impl * window() const;

    // Overriden by Toplevel_impl.
    // Overriden by Popup_impl.
    virtual Window_impl * root();

    // Overriden by Toplevel_impl.
    // Overriden by Popup_impl.
    virtual const Window_impl * root() const;

    // Overriden by Window_impl.
    virtual Display_ptr display();

    // Overriden by Window_impl.
    virtual Display_cptr display() const;

    // Overriden by Container_impl.
    // Overriden by Dialog_impl.
    virtual bool running() const { return !signal_run_.empty(); }

    // Overriden by Container_impl.
    virtual void shutdown(bool yes);

    // Overriden by Container_impl.
    virtual void update_pdata();

    // Overriden by Container_impl.
    virtual Action_base * lookup_action(char32_t kc, int km);

    connection connect_accel(Accel & accel, bool prepend=false);
    void connect_action(Action_base & action, bool prepend=false);

    void  enable();
    void  disable();
    bool  disabled() const;

    void  hide();
    void  show();
    bool  hidden() const;
    bool  visible() const;
    bool  obscured() const { return !pclip_.empty(); }

    bool  grab_modal();
    void  end_modal();
    void  allow_focus();
    void  disallow_focus();
    bool  grab_focus();
    void  drop_focus();
    bool  focusable() const;
    bool  focused() const { return focused_; }
    bool  take_focus();
    bool  grab_mouse();
    bool  ungrab_mouse();
    bool  enabled() const { return enabled_ && !disabled(); }

    Rect  exposed_area() const;
    bool  has_parent() const;
    bool  scrollable() const;

    bool  hint_size(const Size & sz);
    bool  hint_size(unsigned width, unsigned height);
    Size  size_hint() const { return size_hint_; }

    bool  hint_min_size(const Size & sz);
    bool  hint_min_size(unsigned width, unsigned height);
    Size  min_size_hint() const { return min_size_hint_; }

    bool  hint_max_size(const Size & sz);
    bool  hint_max_size(unsigned width, unsigned height);
    Size  max_size_hint() const { return max_size_hint_; }

    bool  hint_margin_left(unsigned left);
    bool  hint_margin_right(unsigned right);
    bool  hint_margin_top(unsigned top);
    bool  hint_margin_bottom(unsigned bottom);
    bool  hint_margin(unsigned left, unsigned right, unsigned top, unsigned bottom);
    bool  hint_margin(unsigned all);
    Size  margin_hint() const;
    Point margin_origin() const;
    unsigned margin_left_hint() const { return margin_left_hint_; }
    unsigned margin_right_hint() const { return margin_right_hint_; }
    unsigned margin_top_hint() const { return margin_top_hint_; }
    unsigned margin_bottom_hint() const { return margin_bottom_hint_; }

    Point origin() const { return origin_; }
    bool  update_origin(const Point & pt);
    bool  update_origin(int x, int y) { return update_origin(Point(x, y)); }

    Size  size() const { return size_; }
    bool  update_size(const Size & sz);
    bool  update_size(unsigned width, unsigned height) { return update_size(Size(width, height)); }
    Size  required_size() const { return required_size_; }

protected:

    bool require_size(const Size & size);
    bool require_size(unsigned width, unsigned height);

public:

    Container_impl * parent() { return parent_; }
    const Container_impl * parent() const { return parent_; }
    void  unparent();

    void handle_visible(bool show);
    void handle_enable(bool yes);

    void   scroll_to(const Point & pt);
    void   scroll_to(int x, int y);
    void   scroll_to_x(int x);
    void   scroll_to_y(int y);
    Point  scroll_position() const;

    Point  worigin() const { return worigin_; }
    Point  poffset() const { return poffset_; }
    Rect   pclip() const { return pclip_; }

    bool   has_tooltip() const;
    void   hide_tooltip();
    void   set_tooltip(const ustring & tooltip_text);
    void   set_tooltip(Widget_ptr tooltip_widget);
    void   show_tooltip(const ustring & tooltip_text);
    void   show_tooltip(Widget_ptr tooltip_widget);
    void   show_tooltip(const ustring & tooltip_text, const Point & pt, Gravity gravity, unsigned time_ms=0);
    void   show_tooltip(Widget_ptr tooltip_widget, const Point & pt, Gravity gravity, unsigned time_ms=0);
    void   unset_tooltip();

    void        set_cursor(Cursor_ptr cursor);
    void        set_cursor(const ustring & name, unsigned size=0);
    void        unset_cursor();
    void        show_cursor();
    void        hide_cursor();
    Cursor_ptr  cursor() { return cursor_; }
    bool        cursor_hidden() const;

    Style & style() { return style_; }
    const Style & style() const { return style_; }

    Point to_parent(const Point & pt=Point()) const;
    Point to_parent(const Container_impl * ci, const Point & pt=Point()) const;

    Rect visible_area() const;

    static Painter wrap_painter(Painter_ptr pp);
    static Painter_ptr strip(Painter pr);

    signal<bool(Painter, Rect)> & signal_paint() { return signal_paint_; }
    signal<bool(Painter, Rect)> & signal_backpaint() { return signal_backpaint_; }
    signal<bool(char32_t, int)> & signal_key_down();
    signal<bool(char32_t, int)> & signal_key_up();
    signal<bool(const ustring &)> & signal_input();
    signal<bool(char32_t, int)> & signal_accel();
    signal<bool(int, int, Point)> & signal_mouse_down();
    signal<bool(int, int, Point)> & signal_mouse_double_click();
    signal<bool(int, int, Point)> & signal_mouse_up();
    signal<void(int, Point)> & signal_mouse_motion();
    signal<void(Point)> & signal_mouse_enter();
    signal<void()> & signal_mouse_leave();
    signal<bool(int, int, Point)> & signal_mouse_wheel();
    signal<void()> & signal_origin_changed() { return signal_origin_changed_; }
    signal<void()> & signal_size_changed() { return signal_size_changed_; }
    signal<void()> & signal_scroll_changed();
    signal<void()> & signal_hints_changed() { return signal_hints_changed_; }
    signal<void()> & signal_requisition_changed() { return signal_requisition_changed_; }
    signal<void()> & signal_enable() { return signal_enable_; }
    signal<void()> & signal_disable() { return signal_disable_; }
    signal<void()> & signal_visible() { return signal_visible_; }
    signal<void()> & signal_invisible() { return signal_invisible_; }
    signal<void()> & signal_focus_in() { return signal_focus_in_; }
    signal<void()> & signal_focus_out() { return signal_focus_out_; }
    signal<void()> & signal_select() { return signal_select_; }
    signal<void()> & signal_unselect() { return signal_unselect_; }
    signal<void()> & signal_show() { return signal_show_; }
    signal<void()> & signal_hide() { return signal_hide_; }
    signal<void()> & signal_parent();
    signal<void()> & signal_display() { return signal_display_; }
    signal<void()> & signal_unparent() { return signal_unparent_; }
    signal<void()> & signal_destroy() { return signal_destroy_; }
    signal<bool()> & signal_take_focus() { return signal_take_focus_; }

protected:

    Container_impl *    parent_ = nullptr;
    Scroller_impl *     scroller_ = nullptr;
    Cursor_ptr          cursor_;
    bool                cursor_hidden_ = false;
    bool                focus_allowed_ = false;
    bool                shut_ = false;
    bool                hidden_ = false;
    Style               style_;

    signal<bool(Painter, Rect)> signal_paint_;
    signal<bool(Painter, Rect)> signal_backpaint_;
    signal<void()> signal_size_changed_;
    signal<void()> signal_origin_changed_;
    signal<void()> signal_hints_changed_;
    signal<void()> signal_requisition_changed_;
    signal<void()> signal_enable_;
    signal<void()> signal_disable_;
    signal<void()> signal_visible_;
    signal<void()> signal_invisible_;
    signal<void()> signal_focus_in_;
    signal<void()> signal_focus_out_;
    signal<void()> signal_select_;
    signal<void()> signal_unselect_;
    signal<void()> signal_show_;
    signal<void()> signal_hide_;
    signal<void()> signal_display_;
    signal<void()> signal_unparent_;
    signal<void()> signal_destroy_;
    signal<bool()> signal_take_focus_;
    signal<void()> signal_pdata_changed_;
    signal<void()> signal_run_;

protected:

    void freeze();
    void thaw();
    void disappear();
    void appear();

private:

    bool        block_tooltip_ = false;
    bool        tooltip_exposed_ = false;
    ustring     tooltip_text_;
    Widget_ptr  tooltip_widget_;
    unsigned    tooltip_delay_ = 611;
    connection  tooltip_cx_ { true };

    ustring     cursor_name_;
    unsigned    cursor_size_ = 0;

    Point       origin_;
    Point       worigin_ { INT_MIN, INT_MIN }; // Origin within Window.
    Point       poffset_;       // Is for Painting Offset.
    Rect        pclip_;         // Is for Painting Clip.
    Size        size_;
    Size        size_hint_;
    Size        min_size_hint_;
    Size        max_size_hint_;
    Size        required_size_;

    bool        focused_ = false;
    bool        upshow_ = false;
    bool        enabled_ = false;
    bool        disappeared_ = false;
    bool        disabled_ = false;
    bool        frozen_ = false;

    unsigned    margin_left_hint_ = 0;
    unsigned    margin_right_hint_ = 0;
    unsigned    margin_top_hint_ = 0;
    unsigned    margin_bottom_hint_ = 0;

    connection  pan_cx_;

    connection  cursor_theme_cx_;
    signal<void()> * signal_scroll_changed_ = nullptr;
    signal<bool(char32_t, int)> * signal_accel_ = nullptr;
    signal<Action_base *(char32_t, int)> * signal_lookup_action_ = nullptr;
    signal<void()> * signal_parent_ = nullptr;
    signal<bool(char32_t, int)> * signal_key_down_ = nullptr;
    signal<bool(char32_t, int)> * signal_key_up_ = nullptr;
    signal<bool(const ustring &)> * signal_input_ = nullptr;
    signal<bool(int, int, Point)> * signal_mouse_down_ = nullptr;
    signal<bool(int, int, Point)> * signal_mouse_double_click_ = nullptr;
    signal<bool(int, int, Point)> * signal_mouse_up_ = nullptr;
    signal<void(int, Point)> * signal_mouse_motion_ = nullptr;
    signal<void(Point)> * signal_mouse_enter_ = nullptr;
    signal<void()> * signal_mouse_leave_ = nullptr;
    signal<bool(int, int, Point)> * signal_mouse_wheel_ = nullptr;

private:

    void update_cursor();
    void enter_cursor();
    void leave_cursor();
    void delete_signals();

    bool on_backpaint(Painter pr, const Rect & inval);
    void on_tooltip_timer();
    void on_enable();
    void on_disable();
    void on_tooltip_close();
    void on_tooltip_mouse_leave();
    void on_action_accel_added(Accel & accel);
    void on_pan_changed();
};

} // namespace tau

#endif // TAU_WIDGET_IMPL_HH
