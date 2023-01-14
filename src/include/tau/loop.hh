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

/// @file loop.hh The Loop class.

#ifndef TAU_LOOP_HH
#define TAU_LOOP_HH

#include <tau/types.hh>
#include <tau/signal.hh>

namespace tau {

/// The event loop.
/// @ingroup sys_group
class Loop {
public:

    /// Default constructor creates a current thread %Loop.
    Loop();

    /// Copy constructor.
    Loop(const Loop & other) = default;

    /// Copy operator.
    Loop & operator=(const Loop & other) = default;

    /// Run loop.
    void run();

    /// Quit loop.
    void quit();

    /// Test if alive.
    bool alive() const;

    /// Test if running.
    bool running() const;

    /// Create event.
    Event create_event();

    /// Create event.
    /// @param slot_ready slot to be called when event become ready.
    Event create_event(const slot<void()> & slot_ready);

    /// Signal emitted when first instance of run() method start its work.
    signal<void()> & signal_start();

    /// Signal emitted when there are no events happened during last idle timeout.
    signal<void()> & signal_idle();

    /// Signal emitted when first instance of run() method quits.
    signal<void()> & signal_quit();

    /// Signal alarm.
    signal<void()> & signal_alarm(int timeout_ms, bool periodical=false);

    /// Signal emitted when disk partititon mounted or unmounted.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_mount(int file_flags, const ustring & path).
    /// ~~~~~~~~~~~~~~~
    /// The meaning of file_flags can be found at #File_flags enum.
    signal<void(int, ustring)> & signal_mount();

private:

    Loop_ptr impl;
};

} // namespace tau

#endif // TAU_LOOP_HH
