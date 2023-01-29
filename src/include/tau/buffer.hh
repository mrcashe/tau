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

/// Shows current position and perform search operations within Buffer.
///
/// @ingroup text_group
class Buffer_citer {
public:

    /// @name Constructors, destructor, operators
    /// @{

    /// Default constructor.
    Buffer_citer();

    /// Copy constructor.
    Buffer_citer(const Buffer_citer & other);

    /// Copy operator.
    Buffer_citer & operator=(const Buffer_citer & other);

    /// Constructor with coordinates.
    /// Constructs Buffer_citer with same buffer as other and different location.
    Buffer_citer(const Buffer_citer & other, std::size_t row, std::size_t col);

    /// Destructor.
   ~Buffer_citer();

    /// @}
    /// Get current row (line) number.
    std::size_t row() const;

    /// Get current position within the row (line).
    std::size_t col() const;

    /// Assign value of other Buffer_citer buffer and position.
    void set(const Buffer_citer & other, std::size_t row, std::size_t col);

    /// Get pointing unicode character.
    char32_t operator*() const;

    /// Get an UTF-8 text between two iterators.
    ustring text(Buffer_citer other) const;

    /// Get an UTF-8 text containing specified character count.
    ustring text(std::size_t nchars) const;

    /// Get an UTF-32 text between two iterators.
    std::u32string text32(Buffer_citer other) const;

    /// Get an UTF-32 text containing specified character count.
    std::u32string text32(std::size_t nchars) const;

    /// Advance pointer by one character.
    Buffer_citer & operator++();

    /// Advance pointer by one character.
    Buffer_citer operator++(int);

    /// Reverse pointer by one character.
    Buffer_citer & operator--();

    /// Reverse pointer by one character.
    Buffer_citer operator--(int);

    /// Move forward.
    Buffer_citer & operator+=(std::size_t npos);

    /// Move backward.
    Buffer_citer & operator-=(std::size_t npos);

    /// Compare two positions.
    bool operator==(const Buffer_citer & other) const;

    /// Compare two positions.
    bool operator!=(const Buffer_citer & other) const;

    /// Compare two positions.
    bool operator<(const Buffer_citer & other) const;

    /// Determines whether Buffer_citer points to some place within the buffer or not.
    operator bool() const;

    /// Make Buffer_citer pointing to nowhere.
    void reset();

    /// Find character forward.
    /// If specified character found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find(char32_t wc);

    /// Find character forward.
    /// If specified character found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find(char32_t wc, Buffer_citer other);

    /// Find text forward.
    /// If specified text found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find(const ustring & text);

    /// Find text forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If specified text found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find(const ustring & text, Buffer_citer other);

    /// Find text forward.
    /// If specified text found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find(const std::u32string & text);

    /// Find text forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If specified text found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find(const std::u32string & text, Buffer_citer other);

    /// Find characters forward.
    /// If any of specified characters found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_of(const ustring & chars);

    /// Find characters forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If any of specified characters found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_of(const ustring & chars, Buffer_citer other);

    /// Find characters forward.
    /// If any of specified characters found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_of(const std::u32string & chars);

    /// Find characters forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If any of specified characters found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_of(const std::u32string & chars, Buffer_citer other);

    /// Find characters forward.
    /// If any of specified characters not found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_not_of(const ustring & chars);

    /// Find characters forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If any of specified characters not found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_not_of(const ustring & chars, Buffer_citer other);

    /// Find characters forward.
    /// If any of specified characters not found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_not_of(const std::u32string & chars);

    /// Find characters forward.
    /// The search is performed from the current position and until the place specified by the other.
    /// If any of specified characters not found within buffer, the iterator is pointing to it.
    /// Otherwise, the position is EOF.
    /// @return @b true if found.
    bool find_first_not_of(const std::u32string & chars, Buffer_citer other);

    /// Compare text at current position.
    /// Compare given text against text at current position.
    /// @param text text to be compared.
    /// @param advance advance position by the length of given string in case of equality.
    /// @return @b true if both strings are equal.
    bool equals(const ustring & text, bool advance=false);

    /// Compare text at current position.
    /// Compare given text against text at current position.
    /// @param text text to be compared.
    /// @param advance advance position by the length of given string in case of equality.
    /// @return @b true if both strings are equal.
    bool equals(const std::u32string & text, bool advance=false);

    /// Skip any blank characters within current line.
    void skip_blanks();

    /// Skip any blank and new line characters.
    void skip_whitespace();

    /// Move to specified position.
    void move_to(std::size_t row, std::size_t col);

    /// Move to specified column within current line.
    /// Tries to move to specified position.
    /// If that position does not exist, the position will be set to the rightmost character.
    void move_to_col(std::size_t col);

    /// Move to start of the current line.
    void move_to_sol();

    /// Move to first end-of-line character or end-of-file.
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
    Buffer_citer(Buffer_citer_impl * p);
    Buffer_citer_impl * impl;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/// The text buffer.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Buffer {
public:

    /// Constructs an empty buffer.
    Buffer();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Buffer(const Buffer & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Buffer & operator=(const Buffer & other) = default;

    /// Constructor with UTF-8 text.
    Buffer(const ustring & str);

    /// Constructor with UTF-32 text.
    Buffer(const std::u32string & str);

    /// @name Input/Output
    /// @{

    /// Constructor with stream.
    Buffer(std::istream & is);

    /// Load from file.
    /// @param path an UTF-8 encoded path to the file.
    static Buffer load_from_file(const ustring & path);

    /// Save to stream.
    void save(std::ostream & os);

    /// Save to file.
    void save_to_file(const ustring & path);

    /// Special form of save_to_file() method.
    ///
    /// If buffer was created using load_from_file() method, this call will
    /// save it back. If buffer was not loaded from file that way, the user_error
    /// will be trown.
    ///
    /// @throw user_error if buffer wasn't created using load_from_file.
    void save();

    /// @}
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
    Buffer_citer insert(Buffer_citer pos, char32_t uc, std::size_t count=1);

    /// Insert text from an UTF-8 string.
    /// @return position at the end of inserted text.
    Buffer_citer insert(Buffer_citer pos, const ustring & s);

    /// Insert text from an UTF-32 string.
    /// @return position at the end of inserted text.
    Buffer_citer insert(Buffer_citer pos, const std::u32string & s);

    /// Insert text from STL stream.
    /// @return position at the end of inserted text.
    Buffer_citer insert(Buffer_citer pos, std::istream & is);

    /// Replace text.
    /// @return position at the end of replaced text.
    Buffer_citer replace(Buffer_citer i, const ustring & s);

    /// Replace text.
    /// @return position at the end of replaced text.
    Buffer_citer replace(Buffer_citer i, const std::u32string & s);

    /// Erase range.
    /// @return iterator to the actual end position.
    Buffer_citer erase(Buffer_citer begin, Buffer_citer end);

    /// Clears buffer.
    void clear();

    /// Get text as UTF-8.
    ustring text() const;

    /// Get text between two iterators as UTF-8.
    ustring text(Buffer_citer begin, Buffer_citer end) const;

    /// Get text as UTF-32.
    std::u32string text32() const;

    /// Get text between two iterators as UTF-32.
    std::u32string text32(Buffer_citer begin, Buffer_citer end) const;

    /// Get size in unicode characters.
    std::size_t size() const;

    /// Get size in unicode characters in between two iterators.
    std::size_t length(Buffer_citer begin, Buffer_citer end) const;

    /// Get row count.
    std::size_t rows() const;

    /// Test if empty.
    bool empty() const;

    /// Test if changed.
    bool changed() const;

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

    /// Returns Buffer_citer pointing to specified position.
    Buffer_citer citer(std::size_t row, std::size_t col=0) const;

    /// Returns Buffer_citer pointing to begin.
    Buffer_citer cbegin() const;

    /// Returns Buffer_citer pointing to end.
    Buffer_citer cend() const;

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
    /// void on_buffer_erase(Buffer_citer begin, Buffer_citer end, const std::u32string & erased_text);
    /// ~~~~~~~~~~~~~~~
    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & signal_erase();

    /// Signal emitted when text inserted.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_insert(Buffer_citer begin, Buffer_citer end);
    /// ~~~~~~~~~~~~~~~
    signal<void(Buffer_citer, Buffer_citer)> & signal_insert();

    /// Signal emitted when text replaced.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_replace(Buffer_citer begin, Buffer_citer end, const std::u32string & replaced_text);
    /// ~~~~~~~~~~~~~~~
    signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & signal_replace();

    /// Signal emitted when buffer changes somehow.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_changed();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_changed();

    /// Signal emitted when buffer flushed to disk or elsewhere using save* methods.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_buffer_flush();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_flush();

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
