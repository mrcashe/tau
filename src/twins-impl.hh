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

#ifndef TAU_TWINS_IMPL_HH
#define TAU_TWINS_IMPL_HH

#include <container-impl.hh>

namespace tau {

class Twins_impl: public Container_impl {
public:

    explicit Twins_impl(Orientation orient, double ratio=0.5);
   ~Twins_impl() { destroy_ = true; }

    void insert_first(Widget_ptr wp);
    void insert_second(Widget_ptr wp);
    bool has_first() const { return nullptr != first_; }
    bool has_second() const { return nullptr != second_; }
    bool empty() const { return nullptr == first_ && nullptr == second_; }
    void remove_first();
    void remove_second();
    void clear();
    void set_ratio(double ratio);
    double ratio() const { return ratio_; }

    signal<void(double)> & signal_ratio_changed() { return signal_ratio_changed_; }

private:

    Size child_requisition(Widget_impl * wi);
    void update_requisition();
    void rm_first();
    void rm_second();
    void arrange();

    bool on_sep_mouse_down(int mbt, int mm, const Point & pt);
    bool on_sep_mouse_up(int mbt, int mm, const Point & pt);
    void on_sep_mouse_motion(int mm, const Point & pt);
    void on_child_hide(Widget_impl * wi);
    void on_child_show(Widget_impl * wi);
    bool on_take_focus();

private:

    Separator_impl *     sep_ = nullptr;
    Widget_impl *        first_ = nullptr;
    Widget_impl *        second_ = nullptr;
    double               ratio_;
    Orientation          orient_;

    connection           sep_mouse_motion_cx_;

    connection           first_hints_cx_;
    connection           first_req_cx_;
    connection           first_show_cx_;
    connection           first_hide_cx_;

    connection           second_hints_cx_;
    connection           second_req_cx_;
    connection           second_show_cx_;
    connection           second_hide_cx_;

    signal<void(double)> signal_ratio_changed_;
};

} // namespace tau

#endif // TAU_TWINS_IMPL_HH
