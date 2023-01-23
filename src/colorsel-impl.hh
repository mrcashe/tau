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

#ifndef TAU_COLOR_SELECTOR_IMPL_HH
#define TAU_COLOR_SELECTOR_IMPL_HH

#include <tau/action.hh>
#include <tau/icon.hh>
#include <table-impl.hh>

namespace tau {

class Colorsel_impl: public Table_impl {
public:

    explicit Colorsel_impl(const Color & initial_color);
    void set_color(const Color & color);
    Color color() { return color_; }

    Action & apply_action() { return apply_action_; }
    const Action & apply_action() const { return apply_action_; }
    Action & cancel_action() { return cancel_action_; }
    const Action & cancel_action() const { return cancel_action_; }

    signal<void(const Color &)> & signal_color_changed() { return signal_color_changed_; }

private:

    Color               color_;

    Check_ptr           hue_check_;
    Check_ptr           saturation_check_;
    Check_ptr           value_check_;
    Check_ptr           red_check_;
    Check_ptr           green_check_;
    Check_ptr           blue_check_;

    Counter_ptr         hue_counter_;
    Counter_ptr         saturation_counter_;
    Counter_ptr         value_counter_;
    Counter_ptr         red_counter_;
    Counter_ptr         green_counter_;
    Counter_ptr         blue_counter_;

    std::vector<uint8_t> buffer1_;
    std::vector<uint8_t> buffer2_;

    Pixmap_ptr          pm1_;
    Pixmap_ptr          pm2_;

    Image_ptr           img1_;
    Image_ptr           img2_;

    Widget_ptr          sample_;
    Entry_ptr           html_;
    Table_ptr           ntable_;

    Action              apply_action_ { KC_NONE, KM_NONE, "Apply", ustring(ICON_DIALOG_OK) };
    Action              cancel_action_ { "Escape Cancel", "Cancel", ustring(ICON_DIALOG_CANCEL) };

    signal<void(const Color &)> signal_color_changed_;
    connection          html_cx_;

private:

    void update_all();
    void fill1();
    void fill2();

    void on_img1_size_changed();
    void on_img2_size_changed();
    void on_ntable_size_changed();
    void on_hue_counter_changed(double v);
    void on_saturation_counter_changed(double v);
    void on_value_counter_changed(double v);
    void on_red_counter_changed(double v);
    void on_green_counter_changed(double v);
    void on_blue_counter_changed(double v);
    bool on_named_mouse_down(int mbt, int mm, const Point & pt, Widget_impl * wi);
    bool on_img1_mouse_down(int mbt, int mm, const Point & pt);
    void on_img1_mouse_motion(unsigned shifts, const Point & pt);
    bool on_img1_paint(Painter pr, const Rect & inval);
    void on_img2_mouse_motion(unsigned shifts, const Point & pt);
    bool on_img2_mouse_down(int mbt, int mm, const Point & pt);
    bool on_img2_paint(Painter pr, const Rect & inval);
    void on_html_changed(const ustring & s);
    void on_html_focus_in();
};

} // namespace tau

#endif // TAU_COLOR_SELECTOR_IMPL_HH
