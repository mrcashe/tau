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

#ifndef TAU_BOX_IMPL_HH
#define TAU_BOX_IMPL_HH

#include <tau/enums.hh>
#include <tau/action.hh>
#include <container-impl.hh>

namespace tau {

class Box_impl: public Container_impl {
public:

    explicit Box_impl(Orientation orient, unsigned spacing=0);
   ~Box_impl();

    Align align() const { return align_; }
    void set_align(Align align);
    void set_spacing(unsigned spacing);
    unsigned spacing() const { return spacing_; }

    // Overriden by Menubar_impl.
    // Overriden by Menubox_impl.
    virtual void append(Widget_ptr wp, bool shrink=false);

    // Overriden by Menubar_impl.
    // Overriden by Menubox_impl.
    virtual void prepend(Widget_ptr wp, bool shrink=false);

    // Overriden by Menubar_impl.
    // Overriden by Menubox_impl.
    virtual void insert_before(Widget_ptr wp, const Widget_impl * other, bool shrink=false);

    // Overriden by Menubar_impl.
    // Overriden by Menubox_impl.
    virtual void insert_after(Widget_ptr wp, const Widget_impl * other, bool shrink=false);

    // Overriden by Menubar_impl.
    // Overriden by Menubox_impl.
    virtual void remove(Widget_impl * wp);

    // Overriden by Menubar_impl.
    // Overriden by Menubox_impl.
    virtual void clear();

    void remove_before(const Widget_impl * wp);
    void remove_after(const Widget_impl * wp);
    void remove_front();
    void remove_back();

    bool empty() const;
    bool shrunk(const Widget_impl * wp) const;
    void shrink(Widget_impl * wp);
    void expand(Widget_impl * wp);
    void shrink_all();
    void expand_all();
    bool horizontal() const;
    void set_orientation(Orientation orient);
    Orientation orientation() const { return orient_; }

    Action & focus_next_action() { return next_action_; }
    Action & focus_previous_action() { return prev_action_; }

    signal<void()> & signal_orientation_changed();

private:

    struct Holder {
        Widget_impl *   wp_;
        bool            sh_ = false;
        Size            req_;
    };

    std::list<Holder>   holders_;
    unsigned            spacing_ = 0;
    Align               align_ = ALIGN_START;
    Orientation         orient_;
    Action              next_action_ { "Tab", fun(this, &Box_impl::focus_next) };
    Action              prev_action_ { "<Shift>Tab <Shift>LeftTab", fun(this, &Box_impl::focus_previous) };

    // ---------------------------
    // Cached by calc_requisiton()
    // ---------------------------

    unsigned            nvisible_ = 0;  // Visible widgets count.
    unsigned            nsh_ = 0;       // Shrunk widget count.
    unsigned            req_ = 0;       // Sum of spaces, shrunk requisitions and margins.

    signal<void()> *    signal_orientation_changed_ = nullptr;

private:

    void update_requisition();
    void arrange();
    void new_child(Holder & hol, Widget_ptr wp, bool shrink);
    void rm_child(Holder & h);
    void focus_next();
    void focus_previous();

    void on_child_requisition(Widget_impl * wi);
    void on_child_show(Widget_impl * wi);
    void on_child_hide(Widget_impl * wi);
    bool on_take_focus();
    bool on_forward();
    bool on_reverse();
};

} // namespace tau

#endif // TAU_BOX_IMPL_HH
