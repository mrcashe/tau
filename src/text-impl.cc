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
#include <tau/font.hh>
#include <tau/loop.hh>
#include <tau/painter.hh>
#include <tau/string.hh>
#include <display-impl.hh>
#include <scroller-impl.hh>
#include <text-impl.hh>
#include <iostream>

namespace tau {

static const unsigned CARET_TIMEOUT = 511;

Text_impl::Text_impl():
    Widget_impl()
{
    init();
}

Text_impl::Text_impl(Align xalign, Align yalign):
    Widget_impl(),
    xalign_(xalign),
    yalign_(yalign)
{
    init();
}

Text_impl::Text_impl(const ustring & text, Align xalign, Align yalign):
    Widget_impl(),
    xalign_(xalign),
    yalign_(yalign)
{
    init();
    assign(text);
}

Text_impl::Text_impl(Buffer buf, Align xalign, Align yalign):
    Widget_impl(),
    buffer_(buf),
    xalign_(xalign),
    yalign_(yalign)
{
    init();
}

void Text_impl::init() {
    fonts_.emplace_back(Font());
    caret_timer_.signal_alarm().connect(fun(this, &Text_impl::on_caret_timer));

    auto & fi = style_.get("font");
    fi.signal_changed().connect(fun(this, &Text_impl::update_font));
    fi.signal_changed().connect(fun(this, &Text_impl::update_requisition));
    fi.signal_changed().connect(fun(this, &Text_impl::calc_all_ellipsis));
    fi.signal_changed().connect(fun(this, &Text_impl::align_all));
    fi.signal_changed().connect(fun(this, &Text_impl::refresh_caret));
    fi.signal_changed().connect(fun(this, &Text_impl::scroll_to_caret));

    style_.get("foreground").signal_changed().connect(tau::bind(fun(this, &Text_impl::invalidate), Rect()));

    signal_origin_changed_.connect(fun(this, &Text_impl::calc_all_ellipsis));

    signal_size_changed_.connect(fun(this, &Text_impl::update_va));
    signal_size_changed_.connect(fun(this, &Text_impl::calc_all_ellipsis));
    signal_size_changed_.connect(fun(this, &Text_impl::align_all));
    signal_size_changed_.connect(fun(this, &Text_impl::refresh_caret));
    signal_size_changed_.connect(fun(this, &Text_impl::scroll_to_caret));

    signal_scroll_changed_.connect(fun(this, &Text_impl::update_va));

    signal_visible_.connect(fun(this, &Text_impl::update_va));
    signal_visible_.connect(fun(this, &Text_impl::calc_all_ellipsis));
    signal_visible_.connect(fun(this, &Text_impl::align_all));

    signal_invisible_.connect(fun(this, &Text_impl::update_va));

    signal_display_.connect(fun(this, &Text_impl::update_painter));
    signal_display_.connect(fun(this, &Text_impl::update_font));
    signal_display_.connect(fun(this, &Text_impl::update_requisition));
    signal_display_.connect(fun(this, &Text_impl::align_all));
    signal_display_.connect(fun(this, &Text_impl::refresh_caret));

    signal_focus_in_.connect(fun(this, &Text_impl::on_focus_in));
    signal_focus_out_.connect(fun(this, &Text_impl::hide_caret));

    signal_caret_motion_.connect(fun(this, &Text_impl::refresh_caret));
    signal_caret_motion_.connect(fun(this, &Text_impl::scroll_to_caret));

    signal_mouse_down_.connect(fun(this, &Text_impl::on_mouse_down));
    signal_mouse_up_.connect(fun(this, &Text_impl::on_mouse_up));
    signal_mouse_motion_.connect(fun(this, &Text_impl::on_mouse_motion));

    signal_paint_.connect(fun(this, &Text_impl::on_paint));
    signal_take_focus_.connect(fun(this, &Text_impl::on_take_focus));

    connect_action(move_left_action_);
    connect_action(select_left_action_);
    connect_action(move_right_action_);
    connect_action(select_right_action_);
    connect_action(move_up_action_);
    connect_action(select_up_action_);
    connect_action(move_down_action_);
    connect_action(select_down_action_);
    connect_action(move_word_left_action_);
    connect_action(select_word_left_action_);
    connect_action(move_word_right_action_);
    connect_action(select_word_right_action_);
    connect_action(move_home_action_);
    connect_action(select_home_action_);
    connect_action(move_to_eol_action_);
    connect_action(select_to_eol_action_);
    connect_action(move_to_sof_action_);
    connect_action(select_to_sof_action_);
    connect_action(move_to_eof_action_);
    connect_action(select_to_eof_action_);
    connect_action(move_page_up_action_);
    connect_action(select_page_up_action_);
    connect_action(move_page_down_action_);
    connect_action(select_page_down_action_);
    connect_action(select_all_action_);
    connect_action(copy_action_);
    connect_action(cancel_action_);

    init_buffer();
}

bool Text_impl::empty() const {
    return buffer_.empty();
}

void Text_impl::update_font() {
    if (pr_) {
        fonts_.front() = pr_.select_font(style_.font("font").spec());

        if (fonts_.front()) {
            font_height_ = std::ceil(fonts_.front().ascent())+std::ceil(std::fabs(fonts_.front().descent()));
            space_width_ = std::ceil(pr_.text_size(U" ").x());

            for (std::size_t ri = 0; ri < lines_.size(); ++ri) {
                Line & line = lines_[ri];
                calc_line(line);
            }

            arrange_lines();
            text_width_ = calc_width(0, lines());
            text_height_ = calc_height(0, lines());
            invalidate();
        }
    }
}

void Text_impl::update_painter() {
    pr_ = painter();
}

bool Text_impl::on_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        unselect();
        if (caret_enabled_) { move_to(iter_from_point(pt)); }
        if (select_allowed_) { msel_ = iter_from_point(pt); }
        grab_focus();
        signal_click_();
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

            move_caret(i);
        }
    }
}

void Text_impl::on_focus_in() {
    if (caret_enabled_) {
        show_caret();
    }
}

void Text_impl::on_buffer_replace(Buffer_citer b, Buffer_citer e, const std::u32string & replaced) {
    if (b.row() == e.row()) {
        Line & ln = lines_[b.row()];
        int h0 = ln.ascent+ln.descent;
        load_lines(b.row(), b.row());
        int h1 = ln.ascent+ln.descent;
        if (ln.width < text_width_) { text_width_ = calc_width(0, lines_.size()); }
        e.move_to_eol();
        if (h1 != h0) { translate_lines(b.row()+1, lines_.size(), h1-h0); e = buffer_.cend(); }
        update_requisition();
        align_lines(b.row(), b.row());
        update_range(b, e);
    }
}

void Text_impl::on_buffer_replace_move(Buffer_citer b, Buffer_citer e, const std::u32string & replaced) {
    move_caret(e);
    hint_x();
    signal_changed_();
}

Painter Text_impl::wipe_area(int x1, int y1, int x2, int y2, Painter pr) {
    if (!pr) { pr = pr_; }
    if (!pr) { pr = painter(); }

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

void Text_impl::on_buffer_erase(Buffer_citer b, Buffer_citer e, const std::u32string & erased) {
    if (e < b) { std::swap(b, e); }

    if (e.row() >= lines_.size() || buffer_.empty()) {
        clear();
        return;
    }

    if (e.row() == b.row()) {
        Line & row = lines_[b.row()];
        int h0 = row.ascent+row.descent;
        int y1 = oy_+row.ybase-row.ascent;
        int y2 = oy_+row.ybase+row.descent;

        load_lines(b.row(), b.row());
        int h1 = row.ascent+row.descent;
        y1 = std::min(y1, oy_+row.ybase-row.ascent);
        y2 = std::max(y2, oy_+row.ybase+row.descent);

        if (xalign_ != ALIGN_START) {
            wipe_area(va_.left(), y1, va_.right(), y2);
            b.move_to_sol();
        }

        if (row.width < text_width_) { text_width_ = calc_width(0, lines_.size()); }
        e.move_to_eol();
        if (h1 != h0) { translate_lines(b.row()+1, lines_.size(), h1-h0); e = buffer_.cend(); }
    }

    else {
        int hdel = calc_height(b.row()+1, e.row());
        int wdel = calc_width(b.row()+1, e.row());
        lines_.erase(lines_.begin()+b.row(), lines_.begin()+e.row());
        translate_lines(b.row(), lines_.size(), -hdel);
        load_lines(b.row(), e.row());
        text_height_ = std::max(0, text_height_-hdel);
        if (wdel >= text_width_) { text_width_ = calc_width(0, lines()); }
        e = buffer_.cend();
    }

    update_requisition();
    bool aligned = align_lines(0, lines());
    if (aligned) { b.move_to_sol(); e.move_to_eol(); }
    update_range(b, e);
}

void Text_impl::on_buffer_erase_move(Buffer_citer b, Buffer_citer e, const std::u32string & erased) {
    move_caret(b);
    hint_x();
    signal_changed_();
}

void Text_impl::on_buffer_insert(Buffer_citer b, Buffer_citer e) {
    insert_range(b, e);
}

void Text_impl::on_buffer_insert_move(Buffer_citer b, Buffer_citer e) {
    move_caret(e);
    hint_x();
    signal_changed_();
}

void Text_impl::insert_range(Buffer_citer b, Buffer_citer e) {
    if (e < b) { std::swap(b, e); }
    wipe_caret();
    std::size_t nlines = e.row()-b.row();

    if (lines_.empty()) {
        lines_.emplace_back();
    }

    while (nlines--) {
        lines_.emplace(lines_.begin()+b.row());
    }

    load_lines(b.row(), e.row());
    arrange_lines();
    text_width_ = calc_width(0, lines_.size()-1);
    text_height_ = calc_height(0, lines_.size()-1);
    align_all();
    update_requisition();
    if (e.row() > b.row()) { e = buffer_.cend(); }
    if (ALIGN_START != xalign_) { b.move_to_sol(); e.move_to_eol(); }
    update_range(b, e);
}

// Overriden by Edit_impl.
void Text_impl::assign(Buffer buf) {
    clear();
    insert_cx_.disconnect();
    replace_cx_.disconnect();
    erase_cx_.disconnect();
    insert_move_cx_.disconnect();
    replace_move_cx_.disconnect();
    erase_move_cx_.disconnect();
    buffer_ = buf;
    init_buffer();
    signal_caret_motion_();
    signal_changed_();
}

// Overriden by Edit_impl.
void Text_impl::assign(const ustring & s) {
    clear();
    buffer_.assign(s);
}

void Text_impl::clear() {
    wipe_caret();
    Painter pr = pr_;
    if (!pr) { pr = painter(); }

    if (pr) {
        pr.push(); pr.clear();
        pr.set_brush(Brush(style_.color("background")));
        pr.paint(); pr.pop();
    }

    bool changed = !buffer_.empty();
    buffer_.clear();
    lines_.clear();
    sel_.reset();
    esel_.reset();
    msel_.reset();
    emsel_.reset();
    caret_ = buffer_.cbegin();
    update_requisition();
    signal_caret_motion_();
    if (changed) { signal_changed_(); }
}

void Text_impl::init_buffer() {
    insert_cx_ = buffer_.signal_insert().connect(fun(this, &Text_impl::on_buffer_insert));
    replace_cx_ = buffer_.signal_replace().connect(fun(this, &Text_impl::on_buffer_replace));
    erase_cx_ = buffer_.signal_erase().connect(fun(this, &Text_impl::on_buffer_erase));
    insert_move_cx_ = buffer_.signal_insert().connect(fun(this, &Text_impl::on_buffer_insert_move));
    replace_move_cx_ = buffer_.signal_replace().connect(fun(this, &Text_impl::on_buffer_replace_move));
    erase_move_cx_ = buffer_.signal_erase().connect(fun(this, &Text_impl::on_buffer_erase_move));
    caret_ = buffer_.cbegin();
    insert_range(buffer_.cbegin(), buffer_.cend());
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
        if (align_lines(0, lines())) { invalidate(); }
    }
}

void Text_impl::set_text_align(Align xalign, Align yalign) {
    if (xalign_ != xalign || yalign_ != yalign) {
        xalign_ = xalign;
        yalign_ = yalign;
        align_all();
    }
}

bool Text_impl::align_lines(std::size_t first, std::size_t last) {
    bool changed = false;

    if (va_) {
        if (last < first) { std::swap(first, last); }
        int h = va_.height(), w = va_.width(), ey = h-text_height_, oy = 0;

        if (ey > 0) {
            if (ALIGN_CENTER == yalign_) { oy = ey/2; }
            else if (ALIGN_END == yalign_) { oy = ey; }
        }

        if (oy_ != oy) { oy_ = oy; changed = true; }

        for (std::size_t n = first; n < lines_.size() && n <= last; ++n) {
            Line & line = lines_[n];
            int ex = w-line.width, ox = 0;

            if (ex > 0) {
                if (ALIGN_CENTER == xalign_) { ox = ex/2; }
                else if (ALIGN_END == xalign_) { ox = ex; }
            }

            if (line.ox != ox) { line.ox = ox; changed = true; }
        }
    }

    return changed;
}

void Text_impl::align_all() {
    if (align_lines(0, lines())) {
        invalidate();
    }
}

void Text_impl::translate_lines(std::size_t first, std::size_t last, int dy) {
    if (last < first) { std::swap(first, last); }
    for (std::size_t n = first; n < lines_.size() && n <= last; ++n) { lines_[n].ybase += dy; }
}

void Text_impl::arrange_lines() {
    int ybase = 0;

    for (Line & line: lines_) {
        line.ybase = ybase+line.ascent;
        ybase += line.ascent+line.descent+spacing_;
    }
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

        signal_selection_changed_();
    }
}

void Text_impl::update_range(Buffer_citer b, Buffer_citer e) {
    if (b && e && b.row() < lines_.size() && e.row() < lines_.size()) {
        if (e < b) { std::swap(b, e); }
        if (0 == e.row()) { --e; }

        const Line & line1 = lines_[b.row()], line2 = lines_[e.row()];
        int y1 = oy_+line1.ybase-line1.ascent;
        int y2 = oy_+line2.ybase+line2.descent;
        if (e.row() >= lines_.size()-1) { y2 = va_.bottom(); }
        int x1 = va_.left(), x2 = va_.right();

        if (b.row() == e.row()) {
            x1 = x_at_col(b.row(), b.col());
            x2 = x_at_col(e.row(), e.col());
        }

        redraw(Rect(x1, y1, x2, y2));
    }
}

void Text_impl::calc_all_ellipsis() {
    ellipsis_width_ = 0;

    if (WRAP_NONE != wrap_) {
        ellipsis_width_ = text_size(ellipsis_).width();
        for (auto & row: lines_) { calc_ellipsis(row); }
    }
}

void Text_impl::calc_ellipsis(Line & line) {
    line.ellipsized.clear();

    if (0 != ellipsis_width_ && va_.iwidth() >= ellipsis_width_ && line.ncols > 1) {
        if (line.width > va_.iwidth()) {
            std::size_t col = 1;
            int w = va_.width()-ellipsis_width_;

            if (WRAP_ELLIPSIZE_START == wrap_) {
                line.ellipsized += ellipsis_;
                int skip = line.width-w, skipped = 0;
                while (skipped < skip && col < line.ncols) { skipped = line.poss[col++]; }
                line.ellipsized += line.text.substr(col-1);
            }

            else if (WRAP_ELLIPSIZE_CENTER == wrap_) {
                int lpart = w/2, wnext = 0;

                while (wnext < lpart && col < line.ncols) {
                    wnext = line.poss[col];
                    if (wnext < lpart) { line.ellipsized += line.text[col-1]; }
                    ++col;
                }

                line.ellipsized += ellipsis_;
                col = line.ncols-1;
                int rpart = line.width-(w/2), rpos = line.width;

                while (rpos > rpart && 0 != col) {
                    rpos = line.poss[col];
                    if (rpos > rpart) { --col; }
                }

                if (col < line.ncols-1) { ++col; }
                line.ellipsized += line.text.substr(col);
            }

            // Assume WRAP_ELLIPSIZE_END.
            else {
                int wnext = 0;

                while (wnext < w && col < line.ncols) {
                    wnext = line.poss[col];
                    if (wnext < w) { line.ellipsized += line.text[col-1]; }
                    ++col;
                }

                line.ellipsized += ellipsis_;
            }
        }
    }
}

void Text_impl::calc_line(Line & line) {
    line.ascent = 0;
    line.descent = 0;
    line.width = 0;
    line.ox = 0;
    int x = 0;

    if (Painter pr = priv_painter()) {
        std::size_t pos = 0;
        line.ncols = line.text.size();
        line.poss.resize(line.ncols);

        for (Frag & frag: line.frags) {
            Font font = frag.font ? fonts_[frag.font] : pr.font();
            pr.set_font(font);
            line.ascent = std::max(line.ascent, int(std::ceil(font.ascent())));
            line.descent = std::max(line.descent, int(std::ceil(fabs(font.descent()))));
            std::u32string acc;

            for (std::size_t i = 0; i < frag.ncols; ++i) {
                line.poss[i+frag.start] = x+std::ceil(pr.text_size(acc).x());
                char32_t c = line.text[i+frag.start];

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

            frag.width = std::ceil(pr.text_size(acc).x());
            x += frag.width;
        }
    }

    line.width = x;
    calc_ellipsis(line);
}

void Text_impl::load_lines(std::size_t ln, std::size_t last) {
    for (; ln < lines_.size() && ln <= last; ++ln) {
        Line & line = lines_[ln];
        line.frags.clear();
        line.ncols = 0;
        Buffer_citer b = buffer_.citer(ln, 0), e = b;
        e.move_to_eol();
        line.text = buffer_.text32(b, e);

        line.frags.emplace_back();
        Frag & frag = line.frags.back();
        frag.start = 0;
        frag.ncols = line.text.size();

        calc_line(line);
    }
}

int Text_impl::calc_height(std::size_t first, std::size_t last) {
    if (last < first) { std::swap(first, last); }
    int h = 0;
    std::size_t nrows = lines_.size();

    for (std::size_t n = first; n < nrows && n <= last; ) {
        const Line & row = lines_[n];
        h += row.ascent+row.descent;
        if (++n < nrows) { h += spacing_; }
    }

    return h;
}

int Text_impl::calc_width(std::size_t first, std::size_t last) {
    if (last < first) { std::swap(first, last); }
    int w = 0;

    for (std::size_t n = first; n < lines_.size() && n <= last; ++n) {
        const Line & row = lines_[n];
        w = std::max(w, row.width);
    }

    return w;
}

void Text_impl::update_caret() {
    rcaret_.reset();
    int x1 = 0, x2 = 0, y1 = oy_, y2 = y1;

    if (!buffer_.empty()) {
        if (caret_.row() < lines_.size()) {
            const Line & row = lines_[caret_.row()];
            x1 = x_at_col(row, caret_.col());
            y1 += row.ybase-row.ascent;
            y2 += row.ybase+row.descent;

            if (!insert_ && caret_.col() < row.ncols) {
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
    ccaret_ = style().get("foreground").get();
}

void Text_impl::draw_caret(Painter pr) {
    if (rcaret_) {
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
    if (caret_exposed_) { wipe_caret(); }
    else { expose_caret(); }
}

void Text_impl::refresh_caret() {
    if (caret_enabled_ && caret_visible_) {
        caret_timer_.restart(CARET_TIMEOUT, true);
        wipe_caret();
        update_caret();
        expose_caret();
    }
}

void Text_impl::expose_caret() {
    if (caret_visible_ && visible() && caret_ && pr_) {
        caret_exposed_ = true;
        draw_caret(pr_);
    }
}

void Text_impl::wipe_caret() {
    if (caret_exposed_) {
        draw_caret(pr_);
        caret_exposed_ = false;
    }
}

void Text_impl::scroll_to_caret() {
    if (caret_enabled_ && !buffer_.empty() && caret_.row() < lines_.size() && va_) {
        Point ofs(va_.origin());
        const Line & row = lines_[caret_.row()];
        int y1 = oy_+row.ybase-row.ascent;
        int y2 = oy_+row.ybase+row.descent;
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

void Text_impl::move_caret(std::size_t row, std::size_t col) {
    if (caret_enabled_) {
        caret_.move_to(row, col);
        signal_caret_motion_();
    }
}

void Text_impl::move_caret(Buffer_citer i) {
    move_caret(i.row(), i.col());
}

void Text_impl::move_to(std::size_t row, std::size_t col) {
    move_caret(row, col);
    hint_x();
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
        allow_focus();
        update_requisition();
        refresh_caret();
    }
}

void Text_impl::disable_caret() {
    if (caret_enabled_) {
        caret_enabled_ = false;
        unselect();
        hide_caret();
        drop_focus();
        disallow_focus();
        update_requisition();
    }
}

void Text_impl::show_caret() {
    if (!caret_visible_) {
        caret_visible_ = true;
        refresh_caret();
        calc_all_ellipsis();
    }
}

void Text_impl::hide_caret() {
    if (caret_visible_) {
        if (caret_exposed_) { wipe_caret(); }
        caret_timer_.stop();
        caret_visible_ = false;
        calc_all_ellipsis();
    }
}

std::size_t Text_impl::hinted_pos(std::size_t ri) {
    int x1 = 0;

    if (xhint_ > 0 && ri < lines_.size()) {
        const Line & row = lines_[ri];

        for (std::size_t n = 1; n < row.ncols; ++n) {
            int x2 = x_at_col(row, n);

            if (x2 >= xhint_) {
                std::size_t col = x2-xhint_ < xhint_-x1 ? n : n-1;

                for (Buffer_citer c(caret_, ri, col); col < row.ncols; ++c, ++col) {
                    if (!char32_is_modifier(*c)) {
                        break;
                    }
                }

                return col;
            }

            else {
                x1 = x2;
            }
        }

        return row.ncols;
    }

    return x1;
}

int Text_impl::x_at_col(const Line & row, std::size_t col) const {
    return row.ox+(col < row.ncols ? row.poss[col] : row.width);
}

int Text_impl::x_at_col(std::size_t ri, std::size_t col) const {
    if (ri < lines_.size()) {
        return x_at_col(lines_[ri], col);
    }

    return 0;
}

std::size_t Text_impl::col_at_x(const Line & line, int x) const {
    std::size_t col = 0;
    x -= line.ox;

    if (x > 0) {
        if (x >= line.width) {
            col = line.ncols;
        }

        else {
            for (std::size_t n = 0; n < line.ncols; ++n, ++col) {
                int x0 = line.poss[n];
                int x1 = n+1 < line.ncols ? line.poss[n+1] : line.width;
                if (x >= x0 && x < x1) { break; }
            }
        }
    }

    // std::cout << "col_at_x " << line.text << " " << x << " " << line.ox << " " << col << '\n';
    return col;
}

std::size_t Text_impl::col_at_x(std::size_t ri, int x) const {
    if (ri < lines_.size()) {
        return col_at_x(lines_[ri], x);
    }

    return 0;
}

std::size_t Text_impl::row_at_y(int y) const {
    if (y >= 0 && !lines_.empty()) {
        for (std::size_t ri = 0; ri < lines_.size(); ++ri) {
            const Line & row = lines_[ri];
            int ymax = oy_+row.ybase+row.descent;
            if (y < ymax) { return ri; }
        }

        return lines_.size()-1;
    }

    return 0;
}

int Text_impl::baseline(std::size_t ri) const {
    if (ri < lines_.size()) {
        return lines_[ri].ybase;
    }

    return 0;
}

void Text_impl::get_row_bounds(std::size_t ln, int & top, int & bottom) const {
    if (ln < lines_.size()) {
        const Line & line = lines_[ln];
        top = oy_+line.ybase-line.ascent;
        bottom = oy_+line.ybase+line.descent;
    }

    else {
        top = bottom = 0;
    }
}

Buffer_citer Text_impl::iter_from_point(const Point & pt) {
    std::size_t ln = row_at_y(pt.y());
    std::size_t col = col_at_x(ln, pt.x());
    return iter(ln, col);
}

void Text_impl::paint_ellipsized(const Line & line, Painter pr) {
    int ybase = oy_+line.ybase;
    wipe_area(va_.left(), ybase-line.ascent, va_.right(), ybase+line.descent, pr);
    pr.move_to(0, ybase);
    select_font(pr);
    pr.text(line.ellipsized, enabled() ? style_.color("foreground") : style_.color("background").get().inactive());
    pr.stroke();
}

void Text_impl::paint_line(const Line & line, std::size_t ln, std::size_t pos, Painter pr) {
    pos = std::min(pos, line.ncols);
    std::size_t scol = std::max(pos, col_at_x(line, va_.left()));           // Starting column.
    std::size_t ecol = std::min(line.ncols, 1+col_at_x(line, va_.right())); // Ending column.

    // Include modifiers before starting column.
    for (Buffer_citer i = buffer_.citer(ln, scol); scol > 0; --scol, --i) {
        if (!char32_is_modifier(*i)) { break; }
    }

    // Include modifiers after ending column.
    for (Buffer_citer i = buffer_.citer(ln, ecol); i.col() < line.ncols; ++ecol, ++i) {
        if (!char32_is_modifier(*i)) { break; }
    }

    Color bg = style().color("background");

    if (sel_ && esel_ && (ln > sel_.row() || (ln == sel_.row() && scol >= sel_.col())) && ln <= esel_.row()) {
        bg = style().color("select/background");
    }

    std::size_t col0 = 0;
    int ybase = oy_+line.ybase;
    int y1 = ybase-line.ascent;
    int y2 = ybase+line.descent;

    // Loop over fragmens.
    for (const Frag & frag: line.frags) {
        std::size_t fend = col0+frag.ncols;
        std::size_t col = std::max(scol, col0);

        if (fend > scol) {
            const std::u32string & s = line.text;
            std::size_t col1 = std::min(ecol, fend);

            // Loop inside of fragment.
            while (col < col1) {
                std::size_t col2 = col1;
                select_font(pr);

                if (sel_ && esel_) {
                    if (sel_.row() == ln && col < sel_.col()) {
                        col2 = std::min(col1, sel_.col());
                    }

                    else if (sel_.row() == ln && col == sel_.col()) {
                        bg = style_.color("select/background");
                        if (esel_.row() == ln && col < esel_.col()) { col2 = std::min(col1, esel_.col()); }
                    }

                    else if (esel_.row() == ln && col < esel_.col()) {
                        bg = style_.color("select/background");
                        col2 = std::min(col1, esel_.col());
                    }

                    else if (esel_.row() == ln && col >= esel_.col()) {
                        bg = style_.color("background");
                    }
                }

                std::size_t tab = s.find(U'\x0009', col);
                if (tab > col && tab < col2) { col2 = tab; }

                if (col2 > col) {
                    int x1 = x_at_col(line, col), x2 = x_at_col(line, col2);
                    pr.rectangle(x1, y1, x2, y2);
                    pr.set_brush(bg);
                    pr.fill();

                    if (U'\x0009' != s[col]) {
                        Color c = enabled() ? style_.color("foreground") : style_.color("background").get().inactive();
                        pr.move_to(x1, ybase);
                        pr.text(s.substr(col-col0, col2-col), c);
                        // std::cout << "&&&&& " << s.substr(col-col0, col2-col) << " " << ecol << " " << line.ox << '\n';
                        pr.stroke();
                        col += col2-col;
                    }

                    else {
                        ++col;
                    }
                }
            }
        }

        col0 += frag.ncols;
        if (col0 > ecol) { break; }
    }

    // Draw an empty rectangle after EOL.
    if (ecol >= line.ncols) {
        if (esel_ && esel_.row() == ln && esel_.col() <= line.ncols) { bg = style().color("background"); }
        pr.rectangle(line.ox+line.width, y1, va_.right(), y2);
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
        std::size_t ln = row_at_y(r.top()), len = lines_.size();
        const Line * lp = lines_.data()+ln;

        for (; ln < len; ++ln, ++lp) {
            if (r.bottom() <= oy_+lp->ybase-lp->ascent) { break; }
            if (!lp->ellipsized.empty()) { paint_ellipsized(*lp, pr); }
            else { paint_line(*lp, ln, col_at_x(*lp, r.x()), pr); }
        }

        pr.pop();
        if (caret_exposed) { expose_caret(); }
    }
}

bool Text_impl::on_paint(Painter pr, const Rect & r) {
    redraw(r.translated(scroll_position()), pr);
    return true;
}

Font Text_impl::select_font(Painter pr) {
    Font font;

    if (!pr) { pr = pr_; }
    if (!pr) { pr = painter(); }

    if (pr) {
        if (fonts_.front()) { font = fonts_.front(); pr.set_font(font); }
        else { font = pr.select_font(style_.font("font")); }
    }

    return font;
}

void Text_impl::set_wrap_mode(Wrap_mode wrap_mode) {
    if (wrap_ != wrap_mode) {
        wrap_ = wrap_mode;
        calc_all_ellipsis();
        invalidate();
    }
}

void Text_impl::allow_select() {
    if (!select_allowed_) {
        select_allowed_ = true;
        unselect();
        set_cursor("text:ibeam");
    }
}

void Text_impl::disallow_select() {
    if (select_allowed_) {
        unselect();
        select_allowed_ = false;
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
        signal_selection_changed_();
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
        signal_selection_changed_();
    }
}

std::size_t Text_impl::lines() const {
    return buffer_.lines();
}

Painter Text_impl::priv_painter() {
    Painter pr = pr_;

    if (!pr) {
        pr = painter();
    }

    if (pr) {
        if (fonts_.front()) { pr.set_font(fonts_.front()); }
        else { pr.select_font(style_.font("font").spec()); }
    }

    return pr;
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
            while (!(--j).sof() && char32_is_modifier(*j)) {}
        }

        move_caret(j);
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
            while (!(++j).eof() && char32_is_modifier(*j)) {}
        }

        move_caret(j);
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
            move_caret(i);
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

        if (dest_row < lines()) {
            Buffer_citer i = caret_;
            std::size_t pos = 0 != xhint_ ? hinted_pos(dest_row) : i.col();
            i.move_to(dest_row, pos);
            move_caret(i);
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
        move_caret(i);
        hint_x();
    }
}

void Text_impl::select_word_left() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_word_left();
        move_caret(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::move_word_right() {
    if (caret_) {
        unselect();
        Buffer_citer i = caret_;
        i.move_word_right();
        move_caret(i);
        hint_x();
    }
}

void Text_impl::select_word_right() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_word_right();
        move_caret(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::home() {
    if (caret_ && caret_.col() > 0) {
        Buffer_citer i = caret_;
        i.move_to_sol();
        i.skip_blanks();
        if (i.col() < caret_.col()) { caret_ = i; }
        else { i.move_to_sol(); }
        move_caret(i);
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
        move_caret(i);
        hint_x();
    }
}

void Text_impl::select_to_eol() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_to_eol();
        move_caret(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::move_to_sof() {
    if (caret_) {
        unselect();
        Buffer_citer i(caret_);
        i.move_to(0, 0);
        move_caret(i);
        hint_x();
    }
}

void Text_impl::select_to_sof() {
    if (caret_) {
        Buffer_citer i(caret_), j(caret_);
        j.move_to(0, 0);
        move_caret(j);
        update_selection(i, j);
        hint_x();
    }
}

void Text_impl::move_to_eof() {
    if (caret_) {
        unselect();
        move_caret(buffer_.cend());
        hint_x();
    }
}

void Text_impl::select_to_eof() {
    if (caret_) {
        Buffer_citer i(caret_);
        move_caret(buffer_.cend());
        update_selection(i, caret_);
        hint_x();
    }
}

void Text_impl::page_up() {
    if (va_ && caret_) {
        std::size_t nrows = lines_.size(), ri = caret_.row();

        if (ri < nrows && ri > 0) {
            const Line & row1 = lines_[ri];
            int yb = oy_+row1.ybase-va_.height(), y2 = std::max(0, yb);
            std::size_t ri2;

            for (ri2 = ri; ri2 > 0; --ri2) {
                const Line & row2 = lines_[ri2-1];
                if (oy_+row2.ybase <= y2) break;
            }

            if (ri2 > 0) {
                const Line & row3 = lines_[ri2];
                int top1 = oy_+row1.ybase-row1.ascent;
                int top3 = oy_+row3.ybase-row3.ascent;
                Point sp;

                if (top1 < va_.y()) {
                    sp.set(va_.x(), oy_+row3.ybase-row3.ascent);
                }

                else if (top1 > va_.bottom()) {
                    sp.set(va_.x(), oy_+row3.ybase+row3.descent-va_.height());
                }

                else if (top3 < va_.y()) {
                    sp.set(va_.x(), oy_+row3.ybase-row3.ascent-top1+va_.y());
                }

                scroll_to(sp);
                move_caret(ri2, caret_.col());
            }

            else {
                move_caret(0, 0);
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
        std::size_t nrows = lines(), ri = caret_.row();

        if (ri < nrows) {
            const Line & row1 = lines_[ri];
            int y2 = oy_+va_.height()+row1.ybase;
            std::size_t ri2;

            for (ri2 = ri; ri2 < nrows; ++ri2) {
                const Line & row2 = lines_[ri2+1];
                if (oy_+row2.ybase >= y2) break;
            }

            if (ri2 < nrows) {
                const Line & row3 = lines_[ri2];
                int top1 = oy_+row1.ybase-row1.ascent;
                int bottom3 = oy_+row3.ybase+row3.descent;
                Point sp;

                if (top1 < va_.y()) {
                    sp.set(va_.x(), oy_+row3.ybase-row3.ascent);
                }

                else if (top1 > va_.bottom()) {
                    sp.set(va_.x(), oy_+row3.ybase+row3.descent-va_.height());
                }

                else if (bottom3 > va_.bottom()) {
                    sp.set(va_.x(), oy_+row3.ybase-row3.ascent-top1+va_.y());
                }

                scroll_to(sp);
                move_caret(ri2, caret_.col());
            }

            else {
                Buffer_citer i(buffer_.cend());
                move_caret(i.row(), i.col());
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

} // namespace tau

//END
