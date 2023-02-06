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

#include <tau/accel.hh>
#include <tau/brush.hh>
#include <tau/input.hh>
#include <tau/toplevel.hh>
#include <tau/painter.hh>
#include <tau/widget.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <painter-impl.hh>
#include <scroller-impl.hh>
#include <text-impl.hh>
#include <theme-impl.hh>
#include <window-impl.hh>
#include <iostream>

namespace tau {

extern unsigned nwidgets_;

Widget_impl::Widget_impl():
    trackable()
{
    ++nwidgets_;
    Theme_impl::root()->init_style(style_);
    style_.get(STYLE_BACKGROUND).signal_changed().connect(tau::bind(fun(this, &Widget_impl::invalidate), Rect()));
    signal_origin_changed_.connect(bind(fun(this, &Widget_impl::invalidate), Rect()));
    signal_size_changed_.connect(bind(fun(this, &Widget_impl::invalidate), Rect()));
    signal_backpaint_.connect(fun(this, &Widget_impl::on_backpaint));

    signal_visible_.connect(fun(this, &Widget_impl::update_pdata));
    signal_visible_.connect(bind(fun(this, &Widget_impl::invalidate), Rect()));

    signal_invisible_.connect(fun(this, &Widget_impl::update_pdata));
    signal_invisible_.connect(fun(this, &Widget_impl::hide_tooltip));

    signal_hide_.connect(fun(this, &Widget_impl::clear_focus));
    signal_hide_.connect(fun(this, &Widget_impl::drop_focus));

    signal_enable_.connect(fun(this, &Widget_impl::on_enable));
    signal_disable_.connect(fun(this, &Widget_impl::on_disable));

    signal_display_.connect(fun(this, &Widget_impl::update_cursor));
    signal_display_.connect(fun(this, &Widget_impl::update_pdata));
    signal_destroy_.connect(bind(fun(this, &Widget_impl::shutdown), true));

    signal_focus_in_.connect(fun(this, &Widget_impl::hide_tooltip));
}

Widget_impl::~Widget_impl() {
    --nwidgets_;
    signal_destroy_();
    delete_signals();
}

void Widget_impl::delete_signals() {
    if (signal_accel_) { delete signal_accel_; signal_accel_ = nullptr; }
    if (signal_scroll_changed_) { delete signal_scroll_changed_; signal_scroll_changed_ = nullptr; }
    if (signal_lookup_action_) { delete signal_lookup_action_; signal_lookup_action_ = nullptr; }
    if (signal_parent_) { delete signal_parent_; signal_parent_ = nullptr; }
    if (signal_key_down_) { delete signal_key_down_; signal_key_down_ = nullptr; }
    if (signal_key_up_) { delete signal_key_up_; signal_key_up_ = nullptr; }
    if (signal_input_) { delete signal_input_; signal_input_ = nullptr; }
    if (signal_mouse_down_) { delete signal_mouse_down_; signal_mouse_down_ = nullptr; }
    if (signal_mouse_double_click_) { delete signal_mouse_double_click_; signal_mouse_double_click_ = nullptr; }
    if (signal_mouse_up_) { delete signal_mouse_up_; signal_mouse_up_ = nullptr; }
    if (signal_mouse_wheel_) { delete signal_mouse_wheel_; signal_mouse_wheel_ = nullptr; }
    if (signal_mouse_motion_) { delete signal_mouse_motion_; signal_mouse_motion_ = nullptr; }
    if (signal_mouse_enter_) { delete signal_mouse_enter_; signal_mouse_enter_ = nullptr; }
    if (signal_mouse_leave_) { delete signal_mouse_leave_; signal_mouse_leave_ = nullptr; }
}

// Overriden by Popup_impl.
// Overriden by Toplevel_impl.
void Widget_impl::set_parent(Container_impl * parent) {
    parent_ = parent;
    shutdown(false);
    style_.set_parent(parent->style());

    if ((scroller_ = dynamic_cast<Scroller_impl *>(parent_))) {
        if (signal_scroll_changed_) { delete signal_scroll_changed_; signal_scroll_changed_ = nullptr; }
        pan_cx_ = scroller_->signal_pan_changed().connect(fun(this, &Widget_impl::on_pan_changed));
    }

    handle_parent();
}

void Widget_impl::unparent() {
    shutdown(true);
    hide_tooltip();
    bool vis = visible();
    enabled_ = disabled_ = frozen_ = upshow_ = false;
    if (!shut_ && vis) { signal_invisible_(); }
    style_.unparent();
    pan_cx_.drop();
    scroller_ = nullptr;
    parent_ = nullptr;
    origin_.set(INT_MIN, INT_MIN);
    size_.reset();
    required_size_.reset();
    handle_unparent();
    delete_signals();
}

void Widget_impl::enable() {
    if (disabled_) {
        disabled_ = false;
        if (!shut_ && !frozen_) { signal_enable_(); }
    }
}

void Widget_impl::thaw() {
    if (frozen_) {
        frozen_ = false;
        if (!shut_ && !disabled_) { signal_enable_(); }
    }
}

void Widget_impl::disable() {
    if (!disabled_) {
        disabled_ = true;
        if (!shut_ && !frozen_) { signal_disable_(); }
    }
}

void Widget_impl::freeze() {
    if (!frozen_) {
        frozen_ = true;
        if (!shut_ && !disabled_) { signal_disable_(); }
    }
}

bool Widget_impl::disabled() const {
    return disabled_ || frozen_;
}

void Widget_impl::show() {
    if (hidden_) {
        hidden_ = false;

        if (!shut_ && !disappeared_) {
            if (parent_) { parent_->on_child_show(this); }
            signal_show_();
            if (visible()) { signal_visible_(); }
        }
    }
}

void Widget_impl::appear() {
    if (disappeared_) {
        disappeared_ = false;

        if (!shut_ && !hidden_) {
            if (parent_) { parent_->on_child_show(this); }
            signal_show_();
            if (visible()) { signal_visible_(); }
        }
    }
}

void Widget_impl::hide() {
    if (!hidden_) {
        bool was_visible = visible();
        hidden_ = true;

        if (!shut_ && !disappeared_) {
            if (parent_) { parent_->on_child_hide(this); }
            signal_hide_();
            if (was_visible) { signal_invisible_(); }
        }
    }
}

void Widget_impl::disappear() {
    if (!disappeared_) {
        bool was_visible = visible();
        disappeared_ = true;

        if (!shut_ && !hidden_) {
            if (parent_) { parent_->on_child_hide(this); }
            signal_hide_();
            if (was_visible) { signal_invisible_(); }
        }
    }
}

bool Widget_impl::hidden() const {
    return hidden_ || disappeared_;
}

Rect Widget_impl::exposed_area() const {
    if (auto wnd = window()) {
        if (visible()) {
            Point ofs(to_window());
            Rect va(visible_area()), r(Rect(ofs, va.size()) & Rect(wnd->size()));
            if (r) { return r.translated(scroll_position()-ofs); }
        }
    }

    return Rect();
}

// Overriden by Window_impl.
void Widget_impl::invalidate(const Rect & r) {
    if (!shut_ && parent_) {
        Rect inval(r ? r : visible_area());
        inval &= exposed_area();

        if (inval) {
            inval.translate(origin()-scroll_position());
            parent_->invalidate(inval);
        }
    }
}

// Overriden by Container_impl.
bool Widget_impl::hover() const {
    return !shut_ && parent_ && (this == parent_->mouse_grabber() || this == parent_->mouse_owner());
}

bool Widget_impl::hint_margin_left(unsigned left) {
    if (margin_left_hint_ != left) {
        margin_left_hint_ = left;

        if (!shut_) {
            if (parent_) { parent_->on_child_hints(this); }
            signal_hints_changed_();
        }

        return true;
    }

    return false;
}

bool Widget_impl::hint_margin_right(unsigned right) {
    if (margin_right_hint_ != right) {
        margin_right_hint_ = right;

        if (!shut_) {
            if (parent_) { parent_->on_child_hints(this); }
            signal_hints_changed_();
        }

        return true;
    }

    return false;
}

bool Widget_impl::hint_margin_top(unsigned top) {
    if (margin_top_hint_ != top) {
        margin_top_hint_ = top;

        if (!shut_) {
            if (parent_) { parent_->on_child_hints(this); }
            signal_hints_changed_();
        }

        return true;
    }

    return false;
}

bool Widget_impl::hint_margin_bottom(unsigned bottom) {
    if (margin_bottom_hint_ != bottom) {
        margin_bottom_hint_ = bottom;

        if (!shut_) {
            if (parent_) { parent_->on_child_hints(this); }
            signal_hints_changed_();
        }

        return true;
    }

    return false;
}

bool Widget_impl::hint_margin(unsigned left, unsigned right, unsigned top, unsigned bottom) {
    bool changed = false;

    if (margin_left_hint_ != left) {
        margin_left_hint_ = left;
        changed = true;
    }

    if (margin_right_hint_ != right) {
        margin_right_hint_ = right;
        changed = true;
    }

    if (margin_top_hint_ != top) {
        margin_top_hint_ = top;
        changed = true;
    }

    if (margin_bottom_hint_ != bottom) {
        margin_bottom_hint_ = bottom;
        changed = true;
    }

    if (!shut_ && changed) {
        if (parent_) { parent_->on_child_hints(this); }
        signal_hints_changed_();
    }

    return changed;
}

bool Widget_impl::hint_margin(unsigned w) {
    return hint_margin(w, w, w, w);
}

bool Widget_impl::hint_size(const Size & sz) {
    if (size_hint_.update(sz) && !shut_) {
        if (parent_) { parent_->on_child_hints(this); }
        signal_hints_changed_();
        return true;
    }

    return false;
}

bool Widget_impl::hint_size(unsigned width, unsigned height) {
    return hint_size(Size(width, height));
}

bool Widget_impl::hint_min_size(const Size & sz) {
    if (min_size_hint_.update(sz) && !shut_) {
        if (parent_) { parent_->on_child_hints(this); }
        signal_hints_changed_();
        return true;
    }

    return false;
}

bool Widget_impl::hint_min_size(unsigned width, unsigned height) {
    return hint_min_size(Size(width, height));
}

bool Widget_impl::hint_max_size(const Size & sz) {
    if (max_size_hint_.update(sz) && !shut_) {
        if (parent_) { parent_->on_child_hints(this); }
        signal_hints_changed_();
        return true;
    }

    return false;
}

bool Widget_impl::hint_max_size(unsigned width, unsigned height) {
    return hint_max_size(Size(width, height));
}

bool Widget_impl::require_size(const Size & size) {
    if (required_size_.update(size) && !shut_) {
        if (parent_) { parent_->on_child_requisition(this); }
        signal_requisition_changed_();
        return true;
    }

    return false;
}

bool Widget_impl::require_size(unsigned width, unsigned height) {
    return require_size(Size(width, height));
}

bool Widget_impl::update_origin(const Point & pt) {
    bool changed = origin_.update(pt);

    if (!shut_) {
        if (changed) { signal_origin_changed_(); }
        else { update_pdata(); }
    }

    return changed;
}

bool Widget_impl::update_size(const Size & size) {
    Size was_size = size_;
    bool changed = size_.update(size);

    if (!shut_) {
        update_pdata();

        if (changed) {
            signal_size_changed_();
            if (!was_size && visible()) { signal_visible_(); }
            else if (!size_) { signal_invisible_(); }
        }
    }

    return changed;
}

// Return false to allow user append it's own slot after this one.
bool Widget_impl::on_backpaint(Painter pr, const Rect & inval) {
    auto & ci = style_.get(STYLE_BACKGROUND);

    if (ci.is_set()) {
        pr.set_brush(Color(ci.get()));
        pr.paint();
    }

    return false;
}

// Overriden by Window_xxx.
Painter Widget_impl::painter() {
    if (!shut_) {
        if (auto wnd = window()) {
            if (auto pr = wnd->painter()) {
                if (auto pi = pr.impl) {
                    pi->capture(this);
                    signal_visible_.connect(tau::bind(fun(pi, &Painter_impl::capture), this));
                    signal_invisible_.connect(tau::bind(fun(pi, &Painter_impl::capture), this));
                    signal_pdata_changed_.connect(tau::bind(fun(pi, &Painter_impl::capture), this));
                }

                return pr;
            }
        }
    }

    return Painter();
}

// Overriden by Container_impl.
// Overriden by Window_impl.
bool Widget_impl::grab_mouse_up(Widget_impl *) {
    if (!shut_ && enabled() && parent_) {
        if (this == parent_->mouse_grabber()) { return true; }
        return parent_->grab_mouse_up(this);
    }

    return false;
}

bool Widget_impl::grab_mouse() {
    bool had_mouse = hover();

    if (grab_mouse_up(this)) {
        if (!had_mouse) {
            handle_mouse_enter(where_mouse());
        }

        return true;
    }

    return false;
}

// Overriden by Container_impl.
// Overriden by Window_impl.
bool Widget_impl::ungrab_mouse_up(Widget_impl *) {
    return !shut_ && parent_ && parent_->ungrab_mouse_up(this);
}

bool Widget_impl::ungrab_mouse() {
    bool had_mouse = hover();

    if (ungrab_mouse_up(this)) {
        if (!hover() && had_mouse) { handle_mouse_leave(); }
        return true;
    }

    return false;
}

// Overriden by Container_impl.
// Overriden by Window_impl.
bool Widget_impl::grabs_mouse() const {
    return parent_ && this == parent_->mouse_grabber();
}

// Overriden by Window_impl.
Point Widget_impl::where_mouse() const {
    return !shut_ && parent_ ? parent_->where_mouse()-origin() : Point(INT_MIN, INT_MIN);
}

// Overriden by Window_impl.
Point Widget_impl::to_screen(const Point & pt) const {
    return !shut_ && parent_ ? parent_->to_screen(pt+origin())-parent_->scroll_position() : pt;
}

// Overriden by Window_impl.
// Overriden by Toplevel_impl.
// Overriden by Popup_impl.
Point Widget_impl::to_root(const Point & pt) const {
    return !shut_ && parent_ ? parent_->to_root(pt+origin())-parent_->scroll_position() : pt;
}

// Overriden by Window_impl.
Point Widget_impl::to_window(const Point & pt) const {
    if (INT_MIN != worigin_.x()) { return pt+worigin_; }
    return !shut_ && parent_ ? parent_->to_window(pt+origin())-parent_->scroll_position() : pt;
}

Point Widget_impl::to_parent(const Point & pt) const {
    return !shut_ && parent_ ? pt+origin_-parent_->scroll_position() : pt;
}

// Overriden by Popup_impl.
// Overriden by Toplevel_impl.
Window_impl * Widget_impl::root() {
    return !shut_ && parent_ ? parent_->root() : nullptr;
}

// Overriden by Popup_impl.
// Overriden by Toplevel_impl.
const Window_impl * Widget_impl::root() const {
    return !shut_ && parent_ ? parent_->root() : nullptr;
}

// Overriden by Window_impl.
Window_impl * Widget_impl::window() {
    return !shut_ && parent_ ? parent_->window() : nullptr;
}

// Overriden by Window_impl.
const Window_impl * Widget_impl::window() const {
    return !shut_ && parent_ ? parent_->window() : nullptr;
}

Point Widget_impl::to_parent(const Container_impl * ci, const Point & pt) const {
    if (!shut_ && parent_) {
        if (parent_ == ci) {
            return pt+origin_;
        }

        return parent_->to_parent(ci, pt+origin_);
    }

    return pt;
}

void Widget_impl::set_tooltip(const ustring & tooltip_text) {
    unset_tooltip();
    tooltip_text_ = tooltip_text;
}

void Widget_impl::set_tooltip(Widget_ptr tooltip_widget) {
    unset_tooltip();
    tooltip_widget_ = tooltip_widget;
}

bool Widget_impl::has_tooltip() const {
    return !tooltip_text_.empty() || (nullptr != tooltip_widget_);
}

void Widget_impl::unset_tooltip() {
    hide_tooltip();
    tooltip_widget_.reset();
    tooltip_text_.clear();
}

void Widget_impl::on_tooltip_close() {
    tooltip_exposed_ = false;
}

void Widget_impl::on_tooltip_mouse_leave() {
    block_tooltip_ = Rect(size()).contains(where_mouse());
}

void Widget_impl::show_tooltip(const ustring & tooltip_text) {
    Text_ptr tp = std::make_shared<Text_impl>(tooltip_text);
    tp->style().font("font").enlarge(-2);
    tp->hint_margin(2);
    show_tooltip(tp);
}

void Widget_impl::show_tooltip(Widget_ptr tooltip_widget) {
    hide_tooltip();

    if (hover()) {
        if (auto wip = window()) {
            if (Window_ptr tip = wip->open_tooltip(this, tooltip_widget)) {
                tip->signal_close().connect(fun(this, &Widget_impl::on_tooltip_close));
                tip->signal_mouse_leave().connect(fun(this, &Widget_impl::on_tooltip_mouse_leave));
                tooltip_exposed_ = true;
            }
        }
    }
}

void Widget_impl::show_tooltip(const ustring & tooltip_text, const Point & pt, Gravity gravity, unsigned time_ms) {
    Text_ptr tp = std::make_shared<Text_impl>(tooltip_text);
    tp->style().font("font").enlarge(-2);
    tp->hint_margin(2);
    show_tooltip(tp, pt, gravity, time_ms);
}

void Widget_impl::show_tooltip(Widget_ptr tooltip_widget, const Point & pt, Gravity gravity, unsigned time_ms) {
    hide_tooltip();

    if (auto wip = window()) {
        if (Window_ptr tip = wip->open_tooltip(this, tooltip_widget, to_window(pt), gravity, time_ms)) {
            tip->signal_close().connect(fun(this, &Widget_impl::on_tooltip_close));
            tip->signal_mouse_leave().connect(fun(this, &Widget_impl::on_tooltip_mouse_leave));
            tooltip_exposed_ = true;
        }
    }
}

void Widget_impl::hide_tooltip() {
    tooltip_cx_.drop();

    if (tooltip_exposed_) {
        if (auto wip = window()) { wip->close_tooltip(this); }
        block_tooltip_ = false;
        tooltip_exposed_ = false;
    }
}

void Widget_impl::on_tooltip_timer() {
    if (hover()) {
        if (tooltip_widget_) { show_tooltip(tooltip_widget_); }
        else if (!tooltip_text_.empty()) { show_tooltip(tooltip_text_); }
        tooltip_delay_ = std::min(3015U, tooltip_delay_+212U);
    }
}

void Widget_impl::enter_cursor() {
    if (cursor_hidden_) {
        hide_cursor_up();
    }

    else if (cursor_) {
        set_cursor_up(cursor_);
    }
}

void Widget_impl::leave_cursor() {
    if (cursor_hidden_) {
        show_cursor_up();
    }

    else if (cursor_) {
        unset_cursor_up();
    }
}

void Widget_impl::on_enable() {
    if (hover()) { enter_cursor(); }
    invalidate();
}

void Widget_impl::on_disable() {
    drop_focus();
    ungrab_mouse();
    hide_tooltip();
    if (hover()) { leave_cursor(); }
    invalidate();
}

void Widget_impl::scroll_to(const Point & pt) {
    if (scroller_) { scroller_->pan_to(pt); }
}

void Widget_impl::scroll_to(int x, int y) {
    scroll_to(Point(x, y));
}

void Widget_impl::scroll_to_x(int x) {
    scroll_to(Point(x, scroll_position().y()));
}

void Widget_impl::scroll_to_y(int y) {
    scroll_to(Point(scroll_position().x(), y));
}

Point Widget_impl::scroll_position() const {
    return scroller_ ? scroller_->pan() : Point();
}

signal<void()> & Widget_impl::signal_scroll_changed() {
    if (scroller_) { return scroller_->signal_pan_changed(); }
    if (!signal_scroll_changed_) { signal_scroll_changed_ = new signal<void()>; }
    return *signal_scroll_changed_;
}

void Widget_impl::on_pan_changed() {
    update_pdata();
    invalidate();
}

// Overriden by Container_impl.
// Overriden by Window_impl.
bool Widget_impl::grab_modal_up(Widget_impl * caller) {
    return !shut_ && focusable() && parent_ && parent_->grab_modal_up(this);
}

bool Widget_impl::grab_modal() {
    if (enabled() && !hidden() && grab_modal_up(this)) {
        resume_focus();
        return true;
    }

    return false;
}

// Overriden by Container_impl.
// Overriden by Window_impl.
bool Widget_impl::end_modal_up(Widget_impl * ) {
    return !shut_ && parent_ && parent_->end_modal_up(this);
}

void Widget_impl::end_modal() {
    end_modal_up(this);
}

// Overriden by Window_impl.
bool Widget_impl::has_modal() const {
    return focused_ && parent_ && this == parent_->modal_child();
}

// Overriden by Container_impl.
// Overriden by Window_impl.
int Widget_impl::grab_focus_up(Widget_impl *) {
    return !shut_ && focusable() && parent_ ? parent_->grab_focus_up(this) : -1;
}

bool Widget_impl::grab_focus() {
    if (enabled() && !hidden()) {
        int res = grab_focus_up(this);
        if (res < 0) { return false; }
        if (res > 0) { resume_focus(); }
        return true;
    }

    return false;
}

bool Widget_impl::take_focus() {
    return !hidden() && enabled() && focusable() && signal_take_focus_();
}

// Overriden by Container_impl.
// Overriden by Window_impl.
void Widget_impl::drop_focus_up(Widget_impl *) {
    if (!shut_ && parent_ && this == parent_->focused_child()) {
        parent_->drop_focus_up(this);
    }
}

void Widget_impl::drop_focus() {
    end_modal();
    drop_focus_up(this);
}

// Overriden by Container_impl.
void Widget_impl::resume_focus() {
    if (!focused_ && focus_allowed_) {
        focused_ = true;
        signal_focus_in_();
    }
}

// Overriden by Container_impl.
void Widget_impl::suspend_focus() {
    if (focused_) {
        focused_ = false;
        signal_focus_out_();
    }
}

// Overriden by Container_impl.
void Widget_impl::clear_focus() {
    suspend_focus();
}

void Widget_impl::allow_focus() {
    focus_allowed_ = true;
}

void Widget_impl::disallow_focus() {
    if (focus_allowed_) {
        focus_allowed_ = false;
        clear_focus();
    }
}

bool Widget_impl::focusable() const {
    return focus_allowed_;
}

Size Widget_impl::margin_hint() const {
    return Size(margin_left_hint_+margin_right_hint_, margin_top_hint_+margin_bottom_hint_);
}

Point Widget_impl::margin_origin() const {
    return Point(margin_left_hint_, margin_top_hint_);
}

Rect Widget_impl::visible_area() const {
    Rect r(size_);

    if (size_) {
        if (auto pp = dynamic_cast<const Scroller_impl *>(parent_)) {
            r.translate(pp->pan());
        }
    }

    return r;
}

// Overriden by Window_impl.
Display_ptr Widget_impl::display() {
    return !shut_ && parent_ ? parent_->display() : nullptr;
}

// Overriden by Window_impl.
Display_cptr Widget_impl::display() const {
    return !shut_ && parent_ ? parent_->display() : nullptr;
}

void Widget_impl::update_cursor() {
    if (!cursor_name_.empty()) {
        if (auto cursor = Theme_impl::root()->find_cursor(cursor_name_, cursor_size_)) {
            set_cursor(cursor);
        }
    }
}

// Overriden by Container_impl.
// Overriden by Window_impl.
void Widget_impl::set_cursor_up(Cursor_ptr cursor) {
    if (!shut_ && parent_) {
        parent_->set_cursor_up(cursor);
    }
}

// Overriden by Container_impl.
// Overriden by Window_impl.
void Widget_impl::unset_cursor_up() {
    if (!shut_ && parent_) {
        parent_->unset_cursor_up();
    }
}

void Widget_impl::set_cursor(Cursor_ptr cursor) {
    if (cursor) {
        cursor_ = cursor;
        if (hover() && cursor_visible() && enabled()) { set_cursor_up(cursor_); }
    }
}

void Widget_impl::set_cursor(const ustring & name, unsigned size) {
    if (!name.empty()) {
        cursor_name_ = name;
        cursor_size_ = size;

        if (auto cursor = Theme_impl::root()->find_cursor(name, size)) {
            set_cursor(cursor);

            if (cursor_theme_cx_.empty()) {
                cursor_theme_cx_ = Theme_impl::root()->signal_cursors_changed().connect(fun(this, &Widget_impl::update_cursor));
            }
        }
    }
}

void Widget_impl::unset_cursor() {
    if (cursor_) {
        cursor_.reset();
        unset_cursor_up();
    }

    cursor_theme_cx_.drop();
    cursor_name_.clear();
    cursor_size_ = 0;
}

// Overriden by Container_impl.
// Overriden by Window_impl.
void Widget_impl::show_cursor_up() {
    if (!shut_ && parent_ && hover()) {
        parent_->show_cursor_up();
    }
}

void Widget_impl::show_cursor() {
    if (cursor_hidden_) {
        cursor_hidden_ = false;
        show_cursor_up();
    }
}

// Overriden by Window_impl.
void Widget_impl::hide_cursor_up() {
    if (!shut_ && parent_ && hover()) {
        parent_->hide_cursor_up();
    }
}

void Widget_impl::hide_cursor() {
    if (!cursor_hidden_) {
        cursor_hidden_ = true;
        hide_cursor_up();
    }
}

bool Widget_impl::cursor_hidden() const {
    return cursor_hidden_;
}

// Overriden by Window_impl.
bool Widget_impl::cursor_visible() const {
    return !cursor_hidden() && visible() && !shut_ && parent_ && parent_->cursor_visible();
}

// Overriden by Window_impl.
void Widget_impl::quit_dialog() {
    if (!shut_ && parent_) { parent_->quit_dialog(); }
}

signal<bool(char32_t, int)> & Widget_impl::signal_accel() {
    if (!signal_accel_) { signal_accel_ = new signal<bool(char32_t, int)>; }
    return *signal_accel_;
}

connection Widget_impl::connect_accel(Accel & accel, bool prepend) {
    return signal_accel().connect(fun(accel, &Accel::handle_accel), prepend);
}

void Widget_impl::connect_action(Action_base & action, bool prepend) {
    auto & accels = action.accels();

    for (auto i = accels.rbegin(); i != accels.rend(); ++i) {
        signal_accel().connect(fun(*i, &Accel::handle_accel), prepend);
    }

    action.signal_accel_added().connect(fun(this, &Widget_impl::on_action_accel_added));
    if (!signal_lookup_action_) { signal_lookup_action_ = new signal<Action_base *(char32_t, int)>; }
    signal_lookup_action_->connect(fun(action, &Action_base::lookup));
}

// Overriden by Container_impl.
Action_base * Widget_impl::lookup_action(char32_t kc, int km) {
    return signal_lookup_action_ ? (*signal_lookup_action_)(kc, km) : nullptr;
}

void Widget_impl::on_action_accel_added(Accel & accel) {
    connect_accel(accel);
}

// Overriden by Container_impl.
bool Widget_impl::handle_accel(char32_t kc, int km) {
    return enabled() && signal_accel_ && (*signal_accel_)(kc, km);
}

// Overriden by Container_impl.
bool Widget_impl::handle_key_down(char32_t kc, int km) {
    return enabled() && signal_key_down_ && (*signal_key_down_)(kc, km);
}

signal<bool(char32_t, int)> & Widget_impl::signal_key_down() {
    if (!signal_key_down_) { signal_key_down_ = new signal<bool(char32_t, int)>; }
    return *signal_key_down_;
}

// Overriden by Container_impl.
bool Widget_impl::handle_key_up(char32_t kc, int km) {
    return enabled() && signal_key_up_ && (*signal_key_up_)(kc, km);
}

signal<bool(char32_t, int)> & Widget_impl::signal_key_up() {
    if (!signal_key_up_) { signal_key_up_ = new signal<bool(char32_t, int)>; }
    return *signal_key_up_;
}

signal<bool(const ustring &)> & Widget_impl::signal_input() {
    if (!signal_input_) { signal_input_ = new signal<bool(const ustring &)>; }
    return *signal_input_;
}

// Overriden by Container_impl.
bool Widget_impl::handle_input(const ustring & s) {
    return enabled() && signal_input_ && (*signal_input_)(std::cref(s));
}

// Overriden by Container_impl.
bool Widget_impl::handle_mouse_down(int mbt, int mm, const Point & pt) {
    return signal_mouse_down_ && (*signal_mouse_down_)(mbt, mm, pt);
}

// Overriden by Container_impl.
bool Widget_impl::handle_mouse_up(int mbt, int mm, const Point & pt) {
    return enabled() && signal_mouse_up_ && (*signal_mouse_up_)(mbt, mm, pt);
}

// Overriden by Container_impl.
bool Widget_impl::handle_mouse_double_click(int mbt, int mm, const Point & pt) {
    return enabled() && signal_mouse_double_click_ && (*signal_mouse_double_click_)(mbt, mm, pt);
}

signal<bool(int, int, Point)> & Widget_impl::signal_mouse_down() {
    if (!signal_mouse_down_) { signal_mouse_down_ = new signal<bool(int, int, Point)>; }
    return *signal_mouse_down_;
}

signal<bool(int, int, Point)> & Widget_impl::signal_mouse_double_click() {
    if (!signal_mouse_double_click_) { signal_mouse_double_click_ = new signal<bool(int, int, Point)>; }
    return *signal_mouse_double_click_;
}

signal<bool(int, int, Point)> & Widget_impl::signal_mouse_up() {
    if (!signal_mouse_up_) { signal_mouse_up_ = new signal<bool(int, int, Point)>; }
    return *signal_mouse_up_;
}

// Overriden by Container_impl.
void Widget_impl::handle_mouse_motion(int mm, const Point & pt) {
    if (enabled() && signal_mouse_motion_) { (*signal_mouse_motion_)(mm, pt); }
}

// Overriden by Container_impl.
void Widget_impl::handle_mouse_enter(const Point & pt) {
    enter_cursor();

    if (enabled()) {
        if (has_tooltip() && !block_tooltip_ && !tooltip_exposed_) {
            if (tooltip_cx_.empty()) {
                if (auto dp = display()) {
                    if (auto loop = dp->loop()) {
                        tooltip_cx_ = loop->signal_alarm(tooltip_delay_).connect(fun(this, &Widget_impl::on_tooltip_timer));
                    }
                }
            }
        }

        block_tooltip_ = false;
        if (signal_mouse_enter_) { (*signal_mouse_enter_)(pt); }
    }
}

// Overriden by Container_impl.
void Widget_impl::handle_mouse_leave() {
    tooltip_cx_.drop();
    leave_cursor();
    if (enabled() && signal_mouse_leave_) { (*signal_mouse_leave_)(); }
}

// Overriden by Container_impl.
bool Widget_impl::handle_mouse_wheel(int delta, int mm, const Point & pt) {
    return enabled() && signal_mouse_wheel_ && (*signal_mouse_wheel_)(delta, mm, pt);
}

signal<void(int, Point)> & Widget_impl::signal_mouse_motion() {
    if (!signal_mouse_motion_) { signal_mouse_motion_ = new signal<void(int, Point)>; }
    return *signal_mouse_motion_;
}

signal<void(Point)> & Widget_impl::signal_mouse_enter() {
    if (!signal_mouse_enter_) { signal_mouse_enter_ = new signal<void(Point)>; }
    return *signal_mouse_enter_;
}

signal<void()> & Widget_impl::signal_mouse_leave() {
    if (!signal_mouse_leave_) { signal_mouse_leave_ = new signal<void()>; }
    return *signal_mouse_leave_;
}

signal<bool(int, int, Point)> & Widget_impl::signal_mouse_wheel() {
    if (!signal_mouse_wheel_) { signal_mouse_wheel_ = new signal<bool(int, int, Point)>; }
    return *signal_mouse_wheel_;
}

// Overriden by Container_impl.
void Widget_impl::handle_paint(Painter pr, const Rect & inval) {
    signal_paint_(pr, inval.translated(scroll_position()));
}

// Overriden by Container_impl.
void Widget_impl::handle_backpaint(Painter pr, const Rect & inval) {
    signal_backpaint_(pr, inval.translated(scroll_position()));
}

// Overriden by Container_impl.
void Widget_impl::handle_display() {
    if (!shut_) { signal_display_(); }
}

// Overriden by Container_impl.
void Widget_impl::handle_parent() {
    if (!shut_ && signal_parent_) { (*signal_parent_)(); }
}

signal<void()> & Widget_impl::signal_parent() {
    if (!signal_parent_) { signal_parent_ = new signal<void()>; }
    return *signal_parent_;
}

// Overriden by Container_impl.
void Widget_impl::handle_unparent() {
    if (!shut_) { signal_unparent_(); }
}

// Overriden by Window_impl.
bool Widget_impl::has_window() const {
    return !shut_ && parent_ && parent_->has_window();
}

bool Widget_impl::has_parent() const {
    return nullptr != parent_;
}

bool Widget_impl::scrollable() const {
    return nullptr != scroller_;
}

void Widget_impl::handle_enable(bool enable) {
    if (enabled_ != enable) {
        enabled_ = enable;

        if (!disabled()) {
            if (enable) { signal_enable_(); }
            else { signal_disable_(); }
        }
    }
}

bool Widget_impl::visible() const {
    return !hidden() && size() && upshow_;
}

void Widget_impl::handle_visible(bool show) {
    if (upshow_ != show) {
        upshow_ = show;

        if (!hidden()) {
            if (show) { signal_visible_(); }
            else { signal_invisible_(); }
        }
    }
}

// Overriden by Container_impl.
void Widget_impl::update_pdata() {
    worigin_.set(INT_MIN, INT_MIN);
    Point po = scroll_position()-origin();
    Rect  cr;

    if (visible()) {
        cr.set(size_);

        if (!shut_ && parent_) {
            po += parent_->poffset();
            Widget_impl * wi = this;

            for (Container_impl * pp = parent_; cr && pp; wi = pp, pp = pp->parent()) {
                cr.translate(wi->origin()-pp->scroll_position());
                cr &= Rect(pp->size());
            }
        }
    }

    bool changed = false, obscured_changed = false;

    if (worigin_.update(to_window())) {
        changed = true;
    }

    if (poffset_.update(po)) {
        changed = true;
    }

    if (pclip_ != cr) {
        obscured_changed = cr.empty() || pclip_.empty();
        pclip_ = cr;
        changed = true;
    }

    if (obscured_changed && parent_) {
        parent_->on_child_obscured(this, !cr.empty());
    }

    if (changed) {
        signal_pdata_changed_();
    }
}

Painter Widget_impl::wrap_painter(Painter_ptr pp) {
    return Painter(pp);
}

Painter_ptr Widget_impl::strip(Painter pr) {
    return pr.impl;
}

// Overriden by Container_impl.
void Widget_impl::shutdown(bool yes) {
    shut_ = yes;
}

} // namespace tau

//END
