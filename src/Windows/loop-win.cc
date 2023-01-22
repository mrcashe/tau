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
#include <tau/locale.hh>
#include <tau/sys.hh>
#include "loop-win.hh"
#include <atomic>
#include <iostream>
#include <map>
#include <mutex>

namespace {

using Mutex = std::recursive_mutex;
using Lock  = std::lock_guard<Mutex>;
using Loops = std::map<std::thread::id, tau::Loop_win_ptr>;

Mutex           smx_;
Loops           loops_;
std::atomic_int loopcnt_;

} // anonymous namespace

namespace tau {

Loop_win::Loop_win(std::thread::id tid):
    Loop_impl()
{
    tid_ = tid;
    id_ = loopcnt_;
    signal_quit_.connect(fun(this, &Loop_win::done));
    drives_ = list_drives();
}

void Loop_win::done() {
    Lock lk(smx_);
    loops_.erase(tid_);
}

// Overrides pure Loop_impl.
bool Loop_win::iterate(int timeout_ms) {
    std::size_t n = std::min(std::size_t(MAXIMUM_WAIT_OBJECTS), handles_.size());
    DWORD result = MsgWaitForMultipleObjects(n, handles_.data(), false, timeout_ms, QS_ALLINPUT);
    if (WAIT_TIMEOUT == result) { return false; }

    if (0 != n && result < WAIT_OBJECT_0+n) {
        HANDLE handle = handles_[result-WAIT_OBJECT_0];
        signal_chain_poll_(handle);
    }

    MSG msg;

    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return true;
}

// Overrides pure Loop_impl.
Event_ptr Loop_win::create_event() {
    auto evp = std::make_shared<Event_win>();
    HANDLE handle = evp->handle();
    handles_.push_back(handle);
    signal_chain_poll_.connect(fun(evp, &Event_win::on_poll));
    evp->signal_ready().connect(fun(evp, &Event_win::release));
    evp->signal_destroy().connect(tau::bind(fun(this, &Loop_win::on_handle_die), handle));
    return evp;
}

// Overrides pure Loop_impl.
File_monitor_ptr Loop_win::create_file_monitor(const ustring & path, int umask) {
    auto fm = std::make_shared<File_monitor_win>(path, umask);
    HANDLE handle = fm->handle();
    handles_.push_back(handle);
    signal_chain_poll_.connect(fun(fm, &File_monitor_win::on_poll));
    fm->signal_destroy().connect(tau::bind(fun(this, &Loop_win::on_handle_die), handle));
    return fm;
}

// Overrides pure Loop_impl.
std::vector<ustring> Loop_win::mounts() {
    std::vector<ustring>  v;
    for (const ustring & s: drives_) { v.push_back(s); }
    return v;
}

void Loop_win::on_handle_die(HANDLE handle) {
    auto p = handles_.data();

    for (std::size_t i = 0; i < handles_.size(); i++, p++) {
        if (*p == handle) {
            handles_.erase(handles_.begin()+i);
            break;
        }
    }
}

void Loop_win::handle_mount() {
    Drives drives = list_drives();

    for (const ustring & drv: drives) {
        if (drives_.end() == std::find(drives_.begin(), drives_.end(), drv)) {
            drives_.push_back(drv);
            int flags = FILE_MOUNT;
            signal_mount_(flags, drv);
        }
    }
}

void Loop_win::handle_umount() {
    Drives drives = list_drives();
    bool rem;

    do {
        rem = false;

        for (auto i = drives_.begin(); i != drives_.end(); ++i) {
            ustring s = *i;

            if (drives.end() == std::find(drives.begin(), drives.end(), s)) {
                rem = true;
                drives_.erase(i);
                int flags = FILE_MOUNT;
                signal_mount_(flags, s);
                break;
            }
        }

    } while (rem);
}

Loop_win::Drives Loop_win::list_drives() const {
    std::list<ustring> v;
    DWORD nbytes = GetLogicalDriveStringsA(0, NULL);

    if (0 != nbytes) {
        Locale loc;
        char buf[1+nbytes];
        GetLogicalDriveStringsA(1+nbytes, buf);

        for (std::size_t i = 0; i < nbytes; ++i) {
            if ('\0' != buf[i]) {
                UINT drive_type = GetDriveType(buf+i);

                if (DRIVE_UNKNOWN != drive_type && DRIVE_NO_ROOT_DIR != drive_type) {
                    v.push_back(loc.io_encode(buf+i));
                }
            }

            while ('\0' != buf[i]) { ++i; }
        }
    }

    return v;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// static
Loop_win_ptr Loop_win::this_win_loop() {
    std::thread::id tid = std::this_thread::get_id();
    Lock lk(smx_);
    auto i = loops_.find(tid);
    if (loops_.end() != i) { return i->second; }
    smx_.unlock();
    auto lp = std::make_shared<Loop_win>(tid);
    smx_.lock();
    loops_[tid] = lp;
    if (0 == loopcnt_) { lp->boot(); }
    ++loopcnt_;
    return lp;
}

// static
Loop_ptr Loop_impl::this_loop() {
    return Loop_win::this_win_loop();
}

// static
Loop_ptr Loop_impl::that_loop(std::thread::id tid) {
    Lock lk(smx_);
    auto i = loops_.find(tid);
    return loops_.end() != i ? i->second : nullptr;
}

} // namespace tau

//END
