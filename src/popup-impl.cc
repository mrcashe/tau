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
    int x, y;
    std::size_t w = required_size().width(), h = required_size().height();

    switch (gravity_) {
        case GRAVITY_LEFT:
            x = upos_.x();
            y = upos_.y()-h/2;
            break;

        case GRAVITY_RIGHT:
            x = upos_.x()-w;
            y = upos_.y()-h/2;
            break;

        case GRAVITY_TOP:
            x = upos_.x()-w/2;
            y = upos_.y();
            break;

        case GRAVITY_BOTTOM:
            x = upos_.x()-w/2;
            y = upos_.y()-h;
            break;

        case GRAVITY_TOP_LEFT:
            x = upos_.x();
            y = upos_.y();
            break;

        case GRAVITY_TOP_RIGHT:
            x = upos_.x()-w;
            y = upos_.y();
            break;

        case GRAVITY_BOTTOM_LEFT:
            x = upos_.x();
            y = upos_.y()-h;
            break;

        case GRAVITY_BOTTOM_RIGHT:
            x = upos_.x()-w;
            y = upos_.y()-h;
            break;

        case GRAVITY_CENTER:
            x = upos_.x()-w/2;
            y = upos_.y()-h/2;
            break;

        default:
            return;
    }

    if (wpp_) {
        if (x+w > wpp_->size().width()) { x = wpp_->size().width()-w; }
        else if (x < 0) { x = 0; }

        if (y+h > wpp_->size().height()) { y = wpp_->size().height()-h; }
        else if (y < 0) { y = 0; }

        move(Rect(x, y, required_size()));
    }
}

} // namespace tau

//END
