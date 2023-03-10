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

#include <tau/container.hh>
#include <container-impl.hh>

namespace tau {

#define CONTAINER_IMPL (std::static_pointer_cast<Container_impl>(impl))

Container::Container(Widget_ptr wp):
    Widget(wp)
{
}

Container::Container(std::nullptr_t):
    Widget(nullptr)
{
}

Widget_ptr Container::focus_owner() {
    return CONTAINER_IMPL->focus_owner();
}

Widget_cptr Container::focus_owner() const {
    return CONTAINER_IMPL->focus_owner();
}

std::vector<Widget_ptr> Container::children() const {
    return CONTAINER_IMPL->children();
}

void Container::queue_arrange() {
    CONTAINER_IMPL->queue_arrange();
}

void Container::make_child(Widget & w) {
    CONTAINER_IMPL->make_child(w.ptr());
}

void Container::unparent_child(Widget & w) {
    CONTAINER_IMPL->unparent_child(w.ptr().get());
}

bool Container::update_child_bounds(Widget & w, const Rect & bounds) {
    if (w.ptr()->parent() == impl.get()) {
        return CONTAINER_IMPL->update_child_bounds(w.ptr().get(), bounds);
    }

    return false;
}

bool Container::update_child_bounds(Widget & w, const Point & origin, const Size & sz) {
    if (w.ptr()->parent() == impl.get()) {
        return CONTAINER_IMPL->update_child_bounds(w.ptr().get(), origin, sz);
    }

    return false;
}

bool Container::update_child_bounds(Widget & w, int x, int y, const Size & sz) {
    if (w.ptr()->parent() == impl.get()) {
        return CONTAINER_IMPL->update_child_bounds(w.ptr().get(), x, y, sz);
    }

    return false;
}

bool Container::update_child_bounds(Widget & w, int x, int y, unsigned width, unsigned height) {
    if (w.ptr()->parent() == impl.get()) {
        return CONTAINER_IMPL->update_child_bounds(w.ptr().get(), x, y, width, height);
    }

    return false;
}

signal<void()> & Container::signal_arrange() {
    return CONTAINER_IMPL->signal_arrange();
}

signal<void()> & Container::signal_children_changed() {
    return CONTAINER_IMPL->signal_children_changed();
}

} // namespace tau

//END
