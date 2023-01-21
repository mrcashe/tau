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

#include <tau/enums.hh>
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <tau/sys.hh>
#include <sys/inotify.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <atomic>
#include <iostream>
#include <map>
#include <mutex>
#include "loop-freebsd.hh"

namespace {

using Mutex = std::recursive_mutex;
using Lock  = std::lock_guard<Mutex>;
using Loops = std::map<std::thread::id, tau::Loop_freebsd_ptr>;

Mutex           smx_;
Loops           loops_;
std::atomic_int loopcnt_;

} // anonymous namespace

namespace tau {

Loop_freebsd::Loop_freebsd(std::thread::id tid):
    Loop_posix()
{
    tid_ = tid;
    id_ = loopcnt_;
    signal_quit_.connect(fun(this, &Loop_freebsd::done));
    init_mounts();
}

void Loop_freebsd::done() {
    if (infd_poller_) { delete infd_poller_; infd_poller_ = nullptr; }
    if (-1 != infd_) { close(infd_); }
    Lock lk(smx_);
    loops_.erase(tid_);
}

// Overrides pure Loop_impl.
std::vector<ustring> Loop_freebsd::mounts() {
    std::vector<ustring> v;
    for (auto & m: mounts_) { v.push_back(m.mpoint); }
    return v;
}

void Loop_freebsd::on_inotify() {
    char buffer[16384];

    for (;;) {
        ssize_t n_read = read(infd_, buffer, sizeof(buffer));
        if (n_read < 1) { break; }
        std::size_t offset = 0, n_bytes = n_read;

        while (offset < n_bytes) {
            inotify_event * kevent = reinterpret_cast<inotify_event *>(buffer+offset);
            std::size_t event_size = sizeof(inotify_event)+kevent->len;

            unsigned mask = 0;
            if (IN_ACCESS & kevent->mask) { mask = FILE_ACCESSED; }
            if (IN_MODIFY & kevent->mask) { mask = FILE_CHANGED; }
            if (IN_ATTRIB & kevent->mask) { mask = FILE_ATTRIB; }
            if (IN_CLOSE & kevent->mask) { mask = FILE_CLOSED; }
            if (IN_OPEN & kevent->mask) { mask = FILE_OPENED; }
            if (IN_MOVED_FROM & kevent->mask) { mask = FILE_MOVED_OUT; }
            if (IN_MOVED_TO & kevent->mask) { mask = FILE_MOVED_IN; }
            if (IN_CREATE & kevent->mask) { mask = FILE_CREATED; }
            if (IN_DELETE & kevent->mask) { mask = FILE_DELETED; }
            if (IN_DELETE_SELF & kevent->mask) { mask = FILE_SELF_DELETED; }
            if (IN_MOVE_SELF & kevent->mask) { mask = FILE_SELF_MOVED; }
            // Omit IN_UNMOUNT, IN_Q_OVERFLOW and IN_IGNORED.

            if (0 != mask) {
                ustring name;

                if (0 != kevent->len) {
                    std::string s(kevent->name, kevent->len);
                    name = Locale().decode_filename(s);
                }

                signal_chain_notify_(kevent->wd, name, mask);
            }

            offset += event_size;
        }
    }
}

// Overrides pure Loop.
File_monitor_ptr Loop_freebsd::create_file_monitor(const ustring & path, int mask) {
    uint32_t umask = 0;
    if (FILE_ACCESSED & mask) { umask |= IN_ACCESS; }
    if (FILE_ATTRIB & mask) { umask |= IN_ATTRIB; }
    if (FILE_CHANGED & mask) { umask |= IN_MODIFY; }
    if (FILE_OPENED & mask) { umask |= IN_OPEN; }
    if (FILE_CLOSED & mask) { umask |= IN_CLOSE; }
    if (FILE_CREATED & mask) { umask |= IN_CREATE; }
    if (FILE_DELETED & mask) { umask |= IN_DELETE; }
    if (FILE_MOVED_IN & mask) { umask |= IN_MOVED_TO; }
    if (FILE_MOVED_OUT & mask) { umask |= IN_MOVED_FROM; }
    if (FILE_SELF_MOVED & mask) { umask |= IN_MOVE_SELF; }
    if (FILE_SELF_DELETED & mask) { umask |= IN_DELETE_SELF; }

    int fd = infd_;

    if (fd < 0) {
        fd = inotify_init1(IN_NONBLOCK|IN_CLOEXEC);
        if (fd < 0) { throw sys_error("inotify_init1(): "+path); }
    }

    int wd = inotify_add_watch(fd, Locale().encode_filename(path).c_str(), umask);

    if (wd < 0) {
        sys_error x("inotify_add_watch(): "+path);
        if (infd_ < 0) { close(fd); }
        throw x;
    }

    if (infd_ < 0) {
        infd_ = fd;
        infd_poller_ = new Poller_posix(fd);
        infd_poller_->signal_poll().connect(fun(this, &Loop_freebsd::on_inotify));
        add_poller(infd_poller_, POLLIN);
    }

    auto fm = std::make_shared<File_monitor_freebsd>(wd, path);
    signal_chain_notify_.connect(fun(fm, &File_monitor_freebsd::on_inotify));
    fm->signal_destroy().connect(tau::bind(fun(this, &Loop_freebsd::on_file_monitor_destroy), wd));
    return fm;
}

void Loop_freebsd::on_file_monitor_destroy(int wd) {
    inotify_rm_watch(infd_, wd);

    if (1 == signal_chain_notify_.size()) {
        if (infd_poller_) { delete infd_poller_; infd_poller_ = nullptr; }
        close(infd_); infd_ = -1;
    }
}

void Loop_freebsd::init_mounts() {
}

void Loop_freebsd::check_mounts() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// static
Loop_freebsd_ptr Loop_freebsd::this_freebsd_loop() {
    std::thread::id tid = std::this_thread::get_id();
    Lock lk(smx_);
    auto i = loops_.find(tid);
    if (loops_.end() != i) { return i->second; }
    smx_.unlock();
    auto lp = std::make_shared<Loop_freebsd>(tid);
    smx_.lock();
    loops_[tid] = lp;
    if (0 == loopcnt_) { lp->boot(); }
    ++loopcnt_;
    return lp;
}

// static
Loop_posix_ptr Loop_posix::this_posix_loop() {
    return Loop_freebsd::this_freebsd_loop();
}

// static
Loop_ptr Loop_impl::this_loop() {
    return Loop_freebsd::this_freebsd_loop();
}

// static
Loop_ptr Loop_impl::that_loop(std::thread::id tid) {
    Lock lk(smx_);
    auto i = loops_.find(tid);
    return loops_.end() != i ? i->second : nullptr;
}

} // namespace tau

//END
