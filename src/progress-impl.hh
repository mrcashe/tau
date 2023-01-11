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

#ifndef TAU_PROGRESS_IMPL_HH
#define TAU_PROGRESS_IMPL_HH

#include <frame-impl.hh>

namespace tau {

class Progress_impl: public Frame_impl {
public:

    Progress_impl(bool vertical=false);

    Border_style border_style() const {
        return border_left_style();
    }

    Color border_color() const {
        return border_left_color();
    }

    unsigned border() const {
        return border_left();
    }

    void set_text_align(Align align);

    Align text_align() const {
        return text_align_;
    }

    void set_value(double value);

    double value() const {
        return value_;
    }

    void set_min_value(double min_value);

    double min_value() const {
        return min_value_;
    }

    void set_max_value(double max_value);

    double max_value() const {
        return max_value_;
    }

    void set_precision(int prec);

    int precision() const {
        return precision_;
    }

    void set_format(const ustring & fmt);

    ustring format() const {
        return fmt_;
    }

private:

    bool        vertical_;
    double      value_ = 0.0;
    double      min_value_ = 0.0;
    double      max_value_ = 100.0;
    int         precision_ = 0;
    ustring     fmt_ { "%$%%" };
    ustring     msg_;
    Size        text_size_;
    Widget_ptr  area_;
    Align       text_align_ = ALIGN_CENTER;

private:

    void paint_now();
    void redraw(Painter pr);
    void format_str();
    bool on_area_paint(Painter pr, const Rect & inval);
    void on_area_hierarchy_changed();
    void calc_hints();
};

} // namespace tau

#endif // TAU_PROGRESS_IMPL_HH
