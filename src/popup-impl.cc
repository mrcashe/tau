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

#include <tau/exception.hh>
#include <display-impl.hh>
#include <popup-impl.hh>

namespace tau {

Popup_impl::Popup_impl(Winface_ptr winface, const Point & upos, Window_ptr wpp, Gravity gravity):
    Window_impl(),
    upos_(upos),
    gravity_(gravity)
{
    hidden_ = true;
    winface_ = winface;
    wpp_ = wpp;
    signal_visible_.connect(fun(this, &Popup_impl::on_show));
    signal_requisition_changed_.connect(fun(this, &Popup_impl::adjust));
    signal_hints_changed_.connect(fun(this, &Popup_impl::adjust));
    signal_size_changed_.connect(fun(this, &Popup_impl::adjust));
}

// Overrides Widget_impl.
Window_impl * Popup_impl::root() {
    return parent_window() ? parent_window()->root() : this;
}

// Overrides Widget_impl.
const Window_impl * Popup_impl::root() const {
    return parent_window() ? parent_window()->root() : this;
}

// Overrides Widget_impl.
// Overrides Window_impl.
Point Popup_impl::to_root(const Point & pt) const {
    return parent_window() ? pt+position() : pt;
}

// Overrides Widget_impl.
void Popup_impl::set_parent(Container_impl * parent) {
    throw user_error("Popup_impl::set_parent(): impossible to insert Popup into Container");
}

void Popup_impl::set_gravity(Gravity gravity) {
    if (gravity_ != gravity) {
        gravity_ = gravity;
        adjust();
    }
}

void Popup_impl::on_show() {
    Size rs = required_size();
    resize(rs);
}

void Popup_impl::adjust() {
    Size rs = required_size(), ps = rs;
    auto wpp = parent_window();
    if (wpp) { ps = wpp->size(); }
    Rect req(rs), pb(ps);

    switch (gravity_) {
        case GRAVITY_TOP_LEFT:
            req.translate(upos_);
            break;

        case GRAVITY_LEFT:
            req.translate(upos_.x(), upos_.y()-req.iheight()/2);
            break;

        case GRAVITY_BOTTOM_LEFT:
            req.translate(upos_.x(), upos_.y()-req.iheight());
            break;

        case GRAVITY_TOP_RIGHT:
            req.translate(upos_.x()-req.iwidth(), upos_.y());
            break;

        case GRAVITY_RIGHT:
            req.translate(upos_.x()-req.iwidth(), upos_.y()-req.iheight()/2);
            break;

        case GRAVITY_BOTTOM_RIGHT:
            req.translate(upos_.x()-req.iwidth(), upos_.y()-req.iheight());
            break;

        case GRAVITY_TOP:
            req.translate(upos_.x()-req.iwidth()/2, upos_.y());
            break;

        case GRAVITY_CENTER:
            req.translate(upos_.x()-req.iwidth()/2, upos_.y()-req.iheight()/2);
            break;

        case GRAVITY_BOTTOM:
            req.translate(upos_.x()-req.iwidth()/2, upos_.y()-req.iheight());
            break;

        default:
            return;
    }

    Rect united(pb|req);

    if (united != pb) {
        Size ds(united.size()-ps), pps(ps);
        pps.update_min(req.size());

        if (ds.width()) {
            req.update_width(pps.width());
            req.update_left((ps-req.size()).width());
        }

        if (ds.height()) {
            req.update_height(pps.height());
            req.update_top((ps-req.size()).height());
        }
    }

    move(req);
}

} // namespace tau

//END
