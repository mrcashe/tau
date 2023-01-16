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

#include <tau/button.hh>
#include <button-impl.hh>

namespace tau {

#define BUTTON_BASE_IMPL (std::static_pointer_cast<Button_base_impl>(impl))
#define BUTTON_IMPL (std::static_pointer_cast<Button_impl>(impl))
#define TOGGLE_IMPL (std::static_pointer_cast<Toggle_impl>(impl))

Button_base::Button_base(Widget_ptr wp):
    Widget(wp)
{
}

void Button_base::set_image(Widget & w) {
    BUTTON_BASE_IMPL->set_image(w.ptr());
}

void Button_base::set_label(const ustring & text) {
    BUTTON_BASE_IMPL->set_label(text);
}

void Button_base::set_icon(const ustring & icon_name, int icon_size) {
    BUTTON_BASE_IMPL->set_icon(icon_name, icon_size);
}

void Button_base::show_relief() {
    BUTTON_BASE_IMPL->show_relief();
}

void Button_base::hide_relief() {
    BUTTON_BASE_IMPL->hide_relief();
}

bool Button_base::relief_visible() const {
    return BUTTON_BASE_IMPL->relief_visible();
}

Button::Button():
    Button_base(std::make_shared<Button_impl>())
{
}

Button::Button(const ustring & label):
    Button_base(std::make_shared<Button_impl>(label))
{
}

Button::Button(Widget & img):
    Button_base(std::make_shared<Button_impl>(img.ptr()))
{
}

Button::Button(Widget & img, const ustring & label):
    Button_base(std::make_shared<Button_impl>(img.ptr(), label))
{
}

Button::Button(const ustring & icon_name, int icon_size):
    Button_base(std::make_shared<Button_impl>(icon_name, icon_size))
{
}

Button::Button(const ustring & label, const ustring & icon_name, int icon_size):
    Button_base(std::make_shared<Button_impl>(label, icon_name, icon_size))
{
}

Button::Button(Action & action, Action_items items):
    Button_base(std::make_shared<Button_impl>(action, items))
{
}

Button::Button(Action & action, int icon_size, Action_items items):
    Button_base(std::make_shared<Button_impl>(action, icon_size, items))
{
}

void Button::enable_repeat() {
    BUTTON_IMPL->enable_repeat();
}

void Button::disable_repeat() {
    BUTTON_IMPL->disable_repeat();
}

bool Button::repeat_enabled() const {
    return BUTTON_IMPL->repeat_enabled();
}

void Button::set_repeat_delay(unsigned first, unsigned next) {
    BUTTON_IMPL->set_repeat_delay(first, next);
}

unsigned Button::repeat_delay() const {
    return BUTTON_IMPL->repeat_delay();
}

unsigned Button::repeat_interval() const {
    return BUTTON_IMPL->repeat_interval();
}

signal<void()> & Button::signal_click() {
    return BUTTON_IMPL->signal_click();
}

Toggle::Toggle():
    Button_base(std::make_shared<Toggle_impl>())
{
}

Toggle::Toggle(const ustring & label):
    Button_base(std::make_shared<Toggle_impl>(label))
{
}

Toggle::Toggle(Widget & img):
    Button_base(std::make_shared<Toggle_impl>(img.ptr()))
{
}

Toggle::Toggle(Widget & img, const ustring & label):
    Button_base(std::make_shared<Toggle_impl>(img.ptr(), label))
{
}

Toggle::Toggle(const ustring & icon_name, int icon_size):
    Button_base(std::make_shared<Toggle_impl>(icon_name, icon_size))
{
}

Toggle::Toggle(const ustring & label, const ustring & icon_name, int icon_size):
    Button_base(std::make_shared<Toggle_impl>(label, icon_name, icon_size))
{
}

Toggle::Toggle(Toggle_action & action, Action_items items):
    Button_base(std::make_shared<Toggle_impl>(action, items))
{
}

Toggle::Toggle(Toggle_action & action, int icon_size, Action_items items):
    Button_base(std::make_shared<Toggle_impl>(action, icon_size, items))
{
}

void Toggle::toggle() {
    TOGGLE_IMPL->toggle();
}

bool Toggle::get() const {
    return TOGGLE_IMPL->get();
}

signal<void(bool)> & Toggle::signal_toggle() {
    return TOGGLE_IMPL->signal_toggle();
}

} // namespace tau

//END
