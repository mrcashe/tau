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

#include <absolute-impl.hh>
#include <climits>

namespace tau {

Absolute_impl::Absolute_impl():
    Container_impl()
{
    signal_arrange().connect(fun(this, &Absolute_impl::arrange));
    signal_size_changed().connect(fun(this, &Container_impl::queue_arrange));
    signal_visible().connect(fun(this, &Container_impl::queue_arrange));
    signal_display().connect(fun(this, &Absolute_impl::update_requisition));
    signal_take_focus().connect(fun(this, &Absolute_impl::on_take_focus));
}

void Absolute_impl::put(Widget_ptr wp, const Point & pos, const Size & size) {
    make_child(wp);
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    Holder hol;
    hol.wp = wp;
    hol.pos = pos;
    hol.size = size;
    hol.hints_cx = wp->signal_hints_changed().connect(tau::bind(fun(this, &Absolute_impl::on_child_hints_changed), wp.get()));
    hol.req_cx = wp->signal_requisition_changed().connect(tau::bind(fun(this, &Absolute_impl::on_child_requisition_changed), wp.get()));
    hol.show_cx = wp->signal_show().connect(tau::bind(fun(this, &Absolute_impl::on_child_show), wp.get()));
    hol.hide_cx = wp->signal_hide().connect(tau::bind(fun(this, &Absolute_impl::on_child_hide), wp.get()));
    holders_.push_back(hol);
    update_requisition();
    queue_arrange();
}

void Absolute_impl::rm_child(Holder & hol) {
    hol.hints_cx.disconnect();
    hol.req_cx.disconnect();
    hol.hide_cx.disconnect();
    hol.show_cx.disconnect();
    unparent_child(hol.wp.get());
    hol.wp->update_origin(INT_MIN, INT_MIN);
    hol.wp->update_size(0, 0);
}

void Absolute_impl::remove(Widget_impl * wi) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [wi](Holder & hol) { return wi == hol.wp.get(); });

    if (i != holders_.end()) {
        rm_child(*i);
        holders_.erase(i);
        update_requisition();
        queue_arrange();
    }
}

void Absolute_impl::clear() {
    for (Holder & hol: holders_) { rm_child(hol); }
    holders_.clear();
    update_requisition();
    queue_arrange();
}

bool Absolute_impl::empty() const {
    return holders_.empty();
}

void Absolute_impl::move(Widget_impl * wp, const Point & pos) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [wp](Holder & hol) { return wp == hol.wp.get(); });

    if (i != holders_.end()) {
        i->pos = pos;
        Rect was(wp->origin(), wp->size());

        if (wp->update_origin(pos)) {
            Rect now(pos, wp->size());
            invalidate(was|now);
            update_requisition();
            queue_arrange();
        }
    }
}

void Absolute_impl::move_rel(Widget_impl * wp, const Point & offset) {
    move(wp, wp->origin()+offset);
}

void Absolute_impl::resize(Widget_impl * wp, const Size & size) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [wp](Holder & hol) { return wp == hol.wp.get(); });

    if (i != holders_.end()) {
        i->size = size;
        Rect was(wp->origin(), wp->size());

        if (wp->update_size(size)) {
            Rect now(i->pos, wp->size());
            invalidate(was|now);
            update_requisition();
            queue_arrange();
        }
    }
}

Size Absolute_impl::child_requisition(const Holder & hol) {
    unsigned width = 0, height = 0;
    Size min = hol.wp->min_size_hint(), max = hol.wp->max_size_hint();

    if (0 != hol.size.width()) {
        width = hol.size.width();
    }

    else {
        width = hol.wp->size_hint().width();
        if (0 == width) { width = hol.wp->required_size().width(); }

        if (0 != width) {
            if (0 != min.width()) { width = std::max(width, min.width()); }
            if (0 != max.width()) { width = std::min(width, max.width()); }
        }
    }

    if (0 != hol.size.height()) {
        height = hol.size.height();
    }

    else {
        height = hol.wp->size_hint().height();
        if (0 == height) { height = hol.wp->required_size().height(); }

        if (0 != height) {
            if (0 != min.height()) { height = std::max(height, min.height()); }
            if (0 != max.height()) { height = std::min(height, max.height()); }
        }
    }

    return Size(width, height);
}

void Absolute_impl::update_requisition() {
    int xmax = 0, ymax = 0;

    for (const Holder & hol: holders_) {
        if (!hol.wp->hidden()) {
            Point bottom_right = hol.pos;
            bottom_right += child_requisition(hol);
            xmax = std::max(xmax, bottom_right.x());
            ymax = std::max(ymax, bottom_right.y());
        }
    }

    require_size(xmax, ymax);
}

void Absolute_impl::arrange() {
    Rect inval;

    for (Holder & hol: holders_) {
        if (!hol.wp->hidden()) {
            Rect was(hol.wp->origin(), hol.wp->size()); was &= Rect(0, 0, size());
            Size req = child_requisition(hol);
            bool changed = false;

            if (hol.wp->update_origin(hol.pos)) { changed = true; }
            if (hol.wp->update_size(req)) { changed = true; }
            if (changed) { inval |= (was|Rect(hol.pos, req)); }
        }
    }

    if (inval) { invalidate(inval); }
}

void Absolute_impl::on_child_hints_changed(Widget_impl * wi) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [wi](Holder & hol) { return wi == hol.wp.get(); });

    if (i != holders_.end()) {
        if (!i->size) {
            if (child_requisition(*i) != wi->size()) {
                update_requisition();
            }
        }
    }
}

void Absolute_impl::on_child_requisition_changed(Widget_impl * wi) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [wi](Holder & hol) { return wi == hol.wp.get(); });

    if (i != holders_.end()) {
        if (!i->size) {
            if (child_requisition(*i) != wi->size()) {
                update_requisition();
            }
        }
    }
}

void Absolute_impl::on_child_hide(Widget_impl * wi) {
    wi->update_origin(INT_MIN, INT_MIN);
    wi->update_size(0, 0);
    update_requisition();
    queue_arrange();
}

void Absolute_impl::on_child_show(Widget_impl * wi) {
    update_requisition();
    queue_arrange();
}

bool Absolute_impl::on_take_focus() {
    if (focused_child_ && focused_child_->take_focus()) {
        return true;
    }

    for (Holder & hol: holders_) {
        if (hol.wp->take_focus()) {
            return true;
        }
    }

    return false;
}

} // namespace tau

//END
