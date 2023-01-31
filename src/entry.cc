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

#include <tau/entry.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <entry-impl.hh>

namespace tau {

#define ENTRY_IMPL (std::static_pointer_cast<Entry_impl>(impl))

Entry::Entry(Border_style border_style):
    Widget(std::make_shared<Entry_impl>(border_style))
{
}

Entry::Entry(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Entry_impl>(wp))
{
}

Entry & Entry::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Entry_impl>(wp)) {
        throw user_error(str_format(this, " Entry::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

Entry::Entry(Align text_align, Border_style border_style):
    Widget(std::make_shared<Entry_impl>(text_align, border_style))
{
}

Entry::Entry(const ustring & s, Border_style border_style):
    Widget(std::make_shared<Entry_impl>(s, border_style))
{
}

Entry::Entry(const ustring & s, Align text_align, Border_style border_style):
    Widget(std::make_shared<Entry_impl>(s, text_align, border_style))
{
}

void Entry::allow_edit() {
    ENTRY_IMPL->allow_edit();
}

void Entry::disallow_edit() {
    ENTRY_IMPL->disallow_edit();
}

bool Entry::edit_allowed() const {
    return ENTRY_IMPL->edit_allowed();
}

void Entry::set_text_align(Align align) {
    ENTRY_IMPL->set_text_align(align);
}

Align Entry::text_align() const {
    return ENTRY_IMPL->text_align();
}

void Entry::assign(const ustring & s) {
    ENTRY_IMPL->assign(s);
}

ustring Entry::text() const {
    return ENTRY_IMPL->text();
}

Size Entry::text_size(const ustring & s) {
    return ENTRY_IMPL->text_size(s);
}

const Buffer & Entry::buffer() const {
    return ENTRY_IMPL->buffer();
}

bool Entry::empty() const {
    return ENTRY_IMPL->empty();
}

void Entry::clear() {
    ENTRY_IMPL->clear();
}

void Entry::select_all() {
    ENTRY_IMPL->select_all();
}

void Entry::select(Buffer_citer b, Buffer_citer e) {
    ENTRY_IMPL->select(b, e);
}

bool Entry::has_selection() const {
    return ENTRY_IMPL->has_selection();
}

void Entry::unselect() {
    ENTRY_IMPL->unselect();
}

void Entry::move_to(const Buffer_citer & pos) {
    ENTRY_IMPL->move_to(pos);
}

void Entry::move_to(std::size_t row, std::size_t col) {
    ENTRY_IMPL->move_to(row, col);
}

Buffer_citer Entry::caret() const {
    return ENTRY_IMPL->caret();
}

void Entry::append(Widget & w, bool shrink) {
    ENTRY_IMPL->append(w.ptr(), shrink);
}

Widget_ptr Entry::append(const ustring & text, unsigned margin_left, unsigned margin_right) {
    return ENTRY_IMPL->append(text, margin_left, margin_right);
}

void Entry::prepend(Widget & w, bool shrink) {
    ENTRY_IMPL->prepend(w.ptr(), shrink);
}

Widget_ptr Entry::prepend(const ustring & text, unsigned margin_left, unsigned margin_right) {
    return ENTRY_IMPL->prepend(text, margin_left, margin_right);
}

Action & Entry::cancel_action() {
    return ENTRY_IMPL->cancel_action();
}

const Action & Entry::cancel_action() const {
    return ENTRY_IMPL->cancel_action();
}

signal<void(const ustring &)> & Entry::signal_changed() {
    return ENTRY_IMPL->signal_changed();
}

signal<void(const ustring &)> & Entry::signal_activate() {
    return ENTRY_IMPL->signal_activate();
}

signal<bool(const ustring &)> & Entry::signal_validate() {
    return ENTRY_IMPL->signal_validate();
}

} // namespace tau

//END
