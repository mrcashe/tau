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

#include <tau/signal.hh>
#include <algorithm>
#include <iostream>

namespace tau {

trackable::trackable() {}

trackable::trackable(const trackable & src) {}

trackable::trackable(trackable && src) {}

trackable::~trackable() {
    for (auto s: slots_) { s->reset(); s->disconnect(); }
}

void trackable::track(slot_impl * s) {
    if (std::find(slots_.begin(), slots_.end(), s) == slots_.end()) {
        slots_.push_back(s);
    }
}

void trackable::untrack(slot_impl * s) {
    slots_.remove(s);
}

trackable & trackable::operator=(const trackable & src) {
    return *this;
}

trackable & trackable::operator=(trackable && src) {
    return *this;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

signal_base::~signal_base() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

slot_base::~slot_base() {
    if (impl_) {
        impl_->link(nullptr);
    }
}

void slot_base::disconnect() {
    if (auto s = signal_) {
        signal_ = nullptr;
        s->erase(this);
    }
}

void slot_base::link(signal_base * signal) {
    signal_ = signal;
}

connection slot_base::cx() {
    return connection(impl_);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

connection::connection(bool autodrop):
    autodrop_(autodrop)
{
}

connection::~connection() {
    if (autodrop_) {
        // std::cout << this << " connection::drop() " << slot_.get() << "\n";
        drop();
    }
}

connection::connection(const connection & other):
    slot_(other.slot_),
    autodrop_(other.autodrop_)
{
}

connection::connection(connection && other):
    slot_(other.slot_)
{
    other.slot_.reset();
}

connection & connection::operator=(const connection & other) {
    if (this != &other) {
        if (autodrop_) { drop(); }
        slot_ = other.slot_;
    }

    return *this;
}

connection & connection::operator=(connection && other) {
    if (this != &other) {
        if (autodrop_) { drop(); }
        slot_ = other.slot_;
        other.slot_.reset();
    }

    return *this;
}

connection::connection(slot_ptr slot):
    slot_(slot)
{
}

void connection::drop() {
    if (slot_) {
        slot_->untrack();
        slot_->reset();
        slot_->disconnect();
        slot_.reset();
    }
}

void connection::set_autodrop(bool yes) {
    autodrop_ = yes;
}

bool connection::autodrop() const {
    return autodrop_;
}

bool connection::blocked() const {
    return slot_ && slot_->blocked();
}

void connection::block() {
    if (slot_) {
        slot_->block();
    }
}

void connection::unblock() {
    if (slot_) {
        slot_->unblock();
    }
}

bool connection::empty() const {
    return !slot_ || slot_->empty();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

slot_impl::slot_impl(slot_base * base):
    base_(base)
{
}

slot_impl::~slot_impl() {}

bool slot_impl::blocked() const {
    return 0 != blocked_;
}

void slot_impl::block() {
    ++blocked_;
}

void slot_impl::unblock() {
    if (0 != blocked_) {
        --blocked_;
    }
}

// Called by owning slot.
void slot_impl::link(slot_base * base) {
    base_ = base;
    // std::cout << this << " slot_impl::link " << base_ << std::endl;
}

// Called by connection and trackable.
void slot_impl::disconnect() {
    if (auto base = base_) {
        base_ = nullptr;
        base->disconnect();
    }
}

void slot_impl::track(trackable * target) {
    target_ = target;
    if (target_) { target_->track(this); }
}

void slot_impl::untrack() {
    if (target_) {
        target_->untrack(this);
    }
}

void slot_impl::reset() {
    target_ = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

connection signal_base::link(slot_base & slot) {
    slot.link(this);
    return slot.cx();
}

} // namespace tau

//END
