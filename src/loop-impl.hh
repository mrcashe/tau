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

#ifndef TAU_LOOP_IMPL_HH
#define TAU_LOOP_IMPL_HH

#include <types-impl.hh>
#include <tau/signal.hh>
#include <vector>
#include <thread>

namespace tau {

class Loop_impl: public trackable {
public:

    Loop_impl(const Loop_impl & other) = delete;
    Loop_impl(Loop_impl && other) = delete;
    Loop_impl & operator=(const Loop_impl & other) = delete;
    Loop_impl & operator=(Loop_impl && other) = delete;
    virtual ~Loop_impl() {}

    static Loop_ptr this_loop();
    static Loop_ptr that_loop(std::thread::id tid);

    void run();
    void quit();
    int id() const { return id_; }
    bool running() const { return 0 != runlevel_; }
    bool alive() const { return runlevel_ >= 0; }
    void start_timer(Timer_ptr tp);
    void stop_timer(Timer_impl * tpi);

    virtual File_monitor_ptr create_file_monitor(const ustring & path, int event_mask) = 0;
    virtual Event_ptr create_event() = 0;
    virtual std::vector<ustring> mounts() = 0;

    signal<void()> & signal_start() { return signal_start_; }
    signal<void()> & signal_idle() { return signal_idle_; }
    signal<void()> & signal_run() { return signal_run_; }
    signal<void()> & signal_quit() { return signal_quit_; }
    signal<void(int, const ustring &)> & signal_mount() { return signal_mount_; }
    signal<void()> & signal_alarm(int timeout_ms, bool periodical=false);

protected:

    Loop_impl() = default;

protected:

    using Timers = std::list<Timer_ptr>;

    int             runlevel_ = 0;
    Timers          timers_;
    uint64_t        uidle_ = 200000;    // Idle timeout in microseconds.
    uint64_t        next_idle_ = 0;
    std::thread::id tid_;
    int             id_ = -1;

    signal<void()>  signal_start_;
    signal<void()>  signal_idle_;
    signal<void()>  signal_run_;
    signal<void()>  signal_quit_;
    signal<void(int, const ustring &)> signal_mount_;

protected:

    /// Do one iteration of run cycle.
    /// @param timeout_ms the timeout in milliseconds.
    /// @return true if some event was handled or false if timeout has been expired.
    virtual bool iterate(int timeout_ms) = 0;

    /// Run once.
    void boot();

private:

    /// Linkage specific boot.
    void boot_linkage();
};

} // namespace tau

#endif // TAU_LOOP_IMPL_HH
