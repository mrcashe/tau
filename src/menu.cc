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

#include <tau/menu.hh>
#include <tau/toplevel.hh>
#include <toplevel-impl.hh>
#include <menu-impl.hh>

namespace tau {

#define MENU_IMPL (std::static_pointer_cast<Menu_impl>(impl))

Menu::Menu(Widget_ptr wp):
    Container(wp)
{
}

void Menu::append(Widget & w) {
    MENU_IMPL->append_widget(w.ptr());
}

void Menu::append(Action & action) {
    Action_menu_item item(action);
    append(item);
}

void Menu::append(Toggle_action & action, Check_style check_style, Border_style border_style) {
    Toggle_menu_item item(action, check_style, border_style);
    append(item);
}

void Menu::append(const ustring & label, const slot<void()> & slot_activate) {
    Slot_menu_item item(label, slot_activate);
    append(item);
}

void Menu::append(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name) {
    Slot_menu_item item(label, slot_activate, icon_name);
    append(item);
}

void Menu::append(const ustring & label, Menu & menu) {
    Submenu_item submenu(label, menu);
    append(submenu);
}

void Menu::append_separator(Separator_style separator_style) {
    MENU_IMPL->append_separator(separator_style);
}

void Menu::prepend(Widget & w) {
    MENU_IMPL->prepend_widget(w.ptr());
}

void Menu::prepend(Action & action) {
    Action_menu_item item(action);
    prepend(item);
}

void Menu::prepend(Toggle_action & action, Check_style check_style, Border_style border_style) {
    Toggle_menu_item item(action, check_style, border_style);
    prepend(item);
}

void Menu::prepend(const ustring & label, const slot<void()> & slot_activate) {
    Slot_menu_item item(label, slot_activate);
    prepend(item);
}

void Menu::prepend(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name) {
    Slot_menu_item item(label, slot_activate, icon_name);
    prepend(item);
}

void Menu::prepend(const ustring & label, Menu & menu) {
    Submenu_item submenu(label, menu);
    prepend(submenu);
}

void Menu::prepend_separator(Separator_style separator_style) {
    MENU_IMPL->prepend_separator(separator_style);
}

void Menu::insert_before(Widget & w, const Widget & other) {
    MENU_IMPL->insert_widget_before(w.ptr(), other.ptr());
}

void Menu::insert_before(Action & action, const Widget & other) {
    Action_menu_item item(action);
    insert_before(item, other);
}

void Menu::insert_before(Toggle_action & action, const Widget & other, Check_style check_style, Border_style border_style) {
    Toggle_menu_item item(action, check_style, border_style);
    insert_before(item, other);
}

void Menu::insert_before(const ustring & label, const slot<void()> & slot_activate, const Widget & other) {
    Slot_menu_item item(label, slot_activate);
    insert_before(item, other);
}

void Menu::insert_before(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name, const Widget & other) {
    Slot_menu_item item(label, slot_activate, icon_name);
    insert_before(item, other);
}

void Menu::insert_before(const ustring & label, Menu & menu, const Widget & other) {
    Submenu_item item(label, menu);
    insert_before(item, other);
}

void Menu::insert_separator_before(const Widget & other, Separator_style separator_style) {
    MENU_IMPL->insert_separator_before(other.ptr(), separator_style);
}

void Menu::insert_after(Widget & w, const Widget & other) {
    MENU_IMPL->insert_widget_after(w.ptr(), other.ptr());
}

void Menu::insert_after(Action & action, const Widget & other) {
    Action_menu_item item(action);
    insert_after(item, other);
}

void Menu::insert_after(Toggle_action & action, const Widget & other, Check_style check_style, Border_style border_style) {
    Toggle_menu_item item(action, check_style, border_style);
    insert_after(item, other);
}

void Menu::insert_after(const ustring & label, const slot<void()> & slot_activate, const Widget & other) {
    Slot_menu_item item(label, slot_activate);
    insert_after(item, other);
}

void Menu::insert_after(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name, const Widget & other) {
    Slot_menu_item item(label, slot_activate, icon_name);
    insert_after(item, other);
}

void Menu::insert_after(const ustring & label, Menu & menu, const Widget & other) {
    Submenu_item item(label, menu);
    insert_after(item, other);
}

void Menu::insert_separator_after(const Widget & other, Separator_style separator_style) {
    MENU_IMPL->insert_separator_after(other.ptr(), separator_style);
}

void Menu::remove(Widget & w) {
    MENU_IMPL->remove_widget(w.ptr());
}

void Menu::clear() {
    MENU_IMPL->clear_widgets();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define MENU_ITEM_IMPL (std::static_pointer_cast<Menu_item_impl>(impl))

Menu_item::Menu_item(Widget_ptr wp):
    Widget(wp)
{
}

ustring Menu_item::label() const {
    return MENU_ITEM_IMPL->text();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Submenu_item::Submenu_item(const ustring & label, Menu & menu):
    Menu_item(std::make_shared<Submenu_impl>(label, std::static_pointer_cast<Menu_impl>(menu.ptr())))
{
}

Submenu_item::Submenu_item(const ustring & label, Menu & menu, const ustring & icon_name):
    Menu_item(std::make_shared<Submenu_impl>(label, std::static_pointer_cast<Menu_impl>(menu.ptr()), icon_name))
{
}

void Submenu_item::set_label(const ustring & label) {
    MENU_ITEM_IMPL->assign(label);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Slot_menu_item::Slot_menu_item(const ustring & label, const slot<void()> & slot_activate):
    Menu_item(std::make_shared<Slot_menu_impl>(label, slot_activate))
{
}

Slot_menu_item::Slot_menu_item(const ustring & label, const slot<void()> & slot_activate, const ustring & image_name):
    Menu_item(std::make_shared<Slot_menu_impl>(label, slot_activate, image_name))
{
}

void Slot_menu_item::set_label(const ustring & label) {
    MENU_ITEM_IMPL->assign(label);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Action_menu_item::Action_menu_item(Action & action):
    Menu_item(std::make_shared<Action_menu_impl>(action))
{
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define TOGGLE_MENU_IMPL (std::static_pointer_cast<Toggle_menu_impl>(impl))

Toggle_menu_item::Toggle_menu_item(Toggle_action & toggle_action, Check_style check_style, Border_style border_style):
    Menu_item(std::make_shared<Toggle_menu_impl>(toggle_action, check_style, border_style))
{
}

void Toggle_menu_item::set_check_style(Check_style check_style) {
    TOGGLE_MENU_IMPL->set_check_style(check_style);
}

Check_style Toggle_menu_item::check_style() const {
    return TOGGLE_MENU_IMPL->check_style();
}

void Toggle_menu_item::set_border_style(Border_style border_style) {
    TOGGLE_MENU_IMPL->set_border_style(border_style);
}

Border_style Toggle_menu_item::border_style() const {
    return TOGGLE_MENU_IMPL->border_style();
}

void Toggle_menu_item::set_border_width(unsigned npx) {
    TOGGLE_MENU_IMPL->set_border_width(npx);
}

unsigned Toggle_menu_item::border_width() const {
    return TOGGLE_MENU_IMPL->border_width();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define CHECK_MENU_IMPL (std::static_pointer_cast<Check_menu_impl>(impl))

Check_menu_item::Check_menu_item(const ustring & label, bool checked):
    Menu_item(std::make_shared<Check_menu_impl>(label, checked))
{
}

Check_menu_item::Check_menu_item(const ustring & label, Check_style check_style, bool checked):
    Menu_item(std::make_shared<Check_menu_impl>(label, check_style, checked))
{
}

Check_menu_item::Check_menu_item(const ustring & label, Border_style border_style, bool checked):
    Menu_item(std::make_shared<Check_menu_impl>(label, border_style, checked))
{
}

Check_menu_item::Check_menu_item(const ustring & label, Check_style check_style, Border_style border_style, bool checked):
    Menu_item(std::make_shared<Check_menu_impl>(label, check_style, border_style, checked))
{
}

void Check_menu_item::set_label(const ustring & label) {
    CHECK_MENU_IMPL->assign(label);
}

void Check_menu_item::set_check_style(Check_style check_style) {
    CHECK_MENU_IMPL->set_check_style(check_style);
}

Check_style Check_menu_item::check_style() const {
    return CHECK_MENU_IMPL->check_style();
}

void Check_menu_item::set_border_style(Border_style border_style) {
    CHECK_MENU_IMPL->set_border_style(border_style);
}

Border_style Check_menu_item::border_style() const {
    return CHECK_MENU_IMPL->border_style();
}

void Check_menu_item::set_border_width(unsigned npx) {
    CHECK_MENU_IMPL->set_border_width(npx);
}

unsigned Check_menu_item::border_width() const {
    return CHECK_MENU_IMPL->border_width();
}

void Check_menu_item::check() {
    CHECK_MENU_IMPL->check();
}

void Check_menu_item::uncheck() {
    CHECK_MENU_IMPL->uncheck();
}

void Check_menu_item::toggle() {
    CHECK_MENU_IMPL->toggle();
}

bool Check_menu_item::checked() const {
    return CHECK_MENU_IMPL->checked();
}

void Check_menu_item::join(Check_menu_item & other) {
    CHECK_MENU_IMPL->join(std::static_pointer_cast<Check_menu_impl>(other.ptr()));
}

signal<void()> & Check_menu_item::signal_check() {
    return CHECK_MENU_IMPL->signal_check();
}

signal<void()> & Check_menu_item::signal_uncheck() {
    return CHECK_MENU_IMPL->signal_uncheck();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define MENUBAR_IMPL (std::static_pointer_cast<Menubar_impl>(impl))

Menubar::Menubar():
    Menu(std::make_shared<Menubar_impl>())
{
}

void Menubar::activate() {
    MENUBAR_IMPL->activate();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define MENUBOX_IMPL (std::static_pointer_cast<Menubox_impl>(impl))

Menubox::Menubox():
    Menu(std::make_shared<Menubox_impl>())
{
}

void Menubox::popup(Toplevel & parent_window, const Point & position, Gravity gravity) {
    auto wpp = std::dynamic_pointer_cast<Window_impl>(parent_window.ptr());

    if (wpp) {
        MENUBOX_IMPL->popup(wpp.get(), impl, position, gravity, nullptr);
        MENUBOX_IMPL->grab_modal();
    }
}

} // namespace tau

//END
