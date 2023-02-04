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

#ifndef TAU_LOOP_WIN_HH
#define TAU_LOOP_WIN_HH

#include "types-win.hh"
#include <event-impl.hh>
#include <file-monitor-impl.hh>
#include <loop-impl.hh>

namespace tau {

class Poller {
public:

    virtual ~Poller() {}
    virtual HANDLE handle() const = 0;
    virtual signal<void()> & signal_poll() = 0;

    bool on_poll(HANDLE h) {
        if (handle() == h) {
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Event_win: public Event_impl, public Poller {
    HANDLE handle_ = NULL;

public:

    Event_win();
   ~Event_win();

    // Overrides pure Event_impl.
    void emit() override;

    // Overrides pure Event_impl.
    void release() override;

    HANDLE handle() const override { return handle_; }
    signal<void()> & signal_poll() override { return signal_ready_; }
};


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class File_monitor_win: public File_monitor, public Poller {
public:

    File_monitor_win(const ustring & path, int umask);
   ~File_monitor_win();

    void notify();

    HANDLE handle() const override { return overlapped_.hEvent; }
    signal<void()> & signal_poll() override { return signal_poll_; }

    bool is_dir() const { return isdir_; }

private:

    bool                isdir_;
    DWORD               filter_;
    DWORD               attribs_;
    HANDLE              hdir_;
    OVERLAPPED          overlapped_;
    std::vector<char>   buffer_;
    int                 umask_;
    ustring             dirpath_;
    std::wstring        wfilename_;
    ustring             filename_;
    signal<void()>      signal_poll_;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Loop_win: public Loop_impl {
public:

    static Loop_win_ptr this_win_loop();

    explicit Loop_win(std::thread::id tid);

    // Overrides pure Loop_impl.
    File_monitor_ptr create_file_monitor(const ustring & path, int mask) override;

    // Overrides pure Loop_impl.
    Event_ptr create_event() override;

    // Overrides pure Loop_impl.
    std::vector<ustring> mounts() override;

    void handle_mount();
    void handle_umount();

protected:

    // Overrides pure Loop_impl.
    void iterate(int timeout_ms) override;

private:

    using Handles = std::vector<HANDLE>;
    using Drives = std::list<ustring>;

    Handles         handles_;
    Drives          drives_;

    signal<bool(HANDLE)> signal_chain_poll_;

private:

    Drives list_drives() const;
    void on_handle_die(HANDLE handle);
    void done();
};

} // namespace tau

#endif // TAU_LOOP_WIN_HH
