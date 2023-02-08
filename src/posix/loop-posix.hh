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

#ifndef TAU_LOOP_POSIX_HH
#define TAU_LOOP_POSIX_HH

#include "types-posix.hh"
#include <tau/ustring.hh>
#include <event-impl.hh>
#include <loop-impl.hh>
#include <poll.h>

namespace tau {

class Poller_base {
public:

    virtual ~Poller_base() {}
    virtual int fd() const = 0;
    virtual signal<void()> & signal_poll() = 0;

    bool on_poll(int pfd) {
        if (fd() == pfd) {
            signal_poll()();
            return true;
        }

        return false;
    }

    signal<void()> & signal_destroy() {
        return signal_destroy_;
    }

protected:

    signal<void()> signal_destroy_;
};

class Poller_posix: public Poller_base {
    int fd_ = -1;
    signal<void()> signal_poll_;

public:

    Poller_posix(int fd): fd_(fd) {}
   ~Poller_posix() { signal_destroy_(); }
    int fd() const override { return fd_; }
    signal<void()> & signal_poll() override { return signal_poll_; }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Event_posix: public Event_impl, public Poller_base {
public:

    Event_posix();
   ~Event_posix();

    // Overrides Event_impl.
    void emit() override;

    // Overrides pure Event_impl.
    void release() override;

    int fd() const override { return fds_[0]; }
    signal<void()> & signal_poll() override { return signal_ready_; }

private:

    int fds_[2] { -1, -1 };
    signal<void()> signal_destroy_;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Loop_posix: public Loop_impl {
public:

    static Loop_posix_ptr this_posix_loop();

    bool is_removable(const ustring & mp);

    // Overrides pure Loop_impl.
    Event_ptr create_event() override;

    void add_poller(Poller_base * ppi, short events);

protected:

    struct Mount {
        ustring     dev;
        ustring     mpoint;
        bool        removable = false;
    };

    using Mounts = std::list<Mount>;

    Mounts          mounts_;

protected:

    Loop_posix() = default;

    // Overrides pure Loop_impl.
    void iterate(int timeout_ms) override;

private:

    using Fds = std::vector<struct pollfd>;

    Fds fds_;
    signal<bool(int)> signal_chain_poll_;

private:

    void on_poller_destroy(int fd);
};

} // namespace tau

#endif // TAU_LOOP_POSIX_HH
