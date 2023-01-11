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

#include <tau/sys.hh>
#include <unistd.h>
#include <sys/sysctl.h>
#include <iostream>
#include <mutex>

namespace {

using Mutex = std::recursive_mutex;
using Lock  = std::lock_guard<Mutex>;

Mutex mx_;
tau::ustring path_self_;

} // anonymous namespace

namespace tau {

ustring path_self() {
    ustring s; { Lock lk(mx_); s = path_self_; }
    if (!s.empty()) { return s; }

    int oid[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, getpid() };
    size_t oldlen = 0;
    int result = sysctl(oid, 4, NULL, &oldlen, NULL, 0);

    if (0 == result && 0 != oldlen) {
        char buffer[oldlen+1];
        result = sysctl(oid, 4, buffer, &oldlen, NULL, 0);

        if (0 == result) {
            s = buffer;
            Lock lk(mx_);
            path_self_ = s;
            return s;
        }
    }

    std::cerr << "** path_self(): sysctl() failed: " << strerror(errno) << std::endl;
    return ustring();
}

std::vector<ustring> list_removable_drives() {
    std::vector<ustring> v;

    return v;
}

} // namespace tau

//END
