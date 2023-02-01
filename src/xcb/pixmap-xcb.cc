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

#include <tau/string.hh>
#include "pixmap-xcb.hh"
#include "display-xcb.hh"
#include <cstring>
#include <iostream>

namespace tau {

Pix_store::Pix_store(int depth, const Size & sz):
    depth_(0 != depth ? depth : 1)
{
    format_ = 1 == depth_ ? XCB_IMAGE_FORMAT_XY_BITMAP : (depth_ < 8 ? XCB_IMAGE_FORMAT_XY_PIXMAP : XCB_IMAGE_FORMAT_Z_PIXMAP);
    if (sz) { resize(sz); }
}

void Pix_store::resize(const Size & sz) {
    unsigned width = sz.width();
    std::size_t nwords, nbytes;

    if (1 == depth_) {
        nwords = width >> 5;
        stride_ = (0x1f & width) ? 1 : 0;
        stride_ += nwords;
        stride_ *= 4;
        nbytes = stride_*depth_*sz.height();
    }

    else if (8 == depth_) {
        nwords = width >> 2;
        stride_ = (0x03 & width) ? 1 : 0;
        stride_ += nwords;
        stride_ *= 4;
        nbytes = stride_*sz.height();
    }

    else {
        stride_ = width << 2;
        nbytes = stride_*sz.height();
    }

    raw_.assign(nbytes, 0);
    sz_ = sz;
}

uint32_t Pix_store::get_pixel(const Point & pt) const {
    uint32_t w = 0;
    std::size_t index;

    if (depth_ < 8) {
        index = (pt.y()*stride_)+(pt.x() >> 3);
        unsigned shift = 0x07 & pt.x();

        for (std::size_t nbits = 0; index < raw_.size() && nbits < depth_; ++nbits) {
            w <<= 1;
            if (raw_[index] & (1 << shift)) { ++w; }
            index += (sz_.width()*stride_);
        }
    }

    else if (8 == depth_) {
        index = (pt.y()*stride_)+pt.x();

        if (index < raw_.size()) {
            w = raw_[index];
        }
    }

    else {
        index = (pt.y()*stride_)+(pt.x() << 2);

        if (index < raw_.size()-4) {
            w |= uint32_t(raw_[index+3]) << 24;
            w |= uint32_t(raw_[index+2]) << 16;
            w |= uint32_t(raw_[index+1]) << 8;
            w |= uint32_t(raw_[index]);
        }
    }

    return w;
}

void Pix_store::put_pixel(const Point & pt, uint32_t rgb) {
    std::size_t index;

    if (1 == depth_) {
        rgb = rgb ? 1 : 0;
        index = (pt.x() >> 3)+pt.y()*stride_;
        unsigned shift = 0x07 & pt.x();

        for (std::size_t nbits = 0; index < raw_.size() && nbits < depth_; ++nbits) {
            raw_[index] &= ~(1 << shift);
            raw_[index] |= (rgb << shift);
            index += (sz_.width()*stride_);
            rgb <<= 1;
        }
    }

    else if (8 == depth_) {
        index = (pt.y()*stride_)+pt.x();
        if (index < raw_.size()) { raw_[index] = rgb; }
    }

    else {
        index = (pt.y()*stride_)+(pt.x() << 2);
        if (index+3 < raw_.size()) { *reinterpret_cast<uint32_t *>(raw_.data()+index) = rgb; }
    }
}

void Pix_store::fill_rectangle(const Point & pt, const Size & sz, uint32_t rgb) {
    std::size_t sindex, rbytes = raw_.size();
    if (0 == rbytes) { return; }
    unsigned height = sz.height();

    if (1 == depth_) {
        rgb = rgb ? 0xff : 0;
        sindex = (pt.y()*stride_)+(pt.x() >> 3);
        unsigned sshift = 0x07 & pt.x();

        for (; sindex < rbytes && 0 != height; sindex += stride_, --height) {
            std::size_t index = sindex, shift = sshift, width = sz.width();

            for (; index < rbytes && 0 != width; --width) {
                uint32_t w = rgb;
                std::size_t rindex = index;

                for (std::size_t nbits = 0; rindex < rbytes && nbits < depth_; ++nbits) {
                    raw_[rindex] &= ~(1 << shift);
                    if (1 & w) { raw_[rindex] |= (1 << shift); }
                    rindex += (sz_.width()*stride_);
                    w <<= 1;
                }

                if (8 == ++shift) { shift = 0; ++index; }
            }
        }
    }

    else if (8 == depth_) {
        sindex = (pt.y()*stride_)+pt.x();

        for (; sindex < rbytes && 0 != height; sindex += stride_, --height) {
            std::size_t index = sindex, width = sz.width();

            for (; index < rbytes && 0 != width; --width) {
                raw_[index++] = rgb;
            }
        }
    }

    else {
        sindex = (pt.y()*stride_)+(pt.x() << 2);

        for (; sindex < rbytes && 0 != height; sindex += stride_, --height) {
            std::wmemset(reinterpret_cast<wchar_t *>(raw_.data()+sindex), rgb, sz.width());
        }
    }
}

void Pix_store::set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) {
    std::size_t index, rbytes = raw_.size();
    if (!rbytes) { return; }

    if (1 == depth_) {
        index = (pt.y()*stride_)+(pt.x() >> 3);
        unsigned shift = 0x07 & pt.x();

        while (index < rbytes && nbytes >= 4) {
            uint32_t c = 0;
            c |= buffer[3]; c <<= 8;
            c |= buffer[2]; c <<= 8;
            c |= buffer[1]; c <<= 8;
            c |= buffer[0];
            if (0 != c) { raw_[index] |= (1 << shift); }
            else { raw_[index] &= ~(1 << shift); }
            buffer += 4;
            nbytes -= 4;
            if (8 == ++shift) { shift = 0; ++index; }
        }
    }

    else if (8 == depth_) {
        index = (pt.y()*stride_)+pt.x();

        while (index < rbytes && nbytes >= 4) {
            uint32_t c = 0;
            c |= buffer[3]; c <<= 8;
            c |= buffer[2]; c <<= 8;
            c |= buffer[1]; c <<= 8;
            c |= buffer[0];
            Color cc = Color::from_argb32(c);
            raw_[index++] = cc.gray8();
            buffer += 4;
            nbytes -= 4;
        }
    }

    else {
        index = (pt.y()*stride_)+(pt.x() << 2);
        if (index < rbytes) { std::memcpy(raw_.data()+index, buffer, std::min(nbytes, rbytes-index)); }
    }
}

void Pix_store::to_full(Pix_store & pm) const {
    unsigned w = sz_.width(), h = sz_.height();

    if (1 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (std::size_t x = 0; x < w; ++x) {
                if (mask & raw_[src]) {
                    pm.raw_[dst++] = 0xff;
                    pm.raw_[dst++] = 0xff;
                    pm.raw_[dst++] = 0xff;
                    pm.raw_[dst++] = 0xff;
                }

                else {
                    pm.raw_[dst++] = 0x00;
                    pm.raw_[dst++] = 0x00;
                    pm.raw_[dst++] = 0x00;
                    pm.raw_[dst++] = 0xff;     // FIXME What aboutl alpha here?
                }

                if (0x80 == mask) { mask = 0x01; ++src; }
                else { mask <<= 1; }
            }
        }
    }

    else if (8 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;

            for (unsigned x = 0; x < w; ++x) {
                uint8_t g8 = raw_[src++];
                pm.raw_[dst++] = g8;
                pm.raw_[dst++] = g8;
                pm.raw_[dst++] = g8;
                pm.raw_[dst++] = 0xff;
            }
        }
    }

    else {
        pm.raw_ = raw_;
    }
}

void Pix_store::to_true(Pix_store & pm) const {
    unsigned w = sz_.width(), h = sz_.height();

    if (1 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            const uint8_t * src = raw_.data()+y*stride_;
            uint32_t * dst = reinterpret_cast<uint32_t *>(pm.raw_.data()+y*pm.stride_);
            uint8_t mask = 0x01;

            for (unsigned x = 0; x < w; ++x) {
                *dst++ = mask & *src ? 0x00ffffff : 0;
                if (0x80 == mask) { mask = 0x01; ++src; }
                else { mask <<= 1; }
            }
        }
    }

    else if (8 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            const uint8_t * src = raw_.data()+y*stride_;
            uint8_t * dst = pm.raw_.data()+y*pm.stride_;

            for (unsigned x = 0; x < w; ++x) {
                uint8_t g8 = *src++;
                *dst++ = g8;
                *dst++ = g8;
                *dst++ = g8;
                *dst++ = 0;
            }
        }
    }

    else if (32 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            const uint32_t * src = reinterpret_cast<const uint32_t *>(raw_.data()+y*stride_);
            uint32_t * dst = reinterpret_cast<uint32_t *>(pm.raw_.data()+y*pm.stride_);

            for (unsigned x = 0; x < w; ++x) {
                Color c = Color::from_argb32(*src++), c2;
                double a = c.alpha(), r = c.red(), g = c.green(), b = c.blue();
                // Source => Target = (BGColor + Source) =
                // Target.R = ((1 - Source.A) * BGColor.R) + (Source.A * Source.R)
                // Target.G = ((1 - Source.A) * BGColor.G) + (Source.A * Source.G)
                // Target.B = ((1 - Source.A) * BGColor.B) + (Source.A * Source.B)
                // Do alpha blending with White destination color.
                c2.set((1-a)+a*r, (1-a)+a*g, (1-a)+a*b, 0);
                *dst++ = c2.rgb24();
            }
        }
    }

    else {
        pm.raw_ = raw_;
    }
}

void Pix_store::to_gray(Pix_store & pm) const {
    unsigned w = sz_.width(), h = sz_.height();

    if (1 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < w; ++x) {
                pm.raw_[dst++] = mask & raw_[src] ? 0xff : 0x00;
                if (0x80 == mask) { mask = 0x01; ++src; }
                else { mask <<= 1; }
            }
        }
    }

    else if (8 == depth_) {
        pm.raw_ = raw_;
    }

    else {
        for (unsigned y = 0; y < h; ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;

            for (unsigned x = 0; x < w; ++x) {
                uint32_t c = 0;
                c |= raw_[src+3]; c <<= 8;
                c |= raw_[src+2]; c <<= 8;
                c |= raw_[src+1]; c <<= 8;
                c |= raw_[src];
                src += 4;
                pm.raw_[dst++] = Color::from_argb32(c).gray8();
            }
        }
    }
}

void Pix_store::to_mono(Pix_store & pm) const {
    unsigned w = sz_.width(), h = sz_.height();

    if (1 == depth_) {
        pm.raw_ = raw_;
    }

    else if (8 == depth_) {
        for (unsigned y = 0; y < h; ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < w; ++x) {
                pm.raw_[dst] &= ~mask;
                if (0 != raw_[src++]) { pm.raw_[dst] |= mask; }
                if (0x80 == mask) { mask = 0x01; ++dst; }
                else { mask <<= 1; }
            }
        }
    }

    else {
        for (unsigned y = 0; y < h; ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < w; ++x) {
                pm.raw_[dst] &= ~mask;

                uint8_t w = 0;
                w |= raw_[src++];
                w |= raw_[src++];
                w |= raw_[src++];
                w |= raw_[src++];

                if (0 != w) { pm.raw_[dst] |= mask; }
                if (0x80 == mask) { mask = 0x01; ++dst; }
                else { mask <<= 1; }
            }
        }
    }
}

void Pix_store::convert(Pix_store & pm) const {
    if (pm.depth_ == depth_) {
        pm.raw_ = raw_;
    }

    else {
        switch (pm.depth_) {
            case 32: to_full(pm); break;
            case 24: to_true(pm); break;
            case  8: to_gray(pm); break;
            default: to_mono(pm);
        }
    }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

Pixmap_xcb::Pixmap_xcb(int depth, const Size & sz):
    Pixmap_impl()
{
    sys.store_ = new Pix_store(depth, sz);
}

Pixmap_xcb::~Pixmap_xcb() {
    drop_cache();
    if (sys.store_) { delete sys.store_; }
}

// Overrides pure Pixmap_impl.
Size Pixmap_xcb::size() const {
    return sys.store_->sz_;
}

// Overrides pure Pixmap_impl.
const uint8_t * Pixmap_xcb::raw() const {
    return sys.store_->raw_.data();
}

// Overrides pure Pixmap_impl.
int Pixmap_xcb::depth() const {
    return sys.store_->depth_;
}

// Overrides pure Pixmap_impl.
std::size_t Pixmap_xcb::bytes() const {
    return sys.store_->raw_.size();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::resize(const Size & sz) {
    sys.store_->resize(sz);
    drop_cache();
    signal_changed_();
}

// Overrides pure Pixmap_impl.
Color Pixmap_xcb::get_pixel(const Point & pt) const {
    if (1 == depth()) {
        return Color(sys.store_->get_pixel(pt) ? COLOR_WHITE : COLOR_BLACK);
    }

    else if (8 == depth()) {
        return Color::from_gray8(sys.store_->get_pixel(pt));
    }

    else if (24 == depth()) {
        return Color::from_rgb24(sys.store_->get_pixel(pt));
    }

    else {
        return Color::from_argb32(sys.store_->get_pixel(pt));
    }

    return Color();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::put_pixel_v(const Point & pt, const Color & c) {
    uint32_t argb = c.argb32();
    if (8 == sys.store_->depth_) { argb = c.gray8(); }
    sys.store_->put_pixel(pt, argb);
    drop_cache();
    signal_changed_();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    uint32_t argb = c.argb32();
    if (8 == sys.store_->depth_) { argb = c.gray8(); }
    sys.store_->fill_rectangle(rs->origin(), rs->size(), argb);
    drop_cache();
    signal_changed_();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) {
    sys.store_->set_argb32(pt, buffer, nbytes);
    drop_cache();
    signal_changed_();
}

void Pixmap_xcb::set_display(Display_xcb_ptr dp) const {
    if (!sys.dp_ || sys.dp_ != dp) {
        drop_cache();
        sys.dp_ = dp;
        sys.cx_ = dp->conn();
    }
}

void Pixmap_xcb::drop_cache() const {
    if (XCB_NONE != sys.mask_picture_) {
        if (sys.cx_) { xcb_render_free_picture(sys.cx_, sys.mask_picture_); }
        sys.mask_picture_ = XCB_NONE;
    }

    if (XCB_NONE != sys.picture_) {
        if (sys.cx_) { xcb_render_free_picture(sys.cx_, sys.picture_); }
        sys.picture_ = XCB_NONE;
    }

    if (XCB_NONE != sys.mask_pixmap_) {
        if (sys.cx_) { xcb_free_pixmap(sys.cx_, sys.mask_pixmap_); }
        sys.mask_pixmap_ = XCB_NONE;
    }

    if (XCB_NONE != sys.pixmap_) {
        if (sys.cx_) { xcb_free_pixmap(sys.cx_, sys.pixmap_); }
        sys.pixmap_ = XCB_NONE;
    }

    if (sys.gc_) {
        delete sys.gc_;
        sys.gc_ = nullptr;
    }

    if (sys.gcm_) {
        delete sys.gcm_;
        sys.gcm_ = nullptr;
    }
}

void Pixmap_xcb::put(uint8_t str_format, xcb_drawable_t drw, const Context_xcb * gc, const Size & sz, const Point & dst_pos, uint8_t left_pad, uint8_t depth, uint32_t data_len, const uint8_t * data) const {
    gc->flush();
    xcb_put_image(sys.cx_, str_format, drw, gc->xid(), sz.width(), sz.height(), dst_pos.x(), dst_pos.y(), left_pad, depth, data_len, data);
    xcb_flush(sys.cx_);
}

void Pixmap_xcb::draw(xcb_drawable_t drw, xcb_render_picture_t pict, Oper op, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) const {
    if (!sys.dp_ || !sys.store_) { return; }

    if (XCB_NONE == sys.pixmap_) {
        sys.pixmap_ = xcb_generate_id(sys.cx_);
        xcb_create_pixmap(sys.cx_, sys.dp_->depth(), sys.pixmap_, drw, size().width(), size().height());
        sys.gc_ = new Context_xcb(sys.cx_, sys.pixmap_);

        sys.picture_ = xcb_generate_id(sys.cx_);
        const uint32_t v[1] = { 0 };
        xcb_render_create_picture(sys.cx_, sys.picture_, sys.pixmap_, sys.dp_->pictformat(), 1, v);

        if (sys.dp_->depth() != depth()) {
            Pix_store pm(sys.dp_->depth(), size());
            sys.store_->convert(pm);
            put(pm.format_, sys.pixmap_, sys.gc_, size(), Point(), 0, pm.depth_, pm.raw_.size(), pm.raw_.data());
        }

        else {
            put(sys.store_->format_, sys.pixmap_, sys.gc_, size(), Point(), 0, depth(), bytes(), raw());
        }
    }

    xcb_render_picture_t pmask = XCB_NONE;

    if (transparent && 32 == depth()) {
        if (XCB_NONE == sys.mask_pixmap_) {
            sys.mask_pixmap_ = xcb_generate_id(sys.cx_);
            xcb_create_pixmap(sys.cx_, 1, sys.mask_pixmap_, drw, size().width(), size().height());
            sys.gcm_ = new Context_xcb(sys.cx_, sys.mask_pixmap_);
            Pix_store xpm(1, size());

            for (int y = 0; y < size().iheight(); ++y) {
                for (int x = 0; x < size().iwidth(); ++x) {
                    uint32_t v = sys.store_->get_pixel(Point(x, y));
                    xpm.put_pixel(Point(x, y), 0 != (v >> 24) ? 0 : 1);
                }
            }

            put(xpm.format_, sys.mask_pixmap_, sys.gcm_, size(), Point(), 0, 1, xpm.raw_.size(), xpm.raw_.data());
            sys.mask_picture_ = xcb_generate_id(sys.cx_);
            const uint32_t v[1] = { 0 };
            xcb_render_create_picture(sys.cx_, sys.mask_picture_, sys.mask_pixmap_, sys.dp_->pictformat(1), 1, v);
        }

        pmask = sys.mask_picture_;
    }

    xcb_render_composite(sys.cx_, xrender_oper(op), sys.picture_, pmask, pict, pix_origin.x(), pix_origin.y(), 0, 0, pt.x(), pt.y(), pix_size.width(), pix_size.height());
    xcb_flush(sys.cx_);
}

} // namespace tau

//END
