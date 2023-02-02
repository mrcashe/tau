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
#include <pixmap-impl.hh>
#include <theme-impl.hh>
#include <toplevel-impl.hh>

#include <iostream>

namespace tau {

Toplevel_impl::Toplevel_impl(Winface_ptr winface, const Rect & ubounds):
    Window_impl()
{
    hidden_ = true;
    winface_ = winface;
    position_ = ubounds.origin();
}

// Overrides Widget_impl.
void Toplevel_impl::set_parent(Container_impl * parent) {
    throw user_error("Toplevel_impl::set_parent(): impossible to insert Toplevel into Container");
}

void Toplevel_impl::set_title(const ustring & title) {
    winface_->set_title(title);
}

void Toplevel_impl::set_icon(Pixmap_ptr icon) {
    icon_theme_cx_.drop();
    winface_->set_icon(icon);
}

void Toplevel_impl::set_icon(const ustring & icon_name, unsigned icon_size) {
    if (auto pix = Theme_impl::root()->find_icon(icon_name, icon_size)) {
        winface_->set_icon(pix->dup());
        icon_name_ = icon_name;
        icon_size_ = icon_size;

        if (icon_theme_cx_.empty()) {
            icon_theme_cx_ = Theme_impl::root()->signal_icons_changed().connect(fun(this, &Toplevel_impl::on_icon_theme_changed));
        }
    }
}

void Toplevel_impl::on_icon_theme_changed() {
    if (auto pix = Theme_impl::root()->find_icon(icon_name_, icon_size_)) {
        winface_->set_icon(pix->dup());
    }
}

void Toplevel_impl::set_icon_from_file(const ustring & path) {
    try {
        if (auto pix = Pixmap_impl::load_from_file(path)) {
            icon_theme_cx_.drop();
            winface_->set_icon(pix);
        }
    }

    catch (exception & x) {
        std::cerr << "** Failed to set window icon from file " << path << ": " << x.what() << std::endl;
    }
}

// From window system: [x] button or Alt-F4 pressed.
void Toplevel_impl::handle_close() {
    if (!signal_about_close_()) { close(); }
}

void Toplevel_impl::handle_minimize() {
    if (!minimized_) {
        minimized_ = true;
        maximized_ = false;
        signal_minimize_();
    }
}

void Toplevel_impl::handle_maximize() {
    if (!maximized_) {
        minimized_ = false;
        maximized_ = true;
        signal_maximize_();
    }
}

void Toplevel_impl::handle_restore() {
    if (maximized_ || minimized_) {
        maximized_ = false;
        minimized_ = false;
        signal_restore_();
    }
}

void Toplevel_impl::handle_fullscreen(bool yes) {
    if (fullscreen_ != yes) {
        fullscreen_ = yes;
        signal_fullscreen_(yes);
    }
}

void Toplevel_impl::minimize() {
    winface_->minimize();
}

void Toplevel_impl::maximize() {
    winface_->maximize();
}

void Toplevel_impl::restore() {
    winface_->restore();
}

void Toplevel_impl::fullscreen() {
    winface_->set_fullscreen(true);
}

void Toplevel_impl::unfullscreen() {
    winface_->set_fullscreen(false);
}

void Toplevel_impl::show_frame() {
    frame_visible_ = true;
    winface_->show_frame(true);
}

void Toplevel_impl::hide_frame() {
    frame_visible_ = false;
    winface_->show_frame(false);
}

void Toplevel_impl::enable_maximize() {
    winface_->enable_maximize(true);
}

void Toplevel_impl::disable_maximize() {
    winface_->enable_maximize(false);
}

bool Toplevel_impl::maximize_enabled() const {
    return winface_->maximize_enabled();
}

void Toplevel_impl::enable_minimize() {
    winface_->enable_minimize(true);
}

void Toplevel_impl::disable_minimize() {
    winface_->enable_minimize(false);
}

bool Toplevel_impl::minimize_enabled() const {
    return winface_->minimize_enabled();
}

} // namespace tau

//END
