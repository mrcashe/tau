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

#include <tau/timer.hh>
#include <tau/timeval.hh>
#include <loop-impl.hh>
#include <timer-impl.hh>

namespace tau {

Timer::Timer():
    impl(std::make_shared<Timer_impl>())
{
}

Timer::Timer(slot<void()> slot_alarm, int time_ms, bool periodical):
    impl(std::make_shared<Timer_impl>())
{
    impl->signal_alarm.connect(slot_alarm);
    restart(time_ms, periodical);
}

void Timer::start(int time_ms, bool periodical) {
    if (!running()) {
        restart(time_ms, periodical);
    }
}

void Timer::restart(int time_ms, bool periodical) {
    if (time_ms > 0) {
        stop();
        impl->time_ms = time_ms;
        impl->periodical = periodical;
        if (impl->loop) { impl->loop->start_timer(impl); }
    }
}

void Timer::stop() {
    if (running() && impl->loop) {
        impl->loop->stop_timer(impl.get());
    }
}

bool Timer::running() const {
    return impl->running;
}

signal<void()> & Timer::signal_alarm() {
    return impl->signal_alarm;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Timer_impl::Timer_impl():
    trackable()
{
    loop = Loop_impl::this_loop().get();
    loop->signal_quit().connect(fun(this, &Timer_impl::on_loop_quit));
}

void Timer_impl::on_loop_quit() {
    loop = nullptr;
}

} // namespace tau

//END
