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
#include <tau/string.hh>
#include <tau/sys.hh>
#include "fileinfo-posix.hh"
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

namespace tau {

Fileinfo_posix::Fileinfo_posix(const ustring & uri) {
    uri_ = path_real(uri);
    update_stat();
}

void Fileinfo_posix::update_stat() {
    flags_ = 0;
    atime_ = ctime_ = mtime_ = 0;
    exists_ = noacc_ = exec_ = false;
    bytes_ = 0;

    if (!uri_.empty()) {
        auto & io = Locale().iocharset();
        std::string path = io.is_utf8() ? std::string(uri_) : io.encode(uri_);
        struct stat st;

        if (0 > lstat(path.c_str(), &st)) {
            if (EACCES == errno) {
                exists_ = noacc_ = true;
            }
        }

        else {
            exists_ = true;
            bytes_ = st.st_size;
            // if (str_has_prefix(uri_, "/sys")) std::cout << "stat " << uri_ << '\n';
            if (S_ISDIR(st.st_mode)) { flags_ |= IS_DIR; }
            if (S_ISREG(st.st_mode)) { flags_ |= IS_REG; }
            if (S_ISCHR(st.st_mode)) { flags_ |= IS_CHR; }
            if (S_ISBLK(st.st_mode)) { flags_ |= IS_BLK; }
            if (S_ISFIFO(st.st_mode)) { flags_ |= IS_FIFO; }
            if (S_ISLNK(st.st_mode)) { flags_ |= IS_LNK; }
            if (S_ISSOCK(st.st_mode)) { flags_ |= IS_SOCK; }
            exec_ = S_ISREG(st.st_mode) && (S_IXUSR & st.st_mode);
            atime_ = Timeval(uint64_t(1000000)*uint64_t(st.st_atime));
            ctime_ = Timeval(uint64_t(1000000)*uint64_t(st.st_ctime));
            mtime_ = Timeval(uint64_t(1000000)*uint64_t(st.st_mtime));
        }
    }
}

// Overrides pure Fileinfo_impl.
bool Fileinfo_posix::is_hidden() {
    return exists_ && !uri_.empty() && '.' == path_notdir(uri_)[0];
}

// Overrides pure Fileinfo_impl.
bool Fileinfo_posix::is_removable() {
    auto loop = Loop_posix::this_posix_loop();

    if (!uri_.empty()) {
        for (const ustring & s: loop->mounts()) {
            for (ustring p = uri_; "/" != p; p = path_dirname(p)) {
                if (p == s) {
                    return loop->is_removable(p);
                }
            }
        }
    }

    return false;
}

// Overrides pure Fileinfo_impl.
void Fileinfo_posix::rm(int opts, slot<void(int)> slot_async) {
    auto & io = Locale().iocharset();
    auto p = io.is_utf8() ? std::string(uri_) : io.encode(uri_);
    int result = unlink(p.c_str());
    if (-1 == result) { throw sys_error(); }
    exists_ = false;
}

} // namespace tau

//END
