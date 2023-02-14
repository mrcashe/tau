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

#include <tau/exception.hh>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace tau {

static char32_t * utf8_to_ucs4_fast(const char * str, long len, long * items_written) {
    char32_t * result;
    int n_chars = 0, i;
    const char * p = str;

    if (len < 0) {
        while (*p) {
            p = utf8_next(p);
            ++n_chars;
        }
    }

    else {
        while (p < str+len && *p) {
            p = utf8_next(p);
            ++n_chars;
        }
    }

    result = new char32_t[n_chars + 1];
    p = str;

    for (i = 0; i < n_chars; ++i) {
        char32_t wc = (uint8_t)*p++;

        if (wc < 0x80) {
            result[i] = wc;
        }

        else {
            char32_t mask = 0x40;

            if ((wc & mask) == 0) {
                // It's an out-of-sequence 10xxxxxxx byte.
                // Rather than making an ugly hash of this and the next byte
                // and overrunning the buffer, it's more useful to treat it
                // with a replacement character
                result[i] = 0xfffd;
                continue;
            }

            do {
                wc <<= 6;
                wc |= (uint8_t)(*p++) & 0x3f;
                mask <<= 5;
            } while((wc & mask) != 0);

            wc &= mask - 1;
            result[i] = wc;
        }
    }

    result[i] = 0;
    if (items_written) { *items_written = i; }
    return result;
}

static long utf8_pointer_to_offset(const char * str, const char * pos) {
    const char * s = str;
    long offset = 0;

    if (pos < str) {
        offset = -utf8_pointer_to_offset(pos, str);
    }

    else {
        while (s < pos) {
            s = utf8_next(s);
            offset++;
        }
    }

    return offset;
}

static char * utf8_offset_to_pointer(const char * str, long offset) {
    const char * s = str;

    if (offset >= 0) {
        while (offset--) {
            s = utf8_next(s);
        }
    }

    else {
        while (offset < 0) {
            const char * s1 = s;
            s += offset;
            while ((*s & 0xc0) == 0x80) { --s; }
            offset += utf8_pointer_to_offset(s, s1);
        }
    }

    return (char *)s;
}

// First overload: stop on '\0' character.
static ustring::size_type utf8_byte_offset(const char * str, ustring::size_type offset) {
    if (offset == ustring::npos) { return ustring::npos; }
    const char * p;

    for (p = str; 0 != offset; --offset) {
        if ('\0' == *p) { return ustring::npos; }
        p += utf8_len(*p);
    }

    return p-str;
}

// Second overload: stop when reaching maxlen.
static ustring::size_type utf8_byte_offset(const char * str, ustring::size_type offset, ustring::size_type maxlen) {
    if (offset == ustring::npos) { return ustring::npos; }

    const char * const pend = str+maxlen;
    const char * p;

    for (p = str; offset != 0; --offset) {
        if (p >= pend) { return ustring::npos; }
        p += utf8_len(*p);
    }

    return p-str;
}

// Converts byte offset to UTF-8 character offset.
inline ustring::size_type utf8_char_offset(const std::string & str, ustring::size_type offset) {
    if (offset == ustring::npos) { return ustring::npos; }
    const char * const pdata = str.data();
    return utf8_pointer_to_offset(pdata, pdata+offset);
}

// Helper to implement ustring::find_first_of() and find_first_not_of().
// Takes UTF-8 character offset and count in ci and cn.
// Returns the byte offset and count in i and n.
struct utf_substr_bounds {
    ustring::size_type i, n;

    utf_substr_bounds(const std::string & str, ustring::size_type ci, ustring::size_type cn):
        i(utf8_byte_offset(str.data(), ci, str.size())),
        n(ustring::npos)
    {
        if (i != ustring::npos) {
            n = utf8_byte_offset(str.data()+i, cn, str.size()-i);
        }
    }
};

// Returns the UTF-8 character offset, or ustring::npos if not found.
static ustring::size_type utf8_find_first_of(const std::string & str,
                                             ustring::size_type offset,
                                             const char * utf8_match,
                                             long utf8_match_size,
                                             bool find_not_of)
{
    const ustring::size_type byte_offset = utf8_byte_offset(str.data(), offset, str.size());
    if (byte_offset == ustring::npos) { return ustring::npos; }

    long ucs4_match_size = 0;
    const std::unique_ptr<char32_t> ucs4_match(utf8_to_ucs4_fast(utf8_match, utf8_match_size, &ucs4_match_size));
    const char32_t * const match_begin = ucs4_match.get();
    const char32_t * const match_end   = match_begin+ucs4_match_size;
    const char * const str_begin = str.data();
    const char * const str_end = str_begin+str.size();

    for (const char * pstr = str_begin+byte_offset; pstr < str_end; pstr = utf8_next(pstr)) {
        const char32_t * const pfound = std::find(match_begin, match_end, char32_from_pointer(pstr));
        if ((pfound != match_end) != find_not_of) return offset;
        ++offset;
    }

    return ustring::npos;
}

// Helper to implement ustring::find_last_of() and find_last_not_of().
// Returns the UTF-8 character offset, or ustring::npos if not found.
static ustring::size_type utf8_find_last_of(const std::string & str,
                                            ustring::size_type offset,
                                            const char * utf8_match,
                                            long utf8_match_size,
                                            bool find_not_of)
{
    long ucs4_match_size = 0;
    const std::unique_ptr<char32_t> ucs4_match(utf8_to_ucs4_fast(utf8_match, utf8_match_size, &ucs4_match_size));
    const char32_t * const match_begin = ucs4_match.get();
    const char32_t * const match_end   = match_begin + ucs4_match_size;
    const char * const str_begin = str.data();
    const char * pstr = str_begin;
    ustring::size_type str_len = str.size();

    // Set pstr one byte beyond the actual start position.
    const ustring::size_type byte_offset = utf8_byte_offset(str.data(), offset, str_len);
    pstr += byte_offset < str_len ? byte_offset + 1 : str_len;

    while (pstr > str_begin) {
        // Move to previous character.
        do { --pstr; } while((static_cast<unsigned char>(*pstr) & 0xC0u) == 0x80);
        const char32_t * const pfound = std::find(match_begin, match_end, char32_from_pointer(pstr));
        if ((pfound != match_end) != find_not_of) return utf8_pointer_to_offset(str_begin, pstr);
    }

    return ustring::npos;
}

char32_t char32_from_iterator(std::string::const_iterator pos) {
    char32_t result = static_cast<unsigned char>(*pos);

    if((result & 0x80) != 0) {
        unsigned int mask = 0x40;

        do {
            result <<= 6;
            const unsigned int c = static_cast<unsigned char>(*++pos);
            mask   <<= 5;
            result += c-0x80;
        } while((result & mask) != 0);

        result &= mask-1;
    }

    return result;
}

#if 0
int utf8_collate (const char * str1, const char * str2) {
    int result;

    const char * charset;
    char * str1_norm;
    char * str2_norm;

    str1_norm = utf8_normalize (str1, -1);
    str2_norm = utf8_normalize (str2, -1);

    if (g_get_charset (&charset)) {
        result = strcoll (str1_norm, str2_norm);
}

else {
    char * str1_locale = convert(str1_norm, -1, charset, "UTF-8", 0, 0, 0);
    char * str2_locale = convert(str2_norm, -1, charset, "UTF-8", 0, 0, 0);

    if (str1_locale && str2_locale)
        result =  strcoll (str1_locale, str2_locale);
    else if (str1_locale)
        result = -1;
    else if (str2_locale)
        result = 1;
    else
        result = strcmp(str1_norm, str2_norm);

    free (str1_locale);
    free (str2_locale);
}

free (str1_norm);
free (str2_norm);
return result;
}
#endif

ustring::ustring():
    str_()
{
}

ustring::~ustring() {}

ustring::ustring(const ustring & other):
    str_(other.str_),
    size_(other.size_)
{
}

ustring::ustring(ustring && other):
    str_(std::move(other.str_))
{
    other.size_ = npos;
}

ustring::ustring(const ustring & src, size_type i, size_type n):
    str_()
{
    const utf_substr_bounds bounds(src.str_, i, n);
    str_.assign(src.str_, bounds.i, bounds.n);
}

ustring::ustring(const char * src, size_type n):
    str_(src, utf8_byte_offset(src, n))
{
}

ustring::ustring(const char * src):
    str_ (src)
{
}

ustring::ustring(ustring::size_type n, char32_t uc):
    str_(),
    size_(n)
{
    if (uc < 0x80) {
        str_.assign(n, static_cast<char>(uc));
    }

    else {
        const std::string s = char32_to_string(uc);
        while (n--) { str_.append(s); }
    }
}

ustring::ustring(size_type n, char c):
    str_(n, c)
{
}

ustring::ustring(const std::string & src):
    str_(src)
{
}

ustring::ustring(std::string && src):
    str_(std::move(src))
{
}

ustring::ustring(const std::u32string & src) {
    for (const char32_t * p = src.c_str(); *p; ++p) {
        push_back(*p);
    }
}

ustring::ustring(const std::u16string & ws) {
    char16_t surr = 0;

    for (const char16_t * p = ws.c_str(); *p; ++p) {
        char16_t wc = *p;

        if (char16_is_surrogate(wc)) {
            if (0 != surr) {
                push_back(char32_from_surrogate(surr, wc));
                surr = 0;
            }

            else {
                surr = wc;
            }
        }

        else {
            if (0 != surr) { break; }
            else { push_back(char32_t(wc)); }
        }
    }
}

void ustring::swap(ustring & other) {
    str_.swap(other.str_);
    std::swap(size_, other.size_);
}

ustring & ustring::operator=(const ustring & other) {
    if (this != &other) {
        str_ = other.str_;
        size_ = npos;
    }

    return *this;
}

ustring & ustring::operator=(ustring && other) {
    if (this != &other) {
        str_ = std::move(other.str_);
        size_ = other.size_;
        other.size_ = npos;
    }

    return *this;
}

ustring & ustring::operator=(const std::string & src) {
    str_ = src;
    size_ = npos;
    return *this;
}

ustring & ustring::operator=(std::string && src) {
    str_ = std::move(src);
    size_ = npos;
    return *this;
}

ustring & ustring::operator=(const char * src) {
    str_ = src;
    size_ = npos;
    return *this;
}

ustring & ustring::operator=(char32_t uc) {
    str_ = char32_to_string(uc);
    size_ = npos;
    return *this;
}

ustring & ustring::operator=(char c) {
    str_ = c;
    size_ = npos;
    return *this;
}

ustring & ustring::assign(const ustring & src) {
    str_ = src.str_;
    size_ = src.size_;
    return *this;
}

ustring & ustring::assign(const ustring & src, size_type i, size_type n) {
    const utf_substr_bounds bounds (src.str_, i, n);
    str_.assign(src.str_, bounds.i, bounds.n);
    size_ = npos;
    return *this;
}

ustring & ustring::assign(const char * src, size_type n) {
    str_.assign(src, utf8_byte_offset(src, n));
    size_ = npos;
    return *this;
}

ustring& ustring::assign(const char * src) {
    str_ = src;
    size_ = npos;
    return *this;
}

ustring & ustring::assign(size_type n, char32_t uc) {
    ustring temp(n, uc);
    str_.swap(temp.str_);
    size_ = n;
    return *this;
}

ustring & ustring::assign(size_type n, char c) {
    str_.assign(n, c);
    size_ = npos;
    return *this;
}

ustring & ustring::operator+=(const ustring & src) {
    str_ += src.str_;
    size_ = npos;
    return *this;
}

ustring & ustring::operator+=(const char * src) {
    str_ += src;
    size_ = npos;
    return *this;
}

ustring & ustring::operator+=(char32_t wc) {
    push_back(wc);
    return *this;
}

void ustring::push_back(char32_t wc) {
    char buffer[8];
    char32_to_utf8(wc, buffer, sizeof(buffer));
    str_ += buffer;
    size_ = npos;
}

ustring & ustring::operator+=(char c) {
    str_ += c;
    size_ = npos;
    return *this;
}

void ustring::push_back(char c) {
    str_ += c;
    size_ = npos;
}

ustring & ustring::append(const ustring & src) {
    str_.append(src.str_);
    size_ = npos;
    return *this;
}

ustring & ustring::append(const ustring & src, size_type i, size_type n) {
    const utf_substr_bounds bounds(src.str_, i, n);
    str_.append(src.str_, bounds.i, bounds.n);
    size_ = npos;
    return *this;
}

ustring & ustring::append(const char * src, size_type n) {
    str_.append(src, utf8_byte_offset(src, n));
    size_ = npos;
    return *this;
}

ustring & ustring::append(const char * src) {
    str_ += src;
    size_ = npos;
    return *this;
}

ustring & ustring::append(size_type n, char32_t uc) {
    str_.append(ustring(n, uc).str_);
    size_ = npos;
    return *this;
}

ustring & ustring::append(size_type n, char c) {
    str_.append(n, c);
    size_ = npos;
    return *this;
}

ustring & ustring::insert(size_type i, const ustring & src) {
    str_.insert(utf8_byte_offset(str_.data(), i, str_.size()), src.str_);
    size_ = npos;
    return *this;
}

ustring & ustring::insert(size_type i, const ustring & src, size_type i2, size_type n) {
    const utf_substr_bounds bounds2(src.str_, i2, n);
    str_.insert(utf8_byte_offset(str_.data(), i, str_.size()), src.str_, bounds2.i, bounds2.n);
    size_ = npos;
    return *this;
}

ustring & ustring::insert(size_type i, const char * src, size_type n) {
    str_.insert(utf8_byte_offset(str_.data(), i, str_.size()), src, utf8_byte_offset(src, n));
    size_ = npos;
    return *this;
}

ustring & ustring::insert(size_type i, const char * src) {
    str_.insert(utf8_byte_offset(str_.data(), i, str_.size()), src);
    size_ = npos;
    return *this;
}

ustring & ustring::insert(size_type i, size_type n, char32_t uc) {
    str_.insert(utf8_byte_offset(str_.data(), i, str_.size()), ustring(n, uc).str_);
    size_ = npos;
    return *this;
}

ustring & ustring::insert(size_type i, size_type n, char c) {
    str_.insert(utf8_byte_offset(str_.data(), i, str_.size()), n, c);
    size_ = npos;
    return *this;
}

ustring::iterator ustring::insert(iterator p, char32_t uc) {
    const size_type offset = p.base()-str_.begin();
    str_.insert(offset, char32_to_string(uc));
    size_ = npos;
    return iterator(str_.begin()+offset);
}

ustring::iterator ustring::insert(iterator p, char c) {
    size_ = npos;
    return iterator(str_.insert(p.base(), c));
}

void ustring::insert(iterator p, size_type n, char32_t uc) {
    size_ = npos;
    str_.insert(p.base()-str_.begin(), ustring(n, uc).str_);
}

void ustring::insert(iterator p, size_type n, char c) {
    size_ = npos;
    str_.insert(p.base(), n, c);
}

ustring & ustring::replace(size_type i, size_type n, const ustring & src) {
    const utf_substr_bounds bounds(str_, i, n);
    str_.replace(bounds.i, bounds.n, src.str_);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(size_type i, size_type n, const ustring & src, size_type i2, size_type n2) {
    const utf_substr_bounds bounds (str_, i, n);
    const utf_substr_bounds bounds2 (src.str_, i2, n2);
    str_.replace(bounds.i, bounds.n, src.str_, bounds2.i, bounds2.n);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(size_type i, size_type n, const char * src, size_type n2) {
    const utf_substr_bounds bounds (str_, i, n);
    str_.replace(bounds.i, bounds.n, src, utf8_byte_offset(src, n2));
    size_ = npos;
    return *this;
}

ustring & ustring::replace(size_type i, size_type n, const char * src) {
    const utf_substr_bounds bounds(str_, i, n);
    str_.replace(bounds.i, bounds.n, src);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(size_type i, size_type n, size_type n2, char32_t uc) {
    const utf_substr_bounds bounds(str_, i, n);
    str_.replace(bounds.i, bounds.n, ustring(n2, uc).str_);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(size_type i, size_type n, size_type n2, char c) {
    const utf_substr_bounds bounds(str_, i, n);
    str_.replace(bounds.i, bounds.n, n2, c);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(iterator pbegin, iterator pend, const ustring & src) {
    str_.replace(pbegin.base(), pend.base(), src.str_);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(iterator pbegin, iterator pend, const char * src, size_type n) {
    str_.replace(pbegin.base(), pend.base(), src, utf8_byte_offset(src, n));
    size_ = npos;
    return *this;
}

ustring & ustring::replace(iterator pbegin, iterator pend, const char * src) {
    str_.replace(pbegin.base(), pend.base(), src);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(iterator pbegin, iterator pend, size_type n, char32_t uc) {
    str_.replace(pbegin.base(), pend.base(), ustring(n, uc).str_);
    size_ = npos;
    return *this;
}

ustring & ustring::replace(iterator pbegin, iterator pend, size_type n, char c) {
    str_.replace(pbegin.base(), pend.base(), n, c);
    size_ = npos;
    return *this;
}

void ustring::clear() {
    str_.erase();
    size_ = 0;
}

ustring & ustring::erase(size_type i, size_type n) {
    const utf_substr_bounds bounds(str_, i, n);
    str_.erase(bounds.i, bounds.n);
    size_ = npos;
    return *this;
}

ustring & ustring::erase() {
    str_.erase();
    size_ = 0;
    return *this;
}

ustring::iterator ustring::erase(iterator p) {
    iterator iter_end = p;
    ++iter_end;
    size_ = npos;
    return iterator(str_.erase(p.base(), iter_end.base()));
}

ustring::iterator ustring::erase(iterator pbegin, iterator pend) {
    size_ = npos;
    return iterator(str_.erase(pbegin.base(), pend.base()));
}

int ustring::compare(const char * rhs) const {
    static const char * mt = "";
    return strcoll(size_ ? str_.c_str() : mt, rhs ? rhs : mt);
}

int ustring::compare(const ustring & rhs) const {
    return compare(rhs.str_.c_str());
}

int ustring::compare(size_type i, size_type n, const ustring & rhs) const {
    return ustring(*this, i, n).compare(rhs);
}

int ustring::compare(size_type i, size_type n, const ustring & rhs, size_type i2, size_type n2) const {
    return ustring(*this, i, n).compare(ustring(rhs, i2, n2));
}

int ustring::compare(size_type i, size_type n, const char * rhs, size_type n2) const {
    return ustring(*this, i, n).compare(ustring(rhs, n2));
}

int ustring::compare(size_type i, size_type n, const char * rhs) const {
    return ustring(*this, i, n).compare(rhs);
}

ustring::value_type ustring::operator[](size_type i) const {
    return char32_from_pointer(utf8_offset_to_pointer(str_.data(), i));
}

ustring::value_type ustring::at(size_type i) const {
    const size_type byte_offset = utf8_byte_offset(str_.data(), i, str_.size());
    return char32_from_pointer(&str_.at(byte_offset));
}

ustring::iterator ustring::begin() {
    return iterator(str_.begin());
}

ustring::iterator ustring::end() {
    return iterator(str_.end());
}

ustring::const_iterator ustring::begin() const {
    return const_iterator(str_.begin());
}

ustring::const_iterator ustring::end() const {
    return const_iterator(str_.end());
}

ustring::reverse_iterator ustring::rbegin() {
    return reverse_iterator(iterator(str_.end()));
}

ustring::reverse_iterator ustring::rend() {
    return reverse_iterator(iterator(str_.begin()));
}

ustring::const_reverse_iterator ustring::rbegin() const {
    return const_reverse_iterator(const_iterator(str_.end()));
}

ustring::const_reverse_iterator ustring::rend() const {
    return const_reverse_iterator(const_iterator(str_.begin()));
}

ustring::size_type ustring::find(const ustring & str, size_type i) const {
    return utf8_char_offset(str_, str_.find(str.str_, utf8_byte_offset(str_.data(), i, str_.size())));
}

ustring::size_type ustring::find(const char * str, size_type i, size_type n) const {
    return utf8_char_offset(str_, str_.find(str, utf8_byte_offset(str_.data(), i, str_.size()), utf8_byte_offset(str, n)));
}

ustring::size_type ustring::find(const char * str, size_type i) const {
    return utf8_char_offset(str_, str_.find(str, utf8_byte_offset(str_.data(), i, str_.size())));
}

ustring::size_type ustring::find(char32_t uc, size_type i) const {
    const std::string s = char32_to_string(uc);
    return utf8_char_offset(str_, str_.find(s, utf8_byte_offset(str_.data(), i, str_.size())));
}

ustring::size_type ustring::rfind(const ustring & str, size_type i) const {
    return utf8_char_offset(str_, str_.rfind(str.str_, utf8_byte_offset(str_.data(), i, str_.size())));
}

ustring::size_type ustring::rfind(const char * str, size_type i, size_type n) const {
    return utf8_char_offset(str_, str_.rfind(str, utf8_byte_offset(str_.data(), i, str_.size()), utf8_byte_offset(str, n)));
}

ustring::size_type ustring::rfind(const char * str, size_type i) const {
    return utf8_char_offset(str_, str_.rfind(str, utf8_byte_offset(str_.data(), i, str_.size())));
}

ustring::size_type ustring::rfind(char32_t wc, size_type i) const {
    return utf8_char_offset(str_, str_.rfind(char32_to_string(wc), utf8_byte_offset(str_.data(), i, str_.size())));
}

ustring::size_type ustring::find_first_of(const ustring & match, size_type i) const {
    return utf8_find_first_of(str_, i, match.str_.data(), match.str_.size(), false);
}

ustring::size_type ustring::find_first_of(const char * match, size_type i, size_type n) const {
    return utf8_find_first_of(str_, i, match, n, false);
}

ustring::size_type ustring::find_first_of(const char * match, size_type i) const {
    return utf8_find_first_of(str_, i, match, -1, false);
}

ustring::size_type ustring::find_first_of(char32_t wc, size_type i) const {
    return find(wc, i);
}

ustring::size_type ustring::find_first_of(char c, size_type i) const {
    return find(c, i);
}

ustring::size_type ustring::find_last_of(const ustring & match, size_type i) const {
    return utf8_find_last_of(str_, i, match.str_.data(), match.str_.size(), false);
}

ustring::size_type ustring::find_last_of(const char* match, size_type i, size_type n) const {
    return utf8_find_last_of(str_, i, match, n, false);
}

ustring::size_type ustring::find_last_of(const char* match, size_type i) const {
    return utf8_find_last_of(str_, i, match, -1, false);
}

ustring::size_type ustring::find_last_of(char32_t uc, size_type i) const {
    return rfind(uc, i);
}

ustring::size_type ustring::find_last_of(char c, size_type i) const {
    return rfind(c, i);
}

ustring::size_type ustring::find_first_not_of(const ustring & match, size_type i) const {
    return utf8_find_first_of(str_, i, match.str_.data(), match.str_.size(), true);
}

ustring::size_type ustring::find_first_not_of(const char* match, size_type i, size_type n) const {
    return utf8_find_first_of(str_, i, match, n, true);
}

ustring::size_type ustring::find_first_not_of(const char* match, size_type i) const {
    return utf8_find_first_of(str_, i, match, -1, true);
}

ustring::size_type ustring::find_first_not_of(char32_t uc, size_type i) const {
    const size_type bi = utf8_byte_offset(str_.data(), i, str_.size());

    if(bi != npos) {
        const char * const pbegin = str_.data();
        const char * const pend = pbegin+str_.size();

        for (const char * p = pbegin+bi; p < pend; p = utf8_next(p), ++i) {
            if (char32_from_pointer(p) != uc) { return i; }
        }
    }

    return npos;
}

ustring::size_type ustring::find_first_not_of(char c, size_type i) const {
    const char * pbegin = str_.data();
    size_type str_size = str_.size();
    const size_type bi = utf8_byte_offset(pbegin, i, str_size);

    if (bi != npos) {
        const char * pend = pbegin+str_.size();

        for (const char * p = pbegin+bi; p < pend; p = utf8_next(p), ++i) {
            if (*p != c) { return i; }
        }
    }

    return npos;
}

ustring::size_type ustring::find_last_not_of(const ustring & match, size_type i) const {
    return utf8_find_last_of(str_, i, match.str_.data(), match.str_.size(), true);
}

ustring::size_type ustring::find_last_not_of(const char * match, size_type i, size_type n) const {
    return utf8_find_last_of(str_, i, match, n, true);
}

ustring::size_type ustring::find_last_not_of(const char * match, size_type i) const {
    return utf8_find_last_of(str_, i, match, -1, true);
}

ustring::size_type ustring::find_last_not_of(char32_t uc, size_type i) const {
    const char * const pbegin = str_.data();
    const char * const pend = pbegin+str_.size();
    size_type i_cur = 0;
    size_type i_found = npos;

    for (const char * p = pbegin; p < pend && i_cur <= i; p = utf8_next(p), ++i_cur) {
        if (char32_from_pointer(p) != uc) {
            i_found = i_cur;
        }
    }

    return i_found;
}

ustring::size_type ustring::find_last_not_of(char c, size_type i) const {
    const char * const pbegin = str_.data();
    const char * const pend = pbegin+str_.size();
    size_type i_cur = 0;
    size_type i_found = npos;

    for (const char * p = pbegin; p < pend && i_cur <= i; p = utf8_next(p), ++i_cur) {
        if (*p != c) {
            i_found = i_cur;
        }
    }

    return i_found;
}

bool ustring::empty() const {
    return npos == size_ ? str_.empty() : 0 == size_;
}

ustring::size_type ustring::size() const {
    if (npos == size_) {
        const char * const pdata = str_.data();
        size_ = utf8_pointer_to_offset(pdata, pdata+str_.size());
    }

    return size_;
}

ustring ustring::substr(size_type i, size_type n) const {
    return ustring(*this, i, n);
}


ustring::operator std::string() const {
    return str_;
}

const std::string & ustring::raw() const {
    return str_;
}

ustring::size_type ustring::length() const {
    return size();
}

ustring::size_type ustring::bytes() const {
    return str_.size();
}

ustring::size_type ustring::capacity() const {
    return str_.capacity();
}

ustring::size_type ustring::max_size() const {
    return str_.max_size();
}

void ustring::resize(size_type n, char32_t uc) {
    const size_type size_now = size();

    if (n < size_now) {
        erase(n, npos);
    }

    else if (n > size_now) {
        append(n-size_now, uc);
    }
}

void ustring::resize(size_type n, char c) {
    const size_type size_now = size();

    if (n < size_now) {
        erase(n, npos);
    }

    else if (n > size_now) {
        str_.append(n-size_now, c);
        size_ = npos;
    }
}

void ustring::reserve(size_type n) {
    str_.reserve(n);
    size_ = npos;
}

const char * ustring::data() const {
    return str_.data();
}

const char * ustring::c_str() const {
    return str_.c_str();
}

// Note that copy() requests UTF-8 character offsets as
// parameters, but returns the number of copied bytes.
ustring::size_type ustring::copy(char * dest, size_type n, size_type i) const {
    const utf_substr_bounds bounds(str_, i, n);
    return str_.copy(dest, bounds.n, bounds.i);
}

bool operator==(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs) {
    return (lhs.base() == rhs.base());
}

bool operator!=(const ustring::const_iterator& lhs, const ustring::const_iterator& rhs) {
    return (lhs.base() != rhs.base());
}

bool operator<(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs) {
    return (lhs.base() < rhs.base());
}

bool operator>(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs) {
    return (lhs.base() > rhs.base());
}

bool operator<=(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs) {
    return (lhs.base() <= rhs.base());
}

bool operator>=(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs) {
    return (lhs.base() >= rhs.base());
}

void swap(ustring & lhs, ustring & rhs) {
    lhs.swap(rhs);
}

bool operator==(const ustring & lhs, const ustring & rhs) {
    return (lhs.compare(rhs) == 0);
}

bool operator==(const ustring & lhs, const char * rhs) {
    return (lhs.compare(rhs) == 0);
}

bool operator==(const char * lhs, const ustring & rhs) {
    return (rhs.compare(lhs) == 0);
}

bool operator!=(const ustring & lhs, const ustring & rhs) {
    return (lhs.compare(rhs) != 0);
}

bool operator!=(const ustring & lhs, const char * rhs) {
    return (lhs.compare(rhs) != 0);
}

bool operator!=(const char * lhs, const ustring & rhs) {
    return (rhs.compare(lhs) != 0);
}

bool operator<(const ustring & lhs, const ustring & rhs) {
    return (lhs.compare(rhs) < 0);
}

bool operator<(const ustring & lhs, const char * rhs) {
    return (lhs.compare(rhs) < 0);
}

bool operator<(const char * lhs, const ustring & rhs) {
    return (rhs.compare(lhs) > 0);
}

bool operator>(const ustring & lhs, const ustring & rhs) {
    return (lhs.compare(rhs) > 0);
}

bool operator>(const ustring & lhs, const char * rhs) {
    return (lhs.compare(rhs) > 0);
}

bool operator>(const char * lhs, const ustring & rhs) {
    return (rhs.compare(lhs) < 0);
}

bool operator<=(const ustring & lhs, const ustring & rhs) {
    return (lhs.compare(rhs) <= 0);
}

bool operator<=(const ustring & lhs, const char * rhs) {
    return (lhs.compare(rhs) <= 0);
}

bool operator<=(const char * lhs, const ustring & rhs) {
    return (rhs.compare(lhs) >= 0);
}

bool operator>=(const ustring & lhs, const ustring & rhs) {
    return (lhs.compare(rhs) >= 0);
}

bool operator>=(const ustring & lhs, const char * rhs) {
    return (lhs.compare(rhs) >= 0);
}

bool operator>=(const char * lhs, const ustring & rhs) {
    return (rhs.compare(lhs) <= 0);
}

ustring operator+(const ustring & lhs, const ustring & rhs) {
    ustring temp(lhs);
    temp += rhs;
    return temp;
}

ustring operator+(const ustring & lhs, const char * rhs) {
    ustring temp(lhs);
    temp += rhs;
    return temp;
}

ustring operator+(const char * lhs, const ustring & rhs) {
    ustring temp(lhs);
    temp += rhs;
    return temp;
}

ustring operator+(const ustring & lhs, char32_t rhs) {
    ustring temp(lhs);
    temp += rhs;
    return temp;
}

ustring operator+(char32_t lhs, const ustring & rhs) {
    ustring temp(1, lhs);
    temp += rhs;
    return temp;
}

ustring operator+(const ustring & lhs, char rhs) {
    ustring temp(lhs);
    temp += rhs;
    return temp;
}

ustring operator+(char lhs, const ustring & rhs) {
    ustring temp(1, lhs);
    temp += rhs;
    return temp;
}

ustring::operator std::u16string() const {
    std::u16string ws;

    for (const char * p = str_.c_str(); '\0' != *p; p = utf8_next(p)) {
        char32_t wc = char32_from_pointer(p);
        char16_t c1, c2;
        char32_to_surrogate(wc, c1, c2);
        ws.push_back(c1);
        if (0 != c2) { ws.push_back(c2); }
    }

    return ws;
}

ustring::operator std::u32string() const {
    std::u32string ws;

    for (const char * p = str_.c_str(); '\0' != *p; p = utf8_next(p)) {
        ws.push_back(char32_from_pointer(p));
    }

    return ws;
}

std::istream & operator>>(std::istream & is, ustring & str) {
    std::string s;
    is >> s;
    str = s;
    return is;
}

std::ostream & operator<<(std::ostream & os, const ustring & str) {
    os << str.c_str();
    return os;
}

} // namespace tau

//END

