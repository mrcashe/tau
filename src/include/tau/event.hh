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

/// @file event.hh Event class.

#ifndef TAU_EVENT_HH
#define TAU_EVENT_HH

#include <tau/types.hh>
#include <tau/signal.hh>

namespace tau {

/// %Event.
/// %Event can be emitted by non-GUI thread.
/// After emission an event loop emits signal_activate().
/// @ingroup sys_group
class Event {
public:

    /// Constructs an empty %Event.
    Event() {}

    /// Copy constructor.
    Event(const Event & other) = default;

    /// Copy operator.
    Event & operator=(const Event & other) = default;

    /// Test if empty.
    operator bool() const { return nullptr != impl; }

    /// Reset implementation pointer.
    void reset() { impl.reset(); }

    /// Set event to signalled state.
    void emit();

    /// Reset event state.
    void unset();

    /// Get "signal_ready" reference.
    signal<void()> & signal_ready();

private:

    Event_ptr impl;
    Event(Event_ptr evp);
    friend class Event_impl;
};

} // namespace tau

#endif // TAU_EVENT_HH
