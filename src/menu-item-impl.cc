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

namespace tau {

Menu_item_impl::Menu_item_impl():
    Text_impl(ALIGN_START, ALIGN_CENTER)
{
}

Menu_item_impl::Menu_item_impl(const ustring & label):
    Text_impl(label, ALIGN_START, ALIGN_CENTER)
{
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Submenu_impl::Submenu_impl(const ustring & label, Menu_ptr menu):
    Menu_item_impl(label),
    Menu_image(),
    menu_(menu)
{
    init();
}

Submenu_impl::Submenu_impl(const ustring & label, Menu_ptr menu, const ustring & icon_name):
    Menu_item_impl(label),
    Menu_image(icon_name),
    menu_(menu)
{
    init();
}

void Submenu_impl::init() {
    arrow_ = std::make_shared<Icon_impl>("picto-right", 12);

    if (menu_) {
        menu_->signal_enable().connect(fun(this, &Submenu_impl::on_menu_enable));
        menu_->signal_disable().connect(fun(this, &Submenu_impl::on_menu_disable));
        if (!menu_->has_enabled_items()) { freeze(); }
    }
}

void Submenu_impl::on_menu_enable() {
    thaw();
}

void Submenu_impl::on_menu_disable() {
    freeze();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Action_menu_impl::Action_menu_impl(Action & action):
    Menu_item_impl(action.label()),
    action_(action),
    accel_label_(std::make_shared<Text_impl>())
{
    if (!action_.enabled()) { freeze(); }
    if (!action_.visible()) { disappear(); }

    action_.signal_enable().connect(fun(this, &Action_menu_impl::thaw));
    action_.signal_disable().connect(fun(this, &Action_menu_impl::freeze));
    action_.signal_hide().connect(fun(this, &Action_menu_impl::disappear));
    action_.signal_show().connect(fun(this, &Action_menu_impl::appear));
    action_.signal_label_changed().connect(fun(this, &Action_menu_impl::on_label_changed));
    action_.signal_accel_added().connect(fun(this, &Action_menu_impl::on_accel_added));
    action_.signal_accel_removed().connect(fun(this, &Action_menu_impl::on_accel_removed));
    action_.signal_destroy().connect(fun(this, &Action_menu_impl::on_action_destroy));

    update_label();
    update_icon();
    update_accels();
}

void Action_menu_impl::on_label_changed(const ustring & label) {
    assign(label);
    update_label();
}

void Action_menu_impl::update_accels() {
    auto & accels = action_.accels();

    if (!accels.empty()) {
        Accel & accel = accels.front();
        accel_changed_cx_.disconnect();
        accel_changed_cx_ = accel.signal_changed().connect(tau::bind(fun(this, &Action_menu_impl::on_accel_changed), std::ref(accel)));
        accel_label_->show();
        accel_label_->assign(accel.label());
    }

    else {
        accel_label_->clear();
        accel_label_->hide();
    }
}

void Action_menu_impl::on_accel_changed(Accel & accel) {
    update_accels();
}

void Action_menu_impl::on_accel_added(Accel & accel) {
    update_accels();
}

void Action_menu_impl::on_accel_removed(Accel & accel) {
    update_accels();
}

void Action_menu_impl::update_label() {
    if (text().empty()) {
        disappear();
    }

    else {
        appear();
    }
}

void Action_menu_impl::update_icon() {
    if (!action_.icon_name().empty()) {
        assign_icon(action_.icon_name());
    }
}

void Action_menu_impl::on_action_destroy() {
    destroyed_ = true;
    freeze();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Toggle_menu_impl::Toggle_menu_impl(Toggle_action & toggle_action, Check_style check_style, Border_style border_style):
    Menu_item_impl(toggle_action.label()),
    action_(toggle_action),
    check_(std::make_shared<Check_impl>(check_style, border_style)),
    accel_label_(std::make_shared<Text_impl>())
{
    if (!action_.enabled()) { freeze(); }
    if (!action_.visible()) { disappear(); }

    action_.signal_enable().connect(fun(this, &Toggle_menu_impl::thaw));
    action_.signal_disable().connect(fun(this, &Toggle_menu_impl::freeze));
    action_.signal_hide().connect(fun(this, &Toggle_menu_impl::disappear));
    action_.signal_show().connect(fun(this, &Toggle_menu_impl::appear));
    action_.signal_label_changed().connect(fun(this, &Toggle_menu_impl::on_label_changed));
    action_.signal_accel_added().connect(fun(this, &Toggle_menu_impl::on_accel_added));
    action_.signal_accel_removed().connect(fun(this, &Toggle_menu_impl::on_accel_removed));
    action_.connect(fun(this, &Toggle_menu_impl::on_action_toggle));
    action_.signal_destroy().connect(fun(this, &Toggle_menu_impl::on_action_destroy));

    if (action_.get()) { check_->toggle(); }
    check_cx_ = check_->signal_check().connect(fun(this, &Toggle_menu_impl::on_toggle));
    uncheck_cx_ = check_->signal_uncheck().connect(fun(this, &Toggle_menu_impl::on_toggle));

    update_label();
    update_accels();
}

void Toggle_menu_impl::on_action_destroy() {
    destroyed_ = true;
    freeze();
}

void Toggle_menu_impl::set_check_style(Check_style check_style) {
    check_->set_check_style(check_style);
}

Check_style Toggle_menu_impl::check_style() const {
    return check_->check_style();
}

void Toggle_menu_impl::set_border_style(Border_style border_style) {
    check_->set_border_style(border_style);
}

Border_style Toggle_menu_impl::border_style() const {
    return check_->border_style();
}

void Toggle_menu_impl::set_border_width(unsigned npx) {
    check_->set_border_width(npx);
}

unsigned Toggle_menu_impl::border_width() const {
    return check_->border_width();
}

void Toggle_menu_impl::update_label() {
    if (text().empty()) {
        disappear();
    }

    else {
        appear();
    }
}

void Toggle_menu_impl::update_accels() {
    auto & accels = action_.accels();

    if (!accels.empty()) {
        Accel & accel = accels.front();
        accel_changed_cx_.disconnect();
        accel_changed_cx_ = accel.signal_changed().connect(tau::bind(fun(this, &Toggle_menu_impl::on_accel_changed), std::ref(accel)));
        accel_label_->show();
        accel_label_->assign(accel.label());
    }

    else {
        accel_label_->clear();
        accel_label_->hide();
    }
}

void Toggle_menu_impl::on_label_changed(const ustring & label) {
    assign(label);
    update_label();
}

void Toggle_menu_impl::on_accel_added(Accel & accel) {
    update_accels();
}

void Toggle_menu_impl::on_accel_changed(Accel & accel) {
    update_accels();
}

void Toggle_menu_impl::on_accel_removed(Accel & accel) {
    update_accels();
}

void Toggle_menu_impl::toggle() {
    if (enabled() && !destroyed_) {
        check_->toggle();
    }
}

void Toggle_menu_impl::on_toggle() {
    action_.toggle();
    signal_quit_();
}

void Toggle_menu_impl::on_action_toggle(bool toggled) {
    check_cx_.block();
    uncheck_cx_.block();
    if (toggled) { check_->check(); }
    else { check_->uncheck(); }
    check_cx_.unblock();
    uncheck_cx_.unblock();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Slot_menu_impl::Slot_menu_impl(const ustring & label, const slot<void()> & slot_activate):
    Menu_item_impl(label),
    Menu_image()
{
    signal_activate_.connect(slot_activate);
    init();
}

Slot_menu_impl::Slot_menu_impl(const ustring & label, const slot<void()> & slot_activate, const ustring & image_name):
    Menu_item_impl(label),
    Menu_image(image_name)
{
    signal_activate_.connect(slot_activate);
    init();
}

void Slot_menu_impl::init() {
    if (signal_activate_.empty()) {
        freeze();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Check_menu_impl::Check_menu_impl(const ustring & label, bool checked):
    Menu_item_impl(label),
    check_(std::make_shared<Check_impl>(checked))
{
    init();
}

Check_menu_impl::Check_menu_impl(const ustring & label, Check_style check_style, bool checked):
    Menu_item_impl(label),
    check_(std::make_shared<Check_impl>(check_style, checked))
{
    init();
}

Check_menu_impl::Check_menu_impl(const ustring & label, Border_style border_style, bool checked):
    Menu_item_impl(label),
    check_(std::make_shared<Check_impl>(border_style, checked))
{
    init();
}

Check_menu_impl::Check_menu_impl(const ustring & label, Check_style check_style, Border_style border_style, bool checked):
    Menu_item_impl(label),
    check_(std::make_shared<Check_impl>(check_style, border_style, checked))
{
    init();
}

void Check_menu_impl::init() {
    check_->signal_check().connect(fun(this, &Check_menu_impl::on_check));
    check_->signal_uncheck().connect(fun(this, &Check_menu_impl::on_uncheck));
}

void Check_menu_impl::set_check_style(Check_style check_style) {
    check_->set_check_style(check_style);
}

Check_style Check_menu_impl::check_style() const {
    return check_->check_style();
}

void Check_menu_impl::set_border_style(Border_style border_style) {
    check_->set_border_style(border_style);
}

Border_style Check_menu_impl::border_style() const {
    return check_->border_style();
}

void Check_menu_impl::set_border_width(unsigned npx) {
    check_->set_border_width(npx);
}

unsigned Check_menu_impl::border_width() const {
    return check_->border_width();
}

void Check_menu_impl::check() {
    check_->check();
}

void Check_menu_impl::uncheck() {
    check_->uncheck();
}

void Check_menu_impl::toggle() {
    check_->toggle();
}

bool Check_menu_impl::checked() const {
    return check_->checked();
}

void Check_menu_impl::join(Check_menu_ptr other) {
    check_->join(other->check_ptr());
}

void Check_menu_impl::on_check() {
    signal_check_();
    signal_quit_();
}

void Check_menu_impl::on_uncheck() {
    if (!check_->joined()) {
        signal_uncheck_();
        signal_quit_();
    }
}

} // namespace tau

//END
