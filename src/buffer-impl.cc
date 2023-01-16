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
#include <buffer-impl.hh>
#include <array>
#include <fstream>
#include <iostream>

namespace tau {

Buffer_citer_impl * Buffer_citer_impl::create() {
    static std::array<Buffer_citer_impl, 1024> v;

    for (auto & i: v) {
        if (!i.busy) {
            i.busy = true;
            i.row = 0;
            i.col = 0;
            return &i;
        }
    }

    Buffer_citer_impl * tp = new Buffer_citer_impl;
    tp->heap = true;
    tp->busy = true;
    return tp;
}

Buffer_citer_impl * Buffer_citer_impl::create(Buffer_ptr buf, std::size_t row, std::size_t col) {
    Buffer_citer_impl * tp = create();
    tp->buf = buf;
    tp->row = row;
    tp->col = col;
    return tp;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Buffer_citer::Buffer_citer():
    impl(Buffer_citer_impl::create())
{
}

Buffer_citer::Buffer_citer(const Buffer_citer & other):
    impl(Buffer_citer_impl::create())
{
    impl->buf = other.impl->buf;
    impl->row = other.impl->row;
    impl->col = other.impl->col;
}

Buffer_citer::Buffer_citer(const Buffer_citer & other, std::size_t row, std::size_t col):
    impl(Buffer_citer_impl::create())
{
    impl->buf = other.impl->buf;
    impl->row = row;
    impl->col = col;
}

Buffer_citer & Buffer_citer::operator=(const Buffer_citer & other) {
    if (this != &other) {
        impl->buf = other.impl->buf;
        impl->row = other.impl->row;
        impl->col = other.impl->col;
    }

    return *this;
}

Buffer_citer::Buffer_citer(Buffer_citer_impl * p):
    impl(p)
{
}

Buffer_citer::~Buffer_citer() {
    impl->buf.reset();
    impl->busy = false;
    if (impl->heap) { delete impl; }
}

void Buffer_citer::set(const Buffer_citer & other, std::size_t row, std::size_t col) {
    impl->buf = other.impl->buf;
    impl->row = row;
    impl->col = col;
}

char32_t Buffer_citer::operator*() const {
    if (impl->buf && row() < impl->buf->lines() && col() < impl->buf->length(row())) {
        return impl->buf->at(row(), col());
    }

    return 0;
}

ustring Buffer_citer::peek(Buffer_citer other) const {
    return peek32(other);
}

ustring Buffer_citer::peek(std::size_t nchars) const {
    return peek32(nchars);
}

std::u32string Buffer_citer::peek32(Buffer_citer other) const {
    std::u32string res;

    if (impl->buf && other.impl->buf == impl->buf) {
        res = impl->buf->text(impl->row, impl->col, other.impl->row, other.impl->col);
    }

    return res;
}

std::u32string Buffer_citer::peek32(std::size_t nchars) const {
    std::u32string res;

    if (impl->buf && !eof()) {
        std::size_t r = row(), c = col(), nlines = impl->buf->lines(), len = impl->buf->length(r);

        while (0 != nchars--) {
            res += impl->buf->at(r, c++);

            if (c >= len) {
                if (++r >= nlines) { break; }
                c = 0;
                len = impl->buf->length(r);
            }
        }
    }

    return res;
}

Buffer_citer & Buffer_citer::operator++() {
    if (impl->buf) {
        std::size_t len = impl->buf->length(row());

        if (1+col() < len) {
            ++impl->col;
        }

        else if (1+row() < impl->buf->lines()) {
            ++impl->row;
            impl->col = 0;
        }

        else {
            impl->col = len;
        }
    }

    return *this;
}

Buffer_citer Buffer_citer::operator++(int) {
    Buffer_citer result(*this);
    operator++();
    return result;
}

Buffer_citer & Buffer_citer::operator--() {
    if (impl->buf) {
        if (0 != col()) {
            --impl->col;
        }

        else if (0 != row()) {
            --impl->row;
            impl->col = impl->buf->length(row());
            if (0 != col()) { --impl->col; }
        }
    }

    return *this;
}

Buffer_citer Buffer_citer::operator--(int) {
    Buffer_citer result(*this);
    operator--();
    return result;
}

Buffer_citer & Buffer_citer::operator+=(std::size_t npos) {
    while (npos--) { operator++(); }
    return *this;
}

Buffer_citer & Buffer_citer::operator-=(std::size_t npos) {
    while (npos--) { operator--(); }
    return *this;
}

bool Buffer_citer::operator==(const Buffer_citer & other) const {
    if (impl->buf && impl->buf == other.impl->buf) {
        if (row() == other.row()) {
            return col() == other.col();
        }

        if (eof() && other.eof()) {
            return true;
        }
    }

    return false;
}

bool Buffer_citer::operator!=(const Buffer_citer & other) const {
    return !operator==(other);
}

bool Buffer_citer::operator<(const Buffer_citer & other) const {
    if (impl->buf && impl->buf == other.impl->buf) {
        if (row() < other.row()) {
            return true;
        }

        if (row() == other.row()) {
            return col() < other.col();
        }
    }

    return false;
}

Buffer_citer::operator bool() const {
    return nullptr != impl->buf;
}

size_t Buffer_citer::row() const {
    return impl->row;
}

size_t Buffer_citer::col() const {
    return impl->col;
}

bool Buffer_citer::eol() const {
    return eof() || char32_is_newline(operator*());
}

bool Buffer_citer::eof() const {
    if (impl->buf) {
        std::size_t nrows = impl->buf->lines();

        if (0 == nrows) {
            return true;
        }

        if (row() >= nrows) {
            return true;
        }

        if (row() == nrows-1) {
            if (col() >= impl->buf->length(row())) {
                return true;
            }
        }

        return false;
    }

    return true;
}

bool Buffer_citer::sof() const {
    if (impl->buf) {
        return 0 == row() && 0 == col();
    }

    return true;
}

void Buffer_citer::move_to(size_t row, size_t col) {
    if (impl->buf) {
        if (impl->buf->empty()) {
            impl->row = 0;
            impl->col = 0;
        }

        else if (row < impl->buf->lines()) {
            impl->row = row;
            impl->col = std::min(col, impl->buf->length(row));
        }

        else {
            impl->row = impl->buf->lines()-1;
            impl->col = impl->buf->length(impl->row);
        }
    }
}

void Buffer_citer::move_to_col(size_t col) {
    if (impl->buf) {
        if (impl->row < impl->buf->lines()) {
            impl->col = std::min(col, impl->buf->length(impl->row));
        }
    }
}

void Buffer_citer::move_to_sol() {
    if (impl->buf) {
        impl->col = 0;
    }
}

void Buffer_citer::move_to_eol() {
    if (impl->buf) {
        while (!eof() && !char32_is_newline(operator*())) {
            operator++();
        }
    }
}

void Buffer_citer::move_backward_line() {
    if (impl->buf) {
        impl->col = 0;
        if (0 != impl->row) { --impl->row; }
    }
}

void Buffer_citer::move_forward_line() {
    if (impl->buf) {
        size_t nrows = impl->buf->lines();

        if (0 != nrows) {
            if (1+row() < nrows) {
                impl->col = 0;
                ++impl->row;
            }

            else {
                impl->row = nrows-1;
                impl->col = impl->buf->length(impl->row);
            }
        }
    }
}

void Buffer_citer::move_word_left() {
    if (impl->buf) {
        std::size_t col = impl->col;

        if (0 == col) {
            operator--();
        }

        else {
            operator--();

            if (char32_is_delimiter(operator*())) {
                while (0 != impl->col && char32_is_delimiter(operator*())) { operator--(); }
                if (0 == impl->col) { return; }
                while (0 != impl->col && !char32_is_delimiter(operator*())) { operator--(); }
                if (impl->col < col && char32_is_delimiter(operator*())) { operator++(); }
            }

            else {
                while (0 != impl->col && !char32_is_delimiter(operator*())) { operator--(); }
                if (0 == impl->col) { return; }
                if (impl->col < col && !char32_is_delimiter(operator*())) { operator++(); }
            }
        }
    }
}

void Buffer_citer::move_word_right() {
    if (impl->buf) {
        if (eol()) {
            operator++();
        }

        else if (char32_is_delimiter(operator*())) {
            while (!eol() && char32_is_delimiter(operator*())) { operator++(); }
        }

        else {
            while (!eol() && !char32_is_delimiter(operator*())) { operator++(); }
            while (!eol() && char32_is_delimiter(operator*())) { operator++(); }
        }
    }
}

void Buffer_citer::skip_blanks() {
    if (impl->buf) {
        while (!eol() && char32_isblank(operator*())) {
            operator++();
        }
    }
}

void Buffer_citer::skip_whitespace() {
    if (impl->buf) {
        while (!eof()) {
            char32_t c = operator*();
            if (!char32_isblank(c) && !char32_is_newline(c)) { break; }
            operator++();
        }
    }
}

void Buffer_citer::reset() {
    impl->buf.reset();
    impl->row = 0;
    impl->col = 0;
}

bool Buffer_citer::find(char32_t wc) {
    if (impl->buf && 0x000000 != wc) {
        while (!eof()) {
            if (wc == operator*()) { return true; }
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find(char32_t wc, Buffer_citer other) {
    if (impl->buf && operator<(other) && 0x000000 != wc) {
        while (!eof() && operator<(other)) {
            if (wc == operator*()) { return true; }
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find(const ustring & text) {
    return find(std::u32string(text));
}

bool Buffer_citer::find(const ustring & text, Buffer_citer other) {
    return find(std::u32string(text), other);
}

bool Buffer_citer::find(const std::u32string & text) {
    std::size_t len = text.size();

    if (impl->buf && 0 != len) {
        while (!eof()) {
            if (text == peek32(len)) { return true; }
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find(const std::u32string & text, Buffer_citer other) {
    if (impl->buf && operator<(other)) {
        std::size_t len = text.size();

        if (0 != len) {
            while (!eof() && operator<(other)) {
                if (text == peek32(len)) { return true; }
                operator++();
            }
        }
    }

    return false;
}

bool Buffer_citer::find_first_of(const ustring & chars) {
    if (impl->buf && !chars.empty()) {
        while (!eof()) {
            char32_t wc = operator*();

            for (const char * p = chars.c_str(); '\0' != *p; p = str8_next(p)) {
                if (char32_from_pointer(p) == wc) {
                    return true;
                }
            }

            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_of(const ustring & chars, Buffer_citer other) {
    if (impl->buf && operator<(other) && !chars.empty()) {
        while (!eof() && operator<(other)) {
            char32_t wc = operator*();

            for (const char * p = chars.c_str(); '\0' != *p; p = str8_next(p)) {
                if (char32_from_pointer(p) == wc) {
                    return true;
                }
            }

            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_of(const std::u32string & chars) {
    if (impl->buf && !chars.empty()) {
        while (!eof()) {
            char32_t wc = operator*();

            for (const char32_t * p = chars.c_str(); U'\0' != *p; ++p) {
                if (*p == wc) {
                    return true;
                }
            }

            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_of(const std::u32string & chars, Buffer_citer other) {
    if (impl->buf && operator<(other) && !chars.empty()) {
        while (!eof() && operator<(other)) {
            char32_t wc = operator*();

            for (const char32_t * p = chars.c_str(); U'\0' != *p; ++p) {
                if (*p == wc) {
                    return true;
                }
            }

            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_not_of(const ustring & chars) {
    if (impl->buf && !chars.empty()) {
        while (!eof()) {
            char32_t wc = operator*();

            for (const char * p = chars.c_str(); '\0' != *p; p = str8_next(p)) {
                if (char32_from_pointer(p) == wc) {
                    goto next;
                }
            }

            return true;

            next:
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_not_of(const ustring & chars, Buffer_citer other) {
    if (impl->buf && operator<(other) && !chars.empty()) {
        while (!eof() && operator<(other)) {
            char32_t wc = operator*();

            for (const char * p = chars.c_str(); '\0' != *p; p = str8_next(p)) {
                if (char32_from_pointer(p) == wc) {
                    goto next;
                }
            }

            return true;

            next:
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_not_of(const std::u32string & chars) {
    if (impl->buf && !chars.empty()) {
        while (!eof()) {
            char32_t wc = operator*();

            for (const char32_t * p = chars.c_str(); U'\0' != *p; ++p) {
                if (*p == wc) {
                    goto next;
                }
            }

            return true;

            next:
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::find_first_not_of(const std::u32string & chars, Buffer_citer other) {
    if (impl->buf && operator<(other) && !chars.empty()) {
        while (!eof() && operator<(other)) {
            char32_t wc = operator*();

            for (const char32_t * p = chars.c_str(); U'\0' != *p; ++p) {
                if (*p == wc) {
                    goto next;
                }
            }

            return true;

            next:
            operator++();
        }
    }

    return false;
}

bool Buffer_citer::equals(const ustring & text, bool advance) {
    return equals(std::u32string(text), advance);
}

bool Buffer_citer::equals(const std::u32string & text, bool advance) {
    std::size_t len = text.size();

    if (impl->buf && 0 != len) {
        if (peek32(len) == text) {
            if (advance) { operator+=(len); }
            return true;
        }
    }

    return false;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

std::size_t Buffer_impl::size() const {
    std::size_t sz = 0;
    for (auto & row: rows) { sz += row.s.size(); }
    return sz;
}

std::size_t Buffer_impl::lines() const {
    return rows.size();
}

std::size_t Buffer_impl::length(std::size_t row) const {
    return row < rows.size() ? rows[row].s.size() : 0;
}

bool Buffer_impl::empty() const {
    return rows.empty();
}

char32_t Buffer_impl::at(std::size_t row, std::size_t col) const {
    if (row < rows.size()) {
        auto & s = rows[row].s;
        if (col < s.size()) { return s[col]; }
    }

    return 0;
}

std::u32string Buffer_impl::text(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const {
    if (r1 > r2) { std::swap(r1, r2); std::swap(c1, c2); }
    if (r1 == r2 && c1 > c2) { std::swap(c1, c2); }

    std::size_t nrows = rows.size();
    std::u32string s;

    while (r1 < r2 && r1 < nrows) {
        const std::u32string & d = rows[r1].s;
        s += d.substr(c1);
        ++r1, c1 = 0;
    }

    if (r1 == r2 && r1 < nrows) {
        const std::u32string & d = rows[r1].s;

        if (c1 < d.size() && c2 > c1) {
            s += d.substr(c1, c2-c1);
        }
    }

    return s;
}

std::size_t Buffer_impl::length(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const {
    if (r1 > r2) { std::swap(r1, r2); std::swap(c1, c2); }
    if (r1 == r2 && c1 > c2) { std::swap(c1, c2); }

    std::size_t nrows = rows.size(), result = 0;

    while (r1 < r2 && r1 < nrows) {
        const std::u32string & d = rows[r1].s;
        std::size_t len = d.size(), cm = std::min(len, c1);
        result += len-cm;
        ++r1, c1 = 0;
    }

    if (r1 == r2 && r1 < nrows) {
        const std::u32string & d = rows[r1].s;
        std::size_t len = d.size();

        if (c1 < len && c2 > c1) {
            c2 = std::min(c2, len);
            result += c2-c1;
        }
    }

    return result;
}

} // namespace tau

//END
