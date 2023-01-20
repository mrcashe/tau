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

/// @file list-text.hh The List_text class.

#ifndef TAU_LIST_TEXT_HH
#define TAU_LIST_TEXT_HH

#include <tau/list.hh>

namespace tau {

/// %Text specialization of List container.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
class List_text: public Widget {
public:

    /// Default constructor.
    List_text(Align align=ALIGN_START);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    List_text(const List_text & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    List_text & operator=(const List_text & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    List_text(Widget_ptr wp);

    /// Set horizontal text align.
    void set_text_align(Align align);

    /// Get horizontal text align.
    Align text_align() const;

    /// Prepend text.
    int prepend(const ustring & str);

    /// Insert text at specified row.
    int insert(const ustring & str, int row);

    /// Insert text above specified text.
    int insert_before(const ustring & str, const ustring & other);

    /// Insert text below specified text.
    int insert_after(const ustring & str, const ustring & other);

    /// Append text.
    int append(const ustring & str);

    /// Select specified row.
    void select(int row);

    /// Select specified text.
    /// @param str text to be selected.
    /// @param similar if @b true, use str_similar() to compare, else compare byte-to-byte.
    void select(const ustring & str, bool similar=false);

    /// Select top row.
    int select_front();

    /// Select bottom row.
    int select_back();

    /// Get selected text.
    /// @return text at current row or empty string if there are no selection.
    ustring selection() const;

    /// Find first row where specified text is.
    /// @return INT_MIN if not found or row number.
    int find(const ustring & str) const;

    /// Get text at given row.
    /// @param row row number.
    /// @return text or empty string if not found.
    ustring at(int row) const;

    /// Test if specified text already exists in the list.
    bool contains(const ustring & str) const;

    /// Remove specified row.
    void remove(int row);

    /// Remove rows having specified text.
    void remove_text(const ustring & str);

    /// Unselect all.
    void unselect();

    /// Test if empty.
    /// @return true if no branches made.
    bool empty() const;

    /// Remove all rows.
    void clear();

    /// Allow multiple select.
    /// Disallowed by default.
    void allow_multiple_select();

    /// Disallow multiple select.
    /// Disallowed by default.
    void disallow_multiple_select();

    /// Test if multiple select allowed.
    /// Disallowed by default.
    bool multiple_select_allowed() const;

    /// Show all headers.
    void show_headers();

    /// Hide all headers.
    void hide_headers();

    /// Test if headers visible.
    bool headers_visible() const;

    /// Show header for specified column.
    void show_header(int column);

    /// Show header for specified column and assign a title text with align.
    void show_header(int column, const ustring & title, Align align=ALIGN_START);

    /// Show header for specified column and assign a title widget.
    void show_header(int column, Widget & w);

    /// Hide specified header.
    void hide_header(int column);

    /// Show sort marker.
    /// Shows sort marker as arrow triangle at specified column.
    /// This method works only when specified column has visible header
    /// shown by show_header() methods. If other column has marker shown,
    /// it will be shown at new place.
    void show_sort_marker(int column, bool descend=false);

    /// Hide sort marker.
    void hide_sort_marker();

    /// Set column spacing in pixels.
    /// @param spacing the spacing in pixels along X axis.
    void set_column_spacing(unsigned spacing);

    /// Set row spacing in pixels.
    /// @param spacing the spacing in pixels along Y axis.
    void set_row_spacing(unsigned spacing);

    /// Get column (along X axis) spacing in pixels.
    /// @return column spacing in pixels.
    unsigned column_spacing() const;

    /// Get row (along Y axis) spacing in pixels.
    /// @return row spacing in pixels.
    unsigned row_spacing() const;

    /// Set column width (in pixels).
    /// @param column the column number.
    /// @param width column width in pixels.
    void set_column_width(int column, unsigned width);

    /// Get column width (in pixels).
    /// @param column the column number.
    /// @return column width in pixels previously set by set_column_width().
    unsigned column_width(int column) const;

    /// Set row height (in pixels).
    /// @param row the row number.
    /// @param height row height in pixels.
    void set_row_height(int row, unsigned height);

    /// Get row height (in pixels).
    /// @param row the row number.
    /// @return row height in pixels previously set by set_row_height().
    unsigned row_height(int row) const;

    /// Set minimal column width (in pixels).
    /// @param column the column number.
    /// @param width minimal column width in pixels.
    void set_min_column_width(int column, unsigned width);

    /// Get minimal column width (in pixels).
    /// @param column the column number.
    /// @return minimal column width in pixels previously set by set_min_column_width().
    unsigned min_column_width(int column) const;

    /// Set minimal row height (in pixels).
    /// @param row the row number.
    /// @param height minimal row height in pixels.
    void set_min_row_height(int row, unsigned height);

    /// Get minimal row height (in pixels).
    /// @param row the row number.
    /// @return minimal row height in pixels previously set by set_min_row_height().
    unsigned min_row_height(int row) const;

    /// Set maximal column width (in pixels).
    /// @param column the column number.
    /// @param width maximal column width in pixels.
    void set_max_column_width(int column, unsigned width);

    /// Get maximal column width (in pixels).
    /// @param column the column number.
    /// @return maximal column width in pixels previously set by set_max_column_width().
    unsigned max_column_width(int column) const;

    /// Set maximal row height (in pixels).
    /// @param row the row number.
    /// @param height maximal row height in pixels.
    void set_max_row_height(int row, unsigned height);

    /// Get maximal row height (in pixels).
    /// @param row the row number.
    /// @return minimal row height in pixels previously set by set_max_row_height().
    unsigned max_row_height(int row) const;

    /// Set single column margins.
    /// @param x the column coordinate in cells.
    /// @param left the left margin in pixels.
    /// @param right the left margin in pixels.
    /// When single column margins are set, the overall column margins ignored.
    void set_column_margin(int x, unsigned left, unsigned right);

    /// Get single column margins.
    /// @param x the column coordinate in cells.
    /// @param left where to store left margin width in pixels.
    /// @param right where to store right margin width in pixels.
    void get_column_margin(int x, unsigned & left, unsigned & right) const;

    /// Signal emitted when text selected.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_text_selected(int row, const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<void(int, const ustring &)> & signal_text_selected();

    /// Signal emitted when some text activated.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_text_activated(int row, const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<void(int, const ustring &)> & signal_text_activated();

    /// Signal emitted when some text removed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_text_removed(int row, const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<void(int, const ustring &)> & signal_text_removed();

    /// Signal emitted when some text changed it's position.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_text_moved(int old_row, int new_row, const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<void(int, int, const ustring &)> & signal_text_moved();

    /// Signal emitted when %List going to mark branch.
    /// Returning true from this signal will prevent marking.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// bool on_mark_validate(int branch);
    /// ~~~~~~~~~~~~~~
    signal<bool(int)> & signal_mark_validate();

    /// Signal emitted when user clicks onto column header.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_header_click(int column);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_header_click();

    /// Signal emitted when user changes header column width by dragging mouse.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_header_width_changed(int column);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_header_width_changed();
};

} // namespace tau

#endif // TAU_LIST_TEXT_HH
