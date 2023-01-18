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

#include <tau/display.hh>
#include <posix/font-face-posix.hh>
#include "font-posix.hh"

namespace tau {

Font_posix::Font_posix(Font_face_ptr face, const ustring & spec, double size_pt, unsigned dpi):
    Font_impl(),
    face_(face),
    dpi_(dpi),
    sz_(size_pt)
{
    scale_.set(sz_*dpi/(72*face->upm()), sz_*dpi/(72*face->upm()));
    ascent_ = scale_.y()*face->ascent();
    descent_ = scale_.y()*face->descent();
    linegap_ = scale_.y()*face->linegap();
    Rect bbox = face->bounds();
    min_.set(scale_.x()*bbox.left(), scale_.y()*bbox.top());
    max_.set(scale_.x()*bbox.right(), scale_.y()*bbox.bottom());
    spec_ = font_size_change(spec, sz_);
}

// Overrides pure Font_impl.
ustring Font_posix::psname() const {
    return face_->psname();
}

// Overrides pure Font_impl.
Glyph_ptr Font_posix::glyph(char32_t wc) {
    auto iter = gmap_.find(wc);
    if (iter != gmap_.end()) { return iter->second; }
    auto master = face_->glyph(wc);
    auto gl = master->glyph(scale_);
    gmap_[wc] = gl;
    return gl;
}

} // namespace tau

//END

