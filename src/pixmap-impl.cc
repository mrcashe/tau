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

#include <tau/color.hh>
#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/string.hh>
#include <pixmap-impl.hh>

namespace tau {

void Pixmap_impl::copy(Pixmap_cptr other) {
    int iwidth = other->size().width(), iheight = other->size().height();

    for (int y = 0; y < iheight; ++y) {
        for (int x = 0; x < iwidth; ++x) {
            put_pixel(x, y, other->get_pixel(Point(x, y)));
        }
    }
}

void Pixmap_impl::put_pixel(const Point & pt, const Color & c) {
    Rect r(pt, Size(1));
    fill_rectangles(&r, 1, c);
}

// static
Pixmap_ptr Pixmap_impl::dup(Pixmap_cptr other) {
    Pixmap_ptr pix = create(other->depth(), other->size());
    pix->copy(other);
    return pix;
}

// static
Pixmap_ptr Pixmap_impl::load_from_file(const ustring & path) {
    ustring suf = str_tolower(path_suffix(path));

    if ("xpm" == suf) {
        return Pixmap_impl::load_xpm_from_file(path);
    }

    if ("bmp" == suf) {
        return Pixmap_impl::load_bmp_from_file(path);
    }

    if ("png" == suf) {
        return Pixmap_impl::load_png_from_file(path);
    }

    if ("ico" == suf) {
        return Pixmap_impl::load_ico_from_file(path);
    }

    throw bad_pixmap(path);
}

} // namespace tau

//END
