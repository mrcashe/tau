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

#include <tau/event.hh>
#include <tau/loop.hh>
#include <tau/timer.hh>
#include <tau/ustring.hh>
#include <event-impl.hh>
#include <loop-impl.hh>

namespace tau {

Loop::Loop():
    impl(Loop_impl::this_loop())
{
}

void Loop::run() {
    impl->run();
}

void Loop::quit() {
    impl->quit();
}

bool Loop::running() const {
    return impl->running();
}

bool Loop::alive() const {
    return impl->alive();
}

Event Loop::create_event() {
    return Event_impl::wrap(impl->create_event());
}

Event Loop::create_event(const slot<void()> & slot_ready) {
    return Event_impl::wrap(impl->create_event(slot_ready));
}

signal<void()> & Loop::signal_start() {
    return impl->signal_start();
}

signal<void()> & Loop::signal_idle() {
    return impl->signal_idle();
}

signal<void()> & Loop::signal_quit() {
    return impl->signal_quit();
}

signal<void(int, ustring)> & Loop::signal_mount() {
    return impl->signal_mount();
}

signal<void()> & Loop::signal_alarm(int timeout_ms, bool periodical) {
    return impl->signal_alarm(timeout_ms, periodical);
}

} // namespace tau

//END
