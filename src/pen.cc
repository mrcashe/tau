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

#include <pen-impl.hh>

namespace tau {

Pen::Pen():
    impl(std::make_shared<Pen_impl>())
{
}

Pen::~Pen() {}

Pen::Pen(Pen_ptr pp):
    impl(pp)
{
}

Pen::Pen(const Color & color):
    impl(std::make_shared<Pen_impl>())
{
    set_color(color);
}

Pen::Pen(const Color & color, double width, Line_style ls, Cap_style caps, Join_style js):
    impl(std::make_shared<Pen_impl>())
{
    impl->color = color;
    impl->line_width = width;
    impl->line_style = ls;
    impl->cap_style = caps;
    impl->join_style = js;
    impl->cosmetic = false;
}

void Pen::set_color(const Color & color) {
    impl->color = color;
}

Color Pen::color() const {
    return impl->color;
}

Line_style Pen::line_style() const {
    return impl->line_style;
}

void Pen::set_line_style(Line_style ls) {
    impl->line_style = ls;
}

double Pen::line_width() const {
    return impl->line_width;
}

void Pen::set_line_width(double width) {
    impl->line_width = width;
}

Cap_style Pen::cap_style() const {
    return impl->cap_style;
}

void Pen::set_cap_style(Cap_style caps) {
    impl->cap_style = caps;
}

Join_style Pen::join_style() const {
    return impl->join_style;
}

void Pen::set_join_style(Join_style js) {
    impl->join_style = js;
}

void Pen::set_dash_pattern(const std::vector<double> & dashes) {
    impl->dashes = dashes;
}

std::vector<double> Pen::dash_pattern() const {
    return impl->dashes;
}

void Pen::set_dash_offset(double offset) {
    impl->dash_offset = offset;
}

double Pen::dash_offset() const {
    return impl->dash_offset;
}

void Pen::set_miter_limit(double limit) {
    impl->miter_limit = limit;
}

double Pen::miter_limit() const {
    return impl->miter_limit;
}

} // namespace tau

//END
