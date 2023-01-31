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

#include <tau/fontsel.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <fontsel-impl.hh>

#define FONTSEL_IMPL (std::static_pointer_cast<Fontsel_impl>(impl))

namespace tau {

Fontsel::Fontsel():
    Widget(std::make_shared<Fontsel_impl>())
{
}

Fontsel::Fontsel(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Fontsel_impl>(wp))
{
}

Fontsel & Fontsel::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Fontsel_impl>(wp)) {
        throw user_error(str_format(this, " Fontsel::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

Fontsel::Fontsel(const ustring & spec, const ustring & sample):
    Widget(std::make_shared<Fontsel_impl>(spec, sample))
{
}

void Fontsel::select(const ustring & spec) {
    FONTSEL_IMPL->select(spec);
}

void Fontsel::set_sample(const ustring & sample) {
    FONTSEL_IMPL->set_sample(sample);
}

ustring Fontsel::spec() const {
    return FONTSEL_IMPL->spec();
}

ustring Fontsel::sample() const {
    return FONTSEL_IMPL->sample();
}

Action & Fontsel::apply_action() {
    return FONTSEL_IMPL->apply_action();
}

const Action & Fontsel::apply_action() const {
    return FONTSEL_IMPL->apply_action();
}

Action & Fontsel::cancel_action() {
    return FONTSEL_IMPL->cancel_action();
}

const Action & Fontsel::cancel_action() const {
    return FONTSEL_IMPL->cancel_action();
}

Action & Fontsel::focus_next_action() {
    return FONTSEL_IMPL->focus_next_action();
}

const Action & Fontsel::focus_next_action() const {
    return FONTSEL_IMPL->focus_next_action();
}

Action & Fontsel::focus_previous_action() {
    return FONTSEL_IMPL->focus_previous_action();
}

const Action & Fontsel::focus_previous_action() const {
    return FONTSEL_IMPL->focus_previous_action();
}

signal<void(const ustring &)> & Fontsel::signal_selection_changed() {
    return FONTSEL_IMPL->signal_selection_changed();
}

signal<void(const ustring &)> & Fontsel::signal_font_activated() {
    return FONTSEL_IMPL->signal_font_activated();
}

} // namespace tau

//END
