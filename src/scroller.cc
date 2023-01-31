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
#include <tau/scroller.hh>
#include <tau/string.hh>
#include <scroller-impl.hh>

namespace tau {

#define SCROLLER_IMPL (std::static_pointer_cast<Scroller_impl>(impl))

Scroller::Scroller():
    Container(std::make_shared<Scroller_impl>())
{
}

Scroller::Scroller(Widget_ptr wp):
    Container(std::dynamic_pointer_cast<Scroller_impl>(wp))
{
}

Scroller & Scroller::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Scroller_impl>(wp)) {
        throw user_error(str_format(this, " Scroller::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

void Scroller::insert(Widget & w) {
    SCROLLER_IMPL->insert(w.ptr());
}

void Scroller::clear() {
    SCROLLER_IMPL->clear();
}

Size Scroller::logical_size() const {
    return SCROLLER_IMPL->logical_size();
}

void Scroller::pan_to_x(int x) {
    SCROLLER_IMPL->pan_to_x(x);
}

void Scroller::pan_to_y(int y) {
    SCROLLER_IMPL->pan_to_y(y);
}

void Scroller::pan_to(const Point & pos) {
    SCROLLER_IMPL->pan_to(pos);
}

void Scroller::pan_to(int x, int y) {
    SCROLLER_IMPL->pan_to(x, y);
}

Point Scroller::pan() const {
    return SCROLLER_IMPL->pan();
}

void Scroller::set_step(const Point & step) {
    SCROLLER_IMPL->set_step(step);
}

void Scroller::set_step(int xstep, int ystep) {
    SCROLLER_IMPL->set_step(xstep, ystep);
}

Point Scroller::step() const {
    return SCROLLER_IMPL->step();
}

Action & Scroller::pan_left_action() {
    return SCROLLER_IMPL->pan_left_action();
}

const Action & Scroller::pan_left_action() const {
    return SCROLLER_IMPL->pan_left_action();
}

Action & Scroller::pan_right_action() {
    return SCROLLER_IMPL->pan_right_action();
}

const Action & Scroller::pan_right_action() const {
    return SCROLLER_IMPL->pan_right_action();
}

Action & Scroller::pan_up_action() {
    return SCROLLER_IMPL->pan_up_action();
}

const Action & Scroller::pan_up_action() const {
    return SCROLLER_IMPL->pan_up_action();
}

Action & Scroller::pan_down_action() {
    return SCROLLER_IMPL->pan_down_action();
}

const Action & Scroller::pan_down_action() const {
    return SCROLLER_IMPL->pan_down_action();
}

Action & Scroller::page_up_action() {
    return SCROLLER_IMPL->page_up_action();
}

const Action & Scroller::page_up_action() const {
    return SCROLLER_IMPL->page_up_action();
}

Action & Scroller::page_down_action() {
    return SCROLLER_IMPL->page_down_action();
}

const Action & Scroller::page_down_action() const {
    return SCROLLER_IMPL->page_down_action();
}

Action & Scroller::home_action() {
    return SCROLLER_IMPL->home_action();
}

const Action & Scroller::home_action() const {
    return SCROLLER_IMPL->home_action();
}

Action & Scroller::end_action() {
    return SCROLLER_IMPL->end_action();
}

const Action & Scroller::end_action() const {
    return SCROLLER_IMPL->end_action();
}

signal<void()> & Scroller::signal_pan_changed() {
    return SCROLLER_IMPL->signal_pan_changed();
}

signal<void()> & Scroller::signal_logical_size_changed() {
    return SCROLLER_IMPL->signal_logical_size_changed();
}

} // namespace tau

//END
