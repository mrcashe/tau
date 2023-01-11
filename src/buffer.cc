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
    impl->newlines = str_newlines().to_u32string();
}

Buffer::Buffer(const ustring & s):
    impl(std::make_shared<Buffer_impl>())
{
    impl->newlines = str_newlines().to_u32string();
    assign(s);
}

Buffer::Buffer(std::istream & is):
    impl(std::make_shared<Buffer_impl>())
{
    impl->newlines = str_newlines().to_u32string();
    insert(end(), is);
}

void Buffer::assign(const ustring & str) {
    clear();
    insert(end(), str);
}

void Buffer::assign(const std::u32string & str) {
    clear();
    insert(end(), str);
}

void Buffer::assign(const Buffer other) {
    clear();
    insert(end(), other.text());
}

Buffer_iter Buffer::replace(Buffer_iter i, const ustring & str) {
    return replace(i, str.to_u32string());
}

Buffer_iter Buffer::replace(Buffer_iter i, const std::u32string & str) {
    if (locked()) {
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
            std::u32string replaced_text;

            if (0 != n_repl) {
                replaced_text = d.substr(i.col(), n_repl);
                d.replace(i.col(), n_repl, str, n, n_repl);
                std::size_t col = i.col()+n_repl;
                impl->signal_replace(i, iter(i.row(), col), replaced_text);
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

    return i;
}

Buffer_iter Buffer::insert(Buffer_iter i, char32_t uc, std::size_t count) {
    return 0 != count ? insert(i, std::u32string(count, uc)) : i;
}

Buffer_iter Buffer::insert(Buffer_iter i, const ustring & str) {
    return insert(i, str.to_u32string());
}

Buffer_iter Buffer::insert(Buffer_iter i, const std::u32string & str) {
    if (locked()) {
        return i;
    }

    std::size_t n = 0, len = str.size(), row, col, eol;

    if (impl->rows.empty()) {
        row = 0;
        col = 0;
    }

    else if (i.row() < impl->rows.size()) {
        row = i.row();
        col = std::min(impl->rows[row].s.size(), i.col());
    }

    else {
        row = impl->rows.size()-1;
        col = impl->rows[row].s.size();
    }

    while (n < len) {
        eol = str.find_first_of(impl->newlines, n);

        // No EOL character.
        // Add text at current position and exit.
        if (std::u32string::npos == eol) {
            if (impl->rows.empty()) { impl->rows.emplace_back(str.substr(n)); }
            else { impl->rows[row].s.insert(col, str, n, len-n); }
            col += len-n;
            n = len;
        }

        // Have EOL character.
        else {
            // Insert text between col and next at current position.
            std::u32string::size_type next, eeol = std::u32string::npos;

            if (0x000a == str[eol] && eol+1 < len && 0x000d == str[eol+1]) { eeol = eol+2; }
            else if (0x000d == str[eol] && eol+1 < len && 0x000a == str[eol+1]) { eeol = eol+2; }
            else { eeol = eol+1; }

            std::u32string newline = std::u32string::npos == eeol ? str.substr(eol) : str.substr(eol, eeol-eol);
            next = eol+newline.size();

            if (impl->rows.empty()) {
                impl->rows.emplace_back(str.substr(n, next-n));
                impl->rows.emplace_back();
            }

            else {
                std::u32string right = impl->rows[row].s.substr(col);
                impl->rows[row].s.erase(col);
                impl->rows[row].s.insert(col, str, n, next-n);
                impl->rows.emplace(impl->rows.begin()+row+1, right);
            }

            n = next;
            ++row;
            col = 0;
        }
    }

    Buffer_iter e(iter(row, col));
    signal_insert()(i, e);
    return e;
}

Buffer_iter Buffer::erase(Buffer_iter b, Buffer_iter e) {
    Buffer_iter ret(b);

    if (!locked() && !empty()) {
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
                ret = iter(row2, col2);
                impl->signal_erase(b, ret, erased_text);
            }
        }
    }

    return ret;
}

void Buffer::save(std::ostream & os) const {
    if (Encoding("UTF-8") == impl->encoding) {
        if (impl->bom) { os.write("\xef\xbb\xbf", 3); }

        for (auto & row: impl->rows) {
            ustring s = ustring::from_u32string(row.s);
            os.write(s.c_str(), s.bytes());
        }
    }

    else if (Encoding("UTF-16BE") == impl->encoding) {
        if (impl->bom) { os.write("\xfe\xff", 2); }

        for (auto & row: impl->rows) {
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

    else if (Encoding("UTF-16LE") == impl->encoding) {
        if (impl->bom) { os.write("\xff\xfe", 2); }

        for (auto & row: impl->rows) {
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

    else if (Encoding("UTF-32BE") == impl->encoding) {
        if (impl->bom) { os.write("\x00\x00\xfe\xff", 4); }

        for (auto & row: impl->rows) {
            for (char32_t wc: row.s) {
                os.put(wc);
                os.put(wc >> 8);
                os.put(wc >> 16);
                os.put(wc >> 24);
            }
        }
    }

    else if (Encoding("UTF-32LE") == impl->encoding) {
        if (impl->bom) { os.write("\xff\xfe\x00\x00", 4); }

        for (auto & row: impl->rows) {
            for (char32_t wc: row.s) {
                os.put(wc >> 24);
                os.put(wc >> 16);
                os.put(wc >> 8);
                os.put(wc);
            }
        }
    }
}

void Buffer::save_to_file(const ustring & path) const {
    tau::Locale lc;
    std::ofstream os(lc.encode_filename(path));
    if (!os.good()) { throw sys_error(); }
    save(os);
    os.close();
}

std::size_t Buffer::size() const {
    return impl->size();
}

std::size_t Buffer::length(Buffer_iter b, Buffer_iter e) const {
    return impl->length(b.row(), b.col(), e.row(), e.col());
}

std::size_t Buffer::lines() const {
    return impl->lines();
}

bool Buffer::empty() const {
    return impl->empty();
}

Encoding Buffer::encoding() const {
    return impl->encoding;
}

void Buffer::change_encoding(const Encoding & enc) {
    if (impl->encoding != enc) {
        impl->encoding = enc;
        impl->signal_encoding_changed(impl->encoding);
    }
}

Buffer_iter Buffer::iter(std::size_t row, std::size_t col) {
    return Buffer_iter(Buffer_iter_impl::create(impl, row, col));
}

const Buffer_iter Buffer::iter(std::size_t row, std::size_t col) const {
    return Buffer_iter(Buffer_iter_impl::create(impl, row, col));
}

Buffer_iter Buffer::begin() {
    return iter(0, 0);
}

const Buffer_iter Buffer::begin() const {
    return iter(0, 0);
}

Buffer_iter Buffer::end() {
    std::size_t row = 0, col = 0;

    if (0 != lines()) {
        row = lines()-1;
        col = impl->length(row);
    }

    return iter(row, col);
}

const Buffer_iter Buffer::end() const {
    std::size_t row = 0, col = 0;

    if (0 != lines()) {
        row = lines()-1;
        col = impl->length(row);
    }

    return iter(row, col);
}

void Buffer::clear() {
    erase(begin(), end());
}

ustring Buffer::text() const {
    return text(begin(), end());
}

ustring Buffer::text(Buffer_iter b, Buffer_iter e) const {
    return ustring::from_u32string(impl->text(b.row(), b.col(), e.row(), e.col()));
}

std::u32string Buffer::text32() const {
    return text32(begin(), end());
}

std::u32string Buffer::text32(Buffer_iter b, Buffer_iter e) const {
    return impl->text(b.row(), b.col(), e.row(), e.col());
}

Buffer_iter Buffer::insert(Buffer_iter iter, std::istream & is) {
    if (!locked()) {
        char buffer[2048];
        std::size_t fpos = 0;
        std::string acc;
        bool first = true;

        while (!is.eof()) {
            is.read(buffer, sizeof(buffer));
            std::size_t offset = 0, len = is.gcount();

            if (first) {
                first = false;

                if (len >= 4 && '\0' == buffer[0] && '\0' == buffer[1] && '\xfe' == buffer[2] && '\xff' == buffer[3]) {
                    offset += 4;
                    len -= 4;
                    change_encoding(Encoding("UTF-32BE"));
                    enable_bom();
                }

                else if (len >= 4 && '\xff' == buffer[0] && '\xfe' == buffer[1] && '\0' == buffer[2] && '\0' == buffer[3]) {
                    offset += 4;
                    len -= 4;
                    change_encoding(Encoding("UTF-32LE"));
                    enable_bom();
                }

                else if (len >= 3 && '\xef' == buffer[0] && '\xbb' == buffer[1] && '\xbf' == buffer[2]) {
                    offset += 3;
                    len -= 3;
                    change_encoding(Encoding("UTF-8"));
                    enable_bom();
                }

                else if (len >= 2 && '\xfe' == buffer[0] && '\xff' == buffer[1]) {
                    offset += 2;
                    len -= 2;
                    change_encoding(Encoding("UTF-16BE"));
                    enable_bom();
                }

                else if (len >= 2 && '\xff' == buffer[0] && '\xfe' == buffer[1]) {
                    offset += 2;
                    len -= 2;
                    change_encoding(Encoding("UTF-16LE"));
                    enable_bom();
                }
            }

            if (Encoding("UTF-8") == impl->encoding) {
                if (!acc.empty()) {
                    std::size_t u8len = char8_len(acc[0]), more = u8len-acc.size();

                    if (more > len) {
                        throw std::length_error(str_format(fpos+offset, "incomplete UTF-8 sequence"));
                    }

                    while (0 != more--) {
                        acc += buffer[offset++];
                    }

                    iter = insert(iter, acc);
                    fpos += acc.size();
                    acc.clear();
                }

                if (0 != len) {
                    std::size_t pos = len-1;

                    if (buffer[pos] < 0) {
                        while (pos > offset) {
                            char c = buffer[pos];

                            if ('\xc0' == ('\xc0' & c)) {
                                if (char8_len(c) > len-pos) {
                                    acc = std::string(buffer+pos, len-pos);
                                    --pos;
                                }

                                else {
                                    pos = len-1;
                                }

                                break;
                            }

                            --pos;
                            if (len-pos > 6) {
                                throw std::length_error(str_format(fpos+pos, ": UTF-8 sequence (", len-pos, " bytes) is too long"));
                            }
                        }
                    }

                    len = pos-offset+1;
                    fpos += len;
                    const char * pe = buffer+pos+1;
                    std::u32string ws;
                    for (const char * p = buffer+offset; p < pe; p = str8_next(p)) { ws += char32_from_pointer(p); }
                    iter = insert(iter, ws);
                }
            } // UTF-8

            else if (Encoding("UTF-32BE") == impl->encoding) {
                const char * p = buffer+offset;
                std::u32string ws;

                while (len >= 4) {
                    char32_t wc = uint8_t(*p++);
                    wc <<= 8; wc += uint8_t(*p++);
                    wc <<= 8; wc += uint8_t(*p++);
                    wc <<= 8; wc += uint8_t(*p++);
                    ws += wc;
                    len -= 4;
                }

                iter = insert(iter, ws);
            }

            else if (Encoding("UTF-32LE") == impl->encoding) {
                const char * p = buffer+offset;
                std::u32string ws;

                while (len >= 4) {
                    char32_t wc = uint8_t(p[3]);
                    wc <<= 8; wc += uint8_t(p[2]);
                    wc <<= 8; wc += uint8_t(p[1]);
                    wc <<= 8; wc += uint8_t(p[0]);
                    ws += wc;
                    p += 4;
                    len -= 4;
                }

                iter = insert(iter, ws);
            }

            else if (Encoding("UTF-16BE") == impl->encoding) {
                const char * p = buffer+offset;
                std::u32string ws;
                char16_t sur = 0;

                while (len > 1) {
                    char16_t wc = uint8_t(*p++);
                    wc <<= 8; wc += uint8_t(*p++);

                    if (0 != sur) {
                        if (char16_is_surrogate(wc)) {
                            ws += char32_from_surrogate(sur, wc);
                            sur = 0;
                        }

                        else {
                            throw bad_encoding(impl->encoding);
                        }
                    }

                    else {
                        if (char16_is_surrogate(wc)) {
                            sur = wc;
                        }

                        else {
                            ws += static_cast<char32_t>(wc);
                        }
                    }

                    len -= 2;
                }

                iter = insert(iter, ws);
            }

            else if (Encoding("UTF-16LE") == impl->encoding) {
                const char * p = buffer+offset;
                std::u32string ws;
                char16_t sur = 0;

                while (len > 1) {
                    char16_t wc = uint8_t(p[1]);
                    wc <<= 8; wc += uint8_t(p[0]);

                    if (0 != sur) {
                        if (char16_is_surrogate(wc)) {
                            ws += char32_from_surrogate(sur, wc);
                            sur = 0;
                        }

                        else {
                            throw bad_encoding(impl->encoding);
                        }
                    }

                    else {
                        if (char16_is_surrogate(wc)) {
                            sur = wc;
                        }

                        else {
                            ws += static_cast<char32_t>(wc);
                        }
                    }

                    p += 2;
                    len -= 2;
                }

                iter = insert(iter, ws);
            }

            else {
                throw bad_encoding(impl->encoding);
            }
        }
    }

    return iter;
}

Buffer Buffer::load_from_file(const ustring & path) {
    std::ifstream is(Locale().encode(path), std::ios::binary);
    if (!is.good()) { throw sys_error(path); }
    Buffer buffer;
    buffer.insert(buffer.end(), is);
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
    if (!impl->bom) {
        impl->bom = true;
        impl->signal_bom_changed();
    }
}

void Buffer::disable_bom() {
    if (impl->bom) {
        impl->bom = false;
        impl->signal_bom_changed();
    }
}

bool Buffer::bom_enabled() const {
    return impl->bom;
}

signal<void(Buffer_iter, Buffer_iter, std::u32string)> & Buffer::signal_erase() {
    return impl->signal_erase;
}

signal<void(Buffer_iter, Buffer_iter)> & Buffer::signal_insert() {
    return impl->signal_insert;
}

signal<void(Buffer_iter, Buffer_iter, std::u32string)> & Buffer::signal_replace() {
    return impl->signal_replace;
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
