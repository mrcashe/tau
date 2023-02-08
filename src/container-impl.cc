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

#include <tau/exception.hh>
#include <tau/painter.hh>
#include <container-impl.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <painter-impl.hh>
#include <window-impl.hh>

#include <iostream>

namespace tau {

Container_impl::Container_impl():
    Widget_impl()
{
    allow_focus();

    signal_size_changed_.connect(fun(this, &Container_impl::update_mouse_owner));
    signal_visible_.connect(fun(this, &Container_impl::on_visible));
    signal_invisible_.connect(fun(this, &Container_impl::on_invisible));
    signal_enable_.connect(fun(this, &Container_impl::on_enable));
    signal_disable_.connect(fun(this, &Container_impl::on_disable));
    signal_unparent_.connect(fun(this, &Container_impl::on_unparent));
}

Container_impl::~Container_impl() {
    signal_destroy_();
    unparent_all();
}

void Container_impl::make_child(Widget_ptr wp) {
    if (!wp) { throw internal_error("Container_impl::make_child(): got a pure widget pointer"); }
    if (wp->parent()) { throw user_error(str_format("Container_impl::make_child(): widget ", wp.get(), " already has parent")); }
    children_.push_back(wp);
    if (auto * ci = dynamic_cast<Container_impl *>(wp.get())) { containers_.push_back(ci); }
    wp->set_parent(this);
    wp->handle_enable(enabled());
    if (display()) { wp->handle_display(); }
    wp->handle_visible(visible());
    signal_children_changed_();
}

void Container_impl::unparent_child(Widget_impl * wi) {
    auto i = std::find_if(children_.begin(), children_.end(), [wi](Widget_ptr wp) { return wi == wp.get(); } );

    if (children_.end() != i) {
        wi->end_modal();
        wi->drop_focus();
        wi->ungrab_mouse();
        wi->unparent();
        auto j = std::find(containers_.begin(), containers_.end(), static_cast<Container_impl *>(wi));
        if (j != containers_.end()) { containers_.erase(j); }
        auto k = std::find(obscured_.begin(), obscured_.end(), wi);
        if (k != obscured_.end()) { obscured_.erase(k); }
        woff_.push_back(*i);
        children_.erase(i);
        woff_timer_.restart(11, true);
        if (wi == modal_child_) { modal_child_ = nullptr; }
        if (wi == focused_child_) { focused_child_ = nullptr; }
        if (wi == mouse_grabber_) { mouse_grabber_ = nullptr; }
        if (wi == mouse_owner_) { mouse_owner_ = nullptr; }
        update_mouse_owner();
        signal_children_changed_();
    }
}

void Container_impl::unparent_all() {
    for (auto wp: children_) { woff_.push_back(wp); wp->unparent(); }
    woff_timer_.restart(11, true);
    children_.clear();
    obscured_.clear();
    containers_.clear();
    modal_child_ = nullptr;
    focused_child_ = nullptr;
    mouse_grabber_ = nullptr;
    mouse_owner_ = nullptr;

    if (!shut_) {
        end_modal_up(modal_child_);
        ungrab_mouse_up(mouse_grabber_);
        if (focused()) { grab_focus(); }
        signal_children_changed_();
    }
}

void Container_impl::paint_children(Painter pr, const Rect & inval, bool backpaint) {
    Painter_ptr pp = pr.impl;
    Point wpos = worigin(), sc = scroll_position();

    for (auto wp: obscured_) {
        Point worg = wp->origin();
        Rect  wbounds(worg-sc, wp->size());
        Rect  intersection = inval & wbounds;

        if (intersection) {
            pp->wpush();
            pp->poffset(wp->poffset());
            pp->pclip(intersection.translated(wpos));
            pp->push();
            pp->clear();
            Rect cinval(intersection.translated(sc-worg));
            if (backpaint) { wp->handle_backpaint(pr, cinval); }
            else { wp->handle_paint(pr, cinval); }
            pp->pop();
            pp->wpop();
        }
    }
}

// Overrides Widget_impl.
void Container_impl::handle_paint(Painter pr, const Rect & inval) {
    Widget_impl::handle_paint(pr, inval);
    paint_children(pr, inval, false);
}

// Overrides Widget_impl.
void Container_impl::handle_backpaint(Painter pr, const Rect & inval) {
    Widget_impl::handle_backpaint(pr, inval);
    paint_children(pr, inval, true);
}

void Container_impl::on_child_obscured(Widget_impl * wi, bool yes) {
    if (!shut_) {
        if (yes) {
            obscured_.push_back(wi);
        }

        else {
            auto i = std::find(obscured_.begin(), obscured_.end(), wi);
            if (i != obscured_.end()) { obscured_.erase(i); }
        }

        update_mouse_owner();
    }
}

void Container_impl::on_child_requisition(Widget_impl * wi) {
    signal_child_requisition_(wi);
}

void Container_impl::on_child_hints(Widget_impl * wi) {
    signal_child_hints_(wi);
}

void Container_impl::on_child_show(Widget_impl * wi) {
    signal_child_show_(wi);
}

void Container_impl::on_child_hide(Widget_impl * wi) {
    signal_child_hide_(wi);
}

// Overrides Widget_impl.
bool Container_impl::handle_accel(char32_t kc, int km) {
    if (enabled()) {
        if (modal_child_) {
            return modal_child_->handle_accel(kc, km);
        }

        if (focused_child_) {
            if (focused_child_->handle_accel(kc, km)) {
                return true;
            }
        }

        return Widget_impl::handle_accel(kc, km);
    }

    return false;
}

// Overrides Widget_impl.
bool Container_impl::handle_input(const ustring & s) {
    if (enabled()) {
        if (modal_child_) {
            return modal_child_->handle_input(s);
        }

        if (focused_child_) {
            if (focused_child_->handle_input(s)) {
                return true;
            }
        }

        return Widget_impl::handle_input(std::cref(s));
    }

    return false;
}

// Overrides Widget_impl.
bool Container_impl::handle_key_down(char32_t kc, int km) {
    if (enabled()) {
        if (modal_child_ && !modal_child_->hidden()) {
            return modal_child_->handle_key_down(kc, km);
        }

        if (focused_child_ && !focused_child_->hidden()) {
            if (focused_child_->handle_key_down(kc, km)) {
                return true;
            }
        }

        return Widget_impl::handle_key_down(kc, km);
    }

    return false;
}

// Overrides Widget_impl.
bool Container_impl::handle_key_up(char32_t kc, int km) {
    if (enabled()) {
        if (modal_child_ && !modal_child_->hidden()) {
            return modal_child_->handle_key_up(kc, km);
        }

        if (focused_child_ && !focused_child_->hidden()) {
            if (focused_child_->handle_key_up(kc, km)) {
                return true;
            }
        }

        return Widget_impl::handle_key_up(kc, km);
    }

    return false;
}

// Overrides Widget_impl.
void Container_impl::handle_display() {
    Widget_impl::handle_display();

    if (!shut_) {
        for (auto wp: children_) {
            wp->handle_display();
        }
    }
}

// Overrides Widget_impl.
void Container_impl::handle_parent() {
    Widget_impl::handle_parent();

    if (!shut_) {
        for (auto wp: children_) {
            wp->handle_parent();
        }
    }
}

// Overrides Widget_impl.
void Container_impl::handle_unparent() {
    if (!shut_) {
        for (auto wp: children_) {
            wp->handle_unparent();
        }
    }

    Widget_impl::handle_unparent();
}

// Private.
Widget_impl * Container_impl::mouse_target(const Point & pt) {
    if (!shut_) {
        if (mouse_grabber_) {
            return mouse_grabber_;
        }

        if (modal_child_) {
            return modal_child_;
        }

        for (auto wp: obscured_) {
            if (Rect(wp->origin(), wp->size()).contains(pt)) {
                return wp->enabled() ? wp : nullptr;
            }
        }
    }

    return nullptr;
}

// Private.
Widget_impl * Container_impl::mouse_target_update(const Point & pt) {
    Widget_impl * wt = mouse_target(pt);
    set_mouse_owner(wt, pt);
    return wt;
}

// Private.
void Container_impl::set_mouse_owner(Widget_impl * wi, const Point & pt) {
    if (mouse_owner_ != wi) {
        if (auto mo = mouse_owner_) {
            mouse_owner_ = nullptr;
            if (mo != mouse_grabber_) { mo->handle_mouse_leave(); }
        }

        if (wi) {
            mouse_owner_ = wi;
            wi->handle_mouse_enter(pt-wi->origin());
        }
    }
}

void Container_impl::update_mouse_owner() {
    if (!shut_) {
        Widget_impl * mt = nullptr;
        Point pt = where_mouse();
        if (hover()) { mt = mouse_target(pt); }
        set_mouse_owner(mt, pt);
    }
}

// Overrides Widget_impl.
bool Container_impl::handle_mouse_down(int mbt, int mm, const Point & pt) {
    if (auto wt = mouse_target_update(pt)) {
        if (wt->handle_mouse_down(mbt, mm, pt+wt->scroll_position()-wt->origin())) {
            return true;
        }
    }

    return Widget_impl::handle_mouse_down(mbt, mm, pt);
}

// Overrides Widget_impl.
bool Container_impl::handle_mouse_up(int mbt, int mm, const Point & pt) {
    if (auto wt = mouse_target_update(pt)) {
        if (wt->handle_mouse_up(mbt, mm, pt+wt->scroll_position()-wt->origin())) {
            return true;
        }
    }

    return Widget_impl::handle_mouse_up(mbt, mm, pt);
}

// Overrides Widget_impl.
bool Container_impl::handle_mouse_double_click(int mbt, int mm, const Point & pt) {
    if (auto wt = mouse_target_update(pt)) {
        if (wt->handle_mouse_double_click(mbt, mm, pt+wt->scroll_position()-wt->origin())) {
            return true;
        }
    }

    return Widget_impl::handle_mouse_double_click(mbt, mm, pt);
}

// Overrides Widget_impl.
bool Container_impl::handle_mouse_wheel(int delta, int mm, const Point & pt) {
    if (auto wt = mouse_target_update(pt)) {
        if (wt->handle_mouse_wheel(delta, mm, pt+wt->scroll_position()-wt->origin())) {
            return true;
        }
    }

    return Widget_impl::handle_mouse_wheel(delta, mm, pt);
}

// Overrides Widget_impl.
void Container_impl::handle_mouse_motion(int mm, const Point & pt) {
    if (auto wt = mouse_target_update(pt)) {
        wt->handle_mouse_motion(mm, pt+wt->scroll_position()-wt->origin());
    }

    else {
        Widget_impl::handle_mouse_motion(mm, pt);
    }
}

// Overrides Widget_impl.
void Container_impl::handle_mouse_enter(const Point & pt) {
    Widget_impl::handle_mouse_enter(pt);
    mouse_target_update(pt);
}

// Overrides Widget_impl.
void Container_impl::handle_mouse_leave() {
    set_mouse_owner(nullptr);
    Widget_impl::handle_mouse_leave();
}

// Overriden by Window_impl.
// Overrides Widget_impl.
bool Container_impl::grab_mouse_up(Widget_impl * caller) {
    if (!shut_ && enabled()) {
        if (caller == mouse_grabber_) {
            return true;
        }

        if (parent_) {
            if (this == parent_->mouse_grabber()) {
                return this == caller;
            }

            if (parent_->grab_mouse_up(this)) {
                if (this == caller) {
                    ungrab_mouse_down();
                }

                else {
                    mouse_grabber_ = caller;
                }

                return true;
            }
        }
    }

    return false;
}

// Overriden by Window_impl.
// Overrides Widget_impl.
bool Container_impl::ungrab_mouse_up(Widget_impl * caller) {
    if (!shut_ && caller) {
        if (caller == mouse_grabber_) {
            mouse_grabber_ = nullptr;
        }

        if (!mouse_grabber_ && parent_) {
            return parent_->ungrab_mouse_up(this);
        }
    }

    return false;
}

void Container_impl::ungrab_mouse_down() {
    if (auto mg = mouse_grabber_) {
        mouse_grabber_ = nullptr;

        if (Container_impl * ci = dynamic_cast<Container_impl *>(mg)) {
            ci->ungrab_mouse_down();
        }

        else {
            mg->handle_mouse_leave();
        }
    }

    if (parent_ && this != parent_->mouse_owner()) {
        Widget_impl::handle_mouse_leave();
    }
}

// Overrides Widget_impl.
// Overriden by Window_impl.
bool Container_impl::grabs_mouse() const {
    if (parent_ && !mouse_grabber_) {
        return this == parent_->mouse_grabber();
    }

    return false;
}

// Overrides Widget_impl.
// Overriden by Window_impl.
bool Container_impl::grab_modal_up(Widget_impl * caller) {
    if (!shut_ && enabled()) {
        if (this == caller && has_modal()) {
            return true;
        }

        if (!modal_child_ && parent_ && parent_->grab_modal_up(this)) {
            if (this != caller) { set_modal_child(caller); }
            return true;
        }
    }

    return false;
}

// Overriden by Window_impl.
// Overrides Widget_impl.
bool Container_impl::end_modal_up(Widget_impl * caller) {
    bool res = false;

    if (!shut_ && caller) {
        res = (this == caller || modal_child_ == caller) && parent_ && parent_->end_modal_up(this);

        if (res) {
            if (auto mc = modal_child_) {
                modal_child_ = nullptr;
                mc->clear_focus();
            }
        }
    }

    return res;
}

void Container_impl::set_modal_child(Widget_impl * caller) {
    if (modal_child_ != caller) {
        if (auto mc = modal_child_) {
            modal_child_ = nullptr;
            mc->clear_focus();
        }

        if (auto fc = focused_child_) {
            focused_child_ = nullptr;
            fc->clear_focus();
        }

        if (this != caller) { modal_child_ = caller; }
        Widget_impl::resume_focus();
    }
}

void Container_impl::focus_child(Widget_impl * caller, int res) {
    if (caller != focused_child_) {
        if (auto fc = focused_child_) {
            focused_child_ = nullptr;
            fc->clear_focus();
        }

        if (this != caller) {
            focused_child_ = caller;
        }

        if (res > 0) {
            Widget_impl::resume_focus();
            if (caller) { caller->resume_focus(); }
        }
    }
}

// Overriden by Window_impl.
// Overrides Widget_impl.
int Container_impl::grab_focus_up(Widget_impl * caller) {
    if (shut_ || !focusable() || has_modal()) { return -1; }

    if (!focused()) {
        if (!parent_) { return -1; }
        int res = parent_->grab_focus_up(this);
        if (res >= 0) { focus_child(caller, res); }
        return res;
    }

    focus_child(caller, 1);
    return 1;
}

// Overriden by Window_impl.
// Overrides Widget_impl.
void Container_impl::drop_focus_up(Widget_impl * caller) {
    if (!shut_) {
        bool had_focus = parent_ && this == parent_->focused_child();
        auto fc = focused_child_;

        if (fc) {
            focused_child_ = nullptr;
            if ((fc == caller || this == caller) && fc != modal_child_) { fc->clear_focus(); }
        }

        if (this != caller && focusable() && had_focus && fc != modal_child_) {
            signal_focus_in_();
            return;
        }

        if (parent_ && this == parent_->focused_child()) {
            parent_->drop_focus_up(this);
        }
    }
}

// Overrides Widget_impl.
void Container_impl::suspend_focus() {
    Widget_impl::suspend_focus();

    if (modal_child_) {
        modal_child_->suspend_focus();
    }

    else if (focused_child_) {
        focused_child_->suspend_focus();
    }
}

// Overrides Widget_impl.
void Container_impl::resume_focus() {
    Widget_impl::resume_focus();

    if (modal_child_) {
        modal_child_->resume_focus();
    }

    else if (focused_child_) {
        focused_child_->resume_focus();
    }
}

// Overrides Widget_impl.
void Container_impl::clear_focus() {
    Widget_impl::clear_focus();
    for (auto wp: obscured_) { wp->clear_focus(); }
    modal_child_ = nullptr;
    focused_child_ = nullptr;
}

// Overriden by Window_impl.
// Overrides Widget_impl.
void Container_impl::set_cursor_up(Cursor_ptr cursor) {
    if (!shut_ && !cursor_ && parent_) {
        parent_->set_cursor_up(cursor);
    }
}

// Overriden by Window_impl.
// Overrides Widget_impl.
void Container_impl::unset_cursor_up() {
    if (!shut_ && !cursor_hidden_ && parent_) {
        if (cursor_) {
            parent_->set_cursor_up(cursor_);
        }

        else {
            parent_->unset_cursor_up();
        }
    }
}

// Overriden by Window_impl.
// Overrides Widget_impl.
void Container_impl::show_cursor_up() {
    if (!shut_ && parent_ && !cursor_hidden_) {
        parent_->show_cursor_up();
    }
}

void Container_impl::sync_arrange() {
    if (visible()) {
        if (arrange_) { arrange_ = false; signal_arrange_(); }
        for (auto ci: containers_) { ci->sync_arrange(); }
    }
}

// Overriden by Window_impl.
void Container_impl::queue_arrange_up() {
    if (parent_) { parent_->queue_arrange_up(); }
}

void Container_impl::queue_arrange() {
    arrange_ = true;
    queue_arrange_up();
}

void Container_impl::on_visible() {
    for (auto wp: children_) {
        if (!std::dynamic_pointer_cast<Window_impl>(wp)) {
            wp->handle_visible(true);
        }
    }
}

void Container_impl::on_invisible() {
    suspend_focus();
    for (auto wp: children_) { wp->handle_visible(false); }
}

void Container_impl::on_enable() {
    for (auto wp: children_) { wp->handle_enable(true); }
}

void Container_impl::on_disable() {
    suspend_focus();
    for (auto wp: children_) { wp->handle_enable(false); }
}

void Container_impl::on_unparent() {
    obscured_.clear();
    focused_child_ = nullptr;
    modal_child_ = nullptr;
    mouse_grabber_ = nullptr;
    mouse_owner_ = nullptr;
}

void Container_impl::on_woff_timer() {
    for (auto wp: woff_) { if (wp->running()) return; }
    woff_timer_.stop();
    woff_.clear();
}

// Overrides Widget_impl.
// Overriden by Dialog_impl.
bool Container_impl::running() const {
    return Widget_impl::running() || std::any_of(children_.begin(), children_.end(), [](auto wp) { return wp->running(); } );
}

Widget_ptr Container_impl::cptr(Widget_impl * wi) {
    auto i = std::find_if(children_.begin(), children_.end(), [wi](auto wp) { return wp.get() == wi; } );
    return i != children_.end() ? *i : nullptr;
}

Widget_cptr Container_impl::cptr(const Widget_impl * wi) const {
    auto i = std::find_if(children_.begin(), children_.end(), [wi](auto wp) { return wp.get() == wi; } );
    return i != children_.end() ? *i : nullptr;
}

// Overriden by Window_impl.
Widget_ptr Container_impl::focus_owner() {
    if (focused()) {
        auto fc = modal_child_ ? modal_child_ : focused_child_;

        if (auto ci = dynamic_cast<Container_impl *>(fc)) {
            if (auto wp = ci->focus_owner()) {
                return wp;
            }
        }

        if (auto cp = cptr(fc)) { return cp; }
        if (parent_) { return parent_->cptr(this); }
    }

    return nullptr;
}

// Overriden by Window_impl.
Widget_cptr Container_impl::focus_owner() const {
    if (focused()) {
        auto fc = modal_child_ ? modal_child_ : focused_child_;

        if (auto ci = dynamic_cast<const Container_impl *>(fc)) {
            if (auto wp = ci->focus_owner()) {
                return wp;
            }
        }

        if (auto cp = cptr(fc)) { return cp; }
        if (parent_) { return parent_->cptr(this); }
    }

    return nullptr;
}

bool Container_impl::update_child_bounds(Widget_impl * wp, const Rect & bounds) {
    return update_child_bounds(wp, bounds.origin(), bounds.size());
}

bool Container_impl::update_child_bounds(Widget_impl * wp, const Point & origin, const Size & sz) {
    bool changed = wp->update_origin(origin);
    if (wp->update_size(sz)) { changed = true; }
    return changed;
}

bool Container_impl::update_child_bounds(Widget_impl * wp, int x, int y, const Size & sz) {
    return update_child_bounds(wp, Point(x, y), sz);
}

bool Container_impl::update_child_bounds(Widget_impl * wp, int x, int y, unsigned w, unsigned h) {
    return update_child_bounds(wp, Point(x, y), Size(w, h));
}

// Overrides Widget_impl.
void Container_impl::shutdown(bool yes) {
    Widget_impl::shutdown(yes);
    for (auto wp: children_) { wp->shutdown(yes); }
}

// Overrides Widget_impl.
void Container_impl::update_pdata() {
    Widget_impl::update_pdata();
    for (auto wp: children_) { wp->update_pdata(); }
}

// Overrides Widget_impl.
Action_base * Container_impl::lookup_action(char32_t kc, int km) {
    for (auto wp: children_) {
        if (auto action = wp->lookup_action(kc, km)) {
            return action;
        }
    }

    return Widget_impl::lookup_action(kc, km);
}

} // namespace tau

//END
