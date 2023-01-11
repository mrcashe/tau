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

#include <button-impl.hh>
#include <roller-impl.hh>
#include <scroller-impl.hh>
#include <iostream>

namespace tau {

Roller_impl::Roller_impl(Orientation orient, bool autohide):
    Box_impl(orient),
    autohide_(autohide)
{
    disallow_focus();
    set_spacing(3);
    scroller_ = std::make_shared<Scroller_impl>();
    append(scroller_);
    ustring start_icon = horizontal() ? "picto-left" : "picto-up";
    set_start(std::make_shared<Button_impl>(start_icon, 12));
    ustring end_icon = horizontal() ? "picto-right" : "picto-down";
    set_end(std::make_shared<Button_impl>(end_icon, 12));
    scroller_->signal_offset_changed().connect(fun(this, &Roller_impl::update_buttons));
    scroller_->signal_size_changed().connect(fun(this, &Roller_impl::update_buttons));
    scroller_->signal_size_changed().connect(fun(this, &Roller_impl::update_roll));
    scroller_->signal_logical_size_changed().connect(fun(this, &Roller_impl::update_buttons));
    signal_mouse_wheel().connect(fun(this, &Roller_impl::on_mouse_wheel), true);
}

void Roller_impl::insert(Widget_ptr wp) {
    scroller_->insert(wp);
}

void Roller_impl::clear() {
    scroller_->clear();
}

int Roller_impl::offset() const {
    Point pt = scroller_->offset();
    return horizontal() ? pt.x() : pt.y();
}

void Roller_impl::on_start_button_click() {
    if (horizontal()) { scroller_->pan_left_action().exec(); }
    else { scroller_->pan_up_action().exec(); }
}

void Roller_impl::on_end_button_click() {
    if (horizontal()) { scroller_->pan_right_action().exec(); }
    else { scroller_->pan_down_action().exec(); }
}

void Roller_impl::set_autohide() {
    autohide_ = true;
    update_buttons();
}

void Roller_impl::unset_autohide() {
    autohide_ = false;
    update_buttons();
}

void Roller_impl::set_step(int step) {
    scroller_->set_step(horizontal() ? Point(step, 0) : Point(0, step));
}

int Roller_impl::step() const {
    Point s = scroller_->step();
    return horizontal() ? s.x() : s.y();
}

void Roller_impl::roll_to(int pos) {
    Size max = scroller_->logical_size()-scroller_->size();

    if (horizontal()) {
        scroller_->pan_to_x(pos);
        rmax_ = pos >= max.iwidth();
    }

    else {
        scroller_->pan_to_y(pos);
        rmax_ = pos >= max.iheight();
    }
}

void Roller_impl::update_roll() {
    if (rmax_) {
        Size max = scroller_->logical_size()-scroller_->size();
        if (horizontal()) { scroller_->pan_to_x(max.width()); }
        else { scroller_->pan_to_y(max.height()); }
    }
}

void Roller_impl::update_buttons() {
    if (autohide_) {
        Size lsize = scroller_->logical_size(), size = scroller_->size(), max = lsize-size;
        Point offset = scroller_->offset();

        if (horizontal()) {
            if (offset.x() > 0) {
                if (start_ && !start_->hover()) {
                    start_->show();
                }
            }

            else {
                if (start_ && !start_->hover()) {
                    start_->hide();
                }
            }

            if (offset.x() < max.iwidth()) {
                if (end_ && !end_->hover()) {
                    end_->show();
                }
            }

            else {
                if (end_ && !end_->hover()) {
                    end_->hide();
                }
            }
        }

        else {
            if (offset.y() > 0) {
                if (start_ && !start_->hover()) {
                    start_->show();
                }
            }

            else {
                if (start_ && !start_->hover()) {
                    start_->hide();
                }
            }

            if (offset.y() < max.iheight()) {
                if (end_ && !end_->hover()) {
                    end_->show();
                }
            }

            else {
                if (end_ && !end_->hover()) {
                    end_->hide();
                }
            }
        }
    }

    else {
        start_->show();
        end_->show();
    }
}

bool Roller_impl::on_mouse_wheel(int d, unsigned mods, const Point & pt) {
    if (d > 0) {
        if (horizontal()) { scroller_->pan_right_action().exec(); }
        else { scroller_->pan_down_action().exec(); }
    }

    else if (d < 0) {
        if (horizontal()) { scroller_->pan_left_action().exec(); }
        else { scroller_->pan_up_action().exec(); }
    }

    return true;
}

void Roller_impl::set_start(Widget_ptr wp) {
    if (auto bp = std::dynamic_pointer_cast<Button_impl>(wp)) {
        bp->enable_repeat();
        bp->signal_click().connect(fun(this, &Roller_impl::on_start_button_click));
    }

    wp->signal_mouse_leave().connect(fun(this, &Roller_impl::update_buttons));
    remove(start_.get());
    start_ = wp;
    prepend(start_, true);
    if (autohide_) { start_->hide(); }
}

void Roller_impl::set_end(Widget_ptr wp) {
    if (auto bp = std::dynamic_pointer_cast<Button_impl>(wp)) {
        bp->enable_repeat();
        bp->signal_click().connect(fun(this, &Roller_impl::on_end_button_click));
    }

    wp->signal_mouse_leave().connect(fun(this, &Roller_impl::update_buttons));
    remove(end_.get());
    end_ = wp;
    append(end_, true);
    if (autohide_) { end_->hide(); }
}

} // namespace tau

//END
