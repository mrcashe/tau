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

#include <tau/action.hh>
#include <tau/input.hh>
#include <tau/string.hh>
#include <theme-impl.hh>
#include <iostream>

namespace tau {

struct Master_action::Data {
    ustring             label_;
    ustring             icon_name_;
    ustring             tooltip_;
    bool                visible_ : 1;
    bool                enabled_ : 1;
    std::vector<Accel>  accels_;

    signal<void()>      signal_disable_;
    signal<void()>      signal_enable_;
    signal<void()>      signal_show_;
    signal<void()>      signal_hide_;

    signal<void(const Accel &)>     signal_accel_added_;
    signal<void(const Accel &)>     signal_accel_removed_;
    signal<void(const ustring &)>   signal_label_changed_;
    signal<void(const ustring &)>   signal_icon_changed_;
    signal<void(const ustring &)>   signal_tooltip_changed_;

    Data(): visible_(true), enabled_(true) {}
    Data(const Data & other) = default;
    Data(Data && other) = default;
    Data & operator=(const Data & other) = default;
    Data & operator=(Data && other) = default;
};

Master_action::Master_action():
    trackable(),
    data(new Data)
{
}

Master_action::Master_action(const ustring & accels):
    trackable(),
    data(new Data)
{
    add_accels(accels);
}

Master_action::Master_action(const Master_action & other):
    trackable(),
    data(new Data(*other.data))
{
}

Master_action::Master_action(Master_action && other):
    trackable(),
    data(new Data(std::move(*other.data)))
{
}

Master_action & Master_action::operator=(const Master_action & other) {
    if (this != &other) { *data = *other.data; }
    return *this;
}

Master_action & Master_action::operator=(Master_action && other) {
    *data = std::move(*other.data);
    return *this;
}

Master_action::Master_action(char32_t kc, int km):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
}

Master_action::Master_action(const ustring & accels, const ustring & label):
    trackable(),
    data(new Data)
{
    add_accels(accels);
    set_label(label);
}

Master_action::Master_action(char32_t kc, int km, const ustring & label):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
    set_label(label);
}

Master_action::Master_action(const ustring & accels, const ustring & label, const ustring & icon_name):
    trackable(),
    data(new Data)
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
}

Master_action::Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
}

Master_action::Master_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable(),
    data(new Data)
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Master_action::Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Master_action::~Master_action() {
    delete data;
}

void Master_action::set_label(const ustring & label) {
    if (data->label_ != label) {
        data->label_ = label;
        data->signal_label_changed_(data->label_);
    }
}

ustring Master_action::label() const {
    return data->label_;
}

void Master_action::enable() {
    if (!data->enabled_) {
        data->enabled_ = true;
        data->signal_enable_();
    }
}

void Master_action::disable() {
    if (data->enabled_) {
        data->enabled_ = false;
        data->signal_disable_();
    }
}

bool Master_action::enabled() const {
    return data->enabled_;
}

void Master_action::show() {
    if (!data->visible_) {
        data->visible_ = true;
        data->signal_show_();
    }
}

void Master_action::hide() {
    if (data->visible_) {
        data->visible_ = false;
        data->signal_hide_();
    }
}

bool Master_action::visible() const {
    return data->visible_;
}

void Master_action::set_icon_name(const ustring & icon_name) {
    if (data->icon_name_ != icon_name) {
        data->icon_name_ = icon_name;
        data->signal_icon_changed_(data->icon_name_);
    }
}

ustring Master_action::icon_name() const {
    return data->icon_name_;
}

void Master_action::set_tooltip(const ustring & tooltip) {
    if (data->tooltip_ != tooltip) {
        data->tooltip_ = tooltip;
        data->signal_tooltip_changed_(data->tooltip_);
    }
}

void Master_action::unset_tooltip() {
    if (!data->tooltip_.empty()) {
        data->tooltip_.clear();
        data->signal_tooltip_changed_(data->tooltip_);
    }
}

ustring Master_action::tooltip() const {
    return data->tooltip_;
}

void Master_action::add_accel(char32_t kc, int km) {
    Accel accel(kc, km);
    auto i = std::find(data->accels_.begin(), data->accels_.end(), accel);

    if (i == data->accels_.end()) {
        data->accels_.emplace_back(accel);
        data->signal_accel_added_(data->accels_.back());
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
    auto i = std::find(data->accels_.begin(), data->accels_.end(), accel);

    if (i != data->accels_.end()) {
        data->signal_accel_removed_(*i);
        data->accels_.erase(i);
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
    for (auto & accel: data->accels_) { data->signal_accel_removed_(accel); }
    data->accels_.clear();
}

std::vector<Accel> Master_action::accels() const {
    return data->accels_;
}

signal<void()> & Master_action::signal_disable() {
    return data->signal_disable_;
}

signal<void()> & Master_action::signal_enable() {
    return data->signal_enable_;
}

signal<void()> & Master_action::signal_hide() {
    return data->signal_hide_;
}

signal<void()> & Master_action::signal_show() {
    return data->signal_show_;
}

signal<void(const Accel & accel)> & Master_action::signal_accel_added() {
    return data->signal_accel_added_;
}

signal<void(const Accel & accel)> & Master_action::signal_accel_removed() {
    return data->signal_accel_removed_;
}

signal<void(const ustring &)> & Master_action::signal_label_changed() {
    return data->signal_label_changed_;
}

signal<void(const ustring &)> & Master_action::signal_icon_changed() {
    return data->signal_icon_changed_;
}

signal<void(const ustring &)> & Master_action::signal_tooltip_changed() {
    return data->signal_tooltip_changed_;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct Action_base::Data {
    bool                    disabled_ : 1;
    bool                    frozen_ : 1;
    bool                    hidden_ : 1;
    bool                    disappeared_ : 1;
    ustring                 label_;
    ustring                 icon_name_;
    ustring                 tooltip_;
    std::vector<Accel>      accels_;

    signal<void()> *        signal_disable_ = nullptr;
    signal<void()> *        signal_enable_ = nullptr;
    signal<void()> *        signal_show_ = nullptr;
    signal<void()> *        signal_hide_ = nullptr;
    signal<void(Accel &)> * signal_accel_added_ = nullptr;
    signal<void(Accel &)> * signal_accel_removed_ = nullptr;
    signal<void(const ustring &)> * signal_label_changed_ = nullptr;
    signal<void(const ustring &)> * signal_icon_changed_ = nullptr;
    signal<void(const ustring &)> * signal_tooltip_changed_ = nullptr;
    signal<void()>          signal_destroy_;

    connection              accel_added_cx_ { true };
    connection              accel_removed_cx_ { true };
    connection              enable_cx_ { true };
    connection              disable_cx_ { true };
    connection              show_cx_ { true };
    connection              hide_cx_ { true };
    connection              label_changed_cx_ { true };
    connection              icon_changed_cx_ { true };
    connection              tooltip_changed_cx_ { true };

    Data(): disabled_(false), frozen_(false), hidden_(false), disappeared_(false) {}
    Data(const Data & other) = default;
    Data(Data && other) = default;
    Data & operator=(const Data & other) = default;
    Data & operator=(Data && other) = default;

   ~Data() {
       if (signal_disable_) { delete signal_disable_; }
       if (signal_enable_) { delete signal_enable_; }
       if (signal_show_) { delete signal_show_; }
       if (signal_hide_) { delete signal_hide_; }
       if (signal_accel_added_) { delete signal_accel_added_; }
       if (signal_accel_removed_) { delete signal_accel_removed_; }
       if (signal_label_changed_) { delete signal_label_changed_; }
       if (signal_icon_changed_) { delete signal_icon_changed_; }
       if (signal_tooltip_changed_) { delete signal_tooltip_changed_; }
   }

    bool connected() const {
        return  (signal_disable_ && !signal_disable_->empty()) ||
                (signal_enable_ && !signal_enable_->empty()) ||
                (signal_show_ && !signal_show_->empty()) ||
                (signal_hide_ && !signal_hide_->empty()) ||
                (signal_accel_added_ && !signal_accel_added_->empty()) ||
                (signal_accel_removed_ && !signal_accel_removed_->empty()) ||
                (signal_label_changed_ && !signal_label_changed_->empty()) ||
                (signal_icon_changed_ && !signal_icon_changed_->empty()) ||
                (signal_tooltip_changed_ && !signal_tooltip_changed_->empty());
    }

    signal<void()> & signal_disable() {
        if (!signal_disable_) { signal_disable_ = new signal<void()>; }
        return *signal_disable_;
    }

    signal<void()> & signal_enable() {
        if (!signal_enable_) { signal_enable_ = new signal<void()>; }
        return *signal_enable_;
    }

    signal<void()> & signal_show() {
        if (!signal_show_) { signal_show_ = new signal<void()>; }
        return *signal_show_;
    }

    signal<void()> & signal_hide() {
        if (!signal_hide_) { signal_hide_ = new signal<void()>; }
        return *signal_hide_;
    }

    signal<void(Accel &)> & signal_accel_added() {
        if (!signal_accel_added_) { signal_accel_added_ = new signal<void(Accel &)>; }
        return *signal_accel_added_;
    }

    signal<void(Accel &)> & signal_accel_removed() {
        if (!signal_accel_removed_) { signal_accel_removed_ = new signal<void(Accel &)>; }
        return *signal_accel_removed_;
    }

    signal<void(const ustring &)> & signal_label_changed() {
        if (!signal_label_changed_) { signal_label_changed_ = new signal<void(const ustring &)>; }
        return *signal_label_changed_;
    }

    signal<void(const ustring &)> & signal_icon_changed() {
        if (!signal_icon_changed_) { signal_icon_changed_ = new signal<void(const ustring &)>; }
        return *signal_icon_changed_;
    }

    signal<void(const ustring &)> & signal_tooltip_changed() {
        if (!signal_tooltip_changed_) { signal_tooltip_changed_ = new signal<void(const ustring &)>; }
        return *signal_tooltip_changed_;
    }
};

Action_base::Action_base():
    trackable(),
    data(new Data)
{
}

Action_base::Action_base(const ustring & accels):
    trackable(),
    data(new Data)
{
    add_accels(accels);
}

Action_base::Action_base(const Action_base & other):
    trackable(),
    data(new Data(*other.data))
{
}

Action_base::Action_base(Action_base && other):
    trackable(),
    data(new Data(std::move(*other.data)))
{
}

Action_base & Action_base::operator=(const Action_base & other) {
    if (this != &other) { *data = *other.data; }
    return *this;
}

Action_base & Action_base::operator=(Action_base && other) {
    if (this != &other) { *data = std::move(*other.data); }
    return *this;
}

Action_base::Action_base(char32_t kc, int km):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
}

Action_base::Action_base(const ustring & accels, const ustring & label):
    trackable(),
    data(new Data)
{
    add_accels(accels);
    set_label(label);
}

Action_base::Action_base(char32_t kc, int km, const ustring & label):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
    set_label(label);
}

Action_base::Action_base(const ustring & accels, const ustring & label, const ustring & icon_name):
    trackable(),
    data(new Data)
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
}

Action_base::Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
}

Action_base::Action_base(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable(),
    data(new Data)
{
    add_accels(accels);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Action_base::Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip):
    trackable(),
    data(new Data)
{
    add_accel(kc, km);
    set_label(label);
    set_icon_name(icon_name);
    set_tooltip(tooltip);
}

Action_base::Action_base(Master_action & master_action):
    trackable(),
    data(new Data)
{
    set_master_action(master_action);
}

Action_base::~Action_base() {
    data->signal_destroy_();
    show();
    appear();
    enable();
    thaw();
    delete data;
}

void Action_base::set_label(const ustring & label) {
    if (data->label_ != label) {
        data->label_ = label;
        if (data->signal_label_changed_) { (*data->signal_label_changed_)(data->label_); }
    }
}

ustring Action_base::label() const {
    return data->label_;
}

void Action_base::enable() {
    if (data->disabled_) {
        data->disabled_ = false;
        if (!data->frozen_) { on_enable(); if (data->signal_enable_) (*data->signal_enable_)(); }
    }
}

void Action_base::thaw() {
    if (data->frozen_) {
        data->frozen_ = false;
        if (!data->disabled_) { on_enable(); if (data->signal_enable_) (*data->signal_enable_)(); }
    }
}

void Action_base::disable() {
    if (!data->disabled_) {
        data->disabled_ = true;
        if (!data->frozen_) { on_disable(); if (data->signal_disable_) (*data->signal_disable_)(); }
    }
}

void Action_base::freeze() {
    if (!data->frozen_) {
        data->frozen_ = true;
        if (!data->disabled_) { on_disable(); if (data->signal_disable_) (*data->signal_disable_)(); }
    }
}

bool Action_base::enabled() const {
    return !data->disabled_ && !data->frozen_;
}

void Action_base::show() {
    if (data->hidden_) {
        data->hidden_ = false;
        if (!data->disappeared_ && data->signal_show_) { (*data->signal_show_)(); }
    }
}

void Action_base::appear() {
    if (data->disappeared_) {
        data->disappeared_ = false;
        if (!data->hidden_ && data->signal_show_) { (*data->signal_show_)(); }
    }
}

void Action_base::hide() {
    if (!data->hidden_) {
        data->hidden_ = true;
        if (!data->disappeared_ && data->signal_hide_) { (*data->signal_hide_)(); }
    }
}

void Action_base::disappear() {
    if (!data->disappeared_) {
        data->disappeared_ = true;
        if (!data->hidden_ && data->signal_hide_) { (*data->signal_hide_)(); }
    }
}

bool Action_base::visible() const {
    return !data->hidden_ && !data->disappeared_;
}

void Action_base::set_icon_name(const ustring & icon_name) {
    if (data->icon_name_ != icon_name) {
        data->icon_name_ = icon_name;
        if (data->signal_icon_changed_) { (*data->signal_icon_changed_)(data->icon_name_); }
    }
}

ustring Action_base::icon_name() const {
    return data->icon_name_;
}

void Action_base::set_tooltip(const ustring & tooltip) {
    if (data->tooltip_ != tooltip) {
        data->tooltip_ = tooltip;
        if (data->signal_tooltip_changed_) { (*data->signal_tooltip_changed_)(data->tooltip_); }
    }
}

void Action_base::unset_tooltip() {
    if (!data->tooltip_.empty()) {
        data->tooltip_.clear();
        if (data->signal_tooltip_changed_) { (*data->signal_tooltip_changed_)(data->tooltip_); }
    }
}

ustring Action_base::tooltip() const {
    return data->tooltip_;
}

void Action_base::add_accel(char32_t kc, int km) {
    if (KC_NONE != kc) {
        Accel accel(kc, km);
        auto i = std::find(data->accels_.begin(), data->accels_.end(), accel);

        if (i == data->accels_.end()) {
            data->accels_.emplace_back(accel);
            data->accels_.back().connect(fun(this, &Action_base::on_accel));
            if (data->signal_accel_added_) { (*data->signal_accel_added_)(data->accels_.back()); }
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
    auto i = std::find(data->accels_.begin(), data->accels_.end(), accel);

    if (i != data->accels_.end()) {
        if (data->signal_accel_removed_) { (*data->signal_accel_removed_)(*i); }
        data->accels_.erase(i);
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
    if (data->signal_accel_removed_) {
        for (auto & accel: data->accels_) {
            (*data->signal_accel_removed_)(accel);
        }
    }

    data->accels_.clear();
}

void Action_base::on_enable() {
    for (auto & accel: data->accels_) { accel.enable(); }
}

void Action_base::on_disable() {
    for (auto & accel: data->accels_) { accel.disable(); }
}

void Action_base::set_master_action(Master_action & master_action) {
    data->accel_added_cx_ = master_action.signal_accel_added().connect(fun(this, &Action_base::on_accel_added));
    data->accel_removed_cx_ = master_action.signal_accel_removed().connect(fun(this, &Action_base::on_accel_removed));
    data->enable_cx_ = master_action.signal_enable().connect(fun(this, &Action_base::thaw));
    data->disable_cx_ = master_action.signal_disable().connect(fun(this, &Action_base::freeze));
    data->show_cx_ = master_action.signal_show().connect(fun(this, &Action_base::appear));
    data->hide_cx_ = master_action.signal_hide().connect(fun(this, &Action_base::disappear));
    data->label_changed_cx_ = master_action.signal_label_changed().connect(fun(this, &Action_base::set_label));
    data->icon_changed_cx_ = master_action.signal_icon_changed().connect(fun(this, &Action_base::set_icon_name));
    data->tooltip_changed_cx_ = master_action.signal_tooltip_changed().connect(fun(this, &Action_base::set_tooltip));

    clear_accels();

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

void Action_base::set_master_action(Master_action * master_action) {
    if (master_action) {
        set_master_action(*master_action);
    }
}

void Action_base::set_master_action(const ustring & name) {
    set_master_action(Theme_impl::root()->find_action(name));
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
    for (auto & accel: data->accels_) {
        if (accel.equals(kc, km)) {
            return this;
        }
    }

    return nullptr;
}

std::vector<Accel> & Action_base::accels() {
    return data->accels_;
}

signal<void()> & Action_base::signal_disable() {
    return data->signal_disable();
}

signal<void()> & Action_base::signal_enable() {
    return data->signal_enable();
}

signal<void()> & Action_base::signal_hide() {
    return data->signal_hide();
}

signal<void()> & Action_base::signal_show() {
    return data->signal_show();
}

signal<void(Accel &)> & Action_base::signal_accel_added() {
    return data->signal_accel_added();
}

signal<void(Accel &)> & Action_base::signal_accel_removed() {
    return data->signal_accel_removed();
}

signal<void(const ustring &)> & Action_base::signal_label_changed() {
    return data->signal_label_changed();
}

signal<void(const ustring &)> & Action_base::signal_icon_changed() {
    return data->signal_icon_changed();
}

signal<void(const ustring &)> & Action_base::signal_tooltip_changed() {
    return data->signal_tooltip_changed();
}

signal<void()> & Action_base::signal_destroy() {
    return data->signal_destroy_;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Action::Action():
    Action_base()
{
}

Action::~Action() {}

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

bool Action::connected() const {
    return data->connected() || !signal_activate_.empty();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Toggle_action::Toggle_action():
    Action_base()
{
}

Toggle_action::~Toggle_action() {}

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

bool Toggle_action::connected() const {
    return data->connected() || !signal_toggle_.empty();
}

} // namespace tau

//END
