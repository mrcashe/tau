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

class Buffer_citer_impl {
protected:

    friend Buffer_citer;
    Buffer_citer_impl(const Buffer_citer_impl & other) = delete;
    Buffer_citer_impl & operator=(const Buffer_citer_impl & other) = delete;

public:

    Buffer_citer_impl() = default;
    static Buffer_citer_impl * create();
    static Buffer_citer_impl * create(Buffer_ptr buf, std::size_t row, std::size_t col);

private:

    Buffer_ptr  buf;
    std::size_t row = 0;
    std::size_t col = 0;
    bool        busy = false;
    bool        heap = false;
};

struct Buffer_impl {
    Buffer_impl() = default;

    char32_t at(std::size_t row, std::size_t col) const;
    std::size_t size() const;
    std::size_t lines() const;
    std::size_t length(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const;
    std::size_t length(std::size_t row) const;
    void change_encoding(const Encoding & enc);
    Buffer_citer insert(Buffer_citer i, const std::u32string & str);
    Buffer_citer insert(Buffer_citer i, std::istream & is);
    void save(std::ostream & os);
    std::u32string text(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const;
    bool empty() const;
    void enable_bom();
    void disable_bom();

    struct Holder {
        std::u32string  s;

        Holder() = default;
        explicit Holder(const std::u32string & str): s(str) {}
    };

    using Rows = std::vector<Holder>;

    Rows                rows;
    bool                lock = false;
    bool                bom = false;
    bool                changed = false;
    Encoding            encoding { "UTF-8" };
    Encoding            utf8    { "UTF-8" };
    Encoding            utf16be { "UTF-16BE" };
    Encoding            utf16le { "UTF-16LE" };
    Encoding            utf32be { "UTF-32BE" };
    Encoding            utf32le { "UTF-32LE" };
    std::u32string      newlines;

    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> signal_erase;
    signal<void(Buffer_citer, Buffer_citer)> signal_insert;
    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> signal_replace;
    signal<void()> signal_changed;
    signal<void()> signal_flush;
    signal<void()> signal_lock;
    signal<void()> signal_unlock;
    signal<void(const Encoding &)> signal_encoding_changed;
    signal<void()> signal_bom_changed;
};

} // namespace tau

#endif // TAU_BUFFER_IMPL_HH
