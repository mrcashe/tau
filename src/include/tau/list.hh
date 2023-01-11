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

/// @file list.hh The List class.

#ifndef TAU_LIST_HH
#define TAU_LIST_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// A container that allocates widgets along the Y axis.
/// One or more widgets form a branch. Each branch is identified by its coordinate
/// on the Y axis. The coordinates in this case do not imply the
/// pixel coordinates on the screen - on the contrary, now we are talking about
/// cells that hold the whole widget. The coordinates of two neighboring
/// branches differ by at least one. All widgets on the same branch have
/// the same coordinates on the Y axis.
/// As already noted above, the branch can consist of several widgets.
/// This principle allows you to build lists consisting of several columns.
/// All branches are selectable. When list selects or deselects branch, it sends
/// signal_select() or signal_unselect(), respectively, to each widget in the branch.
/// In addition, it is possible to insert widgets outside the branches. Such widgets
/// are not selectable and can serve for better structuring of information.
/// @ingroup container_group
class List: public Widget {
public:

    /// Default constructor.
    List();

    /// Prepend selectable branch.
    /// @throw user_error if widget already inserted into another container.
    int prepend_row(Widget & w, bool shrink=false);

    /// Prepend selectable branch.
    /// @throw user_error if widget already inserted into another container.
    int prepend_row(Widget & w, Align align);

    /// Insert selectable branch at specified position.
    /// @throw user_error if widget already inserted into another container.
    int insert_row(Widget & w, int position, bool shrink=false);

    /// Insert selectable branch at specified position.
    /// @throw user_error if widget already inserted into another container.
    int insert_row(Widget & w, int position, Align align);

    /// Append selectable branch.
    /// @throw user_error if widget already inserted into another container.
    int append_row(Widget & w, bool shrink=false);

    /// Append selectable branch.
    /// @throw user_error if widget already inserted into another container.
    int append_row(Widget & w, Align align);

    /// Prepend non-selectable widget.
    /// @throw user_error if widget already inserted into another container.
    int prepend(Widget & w, bool shrink=false);

    /// Prepend non-selectable widget.
    /// @throw user_error if widget already inserted into another container.
    int prepend(Widget & w, Align align);

    /// Insert non-selectable widget at specified position.
    /// @throw user_error if widget already inserted into another container.
    int insert(Widget & w, int position, bool shrink=false);

    /// Insert non-selectable widget at specified position.
    /// @throw user_error if widget already inserted into another container.
    int insert(Widget & w, int position, Align align);

    /// Append non-selectable widget.
    /// @throw user_error if widget already inserted into another container.
    int append(Widget & w, bool shrink=false);

    /// Append non-selectable widget.
    /// @throw user_error if widget already inserted into another container.
    int append(Widget & w, Align align);

    /// Prepend widget into specified branch.
    /// @throw user_error if widget already inserted into another container.
    /// @throw graphics_error (bad branch).
    int prepend(int branch, Widget & w, bool shrink=false);

    /// Prepend widget into specified branch.
    /// @throw user_error if widget already inserted into another container.
    /// @throw graphics_error (bad branch).
    int prepend(int branch, Widget & w, Align align);

    /// Insert widget into specified branch and specified position.
    /// @throw user_error if widget already inserted into another container.
    /// @throw graphics_error (bad branch).
    int insert(int branch, Widget & w, int position, bool shrink=false);

    /// Insert widget into specified branch and specified position.
    /// @throw user_error if widget already inserted into another container.
    /// @throw graphics_error (bad branch).
    int insert(int branch, Widget & w, int position, Align align);

    /// Append widget into specified branch.
    /// @throw user_error if widget already inserted into another container.
    /// @throw graphics_error (bad branch).
    int append(int branch, Widget & w, bool shrink=false);

    /// Append widget into specified branch.
    /// @throw graphics_error (bad branch).
    /// @throw user_error if widget already inserted into another container.
    int append(int branch, Widget & w, Align align);

    /// Select specified branch.
    /// @return INT_MIN if no selection available or branch index.
    int select_row(int br);

    /// Select first branch.
    /// @return INT_MIN if no selection available or branch index.
    int select_first();

    /// Select last branch.
    /// @return INT_MIN if no selection available or branch index.
    int select_last();

    /// Get current selected branch.
    /// @return INT_MIN if no selection available or branch index.
    int selected_row() const;

    /// Remove specified branch.
    void remove(int br);

    /// Remove all.
    void clear();

    /// Unselect all.
    void unselect();

    /// Test if empty.
    /// @return true if no branches made.
    bool empty() const;

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
    /// @param column column number.
    /// @param w widget used as title.
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

    /// Set column margins.
    /// @param x the column coordinate in cells.
    /// @param left the left margin in pixels.
    /// @param right the left margin in pixels.
    /// When single column margins are set, the overall column margins ignored.
    void set_column_margin(int x, unsigned left, unsigned right);

    /// Get column margins.
    /// @param x the column coordinate in cells.
    /// @param left where to store left margin width in pixels.
    /// @param right where to store right margin width in pixels.
    void get_column_margin(int x, unsigned & left, unsigned & right) const;

    /// Signal emitted when some branch selected.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_row_selected(int branch);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_row_selected();

    /// Signal emitted when some branch activated.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_row_activated(int branch);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_row_activated();

    /// Signal emitted when some branch removed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_row_removed(int branch);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_row_removed();

    /// Signal emitted when some branch changed it position.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_row_moved(int old_row, int new_row);
    /// ~~~~~~~~~~~~~~
    signal<void(int, int)> & signal_row_moved();

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

#endif // TAU_LIST_HH
