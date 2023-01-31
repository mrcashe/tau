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

#include <tau/check.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <check-impl.hh>

namespace tau {

#define CHECK_IMPL (std::static_pointer_cast<Check_impl>(impl))

Check::Check(bool checked):
    Widget(std::make_shared<Check_impl>(checked))
{
}

Check::Check(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Check_impl>(wp))
{
}

Check & Check::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Check_impl>(wp)) {
        throw user_error(str_format(this, " Check::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

Check::Check(Check_style check_style, bool checked):
    Widget(std::make_shared<Check_impl>(check_style, checked))
{
}

Check::Check(Border_style border_style, bool checked):
    Widget(std::make_shared<Check_impl>(border_style, checked))
{
}

Check::Check(Check_style check_style, Border_style border_style, bool checked):
    Widget(std::make_shared<Check_impl>(check_style, border_style, checked))
{
}

void Check::set_check_style(Check_style check_style) {
    CHECK_IMPL->set_check_style(check_style);
}

Check_style Check::check_style() const {
    return CHECK_IMPL->check_style();
}

void Check::set_border_style(Border_style bs) {
    CHECK_IMPL->set_border_style(bs);
}

Border_style Check::border_style() const {
    return CHECK_IMPL->border_style();
}

void Check::set_border_width(unsigned npx) {
    CHECK_IMPL->set_border_width(npx);
}

unsigned Check::border_width() const {
    return CHECK_IMPL->border_width();
}

void Check::check() {
    CHECK_IMPL->check();
}

void Check::uncheck() {
    CHECK_IMPL->uncheck();
}

void Check::toggle() {
    CHECK_IMPL->toggle();
}

bool Check::checked() const {
    return CHECK_IMPL->checked();
}

void Check::join(Check & other) {
    CHECK_IMPL->join(std::static_pointer_cast<Check_impl>(other.impl));
}

bool Check::joined() const {
    return CHECK_IMPL->joined();
}

signal<void()> & Check::signal_check() {
    return CHECK_IMPL->signal_check();
}

signal<void()> & Check::signal_uncheck() {
    return CHECK_IMPL->signal_uncheck();
}

} // namespace tau

//END
