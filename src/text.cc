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

#include <tau/text.hh>
#include <text-impl.hh>
#include <iostream>

namespace tau {

#define TEXT_IMPL (std::static_pointer_cast<Text_impl>(impl))

Text::Text(Widget_ptr wp):
    Widget(wp)
{
}

Text::Text():
    Widget(std::make_shared<Text_impl>())
{
}

Text::Text(Align halign, Align valign):
    Widget(std::make_shared<Text_impl>(halign, valign))
{
}

Text::Text(const ustring & text, Align halign, Align valign):
    Widget(std::make_shared<Text_impl>(text, halign, valign))
{
}

Text::Text(Buffer buf, Align halign, Align valign):
    Widget(std::make_shared<Text_impl>(buf, halign, valign))
{
}

void Text::assign(Buffer buf) {
    TEXT_IMPL->assign(buf);
}

Buffer Text::buffer() {
    return TEXT_IMPL->buffer();
}

bool Text::empty() const {
    return TEXT_IMPL->empty();
}

const Buffer Text::buffer() const {
    return TEXT_IMPL->buffer();
}

std::size_t Text::lines() const {
    return TEXT_IMPL->lines();
}

void Text::assign(const ustring & s) {
    TEXT_IMPL->assign(s);
}

ustring Text::text() const {
    return TEXT_IMPL->text();
}

void Text::clear() {
    TEXT_IMPL->clear();
}

Size Text::text_size(const ustring & s) {
    return TEXT_IMPL->text_size(s);
}

Size Text::text_size(const std::u32string & s) {
    return TEXT_IMPL->text_size(s);
}

void Text::allow_select() {
    TEXT_IMPL->allow_select();
}

void Text::disallow_select() {
    TEXT_IMPL->disallow_select();
}

bool Text::select_allowed() const {
    return TEXT_IMPL->select_allowed();
}

bool Text::has_selection() const {
    return TEXT_IMPL->has_selection();
}

ustring Text::selection() const {
    return TEXT_IMPL->selection();
}

void Text::select(Buffer_citer b, Buffer_citer e) {
    TEXT_IMPL->select(b, e);
}

void Text::select_all() {
    TEXT_IMPL->select_all();
}

void Text::unselect() {
    TEXT_IMPL->unselect();
}

void Text::set_spacing(unsigned spc) {
    TEXT_IMPL->set_spacing(spc);
}

unsigned Text::spacing() const {
    return TEXT_IMPL->spacing();
}

void Text::set_text_align(Align halign, Align valign) {
    TEXT_IMPL->set_text_align(halign, valign);
}

Align Text::horizontal_text_align() const {
    return TEXT_IMPL->horizontal_text_align();
}

Align Text::vertical_text_align() const {
    return TEXT_IMPL->vertical_text_align();
}

void Text::set_wrap_mode(Wrap_mode wrap_mode) {
    TEXT_IMPL->set_wrap_mode(wrap_mode);
}

Wrap_mode Text::wrap_mode() const {
    return TEXT_IMPL->wrap_mode();
}

void Text::move_to(const Buffer_citer pos) {
    TEXT_IMPL->move_to(pos);
}

void Text::move_to(std::size_t ln, std::size_t pos) {
    TEXT_IMPL->move_to(ln, pos);
}

Buffer_citer Text::caret() const {
    return TEXT_IMPL->caret();
}

Buffer_citer Text::iter(std::size_t row, std::size_t col) const {
    return TEXT_IMPL->iter(row, col);
}

void Text::enable_caret() {
    TEXT_IMPL->enable_caret();
}

void Text::disable_caret() {
    TEXT_IMPL->disable_caret();
}

bool Text::caret_enabled() const {
    return TEXT_IMPL->caret_enabled();
}

int Text::x_at_col(std::size_t ri, std::size_t col) const {
    return TEXT_IMPL->x_at_col(ri, col);
}

std::size_t Text::col_at_x(std::size_t ri, int x) const {
    return TEXT_IMPL->col_at_x(ri, x);
}

std::size_t Text::row_at_y(int y) const {
    return TEXT_IMPL->row_at_y(y);
}

int Text::baseline(std::size_t ri) const {
    return TEXT_IMPL->baseline(ri);
}

void Text::get_row_bounds(std::size_t ri, int & top, int & bottom) const {
    TEXT_IMPL->get_row_bounds(ri, top, bottom);
}

Action & Text::move_left_action() {
    return TEXT_IMPL->move_left_action();
}

Action & Text::select_left_action() {
    return TEXT_IMPL->select_left_action();
}

Action & Text::move_right_action() {
    return TEXT_IMPL->move_right_action();
}

Action & Text::select_right_action() {
    return TEXT_IMPL->select_right_action();
}

Action & Text::move_up_action() {
    return TEXT_IMPL->move_up_action();
}

Action & Text::select_up_action() {
    return TEXT_IMPL->select_up_action();
}

Action & Text::move_down_action() {
    return TEXT_IMPL->move_down_action();
}

Action & Text::select_down_action() {
    return TEXT_IMPL->select_down_action();
}

Action & Text::move_word_left_action() {
    return TEXT_IMPL->move_word_left_action();
}

Action & Text::select_word_left_action() {
    return TEXT_IMPL->select_word_left_action();
}

Action & Text::move_word_right_action() {
    return TEXT_IMPL->move_word_right_action();
}

Action & Text::select_word_right_action() {
    return TEXT_IMPL->select_word_right_action();
}

Action & Text::move_home_action() {
    return TEXT_IMPL->move_home_action();
}

Action & Text::select_home_action() {
    return TEXT_IMPL->select_home_action();
}

Action & Text::move_to_eol_action() {
    return TEXT_IMPL->move_to_eol_action();
}

Action & Text::select_to_eol_action() {
    return TEXT_IMPL->select_to_eol_action();
}

Action & Text::move_to_sof_action() {
    return TEXT_IMPL->move_to_sof_action();
}

Action & Text::select_to_sof_action() {
    return TEXT_IMPL->select_to_sof_action();
}

Action & Text::move_to_eof_action() {
    return TEXT_IMPL->move_to_eof_action();
}

Action & Text::select_to_eof_action() {
    return TEXT_IMPL->select_to_eof_action();
}

Action & Text::move_page_up_action() {
    return TEXT_IMPL->move_page_up_action();
}

Action & Text::move_page_down_action() {
    return TEXT_IMPL->move_page_down_action();
}

Action & Text::select_page_up_action() {
    return TEXT_IMPL->select_page_up_action();
}

Action & Text::select_page_down_action() {
    return TEXT_IMPL->select_page_down_action();
}

Action & Text::select_all_action() {
    return TEXT_IMPL->select_all_action();
}

Action & Text::copy_action() {
    return TEXT_IMPL->copy_action();
}

Action & Text::cancel_action() {
    return TEXT_IMPL->cancel_action();
}

signal<void()> & Text::signal_caret_motion() {
    return TEXT_IMPL->signal_caret_motion();
}

signal<void()> & Text::signal_selection_changed() {
    return TEXT_IMPL->signal_selection_changed();
}

signal<void()> & Text::signal_click() {
    return TEXT_IMPL->signal_click();
}

} // namespace tau

//END
