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

#include <tau/twins.hh>
#include <twins-impl.hh>

namespace tau {

#define TWINS_IMPL (std::static_pointer_cast<Twins_impl>(impl))

Twins::Twins(Orientation orient, double ratio):
    Container(std::make_shared<Twins_impl>(orient, ratio))
{
}

Twins::Twins(Widget_ptr wp):
    Container(std::dynamic_pointer_cast<Twins_impl>(wp))
{
}

void Twins::insert_first(Widget & w) {
    TWINS_IMPL->insert_first(w.ptr());
}

void Twins::insert_second(Widget & w) {
    TWINS_IMPL->insert_second(w.ptr());
}

bool Twins::has_first() const {
    return TWINS_IMPL->has_first();
}

bool Twins::has_second() const {
    return TWINS_IMPL->has_second();
}

bool Twins::empty() const {
    return TWINS_IMPL->empty();
}

void Twins::remove_first() {
    TWINS_IMPL->remove_first();
}

void Twins::remove_second() {
    TWINS_IMPL->remove_second();
}

void Twins::clear() {
    TWINS_IMPL->clear();
}

void Twins::set_ratio(double ratio) {
    TWINS_IMPL->set_ratio(ratio);
}

double Twins::ratio() const {
    return TWINS_IMPL->ratio();
}

signal<void(double)> & Twins::signal_ratio_changed() {
    return TWINS_IMPL->signal_ratio_changed();
}

} // namespace tau

//END
