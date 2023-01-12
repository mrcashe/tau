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

#ifndef TAU_BUTTON_IMPL_HH
#define TAU_BUTTON_IMPL_HH

#include <tau/accel.hh>
#include <tau/input.hh>
#include <frame-impl.hh>

namespace tau {

class Button_base_impl: public Frame_impl {
protected:

    Button_base_impl();
    explicit Button_base_impl(const ustring & label);
    explicit Button_base_impl(Widget_ptr img);
    Button_base_impl(Widget_ptr img, const ustring & label);
    Button_base_impl(const ustring & icon_name, int icon_size);
    Button_base_impl(const ustring & label, const ustring & icon_name, int icon_size);
    explicit Button_base_impl(Action_base & action, bool use_label=true);
    Button_base_impl(Action_base & action, int icon_size, bool use_label=true);

public:

    void set_image(Widget_ptr wp);
    void set_label(const ustring & text);
    void set_icon(const ustring & icon_name, int icon_size);
    void show_relief();
    void hide_relief();
    bool relief_visible() const;

protected:

    Table_ptr   table_;
    Widget_ptr  image_;
    Widget_ptr  label_;
    Timer       timer_;

    bool        pressed_ = false;
    bool        fix_press_ = false;
    bool        relief_visible_ = true;
    connection  timer_cx_;

protected:

    void init();
    void init_action(Action_base & action, int icon_size, bool use_label);

    void on_mouse_enter(const Point & pt);
    bool on_mouse_down(int mbt, int mm, const Point & position);
    bool on_mouse_up(int mbt, int mm, const Point & position);

    void press();
    void redraw();

    virtual void on_release() {}
    virtual void on_press() {}

private:

    ustring tooltip_;

private:

    void on_mouse_leave();
    void on_action_label_changed(const ustring & label);
    void on_action_tooltip_changed(const ustring & tooltip, Action_base & action);
    void on_action_accel_changed(const Accel & accel, Action_base & action);
    void set_action_tooltip(Action_base & action);
};

class Button_impl: public Button_base_impl {
public:

    Button_impl();
    explicit Button_impl(const ustring & label);
    explicit Button_impl(Widget_ptr img);
    Button_impl(Widget_ptr img, const ustring & label);
    Button_impl(const ustring & icon_name, int icon_size);
    Button_impl(const ustring & label, const ustring & icon_name, int icon_size);
    explicit Button_impl(Action & action, bool use_label=true);
    Button_impl(Action & action, int icon_size, bool use_label=true);

    void enable_repeat();
    void disable_repeat();
    bool repeat_enabled() const;
    void set_repeat_delay(unsigned first, unsigned next);
    unsigned repeat_delay() const;
    unsigned repeat_interval() const;

    signal<void()> & signal_click() { return signal_click_; }

protected:

    // Overrides Button_base_impl.
    void on_press() override;

    // Overrides Button_base_impl.
    void on_release() override;

private:

    bool            repeat_  = false;
    unsigned        repeat_delay_ = 392;
    unsigned        repeat_interval_ = 124;
    Accel           space_accel_ { U'_', KM_NONE, fun(this, &Button_impl::on_keyboard_activate) };
    Accel           enter_accel_ { KC_ENTER, KM_NONE, fun(this, &Button_impl::on_keyboard_activate) };
    signal<void()>  signal_click_;

private:

    void init();
    bool on_keyboard_activate();
    void on_release_timeout();
    void on_repeat_timeout();
    void on_mouse_leave();
    void on_disable();
    void click();
};

class Toggle_impl: public Button_base_impl {
public:

    Toggle_impl();
    explicit Toggle_impl(const ustring & label);
    explicit Toggle_impl(Widget_ptr img);
    Toggle_impl(Widget_ptr img, const ustring & label);
    Toggle_impl(const ustring & icon_name, int icon_size);
    Toggle_impl(const ustring & label, const ustring & icon_name, int icon_size);
    explicit Toggle_impl(Toggle_action & action, bool use_label=true);
    Toggle_impl(Toggle_action & action, int icon_size, bool use_label=true);

    bool toggled() const { return state_; }
    void toggle();

    signal<void(bool)> & signal_toggle() { return signal_toggle_; }

protected:

    // Overrides Button_base_impl.
    void on_release() override;

private:

    bool state_ = false;
    signal<void(bool)>  signal_toggle_;

private:

    void on_action_toggle(bool state);
};

} // namespace tau

#endif // TAU_BUTTON_IMPL_HH
