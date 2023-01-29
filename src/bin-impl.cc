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

#include <bin-impl.hh>
#include <iostream>

namespace tau {

Bin_impl::Bin_impl():
    Container_impl()
{
    signal_arrange_.connect(fun(this, &Bin_impl::arrange));
    signal_size_changed_.connect(fun(this, &Bin_impl::arrange));
    signal_visible_.connect(fun(this, &Bin_impl::arrange));
    signal_display_.connect(fun(this, &Bin_impl::update_requisition));
}

void Bin_impl::insert(Widget_ptr wp) {
    clear();
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    make_child(wp);
    cp_ = wp.get();
    hints_cx_ = cp_->signal_hints_changed().connect(fun(this, &Bin_impl::update_requisition));
    req_cx_ = cp_->signal_requisition_changed().connect(fun(this, &Bin_impl::update_requisition));
    show_cx_ = cp_->signal_show().connect(fun(this, &Bin_impl::on_child_show));
    hide_cx_ = cp_->signal_hide().connect(fun(this, &Bin_impl::on_child_hide));
    focus_cx_ = signal_take_focus_.connect(fun(cp_, &Widget_impl::take_focus));
    update_requisition();
    arrange();
}

void Bin_impl::clear() {
    if (cp_) {
        hints_cx_.drop();
        req_cx_.drop();
        show_cx_.drop();
        hide_cx_.drop();
        focus_cx_.drop();
        unparent_child(cp_);
        cp_ = nullptr;
        update_requisition();
        invalidate();
    }
}

void Bin_impl::update_requisition() {
    if (!shut_) {
        Size rs;

        if (cp_ && !cp_->hidden()) {
            Size rq = cp_->required_size();
            rs += cp_->margin_hint();
            if (rq) { rs += rq; }
            else { rs.increase(1, 1); }
        }

        require_size(rs);
    }
}

void Bin_impl::arrange() {
    bool changed = false;

    if (cp_ && !cp_->hidden()) {
        if (cp_->update_origin(cp_->margin_origin())) { changed = true; }
        if (cp_->update_size(size()-cp_->margin_hint())) { changed = true; }
    }

    if (changed) { invalidate(); }
}

void Bin_impl::on_child_show() {
    if (!shut_) {
        update_requisition();
        arrange();
        if (focused() && cp_) { cp_->take_focus(); }
    }
}

void Bin_impl::on_child_hide() {
    if (!shut_) {
        cp_->update_origin(INT_MIN, INT_MIN);
        cp_->update_size(0, 0);
        update_requisition();
        invalidate();
    }
}

} // namespace tau

//END
