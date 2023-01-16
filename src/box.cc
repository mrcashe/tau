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

#include <tau/box.hh>
#include <box-impl.hh>

namespace tau {

#define BOX_IMPL (std::static_pointer_cast<Box_impl>(impl))

Box::Box(Orientation orient, unsigned spacing):
    Container(std::make_shared<Box_impl>(orient))
{
    BOX_IMPL->set_spacing(spacing);
}

Box::Box(Widget_ptr wp):
    Container(std::dynamic_pointer_cast<Box_impl>(wp))
{
}

void Box::set_spacing(unsigned spacing) {
    BOX_IMPL->set_spacing(spacing);
}

unsigned Box::spacing() const {
    return BOX_IMPL->spacing();
}

void Box::append(Widget & w, bool shrink) {
    BOX_IMPL->append(w.ptr(), shrink);
}

void Box::prepend(Widget & w, bool shrink) {
    BOX_IMPL->prepend(w.ptr(), shrink);
}

void Box::insert_before(Widget & w, const Widget & other, bool shrink) {
    BOX_IMPL->insert_before(w.ptr(), other.ptr().get(), shrink);
}

void Box::insert_after(Widget & w, const Widget & other, bool shrink) {
    BOX_IMPL->insert_after(w.ptr(), other.ptr().get(), shrink);
}

void Box::remove(Widget & w) {
    BOX_IMPL->remove(w.ptr().get());
}

void Box::remove_before(const Widget & w) {
    BOX_IMPL->remove_before(w.ptr().get());
}

void Box::remove_after(const Widget & w) {
    BOX_IMPL->remove_after(w.ptr().get());
}

void Box::remove_front() {
    BOX_IMPL->remove_front();
}

void Box::remove_back() {
    BOX_IMPL->remove_back();
}

bool Box::empty() const {
    return BOX_IMPL->empty();
}

void Box::clear() {
    BOX_IMPL->clear();
}

void Box::set_align(Align align) {
    BOX_IMPL->set_align(align);
}

Align Box::align() const {
    return BOX_IMPL->align();
}

bool Box::shrunk(const Widget & w) const {
    return BOX_IMPL->shrunk(w.ptr().get());
}

void Box::shrink(Widget & w) {
    BOX_IMPL->shrink(w.ptr().get());
}

void Box::expand(Widget & w) {
    BOX_IMPL->expand(w.ptr().get());
}

void Box::shrink_all() {
    BOX_IMPL->shrink_all();
}

void Box::expand_all() {
    BOX_IMPL->expand_all();
}

Orientation Box::orientation() const {
    return BOX_IMPL->orientation();
}

void Box::set_orientation(Orientation orient) {
    BOX_IMPL->set_orientation(orient);
}

Action & Box::focus_next_action() {
    return BOX_IMPL->focus_next_action();
}

Action & Box::focus_previous_action() {
    return BOX_IMPL->focus_previous_action();
}

signal<void()> & Box::signal_orientation_changed() {
    return BOX_IMPL->signal_orientation_changed();
}

} // namespace tau

//END
