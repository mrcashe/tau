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

#ifndef TAU_NOTEBOOK_IMPL_HH
#define TAU_NOTEBOOK_IMPL_HH

#include <tau/notebook.hh>
#include <box-impl.hh>

namespace tau {

class Notebook_impl: public Box_impl {
public:

    Notebook_impl(Tab_pos tab_pos=TAB_TOP);
   ~Notebook_impl() { signal_destroy_(); }

    int  append_page(Widget_ptr wp);
    int  append_page(Widget_ptr wp, const ustring & title, Widget_ptr * rptr=nullptr);
    int  append_page(Widget_ptr wp, Widget_ptr title);

    void prepend_page(Widget_ptr wp);
    Widget_ptr prepend_page(Widget_ptr wp, const ustring & title);
    void prepend_page(Widget_ptr wp, Widget_ptr tp);

    int insert_page(Widget_ptr wp, int nth_page);
    int insert_page(Widget_ptr wp, int nth_page, const ustring & title, Widget_ptr * rptr=nullptr);
    int insert_page(Widget_ptr wp, int nth_page, Widget_ptr tp);

    int insert_page_after(Widget_ptr wp, Widget_ptr after_this);
    int insert_page_after(Widget_ptr wp, Widget_ptr after_this, const ustring & title, Widget_ptr * rptr=nullptr);
    int insert_page_after(Widget_ptr wp, Widget_ptr after_this, Widget_ptr tp);

    int insert_page_before(Widget_ptr wp, Widget_ptr before_this);
    int insert_page_before(Widget_ptr wp, Widget_ptr before_this, const ustring & title, Widget_ptr * rptr=nullptr);
    int insert_page_before(Widget_ptr wp, Widget_ptr before_this, Widget_ptr tp);

    int  remove_page(Widget_ptr wp);
    int  remove_page(int page);
    void clear_pages();
    int  page_number(Widget_cptr cwp) const;

    int  show_next();
    int  show_previous();
    int  show_page(int nth_page);

    void reorder_page(Widget_ptr wp, int nth_page);
    void reorder_page(int old_page, int new_page);

    void allow_reorder();
    void disallow_reorder();

    bool reorder_allowed() const {
        return reorder_allowed_;
    }

    void allow_rollover() { rollover_allowed_ = true; }
    void disallow_rollover() { rollover_allowed_ = false; }
    bool rollover_allowed() const { return rollover_allowed_; }

    void show_tabs();
    void hide_tabs();

    bool tabs_visible() const {
        return tabs_visible_;
    }

    int current_page() const;

    std::size_t page_count() const {
        return pages_.size();
    }

    bool empty() const {
        return pages_.empty();
    }

    void set_homogeneous_tabs();
    void unset_homogeneous_tabs();

    bool homogeneous_tabs() const {
        return homogeneous_tabs_;
    }

    signal<void(int)> & signal_page_added() {
        return signal_page_added_;
    }

    signal<void(int)> & signal_page_removed() {
        return signal_page_removed_;
    }

    signal<void(int)> & signal_page_changed() {
        return signal_page_changed_;
    }

    signal<void(int, int)> & signal_page_reordered() {
        return signal_page_reordered_;
    }

private:

    struct Page {
        Widget_ptr          wp;
        Widget_ptr          title;
        Frame_ptr           frame;

        connection          size1_cx;
        connection          size2_cx;
        connection          requisition_cx;
        connection          hints_cx;
        connection          mouse_down_cx;
        connection          mouse_up_cx;
        connection          mouse_motion_cx;

        connection          show_cx;
        connection          visible_cx;
        connection          hide_cx;
    };

    Frame_ptr               frame_;
    Roller_ptr              roller_;
    Absolute_ptr            abs_;
    Card_ptr                card_;

    std::vector<Page>       pages_;
    std::size_t             spc_ = 2;       // Tab spacing.
    Widget_impl *           drag_ = nullptr; // Currently dragging frame.
    int                     last_ = -1;
    bool                    reorder_allowed_ = true;
    bool                    rollover_allowed_ = true;
    bool                    in_arrange_ = false;
    bool                    tabs_visible_ = true;
    bool                    homogeneous_tabs_ = false;

    signal<void(int)>       signal_page_added_;
    signal<void(int)>       signal_page_removed_;
    signal<void(int)>       signal_page_changed_;
    signal<void(int, int)>  signal_page_reordered_;

private:

    int  find_current();
    Color sel_color();
    void init_page(unsigned nth_page, Widget_ptr wp, Widget_ptr tp);
    void update_roller();
    void update_tabs();
    void update_current();
    void update_frame_border();
    Size child_requisition(Widget_impl * wp);

    bool on_tab_mouse_down(int mbt, int mm, const Point & pt, Widget_impl * wi);
    bool on_tab_mouse_up(int mbt, int mm, const Point & pt, Widget_impl * wi);
    void on_tab_mouse_motion(int mm, const Point & pt, Widget_impl * wi);
    bool on_mouse_wheel(int d, int mm, const Point & pt);
    void on_frame_background_changed();
};

} // namespace tau

#endif // TAU_NOTEBOOK_IMPL_HH
