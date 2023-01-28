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
#include <tau/input.hh>
#include <tau/string.hh>
#include <theme-impl.hh>
#include <iostream>

namespace tau {

Master_action::Master_action(const ustring & accels):
    trackable()
{
    add_accels(accels);
}

Master_action::Master_action(char32_t kc, int km):
    trackable()
{
    add_accel(kc, km);
}

Master_action::Master_action(const ustring & accels, const ustring & label):
    trackable()
{
    add_accels(accels);
    set_label(label);
}

Master_action::Master_action(char32_t kc, int km, const ustring & label):
    trackable()
{
    add_accel(kc, km);
    set_label(label);
}

Master_action::Master_action(const ustring & accels, const ustring & label, const ustring & icon_name):
    trackable()
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
}

Master_action::Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name):
    trackable()
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
}

Master_action::Master_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable()
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Master_action::Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable()
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

void Master_action::set_label(const ustring & label) {
    if (label_ != label) {
        label_ = label;
        signal_label_changed_(label_);
    }
}

ustring Master_action::label() const {
    return label_;
}

void Master_action::enable() {
    if (!enabled_) {
        enabled_ = true;
        signal_enable_();
    }
}

void Master_action::disable() {
    if (enabled_) {
        enabled_ = false;
        signal_disable_();
    }
}

bool Master_action::enabled() const {
    return enabled_;
}

void Master_action::show() {
    if (!visible_) {
        visible_ = true;
        signal_show_();
    }
}

void Master_action::hide() {
    if (visible_) {
        visible_ = false;
        signal_hide_();
    }
}

bool Master_action::visible() const {
    return visible_;
}

void Master_action::set_icon_name(const ustring & icon_name) {
    if (icon_name_ != icon_name) {
        icon_name_ = icon_name;
        signal_icon_changed_(icon_name_);
    }
}

ustring Master_action::icon_name() const {
    return icon_name_;
}

void Master_action::set_tooltip(const ustring & tooltip) {
    if (tooltip_ != tooltip) {
        tooltip_ = tooltip;
        signal_tooltip_changed_(tooltip_);
    }
}

void Master_action::unset_tooltip() {
    if (!tooltip_.empty()) {
        tooltip_.clear();
        signal_tooltip_changed_(tooltip_);
    }
}

ustring Master_action::tooltip() const {
    return tooltip_;
}

void Master_action::add_accel(char32_t kc, int km) {
    Accel accel(kc, km);
    auto i = std::find(accels_.begin(), accels_.end(), accel);

    if (i == accels_.end()) {
        accels_.emplace_back(accel);
        signal_accel_added_(accels_.back());
    }
}

void Master_action::add_accels(const ustring & key_specs) {
    for (const ustring & spec: str_explode(key_specs, str_blanks())) {
        char32_t kc; int km;
        key_spec_from_string(spec, kc, km);
        add_accel(kc, km);
    }
}

void Master_action::remove_accel(char32_t kc, int km) {
    Accel accel(kc, km);
    auto i = std::find(accels_.begin(), accels_.end(), accel);

    if (i != accels_.end()) {
        signal_accel_removed_(*i);
        accels_.erase(i);
    }
}

void Master_action::remove_accels(const ustring & key_specs) {
    for (const ustring & spec: str_explode(key_specs, str_blanks())) {
        char32_t kc; int km;
        key_spec_from_string(spec, kc, km);
        remove_accel(kc, km);
    }
}

void Master_action::clear_accels() {
    for (auto & accel: accels_) { signal_accel_removed_(accel); }
    accels_.clear();
}

std::vector<Accel> Master_action::accels() const {
    return accels_;
}

signal<void()> & Master_action::signal_disable() {
    return signal_disable_;
}

signal<void()> & Master_action::signal_enable() {
    return signal_enable_;
}

signal<void()> & Master_action::signal_hide() {
    return signal_hide_;
}

signal<void()> & Master_action::signal_show() {
    return signal_show_;
}

signal<void(const Accel & accel)> & Master_action::signal_accel_added() {
    return signal_accel_added_;
}

signal<void(const Accel & accel)> & Master_action::signal_accel_removed() {
    return signal_accel_removed_;
}

signal<void(const ustring &)> & Master_action::signal_label_changed() {
    return signal_label_changed_;
}

signal<void(const ustring &)> & Master_action::signal_icon_changed() {
    return signal_icon_changed_;
}

signal<void(const ustring &)> & Master_action::signal_tooltip_changed() {
    return signal_tooltip_changed_;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Action_base::Action_base(const ustring & accels):
    trackable()
{
    add_accels(accels);
}

Action_base::Action_base(char32_t kc, int km):
    trackable()
{
    add_accel(kc, km);
}

Action_base::Action_base(const ustring & accels, const ustring & label):
    trackable()
{
    add_accels(accels);
    set_label(label);
}

Action_base::Action_base(char32_t kc, int km, const ustring & label):
    trackable()
{
    add_accel(kc, km);
    set_label(label);
}

Action_base::Action_base(const ustring & accels, const ustring & label, const ustring & icon_name):
    trackable()
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
}

Action_base::Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name):
    trackable()
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
}

Action_base::Action_base(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable()
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Action_base::Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable()
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Action_base::Action_base(Master_action & master_action):
    trackable()
{
    connect_master_action(master_action);
}

Action_base::~Action_base() {
    show();
    appear();
    enable();
    thaw();
    signal_destroy_();
}

void Action_base::set_label(const ustring & label) {
    if (label_ != label) {
        label_ = label;
        signal_label_changed_(label_);
    }
}

ustring Action_base::label() const {
    return label_;
}

void Action_base::enable() {
    if (disabled_) {
        disabled_ = false;
        if (!frozen_) { on_enable(); signal_enable_(); }
    }
}

void Action_base::thaw() {
    if (frozen_) {
        frozen_ = false;
        if (!disabled_) { on_enable(); signal_enable_(); }
    }
}

void Action_base::disable() {
    if (!disabled_) {
        disabled_ = true;
        if (!frozen_) { on_disable(); signal_disable_(); }
    }
}

void Action_base::freeze() {
    if (!frozen_) {
        frozen_ = true;
        if (!disabled_) { on_disable(); signal_disable_(); }
    }
}

bool Action_base::enabled() const {
    return !disabled_ && !frozen_;
}

void Action_base::show() {
    if (hidden_) {
        hidden_ = false;
        if (!disappeared_) { signal_show_(); }
    }
}

void Action_base::appear() {
    if (disappeared_) {
        disappeared_ = false;
        if (!hidden_) { signal_show_(); }
    }
}

void Action_base::hide() {
    if (!hidden_) {
        hidden_ = true;
        if (!disappeared_) { signal_hide_(); }
    }
}

void Action_base::disappear() {
    if (!disappeared_) {
        disappeared_ = true;
        if (!hidden_) { signal_hide_(); }
    }
}

bool Action_base::visible() const {
    return !hidden_ && !disappeared_;
}

void Action_base::set_icon_name(const ustring & icon_name) {
    if (icon_name_ != icon_name) {
        icon_name_ = icon_name;
        signal_icon_changed_(icon_name_);
    }
}

ustring Action_base::icon_name() const {
    return icon_name_;
}

void Action_base::set_tooltip(const ustring & tooltip) {
    if (tooltip_ != tooltip) {
        tooltip_ = tooltip;
        signal_tooltip_changed_(tooltip_);
    }
}

void Action_base::unset_tooltip() {
    if (!tooltip_.empty()) {
        tooltip_.clear();
        signal_tooltip_changed_(tooltip_);
    }
}

ustring Action_base::tooltip() const {
    return tooltip_;
}

void Action_base::add_accel(char32_t kc, int km) {
    if (KC_NONE != kc) {
        Accel accel(kc, km);
        auto i = std::find(accels_.begin(), accels_.end(), accel);

        if (i == accels_.end()) {
            accels_.emplace_back(accel);
            accels_.back().connect(fun(this, &Action_base::on_accel));
            signal_accel_added_(accels_.back());
        }
    }
}

void Action_base::add_accels(const ustring & key_specs) {
    for (const ustring & spec: str_explode(key_specs, str_blanks())) {
        if (!spec.empty()) {
            char32_t kc; int km;
            key_spec_from_string(spec, kc, km);
            if (0 == kc) { std::cerr << "** Action_base::add_accels(): failed to resolve specification " << spec << std::endl; }
            else { add_accel(kc, km); }
        }
    }
}

void Action_base::remove_accel(char32_t kc, int km) {
    Accel accel(kc, km);
    auto i = std::find(accels_.begin(), accels_.end(), accel);

    if (i != accels_.end()) {
        signal_accel_removed_(*i);
        accels_.erase(i);
    }
}

void Action_base::remove_accels(const ustring & key_specs) {
    for (const ustring & spec: str_explode(key_specs, str_blanks())) {
        char32_t kc; int km;
        key_spec_from_string(spec, kc, km);
        remove_accel(kc, km);
    }
}

void Action_base::clear_accels() {
    for (auto & accel: accels_) { signal_accel_removed_(accel); }
    accels_.clear();
}

void Action_base::on_enable() {
    for (auto & accel: accels_) { accel.enable(); }
}

void Action_base::on_disable() {
    for (auto & accel: accels_) { accel.disable(); }
}

void Action_base::connect_master_action(Master_action & master_action) {
    master_action.signal_accel_added().connect(fun(this, &Action_base::on_accel_added));
    master_action.signal_accel_removed().connect(fun(this, &Action_base::on_accel_removed));
    master_action.signal_enable().connect(fun(this, &Action_base::thaw));
    master_action.signal_disable().connect(fun(this, &Action_base::freeze));
    master_action.signal_show().connect(fun(this, &Action_base::appear));
    master_action.signal_hide().connect(fun(this, &Action_base::disappear));
    master_action.signal_label_changed().connect(fun(this, &Action_base::set_label));
    master_action.signal_icon_changed().connect(fun(this, &Action_base::set_icon_name));
    master_action.signal_tooltip_changed().connect(fun(this, &Action_base::set_tooltip));

    for (Accel & accel: master_action.accels()) {
        char32_t kc; int km;
        accel.get_keys(kc, km);
        add_accel(kc, km);
    }

    if (!master_action.enabled()) { freeze(); }
    if (!master_action.visible()) { disappear(); }
    set_label(master_action.label());
    set_icon_name(master_action.icon_name());
    set_tooltip(master_action.tooltip());
}

void Action_base::connect_master_action(Master_action * master_action) {
    if (master_action) {
        connect_master_action(*master_action);
    }
}

void Action_base::connect_master_action(const ustring & name) {
    connect_master_action(Theme_impl::root()->find_action(name));
}

void Action_base::on_accel_added(const Accel & accel) {
    char32_t kc; int km;
    accel.get_keys(kc, km);
    add_accel(kc, km);
}

void Action_base::on_accel_removed(const Accel & accel) {
    char32_t kc; int km;
    accel.get_keys(kc, km);
    remove_accel(kc, km);
}

Action_base * Action_base::lookup(char32_t kc, int km) {
    for (auto & accel: accels_) {
        if (accel.equals(kc, km)) {
            return this;
        }
    }

    return nullptr;
}

std::vector<Accel> & Action_base::accels() {
    return accels_;
}

signal<void()> & Action_base::signal_disable() {
    return signal_disable_;
}

signal<void()> & Action_base::signal_enable() {
    return signal_enable_;
}

signal<void()> & Action_base::signal_hide() {
    return signal_hide_;
}

signal<void()> & Action_base::signal_show() {
    return signal_show_;
}

signal<void(Accel &)> & Action_base::signal_accel_added() {
    return signal_accel_added_;
}

signal<void(Accel &)> & Action_base::signal_accel_removed() {
    return signal_accel_removed_;
}

signal<void(const ustring &)> & Action_base::signal_label_changed() {
    return signal_label_changed_;
}

signal<void(const ustring &)> & Action_base::signal_icon_changed() {
    return signal_icon_changed_;
}

signal<void(const ustring &)> & Action_base::signal_tooltip_changed() {
    return signal_tooltip_changed_;
}

signal<void()> & Action_base::signal_destroy() {
    return signal_destroy_;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Action::Action(slot<void()> slot_activate):
    Action_base()
{
    signal_activate_.connect(slot_activate);
}

Action::Action(const ustring & accels, slot<void()> slot_activate):
    Action_base(accels)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(char32_t kc, int km, slot<void()> slot_activate):
    Action_base(kc, km)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(const ustring & accels, const ustring & label, slot<void()> slot_activate):
    Action_base(accels, label)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(char32_t kc, int km, const ustring & label, slot<void()> slot_activate):
    Action_base(kc, km, label)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(const ustring & accels, const ustring & label, const ustring & icon_name, slot<void()> slot_activate):
    Action_base(accels, label, icon_name)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(char32_t kc, int km, const ustring & label, const ustring & icon_name, slot<void()> slot_activate):
    Action_base(kc, km, label, icon_name)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void()> slot_activate):
    Action_base(accels, label, icon_name, tooltip)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void()> slot_activate):
    Action_base(kc, km, label, icon_name, tooltip)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

Action::Action(Master_action & master_action, slot<void()> slot_activate):
    Action_base(master_action)
{
    if (slot_activate) { signal_activate_.connect(slot_activate); }
}

void Action::exec() {
    if (enabled()) {
        signal_activate_();
    }
}

connection Action::connect(slot<void()> slot_activate, bool prepend) {
    return signal_activate_.connect(slot_activate, prepend);
}

// Overrides pure Action_base.
bool Action::on_accel() {
    if (enabled() && !signal_activate_.empty()) {
        signal_activate_();
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Toggle_action::Toggle_action(slot<void(bool)> slot_toggle):
    Action_base()
{
    signal_toggle_.connect(slot_toggle);
}

Toggle_action::Toggle_action(const ustring & accels, slot<void(bool)> slot_toggle):
    Action_base(accels)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(char32_t kc, int km, slot<void(bool)> slot_toggle):
    Action_base(kc, km)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(const ustring & accels, const ustring & label, slot<void(bool)> slot_toggle):
    Action_base(accels, label)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(char32_t kc, int km, const ustring & label, slot<void(bool)> slot_toggle):
    Action_base(kc, km, label)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(const ustring & accels, const ustring & label, const ustring & icon_name, slot<void(bool)> slot_toggle):
    Action_base(accels, label, icon_name)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, slot<void(bool)> slot_toggle):
    Action_base(kc, km, label, icon_name)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void(bool)> slot_toggle):
    Action_base(accels, label, icon_name, tooltip)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void(bool)> slot_toggle):
    Action_base(kc, km, label, icon_name, tooltip)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

Toggle_action::Toggle_action(Master_action & master_action, slot<void(bool)> slot_toggle):
    Action_base(master_action)
{
    if (slot_toggle) { signal_toggle_.connect(slot_toggle); }
}

void Toggle_action::toggle() {
    if (enabled()) {
        state_ = !state_;
        signal_toggle_(state_);
    }
}

bool Toggle_action::get() const {
    return state_;
}

void Toggle_action::set(bool state) {
    if (enabled() && state_ != state) {
        state_ = state;
        signal_toggle_(state_);
    }
}

connection Toggle_action::connect(slot<void(bool)> slot_toggle, bool prepend) {
    return signal_toggle_.connect(slot_toggle, prepend);
}

// Overrides pure Action_base.
bool Toggle_action::on_accel() {
    if (enabled()) {
        state_ = !state_;

        if (!signal_toggle_.empty()) {
            signal_toggle_(state_);
            return true;
        }
    }

    return false;
}

} // namespace tau

//END
