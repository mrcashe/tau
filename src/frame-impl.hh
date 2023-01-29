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

#ifndef TAU_BORDER_IMPL_HH
#define TAU_BORDER_IMPL_HH

#include <tau/enums.hh>
#include <tau/color.hh>
#include <container-impl.hh>

namespace tau {

class Frame_impl: public Container_impl {
public:

    Frame_impl();
   ~Frame_impl() { signal_destroy_(); }

    explicit Frame_impl(const ustring & label);
    Frame_impl(const ustring & label, Align align);

    explicit Frame_impl(Border_style bs, unsigned border_width=1, int border_radius=0);
    Frame_impl(const ustring & label, Border_style bs, unsigned border_width=1, int border_radius=0);
    Frame_impl(const ustring & label, Align align, Border_style bs, unsigned border_width=1, int border_radius=0);

    void insert(Widget_ptr wp);
    void clear();
    void set_label(const ustring & text, Align align=ALIGN_CENTER);
    void set_label(Widget_ptr wp);
    void align_label(Align align);
    Align label_align() const { return align_; }
    void unset_label();
    void set_label_position(Label_pos label_pos);
    Label_pos label_position() const { return lpos_; }

    void set_border_left(unsigned npx);
    void set_border_right(unsigned npx);
    void set_border_top(unsigned npx);
    void set_border_bottom(unsigned npx);

    void set_border_left(unsigned px, Border_style bs);
    void set_border_right(unsigned px, Border_style bs);
    void set_border_top(unsigned px, Border_style bs);
    void set_border_bottom(unsigned px, Border_style bs);

    void set_border_left(unsigned px, Border_style bs, const Color & color);
    void set_border_right(unsigned px, Border_style bs, const Color & color);
    void set_border_top(unsigned px, Border_style bs, const Color & color);
    void set_border_bottom(unsigned px, Border_style bs, const Color & color);

    void set_border(unsigned px);
    void set_border(unsigned px, Border_style bs);
    void set_border(unsigned px, Border_style bs, const Color & color);

    void set_border(unsigned left, unsigned right, unsigned top, unsigned bottom);

    // Overriden by Counter_impl.
    virtual void set_border_style(Border_style bs) {
        set_border_style_priv(bs);
    }

    void set_border_style(Border_style left, Border_style right, Border_style top, Border_style bottom);
    void set_border_left_style(Border_style bs);
    void set_border_right_style(Border_style bs);
    void set_border_top_style(Border_style bs);
    void set_border_bottom_style(Border_style bs);

    void set_border_color(const Color & color);
    void set_border_color(const Color & left, const Color & right, const Color & top, const Color & bottom);
    void set_border_left_color(const Color & color);
    void set_border_right_color(const Color & color);
    void set_border_top_color(const Color & color);
    void set_border_bottom_color(const Color & color);

    void unset_border_color();
    void unset_border_left_color();
    void unset_border_right_color();
    void unset_border_top_color();
    void unset_border_bottom_color();

    unsigned border_left() const {
        return border_left_;
    }

    unsigned border_right() const {
        return border_right_;
    }

    unsigned border_top() const {
        return border_top_;
    }

    unsigned border_bottom() const {
        return border_left_;
    }

    Color border_left_color() const {
        return border_left_color_;
    }

    Color border_right_color() const {
        return border_right_color_;
    }

    Color border_top_color() const {
        return border_top_color_;
    }

    Color border_bottom_color() const {
        return border_bottom_color_;
    }

    Border_style border_left_style() const {
        return border_left_style_;
    }

    Border_style border_right_style() const {
        return border_right_style_;
    }

    Border_style border_top_style() const {
        return border_top_style_;
    }

    Border_style border_bottom_style() const {
        return border_bottom_style_;
    }

    void set_border_radius(int radius);

    int border_radius() const {
        return radius_;
    }

private:

    void init();
    void init_border_style(Border_style bs, unsigned width, int radius);
    void set_border_style_priv(Border_style bs);
    void update_requisition();
    Size child_requisition(Widget_impl * wp);
    unsigned eradius() const; // Radius value used for border stroking.
    unsigned mradius() const; // Radius value used for background rastering.
    unsigned min_border_size(Border_style bs);
    unsigned ceil_border(unsigned px, Border_style bs);

    // Returns true if changed.
    bool set_left_style(Border_style bs);
    bool set_right_style(Border_style bs);
    bool set_top_style(Border_style bs);
    bool set_bottom_style(Border_style bs);
    bool set_style(Border_style bs);

    // Returns true if changed.
    bool set_left_border(unsigned px);
    bool set_right_border(unsigned px);
    bool set_top_border(unsigned px);
    bool set_bottom_border(unsigned px);
    bool set_borders(unsigned px);

    // Returns true if invalidate() call needed.
    bool set_left_color(const Color & color);
    bool set_right_color(const Color & color);
    bool set_top_color(const Color & color);
    bool set_bottom_color(const Color & color);
    bool set_colors(const Color & color);

    void on_label_hide();
    void on_label_show();
    void on_child_hide();
    void on_child_show();

    bool on_paint(Painter pr, const Rect & inval);
    bool on_backpaint(Painter pr, const Rect & inval);
    void paint_border(Painter pr);
    void paint_background(Painter pr, const Rect & inval);
    void arrange();

protected:
    
    Widget_impl * cp_ = nullptr;

private:

    Widget_impl * label_ = nullptr;
    Align         align_ = ALIGN_CENTER;
    Label_pos     lpos_ = LABEL_TOP;
    Rect          lb_;

    // Border sizes set by user.
    unsigned      left_ = 0;
    unsigned      right_ = 0;
    unsigned      top_ = 0;
    unsigned      bottom_ = 0;

    // Actual border sizes.
    unsigned      border_left_ = 0;
    unsigned      border_right_ = 0;
    unsigned      border_top_ = 0;
    unsigned      border_bottom_ = 0;

    // Border sizes + label width/height.
    unsigned      lleft_ = 0;
    unsigned      lright_ = 0;
    unsigned      ltop_ = 0;
    unsigned      lbottom_ = 0;

    // Border radius.
    unsigned      radius_ = 0;

    Color         border_left_color_;
    Color         border_right_color_;
    Color         border_top_color_;
    Color         border_bottom_color_;

    bool          border_left_color_set_ = false;
    bool          border_right_color_set_ = false;
    bool          border_top_color_set_ = false;
    bool          border_bottom_color_set_ = false;

    Border_style  border_left_style_ = BORDER_NONE;
    Border_style  border_right_style_ = BORDER_NONE;
    Border_style  border_top_style_ = BORDER_NONE;
    Border_style  border_bottom_style_ = BORDER_NONE;

    connection    child_hints_cx_;
    connection    child_req_cx_;
    connection    child_hide_cx_;
    connection    child_show_cx_;
    connection    child_focus_cx_;

    connection    label_hints_cx_;
    connection    label_req_cx_;
    connection    label_hide_cx_;
    connection    label_show_cx_;
};

} // namespace tau

#endif // TAU_BORDER_IMPL_HH
