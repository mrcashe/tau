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

#ifndef TAU_LOOP_FREEBSD_HH
#define TAU_LOOP_FREEBSD_HH

#include <tau/sys.hh>
#include <tau/timer.hh>
#include <posix/loop-posix.hh>
#include "types-freebsd.hh"
#include <file-monitor-impl.hh>

namespace tau {

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class File_monitor_freebsd: public File_monitor {
public:

    File_monitor_freebsd(int wd, const ustring & path):
        File_monitor(),
        wd_(wd),
        path_(path)
    {
    }

    ~File_monitor_freebsd() {
        signal_destroy_();
    }

    signal<void()> & signal_destroy() {
        return signal_destroy_;
    }

    int wd() const {
        return wd_;
    }

    bool on_inotify(int wd, const ustring & p, int mask) {
        if (wd == wd_) {
            ustring s = p.empty() ? path_ : (path_is_absolute(p) ? p : path_build(path_, p));
            signal_notify()(mask, s);
            return true;
        }

        return false;
    }

private:

    int            wd_;
    ustring        path_;
    signal<void()> signal_destroy_;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Loop_freebsd: public Loop_posix {
public:

    static Loop_freebsd_ptr this_freebsd_loop();

    Loop_freebsd(std::thread::id tid);

    // Overrides pure Loop.
    File_monitor_ptr create_file_monitor(const ustring & path, int mask) override;

    // Overrides pure Loop_impl.
    std::vector<ustring> mounts() override;

private:

    int             infd_ = -1;
    Poller_posix *  infd_poller_ = nullptr;

    signal<bool(int, ustring, unsigned)> signal_chain_notify_;

private:

    void init_mounts();
    void check_mounts();

    void on_inotify();
    void on_file_monitor_destroy(int wd);
    void done();
};

} // namespace tau

#endif // TAU_LOOP_FREEBSD_HH
