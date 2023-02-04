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

#ifndef TAU_CONTAINER_IMPL_HH
#define TAU_CONTAINER_IMPL_HH

#include <widget-impl.hh>

namespace tau {

class Container_impl: public Widget_impl {
    friend class Container;

public:

    Container_impl();
   ~Container_impl();

    // Overrides Widget_impl.
    bool handle_accel(char32_t kc, int km) override;

    // Overrides Widget_impl.
    bool handle_key_down(char32_t kc, int km) override;

    // Overrides Widget_impl.
    bool handle_key_up(char32_t kc, int km) override;

    // Overrides Widget_impl.
    bool handle_input(const ustring & s) override;

    // Overrides Widget_impl.
    void handle_paint(Painter pr, const Rect & inval) override;

    // Overrides Widget_impl.
    void handle_backpaint(Painter pr, const Rect & inval) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    void set_cursor_up(Cursor_ptr cursor) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    void unset_cursor_up() override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    void show_cursor_up() override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    bool grab_modal_up(Widget_impl * caller) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    bool end_modal_up(Widget_impl * caller) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    int  grab_focus_up(Widget_impl * caller) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    void drop_focus_up(Widget_impl * caller) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    bool grab_mouse_up(Widget_impl * wi) override;

    // Overriden by Window_impl.
    // Overrides Widget_impl.
    bool ungrab_mouse_up(Widget_impl * caller) override;

    // Overrides Widget_impl.
    // Overriden by Window_impl.
    bool grabs_mouse() const override;

    // Overrides Widget_impl.
    void suspend_focus() override;

    // Overrides Widget_impl.
    // Overriden by Dialog_impl.
    bool running() const override;

    void queue_arrange();
    void sync_arrange();
    void ungrab_mouse_down();
    void on_child_visibility(Widget_impl * wi);
    void on_obscure(Widget_impl * wi, bool yes);

    const Widget_impl * focused_child() const { return focused_child_; }
    const Widget_impl * modal_child() const { return modal_child_; }
    const Widget_impl * mouse_grabber() const { return mouse_grabber_; }
    const Widget_impl * mouse_owner() const { return mouse_owner_; }

    // Overriden by Window_impl.
    virtual Widget_ptr focus_owner();

    // Overriden by Window_impl.
    virtual Widget_cptr focus_owner() const;

    std::vector<Widget_ptr> children() { return children_; }
    Widget_ptr cptr(Widget_impl * wi);
    Widget_cptr cptr(const Widget_impl * wi) const;

    signal<void()> & signal_arrange() { return signal_arrange_; }
    signal<void()> & signal_children_changed() { return signal_children_changed_; }

protected:

    Widget_impl *   focused_child_ = nullptr;
    Widget_impl *   modal_child_ = nullptr;
    Widget_impl *   mouse_grabber_ = nullptr;
    Widget_impl *   mouse_owner_ = nullptr;

    signal<void()>  signal_arrange_;
    signal<void()>  signal_children_changed_;

protected:

    void make_child(Widget_ptr  wp);
    void unparent_child(Widget_impl * wi);
    void unparent_all();
    void focus_child(Widget_impl * wi, int res);
    void set_modal_child(Widget_impl * wi);
    bool update_child_bounds(Widget_impl * wp, const Rect & bounds);
    bool update_child_bounds(Widget_impl * wp, const Point & origin, const Size & sz);
    bool update_child_bounds(Widget_impl * wp, int x, int y, const Size & sz);
    bool update_child_bounds(Widget_impl * wp, int x, int y, unsigned w, unsigned h);

    // Overrides Widget_impl.
    void resume_focus() override;

    // Overrides Widget_impl.
    void clear_focus() override;

private:

    using Children = std::vector<Widget_ptr>;
    using Containers = std::list<Container_impl *>;
    using Obscured = std::list<Widget_impl *>;

    Children        children_;          // Children.
    Children        woff_;              // Written off children.
    Containers      containers_;        // Owning containers.
    Obscured        obscured_;          // Currently obscured children.
    connection      woff_cx_ { true };
    connection      arrange_cx_ { true };

private:

    Widget_impl * mouse_target(const Point & pt);
    Widget_impl * mouse_target_update(const Point & pt);
    void set_mouse_owner(Widget_impl * wi, const Point & pt=Point());
    void update_mouse_owner();

    void paint_children(Painter pr, const Rect & inval, bool backpaint);

    bool on_mouse_down(int mbt, int mm, const Point & pt);
    bool on_mouse_double_click(int mbt, int mm, const Point & pt);
    bool on_mouse_up(int mbt, int mm, const Point & pt);
    void on_mouse_motion(int mm, const Point & pt);
    bool on_mouse_wheel(int delta, int mm, const Point & pt);
    void on_mouse_enter(const Point & pt);
    void on_visible();
    void on_invisible();
    void on_enable();
    void on_disable();
    void on_woff_timer();
    void on_unparent();
};

} // namespace tau

#endif // TAU_CONTAINER_IMPL_HH
