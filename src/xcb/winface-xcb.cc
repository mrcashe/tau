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
#include <tau/loop.hh>
#include <tau/timeval.hh>
#include <theme-impl.hh>
#include <toplevel-impl.hh>
#include <popup-impl.hh>
#include "cursor-xcb.hh"
#include "painter-xcb.hh"
#include "pixmap-xcb.hh"
#include "winface-xcb.hh"
#include <unistd.h>
#include <xcb/xfixes.h>
#include <climits>
#include <cstring>
#include <iostream>

namespace tau {

Winface_xcb::Winface_xcb(Display_xcb_ptr dp, xcb_window_t pwid):
    Winface(),
    dp_(dp),
    pwid_(pwid),
    cx_(dp->conn())
{
    wid_ = xcb_generate_id(cx_);
    if (XCB_NONE == wid_) { throw graphics_error("Winface_xcb: xcb_generate_id(): failed to generate Window ID"); }
}

void Winface_xcb::init(Window_impl * wii, const Point & upos, const Size & usize) {
    self_ = wii;
    tpl_ = dynamic_cast<Toplevel_impl *>(wii);
    popup_ = dynamic_cast<Popup_impl *>(wii);
    upos_ = upos;
    usize_ = usize;

    uint32_t vals[1];
    vals[0] = XCB_EVENT_MASK_EXPOSURE|
        XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_KEY_RELEASE|
        XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|
        XCB_EVENT_MASK_ENTER_WINDOW|XCB_EVENT_MASK_LEAVE_WINDOW|
        XCB_EVENT_MASK_POINTER_MOTION|
        XCB_EVENT_MASK_VISIBILITY_CHANGE|
        XCB_EVENT_MASK_FOCUS_CHANGE|
        XCB_EVENT_MASK_STRUCTURE_NOTIFY|
        XCB_EVENT_MASK_PROPERTY_CHANGE;

    auto ck = xcb_create_window_checked(cx_, XCB_COPY_FROM_PARENT, wid_, pwid_,
                                        upos.x(), upos.y(), usize.width(), usize.height(),
                                        0, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, XCB_CW_EVENT_MASK, vals);

    int err = dp_->request_check(ck);
    if (0 != err) { throw graphics_error("Winface_xcb: xcb_create_window_checked(): failed to create window"); }

    if (tpl_) {
        char hostname[256];
        gethostname(hostname, 255);
        hostname[255] = '\0';
        xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, XCB_ATOM_WM_CLIENT_MACHINE, XCB_ATOM_STRING, 8, std::strlen(hostname), hostname);
        uint32_t pid = getpid();
        xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, atom("_NET_WM_PID"), XCB_ATOM_CARDINAL, 32, 1, &pid);

        xcb_atom_t atoms[3];
        atoms[0] = atom("WM_DELETE_WINDOW");
        atoms[1] = atom("WM_TAKE_FOCUS");
        atoms[2] = atom("_NET_WM_PING");
        xcb_icccm_set_wm_protocols(cx_, wid_, atom("WM_PROTOCOLS"), 3, atoms);

        if (0 != dp_->xsync_version()) {
            sync_counter_ = xcb_generate_id(cx_);
            xcb_sync_create_counter(cx_, sync_counter_, sync_value_);
            xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, atom("_NET_WM_SYNC_REQUEST_COUNTER"), XCB_ATOM_CARDINAL, 32, 1, &sync_counter_);
        }

        tpl_->signal_hints_changed().connect(fun(this, &Winface_xcb::on_hints));
    }

    self_->signal_hide().connect(fun(this, &Winface_xcb::on_hide));
    self_->signal_show().connect(fun(this, &Winface_xcb::on_show));

    if (auto cursor = Theme_impl::root()->find_cursor("left_ptr:arrow:default", 24)) {
        icursor_ = std::dynamic_pointer_cast<Cursor_xcb>(cursor);
    }

    xcb_flush(cx_);
}

Winface_xcb::~Winface_xcb() {
    if (XCB_NONE != sync_counter_) { xcb_sync_destroy_counter(cx_, sync_counter_); }
    xcb_destroy_window(cx_, wid_);
    xcb_flush(cx_);
}

// Overrides pure Winface.
Display_ptr Winface_xcb::display() {
    return dp_->loop()->alive() ? dp_ : nullptr;
}

// Overrides pure Winface.
Display_cptr Winface_xcb::display() const {
    return dp_->loop()->alive() ? dp_ : nullptr;
}

Painter_ptr Winface_xcb::painter() {
    return std::make_shared<Painter_xcb>(this);
}

xcb_render_picture_t Winface_xcb::xpicture() {
    xcb_render_picture_t  xid = xpicture_;

    if (XCB_NONE == xid) {
        xid = xcb_generate_id(cx_);
        const uint32_t v[1] = { 0 };
        xcb_render_create_picture(cx_, xid, wid_, dp_->pictformat(), 1, v);
        xpicture_  = xid;
    }

    return xid;
}

void Winface_xcb::hide_taskbar_hint() {
    hide_taskbar_hint_ = true;
    dp_->set_net_wm_state(wid_, true, "_NET_WM_STATE_SKIP_TASKBAR", "_NET_WM_STATE_SKIP_PAGER");
}

void Winface_xcb::grab_mouse() {
    if (mapped_) {
        uint16_t mask = XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|
            XCB_EVENT_MASK_ENTER_WINDOW|XCB_EVENT_MASK_LEAVE_WINDOW|
            XCB_EVENT_MASK_POINTER_MOTION;

        auto ck = xcb_grab_pointer(cx_, true, wid_, mask, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
                                   XCB_NONE, XCB_NONE, XCB_CURRENT_TIME);

        xcb_generic_error_t * err;
        if (auto reply = xcb_grab_pointer_reply(cx_, ck, &err)) {
            if (0 != reply->status) { std::cerr << "** Winface_xcb: grab_pointer failed, status is " << int(reply->status) << std::endl; }
            std::free(reply);
        }

        if (err) { std::free(err); }
    }

    else {
        grab_mouse_ = true;
    }
}

void Winface_xcb::invalidate(const Rect & r) {
    if (r) {
        for (Rect & inval: invals_) {
            if (!inval) { inval = r; goto start; }
            if (inval & r) { inval |= r; goto start; }
        }

        invals_.front() |= r;
    start:
        paint_timer_.start(33);
    }
}

void Winface_xcb::update() {
    paint_timer_.stop();

    if (self_->visible()) {
        if (!pr_) { pr_ = std::make_shared<Painter_xcb>(this); pr_->reserve_stack(16); }
        pr_->capture(self_);
        Painter pr(self_->wrap_painter(pr_));

        for (Rect inval: invals_) {
            if (!inval) { break; }
            pr_->set_obscured_area(inval);
            self_->handle_backpaint(pr, inval);
            self_->handle_paint(pr, inval);
        }

        pr_->wreset();
        invals_.fill(Rect());
    }
}

void Winface_xcb::handle_expose(xcb_expose_event_t * event) {
    invalidate(Rect(event->x, event->y, Size(1+event->width, 1+event->height)));
    update();
}

void Winface_xcb::on_show() {
    xcb_map_window(cx_, wid_);
    xcb_flush(cx_);
}

void Winface_xcb::on_hide() {
    xcb_unmap_window(cx_, wid_);
    xcb_flush(cx_);
}

void Winface_xcb::handle_map(xcb_map_notify_event_t * event) {
    mapped_ = true;

    if (first_map_) {
        first_map_ = false;
        move(Rect(upos_, usize_));
        if (hide_taskbar_hint_) { hide_taskbar_hint(); }
        self_->handle_display();
    }

    Cursor_ptr cursor = self_->cursor();
    if (!cursor) { cursor = icursor_; }
    if (cursor) { set_cursor(cursor); }

    if (self_ == dp_->modal_window()) {
        dp_->set_net_wm_state(wid_, true, "_NET_WM_STATE_MODAL");
    }

    if (tpl_) {
        if (want_fullscreen_) {
            tpl_->fullscreen();
        }

        else if (want_maximize_) {
            tpl_->maximize();
        }

        else if (want_minimize_) {
            tpl_->minimize();
        }
    }

    if (grab_mouse_) {
        grab_mouse_ = false;
        grab_mouse();
    }

    self_->handle_visible(true);
}

void Winface_xcb::handle_unmap(xcb_unmap_notify_event_t * event) {
    mapped_ = false;
    self_->handle_visible(false);
}

void Winface_xcb::handle_configure(xcb_configure_notify_event_t * event) {
    Size size(event->width, event->height);
    Point pt(event->x, event->y);
    self_->update_size(size);

    if (XCB_NONE != sync_counter_) {
        if (configure_value_.lo != sync_value_.lo || configure_value_.hi != sync_value_.hi) {
            sync_value_ = configure_value_;
            xcb_sync_set_counter(cx_, sync_counter_, sync_value_);
        }
    }

    // TODO Filtering out events with both x && y == 0.
    // That situation happens when window is resizing using mouse.
    if (!(0 == event->x && 0 == event->y)) {
        self_->update_position(pt-self_->client_area().origin());
    }
}

void Winface_xcb::handle_property(xcb_property_notify_event_t * event) {
    std::string prop_atom = ratom(event->atom);

    if ("_NET_WM_STATE" == prop_atom) {
        //std::cout << "Winface_xcb: _NET_WM_STATE\n";
        xcb_get_property_cookie_t ck = xcb_get_property(cx_, 0, wid_, event->atom, XCB_ATOM_ATOM, 0, UINT_MAX);

        // typedef struct xcb_get_property_reply_t {
        //    uint8_t    response_type;
        //    uint8_t    format;
        //    uint16_t   sequence;
        //    uint32_t   length;
        //    xcb_atom_t type;
        //    uint32_t   bytes_after;
        //    uint32_t   value_len;
        //    uint8_t    pad0[12];
        // } xcb_get_property_reply_t;

        if (xcb_get_property_reply_t * reply = xcb_get_property_reply(cx_, ck, nullptr)) {
            if (XCB_ATOM_ATOM == reply->type) {
                if (xcb_atom_t * patom = reinterpret_cast<xcb_atom_t *>(xcb_get_property_value(reply))) {
                    unsigned wm_state = 0;

                    for (unsigned n_atoms = xcb_get_property_value_length(reply)/4; n_atoms; --n_atoms, ++patom) {
                        std::string atom = ratom(*patom);

                        if ("_NET_WM_STATE_MAXIMIZED_VERT" == atom) {
                            wm_state |= NET_WM_STATE_MAXIMIZED_VERT;
                        }

                        else if ("_NET_WM_STATE_MAXIMIZED_HORZ" == atom) {
                            wm_state |= NET_WM_STATE_MAXIMIZED_HORZ;
                        }

                        else if ("_NET_WM_STATE_FULLSCREEN" == atom) {
                            wm_state |= NET_WM_STATE_FULLSCREEN;
                        }

                        else if ("_NET_WM_STATE_FOCUSED" == atom) {
                            wm_state |= NET_WM_STATE_FOCUSED;
                        }

                        else if ("_NET_WM_STATE_HIDDEN" == atom) {
                            wm_state |= NET_WM_STATE_HIDDEN;
                        }
                    }

                    if (wm_state != wm_state_) {
                        bool minimized = NET_WM_STATE_HIDDEN & wm_state;
                        bool maximized = (NET_WM_STATE_MAXIMIZED_VERT & wm_state) && (NET_WM_STATE_MAXIMIZED_HORZ & wm_state);

                        if (tpl_) {
                            tpl_->handle_fullscreen(NET_WM_STATE_FULLSCREEN & wm_state);
                            if (minimized) { tpl_->handle_minimize(); }
                            else if (maximized) { tpl_->handle_maximize(); }
                            else { tpl_->handle_restore(); }
                        }

                        wm_state_ = wm_state;
                    }
                }
            }

            std::free(reply);
        }
    }

    else if ("_NET_FRAME_EXTENTS" == prop_atom) {
        xcb_get_property_cookie_t ck = xcb_get_property(cx_, 0, wid_, event->atom, XCB_ATOM_CARDINAL, 0, 4);

        if (xcb_get_property_reply_t * reply = xcb_get_property_reply(cx_, ck, nullptr)) {
            if (16 <= xcb_get_property_value_length(reply)) {
                if (uint32_t * p = reinterpret_cast<uint32_t *>(xcb_get_property_value(reply))) {
                    self_->handle_client_area(Rect(Point(p[0], p[2]), self_->size()));
                }
            }

            std::free(reply);
        }
    }

    else if ("_NET_WM_ALLOWED_ACTIONS" == prop_atom) {
        xcb_get_property_cookie_t ck = xcb_get_property(cx_, 0, wid_, event->atom, XCB_ATOM_ATOM, 0, UINT_MAX);

        if (xcb_get_property_reply_t * reply = xcb_get_property_reply(cx_, ck, nullptr)) {
            if (XCB_ATOM_ATOM == reply->type) {
                allowed_actions_.clear();
                uint32_t len = xcb_get_property_value_length(reply);
                xcb_atom_t * p = reinterpret_cast<xcb_atom_t *>(xcb_get_property_value(reply));

                while (len) {
                    //std::cout << prop_atom << ": " << ratom(*p) << "\n";
                    allowed_actions_.push_back(*p++);
                    len -= 4;
                }
            }
        }
    }

    else {
        //std::cerr << "!! Winface_xcb: unhandled property notify: " << prop_atom << " "  << std::endl;
    }
}


void Winface_xcb::handle_client(xcb_client_message_event_t * event) {
    ustring atom = ratom(event->data.data32[0]);

    // Message from [x] button.
    if ("WM_DELETE_WINDOW" == atom) {
        //std::cout << "Winface_xcb: WM_DELETE_WINDOW\n";
        if (tpl_) { tpl_->handle_close(); }
    }

    else if ("WM_TAKE_FOCUS" == atom) {
        //std::cout << "Winface_xcb: WM_TAKE_FOCUS\n";
        xcb_set_input_focus(cx_, XCB_INPUT_FOCUS_PARENT, wid_, event->data.data32[1]);
        xcb_flush(cx_);
    }

    else if ("_NET_WM_SYNC_REQUEST" == atom) {
        //std::cout << "Winface_xcb: _NET_WM_SYNC_REQUEST\n";
        xcb_sync_int64_t cnt;
        cnt.lo = event->data.data32[2];
        cnt.hi = event->data.data32[3];
        configure_value_ = cnt;
    }

    else {
        //std::cerr << "!! Winface_xcb: unhandled WM_PROTOCOLS message " << atom << std::endl;
    }
}

// Overrides pure Winface.
void Winface_xcb::move(const Point & pt) {
    uint32_t v[2];
    v[0] = pt.x();
    v[1] = pt.y();
    xcb_configure_window(cx_, wid_, XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y, v);
    xcb_flush(cx_);
}

// Overrides pure Winface.
void Winface_xcb::move(const Rect & r) {
    uint32_t v[4];
    v[0] = r.left();
    v[1] = r.top();
    v[2] = r.width();
    v[3] = r.height();
    xcb_configure_window(cx_, wid_, XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT, v);
    xcb_flush(cx_);
}

// Overrides pure Winface.
void Winface_xcb::resize(const Size & size) {
    uint32_t values[2];
    values[0] = size.width();
    values[1] = size.height();
    xcb_configure_window(cx_, wid_, XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT, values);
    xcb_flush(cx_);
}

// Overrides pure Winface.
// XCB_ICCCM_WM_STATE_WITHDRAWN = 0,
// XCB_ICCCM_WM_STATE_NORMAL = 1,
// XCB_ICCCM_WM_STATE_ICONIC = 3
// Overrides pure Display_impl.
void Winface_xcb::minimize() {
    want_fullscreen_ = false;
    want_minimize_ = true;
    want_maximize_ = false;

    if (mapped_) {
        dp_->set_net_wm_state(wid_, true, "_NET_WM_STATE_HIDDEN");
        xcb_client_message_event_t ev;
        ev.response_type = XCB_CLIENT_MESSAGE;
        ev.window = wid_;
        ev.sequence = 0;
        ev.format = 32;
        ev.type = atom("WM_CHANGE_STATE");
        ev.data.data32[0] = XCB_ICCCM_WM_STATE_ICONIC;
        ev.data.data32[1] = 0;
        ev.data.data32[2] = 0;
        ev.data.data32[3] = 0;
        ev.data.data32[4] = 0;
        xcb_send_event(cx_, false, dp_->root(), XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (const char *)&ev);
        xcb_flush(cx_);
    }
}

// Overrides pure Winface.
void Winface_xcb::maximize() {
    want_fullscreen_ = false;
    want_minimize_ = false;
    want_maximize_ = true;

    if (mapped_) {
        dp_->set_net_wm_state(wid_, true, "_NET_WM_STATE_MAXIMIZED_VERT", "_NET_WM_STATE_MAXIMIZED_HORZ");
        xcb_flush(cx_);
    }
}

// Overrides pure Winface.
void Winface_xcb::restore() {
    want_fullscreen_ = false;
    want_minimize_ = false;
    want_maximize_ = false;

    if (mapped_) {
        dp_->set_net_wm_state(wid_, false, "_NET_WM_STATE_MAXIMIZED_VERT", "_NET_WM_STATE_MAXIMIZED_HORZ");
        xcb_flush(cx_);
    }

    else {
        self_->show();
    }
}

// Overrides pure Winface.
void Winface_xcb::set_fullscreen(bool yes) {
    want_fullscreen_ = yes;
    want_minimize_ = false;
    want_maximize_ = false;

    if (mapped_) {
        dp_->set_net_wm_state(wid_, yes, "_NET_WM_STATE_FULLSCREEN");
        xcb_flush(cx_);
    }
}

// Overrides pure Winface.
void Winface_xcb::set_cursor(Cursor_ptr cursor) {
    if (cursor) {
        if (auto c = std::dynamic_pointer_cast<Cursor_xcb>(cursor)) {
            xcb_cursor_t cid = c->xid();
            if (XCB_NONE == cid) { cid = c->upload(dp_.get(), wid_); }
            if (XCB_NONE != cid) { xcb_change_window_attributes(cx_, wid_, XCB_CW_CURSOR, &cid); }
            xcb_flush(cx_);
        }
    }
}

// Overrides pure Winface.
void Winface_xcb::unset_cursor() {
    uint32_t cid = XCB_NONE;
    if (icursor_) { cid = icursor_->xid(); }
    xcb_change_window_attributes(cx_, wid_, XCB_CW_CURSOR, &cid);
    xcb_flush(cx_);
}

// Overrides pure Winface.
void Winface_xcb::show_cursor(bool show) {
    if (0 != dp_->xfixes_version()) {
        if (show) { xcb_xfixes_show_cursor(cx_, wid_); }
        else { xcb_xfixes_hide_cursor(cx_, wid_); }
        xcb_flush(cx_);
    }
}

// Overrides pure Winface.
void Winface_xcb::set_title(const ustring & title) {
    xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, atom("_NET_WM_NAME"), atom("UTF8_STRING"), 8, title.bytes(), title.c_str());
    xcb_flush(cx_);
}

// Overrides pure Winface.
void Winface_xcb::set_icon(Pixmap_ptr icon) {
    if (icon) {
        Size sz = icon->size();

        if (sz) {
            if (sz.width() <= 128 && sz.height() <= 128) {
                std::size_t nwords = 2+sz.width()*sz.height();
                uint32_t v[nwords];
                v[0] = sz.width();
                v[1] = sz.height();
                std::size_t i = 2;

                for (std::size_t j = 0; j < sz.height(); ++j) {
                    for (std::size_t k = 0; k < sz.width(); ++k) {
                        v[i++] = icon->get_pixel(Point(k, j)).argb32();
                    }
                }

                xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, atom("_NET_WM_ICON"), XCB_ATOM_CARDINAL, 32, nwords, v);
                xcb_flush(cx_);
            }
        }
    }
}

// Overrides pure Winface.
void Winface_xcb::show_frame(bool show) {
    Motif_hints hints;
    hints.flags = 2;
    hints.functions = 0;
    hints.decorations = show;
    hints.input_mode = 0;
    hints.status = 0;
    xcb_atom_t atm = atom("_MOTIF_WM_HINTS");
    xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, atm, atm, 32, 5, &hints);
}

void Winface_xcb::allow_action(const std::string & atom_name, bool enable) {
    xcb_atom_t atm = atom(atom_name);
    auto i = std::find(allowed_actions_.begin(), allowed_actions_.end(), atm);

    if (enable) {
        if (allowed_actions_.end() == i) {
            allowed_actions_.push_back(atm);
        }
    }

    else {
        if (allowed_actions_.end() != i) {
            allowed_actions_.erase(i);
        }
    }

    xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, wid_, atom("_NET_WM_ALLOWED_ACTIONS"), XCB_ATOM_ATOM, 32, allowed_actions_.size(), allowed_actions_.data());
}

// Overrides pure Winface.
void Winface_xcb::enable_maximize(bool enable) {
    allow_action("_NET_WM_ACTION_MAXIMIZE_VERT", enable);
    allow_action("_NET_WM_ACTION_MAXIMIZE_HORZ", enable);
}

// Overrides pure Winface.
bool Winface_xcb::maximize_enabled() const {
    return allowed_actions_.end() != std::find(allowed_actions_.begin(), allowed_actions_.end(), atom("_NET_WM_ACTION_MAXIMIZE_VERT"));
}

// Overrides pure Winface.
void Winface_xcb::enable_minimize(bool enable) {
    allow_action("_NET_WM_ACTION_MINIMIZE", enable);
}

// Overrides pure Winface.
bool Winface_xcb::minimize_enabled() const {
    return allowed_actions_.end() != std::find(allowed_actions_.begin(), allowed_actions_.end(), atom("_NET_WM_ACTION_MINIMIZE"));
}

void Winface_xcb::on_hints() {
    static const unsigned lim = 16384;
    Size min = tpl_->min_size_hint(), max = tpl_->max_size_hint();
    unsigned wmin = min.width(), wmax = std::min(max.width(), lim), hmin = min.height(), hmax = std::min(max.height(), lim);

    if (wmin) {
        if (wmax) { wmin = std::min(wmin, wmax); }
        else { wmax = lim; }
    }

    if (hmin) {
        if (hmax) { hmin = std::min(hmin, hmax); }
        else { hmax = lim; }
    }

    xcb_size_hints_t hints;
    std::memset(&hints, 0, sizeof hints);
    xcb_icccm_size_hints_set_min_size(&hints, wmin, hmin);
    xcb_icccm_size_hints_set_max_size(&hints, wmax, hmax);
    xcb_icccm_set_wm_size_hints(cx_, wid_, atom("WM_NORMAL_HINTS"), &hints);
}

} // namespace tau

//END
