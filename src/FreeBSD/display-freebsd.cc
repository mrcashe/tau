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

#include <tau/sys.hh>
#include <tau/string.hh>
#include <xcb/cursor-xcb.hh>
#include <xcb/pixmap-xcb.hh>
#include "display-freebsd.hh"
#include <fstream>
#include <iostream>

namespace {

using Mutex = std::recursive_mutex;
using Lock  = std::lock_guard<Mutex>;
using Displays = std::map<std::thread::id, tau::Display_freebsd_ptr>;

Mutex           smx_;
Displays        dps_;
std::atomic_int dpcnt_;

tau::Display_freebsd_ptr new_display(std::thread::id tid, const tau::ustring & args) {
    auto dp = std::make_shared<tau::Display_freebsd>(tid, args);
    Lock lk(smx_);
    dps_[tid] = dp;
    ++dpcnt_;
    return dp;
}

} // anonymous namespace

namespace tau {

Display_freebsd::Display_freebsd(std::thread::id tid, const ustring & args):
    Display_xcb(),
    loop_(Loop_freebsd::this_freebsd_loop())
{
    tid_ = tid;
    dpid_ = dpcnt_;
    open(args);
}

// Overrides pure Display_impl.
void Display_freebsd::done() {
    Lock lk(smx_);
    dps_.erase(tid_);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// static
Display_freebsd_ptr Display_freebsd::this_freebsd_display() {
    std::thread::id tid = std::this_thread::get_id();
    Lock lk(smx_);
    auto i = dps_.find(tid);
    if (dps_.end() != i) { return i->second; }
    smx_.unlock();
    return new_display(tid, ustring());
}

// static
Display_ptr Display_impl::open(const ustring & args) {
    std::thread::id tid = std::this_thread::get_id();
    Lock lk(smx_);
    auto i = dps_.find(tid);
    if (dps_.end() != i) { return i->second; }
    smx_.unlock();
    return new_display(tid, args);
}

// static
Display_ptr Display_impl::this_display() {
    return Display_freebsd::this_freebsd_display();
}

// static
Pixmap_ptr Pixmap_impl::create(int depth, const Size & sz) {
    return std::make_shared<Pixmap_xcb>(depth, sz);
}

// static
Cursor_ptr Cursor_impl::create() {
    return std::make_shared<Cursor_xcb>();
}

} // namespace tau

//END
