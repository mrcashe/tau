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

#include <tau/exception.hh>
#include <tau/frame.hh>
#include <tau/painter.hh>
#include <frame-impl.hh>
#include <text-impl.hh>

namespace tau {

#define FRAME_IMPL (std::static_pointer_cast<Frame_impl>(impl))

Frame::Frame():
    Container(std::make_shared<Frame_impl>())
{
}

Frame::Frame(const ustring & label):
    Container(std::make_shared<Frame_impl>(label))
{
}

Frame::Frame(const ustring & label, Align align):
    Container(std::make_shared<Frame_impl>(label, align))
{
}

Frame::Frame(Border_style bs, unsigned border_width, int border_radius):
    Container(std::make_shared<Frame_impl>(bs, border_width, border_radius))
{
}

Frame::Frame(const ustring & label, Border_style bs, unsigned border_width, int border_radius):
    Container(std::make_shared<Frame_impl>(label, bs, border_width, border_radius))
{
}

Frame::Frame(const ustring & label, Align align, Border_style bs, unsigned border_width, int border_radius):
    Container(std::make_shared<Frame_impl>(label, align, bs, border_width, border_radius))
{
}

void Frame::insert(Widget & w) {
    FRAME_IMPL->insert(w.ptr());
}

void Frame::clear() {
    FRAME_IMPL->clear();
}

void Frame::set_label(const ustring & label, Align align) {
    FRAME_IMPL->set_label(label, align);
}

void Frame::set_label(Widget & w) {
    FRAME_IMPL->set_label(w.ptr());
}

void Frame::align_label(Align align) {
    FRAME_IMPL->align_label(align);
}

Align Frame::label_align() const {
    return FRAME_IMPL->label_align();
}

void Frame::set_label_position(Label_pos label_pos) {
    FRAME_IMPL->set_label_position(label_pos);
}

Label_pos Frame::label_position() const {
    return FRAME_IMPL->label_position();
}

unsigned Frame::border_left() const {
    return FRAME_IMPL->border_left();
}

unsigned Frame::border_right() const {
    return FRAME_IMPL->border_right();
}

unsigned Frame::border_top() const {
    return FRAME_IMPL->border_top();
}

void Frame::set_border(unsigned npx) {
    FRAME_IMPL->set_border(npx, npx, npx, npx);
}

void Frame::set_border(unsigned left, unsigned right, unsigned top, unsigned bottom) {
    FRAME_IMPL->set_border(left, right, top, bottom);
}

unsigned Frame::border_bottom() const {
    return FRAME_IMPL->border_bottom();
}

void Frame::set_border_left(unsigned npx) {
    FRAME_IMPL->set_border_left(npx);
}

void Frame::set_border_right(unsigned npx) {
    FRAME_IMPL->set_border_right(npx);
}

void Frame::set_border_top(unsigned npx) {
    FRAME_IMPL->set_border_top(npx);
}

void Frame::set_border_bottom(unsigned npx) {
    FRAME_IMPL->set_border_bottom(npx);
}

void Frame::set_border_left(unsigned px, Border_style bs) {
    FRAME_IMPL->set_border_left(px, bs);
}

void Frame::set_border_right(unsigned px, Border_style bs) {
    FRAME_IMPL->set_border_right(px, bs);
}

void Frame::set_border_top(unsigned px, Border_style bs) {
    FRAME_IMPL->set_border_top(px, bs);
}

void Frame::set_border_bottom(unsigned px, Border_style bs) {
    FRAME_IMPL->set_border_bottom(px, bs);
}

void Frame::set_border(unsigned px, Border_style bs) {
    FRAME_IMPL->set_border(px, bs);
}

void Frame::set_border_left(unsigned px, Border_style bs, const Color & color) {
    FRAME_IMPL->set_border_left(px, bs, color);
}

void Frame::set_border_right(unsigned px, Border_style bs, const Color & color) {
    FRAME_IMPL->set_border_right(px, bs, color);
}

void Frame::set_border_top(unsigned px, Border_style bs, const Color & color) {
    FRAME_IMPL->set_border_top(px, bs, color);
}

void Frame::set_border_bottom(unsigned px, Border_style bs, const Color & color) {
    FRAME_IMPL->set_border_bottom(px, bs, color);
}

void Frame::set_border(unsigned px, Border_style bs, const Color & color) {
    FRAME_IMPL->set_border(px, bs, color);
}

void Frame::set_border_left_style(Border_style bs) {
    FRAME_IMPL->set_border_left_style(bs);
}

void Frame::set_border_right_style(Border_style bs) {
    FRAME_IMPL->set_border_right_style(bs);
}

void Frame::set_border_top_style(Border_style bs) {
    FRAME_IMPL->set_border_top_style(bs);
}

void Frame::set_border_bottom_style(Border_style bs) {
    FRAME_IMPL->set_border_bottom_style(bs);
}

void Frame::set_border_style(Border_style bs) {
    FRAME_IMPL->set_border_style(bs);
}

void Frame::set_border_style(Border_style left, Border_style right, Border_style top, Border_style bottom) {
    FRAME_IMPL->set_border_style(left, right, top, bottom);
}

Border_style Frame::border_left_style() const {
    return FRAME_IMPL->border_left_style();
}

Border_style Frame::border_right_style() const {
    return FRAME_IMPL->border_right_style();
}

Border_style Frame::border_top_style() const {
    return FRAME_IMPL->border_top_style();
}

Border_style Frame::border_bottom_style() const {
    return FRAME_IMPL->border_bottom_style();
}

void Frame::set_border_color(const Color & left, const Color & right, const Color & top, const Color & bottom) {
    FRAME_IMPL->set_border_color(left, right, top, bottom);
}

void Frame::set_border_color(const Color & color) {
    FRAME_IMPL->set_border_color(color);
}

void Frame::set_border_left_color(const Color & color) {
    FRAME_IMPL->set_border_left_color(color);
}

void Frame::set_border_right_color(const Color & color) {
    FRAME_IMPL->set_border_right_color(color);
}

void Frame::set_border_top_color(const Color & color) {
    FRAME_IMPL->set_border_top_color(color);
}

void Frame::set_border_bottom_color(const Color & color) {
    FRAME_IMPL->set_border_bottom_color(color);
}

Color Frame::border_left_color() const {
    return FRAME_IMPL->border_left_color();
}

Color Frame::border_right_color() const {
    return FRAME_IMPL->border_right_color();
}

Color Frame::border_top_color() const {
    return FRAME_IMPL->border_top_color();
}

Color Frame::border_bottom_color() const {
    return FRAME_IMPL->border_bottom_color();
}

void Frame::unset_border_color() {
    FRAME_IMPL->unset_border_color();
}

void Frame::unset_border_left_color() {
    FRAME_IMPL->unset_border_left_color();
}

void Frame::unset_border_right_color() {
    FRAME_IMPL->unset_border_right_color();
}

void Frame::unset_border_top_color() {
    FRAME_IMPL->unset_border_top_color();
}

void Frame::unset_border_bottom_color() {
    FRAME_IMPL->unset_border_bottom_color();
}

void Frame::set_border_radius(int radius) {
    FRAME_IMPL->set_border_radius(radius);
}

int Frame::border_radius() const {
    return FRAME_IMPL->border_radius();
}

} // namespace tau

//END
