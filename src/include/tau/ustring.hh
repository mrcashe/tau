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

/// @file ustring.hh The UTF-8 string (ustring) class.

#ifndef TAU_USTRING_HH
#define TAU_USTRING_HH

#include <cctype>
#include <cstdint>
#include <string>

namespace tau {

char32_t char32_from_iterator(std::string::const_iterator pos);
std::size_t utf8_len(char leader);

template <class T>
class ustring_iterator {
public:

    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = char32_t;
    using difference_type = std::string::difference_type;
    using reference = value_type;
    using pointer = void;

    ustring_iterator():
        pos_()
    {
    }

    explicit ustring_iterator(T pos):
        pos_ (pos)
    {
    }

    T base() const {
        return pos_;
    }

    ustring_iterator(const ustring_iterator<std::string::iterator> & other):
        pos_(other.base())
    {
    }

    value_type operator*() const {
        return char32_from_iterator(pos_);
    }

    ustring_iterator & operator++() {
        pos_ += utf8_len(*pos_);
        return *this;
    }

    const ustring_iterator<T> operator++(int) {
        const ustring_iterator<T> temp(*this);
        operator++();
        return temp;
    }

    ustring_iterator<T> & operator--() {
        do {
            --pos_;
        }  while((static_cast<unsigned char>(*pos_) & 0xC0u) == 0x80);

        return *this;
    }

    const ustring_iterator<T> operator--(int) {
        const ustring_iterator<T> temp (*this);
        operator--();
        return temp;
    }

private:

    T pos_;
};

/// An UTF-8 string.
/// @ingroup string_group
class ustring {
public:

    using size_type = std::string::size_type;
    using difference_type = std::string::difference_type;
    using value_type = char32_t;
    using reference = char32_t &;
    using const_reference = const char32_t &;
    using iterator = ustring_iterator<std::string::iterator>;
    using const_iterator = ustring_iterator<std::string::const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    static const size_type npos = std::string::npos;

    /// Default constructor, which creates an empty string.
    ustring();

    /// Construct a ustring as a copy of another ustring.
    /// @param other A source string.
    ustring(const ustring & other);

    /// Move constructor.
    /// @param other A source string.
    ustring(ustring && other);

    /// Construct a ustring as a copy of another std::string.
    /// @param src A source <tt>std::string</tt> containing text encoded as UTF-8.
    ustring(const std::string & src);

    /// Move constructor from std::string.
    /// @param src A source <tt>std::string</tt> containing text encoded as UTF-8.
    ustring(std::string && src);

    /// Construct a ustring as a copy of another std::u16string.
    /// @param src A source @c std::u16string.
    ustring(const std::u16string & src);

    /// Construct a ustring as a copy of another std::u32string.
    /// @param src A source @c std::u32string.
    ustring(const std::u32string & src);

    /// Construct a ustring as a copy of a substring.
    /// @param src %Source ustring.
    /// @param i Index of first character to copy from.
    /// @param n Number of UTF-8 characters to copy (defaults to copying the remainder).
    ustring(const ustring & src, size_type i, size_type n=npos);

    /// Construct a ustring as a partial copy of a C string.
    /// @param src %Source C string encoded as UTF-8.
    /// @param n Number of UTF-8 characters to copy.
    ustring(const char * src, size_type n);

    /// Construct a ustring as a copy of a C string.
    /// @param src %Source C string encoded as UTF-8.
    ustring(const char * src);

    /// Construct a ustring as multiple characters.
    /// @param n Number of characters.
    /// @param uc UCS-4 code point to use.
    ustring(size_type n, char32_t uc);

    /// Construct a ustring as multiple characters.
    /// @param n Number of characters.
    /// @param c ASCII character to use.
    ustring(size_type n, char c);

    /// Construct a ustring as a copy of a range.
    /// @param pbegin Start of range.
    /// @param pend End of range.
    template <class In> ustring(In pbegin, In pend);

    /// Swap contents with another string.
    /// @param other String to swap with.
    void swap(ustring & other);

    /// @name Assign new contents.
    /// @{

    /// Assign the value of another string to this string.
    /// @param other A source string.
    ustring & operator=(const ustring & other);

    /// Move the value from another string to this string.
    /// @param other A source string.
    ustring & operator=(ustring && other);

    ustring & operator=(const std::string & src);
    ustring & operator=(std::string && src);
    ustring & operator=(const char * src);
    ustring & operator=(char32_t uc);
    ustring & operator=(char c);

    ustring & assign(const ustring & src);
    ustring & assign(const ustring & src, size_type i, size_type n);
    ustring & assign(const char * src, size_type n);
    ustring & assign(const char * src);
    ustring & assign(size_type n, char32_t uc);
    ustring & assign(size_type n, char c);
    template <class In> ustring & assign(In pbegin, In pend);

    /// @}
    /// @name Append to the string.
    /// @{

    ustring & operator+=(const ustring & src);
    ustring & operator+=(const char * src);
    ustring & operator+=(char32_t uc);
    ustring & operator+=(char c);
    void push_back(char32_t uc);
    void push_back(char c);

    ustring & append(const ustring & src);
    ustring & append(const ustring & src, size_type i, size_type n);
    ustring & append(const char * src, size_type n);
    ustring & append(const char * src);
    ustring & append(size_type n, char32_t uc);
    ustring & append(size_type n, char c);
    template <class In> ustring & append(In pbegin, In pend);

    /// @}
    /// @name Insert into the string.
    /// @{

    ustring & insert(size_type i, const ustring & src);
    ustring & insert(size_type i, const ustring & src, size_type i2, size_type n);
    ustring & insert(size_type i, const char * src, size_type n);
    ustring & insert(size_type i, const char * src);
    ustring & insert(size_type i, size_type n, char32_t uc);
    ustring & insert(size_type i, size_type n, char c);

    iterator insert(iterator p, char32_t uc);
    iterator insert(iterator p, char c);
    void     insert(iterator p, size_type n, char32_t uc);
    void     insert(iterator p, size_type n, char c);
    template <class In> void insert(iterator p, In pbegin, In pend);

    /// @}
    /// @name Replace sub-strings.
    /// @{

    ustring & replace(size_type i, size_type n, const ustring & src);
    ustring & replace(size_type i, size_type n, const ustring & src, size_type i2, size_type n2);
    ustring & replace(size_type i, size_type n, const char * src, size_type n2);
    ustring & replace(size_type i, size_type n, const char * src);
    ustring & replace(size_type i, size_type n, size_type n2, char32_t uc);
    ustring & replace(size_type i, size_type n, size_type n2, char c);

    ustring & replace(iterator pbegin, iterator pend, const ustring & src);
    ustring & replace(iterator pbegin, iterator pend, const char * src, size_type n);
    ustring & replace(iterator pbegin, iterator pend, const char * src);
    ustring & replace(iterator pbegin, iterator pend, size_type n, char32_t uc);
    ustring & replace(iterator pbegin, iterator pend, size_type n, char c);
    template <class In> ustring & replace(iterator pbegin, iterator pend, In pbegin2, In pend2);

    /// @}
    /// @name Erase sub-strings.
    /// @{

    void clear();
    ustring & erase(size_type i, size_type n=npos);
    ustring & erase();
    iterator erase(iterator p);
    iterator erase(iterator pbegin, iterator pend);

    /// @}
    /// @name Compare and collate.
    /// @{

    int compare(const ustring & rhs) const;
    int compare(const char * rhs) const;
    int compare(size_type i, size_type n, const ustring & rhs) const;
    int compare(size_type i, size_type n, const ustring & rhs, size_type i2, size_type n2) const;
    int compare(size_type i, size_type n, const char * rhs, size_type n2) const;
    int compare(size_type i, size_type n, const char * rhs) const;

    /// @}
    /// @name Extract characters and sub-strings.
    /// @{

    /// No reference return; use replace() to write characters.
    value_type operator[](size_type i) const;

    /// No reference return; use replace() to write characters.
    /// @throw std::out_of_range
    value_type at(size_type i) const;

    ustring substr(size_type i=0, size_type n=npos) const;

    /// @}
    /// @name Access a sequence of characters.
    /// @{

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    /// @}
    /// @name Find sub-strings.
    /// @{

    size_type find(const ustring & str, size_type i=0) const;
    size_type find(const char * str, size_type i, size_type n) const;
    size_type find(const char * str, size_type i=0) const;
    size_type find(char32_t uc, size_type i=0) const;

    size_type rfind(const ustring & str, size_type i=npos) const;
    size_type rfind(const char * str, size_type i, size_type n) const;
    size_type rfind(const char * str, size_type i=npos) const;
    size_type rfind(char32_t uc, size_type i=npos) const;

    /// @}
    /// @name Match against a set of characters.
    /// @{

    size_type find_first_of(const ustring & match, size_type i=0) const;
    size_type find_first_of(const char * match, size_type i, size_type n) const;
    size_type find_first_of(const char * match, size_type i=0) const;
    size_type find_first_of(char32_t uc, size_type i=0) const;
    size_type find_first_of(char c, size_type i=0) const;

    size_type find_last_of(const ustring & match, size_type i=npos) const;
    size_type find_last_of(const char * match, size_type i, size_type n) const;
    size_type find_last_of(const char * match, size_type i=npos) const;
    size_type find_last_of(char32_t uc, size_type i=npos) const;
    size_type find_last_of(char c, size_type i=npos) const;

    size_type find_first_not_of(const ustring & match, size_type i=0) const;
    size_type find_first_not_of(const char * match, size_type i, size_type n) const;
    size_type find_first_not_of(const char * match, size_type i=0) const;
    size_type find_first_not_of(char32_t uc, size_type i=0) const;
    size_type find_first_not_of(char c, size_type i=0) const;

    size_type find_last_not_of(const ustring & match, size_type i=npos) const;
    size_type find_last_not_of(const char * match, size_type i, size_type n) const;
    size_type find_last_not_of(const char * match, size_type i=npos) const;
    size_type find_last_not_of(char32_t uc, size_type i=npos) const;
    size_type find_last_not_of(char c, size_type i=npos) const;

    /// @}
    /// @name Retrieve the string's size.
    /// @{

    /// Returns true if the string is empty. Equivalent to *this == "".
    /// @result Whether the string is empty.
    bool empty() const;

    /// Returns the number of characters in the string, not including any null-termination.
    /// @result The number of UTF-8 characters.
    /// @see bytes(), empty()
    size_type size() const;

    /// This is the same as size().
    size_type length() const;

    /// Returns the number of bytes in the string, not including any null-termination.
    /// @result The number of bytes.
    /// @see size(), empty()
    size_type bytes() const;

    /// @}
    /// @name Change the string's size.
    /// @{

    void resize(size_type n, char32_t uc);
    void resize(size_type n, char c='\0');

    /// @}
    /// @name Control the allocated memory.
    /// @{

    size_type capacity() const;
    size_type max_size() const;
    void reserve(size_type n=0);

    /// @}
    /// @name Get a per-byte representation of the string.
    /// @{

    /// Get underlaying std::string.
    operator std::string() const;

    /// Get underlaying std::string.
    const std::string & raw() const;

    /// Get underlaying C string.
    const char * data() const;

    /// Get underlaying C string.
    const char * c_str() const;

    /// @return Number of copied @em bytes, not characters.
    size_type copy(char * dest, size_type n, size_type i=0) const;

    /// @}
    /// @name Conversions.
    /// @{

    /// Convert to UTF-16.
    operator std::u16string() const;

    /// Convert to UTF-32.
    operator std::u32string() const;

    /// @}

private:

    std::string         str_;
    mutable size_type   size_ = npos;
};

/// Stream input operator.
/// @ingroup string_group
/// @relates ustring
std::istream & operator>>(std::istream & is, ustring & str);

/// Stream output operator.
/// @ingroup string_group
/// @relates ustring
std::ostream & operator<<(std::ostream & os, const ustring & str);

/// @ingroup string_group
/// @relates ustring
bool operator==(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator!=(const ustring::const_iterator& lhs, const ustring::const_iterator& rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<=(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>=(const ustring::const_iterator & lhs, const ustring::const_iterator & rhs);

/// @ingroup string_group
/// @relates ustring
void swap(ustring & lhs, ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator==(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator==(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
bool operator==(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator!=(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator!=(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
bool operator!=(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<=(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<=(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
bool operator<=(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>=(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>=(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
bool operator>=(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(const ustring & lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(const ustring & lhs, const char * rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(const char * lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(const ustring & lhs, char32_t rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(char32_t lhs, const ustring & rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(const ustring & lhs, char rhs);

/// @ingroup string_group
/// @relates ustring
ustring operator+(char lhs, const ustring & rhs);

} // namespace tau

#endif // TAU_USTRING_HH

