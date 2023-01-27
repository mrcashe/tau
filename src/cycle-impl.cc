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

#include <tau/icon.hh>
#include <card-impl.hh>
#include <button-impl.hh>
#include <cycle-impl.hh>
#include <image-impl.hh>
#include <pixmap-impl.hh>
#include <table-impl.hh>
#include <text-impl.hh>
#include <theme-impl.hh>

#include <iostream>

namespace tau {

Cycle_impl::Cycle_impl(Border_style bs):
    Frame_impl(bs)
{
    user_style_ = border_left_style();

    otable_ = std::make_shared<Table_impl>();
    otable_->set_column_spacing(2);
    otable_->set_row_spacing(1);
    otable_->hint_margin(2);
    insert(otable_);

    itable_ = std::make_shared<Table_impl>();
    itable_->set_column_spacing(1);
    itable_->style().redirect("whitespace/background", "background");
    otable_->put(itable_, 0, 0, 1, 2, false, true);
    otable_->align(itable_.get(), ALIGN_CENTER, ALIGN_FILL);

    card_ = std::make_shared<Card_impl>();
    itable_->put(card_, 0, 0, 1, 1, true, false);
    itable_->align(card_.get(), ALIGN_FILL, ALIGN_CENTER);

    Size buttons_size(12, 0);
    up_ = std::make_shared<Button_impl>();
    up_->enable_repeat();
    up_->hint_size(buttons_size);
    up_->signal_click().connect(fun(card_, &Card_impl::show_next));
    otable_->put(up_, 1, 0, 1, 1, true, false);
    otable_->align(up_.get(), ALIGN_CENTER, ALIGN_START);

    down_ = std::make_shared<Button_impl>();
    down_->enable_repeat();
    down_->hint_size(buttons_size);
    down_->signal_click().connect(fun(card_, &Card_impl::show_previous));
    otable_->put(down_, 1, 1, 1, 1, true, false);
    otable_->align(down_.get(), ALIGN_CENTER, ALIGN_END);

    auto theme = Theme_impl::root();

    auto pixmap = theme->find_pixmap(ICON_PICTO_DEC);
    if (pixmap) { down_->set_image(std::make_shared<Image_impl>(pixmap->dup(), true)); }
    else { down_->hint_size(8, 4); }

    pixmap = theme->find_pixmap(ICON_PICTO_INC);
    if (pixmap) { up_->set_image(std::make_shared<Image_impl>(pixmap->dup(), true)); }
    else { up_->hint_size(8, 4); }

    up_action_.connect(fun(card_, &Card_impl::show_next));
    down_action_.connect(fun(card_, &Card_impl::show_previous));

    connect_action(up_action_);
    connect_action(down_action_);
    connect_accel(escape_accel_);

    signal_mouse_wheel_.connect(fun(this, &Cycle_impl::on_mouse_wheel), true);
    signal_mouse_down_.connect(fun(this, &Cycle_impl::on_mouse_down));
    signal_focus_in_.connect(fun(this, &Cycle_impl::on_focus_in));
    signal_focus_out_.connect(fun(this, &Cycle_impl::on_focus_out));
}

void Cycle_impl::set_border_style(Border_style bs) {
    user_style_ = bs;
    if (BORDER_NONE == bs) { otable_->hint_margin(0); }
    else { otable_->hint_margin(2); }
    Frame_impl::set_border_style(bs);
}

Border_style Cycle_impl::border_style() const {
    return user_style_;
}

bool Cycle_impl::on_mouse_wheel(int delta, int mm, const Point & where) {
    if (delta < 0) { card_->show_next(); }
    else { card_->show_previous(); }
    return true;
}

bool Cycle_impl::on_mouse_down(int mbt, int mm, const Point & where) {
    if (MBT_LEFT == mbt) {
        grab_focus();
        return true;
    }

    return false;
}

bool Cycle_impl::on_escape() {
    if (focused()) {
        drop_focus();
        return true;
    }

    return false;
}

void Cycle_impl::on_focus_in() {
    set_border_color(style().get("select/background").get());
    Frame_impl::set_border_style(BORDER_SOLID);
}

void Cycle_impl::on_focus_out() {
    unset_border_color();
    Frame_impl::set_border_style(user_style_);
}

void Cycle_impl::add(Widget_ptr wp) {
    card_->insert(wp);
}

void Cycle_impl::remove(Widget_ptr wp) {
    card_->remove(wp.get());
    itable_->remove(wp.get());
}

void Cycle_impl::clear() {
    card_->clear();
}

bool Cycle_impl::empty() const {
    return card_->empty();
}

void Cycle_impl::append(Widget_ptr wp, bool shrink) {
    Table::Span rng = itable_->span();
    if (rng.xmax < rng.xmin) { rng.xmax = 0; }
    itable_->put(wp, rng.xmax, 0, 1, 1, shrink, false);
}

Widget_ptr Cycle_impl::append(const ustring & text, unsigned margin_left, unsigned margin_right) {
    Text_ptr tp = std::make_shared<Text_impl>(text);
    tp->hint_margin_left(margin_left);
    tp->hint_margin_right(margin_right);
    append(tp, true);
    return tp;
}

void Cycle_impl::prepend(Widget_ptr wp, bool shrink) {
    Table::Span rng = itable_->span();
    if (rng.xmax < rng.xmin) { rng.xmin = 0; }
    itable_->put(wp, rng.xmin-1, 0, 1, 1, shrink, false);
}

Widget_ptr Cycle_impl::prepend(const ustring & text, unsigned margin_left, unsigned margin_right) {
    Text_ptr tp = std::make_shared<Text_impl>(text);
    tp->hint_margin_left(margin_left);
    tp->hint_margin_right(margin_right);
    prepend(tp, true);
    return tp;
}

} // namespace tau

//END
