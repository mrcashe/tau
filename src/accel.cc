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

#include <tau/accel.hh>
#include <tau/input.hh>
#include <iostream>

namespace tau {

Accel::Accel(char32_t kc, int km) {
    assign(kc, km);
}

Accel::Accel(char32_t kc, int km, slot<bool()> slot_activate) {
    assign(kc, km);
    connect(slot_activate);
}

Accel::Accel(const ustring & spec) {
    assign(spec);
}

Accel::Accel(const ustring & spec, slot<bool()> slot_activate) {
    assign(spec);
    connect(slot_activate);
}

bool Accel::handle_accel(char32_t kc, int km) {
    return !disabled_ && kc_ && kc == kc_ && km == km_ && !signal_activate_.empty() && signal_activate_();
}

void Accel::assign(char32_t kc, int km) {
    if (kc != kc_ || km != km_) {
        kc_ = kc;
        km_ = km;
        signal_changed_();
    }
}

Accel::operator bool() const {
    return !empty();
}

bool Accel::operator==(const Accel & other) {
    return equals(other.spec());
}

bool Accel::operator!=(const Accel & other) {
    return !equals(other.spec());
}

void Accel::assign(const ustring & spec) {
    key_spec_from_string(spec, kc_, km_);
    if (0 == kc_) { std::cerr << "** Accel::assign(spec): failed to resolve specification " << spec << std::endl; }
}

ustring Accel::label() const {
    return key_spec_to_label(kc_, km_);
}

void Accel::enable() {
    if (disabled_) {
        disabled_ = false;
        signal_enable_();
    }
}

void Accel::disable() {
    if (!disabled_) {
        disabled_ = true;
        signal_disable_();
    }
}

bool Accel::enabled() const {
    return !disabled_;
}

bool Accel::empty() const {
    return 0 == kc_;
}

char32_t Accel::key_code() const {
    return kc_;
}

int Accel::key_modifier() const {
    return km_;
}

void Accel::get_keys(char32_t & kc, int & km) const {
    kc = kc_;
    km = km_;
}

ustring Accel::spec() const {
    return key_spec_to_string(kc_, km_);
}

bool Accel::equals(char32_t kc, int km) const {
    return kc_ == kc && km_ == km;
}

bool Accel::equals(const ustring & spec) const {
    char32_t kc;
    int      km;
    key_spec_from_string(spec, kc, km);
    return equals(kc, km);
}

connection Accel::connect(slot<bool()> slot_activate) {
    return signal_activate_.connect(slot_activate);
}

signal<void()> & Accel::signal_changed() {
    return signal_changed_;
}

signal<void()> & Accel::signal_enable() {
    return signal_enable_;
}

signal<void()> & Accel::signal_disable() {
    return signal_disable_;
}

} // namespace tau

//END
