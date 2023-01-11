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

#include <menu-impl.hh>
#include <iostream>

namespace tau {

Menubar_impl::Menubar_impl():
    Menu_impl(OR_EAST),
    box_(std::make_shared<Box_impl>(OR_EAST, 12))
{
    insert(box_);
    connect_action(left_action_);
    connect_action(right_action_);
    signal_mouse_leave().connect(tau::fun(this, &Menubar_impl::on_mouse_leave));
}

void Menubar_impl::on_left() {
    select_prev();
    open_current();
}

void Menubar_impl::on_right() {
    select_next();
    open_current();
}

// "ESCAPE" came from the child menu.
void Menubar_impl::child_menu_escape() {
    reset_submenu();
    quit();
}

// "LEFT" came from the child menu.
void Menubar_impl::child_menu_left() {
    reset_submenu();
    select_prev();
    open_current();
    if (!submenu_) { grab_modal(); }
}

// "RIGHT" came from the child menu.
void Menubar_impl::child_menu_right() {
    reset_submenu();
    select_next();
    open_current();
    if (!submenu_) { grab_modal(); }
}

void Menubar_impl::mark_item(Menu_item_ptr ip, bool select) {
    if (select) {
        ip->style().get("background").set(style().get("background-select").get());
    }

    else {
        ip->style().get("background").unset();
    }
}

void Menubar_impl::activate() {
    if (enabled()) {
        grab_modal();
        select_item(current_item());
        open_current();
    }
}

bool Menubar_impl::on_item_mouse_down(int mbt, int mm, const Point & pt, Menu_item_impl * item) {
    if (MBT_LEFT == mbt && !(mm & (MM_CONTROL|MM_SHIFT))) {
        if (auto ip = item_ptr(item)) {
            if (current_item_ != ip) { unselect_current(); select_item(ip); }
            activate_current();
        }

        return true;
    }

    return false;
}

void Menubar_impl::on_item_mouse_enter(const Point & pt, Menu_item_impl * item) {
    if (enabled()) {
        if (auto ip = item_ptr(item)) {
            if (!ip->disabled()) {
                if (ip != current_item_) {
                    select_item(ip);
                }
            }
        }
    }
}

void Menubar_impl::on_mouse_leave() {
    if (!submenu_) { unselect_current(); }
}

void Menubar_impl::add_item(Menu_item_ptr ip) {
    items_.push_back(ip);
    ip->signal_mouse_down().connect(tau::bind(fun(this, &Menubar_impl::on_item_mouse_down), ip.get()));
    ip->signal_mouse_enter().connect(tau::bind(fun(this, &Menubar_impl::on_item_mouse_enter), ip.get()));
    ip->signal_enable().connect(fun(this, &Menu_impl::on_item_enable));
    ip->signal_disable().connect(fun(this, &Menu_impl::on_item_disable));
    if (!ip->disabled()) { thaw(); }
}

// Overrides pure Menu_impl.
void Menubar_impl::append_widget(Widget_ptr wp) {
    box_->append(wp, true);
    if (auto ip = std::dynamic_pointer_cast<Menu_item_impl>(wp)) { add_item(ip); }
}

// Overrides pure Menu_impl.
void Menubar_impl::prepend_widget(Widget_ptr wp) {
    box_->prepend(wp, true);
    if (auto ip = std::dynamic_pointer_cast<Menu_item_impl>(wp)) { add_item(ip); }
}

// Overrides pure Menu_impl.
void Menubar_impl::insert_widget_before(Widget_ptr wp, Widget_cptr other) {
    box_->insert_before(wp, other.get(), true);
    if (auto ip = std::dynamic_pointer_cast<Menu_item_impl>(wp)) { add_item(ip); }
}

// Overrides pure Menu_impl.
void Menubar_impl::insert_widget_after(Widget_ptr wp, Widget_cptr other) {
    box_->insert_after(wp, other.get(), true);
    if (auto ip = std::dynamic_pointer_cast<Menu_item_impl>(wp)) { add_item(ip); }
}

// Overrides pure Menu_impl.
void Menubar_impl::remove_widget(Widget_ptr wp) {
    remove_item(wp);
    box_->remove(wp.get());
}

// Overrides pure Menu_impl.
void Menubar_impl::clear_widgets() {
    items_.clear();
    box_->clear();
}

} // namespace tau

//END
