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

#include <tau/buffer.hh>
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <buffer-impl.hh>
#include <fstream>
#include <iostream>

namespace tau {

Buffer::Buffer():
    impl(std::make_shared<Buffer_impl>())
{
    impl->newlines = str_newlines();
}

Buffer::Buffer(const ustring & s):
    impl(std::make_shared<Buffer_impl>())
{
    impl->newlines = str_newlines();
    assign(s);
}

Buffer::Buffer(const std::u32string & s):
    impl(std::make_shared<Buffer_impl>())
{
    impl->newlines = str_newlines();
    assign(s);
}

Buffer::Buffer(std::istream & is):
    impl(std::make_shared<Buffer_impl>())
{
    impl->newlines = str_newlines();
    insert(cend(), is);
}

void Buffer::assign(const ustring & str) {
    clear();
    insert(cend(), str);
}

void Buffer::assign(const std::u32string & str) {
    clear();
    insert(cend(), str);
}

void Buffer::assign(const Buffer other) {
    clear();
    insert(cend(), other.text());
}

Buffer_citer Buffer::replace(Buffer_citer i, const ustring & str) {
    return replace(i, std::u32string(str));
}

Buffer_citer Buffer::replace(Buffer_citer i, const std::u32string & str) {
    if (locked() || str.empty()) {
        return i;
    }

    std::size_t n = 0, len = str.size();

    while (n < len) {
        if (i.row() >= impl->rows.size()) {
            return insert(i, str);
        }

        if (i.row() == impl->rows.size()-1 && i.col() >= impl->rows[i.row()].s.size()) {
            return insert(i, str);
        }

        std::size_t eol = str.find_first_of(impl->newlines, n);
        if (ustring::npos == eol) { eol = len; }

        if (eol > n) {
            std::u32string & d = impl->rows[i.row()].s;
            std::size_t d_eol = d.find_first_of(impl->newlines);
            if (std::u32string::npos == d_eol) { d_eol = d.size(); }
            std::size_t n_repl = std::min(eol-n, (i.col() < d_eol ? d_eol-i.col() : 0));

            if (0 != n_repl) {
                std::u32string replaced_text = d.substr(i.col(), n_repl);
                d.replace(i.col(), n_repl, str, n, n_repl);
                std::size_t col = i.col()+n_repl;
                impl->signal_replace(i, citer(i.row(), col), replaced_text);
                i.move_to_col(col);
            }

            if (eol-n > n_repl) {
                i = insert(i, str.substr(n+n_repl, eol-n-n_repl));
            }
        }

        if (eol < len) {
            if (U'\x000d' == str[eol] && eol+1 < len && U'\x000a' == str[eol+1]) { ++eol; }
            ++eol;
            i.move_forward_line();
        }

        n = eol;
    }

    impl->changed = true;
    impl->signal_changed();
    return i;
}

Buffer_citer Buffer::insert(Buffer_citer i, const std::u32string & str) {
    return impl->insert(i, str);
}

Buffer_citer Buffer::insert(Buffer_citer i, char32_t uc, std::size_t count) {
    return 0 != count ? insert(i, std::u32string(count, uc)) : i;
}

Buffer_citer Buffer::insert(Buffer_citer i, const ustring & str) {
    return insert(i, std::u32string(str));
}

Buffer_citer Buffer::erase(Buffer_citer b, Buffer_citer e) {
    Buffer_citer ret(b);

    if (!locked() && !empty() && b && e && b != e) {
        if (e < b) { std::swap(b, e); }
        std::size_t row1 = b.row(), col1 = b.col(), row2 = e.row(), col2 = e.col();

        if (row1 < impl->rows.size() && col1 < impl->rows[row1].s.size()) {
            row2 = std::min(row2, impl->rows.size());

            if (row2 < impl->rows.size()) {
                col2 = std::min(col2, impl->rows[row2].s.size());
                std::size_t nlines = row2-row1;
                std::u32string erased_text = text32(b, e);

                if (0 == nlines) {
                    if (col2 > col1) {
                        impl->rows[row1].s.erase(col1, col2-col1);
                    }
                }

                else {
                    impl->rows[row1].s.erase(col1);
                    impl->rows[row1].s.append(impl->rows[row2].s.substr(col2));
                    impl->rows[row2].s.erase(0, col2);
                    while (nlines--) { impl->rows.erase(impl->rows.begin()+row1+1); }
                }

                if (1 == impl->rows.size() && 0 == impl->rows[0].s.size()) { impl->rows.clear(); }
                ret = citer(row2, col2);
                impl->signal_erase(b, ret, erased_text);
                impl->changed = true;
                impl->signal_changed();
            }
        }
    }

    return ret;
}

void Buffer::save(std::ostream & os) {
    impl->save(os);
}

void Buffer::save_to_file(const ustring & path) {
    tau::Locale lc;
    std::ofstream os(lc.encode_filename(path));
    if (!os.good()) { throw sys_error(); }
    save(os);
    os.close();
}

std::size_t Buffer::size() const {
    return impl->size();
}

std::size_t Buffer::length(Buffer_citer b, Buffer_citer e) const {
    return impl->length(b.row(), b.col(), e.row(), e.col());
}

std::size_t Buffer::lines() const {
    return impl->lines();
}

bool Buffer::empty() const {
    return impl->empty();
}

bool Buffer::changed() const {
    return impl->changed;
}

Encoding Buffer::encoding() const {
    return impl->encoding;
}

void Buffer::change_encoding(const Encoding & enc) {
    impl->change_encoding(enc);
}

Buffer_citer Buffer::citer(std::size_t row, std::size_t col) const {
    return Buffer_citer(Buffer_citer_impl::create(impl, row, col));
}

Buffer_citer Buffer::cbegin() const {
    return citer(0, 0);
}

Buffer_citer Buffer::cend() const {
    std::size_t row = 0, col = 0;

    if (0 != lines()) {
        row = lines()-1;
        col = impl->length(row);
    }

    return citer(row, col);
}

void Buffer::clear() {
    erase(cbegin(), cend());
}

ustring Buffer::text() const {
    return text(cbegin(), cend());
}

ustring Buffer::text(Buffer_citer b, Buffer_citer e) const {
    return impl->text(b.row(), b.col(), e.row(), e.col());
}

std::u32string Buffer::text32() const {
    return text32(cbegin(), cend());
}

std::u32string Buffer::text32(Buffer_citer b, Buffer_citer e) const {
    return impl->text(b.row(), b.col(), e.row(), e.col());
}

Buffer_citer Buffer::insert(Buffer_citer iter, std::istream & is) {
    return impl->insert(iter, is);
}

Buffer Buffer::load_from_file(const ustring & path) {
    std::ifstream is(Locale().encode(path), std::ios::binary);
    if (!is.good()) { throw sys_error(path); }
    Buffer buffer;
    buffer.insert(buffer.cend(), is);
    return buffer;
}

void Buffer::lock() {
    if (!impl->lock) {
        impl->lock = true;
        signal_lock()();
    }
}

bool Buffer::locked() const {
    return impl->lock;
}

void Buffer::unlock() {
    if (impl->lock) {
        impl->lock = false;
        signal_unlock()();
    }
}

void Buffer::enable_bom() {
    impl->enable_bom();
}

void Buffer::disable_bom() {
    impl->disable_bom();
}

bool Buffer::bom_enabled() const {
    return impl->bom;
}

signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & Buffer::signal_erase() {
    return impl->signal_erase;
}

signal<void(Buffer_citer, Buffer_citer)> & Buffer::signal_insert() {
    return impl->signal_insert;
}

signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & Buffer::signal_replace() {
    return impl->signal_replace;
}

signal<void()> & Buffer::signal_changed() {
    return impl->signal_changed;
}

signal<void()> & Buffer::signal_flush() {
    return impl->signal_flush;
}

signal<void()> & Buffer::signal_lock() {
    return impl->signal_lock;
}

signal<void()> & Buffer::signal_unlock() {
    return impl->signal_unlock;
}

signal<void(const Encoding &)> & Buffer::signal_encoding_changed() {
    return impl->signal_encoding_changed;
}

signal<void()> & Buffer::signal_bom_changed() {
    return impl->signal_bom_changed;
}

} // namespace tau

//END
