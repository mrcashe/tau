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

#include <tau/fileinfo.hh>
#include <fileinfo-impl.hh>

namespace tau {

Fileinfo::Fileinfo() {
    impl = Fileinfo_impl::create();
}

Fileinfo::Fileinfo(const ustring & uri) {
    impl = Fileinfo_impl::create(uri);
}

bool Fileinfo::exists() const {
    return impl->exists();
}

uint64_t Fileinfo::bytes() const {
    return impl->bytes();
}

bool Fileinfo::is_dir() const {
    return impl->is_dir();
}

bool Fileinfo::is_regular() const {
    return impl->is_regular();
}

bool Fileinfo::is_char() const {
    return impl->is_char();
}

bool Fileinfo::is_block() const {
    return impl->is_block();
}

bool Fileinfo::is_fifo() const {
    return impl->is_fifo();
}

bool Fileinfo::is_exec() const {
    return impl->is_exec();
}

bool Fileinfo::is_hidden() const {
    return impl->is_hidden();
}

bool Fileinfo::is_removable() const {
    return impl->is_removable();
}

Timeval Fileinfo::atime() const {
    return impl->atime();
}

Timeval Fileinfo::ctime() const {
    return impl->ctime();
}

Timeval Fileinfo::mtime() const {
    return impl->mtime();
}

void Fileinfo::rm(int opts, slot<void(int)> slot_async) {
    impl->rm(opts, slot_async);
}

signal<void(int, const ustring &)> & Fileinfo::signal_watch(int event_mask) {
    return impl->signal_watch(event_mask);
}

} // namespace tau

//END
