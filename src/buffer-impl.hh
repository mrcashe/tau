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

#ifndef TAU_BUFFER_IMPL_HH
#define TAU_BUFFER_IMPL_HH

#include <tau/buffer.hh>
#include <tau/encoding.hh>

namespace tau {

class Buffer_iter_impl {
protected:

    friend Buffer_iter;
    Buffer_iter_impl(const Buffer_iter_impl & other) = delete;
    Buffer_iter_impl & operator=(const Buffer_iter_impl & other) = delete;

public:

    Buffer_iter_impl() = default;
    static Buffer_iter_impl * create();
    static Buffer_iter_impl * create(Buffer_ptr buf, size_t row, size_t col);

private:

    Buffer_ptr  buf;
    size_t      row = 0;
    size_t      col = 0;
    bool        busy = false;
    bool        heap = false;
};

struct Buffer_impl {
    Buffer_impl() = default;

    char32_t at(size_t row, size_t col) const;
    size_t size() const;
    size_t lines() const;
    size_t length(size_t r1, size_t c1, size_t r2, size_t c2) const;
    size_t length(size_t row) const;
    std::u32string text(size_t r1, size_t c1, size_t r2, size_t c2) const;
    bool empty() const;

    struct Holder {
        std::u32string  s;

        Holder() = default;
        Holder(const std::u32string & str): s(str) {}
    };

    using Rows = std::vector<Holder>;

    Rows                rows;
    bool                lock = false;
    bool                bom = false;
    Encoding            encoding { "UTF-8" };
    std::u32string      newlines;

    signal<void(Buffer_iter, Buffer_iter, std::u32string)> signal_erase;
    signal<void(Buffer_iter, Buffer_iter)> signal_insert;
    signal<void(Buffer_iter, Buffer_iter, std::u32string)> signal_replace;
    signal<void()> signal_lock;
    signal<void()> signal_unlock;
    signal<void(const Encoding &)> signal_encoding_changed;
    signal<void()> signal_bom_changed;
};

} // namespace tau

#endif // TAU_BUFFER_IMPL_HH
