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

#include <tau/exception.hh>
#include "loop-posix.hh"
#include <unistd.h>
#include <fcntl.h>

namespace tau {

Event_posix::Event_posix():
    Event_impl()
{
    if (-1 == ::pipe2(fds_, O_NONBLOCK)) {
        throw sys_error();
    }
}

Event_posix::~Event_posix() {
    close(fds_[1]);
    close(fds_[0]);
    signal_destroy_();
}

// Overrides pure Event_impl.
void Event_posix::emit() {
    char c = '1';

    for (int n = 0; n < 50; ++n) {
        if (1 == ::write(fds_[1], &c, 1)) { break; }
        // We'll retry later.
        usleep(47);
    }
}

// Overrides pure Event_impl.
void Event_posix::release() {
    char c;
    ssize_t num_read;

    for (;;) {
        num_read = ::read(fds_[0], &c, 1);
        if (num_read < 1) { break; }
    }
}

} // namespace tau

//END
