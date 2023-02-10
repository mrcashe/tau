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

#include <tau/brush.hh>
#include <tau/loop.hh>
#include <tau/painter.hh>
#include <tau/string.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <table-impl.hh>
#include <window-impl.hh>
#include <iostream>

namespace tau {

Table_impl::Table_impl():
    Container_impl()
{
    init();
}

Table_impl::Table_impl(unsigned xspacing, unsigned yspacing):
    Container_impl(),
    xspacing_(xspacing),
    yspacing_(yspacing)
{
    init();
}

Table_impl::Table_impl(unsigned spacing):
    Container_impl(),
    xspacing_(spacing),
    yspacing_(spacing)
{
    init();
}

Table_impl::~Table_impl() {
    signal_destroy_();
    if (signal_column_bounds_changed_) { delete signal_column_bounds_changed_; }
    if (signal_row_bounds_changed_) { delete signal_row_bounds_changed_; }
    if (signal_selection_changed_) { delete signal_selection_changed_; }
}

void Table_impl::init() {
    signal_arrange_.connect(fun(this, &Table_impl::arrange));
    signal_size_changed_.connect(fun(this, &Table_impl::update_requisition));
    signal_size_changed_.connect(fun(this, &Table_impl::arrange));
    signal_visible_.connect(fun(this, &Table_impl::arrange));
    signal_display_.connect(fun(this, &Table_impl::recalc));
    signal_backpaint_.connect(fun(this, &Table_impl::on_backpaint));
    signal_take_focus_.connect(fun(this, &Table_impl::on_take_focus));
    signal_child_requisition_.connect(fun(this, &Table_impl::on_child_requisition));
    signal_child_hints_.connect(fun(this, &Table_impl::on_child_requisition));
    signal_child_show_.connect(fun(this, &Table_impl::on_child_show));
    signal_child_hide_.connect(fun(this, &Table_impl::on_child_hide));
}

void Table_impl::put(Widget_ptr wp, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh) {
    if (wp) {
        make_child(wp);
        update_child_bounds(wp.get(), INT_MIN, INT_MIN, Size());
        auto p = holders_.emplace(wp.get(), Holder());
        Holder & hol = p.first->second;
        hol.wp_ = wp.get();
        hol.xmin_ = x;
        hol.ymin_ = y;
        hol.xmax_ = x+std::max(1U, xspan);
        hol.ymax_ = y+std::max(1U, yspan);
        hol.xsh_ = 1 == hol.xmax_-hol.xmin_ && xsh;
        hol.ysh_ = 1 == hol.ymax_-hol.ymin_ && ysh;
        dist_holder(hol);
    }
}

void Table_impl::remove(Widget_impl * wp) {
    if (wp) {
        auto i = holders_.find(wp);

        if (i != holders_.end()) {
            wipe_holder(i->second);
            unparent_child(wp);
            holders_.erase(i);
        }
    }
}

// Overriden by List_impl.
// Overriden by List_text_impl.
void Table_impl::clear() {
    unmark_all();
    unselect();
    unparent_all();
    holders_.clear();
    nvcol_ = nvrow_ = nucol_ = nurow_ = nshcol_ = nshrow_ = xspc_ = yspc_ = 0;

    if (!cols_.empty()) {
        int xx = cols_.begin()->first, xm = cols_.rbegin()->first;

        for (; xx <= xm; ++xx) {
            auto i = cols_.find(xx);
            if (i != cols_.end()) { erase(i); }
        }
    }

    if (!rows_.empty()) {
        int yy = rows_.begin()->first, ym = rows_.rbegin()->first;

        for (; yy <= ym; ++yy) {
            auto j = rows_.find(yy);
            if (j != rows_.end()) { erase(j); }
        }
    }

    rearrange1();
    invalidate();
}

// Both hints & requisitions come here.
void Table_impl::on_child_requisition(Widget_impl * wi) {
    if (!shut_) {
        rearrange1();
    }
}

void Table_impl::on_child_show(Widget_impl * wi) {
    if (!shut_) {
        auto i = holders_.find(wi);

        if (i != holders_.end()) {
            Holder & hol = i->second;

            for (int xx = hol.xmin_; xx < hol.xmax_; ++xx) {
                Col & col = cols_[xx];
                change_visibility(col, true, hol.xsh_);
            }

            for (int yy = hol.ymin_; yy < hol.ymax_; ++yy) {
                Row & row = rows_[yy];
                change_visibility(row, true, hol.ysh_);
            }

            rearrange1();
        }
    }
}

void Table_impl::on_child_hide(Widget_impl * wi) {
    if (!shut_) {
        auto i = holders_.find(wi);

        if (i != holders_.end()) {
            wi->update_origin(INT_MIN, INT_MIN);
            wi->update_size(0, 0);
            Holder & hol = i->second;

            for (int xx = hol.xmin_; xx < hol.xmax_; ++xx) {
                Col & col = cols_[xx];
                change_visibility(col, false, hol.xsh_);
            }

            for (int yy = hol.ymin_; yy < hol.ymax_; ++yy) {
                Row & row = rows_[yy];
                change_visibility(row, false, hol.ysh_);
            }

            rearrange1();
        }
    }
}

void Table_impl::alloc_child(Holder & hol) {
    hol.wmax_ = hol.hmax_ = hol.wmin_ = hol.hmin_ = 0;
    if (hol.wp_->hidden()) { return; }

    Size req = hol.wp_->required_size(), min = hol.wp_->min_size_hint();
    req.update(hol.wp_->size_hint(), true);
    req.update_max(min);
    req.update_min(hol.wp_->max_size_hint(), true);
    req.increase(hol.wp_->margin_hint());

    hol.wmin_ = min.width();
    hol.hmin_ = min.height();

    hol.wmax_ = req.width();
    hol.hmax_ = req.height();

    // --------------------------------------------------------------------------------
    // X Axis.
    // --------------------------------------------------------------------------------

    auto cb = cols_.find(hol.xmin_), ce = cols_.end();
    unsigned nc = hol.xmax_-hol.xmin_;
    unsigned spc = 0;

    if (1 == nc) {
        cb->second.rmax_ = std::max(cb->second.rmax_, hol.wmax_);
        cb->second.rmin_ = std::max(cb->second.rmin_, hol.wmin_);
    }

    else {
        for (auto i(cb); i != ce && i->first < hol.xmax_; ++i) {
            if (i != cb) { spc += i->second.left_+xspacing_; }
            if ((hol.xmax_-i->first) > 1) { spc += i->second.right_; }
            if (i->second.shrank_) { nc = std::max(1U, nc-1); }
        }

        unsigned wmax = hol.wmax_ > spc ? hol.wmax_-spc : 0;  // maximal width.
        unsigned wmin = hol.wmin_ > spc ? hol.wmin_-spc : 0;  // minimal width.

        // Calculate width per cell and remainder per cell.
        unsigned wpc_max = wmax/nc;
        unsigned rem_max = wmax%nc;
        unsigned wpc_min = wmin/nc;
        unsigned rem_min = wmin%nc;

        for (auto i(cb); i != ce && i->first < hol.xmax_; ++i) {
            if (!i->second.shrank_) {
                unsigned w1 = wpc_max, w2 = wpc_min;
                if (w1 && rem_max) { ++w1, --rem_max; }
                if (w2 && rem_min) { ++w2, --rem_min; }
                i->second.rmax_ = std::max(i->second.rmax_, w1);
                i->second.rmin_ = std::max(i->second.rmin_, w2);
            }
        }
    }

    // --------------------------------------------------------------------------------
    // Y Axis.
    // --------------------------------------------------------------------------------

    auto rb = rows_.find(hol.ymin_), re = rows_.end();
    nc = hol.ymax_-hol.ymin_;
    spc = 0;

    if (1 == nc) {
        rb->second.rmax_ = std::max(rb->second.rmax_, hol.hmax_);
        rb->second.rmin_ = std::max(rb->second.rmin_, hol.hmin_);
    }

    else {
        for (auto i(rb); i != re && i->first < hol.ymax_; ++i) {
            if (i != rb) { spc += i->second.top_+yspacing_; }
            if ((hol.ymax_-i->first) > 1) { spc += i->second.bottom_; }
            if (i->second.shrank_) { nc = std::max(1U, nc-1); }
        }

        unsigned hmax = hol.hmax_ > spc ? hol.hmax_-spc : 0;
        unsigned hmin = hol.hmin_ > spc ? hol.hmin_-spc : 0;

        // Calculate height per cell and remainder per cell.
        unsigned hpc_max = hmax/nc;
        unsigned rem_max = hmax%nc;
        unsigned hpc_min = hmin/nc;
        unsigned rem_min = hmin%nc;

        for (auto i(rb); i != re && i->first < hol.ymax_; ++i) {
            if (!i->second.shrank_) {
                unsigned h1 = hpc_max, h2 = hpc_min;
                if (h1 && rem_max) { ++h1; --rem_max; }
                if (h2 && rem_min) { ++h2; --rem_min; }
                i->second.rmax_ = std::max(i->second.rmax_, h1);
                i->second.rmin_ = std::max(i->second.rmin_, h2);
            }
        }
    }
}

void Table_impl::recalc() {
    for (auto & p: cols_) { p.second.rmin_ = p.second.rmax_ = 0; }
    for (auto & p: rows_) { p.second.rmin_ = p.second.rmax_ = 0; }

    for (auto & p: holders_) {
        if (!p.second.wp_->hidden()) {
            alloc_child(p.second);
        }
    }

    update_requisition();
}

void Table_impl::arrange() {
    alloc_cols();
    alloc_rows();
    place_widgets();
}

void Table_impl::rearrange1() {
    recalc();
    queue_arrange();
}

void Table_impl::rearrange2() {
    update_requisition();
    queue_arrange();
}

void Table_impl::update_requisition() {
    rucol_ = rshcol_ = rurow_ = rshrow_ = 0;
    unsigned rx = 0, ry = 0, px;

    for (auto & p: cols_) {
        if (p.second.visible_) {
            if (p.second.usize_) {
                px = p.second.usize_;
                px = std::max(px, p.second.umin_);
                if (p.second.umax_) { px = std::min(px, p.second.umax_); }
                rucol_ += px;
            }

            else {
                px = std::max(p.second.rmin_, p.second.rmax_);
                px = std::max(px, p.second.umin_);
                if (p.second.umax_) { px = std::min(px, p.second.umax_); }
                if (p.second.shrank_) { rshcol_ += px; }
            }

            rx += px;
        }
    }

    for (auto & p: rows_) {
        if (p.second.visible_) {
            if (p.second.usize_) {
                px = p.second.usize_;
                px = std::max(px, p.second.umin_);
                if (p.second.umax_) { px = std::min(px, p.second.umax_); }
                rurow_ += px;
            }

            else {
                px = std::max(p.second.rmin_, p.second.rmax_);
                px = std::max(px, p.second.umin_);
                if (p.second.umax_) { px = std::min(px, p.second.umax_); }
                if (p.second.shrank_) { rshrow_ += px; }
            }

            ry += px;
        }
    }

    unsigned avail = size().width();
    unsigned req = xspc_+rucol_+rshcol_;
    avail = avail > req ? avail-req : 0;
    unsigned nfree = nvcol_-nucol_-nshcol_;
    unsigned nextra = nfree ? nfree : nshcol_;
    xextra_ = nextra ? avail/nextra : 0;
    xrem_ = nextra ? avail%nextra : 0;

    avail = size().height();
    req = yspc_+rurow_+rshrow_;
    avail = avail > req ? avail-req : 0;
    nfree = nvrow_-nurow_-nshrow_;
    nextra = nfree ? nfree : nshrow_;
    yextra_ = nextra ? avail/nextra : 0;
    yrem_ = nextra ? avail%nextra : 0;

    require_size(xspc_+rx, yspc_+ry);
}

void Table_impl::alloc_cols() {
    unsigned rem = xrem_;
    int x = 0;
    auto ce = cols_.end();

    for (auto i(cols_.begin()); i != ce; ++i) {
        if (i->second.visible_) {
            x += i->second.left_;
            i->second.x_ = x;
            unsigned px;

            if (i->second.usize_) {
                px = i->second.usize_;
                px = std::max(px, i->second.umin_);
                if (i->second.umax_) { px = std::min(px, i->second.umax_); }
            }

            else if (i->second.shrank_) {
                px = std::max(i->second.rmin_, i->second.rmax_);
                unsigned nfree = nvcol_-nucol_-nshcol_;

                if (!nfree && !rucol_ && (i->second.fill_ || (i->second.align_set_ && ALIGN_FILL == i->second.xalign_) || ALIGN_FILL == xalign_)) {
                    px += xextra_;
                    if (rem) { ++px, --rem; }
                }

                px = std::max(px, i->second.umin_);
                if (i->second.umax_) { px = std::min(px, i->second.umax_); }
            }

            else {
                px = std::max(xextra_, i->second.rmin_);
                if (rem) { ++px, --rem; }
                if (!px) { px = i->second.rmin_ ? i->second.rmin_ : i->second.rmax_; }
                px = std::max(px, i->second.umin_);
                if (i->second.umax_) { px = std::min(px, i->second.umax_); }
            }

            x += px;
            i->second.w_ = x-i->second.x_;
            x += i->second.right_+xspacing_;

            if (i->second.lx_ != i->second.x_ || i->second.lw_  != i->second.w_) {
                i->second.lx_ = i->second.x_;
                i->second.lw_ = i->second.w_;
                if (!shut_ && signal_column_bounds_changed_) { (*signal_column_bounds_changed_)(i->first); }
            }
        }
    }
}

void Table_impl::alloc_rows() {
    unsigned rem = yrem_;
    int y = 0;
    auto re = rows_.end();

    for (auto i(rows_.begin()); i != re; ++i) {
        if (i->second.visible_) {
            y += i->second.top_;
            i->second.y_ = y;
            unsigned px;

            if (i->second.usize_) {
                px = i->second.usize_;
                px = std::max(px, i->second.umin_);
                if (i->second.umax_) { px = std::min(px, i->second.umax_); }
            }

            else if (i->second.shrank_) {
                px = std::max(i->second.rmin_, i->second.rmax_);
                unsigned nfree = nvrow_-nurow_-nshrow_;

                if (!nfree && !rurow_ && (i->second.fill_ || (i->second.align_set_ && ALIGN_FILL == i->second.yalign_) || ALIGN_FILL == yalign_)) {
                    px += yextra_;
                    if (rem) { ++px, --rem; }
                }

                px = std::max(px, i->second.umin_);
                if (i->second.umax_) { px = std::min(px, i->second.umax_); }
            }

            else {
                px = std::max(yextra_, i->second.rmin_);
                if (rem) { ++px, --rem; }
                if (!px) { px = i->second.rmin_ ? i->second.rmin_ : i->second.rmax_; }
                px = std::max(px, i->second.umin_);
                if (i->second.umax_) { px = std::min(px, i->second.umax_); }
            }

            y += px;
            i->second.h_ = y-i->second.y_;
            y += i->second.bottom_+yspacing_;

            if (i->second.ly_ != i->second.y_ || i->second.lh_ != i->second.h_) {
                i->second.ly_ = i->second.y_;
                i->second.lh_ = i->second.h_;
                if (!shut_ && signal_row_bounds_changed_) { (*signal_row_bounds_changed_)(i->first); }
            }
        }
    }
}

void Table_impl::place_holder(Holder & hol, Rect * inval) {
    if (hol.wp_->hidden() || !size()) { return; }
    Align xalign = xalign_, yalign = yalign_;
    int x = INT_MAX, y = INT_MAX;
    unsigned w = 0, h = 0;
    auto ce = cols_.cend();

    for (auto i = cols_.find(hol.xmin_); i != ce && i->first < hol.xmax_; ++i) {
        x = std::min(x, i->second.x_);
        w += i->second.w_;

        if (i->first > hol.xmin_) {
            w += xspacing_;
            w += i->second.left_;
        }

        if ((hol.xmax_-i->first) > 1) {
            w += i->second.right_;
        }

        if (i->second.align_set_) {
            xalign = i->second.xalign_;
        }
    }

    auto re = rows_.cend();

    for (auto i = rows_.find(hol.ymin_); i != re && i->first < hol.ymax_; ++i) {
        y = std::min(y, i->second.y_);
        h += i->second.h_;

        if (i->first > hol.ymin_) {
            h += yspacing_;
            h += i->second.top_;
        }

        if ((hol.ymax_-i->first) > 1) {
            h += i->second.bottom_;
        }

        if (i->second.align_set_) {
            yalign = i->second.yalign_;
        }
    }

    // Align widgets inside cell bounds having shrink option and non-zero required size.
    if (hol.xsh_) {
        if (hol.wmax_) {
            if (w > hol.wmax_) {
                if (hol.align_set_) {
                    xalign = hol.xalign_;
                }

                if (ALIGN_END == xalign) {
                    x += w-hol.wmax_;
                }

                else if (ALIGN_CENTER == xalign) {
                    x += (w-hol.wmax_)/2;
                }

                if (ALIGN_FILL != xalign) {
                    w = hol.wmax_;
                }
            }
        }
    }

    if (hol.ysh_) {
        if (hol.hmax_) {
            if (h > hol.hmax_) {
                if (hol.align_set_) {
                    yalign = hol.yalign_;
                }

                if (ALIGN_END == yalign) {
                    y += h-hol.hmax_;
                }

                else if (ALIGN_CENTER == yalign) {
                    y += (h-hol.hmax_)/2;
                }

                if (ALIGN_FILL != yalign) {
                    h = hol.hmax_;
                }
            }
        }
    }

    Point origin(x, y);
    origin += hol.wp_->margin_origin();
    Size size(w, h);
    size.decrease(hol.wp_->margin_hint());
    Rect r(hol.wp_->origin(), hol.wp_->size());

    if (update_child_bounds(hol.wp_, origin, size)) {
        r |= Rect(origin, size);
        if (inval) { *inval |= r; }
        else { invalidate(r); }
    }
}

void Table_impl::place_widgets() {
    Rect inval;

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        place_holder(i->second, &inval);
    }

    if (inval) { invalidate(inval); }
}

bool Table_impl::set_column_spacing_i(unsigned xspacing) {
    int d = xspacing-xspacing_;

    if (0 != d) {
        if (nvcol_ > 1) {
            unsigned ud = abs(d)*(nvcol_-1);
            if (d > 0) { xspc_ += ud; }
            else { xspc_ = xspc_ > ud ? xspc_-ud : 0; }
        }

        xspacing_ = xspacing;
        return true;
    }

    return false;
}

void Table_impl::set_column_spacing(unsigned xspacing) {
    if (set_column_spacing_i(xspacing)) { rearrange2(); }
}

bool Table_impl::set_row_spacing_i(unsigned yspacing) {
    int d = yspacing-yspacing_;

    if (0 != d) {
        if (nvrow_ > 1) {
            unsigned ud = abs(d)*(nvrow_-1);
            if (d > 0) { yspc_ += ud; }
            else { yspc_ = yspc_ > ud ? yspc_-ud : 0; }
        }

        yspacing_ = yspacing;
        return true;
    }

    return false;
}

void Table_impl::set_row_spacing(unsigned yspacing) {
    if (set_row_spacing_i(yspacing)) { rearrange2(); }
}

void Table_impl::set_spacing(unsigned xspacing, unsigned yspacing) {
    bool changed = set_column_spacing_i(xspacing);
    if (set_row_spacing_i(yspacing)) { changed = true; }
    if (changed) { rearrange2(); }
}

void Table_impl::set_spacing(unsigned spacing) {
    set_spacing(spacing, spacing);
}

void Table_impl::set_column_margin(int xx, unsigned left, unsigned right) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        int dl = left-i->second.left_, dr = right-i->second.right_, d = dl+dr;
        i->second.left_ = left;
        i->second.right_ = right;

        if (i->second.visible_ && (0 != dl || 0 != dl)) {
            unsigned ud = abs(d);
            if (d > 0) { xspc_ += ud; }
            else { xspc_ = xspc_ > ud ? xspc_-ud : 0; }
            rearrange2();
        }
    }

    else {
        if (0 != left || 0 != right) {
            auto & col = new_col(xx);
            col.left_ = left;
            col.right_ = right;
        }
    }
}

void Table_impl::set_row_margin(int yy, unsigned top, unsigned bottom) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        int dt = top-i->second.top_, db = bottom-i->second.bottom_, d = dt+db;
        i->second.top_ = top;
        i->second.bottom_ = bottom;

        if (i->second.visible_ && (0 != dt || 0 != db)) {
            unsigned ud = abs(d);
            if (d > 0) { yspc_ += ud; }
            else { yspc_ = yspc_ > ud ? yspc_-ud : 0; }
            rearrange2();
        }
    }

    else {
        if (0 != top || 0 != bottom) {
            auto & row = new_row(yy);
            row.top_ = top;
            row.bottom_ = bottom;
        }
    }
}

void Table_impl::set_columns_margin(unsigned left, unsigned right) {
    if (columns_left_ != left || columns_right_ != right) {
        columns_left_ = left;
        columns_right_ = right;
        bool changed = false;

        for (auto & p: cols_) {
            int dl = left-p.second.left_, dr = right-p.second.right_, d = dl+dr;
            p.second.left_ = left;
            p.second.right_ = right;

            if (0 != d && p.second.visible_) {
                changed = true;
                unsigned ud = abs(d);
                if (d > 0) { xspc_ += ud; }
                else { xspc_ = xspc_ > ud ? xspc_-ud : 0; }
            }
        }

        if (changed) { rearrange2(); }
    }
}

void Table_impl::set_rows_margin(unsigned top, unsigned bottom) {
    if (rows_top_ != top || rows_bottom_ != bottom) {
        rows_top_ = top;
        rows_bottom_ = bottom;
        bool changed = false;

        for (auto & p: rows_) {
            int dt = top-p.second.top_, db = bottom-p.second.bottom_, d = dt+db;
            p.second.top_ = top;
            p.second.bottom_ = bottom;

            if (0 != d && p.second.visible_) {
                changed = true;
                unsigned ud = abs(d);
                if (d > 0) { yspc_ += ud; }
                else { yspc_ = yspc_ > ud ? yspc_-ud : 0; }
            }
        }

        if (changed) { rearrange2(); }
    }
}

void Table_impl::get_column_margin(int xx, unsigned & left, unsigned & right) const {
    left = right = 0;
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        left = i->second.left_;
        right = i->second.right_;
    }
}

void Table_impl::get_row_margin(int yy, unsigned & top, unsigned & bottom) const {
    top = bottom = 0;
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        top = i->second.top_;
        bottom = i->second.bottom_;
    }
}

std::vector<Widget_impl *> Table_impl::children_within_range(int xmin, int ymin, int xmax, int ymax) {
    std::vector<Widget_impl *> v;

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;
        if (hol.xmax_ > xmin && hol.xmin_ < xmax && hol.ymax_ > ymin && hol.ymin_ < ymax) { v.push_back(hol.wp_); }
    }

    return v;
}

std::vector<Widget_impl *> Table_impl::children_within_range(const Span & rng) {
    return children_within_range(rng.xmin, rng.ymin, rng.xmax, rng.ymax);
}

Table::Span Table_impl::span() const {
    Table::Span rng;

    if (!rows_.empty() && !cols_.empty()) {
        rng.xmin = cols_.cbegin()->first;
        rng.ymin = rows_.cbegin()->first;
        rng.xmax = 1+cols_.crbegin()->first;
        rng.ymax = 1+rows_.crbegin()->first;
    }

    else {
        rng.xmin = INT_MAX;
        rng.ymin = INT_MAX;
        rng.xmax = INT_MIN;
        rng.ymax = INT_MIN;
    }

    return rng;
}

void Table_impl::get_column_span(int col, int & ymin, int & ymax) {
    ymin = INT_MAX;
    ymax = INT_MIN;

    if (cols_.end() != cols_.find(col)) {
        for (auto i = holders_.begin(); i != holders_.end(); ++i) {
            Holder & hol = i->second;

            if (hol.xmin_ <= col && hol.xmax_ > col) {
                ymin = std::min(ymin, hol.ymin_);
                ymax = std::max(ymax, hol.ymax_);
            }
        }
    }
}

void Table_impl::get_row_span(int row, int & xmin, int & xmax) {
    xmin = INT_MAX;
    xmax = INT_MIN;

    if (rows_.end() != rows_.find(row)) {
        for (auto i = holders_.begin(); i != holders_.end(); ++i) {
            Holder & hol = i->second;

            if (hol.ymin_ <= row && hol.ymax_ > row) {
                xmin = std::min(xmin, hol.xmin_);
                xmax = std::max(xmax, hol.xmax_);
            }
        }
    }
}

void Table_impl::remove(int xmin, int ymin, int xmax, int ymax) {
    for (auto wp: children_within_range(xmin, ymin, xmax, ymax)) { remove(wp); }
}

void Table_impl::align(Widget_impl * wp, Align xalign, Align yalign) {
    bool changed = false;
    auto i = holders_.find(wp);

    if (i != holders_.end()) {
        changed = !i->second.align_set_;

        if (i->second.xalign_ != xalign) {
            auto ce = cols_.end();

            if (i->second.align_set_ && i->second.xalign_ == ALIGN_FILL) {
                for (auto j = cols_.find(i->second.xmin_); j != ce && j->first < i->second.xmax_; ++j) {
                    if (j->second.fill_) { --j->second.fill_; }
                }
            }

            i->second.xalign_ = xalign;
            changed = true;

            if (xalign == ALIGN_FILL) {
                for (auto j = cols_.find(i->second.xmin_); j != ce && j->first < i->second.xmax_; ++j) {
                    ++j->second.fill_;
                }
            }
        }

        if (i->second.yalign_ != yalign) {
            auto re = rows_.end();

            if (i->second.align_set_ && i->second.yalign_ == ALIGN_FILL) {
                for (auto j = rows_.find(i->second.ymin_); j != re && j->first < i->second.ymax_; ++j) {
                    if (j->second.fill_) { --j->second.fill_; }
                }
            }

            i->second.yalign_ = yalign;
            changed = true;

            if (yalign == ALIGN_FILL) {
                for (auto j = rows_.find(i->second.ymin_); j != re && j->first < i->second.ymax_; ++j) {
                    ++j->second.fill_;
                }
            }
        }

        i->second.align_set_ = true;
    }

    if (changed) {
        queue_arrange();
    }
}

void Table_impl::get_align(const Widget_impl * wp, Align & xalign, Align & yalign) const {
    xalign = xalign_;
    yalign = yalign_;
    auto i = holders_.find(const_cast<Widget_impl *>(wp));

    if (i != holders_.cend() && i->second.align_set_) {
        xalign = i->second.xalign_;
        yalign = i->second.yalign_;
    }
}

void Table_impl::unalign(Widget_impl * wp) {
    auto i = holders_.find(wp);

    if (i != holders_.end() && i->second.align_set_) {
        i->second.align_set_ = false;
        queue_arrange();
    }
}

void Table_impl::align_columns(Align xalign) {
    if (xalign_ != xalign) {
        xalign_ = xalign;
        place_widgets();
    }
}

Align Table_impl::columns_align() const {
    return xalign_;
}

void Table_impl::align_rows(Align yalign) {
    if (yalign_ != yalign) {
        yalign_ = yalign;
        place_widgets();
    }
}

Align Table_impl::rows_align() const {
    return yalign_;
}

void Table_impl::align_column(int xx, Align xalign) {
    bool changed = false;
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (!i->second.align_set_ || i->second.xalign_ != xalign) {
            changed = true;
            i->second.xalign_ = xalign;
        }

        i->second.align_set_ = true;
        if (changed) { place_widgets(); }
    }

    else {
        auto & col = new_col(xx);
        col.align_set_ = true;
        col.xalign_ = xalign;
    }
}

Align Table_impl::column_align(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.xalign_ : xalign_;
}

void Table_impl::unalign_column(int xx) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (i->second.align_set_) {
            i->second.align_set_ = false;
            if (i->second.visible_ && (i->second.xalign_ != xalign_)) { place_widgets(); }
        }
    }
}

void Table_impl::align_row(int yy, Align yalign) {
    bool changed = false;
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (!i->second.align_set_ || i->second.yalign_ != yalign) {
            changed = true;
            i->second.yalign_ = yalign;
        }

        i->second.align_set_ = true;
        if (changed) { place_widgets(); }
    }

    else {
        auto & row = new_row(yy);
        row.align_set_ = true;
        row.yalign_ = yalign;
    }
}

Align Table_impl::row_align(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.yalign_ : yalign_;
}

void Table_impl::unalign_row(int yy) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (i->second.align_set_) {
            i->second.align_set_ = false;
            if (i->second.visible_ && (i->second.yalign_ != yalign_)) { place_widgets(); }
        }
    }
}

bool Table_impl::erasable(const Col & col) const {
    return !col.align_set_ && !col.left_ && !col.right_
        && !col.umin_ && !col.umax_ && !col.usize_;
}

void Table_impl::erase(Col_iter & i) {
    if (i != cols_.end()) {
        if (i->second.visible_) {
            i->second.visible_ = 0;
            if (i->second.w_) { i->second.w_ = 0; if (!shut_ && signal_column_bounds_changed_) (*signal_column_bounds_changed_)(i->first); }
            if (nvcol_) { --nvcol_; }
        }

        i->second.x_ = i->second.lx_ = i->second.w_ = i->second.lw_ = i->second.ref_ = i->second.shrank_ = i->second.fill_ = 0;
        if (erasable(i->second)) { cols_.erase(i); }
    }
}

void Table_impl::unref(Col_iter & i) {
    if (i != cols_.end() && i->second.ref_ && --i->second.ref_ == 0) { erase(i); }
}

bool Table_impl::erasable(const Row & row) const {
    return !row.align_set_ && !row.top_ && !row.bottom_
        && !row.umin_ && !row.umax_ && !row.usize_;
}

void Table_impl::erase(Row_iter & i) {
    if (i != rows_.end()) {
        if (i->second.visible_) {
            i->second.visible_ = 0;
            if (i->second.h_) { i->second.h_ = 0; if (!shut_ && signal_row_bounds_changed_) (*signal_row_bounds_changed_)(i->first); }
            if (nvrow_) { --nvrow_; }
        }

        i->second.y_ = i->second.ly_ = i->second.h_ = i->second.lh_ = i->second.ref_ = i->second.shrank_ = i->second.fill_ = 0;
        if (erasable(i->second)) { rows_.erase(i); }
    }
}

void Table_impl::unref(Row_iter & i) {
    if (i != rows_.end() && i->second.ref_ && --i->second.ref_ == 0) { erase(i); }
}

Table_impl::Col & Table_impl::new_col(int xx) {
    auto p = cols_.emplace(xx, Col());
    p.first->second.left_ = columns_left_;
    p.first->second.right_ = columns_right_;
    return p.first->second;
}

Table_impl::Row & Table_impl::new_row(int yy) {
    auto p = rows_.emplace(yy, Row());
    p.first->second.top_ = rows_top_;
    p.first->second.bottom_ = rows_bottom_;
    return p.first->second;
}

Table_impl::Col & Table_impl::new_col(int xx, const Col & src) {
    auto p = cols_.emplace(xx, Col(src));
    return p.first->second;
}

Table_impl::Row & Table_impl::new_row(int yy, const Row & src) {
    auto p = rows_.emplace(yy, Row(src));
    return p.first->second;
}

void Table_impl::change_visibility(Col & col, bool show, bool shrank) {
    if (show) {
        if (shrank) {
            if (0 == col.shrank_++) { ++nshcol_; }
        }

        if (0 == col.visible_++) {
            if (++nvcol_ > 1) { xspc_ += xspacing_; }
            xspc_ += col.left_+col.right_;
            if (col.usize_) { ++nucol_; }
        }
    }

    else {
        if (shrank && 0 != col.shrank_) {
            if (--col.shrank_ == 0 && nshcol_) { --nshcol_; }
        }

        if (0 != col.visible_ && --col.visible_ == 0) {
            if (1 < nvcol_--) { xspc_ -= xspacing_; }
            unsigned d = col.left_+col.right_;
            xspc_ = xspc_ > d ? xspc_-d : 0;
            if (col.usize_ && nucol_) { ++nucol_; }
        }
    }
}

void Table_impl::change_visibility(Row & row, bool show, bool shrank) {
    if (show) {
        if (shrank) {
            if (0 == row.shrank_++) { ++nshrow_; }
        }

        if (0 == row.visible_++) {
            if (++nvrow_ > 1) { yspc_ += yspacing_; }
            yspc_ += row.top_+row.bottom_;
            if (row.usize_) { ++nurow_; }
        }
    }

    else {
        if (shrank && 0 != row.shrank_) {
            if (--row.shrank_ == 0 && nshrow_) { --nshrow_; }
        }

        if (0 != row.visible_ && --row.visible_ == 0) {
            if (1 < nvrow_--) { yspc_ -= yspacing_; }
            unsigned d = row.top_+row.bottom_;
            yspc_ = yspc_ > d ? yspc_-d : 0;
            if (row.usize_ && nurow_) { ++nurow_; }
        }
    }
}

void Table_impl::dist_holder(Holder & hol) {
    for (int xx = hol.xmin_; xx < hol.xmax_; ++xx) {
        auto i = cols_.find(xx);

        if (cols_.end() != i) {
            ++i->second.ref_;
            if (!hol.wp_->hidden()) { change_visibility(i->second, true, hol.xsh_); }
        }

        else {
            auto & col = new_col(xx);
            col.ref_ = 1;
            if (!hol.wp_->hidden()) { change_visibility(col, true, hol.xsh_); }
        }
    }

    for (int yy = hol.ymin_; yy < hol.ymax_; ++yy) {
        auto i = rows_.find(yy);

        if (rows_.end() != i) {
            ++i->second.ref_;
            if (!hol.wp_->hidden()) { change_visibility(i->second, true, hol.ysh_); }
        }

        else {
            auto & row = new_row(yy);
            row.ref_ = 1;
            if (!hol.wp_->hidden()) { change_visibility(row, true, hol.ysh_); }
        }
    }

    rearrange1();
}

void Table_impl::wipe_holder(Holder & hol) {
    for (int xx = hol.xmin_; xx < hol.xmax_; ++xx) {
        auto i = cols_.find(xx);
        if (!hol.wp_->hidden()) { change_visibility(i->second, false, hol.xsh_); }
        unref(i);
    }

    for (int yy = hol.ymin_; yy < hol.ymax_; ++yy) {
        auto i = rows_.find(yy);
        if (!hol.wp_->hidden()) { change_visibility(i->second, false, hol.ysh_); }
        unref(i);
    }

    update_child_bounds(hol.wp_, INT_MIN, INT_MIN, 0, 0);
    rearrange1();
}

void Table_impl::respan(Widget_impl * wp, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh) {
    auto i = holders_.find(wp);

    if (i != holders_.end()) {
        int xmax = x+std::max(1U, xspan);
        int ymax = y+std::max(1U, yspan);

        if (i->second.xmin_ != x || i->second.ymin_ != y || i->second.xmax_ != xmax || i->second.ymax_ != ymax || i->second.xsh_ != xsh || i->second.ysh_ != ysh) {
            wipe_holder(i->second);
            i->second.xmin_ = x;
            i->second.ymin_ = y;
            i->second.xmax_ = xmax;
            i->second.ymax_ = ymax;
            i->second.xsh_ = xsh;
            i->second.ysh_ = ysh;
            dist_holder(i->second);
        }
    }
}

void Table_impl::respan(Widget_impl * wp, int x, int y, unsigned xspan, unsigned yspan) {
    auto i = holders_.find(wp);
    if (i != holders_.end()) { respan(wp, x, y, xspan, yspan, i->second.xsh_, i->second.ysh_); }
}

Rect Table_impl::range_bounds(const Span & rng) const {
    Rect bounds;
    int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN;

    if (rng.xmax > rng.xmin) {
        auto i = std::find_if(cols_.begin(), cols_.end(), [&rng](auto & p) { return p.second.visible_ && p.first >= rng.xmin; } );

        if (i != cols_.end()) {
            xmin = i->second.x_-i->second.left_;

            do {
                if (i->second.visible_) {
                    xmax = i->second.x_+i->second.w_+i->second.right_;
                }
            } while (++i != cols_.end() && i->first < rng.xmax);

            if (i != cols_.end()) { xmax += xspacing_; }
        }
    }

    if (rng.ymax > rng.ymin) {
        auto i = std::find_if(rows_.begin(), rows_.end(), [&rng](auto & p) { return p.second.visible_ && p.first >= rng.ymin; } );

        if (i != rows_.end()) {
            ymin = i->second.y_-i->second.top_;

            do {
                if (i->second.visible_) {
                    ymax = i->second.y_+i->second.h_+i->second.bottom_;
                }
            } while (++i != rows_.end() && i->first < rng.ymax);

            if (i != rows_.end()) { ymax += yspacing_; }
        }
    }

    if (xmax > xmin && ymax > ymin) { bounds.set(xmin, ymin, xmax, ymax); }
    return bounds;
}

Table::Span Table_impl::span(const Widget_impl * wp) const {
    Table::Span rng;
    auto i = holders_.find(const_cast<Widget_impl *>(wp));

    if (holders_.end() != i) {
        rng.xmin = i->second.xmin_;
        rng.ymin = i->second.ymin_;
        rng.xmax = i->second.xmax_;
        rng.ymax = i->second.ymax_;
    }

    return rng;
}

int Table_impl::column_at_x(int x) const {
    for (auto & p: cols_) {
        if (x >= p.second.x_ && x < p.second.x_+int(p.second.w_)) {
            return p.first;
        }
    }

    return INT_MIN;
}

int Table_impl::row_at_y(int y) const {
    for (auto & p: rows_) {
        if (y >= p.second.y_ && y < p.second.y_+int(p.second.h_)) {
            return p.first;
        }
    }

    return INT_MIN;
}

Rect Table_impl::bounds(int xmin, int ymin, unsigned xspan, unsigned yspan) const {
    Table::Span rng;
    rng.xmin = xmin;
    rng.ymin = ymin;
    rng.xmax = xmin+std::max(1U, xspan);
    rng.ymax = ymin+std::max(1U, yspan);
    return range_bounds(rng);
}

void Table_impl::get_column_bounds(int xx, int & xmin, int & xmax) {
    xmin = INT_MAX;
    xmax = INT_MIN;

    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        xmin = i->second.x_;
        xmax = xmin+i->second.w_;
    }
}

void Table_impl::get_row_bounds(int yy, int & ymin, int & ymax) {
    ymin = INT_MAX;
    ymax = INT_MIN;

    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        ymin = i->second.y_;
        ymax = ymin+i->second.h_;
    }
}

void Table_impl::set_column_width(int xx, unsigned usize) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (i->second.usize_ != usize) {
            std::swap(i->second.usize_, usize);

            if (i->second.visible_) {
                if (0 != usize) { ++nucol_; }
                else if (0 == i->second.usize_ && nucol_) { --nucol_; }
                if (i->second.w_ != i->second.usize_) { rearrange2(); }
            }
        }
    }

    else {
        auto & col = new_col(xx);
        col.usize_ = usize;
    }
}

unsigned Table_impl::column_width(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.usize_ : 0;
}

void Table_impl::set_row_height(int yy, unsigned usize) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (i->second.usize_ != usize) {
            std::swap(i->second.usize_, usize);

            if (i->second.visible_) {
                if (0 != usize) { ++nurow_; }
                else if (0 == i->second.usize_ && nurow_) { --nurow_; }
                if (i->second.h_ != i->second.usize_) { rearrange2(); }
            }
        }
    }

    else {
        auto & row = new_row(yy);
        row.usize_ = usize;
    }
}

unsigned Table_impl::row_height(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.usize_ : 0;
}

void Table_impl::set_min_column_width(int xx, unsigned umin) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (i->second.umin_ != umin) {
            i->second.umin_ = umin;
            if (0 == umin || i->second.w_ < umin) { rearrange2(); }
        }
    }

    else {
        auto & col = new_col(xx);
        col.umin_ = umin;
    }
}

unsigned Table_impl::min_column_width(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.umin_ : 0;
}

void Table_impl::set_min_row_height(int yy, unsigned umin) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (i->second.umin_ != umin) {
            i->second.umin_ = umin;
            if (0 == umin || i->second.h_ < umin) { rearrange2(); }
        }
    }

    else {
        auto & row = new_row(yy);
        row.umin_ = umin;
    }
}

unsigned Table_impl::min_row_height(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.umin_ : 0;
}

void Table_impl::set_max_column_width(int xx, unsigned umax) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (i->second.umax_ != umax) {
            i->second.umax_ = umax;
            if (0 == umax || i->second.w_ > umax) { rearrange2(); }
        }
    }

    else {
        auto & col = new_col(xx);
        col.umax_ = umax;
    }
}

unsigned Table_impl::max_column_width(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.umax_ : 0;
}

void Table_impl::set_max_row_height(int yy, unsigned umax) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (i->second.umax_ != umax) {
            i->second.umax_ = umax;
            if (0 == umax || i->second.h_ > umax) { rearrange2(); }
        }
    }

    else {
        auto & row = new_row(yy);
        row.umax_ = umax;
    }
}

unsigned Table_impl::max_row_height(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.umax_ : 0;
}

void Table_impl::insert_columns(int xmin, unsigned n_columns) {
    if (0 == n_columns || cols_.empty()) { return; }
    int cmax = cols_.rbegin()->first;
    if (xmin >= cmax) { return; }
    int xmax = xmin+n_columns;

    // Process selection.
    if (sel_.xmax > sel_.xmin && sel_.ymax > sel_.ymin) {
        if (sel_.xmin >= xmax) {
            sel_.xmin += n_columns;
            sel_.xmax += n_columns;
        }

        else if (sel_.xmax > xmin) {
            unselect();
        }
    }

    // Process marks.
    Marks marks(marks_);

    for (Table::Span & m: marks) {
        if (m.xmax > xmin && m.xmin < xmax) {
            unmark(m.xmin, m.ymin, m.xmax, m.ymax);
        }
    }

    marks.clear();

    for (Table::Span & m: marks_) {
        if (m.xmin >= xmax) {
            m.xmin += n_columns;
            m.xmax += n_columns;
        }
    }

    // Trim and move children.
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        // Trim child.
        if (i->second.xmin_ < xmin && i->second.xmax_ > xmin) {
            if (i->second.xmax_ > xmax) {
                for (int xx = xmax; xx < i->second.xmax_; ++xx) {
                    auto j = cols_.find(xx);

                    if (j != cols_.end()) {
                        if (!i->second.wp_->hidden()) { change_visibility(j->second, false, i->second.xsh_); }
                        unref(j);
                    }
                }
            }

            i->second.xmax_ = xmin;
        }

        // Move the child to the right.
        else if (i->second.xmin_ >= xmin) {
            i->second.xmin_ += n_columns;
            i->second.xmax_ += n_columns;
        }
    }

    // Test again after trimming.
    if (cols_.empty()) { goto exit; }
    cmax = cols_.rbegin()->first;
    if (xmin >= cmax) { goto exit; }

    // Move the columns to the right.
    for (int x = cmax; x >= xmin; --x) {
        auto c_iter = cols_.find(x);
        if (c_iter != cols_.end()) { new_col(x+n_columns, c_iter->second); }
    }

    // Remove columns.
    {
        auto e = cols_.find(xmax);
        for (auto i = cols_.find(xmin); i != e; ++i) { erase(i); }
    }
exit:
    rearrange1();
}

void Table_impl::insert_rows(int ymin, unsigned n_rows) {
    if (0 == n_rows || rows_.empty()) { return; }
    int rmax = rows_.rbegin()->first;
    if (ymin >= rmax) { return; }
    int ymax = ymin+n_rows;

    // Process selection.
    if (sel_.xmax > sel_.xmin && sel_.ymax > sel_.ymin) {
        if (sel_.ymin >= ymax) {
            sel_.ymin += n_rows;
            sel_.ymax += n_rows;
        }

        else if (sel_.ymax > ymin) {
            unselect();
        }
    }

    // Process marks.
    Marks marks(marks_);

    for (Table::Span & m: marks) {
        if (m.ymax > ymin && m.ymin < ymax) {
            unmark(m.xmin, m.ymin, m.xmax, m.ymax);
        }
    }

    marks.clear();

    for (Table::Span & m: marks_) {
        if (m.ymin >= ymax) {
            m.ymin += n_rows;
            m.ymax += n_rows;
        }
    }

    // Trim and move children.
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        // Trim the child.
        if (i->second.ymin_ < ymin && i->second.ymax_ > ymin) {
            if (i->second.ymax_ > ymax) {
                for (int yy = ymax; yy < i->second.ymax_; ++yy) {
                    auto j = rows_.find(yy);

                    if (j != rows_.end()) {
                        if (!i->second.wp_->hidden()) { change_visibility(j->second, false, i->second.ysh_); }
                        unref(j);
                    }
                }
            }

            i->second.ymax_ = ymin;
        }

        // Move the child down.
        else if (i->second.ymin_ >= ymin) {
            i->second.ymin_ += n_rows;
            i->second.ymax_ += n_rows;
        }
    }

    // Test again after trimming.
    if (rows_.empty()) { goto exit; }
    rmax = rows_.rbegin()->first;
    if (ymin >= rmax) { goto exit; }

    // Move the rows down.
    for (int yy = rmax; yy >= ymin; --yy) {
        auto r_iter = rows_.find(yy);
        if (r_iter != rows_.end()) { new_row(yy+n_rows, r_iter->second); }
    }

    // Remove rows.
    {
        auto e = rows_.find(ymax);
        for (auto i = rows_.find(ymin); i != e; ++i) { erase(i); }
    }
exit:
    rearrange1();
}

void Table_impl::remove_columns(int xmin, unsigned n_columns) {
    if (0 == n_columns || cols_.empty()) { return; }
    int last = cols_.rbegin()->first;
    if (xmin > last) { return; }
    int xmax = xmin+n_columns;
    std::list<Widget_impl *> rem;

    // Trim and move children.
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        // Trim the child.
        if (i->second.xmin_ < xmin && i->second.xmax_ > xmin) {
            if (i->second.xmax_ > xmax) {
                for (int xx = xmax; xx < i->second.xmax_; ++xx) {
                    auto j = cols_.find(xx);

                    if (j != cols_.end()) {
                        if (!i->second.wp_->hidden()) { change_visibility(j->second, false, i->second.xsh_); }
                        unref(j);
                    }
                }
            }

            i->second.xmax_ = xmin;
        }

        // Remove child.
        else if (i->second.xmin_ >= xmin && i->second.xmin_ < xmax) {
            rem.push_back(i->second.wp_);
        }

        // Move the child to the left.
        else if (i->second.xmin_ >= xmax) {
            i->second.xmin_ -= n_columns;
            i->second.xmax_ -= n_columns;
        }
    }

    // Remove children.
    for (auto wp: rem) { remove(wp); }

    // Test again after trimming and deletion.
    if (cols_.empty()) { goto exit; }
    last = cols_.rbegin()->first;
    if (xmin >= last) { goto exit; }

    // Move columns to the left.
    for (int xx = xmax; xx <= last; ++xx) {
        auto c_iter = cols_.find(xx);
        if (c_iter != cols_.end()) { new_col(xx-n_columns, c_iter->second); }
    }

    // Remove columns.
    {
        auto e = cols_.find(last);
        for (auto i = cols_.find(last-n_columns+1); i != e; ++i) { erase(i); }
    }
exit:
    rearrange1();
}

void Table_impl::remove_rows(int ymin, unsigned n_rows) {
    if (0 == n_rows || rows_.empty()) { return; }
    int last = rows_.rbegin()->first;
    if (ymin > last) { return; }
    int ymax = ymin+n_rows;
    std::list<Widget_impl *> rem;

    // Trim and move children.
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        // Trim the child.
        if (i->second.ymin_ < ymin && i->second.ymax_ > ymin) {
            if (i->second.ymax_ > ymax) {
                for (int yy = ymax; yy < i->second.ymax_; ++yy) {
                    auto j = rows_.find(yy);

                    if (j != rows_.end()) {
                        if (!i->second.wp_->hidden()) { change_visibility(j->second, false, i->second.ysh_); }
                        unref(j);
                    }
                }
            }

            i->second.ymax_ = ymin;
        }

        // Remove child.
        else if (i->second.ymin_ >= ymin && i->second.ymin_ < ymax) {
            rem.push_back(i->second.wp_);
        }

        // Move the child to the left.
        else if (i->second.ymin_ >= ymax) {
            i->second.ymin_ -= n_rows;
            i->second.ymax_ -= n_rows;
        }
    }

    // Remove children.
    for (auto wp: rem) { remove(wp); }

    // Test again after trimming and deletion.
    if (rows_.empty()) { goto exit; }
    last = rows_.rbegin()->first;
    if (ymin >= last) { goto exit; }

    // Move rows to the top.
    for (int yy = ymax; yy <= last; ++yy) {
        auto r_iter = rows_.find(yy);
        if (r_iter != rows_.end()) { new_row(yy-n_rows, r_iter->second); }
    }

    // Remove rows.
    {
        auto e = rows_.find(last);
        for (auto i = rows_.find(last-n_rows+1); i != e; ++i) { erase(i); }
    }
exit:
    rearrange1();
}

void Table_impl::select(int xmin, int ymin, unsigned xspan, unsigned yspan) {
    int xmax = xmin+xspan, ymax = ymin+yspan;

    if ((xmin != sel_.xmin || ymin != sel_.ymin || xmax != sel_.xmax || ymax != sel_.ymax)
        && !rows_.empty() && !cols_.empty() && 0 != xspan && 0 != yspan) {
        unselect();
        Span rng = span();
        if (xmax <= rng.xmin || xmin >= rng.xmax || ymax <= rng.ymin || ymin >= rng.ymax) { return; }

        if (xmin < rng.xmin) { xmin = rng.xmin; }
        if (xmax > rng.xmax) { xmax = rng.xmax; }
        if (ymin < rng.ymin) { ymin = rng.ymin; }
        if (ymax > rng.ymax) { ymax = rng.ymax; }

        if (xmax > xmin && ymax > ymin) {
            sel_.xmin = xmin;
            sel_.ymin = ymin;
            sel_.xmax = xmax;
            sel_.ymax = ymax;
            invalidate(range_bounds(sel_));
            Color c = style().color(STYLE_SELECT_BACKGROUND);
            for (auto wp: fchildren(sel_)) { wp->signal_select()(); wp->style().color(STYLE_BACKGROUND) = c; }
            if (!shut_ && signal_selection_changed_) { (*signal_selection_changed_)(); }
        }
    }
}

void Table_impl::select_column(int x) {
    Table::Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { select(x, rng.ymin, 1, rng.ymax-rng.ymin); }
}

void Table_impl::select_row(int y) {
    Table::Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { select(rng.xmin, y, rng.xmax-rng.xmin, 1); }
}

void Table_impl::unselect() {
    if (sel_.xmax > sel_.xmin && sel_.ymax > sel_.ymin) {
        invalidate(range_bounds(sel_));
        for (auto wp: fchildren(sel_)) { wp->style().unset(STYLE_BACKGROUND); wp->signal_unselect()(); }
        if (!shut_ && signal_selection_changed_) { (*signal_selection_changed_)(); }
    }

    sel_.xmin = sel_.ymin = INT_MAX;
    sel_.xmax = sel_.ymax = INT_MIN;
}

Table::Span Table_impl::selection() const {
    Table::Span rng;
    if (sel_.xmax > sel_.xmin && sel_.ymax > sel_.ymin) { rng = sel_; }
    return rng;
}

bool Table_impl::on_backpaint(Painter pr, const Rect & inval) {
    Rect r;

    for (auto & m: marks_) {
        r = range_bounds(m);
        if (r) { pr.rectangle(r.left(), r.top(), r.right(), r.bottom()); }
    }

    Color c = style().color(STYLE_SELECT_BACKGROUND);
    pr.set_brush(c.darken(0.1));
    pr.fill();
    r = range_bounds(sel_);

    if (r) {
        pr.rectangle(r.left(), r.top(), r.right(), r.bottom());
        pr.set_brush(c);
        pr.fill();
    }

    return false;
}

void Table_impl::mark(int xmin, int ymin, unsigned width, unsigned height) {
    if (rows_.empty() || cols_.empty() || 0 == width || 0 == height) { return; }
    int xmax = xmin+width, ymax = ymin+height;

    Span rng = span();
    if (xmax <= rng.xmin || xmin >= rng.xmax || ymax <= rng.ymin || ymin >= rng.ymax) { return; }

    if (xmin < rng.xmin) { xmin = rng.xmin; }
    if (xmax > rng.xmax) { xmax = rng.xmax; }
    if (ymin < rng.ymin) { ymin = rng.ymin; }
    if (ymax > rng.ymax) { ymax = rng.ymax; }
    if (xmax <= xmin || ymax <= ymin) { return; }

    Span m { xmin, ymin, xmax, ymax };
    auto i = std::find(marks_.begin(), marks_.end(), m);

    if (i == marks_.end()) {
        marks_.emplace_front(m);
        Color c = style().color(STYLE_SELECT_BACKGROUND);
        c.darker(0.1);
        invalidate(range_bounds(m));
        for (auto wp: fchildren(m)) { wp->signal_select()(); wp->style().color(STYLE_BACKGROUND) = c; }
        if (!shut_ && signal_selection_changed_) { (*signal_selection_changed_)(); }
    }
}

void Table_impl::mark_column(int x) {
    Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { mark(x, rng.ymin, 1, rng.ymax-rng.ymin); }
}

void Table_impl::mark_row(int y) {
    Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { mark(rng.xmin, y, rng.xmax-rng.xmin, 1); }
}

std::forward_list<Widget_impl *> Table_impl::fchildren(const Span & rng) {
    std::forward_list<Widget_impl *> v;

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;
        if (hol.xmax_ > rng.xmin && hol.xmin_ < rng.xmax && hol.ymax_ > rng.ymin && hol.ymin_ < rng.ymax) { v.push_front(hol.wp_); }
    }

    return v;
}

void Table_impl::unmark_all() {
    std::forward_list<Widget_impl *> v;
    Rect r;
    for (auto & m: marks_) { v.merge(fchildren(m)); r |= range_bounds(m); }
    if (r) { invalidate(r); }
    for (auto wp: v) { wp->style().unset(STYLE_BACKGROUND); wp->signal_unselect()(); }
    if (!marks_.empty() && !shut_ && signal_selection_changed_) { (*signal_selection_changed_)(); }
    marks_.clear();
}

void Table_impl::unmark(int xmin, int ymin, int xmax, int ymax) {
    Span m { xmin, ymin, xmax, ymax };
    auto i = std::find(marks_.begin(), marks_.end(), m);

    if (i != marks_.end()) {
        invalidate(range_bounds(m));
        for (auto wp: fchildren(m)) { wp->style().unset(STYLE_BACKGROUND); wp->signal_unselect()(); }
        marks_.remove(m);
        if (!shut_ && signal_selection_changed_) { (*signal_selection_changed_)(); }
    }
}

void Table_impl::unmark_column(int x) {
    Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { unmark(x, rng.ymin, x+1, rng.ymax); }
}

void Table_impl::unmark_row(int y) {
    Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { unmark(rng.xmin, y, rng.xmax, y+1); }
}

std::vector<Table::Span> Table_impl::marks() const {
    std::vector<Table::Span> v;
    for (auto & m: marks_) { v.emplace_back(m); }
    return v;
}

bool Table_impl::on_take_focus() {
    if (focused_child_ && focused_child_->take_focus()) {
        return true;
    }

    if (std::any_of(holders_.begin(), holders_.end(), [](auto & p) { return p.second.wp_->take_focus(); } )) {
        return true;
    }

    return grab_focus();
}

signal<void(int)> & Table_impl::signal_column_bounds_changed() {
    if (!signal_column_bounds_changed_) { signal_column_bounds_changed_ = new signal<void(int)>; }
    return *signal_column_bounds_changed_;
}

signal<void(int)> & Table_impl::signal_row_bounds_changed() {
    if (!signal_row_bounds_changed_) { signal_row_bounds_changed_ = new signal<void(int)>; }
    return *signal_row_bounds_changed_;
}

signal<void()> & Table_impl::signal_selection_changed() {
    if (!signal_selection_changed_) { signal_selection_changed_ = new signal<void()>; }
    return *signal_selection_changed_;
}

} // namespace tau

//END
