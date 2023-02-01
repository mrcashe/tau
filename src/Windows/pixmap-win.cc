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

#include <cstring>
#include <iostream>
#include "pixmap-win.hh"

namespace tau {

Pixmap_win::Pixmap_win(unsigned depth, const Size & sz):
    Pixmap_impl()
{
    sys.depth_ = depth;
    std::memset(&sys.info_, 0, sizeof(sys.info_));
    sys.info_.bmiHeader.biSize = sizeof(sys.info_);
    if (sz) { resize_priv(sz); }
}

// Overrides pure Pixmap_impl.
Size Pixmap_win::size() const {
    return Size(sys.info_.bmiHeader.biWidth, -sys.info_.bmiHeader.biHeight);
}

// Overrides pure Pixmap_impl.
const uint8_t * Pixmap_win::raw() const {
    return sys.raw_.data();
}

// Overrides pure Pixmap_impl.
int Pixmap_win::depth() const {
    return sys.depth_;
}

// Overrides pure Pixmap_impl.
std::size_t Pixmap_win::bytes() const {
    return sys.raw_.size();
}

unsigned Pixmap_win::bpp() const {
    unsigned nbits = sys.info_.bmiHeader.biBitCount;
    return 0 == nbits ? 1 : nbits;
}

void Pixmap_win::resize_priv(const Size & sz) {
    std::size_t nbytes;
    unsigned nbits = 1 == sys.depth_ ? 1 : 32;

    if (1 == nbits) {
        std::size_t nwords = sz.width()/32;
        sys.stride_ = (0x1f & sz.width()) ? 1 : 0;
        sys.stride_ += nwords;
        sys.stride_ *= 4;
        nbytes = sys.stride_*sz.height();
        sys.raw_.assign(nbytes, 0x00);
    }

    else {
        sys.stride_ = 4*sz.width();
        nbytes = sys.stride_*sz.height();
        sys.raw_.assign(nbytes, 0x00);
    }

    sys.info_.bmiHeader.biWidth = sz.width();
    sys.info_.bmiHeader.biHeight = -int(sz.height());
    sys.info_.bmiHeader.biPlanes = 1;
    sys.info_.bmiHeader.biBitCount = nbits;
    sys.info_.bmiHeader.biCompression = BI_RGB;
    sys.info_.bmiHeader.biSizeImage = nbytes;
    sys.info_.bmiHeader.biClrUsed = 0;
    sys.info_.bmiHeader.biClrImportant = 0;
}

void Pixmap_win::put_pixel_impl(const Point & pt, uint32_t rgb) {
    if (1 == bpp()) {
        std::size_t index = (pt.x() >> 3)+pt.y()*sys.stride_;

        if (index < sys.raw_.size()) {
            unsigned shift = 7-(0x07 & pt.x());

            if (rgb) {
                sys.raw_[index] |= (1 << shift);
            }

            else {
                sys.raw_[index] &= ~(1 << shift);
            }
        }
    }

    else {
        std::size_t index = pt.y()*sys.stride_+(pt.x() << 2);

        if (index < sys.raw_.size()-3) {
            sys.raw_[index++] = rgb;
            sys.raw_[index++] = rgb >> 8;
            sys.raw_[index++] = rgb >> 16;
            sys.raw_[index] = rgb >> 24;
        }
    }
}

// Overrides pure Pixmap_impl.
void Pixmap_win::put_pixel_v(const Point & pt, const Color & c) {
    put_pixel_impl(pt, c.argb32());
    signal_changed_();
}

uint32_t Pixmap_win::get_pixel_impl(const Point & pt) const {
    if (1 == bpp()) {
        std::size_t index = (pt.y()*sys.stride_)+(pt.x() >> 3);
        unsigned shift = 7-(0x07 & pt.x());

        if (index < sys.raw_.size()) {
            return (sys.raw_[index] & (1 << shift)) ? 0x00ffffff : 0;
        }

        return 0;
    }

    else {
        std::size_t index = (pt.y()*sys.stride_)+(pt.x() << 2);
        uint32_t w = 0;

        if (index < sys.raw_.size()-4) {
            w |= uint32_t(sys.raw_[index+3]) << 24;
            w |= uint32_t(sys.raw_[index+2]) << 16;
            w |= uint32_t(sys.raw_[index+1]) << 8;
            w |= uint32_t(sys.raw_[index]);
        }

        return w;
    }
}

// Overrides pure Pixmap_impl.
Color Pixmap_win::get_pixel(const Point & pt) const {
    if (1 == depth()) {
        return Color(get_pixel_impl(pt) ? COLOR_WHITE : COLOR_BLACK);
    }

    else if (32 == depth()) {
        return Color::from_argb32(get_pixel_impl(pt));
    }

    else {
        return Color::from_rgb24(get_pixel_impl(pt));
    }

    return Color();
}

// Overrides pure Pixmap_impl.
void Pixmap_win::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    for (; 0 != nrs; --nrs, ++rs) {
        std::size_t sindex, nbytes = sys.raw_.size();
        unsigned height = rs->height();

        if (1 == bpp()) {
            sindex = (rs->y()*sys.stride_)+(rs->x() >> 3);
            unsigned sshift = 7-(0x07 & rs->x());

            for (int y = rs->y(); sindex < nbytes && 0 != height; ++y, sindex += sys.stride_, --height) {
                std::size_t index = sindex, shift = sshift;
                unsigned width = rs->width();

                for (int x = rs->x(); index < nbytes && 0 != width; ++x, --width) {
                    sys.raw_[index] &= ~(1 << shift);

                    if (c.rgb24()) {
                        sys.raw_[index] |= (1 << shift);
                    }

                    if (0 == shift) {
                        shift = 7;
                        ++index;
                    }

                    else {
                        --shift;
                    }
                }
            }
        }

        else {
            sindex = (rs->y()*sys.stride_)+(rs->x() << 2);
            uint32_t rgb = 8 == depth() ? c.gray24() : (32 == depth() ? c.argb32() : c.rgb24());

            for (int y = rs->y(); sindex < nbytes && 0 != height; ++y, sindex += sys.stride_, --height) {
                std::size_t index = sindex;
                unsigned width = rs->width();

                for (int x = rs->x(); index < nbytes-4 && 0 != width; ++x, --width) {
                    sys.raw_[index++] = rgb;
                    sys.raw_[index++] = rgb >> 8;
                    sys.raw_[index++] = rgb >> 16;
                    sys.raw_[index++] = rgb >> 24;
                }
            }
        }
    }

    signal_changed_();
}

// Overrides pure Pixmap_impl.
void Pixmap_win::set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) {
    if (1 == bpp()) {
        std::size_t index = (pt.y()*sys.stride_)+(pt.x() >> 3);
        unsigned shift = 7-(0x07 & pt.x());

        while (index < sys.raw_.size() && nbytes >= 4) {
            uint32_t c = 0;
            c |= buffer[3]; c <<= 8;
            c |= buffer[2]; c <<= 8;
            c |= buffer[1]; c <<= 8;
            c |= buffer[0];
            if (c) { sys.raw_[index] |= (1 << shift); }
            else { sys.raw_[index] &= ~(1 << shift); }
            buffer += 4;
            nbytes -= 4;
            if (0 == shift) { shift = 7; ++index; }
            else { --shift; }
        }
    }

    else {
        std::size_t index = (pt.y()*sys.stride_)+(pt.x() << 2);

        while (index < sys.raw_.size() && nbytes >= 4) {
            if (8 == depth()) {
                uint32_t c = 0;
                c |= buffer[3]; c <<= 8;
                c |= buffer[2]; c <<= 8;
                c |= buffer[1]; c <<= 8;
                c |= buffer[0];

                Color cc = Color::from_argb32(c);
                uint32_t gray = cc.gray24();
                sys.raw_[index++] = gray;
                sys.raw_[index++] = gray >> 8;
                sys.raw_[index++] = gray >> 16;
                sys.raw_[index++] = gray >> 24;
            }

            else {
                sys.raw_[index++] = *buffer++;
                sys.raw_[index++] = *buffer++;
                sys.raw_[index++] = *buffer++;
                sys.raw_[index++] = *buffer++;
            }

            nbytes -= 4;
        }
    }

    signal_changed_();
}

HBITMAP Pixmap_win::create_bitmap(HDC dc) const {
    if (HBITMAP cbm = CreateCompatibleBitmap(dc, size().width(), size().height())) {
        struct { BITMAPINFO bi; uint32_t pal[2]; } i;
        i.bi = sys.info_;

        if (1 == sys.depth_) {
            i.bi.bmiHeader.biClrUsed = 2;
            i.pal[0] = RGB(0, 0, 0);
            i.pal[1] = RGB(255, 255, 255);
        }

        DWORD scans_written = SetDIBits(dc, cbm, 0, size().height(), sys.raw_.data(), (BITMAPINFO *)&i, DIB_RGB_COLORS);
        if (0 != scans_written) { return cbm; }
        DeleteObject(cbm);
    }

    return NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Pixmap_ptr Pixmap_impl::create(int depth, const Size & sz) {
    return std::make_shared<Pixmap_win>(depth, sz);
}

} // namespace tau

//END
