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

#ifndef TAU_TABLE_IMPL_HH
#define TAU_TABLE_IMPL_HH

#include <tau/table.hh>
#include <container-impl.hh>
#include <forward_list>
#include <map>

namespace tau {

class Table_impl: public Container_impl {
public:

    using Span = Table::Span;

    Table_impl();
    Table_impl(unsigned xspacing, unsigned yspacing);
    Table_impl(unsigned spacing);
   ~Table_impl();

    void put(Widget_ptr wp, int x, int y, unsigned xspan=1, unsigned yspan=1, bool xsh=false, bool ysh=false);
    void remove(Widget_impl * wp);
    void remove(int xmin, int ymin, int xmax, int ymax);

    // Overriden by List_impl.
    // Overriden by List_text_impl.
    virtual void clear();

    void respan(Widget_impl * wp, int x, int y, unsigned xspan, unsigned yspan);
    void respan(Widget_impl * wp, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh);
    std::vector<Widget_impl *> children_within_range(int xmin, int ymin, int xmax, int ymax);
    std::vector<Widget_impl *> children_within_range(const Span & rng);

    void set_column_spacing(unsigned xspacing);
    void set_row_spacing(unsigned yspacing);
    void set_spacing(unsigned xspacing, unsigned yspacing);
    void set_spacing(unsigned spacing);
    unsigned column_spacing() const { return xspacing_; }
    unsigned row_spacing() const { return yspacing_; }

    void set_column_margin(int x, unsigned left, unsigned right);
    void set_row_margin(int y, unsigned top, unsigned bottom);
    void set_columns_margin(unsigned left, unsigned right);
    void set_rows_margin(unsigned top, unsigned bottom);

    void get_column_margin(int x, unsigned & left, unsigned & right) const;
    void get_row_margin(int y, unsigned & top, unsigned & bottom) const;

    void get_columns_margin(unsigned & left, unsigned & right) const {
        left = columns_left_;
        right = columns_right_;
    }

    void get_rows_margin(unsigned & top, unsigned & bottom) const {
        top = rows_top_;
        bottom = rows_bottom_;
    }

    void align_columns(Align xalign);
    Align columns_align() const;

    void align_rows(Align yalign);
    Align rows_align() const;

    void align_column(int x, Align column_align);
    Align column_align(int x) const;
    void unalign_column(int x);

    void align_row(int y, Align row_align);
    Align row_align(int y) const;
    void unalign_row(int y);

    void align(Widget_impl * wp, Align xalign, Align yalign=ALIGN_CENTER);
    void get_align(const Widget_impl * wp, Align & xalign, Align & yalign) const;
    void unalign(Widget_impl * wp);

    void insert_columns(int x, unsigned n_columns=1);
    void insert_rows(int y, unsigned n_rows=1);

    void remove_columns(int x, unsigned n_columns=1);
    void remove_rows(int y, unsigned n_rows=1);

    void set_column_width(int column, unsigned width);
    unsigned column_width(int column) const;

    void set_row_height(int row, unsigned height);
    unsigned row_height(int row) const;

    void set_min_column_width(int column, unsigned width);
    unsigned min_column_width(int column) const;

    void set_min_row_height(int row, unsigned height);
    unsigned min_row_height(int row) const;

    void set_max_column_width(int column, unsigned width);
    unsigned max_column_width(int column) const;

    void set_max_row_height(int row, unsigned height);
    unsigned max_row_height(int row) const;

    Span span() const;
    Span span(const Widget_impl * wp) const;
    void get_column_span(int col, int & ymin, int & ymax);
    void get_row_span(int row, int & xmin, int & xmax);
    Rect bounds(int x, int y, unsigned xspan=1, unsigned yspan=1) const;
    void get_column_bounds(int col, int & xmin, int & xmax);
    void get_row_bounds(int row, int & ymin, int & ymax);

    void select(int x, int y, unsigned xspan=1, unsigned yspan=1);
    void select_column(int nth_col);
    void select_row(int nth_row);
    Span selection() const { return sel_; }
    bool has_selection() const { return sel_.xmax > sel_.xmin && sel_.ymax >= sel_.ymin; }
    void unselect();

    void mark(int x, int y, unsigned xspan, unsigned yspan);
    void mark_column(int x);
    void mark_row(int y);
    void unmark(int xmin, int ymin, int xmax, int ymax);
    void unmark_column(int x);
    void unmark_row(int y);
    void unmark_all();
    bool has_marks() const { return !marks_.empty(); }
    std::vector<Span> marks() const;

    int column_at_x(int x) const;
    int row_at_y(int y) const;

    signal<void(int)> & signal_column_bounds_changed();
    signal<void(int)> & signal_row_bounds_changed();
    signal<void()> & signal_selection_changed();

private:

    struct Holder {
        Widget_impl *   wp_ = nullptr;
        int             xmin_;
        int             ymin_;
        int             xmax_;
        int             ymax_;
        bool            xsh_;               // Shrink widget horizontally.
        bool            ysh_;               // Shrink widget vertically.
        bool            align_set_ = false; // Align was set by user.
        Align           xalign_;            // Horizontal align.
        Align           yalign_;            // Vertical align.
        unsigned        wmax_ = 0;          // Calculated maximal width.
        unsigned        hmax_ = 0;          // Calculated maximal height.
        unsigned        wmin_ = 0;          // Calculated minimal width.
        unsigned        hmin_ = 0;          // Calculated minimal height.
    };

    struct Col {
        int             x_ = 0;             // Origin.
        int             lx_ = 0;            // Last origin.
        unsigned        w_ = 0;             // Width.
        unsigned        lw_ = 0;            // Last width.
        unsigned        left_ = 0;          // User specified left margin.
        unsigned        right_ = 0;         // User specified right margin.
        unsigned        usize_ = 0;         // User specified exact size.
        unsigned        umin_ = 0;          // User specified minimal size.
        unsigned        umax_ = 0;          // User specified maximal size.
        Align           xalign_;            // User specified align.
        bool            align_set_ = false; // Align set by the user.
        unsigned        rmax_ = 0;          // Maximal requisition.
        unsigned        rmin_ = 0;          // Minimal requisition.
        unsigned        ref_ = 0;           // Widget count.
        unsigned        shrank_ = 0;        // Shrank widget count.
        unsigned        visible_ = 0;       // Visible widget count.
        unsigned        fill_ = 0;          // Widget count having ALIGN_FILL align set.
    };

    struct Row {
        int             y_ = 0;             // Origin.
        int             ly_ = 0;            // Last origin.
        unsigned        h_ = 0;             // Height.
        unsigned        lh_ = 0;            // Last height.
        unsigned        top_ = 0;           // User specified top margin.
        unsigned        bottom_ = 0;        // User specified bottom margin.
        unsigned        usize_ = 0;         // User specified exact size.
        unsigned        umin_ = 0;          // User specified minimal size.
        unsigned        umax_ = 0;          // User specified maximal size.
        Align           yalign_;            // User specified align.
        bool            align_set_ = false; // Align set by the user.
        unsigned        rmax_ = 0;          // Maximal requisition.
        unsigned        rmin_ = 0;          // Minimal requisition.
        unsigned        ref_ = 0;           // Widget count.
        unsigned        shrank_ = 0;        // Shrank widget count.
        unsigned        visible_ = 0;       // Visible widget count.
        unsigned        fill_ = 0;          // Widget count having ALIGN_FILL align set.
    };

    using Holders = std::map<Widget_impl *, Holder>;
    using Cols = std::map<int, Col>;
    using Rows = std::map<int, Row>;
    using Col_iter =  Cols::iterator;
    using Col_citer = Cols::const_iterator;
    using Row_iter =  Rows::iterator;
    using Row_citer = Rows::const_iterator;
    using Marks = std::forward_list<Span>;

    Holders             holders_;
    Cols                cols_;
    Rows                rows_;
    Span                sel_;
    Marks               marks_;

    unsigned            xspacing_ = 0;
    unsigned            yspacing_ = 0;
    Align               xalign_ = ALIGN_CENTER;
    Align               yalign_ = ALIGN_CENTER;
    unsigned            columns_left_ = 0;
    unsigned            columns_right_ = 0;
    unsigned            rows_top_ = 0;
    unsigned            rows_bottom_ = 0;

    unsigned            nvcol_ = 0;         // Current visible columns count.
    unsigned            nvrow_ = 0;         // Current visible rows count.
    unsigned            nshcol_ = 0;        // Current shrank columns count.
    unsigned            nshrow_ = 0;        // Current shrank rows count.
    unsigned            nucol_ = 0;         // Current columns count having width set by user.
    unsigned            nurow_ = 0;         // Current rows count having height set by user.
    unsigned            xspc_ = 0;          // Current X space value (calculated).
    unsigned            yspc_ = 0;          // Current Y space value (calculated).

    unsigned            rucol_ = 0;         // Requisition of columns having width set by user.
    unsigned            rshcol_ = 0;        // Shrank columns requisition.
    unsigned            rurow_ = 0;         // Requisition of rows having width set by user.
    unsigned            rshrow_ = 0;        // Shrank rows requisition.
    unsigned            xextra_ = 0;        // Extra space along X axis per free column.
    unsigned            xrem_ = 0;          // Extra space remainder along X axis per free column.
    unsigned            yextra_ = 0;        // Extra space along Y axis per free row.
    unsigned            yrem_ = 0;          // Extra space remainder along Y axis per free row.

    signal<void(int)> * signal_column_bounds_changed_ = nullptr;
    signal<void(int)> * signal_row_bounds_changed_ = nullptr;
    signal<void()> *    signal_selection_changed_ = nullptr;

private:

    void init();
    void arrange();
    void recalc();
    void rearrange1();
    void rearrange2();
    void update_requisition();
    void alloc_child(Holder & hol);
    void alloc_cols();
    void alloc_rows();
    void place_widgets();
    void place_holder(Holder & hol, Rect * inval=nullptr);
    Col & new_col(int xx);
    Row & new_row(int yy);
    Col & new_col(int xx, const Col & src);
    Row & new_row(int yy, const Row & src);
    void erase(Col_iter & i);
    void unref(Col_iter & i);
    void erase(Row_iter & i);
    void unref(Row_iter & i);
    bool erasable(const Col & col) const;
    bool erasable(const Row & row) const;
    void change_visibility(Col & col, bool show, bool shrank);
    void change_visibility(Row & row, bool show, bool shrank);

    void wipe_holder(Holder & hol);
    void dist_holder(Holder & hol);
    bool set_column_spacing_i(unsigned xspacing);
    bool set_row_spacing_i(unsigned yspacing);
    Rect range_bounds(const Span & rng) const;
    std::forward_list<Widget_impl *> fchildren(const Span & rng);

    bool on_backpaint(Painter pr, const Rect & inval);
    void on_child_requisition(Widget_impl * wi);
    void on_child_show(Widget_impl * wi);
    void on_child_hide(Widget_impl * wi);
    bool on_take_focus();
};

} // namespace tau

#endif // TAU_TABLE_IMPL_HH
