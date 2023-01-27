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

#ifndef TAU_COUNTER_IMPL_HH
#define TAU_COUNTER_IMPL_HH

#include <tau/action.hh>
#include <tau/input.hh>
#include <entry-impl.hh>
#include <frame-impl.hh>

namespace tau {

class Counter_impl: public Frame_impl {
public:

    Counter_impl(double value=0.0, double max_value=0.0, double min_value=0.0);
    explicit Counter_impl(Border_style bs, double value=0.0, double max_value=0.0, double min_value=0.0);

    // Overrides Frame_impl.
    void set_border_style(Border_style bs) override;
    Border_style border_style() const;

    void allow_edit();
    void disallow_edit();

    bool edit_allowed() const {
        return edit_allowed_;
    }

    void show_buttons();
    void hide_buttons();
    bool buttons_visible() const;

    void set_value(double value);

    double value() const {
        return value_;
    }

    ustring text() const;
    void set_min_value(double min_value);

    double minimal_value() const {
        return min_value_;
    }

    void set_max_value(double max_value);

    double max_value() const {
        return max_value_;
    }

    void set_step_value(double step_value);

    double step_value() const {
        return step_value_;
    }

    void set_precision(int prec);

    int precision() const {
        return precision_;
    }

    void set_fixed();
    void unset_fixed();

    bool fixed() const {
        return fixed_;
    }

    void set_width(int width);

    int width() const {
        return width_;
    }

    void set_fill(char c);

    char fill() const {
        return fill_;
    }

    void set_caps();
    void unset_caps();

    bool caps() const {
        return caps_;
    }

    void set_base(int base);

    int base() const {
        return base_;
    }

    void increase();
    void increase_page();
    void decrease();
    void decrease_page();

    void append(Widget_ptr wp, bool shrink=false) {
        entry_->append(wp, shrink);
    }

    Widget_ptr append(const ustring & text, unsigned margin_left=0, unsigned margin_right=0) {
        return entry_->append(text, margin_left, margin_right);
    }

    void prepend(Widget_ptr wp, bool shrink=false) {
        entry_->prepend(wp, shrink);
    }

    Widget_ptr prepend(const ustring & text, unsigned margin_left=0, unsigned margin_right=0) {
        return entry_->prepend(text, margin_left, margin_right);
    }

    signal<void(double)> & signal_value_changed() {
        return signal_value_changed_;
    }

private:

    Border_style        user_style_ = BORDER_INSET;
    Table_ptr           table_;
    Entry_ptr           entry_;
    Button_ptr          up_;
    Button_ptr          down_;

    double              value_ = 0.0;
    double              min_value_ = 0.0;
    double              max_value_ = 0.0;
    double              step_value_ = 1.0;
    int                 base_ = 10;
    int                 precision_ = 0;
    int                 width_ = 0;
    char                fill_ = ' ';
    bool                fixed_ = true;
    bool                caps_ = false;
    bool                edit_allowed_ = true;

    Action              up_action_ { KC_UP, KM_NONE };
    Action              down_action_ { KC_DOWN, KM_NONE };
    Action              page_up_action_ { KC_PAGE_UP, KM_NONE };
    Action              page_down_action_ { KC_PAGE_DOWN, KM_NONE };

    signal<void(double)> signal_value_changed_;

private:

    void init(double value, double max_value, double min_value);
    ustring format_value(double v);
    bool update_value(double v, bool force=false);
    void draw_value();
    void drop_entry_focus();
    double get_value();
    void adjust_precision(double example);
    void adjust_width();

    void on_activate(const ustring & s);
    bool on_mouse_wheel(int delta, int mm, const Point & where);
    void on_focus_in();
    void on_focus_out();
    bool on_validate(const tau::ustring & s);
};

} // namespace tau

#endif // TAU_COUNTER_IMPL_HH
