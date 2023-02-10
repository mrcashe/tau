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

#include <tau/edit.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <edit-impl.hh>

namespace tau {

#define EDIT_IMPL (std::static_pointer_cast<Edit_impl>(impl))

Edit::Edit():
    Text(std::make_shared<Edit_impl>())
{
}

Edit::Edit(Widget_ptr wp):
    Text(std::dynamic_pointer_cast<Edit_impl>(wp))
{
}

Edit & Edit::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Edit_impl>(wp)) {
        throw user_error(str_format(this, " Edit::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

Edit::Edit(Align halign, Align valign):
    Text(std::make_shared<Edit_impl>(halign, valign))
{
}

Edit::Edit(const ustring & s, Align halign, Align valign):
    Text(std::make_shared<Edit_impl>(s, halign, valign))
{
}

Edit::Edit(Buffer buf, Align halign, Align valign):
    Text(std::make_shared<Edit_impl>(buf, halign, valign))
{
}

void Edit::allow_edit() {
    EDIT_IMPL->allow_edit();
}

void Edit::disallow_edit() {
    EDIT_IMPL->disallow_edit();
}

bool Edit::edit_allowed() const {
    return EDIT_IMPL->edit_allowed();
}

void Edit::enter_text(const ustring & s) {
    EDIT_IMPL->enter_text(s);
}

bool Edit::modified() const {
    return EDIT_IMPL->modified();
}

Action & Edit::cut_action() {
    return EDIT_IMPL->cut_action();
}

Action & Edit::enter_action() {
    return EDIT_IMPL->enter_action();
}

Action & Edit::del_action() {
    return EDIT_IMPL->del_action();
}

Action & Edit::backspace_action() {
    return EDIT_IMPL->backspace_action();
}

Action & Edit::paste_action() {
    return EDIT_IMPL->paste_action();
}

Action & Edit::undo_action() {
    return EDIT_IMPL->undo_action();
}

Action & Edit::redo_action() {
    return EDIT_IMPL->redo_action();
}

Action & Edit::tab_action() {
    return EDIT_IMPL->tab_action();
}

Toggle_action & Edit::insert_action() {
    return EDIT_IMPL->insert_action();
}

signal<void(bool)> & Edit::signal_modified() {
        return EDIT_IMPL->signal_modified();
}

} // namespace tau

//END
