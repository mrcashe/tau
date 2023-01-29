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

#include <tau/scroller.hh>
#include <tau/slider.hh>
#include <scroller-impl.hh>
#include <slider-impl.hh>

namespace tau {

#define SLIDER_IMPL (std::static_pointer_cast<Slider_impl>(impl))

Slider::Slider():
    Widget(std::make_shared<Slider_impl>())
{
}

Slider::Slider(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Slider_impl>(wp))
{
}

Slider::Slider(Orientation orient, bool autohide):
    Widget(std::make_shared<Slider_impl>(orient, autohide))
{
}

Slider::Slider(Scroller & scroller):
    Widget(std::make_shared<Slider_impl>(std::static_pointer_cast<Scroller_impl>(scroller.ptr())))
{
}

Slider::Slider(Scroller & scroller, Orientation orient, bool autohide):
    Widget(std::make_shared<Slider_impl>(std::static_pointer_cast<Scroller_impl>(scroller.ptr()), orient, autohide))
{
}

void Slider::set_location(double loc) {
    SLIDER_IMPL->set_location(loc);
}

double Slider::location() const {
    return SLIDER_IMPL->location();
}

void Slider::set_range(double range) {
    SLIDER_IMPL->set_range(range);
}

double Slider::range() const {
    return SLIDER_IMPL->range();
}

void Slider::set(double loc, double range) {
    SLIDER_IMPL->set(loc, range);
}

Orientation Slider::orientation() const {
    return SLIDER_IMPL->orientation();
}

void Slider::set_orientation(Orientation orient) {
    SLIDER_IMPL->set_orientation(orient);
}

void Slider::show_buttons() {
    SLIDER_IMPL->show_buttons();
}

void Slider::hide_buttons() {
    SLIDER_IMPL->hide_buttons();
}

bool Slider::buttons_visible() const {
    return SLIDER_IMPL->buttons_visible();
}

signal<void(double)> & Slider::signal_location_changed() {
    return SLIDER_IMPL->signal_location_changed();
}

} // namespace tau

//END
