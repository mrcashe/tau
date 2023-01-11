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

#ifndef TAU_TIMER_HH
#define TAU_TIMER_HH

#include <tau/types.hh>
#include <tau/signal.hh>

/// @file timer.hh Timer class.

namespace tau {

/// Restartable timer.
/// @ingroup time_group
class Timer {
public:

    /// Default constructor.
    Timer();

    /// Constructor with parameters.
    /// @param slot_alarm slot to be connected to signal_alarm().
    /// @param time_ms the time in milliseconds, timer will be stopped if vale of 0 specified.
    /// @param periodical if true, timer will autorestarts after signal emission.
    Timer(slot<void()> slot_alarm, int time_ms=0, bool periodical=false);

    /// Copy constructor.
    /// This class has implementation member placed into std::shared_ptr<> smart pointer,
    /// so copying object of this class simply increases use count on implementation.
    Timer(const Timer & other) = default;

    /// Copy operator.
    /// This class has implementation member placed into std::shared_ptr<> smart pointer,
    /// so copying object of this class simply increases use count on implementation.
    Timer & operator=(const Timer & other) = default;

    /// Start if not running.
    void start(int time_ms, bool periodical=false);

    /// Start with new period.
    void restart(int time_ms, bool periodical=false);

    /// Stop the timer.
    void stop();

    /// Test if running.
    bool running() const;

    /// Signal emitted on alarm.
    signal<void()> & signal_alarm();

private:

    Timer_ptr impl;
};

} // namespace tau

#endif // TAU_TIMER_HH
