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

#ifndef TAU_CYCLE_IMPL_HH
#define TAU_CYCLE_IMPL_HH

#include <tau/action.hh>
#include <tau/input.hh>
#include <frame-impl.hh>

namespace tau {

class Cycle_impl: public Frame_impl {
public:

    Cycle_impl(Border_style bs=BORDER_INSET);

    // Overrides Frame_impl.
    void set_border_style(Border_style bs) override;
    Border_style border_style() const;

    void add(Widget_ptr wp);
    void remove(Widget_ptr wp);
    void clear();
    bool empty() const;
    void append(Widget_ptr wp, bool shrink=false);
    Widget_ptr append(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);
    void prepend(Widget_ptr wp, bool shrink=false);
    Widget_ptr prepend(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);


protected:

    Border_style    user_style_;
    Table_ptr       otable_;
    Table_ptr       itable_;
    Card_ptr        card_;
    Button_ptr      up_;
    Button_ptr      down_;

    Action          up_action_ { KC_UP, KM_NONE };
    Action          down_action_ { KC_DOWN, KM_NONE };
    Accel           escape_accel_ { KC_ESCAPE, KM_NONE, fun(this, &Cycle_impl::on_escape) };

private:

    bool on_escape();
    bool on_mouse_wheel(int delta, int mm, const Point & where);
    bool on_mouse_down(int mbt, int mm, const Point & where);
    void on_focus_in();
    void on_focus_out();
};

} // namespace tau

#endif // TAU_CYCLE_IMPL_HH
