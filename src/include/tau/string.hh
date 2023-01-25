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

/// @file string.hh Unicode manipulation functions.

#ifndef TAU_STRING_HH
#define TAU_STRING_HH

#include <tau/ustring.hh>
#include <sstream>
#include <vector>

namespace tau {

/// Convert to upper case.
/// @ingroup string_group
char32_t char32_toupper(char32_t uc);

/// Convert to lower case.
/// @ingroup string_group
char32_t char32_tolower(char32_t uc);

/// Test if newline character.
/// @ingroup string_group
bool char32_is_newline(char32_t uc);

/// Test if delimiter.
/// @ingroup string_group
bool char32_is_delimiter(char32_t uc);

/// Test if blank.
/// @ingroup string_group
bool char32_isblank(char32_t uc);

/// Test if both modifier letter or diacritical mark.
/// @ingroup string_group
bool char32_is_modifier(char32_t wc);

/// Test if zero width character.
/// @ingroup string_group
bool char32_is_zerowidth(char32_t uc);

/// Test if code is a control code, not alpha-numeric.
/// @ingroup string_group
bool char32_is_control(char32_t kc);

/// Test if valid unicode.
/// @ingroup string_group
bool char32_is_unicode(char32_t uc);

/// Test if 16-bit character is from surrogate pair.
/// @ingroup string_group
bool char16_is_surrogate(char16_t uc);

/// Convert surrogate pair into 32-bit unicode character.
/// @ingroup string_group
char32_t char32_from_surrogate(char16_t c1, char16_t c2);

/// Split 32-bit character into pair of 16-bit characters.
/// If c32 belongs to surrogate range, both c1 and c2
/// values are non-zero. If it is not, the c2 is zero.
/// @ingroup string_group
void char32_to_surrogate(char32_t c32, char16_t & c1, char16_t & c2);

/// Calculate an UTF-8 sequence length for specified UTF-32 character.
/// @ingroup string_group
std::size_t char32_len(char32_t wc);

/// Convert an UTF-32 character into UTF-8 sequence.
/// @ingroup string_group
std::string char32_to_string(char32_t wc);

/// Convert an UTF-32 character into UTF-8 sequence.
/// @param wc the character to be converted.
/// @param buffer the buffer for result storing.
/// @param buffer_size the buffer size in bytes.
/// @return number of characters written into buffer.
/// @ingroup string_group
std::size_t char32_to_utf8(char32_t wc, char * buffer, std::size_t buffer_size);

/// Extract UCS-4 character from pointer.
/// @ingroup string_group
char32_t char32_from_pointer(const char * u);

/// Calculate UTF-8 sequence length in bytes using UTF-8 leading character.
/// @ingroup string_group
std::size_t utf8_len(char leader);

/// Get pointer to the next UTF-8 character.
/// @ingroup string_group
const char * utf8_next(const char * u);

/// Get leading character in UTF-8 sequence.
/// @param nbytes number of bytes in sequence, 1...6.
/// @return leading character.
/// @ingroup string_group
char utf8_leader(std::size_t nbytes);

template<typename  T>
inline void __str_format_helper(std::ostream & os, const T & arg) {
    os << arg;
}

template<typename T, typename... Args>
inline void __str_format_helper(std::ostream & os, const T & arg, const Args & ... args) {
    os << arg;
    __str_format_helper(os, args...);
}

/// Formatting function that accepts ios_base format flags such as std::hex etc.
/// @ingroup string_group
template<typename... Args>
inline ustring str_format(const Args & ... args) {
    std::ostringstream os;
    __str_format_helper(os, args...);
    return os.str();
}

/// Test if string starts with given prefix.
/// @param str testing string.
/// @param prefix searching prefix.
/// @param similar if true, test case insensitive.
/// @ingroup string_group
bool str_has_prefix(const ustring & str, const ustring & prefix, bool similar=false);

/// Test if string ends with given suffix.
/// @param str testing string.
/// @param suffix searching prefix.
/// @param similar if true, test case insensitive.
/// @ingroup string_group
bool str_has_suffix(const ustring & str, const ustring & suffix, bool similar=false);

/// Convert to upper case.
/// @ingroup string_group
ustring str_toupper(const ustring & str);

/// Convert to lower case.
/// @ingroup string_group
ustring str_tolower(const ustring & str);

/// Remove leading blanks and newlines.
/// @ingroup string_group
ustring str_trimleft(const ustring & str);

/// Remove trailing blanks and newlines.
/// @ingroup string_group
ustring str_trimright(const ustring & str);

/// Remove consecutive, leading and trailing blanks and newlines.
/// @ingroup string_group
ustring str_trim(const ustring & str);

/// Split a string by character.
/// @ingroup string_group
std::vector<ustring> str_explode(const ustring & str, char32_t wc);

/// Split a string by any character from given string.
/// @ingroup string_group
std::vector<ustring> str_explode(const ustring & str, const ustring & delimiters);

/// Split a string by blanks and newlines.
/// @ingroup string_group
std::vector<ustring> str_explode(const ustring & str);

/// Join array elements with a character.
/// @ingroup string_group
ustring str_implode(const std::vector<ustring> & pieces, char32_t glue);

/// Join array elements with a string.
/// @ingroup string_group
ustring str_implode(const std::vector<ustring> & pieces, const ustring & glue=ustring());

/// Test strings are similar.
/// @param s1 testing string 1.
/// @param s2 testing string 2.
/// @ingroup string_group
bool str_similar(const ustring & s1, const ustring & s2);

/// Test strings are similar.
/// @param test testing string.
/// @param vars variant(s) for compare against to.
/// @ingroup string_group
bool str_similar(const ustring & test, const std::vector<ustring> & vars);

/// Test strings are similar.
/// @param test testing string.
/// @param vars variant(s) for compare against to.
/// @param delimiter variant delimiter character for use with str_explode().
/// @ingroup string_group
bool str_similar(const ustring & test, const ustring & vars, char32_t delimiter);

/// Test strings are similar.
/// @param test testing string.
/// @param vars variant(s) for compare against to.
/// @param delimiters variant delimiter characters for use with str_explode().
/// @ingroup string_group
bool str_similar(const ustring & test, const ustring & vars, const ustring & delimiters);

/// Get newline characters.
/// @ingroup string_group
const ustring & str_newlines();

/// Get blank characters.
/// @ingroup string_group
const ustring & str_blanks();

/// Get delimiters.
/// @ingroup string_group
const ustring & str_delimiters();

/// Format file or memory size in bytes.
/// @param nbytes number of bytes to be formatted.
/// @param si if @b true, format in SI units (1kB = 1000 bytes), else 1KiB = 1024 bytes.
/// @ingroup string_group
ustring str_bytes(uintmax_t nbytes, bool si=false);

} // namespace tau

#endif // TAU_STRING_HH
