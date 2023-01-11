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

/// @file buffer.hh The Buffer class.

#ifndef TAU_BUFFER_HH
#define TAU_BUFFER_HH

#include <tau/types.hh>
#include <tau/signal.hh>
#include <tau/ustring.hh>

namespace tau {

/// Shows current position within Buffer.
/// @ingroup text_group
class Buffer_iter {
public:

    /// Default constructor.
    Buffer_iter();

    /// Copy constructor.
    Buffer_iter(const Buffer_iter & other);

    /// Copy operator.
    Buffer_iter & operator=(const Buffer_iter & other);

    /// Constructor with coordinates.
    /// Constructs Buffer_iter with same buffer as other and different location.
    Buffer_iter(const Buffer_iter & other, size_t row, size_t col);

    /// Destructor.
    ~Buffer_iter();

    /// Get current line number.
    size_t row() const;

    /// Get current position within the line.
    size_t col() const;

    /// Assign value of other Buffer_iter buffer and position.
    void set(const Buffer_iter & other, size_t row, size_t col);

    /// Get pointing unicode character.
    char32_t operator*() const;

    /// Get text between two iterators.
    ustring peek(Buffer_iter other) const;

    /// Get text containing specified character count.
    ustring peek(size_t nchars) const;

    /// Advance pointer by one character.
    Buffer_iter & operator++();

    /// Advance pointer by one character.
    Buffer_iter operator++(int);

    /// Reverse pointer by one character.
    Buffer_iter & operator--();

    /// Reverse pointer by one character.
    Buffer_iter operator--(int);

    /// Move forward.
    Buffer_iter & operator+=(size_t npos);

    /// Move backward.
    Buffer_iter & operator-=(size_t npos);

    /// Compare two positions.
    bool operator==(const Buffer_iter & other) const;

    /// Compare two positions.
    bool operator!=(const Buffer_iter & other) const;

    /// Compare two positions.
    bool operator<(const Buffer_iter & other) const;

    /// Determines whether Buffer_iter points to some place within the buffer or not.
    operator bool() const;

    /// Make Buffer_iter pointing to nowhere.
    void reset();

    /// Find character forward.
    /// If specified character found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find(char32_t wc);

    /// Find text forward.
    /// If specified text found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find(const ustring & text);

    /// Find character forward.
    /// If specified character found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find(char32_t wc, Buffer_iter other);

    /// Find text forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If specified text found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find(const ustring & text, Buffer_iter other);

    /// Find characters forward.
    /// If any of specified characters found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find_first_of(const ustring & chars);

    /// Find characters forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If any of specified characters found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find_first_of(const ustring & chars, Buffer_iter other);

    /// Find characters forward.
    /// If any of specified characters not found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find_first_not_of(const ustring & chars);

    /// Find characters forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If any of specified characters not found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return true if found.
    bool find_first_not_of(const ustring & chars, Buffer_iter other);

    /// Compare text at current position.
    /// Compare given text against text at current position.
    /// @return true if both strings are equal.
    bool text_cmp(const ustring & text);

    /// Compare text at current position.
    /// Compare given text against text at current position.
    /// Advance position by the length of given string in case of equality.
    /// @return true if both strings are equal.
    bool text_cmp_adv(const ustring & text);

    /// Skip any blank characters within current line.
    void skip_blanks();

    /// Skip any blank and new line characters.
    void skip_whitespace();

    /// Move to specified position.
    void move_to(size_t row, size_t col);

    /// Move to specified column within current line.
    /// Tries to move to specified position.
    /// If that position does not exist, the position will be set to the rightmost character.
    void move_to_col(std::size_t col);

    /// Move to start of current line position.
    void move_to_sol();

    /// Move to first end-of-line character or end-of-file position.
    void move_to_eol();

    /// Move to start of next row.
    void move_forward_line();

    /// Move to start of previous row.
    void move_backward_line();

    /// Move word left.
    void move_word_left();

    /// Move word right.
    void move_word_right();

    /// Test if current position ends line or file.
    bool eol() const;

    /// Test if current position ends file.
    /// This position is also equals to buffer end position.
    bool eof() const;

    /// Test if current position starts file.
    /// The file start position is always at row 0 and column 0.
    bool sof() const;

private:

    friend class Buffer;
    Buffer_iter(Buffer_iter_impl * p);
    Buffer_iter_impl * impl;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/// The text buffer.
/// @ingroup text_group
class Buffer {
public:

    /// Constructs an empty buffer.
    Buffer();

    /// Copy constructor.
    Buffer(const Buffer & other) = default;

    /// Copy operator.
    Buffer & operator=(const Buffer & other) = default;

    /// Constructor with text.
    Buffer(const ustring & str);

    /// Constructor with stream.
    Buffer(std::istream & is);

    /// Load from file.
    /// @param path an UTF-8 encoded path to the file.
    static Buffer load_from_file(const ustring & path);

    /// Save to stream.
    void save(std::ostream & os) const;

    /// Save to file.
    void save_to_file(const ustring & path) const;

    /// Replace buffer content from an UTF-8 string.
    void assign(const ustring & str);

    /// Replace buffer content from an UTF-32 string.
    void assign(const std::u32string & str);

    /// Replace buffer content from other buffer.
    void assign(const Buffer buf);

    /// Insert an UTF-32 characters.
    /// @param pos the position in which insertion will be done.
    /// @param uc the character to be inserted.
    /// @param count repeat count.
    /// @return position at the end of inserted text.
    Buffer_iter insert(Buffer_iter pos, char32_t uc, size_t count=1);

    /// Insert text from an UTF-8 string.
    /// @return position at the end of inserted text.
    Buffer_iter insert(Buffer_iter pos, const ustring & s);

    /// Insert text from an UTF-32 string.
    /// @return position at the end of inserted text.
    Buffer_iter insert(Buffer_iter pos, const std::u32string & s);

    /// Insert text from STL stream.
    /// @return position at the end of inserted text.
    Buffer_iter insert(Buffer_iter pos, std::istream & is);

    /// Replace text.
    /// @return position at the end of replaced text.
    Buffer_iter replace(Buffer_iter i, const ustring & s);

    /// Replace text.
    /// @return position at the end of replaced text.
    Buffer_iter replace(Buffer_iter i, const std::u32string & s);

    /// Erase range.
    /// @return iterator to the actual end position.
    Buffer_iter erase(Buffer_iter begin, Buffer_iter end);

    /// Clears buffer.
    void clear();

    /// Get text as UTF-8.
    ustring text() const;

    /// Get text between two iterators as UTF-8.
    ustring text(Buffer_iter begin, Buffer_iter end) const;

    /// Get text as UTF-32.
    std::u32string text32() const;

    /// Get text between two iterators as UTF-32.
    std::u32string text32(Buffer_iter begin, Buffer_iter end) const;

    /// Get size in unicode characters.
    size_t size() const;

    /// Get size in unicode characters in between two iterators.
    size_t length(Buffer_iter begin, Buffer_iter end) const;

    /// Get line count.
    size_t lines() const;

    /// Test if empty.
    bool empty() const;

    /// Get text encoding.
    Encoding encoding() const;

    /// Change encoding.
    void change_encoding(const Encoding & enc);

    /// Enables byte order mark (BOM) generation on output.
    /// This flag is also can be set on input in case the file
    /// or stream has BOM.
    /// Disabled by default.
    /// @sa disable_bom()
    /// @sa bom_enabled()
    /// @sa signal_bom_changed()
    void enable_bom();

    /// Disables byte order mark (BOM) generation on output.
    /// Disabled by default.
    /// @sa enable_bom()
    /// @sa bom_enabled()
    /// @sa signal_bom_changed()
    void disable_bom();

    /// Determines if BOM generation enabled.
    /// Also indicates BOM presence in input file/stream after load.
    /// @sa enable_bom()
    /// @sa disable_bom()
    /// @sa signal_bom_changed()
    bool bom_enabled() const;

    /// Returns Buffer_iter pointing to specified position.
    Buffer_iter iter(size_t row, size_t col=0);

    /// Returns Buffer_iter pointing to specified position.
    const Buffer_iter iter(size_t row, size_t col=0) const;

    /// Returns Buffer_iter pointing to begin.
    Buffer_iter begin();

    /// Returns Buffer_iter pointing to begin.
    const Buffer_iter begin() const;

    /// Returns Buffer_iter pointing to end.
    Buffer_iter end();

    /// Returns Buffer_iter pointing to end.
    const Buffer_iter end() const;

    /// Lock buffer.
    /// Disables buffer modifying.
    void lock();

    /// Test if locked.
    bool locked() const;

    /// Unlock buffer.
    /// Enables buffer modifying.
    void unlock();

    /// @name Signal accessors
    /// @{

    /// Signal emitted when text erased.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_erase(Buffer_iter begin, Buffer_iter end, const std::u32string & erased_text);
    /// ~~~~~~~~~~~~~~~
    signal<void(Buffer_iter, Buffer_iter, std::u32string)> & signal_erase();

    /// Signal emitted when text inserted.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_insert(Buffer_iter begin, Buffer_iter end);
    /// ~~~~~~~~~~~~~~~
    signal<void(Buffer_iter, Buffer_iter)> & signal_insert();

    /// Signal emitted when text replaced.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_replace(Buffer_iter begin, Buffer_iter end, const std::u32string & replaced_text);
    /// ~~~~~~~~~~~~~~~
    signal<void(Buffer_iter, Buffer_iter, std::u32string)> & signal_replace();

    /// Signal emitted when buffer locked.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_lock();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_lock();

    /// Signal emitted when buffer unlocked.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_unlock();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_unlock();

    /// Signal emitted when encoding changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_encoding_changed(const Encoding & enc);
    /// ~~~~~~~~~~~~~~~
    signal<void(const Encoding &)> & signal_encoding_changed();

    /// Signal emitted when Byte Order Mark changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_bom_changed();
    /// ~~~~~~~~~~~~~~~
    /// @sa enable_bom()
    /// @sa disable_bom()
    /// @sa bom_enabled()
    signal<void()> & signal_bom_changed();

    /// @}

private:

    Buffer_ptr impl;
};

} // namespace tau

#endif // TAU_BUFFER_HH
