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

#include <tau/signal.hh>
#include <algorithm>

namespace tau {

trackable::trackable(const trackable & src) {}

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

connection slot_base::cx() {
    return connection(impl_);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

connection::connection(slot_ptr slot):
    slot_(slot)
{
}

void connection::disconnect() {
    if (slot_) {
        slot_->untrack();
        slot_->disconnect();
        slot_->reset();
        slot_.reset();
    }
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

slot_impl::slot_impl(slot_base * slot):
    slot_(slot)
{
}

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
void slot_impl::link(slot_base * slot) {
    slot_ = slot;
}

// Called by connection and trackable.
void slot_impl::disconnect() {
    if (auto s = slot_) {
        slot_ = nullptr;
        s->disconnect();
    }
}

void slot_impl::track() {
    if (target()) {
        target()->track(this);
    }
}

void slot_impl::untrack() {
    if (target()) {
        target()->untrack(this);
    }
}

} // namespace tau

//END
