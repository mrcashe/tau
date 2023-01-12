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

#include <tau/exception.hh>
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
    table_->style().redirect("whitespace/background", "background");
    table_->signal_mouse_down().connect(fun(this, &List_impl::on_table_mouse_down));
    table_->signal_mouse_double_click().connect(fun(this, &List_impl::on_table_mouse_double_click));
    table_->signal_size_changed().connect(fun(this, &List_impl::scroll_to_selection));
    table_->signal_size_changed().connect(fun(this, &List_impl::arrange_headers));
    table_->signal_column_bounds_changed().connect(fun(this, &List_impl::on_row_bounds_changed));

    scroller_ = std::make_shared<Scroller_impl>();
    scroller_->signal_offset_changed().connect(fun(this, &List_impl::sync_scrollers_offset));
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

    table_->connect_action(enter_action_);
    table_->connect_action(up_action_);
    table_->connect_action(down_action_);
    table_->connect_action(page_up_action_);
    table_->connect_action(page_down_action_);
    table_->connect_action(home_action_);
    table_->connect_action(end_action_);
    table_->connect_action(shift_up_action_);
    table_->connect_action(shift_down_action_);
    table_->connect_action(shift_page_up_action_);
    table_->connect_action(shift_page_down_action_);
    table_->connect_action(shift_home_action_);
    table_->connect_action(shift_end_action_);

    shift_up_action_.disable();
    shift_down_action_.disable();
    shift_page_up_action_.disable();
    shift_page_down_action_.disable();
    shift_home_action_.disable();
    shift_end_action_.disable();

    signal_visible_.connect(fun(this, &List_impl::scroll_to_selection));
    signal_focus_in_.connect(fun(this, &List_impl::on_focus_in), true);
}

int List_impl::prepend_row(Widget_ptr wp, Align align, bool shrink) {
    --trunk_min_;
    table_->put(wp, 0, trunk_min_, 1, 1, shrink, true);
    table_->set_column_margin(0, 2, 2);
    table_->set_row_margin(trunk_min_, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    selectables_[trunk_min_] = Selectable { 0, 0 };
    if (INT_MIN == selected_row() && has_focus()) { select_row(trunk_min_); }
    adjust();
    return trunk_min_;
}

int List_impl::prepend_row(Widget_ptr wp, bool shrink) {
    return prepend_row(wp, ALIGN_START, shrink);
}

int List_impl::prepend_row(Widget_ptr wp, Align align) {
    return prepend_row(wp, align, true);
}

int List_impl::insert_row(Widget_ptr wp, int position, Align align, bool shrink) {
    if (position >= trunk_max_) {
        return append_row(wp, align, shrink);
    }

    if (position < trunk_min_) {
        return prepend_row(wp, align, shrink);
    }

    table_->insert_rows(position, 1);
    table_->put(wp, 0, position, 1, 1, shrink, true);
    table_->set_column_margin(0, 2, 2);
    table_->set_row_margin(position, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);

    ++trunk_max_;

    if (!selectables_.empty()) {
        int first = selectables_.begin()->first, last = selectables_.rbegin()->first;

        if (position >= first && position <= last) {
            for (int y = last; y >= position; --y) {
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

    selectables_[position] = Selectable { 0, 0 };
    if (INT_MIN == selected_row() && has_focus()) { select_row(position); }
    adjust();
    return position;
}

int List_impl::insert_row(Widget_ptr wp, int position, bool shrink) {
    return insert_row(wp, position, ALIGN_START, shrink);
}

int List_impl::insert_row(Widget_ptr wp, int position, Align align) {
    return insert_row(wp, position, align, true);
}

int List_impl::append_row(Widget_ptr wp, Align align, bool shrink) {
    table_->put(wp, 0, trunk_max_, 1, 1, shrink, true);
    table_->set_column_margin(0, 2, 2);
    table_->set_row_margin(trunk_max_, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    int br = trunk_max_++;
    selectables_[br] = Selectable { 0, 0 };
    if (INT_MIN == selected_row() && has_focus()) { select_row(br); }
    adjust();
    return br;
}

int List_impl::append_row(Widget_ptr wp, bool shrink) {
    return append_row(wp, ALIGN_START, shrink);
}

int List_impl::append_row(Widget_ptr wp, Align align) {
    return append_row(wp, align, true);
}

int List_impl::prepend(Widget_ptr wp, Align align, bool shrink) {
    Table::Span rng = table_->span();
    --trunk_min_;
    int xx = rng.xmax > rng.xmin ? rng.xmin : 0;
    table_->put(wp, xx, trunk_min_, rng.xmax > rng.xmin ? rng.xmax-rng.xmin : 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(trunk_min_, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    frees_.push_back(trunk_min_);
    adjust();
    return trunk_min_;
}

int List_impl::prepend(Widget_ptr wp, bool shrink) {
    return prepend(wp, ALIGN_START, shrink);
}

int List_impl::prepend(Widget_ptr wp, Align align) {
    return prepend(wp, align, true);
}

int List_impl::insert(Widget_ptr wp, int position, Align align, bool shrink) {
    if (position >= trunk_max_) {
        return append(wp, align, shrink);
    }

    if (position < trunk_min_) {
        return prepend(wp, align, shrink);
    }

    Table::Span trng = table_->span();
    auto wps = table_->children_within_range(trng.xmin, position, trng.xmax, trng.ymax);

    for (auto w: wps) {
        Table::Span rng = table_->span(w);
        table_->respan(w, rng.xmin, rng.ymin+1, rng.xmax-rng.xmin, rng.ymax-rng.ymin);
    }

    int xx = trng.xmax > trng.xmin ? trng.xmin : 0;
    table_->put(wp, xx, position, trng.xmax > trng.xmin ? trng.xmax-trng.xmin : 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(position, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    ++trunk_max_;
    frees_.push_back(position);
    adjust();
    return position;
}

int List_impl::insert(Widget_ptr wp, int position, bool shrink) {
    return insert(wp, position, ALIGN_START, shrink);
}

int List_impl::insert(Widget_ptr wp, int position, Align align) {
    return insert(wp, position, align, true);
}

int List_impl::append(Widget_ptr wp, Align align, bool shrink) {
    Table::Span rng = table_->span();
    int xx = rng.xmax > rng.xmin ? rng.xmin : 0;
    table_->put(wp, xx, trunk_max_, rng.xmax > rng.xmin ? rng.xmax-rng.xmin : 1, 1, shrink, true);
    table_->set_column_margin(xx, 2, 2);
    table_->set_row_margin(trunk_max_, 1, 1);
    table_->align(wp.get(), align, ALIGN_CENTER);
    frees_.push_back(trunk_max_);
    adjust();
    return trunk_max_++;
}

int List_impl::append(Widget_ptr wp, bool shrink) {
    return append(wp, ALIGN_START, shrink);
}

int List_impl::append(Widget_ptr wp, Align align) {
    return append(wp, align, true);
}

int List_impl::prepend(int row, Widget_ptr wp, Align align, bool shrink) {
    try {
        Selectable & br = selectables_.at(row);
        int tpos = --br.min;
        table_->put(wp, tpos, row, 1, 1, shrink, true);
        table_->set_column_margin(tpos, 2, 2);
        table_->set_row_margin(row, 1, 1);
        table_->align(wp.get(), align, ALIGN_CENTER);
        adjust();
        update_selection();
        return tpos;
    }

    catch (std::out_of_range & ) {
        throw graphics_error(str_format("List_impl: bad row ", row));
    }
}

int List_impl::prepend(int row, Widget_ptr wp, bool shrink) {
    return prepend(row, wp, ALIGN_START, shrink);
}

int List_impl::prepend(int row, Widget_ptr wp, Align align) {
    return prepend(row, wp, align, true);
}

int List_impl::insert(int row, Widget_ptr wp, int index, Align align, bool shrink) {
    try {
        Selectable & br = selectables_.at(row);

        if (index < 0) {
            br.min = index;
            table_->put(wp, index, row, 1, 1, shrink, true);
            table_->set_column_margin(index, 2, 2);
            table_->set_row_margin(row, 1, 1);
        }

        else {
            br.max = 1+index;
            table_->put(wp, 1+index, row, 1, 1, shrink, true);
            table_->set_column_margin(1+index, 2, 2);
            table_->set_row_margin(row, 1, 1);
        }

        table_->align(wp.get(), align, ALIGN_CENTER);
        adjust();
        update_selection();
        return index;
    }

    catch (std::out_of_range & ) {
        throw graphics_error(str_format("List_impl: bad row ", row));
    }
}

int List_impl::insert(int row, Widget_ptr wp, int index, bool shrink) {
    return insert(row, wp, index, ALIGN_START, shrink);
}

int List_impl::insert(int row, Widget_ptr wp, int index, Align align) {
    return insert(row, wp, index, align, true);
}

int List_impl::append(int row, Widget_ptr wp, Align align, bool shrink) {
    try {
        Selectable & br = selectables_.at(row);
        int index = ++br.max;
        table_->put(wp, br.max, row, 1, 1, shrink, true);
        table_->set_column_margin(br.max, 2, 2);
        table_->set_row_margin(row, 1, 1);
        table_->align(wp.get(), align, ALIGN_CENTER);
        adjust();
        update_selection();
        return index;
    }

    catch (std::out_of_range & ) {
        throw graphics_error(str_format("List_impl: bad row ", row));
    }
}

int List_impl::append(int row, Widget_ptr wp, bool shrink) {
    return append(row, wp, ALIGN_START, shrink);
}

int List_impl::append(int row, Widget_ptr wp, Align align) {
    return append(row, wp, align, true);
}

void List_impl::remove(int yy) {
    auto i = selectables_.find(yy);
    table_->remove_rows(yy, 1);

    if (selectables_.end() != i) {
        signal_row_removed_(yy);
        int last = selectables_.rbegin()->first;

        // Move branches up.
        if (yy < last) {
            for (int y = yy+1; y <= last; ++y) {
                auto iter = selectables_.find(y);

                if (iter != selectables_.end()) {
                    Selectable b = iter->second;
                    selectables_[y-1] = b;
                    signal_row_moved_(y, y-1);
                }
            }
        }

        selectables_.erase(last);
    }

    else {
        frees_.remove(yy);
    }

    adjust();
}

int  List_impl::page_down_row() {
    if (selectables_.empty()) { return INT_MIN; }

    Table::Span rng = table_->span();
    Table::Span sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (Rect rsel = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            Rect va = table_->visible_area();
            Size lsize = scroller_->logical_size(), max = lsize-scroller_->size();

            if (max.height()) {
                if (selectables_.rbegin()->first != sel.ymin) {
                    int yt = rsel.top()+va.height()-rsel.height()-rsel.height();
                    auto iter = std::find_if(selectables_.begin(), selectables_.end(),
                                             [sel](const std::pair<const int, Selectable> & p) { return p.first == sel.ymin; });

                    Rect rbr;

                    for (; iter != selectables_.end(); ++iter) {
                        rbr = table_->bounds(rng.xmin, iter->first, rng.xmax-rng.xmin, 1);
                        if (rbr.top() >= yt) break;
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

void List_impl::page_down() {
    if (selectables_.empty()) { return; }

    Table::Span rng = table_->span();
    Table::Span sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (Rect rsel = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            Rect va = table_->visible_area();
            Size lsize = scroller_->logical_size(), max = lsize-scroller_->size();

            if (max.height()) {
                if (selectables_.rbegin()->first != sel.ymin) {
                    int yt = rsel.top()+va.height()-rsel.height()-rsel.height();
                    auto iter = std::find_if(selectables_.begin(), selectables_.end(),
                                                [sel](const std::pair<const int, Selectable> & p) { return p.first == sel.ymin; });

                    Rect rbr;
                    for (; iter != selectables_.end(); ++iter) {
                        rbr = table_->bounds(rng.xmin, iter->first, rng.xmax-rng.xmin, 1);
                        if (rbr.top() >= yt) break;
                    }

                    if (iter != selectables_.end()) {
                        int y;

                        if (rsel.bottom() >= va.bottom()) { y = rbr.bottom()-va.height(); }
                        else if (rsel.top() < va.top()) { y = rbr.top(); }
                        else { y = rbr.top()-rsel.top()+va.top(); }

                        table_->scroll_to_y(y);
                        table_->select_row(iter->first);
                        signal_row_selected_(iter->first);
                        return;
                    }
                }
            }
        }
    }

    select_last();
}

int  List_impl::page_up_row() {
    if (selectables_.empty()) { return INT_MIN; }

    Table::Span rng = table_->span();
    Table::Span sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (Rect rsel = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            Rect va = table_->visible_area();
            Size lsize = scroller_->logical_size(), max = lsize-scroller_->size();

            if (max.height()) {
                if (selectables_.begin()->first != sel.ymin) {
                    int yt = rsel.top()-va.height()+rsel.height()+rsel.height();
                    auto iter = std::find_if(selectables_.rbegin(), selectables_.rend(),
                                                [sel](const std::pair<const int, Selectable> & p) { return p.first == sel.ymin; });

                    Rect rbr;
                    for (; iter != selectables_.rend(); ++iter) {
                        rbr = table_->bounds(rng.xmin, iter->first, rng.xmax-rng.xmin, 1);
                        if (rbr.top() <= yt) break;
                    }

                    if (iter != selectables_.rend()) {
                        return iter->first;
                    }
                }
            }
        }
    }

    return selectables_.begin()->first;
}

void List_impl::page_up() {
    if (selectables_.empty()) { return; }

    Table::Span rng = table_->span();
    Table::Span sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (Rect rsel = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            Rect va = table_->visible_area();
            Size lsize = scroller_->logical_size(), max = lsize-scroller_->size();

            if (max.height()) {
                if (selectables_.begin()->first != sel.ymin) {
                    int yt = rsel.top()-va.height()+rsel.height()+rsel.height();
                    auto iter = std::find_if(selectables_.rbegin(), selectables_.rend(),
                                                [sel](const std::pair<const int, Selectable> & p) { return p.first == sel.ymin; });

                    Rect rbr;
                    for (; iter != selectables_.rend(); ++iter) {
                        rbr = table_->bounds(rng.xmin, iter->first, rng.xmax-rng.xmin, 1);
                        if (rbr.top() <= yt) break;
                    }

                    if (iter != selectables_.rend()) {
                        int y;

                        if (rsel.bottom() >= va.bottom()) { y = rbr.bottom()-va.height(); }
                        else if (rsel.top() < va.top()) { y = rbr.top(); }
                        else { y = rbr.top()-rsel.top()+va.top(); }

                        table_->scroll_to_y(y);
                        table_->select_row(iter->first);
                        signal_row_selected_(iter->first);
                        return;
                    }
                }
            }
        }
    }

    select_first();
}

int List_impl::next_row() {
    Table::Span sel = table_->selection();

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
    Table::Span sel = table_->selection();

    if (sel.xmax > sel.xmin && sel.ymax > sel.ymin) {
        if (selectables_.size() > 1) {
            auto iter = std::find_if(selectables_.rbegin(), selectables_.rend(),
                                     [sel](const std::pair<const int, Selectable> & p) { return p.first == sel.ymin; });

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

int List_impl::select_first() {
    if (!selectables_.empty()) {
        return select_row(selectables_.begin()->first);
    }

    return INT_MIN;
}

int List_impl::select_last() {
    if (!selectables_.empty()) {
        return select_row(selectables_.rbegin()->first);
    }

    return INT_MIN;
}

int List_impl::select_row(int br) {
    if (selectables_.end() != selectables_.find(br)) {
        Table::Span sel = table_->selection();
        if (sel.ymax > sel.ymin && br == sel.ymin) { return br; }
        table_->unmark_row(br);
        table_->select_row(br);
        signal_row_selected_(br);
        scroll_to_selection();
        return br;
    }

    return INT_MIN;
}

void List_impl::unselect() {
    table_->unmark_all();
    table_->unselect();
}

void List_impl::scroll_to_selection() {
    Table::Span sel = table_->selection();
    Rect va = table_->visible_area();

    if (sel.ymax > sel.ymin) {
        if (Rect r = table_->bounds(sel.xmin, sel.ymin, sel.xmax-sel.xmin, sel.ymax-sel.ymin)) {
            if (r.bottom() > va.bottom()) {
                table_->scroll_to_y(1+r.bottom()-va.height());
            }

            else if (r.top() < va.top()) {
                table_->scroll_to_y(r.top());
            }
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
    table_->clear();
    trunk_min_ = trunk_max_ = 0;
    selectables_.clear();
}

void List_impl::allow_multiple_select() {
    multiple_select_allowed_ = true;
    shift_up_action_.enable();
    shift_down_action_.enable();
    shift_page_up_action_.enable();
    shift_page_down_action_.enable();
    shift_home_action_.enable();
    shift_end_action_.enable();
}

void List_impl::disallow_multiple_select() {
    multiple_select_allowed_ = false;
    shift_up_action_.disable();
    shift_down_action_.disable();
    shift_page_up_action_.disable();
    shift_page_down_action_.disable();
    shift_home_action_.disable();
    shift_end_action_.disable();
}

void List_impl::align(Widget_ptr wp, Align xalign, Align yalign) {
    table_->align(wp.get(), xalign, yalign);
}

bool List_impl::row_marked(int row) {
    std::vector<Table::Span> v = table_->marks();

    for (Table::Span & rng: v) {
        if (rng.ymin == row) {
            return true;
        }
    }

    return false;
}

bool List_impl::on_table_mouse_down(int mbt, int mm, const Point & pt) {
    bool res = false;

    if (MBT_LEFT == mbt) {
        if (!has_focus()) { res = scroller_->take_focus(); }

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
                select_row(cy);
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

void List_impl::on_prev_key() {
    table_->unmark_all();
    select_row(prev_row());
}

void List_impl::on_next_key() {
    table_->unmark_all();
    select_row(next_row());
}

void List_impl::on_shift_prev_key() {
    int sel = selected_row(), prev = prev_row();
    bool marked = row_marked(prev);
    select_row(prev);

    if (INT_MIN != sel && INT_MIN != prev && sel != prev && !marked) {
        if (!signal_mark_validate_(sel)) {
            table_->mark_row(sel);
        }
    }
}

void List_impl::on_shift_next_key() {
    int sel = selected_row(), next = next_row();
    bool marked = row_marked(next);
    select_row(next);

    if (INT_MIN != sel && INT_MIN != next && sel != next && !marked) {
        if (!signal_mark_validate_(sel)) {
            table_->mark_row(sel);
        }
    }
}

void List_impl::on_home_key() {
    table_->unmark_all();
    select_first();
}

void List_impl::on_end_key() {
    table_->unmark_all();
    select_last();
}

void List_impl::on_page_down_key() {
    table_->unmark_all();
    page_down();
}

void List_impl::on_page_up_key() {
    table_->unmark_all();
    page_up();
}

void List_impl::on_shift_page_up_key() {
    int sel = selected_row(), next = page_up_row();
    select_row(next);

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

void List_impl::on_shift_page_down_key() {
    int sel = selected_row(), next = page_down_row();
    select_row(next);

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

void List_impl::on_shift_home_key() {
    if (!selectables_.empty()) {
        int sel = selected_row();

        if (INT_MIN != sel) {
            int first = selectables_.begin()->first;
            select_row(first);

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

void List_impl::on_shift_end_key() {
    if (!selectables_.empty()) {
        int sel = selected_row();

        if (INT_MIN != sel) {
            int last = selectables_.rbegin()->first;
            select_row(last);

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

void List_impl::on_enter_key() {
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
    hdr.sep = std::make_shared<Separator_impl>(SEPARATOR_SOLID);
    hdr.sep->hint_size(Size(2, 0));
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
        ustring picto_name = descend ? "picto-up" : "picto-down";

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

void List_impl::on_row_bounds_changed(int br) {
    arrange_headers();
}

void List_impl::sync_scrollers_offset() {
    Point ofs = scroller_->offset();
    hdr_scroller_->pan_to(ofs.x(), 0);
}

void List_impl::adjust() {
    Table::Span rng = table_->span();

    if (rng.xmax > rng.xmin) {
        for (int y: frees_) {
            auto v = table_->children_within_range(rng.xmin, y, rng.xmax, y+1);
            if (1 == v.size()) { table_->respan(v[0], rng.xmin, y, rng.xmax-rng.xmin, 1); }
        }
    }
}

void List_impl::on_focus_in() {
    update_selection();
}

} // namespace tau

//END
