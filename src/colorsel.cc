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

#include <tau/colorsel.hh>
#include <colorsel-impl.hh>

namespace tau {

#define COLORSEL_IMPL (std::static_pointer_cast<Colorsel_impl>(impl))

Colorsel::Colorsel(const Color & initial_color):
    Widget(std::make_shared<Colorsel_impl>(initial_color))
{
}

void Colorsel::set_color(const Color & color) {
    COLORSEL_IMPL->set_color(color);
}

Color Colorsel::color() const {
    return COLORSEL_IMPL->color();
}

signal<void()> & Colorsel::signal_apply() {
    return COLORSEL_IMPL->signal_apply();
}

signal<void()> & Colorsel::signal_cancel() {
    return COLORSEL_IMPL->signal_cancel();
}

signal<void(Color)> & Colorsel::signal_color_changed() {
    return COLORSEL_IMPL->signal_color_changed();
}

} // namespace tau

//END
