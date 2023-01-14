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
#include <scroller-impl.hh>
#include <theme-impl.hh>
#include <climits>
#include <iostream>

namespace tau {

Scroller_impl::Scroller_impl():
    Container_impl()
{
    signal_arrange_.connect(fun(this, &Scroller_impl::arrange));
    signal_mouse_down_.connect(fun(this, &Scroller_impl::on_mouse_down));
    signal_mouse_wheel_.connect(fun(this, &Scroller_impl::on_mouse_wheel));
    signal_size_changed_.connect(fun(this, &Scroller_impl::arrange));
    signal_size_changed_.connect(fun(this, &Scroller_impl::limit_scroll));
    signal_logical_size_changed_.connect(fun(this, &Scroller_impl::limit_scroll));
    signal_visible_.connect(fun(this, &Scroller_impl::arrange));
    signal_display_.connect(fun(this, &Scroller_impl::update_requisition));
    signal_take_focus_.connect(fun(this, &Scroller_impl::on_take_focus));

    auto theme = Theme_impl::root();
    auto ma = theme->find_action(PAN_LEFT_ACTION);
    if (ma) { pan_left_action_.connect_master_action(*ma); }
    ma = theme->find_action(PAN_RIGHT_ACTION);
    if (ma) { pan_right_action_.connect_master_action(*ma); }
    ma = theme->find_action(PAN_UP_ACTION);
    if (ma) { pan_up_action_.connect_master_action(*ma); }
    ma = theme->find_action(PAN_DOWN_ACTION);
    if (ma) { pan_down_action_.connect_master_action(*ma); }

    connect_action(pan_left_action_);
    connect_action(pan_right_action_);
    connect_action(pan_up_action_);
    connect_action(pan_down_action_);
    connect_action(page_up_action_);
    connect_action(page_down_action_);
    connect_action(home_action_);
    connect_action(end_action_);
}

void Scroller_impl::insert(Widget_ptr wp) {
    clear();
    make_child(wp);
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    cp_ = wp;
    req_cx_ = cp_->signal_requisition_changed().connect(fun(this, &Scroller_impl::on_child_requisition_changed));
    hint_cx_ = cp_->signal_hints_changed().connect(fun(this, &Scroller_impl::on_child_requisition_changed));
    show_cx_ = cp_->signal_show().connect(fun(this, &Scroller_impl::on_child_show));
    hide_cx_ = cp_->signal_hide().connect(fun(this, &Scroller_impl::on_child_hide));
    update_requisition();
    queue_arrange();
}

void Scroller_impl::clear() {
    if (Widget_ptr wp = cp_) {
        cp_.reset();
        req_cx_.disconnect();
        hint_cx_.disconnect();
        show_cx_.disconnect();
        hide_cx_.disconnect();
        unparent_child(wp.get());
        wp->update_origin(INT_MIN, INT_MIN);
        wp->update_size(0, 0);
        update_requisition();
        invalidate();
    }
}

Size Scroller_impl::child_requisition() const {
    unsigned w = 0, h = 0;

    if (cp_ && !cp_->hidden()) {
        Size req = cp_->required_size(), hint = cp_->size_hint(), min = cp_->min_size_hint(), max = cp_->max_size_hint();
        w = hint.width() ? hint.width() : req.width();
        w = std::max(w, min.width());
        if (max.width()) { w = std::min(w, max.width()); }

        h = hint.height() ? hint.height() : req.height();
        h = std::max(h, min.height());
        if (max.height()) { h = std::min(h, max.height()); }
    }

    return Size(w, h);
}

void Scroller_impl::update_requisition() {
    if (require_size(child_requisition())) {
        signal_logical_size_changed_();
    }
}

void Scroller_impl::arrange() {
    bool changed = false;

    if (cp_ && !cp_->hidden()) {
        if (cp_->update_origin(0, 0)) { changed = true; }
        if (cp_->update_size(size())) { changed = true; }
    }

    Size max = logical_size()-size();
    int xm = max.iwidth(), ym = max.iheight(), ox = offset().x(), oy = offset().y();
    if (ox > xm || oy > xm) { update_offset(std::min(ox, xm), std::min(oy, ym)); }
    if (changed) { invalidate(); }
}

void Scroller_impl::on_child_requisition_changed() {
    update_requisition();
}

void Scroller_impl::limit_scroll() {
    Size max = logical_size()-size();
    if (offset().x() >= max.iwidth()) { pan_to_x(max.iwidth()); }
    if (offset().y() >= max.iheight()) { pan_to_y(max.iheight()); }
}

void Scroller_impl::on_child_hide() {
    cp_->update_origin(INT_MIN, INT_MIN);
    cp_->update_size(0, 0);
    pan_to(0, 0);
    update_requisition();
    queue_arrange();
}

void Scroller_impl::on_child_show() {
    update_requisition();
    queue_arrange();
}

Size Scroller_impl::logical_size() const {
    return child_requisition();
}

Point Scroller_impl::offset() const {
    return cp_ ? pan_ : Point();
}

void Scroller_impl::pan_to(const Point & pos) {
    Size max = logical_size()-size();
    update_offset(std::min(max.iwidth(), std::max(0, pos.x())), std::min(max.iheight(), std::max(0, pos.y())));
}

void Scroller_impl::pan_to(int x, int y) {
    pan_to(Point(x, y));
}

void Scroller_impl::pan_to_x(int x) {
    pan_to(x, offset().y());
}

void Scroller_impl::pan_to_y(int y) {
    pan_to(offset().x(), y);
}

bool Scroller_impl::update_offset(const Point & pt) {
    if (cp_) {
        if (pan_.update(pt)) {
            cp_->signal_scroll_changed()();
            signal_offset_changed_();
            return true;
        }
    }

    return false;
}

bool Scroller_impl::update_offset(int x, int y) {
    return update_offset(Point(x, y));
}

void Scroller_impl::pan_left() {
    if (pan_.x() > 0) {
        int d = std::min(step_.x(), pan_.x());
        pan_to_x(pan_.x()-d);
    }
}

void Scroller_impl::pan_right() {
    Size max = logical_size()-size();

    if (0 != max.width()) {
        int d = std::min(step_.x(), max.iwidth());
        pan_to_x(pan_.x()+d);
    }
}

void Scroller_impl::pan_up() {
    if (pan_.y() > 0) {
        int d = std::min(step_.y(), pan_.y());
        pan_to_y(pan_.y()-d);
    }
}

void Scroller_impl::pan_down() {
    Size max = logical_size()-size();

    if (0 != max.height()) {
        int d = std::min(step_.y(), max.iheight());
        pan_to_y(pan_.y()+d);
    }
}

void Scroller_impl::page_up() {
    if (pan_.y() > 0) {
        int d = std::min((4*size().iheight())/5, pan_.y());
        pan_to_y(pan_.y()-d);
    }
}

void Scroller_impl::page_down() {
    Size max = logical_size()-size();

    if (0 != max.height()) {
        int d = std::min((4*size().iheight())/5, max.iheight());
        pan_to_y(pan_.y()+d);
    }
}

void Scroller_impl::home() {
    if (pan_.y() > 0) {
        pan_to_y(0);
    }
}

void Scroller_impl::end() {
    Size max = logical_size()-size();

    if (0 != max.height()) {
        pan_to_y(max.height());
    }
}

bool Scroller_impl::on_mouse_down(int mbt, int mm, const Point & pt) {
    return MBT_LEFT == mbt && MM_NONE == mm && !has_focus() && grab_focus();
}

bool Scroller_impl::on_mouse_wheel(int delta, int mm, const Point & pt) {
    if (delta < 0) {
        if ((MM_CONTROL|MM_SHIFT) & mm) { pan_left(); }
        else { pan_up(); }
    }

    else {
        if ((MM_CONTROL|MM_SHIFT) & mm) { pan_right(); }
        else { pan_down(); }
    }

    return true;
}

bool Scroller_impl::on_take_focus() {
    if (cp_) {
        return cp_->take_focus() ? true : grab_focus();
    }

    return false;
}

} // namespace tau

//END
