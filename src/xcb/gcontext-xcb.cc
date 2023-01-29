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

#include <tau/color.hh>
#include "display-xcb.hh"
#include "gcontext-xcb.hh"

namespace tau {

Context_xcb::Context_xcb(Display_xcb_ptr dp, xcb_drawable_t drw):
    dp_(dp)
{
    gc_ = xcb_generate_id(dp_->conn());
    xcb_create_gc(dp_->conn(), gc_, drw, 0, nullptr);
}

Context_xcb::~Context_xcb() {
    xcb_free_gc(dp_->conn(), gc_);
}

void Context_xcb::set_func(xcb_gx_t func) {
    flags_ |= XCB_GC_FUNCTION;
    func_ = func;
}

void Context_xcb::set_plane_mask(uint32_t pmask) {
    flags_ |= XCB_GC_PLANE_MASK;
    pmask_ = pmask;
}

void Context_xcb::set_foreground(const Color & color) {
    flags_ |= XCB_GC_FOREGROUND;
    fore_ = color.argb32();
}

Color Context_xcb::foreground() const {
    return Color::from_rgb24(fore_);
}

void Context_xcb::set_background(const Color & color) {
    flags_ |= XCB_GC_BACKGROUND;
    back_ = color.argb32();
}

void Context_xcb::set_line_width(uint32_t width) {
    flags_ |= XCB_GC_LINE_WIDTH;
    linewidth_ = width;
}

void Context_xcb::set_line_style(uint32_t lstyle) {
    flags_ |= XCB_GC_LINE_STYLE;
    lstyle_ = lstyle;
}

void Context_xcb::set_cap_style(uint32_t capstyle) {
    flags_ |= XCB_GC_CAP_STYLE;
    capstyle_ = capstyle;
}

void Context_xcb::set_join_style(uint32_t jstyle) {
    flags_ |= XCB_GC_JOIN_STYLE;
    jstyle_ = jstyle;
}

void Context_xcb::set_fill_style(uint32_t fstyle) {
    flags_ |= XCB_GC_FILL_STYLE;
    fstyle_ = fstyle;
}

void Context_xcb::set_fill_rule(uint32_t frule) {
    flags_ |= XCB_GC_FILL_RULE;
    frule_ = frule;
}

void Context_xcb::set_tile(uint32_t tile) {
    flags_ |= XCB_GC_TILE;
    tile_ = tile;
}

void Context_xcb::set_stipple(uint32_t stipple) {
    flags_ |= XCB_GC_STIPPLE;
    stipple_ = stipple;
}

void Context_xcb::set_tile_stipple_origin(const Point & origin) {
    xstipple_ = static_cast<uint32_t>(origin.x());
    ystipple_ = static_cast<uint32_t>(origin.y());
    flags_ |= (XCB_GC_TILE_STIPPLE_ORIGIN_X|XCB_GC_TILE_STIPPLE_ORIGIN_Y);
}

void Context_xcb::set_subwindow_mode(bool on) {
    flags_ |= XCB_GC_SUBWINDOW_MODE;
    subwindow_ = on;
}

void Context_xcb::set_graphics_exposures(bool on) {
    flags_ |= XCB_GC_GRAPHICS_EXPOSURES;
    expose_ = on;
}

void Context_xcb::set_clip_mask(uint32_t mask) {
    flags_ |= XCB_GC_CLIP_MASK;
    clip_ = mask;
}

void Context_xcb::set_clip_origin(const Point & origin) {
    flags_ |= (XCB_GC_CLIP_ORIGIN_X|XCB_GC_CLIP_ORIGIN_Y);
    cx_origin_ = static_cast<uint32_t>(origin.x());
    cy_origin_ = static_cast<uint32_t>(origin.y());
}

void Context_xcb::set_dash_offset(uint32_t ofs) {
    flags_ |= XCB_GC_DASH_OFFSET;
    doffset_ = ofs;
}

void Context_xcb::set_dash_list(uint32_t list) {
    flags_ |= XCB_GC_DASH_LIST;
    dlist_ = list;
}

void Context_xcb::set_arc_mode(uint32_t mode) {
    flags_ |= XCB_GC_ARC_MODE;
    arcmode_ = mode;
}

void Context_xcb::flush() {
    if (0 != flags_) {
        uint32_t v[24];
        uint32_t cnt = 0;

        if (flags_ & XCB_GC_FUNCTION) { v[cnt++] = func_; }
        if (flags_ & XCB_GC_PLANE_MASK) { v[cnt++] = pmask_; }
        if (flags_ & XCB_GC_FOREGROUND) { v[cnt++] = fore_; }
        if (flags_ & XCB_GC_BACKGROUND) { v[cnt++] = back_; }
        if (flags_ & XCB_GC_LINE_WIDTH) { v[cnt++] = linewidth_; }
        if (flags_ & XCB_GC_LINE_STYLE) { v[cnt++] = lstyle_; }
        if (flags_ & XCB_GC_CAP_STYLE) { v[cnt++] = capstyle_; }
        if (flags_ & XCB_GC_JOIN_STYLE) { v[cnt++] = jstyle_; }
        if (flags_ & XCB_GC_FILL_STYLE) { v[cnt++] = fstyle_; }
        if (flags_ & XCB_GC_FILL_RULE) { v[cnt++] = frule_; }
        if (flags_ & XCB_GC_TILE) { v[cnt++] = tile_; }
        if (flags_ & XCB_GC_STIPPLE) { v[cnt++] = stipple_; }
        if (flags_ & XCB_GC_TILE_STIPPLE_ORIGIN_X) { v[cnt++] = xstipple_; }
        if (flags_ & XCB_GC_TILE_STIPPLE_ORIGIN_Y) { v[cnt++] = ystipple_; }
        if (flags_ & XCB_GC_SUBWINDOW_MODE) { v[cnt++] = subwindow_; }
        if (flags_ & XCB_GC_GRAPHICS_EXPOSURES) { v[cnt++] = expose_; }
        if (flags_ & XCB_GC_CLIP_ORIGIN_X) { v[cnt++] = cx_origin_; }
        if (flags_ & XCB_GC_CLIP_ORIGIN_Y) { v[cnt++] = cy_origin_; }
        if (flags_ & XCB_GC_CLIP_MASK) { v[cnt++] = clip_; }
        if (flags_ & XCB_GC_DASH_OFFSET) { v[cnt++] = doffset_; }
        if (flags_ & XCB_GC_DASH_LIST) { v[cnt++] = dlist_; }
        if (flags_ & XCB_GC_ARC_MODE) { v[cnt++] = arcmode_; }
        xcb_change_gc(dp_->conn(), gc_, flags_, v);
        flags_ = 0;
    }
}

} // namespace tau

//END
