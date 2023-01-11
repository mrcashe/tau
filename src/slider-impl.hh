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

#include <box-impl.hh>

namespace tau {

class Slider_impl: public Box_impl {
public:

    Slider_impl();
    explicit Slider_impl(Orientation orient, bool autohide=false);
    explicit Slider_impl(Scroller_ptr scroller);
    Slider_impl(Scroller_ptr scroller, Orientation orient, bool autohide=false);

    double location() const;
    void set_location(double loc);
    void set_range(double rng);
    double range() const;
    void set(double loc, double rng);

    void show_buttons();
    void hide_buttons();
    bool buttons_visible() const;

    signal<void(double)> & signal_location_changed() {
        return signal_location_changed_;
    }

private:
    
    enum { HOLD_NONE, HOLD_INCREASE, HOLD_DECREASE };

    Widget_ptr          area_;
    double              loc_ = 0.0;
    double              range_ = 1.0;
    double              step_ = 0.01;
    int                 mloc_;                  // Last mouse location (X or Y).
    bool                press_ = false;
    bool                mouse_on_slider_ = false;
    bool                orient_auto_;
    bool                autohide_;
    Button_ptr          start_;
    Button_ptr          end_;
    Scroller_ptr        scroller_;

    signal<void(double)> signal_location_changed_;

private:

    void init();
    void do_step(bool increase);
    bool mouse_on_slider(const Point & pt) const;
    void draw_slider(Painter pr, bool erase_bkgnd);
    void paint_now();
    void update_arrows();
    void update_scroller();

    void on_size_changed();
    void on_start_click();
    void on_end_click();
    void on_area_mouse_leave();
    void on_area_mouse_enter(const Point & pt);
    bool on_area_mouse_down(int mbt, int mm, const Point & pt);
    bool on_area_mouse_up(int mbt, int mm, const Point & pt);
    void on_area_mouse_motion(int mm, const Point & pt);
    bool on_area_mouse_wheel(int delta, int mm, const Point & where);
    bool on_area_paint(Painter pr, const Rect & inval);
    void on_scroller_changed();
};

} // namespace tau

//END
