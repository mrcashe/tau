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

#include <tau/fileman.hh>
#include <fileman-impl.hh>

namespace tau {

#define FILEMAN_IMPL (std::static_pointer_cast<Fileman_impl>(impl))

Fileman::Fileman(Fileman_mode mode, const ustring & path):
    Widget(Fileman_impl::create(mode, path))
{
}

Fileman::Fileman(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Fileman_impl>(wp))
{
}

Widget_ptr Fileman::navigator_ptr() {
    return FILEMAN_IMPL->navigator();
}

ustring Fileman::uri() const {
    return FILEMAN_IMPL->uri();
}

void Fileman::set_uri(const ustring & uri) {
    FILEMAN_IMPL->set_uri(uri);
}

std::vector<ustring> Fileman::selection() const {
    return FILEMAN_IMPL->selection();
}

ustring Fileman::entry() const {
    return FILEMAN_IMPL->entry();
}

void Fileman::add_filter(const ustring & patterns, const ustring & title) {
    FILEMAN_IMPL->add_filter(patterns, title);
}

ustring Fileman::filter() const {
    return FILEMAN_IMPL->filter();
}

void Fileman::show_info(const ustring & items, char32_t sep) {
    FILEMAN_IMPL->show_info(items, sep);
}

void Fileman::hide_info(const ustring & items, char32_t sep) {
    FILEMAN_IMPL->hide_info(items, sep);
}

bool Fileman::info_visible(const ustring & item) const {
    return FILEMAN_IMPL->info_visible(item);
}

ustring Fileman::visible_info_items(char32_t sep) const {
    return FILEMAN_IMPL->visible_info_items(sep);
}

ustring Fileman::invisible_info_items(char32_t sep) const {
    return FILEMAN_IMPL->invisible_info_items(sep);
}

void Fileman::allow_overwrite() {
    FILEMAN_IMPL->allow_overwrite();
}

void Fileman::disallow_overwrite() {
    FILEMAN_IMPL->disallow_overwrite();
}

bool Fileman::overwrite_allowed() const {
    return FILEMAN_IMPL->overwrite_allowed();
}

void Fileman::load_state(Key_file & kf, Key_section & sect) {
    FILEMAN_IMPL->load_state(kf, sect);
}

void Fileman::save_state(Key_file & kf, Key_section & sect) {
    FILEMAN_IMPL->save_state(kf, sect);
}

Action & Fileman::apply_action() {
    return FILEMAN_IMPL->apply_action();
}

const Action & Fileman::apply_action() const {
    return FILEMAN_IMPL->apply_action();
}

Action & Fileman::cancel_action() {
    return FILEMAN_IMPL->cancel_action();
}

const Action & Fileman::cancel_action() const {
    return FILEMAN_IMPL->cancel_action();
}

} // namespace tau

//END
