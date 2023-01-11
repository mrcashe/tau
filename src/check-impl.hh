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

#ifndef TAU_CHECK_IMPL_HH
#define TAU_CHECK_IMPL_HH

#include <tau/action.hh>
#include <tau/enums.hh>
#include <frame-impl.hh>

namespace tau {

class Check_impl: public Frame_impl {
public:

    using Radio_signal = signal<void()>;
    using Radio_signal_ptr = std::shared_ptr<Radio_signal>;

    Check_impl(bool checked=false);
    Check_impl(Check_style cs, bool checked=false);
    Check_impl(Border_style bs, bool checked=false);
    Check_impl(Check_style cs, Border_style bs, bool checked=false);

    void set_check_style(Check_style cs);

    Check_style check_style() const {
        return check_style_;
    }

    void check();
    void uncheck();
    void toggle();
    void join(Check_ptr other);
    bool joined() const;

    bool checked() const {
        return checked_;
    }

    Radio_signal_ptr radio_signal_ptr() {
        return radio_signal_;
    }

    // Overrides Frame_impl.
    void set_border_style(Border_style bs) override {
        user_border_style_ = bs;
        Frame_impl::set_border_style(bs);
    }

    Border_style border_style() const {
        return user_border_style_;
    }

    void set_border_width(unsigned npx);

    unsigned border_width() const {
        return user_border_width_;
    }

    signal<void()> & signal_check() {
        return signal_check_;
    }

    signal<void()> & signal_uncheck() {
        return signal_uncheck_;
    }

private:

    bool                checked_;
    Check_style         check_style_;
    Action              toggle_action_ { "Space Enter", fun(this, &Check_impl::toggle) };
    Action              cancel_action_ { "Escape Cancel", fun(this, &Check_impl::on_cancel) };
    Border_style        user_border_style_;
    unsigned            user_border_width_ = 1;
    Radio_signal_ptr    radio_signal_;
    connection          radio_cx_;

    signal<void()>      signal_check_;
    signal<void()>      signal_uncheck_;

private:

    void init();
    void draw_check(Painter pr);
    void redraw();

    void on_display();
    void on_cancel();
    bool on_paint(Painter pr, const Rect & inval);
    bool on_mouse_down(int mbt, int mm, const Point & pt);
    void on_radio_signal();
};

} // namespace tau

#endif // TAU_CHECK_IMPL_HH
