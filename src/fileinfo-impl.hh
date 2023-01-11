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

#ifndef TAU_FILEINFO_IMPL_HH
#define TAU_FILEINFO_IMPL_HH

#include <types-impl.hh>
#include <tau/timeval.hh>
#include <tau/signal.hh>

namespace tau {

class Fileinfo_impl {
protected:

    Fileinfo_impl() = default;

public:

    Fileinfo_impl(const Fileinfo_impl & other) = delete;
    Fileinfo_impl(Fileinfo_impl && other) = delete;
    Fileinfo_impl & operator=(const Fileinfo_impl & other) = delete;
    Fileinfo_impl & operator=(Fileinfo_impl && other) = delete;
    virtual ~Fileinfo_impl() {}

    static Fileinfo_ptr create(const ustring & uri=ustring());

    enum {
        IS_DIR      = 1 << 0,
        IS_LNK      = 1 << 1,
        IS_REG      = 1 << 2,
        IS_CHR      = 1 << 3,
        IS_BLK      = 1 << 4,
        IS_FIFO     = 1 << 5,
        IS_SOCK     = 1 << 6
    };

    bool exists() const { return exists_; }
    uintmax_t bytes() const { return bytes_; }
    bool is_dir() const { return IS_DIR & flags_; }
    bool is_link() const { return IS_LNK & flags_; }
    bool is_regular() const { return IS_REG & flags_; }
    bool is_char() const { return IS_CHR & flags_; }
    bool is_block() const { return IS_BLK & flags_; }
    bool is_fifo() const { return IS_FIFO & flags_; }
    bool is_socket() const { return IS_SOCK & flags_; }

    Timeval atime() const { return atime_; }
    Timeval ctime() const { return ctime_; }
    Timeval mtime() const { return mtime_; }

    virtual signal<void(int, ustring)> & signal_watch(int event_mask) = 0;

protected:

    ustring     uri_;
    bool        exists_ = false;
    uintmax_t   bytes_ = 0;
    uint32_t    flags_ = 0;

    Timeval     atime_ { 0 };
    Timeval     ctime_ { 0 };
    Timeval     mtime_ { 0 };
};

} // namespace tau

#endif // TAU_FILEINFO_IMPL_HH
