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

#include <tau/encoding.hh>
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <tau/string.hh>
#include <cstring>
#include <memory>
#include <map>

namespace tau {

const ustring & str_newlines() {
    static const ustring s("\u000A\u000D\u2028\u2029");
    return s;
}

const ustring & str_blanks() {
    static const ustring s("\u0009\u0020\u00A0\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200a\u200b\u202F\u205f\u2060\ufeff");
    return s;
}

const ustring & str_delimiters() {
    static const ustring s("\u0009\u000A\u000D\u0020\u00A0"
                           ".,:;!?-+=|\\/<>(){}[]~`\"@#$%^&*"
                           "\u2007\u2028\u2029\u202F\u2060");
    return s;
}

bool char32_is_delimiter(char32_t uc) {
    return ustring::npos != str_delimiters().find(uc);
}

bool char32_is_newline(char32_t uc) {
    return ustring::npos != str_newlines().find(uc);
}

bool char32_isblank(char32_t uc) {
    return ustring::npos != str_blanks().find(uc);
}

bool char32_is_zerowidth(char32_t uc) {
    static const ustring s("\u00AD\u034F\u200B\u200C\u200D\u200E\u200F"
                           "\u2028\u202A\u202B\u202C\u202D\u202E\u2060"
                           "\u2061\u2062\u2063\uFEFF");
    return ustring::npos != s.find(uc);
}

bool str_has_prefix(const ustring & str, const ustring & prefix, bool similar) {
    if (prefix.size() > str.size()) { return false; }
    return similar ? str_similar(prefix, str.substr(0, prefix.size())) : (prefix == str.substr(0, prefix.size()));
}

bool str_has_suffix(const ustring & str, const ustring & suffix, bool similar) {
    if (suffix.size() > str.size()) { return false; }
    return similar ? str_similar(suffix, str.substr(str.size()-suffix.size())) : (suffix == str.substr(str.size()-suffix.size()));
}

char32_t char32_toupper(char32_t uc) {
    if (0x0061 <= uc && uc <= 0x007a) return uc-0x0020;
    if (0x00e0 <= uc && uc <= 0x00f6) return uc-0x0020;
    if (0x00f8 <= uc && uc <= 0x00fe) return uc-0x0020;
    if (0x0101 <= uc && uc <= 0x0147 && 0 != (uc & 1)) return --uc;
    if (0x014b <= uc && uc <= 0x0177 && 0 != (uc & 1)) return --uc;
    if (0x017a <= uc && uc <= 0x017e && 0 == (uc & 1)) return --uc;
    if (0x0450 <= uc && uc <= 0x045f) return uc-0x0050;
    if (0x0430 <= uc && uc <= 0x044f) return uc-0x0020;
    if (0x0461 <= uc && uc <= 0x0481 && 0 != (uc & 1)) return --uc;
    if (0x048b <= uc && uc <= 0x04bf && 0 != (uc & 1)) return --uc;
    if (0x04c2 <= uc && uc <= 0x04ce && 0 == (uc & 1)) return --uc;
    return uc;
}

char32_t char32_tolower(char32_t uc) {
    if (0x0041 <= uc && uc <= 0x005a) return uc+0x0020;
    if (0x00c0 <= uc && uc <= 0x00d6) return uc+0x0020;
    if (0x00d8 <= uc && uc <= 0x00de) return uc+0x0020;
    if (0x0100 <= uc && uc <= 0x0146 && 0 == (uc & 1)) return ++uc;
    if (0x014a <= uc && uc <= 0x0176 && 0 == (uc & 1)) return ++uc;
    if (0x0179 <= uc && uc <= 0x017d && 0 != (uc & 1)) return ++uc;
    if (0x0400 <= uc && uc <= 0x040f) return uc+0x0050;
    if (0x0410 <= uc && uc <= 0x042f) return uc+0x0020;
    if (0x0460 <= uc && uc <= 0x0480 && 0 == (uc & 1)) return ++uc;
    if (0x048a <= uc && uc <= 0x04be && 0 == (uc & 1)) return ++uc;
    if (0x04c1 <= uc && uc <= 0x04cd && 0 != (uc & 1)) return ++uc;
    return uc;
}

/// Check is key code is a control code, not alpha-numeric.
bool char32_is_control(char32_t uc) {
    if (uc <  0x00000020) return true;
    if (uc == 0x0000007f) return true;
    if (uc <  0x00000080) return false;
    if (uc <  0x000000a0) return true;

    static const ustring ctrls = "\u034f\u200b\u200c\u200d\u200e\u200f\u2028\u2029\u202a\202b\u202c\u202d\u202e\u2060";
    if (ustring::npos != ctrls.find(uc)) return true;

    if (uc <  0x0000fe01) return false;
    if (uc <  0x0000fe10) return true;
    if (uc == 0x0000feff) return true;
    if (uc == 0x0000fffd) return true;
    if (uc <  0x000e0100) return false;
    if (uc <  0x000e01f0) return true;
    if (uc <  0x00110000) return false;
    return true;
}

bool char32_is_unicode(char32_t uc) {
    if (char32_is_control(uc)) return false;
    if (uc < 0x00110000) return true;
    return false;
}

bool char32_is_modifier(char32_t uc) {
    if (uc < 0x02b0) return false;
    if (uc < 0x0370) return true;
    if (uc < 0x0483) return false;
    if (uc < 0x048a) return true;
    if (uc < 0x1dc0) return false;
    if (uc < 0x1de7) return true;
    if (uc < 0x1dfc) return false;
    if (uc < 0x1e00) return true;
    if (uc < 0x20d0) return false;
    if (uc < 0x20f1) return true;

    return false;
}

bool char16_is_surrogate(char16_t uc) {
    return (uc >= u'\xdc00' && uc <= u'\xdfff') || (uc >= u'\xd800' && uc <= u'\xdbff');
}

void char32_to_surrogate(char32_t c32, char16_t & c1, char16_t & c2) {
    if (c32 <= U'\x0000ffff') {
        c1 = c32;
        c2 = 0;
    }

    else if (c32 >= U'\x10000' && c32 <= U'\x10ffff') {
        c32 -= U'\x10000';
        c1 = u'\xd800'+(c32 >> 10);
        c2 = u'\xdc00'+(c32 & 0x3ff);
    }

    else {
        c1 = c2 = u'\0';
    }
}

char32_t char32_from_surrogate(char16_t c1, char16_t c2) {
    char32_t cc = U'\x000000';

    if (char16_is_surrogate(c1) && char16_is_surrogate(c2)) {
        if (c1 >= u'\xdc00') { std::swap(c1, c2); }
        cc = c1-u'\xd800';
        cc <<= 10;
        cc += c2-U'\xdc00'+U'\x10000';
    }

    return cc;
}

// 1  00000000 00000000 00000000 0xxxxxxx
// 2  00000000 00000000 00000xxx xxxxxxxx
// 3  00000000 00000000 xxxxxxxx xxxxxxxx
// 4  00000000 000xxxxx xxxxxxxx xxxxxxxx
// 5  000000xx xxxxxxxx xxxxxxxx xxxxxxxx
std::size_t char32_len(char32_t wc) {
    static const char32_t m[5] = { U'\xffffff80', U'\xfffff800', U'\xffff0000', U'\xffe00000', U'\xfc000000' };

    for (std::size_t n = 0; n < 6; ++n) {
        if (0 == (wc & m[n])) {
            return n+1;
        }
    }

    return 6;
}

std::string char32_to_string(char32_t uc) {
    unsigned len = char32_len(uc);
    std::string s(len, ' ');
    for (unsigned i = len-1; i; --i) { s[i] = (uc & U'\x3f') | U'\x80'; uc >>= 6; }
    s[0] = uc|utf8_leader(len);
    return s;
}

std::size_t char32_to_utf8(char32_t wc, char * buffer, std::size_t buffer_size) {
    const std::size_t len = char32_len(wc);

    for (std::size_t i = len-1; 0 != i; --i) {
        if (i < buffer_size) { buffer[i] = (wc & 0x3f) | 0x80; }
        wc >>= 6;
    }

    buffer[0] = wc|utf8_leader(len);
    if (len+1 < buffer_size) { buffer[len] = '\0'; }
    return std::min(len, buffer_size);
}

// (1) 0aaa aaaa
// (2) 110x xxxx 10xx xxxx
// (3) 1110 xxxx 10xx xxxx 10xx xxxx
// (4) 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
// (5) 1111 10xx 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx
// (6) 1111 110x 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx
std::size_t utf8_len(char leader) {
    if (leader >= 0) return 1;
    std::size_t n;
    for (n = 0; leader < 0 && n != 6; leader <<= 1) { ++n; }
    return n;
}

const char * utf8_next(const char * p) {
    return p+utf8_len(*p);
}

char32_t char32_from_pointer(const char * u) {
    if (*u >= 0) { return *u; }
    unsigned n = utf8_len(*u);
    if (!n) { return U'\0'; }
    char32_t w = static_cast<uint8_t>(*u++ & ('\x7f' >> n));
    while (--n) { w <<= 6; w |= static_cast<uint8_t>('\x3f' & *u++); }
    return w;
}

ustring str_toupper(const ustring & s) {
    ustring result;
    for (auto uc: s) { result += char32_toupper(uc); }
    return result;
}

ustring str_tolower(const ustring & s) {
    ustring result;
    for (auto uc: s) { result += char32_tolower(uc); }
    return result;
}

ustring str_trim(const ustring & str) {
    ustring tstr = str_trimright(str_trimleft(str)), res, blanks(str_blanks()+str_newlines());
    bool skip = true;

    for (auto wc: tstr) {
        if (ustring::npos == blanks.find(wc)) {
            skip = false;
            res += wc;
        }

        else {
            if (!skip) { res += U' '; }
            skip = true;
        }
    }

    return res;
}

ustring str_trimright(const ustring & str) {
    ustring::size_type pos = str.find_last_not_of(str_blanks()+str_newlines());
    if (ustring::npos == pos) { return str; }
    return str.substr(0, pos+1);
}

ustring str_trimleft(const ustring & str) {
    ustring::size_type pos = str.find_first_not_of(str_blanks()+str_newlines());
    return ustring::npos == pos ? str : str.substr(pos);
}

std::vector<ustring> str_explode(const ustring & str, const ustring & delimiters) {
    std::vector<ustring> v;
    ustring::size_type pos = 0, len = str.size();

    while (pos < len) {
        while (ustring::npos != delimiters.find(str[pos])) { ++pos; }
        ustring::size_type del = str.find_first_of(delimiters, pos);

        if (ustring::npos != del) {
            if (pos < del) {
                v.push_back(str.substr(pos, del-pos));
                pos = del;
            }
        }

        else {
            if (pos < len) {
                v.push_back(str.substr(pos));
                pos = len;
            }
        }
    }

    return v;
}

std::vector<ustring> str_explode(const ustring & str, char32_t wc) {
    return str_explode(str, ustring(1, wc));
}

std::vector<ustring> str_explode(const ustring & str) {
    return str_explode(str, str_newlines()+str_blanks());
}

ustring str_implode(const std::vector<ustring> & pieces, char32_t glue) {
    return str_implode(pieces, ustring(1, glue));
}

ustring str_implode(const std::vector<ustring> & pieces, const ustring & glue) {
    ustring result;

    for (std::size_t n = 0; n < pieces.size(); ) {
        result += pieces[n];
        if (++n != pieces.size()) { result += glue; }
    }

    return result;
}

bool str_similar(const ustring & s1, const ustring & s2) {
    std::string ss1 = str_trim(str_toupper(s1)), ss2 = str_trim(str_toupper(s2));
    for (std::size_t i = 0; i < ss1.size(); ++i) { if (ss1[i] == '_') ss1[i] = '-'; }
    for (std::size_t i = 0; i < ss2.size(); ++i) { if (ss2[i] == '_') ss2[i] = '-'; }
    return ss1 == ss2;
}

bool str_similar(const ustring & test, const std::vector<ustring> & vars) {
    const ustring utest = str_trim(str_toupper(test));

    for (const ustring & s: vars) {
        if (utest == str_trim(str_toupper(s))) {
            return true;
        }
    }

    return false;
}

bool str_similar(const ustring & test, const ustring & vars, char32_t delimiter) {
    if (0 == delimiter) {
        return str_similar(test, vars);
    }

    else {
        return str_similar(test, str_explode(vars, delimiter));
    }
}

ustring str_bytes(uintmax_t nbytes, bool si) {
    static const char * prefixes_si[5] = { "bytes", "kB", "MB", "GB", "TB" };
    static const char * prefixes[5] = { "bytes", "KiB", "MiB", "GiB", "TiB" };
    char buffer[64];
    const char ** p = si ? prefixes_si : prefixes;
    uintmax_t m = si ? 1000 : 1024, m1 = m, m2 = 1;

    if (nbytes < m1) {
        unsigned ubytes = nbytes;
        std::snprintf(buffer, sizeof(buffer), "%u %s", ubytes, p[0]);
        return buffer;
    }

    m1 *= m; m2 *= m;

    if (nbytes < m1) {
        double d = double(nbytes)/m2;
        std::snprintf(buffer, sizeof(buffer), "%.1f %s", d, p[1]);
        return buffer;
    }

    m1 *= m; m2 *= m;

    if (nbytes < m1) {
        double d = double(nbytes)/m2;
        std::snprintf(buffer, sizeof(buffer), "%.1f %s", d, p[2]);
        return buffer;
    }

    m1 *= m; m2 *= m;

    if (nbytes < m1) {
        double d = double(nbytes)/m2;
        std::snprintf(buffer, sizeof(buffer), "%.1f %s", d, p[3]);
        return buffer;
    }

    m2 *= m;

    double d = double(nbytes)/m2;
    std::snprintf(buffer, sizeof(buffer), "%.1f %s", d, p[4]);
    return buffer;
}

} // namespace tau

//END
