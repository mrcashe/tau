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
#include <tau/icon.hh>
#include <tau/input.hh>
#include <tau/painter.hh>
#include <tau/pen.hh>
#include <tau/widget.hh>
#include <button-impl.hh>
#include <icon-impl.hh>
#include <scroller-impl.hh>
#include <slider-impl.hh>
#include <iostream>

namespace tau {

Slider_impl::Slider_impl():
    Box_impl(OR_RIGHT, 2),
    orient_auto_(true),
    autohide_(false)
{
    init();
}

Slider_impl::Slider_impl(Orientation orient, bool autohide):
    Box_impl(orient, 2),
    orient_auto_(false),
    autohide_(autohide)
{
    init();
}

Slider_impl::Slider_impl(Scroller_ptr scroller):
    Box_impl(OR_EAST, 2),
    orient_auto_(true),
    autohide_(false),
    scroller_(scroller)
{
    init();
}

Slider_impl::Slider_impl(Scroller_ptr scroller, Orientation orient, bool autohide):
    Box_impl(orient, 2),
    orient_auto_(false),
    autohide_(autohide),
    scroller_(scroller)
{
    init();
}

void Slider_impl::init() {
    disallow_focus();

    if (scroller_) {
        scroller_->signal_size_changed().connect(fun(this, &Slider_impl::on_scroller_changed));
        scroller_->signal_logical_size_changed().connect(fun(this, &Slider_impl::on_scroller_changed));
        scroller_->signal_pan_changed().connect(fun(this, &Slider_impl::on_scroller_changed));
        signal_visible_.connect(fun(this, &Slider_impl::update_scroller));
    }

    area_ = std::make_shared<Widget_impl>();
    start_ = std::make_shared<Button_impl>();
    end_ = std::make_shared<Button_impl>();

    area_->signal_paint().connect(fun(this, &Slider_impl::on_area_paint));
    area_->signal_mouse_down().connect(fun(this, &Slider_impl::on_area_mouse_down));
    area_->signal_mouse_up().connect(fun(this, &Slider_impl::on_area_mouse_up));
    area_->signal_mouse_motion().connect(fun(this, &Slider_impl::on_area_mouse_motion));
    area_->signal_mouse_wheel().connect(fun(this, &Slider_impl::on_area_mouse_wheel));
    area_->signal_mouse_enter().connect(fun(this, &Slider_impl::on_area_mouse_enter));
    area_->signal_mouse_leave().connect(fun(this, &Slider_impl::on_area_mouse_leave));

    start_->signal_click().connect(fun(this, &Slider_impl::on_start_click));
    end_->signal_click().connect(fun(this, &Slider_impl::on_end_click));
    style().get("slider/foreground").signal_changed().connect(fun(this, &Slider_impl::paint_now));
    area_->style().redirect("slider/background", "background");
    signal_size_changed().connect(fun(this, &Slider_impl::on_size_changed));
    signal_orientation_changed().connect(fun(this, &Slider_impl::update_arrows));

    start_->hide_relief();
    start_->enable_repeat();
    end_->hide_relief();
    end_->enable_repeat();
    append(area_, false);
    append(end_, true);
    prepend(start_, true);
    area_->hint_min_size(12);
    update_arrows();
}

void Slider_impl::update_arrows() {
    start_->set_image(std::make_shared<Icon_impl>(horizontal() ? ICON_PICTO_LEFT : ICON_PICTO_UP, 12));
    end_->set_image(std::make_shared<Icon_impl>(horizontal() ? ICON_PICTO_RIGHT : ICON_PICTO_DOWN, 12));
}

double Slider_impl::location() const {
    return loc_;
}

double Slider_impl::range() const {
    return range_;
}

void Slider_impl::set_location(double loc) {
    if (loc_ != loc) {
        loc_ = loc;
        signal_location_changed_(loc);
        paint_now();

        if (scroller_) {
            Size lsize = scroller_->logical_size(), size = scroller_->size();

            if (horizontal()) {
                double rng = lsize ? double(size.width())/lsize.width() : 1.0;
                double d = lsize.width()*std::max(0.0, std::min(loc_, 1.0-rng));
                scroller_->pan_to_x(d);
            }

            else {
                double rng = lsize ? double(size.height())/lsize.height() : 1.0;
                double d = lsize.height()*std::max(0.0, std::min(loc_, 1.0-rng));
                scroller_->pan_to_y(d);
            }
        }
    }
}

void Slider_impl::set_range(double range) {
    if (range_ != range) {
        range_ = range;
        paint_now();
    }
}

void Slider_impl::set(double loc, double range) {
    bool changed = false;

    if (range_ != range) {
        changed = true;
        range_ = range;
    }

    if (loc_ != loc) {
        loc_ = loc;
        changed = true;
        signal_location_changed_(loc);
    }

    if (changed) { paint_now(); }
}

// Do one step from mouse click on arrows up/left or down/right.
void Slider_impl::do_step(bool increase) {
    if (enabled()) {
        double loc;

        if (increase) {
            loc = std::max(0.0, std::min(1.0, loc_+step_));
        }

        else {
            loc = std::max(0.0, std::min(1.0, loc_-step_));
        }

        if (loc != loc_) {
            set_location(loc);
        }
    }
}

void Slider_impl::on_start_click() {
    do_step(false);
}

void Slider_impl::on_end_click() {
    do_step(true);
}

bool Slider_impl::mouse_on_slider(const Point & pt) const {
    if (area_->hover() && area_->size()) {
        double l = horizontal() ? pt.x()/double(area_->size().width()) : pt.y()/double(area_->size().height());
        return l >= loc_ && l < loc_+range_;
    }

    return false;
}

void Slider_impl::on_area_mouse_leave() {
    area_->unset_cursor();
    bool m = false;

    if (mouse_on_slider_) {
        mouse_on_slider_ = false;
        m = true;
    }

    if (press_) {
        press_ = false;
        m = true;
    }

    if (m) {
        paint_now();
    }
}

void Slider_impl::on_area_mouse_enter(const Point & pt) {
    if (mouse_on_slider(pt)) {
        if (!mouse_on_slider_) {
            mouse_on_slider_ = true;
            paint_now();
        }
    }
}

bool Slider_impl::on_area_mouse_up(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        if (area_->grabs_mouse()) {
            area_->ungrab_mouse();
            area_->unset_cursor();

            if (press_) {
                press_ = false;
                paint_now();
            }
        }

        return true;
    }

    return false;
}

bool Slider_impl::on_area_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        // Click onto slider rectangle.
        // Start drag the slider.
        // Simply fix mouse position here.
        if (mouse_on_slider(pt)) {
            if (!press_) {
                press_ = true;
                area_->grab_mouse();
                area_->set_cursor(horizontal() ? "size_hor" : "size_ver");
                mloc_ = horizontal() ? pt.x() : pt.y();
                paint_now();
            }
        }

        // The click point is outside of slider rectangle.
        // Move slider to the click position.
        else {
            if (area_->size()) {
                double l;

                if (horizontal()) {
                    l = pt.x()/double(area_->size().width());
                }

                else {
                    l = pt.y()/double(area_->size().height());
                }

                if (l < range_) { l = 0.0; }
                else if (l > 1.0-range_) { l = 1.0-range_; }
                else { l -= range_/2; }
                set_location(l);
            }
        }

        return true;
    }

    return false;
}

void Slider_impl::on_area_mouse_motion(int mm, const Point & pt) {
    if (area_->grabs_mouse()) {
        if (area_->size()) {
            if (horizontal()) {
                double d = pt.x()-mloc_;
                mloc_ = pt.x();
                set_location(std::max(0.0, std::min(1.0-range_, loc_+d/area_->size().width())));
            }

            else {
                double d = pt.y()-mloc_;
                mloc_ = pt.y();
                set_location(std::max(0.0, std::min(1.0-range_, loc_+d/area_->size().height())));
            }
        }
    }

    else {
        if (mouse_on_slider(pt)) {
            if (!mouse_on_slider_) {
                mouse_on_slider_ = true;
                paint_now();
            }
        }

        else {
            if (mouse_on_slider_) {
                mouse_on_slider_ = false;
                paint_now();
            }
        }

        if (press_) {
            press_ = false;
            paint_now();
        }
    }
}

bool Slider_impl::on_area_mouse_wheel(int delta, int mm, const Point & where) {
    if (delta < 0) {
        set_location(std::max(0.0, std::min(1.0-range_, loc_-step_)));
    }

    else if (delta > 0) {
        set_location(std::max(0.0, std::min(1.0-range_, loc_+step_)));
    }

    return true;
}

// Here is painter from area_ child.
void Slider_impl::draw_slider(Painter pr, bool erase_bkgnd) {
    if (Size sz = area_->size()) {
        int x, y, w, h;

        if (erase_bkgnd) {
            Color c = area_->style().color("background");
            Rect va = area_->visible_area();
            pr.rectangle(va.left(), va.top(), va.right(), va.bottom());
            pr.set_brush(Brush(c));
            pr.fill();
        }

        if (horizontal()) {
            int rng = range_*sz.width();
            int xmax = sz.width()-rng;
            int x0 = loc_*sz.width();
            x = std::min(x0, xmax);
            y = 0;
            w = std::max(5, rng);
            h = sz.height()-1;
        }

        else {
            int rng = range_*sz.height();
            int ymax = sz.height()-rng;
            int y0 = loc_*sz.height();
            x = 0;
            y = std::min(y0, ymax);
            w = sz.width()-1;
            h = std::max(5, rng);
        }

        if (w > 0 && h > 0) {
            Color c = area_->style().get("slider/foreground").get();
            if (press_) { c.darker(0.1); }
            else if (mouse_on_slider_) { c.lighter(0.1); }

            pr.rectangle(x, y, x+w-1, y+h-1);
            pr.set_brush(Brush(c));
            pr.fill_preserve();

            c.darker(0.15);
            pr.set_pen(Pen(c));
            pr.stroke();
        }
    }
}

void Slider_impl::paint_now() {
    if (visible()) {
        if (Painter pr = area_->painter()) {
            draw_slider(pr, true);
        }
    }
}

bool Slider_impl::on_area_paint(Painter pr, const Rect & inval) {
    draw_slider(pr, true);
    return false;
}

void Slider_impl::on_size_changed() {
    if (!hidden() && orient_auto_) {
        set_orientation(size().width() > size().height() ? OR_RIGHT : OR_DOWN);
    }
}

void Slider_impl::show_buttons() {
    start_->show();
    end_->show();
}

void Slider_impl::hide_buttons() {
    start_->hide();
    end_->hide();
}

bool Slider_impl::buttons_visible() const {
    return !start_->hidden();
}

void Slider_impl::update_scroller() {
    if (scroller_) {
        Size lsize = scroller_->logical_size(), size = scroller_->size(), max(lsize-size);
        range_ = lsize ? (horizontal() ? double(size.width())/lsize.width() : double(size.height())/lsize.height()) : 1.0;

        Point ofs = scroller_->pan();
        loc_ = lsize ? (horizontal() ? double(ofs.x())/lsize.iwidth() : double(ofs.y())/lsize.iheight()) : 0.0;
        paint_now();

        if (horizontal()) {
            if (ofs.x() > 0) { start_->enable(); }
            else { start_->disable(); }
            if (ofs.x() >= max.iwidth()) { end_->disable(); }
            else { end_->enable(); }
        }

        else {
            if (ofs.y() > 0) { start_->enable(); }
            else { start_->disable(); }
            if (ofs.y() >= max.iheight()) { end_->disable(); }
            else { end_->enable(); }
        }
    }
}

void Slider_impl::on_scroller_changed() {
    if (scroller_) {
        Size lsize = scroller_->logical_size(), size = scroller_->size(), max(lsize-size);

        if (hidden()) {
            if (orient_auto_ && max.max()) {
                appear();
                return;
            }
        }

        if (horizontal()) {
            if (autohide_ && 0 == max.width()) { disappear(); return; }
            else { appear(); }
        }

        else {
            if (autohide_ && 0 == max.height()) { disappear(); return; }
            else { appear(); }
        }

        update_scroller();
    }
}

} // namespace tau

//END
