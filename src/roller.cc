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

#include <tau/roller.hh>
#include <roller-impl.hh>

namespace tau {

#define ROLLER_IMPL (std::static_pointer_cast<Roller_impl>(impl))

Roller::Roller(Orientation orient, bool autohide):
    Widget(std::make_shared<Roller_impl>(orient, autohide))
{
}

Roller::Roller(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Roller_impl>(wp))
{
}

void Roller::insert(Widget & w) {
    ROLLER_IMPL->insert(w.ptr());
}

void Roller::clear() {
    ROLLER_IMPL->clear();
}

void Roller::roll_to(int pos) {
    ROLLER_IMPL->roll_to(pos);
}

int Roller::offset() const {
    return ROLLER_IMPL->offset();
}

void Roller::set_autohide() {
    ROLLER_IMPL->set_autohide();
}

void Roller::unset_autohide() {
    ROLLER_IMPL->unset_autohide();
}

bool Roller::autohide() const {
    return ROLLER_IMPL->autohide();
}

void Roller::set_step(int step) {
    ROLLER_IMPL->set_step(step);
}

int Roller::step() const {
    return ROLLER_IMPL->step();
}

void Roller::set_start(Widget & w) {
    ROLLER_IMPL->set_start(w.ptr());
}

void Roller::set_end(Widget & w) {
    ROLLER_IMPL->set_end(w.ptr());
}

} // namespace tau

//END
