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

#ifndef TAU_ROLLER_IMPL_HH
#define TAU_ROLLER_IMPL_HH

#include <box-impl.hh>

namespace tau {

class Roller_impl: public Box_impl {
public:

    explicit Roller_impl(Orientation orient, bool autohide=true);
   ~Roller_impl() { signal_destroy_(); }

    void set_autohide();
    void unset_autohide();
    bool autohide() const { return autohide_; }
    void insert(Widget_ptr wp);
    void clear();
    void roll_to(int pos);
    void roll_to(Widget_impl * wp);
    int  offset() const;
    void set_step(int step);
    int  step() const;
    void set_start(Widget_ptr wp);
    void set_end(Widget_ptr wp);

protected:

    Widget_ptr   start_;
    Widget_ptr   end_;
    Scroller_ptr scroller_;
    bool         autohide_;

private:

    bool         rmax_ = false; // Roll to maximal.

private:

    void update_buttons();
    void update_roll();
    void on_start_button_click();
    void on_end_button_click();
    bool on_mouse_wheel(int d, unsigned mods, const Point & pt);
};

} // namespace tau

#endif // TAU_ROLLER_IMPL_HH
