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

#ifndef TAU_XCB_FONT_HH
#define TAU_XCB_FONT_HH

#include <glyph-impl.hh>
#include <posix/font-posix.hh>
#include "display-xcb.hh"

namespace tau {

class Font_xcb: public Font_posix {
public:

    Font_xcb(Font_face_ptr fface, double size_pt, Display_xcb_ptr dp);
   ~Font_xcb();

    void render_glyphs(const std::u32string & str, Point pt, uint8_t oper, xcb_render_picture_t src, xcb_render_picture_t dst);

private:

    Display_xcb_ptr         dp_;
    xcb_connection_t *      cx_;
    xcb_render_glyphset_t   east_ = XCB_NONE;
    std::u32string          east_chars_;
    std::vector<uint8_t>    bits_;
    std::vector<xcb_render_glyphinfo_t> ginfos_;
};

} // namespace tau

#endif // TAU_XCB_FONT_HH
