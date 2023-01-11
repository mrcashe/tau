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

#include <tau/toplevel.hh>
#include <tau/window.hh>
#include <window-impl.hh>

namespace tau {

#define WINDOW_IMPL (std::static_pointer_cast<Window_impl>(impl))

Window::Window(Widget_ptr wp):
    Bin(wp)
{
}

Window::Window(std::nullptr_t):
    Bin(nullptr)
{
}

Point Window::position() const {
    return WINDOW_IMPL->position();
}

void Window::move(const Point & pt) {
    WINDOW_IMPL->move(pt);
}

void Window::move(int x, int y) {
    WINDOW_IMPL->move(x, y);
}

void Window::move(const Rect & r) {
    WINDOW_IMPL->move(r);
}

void Window::move(int x, int y, unsigned width, unsigned height) {
    WINDOW_IMPL->move(x, y, width, height);
}

void Window::resize(const Size & sz) {
    WINDOW_IMPL->resize(sz);
}

void Window::resize(unsigned width, unsigned height) {
    WINDOW_IMPL->resize(width, height);
}

void Window::update() {
    WINDOW_IMPL->update();
}

signal<void()> & Window::signal_position_changed() {
    return WINDOW_IMPL->signal_position_changed();
}

signal<void()> & Window::signal_close() {
    return WINDOW_IMPL->signal_close();
}

} // namespace tau

//END
