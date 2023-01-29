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

#include <tau/counter.hh>
#include <counter-impl.hh>

namespace tau {

#define COUNTER_IMPL (std::static_pointer_cast<Counter_impl>(impl))

Counter::Counter(double value, double max_value, double min_value):
    Widget(std::make_shared<Counter_impl>(value, max_value, min_value))
{
}

Counter::Counter(Border_style bs, double value, double max_value, double min_value):
    Widget(std::make_shared<Counter_impl>(bs, value, max_value, min_value))
{
}

Counter::Counter(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Counter_impl>(wp))
{
}

void Counter::set_border_style(Border_style bs) {
    COUNTER_IMPL->set_border_style(bs);
}

Border_style Counter::border_style() const {
    return COUNTER_IMPL->border_style();
}

void Counter::allow_edit() {
    COUNTER_IMPL->allow_edit();
}

void Counter::disallow_edit() {
    COUNTER_IMPL->disallow_edit();
}

bool Counter::edit_allowed() const {
    return COUNTER_IMPL->edit_allowed();
}

void Counter::show_buttons() {
    COUNTER_IMPL->show_buttons();
}

void Counter::hide_buttons() {
    COUNTER_IMPL->hide_buttons();
}

bool Counter::buttons_visible() const {
    return COUNTER_IMPL->buttons_visible();
}

void Counter::set_value(double value) {
    COUNTER_IMPL->set_value(value);
}

double Counter::value() const {
    return COUNTER_IMPL->value();
}

ustring Counter::text() const {
    return COUNTER_IMPL->text();
}

void Counter::set_min_value(double minimal_value) {
    COUNTER_IMPL->set_min_value(minimal_value);
}

double Counter::min_value() const {
    return COUNTER_IMPL->minimal_value();
}

void Counter::set_max_value(double max_value) {
    COUNTER_IMPL->set_max_value(max_value);
}

double Counter::max_value() const {
    return COUNTER_IMPL->max_value();
}

void Counter::set_step_value(double step_value) {
    COUNTER_IMPL->set_step_value(step_value);
}

double Counter::step_value() const {
    return COUNTER_IMPL->step_value();
}

void Counter::set_precision(int prec) {
    COUNTER_IMPL->set_precision(prec);
}

int Counter::precision() const {
    return COUNTER_IMPL->precision();
}

void Counter::set_fixed() {
    COUNTER_IMPL->set_fixed();
}

void Counter::unset_fixed() {
    COUNTER_IMPL->unset_fixed();
}

bool Counter::fixed() const {
    return COUNTER_IMPL->fixed();
}

void Counter::set_width(int width) {
    COUNTER_IMPL->set_width(width);
}

int Counter::width() const {
    return COUNTER_IMPL->width();
}

void Counter::set_fill(char c) {
    COUNTER_IMPL->set_fill(c);
}

char Counter::fill() const {
    return COUNTER_IMPL->fill();
}

void Counter::set_caps() {
    COUNTER_IMPL->set_caps();
}

void Counter::unset_caps() {
    COUNTER_IMPL->unset_caps();
}

bool Counter::caps() const {
    return COUNTER_IMPL->caps();
}

void Counter::set_base(int base) {
    COUNTER_IMPL->set_base(base);
}

int Counter::base() const {
    return COUNTER_IMPL->base();
}

void Counter::increase() {
    COUNTER_IMPL->increase();
}

void Counter::increase_page() {
    COUNTER_IMPL->increase_page();
}

void Counter::decrease() {
    COUNTER_IMPL->decrease();
}

void Counter::decrease_page() {
    COUNTER_IMPL->decrease_page();
}

void Counter::append(Widget & w, bool shrink) {
    COUNTER_IMPL->append(w.ptr(), shrink);
}

Widget_ptr Counter::append(const ustring & text, unsigned margin_left_hint, unsigned margin_right_hint) {
    return COUNTER_IMPL->append(text, margin_left_hint, margin_right_hint);
}

void Counter::prepend(Widget & w, bool shrink) {
    COUNTER_IMPL->prepend(w.ptr(), shrink);
}

Widget_ptr Counter::prepend(const ustring & text, unsigned margin_left_hint, unsigned margin_right_hint) {
    return COUNTER_IMPL->prepend(text, margin_left_hint, margin_right_hint);
}

signal<void(double)> & Counter::signal_value_changed() {
    return COUNTER_IMPL->signal_value_changed();
}

} // namespace tau

//END
