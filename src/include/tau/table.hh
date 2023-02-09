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

/// @file table.hh The Table class.

#ifndef TAU_TABLE_HH
#define TAU_TABLE_HH

#include <tau/enums.hh>
#include <tau/container.hh>
#include <climits>

namespace tau {

/// A container which arranges its child widgets in rows and columns.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// The table places widgets in two-dimensional space using X and Y axes.
/// The X and Y cell coordinates may be both negative or positive excluding
/// INT_MIN and INT_MAX values.
/// %Table can select widgets within specified range.
/// @ingroup container_group
class Table: public Container {
public:

    /// Repsents span within Table.
    /// It may be widget's span or table's span.
    struct Span {

        /// Minimal column index.
        int     xmin = INT_MAX;

        /// Minimal row index.
        int     ymin = INT_MAX;

        /// Maximal column index outside of specified range.
        int     xmax = INT_MIN;

        /// Maximal row index outside of specified range.
        int     ymax = INT_MIN;
    };

    /// Default constructor.
    Table();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Table(const Table & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Table & operator=(const Table & other) = default;

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
    Table(Widget_ptr wp);

    /// Assign implementation.
    ///
    /// @since 0.4.0
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Table & operator=(Widget_ptr wp);

    /// Get table span in cells.
    /// On empty table both xmin and ymin are equals to @b INT_MAX and
    /// both xmax and ymax are equals to @b INT_MIN.
    Span span() const;

    /// Get column span in cells.
    /// @param[in] col the column number.
    /// @param[out] ymin where to store minimal Y value.
    /// @param[out] ymax where to store maximal Y value (first outside of range).
    ///
    /// When column does not exist, the returning values are @b INT_MAX for ymin and @b INT_MIN for ymax.
    void get_column_span(int col, int & ymin, int & ymax);

    /// Get row span in cells.
    /// @param[in] row the row number.
    /// @param[out] xmin where to store minimal X value.
    /// @param[out] xmax where to store maximal X value (first outside of range).
    ///
    /// When row does not exist, the returning values are @b INT_MAX for xmin and @b INT_MIN for xmax.
    void get_row_span(int row, int & xmin, int & xmax);

    /// Set column spacing in pixels.
    /// @param xspacing the spacing in pixels along X axis.
    void set_column_spacing(unsigned xspacing);

    /// Set row spacing in pixels.
    /// @param yspacing the spacing in pixels along Y axis.
    void set_row_spacing(unsigned yspacing);

    /// Set column and row spacing.
    /// @param xspacing the spacing in pixels along X axis.
    /// @param yspacing the spacing in pixels along Y axis.
    /// @overload set_spacing(unsigned xspacing, unsigned yspacing)
    void set_spacing(unsigned xspacing, unsigned yspacing);

    /// Set column and row spacing.
    /// @param spacing the spacing in pixels along both X and Y axes.
    /// @since 0.4.0
    /// @overload set_spacing(unsigned spacing)
    void set_spacing(unsigned spacing);

    /// Get column (along X axis) spacing in pixels.
    /// @return column spacing in pixels.
    unsigned column_spacing() const;

    /// Get row (along Y axis) spacing in pixels.
    /// @return row spacing in pixels.
    unsigned row_spacing() const;

    /// Put widget.
    /// @param w the widget to be inserted.
    /// @param x the X cell coordinate of top left corner.
    /// @param y the Y cell coordinate of top left corner.
    /// @param xspan the number of cells to be occupied along X axis.
    /// @param yspan the number of cells to be occupied along Y axis.
    /// @param xsh shrink child horizontally.
    /// @param ysh shrink child vertically.
    /// @throw user_error if widget already inserted into another container.
    void put(Widget & w, int x, int y, unsigned xspan=1, unsigned yspan=1, bool xsh=false, bool ysh=false);

    /// Remove specified widget.
    /// @param w widget to be removed.
    void remove(Widget & w);

    /// Remove all widgets within specified range.
    /// @param xmin minimal X coordinate.
    /// @param ymin minimal Y coordinate.
    /// @param xmax maximal X coordinate (the first coordinate outside of range).
    /// @param ymax maximal Y coordinate (the first coordinate outside of range).
    void remove(int xmin, int ymin, int xmax, int ymax);

    /// Remove all children.
    void clear();

    /// Change widget span in cells using existing shrink options.
    /// @param w the widget.
    /// @param x the X cell coordinate of top left corner.
    /// @param y the Y cell coordinate of top left corner.
    /// @param xspan the number of cells to be occupied along X axis.
    /// @param yspan the number of cells to be occupied along Y axis.
    /// %Widget w must be child of the table using put() method.
    void respan(Widget & w, int x, int y, unsigned xspan, unsigned yspan);

    /// Change widget span in cells.
    /// @param w the widget.
    /// @param x the X cell coordinate of top left corner.
    /// @param y the Y cell coordinate of top left corner.
    /// @param xspan the number of cells to be occupied along X axis.
    /// @param yspan the number of cells to be occupied along Y axis.
    /// @param xsh shrink widget along X axis.
    /// @param ysh shrink widget along X axis.
    /// %Widget w must be child of the table using put() method.
    void respan(Widget & w, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh);

    /// Get widget span.
    /// @param w the widget.
    ///
    /// If widget not found, both xmin and ymin are equals to @b INT_MAX and
    /// both xmax and ymax are equals to @b INT_MIN.
    Span span(const Widget & w) const;

    /// Insert columns.
    /// @param x the column index.
    /// @param n_columns column count to insert.
    void insert_columns(int x, unsigned n_columns);

    /// Insert rows.
    /// @param y the row index.
    /// @param n_rows row count to insert.
    void insert_rows(int y, unsigned n_rows);

    /// Remove columns.
    /// The remaining columns are shifted to the left.
    void remove_columns(int x, unsigned n_columns);

    /// Remove rows.
    /// The remaining rows are shifted to the top.
    void remove_rows(int y, unsigned n_rows);

    /// Set overall column align.
    void align_columns(Align xalign);

    /// Get overall column align.
    Align columns_align() const;

    /// Set single column align.
    /// When single column align set, the overall column align ignored.
    /// @param x column index.
    /// @param xalign the align.
    void align_column(int x, Align xalign);

    /// Get specified column align.
    /// @param x column index.
    Align column_align(int x) const;

    /// Unset single column align.
    /// Unsets align prevously set by align_column().
    /// @param x column index.
    void unalign_column(int x);

    /// Set overall row align.
    void align_rows(Align xalign);

    /// Get overall row align.
    Align rows_align() const;

    /// Set single row align.
    /// When single row align set, the overall row align ignored.
    /// @param y row index.
    /// @param yalign the align.
    void align_row(int y, Align yalign);

    /// Get row align.
    Align row_align(int y) const;

    /// Unset single row align.
    /// Unsets row align previously set by align_row().
    /// @param y row index.
    void unalign_row(int y);

    /// Set widget align.
    /// @param w the widget.
    /// @param xalign align along X axis.
    /// @param yalign align along Y axis.
    void align(Widget & w, Align xalign, Align yalign=ALIGN_CENTER);

    /// Get widget align.
    /// @param[in] w the widget.
    /// @param[out] xalign where to store align along X axis.
    /// @param[out] yalign where to store align along Y axis.
    void get_align(const Widget & w, Align & xalign, Align & yalign) const;

    /// Unset widget align.
    /// @param w the widget.
    void unalign(Widget & w);

    /// Select range.
    /// @param x the X coordinate in cells of top left selection corner.
    /// @param y the Y coordinate in cells of top left selection corner.
    /// @param xspan the number of cells along X axis in selection.
    /// @param yspan the number of cells along Y axis in selection.
    ///
    /// Removes previous selection and selects specified range.
    void select(int x, int y, unsigned xspan=1, unsigned yspan=1);

    /// Select the specified column.
    /// Removes previous selection and selects specified column.
    void select_column(int x);

    /// Select the specified row.
    /// Removes previous selection and selects specified row.
    void select_row(int y);

    /// Remove current selection.
    void unselect();

    /// Get current selection coordinates.
    ///
    /// If no selection, both xmin and ymin are equals to @b INT_MAX and
    /// both xmax and ymax are equals to @b INT_MIN.
    Span selection() const;

    /// Mark the specified range.
    /// @param x the X coordinate in cells of top left mark corner.
    /// @param y the Y coordinate in cells of top left mark corner.
    /// @param xspan the number of cells along X axis in mark.
    /// @param yspan the number of cells along Y axis in mark.
    ///
    /// The mark is added to the list of previous marks.
    void mark(int x, int y, unsigned xspan, unsigned yspan);

    /// Mark the specified column.
    /// @param x the column index to be marked.
    ///
    /// The mark is added to the list of previous marks.
    void mark_column(int x);

    /// Mark the specified row.
    /// @param y the row index to be marked.
    ///
    /// The mark is added to the list of previous marks.
    void mark_row(int y);

    /// Unmark the specified range.
    /// @param xmin the X coordinate in cells of top left corner.
    /// @param ymin the Y coordinate in cells of top left corner.
    /// @param xmax the X coordinate in cells of bottom right corner (not included in mark).
    /// @param ymax the Y coordinate in cells of bottom right corner (not included in mark).
    void unmark(int xmin, int ymin, int xmax, int ymax);

    /// Unmark specified column.
    /// @param x the column index in cells to unmark.
    void unmark_column(int x);

    /// Unmark specified row.
    /// @param y the row index in cells to unmark.
    void unmark_row(int y);

    /// Unmark all.
    void unmark_all();

    /// Get marks.
    std::vector<Span> marks() const;

    /// Set column margins.
    /// @param x the column coordinate in cells.
    /// @param left the left margin in pixels.
    /// @param right the left margin in pixels.
    /// When single column margins are set, the overall column margins ignored.
    void set_column_margin(int x, unsigned left, unsigned right);

    /// Set row margins.
    /// @param y the row coordinate in cells.
    /// @param top the top margin in pixels.
    /// @param bottom the bottom margin in pixels.
    void set_row_margin(int y, unsigned top, unsigned bottom);

    /// Get single column margins.
    /// @param[in] x the column coordinate in cells.
    /// @param[out] left where to store left margin width in pixels.
    /// @param[out] right where to store right margin width in pixels.
    void get_column_margin(int x, unsigned & left, unsigned & right) const;

    /// Get row margins.
    /// @param[in] y the row coordinate in cells.
    /// @param[out] top where to store top margin width in pixels.
    /// @param[out] bottom where to store bottom margin width in pixels.
    void get_row_margin(int y, unsigned & top, unsigned & bottom) const;

    /// Set all columns margins.
    /// @param left the left margin in pixels.
    /// @param right the left margin in pixels.
    void set_columns_margin(unsigned left, unsigned right);

    /// Set all rows margins.
    /// @param top the top margin in pixels.
    /// @param bottom the bottom margin in pixels.
    void set_rows_margin(unsigned top, unsigned bottom);

    /// Get columns margins.
    /// @param[out] left where to store left margin width in pixels.
    /// @param[out] right where to store right margin width in pixels.
    void get_columns_margin(unsigned & left, unsigned & right) const;

    /// Get rows margins.
    /// @param[out] top where to store top margin width in pixels.
    /// @param[out] bottom where to store bottom margin width in pixels.
    void get_rows_margin(unsigned & top, unsigned & bottom) const;

    /// Get range bounds in pixels for the specified range.
    /// @param x the X coordinate in cells of top left corner.
    /// @param y the Y coordinate in cells of top left corner.
    /// @param xspan the number of cells along X axis.
    /// @param yspan the number of cells along Y axis.
    Rect bounds(int x, int y, unsigned xspan=1, unsigned yspan=1) const;

    /// Get column bounds in pixels.
    /// @param[in] col the column number.
    /// @param[out] xmin where to store minimal X coordinate in pixels.
    /// @param[out] xmax where to store maximal X coordinate in pixels (the first outside of range).
    void get_column_bounds(int col, int & xmin, int & xmax);

    /// Get row bounds in pixels.
    /// @param[in] row the column number.
    /// @param[out] ymin where to store minimal Y coordinate in pixels.
    /// @param[out] ymax where to store maximal Y coordinate in pixels (the first outside of range).
    void get_row_bounds(int row, int & ymin, int & ymax);

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

    /// Signal emitted when column bounds changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_column_bounds_changed(int col);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_column_bounds_changed();

    /// Signal emitted when row bounds changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_row_bounds_changed(int row);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_row_bounds_changed();

    /// Signal emitted when selection changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_selection_changed();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_selection_changed();
};

} // namespace tau

#endif // TAU_TABLE_HH
