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

#include <tau/input.hh>
#include <tau/exception.hh>
#include <tau/painter.hh>
#include <display-impl.hh>
#include <frame-impl.hh>
#include <loop-impl.hh>
#include <popup-impl.hh>
#include <iostream> // needed for error message output.

namespace tau {

Window_impl::Window_impl():
    Bin_impl()
{
    signal_focus_in_.connect(fun(this, &Widget_impl::resume_focus));
    signal_focus_out_.connect(fun(this, &Widget_impl::suspend_focus));
}

void Window_impl::close() {
    if (tooltip_) { tooltip_->close(); }
    tooltip_widget_ = nullptr;
    end_modal();
    ungrab_mouse();
    hide();
    signal_close_();
}

// Overrides Widget_impl.
Point Window_impl::to_screen(const Point & pt) const {
    if (auto pw = parent_window()) { return pw->to_screen(pt+position()); }
    return pt+position();
}

// Overrides Widget_impl.
Point Window_impl::to_window(const Point & pt) const {
    return pt;
}

// Overrides Widget_impl.
Point Window_impl::to_root(const Point & pt) const {
    return !shut_ && parent_ ? parent_->to_root(pt+origin())-parent_->scroll_position() : pt;
}

// Overrides Widget_impl.
bool Window_impl::has_modal() const {
    return !shut_ && display() && this == display()->modal_window();
}

// Overrides Container_impl.
// Overrides Widget_impl.
bool Window_impl::grab_modal_up(Widget_impl * caller) {
    if (caller == modal_child_) {
        return true;
    }

    if (has_modal()) {
        return caller == this && !modal_child_;
    }

    if (!shut_ && display() && display()->grab_modal(this)) {
        Widget_impl::resume_focus();
        if (caller != this) { set_modal_child(caller); }
        return true;
    }

    return false;
}

// Overrides Container_impl.
// Overrides Widget_impl.
bool Window_impl::end_modal_up(Widget_impl * caller) {
    if (!shut_ && caller && display() && display()->end_modal(this)) {
        if (auto mc = modal_child_) {
            if (mc == caller) {
                modal_child_ = nullptr;
                mc->clear_focus();
            }
        }

        return true;
    }

    return false;
}

// Overrides Container_impl.
// Overrides Widget_impl.
int Window_impl::grab_focus_up(Widget_impl * caller) {
    if (shut_ || has_modal()) { return -1; }

    int res = 1;

    if (!focused() && display()) {
        display()->grab_window_focus(this);
        res = 0;
    }

    focus_child(caller, res);
    return res;
}

// Overrides Container_impl.
// Overrides Widget_impl.
void Window_impl::drop_focus_up(Widget_impl * caller) {
    if (auto fc = focused_child_) {
        focused_child_ = nullptr;
        if (fc == caller && fc != modal_child_) { fc->clear_focus(); }
    }
}

bool Window_impl::update_position(const Point & pt) {
    bool changed = position_.update(pt);
    if (changed) { signal_position_changed_(); }
    return changed;
}

// From window system: client area changed.
void Window_impl::handle_client_area(const Rect & r) {
    if (r != client_area_) {
        client_area_ = r;
    }
}

// Overrides Container_impl.
// Overrides Widget_impl.
bool Window_impl::grab_mouse_up(Widget_impl * caller) {
    if (!shut_ && enabled()) {
        if (caller != mouse_grabber_ && display()) {
            ungrab_mouse_down();
            mouse_grabber_ = this != caller ? caller : nullptr;
            display()->grab_mouse(this);
        }

        return true;
    }

    return false;
}

// Overrides Container_impl.
// Overrides Widget_impl.
bool Window_impl::ungrab_mouse_up(Widget_impl * caller) {
    if (!shut_ && caller && display()) {
        if (caller == mouse_grabber_) {
            mouse_grabber_ = nullptr;
        }

        if (this != display()->mouse_grabber()) {
            return true;
        }

        if (!mouse_grabber_) {
            display()->ungrab_mouse();
            return true;
        }
    }

    return false;
}

// Overrides Container_impl.
// Overrides Widget_impl.
bool Window_impl::grabs_mouse() const {
    return !shut_ && !mouse_grabber_ && display() && this == display()->mouse_grabber();
}

// Overrides Widget_impl.
bool Window_impl::hover() const {
    return !shut_ && display() && (this == display()->mouse_owner() || this == display()->mouse_grabber());
}

// Overrides Widget_impl.
Point Window_impl::where_mouse() const {
    Point pt(INT_MIN, INT_MIN);

    if (!shut_ && display()) {
        Point co = client_area().origin();
        if (auto pw = parent_window()) { return pw->where_mouse()-position()-co; }
        pt.set(display()->where_mouse()-position()-co);
    }

    return pt;
}

void Window_impl::move(const Point & pt) { winface_->move(pt); }
void Window_impl::move(int x, int y) { move(Point(x, y)); }

void Window_impl::move(const Rect & r) { winface_->move(r); }
void Window_impl::move(int x, int y, unsigned width, unsigned height) { move(Rect(x, y, Size(width, height))); }

void Window_impl::resize(const Size & size) { winface_->resize(size); }
void Window_impl::resize(unsigned width, unsigned height) { resize(Size(width, height)); }

// Overrides Widget_impl.
bool Window_impl::cursor_visible() const {
    return !hidden() && size() && !cursor_hidden();
}

// Overrides Widget_impl.
void Window_impl::set_cursor_up(Cursor_ptr cursor) {
    if (!shut_ && (!cursor_ || cursor_ == cursor)) {
        winface_->set_cursor(cursor);
    }
}

// Overrides Widget_impl.
void Window_impl::unset_cursor_up() {
    if (!shut_) {
        if (cursor_) {
            winface_->set_cursor(cursor_);
        }

        else {
            winface_->unset_cursor();
        }
    }
}

// Overrides Widget_impl.
void Window_impl::show_cursor_up() {
    if (!cursor_hidden_) {
        winface_->show_cursor(true);
    }
}

// Overrides Widget_impl.
void Window_impl::hide_cursor_up() {
    winface_->show_cursor(false);
}

bool Window_impl::has_window() const {
    return true;
}

void Window_impl::on_tooltip_close() {
    tooltip_.reset();
    tooltip_cx_.drop();
    tooltip_widget_ = nullptr;
}

void Window_impl::on_tooltip_mouse_motion(int mm, const Point & pt) {
    if (tooltip_) {
        if (!Rect(tooltip_->size()).contains(pt)) {
            Rect r(tooltip_widget_->to_window(), tooltip_widget_->size());
            if (!r.contains(where_mouse())) { close_tooltip(tooltip_widget_); }
        }
    }
}

bool Window_impl::on_tooltip_mouse_down(int mbt, int mm, const Point & pt) {
    Point mpt = where_mouse();
    signal_mouse_down_(mbt, mm, mpt);
    signal_mouse_up_(mbt, mm, mpt);
    close_tooltip(tooltip_widget_);
    return true;
}

bool Window_impl::on_tooltip_mouse_wheel(int d, int mm, const Point & pt) {
    close_tooltip(tooltip_widget_);
    signal_mouse_wheel_(d, mm, where_mouse());
    return true;
}

Window_ptr Window_impl::open_tooltip(Widget_impl * caller, Widget_ptr tooltip, const Point & pt, Gravity gravity, unsigned time_ms) {
    if (!display()) { return nullptr; }
    tooltip_ = display()->create_popup(display(), this, pt, gravity);
    tooltip_->style().redirect("tooltip/background", "background");
    tooltip_->signal_close().connect(fun(this, &Window_impl::on_tooltip_close));
    tooltip_->signal_mouse_motion().connect(fun(this, &Window_impl::on_tooltip_mouse_motion));
    tooltip_->signal_mouse_down().connect(fun(this, &Window_impl::on_tooltip_mouse_down), true);
    tooltip_->signal_mouse_wheel().connect(fun(this, &Window_impl::on_tooltip_mouse_wheel));
    Frame_ptr frame = std::make_shared<Frame_impl>(BORDER_SOLID);
    tooltip_->insert(frame);
    frame->insert(tooltip);
    tooltip_->show();
    tooltip_->grab_mouse();
    tooltip_widget_ = caller;
    tooltip_cx_ = Loop_impl::this_loop()->signal_alarm(time_ms).connect(tau::bind(fun(this, &Window_impl::close_tooltip), caller));
    return tooltip_;
}

Window_ptr Window_impl::open_tooltip(Widget_impl * caller, Widget_ptr tooltip) {
    if (!display()) { return nullptr; }
    close_tooltip(tooltip_widget_);
    Point pt = where_mouse();
    Gravity  gravity;
    Rect  cr = display()->cursor_area();
    int   x = pt.x(), y = pt.y(), zx = size().width()/3, zy = size().height()/3;

    if (x < zx) {
        if (y < zy) {
            x += cr.right();
            y += cr.bottom();
            gravity = GRAVITY_TOP_LEFT;
        }

        else if (pt.y() < zy+zy) {
            x += cr.right();
            y += cr.bottom();
            gravity = GRAVITY_LEFT;
        }

        else {
            y += cr.top();
            gravity = GRAVITY_BOTTOM_LEFT;
        }
    }

    else if (x < zx+zx) {
        if (y < zy) {
            y += cr.bottom();
            gravity = GRAVITY_TOP_LEFT;
        }

        else if (pt.y() < zy+zy) {
            gravity = GRAVITY_BOTTOM;
        }

        else {
            y += cr.top();
            gravity = GRAVITY_BOTTOM_LEFT;
        }
    }

    else {
        x += cr.left();

        if (y < zy) {
            gravity = GRAVITY_TOP_RIGHT;
        }

        else if (pt.y() < zy+zy) {
            gravity = GRAVITY_RIGHT;
        }

        else {
            y += cr.top();
            gravity = GRAVITY_BOTTOM_RIGHT;
        }
    }

    return open_tooltip(caller, tooltip, Point(x, y), gravity, 9997);
}

void Window_impl::close_tooltip(Widget_impl * caller) {
    if (caller == tooltip_widget_) {
        if (tooltip_) { tooltip_->close(); }
        tooltip_cx_.drop();
        tooltip_widget_ = nullptr;
    }
}

// Overrides Widget_impl.
void Window_impl::invalidate(const Rect & inval) {
    winface_->invalidate(inval);
}

// Overrides Widget_impl.
Painter Window_impl::painter() {
    return wrap_painter(winface_->painter());
}

void Window_impl::update() {
    sync_arrange();
    winface_->update();
}

// Overrides Container_impl.
Widget_ptr Window_impl::focus_owner() {
    if (parent_) { return Container_impl::focus_owner(); }
    if (auto cp = Container_impl::focus_owner()) { return cp; }
    return display() ? display()->winptr(this) : nullptr;
}

// Overrides Container_impl.
Widget_cptr Window_impl::focus_owner() const {
    if (parent_) { return Container_impl::focus_owner(); }
    if (auto cp = Container_impl::focus_owner()) { return cp; }
    return display() ? display()->winptr(this) : nullptr;
}

} // namespace tau

//END
