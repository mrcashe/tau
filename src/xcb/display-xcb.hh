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

#ifndef TAU_DISPLAY_XCB_HH
#define TAU_DISPLAY_XCB_HH

#include "types-xcb.hh"
#include <tau/input.hh>
#include <tau/timeval.hh>
#include <display-impl.hh>
#include <xkbcommon/xkbcommon-x11.h>
#include <xcb/xcb_cursor.h>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>

namespace tau {

class Winface_xcb;

class Display_xcb: public Display_impl {
public:

   ~Display_xcb();

    // Overrides pure Display_impl.
    int depth() const override;

    // Overrides pure Display_impl.
    Point where_mouse() const override;

    // Overrides pure Display_impl.
    Rect cursor_area() const override;

    // Overrides pure Display_impl.
    bool can_paste_text() const override;

    // Overrides pure Display_impl.
    void paste_text() override;

    // Overrides pure Display_impl.
    void copy_text(const ustring & str) override;

    // Allocate an atom.
    xcb_atom_t atom(const std::string & name);

    // Convert atom to atom name (reverse atom).
    std::string ratom(xcb_atom_t atom);

    // Set "NET_WM_STATE" property.
    void set_net_wm_state(xcb_window_t wid, bool add, const std::string & atom1, const std::string & atom2=ustring());

    // Check xcb request.
    int request_check(xcb_void_cookie_t ck);

    // Get root window ID.
    xcb_window_t root() const;

    // Overrides Display_impl.
    void allow_screensaver() override;

    // Overrides Display_impl.
    void disallow_screensaver() override;

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    // Overrides pure Display_impl.
    void grab_mouse(Window_impl * wii) override;

    // Overrides pure Display_impl.
    void ungrab_mouse() override;

    // Overrides pure Display_impl.
    bool grab_modal(Window_impl * wii) override;

    // Overrides pure Display_impl.
    bool end_modal(Window_impl * wii) override;

    // Overrides pure Display_impl.
    void grab_window_focus(Window_impl * wii) override;

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    xcb_visualid_t visualid() const;
    xcb_render_pictformat_t pictformat();
    xcb_render_pictformat_t pictformat(unsigned depth);
    std::vector<ustring> list_xrender_filters(xcb_render_picture_t picture) const;
    xcb_render_picture_t solid_fill(const Color & c);
    void track_mouse_grab(xcb_window_t xid);

    xcb_connection_t * conn() { return cx_; }
    uint16_t xsync_version() const { return xsync_version_; }
    uint16_t xfixes_version() const { return xfixes_version_; }

protected:

    Display_xcb() = default;

    void open(const ustring & args);

    // Overrides pure Display_impl.
    Toplevel_ptr create_toplevel(Display_ptr dp, const Rect & bounds) override;

    // Overrides pure Display_impl.
    Dialog_ptr create_dialog(Window_impl * wii, const Rect & bounds=Rect()) override;

    // Overrides pure Display_impl.
    Popup_ptr create_popup(Display_ptr dp, Widget_impl * wpp, const Point & origin, Gravity gravity) override;

private:

    struct Pict_format {
        uint16_t depth;
        uint16_t red_shift;
        uint16_t red_mask;
        uint16_t green_shift;
        uint16_t green_mask;
        uint16_t blue_shift;
        uint16_t blue_mask;
        uint16_t alpha_shift;
        uint16_t alpha_mask;
    };

    using Atoms = std::map<std::string, xcb_atom_t>;
    using RAtoms = std::map<xcb_atom_t, std::string>;
    using Visual_formats = std::map<xcb_visualid_t, xcb_render_pictformat_t>;
    using Pict_formats = std::map<xcb_render_pictformat_t, Pict_format>;
    using Depth_formats = std::map<unsigned, xcb_render_pictformat_t>;
    using Winmap = std::map<xcb_window_t, Winface_xcb_ptr>;
    using Solid_fills = std::map<uint32_t, uint32_t>;
    using Xcb_events = std::vector<xcb_generic_event_t *>;

    xcb_connection_t *  cx_ = nullptr;
    xcb_screen_t *      scr_ = nullptr;

    xkb_context *       xkbcontext_ = nullptr;
    xkb_keymap *        xkbkeymap_ = nullptr;
    xkb_state *         xkbstate_ = nullptr;

    std::mutex          xcb_mx_;
    Event_ptr           xcb_event_;
    std::atomic_bool    xcb_thread_running_ { false };
    std::thread         xcb_thr_;
    Xcb_events          xcb_events_;

    unsigned            dclick_time_ = 250000;  // Double click timeout in microseconds.
    Timeval             click_ts_;              // Last click timestamp.
    unsigned            last_button_ = MBT_NONE; // Last mouse click button number.
    Widget_impl *       last_clicker_ = nullptr;

    Timeval             idle_ts_;
    uint16_t            xsync_version_ = 0;
    uint16_t            xfixes_version_ = 0;
    Atoms               atoms_;
    RAtoms              ratoms_;
    xcb_visualid_t      visualid_ = XCB_NONE;
    Visual_formats      visual_formats_;
    Pict_formats        pict_formats_;
    Depth_formats       depth_formats_;
    Solid_fills         solid_fills_;
    Winmap              winmap_;
    xcb_window_t        whidden_ = XCB_NONE;
    ustring             copy_;
    xcb_atom_t          utf8_string_atom_;
    xcb_atom_t          targets_atom_;
    xcb_atom_t          clipboard_atom_;
    xcb_atom_t          abcd_atom_;

private:

    void init_xkb();
    void init_xrender();
    void init_xfixes();
    void init_xsync();

    Winface_xcb_ptr find(xcb_window_t xid);
    bool query_pointer(xcb_window_t wid, Point & pt) const;
    xcb_window_t selection_owner();
    Cursor_ptr lookup_cursor(const ustring & name);

    void handle_xcb_event(xcb_generic_event_t * event);
    void handle_kbd(bool press, xcb_key_press_event_t * event);
    void handle_button(bool press, xcb_button_press_event_t * event);
    void handle_client(xcb_client_message_event_t * event);
    void handle_selection_notify(xcb_selection_notify_event_t * event);
    void handle_selection_request(xcb_selection_request_event_t * event);
    void handle_selection_clear(xcb_selection_clear_event_t * event);

    void on_window_close(xcb_window_t wid);

    void on_xcb_event();
    void xcb_thread();
    void on_loop_quit();
};

} // namespace tau

#endif // TAU_DISPLAY_XCB_HH
