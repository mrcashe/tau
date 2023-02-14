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
#include <tau/font.hh>
#include <tau/loop.hh>
#include <tau/painter.hh>
#include <tau/string.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <painter-impl.hh>
#include <scroller-impl.hh>
#include <text-impl.hh>
#include <iostream>

namespace tau {

static const unsigned CARET_TIMEOUT = 511;

Text_impl::Text_impl():
    Widget_impl(),
    caret_visible_(false),
    caret_exposed_(false),
    caret_refresh_(false),
    caret_enabled_(false),
    select_allowed_(false)
{
    init();
}

Text_impl::Text_impl(Align xalign, Align yalign):
    Widget_impl(),
    caret_visible_(false),
    caret_exposed_(false),
    caret_refresh_(false),
    caret_enabled_(false),
    select_allowed_(false),
    xalign_(xalign),
    yalign_(yalign)
{
    init();
}

Text_impl::Text_impl(const ustring & text, Align xalign, Align yalign):
    Widget_impl(),
    caret_visible_(false),
    caret_exposed_(false),
    caret_refresh_(false),
    caret_enabled_(false),
    select_allowed_(false),
    xalign_(xalign),
    yalign_(yalign)
{
    init();
    assign(text);
}

Text_impl::Text_impl(Buffer buf, Align xalign, Align yalign):
    Widget_impl(),
    buffer_(buf),
    caret_visible_(false),
    caret_exposed_(false),
    caret_refresh_(false),
    caret_enabled_(false),
    select_allowed_(false),
    xalign_(xalign),
    yalign_(yalign)
{
    init();
}

Text_impl::~Text_impl() {
    if (actions_) { delete actions_; }
    if (signal_selection_changed_) { delete signal_selection_changed_; }
    if (signal_caret_motion_) { delete signal_caret_motion_; }
    if (signal_click_) { delete signal_click_; }
}

void Text_impl::init() {
    fonts_.emplace_back(Font());

    auto & fi = style_.get(STYLE_FONT);
    fi.signal_changed().connect(fun(this, &Text_impl::update_font));
    fi.signal_changed().connect(fun(this, &Text_impl::calc_rows));
    fi.signal_changed().connect(fun(this, &Text_impl::refresh_caret));
    fi.signal_changed().connect(fun(this, &Text_impl::scroll_to_caret));
    fi.signal_changed().connect(bind(fun(this, &Widget_impl::invalidate), Rect()));

    style_.get(STYLE_FOREGROUND).signal_changed().connect(bind(fun(this, &Widget_impl::invalidate), Rect()));

    signal_origin_changed_.connect(fun(this, &Text_impl::calc_rows), true);

    signal_size_changed_.connect(fun(this, &Text_impl::update_va));
    signal_size_changed_.connect(fun(this, &Text_impl::calc_rows));
    signal_size_changed_.connect(fun(this, &Text_impl::refresh_caret));
    signal_size_changed_.connect(fun(this, &Text_impl::scroll_to_caret));

    signal_visible_.connect(fun(this, &Text_impl::update_va));

    signal_invisible_.connect(fun(this, &Text_impl::update_va));

    signal_display_.connect(fun(this, &Text_impl::on_display));
    signal_focus_in_.connect(fun(this, &Text_impl::on_focus_in));
    signal_focus_out_.connect(fun(this, &Text_impl::hide_caret));

    signal_paint_.connect(fun(this, &Text_impl::on_paint));
    signal_take_focus_.connect(fun(this, &Text_impl::on_take_focus));

    init_buffer();
}

void Text_impl::on_display() {
    update_font();
    calc_rows();
    refresh_caret();
    scroll_to_caret();
    if (scrollable()) { signal_scroll_changed().connect(fun(this, &Text_impl::update_va)); }
}

bool Text_impl::empty() const {
    return buffer_.empty();
}

void Text_impl::update_font() {
    if (auto pr = priv_painter()) {
        fonts_.front() = pr.select_font(style_.font(STYLE_FONT).spec());

        if (fonts_.front()) {
            font_height_ = std::ceil(fonts_.front().ascent())+std::ceil(std::fabs(fonts_.front().descent()));
            space_width_ = std::ceil(pr.text_size(U" ").x());
        }
    }
}

bool Text_impl::on_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        unselect();
        auto i = iter_from_point(pt);
        if (select_allowed_) { enable_caret(); msel_ = i; }
        if (caret_enabled_) { move_to(i); }
        grab_focus();
        if (signal_click_) { (*signal_click_)(); }
    }

    return false;
}

bool Text_impl::on_mouse_up(int mbt, int mm, const Point & pt) {
    ungrab_mouse();

    if (MBT_LEFT == mbt) {
        msel_.reset();
        emsel_.reset();
    }

    return false;
}

void Text_impl::on_mouse_motion(int mm, const Point & pt) {
    if (select_allowed_ && (MM_LEFT & mm) && msel_) {
        Buffer_citer i = iter_from_point(pt);

        if (msel_ != i) {
            if (!sel_ && !esel_) {
                emsel_ = i;
                update_selection(msel_, emsel_);
                grab_mouse();
            }

            else if (emsel_ && emsel_ != i) {
                update_selection(emsel_, i);
                emsel_ = i;
            }

            move_to(i);
        }
    }
}

void Text_impl::on_focus_in() {
    if (caret_enabled_) {
        show_caret();
    }
}

Painter Text_impl::wipe_area(int x1, int y1, int x2, int y2, Painter pr) {
    if (!pr) { pr = priv_painter(); }

    if (pr) {
        pr.push();
        pr.clear();
        pr.rectangle(x1, y1, x2, y2);
        pr.set_brush(Brush(style_.color("background")));
        pr.fill();
        pr.pop();
    }

    return pr;
}

void Text_impl::on_buffer_replace(Buffer_citer b, Buffer_citer e, const std::u32string & replaced) {
    if (b.row() == e.row()) {
        auto i = rows_.begin()+b.row();
        int h0 = i->ascent_+i->descent_;
        load_rows(i, i);
        calc_row(i);
        int h1 = i->ascent_+i->descent_;
        if (i->width_ < text_width_) { text_width_ = calc_width(i, rows_.end()); }
        e.move_to_eol();
        if (h1 != h0) { translate_rows(i+1, rows_.end(), h1-h0); e = buffer_.cend(); }
        update_requisition();
        align_rows(i, i);
        update_range(b, e);
    }
}

void Text_impl::on_buffer_replace_move(Buffer_citer b, Buffer_citer e, const std::u32string & replaced) {
    move_to(e);
    hint_x();
}

void Text_impl::on_buffer_erase(Buffer_citer b, Buffer_citer e, const std::u32string & erased) {
    if (e < b) { std::swap(b, e); }

    if (e.row() >= rows_.size() || buffer_.empty()) {
        clear();
        return;
    }

    auto i = rows_.begin()+b.row();

    if (e.row() == b.row()) {
        int h0 = i->ascent_+i->descent_;
        int y1 = oy_+i->ybase_-i->ascent_;
        int y2 = oy_+i->ybase_+i->descent_;

        load_rows(i, i);
        calc_row(i);
        int h1 = i->ascent_+i->descent_;
        y1 = std::min(y1, oy_+i->ybase_-i->ascent_);
        y2 = std::max(y2, oy_+i->ybase_+i->descent_);

        if (xalign_ != ALIGN_START) {
            wipe_area(va_.left(), y1, va_.right(), y2);
            b.move_to_sol();
        }

        if (i->width_ < text_width_) { text_width_ = calc_width(rows_.begin(), rows_.end()); }
        e.move_to_eol();
        if (h1 != h0) { translate_rows(i+1, rows_.end(), h1-h0); e = buffer_.cend(); }
    }

    else {
        auto j = rows_.begin()+e.row();
        int hdel = calc_height(i+1, j);
        rows_.erase(i, j);
        translate_rows(i, rows_.end(), -hdel);
        load_rows(i, j);
        calc_rows();
        e = buffer_.cend();
        wipe_area(va_.x(), i->ybase_-i->ascent_, va_.right(), va_.bottom());
    }

    update_requisition();
    bool aligned = align_rows(rows_.begin(), rows_.end());
    if (aligned) { b.move_to_sol(); e.move_to_eol(); }
    update_range(b, e);
}

void Text_impl::on_buffer_erase_move(Buffer_citer b, Buffer_citer e, const std::u32string & erased) {
    move_to(b);
    hint_x();
}

void Text_impl::on_buffer_insert(Buffer_citer b, Buffer_citer e) {
    insert_range(b, e);
}

void Text_impl::on_buffer_insert_move(Buffer_citer b, Buffer_citer e) {
    move_to(e);
    hint_x();
}

void Text_impl::insert_range(Buffer_citer b, Buffer_citer e) {
    if (rows_.empty()) { rows_.emplace_back(); }
    if (e < b) { std::swap(b, e); }
    std::size_t nlines = e.row()-b.row();
    while (nlines--) { rows_.emplace(rows_.begin()+b.row()); }
    load_rows(rows_.begin()+b.row(), rows_.begin()+e.row());
    if (e.row() > b.row()) { e = buffer_.cend(); }
    if (ALIGN_START != xalign_) { b.move_to_sol(); e.move_to_eol(); }
    calc_rows();
    update_range(b, e);
}

// Overriden by Edit_impl.
void Text_impl::assign(Buffer buf) {
    clear();
    buffer_ = buf;
    init_buffer();
}

// Overriden by Edit_impl.
void Text_impl::assign(const ustring & s) {
    clear();
    buffer_.assign(s);
    if (caret_ >= buffer_.cend()) { move_to(buffer_.cend()); }
}

// Overriden by Edit_impl.
void Text_impl::clear() {
    wipe_caret();

    if (auto pr = priv_painter()) {
        pr.push(); pr.clear();
        pr.set_brush(Brush(style_.color(STYLE_BACKGROUND)));
        pr.paint(); pr.pop();
    }

    buffer_.clear();
    rows_.clear();
    sel_.reset();
    esel_.reset();
    msel_.reset();
    emsel_.reset();
    update_requisition();
    move_to(buffer_.cbegin());
}

// Overriden by Edit_impl.
void Text_impl::init_buffer() {
    insert_cx_ = buffer_.signal_insert().connect(fun(this, &Text_impl::on_buffer_insert));
    replace_cx_ = buffer_.signal_replace().connect(fun(this, &Text_impl::on_buffer_replace));
    erase_cx_ = buffer_.signal_erase().connect(fun(this, &Text_impl::on_buffer_erase));
    insert_move_cx_ = buffer_.signal_insert().connect(fun(this, &Text_impl::on_buffer_insert_move));
    replace_move_cx_ = buffer_.signal_replace().connect(fun(this, &Text_impl::on_buffer_replace_move));
    erase_move_cx_ = buffer_.signal_erase().connect(fun(this, &Text_impl::on_buffer_erase_move));
    insert_range(buffer_.cbegin(), buffer_.cend());
    caret_ = buffer_.cbegin();
    xhint_ = 0;
    refresh_caret();
    scroll_to_caret();
}

void Text_impl::hint_x() {
    xhint_ = x_at_col(caret_.row(), caret_.col());
}

ustring Text_impl::text() const {
    return buffer_.text();
}

void Text_impl::set_spacing(unsigned spc) {
    if (spacing_ != spc) {
        spacing_ = spc;
        if (align_rows(rows_.begin(), rows_.end())) { invalidate(); }
    }
}

void Text_impl::set_text_align(Align xalign, Align yalign) {
    if (xalign_ != xalign || yalign_ != yalign) {
        xalign_ = xalign;
        yalign_ = yalign;
        align_all();
    }
}

bool Text_impl::align_rows(R_iter first, R_iter last) {
    bool changed = false;

    if (va_) {
        if (last < first) { std::swap(first, last); }
        int h = va_.height(), w = va_.width(), ey = h-text_height_, oy = 0;

        if (ey > 0) {
            if (ALIGN_CENTER == yalign_) { oy = ey/2; }
            else if (ALIGN_END == yalign_) { oy = ey; }
        }

        if (oy_ != oy) { oy_ = oy; changed = true; }

        for (; first != rows_.end() && first <= last; ++first) {
            int ex = w-first->width_, ox = 0;

            if (ex > 0) {
                if (ALIGN_CENTER == xalign_) { ox = ex/2; }
                else if (ALIGN_END == xalign_) { ox = ex; }
            }

            if (first->ox_ != ox) { first->ox_ = ox; changed = true; }
        }
    }

    return changed;
}

void Text_impl::align_all() {
    if (align_rows(rows_.begin(), rows_.end())) {
        invalidate();
    }
}

void Text_impl::translate_rows(R_iter first, R_iter last, int dy) {
    if (last < first) { std::swap(first, last); }
    for (; first != rows_.end() && first <= last; ++first) { first->ybase_ += dy; }
}

void Text_impl::update_requisition() {
    Size req(text_width_, text_height_);
    if (buffer_.empty()) { req = text_size("|"); }
    if (caret_enabled_) { req.increase(text_size("|").width(), 0); }
    require_size(req);
}

void Text_impl::update_selection(Buffer_citer i, Buffer_citer j) {
    if (select_allowed_ && i && j) {
        if (!sel_ && !esel_) {
            sel_ = i;
            esel_ = j;
            if (esel_ < sel_) { std::swap(sel_, esel_); }
            update_range(sel_, esel_);
        }

        else if (i != j && i == esel_) {
            esel_ = j;
            if (esel_ < sel_) { std::swap(sel_, esel_); }

            if (esel_ == sel_) {
                unselect();
            }

            else if (j < i) {
                update_range(sel_, i);
            }

            else {
                update_range(sel_, j);
            }
        }

        else if (i != j && i == sel_) {
            sel_ = j;
            if (esel_ < sel_) { std::swap(sel_, esel_); }

            if (esel_ == sel_) {
                unselect();
            }

            else if (j < i) {
                update_range(j, esel_);
            }

            else {
                update_range(i, esel_);
            }
        }

        if (signal_selection_changed_) { signal_selection_changed_->operator()(); }
    }
}

void Text_impl::update_range(Buffer_citer b, Buffer_citer e) {
    if (b && e && b.row() < rows_.size() && e.row() < rows_.size()) {
        if (e < b) { std::swap(b, e); }
        if (0 == e.row()) { --e; }

        auto & row1 = rows_[b.row()], row2 = rows_[e.row()];
        int y1 = oy_+row1.ybase_-row1.ascent_;
        int y2 = oy_+row2.ybase_+row2.descent_;
        if (e.row() >= rows_.size()-1) { y2 = va_.bottom(); }
        int x1 = va_.left(), x2 = va_.right();

        if (b.row() == e.row()) {
            x1 = x_at_col(b.row(), b.col());
            x2 = x_at_col(e.row(), e.col());
        }

        redraw(Rect(x1, y1, x2, y2));
    }
}

void Text_impl::load_rows(R_iter first, R_iter last) {
    for (; first != rows_.end() && first <= last; ++first) {
        first->frags_.clear();
        auto rn = first-rows_.begin();
        auto b = buffer_.citer(rn, 0), e = b; e.move_to_eol();
        first->ncols_ = b.length(e);
        first->poss_.assign(first->ncols_, 0);
        first->frags_.emplace_back();
        Frag & frag = first->frags_.back();
        frag.start_ = 0;
        frag.ncols_ = first->ncols_;
    }
}

void Text_impl::calc_row(R_iter i, Painter pr) {
    if (!pr) { pr = priv_painter(); }
    i->ascent_ = 0;
    i->descent_ = 0;
    i->width_ = 0;
    i->ox_ = 0;
    int x = 0;

    if (pr) {
        std::size_t pos = 0;
        auto rn = i-rows_.begin();
        auto b = buffer_.citer(rn, 0), e = b; b.move_to_col(i->ncols_);
        std::u32string s = buffer_.text32(b, e);

        for (Frag & frag: i->frags_) {
            Font font = fonts_[frag.font_];
            pr.set_font(font);
            i->ascent_ = std::max(i->ascent_, int(std::ceil(font.ascent())));
            i->descent_ = std::max(i->descent_, int(std::ceil(fabs(font.descent()))));
            std::u32string acc;

            for (std::size_t j = 0; j < frag.ncols_; ++j) {
                i->poss_[j+frag.start_] = x+std::ceil(pr.text_size(acc).x());
                char32_t c = s[j+frag.start_];

                if (0x0009 == c) {
                    std::size_t n_spaces = tab_width_-(pos%tab_width_);
                    acc.append(n_spaces, U' ');
                    pos += n_spaces;
                }

                else {
                    acc.append(1, c);
                    ++pos;
                }
            }

            frag.width_ = std::ceil(pr.text_size(acc).x());
            x += frag.width_;
        }

        i->width_ = x;
        i->ellipsized_.clear();

        if (0 != ellipsis_width_ && va_.iwidth() >= ellipsis_width_ && i->ncols_ > 1) {
            if (i->width_ > va_.iwidth()) {
                std::size_t col = 1;
                int w = va_.width()-ellipsis_width_;

                if (WRAP_ELLIPSIZE_START == wrap_) {
                    i->ellipsized_ += ellipsis_;
                    int skip = i->width_-w, skipped = 0;
                    while (skipped < skip && col < i->ncols_) { skipped = i->poss_[col++]; }
                    i->ellipsized_ += s.substr(col-1);
                }

                else if (WRAP_ELLIPSIZE_CENTER == wrap_) {
                    int lpart = w/2, wnext = 0;

                    while (wnext < lpart && col < i->ncols_) {
                        wnext = i->poss_[col];
                        if (wnext < lpart) { i->ellipsized_ += s[col-1]; }
                        ++col;
                    }

                    i->ellipsized_ += ellipsis_;
                    col = i->ncols_-1;
                    int rpart = i->width_-(w/2), rpos = i->width_;

                    while (rpos > rpart && 0 != col) {
                        rpos = i->poss_[col];
                        if (rpos > rpart) { --col; }
                    }

                    if (col < i->ncols_-1) { ++col; }
                    i->ellipsized_ += s.substr(col);
                }

                // Assume WRAP_ELLIPSIZE_END.
                else {
                    int wnext = 0;

                    while (wnext < w && col < i->ncols_) {
                        wnext = i->poss_[col];
                        if (wnext < w) { i->ellipsized_ += s[col-1]; }
                        ++col;
                    }

                    i->ellipsized_ += ellipsis_;
                }
            }
        }
    }
}

void Text_impl::calc_rows() {
    int ybase = 0;
    text_height_ = text_width_ = 0;
    auto pr = priv_painter();

    for (auto i = rows_.begin(); i != rows_.end(); ++i) {
        calc_row(i, pr);
        i->ybase_ = ybase+i->ascent_;
        ybase += i->ascent_+i->descent_+spacing_;
        text_height_ += i->ascent_+i->descent_;
        if (i+1 < rows_.end()) { text_height_ += spacing_; }
        text_width_ = std::max(text_width_, i->width_);
    }

    update_requisition();
    align_all();
}

int Text_impl::calc_height(R_citer first, R_citer last) {
    if (last < first) { std::swap(first, last); }
    int h = 0;

    for (; first != rows_.end() && first <= last; ) {
        h += first->ascent_+first->descent_;
        if (++first != rows_.end()) { h += spacing_; }
    }

    return h;
}

int Text_impl::calc_width(R_citer first, R_citer last) {
    if (last < first) { std::swap(first, last); }
    int w = 0;

    for (; first != rows_.end() && first <= last; ++first) {
        w = std::max(w, first->width_);
    }

    return w;
}

void Text_impl::update_caret() {
    rcaret_.reset();
    int x1 = 0, x2 = 0, y1 = oy_, y2 = y1;

    if (!buffer_.empty()) {
        if (caret_.row() < rows_.size()) {
            const Row & row = rows_[caret_.row()];
            x1 = x_at_col(row, caret_.col());
            y1 += row.ybase_-row.ascent_;
            y2 += row.ybase_+row.descent_;

            if (!insert_ && caret_.col() < row.ncols_) {
                x2 = std::max(x2, x_at_col(row, caret_.col()+1));
            }
        }
    }

    else {
        if (ALIGN_END == xalign_) {
            x1 = va_.width();
        }

        else if (ALIGN_CENTER == xalign_) {
            x1 = va_.width()/2;
        }

        if (ALIGN_END == yalign_) {
            y1 += va_.height()-font_height_;
        }

        else if (ALIGN_CENTER == yalign_) {
            y1 += (va_.height()-font_height_)/2;
        }
    }

    x2 = std::max(x2, x1+3);
    if (y1 == y2) { y2 += font_height_; }
    rcaret_.update_width(x2-x1);
    rcaret_.update_origin(x1, y1);
    rcaret_.update_height(y2-y1);
    ccaret_ = style().get(STYLE_FOREGROUND).get();
}

void Text_impl::draw_caret(Painter pr) {
    if (pr && rcaret_) {
        pr.push();
        pr.clear();
        pr.rectangle(rcaret_.left(), rcaret_.top(), rcaret_.right(), rcaret_.bottom());
        pr.set_brush(ccaret_);
        pr.set_oper(OPER_NOT);
        pr.fill();
        pr.pop();
    }
}

void Text_impl::on_caret_timer() {
    if (!caret_refresh_) {
        if (caret_exposed_) { wipe_caret(); }
        else { expose_caret(); }
    }

    caret_refresh_ = false;
}

void Text_impl::refresh_caret() {
    if (caret_enabled_ && caret_visible_) {
        caret_refresh_ = true;
        wipe_caret();
        update_caret();
        expose_caret();
    }
}

void Text_impl::expose_caret() {
    if (caret_visible_ && visible()) {
        caret_exposed_ = true;
        draw_caret(priv_painter());
    }
}

void Text_impl::wipe_caret() {
    if (caret_exposed_) {
        caret_exposed_ = false;
        draw_caret(priv_painter());
    }
}

void Text_impl::scroll_to_caret() {
    if (caret_enabled_ && !buffer_.empty() && caret_.row() < rows_.size() && va_) {
        Point ofs(va_.origin());
        const Row & row = rows_[caret_.row()];
        int y1 = oy_+row.ybase_-row.ascent_;
        int y2 = oy_+row.ybase_+row.descent_;
        int x1 = x_at_col(caret_.row(), caret_.col());
        int x2 = x1+8;

        if (y1 < ofs.y()) {
            ofs.update_y(y1);
        }

        else if (y2 >= va_.bottom()) {
            ofs.update_y(y2-va_.height());
        }

        if (x1 < ofs.x()) {
            int x = x1, w = va_.width(), d = w/8;
            if (x > d) { x -= d; }
            ofs.update_x(x < w ? 0 : x);
        }

        else if (x2 > va_.right()) {
            int w = va_.width(), d = w/8, x = x1+d;
            if (x >= text_width_) { x = text_width_+text_size("W").width(); }
            ofs.update_x(x-va_.width());
        }

        if (ofs != va_.origin()) { scroll_to(ofs); }
    }
}

void Text_impl::move_to(std::size_t row, std::size_t col) {
    if (caret_enabled_) {
        auto c(caret_);
        caret_.move_to(row, col);

        if (caret_ != c) {
            if (signal_caret_motion_) { (*signal_caret_motion_)(); }
            refresh_caret();
            scroll_to_caret();
        }
    }
}

void Text_impl::move_to(const Buffer_citer i) {
    move_to(i.row(), i.col());
}

Buffer_citer Text_impl::caret() const {
    return caret_;
}

Buffer_citer Text_impl::iter(std::size_t row, std::size_t col) const {
    return Buffer_citer(caret_, row, col);
}

void Text_impl::enable_caret() {
    if (!caret_enabled_) {
        unselect();
        caret_enabled_ = true;
        init_actions();
        allow_focus();
        update_requisition();
        refresh_caret();
    }
}

void Text_impl::disable_caret() {
    if (caret_enabled_) {
        caret_enabled_ = false;
        caret_cx_.drop();
        hide_caret();
        unselect();
        disallow_focus();
        update_requisition();
    }
}

void Text_impl::show_caret() {
    if (!caret_visible_) {
        caret_visible_ = true;
        if (!loop_) { loop_ = Loop_impl::this_loop(); }
        if (loop_) { caret_cx_ = loop_->signal_alarm(CARET_TIMEOUT, true).connect(fun(this, &Text_impl::on_caret_timer)); }
        refresh_caret();
    }
}

void Text_impl::hide_caret() {
    if (caret_visible_) {
        if (caret_exposed_) { wipe_caret(); }
        caret_cx_.drop();
        caret_visible_ = false;
    }
}

std::size_t Text_impl::hinted_pos(std::size_t ri) {
    int x1 = 0;

    if (xhint_ > 0 && ri < rows_.size()) {
        auto & row = rows_[ri];

        for (std::size_t n = 1; n < row.ncols_; ++n) {
            int x2 = x_at_col(row, n);

            if (x2 >= xhint_) {
                std::size_t col = x2-xhint_ < xhint_-x1 ? n : n-1;
                Buffer_citer c(caret_, ri, col);
                if (char32_is_modifier(*c)) { ++c; }
                return c.col();
            }

            else {
                x1 = x2;
            }
        }

        return row.ncols_;
    }

    return x1;
}

int Text_impl::x_at_col(const Row & row, std::size_t col) const {
    return row.ox_+(col < row.ncols_ ? row.poss_[col] : row.width_);
}

int Text_impl::x_at_col(std::size_t ri, std::size_t col) const {
    if (ri < rows_.size()) {
        return x_at_col(rows_[ri], col);
    }

    return 0;
}

std::size_t Text_impl::col_at_x(const Row & row, int x) const {
    std::size_t col = 0;
    x -= row.ox_;

    if (x > 0) {
        if (x >= row.width_) {
            col = row.ncols_;
        }

        else {
            for (std::size_t n = 0; n < row.ncols_; ++n, ++col) {
                int x0 = row.poss_[n];
                int x1 = n+1 < row.ncols_ ? row.poss_[n+1] : row.width_;
                if (x >= x0 && x < x1) { break; }
            }
        }
    }

    return col;
}

std::size_t Text_impl::col_at_x(std::size_t ri, int x) const {
    if (ri < rows_.size()) {
        return col_at_x(rows_[ri], x);
    }

    return 0;
}

std::size_t Text_impl::row_at_y(int y) const {
    if (!rows_.empty() && y >= 0) {
        auto i = std::find_if(rows_.begin(), rows_.end(), [y](auto & row) { return row.ybase_+row.descent_ >= y; } );
        if (i != rows_.end()) { return i-rows_.begin(); }
        return rows_.size()-1;
    }

    return 0;
}

int Text_impl::baseline(std::size_t ri) const {
    return ri < rows_.size() ? rows_[ri].ybase_ : 0;
}

void Text_impl::get_row_bounds(std::size_t rn, int & top, int & bottom) const {
    top = bottom = 0;

    if (rn < rows_.size()) {
        const Row & row = rows_[rn];
        top = oy_+row.ybase_-row.ascent_;
        bottom = oy_+row.ybase_+row.descent_;
    }
}

Buffer_citer Text_impl::iter_from_point(const Point & pt) {
    std::size_t row = row_at_y(pt.y());
    std::size_t col = col_at_x(row, pt.x());
    return iter(row, col);
}

void Text_impl::paint_ellipsized(const Row & row, Painter pr) {
    int ybase = oy_+row.ybase_;
    wipe_area(va_.left(), ybase-row.ascent_, va_.right(), ybase+row.descent_, pr);
    pr.move_to(0, ybase);
    select_font(pr);
    pr.text(row.ellipsized_, enabled() ? style_.color(STYLE_FOREGROUND) : style_.color(STYLE_BACKGROUND).get().inactive());
    pr.stroke();
}

void Text_impl::paint_row(R_citer ri, std::size_t pos, Painter pr) {
    pos = std::min(pos, ri->ncols_);
    std::size_t scol = std::max(pos, col_at_x(*ri, va_.left()));            // Starting column.
    std::size_t ecol = std::min(ri->ncols_, 1+col_at_x(*ri, va_.right()));   // Ending column.
    std::size_t rn = ri-rows_.begin();

    // Include modifiers before starting column.
    for (Buffer_citer i = buffer_.citer(rn, scol); scol > 0; --scol, --i) {
        if (!char32_is_modifier(*i)) { break; }
    }

    // Include modifiers after ending column.
    for (Buffer_citer i = buffer_.citer(rn, ecol); i.col() < ri->ncols_; ++ecol, ++i) {
        if (!char32_is_modifier(*i)) { break; }
    }

    Color bg = style().color(STYLE_BACKGROUND);

    if (sel_ && esel_ && (rn > sel_.row() || (rn == sel_.row() && scol >= sel_.col())) && rn <= esel_.row()) {
        bg = style().color(STYLE_SELECT_BACKGROUND);
    }

    std::size_t col0 = 0;
    int ybase = oy_+ri->ybase_;
    int y1 = ybase-ri->ascent_;
    int y2 = ybase+ri->descent_;

    // Loop over fragmens.
    for (const Frag & frag: ri->frags_) {
        std::size_t fend = col0+frag.ncols_;
        std::size_t col = std::max(scol, col0);

        if (fend > scol) {
            auto b = buffer_.citer(rn, 0), e = buffer_.citer(rn, ri->ncols_);
            const std::u32string s = buffer_.text32(b, e);
            std::size_t col1 = std::min(ecol, fend);

            // Loop inside of fragment.
            while (col < col1) {
                std::size_t col2 = col1;
                select_font(pr);

                if (sel_ && esel_) {
                    if (sel_.row() == rn && col < sel_.col()) {
                        col2 = std::min(col1, sel_.col());
                    }

                    else if (sel_.row() == rn && col == sel_.col()) {
                        bg = style_.color(STYLE_SELECT_BACKGROUND);
                        if (esel_.row() == rn && col < esel_.col()) { col2 = std::min(col1, esel_.col()); }
                    }

                    else if (esel_.row() == rn && col < esel_.col()) {
                        bg = style_.color(STYLE_SELECT_BACKGROUND);
                        col2 = std::min(col1, esel_.col());
                    }

                    else if (esel_.row() == rn && col >= esel_.col()) {
                        bg = style_.color(STYLE_BACKGROUND);
                    }
                }

                std::size_t tab = s.find(U'\x0009', col);
                if (tab > col && tab < col2) { col2 = tab; }

                if (col2 > col) {
                    int x1 = x_at_col(*ri, col), x2 = x_at_col(*ri, col2);
                    pr.rectangle(x1, y1, x2, y2);
                    pr.set_brush(bg);
                    pr.fill();

                    if (U'\x0009' != s[col]) {
                        Color c = enabled() ? style_.color(STYLE_FOREGROUND) : style_.color(STYLE_BACKGROUND).get().inactive();
                        auto ppr = strip(pr);
                        ppr->move_to(x1, ybase);
                        ppr->text(s.substr(col-col0, col2-col), c);
                        ppr->stroke();
                        col += col2-col;
                    }

                    else {
                        ++col;
                    }
                }
            }
        }

        col0 += frag.ncols_;
        if (col0 > ecol) { break; }
    }

    // Draw an empty rectangle after EOL.
    if (ecol >= ri->ncols_) {
        if (esel_ && esel_.row() == rn && esel_.col() <= ri->ncols_) { bg = style().color(STYLE_BACKGROUND); }
        pr.rectangle(ri->ox_+ri->width_, y1, va_.right(), y2);
        pr.set_brush(bg);
        pr.fill();
    }
}

void Text_impl::redraw(const Rect & r, Painter pr) {
    if (!pr) { pr = priv_painter(); }

    if (pr && va_ && r) {
        bool caret_exposed = caret_exposed_;
        wipe_caret();
        pr.push();

        R_iter b = std::find_if(rows_.begin(), rows_.end(), [r](auto & row) { return row.ybase_+row.descent_ >= r.top(); } );
        R_iter e = std::find_if(b, rows_.end(), [r](auto & row) { return row.ybase_-row.ascent_ > r.bottom(); } );

        for (; b != e && b != rows_.end(); ++b) {
            if (!b->ellipsized_.empty()) { paint_ellipsized(*b, pr); }
            else { paint_row(b, col_at_x(*b, r.x()), pr); }
        }

        pr.pop();
        if (caret_exposed) { expose_caret(); }
    }
}

bool Text_impl::on_paint(Painter pr, const Rect & r) {
    redraw(r, pr);
    return true;
}

Font Text_impl::select_font(Painter pr) {
    Font font;

    if (pr) {
        if (fonts_.front()) { font = fonts_.front(); pr.set_font(font); }
        else { font = pr.select_font(style_.font(STYLE_FONT)); }
    }

    return font;
}

void Text_impl::set_wrap_mode(Wrap_mode wrap_mode) {
    if (wrap_ != wrap_mode) {
        wrap_ = wrap_mode;
        calc_rows();
        invalidate();
    }
}

void Text_impl::allow_select() {
    if (!select_allowed_) {
        select_allowed_ = true;
        unselect();
        set_cursor("text:ibeam");
        mouse_down_cx_ = signal_mouse_down().connect(fun(this, &Text_impl::on_mouse_down));
        mouse_up_cx_ = signal_mouse_up().connect(fun(this, &Text_impl::on_mouse_up));
        mouse_motion_cx_ = signal_mouse_motion().connect(fun(this, &Text_impl::on_mouse_motion));
    }
}

void Text_impl::disallow_select() {
    if (select_allowed_) {
        unselect();
        select_allowed_ = false;
        mouse_down_cx_.drop();
        mouse_up_cx_.drop();
        mouse_motion_cx_.drop();
        disable_caret();
        unset_cursor();
    }
}

bool Text_impl::has_selection() const {
    return (sel_ && esel_);
}

ustring Text_impl::selection() const {
    if (sel_ && esel_) {
        return buffer_.text(sel_, esel_);
    }

    return ustring();
}

void Text_impl::select(Buffer_citer b, Buffer_citer e) {
    unselect();

    if (select_allowed_) {
        if (buffer_.cend() < b) {
            b = buffer_.cend();
        }

        if (e < b) { std::swap(b, e); }

        if (buffer_.cend() < e) {
            e = buffer_.cend();
        }

        for (; b != buffer_.cbegin(); --b) {
            if (!char32_is_modifier(*b)) { break; }
        }

        for (; e != buffer_.cend(); ++e) {
            if (!char32_is_modifier(*e)) { break; }
        }

        sel_ = b;
        esel_ = e;
        update_range(b, e);
        if (signal_selection_changed_) { signal_selection_changed_->operator()(); }
    }
}

void Text_impl::select_all() {
    if (!buffer_.empty()) {
        select(buffer_.cbegin(), buffer_.cend());
    }
}

void Text_impl::unselect() {
    if (sel_ && esel_) {
        Buffer_citer b(sel_), e(esel_);
        sel_.reset(); esel_.reset();
        update_range(b, e);
        if (signal_selection_changed_) { signal_selection_changed_->operator()(); }
    }
}

std::size_t Text_impl::rows() const {
    return buffer_.rows();
}

Painter Text_impl::priv_painter() {
    return ppr_ ? ppr_ : painter();
}

Size Text_impl::text_size(const ustring & s) {
    Size size;

    if (!s.empty()) {
        if (Painter pr = priv_painter()) {
            Vector v = pr.text_size(s);
            size.set(std::ceil(v.x()), std::ceil(pr.font().ascent())+std::ceil(std::fabs(pr.font().descent())));
        }
    }

    return size;
}

Size Text_impl::text_size(const std::u32string & s) {
    Size size;

    if (!s.empty()) {
        if (Painter pr = priv_painter()) {
            Vector v = pr.text_size(s);
            size.set(std::ceil(v.x()), std::ceil(pr.font().ascent())+std::ceil(std::fabs(pr.font().descent())));
        }
    }

    return size;
}

void Text_impl::left() {
    if (caret_) {
        Buffer_citer j = caret_;

        if (0 == j.col() && 0 != j.row()) {
            j.move_backward_line();
            j.move_to_eol();
        }

        else {
            --j;
        }

        move_to(j);
        hint_x();
    }
}

void Text_impl::move_left() {
    unselect();
    left();
}

void Text_impl::select_left() {
    Buffer_citer i(caret_);
    left();
    update_selection(i, caret_);
}

void Text_impl::right() {
    if (caret_) {
        Buffer_citer j = caret_;

        if (char32_is_newline(*j)) {
            while (!j.eof() && caret_.row() == j.row()) { ++j; }
        }

        else {
            ++j;
        }

        move_to(j);
        hint_x();
    }
}

void Text_impl::move_right() {
    unselect();
    right();
}

void Text_impl::select_right() {
    Buffer_citer i(caret_);
    right();
    update_selection(i, caret_);
}

void Text_impl::up() {
    if (caret_) {
        if (0 != caret_.row()) {
            Buffer_citer i = caret_;
            std::size_t pos = 0 != xhint_ ? hinted_pos(i.row()-1) : i.col();
            i.move_to(i.row()-1, pos);
            move_to(i);
        }
    }
}

void Text_impl::move_up() {
    unselect();
    up();
}

void Text_impl::select_up() {
    Buffer_citer i(caret_);
    up();
    update_selection(i, caret_);
}

void Text_impl::down() {
    if (caret_) {
        std::size_t dest_row = caret_.row()+1;

        if (dest_row < rows()) {
            Buffer_citer i = caret_;
            std::size_t pos = 0 != xhint_ ? hinted_pos(dest_row) : i.col();
            i.move_to(dest_row, pos);
            move_to(i);
        }
    }
}

void Text_impl::move_down() {
    unselect();
    down();
}

void Text_impl::select_down() {
    Buffer_citer i(caret_);
    down();
    update_selection(i, caret_);
}

void Text_impl::move_word_left() {
    if (caret_) {
        unselect();
        Buffer_citer i = caret_;
        i.move_word_left();
        move_to(i);
        hint_x();
    }
}

void Text_impl::select_word_left() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_word_left();
        move_to(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::move_word_right() {
    if (caret_) {
        unselect();
        Buffer_citer i = caret_;
        i.move_word_right();
        move_to(i);
        hint_x();
    }
}

void Text_impl::select_word_right() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_word_right();
        move_to(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::home() {
    if (caret_ && caret_.col() > 0) {
        auto i(caret_);
        i.move_to_sol();
        i.skip_blanks();
        if (i.col() >= caret_.col()) { i.move_to_sol(); }
        move_to(i);
        hint_x();
    }
}

void Text_impl::move_home() {
    unselect();
    home();
}

void Text_impl::select_home() {
    Buffer_citer i(caret_);
    home();
    update_selection(i, caret_);
}

void Text_impl::move_to_eol() {
    if (caret_) {
        unselect();
        Buffer_citer i(caret_);
        i.move_to_eol();
        move_to(i);
        hint_x();
    }
}

void Text_impl::select_to_eol() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_to_eol();
        move_to(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::move_to_sof() {
    if (caret_) {
        unselect();
        Buffer_citer i(caret_);
        i.move_to(0, 0);
        move_to(i);
        hint_x();
    }
}

void Text_impl::select_to_sof() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_to(0, 0);
        move_to(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::move_to_eof() {
    if (caret_) {
        unselect();
        move_to(buffer_.cend());
        hint_x();
    }
}

void Text_impl::select_to_eof() {
    if (caret_) {
        Buffer_citer i(caret_);
        move_to(buffer_.cend());
        update_selection(i, caret_);
        hint_x();
    }
}

void Text_impl::page_up() {
    if (va_ && caret_) {
        std::size_t nrows = rows_.size(), ri = caret_.row();

        if (ri < nrows && ri > 0) {
            const Row & row1 = rows_[ri];
            int yb = oy_+row1.ybase_-va_.height(), y2 = std::max(0, yb);
            std::size_t ri2;

            for (ri2 = ri; ri2 > 0; --ri2) {
                const Row & row2 = rows_[ri2-1];
                if (oy_+row2.ybase_ <= y2) break;
            }

            if (ri2 > 0) {
                const Row & row3 = rows_[ri2];
                int top1 = oy_+row1.ybase_-row1.ascent_;
                int top3 = oy_+row3.ybase_-row3.ascent_;
                Point sp;

                if (top1 < va_.y()) {
                    sp.set(va_.x(), oy_+row3.ybase_-row3.ascent_);
                }

                else if (top1 > va_.bottom()) {
                    sp.set(va_.x(), oy_+row3.ybase_+row3.descent_-va_.height());
                }

                else if (top3 < va_.y()) {
                    sp.set(va_.x(), oy_+row3.ybase_-row3.ascent_-top1+va_.y());
                }

                scroll_to(sp);
                move_to(ri2, caret_.col());
            }

            else {
                move_to(0, 0);
            }
        }
    }
}

void Text_impl::move_page_up() {
    unselect();
    page_up();
}

void Text_impl::select_page_up() {
    Buffer_citer i(caret_);
    page_up();
    update_selection(i, caret_);
}

void Text_impl::page_down() {
    if (va_ && caret_) {
        std::size_t nrows = rows(), ri = caret_.row();

        if (ri < nrows) {
            const Row & row1 = rows_[ri];
            int y2 = oy_+va_.height()+row1.ybase_;
            std::size_t ri2;

            for (ri2 = ri; ri2 < nrows; ++ri2) {
                const Row & row2 = rows_[ri2+1];
                if (oy_+row2.ybase_ >= y2) break;
            }

            if (ri2 < nrows) {
                const Row & row3 = rows_[ri2];
                int top1 = oy_+row1.ybase_-row1.ascent_;
                int bottom3 = oy_+row3.ybase_+row3.descent_;
                Point sp;

                if (top1 < va_.y()) {
                    sp.set(va_.x(), oy_+row3.ybase_-row3.ascent_);
                }

                else if (top1 > va_.bottom()) {
                    sp.set(va_.x(), oy_+row3.ybase_+row3.descent_-va_.height());
                }

                else if (bottom3 > va_.bottom()) {
                    sp.set(va_.x(), oy_+row3.ybase_-row3.ascent_-top1+va_.y());
                }

                scroll_to(sp);
                move_to(ri2, caret_.col());
            }

            else {
                Buffer_citer i(buffer_.cend());
                move_to(i.row(), i.col());
            }
        }
    }
}

void Text_impl::move_page_down() {
    unselect();
    page_down();
}

void Text_impl::select_page_down() {
    Buffer_citer i(caret_);
    page_down();
    update_selection(i, caret_);
}

void Text_impl::copy() {
    if (sel_ && esel_) {
        if (auto dp = display()) {
            dp->copy_text(selection());
        }
    }
}

bool Text_impl::on_take_focus() {
    return (caret_enabled_ || select_allowed_) && grab_focus();
}

void Text_impl::update_va() {
    va_ = visible_area();
}

void Text_impl::init_actions() {
    if (!actions_) {
        actions_ = new Actions;
        actions_->move_left_action_.connect(fun(this, &Text_impl::move_left));
        actions_->select_previous_char_action_.connect(fun(this, &Text_impl::select_left));
        actions_->next_char_action_.connect(fun(this, &Text_impl::move_right));
        actions_->select_next_char_action_.connect(fun(this, &Text_impl::select_right));
        actions_->previous_line_action_.connect(fun(this, &Text_impl::move_up));
        actions_->select_previous_line_action_.connect(fun(this, &Text_impl::select_up));
        actions_->next_line_action_.connect(fun(this, &Text_impl::move_down));
        actions_->select_next_line_action_.connect(fun(this, &Text_impl::select_down));
        actions_->previous_word_action_.connect(fun(this, &Text_impl::move_word_left));
        actions_->select_previous_word_action_.connect(fun(this, &Text_impl::select_word_left));
        actions_->next_word_action_.connect(fun(this, &Text_impl::move_word_right));
        actions_->select_next_word_action_.connect(fun(this, &Text_impl::select_word_right));
        actions_->move_home_action_.connect(fun(this, &Text_impl::move_home));
        actions_->select_home_action_.connect(fun(this, &Text_impl::select_home));
        actions_->move_to_eol_action_.connect(fun(this, &Text_impl::move_to_eol));
        actions_->select_to_eol_action_.connect(fun(this, &Text_impl::select_to_eol));
        actions_->move_to_sof_action_.connect(fun(this, &Text_impl::move_to_sof));
        actions_->select_to_sof_action_.connect(fun(this, &Text_impl::select_to_sof));
        actions_->move_to_eof_action_.connect(fun(this, &Text_impl::move_to_eof));
        actions_->select_to_eof_action_.connect(fun(this, &Text_impl::select_to_eof));
        actions_->previous_page_action_.connect(fun(this, &Text_impl::move_page_up));
        actions_->select_previous_page_action_.connect(fun(this, &Text_impl::select_page_up));
        actions_->next_page_action_.connect(fun(this, &Text_impl::move_page_down));
        actions_->select_next_page_action_.connect(fun(this, &Text_impl::select_page_down));
        actions_->select_all_action_.connect(fun(this, &Text_impl::select_all));
        actions_->copy_action_.connect(fun(this, &Text_impl::copy));
        actions_->cancel_action_.connect(fun(this, &Widget_impl::drop_focus));

        connect_action(actions_->move_left_action_);
        connect_action(actions_->select_previous_char_action_);
        connect_action(actions_->next_char_action_);
        connect_action(actions_->select_next_char_action_);
        connect_action(actions_->previous_line_action_);
        connect_action(actions_->select_previous_line_action_);
        connect_action(actions_->next_line_action_);
        connect_action(actions_->select_next_line_action_);
        connect_action(actions_->previous_word_action_);
        connect_action(actions_->select_previous_word_action_);
        connect_action(actions_->next_word_action_);
        connect_action(actions_->select_next_word_action_);
        connect_action(actions_->move_home_action_);
        connect_action(actions_->select_home_action_);
        connect_action(actions_->move_to_eol_action_);
        connect_action(actions_->select_to_eol_action_);
        connect_action(actions_->move_to_sof_action_);
        connect_action(actions_->select_to_sof_action_);
        connect_action(actions_->move_to_eof_action_);
        connect_action(actions_->select_to_eof_action_);
        connect_action(actions_->previous_page_action_);
        connect_action(actions_->select_previous_page_action_);
        connect_action(actions_->next_page_action_);
        connect_action(actions_->select_next_page_action_);
        connect_action(actions_->select_all_action_);
        connect_action(actions_->copy_action_);
        connect_action(actions_->cancel_action_);
    }
}

Action & Text_impl::move_left_action() {
    init_actions();
    return actions_->move_left_action_;
}

Action & Text_impl::select_previous_char_action() {
    init_actions();
    return actions_->select_previous_char_action_;
}

Action & Text_impl::next_char_action() {
    init_actions();
    return actions_->next_char_action_;
}

Action & Text_impl::select_next_char_action() {
    init_actions();
    return actions_->select_next_char_action_;
}

Action & Text_impl::previous_line_action() {
    init_actions();
    return actions_->previous_line_action_;
}

Action & Text_impl::select_previous_line_action() {
    init_actions();
    return actions_->select_previous_line_action_;
}

Action & Text_impl::next_line_action() {
    init_actions();
    return actions_->next_line_action_;
}

Action & Text_impl::select_next_line_action() {
    init_actions();
    return actions_->select_next_line_action_;
}

Action & Text_impl::previous_word_action() {
    init_actions();
    return actions_->previous_word_action_;
}

Action & Text_impl::select_previous_word_action() {
    init_actions();
    return actions_->select_previous_word_action_;
}

Action & Text_impl::next_word_action() {
    init_actions();
    return actions_->next_word_action_;
}

Action & Text_impl::select_next_word_action() {
    init_actions();
    return actions_->select_next_word_action_;
}

Action & Text_impl::move_home_action() {
    init_actions();
    return actions_->move_home_action_;
}

Action & Text_impl::select_home_action() {
    init_actions();
    return actions_->select_home_action_;
}

Action & Text_impl::move_to_eol_action() {
    init_actions();
    return actions_->move_to_eol_action_;
}

Action & Text_impl::select_to_eol_action() {
    init_actions();
    return actions_->select_to_eol_action_;
}

Action & Text_impl::move_to_sof_action() {
    init_actions();
    return actions_->move_to_sof_action_;
}

Action & Text_impl::select_to_sof_action() {
    init_actions();
    return actions_->select_to_sof_action_;
}

Action & Text_impl::move_to_eof_action() {
    init_actions();
    return actions_->move_to_eof_action_;
}

Action & Text_impl::select_to_eof_action() {
    init_actions();
    return actions_->select_to_eof_action_;
}

Action & Text_impl::previous_page_action() {
    init_actions();
    return actions_->previous_page_action_;
}

Action & Text_impl::next_page_action() {
    init_actions();
    return actions_->next_page_action_;
}

Action & Text_impl::select_previous_page_action() {
    init_actions();
    return actions_->select_previous_page_action_;
}

Action & Text_impl::select_next_page_action() {
    init_actions();
    return actions_->select_next_page_action_;
}

Action & Text_impl::select_all_action() {
    init_actions();
    return actions_->select_all_action_;
}

Action & Text_impl::copy_action() {
    init_actions();
    return actions_->copy_action_;
}

Action & Text_impl::cancel_action() {
    init_actions();
    return actions_->cancel_action_;
}

signal<void()> & Text_impl::signal_caret_motion() {
    if (!signal_caret_motion_) { signal_caret_motion_ = new signal<void()>; }
    return *signal_caret_motion_;
}

signal<void()> & Text_impl::signal_selection_changed() {
    if (!signal_selection_changed_) { signal_selection_changed_ = new signal<void()>; }
    return *signal_selection_changed_;
}

signal<void()> & Text_impl::signal_click() {
    if (!signal_click_) { signal_click_ = new signal<void()>; }
    return *signal_click_;
}

} // namespace tau

//END
