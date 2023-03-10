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

#include <tau/painter.hh>
#include <box-impl.hh>
#include <button-impl.hh>
#include <icon-impl.hh>
#include <text-impl.hh>
#include <table-impl.hh>
#include <iostream>

namespace tau {

Button_base_impl::Button_base_impl():
    Frame_impl()
{
    init();
}

Button_base_impl::Button_base_impl(const ustring & label):
    Frame_impl()
{
    init();
    set_label(label);
}

Button_base_impl::Button_base_impl(Widget_ptr img):
    Frame_impl()
{
    init();
    set_image(img);
}

Button_base_impl::Button_base_impl(Widget_ptr img, const ustring & label):
    Frame_impl()
{
    init();
    set_image(img);
    set_label(label);
}

Button_base_impl::Button_base_impl(const ustring & label, const ustring & icon_name, int icon_size):
    Frame_impl()
{
    init();
    set_label(label);
    set_icon(icon_name, icon_size);
}

Button_base_impl::Button_base_impl(const ustring & icon_name, int icon_size):
    Frame_impl()
{
    init();
    set_icon(icon_name, icon_size);
}

Button_base_impl::Button_base_impl(Action_base & action, Action_items items):
    Frame_impl()
{
    init();
    init_action(action, MEDIUM_ICON, items);
}

Button_base_impl::Button_base_impl(Action_base & action, int icon_size, Action_items items):
    Frame_impl()
{
    init();
    init_action(action, icon_size, items);
}

void Button_base_impl::init() {
    disallow_focus();
    set_border(1);
    set_border_style(BORDER_OUTSET);

    style_.redirect(STYLE_BUTTON_BACKGROUND, STYLE_BACKGROUND);
    style_.get(STYLE_BACKGROUND).signal_changed().connect(fun(this, &Button_base_impl::redraw));
    style_.get(STYLE_FOREGROUND).signal_changed().connect(fun(this, &Button_base_impl::redraw));

    table_ = std::make_shared<Table_impl>(5);
    insert(table_);

    signal_mouse_enter().connect(fun(this, &Button_base_impl::on_mouse_enter));
    signal_mouse_leave().connect(fun(this, &Button_base_impl::on_mouse_leave));
    signal_mouse_down().connect(fun(this, &Button_base_impl::on_mouse_down), true);
    signal_mouse_up().connect(fun(this, &Button_base_impl::on_mouse_up), true);
}

void Button_base_impl::init_action(Action_base & action, int icon_size, Action_items items) {
    action.signal_enable().connect(fun(this, &Button_base_impl::thaw));
    action.signal_disable().connect(fun(this, &Button_base_impl::freeze));
    action.signal_show().connect(fun(this, &Button_base_impl::appear));
    action.signal_hide().connect(fun(this, &Button_base_impl::disappear));
    if (!action.visible()) { disappear(); }
    if (!action.enabled()) { freeze(); }

    if (items & ACTION_LABEL) {
        action.signal_label_changed().connect(fun(this, &Button_base_impl::on_action_label_changed));
        if (!action.label().empty()) { set_label(action.label()); }
    }

    if (items & ACTION_TOOLTIP) {
        action.signal_tooltip_changed().connect(tau::bind(fun(this, &Button_base_impl::on_action_tooltip_changed), std::ref(action)));
        set_action_tooltip(action);
        tooltip_ = action.tooltip();
    }

    if (items & ACTION_ACCEL) {
        action.signal_accel_added().connect(tau::bind(fun(this, &Button_base_impl::on_action_accel_changed), std::ref(action)));
        action.signal_accel_removed().connect(tau::bind(fun(this, &Button_base_impl::on_action_accel_changed), std::ref(action)));
    }

    if (items & ACTION_ICON) {
        action.signal_icon_changed().connect(tau::bind(fun(this, &Button_base_impl::set_icon), icon_size));
        if (!action.icon_name().empty()) { set_icon(action.icon_name(), icon_size); }
    }
}

void Button_base_impl::set_label(const ustring & s) {
    table_->remove(label_.get());

    if (!s.empty()) {
        label_ = std::make_shared<Text_impl>(s);
        label_->hint_margin(1);
        table_->put(label_, 1, 0, 1, 1, false, true);
        table_->align(label_.get(), ALIGN_CENTER, ALIGN_FILL);
    }
}

void Button_base_impl::set_image(Widget_ptr wp) {
    table_->remove(image_.get());
    image_ = wp;
    table_->set_column_margin(0, 1, 1);
    table_->put(image_, 0, 0, 1, 1, true, true);
    table_->align(image_.get(), ALIGN_FILL, ALIGN_FILL);
}

void Button_base_impl::set_icon(const ustring & icon_name, int icon_size) {
    set_image(std::make_shared<Icon_impl>(icon_name, icon_size));
}

void Button_base_impl::on_action_label_changed(const ustring & label) {
    if (label_) {
        set_label(label);
    }
}

void Button_base_impl::set_action_tooltip(Action_base & action) {
    auto & accels = action.accels();

    if (!accels.empty()) {
        auto box = std::make_shared<Box_impl>(OR_EAST, 8);
        if (!tooltip_.empty()) { box->append(std::make_shared<Text_impl>(tooltip_), true); }
        auto tp = std::make_shared<Text_impl>(accels.front().label());
        tp->style().redirect(STYLE_ACCEL_FOREGROUND, STYLE_FOREGROUND);
        box->append(tp, true);
        box->hint_margin(2);
        box->style().font(STYLE_FONT).enlarge(-2);
        set_tooltip(box);
    }

    else if (!tooltip_.empty()) {
        set_tooltip(tooltip_);
    }

    else {
        unset_tooltip();
    }
}

void Button_base_impl::on_action_accel_changed(const Accel & accel, Action_base & action) {
    set_action_tooltip(action);
}

void Button_base_impl::on_action_tooltip_changed(const ustring & tooltip, Action_base & action) {
    tooltip_ = tooltip;
    set_action_tooltip(action);
}

void Button_base_impl::on_mouse_enter(const Point & pt) {
    if (!pressed_) {
        redraw();
    }
}

void Button_base_impl::on_mouse_leave() {
    fix_press_ = false;
    timer_.stop();
    timer_cx_.drop();
    redraw();
}

bool Button_base_impl::on_mouse_up(int mbt, int mm, const Point & position) {
    if (MBT_LEFT == mbt) {
        if (fix_press_) {
            fix_press_ = false;
            timer_.stop();
            timer_cx_.drop();
            on_release();
        }

        return true;
    }

    return false;
}

bool Button_base_impl::on_mouse_down(int mbt, int mm, const Point & position) {
    if (MBT_LEFT == mbt) {
        hide_tooltip();
        fix_press_ = true;
        grab_focus();
        press();
        on_press();
        return true;
    }

    return false;
}

void Button_base_impl::show_relief() {
    if (!relief_visible_) {
        relief_visible_ = true;
        if (label_) { label_->hint_margin(2); }
        set_border(1);
        redraw();
    }
}

void Button_base_impl::hide_relief() {
    if (relief_visible_) {
        relief_visible_ = false;
        if (label_) { label_->hint_margin(0); }
        set_border(0);
        redraw();
    }
}

bool Button_base_impl::relief_visible() const {
    return relief_visible_;
}

void Button_base_impl::redraw() {
    Color c;

    if (relief_visible_) {
        c = style().color(STYLE_BACKGROUND);

        if (hover() && enabled() && !pressed_) {
            set_border_color(style().color(STYLE_SELECT_BACKGROUND));
            set_border_style(BORDER_SOLID);
        }

        else {
            unset_border_color();
            set_border_style(pressed_ ? BORDER_INSET : BORDER_OUTSET);
        }
    }

    else {
        c = style().color(STYLE_BACKGROUND);
    }

    if (enabled()) {
        if (pressed_) {
            c.darker(0.1);
        }

        else {
            if (hover()) {
                c.lighter(0.07);
            }
        }
    }

    table_->style().color(STYLE_BUTTON_BACKGROUND).set(c);
}

void Button_base_impl::press() {
    if (enabled()) {
        if (!pressed_) {
            pressed_ = true;
            redraw();
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Button_impl::Button_impl():
    Button_base_impl()
{
    init();
}

Button_impl::Button_impl(const ustring & label):
    Button_base_impl(label)
{
    init();
}

Button_impl::Button_impl(Widget_ptr img):
    Button_base_impl(img)
{
    init();
}

Button_impl::Button_impl(Widget_ptr img, const ustring & label):
    Button_base_impl(img, label)
{
    init();
}

Button_impl::Button_impl(const ustring & label, const ustring & icon_name, int icon_size):
    Button_base_impl(label, icon_name, icon_size)
{
    init();
}

Button_impl::Button_impl(const ustring & icon_name, int icon_size):
    Button_base_impl(icon_name, icon_size)
{
    init();
}

Button_impl::Button_impl(Action & action, Action_items items):
    Button_base_impl(action, items)
{
    init();
    signal_click_.connect(fun(action, &Action::exec));
}

Button_impl::Button_impl(Action & action, int icon_size, Action_items items):
    Button_base_impl(action, icon_size, items)
{
    init();
    signal_click_.connect(fun(action, &Action::exec));
}

void Button_impl::init() {
    connect_accel(space_accel_);
    connect_accel(enter_accel_);
    signal_mouse_leave().connect(fun(this, &Button_impl::on_mouse_leave), true);
    signal_disable().connect(fun(this, &Button_impl::on_disable));
}

void Button_impl::click() {
    signal_click_();
}

void Button_impl::enable_repeat() {
    repeat_ = true;
}

void Button_impl::disable_repeat() {
    repeat_ = false;
}

bool Button_impl::repeat_enabled() const {
    return repeat_;
}

unsigned Button_impl::repeat_delay() const {
    return repeat_delay_;
}

unsigned Button_impl::repeat_interval() const {
    return repeat_interval_;
}

void Button_impl::set_repeat_delay(unsigned first, unsigned next) {
    if (0 != first) {
        repeat_delay_ = first;
        repeat_interval_ = 0 != next ? next : first;
    }
}

bool Button_impl::on_keyboard_activate() {
    hide_tooltip();
    press();
    timer_cx_.drop();
    timer_cx_ = timer_.signal_alarm().connect(fun(this, &Button_impl::on_release_timeout));
    timer_.start(140);
    return true;
}

void Button_impl::on_release_timeout() {
    timer_.stop();
    timer_cx_.drop();
    on_release();
}

void Button_impl::on_repeat_timeout() {
    if (repeat_) {
        if (hover()) { timer_.restart(repeat_interval_); }
        click();
    }

    else {
        timer_.stop();
        timer_cx_.drop();
    }
}

void Button_impl::on_mouse_leave() {
    pressed_ = false;
}

// Overrides Button_base_impl.
void Button_impl::on_press() {
    if (repeat_) {
        timer_cx_.drop();
        timer_cx_ = timer_.signal_alarm().connect(fun(this, &Button_impl::on_repeat_timeout));
        timer_.restart(repeat_delay_);
        click();
    }
}

void Button_impl::on_disable() {
    timer_.stop();
    timer_cx_.drop();
    if (pressed_) { pressed_ = false; redraw(); }
}

void Button_impl::on_release() {
    pressed_ = false;
    redraw();
    if (!repeat_) { click(); }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Toggle_impl::Toggle_impl():
    Button_base_impl()
{
}

Toggle_impl::Toggle_impl(const ustring & label):
    Button_base_impl(label)
{
}

Toggle_impl::Toggle_impl(Widget_ptr img):
    Button_base_impl(img)
{
}

Toggle_impl::Toggle_impl(Widget_ptr img, const ustring & label):
    Button_base_impl(img, label)
{
}

Toggle_impl::Toggle_impl(const ustring & label, const ustring & icon_name, int icon_size):
    Button_base_impl(label, icon_name, icon_size)
{
}

Toggle_impl::Toggle_impl(const ustring & icon_name, int icon_size):
    Button_base_impl(icon_name, icon_size)
{
}

Toggle_impl::Toggle_impl(Toggle_action & action, Action_items items):
    Button_base_impl(action, items)
{
    signal_toggle_.connect(fun(action, &Toggle_action::set));
    action.connect(fun(this, &Toggle_impl::on_action_toggle));
}

Toggle_impl::Toggle_impl(Toggle_action & action, int icon_size, Action_items items):
    Button_base_impl(action, icon_size, items)
{
    signal_toggle_.connect(fun(action, &Toggle_action::set));
    action.connect(fun(this, &Toggle_impl::on_action_toggle));
}

void Toggle_impl::toggle() {
    if (state_) {
        state_ = pressed_ = false;
        signal_toggle_(false);
    }

    else {
        state_ = pressed_ = true;
        signal_toggle_(true);
    }

    redraw();
}

void Toggle_impl::on_release() {
    pressed_ = !state_;
    state_ = pressed_;
    redraw();
    signal_toggle_(state_);
}

void Toggle_impl::on_action_toggle(bool state) {
    state_ = pressed_ = state;
    redraw();
}

} // namespace tau

//END
