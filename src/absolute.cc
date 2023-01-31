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

#include <tau/absolute.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <absolute-impl.hh>

#define ABSOLUTE_IMPL (std::static_pointer_cast<Absolute_impl>(impl))

namespace tau {

Absolute::Absolute():
    Container(std::make_shared<Absolute_impl>())
{
}

Absolute::Absolute(Widget_ptr wp):
    Container(std::dynamic_pointer_cast<Absolute_impl>(wp))
{
}

Absolute & Absolute::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Absolute_impl>(wp)) {
        throw user_error(str_format(this, " Absolute::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

void Absolute::put(Widget & w, const Point & pos, const Size & size) {
    ABSOLUTE_IMPL->put(w.ptr(), pos, size);
}

void Absolute::put(Widget & w, const Point & pos, unsigned width, unsigned height) {
    ABSOLUTE_IMPL->put(w.ptr(), pos, width, height);
}

void Absolute::put(Widget & w, int x, int y, const Size & size) {
    ABSOLUTE_IMPL->put(w.ptr(), x, y, size);
}

void Absolute::put(Widget & w, int x, int y, unsigned width, unsigned height) {
    ABSOLUTE_IMPL->put(w.ptr(), x, y, width, height);
}

void Absolute::move(Widget & w, const Point & pt) {
    ABSOLUTE_IMPL->move(w.ptr().get(), pt);
}

void Absolute::move(Widget & w, int x, int y) {
    ABSOLUTE_IMPL->move(w.ptr().get(), x, y);
}

void Absolute::move_rel(Widget & w, const Point & dpt) {
    ABSOLUTE_IMPL->move_rel(w.ptr().get(), dpt);
}

void Absolute::move_rel(Widget & w, int dx, int dy) {
    ABSOLUTE_IMPL->move_rel(w.ptr().get(), dx, dy);
}

void Absolute::resize(Widget & w, const Size & sz) {
    ABSOLUTE_IMPL->resize(w.ptr().get(), sz);
}

void Absolute::resize(Widget & w, unsigned width, unsigned height) {
    ABSOLUTE_IMPL->resize(w.ptr().get(), width, height);
}

void Absolute::remove(Widget & w) {
    ABSOLUTE_IMPL->remove(w.ptr().get());
}

void Absolute::clear() {
    ABSOLUTE_IMPL->clear();
}

bool Absolute::empty() const {
    return ABSOLUTE_IMPL->empty();
}

} // namespace tau

//END
