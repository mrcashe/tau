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

#include <tau/cycle.hh>
#include <cycle-impl.hh>

namespace tau {

#define CYCLE_IMPL (std::static_pointer_cast<Cycle_impl>(impl))

Cycle::Cycle(Border_style bs):
    Widget(std::make_shared<Cycle_impl>(bs))
{
}

Cycle::Cycle(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Cycle_impl>(wp))
{
}

void Cycle::set_border_style(Border_style bs) {
    CYCLE_IMPL->set_border_style(bs);
}

Border_style Cycle::border_style() const {
    return CYCLE_IMPL->border_style();
}

void Cycle::add(Widget & w) {
    CYCLE_IMPL->add(w.ptr());
}

void Cycle::append(Widget & w, bool shrink) {
    CYCLE_IMPL->append(w.ptr(), shrink);
}

Widget_ptr Cycle::append(const ustring & text, unsigned margin_left, unsigned margin_right) {
    return CYCLE_IMPL->append(text, margin_left, margin_right);
}

void Cycle::prepend(Widget & w, bool shrink) {
    CYCLE_IMPL->prepend(w.ptr(), shrink);
}

Widget_ptr Cycle::prepend(const ustring & text, unsigned margin_left, unsigned margin_right) {
    return CYCLE_IMPL->prepend(text, margin_left, margin_right);
}

void Cycle::remove(Widget & w) {
    CYCLE_IMPL->remove(w.ptr());
}

} // namespace tau

//END
