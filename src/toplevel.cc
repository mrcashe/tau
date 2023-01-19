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

#include <tau/toplevel.hh>
#include <display-impl.hh>
#include <pixmap-impl.hh>
#include <toplevel-impl.hh>

namespace tau {

#define TOPLEVEL_IMPL (std::static_pointer_cast<Toplevel_impl>(impl))

Toplevel::Toplevel(const Rect & bounds):
    Window(nullptr)
{
    auto dp = Display_impl::this_display();
    impl = dp->create_toplevel(dp, bounds);
}

Toplevel::Toplevel(const ustring & title, const Rect & bounds):
    Window(nullptr)
{
    auto dp = Display_impl::this_display();
    impl = dp->create_toplevel(dp, bounds);
    TOPLEVEL_IMPL->set_title(title);
}

Toplevel::Toplevel(Widget_ptr wp):
    Window(std::dynamic_pointer_cast<Window_impl>(wp))
{
}

Toplevel::Toplevel(std::nullptr_t):
    Window(nullptr)
{
}

void Toplevel::set_title(const ustring & title) {
    TOPLEVEL_IMPL->set_title(title);
}

void Toplevel::set_icon(Pixmap pix) {
    TOPLEVEL_IMPL->set_icon(Pixmap_impl::strip(pix));
}

void Toplevel::set_icon(const ustring & icon_name, int icon_size) {
    TOPLEVEL_IMPL->set_icon(icon_name, icon_size);
}

void Toplevel::set_icon_from_file(const ustring & path) {
    TOPLEVEL_IMPL->set_icon_from_file(path);
}

void Toplevel::close() {
    TOPLEVEL_IMPL->close();
}

bool Toplevel::minimized() const {
    return TOPLEVEL_IMPL->minimized();
}

bool Toplevel::maximized() const {
    return TOPLEVEL_IMPL->maximized();
}

void Toplevel::fullscreen() {
    TOPLEVEL_IMPL->fullscreen();
}

void Toplevel::unfullscreen() {
    TOPLEVEL_IMPL->unfullscreen();
}

bool Toplevel::fullscreened() const {
    return TOPLEVEL_IMPL->fullscreened();
}

void Toplevel::maximize() {
    TOPLEVEL_IMPL->maximize();
}

void Toplevel::minimize() {
    TOPLEVEL_IMPL->minimize();
}

void Toplevel::restore() {
    TOPLEVEL_IMPL->restore();
}

void Toplevel::show_frame() {
    TOPLEVEL_IMPL->show_frame();
}

void Toplevel::hide_frame() {
    TOPLEVEL_IMPL->hide_frame();
}

bool Toplevel::frame_visible() const {
    return TOPLEVEL_IMPL->frame_visible();
}

void Toplevel::enable_maximize() {
    TOPLEVEL_IMPL->enable_maximize();
}

void Toplevel::disable_maximize() {
    TOPLEVEL_IMPL->disable_maximize();
}

bool Toplevel::maximize_enabled() const {
    return TOPLEVEL_IMPL->maximize_enabled();
}

void Toplevel::enable_minimize() {
    TOPLEVEL_IMPL->enable_minimize();
}

void Toplevel::disable_minimize() {
    TOPLEVEL_IMPL->disable_minimize();
}

bool Toplevel::minimize_enabled() const {
    return TOPLEVEL_IMPL->minimize_enabled();
}

signal<bool()> & Toplevel::signal_about_close() {
    return TOPLEVEL_IMPL->signal_about_close();
}

signal<void()> & Toplevel::signal_minimize() {
    return TOPLEVEL_IMPL->signal_minimize();
}

signal<void()> & Toplevel::signal_maximize() {
    return TOPLEVEL_IMPL->signal_maximize();
}

signal<void()> & Toplevel::signal_restore() {
    return TOPLEVEL_IMPL->signal_restore();
}

signal<bool()> & Toplevel::signal_menu() {
    return TOPLEVEL_IMPL->signal_menu();
}

signal<bool()> & Toplevel::signal_help() {
    return TOPLEVEL_IMPL->signal_help();
}

signal<void(bool)> & Toplevel::signal_fullscreen() {
    return TOPLEVEL_IMPL->signal_fullscreen();
}

} // namespace tau

//END
