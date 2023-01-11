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

#include <posix/fileinfo-posix.hh>
#include "loop-linux.hh"

namespace tau {

class Fileinfo_linux: public Fileinfo_posix {
public:

    explicit Fileinfo_linux(const ustring & uri):
        Fileinfo_posix(uri)
    {
    }

    // Overrides pure Fileinfo_impl.
    signal<void(int, ustring)> & signal_watch(int event_mask) override {
        if (!loop_) { loop_ = Loop_linux::this_linux_loop(); }
        if (!mon_) { mon_ = loop_->create_file_monitor(uri_, event_mask); }
        if (mon_) { mon_->signal_notify().connect(fun(signal_watch_)); }
        return signal_watch_;
    }

private:

    signal<void(int, ustring)> signal_watch_;
    Loop_linux_ptr   loop_;
    File_monitor_ptr mon_;
};

Fileinfo_ptr Fileinfo_impl::create(const ustring & uri) {
    return std::make_shared<Fileinfo_linux>(uri);
}

} // namespace tau

//END
