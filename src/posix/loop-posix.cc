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

#include "loop-posix.hh"
#include <iostream>

namespace tau {

// Overrides pure Loop_impl.
bool Loop_posix::iterate(int timeout_ms) {
    int nfd = poll(fds_.data(), fds_.size(), timeout_ms);

    if (nfd > 0) {
        for (auto & pfd: fds_) {
            if (pfd.revents) {
                signal_chain_poll_(pfd.fd);
            }
        }

        return true;
    }

    return false;
}

void Loop_posix::add_poller(Poller_base * ppi, short events) {
    fds_.emplace_back();
    fds_.back().fd = ppi->fd();
    fds_.back().events = events;
    ppi->signal_destroy().connect(tau::bind(fun(this, &Loop_posix::on_poller_destroy), ppi->fd()));
    signal_chain_poll_.connect(fun(ppi, &Poller_base::on_poll));
}

// Overrides pure Loop_impl.
Event_ptr Loop_posix::create_event() {
    auto evp = std::make_shared<Event_posix>();
    evp->signal_ready().connect(fun(evp, &Event_posix::release));
    add_poller(evp.get(), POLLIN);
    return evp;
}

void Loop_posix::on_poller_destroy(int fd) {
    auto * p = fds_.data();

    for (std::size_t i = 0; i < fds_.size(); i++) {
        if (p->fd == fd) {
            fds_.erase(fds_.begin()+i);
            return;
        }
    }
}

bool Loop_posix::is_removable(const ustring & mp) {
    auto i = std::find_if(mounts_.begin(), mounts_.end(), [mp](const Mount & m) { return mp == m.mpoint; } );
    return i != mounts_.end() && i->removable;
}

} // namespace tau

//END
