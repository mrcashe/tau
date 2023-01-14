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

#include <tau/locale.hh>
#include <tau/sys.hh>
#include <fileinfo-impl.hh>
#include <sys/stat.h>
#include <errno.h>
#include "loop-win.hh"

namespace tau {

struct Fileinfo_win: public Fileinfo_impl {

    explicit Fileinfo_win(const ustring & uri) {
        uri_ = uri;
        update_stat();
    }

    void update_stat() {
        flags_ = 0;
        atime_ = ctime_ = mtime_ = 0;
        exists_ = false;
        noacc_ = false;
        bytes_ = 0;

        if (!uri_.empty()) {
            std::string path = Locale().encode_filename(path_real(uri_));
            struct stat st;

            if (0 > stat(path.c_str(), &st)) {
                if (EACCES == errno) {
                    exists_ = noacc_ = true;
                }
            }

            else {
                exists_ = true;
                bytes_ = st.st_size;

                if (S_ISDIR(st.st_mode)) { flags_ |= IS_DIR; }
                if (S_ISREG(st.st_mode)) { flags_ |= IS_REG; }
                if (S_ISCHR(st.st_mode)) { flags_ |= IS_CHR; }
                if (S_ISBLK(st.st_mode)) { flags_ |= IS_BLK; }
                if (S_ISFIFO(st.st_mode)) { flags_ |= IS_FIFO; }

                atime_ = Timeval(uint64_t(1000000)*uint64_t(st.st_atime));
                ctime_ = Timeval(uint64_t(1000000)*uint64_t(st.st_ctime));
                mtime_ = Timeval(uint64_t(1000000)*uint64_t(st.st_mtime));
            }
        }
    }

    // Overrides pure Fileinfo_impl.
    signal<void(int, ustring)> & signal_watch(int event_mask) override {
        if (!loop_) {
            loop_ = Loop_win::this_win_loop();
        }

        if (!mon_) {
            try {
                mon_ = loop_->create_file_monitor(uri_, event_mask);
            }

            catch (...) {
            }
        }

        if (mon_) {
            mon_->signal_notify().connect(fun(signal_watch_));
        }

        return signal_watch_;
    }

    bool noacc_ = false;         // Access denied.
    signal<void(int, ustring)> signal_watch_;
    Loop_win_ptr     loop_;
    File_monitor_ptr mon_;
};

Fileinfo_ptr Fileinfo_impl::create(const ustring & uri) {
    return std::make_shared<Fileinfo_win>(uri);
}

} // namespace tau

//END
