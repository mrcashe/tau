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

#ifndef TAU_BUFFER_IMPL_HH
#define TAU_BUFFER_IMPL_HH

#include <tau/buffer.hh>
#include <tau/encoding.hh>

namespace tau {

struct Buffer_citer_impl {
    Buffer_citer_impl(const Buffer_citer_impl & other) = delete;
    Buffer_citer_impl & operator=(const Buffer_citer_impl & other) = delete;

    Buffer_citer_impl() = default;
    static Buffer_citer_impl * create();
    static Buffer_citer_impl * create(Buffer_ptr buf, std::size_t row, std::size_t col);

    Buffer_ptr  buf_;
    std::size_t row_ = 0;
    std::size_t col_ = 0;
    bool        busy_ = false;
    bool        heap_ = false;
};

struct Buffer_impl {

    Buffer_impl();
   ~Buffer_impl();

    char32_t at(std::size_t row, std::size_t col) const;
    std::size_t size() const;
    std::size_t rows() const;
    void change_encoding(const Encoding & enc);
    Buffer_citer insert(Buffer_citer i, const std::u32string & str);
    Buffer_citer insert(Buffer_citer i, std::istream & is);
    Buffer_citer replace(Buffer_citer i, const std::u32string & str);
    Buffer_citer erase(Buffer_citer b, Buffer_citer e);
    void save(std::ostream & os);
    void save_to_file(const ustring & path);
    bool empty() const;
    void enable_bom();
    void disable_bom();
    void save();
    void lock();
    void unlock();

    std::u32string text(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const;

    std::u32string text(Buffer_citer b, Buffer_citer e) const {
        if (e < b) { std::swap(b, e); }
        return text(b.row(), b.col(), e.row(), e.col());
    }

    std::size_t length(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const;
    std::size_t length(std::size_t row) const;

    std::size_t length(Buffer_citer b, Buffer_citer e) const {
        if (e < b) { std::swap(b, e); }
        return length(b.row(), b.col(), e.row(), e.col());
    }

    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & signal_erase();
    signal<void(Buffer_citer, Buffer_citer)> & signal_insert();
    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & signal_replace();
    signal<void()> & signal_changed();
    signal<void()> & signal_flush();
    signal<void()> & signal_lock();
    signal<void()> & signal_unlock();
    signal<void(const Encoding &)> & signal_encoding_changed();
    signal<void()> & signal_bom_changed();

    struct Holder {
        std::u32string  s;

        Holder() = default;
        explicit Holder(const std::u32string & str): s(str) {}
    };

    using Rows = std::vector<Holder>;

    Rows                rows_;
    bool                locked_ = false;
    bool                bom_ = false;
    bool                changed_ = false;
    Encoding            encoding_ { "UTF-8" };
    Encoding            utf8_    { "UTF-8" };
    Encoding            utf16be_ { "UTF-16BE" };
    Encoding            utf16le_ { "UTF-16LE" };
    Encoding            utf32be_ { "UTF-32BE" };
    Encoding            utf32le_ { "UTF-32LE" };
    std::u32string      newlines_;
    ustring             path_;

    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> * signal_erase_ = nullptr;
    signal<void(Buffer_citer, Buffer_citer)> * signal_insert_ = nullptr;
    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> * signal_replace_ = nullptr;
    signal<void()> * signal_changed_ = nullptr;
    signal<void()> * signal_flush_ = nullptr;
    signal<void()> * signal_lock_ = nullptr;
    signal<void()> * signal_unlock_ = nullptr;
    signal<void(const Encoding &)> * signal_encoding_changed_ = nullptr;
    signal<void()> * signal_bom_changed_ = nullptr;
};

} // namespace tau

#endif // TAU_BUFFER_IMPL_HH
