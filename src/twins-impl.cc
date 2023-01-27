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

#include <tau/input.hh>
#include <separator-impl.hh>
#include <twins-impl.hh>
#include <climits>
#include <iostream>

namespace tau {

Twins_impl::Twins_impl(Orientation orient, double ratio):
    Container_impl(),
    orient_(orient)
{
    signal_arrange_.connect(fun(this, &Twins_impl::arrange));
    signal_size_changed_.connect(fun(this, &Twins_impl::arrange));
    signal_visible_.connect(fun(this, &Twins_impl::arrange));
    signal_display_.connect(fun(this, &Twins_impl::update_requisition));
    signal_take_focus_.connect(fun(this, &Twins_impl::on_take_focus));

    set_ratio(ratio);
    auto sep = std::make_shared<Separator_impl>(SEPARATOR_HANDLE);
    sep->hide();
    make_child(sep);

    sep_ = sep.get();
    sep_->update_origin(INT_MIN, INT_MIN);
    sep_->update_size(0, 0);
    sep_->signal_show().connect(tau::bind(fun(this, &Twins_impl::on_child_show), sep_));
    sep_->signal_hide().connect(tau::bind(fun(this, &Twins_impl::on_child_hide), sep_));
    sep_->signal_hints_changed().connect(fun(this, &Twins_impl::update_requisition));
    sep_->signal_requisition_changed().connect(fun(this, &Twins_impl::update_requisition));
    sep_->signal_mouse_down().connect(fun(this, &Twins_impl::on_sep_mouse_down));
    sep_->signal_mouse_up().connect(fun(this, &Twins_impl::on_sep_mouse_up));
    sep_->set_cursor((OR_RIGHT == orient || OR_LEFT == orient) ? "size_hor" : "size_ver");
}

void Twins_impl::insert_first(Widget_ptr wp) {
    if (wp) {
        rm_first();
        make_child(wp);
        first_ = wp.get();
        first_->update_origin(INT_MIN, INT_MIN);
        first_->update_size(0, 0);
        first_hints_cx_ = first_->signal_hints_changed().connect(fun(this, &Twins_impl::update_requisition));
        first_req_cx_ = first_->signal_requisition_changed().connect(fun(this, &Twins_impl::update_requisition));
        first_show_cx_ = first_->signal_show().connect(tau::bind(fun(this, &Twins_impl::on_child_show), first_));
        first_hide_cx_ = first_->signal_hide().connect(tau::bind(fun(this, &Twins_impl::on_child_hide), first_));
        if (second_) { sep_->show(); }
        update_requisition();
        queue_arrange();
    }
}

void Twins_impl::insert_second(Widget_ptr wp) {
    if (wp) {
        rm_second();
        make_child(wp);
        second_ = wp.get();
        second_->update_origin(INT_MIN, INT_MIN);
        second_->update_size(0, 0);
        second_hints_cx_ = second_->signal_hints_changed().connect(fun(this, &Twins_impl::update_requisition));
        second_req_cx_ = second_->signal_requisition_changed().connect(fun(this, &Twins_impl::update_requisition));
        second_show_cx_ = second_->signal_show().connect(tau::bind(fun(this, &Twins_impl::on_child_show), second_));
        second_hide_cx_ = second_->signal_hide().connect(tau::bind(fun(this, &Twins_impl::on_child_hide), second_));
        if (first_) { sep_->show(); }
        update_requisition();
        queue_arrange();
    }
}

void Twins_impl::rm_first() {
    if (first_) {
        first_hints_cx_.drop();
        first_req_cx_.drop();
        first_show_cx_.drop();
        first_hide_cx_.drop();
        unparent_child(first_);
        first_->update_origin(INT_MIN, INT_MIN);
        first_->update_size(0, 0);
        first_ = nullptr;
    }
}

void Twins_impl::rm_second() {
    if (second_) {
        second_hints_cx_.drop();
        second_req_cx_.drop();
        second_show_cx_.drop();
        second_hide_cx_.drop();
        unparent_child(second_);
        second_->update_origin(INT_MIN, INT_MIN);
        second_->update_size(0, 0);
        second_ = nullptr;
    }
}

void Twins_impl::remove_first() {
    if (first_) {
        rm_first();
        sep_->hide();
        update_requisition();
        queue_arrange();
        invalidate();
    }
}

void Twins_impl::remove_second() {
    if (second_) {
        rm_second();
        sep_->hide();
        update_requisition();
        queue_arrange();
        invalidate();
    }
}

void Twins_impl::clear() {
    bool need_arrange = false;

    if (first_) {
        rm_first();
        need_arrange = true;
    }

    if (second_) {
        rm_second();
        need_arrange = true;
    }

    if (need_arrange) {
        update_requisition();
        queue_arrange();
    }
}

void Twins_impl::on_child_hide(Widget_impl * wi) {
    wi->update_origin(INT_MIN, INT_MIN);
    wi->update_size(0, 0);
    update_requisition();
    queue_arrange();
}

void Twins_impl::on_child_show(Widget_impl * wi) {
    update_requisition();
    queue_arrange();
}

Size Twins_impl::child_requisition(Widget_impl * wi) {
    Size req(wi->required_size()), min(wi->min_size_hint()), max(wi->max_size_hint()),
         hint(wi->size_hint()), margins(wi->margin_hint());

    unsigned w = 0 == hint.width() ? req.width() : hint.width();
    unsigned h = 0 == hint.height() ? req.height() : hint.height();

    if (0 != min.width()) { w = std::max(w, min.width()); }
    if (0 != min.height()) { h = std::max(h, min.height()); }
    if (0 != max.width()) { w = std::min(w, max.width()); }
    if (0 != max.height()) { h = std::min(h, max.height()); }

    w += margins.width(); h += margins.height();
    return Size(w, h);
}

void Twins_impl::update_requisition() {
    Size rq;

    if (first_ && !first_->hidden() && (!second_ || second_->hidden())) {
        rq += child_requisition(first_);
    }

    else if (second_ && !second_->hidden() && (!first_ || first_->hidden())) {
        rq += child_requisition(second_);
    }

    else if (first_ && second_ && !first_->hidden() && !second_->hidden()) {
        Size req0(child_requisition(sep_)), req1(child_requisition(first_)), req2(child_requisition(second_));

        if (OR_UP == orient_ || OR_DOWN == orient_) {
            rq.increase(0, req1.height()+req0.height()+req2.height());
            rq.update_width(std::max(req1.width(), req2.width()));
        }

        else {
            rq.increase(req1.width()+req0.width()+req2.width(), 0);
            rq.update_height(std::max(req1.height(), req2.height()));
        }
    }

    require_size(rq);
    queue_arrange();
}

void Twins_impl::arrange() {
    bool inval = false;

    if (first_ && !first_->hidden() && (!second_ || second_->hidden())) {
        if (first_->update_origin(first_->margin_origin())) { inval = true; }
        if (first_->update_size(size()-first_->margin_hint())) { inval = true; }
    }

    else if (second_ && !second_->hidden() && (!first_ || first_->hidden())) {
        if (second_->update_origin(second_->margin_origin())) { inval = true; }
        if (second_->update_size(size()-second_->margin_hint())) { inval = true; }
    }

    else if (first_ && second_ && !first_->hidden() && !second_->hidden()) {
        if (OR_UP == orient_) {
            unsigned spc = child_requisition(sep_).height(), h2 = (1.0-ratio_)*(size().height()-spc);
            if (second_->update_origin(second_->margin_origin())) { inval = true; }
            if (second_->update_size(Size(size().width(), h2)-second_->margin_hint())) { inval = true; }
            if (first_->update_origin(Point(0, h2+spc)+first_->margin_origin())) { inval = true; }
            if (first_->update_size(Size(size().width(), size().height()-h2-spc)-first_->margin_hint())) { inval = true; }
            if (sep_->update_origin(0, h2)) { inval = true; }
            if (sep_->update_size(size().width(), spc)) { inval = true; }
        }

        else if (OR_RIGHT == orient_) {
            unsigned spc = child_requisition(sep_).width(), w1 = ratio_*(size().width()-spc);
            if (first_->update_origin(first_->margin_origin())) { inval = true; }
            if (first_->update_size(Size(w1, size().height())-first_->margin_hint())) { inval = true; }
            if (second_->update_origin(Point(w1+spc, 0)+second_->margin_origin())) { inval = true; }
            if (second_->update_size(Size(size().width()-w1-spc, size().height())-second_->margin_hint())) { inval = true; }
            if (sep_->update_origin(w1, 0)) { inval = true; }
            if (sep_->update_size(spc, size().height())) { inval = true; }
        }

        else if (OR_LEFT == orient_) {
            unsigned spc = child_requisition(sep_).width(), w2 = (1.0-ratio_)*(size().width()-spc);
            if (second_->update_origin(second_->margin_origin())) { inval = true; }
            if (second_->update_size(Size(w2, size().height())-second_->margin_hint())) { inval = true; }
            if (first_->update_origin(Point(w2+spc, 0)+first_->margin_origin())) { inval = true; }
            if (first_->update_size(Size(size().width()-w2-spc, size().height())-first_->margin_hint())) { inval = true; }
            if (sep_->update_origin(w2, 0)) { inval = true; }
            if (sep_->update_size(spc, size().height())) { inval = true; }
        }

        else {
            unsigned spc = child_requisition(sep_).height(), h1 = ratio_*(size().height()-spc);
            if (first_->update_origin(first_->margin_origin())) { inval = true; }
            if (first_->update_size(Size(size().width(), h1)-first_->margin_hint())) { inval = true; }
            if (second_->update_origin(Point(0, h1+spc)+second_->margin_origin())) { inval = true; }
            if (second_->update_size(Size(size().width(), size().height()-h1-spc)-second_->margin_hint())) { inval = true; }
            if (sep_->update_origin(0, h1)) { inval = true; }
            if (sep_->update_size(size().width(), spc)) { inval = true; }
        }
    }

    if (inval) { invalidate(); }
}

void Twins_impl::set_ratio(double ratio) {
    ratio_ = std::max(0.0, std::min(1.0, ratio));
    if (first_ && !first_->hidden() && second_ && !second_->hidden()) { queue_arrange(); }
}

bool Twins_impl::on_sep_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        sep_mouse_motion_cx_ = sep_->signal_mouse_motion().connect(fun(this, &Twins_impl::on_sep_mouse_motion));
        sep_->grab_mouse();
    }

    return false;
}

bool Twins_impl::on_sep_mouse_up(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        sep_->ungrab_mouse();
        sep_mouse_motion_cx_.drop();
    }

    return false;
}

void Twins_impl::on_sep_mouse_motion(int mm, const Point & pt) {
    Size sz(size());
    Point ptp = sep_->to_parent(pt);
    int dmin, dmax;
    unsigned spc;

    if (OR_RIGHT == orient_ || OR_LEFT == orient_) {
        spc = sep_->size().width();
        dmin = spc;
        dmax = spc < sz.width() ? sz.width()-spc : sz.width();

        if (ptp.x() >= dmin && ptp.x() < dmax) {
            double ratio = double(ptp.x())/dmax;
            if (OR_LEFT == orient_) { ratio = 1.0-ratio; }
            set_ratio(ratio);
            signal_ratio_changed_(ratio);
        }
    }

    else {
        spc = sep_->size().height();
        dmin = spc;
        dmax = spc < sz.height() ? sz.height()-spc : sz.height();

        if (ptp.y() >= dmin && ptp.y() < dmax) {
            double ratio = double(ptp.y())/dmax;
            if (OR_UP == orient_) { ratio = 1.0-ratio; }
            set_ratio(ratio);
            signal_ratio_changed_(ratio);
        }
    }
}

bool Twins_impl::on_take_focus() {
    if (focused_child_ && focused_child_->take_focus()) { return true; }
    if (first_ && first_->take_focus()) { return true; }
    if (second_ && second_->take_focus()) { return true; }
    return grab_focus();
}

} // namespace tau

//END
