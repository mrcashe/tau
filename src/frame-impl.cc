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
#include <tau/brush.hh>
#include <tau/painter.hh>
#include <tau/pen.hh>
#include <tau/text.hh>
#include <frame-impl.hh>
#include <text-impl.hh>
#include <climits>
#include <iostream>

namespace tau {

static const double PI = 3.141592654;

Frame_impl::Frame_impl():
    Container_impl()
{
    init();
}

Frame_impl::Frame_impl(const ustring & label):
    Container_impl()
{
    init();
    set_label(label);
}

Frame_impl::Frame_impl(const ustring & label, Align align):
    Container_impl()
{
    init();
    set_label(label, align);
}

Frame_impl::Frame_impl(Border_style bs, unsigned border_width, int border_radius):
    Container_impl()
{
    init();
    init_border_style(bs, border_width, border_radius);
}

Frame_impl::Frame_impl(const ustring & label, Border_style bs, unsigned border_width, int border_radius):
    Container_impl()
{
    init();
    init_border_style(bs, border_width, border_radius);
    set_label(label);
}

Frame_impl::Frame_impl(const ustring & label, Align align, Border_style bs, unsigned border_width, int border_radius):
    Container_impl()
{
    init();
    init_border_style(bs, border_width, border_radius);
    set_label(label, align);
}

void Frame_impl::init() {
    signal_arrange().connect(fun(this, &Frame_impl::arrange));
    signal_size_changed().connect(fun(this, &Frame_impl::arrange));
    signal_visible().connect(fun(this, &Frame_impl::arrange));
    signal_display().connect(fun(this, &Frame_impl::update_requisition));
    signal_paint().connect(fun(this, &Frame_impl::on_paint));
    signal_backpaint().connect(fun(this, &Frame_impl::on_backpaint), true);
}

void Frame_impl::init_border_style(Border_style bs, unsigned width, int radius) {
    unsigned s = min_border_size(bs);
    radius_ = std::max(0, radius);
    set_border_style(0 != s ? bs : BORDER_NONE);
    set_border(0 != s ? std::max(s, width) : 0);
}

void Frame_impl::insert(Widget_ptr wp) {
    clear();
    make_child(wp);
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    child_req_cx_ = wp->signal_requisition_changed().connect(fun(this, &Frame_impl::update_requisition));
    child_hints_cx_ = wp->signal_hints_changed().connect(fun(this, &Frame_impl::update_requisition));
    child_show_cx_ = wp->signal_show().connect(fun(this, &Frame_impl::on_child_show));
    child_hide_cx_ = wp->signal_hide().connect(fun(this, &Frame_impl::on_child_hide));
    child_focus_cx_ = signal_take_focus().connect(fun(wp, &Widget_impl::take_focus));
    cp_ = wp.get();
    update_requisition();
    queue_arrange();
}

void Frame_impl::clear() {
    if (auto wp = cp_) {
        child_hints_cx_.disconnect();
        child_req_cx_.disconnect();
        child_hide_cx_.disconnect();
        child_show_cx_.disconnect();
        child_focus_cx_.disconnect();
        cp_ = nullptr;
        unparent_child(wp);
        wp->update_origin(INT_MIN, INT_MIN);
        wp->update_size(0, 0);
        update_requisition();
        queue_arrange();
    }
}

void Frame_impl::set_label(Widget_ptr wp) {
    unset_label();
    make_child(wp);
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    wp->disallow_focus();
    label_ = wp.get();
    label_req_cx_ = wp->signal_requisition_changed().connect(fun(this, &Frame_impl::update_requisition));
    label_hints_cx_ = wp->signal_hints_changed().connect(fun(this, &Frame_impl::update_requisition));
    label_show_cx_ = wp->signal_show().connect(fun(this, &Frame_impl::on_label_show));
    label_hide_cx_ = wp->signal_hide().connect(fun(this, &Frame_impl::on_label_hide));
    update_requisition();
    queue_arrange();
}

void Frame_impl::unset_label() {
    if (auto wp = label_) {
        lb_.reset();
        label_hints_cx_.disconnect();
        label_req_cx_.disconnect();
        label_hide_cx_.disconnect();
        label_show_cx_.disconnect();
        label_ = nullptr;
        unparent_child(wp);
        wp->update_origin(INT_MIN, INT_MIN);
        wp->update_size(0, 0);
        update_requisition();
        queue_arrange();
    }
}

void Frame_impl::set_label(const ustring & text, Align align) {
    unset_label();
    auto label = std::make_shared<Text_impl>(text);
    label->hint_margin(3, 3, 1, 1);
    set_label(label);
    align_label(align);
}

Size Frame_impl::child_requisition(Widget_impl * wp) {
    Size req;

    if (wp && !wp->hidden()) {
        req = wp->required_size();
        req.update(wp->size_hint(), true);
        req.update_max(wp->min_size_hint());
        req.update_min(wp->max_size_hint(), true);
        req.increase(wp->margin_hint());
    }

    return req;
}

void Frame_impl::update_requisition() {
    Size rs;

    unsigned r = eradius();
    unsigned left = std::max(left_, r);
    unsigned right = std::max(right_, r);
    unsigned top = std::max(top_, r);
    unsigned bottom = std::max(bottom_, r);

    if (Size req = child_requisition(label_)) {
        if (LABEL_TOP == lpos_) { top = std::max(top, req.height()); }
        else if (LABEL_BOTTOM == lpos_) { bottom = std::max(bottom, req.height()); }
        else if (LABEL_LEFT == lpos_) { left = std::max(left, req.width()); }
        else if (LABEL_RIGHT == lpos_) { right = std::max(right, req.width()); }
    }

    rs.increase(left+right, top+bottom);
    rs += child_requisition(cp_);
    require_size(rs);
}

void Frame_impl::arrange() {
    Size  csize(size());
    Point corigin;

    lleft_ = left_;
    lright_ = right_;
    ltop_ = top_;
    lbottom_ = bottom_;

    unsigned r = eradius();

    border_left_ = r ? std::min(r-1, left_) : left_;
    border_right_ = r ? std::min(r-1, right_) : right_;
    border_top_ = r ? std::min(r-1, bottom_) : top_;
    border_bottom_ = r ? std::min(r-1, bottom_) : bottom_;

    unsigned rmin = double(mradius())*0.71;
    unsigned left = std::max(border_left_, rmin);
    unsigned right = std::max(border_right_, rmin);
    unsigned top = std::max(border_top_, rmin);
    unsigned bottom = std::max(border_bottom_, rmin);

    csize.decrease(left+right, top+bottom);
    corigin.translate(left, top);

    if (label_) {
        if (!label_->hidden()) {
            Size req = child_requisition(label_);

            if (req) {
                unsigned w = std::min(req.width(), csize.width());
                unsigned h = std::min(req.height(), csize.height()/2);
                int x, y;

                if (LABEL_BOTTOM == lpos_) {
                    unsigned new_bottom = std::max(bottom, req.height());
                    csize.decrease(0, new_bottom-bottom);
                    bottom = new_bottom;
                    x = corigin.x();
                    y = size().height()-bottom+(bottom-req.height())/2;
                    lbottom_ = std::max(lbottom_, req.height());

                    if (w < csize.width()) {
                        if (ALIGN_CENTER == align_) { x += (csize.width()-w)/2; }
                        else if (ALIGN_END == align_) { x += csize.width()-w; }
                    }
                }

                else if (LABEL_LEFT == lpos_) {
                    unsigned new_left = std::max(left, req.width());
                    csize.decrease(new_left-left, 0);
                    corigin.translate(new_left-left, 0);
                    left = new_left;
                    x = 0;
                    y = corigin.y();
                    lleft_ = std::max(lleft_, req.width());

                    if (h < csize.height()) {
                        if (ALIGN_CENTER == align_) { y += (csize.height()-h)/2; }
                        else if (ALIGN_END == align_) { y += csize.height()-h; }
                    }
                }

                else if (LABEL_RIGHT == lpos_) {
                    unsigned new_right = std::max(right, req.width());
                    csize.decrease(new_right-right, 0);
                    right = new_right;
                    x = size().width()-right;
                    y = corigin.y();
                    lright_ = std::max(lright_, req.width());

                    if (h < csize.height()) {
                        if (ALIGN_CENTER == align_) { y += (csize.height()-h)/2; }
                        else if (ALIGN_END == align_) { y += csize.height()-h; }
                    }
                }

                else {
                    unsigned new_top = std::max(top, req.height());
                    csize.decrease(0, new_top-top);
                    corigin.translate(0, new_top-top);
                    top = new_top;
                    x = corigin.x();
                    y = (top-req.height())/2;
                    ltop_ = std::max(ltop_, req.height());

                    if (w < csize.width()) {
                        if (ALIGN_CENTER == align_) { x += (csize.width()-w)/2; }
                        else if (ALIGN_END == align_) { x += csize.width()-w; }
                    }
                }

                Rect was(label_->origin(), label_->size());
                Point origin(x, y);
                Size size(w, h);
                lb_.set(origin, size);
                origin += label_->margin_origin();
                size.decrease(label_->margin_hint());
                bool changed = false;
                if (label_->update_origin(origin)) { changed = true; }
                if (label_->update_size(size)) { changed = true; }
                if (changed) { invalidate(was|lb_); }
                changed = false;
            }
        }
    }

    if (cp_) {
        Rect was(cp_->origin(), cp_->size());
        corigin += cp_->margin_origin();
        csize.decrease(cp_->margin_hint());
        bool changed = false;
        if (cp_->update_origin(corigin)) { changed = true; }
        if (cp_->update_size(csize)) { changed = true; }
        if (changed) { invalidate(was|Rect(corigin, csize)); }
    }
}

void Frame_impl::on_label_hide() {
    lb_.reset();
    label_->update_origin(INT_MIN, INT_MIN);
    label_->update_size(0, 0);
    update_requisition();
    queue_arrange();
}

void Frame_impl::on_label_show() {
    update_requisition();
    queue_arrange();
}

void Frame_impl::on_child_hide() {
    cp_->update_origin(INT_MIN, INT_MIN);
    cp_->update_size(0, 0);
    update_requisition();
    queue_arrange();
}

void Frame_impl::on_child_show() {
    update_requisition();
    queue_arrange();
}

void Frame_impl::align_label(Align align) {
    if (align_ != align) {
        align_ = align;
        if (label_) { arrange(); }
    }
}

void Frame_impl::paint_border(Painter pr) {
    Size ws = size();
    if (!ws) { return; }

    static const double DARKEN = 0.15, LITEN = 0.1;
    const unsigned r0 = eradius();

    Color c, c2;

    if (0 != border_left_) {
        int x0 = 0, y0 = (ltop_-border_top_)/2, y1 = ws.height()-(lbottom_-border_bottom_)/2, y2 = y1;

        if (lb_ && LABEL_LEFT == lpos_) {
            x0 = lb_.center().x()-(border_left_/2);
            y2 = lb_.top();
        }

        c = border_left_color_set_ ? border_left_color_ : style().color("background"), c2 = c;

        y0 += r0;
        y2 = y2 == y1 ? y2-r0 : y2;
        y1 -= r0;

        switch (border_left_style_) {
            case BORDER_SOLID:
                c = border_left_color_set_ ? border_left_color_ : style().color("foreground");
                goto a1;

            case BORDER_OUTSET:
                c.lighter(LITEN);
                goto a1;

            case BORDER_INSET:
                c.darker(DARKEN);
                a1:
                pr.rectangle(x0, y0, x0+border_left_, y2);
                if (y2 != y1) { pr.rectangle(x0, lb_.bottom(), x0+border_left_, y1); }
                pr.set_brush(c);
                pr.fill();
                break;

            case BORDER_GROOVE:
                c.darker(DARKEN);
                c2.lighter(LITEN);
                goto b1;

            case BORDER_RIDGE:
                c.lighter(LITEN);
                c2.darker(DARKEN);
                b1:
                pr.rectangle(x0, y0, x0+border_left_/2, y2-(border_bottom_/2));
                if (y2 != y1) { pr.rectangle(x0, lb_.bottom(), x0+border_left_/2, y1-(border_bottom_/2)); }
                pr.set_brush(c);
                pr.fill();

                pr.rectangle(x0+border_left_/2, y0+border_top_/2, x0+border_left_, y2-(border_bottom_/2));
                if (y2 != y1) { pr.rectangle(x0+border_left_/2, lb_.bottom(), x0+border_left_, y1-(border_bottom_/2)); }
                pr.set_brush(c2);
                pr.fill();
                break;

            default:
                ;
        }
    }

    if (0 != border_right_) {
        int x0 = ws.width()-border_right_, y0 = (ltop_-border_top_)/2, y1 = ws.height()-(lbottom_-border_bottom_)/2, y2 = y1;

        if (lb_ && LABEL_RIGHT == lpos_) {
            y2 = lb_.top();
            x0 = lb_.center().x()-(border_right_/2);
        }

        c = border_right_color_set_ ? border_right_color_ : style().get("background").get(), c2 = c;

        y0 += r0;
        y2 = y2 == y1 ? y2-r0 : y2;
        y1 -= r0;

        switch (border_right_style_) {
            case BORDER_SOLID:
                c = border_right_color_set_ ? border_right_color_ : style().get("foreground").get();
                goto a2;

            case BORDER_OUTSET:
                c.darker(DARKEN);
                goto a2;

            case BORDER_INSET:
                c.lighter(LITEN);
                a2:
                pr.rectangle(x0, y0, x0+border_right_, y2);
                if (y2 != y1) { pr.rectangle(x0, lb_.bottom(), x0+border_right_, y1); }
                pr.set_brush(c);
                pr.fill();
                break;

            case BORDER_GROOVE:
                c.lighter(LITEN);
                c2.darker(DARKEN);
                goto b2;

            case BORDER_RIDGE:
                c.darker(DARKEN);
                c2.lighter(LITEN);
                b2:
                pr.rectangle(x0, y0+(border_top_/2), x0+(border_right_/2), y2-(border_bottom_/2));
                if (y2 != y1) { pr.rectangle(x0, lb_.bottom(), x0+(border_right_/2), y1-(border_bottom_/2)); }
                pr.set_brush(c);
                pr.fill();

                pr.rectangle(x0+(border_right_/2), y0, x0+border_right_, y2-(border_bottom_/2));
                if (y2 != y1) { pr.rectangle(x0+(border_right_/2), lb_.bottom(), x0+border_right_, y1-(border_bottom_/2)); }
                pr.set_brush(c2);
                pr.fill();
                break;

            default:
                ;
        }
    }

    // -------- Top --------

    if (0 != border_top_) {
        int x0 = (lleft_-border_left_)/2, x1 = ws.width()-(lright_-border_right_)/2, x2 = x1, y0 = 0;

        if (lb_ && LABEL_TOP == lpos_) {
            x2 = lb_.left();
            y0 = lb_.center().y()-(border_top_/2);
        }

        c = border_top_color_set_ ? border_top_color_ : style_.color("background"), c2 = c;

        x0 += r0;
        x2 = x2 == x1 ? x2-r0 : x2;
        x1 -= r0;

        switch (border_top_style_) {
            case BORDER_SOLID:
                c = border_top_color_set_ ? border_top_color_ : style_.color("foreground");
                goto a3;
                break;

            case BORDER_INSET:
                c.darker(DARKEN);
                goto a3;

            case BORDER_OUTSET:
                c.lighter(LITEN);
                a3:
                pr.rectangle(x0, y0, x2, y0+border_top_);
                if (x2 != x1) { pr.rectangle(lb_.right(), y0, x1, y0+border_top_); }
                pr.set_brush(c);
                pr.fill();

                if (r0) {
                    pr.arc(r0+(lleft_-border_left_)/2, y0+r0, r0-(border_top_/2), PI, PI/2);
                    pr.arc(ws.width()-r0-1-(lright_-border_right_)/2, y0+r0, r0-(border_top_/2), 0, PI/2);
                    pr.set_pen(Pen(c, border_top_));
                    pr.stroke();
                }

                break;

            case BORDER_GROOVE:
                c.darker(DARKEN);
                c2.lighter(LITEN);
                goto b3;

            case BORDER_RIDGE:
                c.lighter(LITEN);
                c2.darker(DARKEN);
                b3:
                pr.rectangle(x0, y0, x2, y0+(border_top_/2));
                if (x2 != x1) { pr.rectangle(lb_.right(), y0, x1, y0+(border_top_/2)); }
                pr.set_brush(c);
                pr.fill();

                pr.rectangle(x0+(border_left_/2), y0+(border_top_/2), x2-(border_right_/2), y0+border_top_);
                if (x2 != x1) { pr.rectangle(lb_.right(), y0+(border_top_/2), x1-(border_right_/2), y0+border_top_); }
                pr.set_brush(c2);
                pr.fill();

                if (r0) {
                    pr.arc(r0+(lleft_-border_left_)/2, y0+r0, r0-(border_top_/4), PI, PI/2);
                    pr.arc(ws.width()-r0-1-(lright_-border_right_)/2, y0+r0, r0-(border_top_/4), 0, PI/2);
                    pr.set_pen(Pen(c, border_top_/2));
                    pr.stroke();

                    pr.arc(r0+(lleft_-border_left_)/2, y0+r0, r0-(border_top_/2), PI, PI/2);
                    pr.arc(ws.width()-r0-1-(lright_-border_right_)/2, y0+r0, r0-(border_top_/2), 0, PI/2);
                    pr.set_pen(Pen(c2, border_top_/2));
                    pr.stroke();
                }

                break;

            default:
                ;
        }
    }

    // -------- Bottom --------

    if (0 != border_bottom_) {
        int x0 = (lleft_-border_left_)/2, x1 = ws.width()-(lright_-border_right_)/2, x2 = x1, y0 = ws.height()-border_bottom_;

        if (lb_ && LABEL_BOTTOM == lpos_) {
            y0 = lb_.center().y()-(border_bottom_/2);
            x2 = lb_.left();
        }

        c = border_bottom_color_set_ ? border_bottom_color_ : style_.color("background"), c2 = c;

        x0 += r0;
        x2 = x2 == x1 ? x2-r0 : x2;
        x1 -= r0;

        switch (border_bottom_style_) {
            case BORDER_SOLID:
                c = border_bottom_color_set_ ? border_bottom_color_ : style_.color("foreground");
                goto a4;

            case BORDER_INSET:
                c.lighter(LITEN);
                goto a4;

            case BORDER_OUTSET:
                c.darker(DARKEN);
                a4:
                pr.rectangle(x0, y0, x2, y0+border_bottom_);
                if (x2 != x1) { pr.rectangle(lb_.right(), y0, x1, y0+border_bottom_); }
                pr.set_brush(c);
                pr.fill();

                if (r0) {
                    int y = ws.height()-(lbottom_-border_bottom_)/2-r0-1;
                    pr.arc(r0+(lleft_-border_left_)/2, y, r0-(border_bottom_/2), PI, 3*PI/2);
                    pr.arc(ws.width()-r0-1-(lright_-border_right_)/2, y, r0-(border_bottom_/2), 0, -PI/2);
                    pr.set_pen(Pen(c, border_bottom_));
                    pr.stroke();
                }

                break;

            case BORDER_GROOVE:
                c.lighter(LITEN);
                c2.darker(DARKEN);
                goto b4;

            case BORDER_RIDGE:
                c.darker(DARKEN);
                c2.lighter(LITEN);
                b4:
                pr.rectangle(x0+(border_left_/2), y0, x2-(border_right_/2), y0+(border_bottom_/2));
                if (x2 != x1) { pr.rectangle(lb_.right(), y0, x1-(border_right_/2), y0+(border_bottom_/2)); }
                pr.set_brush(c);
                pr.fill();

                pr.rectangle(x0, y0+(border_bottom_/2), x2, y0+border_bottom_);
                if (x2 != x1) { pr.rectangle(lb_.right(), y0+(border_bottom_/2), x1, y0+border_bottom_); }
                pr.set_brush(c2);
                pr.fill();

                if (r0) {
                    int y = ws.height()-(lbottom_-border_bottom_)/2-r0-1;
                    pr.arc(r0+(lleft_-border_left_)/2, y, r0-(border_top_/4), PI, 3*PI/2);
                    pr.arc(ws.width()-r0-1-(lright_-border_right_)/2, y, r0-(border_top_/4), 3*PI/2, 2*PI);
                    pr.set_pen(Pen(c2, border_top_/2));
                    pr.stroke();

                    pr.arc(r0+(lleft_-border_left_)/2, y, r0-(border_top_/2), PI, 3*PI/2);
                    pr.arc(ws.width()-r0-1-(lright_-border_right_)/2, y, r0-(border_top_/2), 3*PI/2, 2*PI);
                    pr.set_pen(Pen(c, border_top_/2));
                    pr.stroke();
                }

                break;

            default:
                ;
        }
    }
}

void Frame_impl::paint_background(Painter pr, const Rect & inval) {
    Color bg; if (parent_) { bg = parent_->style().color("background"); }
    unsigned r0 = mradius();

    // Erase corners.
    pr.rectangle(0, 0, r0, r0);
    pr.rectangle(size().width()-r0, 0, size().width(), r0);
    pr.rectangle(0, size().height()-r0, r0, size().height());
    pr.rectangle(size().width()-r0, size().height()-r0, size().width(), size().height());
    pr.set_brush(bg);
    pr.fill();

    Color fg = style().color("background");
    pr.arc(r0, r0, r0, PI, PI/2, true);
    pr.arc(size().width()-r0, r0, r0, 0, PI/2, true);
    pr.arc(size().width()-r0, size().height()-r0, r0, 0, -PI/2, true);
    pr.arc(r0, size().height()-r0, r0, PI, 3*PI/2, true);
    pr.rectangle(r0, 0, size().width()-r0, r0);
    pr.rectangle(0, r0, size().width(), size().height()-r0);
    pr.rectangle(r0, size().height()-r0, size().width()-r0, size().height());
    pr.set_brush(fg);
    pr.fill();
}

bool Frame_impl::on_backpaint(Painter pr, const Rect & inval) {
    if (mradius() && style().get("background").is_set()) {
        paint_background(pr, inval);
        return true;
    }

    return false;
}

bool Frame_impl::on_paint(Painter pr, const Rect & inval) {
    paint_border(pr);
    return false;
}

void Frame_impl::set_label_position(Label_pos label_pos) {
    if (lpos_ != label_pos) {
        lpos_ = label_pos;

        if (label_) {
            lb_.reset();
            queue_arrange();
        }
    }
}

unsigned Frame_impl::min_border_size(Border_style bs) {
    switch (bs) {
        case BORDER_SOLID:
        case BORDER_DOTTED:
        case BORDER_DASHED:
        case BORDER_INSET:
        case BORDER_OUTSET:
            return 1;

        case BORDER_GROOVE:
        case BORDER_RIDGE:
            return 2;

        case BORDER_DOUBLE:
            return 3;

        default: ;
    }

    return 0;
}

bool Frame_impl::set_left_style(Border_style bs) {
    if (border_left_style_ != bs) {
        unsigned px = min_border_size(bs);
        border_left_style_ = 0 != px ? bs : BORDER_NONE;
        set_left_border(std::max(left_, px));
        return true;
    }

    return false;
}

bool Frame_impl::set_right_style(Border_style bs) {
    if (border_right_style_ != bs) {
        unsigned px = min_border_size(bs);
        border_right_style_ = 0 != px ? bs : BORDER_NONE;
        set_right_border(std::max(right_, px));
        return true;
    }

    return false;
}

bool Frame_impl::set_top_style(Border_style bs) {
    if (border_top_style_ != bs) {
        unsigned px = min_border_size(bs);
        border_top_style_ = 0 != px ? bs : BORDER_NONE;
        set_top_border(std::max(top_, px));
        return true;
    }

    return false;
}

bool Frame_impl::set_bottom_style(Border_style bs) {
    if (border_bottom_style_ != bs) {
        unsigned px = min_border_size(bs);
        border_bottom_style_ = 0 != px ? bs : BORDER_NONE;
        set_bottom_border(std::max(bottom_, px));
        return true;
    }

    return false;
}

bool Frame_impl::set_style(Border_style bs) {
    bool changed = false;
    if (set_left_style(bs)) { changed = true; }
    if (set_right_style(bs)) { changed = true; }
    if (set_top_style(bs)) { changed = true; }
    if (set_bottom_style(bs)) { changed = true; }
    return changed;
}

void Frame_impl::set_border_left_style(Border_style bs) {
    if (set_left_style(bs)) {
        queue_arrange();
        invalidate();
    }
}

void Frame_impl::set_border_right_style(Border_style bs) {
    if (set_right_style(bs)) {
        queue_arrange();
        invalidate();
    }
}

void Frame_impl::set_border_top_style(Border_style bs) {
    if (set_top_style(bs)) {
        queue_arrange();
        invalidate();
    }
}

void Frame_impl::set_border_bottom_style(Border_style bs) {
    if (set_bottom_style(bs)) {
        queue_arrange();
        invalidate();
    }
}

void Frame_impl::set_border_style(Border_style left, Border_style right, Border_style top, Border_style bottom) {
    bool changed = false;

    if (set_left_style(left)) { changed = true; }
    if (set_right_style(right)) { changed = true; }
    if (set_top_style(top)) { changed = true; }
    if (set_bottom_style(bottom)) { changed = true; }

    if (changed) { queue_arrange(); invalidate(); }
}

// Overriden by Counter_impl.
void Frame_impl::set_border_style(Border_style bs) {
    if (set_style(bs)) { queue_arrange(); invalidate(); }
}

bool Frame_impl::set_left_color(const Color & color) {
    if (!border_left_color_set_ || border_left_color_ != color) {
        border_left_color_ = color;
        border_left_color_set_ = true;
        return 0 != border_left_;
    }

    return false;
}

bool Frame_impl::set_right_color(const Color & color) {
    if (!border_right_color_set_ || border_right_color_ != color) {
        border_right_color_ = color;
        border_right_color_set_ = true;
        return 0 != border_right_;
    }

    return false;
}

bool Frame_impl::set_top_color(const Color & color) {
    if (!border_top_color_set_ || border_top_color_ != color) {
        border_top_color_ = color;
        border_top_color_set_ = true;
        return 0 != border_top_;
    }

    return false;
}

bool Frame_impl::set_bottom_color(const Color & color) {
    if (!border_bottom_color_set_ || border_bottom_color_ != color) {
        border_bottom_color_ = color;
        border_bottom_color_set_ = true;
        return 0 != border_bottom_;
    }

    return false;
}

bool Frame_impl::set_colors(const Color & color) {
    bool changed = set_left_color(color);
    if (set_right_color(color)) { changed = true; }
    if (set_top_color(color)) { changed = true; }
    if (set_bottom_color(color)) { changed = true; }
    return changed;
}

void Frame_impl::set_border_left_color(const Color & color) {
    if (set_left_color(color)) { invalidate(); }
}

void Frame_impl::set_border_right_color(const Color & color) {
    if (set_right_color(color)) { invalidate(); }
}

void Frame_impl::set_border_top_color(const Color & color) {
    if (set_top_color(color)) { invalidate(); }
}

void Frame_impl::set_border_bottom_color(const Color & color) {
    if (set_bottom_color(color)) { invalidate(); }
}

void Frame_impl::set_border_color(const Color & color) {
    if (set_colors(color)) { invalidate(); }
}

void Frame_impl::unset_border_color() {
    bool changed = false;

    if (border_left_color_set_) {
        border_left_color_set_ = false;
        if (0 != border_left_) { changed = true; }
    }

    if (border_right_color_set_) {
        border_right_color_set_ = false;
        if (0 != border_right_) { changed = true; }
    }

    if (border_top_color_set_) {
        border_top_color_set_ = false;
        if (0 != border_top_) { changed = true; }
    }

    if (border_bottom_color_set_) {
        border_bottom_color_set_ = false;
        if (0 != border_bottom_) { changed = true; }
    }

    if (changed) { invalidate(); }
}

void Frame_impl::unset_border_left_color() {
    if (border_left_color_set_) {
        border_left_color_set_ = false;
        if (0 != border_left_) { invalidate(); }
    }
}

void Frame_impl::unset_border_right_color() {
    if (border_right_color_set_) {
        border_right_color_set_ = false;
        if (0 != border_right_) { invalidate(); }
    }
}

void Frame_impl::unset_border_top_color() {
    if (border_top_color_set_) {
        border_top_color_set_ = false;
        if (0 != border_top_) { invalidate(); }
    }
}

void Frame_impl::unset_border_bottom_color() {
    if (border_bottom_color_set_) {
        border_bottom_color_set_ = false;
        if (0 != border_bottom_) { invalidate(); }
    }
}

void Frame_impl::set_border_color(const Color & left, const Color & right, const Color & top, const Color & bottom) {
    bool changed = set_left_color(left);
    if (set_right_color(right)) { changed = true; }
    if (set_top_color(top)) { changed = true; }
    if (set_bottom_color(bottom)) { changed = true; }
    if (changed) { invalidate(); }
}

bool Frame_impl::set_left_border(unsigned px) {
    px = ceil_border(px, border_left_style_);
    px = std::max(px, eradius());

    if (left_ != px) {
        left_ = px;
        return true;
    }

    return false;
}

bool Frame_impl::set_right_border(unsigned px) {
    px = ceil_border(px, border_right_style_);
    px = std::max(px, eradius());

    if (right_ != px) {
        right_ = px;
        return true;
    }

    return false;
}

bool Frame_impl::set_top_border(unsigned px) {
    px = ceil_border(px, border_top_style_);
    px = std::max(px, eradius());

    if (top_ != px) {
        top_ = px;
        return true;
    }

    return false;
}

bool Frame_impl::set_bottom_border(unsigned px) {
    px = ceil_border(px, border_bottom_style_);
    px = std::max(px, eradius());

    if (bottom_ != px) {
        bottom_ = px;
        return true;
    }

    return false;
}

bool Frame_impl::set_borders(unsigned px) {
    bool changed = set_left_border(px);
    if (set_right_border(px)) { changed = true; }
    if (set_top_border(px)) { changed = true; }
    if (set_bottom_border(px)) { changed = true; }
    return changed;
}

void Frame_impl::set_border_left(unsigned px) {
    if (set_left_border(px)) {
        update_requisition();
        queue_arrange();
    }
}

void Frame_impl::set_border_right(unsigned px) {
    if (set_right_border(px)) {
        update_requisition();
        queue_arrange();
    }
}

void Frame_impl::set_border_top(unsigned px) {
    if (set_top_border(px)) {
        update_requisition();
        queue_arrange();
    }
}

void Frame_impl::set_border_bottom(unsigned px) {
    if (set_bottom_border(px)) {
        update_requisition();
        queue_arrange();
    }
}

void Frame_impl::set_border(unsigned px) {
    if (set_borders(px)) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border(unsigned left, unsigned right, unsigned top, unsigned bottom) {
    bool changed = false;
    if (set_left_border(left)) { changed = true; }
    if (set_right_border(right)) { changed = true; }
    if (set_top_border(top)) { changed = true; }
    if (set_bottom_border(bottom)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_left(unsigned px, Border_style bs) {
    bool changed = false;
    if (set_left_style(bs)) { changed = true; }
    if (set_left_border(px)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_right(unsigned px, Border_style bs) {
    bool changed = false;
    if (set_right_style(bs)) { changed = true; }
    if (set_right_border(px)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_top(unsigned px, Border_style bs) {
    bool changed = false;
    if (set_top_style(bs)) { changed = true; }
    if (set_top_border(px)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_bottom(unsigned px, Border_style bs) {
    bool changed = false;
    if (set_bottom_style(bs)) { changed = true; }
    if (set_bottom_border(px)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border(unsigned px, Border_style bs) {
    bool changed = set_style(bs);
    if (set_borders(px)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_left(unsigned px, Border_style bs, const Color & color) {
    bool changed = false;
    if (set_left_style(bs)) { changed = true; }
    if (set_left_border(px)) { changed = true; }
    if (set_left_color(color)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_right(unsigned px, Border_style bs, const Color & color) {
    bool changed = false;
    if (set_right_style(bs)) { changed = true; }
    if (set_right_border(px)) { changed = true; }
    if (set_right_color(color)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_top(unsigned px, Border_style bs, const Color & color) {
    bool changed = false;
    if (set_top_style(bs)) { changed = true; }
    if (set_top_border(px)) { changed = true; }
    if (set_top_color(color)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border_bottom(unsigned px, Border_style bs, const Color & color) {
    bool changed = false;
    if (set_bottom_style(bs)) { changed = true; }
    if (set_bottom_border(px)) { changed = true; }
    if (set_bottom_color(color)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

void Frame_impl::set_border(unsigned px, Border_style bs, const Color & color) {
    bool changed = set_style(bs);
    if (set_borders(px)) { changed = true; }
    if (set_colors(color)) { changed = true; }
    if (changed) { update_requisition(); queue_arrange(); }
}

unsigned Frame_impl::ceil_border(unsigned px, Border_style bs) {
    unsigned min_px = min_border_size(bs);
    if (min_px < 2) { return px; }
    unsigned quo = px/min_px, rem = px%min_px;
    return rem ? (1+quo)*min_px : px;
}

void Frame_impl::set_border_radius(int radius) {
    unsigned uradius = std::max(0, radius);

    if (radius_ != uradius) {
        radius_ = uradius;
        update_requisition();
        queue_arrange();
    }
}

unsigned Frame_impl::eradius() const {
    unsigned rmin = left_ && right_ && top_ && bottom_ ? radius_ : 0;
    return std::min(rmin, size().min()/2);
}

unsigned Frame_impl::mradius() const {
    return std::min(radius_, size().min()/2);
}

} // namespace tau

//END
