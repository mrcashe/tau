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
#include <tau/exception.hh>
#include <tau/timer.hh>
#include <tau/timeval.hh>
#include <event-impl.hh>
#include <loop-impl.hh>
#include <timer-impl.hh>
#include <sys-impl.hh>
#include <iomanip>
#include <iostream>

namespace tau {

void Loop_impl::boot() {
    sysinfo_.abits = 8*sizeof(void*);
    sysinfo_.ibits = 8*sizeof(int);
    sysinfo_.lbits = 8*sizeof(long);
    sysinfo_.llbits = 8*sizeof(long long);
    sysinfo_.mbits = 8*sizeof(intmax_t);
    boot_linkage();
}

void Loop_impl::start_timer(Timer_ptr tp) {
    if (runlevel_ >= 0) {
        // Check existance.
        if (timers_.end() != std::find(timers_.begin(), timers_.end(), tp)) { return; }
        tp->time_point = Timeval::future(1000*tp->time_ms);
        tp->running = true;

        // No timers -> put first and last same time.
        if (timers_.empty()) { timers_.push_front(tp); return; }

        // Put in front of queue.
        if (tp->time_point <= timers_.front()->time_point) { timers_.push_front(tp); return; }

        // Put behind the last.
        if (tp->time_point > timers_.back()->time_point) { timers_.push_back(tp); return; }

        // Search throught the list.
        for (auto iter = timers_.begin(); iter != timers_.end(); ++iter) {
            if (tp->time_point <= (*iter)->time_point) {
                timers_.insert(iter, tp);
                return;
            }
        }
    }
}

void Loop_impl::stop_timer(Timer_impl * tpi) {
    if (tpi) {
        tpi->running = false;
        timers_.remove_if([tpi](Timer_ptr tp) { return tp.get() == tpi; } );
    }
}

signal<void()> & Loop_impl::signal_alarm(int timeout_ms, bool periodical) {
    Timer_ptr tp = std::make_shared<Timer_impl>();
    tp->time_ms = std::max(1, timeout_ms);
    tp->periodical = periodical;
    start_timer(tp);
    return tp->signal_alarm;
}

void Loop_impl::run() {
    if (runlevel_ < 0) { throw user_error("Loop_impl::run(): attempt to rerun dead loop"); }
    int runlevel = ++runlevel_;
    if (1 == runlevel) { signal_start_(); }
    next_idle_ = Timeval::future(uidle_);

    uint64_t now, ts, dts;
    bool     run;

    while (runlevel_ >= runlevel) {
        run = false;
        now = Timeval::now();
        ts = next_idle_ >= now ? next_idle_ : now;

        // Have pending timer at the front of timer queue?
        if (!timers_.empty()) {
            uint64_t t = timers_.front()->time_point;
            if (t <= now) { ts = now; }
            else if (t < ts) { ts = t; }
        }

        dts = ts-now;

        if (!signal_run_.empty() && dts >= 2000) {
            ts = now+2000;
            dts = ts-now;
            run = true;
        }

        int tw = dts/1000;

        // Do not call iterate() with 0 timeout value!
        if (tw) {
            iterate(tw);
            now = Timeval::now();
        }

        // In result of iterate() call timer queue may be modified, so test it again.
        while (!timers_.empty()) {
            Timer_ptr tp = timers_.front();

            if (now >= tp->time_point) {
                tp->running = false;
                timers_.pop_front();
                tp->signal_alarm();
                if (tp->periodical && !tp->signal_alarm.empty()) { start_timer(tp); }
            }

            else {
                now = Timeval::now();
                break;
            }
        }

        if (run) {
            signal_run_();
            now = Timeval::now();
        }

        if (now >= next_idle_) {
            next_idle_ = now+uidle_;
            signal_idle_();
        }
    }

    if (1 == runlevel) {
        runlevel_ = -1;
        signal_quit_();
        timers_.clear();
    }
}

void Loop_impl::quit() {
    if (runlevel_ > 0) { --runlevel_; }
}

} // namespace tau

//END
