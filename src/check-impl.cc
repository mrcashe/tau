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
#include <tau/matrix.hh>
#include <tau/painter.hh>
#include <tau/pen.hh>
#include <check-impl.hh>

namespace tau {

Check_impl::Check_impl(bool checked):
    Frame_impl(),
    checked_(checked),
    check_style_(CHECK_VSTYLE),
    user_border_style_(BORDER_INSET)
{
    init();
}

Check_impl::Check_impl(Check_style cs, bool checked):
    Frame_impl(),
    checked_(checked),
    check_style_(cs),
    user_border_style_(BORDER_INSET)
{
    init();
}

Check_impl::Check_impl(Border_style bs, bool checked):
    Frame_impl(),
    checked_(checked),
    check_style_(CHECK_VSTYLE),
    user_border_style_(bs)
{
    init();
}

Check_impl::Check_impl(Check_style cs, Border_style bs, bool checked):
    Frame_impl(),
    checked_(checked),
    check_style_(cs),
    user_border_style_(bs)
{
    init();
}

void Check_impl::init() {
    auto wp = std::make_shared<Widget_impl>();
    insert(wp);
    radio_signal_ = std::make_shared<Radio_signal>();

    if (CHECK_XSTYLE == check_style_ || CHECK_VSTYLE == check_style_) {
        set_border_style(user_border_style_);
        set_border(user_border_width_);
    }

    radio_cx_ = radio_signal_->connect(fun(this, &Check_impl::on_radio_signal));
    connect_action(toggle_action_);
    connect_action(cancel_action_);

    wp->signal_paint().connect(fun(this, &Check_impl::on_paint));
    wp->signal_display().connect(fun(this, &Check_impl::on_display));
    wp->signal_mouse_down().connect(fun(this, &Check_impl::on_mouse_down));
    signal_focus_in_.connect(fun(this, &Check_impl::redraw));
    signal_focus_out_.connect(fun(this, &Check_impl::redraw));
}

void Check_impl::set_check_style(Check_style cs) {
    if (check_style_ != cs) {
        check_style_ = cs;

        if (CHECK_XSTYLE == cs || CHECK_VSTYLE == cs) {
            set_border_style(user_border_style_);
            set_border(user_border_width_);
        }

        else {
            set_border_style(BORDER_NONE);
        }

        redraw();
    }
}

void Check_impl::check() {
    if (!checked_) {
        checked_ = true;
        radio_cx_.block();
        radio_signal_->operator()();
        radio_cx_.unblock();
        redraw();
        signal_check_();
    }
}

void Check_impl::uncheck() {
    if (checked_ && !joined()) {
        checked_ = false;
        redraw();
        signal_uncheck_();
    }
}

void Check_impl::join(Check_ptr other) {
    if (this != other.get()) {
        radio_signal_ = other->radio_signal_ptr();
        if (checked_) { radio_signal_->operator()(); }
        radio_cx_ = radio_signal_->connect(fun(this, &Check_impl::on_radio_signal));
    }
}

bool Check_impl::joined() const {
    return radio_signal_->size() > 1;
}

void Check_impl::set_border_width(unsigned npx) {
    user_border_width_ = npx;
    set_border(npx);
}

void Check_impl::toggle() {
    if (checked() && !joined()) {
        uncheck();
    }

    else {
        check();
    }
}

void Check_impl::on_cancel() {
    if (has_focus()) {
        drop_focus();
    }
}

bool Check_impl::on_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        grab_focus();
        toggle();
        return true;
    }

    return false;
}

void Check_impl::on_display() {
    if (Painter pr = cp_->painter()) {
        Size rs;

        if (Font font = pr.select_font(style().font("font").spec())) {
            unsigned s = std::ceil(font.ascent()+std::fabs(font.descent()));
            rs.update_max_height(s);
            rs.update_max_width(s);
            cp_->hint_min_size(rs);
        }
    }
}

bool Check_impl::on_paint(Painter pr, const Rect & inval) {
    draw_check(pr);
    return true;
}

void Check_impl::redraw() {
    if (Painter pr = cp_->painter()) { draw_check(pr); }
    else { invalidate(); }
}

void Check_impl::draw_check(Painter pr) {
    Rect r(cp_->size());

    Color b = style().color("background").get().lighten(0.1);
    Color f = enabled() ? style().color("foreground").get() : b.inactive();

    if (CHECK_RSTYLE == check_style_) {
        pr.set_brush(style().color("background").get());
        pr.paint();
        unsigned s = r.size().min();
        Point rc = r.center();
        pr.matrix().translate(rc.x(), rc.y());
        pr.matrix().scale(s/2);

        pr.circle(0.0, 0.0, 1.0);
        pr.set_brush(f);
        pr.fill();

        pr.circle(0.0, 0.0, 0.9);
        pr.set_brush(b);
        pr.fill();

        if (checked_) {
            pr.circle(0.0, 0.0, 0.4);
            pr.set_brush(f);
            pr.fill();
        }
    }

    else if (CHECK_XSTYLE == check_style_) {
        pr.set_brush(b);
        pr.paint();
        unsigned s = r.size().min();
        if (!(1 & s)) { r.decrease(1); r.translate(1, 1); --s; }

        if (r && checked_) {
            Point rc = r.center();
            pr.matrix().translate(rc.x(), rc.y());
            pr.matrix().scale(s/2);
            pr.move_to(-0.8, -0.8);
            pr.line_to(0.8, 0.8);
            pr.move_to(0.8, -0.8);
            pr.line_to(-0.8, 0.8);
            pr.set_pen(Pen(f, 2));
            pr.stroke();
        }
    }

    // Assume CHECK_VSTYLE.
    else {
        pr.set_brush(b);
        pr.paint();
        unsigned s = r.size().min();
        if (!(1 & s)) { r.decrease(1); r.translate(1, 1); --s; }

        if (r && checked_) {
            Point rc = r.center();
            pr.matrix().translate(rc.x(), rc.y());
            pr.matrix().scale(s/2);
            pr.move_to(-0.8, 0);
            pr.line_to(-0.2, 0.8);
            pr.line_to(0.8, -0.8);
            pr.set_pen(Pen(f, 2));
            pr.stroke();
        }
    }
}

void Check_impl::on_radio_signal() {
    if (checked_) {
        checked_ = false;
        redraw();
        signal_uncheck_();
    }
}

} // namespace tau

//END
