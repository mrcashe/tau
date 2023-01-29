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
    if (impl->buf && row() < impl->buf->rows() && col() < impl->buf->length(row())) {
        return impl->buf->at(row(), col());
    }

    return 0;
}

ustring Buffer_citer::text(Buffer_citer other) const {
    return text32(other);
}

ustring Buffer_citer::text(std::size_t nchars) const {
    return text32(nchars);
}

std::u32string Buffer_citer::text32(Buffer_citer other) const {
    std::u32string res;

    if (impl->buf && other.impl->buf == impl->buf) {
        res = impl->buf->text(impl->row, impl->col, other.impl->row, other.impl->col);
    }

    return res;
}

std::u32string Buffer_citer::text32(std::size_t nchars) const {
    std::u32string res;

    if (impl->buf && !eof()) {
        std::size_t r = row(), c = col(), nlines = impl->buf->rows(), len = impl->buf->length(r);

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

        else if (1+row() < impl->buf->rows()) {
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
        std::size_t nrows = impl->buf->rows();

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

        else if (row < impl->buf->rows()) {
            impl->row = row;
            impl->col = std::min(col, impl->buf->length(row));
        }

        else {
            impl->row = impl->buf->rows()-1;
            impl->col = impl->buf->length(impl->row);
        }
    }
}

void Buffer_citer::move_to_col(size_t col) {
    if (impl->buf) {
        if (impl->row < impl->buf->rows()) {
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
        size_t nrows = impl->buf->rows();

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
            if (text == text32(len)) { return true; }
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
                if (text == text32(len)) { return true; }
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

            for (const char * p = chars.c_str(); '\0' != *p; p = utf8_next(p)) {
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

            for (const char * p = chars.c_str(); '\0' != *p; p = utf8_next(p)) {
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

            for (const char * p = chars.c_str(); '\0' != *p; p = utf8_next(p)) {
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

            for (const char * p = chars.c_str(); '\0' != *p; p = utf8_next(p)) {
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
        if (text32(len) == text) {
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
    for (auto & row: rows_) { sz += row.s.size(); }
    return sz;
}

std::size_t Buffer_impl::rows() const {
    return rows_.size();
}

std::size_t Buffer_impl::length(std::size_t row) const {
    return row < rows_.size() ? rows_[row].s.size() : 0;
}

bool Buffer_impl::empty() const {
    return rows_.empty();
}

char32_t Buffer_impl::at(std::size_t row, std::size_t col) const {
    if (row < rows_.size()) {
        auto & s = rows_[row].s;
        if (col < s.size()) { return s[col]; }
    }

    return 0;
}

std::u32string Buffer_impl::text(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const {
    if (r1 > r2) { std::swap(r1, r2); std::swap(c1, c2); }
    if (r1 == r2 && c1 > c2) { std::swap(c1, c2); }

    std::size_t nrows = rows_.size();
    std::u32string s;

    while (r1 < r2 && r1 < nrows) {
        const std::u32string & d = rows_[r1].s;
        s += d.substr(c1);
        ++r1, c1 = 0;
    }

    if (r1 == r2 && r1 < nrows) {
        const std::u32string & d = rows_[r1].s;

        if (c1 < d.size() && c2 > c1) {
            s += d.substr(c1, c2-c1);
        }
    }

    return s;
}

std::size_t Buffer_impl::length(std::size_t r1, std::size_t c1, std::size_t r2, std::size_t c2) const {
    if (r1 > r2) { std::swap(r1, r2); std::swap(c1, c2); }
    if (r1 == r2 && c1 > c2) { std::swap(c1, c2); }

    std::size_t nrows = rows_.size(), result = 0;

    while (r1 < r2 && r1 < nrows) {
        const std::u32string & d = rows_[r1].s;
        std::size_t len = d.size(), cm = std::min(len, c1);
        result += len-cm;
        ++r1, c1 = 0;
    }

    if (r1 == r2 && r1 < nrows) {
        const std::u32string & d = rows_[r1].s;
        std::size_t len = d.size();

        if (c1 < len && c2 > c1) {
            c2 = std::min(c2, len);
            result += c2-c1;
        }
    }

    return result;
}

Buffer_citer Buffer_impl::insert(Buffer_citer i, const std::u32string & str) {
    if (lock_ || str.empty()) {
        return i;
    }

    Buffer_citer e(i);
    std::size_t n = 0, len = str.size(), row, col;

    if (rows_.empty()) {
        row = 0;
        col = 0;
    }

    else if (i.row() < rows_.size()) {
        row = i.row();
        col = std::min(rows_[row].s.size(), i.col());
    }

    else {
        row = rows_.size()-1;
        col = rows_[row].s.size();
    }

    while (n < len) {
        std::size_t eol = str.find_first_of(newlines_, n);

        // No EOL character.
        // Add text at current position and exit.
        if (std::u32string::npos == eol) {
            if (rows_.empty()) { rows_.emplace_back(str.substr(n)); }
            else { rows_[row].s.insert(col, str, n, len-n); }
            col += len-n;
            n = len;
        }

        // Have EOL character.
        else {
            // Insert text between col and next at current position.
            std::u32string::size_type next, eeol;

            if (0x000a == str[eol] && eol+1 < len && 0x000d == str[eol+1]) { eeol = eol+2; }
            else if (0x000d == str[eol] && eol+1 < len && 0x000a == str[eol+1]) { eeol = eol+2; }
            else { eeol = eol+1; }

            std::u32string newline = std::u32string::npos == eeol ? str.substr(eol) : str.substr(eol, eeol-eol);
            next = eol+newline.size();

            if (rows_.empty()) {
                rows_.emplace_back(str.substr(n, next-n));
                rows_.emplace_back();
            }

            else {
                std::u32string right = rows_[row].s.substr(col);
                rows_[row].s.erase(col);
                rows_[row].s.insert(col, str, n, next-n);
                rows_.emplace(rows_.begin()+row+1, right);
            }

            n = next;
            ++row;
            col = 0;
        }
    }

    e.move_to(row, col);
    signal_insert_(i, e);
    changed_ = true;
    signal_changed_();
    return e;
}

void Buffer_impl::change_encoding(const Encoding & enc) {
    if (encoding_ != enc) {
        encoding_ = enc;
        signal_encoding_changed_(encoding_);
    }
}

void Buffer_impl::enable_bom() {
    if (!bom_) {
        bom_ = true;
        signal_bom_changed_();
    }
}

void Buffer_impl::disable_bom() {
    if (bom_) {
        bom_ = false;
        signal_bom_changed_();
    }
}

Buffer_citer Buffer_impl::insert(Buffer_citer iter, std::istream & is) {
    if (lock_ || !is.good()) {
        return iter;
    }

    char buffer[2048];
    char32_t ob[10240];
    std::size_t fpos = 0, len = 0;
    bool not8 = false;
    std::string acc;

    while (!is.eof()) {
        fpos += len;
        is.read(buffer, sizeof(buffer));
        len = is.gcount();
        if (0 == len) { break; }
        std::size_t offset = 0, epos = len;
        char32_t * obp = ob;

        if (0 == fpos) {
            if (len >= 4 && '\0' == buffer[0] && '\0' == buffer[1] && '\xfe' == buffer[2] && '\xff' == buffer[3]) {
                offset += 4;
                len -= 4;
                change_encoding(utf32be_);
                enable_bom();
            }

            else if (len >= 4 && '\xff' == buffer[0] && '\xfe' == buffer[1] && '\0' == buffer[2] && '\0' == buffer[3]) {
                offset += 4;
                len -= 4;
                change_encoding(utf32le_);
                enable_bom();
            }

            else if (len >= 3 && '\xef' == buffer[0] && '\xbb' == buffer[1] && '\xbf' == buffer[2]) {
                offset += 3;
                len -= 3;
                change_encoding(utf8_);
                enable_bom();
            }

            else if (len >= 2 && '\xfe' == buffer[0] && '\xff' == buffer[1]) {
                offset += 2;
                len -= 2;
                change_encoding(utf16be_);
                enable_bom();
            }

            else if (len >= 2 && '\xff' == buffer[0] && '\xfe' == buffer[1]) {
                offset += 2;
                len -= 2;
                change_encoding(utf16le_);
                enable_bom();
            }
        }

        if (utf32be_ == encoding_) {
            const char * p = buffer+offset;

            while (len >= 4) {
                char32_t wc = uint8_t(*p++);
                wc <<= 8; wc += uint8_t(*p++);
                wc <<= 8; wc += uint8_t(*p++);
                wc <<= 8; wc += uint8_t(*p++);
                *obp++ = wc;
                len -= 4;
            }
        }

        else if (utf32le_ == encoding_) {
            const char * p = buffer+offset;

            while (len >= 4) {
                char32_t wc = uint8_t(p[3]);
                wc <<= 8; wc += uint8_t(p[2]);
                wc <<= 8; wc += uint8_t(p[1]);
                wc <<= 8; wc += uint8_t(p[0]);
                *obp++ = wc;
                p += 4;
                len -= 4;
            }
        }

        else if (utf16be_ == encoding_) {
            const char * p = buffer+offset;
            char16_t sur = 0;

            while (len >= 2) {
                char16_t wc = uint8_t(*p++);
                wc <<= 8; wc += uint8_t(*p++);

                if (0 != sur) {
                    if (char16_is_surrogate(wc)) {
                        *obp++ = char32_from_surrogate(sur, wc);
                    }

                    // FIXME Simply skip character on error? Is it correct?
                    else {
                        std::cerr << "** Buffer::insert(stream): (" << encoding_.name() << "), position "
                        << fpos+offset << ": skip surrogate (?) pair " << std::hex << std::showbase
                        << sur << ':' << std::hex << std::showbase << wc << std::dec << std::endl;
                    }

                    sur = 0;
                }

                else {
                    if (char16_is_surrogate(wc)) {
                        sur = wc;
                    }

                    else {
                        *obp++ = char32_t(wc);
                    }
                }

                len -= 2;
                offset += 2;
            }
        }

        else if (utf16le_ == encoding_) {
            const char * p = buffer+offset;
            char16_t sur = 0;

            while (len >= 2) {
                char16_t wc = uint8_t(p[1]);
                wc <<= 8; wc += uint8_t(p[0]);

                if (0 != sur) {
                    if (char16_is_surrogate(wc)) {
                        *obp++ = char32_from_surrogate(sur, wc);
                    }

                    // FIXME Simply skip character on error? Is it correct?
                    else {
                        std::cerr << "** Buffer::insert(stream): (" << encoding_.name() << "), position "
                        << fpos+offset << ": skip surrogate (?) pair " << std::hex << std::showbase
                        << sur << ':' << std::hex << std::showbase << wc << std::dec << std::endl;
                    }

                    sur = 0;
                }

                else {
                    if (char16_is_surrogate(wc)) {
                        sur = wc;
                    }

                    else {
                        *obp++ = char32_t(wc);
                    }
                }

                p += 2;
                len -= 2;
                offset += 2;
            }
        }

        // Assume UTF-8?
        else {
            if (!not8) {
                // Incomplete sequence stored in acc.
                if (!acc.empty()) {
                    std::size_t u8len = utf8_len(acc[0]), more = u8len-acc.size();

                    // Assume encoding is not UTF-8.
                    if (more > len) {
                        not8 = true;
                    }

                    else {
                        while (0 != more--) { acc += buffer[offset++]; }
                        ustring uacc(acc);
                        *obp++ = char32_t(uacc[0]);
                        offset += more;
                        acc.clear();
                    }
                }
            }

            if (!not8) {
                int n8bytes = 7;

                for (std::size_t pos = epos-1; pos >= offset && (buffer[pos] < 0) && n8bytes; --pos, --n8bytes) {
                    char c = buffer[pos];

                    // Found UTF-8 sequence leader character?
                    if ('\xc0' == ('\xc0' & c)) {

                        // Has incomplete sequence within buffer?
                        if (utf8_len(c) > epos-pos) {
                            acc.assign(buffer+pos-1, epos-pos);
                            epos = pos;
                            // std::cout << "c0 " << " " << epos << " " << epos-pos << "\n";
                        }

                        break;
                    }
                }

                not8 = !n8bytes;
            }

            if (!not8) {
                for (const char * p = buffer+offset; offset < epos; p = utf8_next(p)) {
                    char32_t wc = char32_from_pointer(p);

                    if (!wc) {
                        not8 = true;
                        iter = insert(iter, std::u32string(ob, obp-ob));
                        obp = ob;
                        // std::cout << "non UTF-8 " << fpos+offset << " " << acc.size() << "\n";
                        break;
                    }

                    *obp++ = wc;
                    offset += utf8_len(*p);
                }
            }

            // Not UTF-8, so far simply skip non UTF-8 characters.
            if (not8) {
                change_encoding(Encoding("ASCII"));

                for (const char * p = buffer+offset; p < buffer+epos; ++p) {
                    if (*p > 0) {
                        *obp++ = char32_t(*p);
                    }
                }
            }
        }

        iter = insert(iter, std::u32string(ob, obp-ob));
    }

    return iter;
}

void Buffer_impl::save(std::ostream & os) {
    if (utf16be_ == encoding_) {
        if (bom_) { os.write("\xfe\xff", 2); }

        for (auto & row: rows_) {
            for (char32_t wc: row.s) {
                char16_t c1, c2;
                char32_to_surrogate(wc, c1, c2);
                os.put(c1);
                os.put(c1 >> 8);

                if (0 != c2) {
                    os.put(c2);
                    os.put(c2 >> 8);
                }
            }
        }
    }

    else if (utf16le_ == encoding_) {
        if (bom_) { os.write("\xff\xfe", 2); }

        for (auto & row: rows_) {
            for (char32_t wc: row.s) {
                char16_t c1, c2;
                char32_to_surrogate(wc, c1, c2);
                os.put(c1 >> 8);
                os.put(c1);

                if (0 != c2) {
                    os.put(c2 >> 8);
                    os.put(c2);
                }
            }
        }
    }

    else if (utf32be_ == encoding_) {
        if (bom_) { os.write("\x00\x00\xfe\xff", 4); }

        for (auto & row: rows_) {
            for (char32_t wc: row.s) {
                os.put(wc);
                os.put(wc >> 8);
                os.put(wc >> 16);
                os.put(wc >> 24);
            }
        }
    }

    else if (utf32le_ == encoding_) {
        if (bom_) { os.write("\xff\xfe\x00\x00", 4); }

        for (auto & row: rows_) {
            for (char32_t wc: row.s) {
                os.put(wc >> 24);
                os.put(wc >> 16);
                os.put(wc >> 8);
                os.put(wc);
            }
        }
    }

    else {
        if (utf8_ == encoding_) {
            if (bom_) {
                os.write("\xef\xbb\xbf", 3);
            }
        }

        for (auto & row: rows_) {
            ustring s(row.s);
            os.write(s.c_str(), s.bytes());
        }
    }

    changed_ = false;
    signal_flush_();
}

void Buffer_impl::save() {
    if (path_.empty()) {
        throw user_error("Buffer_impl::save(): was not created using load_from_file()");
    }
}

} // namespace tau

//END
