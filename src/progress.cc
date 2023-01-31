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

#include <tau/exception.hh>
#include <tau/progress.hh>
#include <tau/string.hh>
#include <progress-impl.hh>

namespace tau {

#define PROGRESS_IMPL (std::static_pointer_cast<Progress_impl>(impl))

Progress::Progress(bool vertical):
    Widget(std::make_shared<Progress_impl>(vertical))
{
}

Progress::Progress(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Progress_impl>(wp))
{
}

Progress & Progress::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Progress_impl>(wp)) {
        throw user_error(str_format(this, " Progress::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

void Progress::set_border_style(Border_style bs) {
    PROGRESS_IMPL->set_border_style(bs);
}

Border_style Progress::border_style() const {
    return PROGRESS_IMPL->border_style();
}

void Progress::set_border_color(const Color & color) {
    PROGRESS_IMPL->set_border_color(color);
}

void Progress::unset_border_color() {
    PROGRESS_IMPL->unset_border_color();
}

Color Progress::border_color() const {
    return PROGRESS_IMPL->border_color();
}

void Progress::set_border(unsigned npx) {
    PROGRESS_IMPL->set_border(npx);
}

unsigned Progress::border() const {
    return PROGRESS_IMPL->border();
}

void Progress::set_text_align(Align align) {
    PROGRESS_IMPL->set_text_align(align);
}

Align Progress::text_align() const {
    return PROGRESS_IMPL->text_align();
}

void Progress::set_value(double value) {
    PROGRESS_IMPL->set_value(value);
}

double Progress::value() const {
    return PROGRESS_IMPL->value();
}

void Progress::set_min_value(double min_value) {
    PROGRESS_IMPL->set_min_value(min_value);
}

double Progress::min_value() const {
    return PROGRESS_IMPL->min_value();
}

void Progress::set_max_value(double max_value) {
    PROGRESS_IMPL->set_max_value(max_value);
}

double Progress::max_value() const {
    return PROGRESS_IMPL->max_value();
}

void Progress::set_precision(int prec) {
    PROGRESS_IMPL->set_precision(prec);
}

int Progress::precision() const {
    return PROGRESS_IMPL->precision();
}

void Progress::set_format(const ustring & fmt) {
    PROGRESS_IMPL->set_format(fmt);
}

ustring Progress::format() const {
    return PROGRESS_IMPL->format();
}

} // namespace tau

//END
