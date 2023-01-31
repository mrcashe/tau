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

#ifndef TAU_GCONTEXT_XCB_HH
#define TAU_GCONTEXT_XCB_HH

#include "types-xcb.hh"

namespace tau {

class Context_xcb {
public:

    Context_xcb(Display_xcb_ptr dp, xcb_drawable_t drawable);
    Context_xcb(const Context_xcb & other) = default;
    Context_xcb & operator=(const Context_xcb & other) = default;
   ~Context_xcb();

    xcb_gcontext_t xid() const { return gc_; }
    void set_func(xcb_gx_t func);
    xcb_gx_t func() { return func_; }
    void set_plane_mask(uint32_t pmask);
    void set_foreground(const Color & color);
    Color foreground() const;
    void set_background(const Color & color);
    void set_line_width(uint32_t width);
    void set_line_style(uint32_t lstyle);
    void set_cap_style(uint32_t capstyle);
    void set_join_style(uint32_t jstyle);
    void set_fill_style(uint32_t fstyle);
    void set_fill_rule(uint32_t frule);
    void set_tile(uint32_t tile);
    void set_stipple(uint32_t stipple);
    void set_tile_stipple_origin(const Point & origin);
    void set_subwindow_mode(bool on);
    void set_graphics_exposures(bool on);
    void set_clip_mask(uint32_t mask);
    void set_clip_origin(const Point & origin);
    void set_dash_offset(uint32_t ofs);
    void set_dash_list(uint32_t list);
    void set_arc_mode(uint32_t mode);
    void flush() const;

private:

    Display_xcb_ptr     dp_;
    xcb_gcontext_t      gc_ = 0;
    mutable uint32_t    flags_ = 0;
    xcb_gx_t            func_ = XCB_GX_COPY;
    uint32_t            pmask_ = XCB_NONE;
    uint32_t            fore_ = 0xffffffff;
    uint32_t            back_ = 0;
    uint32_t            linewidth_ = 1;
    uint32_t            lstyle_ = XCB_LINE_STYLE_SOLID;
    uint32_t            capstyle_ = XCB_CAP_STYLE_NOT_LAST;
    uint32_t            jstyle_ = XCB_JOIN_STYLE_MITER;
    uint32_t            fstyle_ = XCB_FILL_STYLE_SOLID;
    uint32_t            frule_ = XCB_FILL_RULE_EVEN_ODD;
    uint32_t            tile_ = 0;
    uint32_t            stipple_ = 0;
    uint32_t            xstipple_ = 0;
    uint32_t            ystipple_ = 0;
    uint32_t            subwindow_ = 0;
    uint32_t            expose_ = 0;
    uint32_t            cx_origin_ = 0;
    uint32_t            cy_origin_ = 0;
    uint32_t            clip_ = 0;
    uint32_t            doffset_ = 0;
    uint32_t            dlist_ = 0;
    uint32_t            arcmode_ = 0;

};

} // namespace tau

#endif // TAU_GCONTEXT_XCB_HH
