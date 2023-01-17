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

#include <tau/action.hh>
#include <tau/brush.hh>
#include <tau/painter.hh>
#include <icon-impl.hh>
#include <theme-impl.hh>

namespace tau {

Icon_impl::Icon_impl():
    Image_impl()
{
    init();
    set_transparent();
}

Icon_impl::Icon_impl(const ustring & icon_name, unsigned icon_size):
    Image_impl(),
    icon_name_(icon_name),
    icon_size_(icon_size)
{
    init();
    set_transparent();
}

Icon_impl::Icon_impl(Action_base & action, unsigned icon_size):
    Image_impl()
{
    icon_size_ = icon_size;
    init();
    set_transparent();
    init_action(action);
}

void Icon_impl::init_action(Action_base & action) {
    icon_name_ = action.icon_name();
    if (!action.enabled()) { freeze(); }
    if (!action.visible()) { disappear(); }
    if (action.has_tooltip()) { set_tooltip(action.tooltip()); }
    action.signal_disable().connect(fun(this, &Icon_impl::freeze));
    action.signal_enable().connect(fun(this, &Icon_impl::thaw));
    action.signal_hide().connect(fun(this, &Icon_impl::disappear));
    action.signal_show().connect(fun(this, &Icon_impl::appear));
    action.signal_icon_changed().connect(fun(this, &Icon_impl::set_icon_name));
    action.signal_tooltip_changed().connect(fun(this, &Icon_impl::on_action_tooltip));
}

void Icon_impl::init() {
    signal_display().connect(fun(this, &Icon_impl::on_display));
}

void Icon_impl::on_display() {
    if (film_.empty() && !icon_name_.empty()) {
        Theme_impl::root()->signal_icons_changed().connect(fun(this, &Icon_impl::update_pixmap));
        update_pixmap();
    }
}

void Icon_impl::update_pixmap() {
    set_pixmap(Theme_impl::root()->get_icon(icon_name_, icon_size_), true);
}

void Icon_impl::assign(const ustring & icon_name, unsigned icon_size) {
    if (icon_name_ != icon_name || icon_size_ != icon_size) {
        icon_name_ = icon_name;
        icon_size_ = icon_size;
        update_pixmap();
    }
}

void Icon_impl::set_icon_name(const ustring & icon_name) {
    if (icon_name_ != icon_name) {
        icon_name_ = icon_name;
        update_pixmap();
    }
}

ustring Icon_impl::icon_name() const {
    return icon_name_;
}

void Icon_impl::set_icon_size(unsigned icon_size) {
    if (icon_size_ != icon_size) {
        icon_size_ = icon_size;
        update_pixmap();
    }
}

unsigned Icon_impl::icon_size() const {
    return icon_size_;
}

void Icon_impl::on_action_tooltip(const ustring & tooltip) {
    if (tooltip.empty()) { unset_tooltip(); }
    else { set_tooltip(tooltip); }
}

} // namespace tau

//END
