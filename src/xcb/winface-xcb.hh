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

#ifndef TAU_WINFACE_XCB_HH
#define TAU_WINFACE_XCB_HH

#include <window-impl.hh>
#include "display-xcb.hh"

namespace tau {

class Winface_xcb: public Winface {
public:

    Winface_xcb(Display_xcb_ptr dp, xcb_window_t pwid);
   ~Winface_xcb();
    void init(Window_impl * wi, const Point & upos, const Size & usize);

    Display_xcb_ptr xdp() { return dp_; }
    Display_xcb_cptr xdp() const { return dp_; }
    xcb_connection_t * conn() { return cx_; }

    xcb_window_t wid() const { return wid_; }
    xcb_window_t pwid() const { return pwid_; }
    xcb_render_picture_t xpicture();
    Window_impl * self() { return self_; }
    const Window_impl * self() const { return self_; }

    bool mapped() const { return mapped_; }
    void hide_taskbar_hint();
    void grab_mouse();

    void handle_expose(xcb_expose_event_t * event);
    void handle_map(xcb_map_notify_event_t * event);
    void handle_unmap(xcb_unmap_notify_event_t * event);
    void handle_configure(xcb_configure_notify_event_t * event);
    void handle_property(xcb_property_notify_event_t * event);
    void handle_client(xcb_client_message_event_t * event);

    // Overrides pure Winface.
    Display_ptr display() override;

    // Overrides pure Winface.
    Display_cptr display() const override;

    // Overrides pure Winface.
    Painter_ptr painter() override;

    // Overrides pure Winface.
    void invalidate(const Rect & inval) override;

    // Overrides pure Winface.
    void update() override;

    // Overrides pure Winface.
    void move(const Point & pt) override;

    // Overrides pure Winface.
    void move(const Rect & r) override;

    // Overrides pure Winface.
    void resize(const Size & size) override;

    // Overrides pure Winface.
    void minimize() override;

    // Overrides pure Winface.
    void maximize() override;

    // Overrides pure Winface.
    void restore() override;

    // Overrides pure Winface.
    void set_fullscreen(bool yes) override;

    // Overrides pure Winface.
    void set_cursor(Cursor_ptr cursor) override;

    // Overrides pure Winface.
    void unset_cursor() override;

    // Overrides pure Winface.
    void show_cursor(bool show) override;

    // Overrides pure Winface.
    void set_title(const ustring & title) override;

    // Overrides pure Winface.
    void set_icon(Pixmap_ptr icon) override;

    // Overrides pure Winface.
    void show_frame(bool show) override;

    // Overrides pure Winface.
    void enable_maximize(bool enable) override;

    // Overrides pure Winface.
    bool maximize_enabled() const override;

    // Overrides pure Winface.
    void enable_minimize(bool enable) override;

    // Overrides pure Winface.
    bool minimize_enabled() const override;

    // Overrides pure Winface.
    bool visible() const override { return mapped_; }

    xcb_sync_int64_t    configure_value_ { 0, 0 };

private:

    struct Motif_hints {
        uint32_t   flags;
        uint32_t   functions;
        uint32_t   decorations;
        int32_t    input_mode;
        uint32_t   status;
    };

    xcb_atom_t atom(const std::string & name) const { return dp_->atom(name); }
    std::string ratom(xcb_atom_t atom) const { return dp_->ratom(atom); }
    void allow_action(const std::string & atom_name, bool enable);
    void on_show();
    void on_hide();
    void on_hints();

    Display_xcb_ptr     dp_;
    Window_impl *       self_ = nullptr;
    Toplevel_impl *     tpl_ = nullptr;
    Popup_impl *        popup_ = nullptr;

    xcb_window_t        wid_ = XCB_NONE;
    xcb_window_t        pwid_ = XCB_NONE;
    Cursor_xcb_ptr      icursor_;
    xcb_connection_t *  cx_ = nullptr;
    bool                hide_taskbar_hint_ = false;
    xcb_render_picture_t xpicture_ = XCB_NONE;
    unsigned            wm_state_ = 0;
    bool                mapped_ = false;
    bool                first_map_ = true;
    bool                grab_mouse_ = false;
    bool                want_fullscreen_ = false;
    bool                want_maximize_ = false;
    bool                want_minimize_ = false;
    xcb_sync_counter_t  sync_counter_ = XCB_NONE;
    xcb_sync_int64_t    sync_value_ { 0, 0 };
    Timer               paint_timer_ { fun(this, &Winface_xcb::update) };
    Rect                inval_;
    Painter_xcb_ptr     pr_;
    Point               upos_;  // User requested position.
    Size                usize_; // User requested size.
    std::vector<xcb_atom_t> allowed_actions_;
};

#define WINFACE_XCB(wip) std::static_pointer_cast<Winface_xcb>(wip->winface())

} // namespace tau

#endif // TAU_WINFACE_XCB_HH
