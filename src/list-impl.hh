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

#ifndef TAU_LIST_IMPL_HH
#define TAU_LIST_IMPL_HH

#include <tau/action.hh>
#include <tau/input.hh>
#include <table-impl.hh>
#include <set>

namespace tau {

class List_impl: public Table_impl {
public:

    List_impl();
   ~List_impl() { signal_destroy_(); }

    int  prepend_row(Widget_ptr wp, bool shrink=false);
    int  prepend_row(Widget_ptr wp, Align align);
    int  insert_row(Widget_ptr wp, int position, bool shrink=false);
    int  insert_row(Widget_ptr wp, int position, Align align);
    int  append_row(Widget_ptr wp, bool shrink=false);
    int  append_row(Widget_ptr wp, Align align);

    int  prepend(Widget_ptr wp, bool shrink=false);
    int  prepend(Widget_ptr wp, Align align);
    int  insert(Widget_ptr wp, int position, bool shrink=false);
    int  insert(Widget_ptr wp, int position, Align align);
    int  append(Widget_ptr wp, bool shrink=false);
    int  append(Widget_ptr wp, Align align);

    int  prepend(int row, Widget_ptr wp, bool shrink=false);
    int  prepend(int row, Widget_ptr wp, Align align);
    int  insert(int row, Widget_ptr wp, int position, bool shrink=false);
    int  insert(int row, Widget_ptr wp, int position, Align align);
    int  append(int row, Widget_ptr wp, bool shrink=false);
    int  append(int row, Widget_ptr wp, Align align);

    // Overrides Table_impl.
    void remove(Widget_impl * wp) override;

    void remove(int row);

    int select(int row);
    int select_next();
    int select_previous();
    int select_front(bool emit=false);
    int select_back(bool emit=false);
    int selected_row() const;
    void unselect();

    void show_headers();
    void hide_headers();
    bool headers_visible() const;

    void show_header(int column);
    void show_header(int column, const ustring & title, Align align=ALIGN_START);
    void show_header(int column, Widget_ptr title);
    void hide_header(int column);

    void show_sort_marker(int column, bool descend=false);
    void hide_sort_marker();

    void set_column_spacing(unsigned spacing);
    void set_row_spacing(unsigned spacing);
    unsigned column_spacing() const;
    unsigned row_spacing() const;

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

    void set_column_margin(int x, unsigned left, unsigned right);
    void get_column_margin(int x, unsigned & left, unsigned & right) const;

    bool empty() const;

    void allow_multiple_select();
    void disallow_multiple_select();

    bool multiple_select_allowed() const {
        return multiple_select_allowed_;
    }

    // Overriden by List_text_impl.
    // Overrides Table_impl.
    virtual void clear() override;

    void align(Widget_ptr wp, Align xalign, Align yalign);

    Action & cancel_action() { return cancel_action_; }
    Action & enter_action() { return enter_action_; }
    Action & previous_action() { return previous_action_; }
    Action & next_action() { return next_action_; }
    Action & previous_page_action() { return previous_page_action_; }
    Action & next_page_action() { return next_page_action_; }
    Action & home_action() { return home_action_; }
    Action & end_action() { return end_action_; }
    Action & select_previous_action() { return select_previous_action_; }
    Action & select_next_action() { return select_next_action_; }
    Action & select_previous_page_action() { return select_previous_page_action_; }
    Action & select_next_page_action() { return select_next_page_action_; }
    Action & select_home_action() { return select_home_action_; }
    Action & select_end_action() { return select_end_action_; }

    signal<void(int)> & signal_row_selected() {
        return signal_row_selected_;
    }

    signal<void(int)> & signal_row_activated() {
        return signal_row_activated_;
    }

    signal<void(int)> & signal_row_removed() {
        return signal_row_removed_;
    }

    signal<void(int, int)> & signal_row_moved() {
        return signal_row_moved_;
    }

    signal<bool(int)> & signal_mark_validate() {
        return signal_mark_validate_;
    }

    signal<void(int)> & signal_header_click() {
        return signal_header_click_;
    }

    signal<void(int)> & signal_header_width_changed() {
        return signal_header_width_changed_;
    }

protected:

    struct Selectable {
        int             min_ = 0;
        int             max_ = 0;
    };

    // .first is for row index & table index.
    using Selectables = std::map<int, Selectable>;

    // Header container.
    struct Header {
        int             column;         // Column where header is bound to.
        Box_ptr         box;            // Header widget.
        Widget_ptr      title;          // Title widget.
        Widget_ptr      marker;         // Sort marker.
        Separator_ptr   sep;            // Separator after header.
        bool            fix = false;    // Fix mouse button press.
        int             lx;             // Last x mouse position during separator dragging.
        int             cw;             // Column width before separator dragging.
    };

    using Headers = std::list<Header>;  // Table headers.
    using Frees = std::set<Widget_impl *>;  // Freely allocated rows.

    bool                multiple_select_allowed_ = false;
    Scroller_ptr        scroller_;
    Scroller_ptr        hdr_scroller_;
    Slider_ptr          hslider_;
    Slider_ptr          vslider_;
    Table_ptr           table_;
    Absolute_ptr        hdrbox_;
    Headers             headers_;
    Selectables         selectables_;
    Frees               frees_;
    int                 trunk_min_ = 0;
    int                 trunk_max_ = 0;

    Action              cancel_action_ { "Escape Cancel", fun(this, &Widget_impl::drop_focus) };
    Action              enter_action_ { KC_ENTER, KM_NONE, fun(this, &List_impl::on_enter) };
    Action              previous_action_ { KC_UP, KM_NONE, fun(this, &List_impl::on_prev) };
    Action              next_action_ { KC_DOWN, KM_NONE, fun(this, &List_impl::on_next) };
    Action              previous_page_action_ { KC_PAGE_UP, KM_NONE, fun(this, &List_impl::on_page_up) };
    Action              next_page_action_ { KC_PAGE_DOWN, KM_NONE, fun(this, &List_impl::on_page_down) };
    Action              home_action_ { KC_HOME, KM_NONE, fun(this, &List_impl::on_home) };
    Action              end_action_ { KC_END, KM_NONE, fun(this, &List_impl::on_end) };
    Action              select_previous_action_ { KC_UP, KM_SHIFT, fun(this, &List_impl::on_select_prev) };
    Action              select_next_action_ { KC_DOWN, KM_SHIFT, fun(this, &List_impl::on_select_next) };
    Action              select_previous_page_action_ { KC_PAGE_UP, KM_SHIFT, fun(this, &List_impl::on_select_page_up) };
    Action              select_next_page_action_ { KC_PAGE_DOWN, KM_SHIFT, fun(this, &List_impl::on_select_page_down) };
    Action              select_home_action_ { KC_HOME, KM_SHIFT, fun(this, &List_impl::on_select_home) };
    Action              select_end_action_ { KC_END, KM_SHIFT, fun(this, &List_impl::on_select_end) };

    signal<void(int)>   signal_row_selected_;
    signal<void(int)>   signal_row_activated_;
    signal<void(int)>   signal_row_removed_;
    signal<void(int, int)> signal_row_moved_;
    signal<bool(int)>   signal_mark_validate_;
    signal<void(int)>   signal_header_click_;
    signal<void(int)>   signal_header_width_changed_;

private:

    int  select_emit(int row);
    void unselect_emit();

    int  prepend_row(Widget_ptr wp, Align align, bool shrink);
    int  insert_row(Widget_ptr wp, int position, Align align, bool shrink);
    int  append_row(Widget_ptr wp, Align align, bool shrink);

    int  prepend(Widget_ptr wp, Align align, bool shrink);
    int  insert(Widget_ptr wp, int position, Align align, bool shrink);
    int  append(Widget_ptr wp, Align align, bool shrink);

    int  insert(int branch, Widget_ptr wp, int position, Align align, bool shrink);
    int  prepend(int branch, Widget_ptr wp, Align align, bool shrink);
    int  append(int branch, Widget_ptr wp, Align align, bool shrink);

    bool row_marked(int row);
    void update_selection();
    void activate_current();
    void arrange_headers();
    void sync_scrollers_offset();
    void scroll_to_selection();
    void adjust();
    int  prev_row();
    int  next_row();
    int  page_up_row();
    int  page_down_row();

    bool on_table_mouse_down(int mbt, int mm, const Point & pt);
    bool on_table_mouse_double_click(int mbt, int mm, const Point & pt);
    bool on_header_mouse_down(int mbt, int mm, const Point & pt, int column);
    bool on_sep_mouse_down(int mbt, int mm, const Point & pt, Header & hdr);
    bool on_sep_mouse_up(int mbt, int mm, const Point & pt, Header & hdr);
    void on_sep_mouse_motion(int mm, const Point & pt, Header & hdr);
    void on_row_bounds_changed(int row);
    void on_focus_in();

    void on_enter();
    void on_next();
    void on_prev();
    void on_page_up();
    void on_page_down();
    void on_home();
    void on_end();
    void on_select_next();
    void on_select_prev();
    void on_select_page_up();
    void on_select_page_down();
    void on_select_home();
    void on_select_end();
};

} // namespace tau

#endif // TAU_LIST_IMPL_HH
