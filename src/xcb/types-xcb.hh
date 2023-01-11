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

#ifndef TAU_TYPES_XCB_HH
#define TAU_TYPES_XCB_HH

#include <tau/painter.hh>
#include <types-impl.hh>
#include <xcb/xcb.h>
#include <xcb/xcb_renderutil.h>
#include <xcb/xcb_icccm.h>
#include <xcb/sync.h>
#include <memory>

// Success            0    everything's okay
// BadRequest         1    bad request code
// BadValue           2    int parameter out of range
// BadWindow          3    parameter not a Window
// BadPixmap          4    parameter not a Pixmap
// BadAtom            5    parameter not an Atom
// BadCursor          6    parameter not a Cursor
// BadFont            7    parameter not a Font
// BadMatch           8    parameter mismatch
// BadDrawable        9    parameter not a Pixmap or Window
// BadAccess         10    depending on context:
//                              - key/button already grabbed
//                              - attempt to free an illegal cmap entry
//                              - attempt to store into a read-only color map entry.
//                              - attempt to modify the access control list from other than the local host.
//
// BadAlloc          11    insufficient resources
// BadColor          12    no such colormap
// BadGC             13    parameter not a GC
// BadIDChoice       14    choice not in range or already used
// BadName           15    font or color name doesn't exist
// BadLength         16    Request length incorrect
// BadImplementation 17    server is defective


namespace tau {

enum {
    NET_WM_STATE_MAXIMIZED_VERT = 1 << 0,
    NET_WM_STATE_MAXIMIZED_HORZ = 1 << 1,
    NET_WM_STATE_FULLSCREEN     = 1 << 2,
    NET_WM_STATE_FOCUSED        = 1 << 3,
    NET_WM_STATE_HIDDEN         = 1 << 4
};

#define NET_WM_STATE_REMOVE        0    // remove/unset property
#define NET_WM_STATE_ADD           1    // add/set property
#define NET_WM_STATE_TOGGLE        2    // toggle property

class Cursor_xcb;
using Cursor_xcb_ptr = std::shared_ptr<Cursor_xcb>;
using Cursor_xcb_cptr = std::shared_ptr<const Cursor_xcb>;

class Display_xcb;
using Display_xcb_ptr = std::shared_ptr<Display_xcb>;
using Display_xcb_cptr = std::shared_ptr<const Display_xcb>;

class Pixmap_xcb;
using Pixmap_xcb_ptr = std::shared_ptr<Pixmap_xcb>;
using Pixmap_xcb_cptr = std::shared_ptr<const Pixmap_xcb>;

class Toplevel_xcb;
using Toplevel_xcb_ptr = std::shared_ptr<Toplevel_xcb>;
using Toplevel_xcb_cptr = std::shared_ptr<const Toplevel_xcb>;

class Painter_xcb;
using Painter_xcb_ptr = std::shared_ptr<Painter_xcb>;
using Painter_xcb_cptr = std::shared_ptr<const Painter_xcb>;

class Popup_xcb;
using Popup_xcb_ptr = std::shared_ptr<Popup_xcb>;
using Popup_xcb_cptr = std::shared_ptr<const Popup_xcb>;

class Dialog_xcb;
using Dialog_xcb_ptr = std::shared_ptr<Dialog_xcb>;
using Dialog_xcb_cptr = std::shared_ptr<const Dialog_xcb>;

class Winface_xcb;
using Winface_xcb_ptr = std::shared_ptr<Winface_xcb>;
using Winface_xcb_cptr = std::shared_ptr<const Winface_xcb>;

xcb_render_color_t x11_render_color(const Color & color);
ustring x11_error_msg(int error);
xcb_point_t to_xcb_point(const Point & pt);
xcb_rectangle_t to_xcb_rectangle(const Rect & r);
xcb_gx_t gx_oper(Oper op);
xcb_render_pict_op_t xrender_oper(Oper op);
xcb_line_style_t xcb_line_style(Line_style ls);
xcb_cap_style_t xcb_cap_style(Cap_style cs);
xcb_join_style_t xcb_join_style(Join_style js);

} // namespace tau

#endif // TAU_TYPES_XCB_HH
