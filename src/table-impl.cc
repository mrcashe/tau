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

#include <tau/brush.hh>
#include <tau/loop.hh>
#include <tau/painter.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <table-impl.hh>
#include <window-impl.hh>

#include <iostream>

namespace tau {

Table_impl::Table_impl():
    Container_impl()
{
    signal_arrange_.connect(fun(this, &Table_impl::arrange));
    signal_size_changed_.connect(fun(this, &Table_impl::arrange));
    signal_visible_.connect(fun(this, &Table_impl::arrange));
    signal_display_.connect(fun(this, &Table_impl::update_requisition));
    signal_backpaint_.connect(fun(this, &Table_impl::on_backpaint));
    signal_take_focus_.connect(fun(this, &Table_impl::on_take_focus));
}

void Table_impl::put(Widget_ptr wp, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh) {
    if (wp) {
        Holder hol;
        make_child(wp);
        wp->update_origin(INT_MIN, INT_MIN);
        wp->update_size(0, 0);
        hol.wp = wp.get();
        hol.xmin = x;
        hol.ymin = y;
        hol.xmax = x+std::max(1U, xspan);
        hol.ymax = y+std::max(1U, yspan);
        hol.xsh = 1 == hol.xmax-hol.xmin && xsh;
        hol.ysh = 1 == hol.ymax-hol.ymin && ysh;
        hol.align_set = false;
        hol.hints_cx = wp->signal_requisition_changed().connect(tau::bind(fun(this, &Table_impl::on_child_requisition_changed), wp.get()));
        hol.req_cx = wp->signal_requisition_changed().connect(tau::bind(fun(this, &Table_impl::on_child_requisition_changed), wp.get()));
        hol.show_cx = wp->signal_show().connect(tau::bind(fun(this, &Table_impl::on_child_show), wp.get()), true);
        hol.hide_cx = wp->signal_hide().connect(tau::bind(fun(this, &Table_impl::on_child_hide), wp.get()), true);
        holders_[wp.get()] = hol;
        dist_holder(hol);
        update_requisition();
        queue_arrange();
    }
}

void Table_impl::remove(Widget_impl * wp) {
    if (wp) {
        auto i = holders_.find(wp);

        if (i != holders_.end()) {
            wipe_holder(i->second);
            i->second.hints_cx.disconnect();
            i->second.req_cx.disconnect();
            i->second.hide_cx.disconnect();
            i->second.show_cx.disconnect();
            unparent_child(wp);
            wp->update_origin(INT_MIN, INT_MIN);
            wp->update_size(0, 0);
            holders_.erase(i);
            update_requisition();
            queue_arrange();
        }
    }
}

// Both hints & requisitions come here.
void Table_impl::on_child_requisition_changed(Widget_impl * wi) {
    if (destroy_) { return; }
//     Size was_req = required_size(), req = get_requisition();
//
//     if (was_req == req) {
//         loarr_tmr_.start(46);
//         return;
//     }
//     require_size(req);

    update_requisition();
    queue_arrange();
}

void Table_impl::on_child_show(Widget_impl * wi) {
    if (!destroy_) {
        auto i = holders_.find(wi);

        if (i != holders_.end()) {
            Holder & hol = i->second;

            for (int x = hol.xmin; x < hol.xmax; ++x) {
                Col & col = cols_[x];
                ++col.visible;
                if (hol.xsh) { ++col.shrank; }
            }

            for (int y = hol.ymin; y < hol.ymax; ++y) {
                Row & row = rows_[y];
                ++row.visible;
                if (hol.ysh) { ++row.shrank; }
            }

            update_requisition();
            queue_arrange();
        }
    }
}

void Table_impl::on_child_hide(Widget_impl * wi) {
    if (!destroy_) {
        auto i = holders_.find(wi);

        if (i != holders_.end()) {
            wi->update_origin(INT_MIN, INT_MIN);
            wi->update_size(0, 0);
            Holder & hol = i->second;

            for (int x = hol.xmin; x < hol.xmax; ++x) {
                Col & col = cols_[x];
                if (0 != col.visible) { --col.visible; }
                if (hol.xsh && 0 != col.shrank) { --col.shrank; }
            }

            for (int y = hol.ymin; y < hol.ymax; ++y) {
                Row & row = rows_[y];
                if (0 != row.visible) { --row.visible; }
                if (hol.ysh && 0 != row.shrank) { --row.shrank; }
            }

            update_requisition();
            queue_arrange();
        }
    }
}

void Table_impl::arrange() {
    alloc_cols();
    alloc_rows();
    place_widgets();
}

void Table_impl::update_requisition() {
    require_size(get_requisition());
}

void Table_impl::alloc_child(Holder & hol) {
    hol.wmax = hol.hmax = hol.wmin = hol.hmin = 0;
    if (hol.wp->hidden()) { return; }

    Size req = hol.wp->required_size(), min = hol.wp->min_size_hint();
    req.update(hol.wp->size_hint(), true);
    req.update_max(min);
    req.update_min(hol.wp->max_size_hint(), true);
    req.increase(hol.wp->margin_hint());

    hol.wmin = min.width();
    hol.hmin = min.height();

    hol.wmax = req.width();
    hol.hmax = req.height();

    // --------------------------------------------------------------------------------
    // X Axis.
    // --------------------------------------------------------------------------------

    auto cb = cols_.find(hol.xmin), ce = cols_.end();
    unsigned nc = hol.xmax-hol.xmin;
    unsigned spc = 0;

    if (1 == nc) {
        cb->second.rmax = std::max(cb->second.rmax, hol.wmax);
        cb->second.rmin = std::max(cb->second.rmin, hol.wmin);
    }

    else {
        for (auto i(cb); i != ce && i->first < hol.xmax; ++i) {
            const Col & col = i->second;
            if (i != cb) { spc += col.left+xspacing_; }
            if ((hol.xmax-i->first) > 1) { spc += col.right; }
            if (col.shrank) { nc = std::max(1U, nc-1); }
        }

        unsigned wmax = hol.wmax > spc ? hol.wmax-spc : 0;  // maximal width.
        unsigned wmin = hol.wmin > spc ? hol.wmin-spc : 0;  // minimal width.

        // Calculate width per cell and remainder per cell.
        unsigned wpc_max = wmax/nc;
        unsigned rem_max = wmax%nc;
        unsigned wpc_min = wmin/nc;
        unsigned rem_min = wmin%nc;

        for (auto i(cb); i != ce && i->first < hol.xmax; ++i) {
            Col & col = i->second;

            if (!col.shrank) {
                unsigned w1 = wpc_max, w2 = wpc_min;
                if (w1 && rem_max) { ++w1, --rem_max; }
                if (w2 && rem_min) { ++w2, --rem_min; }
                col.rmax = std::max(col.rmax, w1);
                col.rmin = std::max(col.rmin, w2);
            }
        }
    }

    // --------------------------------------------------------------------------------
    // Y Axis.
    // --------------------------------------------------------------------------------

    auto rb = rows_.find(hol.ymin), re = rows_.end();
    nc = hol.ymax-hol.ymin;
    spc = 0;

    if (1 == nc) {
        rb->second.rmax = std::max(rb->second.rmax, hol.hmax);
        rb->second.rmin = std::max(rb->second.rmin, hol.hmin);
    }

    else {
        for (auto i(rb); i != re && i->first < hol.ymax; ++i) {
            const Row & row = i->second;
            if (i != rb) { spc += row.top+yspacing_; }
            if ((hol.ymax-i->first) > 1) { spc += row.bottom; }
            if (row.shrank) { nc = std::max(1U, nc-1); }
        }

        unsigned hmax = hol.hmax > spc ? hol.hmax-spc : 0;
        unsigned hmin = hol.hmin > spc ? hol.hmin-spc : 0;

        // Calculate height per cell and remainder per cell.
        unsigned hpc_max = hmax/nc;
        unsigned rem_max = hmax%nc;
        unsigned hpc_min = hmin/nc;
        unsigned rem_min = hmin%nc;

        for (auto i(rb); i != re && i->first < hol.ymax; ++i) {
            Row & row = i->second;

            if (!row.shrank) {
                unsigned h1 = hpc_max, h2 = hpc_min;
                if (h1 && rem_max) { ++h1; --rem_max; }
                if (h2 && rem_min) { ++h2; --rem_min; }
                row.rmax = std::max(row.rmax, h1);
                row.rmin = std::max(row.rmin, h2);
            }
        }
    }
}

Size Table_impl::get_requisition() {
    auto cb = cols_.begin(), ce = cols_.end();
    auto rb = rows_.begin(), re = rows_.end();

    for (auto i(cb); i != ce; ++i) { i->second.rmin = i->second.rmax = 0; }
    for (auto i(rb); i != re; ++i) { i->second.rmin = i->second.rmax = 0; }

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;
        if (!hol.wp->hidden()) { alloc_child(hol); }
    }

    unsigned rx = 0;

    for (auto i(cb); i != ce; ++i) {
        const Col & col = i->second;

        if (col.visible) {
            if (i != cb) { rx += xspacing_; }
            unsigned w = col.usize ? col.usize : std::max(col.rmin, col.rmax);
            if (col.umax) { w = std::min(w, col.umax); }
            w = std::max(w, col.umin);
            rx += w+col.left+col.right;
        }
    }

    unsigned ry = 0;

    for (auto i(rb); i != re; ++i) {
        const Row & row = i->second;

        if (row.visible) {
            if (i != rb) { ry += yspacing_; }
            unsigned h = row.usize ? row.usize : std::max(row.rmin, row.rmax);
            if (row.umax) { h = std::min(h, row.umax); }
            h = std::max(h, row.umin);
            ry += h+row.top+row.bottom;
        }
    }

    return Size(rx, ry);
}

void Table_impl::alloc_cols() {
    unsigned px;
    unsigned user = 0;
    unsigned sh = 0;
    unsigned nsh = 0;
    unsigned nfree = 0;
    unsigned spc = 0;

    auto cb = cols_.begin(), ce = cols_.end();

    // First pass.
    for (auto i(cb); i != ce; ++i) {
        const Col & col = i->second;

        if (col.visible) {
            if (i != cb) { spc += xspacing_; }
            spc += col.left+col.right;

            if (col.usize) {
                px = col.usize;
                if (col.umax) { px = std::min(px, col.umax); }
                user += px;
            }

            else if (col.shrank) {
                px = col.rmax;
                if (col.umax) { px = std::min(px, col.umax); }
                sh += px;
                ++nsh;
            }

            else {
                ++nfree;
            }
        }
    }

    int x = 0;
    unsigned avail = size().width();
    unsigned req = spc+user+sh;
    avail = avail > req ? avail-req : 0;
    unsigned nextra = nfree ? nfree : nsh;
    unsigned extra = nextra ? avail/nextra : 0;
    unsigned rem = nextra ? avail%nextra : 0;

    // Second pass.
    for (auto i(cb); i != ce; ++i) {
        Col & col = i->second;

        if (col.visible) {
            col.ox = col.x;
            col.ow = col.w;
            x += col.left;
            col.x = x;

            if (col.usize) {
                px = col.usize;
                px = std::max(px, col.umin);
                if (col.umax) { px = std::min(px, col.umax); }
                user = user > px ? user-px : 0;
            }

            else if (col.shrank) {
                px = std::max(col.rmin, col.rmax);

                if (!nfree && !user && (col.fill || (col.align_set && ALIGN_FILL == col.xalign) || ALIGN_FILL == xalign_)) {
                    px += extra;
                    if (rem) { ++px, --rem; }
                }

                px = std::max(px, col.umin);
                if (col.umax) { px = std::min(px, col.umax); }
                sh = sh > px ? sh-px : 0;
            }

            else {
                px = std::max(extra, col.rmin);
                if (rem) { ++px, --rem; }
                if (!px) { px = col.rmin ? col.rmin : col.rmax; }
                px = std::max(px, col.umin);
                if (col.umax) { px = std::min(px, col.umax); }
            }

            x += px;
            col.w = x-col.x;
            x += col.right+xspacing_;
        }
    }

    // Third pass.
    for (auto i(cb); i != ce; ++i) {
        if (i->second.ox != i->second.x || i->second.ow != i->second.w) {
            signal_column_bounds_changed_(i->first);
        }
    }
}

void Table_impl::alloc_rows() {
    unsigned px;
    unsigned user = 0;
    unsigned sh = 0;
    unsigned nsh = 0;
    unsigned nfree = 0;
    unsigned spc = 0;

    auto rb = rows_.begin(), re = rows_.end();

    // First pass.
    for (auto i(rb); i != re; ++i) {
        const Row & row = i->second;

        if (row.visible) {
            if (i != rb) { spc += yspacing_; }
            spc += row.top+row.bottom;

            if (row.usize) {
                px = std::max(row.usize, row.umin);
                if (row.umax) { px = std::min(px, row.umax); }
                user += px;
            }

            else if (row.shrank) {
                px = row.rmax;
                if (row.umax) { px = std::min(px, row.umax); }
                sh += px;
                ++nsh;
            }

            else {
                ++nfree;
            }
        }
    }

    int y = 0;
    unsigned avail = size().height();
    unsigned req = spc+user+sh;
    avail = avail > req ? avail-req : 0;
    unsigned nextra = nfree ? nfree : nsh;
    unsigned extra = nextra ? avail/nextra : 0;
    unsigned rem = nextra ? avail%nextra : 0;

    // Second pass.
    for (auto i(rb); i != re; ++i) {
        Row & row = i->second;

        if (row.visible) {
            row.oy = row.y;
            row.oh = row.h;
            y += row.top;
            row.y = y;

            if (row.usize) {
                px = row.usize;
                px = std::max(px, row.umin);
                if (row.umax) { px = std::min(px, row.umax); }
                user = user > px ? user-px : 0;
            }

            else if (row.shrank) {
                px = std::max(row.rmin, row.rmax);

                //if (!nfree && 1 == nsh) {
                if (!nfree && !user && (row.fill || (row.align_set && ALIGN_FILL == row.yalign) || ALIGN_FILL == yalign_)) {
                    px += extra;
                    if (rem) { ++px, --rem; }
                }

                px = std::max(px, row.umin);
                if (row.umax) { px = std::min(px, row.umax); }
                sh = sh > px ? sh-px : 0;
            }

            else {
                px = std::max(extra, row.rmin);
                if (rem) { ++px, --rem; }
                if (!px) { px = row.rmin ? row.rmin : row.rmax; }
                px = std::max(px, row.umin);
                if (row.umax) { px = std::min(px, row.umax); }
            }

            y += px;
            row.h = y-row.y;
            y += row.bottom+yspacing_;
        }
    }

    // Third pass.
    for (auto i(rb); i != re; ++i) {
        if (i->second.oy != i->second.y || i->second.oh != i->second.h) {
            signal_row_bounds_changed_(i->first);
        }
    }
}

void Table_impl::place_holder(Holder & hol, Rect * inval) {
    if (hol.wp->hidden() || !size()) { return; }

    Align xalign = xalign_;
    Align yalign = yalign_;

    int x = INT_MAX;
    int y = INT_MAX;

    unsigned w = 0;
    unsigned h = 0;

    auto ce = cols_.cend();

    for (auto i = cols_.find(hol.xmin); i != ce && i->first < hol.xmax; ++i) {
        const Col & col = i->second;

        x = std::min(x, col.x);
        w += col.w;

        if (i->first > hol.xmin) {
            w += xspacing_;
            w += col.left;
        }

        if ((hol.xmax-i->first) > 1) {
            w += col.right;
        }

        if (col.align_set) {
            xalign = col.xalign;
        }
    }

    auto re = rows_.cend();

    for (auto i = rows_.find(hol.ymin); i != re && i->first < hol.ymax; ++i) {
        const Row & row = i->second;

        y = std::min(y, row.y);
        h += row.h;

        if (i->first > hol.ymin) {
            h += yspacing_;
            h += row.top;
        }

        if ((hol.ymax-i->first) > 1) {
            h += row.bottom;
        }

        if (row.align_set) {
            yalign = row.yalign;
        }
    }

    // Align widgets inside cell bounds having shrink option and non-zero required size.
    if (hol.xsh) {
        if (hol.wmax) {
            if (w > hol.wmax) {
                if (hol.align_set) {
                    xalign = hol.xalign;
                }

                if (ALIGN_END == xalign) {
                    x += w-hol.wmax;
                }

                else if (ALIGN_CENTER == xalign) {
                    x += (w-hol.wmax)/2;
                }

                if (ALIGN_FILL != xalign) {
                    w = hol.wmax;
                }
            }
        }
    }

    if (hol.ysh) {
        if (hol.hmax) {
            if (h > hol.hmax) {
                if (hol.align_set) {
                    yalign = hol.yalign;
                }

                if (ALIGN_END == yalign) {
                    y += h-hol.hmax;
                }

                else if (ALIGN_CENTER == yalign) {
                    y += (h-hol.hmax)/2;
                }

                if (ALIGN_FILL != yalign) {
                    h = hol.hmax;
                }
            }
        }
    }

    Point origin(x, y);
    origin += hol.wp->margin_origin();
    Size size(w, h);
    size.decrease(hol.wp->margin_hint());
    Rect r(hol.wp->origin(), hol.wp->size());
    bool refresh = false;

    if (hol.wp->update_origin(origin)) { refresh = true; }
    if (hol.wp->update_size(size)) { refresh = true; }

    if (refresh) {
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

void Table_impl::set_column_spacing(unsigned spacing) {
    if (xspacing_ != spacing) {
        xspacing_ = spacing;
        update_requisition();
        queue_arrange();
    }
}

void Table_impl::set_row_spacing(unsigned spacing) {
    if (yspacing_ != spacing) {
        yspacing_ = spacing;
        update_requisition();
        queue_arrange();
    }
}

void Table_impl::set_spacing(unsigned xspacing, unsigned yspacing) {
    set_column_spacing(xspacing);
    set_row_spacing(yspacing);
}

std::vector<Widget_impl *> Table_impl::children_within_range(int xmin, int ymin, int xmax, int ymax) {
    std::vector<Widget_impl *> v;

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;
        if (hol.xmax > xmin && hol.xmin < xmax && hol.ymax > ymin && hol.ymin < ymax) { v.push_back(hol.wp); }
    }

    return v;
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

            if (hol.xmin <= col && hol.xmax > col) {
                ymin = std::min(ymin, hol.ymin);
                ymax = std::max(ymax, hol.ymax);
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

            if (hol.ymin <= row && hol.ymax > row) {
                xmin = std::min(xmin, hol.xmin);
                xmax = std::max(xmax, hol.xmax);
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
        Holder & hol = i->second;

        changed = !hol.align_set;

        if (hol.xalign != xalign) {
            if (hol.align_set && hol.xalign == ALIGN_FILL) {
                auto ce = cols_.end();

                for (auto i = cols_.find(hol.xmin); i != ce && i->first < hol.xmax; ++i) {
                    if (i->second.fill) { --i->second.fill; }
                }
            }

            hol.xalign = xalign;
            changed = true;

            if (xalign == ALIGN_FILL) {
                auto ce = cols_.end();

                for (auto i = cols_.find(hol.xmin); i != ce && i->first < hol.xmax; ++i) {
                    ++i->second.fill;
                }
            }
        }

        if (hol.yalign != yalign) {
            if (hol.align_set && hol.yalign == ALIGN_FILL) {
                auto re = rows_.end();

                for (auto i = rows_.find(hol.ymin); i != re && i->first < hol.ymax; ++i) {
                    if (i->second.fill) { --i->second.fill; }
                }
            }

            hol.yalign = yalign;
            changed = true;

            if (yalign == ALIGN_FILL) {
                auto re = rows_.end();

                for (auto i = rows_.find(hol.ymin); i != re && i->first < hol.ymax; ++i) {
                    ++i->second.fill;
                }
            }
        }

        hol.align_set = true;
    }

    if (changed) {
        queue_arrange();
    }
}

void Table_impl::get_align(const Widget_impl * wp, Align & xalign, Align & yalign) const {
    xalign = xalign_;
    yalign = yalign_;
    auto i = holders_.find(const_cast<Widget_impl *>(wp));

    if (i != holders_.cend()) {
        const Holder & hol = i->second;
        if (hol.align_set) { xalign = hol.xalign; yalign = hol.yalign; }
    }
}

void Table_impl::unalign(Widget_impl * wp) {
    auto i = holders_.find(wp);

    if (i != holders_.end()) {
        Holder & hol = i->second;
        if (hol.align_set) { hol.align_set = false; queue_arrange(); }
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
        Col & col = i->second;

        if (!col.align_set || col.xalign != xalign) {
            changed = true;
            col.xalign = xalign;
        }

        col.align_set = true;
        if (changed) { place_widgets(); }
    }

    else {
        Col col;
        col.align_set = true;
        col.xalign = xalign;
        cols_[xx] = col;
    }
}

Align Table_impl::column_align(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.xalign : xalign_;
}

void Table_impl::unalign_column(int xx) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        Col & col = i->second;

        if (col.align_set) {
            col.align_set = false;
            if (col.visible && (col.xalign != xalign_)) { place_widgets(); }
        }
    }
}

void Table_impl::align_row(int yy, Align yalign) {
    bool changed = false;
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        Row & row = i->second;

        if (!row.align_set || row.yalign != yalign) {
            changed = true;
            row.yalign = yalign;
        }

        row.align_set = true;
        if (changed) { place_widgets(); }
    }

    else {
        Row row;
        row.align_set = true;
        row.yalign = yalign;
        rows_[yy] = row;
    }
}

Align Table_impl::row_align(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.yalign : yalign_;
}

void Table_impl::unalign_row(int yy) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        Row & row = i->second;

        if (row.align_set) {
            row.align_set = false;
            if (row.visible && (row.yalign != yalign_)) { place_widgets(); }
        }
    }
}

void Table_impl::erase_col(Col_iter & i) {
    if (i != cols_.end()) {
        i->second.ref = 0;

        if (!i->second.align_set && !i->second.left &&
            !i->second.right && !i->second.umin && !i->second.umax && !i->second.usize)
        {
            cols_.erase(i);
        }
    }
}

void Table_impl::unref_col(Col_iter & i) {
    if (i != cols_.end() && i->second.ref && --i->second.ref == 0) { erase_col(i); }
}

void Table_impl::erase_row(Row_iter & i) {
    if (i != rows_.end()) {
        i->second.ref = 0;

        if (!i->second.align_set && !i->second.top &&
            !i->second.bottom && !i->second.umin && !i->second.umax && !i->second.usize)
        {
            rows_.erase(i);
        }
    }
}

void Table_impl::unref_row(Row_iter & i) {
    if (i != rows_.end() && i->second.ref && --i->second.ref == 0) { erase_row(i); }
}

void Table_impl::wipe_holder(Holder & hol) {
    auto ce = cols_.end();

    for (auto i = cols_.find(hol.xmin); i != ce && i->first < hol.xmax; ++i) {
        Col & col = i->second;
        if (hol.wp->visible() && 0 != col.visible) { --col.visible; }
        unref_col(i);
    }

    auto re = rows_.end();

    for (auto i = rows_.find(hol.ymin); i != re && i->first < hol.ymax; ++i) {
        Row & row = i->second;
        if (hol.wp->visible() && 0 != row.visible) { --row.visible; }
        unref_row(i);
    }
}

void Table_impl::dist_holder(Holder & hol) {
    for (int xx = hol.xmin; xx < hol.xmax; ++xx) {
        auto i = cols_.find(xx);

        if (cols_.end() != i) {
            Col & col = i->second;
            ++col.ref;
            if (hol.xsh) { ++col.shrank; }
            if (!hol.wp->hidden()) { ++col.visible; }
        }

        else {
            Col col;
            col.ref = 1;
            col.shrank = hol.xsh ? 1 : 0;
            col.visible = hol.wp->hidden() ? 0 : 1;
            cols_[xx] = col;
        }
    }

    for (int yy = hol.ymin; yy < hol.ymax; ++yy) {
        auto i = rows_.find(yy);

        if (rows_.end() != i) {
            Row & row = i->second;
            ++row.ref;
            if (hol.ysh) { ++row.shrank; }
            if (!hol.wp->hidden()) { ++row.visible; }
        }

        else {
            Row row;
            row.ref = 1;
            row.shrank = hol.ysh ? 1 : 0;
            row.visible = hol.wp->hidden() ? 0 : 1;
            rows_[yy] = row;
        }
    }
}

void Table_impl::set_column_margin(int xx, unsigned left, unsigned right) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        Col & col = i->second;
        bool changed = false;

        if (col.left != left) {
            col.left = left;
            changed = true;
        }

        if (col.right != right) {
            col.right = right;
            changed = true;
        }

        if (changed) {
            update_requisition();
            queue_arrange();
        }
    }

    else {
        Col col;
        col.left = left;
        col.right = right;
        cols_[xx] = col;
    }
}

void Table_impl::set_row_margin(int yy, unsigned top, unsigned bottom) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        Row & row = i->second;
        bool changed = false;

        if (row.top != top) {
            row.top = top;
            changed = true;
        }

        if (row.bottom != bottom) {
            row.bottom = bottom;
            changed = true;
        }

        if (changed) {
            update_requisition();
            queue_arrange();
        }
    }

    else {
        Row row;
        row.top = top;
        row.bottom = bottom;
        rows_[yy] = row;
    }
}

void Table_impl::get_column_margin(int xx, unsigned & left, unsigned & right) const {
    left = right = 0;
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        const Col & col = i->second;
        left = col.left;
        right = col.right;
    }
}

void Table_impl::get_row_margin(int yy, unsigned & top, unsigned & bottom) const {
    top = bottom = 0;
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        const Row & row = i->second;
        top = row.top;
        bottom = row.bottom;
    }
}

bool Table_impl::on_backpaint(Painter pr, const Rect & inval) {
    Color c = style().color("select/background");

    for (Table::Span & m: marks_) {
        Rect rsel = range_bounds(m);
        Rect r = rsel & inval;

        if (r) {
            pr.move_to(r.left(), r.top());
            pr.line_to(r.right()+1, r.top());
            pr.line_to(r.right()+1, r.bottom()+1);
            pr.line_to(r.left(), r.bottom()+1);
            pr.line_to(r.left(), r.top());
            pr.set_brush(c.darken(0.1));
            pr.fill();
        }
    }

    if (sel_.xmax > sel_.xmin && sel_.ymax > sel_.ymin) {
        Rect rsel = range_bounds(sel_);
        Rect r = rsel & inval;

        if (r) {
            pr.move_to(r.left(), r.top());
            pr.line_to(r.right()+1, r.top());
            pr.line_to(r.right()+1, r.bottom()+1);
            pr.line_to(r.left(), r.bottom()+1);
            pr.line_to(r.left(), r.top());
            pr.set_brush(c);
            pr.fill();
        }
    }

    return false;
}

Rect Table_impl::range_bounds(const Table::Span & rng) const {
    int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN;

    if (rng.xmax > rng.xmin) {
        for (auto & p: cols_) {
            if (p.first >= rng.xmin && p.first < rng.xmax) {
                xmin = std::min(xmin, p.second.x-int(p.second.left));
                xmax = std::max(xmax, p.second.x+int(p.second.w+p.second.right));
            }
        }
    }

    if (rng.ymax > rng.ymin) {
        for (auto & p: rows_) {
            if (p.first >= rng.ymin && p.first < rng.ymax) {
                ymin = std::min(ymin, p.second.y-int(p.second.top));
                ymax = std::max(ymax, p.second.y+int(p.second.h+p.second.bottom));
            }
        }
    }

    if (xmax > xmin && ymax > ymin) {
        return Rect(xmin, ymin, Size(xmax-xmin, ymax-ymin));
    }

    else {
        return Rect();
    }
}

Table::Span Table_impl::span(const Widget_impl * wp) const {
    Table::Span rng;

    auto i = holders_.find(const_cast<Widget_impl *>(wp));

    if (holders_.cend() != i) {
        rng.xmin = i->second.xmin;
        rng.ymin = i->second.ymin;
        rng.xmax = i->second.xmax;
        rng.ymax = i->second.ymax;
    }

    return rng;
}

void Table_impl::respan(Widget_impl * wp, int x, int y, unsigned xspan, unsigned yspan) {
    if (wp) {
        auto i = holders_.find(wp);
        if (i != holders_.end()) { respan(wp, x, y, xspan, yspan, i->second.xsh, i->second.ysh); }
    }
}

void Table_impl::respan(Widget_impl * wp, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh) {
    if (wp) {
        auto i = holders_.find(wp);

        if (i != holders_.end()) {
            Holder & hol = i->second;
            int xmax = x+std::max(1U, xspan);
            int ymax = y+std::max(1U, yspan);

            if (hol.xmin != x || hol.ymin != y || hol.xmax != xmax || hol.ymax != ymax || hol.xsh != xsh || hol.ysh != ysh) {
                wipe_holder(hol);
                hol.xmin = x;
                hol.ymin = y;
                hol.xmax = xmax;
                hol.ymax = ymax;
                hol.xsh = xsh;
                hol.ysh = ysh;
                dist_holder(hol);
                update_requisition();
                queue_arrange();
            }
        }
    }
}

int Table_impl::column_at_x(int x) const {
    for (auto & p: cols_) {
        if (x >= p.second.x && x < p.second.x+int(p.second.w)) {
            return p.first;
        }
    }

    return INT_MIN;
}

int Table_impl::row_at_y(int y) const {
    for (auto & p: rows_) {
        if (y >= p.second.y && y < p.second.y+int(p.second.h)) {
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
        xmin = i->second.x;
        xmax = xmin+i->second.w;
    }

    //std::cout << "get_column_bounds " << xx << " " << xmin << " " << xmax << "\n";
}

void Table_impl::get_row_bounds(int yy, int & ymin, int & ymax) {
    ymin = INT_MAX;
    ymax = INT_MIN;

    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        ymin = i->second.y;
        ymax = ymin+i->second.h;
    }
}

void Table_impl::set_column_width(int xx, unsigned usize) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        Col & col = i->second;

        if (col.usize != usize) {
            col.usize = usize;

            if (col.w != usize) {
                update_requisition();
                queue_arrange();
            }
        }
    }

    else {
        Col col;
        col.usize = usize;
        cols_[xx] = col;
    }
}

unsigned Table_impl::column_width(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.usize : 0;
}

void Table_impl::set_row_height(int yy, unsigned usize) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        Row & row = i->second;

        if (row.usize != usize) {
            row.usize = usize;

            if (row.h != usize) {
                update_requisition();
                queue_arrange();
            }
        }
    }

    else {
        Row row;
        row.usize = usize;
        rows_[yy] = row;
    }
}

unsigned Table_impl::row_height(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.usize : 0;
}

void Table_impl::set_min_column_width(int xx, unsigned umin) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (i->second.umin != umin) {
            i->second.umin = umin;

            if (0 == umin || i->second.w < umin) {
                update_requisition();
                queue_arrange();
            }
        }
    }

    else {
        Col col;
        col.umin = umin;
        cols_[xx] = col;
    }
}

unsigned Table_impl::min_column_width(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.umin : 0;
}

void Table_impl::set_min_row_height(int yy, unsigned umin) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (i->second.umin != umin) {
            i->second.umin = umin;

            if (0 == umin || i->second.h < umin) {
                update_requisition();
                queue_arrange();
            }
        }
    }

    else {
        Row row;
        row.umin = umin;
        rows_[yy] = row;
    }
}

unsigned Table_impl::min_row_height(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.umin : 0;
}

void Table_impl::set_max_column_width(int xx, unsigned umax) {
    auto i = cols_.find(xx);

    if (i != cols_.end()) {
        if (i->second.umax != umax) {
            i->second.umax = umax;

            if (0 == umax || i->second.w > umax) {
                update_requisition();
                queue_arrange();
            }
        }
    }

    else {
        Col col;
        col.umax = umax;
        cols_[xx] = col;
    }
}

unsigned Table_impl::max_column_width(int xx) const {
    auto i = cols_.find(xx);
    return i != cols_.end() ? i->second.umax : 0;
}

void Table_impl::set_max_row_height(int yy, unsigned umax) {
    auto i = rows_.find(yy);

    if (i != rows_.end()) {
        if (i->second.umax != umax) {
            i->second.umax = umax;

            if (0 == umax || i->second.h > umax) {
                update_requisition();
                queue_arrange();
            }
        }
    }

    else {
        Row row;
        row.umax = umax;
        rows_[yy] = row;
    }
}

unsigned Table_impl::max_row_height(int yy) const {
    auto i = rows_.find(yy);
    return i != rows_.end() ? i->second.umax : 0;
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
        Holder & hol = i->second;

        // Trim the child.
        if (hol.xmin < xmin && hol.xmax > xmin) {
            if (hol.xmax > xmax) {
                for (int x = xmax; x < hol.xmax; ++x) {
                    auto i = cols_.find(x);

                    if (i != cols_.end()) {
                        Col & col = i->second;
                        if (0 != col.visible && !hol.wp->hidden()) { --col.visible; }
                        unref_col(i);
                    }
                }
            }

            hol.xmax = xmin;
        }

        // Move the child to the right.
        else if (hol.xmin >= xmin) {
            hol.xmin += n_columns;
            hol.xmax += n_columns;
        }
    }

    // Test again after trimming.
    if (cols_.empty()) { goto exit; }
    cmax = cols_.rbegin()->first;
    if (xmin >= cmax) { goto exit; }

    // Move the columns to the right.
    for (int x = cmax; x >= xmin; --x) {
        auto c_iter = cols_.find(x);

        if (c_iter != cols_.end()) {
            Col col(c_iter->second);
            cols_[x+n_columns] = col;
        }
    }

    // Remove columns.
    {
        auto e = cols_.find(xmax);
        for (auto i = cols_.find(xmin); i != e; ++i) { erase_col(i); }
    }
exit:
    update_requisition();
    queue_arrange();
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
        Holder & hol = i->second;

        // Trim the child.
        if (hol.ymin < ymin && hol.ymax > ymin) {
            if (hol.ymax > ymax) {
                for (int y = ymax; y < hol.ymax; ++y) {
                    auto j = rows_.find(y);

                    if (j != rows_.end()) {
                        Row & row = j->second;
                        if (0 != row.visible && !hol.wp->hidden()) { --row.visible; }
                        unref_row(j);
                    }
                }
            }

            hol.ymax = ymin;
        }

        // Move the child down.
        else if (hol.ymin >= ymin) {
            hol.ymin += n_rows;
            hol.ymax += n_rows;
        }
    }

    // Test again after trimming.
    if (rows_.empty()) { goto exit; }
    rmax = rows_.rbegin()->first;
    if (ymin >= rmax) { goto exit; }

    // Move the rows down.
    for (int y = rmax; y >= ymin; --y) {
        auto r_iter = rows_.find(y);

        if (r_iter != rows_.end()) {
            Row row(r_iter->second);
            rows_[y+n_rows] = row;
        }
    }

    // Remove rows.
    {
        auto e = rows_.find(ymax);
        for (auto i = rows_.find(ymin); i != e; ++i) { erase_row(i); }
    }
exit:
    update_requisition();
    queue_arrange();
}

void Table_impl::remove_columns(int xmin, unsigned n_columns) {
    if (0 == n_columns || cols_.empty()) { return; }

    int last = cols_.rbegin()->first;
    if (xmin > last) { return; }

    int xmax = xmin+n_columns;
    std::list<Widget_impl *> rem;

    // Trim and move children.
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;

        // Trim the child.
        if (hol.xmin < xmin && hol.xmax > xmin) {
            if (hol.xmax > xmax) {
                for (int x = xmax; x < hol.xmax; ++x) {
                    auto i = cols_.find(x);

                    if (i != cols_.end()) {
                        Col & col = i->second;
                        if (0 != col.visible && !hol.wp->hidden()) { --col.visible; }
                        unref_col(i);
                    }
                }
            }

            hol.xmax = xmin;
        }

        // Remove child.
        else if (hol.xmin >= xmin && hol.xmin < xmax) {
            rem.push_back(hol.wp);
        }

        // Move the child to the left.
        else if (hol.xmin >= xmax) {
            hol.xmin -= n_columns;
            hol.xmax -= n_columns;
        }
    }

    // Remove children.
    for (Widget_impl * wp: rem) { remove(wp); }

    // Test again after trimming and deletion.
    if (cols_.empty()) { goto exit; }
    last = cols_.rbegin()->first;
    if (xmin >= last) { goto exit; }

    // Move columns to the left.
    for (int x = xmax; x <= last; ++x) {
        auto c_iter = cols_.find(x);

        if (c_iter != cols_.end()) {
            Col col(c_iter->second);
            cols_[x-n_columns] = col;
        }
    }

    // Remove columns.
    {
        auto e = cols_.find(last);
        for (auto i = cols_.find(last-n_columns+1); i != e; ++i) { erase_col(i); }
    }
exit:
    update_requisition();
    queue_arrange();
}

void Table_impl::remove_rows(int ymin, unsigned n_rows) {
    if (0 == n_rows || rows_.empty()) { return; }

    int last = rows_.rbegin()->first;
    if (ymin > last) { return; }

    int ymax = ymin+n_rows;
    std::list<Widget_impl *> rem;

    // Trim and move children.
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;

        // Trim the child.
        if (hol.ymin < ymin && hol.ymax > ymin) {
            if (hol.ymax > ymax) {
                for (int y = ymax; y < hol.ymax; ++y) {
                    auto i = rows_.find(y);

                    if (i != rows_.end()) {
                        Row & row = i->second;
                        if (0 != row.visible && !hol.wp->hidden()) { --row.visible; }
                        unref_row(i);
                    }
                }
            }

            hol.ymax = ymin;
        }

        // Remove child.
        else if (hol.ymin >= ymin && hol.ymin < ymax) {
            rem.push_back(hol.wp);
        }

        // Move the child to the left.
        else if (hol.ymin >= ymax) {
            hol.ymin -= n_rows;
            hol.ymax -= n_rows;
        }
    }

    // Remove children.
    for (auto wp: rem) { remove(wp); }

    // Test again after trimming and deletion.
    if (rows_.empty()) { goto exit; }
    last = rows_.rbegin()->first;
    if (ymin >= last) { goto exit; }

    // Move rows to the top.
    for (int y = ymax; y <= last; ++y) {
        auto r_iter = rows_.find(y);

        if (r_iter != rows_.end()) {
            Row row(r_iter->second);
            rows_[y-n_rows] = row;
        }
    }

    // Remove rows.
    {
        auto e = rows_.find(last);
        for (auto i = rows_.find(last-n_rows+1); i != e; ++i) { erase_row(i); }
    }
exit:
    update_requisition();
    queue_arrange();
}

void Table_impl::select(int xmin, int ymin, unsigned xspan, unsigned yspan) {
    unselect();
    if (rows_.empty() || cols_.empty() || 0 == xspan || 0 == yspan) { return; }
    int xmax = xmin+xspan, ymax = ymin+yspan;

    Table::Span rng = span();
    if (xmax <= rng.xmin || xmin >= rng.xmax || ymax <= rng.ymin || ymin >= rng.ymax) { return; }

    if (xmin < rng.xmin) { xmin = rng.xmin; }
    if (xmax > rng.xmax) { xmax = rng.xmax; }
    if (ymin < rng.ymin) { ymin = rng.ymin; }
    if (ymax > rng.ymax) { ymax = rng.ymax; }
    if (xmax <= xmin || ymax <= ymin) { return; }

    sel_.xmin = xmin;
    sel_.ymin = ymin;
    sel_.xmax = xmax;
    sel_.ymax = ymax;
    Color c = style().color("select/background");
    invalidate(range_bounds(sel_));

    for (auto wp: children_within_range(xmin, ymin, xmax, ymax)) {
        wp->signal_select()();
        wp->style().color("background") = c;
    }

    signal_selection_changed_();
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

        for (auto wp: children_within_range(sel_.xmin, sel_.ymin, sel_.xmax, sel_.ymax)) {
            wp->style().unset("background");
            wp->signal_unselect()();
        }

        signal_selection_changed_();
    }

    sel_.xmin = sel_.ymin = INT_MAX;
    sel_.xmax = sel_.ymax = INT_MIN;
}

Table::Span Table_impl::selection() const {
    Table::Span rng;
    if (sel_.xmax > sel_.xmin && sel_.ymax > sel_.ymin) { rng = sel_; }
    return rng;
}

void Table_impl::mark(int xmin, int ymin, unsigned width, unsigned height) {
    if (rows_.empty() || cols_.empty() || 0 == width || 0 == height) { return; }
    int xmax = xmin+width, ymax = ymin+height;

    Table::Span rng = span();
    if (xmax <= rng.xmin || xmin >= rng.xmax || ymax <= rng.ymin || ymin >= rng.ymax) { return; }

    if (xmin < rng.xmin) { xmin = rng.xmin; }
    if (xmax > rng.xmax) { xmax = rng.xmax; }
    if (ymin < rng.ymin) { ymin = rng.ymin; }
    if (ymax > rng.ymax) { ymax = rng.ymax; }
    if (xmax <= xmin || ymax <= ymin) { return; }

    for (auto & m: marks_) {
        if (m.xmin == xmin && m.xmax == xmax && m.ymin == ymin && m.ymax == ymax) {
            return;
        }
    }

    Table::Span m;
    m.xmin = xmin;
    m.ymin = ymin;
    m.xmax = xmax;
    m.ymax = ymax;
    marks_.push_back(m);

    Color c = style().color("select/background");
    c.darker(0.1);
    invalidate(range_bounds(m));

    for (auto wp: children_within_range(xmin, ymin, xmax, ymax)) {
        wp->signal_select()();
        wp->style().color("background") = c;
    }

    signal_selection_changed_();
}

void Table_impl::mark_column(int x) {
    Table::Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { mark(x, rng.ymin, 1, rng.ymax-rng.ymin); }
}

void Table_impl::mark_row(int y) {
    Table::Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { mark(rng.xmin, y, rng.xmax-rng.xmin, 1); }
}

void Table_impl::unmark(int xmin, int ymin, int xmax, int ymax) {
    for (auto iter = marks_.begin(); iter != marks_.end(); ++iter) {
        Table::Span & m = *iter;

        if (m.xmin == xmin && m.ymin == ymin && m.xmax == xmax && m.ymax == ymax) {
            invalidate(range_bounds(m));

            for (auto wp: children_within_range(xmin, ymin, xmax, ymax)) {
                wp->style().unset("background");
                wp->signal_unselect()();
            }

            marks_.erase(iter);
            signal_selection_changed_();
            return;
        }
    }
}

void Table_impl::unmark_column(int x) {
    Table::Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { unmark(x, rng.ymin, x+1, rng.ymax); }
}

void Table_impl::unmark_row(int y) {
    Table::Span rng = span();
    if (rng.xmax > rng.xmin && rng.ymax > rng.ymin) { unmark(rng.xmin, y, rng.xmax, y+1); }
}

void Table_impl::unmark_all() {
    bool changed = false;

    for (auto & m: marks_) {
        changed = true;
        invalidate(range_bounds(m));

        for (auto wp: children_within_range(m.xmin, m.ymin, m.xmax, m.ymax)) {
            wp->style().unset("background");
            wp->signal_unselect()();
        }
    }

    if (changed) { signal_selection_changed_(); }
    marks_.clear();
}

std::vector<Table::Span> Table_impl::marks() const {
    std::vector<Table::Span> v;
    for (auto & rng: marks_) { v.push_back(rng); }
    return v;
}

bool Table_impl::on_take_focus() {
//     std::cout << this << " tf: " << focused_child_ << '\n';
    if (focused_child_ && focused_child_->take_focus()) {
        return true;
    }

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;

        if (hol.wp->take_focus()) {
//             std::cout << "  " << this << " TF: " << hol.wp << '\n';
            return true;
        }
    }

    return grab_focus();
}

// Overriden by List_impl.
// Overriden by List_text_impl.
void Table_impl::clear() {
    unmark_all();
    unselect();

    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = i->second;
        unparent_child(hol.wp);
        hol.wp->update_origin(INT_MIN, INT_MIN);
        hol.wp->update_size(0, 0);
    }

    holders_.clear();
    cols_.clear();
    rows_.clear();
    require_size(0);
    invalidate();
}

void Table_impl::on_async_clear() {
    if (holders_.empty()) {
        async_clear_cx_.disconnect();
        cols_.clear();
        rows_.clear();
        require_size(0);
        invalidate();
        return;
    }

    Holder & hol = holders_.begin()->second;
    holders_.erase(holders_.begin());
    unparent_child(hol.wp);
    hol.wp->update_origin(INT_MIN, INT_MIN);
    hol.wp->update_size(0, 0);

    std::cout << this << " Table_impl::on_async_clear(): wp=" << hol.wp << ", " << holders_.size() << " remaining\n";
}

} // namespace tau

//END
