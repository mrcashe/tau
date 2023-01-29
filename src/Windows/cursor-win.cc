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

#include "cursor-win.hh"
#include "display-win.hh"
#include "pixmap-win.hh"
#include <cstring>
#include <iostream>

namespace {

inline char * s16(char * p, uint16_t w) {
    *p++ = w;
    *p++ = w >> 8;
    return p;
}

inline char * s32(char * p, uint32_t w) {
    *p++ = w;
    *p++ = w >> 8;
    *p++ = w >> 16;
    *p++ = w >> 24;
    return p;
}

} // anonymous namespace

namespace tau {

Cursor_win::Cursor_win():
    Cursor_impl()
{
}

Cursor_win::Cursor_win(HCURSOR hc):
    Cursor_impl(),
    hc_(hc)
{
}

Cursor_win::~Cursor_win() {
    if (hc_) {
        DestroyCursor(hc_);
    }
}

HCURSOR Cursor_win::handle() {
    if (!hc_) { create_handle(); }
    return hc_;
}

HCURSOR Cursor_win::create_handle() {
    if (!frames_.empty()) {
        std::size_t n_frames = frames_.size();

        if (1 == n_frames) {
            auto & cur = frames_[0];

            if (auto pix = std::dynamic_pointer_cast<Pixmap_win>(cur.pix)) {
                Size sz = pix->size();

                if (sz) {
                    auto mask = new Pixmap_win(1, sz);

                    for (std::size_t y = 0; y < sz.height(); ++y) {
                        for (std::size_t x = 0; x < sz.width(); ++x) {
                            Color c = pix->get_pixel(Point(x, y));
                            mask->put_pixel(x, y, c);
                        }
                    }

                    if (HDC dc = GetDC(NULL)) {
                        if (HBITMAP hpix = pix->create_bitmap(dc)) {
                            if (HBITMAP hmask = mask->create_bitmap(dc)) {
                                ICONINFO ii;
                                ii.fIcon = false;
                                ii.xHotspot = cur.hotspot.x();
                                ii.yHotspot = cur.hotspot.y();
                                ii.hbmColor = hpix;
                                ii.hbmMask = hmask;
                                hc_ = CreateIconIndirect(&ii);
                                DeleteObject(hmask);
                            }

                            DeleteObject(hpix);
                        }

                        ReleaseDC(NULL, dc);
                    }

                    delete mask;
                }
            }
        }

        else {
            // header[12]+LIST[8]+anih[44]+rate[8] are constant.
            // fram[4] -> list.
            // Each pixmap: rate[4]+icon[8]+4*W*H, +CUR_header[22]+BMP_header[40].
            std::size_t bytes = 12+8+44+8+4*n_frames, list_bytes = 4+(8+22+40)*n_frames;

            for (std::size_t n = 0; n < n_frames; ++n) {
                Size sz = frames_[n].pix->size();
                list_bytes += 4*sz.width()*sz.height();
                std::size_t mask_stride = ~3 & (sz.width()+31)/8;
                list_bytes += mask_stride*sz.height();
            }

            bytes += list_bytes;

            // Create image.
            auto v = std::make_unique<char[]>(bytes);
            char * p = v.get();

            // Fill header.
            std::memcpy(p, "RIFF", 4); p = s32(p+4, bytes-8);
            std::memcpy(p, "ACON", 4); p += 4;

            // anih.
            std::memcpy(p, "anih", 4); p = s32(p+4, 36);
            s32(p+0, 36);               // size of structure.
            s32(p+4, n_frames);         // frame count.
            s32(p+8, n_frames);         // n_steps.
            s32(p+12, 0);               // width.
            s32(p+16, 0);               // height.
            s32(p+20, 0);               // n_bits.
            s32(p+24, 1);               // n_planes.
            s32(p+28, 1);               // rate.
            p = s32(p+32, 0x00000001);  // flags.

            // rate.
            std::memcpy(p, "rate", 4); p = s32(p+4, 4*n_frames);
            for (std::size_t n = 0; n < n_frames; ++n) { p = s32(p, frames_[n].delay/33); }

            // LIST.
            std::memcpy(p, "LIST", 4); p = s32(p+4, list_bytes);
            std::memcpy(p, "fram", 4); p += 4;

            for (std::size_t n = 0; n < n_frames; ++n) {
                Pixmap_ptr pix = frames_[n].pix;
                Size sz = pix->size();
                int iwidth = sz.width(), iheight = sz.height();
                std::size_t mask_stride = ~3 & (31+sz.width())/8, mask_bytes = sz.height()*mask_stride;
                std::size_t pix_bytes = 4*sz.width()*sz.height();
                std::memcpy(p, "icon", 4); p = s32(p+4, 22+40+pix_bytes+mask_bytes);

                // CUR header.
                s16(p, 0);              // reserved, must be 0.
                s16(p+2, 2);            // 1 - icon, 2 - cursor.
                p = s16(p+4, 1);        // image count.

                // CUR info dir.
                *p++ = sz.width();      // width.
                *p++ = sz.height();     // height.
                *p++ = 0;               // ncolors, must be 0.
                *p++ = 0;               // reserved, must be 0.
                p = s16(p, frames_[n].hotspot.x());
                p = s16(p, frames_[n].hotspot.y());
                p = s32(p, 40+pix_bytes+mask_bytes); // image size in bytes.
                p = s32(p, 22);         // absolute offset in bytes.

                // BMP header.
                s32(p, 40);             // structure size.
                s32(p+4, iwidth);       // width in pixels.
                s32(p+8, 2*iheight);    // height in pixels.
                s16(p+12, 1);           // planes, must be 1.
                s16(p+14, 32);          // color depth.
                s32(p+16, 0);           // compression.
                s32(p+20, 0);           // pixel data size in bytes.
                s32(p+24, 0);           // xpels, 0.
                s32(p+28, 0);           // ypels, 0.
                s32(p+32, 0);           // colors used.
                p = s32(p+36, 0);       // colors important.

                // 32-bit pixel data.
                for (int y = iheight-1; y >= 0; --y) {
                    for (int x = 0; x < iwidth; ++x) {
                        p = s32(p, pix->get_pixel(Point(x, y)).argb32());
                    }
                }

                // Mask.
                std::memset(p, 0xff, mask_bytes);
                p += mask_bytes;
            }

            hc_ = HCURSOR(CreateIconFromResource(PBYTE(v.get()), bytes, false, 0x00030000));
        }
    }

    return hc_;
}

// Overrides Cursor_impl.
bool Cursor_win::has_sys_handle() const {
    return NULL != hc_;
}

// Overrides Cursor_impl.
void Cursor_win::sys_update() {
    //TODO Add actual stuff.
}

// static
Cursor_ptr Cursor_impl::create() {
    return std::make_shared<Cursor_win>();
}

} // namespace tau

//END
