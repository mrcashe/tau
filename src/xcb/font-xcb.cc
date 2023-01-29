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

#include <tau/brush.hh>
#include <tau/painter.hh>
#include <tau/string.hh>
#include <pixmap-impl.hh>
#include "font-xcb.hh"
#include <cstring>

namespace tau {

Font_xcb::Font_xcb(Font_face_ptr fface, const ustring & spec, double size_pt, Display_xcb_ptr dp):
    Font_posix(fface, spec, size_pt, dp->dpi()),
    dp_(dp),
    cx_(dp->conn())
{
    east_ = xcb_generate_id(cx_);
    xcb_render_create_glyph_set(cx_, east_, dp_->pictformat(8));
}

Font_xcb::~Font_xcb() {
    xcb_render_free_glyph_set(cx_, east_);
}

void Font_xcb::render_glyphs(const std::u32string & str, Point pt, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst) {
    xcb_render_glyphset_t gs = east_;
    std::u32string & avail = east_chars_;

    std::size_t n = 254*4;
    n = std::min(n, str.size());
    std::size_t ns = n;
    const char32_t * cstr = str.c_str();
    const char32_t * s = cstr;

    uint32_t new_chars[n];
    uint32_t * nnc = new_chars;

    Point pts[n];
    Point * ppts = pts;

    for (; ns; --ns, ++s) {
        *ppts++ = pt;
        auto g = glyph(*s);

        if (g) {
            Vector adv = g->advance();
            pt.translate(std::ceil(adv.x()), std::ceil(adv.y()));

            if (std::u32string::npos == avail.find_first_of(*s)) {
                avail += *s;

                if (Rect r = g->bounds()) {
                    Pixmap_ptr pix = Pixmap_impl::create(8, r.size());

                    if (auto pr = pix->painter()) {
                        pr.move_to(-r.left(), std::ceil(g->max().y()));
                        pr.glyph(Glyph_impl::wrap(g));
                        pr.set_brush(Color("White"));
                        pr.fill();

                        ginfos_.emplace_back();
                        xcb_render_glyphinfo_t & ginfo = ginfos_.back();

                        ginfo.width = pix->size().width();
                        ginfo.height = pix->size().height();

                        ginfo.x = -std::floor(g->bearing().x());
                        ginfo.y = std::floor(g->max().y());

                        ginfo.x_off = std::ceil(adv.x());
                        ginfo.y_off = std::ceil(adv.y());

                        *nnc++ = *s;
                        std::size_t nbits = bits_.size(), nbytes = pix->bytes();
                        bits_.resize(nbits+nbytes);
                        uint8_t * pbits = bits_.data();
                        if (pbits) { std::memcpy(pbits+nbits, pix->raw(), nbytes); }
                    }
                }
            }
        }
    }

    xcb_render_add_glyphs(cx_, gs, ginfos_.size(), new_chars, ginfos_.data(), bits_.size(), bits_.data());
    ginfos_.clear();
    bits_.clear();

    // X11 protocol can accept no more than 1Kb of data per call.
    // It gives maximal glyph string length of 254 characters.
    // The string coordinates set in the buffer in two 16 bit words
    // with offsets of +4 and +6 of header is applying to the entire string.

    for (ppts = pts; n; ) {
        std::size_t nchars = 254;
        nchars = std::min(nchars, n);
        std::size_t nb = 8+4*nchars;
        uint8_t buffer[nb];
        buffer[0] = nchars;
        int16_t * p = reinterpret_cast<int16_t *>(buffer+4);
        *p++ = ppts->x();
        *p = ppts->y();
        std::memcpy(buffer+8, cstr, 4*nchars);
        xcb_render_composite_glyphs_32(cx_, op, src, dst, 0, gs, 0, 0, nb, buffer);
        n -= nchars;
        ppts += nchars;
        cstr += nchars;
    }

    xcb_flush(cx_);
}

} // namespace tau

//END
