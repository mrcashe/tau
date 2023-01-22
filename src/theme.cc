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

#include <tau/theme.hh>
#include <cursor-impl.hh>
#include <pixmap-impl.hh>
#include <theme-impl.hh>

namespace tau {

Theme::Theme():
    impl(Theme_impl::root())
{
}

void Theme::add_icon_dir(const ustring & dir) {
    impl->add_icon_dir(dir);
}

void Theme::add_pixmap_dir(const ustring & dir) {
    impl->add_pixmap_dir(dir);
}

void Theme::add_cursor_dir(const ustring & dir) {
    impl->add_cursor_dir(dir);
}

Cursor Theme::find_cursor(const ustring & names, int size) {
    return Cursor_impl::wrap(impl->find_cursor(names, size));
}

Pixmap Theme::find_pixmap(const ustring & names) {
    auto pix = impl->find_pixmap(names);
    return pix ? Pixmap_impl::wrap(pix->dup()) : Pixmap();
}

Pixmap Theme::find_icon(const ustring & names, int icon_size, const ustring & context) {
    auto icon = impl->find_icon(names, icon_size, context);
    return icon ? Pixmap_impl::wrap(icon->dup()) : Pixmap();
}

Pixmap Theme::get_icon(const ustring & names, int icon_size, const ustring & context) {
    return Pixmap_impl::wrap(impl->get_icon(names, icon_size, context));
}

std::vector<ustring> Theme::list_icon_themes() const {
    return impl->list_icon_themes();
}

std::vector<ustring> Theme::list_cursor_themes() const {
    return impl->list_cursor_themes();
}

void Theme::set_cursor_theme(const ustring & names) {
    impl->set_cursor_theme(names);
}

void Theme::set_icon_theme(const ustring & names) {
    impl->set_icon_theme(names);
}

ustring Theme::cursor_theme() const {
    return impl->cursor_theme();
}

ustring Theme::icon_theme() const {
    return impl->icon_theme();
}

Master_action * Theme::find_action(const ustring & name) {
    return impl->find_action(name);
}

signal<void()> & Theme::signal_cursors_changed() {
    return impl->signal_cursors_changed();
}

signal<void()> & Theme::signal_icons_changed() {
    return impl->signal_icons_changed();
}

} // namespace tau

//END
