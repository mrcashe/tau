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

#include <tau/buffer.hh>
#include <tau/container.hh>
#include <tau/exception.hh>
#include <tau/painter.hh>
#include <tau/string.hh>
#include <tau/toplevel.hh>
#include <tau/widget.hh>
#include <container-impl.hh>
#include <cursor-impl.hh>
#include <display-impl.hh>
#include <window-impl.hh>

namespace tau {

Widget::Widget():
    trackable(),
    impl(std::make_shared<Widget_impl>())
{
}

Widget::~Widget() {}

Widget::Widget(Widget_ptr wp):
    trackable(),
    impl(wp)
{
    if (!impl) {
        throw user_error(str_format(this, " Widget::Widget(Widget_ptr): got pure or incompatible implementation pointer"));
    }
}

Widget::Widget(std::nullptr_t):
    trackable()
{
}

Widget_ptr Widget::ptr() {
    return impl;
}

Widget_cptr Widget::ptr() const {
    return impl;
}

Widget & Widget::operator=(Widget_ptr wp) {
    if (!wp) { throw user_error(str_format(this, " Widget::operator=(Widget_ptr): got pure or incompatible implementation pointer")); }
    impl = wp;
    return *this;
}

void Widget::show() {
    impl->show();
}

void Widget::hide() {
    impl->hide();
}

void Widget::appear() {
    impl->appear();
}

void Widget::disappear() {
    impl->disappear();
}

Rect Widget::exposed_area() const {
    return impl->exposed_area();
}

Point Widget::origin() const {
    return impl->origin();
}

Size Widget::size() const {
    return impl->size();
}

bool Widget::require_size(const Size & sz) {
    return impl->require_size(sz);
}

bool Widget::require_size(unsigned width, unsigned height) {
    return impl->require_size(width, height);
}

Size Widget::required_size() const {
    return impl->required_size();
}

bool Widget::hint_size(const Size & sz) {
    return impl->hint_size(sz);
}

bool Widget::hint_size(unsigned width, unsigned height) {
    return impl->hint_size(width, height);
}

Size Widget::size_hint() const {
    return impl->size_hint();
}

bool Widget::hint_min_size(const Size & sz) {
    return impl->hint_min_size(sz);
}

bool Widget::hint_min_size(unsigned width, unsigned height) {
    return impl->hint_min_size(width, height);
}

Size Widget::min_size_hint() const {
    return impl->min_size_hint();
}

bool Widget::hint_max_size(const Size & sz) {
    return impl->hint_max_size(sz);
}

bool Widget::hint_max_size(unsigned width, unsigned height) {
    return impl->hint_max_size(width, height);
}

Size Widget::max_size_hint() const {
    return impl->max_size_hint();
}

bool Widget::hint_margin_left(unsigned left) {
    return impl->hint_margin_left(left);
}

bool Widget::hint_margin_right(unsigned right) {
    return impl->hint_margin_right(right);
}

bool Widget::hint_margin_top(unsigned top) {
    return impl->hint_margin_top(top);
}

bool Widget::hint_margin_bottom(unsigned bottom) {
    return impl->hint_margin_bottom(bottom);
}

bool Widget::hint_margin(unsigned left, unsigned right, unsigned top, unsigned bottom) {
    return impl->hint_margin(left, right, top, bottom);
}

bool Widget::hint_margin(unsigned width) {
    return impl->hint_margin(width);
}

unsigned Widget::margin_left_hint() const {
    return impl->margin_left_hint();
}

unsigned Widget::margin_right_hint() const {
    return impl->margin_right_hint();
}

unsigned Widget::margin_top_hint() const {
    return impl->margin_top_hint();
}

unsigned Widget::margin_bottom_hint() const {
    return impl->margin_bottom_hint();
}

Size Widget::margin_hint() const {
    return impl->margin_hint();
}

Point Widget::margin_origin() const {
    return impl->margin_origin();
}

void Widget::enable() {
    impl->enable();
}

void Widget::disable() {
    impl->disable();
}

void Widget::freeze() {
    impl->freeze();
}

void Widget::thaw() {
    impl->thaw();
}

bool Widget::enabled() const {
    return impl->enabled();
}

bool Widget::disabled() const {
    return impl->disabled();
}

bool Widget::hover() const {
    return impl->hover();
}

Style & Widget::style() {
    return impl->style();
}

const Style & Widget::style() const {
    return impl->style();
}

bool Widget::hidden() const {
    return impl->hidden();
}

bool Widget::visible() const {
    return impl->visible();
}

bool Widget::grab_modal() {
    return impl->grab_modal();
}

void Widget::end_modal() {
    impl->end_modal();
}

bool Widget::has_modal() const {
    return impl->has_modal();
}

void Widget::allow_focus() {
    impl->allow_focus();
}

void Widget::disallow_focus() {
    impl->disallow_focus();
}

bool Widget::focused() const {
    return impl->focused();
}

bool Widget::focusable() const {
    return impl->focusable();
}

bool Widget::grab_focus() {
    return impl->grab_focus();
}

void Widget::drop_focus() {
    impl->drop_focus();
}

bool Widget::take_focus() {
    return impl->take_focus();
}

bool Widget::grab_mouse() {
    return impl->grab_mouse();
}

Point Widget::where_mouse() const {
    return impl->where_mouse();
}

bool Widget::grabs_mouse() const {
    return impl->grabs_mouse();
}

bool Widget::ungrab_mouse() {
    return impl->ungrab_mouse();
}

void Widget::set_cursor(Cursor cursor) {
    impl->set_cursor(Cursor_impl::strip(cursor));
}

void Widget::set_cursor(const ustring & name, unsigned size) {
    impl->set_cursor(name, size);
}

Cursor Widget::cursor() {
    return Cursor_impl::wrap(impl->cursor());
}

void Widget::unset_cursor() {
    impl->unset_cursor();
}

void Widget::show_cursor() {
    impl->show_cursor();
}

void Widget::hide_cursor() {
    impl->hide_cursor();
}

bool Widget::cursor_hidden() const {
    return impl->cursor_hidden();
}

bool Widget::cursor_visible() const {
    return impl->cursor_visible();
}

void Widget::invalidate(const Rect & r) {
    impl->invalidate(r);
}

Painter Widget::painter() {
    return impl->painter();
}

void Widget::quit_dialog() {
    impl->quit_dialog();
}

Point Widget::to_screen(const Point & pt) const {
    return impl->to_screen(pt);
}

Point Widget::to_root(const Point & pt) const {
    return impl->to_root(pt);
}

Point Widget::to_window(const Point & pt) const {
    return impl->to_window(pt);
}

Point Widget::to_parent(const Point & pt) const {
    return impl->to_parent(pt);
}

Point Widget::to_parent(const Container & cont, const Point & pt) const {
    return impl->to_parent(static_cast<const Container_impl *>(cont.ptr().get()), pt);
}

connection Widget::connect_accel(Accel & accel, bool prepend) {
    return impl->connect_accel(accel, prepend);
}

void Widget::connect_action(Action_base & action, bool prepend) {
    impl->connect_action(action, prepend);
}

Action_base * Widget::lookup_action(char32_t kc, int km) {
    return impl->lookup_action(kc, km);
}

void Widget::set_tooltip(const ustring & tooltip_text) {
    impl->set_tooltip(tooltip_text);
}

void Widget::set_tooltip(Widget & tooltip_widget) {
    impl->set_tooltip(tooltip_widget.ptr());
}

bool Widget::has_tooltip() const {
    return impl->has_tooltip();
}

void Widget::unset_tooltip() {
    impl->unset_tooltip();
}

void Widget::show_tooltip(const ustring & tooltip_text) {
    impl->show_tooltip(tooltip_text);
}

void Widget::show_tooltip(Widget & tooltip_widget) {
    impl->show_tooltip(tooltip_widget.ptr());
}

void Widget::show_tooltip(const ustring & tooltip_text, const Point & pt, Gravity gravity, unsigned time_ms) {
    impl->show_tooltip(tooltip_text, pt, gravity, time_ms);
}

void Widget::show_tooltip(Widget & tooltip_widget, const Point & pt, Gravity gravity, unsigned time_ms) {
    impl->show_tooltip(tooltip_widget.ptr(), pt, gravity, time_ms);
}

void Widget::hide_tooltip() {
    impl->hide_tooltip();
}

Rect Widget::visible_area() const {
    return impl->visible_area();
}

bool Widget::obscured() const {
    return impl->obscured();
}

Point Widget::scroll_position() const {
    return impl->scroll_position();
}

void Widget::scroll_to(const Point & pt) {
    impl->scroll_to(pt);
}

void Widget::scroll_to(int x, int y) {
    impl->scroll_to(x, y);
}

void Widget::scroll_to_x(int x) {
    impl->scroll_to_x(x);
}

void Widget::scroll_to_y(int y) {
    impl->scroll_to_y(y);
}

Display Widget::display() {
    return Display_impl::wrap(impl->display());
}

const Display Widget::display() const {
    return Display_impl::wrap(impl->display());
}

bool Widget::has_parent() const {
    return impl->has_parent();
}

bool Widget::has_window() const {
    return impl->has_window();
}

bool Widget::scrollable() const {
    return impl->scrollable();
}

signal<void()> & Widget::signal_destroy() {
    return impl->signal_destroy();
}

signal<void()> & Widget::signal_visible() {
    return impl->signal_visible();
}

signal<void()> & Widget::signal_invisible() {
    return impl->signal_invisible();
}

signal<void()> & Widget::signal_enable() {
    return impl->signal_enable();
}

signal<void()> & Widget::signal_disable() {
    return impl->signal_disable();
}

signal<void()> & Widget::signal_origin_changed() {
    return impl->signal_origin_changed();
}

signal<void()> & Widget::signal_size_changed() {
    return impl->signal_size_changed();
}

signal<void()> & Widget::signal_scroll_changed() {
    return impl->signal_scroll_changed();
}

signal<bool(Painter, Rect)> & Widget::signal_paint() {
    return impl->signal_paint();
}

signal<bool(Painter, Rect)> & Widget::signal_backpaint() {
    return impl->signal_backpaint();
}

signal<bool(char32_t, int)> & Widget::signal_key_down() {
    return impl->signal_key_down();
}

signal<bool(char32_t, int)> & Widget::signal_key_up() {
    return impl->signal_key_up();
}

signal<bool(const ustring &)> & Widget::signal_input() {
    return impl->signal_input();
}

signal<bool(int, int, Point)> & Widget::signal_mouse_down() {
    return impl->signal_mouse_down();
}

signal<bool(int, int, Point)> & Widget::signal_mouse_double_click() {
    return impl->signal_mouse_double_click();
}

signal<bool(int, int, Point)> & Widget::signal_mouse_up() {
    return impl->signal_mouse_up();
}

signal<void(int, Point)> & Widget::signal_mouse_motion() {
    return impl->signal_mouse_motion();
}

signal<void(Point)> & Widget::signal_mouse_enter() {
    return impl->signal_mouse_enter();
}

signal<void()> & Widget::signal_mouse_leave() {
    return impl->signal_mouse_leave();
}

signal<bool(int, int, Point)> & Widget::signal_mouse_wheel() {
    return impl->signal_mouse_wheel();
}

signal<void()> & Widget::signal_focus_in() {
    return impl->signal_focus_in();
}

signal<void()> & Widget::signal_focus_out() {
    return impl->signal_focus_out();
}

signal<void()> & Widget::signal_select() {
    return impl->signal_select();
}

signal<void()> & Widget::signal_unselect() {
    return impl->signal_unselect();
}

signal<void()> & Widget::signal_hints_changed() {
    return impl->signal_hints_changed();
}

signal<void()> & Widget::signal_requisition_changed() {
    return impl->signal_requisition_changed();
}

signal<void()> & Widget::signal_show() {
    return impl->signal_show();
}

signal<void()> & Widget::signal_hide() {
    return impl->signal_hide();
}

signal<void()> & Widget::signal_parent() {
    return impl->signal_parent();
}

signal<void()> & Widget::signal_display() {
    return impl->signal_display();
}

signal<void()> & Widget::signal_unparent() {
    return impl->signal_unparent();
}

signal<bool()> & Widget::signal_take_focus() {
    return impl->signal_take_focus();
}

} // namespace tau

//END
