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

#include <tau/string.hh>
#include <button-impl.hh>
#include <counter-impl.hh>
#include <image-impl.hh>
#include <pixmap-impl.hh>
#include <table-impl.hh>
#include <theme-impl.hh>
#include <iomanip>
#include <iostream>

namespace {

const int MAX_PRECISION = 12;

} // anonymous namespace

namespace tau {

Counter_impl::Counter_impl(double value, double max_value, double min_value):
    Frame_impl(BORDER_INSET)
{
    init(value, max_value, min_value);
}

Counter_impl::Counter_impl(Border_style bs, double value, double max_value, double min_value):
    Frame_impl(bs)
{
    init(value, max_value, min_value);
}

void Counter_impl::init(double value, double max_value, double min_value) {
    table_ = std::make_shared<Table_impl>();
    table_->set_column_spacing(1);
    table_->hint_margin(1);
    insert(table_);

    entry_ = std::make_shared<Entry_impl>(ALIGN_CENTER, BORDER_NONE);
    table_->put(entry_, 0, 0, 1, 2, false, false);
    table_->align(entry_.get(), ALIGN_FILL, ALIGN_CENTER);
    entry_->signal_mouse_wheel().connect(fun(this, &Counter_impl::on_mouse_wheel), true);
    entry_->signal_validate().connect(fun(this, &Counter_impl::on_validate));
    entry_->signal_activate().connect(fun(this, &Counter_impl::on_activate));
    entry_->cancel_action().connect(fun(this, &Counter_impl::draw_value));
    entry_->cancel_action().connect(fun(this, &Counter_impl::drop_focus));

    up_ = std::make_shared<Button_impl>();
    up_->enable_repeat();
    up_->hint_size(12, 0);
    up_->signal_click().connect(fun(this, &Counter_impl::drop_entry_focus));
    up_->signal_click().connect(fun(this, &Counter_impl::increase));
    table_->put(up_, 1, 0, 1, 1, true);
    table_->align(up_.get(), ALIGN_CENTER, ALIGN_START);

    down_ = std::make_shared<Button_impl>();
    down_->enable_repeat();
    down_->hint_size(12, 0);
    down_->signal_click().connect(fun(this, &Counter_impl::drop_entry_focus));
    down_->signal_click().connect(fun(this, &Counter_impl::decrease));
    table_->put(down_, 1, 1, 1, 1, true);
    table_->align(down_.get(), ALIGN_CENTER, ALIGN_END);

    auto theme = Theme_impl::root();

    Pixmap_cptr pixmap = theme->find_pixmap("picto-dec");
    if (pixmap) { down_->set_image(std::make_shared<Image_impl>(pixmap->dup(), true)); }
    else { down_->hint_size(8, 4); }

    pixmap = theme->find_pixmap("picto-inc");
    if (pixmap) { up_->set_image(std::make_shared<Image_impl>(pixmap->dup(), true)); }
    else { up_->hint_size(8, 4); }

    set_max_value(max_value);
    set_min_value(min_value);
    adjust_precision(value);
    update_value(value, true);

    up_action_.connect(fun(entry_, &Entry_impl::unselect));
    up_action_.connect(fun(this, &Counter_impl::increase));
    down_action_.connect(fun(entry_, &Entry_impl::unselect));
    down_action_.connect(fun(this, &Counter_impl::decrease));
    page_up_action_.connect(fun(entry_, &Entry_impl::unselect));
    page_up_action_.connect(fun(this, &Counter_impl::increase_page));
    page_down_action_.connect(fun(entry_, &Entry_impl::unselect));
    page_down_action_.connect(fun(this, &Counter_impl::decrease_page));

    connect_action(up_action_);
    connect_action(down_action_);
    connect_action(page_up_action_);
    connect_action(page_down_action_);

    signal_focus_in_.connect(fun(this, &Counter_impl::on_focus_in));
    signal_focus_out_.connect(fun(this, &Counter_impl::on_focus_out));
    signal_take_focus_.connect(fun(entry_, &Widget_impl::take_focus), true);
    signal_display_.connect(fun(this, &Counter_impl::adjust_width));
}

void Counter_impl::set_border_style(Border_style style) {
    user_style_ = style;
    table_->hint_margin(BORDER_NONE == user_style_ ? 0 : 1);
    Frame_impl::set_border_style(user_style_);
}

Border_style Counter_impl::border_style() const {
    return user_style_;
}

ustring Counter_impl::text() const {
    return entry_->text();
}

void Counter_impl::allow_edit() {
    if (!edit_allowed_) {
        edit_allowed_ = true;
        entry_->allow_edit();
        down_->enable();
        up_->enable();
        page_up_action_.enable();
        up_action_.enable();
        page_down_action_.enable();
        down_action_.enable();
    }
}

void Counter_impl::disallow_edit() {
    if (edit_allowed_) {
        drop_entry_focus();
        update_value(value_);
        edit_allowed_ = false;
        entry_->disallow_edit();
        down_->disable();
        up_->disable();
        page_up_action_.disable();
        up_action_.disable();
        page_down_action_.disable();
        down_action_.disable();
    }
}

void Counter_impl::show_buttons() {
    up_->show();
    down_->show();
}

void Counter_impl::hide_buttons() {
    up_->hide();
    down_->hide();
}

bool Counter_impl::buttons_visible() const {
    return !(up_->hidden() || down_->hidden());
}

void Counter_impl::draw_value() {
    ustring s = format_value(value_);
    entry_->assign(s);
}

bool Counter_impl::update_value(double value, bool force) {
    if (value < min_value_) {
        value = min_value_;
    }

    else if (max_value_ > min_value_ && value >= max_value_) {
        value = max_value_;
    }

    if (force || (value_ != value)) {
        value_ = value;
        draw_value();
        return true;
    }

    return false;
}

void Counter_impl::set_value(double value) {
    update_value(value);
}

void Counter_impl::set_min_value(double min_value) {
    if (min_value_ != min_value) {
        min_value_ = min_value;

        if (value_ < min_value_) {
            if (update_value(min_value_)) {
                signal_value_changed_(value_);
            }
        }
    }
}

void Counter_impl::set_max_value(double max_value) {
    if (max_value_ != max_value) {
        max_value_ = max_value;

        if (value_ > max_value_) {
            if (update_value(max_value_)) {
                signal_value_changed_(value_);
            }
        }
    }
}

void Counter_impl::adjust_precision(double example) {
    double fract = std::fabs(example)-std::floor(std::fabs(example));
    ustring s = str_format(fract);
    ustring::size_type dot_pos = s.find('.');
    int prec = ustring::npos != dot_pos ? s.size()-dot_pos-1 : 0;

    if (prec > precision_) {
        precision_ = std::min(prec, MAX_PRECISION);
        draw_value();
    }

    adjust_width();
}

void Counter_impl::set_step_value(double step_value) {
    if (step_value > 0) {
        if (step_value_ != step_value) {
            if (max_value_ > min_value_) { step_value_ = std::min(step_value, max_value_-min_value_); }
            else { step_value_ = step_value; }
            adjust_precision(step_value_);
        }
    }
}

void Counter_impl::set_precision(int prec) {
    if (precision_ != prec) {
        precision_ = std::min(prec, MAX_PRECISION);
        adjust_width();
        draw_value();
    }
}

void Counter_impl::set_fixed() {
    if (!fixed_) {
        fixed_ = true;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::unset_fixed() {
    if (fixed_) {
        fixed_ = false;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::set_width(int width) {
    int wabs = abs(width);

    if (width_ != wabs) {
        width_ = wabs;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::set_fill(char c) {
    if (fill_ != c) {
        fill_ = c;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::set_caps() {
    if (!caps_) {
        caps_ = true;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::unset_caps() {
    if (caps_) {
        caps_ = false;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::set_base(int base) {
    int corrected;

    switch (base) {
        case 2: corrected = 2; break;
        case 8: corrected = 8; break;
        case 16: corrected = 16; break;
        default: corrected = 10;
    }

    if (base_ != corrected) {
        base_ = corrected;
        adjust_width();
        draw_value();
    }
}

void Counter_impl::increase() {
    if (enabled()) {
        if (update_value(value_+step_value_)) {
            signal_value_changed_(value_);
        }
    }
}

void Counter_impl::increase_page() {
    if (enabled()) {
        if (update_value(value_+base_*step_value_)) {
            signal_value_changed_(value_);
        }
    }
}

void Counter_impl::decrease() {
    if (enabled()) {
        if (update_value(value_-step_value_)) {
            signal_value_changed_(value_);
        }
    }
}

void Counter_impl::decrease_page() {
    if (enabled()) {
        if (update_value(value_-base_*step_value_)) {
            signal_value_changed_(value_);
        }
    }
}

bool Counter_impl::on_mouse_wheel(int delta, int mm, const Point & where) {
    if (edit_allowed_) {
        drop_entry_focus();

        if (delta > 0) {
            if (MM_CONTROL & mm) { decrease_page(); }
            else { decrease(); }
        }

        else {
            if (MM_CONTROL & mm) { increase_page(); }
            else { increase(); }
        }

        return true;
    }

    return false;
}

void Counter_impl::drop_entry_focus() {
    entry_->unselect();

    if (entry_->focused()) {
        entry_->move_to(0, 0);
        grab_focus();
    }
}

void Counter_impl::on_activate(const ustring & s) {
    if (update_value(get_value())) { signal_value_changed_(value_); }
    else { draw_value(); }
    drop_focus();
}

ustring Counter_impl::format_value(double v) {
    ustring s;

    if (16 == base_) {
        uintmax_t iv = v;
        s = str_format(std::hex, std::setfill(fill_), std::setw(width_), iv);
    }

    else if (8 == base_) {
        uintmax_t iv = v;
        s = str_format(std::oct, std::setfill(fill_), std::setw(width_), iv);
    }

    else if (2 == base_) {
        uintmax_t iv = v;

        while (0 != iv) {
            s.insert(0, 1, (1 & iv ? '1' : '0'));
            iv >>= 1;
        }

        while (int(s.size()) < width_) { s.insert(0, 1, fill_); }
        if (s.empty()) { s += '0'; }
    }

    else {
        s = fixed_ ? str_format(std::setw(width_), std::setfill(fill_), std::setprecision(precision_), std::fixed, v):
        str_format(std::setw(width_), std::setfill(fill_), std::setprecision(precision_), v);
    }

    return caps_ ? str_toupper(s) : s;
}

double Counter_impl::get_value() {
    ustring s = entry_->text();

    if (16 == base_) {
        uintmax_t val = 0;

        for (char32_t wc: s) {
            val <<= 4;
            if (wc < 0x0030) { return val; }
            if (wc < 0x003A) { val |= wc-0x0030; continue; }
            if (wc < 0x0041) { return val; }
            if (wc < 0x0047) { val |= wc-0x0037; continue; }
            if (wc < 0x0061) { return val; }
            if (wc < 0x0067) { val |= wc-0x0057; continue; }
            return val;
        }

        return val;
    }

    if (8 == base_) {
        uintmax_t val = 0;

        for (char32_t wc: s) {
            val <<= 3;
            if (wc < 0x0030) { return val; }
            if (wc < 0x0038) { val |= wc-0x0030; continue; }
            return val;
        }

        return val;
    }

    if (2 == base_) {
        uintmax_t val = 0;

        for (char32_t wc: s) {
            val <<= 1;
            if (wc < 0x0030) { return val; }
            if (wc < 0x0032) { val |= wc-0x0030; continue; }
            return val;
        }

        return val;
    }

    double d = 0;
    try { d = std::stod(s); } catch (...) {}
    return d;
}

void Counter_impl::adjust_width() {
    ustring smin = format_value(min_value_);
    ustring smax = format_value(max_value_);
    unsigned w = std::max(entry_->text_size(smin).width(), entry_->text_size(smax).width());
    entry_->reserve(w+8);
    entry_->limit(2*w+8);
}

bool Counter_impl::on_validate(const ustring & s) {
    if (16 == base_) {
        for (char32_t wc: s) {
            if (wc < 0x0030) { return true; }
            if (wc < 0x003A) { continue; }
            if (wc < 0x0041) { return true; }
            if (wc < 0x0047) { continue; }
            if (wc < 0x0061) { return true; }
            if (wc < 0x0067) { continue; }
            return true;
        }

        return false;
    }

    if (8 == base_) {
        for (char32_t wc: s) {
            if (wc < 0x0030) { return true; }
            if (wc < 0x0038) { continue; }
            return true;
        }

        return false;
    }

    if (2 == base_) {
        for (char32_t wc: s) {
            if (wc < 0x0030) { return true; }
            if (wc < 0x0032) { continue; }
            return true;
        }

        return false;
    }

    for (char32_t wc: s) {
        if (ustring::npos == ustring("0123456789+-eE.,").find(wc)) {
            return true;
        }
    }

    return false;
}

void Counter_impl::on_focus_in() {
    if (BORDER_NONE != user_style_) {
        set_border_color(style_.color("select/background"));
        Frame_impl::set_border_style(BORDER_SOLID);
    }
}

void Counter_impl::on_focus_out() {
    if (BORDER_NONE != user_style_) {
        unset_border_color();
        Frame_impl::set_border_style(user_style_);
        draw_value();
    }
}

} // namespace tau

//END
