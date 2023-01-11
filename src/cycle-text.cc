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

#include <tau/cycle-text.hh>
#include <cycle-text-impl.hh>

namespace tau {

#define CYCLE_IMPL (std::static_pointer_cast<Cycle_impl>(impl))
#define CYCLE_TEXT_IMPL (std::static_pointer_cast<Cycle_text_impl>(impl))

Cycle_text::Cycle_text(Border_style bs):
    Widget(std::make_shared<Cycle_text_impl>(bs))
{
}

Cycle_text::Cycle_text(Border_style bs, Align align):
    Widget(std::make_shared<Cycle_text_impl>(bs, align))
{
}

Cycle_text::Cycle_text(Align align):
    Widget(std::make_shared<Cycle_text_impl>(align))
{
}

void Cycle_text::set_border_style(Border_style bs) {
    CYCLE_TEXT_IMPL->set_border_style(bs);
}

Border_style Cycle_text::border_style() const {
    return CYCLE_TEXT_IMPL->border_style();
}

void Cycle_text::set_text_align(Align align) {
    CYCLE_TEXT_IMPL->set_text_align(align);
}

Align Cycle_text::text_align() const {
    return CYCLE_TEXT_IMPL->text_align();
}

void Cycle_text::allow_edit() {
    CYCLE_TEXT_IMPL->allow_edit();
}

void Cycle_text::disallow_edit() {
    CYCLE_TEXT_IMPL->disallow_edit();
}

bool Cycle_text::edit_allowed() const {
    return CYCLE_TEXT_IMPL->edit_allowed();
}

void Cycle_text::add_text(const ustring & text, const ustring & tooltip) {
    CYCLE_TEXT_IMPL->add_text(text, tooltip);
}

void Cycle_text::select_text(const ustring & text) {
    CYCLE_TEXT_IMPL->select_text(text);
}

void Cycle_text::remove_text(const ustring & text) {
    CYCLE_TEXT_IMPL->remove_text(text);
}

void Cycle_text::clear() {
    CYCLE_TEXT_IMPL->clear();
}

bool Cycle_text::empty() const {
    return CYCLE_TEXT_IMPL->empty();
}

ustring Cycle_text::selection() const {
    return CYCLE_TEXT_IMPL->selection();
}

void Cycle_text::append(Widget & w, bool shrink) {
    CYCLE_TEXT_IMPL->append(w.ptr(), shrink);
}

void Cycle_text::append(const ustring & text, unsigned margin_left, unsigned margin_right) {
    CYCLE_TEXT_IMPL->append(text, margin_left, margin_right);
}

void Cycle_text::prepend(Widget & w, bool shrink) {
    CYCLE_TEXT_IMPL->prepend(w.ptr(), shrink);
}

void Cycle_text::prepend(const ustring & text, unsigned margin_left, unsigned margin_right) {
    CYCLE_TEXT_IMPL->prepend(text, margin_left, margin_right);
}

void Cycle_text::remove(Widget & w) {
    CYCLE_IMPL->remove(w.ptr());
}

signal<void(ustring)> & Cycle_text::signal_selection_changed() {
    return CYCLE_TEXT_IMPL->signal_selection_changed();
}

} // namespace tau

//END
