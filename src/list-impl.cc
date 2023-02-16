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

#include <tau/exception.hh>
#include <tau/icon.hh>
#include <absolute-impl.hh>
#include <box-impl.hh>
#include <icon-impl.hh>
#include <list-impl.hh>
#include <scroller-impl.hh>
#include <separator-impl.hh>
#include <slider-impl.hh>
#include <text-impl.hh>
#include <climits>
#include <iostream>

namespace tau {

List_impl::List_impl():
    Table_impl(),
    table_(std::make_shared<Table_impl>())
{
    table_->style().redirect(STYLE_WHITESPACE_BACKGROUND, STYLE_BACKGROUND);
    table_->signal_mouse_down().connect(fun(this, &List_impl::on_table_mouse_down));
    table_->signal_mouse_double_click().connect(fun(this, &List_impl::on_table_mouse_double_click));
    table_->signal_size_changed().connect(fun(this, &List_impl::scroll_to_selection));
    table_->signal_size_changed().connect(fun(this, &List_impl::arrange_headers));
    table_->signal_column_bounds_changed().connect(fun(this, &List_impl::on_row_bounds_changed));

    scroller_ = std::make_shared<Scroller_impl>();
    scroller_->signal_pan_changed().connect(fun(this, &List_impl::sync_scrollers_offset));
    scroller_->insert(table_);
    put(scroller_, 0, 1);

    hdrbox_ = std::make_shared<Absolute_impl>();
    hdrbox_->hide();
    hdr_scroller_ = std::make_shared<Scroller_impl>();
    hdr_scroller_->insert(hdrbox_);
    hdr_scroller_->disallow_focus();
    put(hdr_scroller_, 0, 0, 1, 1, false, true);

    hslider_ = std::make_shared<Slider_impl>(scroller_, OR_RIGHT, true);
    put(hslider_, 0, 2, 1, 1, false, true);

    vslider_ = std::make_shared<Slider_impl>(scroller_, OR_DOWN, true);
    put(vslider_, 1, 1, 1, 1, true, false);

    table_->connect_action(cancel_action_);
    table_->connect_action(enter_action_);
    table_->connect_action(previous_action_);
    table_->connect_action(next_action_);
    table_->connect_action(previous_page_action_);
    table_->connect_action(next_page_action_);
    table_->connect_action(home_action_);
    table_->connect_action(end_action_);
    table_->connect_action(select_previous_action_);
    table_->connect_action(select_next_action_);
    table_->connect_action(select_previous_page_action_);
    table_->connect_action(select_next_page_action_);
    table_->connect_action(select_home_action_);
    table_->connect_action(select_end_action_);

    select_previous_action_.disable();
    select_next_action_.disable();
    select_previous_page_action_.disable();
    select_next_page_action_.disable();
    select_home_action_.disable();
    select_end_action_.disable();

    signal_visible_.connect(fun(this, &List_impl::scroll_to_selection));
    signal_focus_in_.connect(fun(this, &List_impl::on_focus_in), true);
}

int List_impl::prepend_row(Widget_ptr wp, Align align, bool shrink) {
    int row = --trunk_min_;
    table_->put(wp, 0, row, 1, 1, shrink, true);
    table_->set_column_margin(0, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    selectables_.emplace(row, Selectable());
    if (INT_MIN == selected_row() && focused()) { select_emit(row); }
    return row;
}

int List_impl::prepend_row(Widget_ptr wp, bool shrink) {
    return prepend_row(wp, ALIGN_START, shrink);
}

int List_impl::prepend_row(Widget_ptr wp, Align align) {
    return prepend_row(wp, align, true);
}

int List_impl::insert_row(Widget_ptr wp, int row, Align align, bool shrink) {
    if (row >= trunk_max_) { return append_row(wp, align, shrink); }
    if (row < trunk_min_) { return prepend_row(wp, align, shrink); }

    table_->insert_rows(row);
    table_->put(wp, 0, row, 1, 1, shrink, true);
    table_->set_column_margin(0, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    ++trunk_max_;

    if (!selectables_.empty()) {
        int first = selectables_.begin()->first, last = selectables_.rbegin()->first;

        if (row >= first && row <= last) {
            for (int y = last; y >= row; --y) {
                auto iter = selectables_.find(y);

                if (iter != selectables_.end()) {
                    Selectable bb(iter->second);
                    selectables_.erase(y);
                    selectables_[y+1] = bb;
                    signal_row_moved_(y, y+1);
                }
            }
        }
    }

    selectables_.emplace(row, Selectable());
    if (INT_MIN == selected_row() && focused()) { select_emit(row); }
    return row;
}

int List_impl::insert_row(Widget_ptr wp, int row, bool shrink) {
    return insert_row(wp, row, ALIGN_START, shrink);
}

int List_impl::insert_row(Widget_ptr wp, int row, Align align) {
    return insert_row(wp, row, align, true);
}

int List_impl::append_row(Widget_ptr wp, Align align, bool shrink) {
    int row = trunk_max_++;
    table_->put(wp, 0, row, 1, 1, shrink, true);
    table_->set_column_margin(0, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    selectables_.emplace(row, Selectable());
    if (INT_MIN == selected_row() && focused()) { select_emit(row); }
    return row;
}

int List_impl::append_row(Widget_ptr wp, bool shrink) {
    return append_row(wp, ALIGN_START, shrink);
}

int List_impl::append_row(Widget_ptr wp, Align align) {
    return append_row(wp, align, true);
}

int List_impl::prepend(Widget_ptr wp, Align align, bool shrink) {
    auto rng = table_->span();
    int row = --trunk_min_;
    int xx = rng.xmax > rng.xmin ? rng.xmin : 0;
    table_->put(wp, xx, row, rng.xmax > rng.xmin ? rng.xmax-rng.xmin : 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    frees_.insert(wp.get());
    return row;
}

int List_impl::prepend(Widget_ptr wp, bool shrink) {
    return prepend(wp, ALIGN_START, shrink);
}

int List_impl::prepend(Widget_ptr wp, Align align) {
    return prepend(wp, align, true);
}

int List_impl::insert(Widget_ptr wp, int row, Align align, bool shrink) {
    if (row >= trunk_max_) { return append(wp, align, shrink); }
    if (row < trunk_min_) { return prepend(wp, align, shrink); }
    auto trng = table_->span();
    auto wps = table_->children_within_range(trng);

    for (auto w: wps) {
        auto rng = table_->span(w);
        table_->respan(w, rng.xmin, rng.ymin+1, rng.xmax-rng.xmin, rng.ymax-rng.ymin);
    }

    int xx = trng.xmax > trng.xmin ? trng.xmin : 0;
    table_->put(wp, xx, row, trng.xmax > trng.xmin ? trng.xmax-trng.xmin : 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    ++trunk_max_;
    frees_.insert(wp.get());
    return row;
}

int List_impl::insert(Widget_ptr wp, int row, bool shrink) {
    return insert(wp, row, ALIGN_START, shrink);
}

int List_impl::insert(Widget_ptr wp, int row, Align align) {
    return insert(wp, row, align, true);
}

int List_impl::append(Widget_ptr wp, Align align, bool shrink) {
    auto rng = table_->span();
    int xx = rng.xmax > rng.xmin ? rng.xmin : 0;
    int row = trunk_max_++;
    table_->put(wp, xx, row, rng.xmax > rng.xmin ? rng.xmax-rng.xmin : 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    frees_.insert(wp.get());
    adjust();
    return row;
}

int List_impl::append(Widget_ptr wp, bool shrink) {
    return append(wp, ALIGN_START, shrink);
}

int List_impl::append(Widget_ptr wp, Align align) {
    return append(wp, align, true);
}

int List_impl::prepend(int row, Widget_ptr wp, Align align, bool shrink) {
    auto i = selectables_.find(row);
    if (i == selectables_.end()) { throw user_error(str_format("List_impl: bad row ", row)); }

    int xx = --i->second.min_;
    table_->put(wp, xx, row, 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    adjust();
    update_selection();
    return xx;
}

int List_impl::prepend(int row, Widget_ptr wp, bool shrink) {
    return prepend(row, wp, ALIGN_START, shrink);
}

int List_impl::prepend(int row, Widget_ptr wp, Align align) {
    return prepend(row, wp, align, true);
}

int List_impl::insert(int row, Widget_ptr wp, int xx, Align align, bool shrink) {
    auto i = selectables_.find(row);
    if (i == selectables_.end()) { throw user_error(str_format("List_impl: bad row ", row)); }

    if (xx < 0) {
        i->second.min_ = xx;
        table_->put(wp, xx, row, 1, 1, shrink, true);
        table_->set_column_margin(xx, 2, 2);
        table_->set_row_margin(row, 1, 1);
    }

    else {
        i->second.max_ = xx;
        table_->put(wp, 1+xx, row, 1, 1, shrink, true);
        table_->set_column_margin(1+xx, 2, 2);
        table_->set_row_margin(row, 1, 1);
    }

    table_->align(wp.get(), align, ALIGN_CENTER);
    adjust();
    update_selection();
    return xx;
}

int List_impl::insert(int row, Widget_ptr wp, int index, bool shrink) {
    return insert(row, wp, index, ALIGN_START, shrink);
}

int List_impl::insert(int row, Widget_ptr wp, int index, Align align) {
    return insert(row, wp, index, align, true);
}

int List_impl::append(int row, Widget_ptr wp, Align align, bool shrink) {
    auto i = selectables_.find(row);
    if (i == selectables_.end()) { throw user_error(str_format("List_impl: bad row ", row)); }

    int xx = ++i->second.max_;
    table_->put(wp, xx, row, 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(row, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    adjust();
    update_selection();
    return xx;
}

int List_impl::append(int row, Widget_ptr wp, bool shrink) {
    return append(row, wp, ALIGN_START, shrink);
}

int List_impl::append(int row, Widget_ptr wp, Align align) {
    return append(row, wp, align, true);
}

void List_impl::remove(int yy) {
    auto i = selectables_.find(yy);

    if (selectables_.end() != i) {
        signal_row_removed_(yy);
        int last = selectables_.rbegin()->first;

        // Move rows up.
        if (yy < last) {
            for (int y = yy+1; y <= last; ++y) {
                auto iter = selectables_.find(y);

                if (iter != selectables_.end()) {
                    auto b = iter->second;
                    selectables_[y-1] = b;
                    signal_row_moved_(y, y-1);
                }
            }
        }

        table_->remove_rows(yy);
        selectables_.erase(last);
    }

    auto spn = table_->span();
    spn.ymin = yy, spn.ymax = 1+yy;
    auto v = table_->children_within_range(spn);
    for (auto wp: v) { frees_.erase(wp); }
    table_->remove(spn.xmin, spn.ymin, spn.xmax-spn.xmin, spn.ymax-spn.ymin);
    adjust();
}

// Overrides Table_impl.
void List_impl::remove(Widget_impl * wp) {
    table_->remove(wp);
}

int List_impl::next_row() {
    auto sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (selectables_.size() > 1) {
            auto iter = selectables_.find(sel.ymin);

            if (selectables_.end() != iter) {
                if (++iter != selectables_.end()) {
                    return iter->first;
                }
            }
        }
    }

    else {
        if (!selectables_.empty()) {
            return selectables_.begin()->first;
        }
    }

    return INT_MIN;
}

int List_impl::prev_row() {
    auto sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (selectables_.size() > 1) {
            auto iter = std::find_if(selectables_.rbegin(), selectables_.rend(), [sel](auto & p) { return p.first == sel.ymin; });

            if (selectables_.rend() != iter) {
                if (++iter != selectables_.rend()) {
                    return iter->first;
                }
            }
        }
    }

    else {
        if (!selectables_.empty()) {
            return selectables_.begin()->first;
        }
    }

    return INT_MIN;
}

int List_impl::select_next() {
    if (!selectables_.empty()) {
        table_->unmark_all();
        return select(next_row());
    }

    return INT_MIN;
}

int List_impl::select_previous() {
    if (!selectables_.empty()) {
        table_->unmark_all();
        return select(prev_row());
    }

    return INT_MIN;
}

int List_impl::select_front(bool emit) {
    if (!selectables_.empty()) {
        table_->unmark_all();
        int row = selectables_.begin()->first;
        return emit ? select_emit(row) : select(row);
    }

    return INT_MIN;
}

int List_impl::select_back(bool emit) {
    if (!selectables_.empty()) {
        table_->unmark_all();
        int row = selectables_.rbegin()->first;
        return emit ? select_emit(row) : select(row);
    }

    return INT_MIN;
}

int List_impl::select(int row) {
    if (selectables_.end() != selectables_.find(row)) {
        auto sel = table_->selection();
        if (sel.ymax > sel.ymin && row == sel.ymin) { return row; }
        table_->unmark_row(row);
        table_->select_row(row);
        scroll_to_selection();
        return row;
    }

    return INT_MIN;
}

int List_impl::select_emit(int row) {
    int res = select(row);
    if (INT_MIN != res) { signal_row_selected_(res); }
    return res;
}

void List_impl::unselect() {
    table_->unmark_all();
    table_->unselect();
}

void List_impl::unselect_emit() {
    if (table_->has_selection() || table_->has_marks()) {
        unselect();
    }
}

void List_impl::scroll_to_selection() {
    Table::Span sel = table_->selection();
    Rect va = table_->visible_area();

    if (sel.ymax > sel.ymin) {
        if (Rect r = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            if (r.bottom() > va.bottom()) { table_->scroll_to_y(1+r.bottom()-va.height()); }
            else if (r.top() < va.top()) { table_->scroll_to_y(r.top()); }
        }
    }
}

int List_impl::selected_row() const {
    Table::Span sel = table_->selection();
    return sel.ymax > sel.ymin ? sel.ymin : INT_MIN;
}

void List_impl::update_selection() {
    table_->select_row(selected_row());
}

void List_impl::activate_current() {
    Table::Span sel = table_->selection();
    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) { signal_row_activated_(sel.ymin); }
}

bool List_impl::empty() const {
    return selectables_.empty();
}

// Overrides Table_impl.
// Overriden by List_text_impl.
void List_impl::clear() {
    unselect();
    table_->clear();
    trunk_min_ = trunk_max_ = 0;
    selectables_.clear();
}

void List_impl::allow_multiple_select() {
    multiple_select_allowed_ = true;
    select_previous_action_.enable();
    select_next_action_.enable();
    select_previous_page_action_.enable();
    select_next_page_action_.enable();
    select_home_action_.enable();
    select_end_action_.enable();
}

void List_impl::disallow_multiple_select() {
    multiple_select_allowed_ = false;
    select_previous_action_.disable();
    select_next_action_.disable();
    select_previous_page_action_.disable();
    select_next_page_action_.disable();
    select_home_action_.disable();
    select_end_action_.disable();
}

void List_impl::align(Widget_ptr wp, Align xalign, Align yalign) {
    table_->align(wp.get(), xalign, yalign);
}

bool List_impl::row_marked(int row) {
    for (auto & rng: table_->marks()) {
        if (rng.ymin == row) {
            return true;
        }
    }

    return false;
}

void List_impl::on_enter() {
    activate_current();
}

void List_impl::show_headers() {
    if (hdrbox_->hidden()) {
        hdrbox_->show();
        arrange_headers();
    }
}

void List_impl::hide_headers() {
    hdrbox_->hide();
}

bool List_impl::headers_visible() const {
    return !hdrbox_->hidden();
}

void List_impl::show_header(int column) {
    show_header(column, str_format("Column ", column));
}

void List_impl::show_header(int column, const ustring & title, Align align) {
    show_header(column, std::make_shared<Text_impl>(title, align, ALIGN_CENTER));
}

void List_impl::show_header(int column, Widget_ptr title) {
    for (auto & hdr: headers_) {
        if (column == hdr.column) {
            return;
        }
    }

    if (title->margin_left_hint() < 2) { title->hint_margin_left(2); }

    headers_.emplace_back();
    Header & hdr = headers_.back();
    hdr.column = column;
    hdr.title = title;
    hdr.box = std::make_shared<Box_impl>(OR_RIGHT);
    hdr.box->signal_mouse_down().connect(tau::bind(fun(this, &List_impl::on_header_mouse_down), column));
    hdr.box->append(hdr.title);
    hdr.sep = std::make_shared<Separator_impl>(SEPARATOR_HANDLE);
    hdr.sep->signal_mouse_down().connect(tau::bind(fun(this, &List_impl::on_sep_mouse_down), std::ref(hdr)));
    hdr.sep->signal_mouse_up().connect(tau::bind(fun(this, &List_impl::on_sep_mouse_up), std::ref(hdr)));
    hdr.sep->signal_mouse_motion().connect(tau::bind(fun(this, &List_impl::on_sep_mouse_motion), std::ref(hdr)));
    hdr.sep->set_cursor("size_hor");
    hdr.box->append(hdr.sep, true);

    hdrbox_->put(hdr.box, 0, 0);
    hdrbox_->show();
    arrange_headers();
}

void List_impl::hide_header(int column) {
    for (auto i = headers_.begin(); i != headers_.end(); ++i) {
        if (column == i->column) {
            hdrbox_->remove(i->box.get());
            headers_.erase(i);
            if (hdrbox_->empty()) { hdrbox_->hide(); }
            else { arrange_headers(); }
            return;
        }
    }
}

void List_impl::show_sort_marker(int column, bool descend) {
    hide_sort_marker();
    auto i = std::find_if(headers_.begin(), headers_.end(), [column](Header & hdr) { return hdr.column == column; } );

    if (i != headers_.end()) {
        ustring picto_name = descend ? ICON_PICTO_UP : ICON_PICTO_DOWN;

        if (!i->marker) {
            i->marker = std::make_shared<Icon_impl>(picto_name, 12);
            i->marker->hint_margin(2, 4, 0, 0);
            i->box->insert_after(i->marker, i->title.get(), true);
        }

        else {
            std::static_pointer_cast<Icon_impl>(i->marker)->assign(picto_name, 12);
        }

        return;
    }
}

void List_impl::hide_sort_marker() {
    for (auto & hdr: headers_) {
        if (hdr.marker) {
            hdr.box->remove(hdr.marker.get());
            hdr.marker.reset();
            return;
        }
    }
}

void List_impl::sync_scrollers_offset() {
    Point ofs = scroller_->pan();
    hdr_scroller_->pan_to(ofs.x(), 0);
}

void List_impl::adjust() {
    Span rng = table_->span();

    if (rng.xmax > rng.xmin) {
        for (auto wp: frees_) {
            Span spn = table_->span(wp);
            table_->respan(wp, rng.xmin, spn.ymin, rng.xmax-rng.xmin, 1);
        }
    }
}

void List_impl::arrange_headers() {
    if (!hdrbox_->hidden()) {
        unsigned req_size = 0;

        for (auto & hdr: headers_) {
            if (table_->bounds(hdr.column, 0, 1, 1)) {
                req_size = std::max(req_size, hdr.box->required_size().height());
            }
        }

        for (auto & hdr: headers_) {
            int xmin, xmax;

            table_->get_column_bounds(hdr.column, xmin, xmax);

            if (xmax > xmin) {
                hdr.box->show();
                hdrbox_->move(hdr.box.get(), xmin, 0);
                hdrbox_->resize(hdr.box.get(), xmax-xmin, req_size);
            }

            else {
                hdr.box->hide();
            }
        }
    }
}

void List_impl::set_column_spacing(unsigned spacing) {
    table_->set_column_spacing(spacing);
}

void List_impl::set_row_spacing(unsigned spacing) {
    table_->set_row_spacing(spacing);
}

unsigned List_impl::column_spacing() const {
    return table_->column_spacing();
}

unsigned List_impl::row_spacing() const {
    return table_->row_spacing();
}

void List_impl::set_column_width(int column, unsigned width) {
    table_->set_column_width(column, width);
}

unsigned List_impl::column_width(int column) const {
    return table_->column_width(column);
}

void List_impl::set_row_height(int row, unsigned height) {
    table_->set_row_height(row, height);
}

unsigned List_impl::row_height(int row) const {
    return table_->row_height(row);
}

void List_impl::set_min_column_width(int column, unsigned width) {
    table_->set_min_column_width(column, width);
}

unsigned List_impl::min_column_width(int column) const {
    return table_->min_column_width(column);
}

void List_impl::set_min_row_height(int row, unsigned height) {
    table_->set_min_row_height(row, height);
}

unsigned List_impl::min_row_height(int row) const {
    return table_->min_row_height(row);
}

void List_impl::set_max_column_width(int column, unsigned width) {
    table_->set_max_column_width(column, width);
}

unsigned List_impl::max_column_width(int column) const {
    return table_->max_column_width(column);
}

void List_impl::set_max_row_height(int row, unsigned height) {
    table_->set_max_row_height(row, height);
}

unsigned List_impl::max_row_height(int row) const {
    return table_->max_row_height(row);
}

void List_impl::set_column_margin(int x, unsigned left, unsigned right) {
    table_->set_column_margin(x, left, right);
}

void List_impl::get_column_margin(int x, unsigned & left, unsigned & right) const {
    table_->get_column_margin(x, left, right);
}

bool List_impl::on_sep_mouse_down(int mbt, int mm, const Point & pt, Header & hdr) {
    if (MBT_LEFT == mbt) {
        int x1, x2;
        table_->get_column_bounds(hdr.column, x1, x2);
        hdr.cw = x2-x1;
        hdr.lx = hdr.sep->to_parent(this, pt).x();
        hdr.fix = true;
        hdr.sep->grab_mouse();
        return true;
    }

    return false;
}

bool List_impl::on_sep_mouse_up(int mbt, int mm, const Point & pt, Header & hdr) {
    if (MBT_LEFT == mbt) {
        hdr.sep->ungrab_mouse();
        hdr.fix = false;
        return true;
    }

    return false;
}

void List_impl::on_sep_mouse_motion(int mm, const Point & pt, Header & hdr) {
    if (hdr.fix) {
        int dx = hdr.sep->to_parent(this, pt).x()-hdr.lx, w = hdr.cw+dx, wmin = hdr.box->required_size().width();

        if (w >= wmin) {
            table_->set_column_width(hdr.column, w);
            signal_header_width_changed_(hdr.column);
        }
    }
}

void List_impl::on_row_bounds_changed(int) {
    arrange_headers();
}

void List_impl::on_focus_in() {
    if (INT_MIN == selected_row()) { select_emit(trunk_min_); }
}

bool List_impl::on_table_mouse_down(int mbt, int mm, const Point & pt) {
    bool res = false;

    if (MBT_LEFT == mbt) {
        if (!focused()) { res = scroller_->take_focus(); }
        int cy = table_->row_at_y(pt.y());

        if (cy > INT_MIN) {
            if (MM_CONTROL & mm && multiple_select_allowed()) {
                if (row_marked(cy)) {
                    table_->unmark_row(cy);
                }

                else {
                    if (!signal_mark_validate_(cy)) {
                        table_->mark_row(cy);
                    }
                }
            }

            else {
                table_->unmark_all();
                select_emit(cy);
            }
        }
    }

    return res;
}

bool List_impl::on_table_mouse_double_click(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        activate_current();
        return true;
    }

    return false;
}

bool List_impl::on_header_mouse_down(int mbt, int mm, const Point & pt, int column) {
    if (MBT_LEFT == mbt) {
        signal_header_click_(column);
        return true;
    }

    return false;
}

int List_impl::page_down_row() {
    if (selectables_.empty()) { return INT_MIN; }
    auto rng = table_->span(), sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (Rect rsel = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            Rect va = table_->visible_area();
            Size lsize = scroller_->logical_size(), max = lsize-scroller_->size();

            if (max.height()) {
                if (selectables_.rbegin()->first != sel.ymin) {
                    int yt = rsel.top()+va.height()-rsel.height()-rsel.height();
                    auto iter = std::find_if(selectables_.begin(), selectables_.end(), [sel](auto & p) { return p.first == sel.ymin; });

                    Rect r;

                    for (; iter != selectables_.end(); ++iter) {
                        r = table_->bounds(rng.xmin, iter->first, rng.xmax-rng.xmin, 1);
                        if (r.top() >= yt) break;
                    }

                    if (iter != selectables_.end()) {
                        return iter->first;
                    }
                }
            }
        }
    }

    return selectables_.rbegin()->first;
}

void List_impl::on_page_down() {
    int sel = selected_row(), new_sel = page_down_row();
    if (new_sel != sel) { unselect_emit(); select_emit(new_sel); }
}

void List_impl::on_select_page_down() {
    int sel = selected_row(), next = page_down_row();

    if (sel != next) {
        select_emit(next);

        if (INT_MIN != sel && INT_MIN != next && next > sel) {
            for (int y = sel; y < next; ++y) {
                if (row_marked(y)) {
                    table_->unmark_row(y);
                }

                else {
                    if (!signal_mark_validate_(y)) {
                        table_->mark_row(y);
                    }
                }
            }
        }
    }
}

int List_impl::page_up_row() {
    auto rng = table_->span(), sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (Rect rsel = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            Size lsize = scroller_->logical_size(), max = lsize-scroller_->size();
            Rect r = table_->visible_area();

            if (max.height()) {
                int yt = rsel.top()-(7*r.height()/8);
                auto i = selectables_.find(sel.ymin);

                if (i != selectables_.end()) {
                    for (; i != selectables_.begin(); --i) {
                        r = table_->bounds(rng.xmin, i->first, rng.xmax-rng.xmin, 1);
                        if (r.top() <= yt) { return i->first; }
                    }
                }
            }
        }
    }

    return selectables_.begin()->first;
}

void List_impl::on_page_up() {
    int sel = selected_row(), new_sel = page_up_row();
    if (new_sel != sel) { unselect_emit(); select_emit(new_sel); }
}

void List_impl::on_select_page_up() {
    int sel = selected_row(), next = page_up_row();

    if (sel != next) {
        select_emit(next);

        if (INT_MIN != sel && INT_MIN != next && next < sel) {
            for (int y = sel; y > next; --y) {
                if (row_marked(y)) {
                    table_->unmark_row(y);
                }

                else {
                    if (!signal_mark_validate_(y)) {
                        table_->mark_row(y);
                    }
                }
            }
        }
    }
}

void List_impl::on_prev() {
    table_->unmark_all();
    select_emit(prev_row());
}

void List_impl::on_next() {
    table_->unmark_all();
    select_emit(next_row());
}

void List_impl::on_select_prev() {
    int sel = selected_row(), prev = prev_row();

    if (sel != prev) {
        bool marked = row_marked(prev);
        select_emit(prev);

        if (INT_MIN != sel && INT_MIN != prev && sel != prev && !marked) {
            if (!signal_mark_validate_(sel)) {
                table_->mark_row(sel);
            }
        }
    }
}

void List_impl::on_select_next() {
    int sel = selected_row(), next = next_row();

    if (sel != next) {
        bool marked = row_marked(next);
        select_emit(next);

        if (INT_MIN != sel && INT_MIN != next && sel != next && !marked) {
            if (!signal_mark_validate_(sel)) {
                table_->mark_row(sel);
            }
        }
    }
}

void List_impl::on_home() {
    table_->unmark_all();
    select_front(true);
}

void List_impl::on_end() {
    table_->unmark_all();
    select_back(true);
}

void List_impl::on_select_home() {
    if (!selectables_.empty()) {
        int sel = selected_row();

        if (INT_MIN != sel) {
            int first = selectables_.begin()->first;
            select_emit(first);

            if (first < sel) {
                for (int y = sel; y > first; --y) {
                    if (row_marked(y)) {
                        table_->unmark_row(y);
                    }

                    else {
                        if (!signal_mark_validate_(y)) {
                            table_->mark_row(y);
                        }
                    }
                }
            }
        }
    }
}

void List_impl::on_select_end() {
    if (!selectables_.empty()) {
        int sel = selected_row();

        if (INT_MIN != sel) {
            int last = selectables_.rbegin()->first;
            select_emit(last);

            if (last > sel) {
                for (int y = sel; y < last; ++y) {
                    if (row_marked(y)) {
                        table_->unmark_row(y);
                    }

                    else {
                        if (!signal_mark_validate_(y)) {
                            table_->mark_row(y);
                        }
                    }
                }
            }
        }
    }
}

} // namespace tau

//END
