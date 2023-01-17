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

/// @file text.hh The Text class.

#ifndef TAU_TEXT_HH
#define TAU_TEXT_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// %Text displaying widget.
/// @ingroup widget_group
class Text: public Widget {
public:

    /// Default constructor.
    Text();

    /// Constructor with text alignment.
    Text(Align halign, Align valign=ALIGN_CENTER);

    /// Constructor with text and text alignment.
    Text(const ustring & text, Align halign=ALIGN_CENTER, Align valign=ALIGN_CENTER);

    /// Constructor with buffer and text alignment.
    Text(Buffer buf, Align halign=ALIGN_CENTER, Align valign=ALIGN_CENTER);

    /// Assign text.
    void assign(const ustring & text);

    /// Assign buffer.
    void assign(Buffer buf);

    /// Get buffer.
    Buffer buffer();

    /// Get buffer.
    const Buffer buffer() const;

    /// Test empty.
    bool empty() const;

    /// Clear buffer.
    void clear();

    /// Get line count.
    std::size_t lines() const;

    /// Get text.
    ustring text() const;

    /// Calculate text size in pixels.
    /// Calculate given text size using current font settings. Unlike Painter::text_size(), this
    /// method returns text height not for given string, but for entire font currently set and consists
    /// of Font::ascent() and Font::descent() components.
    /// @param s the string to be measured.
    /// @return given text size in pixels.
    /// @note This method returns an empty Size if widget is not inserted into widget hierarchy.
    Size text_size(const ustring & s);

    /// Calculate text size in pixels.
    /// Calculate given text size using current font settings. Unlike Painter::text_size(), this
    /// method returns text height not for given string, but for entire font currently set and consists
    /// of Font::ascent() and Font::descent() components.
    /// @param s the string to be measured.
    /// @return given text size in pixels.
    /// @note This method returns an empty Size if widget is not inserted into widget hierarchy.
    Size text_size(const std::u32string & s);

    /// @name Selection control.
    /// @{

    /// Allow select.
    /// If enabled, the selection of the text can be done using the keyboard or
    /// by moving the mouse pointer while holding down the left button.
    /// It is not possible to select text with the keys if the cursor is disabled by a call to disable_caret().
    /// @note disabled by default in Text, enabled in Edit.
    /// @sa disallow_select()
    /// @sa select_allowed()
    void allow_select();

    /// Disallow select.
    /// If disabled, шt is impossible to select text in any way.
    /// @sa allow_select.
    /// @sa select_allowed()
    void disallow_select();

    /// Determines if a text selection allowed.
    /// @sa allow_select()
    /// @sa disallow_select()
    bool select_allowed() const;

    /// Get selected text.
    ustring selection() const;

    /// Test has selection.
    /// @return true if has some selection.
    bool has_selection() const;

    /// Select text between begin and end.
    void select(Buffer_citer begin, Buffer_citer end);

    /// Select all.
    void select_all();

    /// Cancel selection.
    void unselect();

    /// @}

    /// Set extra space between text lines.
    /// @param spc extra space in pixels.
    /// @sa spacing.
    void set_spacing(unsigned spc);

    /// Get spacing.
    /// @sa set_spacing.
    unsigned spacing() const;

    /// Set text align.
    void set_text_align(Align halign, Align valign=ALIGN_CENTER);

    /// Get horizontal align.
    Align horizontal_text_align() const;

    /// Get vertical align.
    Align vertical_text_align() const;

    /// Set word wrap mode.
    void set_wrap_mode(Wrap_mode wrap_mode);

    /// Get wrap_mode.
    Wrap_mode wrap_mode() const;

    /// Set caret position.
    void move_to(const Buffer_citer pos);

    /// Set caret position.
    void move_to(std::size_t row, std::size_t col);

    /// Get iterator for current caret position.
    Buffer_citer caret() const;

    /// Get iterator with specified position.
    Buffer_citer iter(std::size_t row, std::size_t col) const;

    /// Enable caret.
    void enable_caret();

    /// Disable caret.
    void disable_caret();

    /// Determines whether caret is enabled.
    bool caret_enabled() const;

    /// Get X coordinate of specified column.
    /// @param ri the row (line) index, starting with 0.
    /// @param col the column (character) index within row, starting with 0.
    /// @return the X column (character) coordinate.
    /// If there are no such column, the return value is 0.
    int x_at_col(std::size_t ri, std::size_t col) const;

    /// Get column index from known row (line) index and X coordinate.
    /// @param ri the row (line) index, starting with 0.
    /// @param x the X coordinate, starting with 0.
    /// @return the column (character) index.
    /// If specified row does not exist, the return value is 0.
    /// If specified X coordinate is out of row coordinate range, the return value is 0.
    std::size_t col_at_x(std::size_t ri, int x) const;

    /// Get row (line) index from Y coordinate.
    /// @param y the Y coordinate.
    /// @return the row (line) index, starting with 0.
    /// If specified Y coordinate is above used space, the last row returned.
    /// If text buffer is empty or specified Y coordinate negative, the return value is 0.
    std::size_t row_at_y(int y) const;

    /// Get baseline coordinate of the specified row (line).
    /// @param ri the row (line) index, starting with 0.
    /// @return the baseline Y coordinate, starting with 0.
    /// If specified row index is out of range, the returning value is 0.
    /// If specified row index is valid and row exists, the returning value can not be 0.
    int baseline(std::size_t ri) const;

    /// Get row (line) bounds.
    /// @param ri the row (line) index, starting with 0.
    /// @param top a reference to the returning top Y coordinate.
    /// @param bottom a reference to the returning bottom Y coordinate.
    /// If specified row does not exist, the returning value are equals to zero.
    void get_row_bounds(std::size_t ri, int & top, int & bottom) const;

    /// @name Accessors to established actions.
    /// @{

    /// Gets "Move Left" action.
    Action & move_left_action();

    /// Gets "Select Left" action.
    Action & select_left_action();

    /// Gets "Move Right" action.
    Action & move_right_action();

    /// Gets "Select Right" action.
    Action & select_right_action();

    /// Gets "Move Up" action.
    Action & move_up_action();

    /// Gets "Select Up" action.
    Action & select_up_action();

    /// Gets "Move Down" action.
    Action & move_down_action();

    /// Gets "Select Down" action.
    Action & select_down_action();

    /// Gets "Move Word Left" action.
    Action & move_word_left_action();

    /// Gets "Select Word Left" action.
    Action & select_word_left_action();

    /// Gets "Move Word Right" action.
    Action & move_word_right_action();

    /// Gets "Select Word Right" action.
    Action & select_word_right_action();

    /// Gets "Move Home" action.
    Action & move_home_action();

    /// Gets "Select Home" action.
    Action & select_home_action();

    /// Gets "Move To End of Line" action.
    Action & move_to_eol_action();

    /// Gets "Select To End of Line" action.
    Action & select_to_eol_action();

    /// Gets "Move To Start of File" action.
    Action & move_to_sof_action();

    /// Gets "Select To Start of File" action.
    Action & select_to_sof_action();

    /// Gets "Move To End of File" action.
    Action & move_to_eof_action();

    /// Gets "Select To End of File" action.
    Action & select_to_eof_action();

    /// Gets "Move Page Up" action.
    Action & move_page_up_action();

    /// Gets "Move Page Down" action.
    Action & move_page_down_action();

    /// Gets "Select Page Up" action.
    Action & select_page_up_action();

    /// Gets "Select Page Down" action.
    Action & select_page_down_action();

    /// Gets "Select All" action.
    Action & select_all_action();

    /// Gets "Copy" action.
    Action & copy_action();

    /// Gets "Cancel" action.
    Action & cancel_action();

    /// @}
    /// @name Accessors to established signals.
    /// @{

    /// Signal emitted when selection changes.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_selection_changed();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_selection_changed();

    /// Signal emitted when caret moves.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_caret_motion();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_caret_motion();

    /// Signal emitted when user clicks on the text.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_click();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_click();

    /// @}

protected:

    Text(Widget_ptr wp);

};

} // namespace tau

#endif // TAU_TEXT_HH
