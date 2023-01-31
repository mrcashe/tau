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

#include <tau/colorsel.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <colorsel-impl.hh>

namespace tau {

#define COLORSEL_IMPL (std::static_pointer_cast<Colorsel_impl>(impl))

Colorsel::Colorsel(const Color & initial_color):
    Widget(std::make_shared<Colorsel_impl>(initial_color))
{
}

Colorsel::Colorsel(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Colorsel_impl>(wp))
{
}

Colorsel & Colorsel::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Colorsel_impl>(wp)) {
        throw user_error(str_format(this, " Colorsel::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

void Colorsel::set_color(const Color & color) {
    COLORSEL_IMPL->set_color(color);
}

Color Colorsel::color() const {
    return COLORSEL_IMPL->color();
}

Action & Colorsel::apply_action() {
    return COLORSEL_IMPL->apply_action();
}

const Action & Colorsel::apply_action() const {
    return COLORSEL_IMPL->apply_action();
}

Action & Colorsel::cancel_action() {
    return COLORSEL_IMPL->cancel_action();
}

/// Get "Cancel" action.
const Action & Colorsel::cancel_action() const {
    return COLORSEL_IMPL->cancel_action();
}

signal<void(const Color &)> & Colorsel::signal_color_changed() {
    return COLORSEL_IMPL->signal_color_changed();
}

} // namespace tau

//END
