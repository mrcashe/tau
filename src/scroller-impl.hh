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

#ifndef TAU_SCROLLER_IMPL_HH
#define TAU_SCROLLER_IMPL_HH

#include <tau/action.hh>
#include <container-impl.hh>

namespace tau {

class Scroller_impl: public Container_impl {
public:

    Scroller_impl();
   ~Scroller_impl() { signal_destroy_(); }

    void insert(Widget_ptr wp);
    void clear();

    Size logical_size() const;
    Point pan() const;

    void pan_to_x(int x);
    void pan_to_y(int y);
    void pan_to(const Point & pos);
    void pan_to(int x, int y);

    Action & pan_left_action() { return pan_left_action_; }
    Action & pan_right_action() { return pan_right_action_; }
    Action & pan_up_action() { return pan_up_action_; }
    Action & pan_down_action() { return pan_down_action_; }
    Action & previous_page_action() { return previous_page_action_; }
    Action & next_page_action() { return next_page_action_; }
    Action & home_action() { return home_action_; }
    Action & end_action() { return end_action_; }

    void set_step(const Point & step) { step_ = step; }
    void set_step(int xstep, int ystep) { step_.set(xstep, ystep); }
    Point step() const { return step_; }

    signal<void()> & signal_pan_changed() { return signal_pan_changed_; }
    signal<void()> & signal_logical_size_changed() { return signal_logical_size_changed_; }

private:

    Widget_ptr  cp_;
    connection  req_cx_;
    connection  hint_cx_;
    connection  show_cx_;
    connection  hide_cx_;
    Point       pan_;
    Point       step_ { 16, 16 };

    Action      pan_left_action_   { "<Ctrl><Alt>Left",  fun(this, &Scroller_impl::pan_left)  };
    Action      pan_right_action_  { "<Ctrl><Alt>Right", fun(this, &Scroller_impl::pan_right) };
    Action      pan_up_action_     { "<Ctrl>Up",         fun(this, &Scroller_impl::pan_up)    };
    Action      pan_down_action_   { "<Ctrl>Down",       fun(this, &Scroller_impl::pan_down)  };
    Action      previous_page_action_    { "PageUp",           fun(this, &Scroller_impl::page_up)   };
    Action      next_page_action_  { "PageDown",         fun(this, &Scroller_impl::page_down) };
    Action      home_action_       { KC_HOME, KM_NONE,   fun(this, &Scroller_impl::home)      };
    Action      end_action_        { KC_END,  KM_NONE,   fun(this, &Scroller_impl::end)       };

    signal<void()> signal_pan_changed_;
    signal<void()> signal_logical_size_changed_;

private:

    void update_requisition();
    Size child_requisition() const;
    void limit_scroll();
    void arrange();
    bool update_offset(const Point & pt);
    bool update_offset(int x, int y);

    void pan_left();
    void pan_right();
    void pan_down();
    void pan_up();
    void page_down();
    void page_up();
    void home();
    void end();

    bool on_mouse_down(int mbt, int mm, const Point & pt);
    bool on_mouse_wheel(int delta, int mm, const Point & pt);
    void on_child_show();
    void on_child_hide();
    bool on_take_focus();
};

} // namespace tau

#endif // TAU_SCROLLER_IMPL_HH
