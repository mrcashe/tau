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

#include "pixmap-xcb.hh"
#include "display-xcb.hh"

namespace tau {

Pix_store::Pix_store(unsigned depth, const Size & sz):
    depth_(0 != depth ? depth : 1)
{
    if (sz) { resize(sz); }
}

uint32_t Pix_store::format() const {
    return 1 == depth_ ? XCB_IMAGE_FORMAT_XY_BITMAP : (depth_ < 8 ? XCB_IMAGE_FORMAT_XY_PIXMAP : XCB_IMAGE_FORMAT_Z_PIXMAP);
}

void Pix_store::resize(const Size & sz) {
    unsigned width = sz.width();
    std::size_t nwords, nbytes;

    if (depth_ < 8) {
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

    if (depth_ < 8) {
        index = (pt.x() >> 3)+pt.y()*stride_;
        unsigned shift = 0x07 & pt.x();

        for (std::size_t nbits = 0; index < raw_.size() && nbits < depth_; ++nbits) {
            raw_[index] &= ~(1 << shift);
            if (1 & rgb) { raw_[index] |= (1 << shift); }
            index += (sz_.width()*stride_);
            rgb <<= 1;
        }
    }

    else if (8 == depth_) {
        index = (pt.y()*stride_)+pt.x();

        if (index < raw_.size()) {
            raw_[index] = rgb;
        }
    }

    else {
        index = (pt.y()*stride_)+(pt.x() << 2);

        if (index < raw_.size()-3) {
            raw_[index++] = rgb;
            raw_[index++] = rgb >> 8;
            raw_[index++] = rgb >> 16;
            raw_[index] = rgb >> 24;
        }
    }
}

void Pix_store::fill_rectangle(const Point & pt, const Size & sz, uint32_t rgb) {
    std::size_t sindex, nbytes = raw_.size();
    unsigned height = sz.height();

    if (depth_ < 8) {
        sindex = (pt.y()*stride_)+(pt.x() >> 3);
        unsigned sshift = 0x07 & pt.x();

        for (; sindex < nbytes && 0 != height; sindex += stride_, --height) {
            std::size_t index = sindex, shift = sshift, width = sz.width();

            for (; index < nbytes && 0 != width; --width) {
                uint32_t w = rgb;
                std::size_t rindex = index;

                for (std::size_t nbits = 0; rindex < nbytes && nbits < depth_; ++nbits) {
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

        for (; sindex < nbytes && 0 != height; sindex += stride_, --height) {
            std::size_t index = sindex, width = sz.width();

            for (; index < nbytes && 0 != width; --width) {
                raw_[index++] = rgb;
            }
        }
    }

    else {
        sindex = (pt.y()*stride_)+(pt.x() << 2);

        for (; sindex < nbytes && 0 != height; sindex += stride_, --height) {
            std::size_t index = sindex, width = sz.width();

            for (; index < nbytes-4 && 0 != width; --width) {
                raw_[index++] = rgb;
                raw_[index++] = rgb >> 8;
                raw_[index++] = rgb >> 16;
                raw_[index++] = rgb >> 24;
            }
        }
    }
}

void Pix_store::set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) {
    if (depth_ < 8) {
        std::size_t index = (pt.y()*stride_)+(pt.x() >> 3);
        unsigned shift = 0x07 & pt.x();

        while (index < raw_.size() && nbytes >= 4) {
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
        std::size_t index = (pt.y()*stride_)+pt.x();

        while (index < raw_.size() && nbytes >= 4) {
            uint32_t c = 0;
            c |= buffer[3]; c <<= 8;
            c |= buffer[2]; c <<= 8;
            c |= buffer[1]; c <<= 8;
            c |= buffer[0];
            Color cc = Color::from_argb32(c);
            raw_[index] = cc.gray8();
            ++index;
            buffer += 4;
            nbytes -= 4;
        }
    }

    else {
        std::size_t index = (pt.y()*stride_)+(pt.x() << 2);

        while (index < raw_.size() && nbytes >= 4) {
            raw_[index++] = *buffer++;
            raw_[index++] = *buffer++;
            raw_[index++] = *buffer++;
            raw_[index++] = *buffer++;
            nbytes -= 4;
        }
    }
}

void Pix_store::to_full(Pix_store & pm) const {
    if (1 == depth_) {
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (std::size_t x = 0; x < sz_.width(); ++x) {
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
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;

            for (unsigned x = 0; x < sz_.width(); ++x) {
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
    if (1 == depth_) {
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < sz_.width(); ++x) {
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
                    pm.raw_[dst++] = 0xff;
                }

                if (0x80 == mask) { mask = 0x01; ++src; }
                else { mask <<= 1; }
            }
        }
    }

    else if (8 == depth_) {
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;

            for (unsigned x = 0; x < sz_.width(); ++x) {
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

void Pix_store::to_gray(Pix_store & pm) const {
    if (1 == depth_) {
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < sz_.width(); ++x) {
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
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;

            for (unsigned x = 0; x < sz_.width(); ++x) {
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
    if (1 == depth_) {
        pm.raw_ = raw_;
    }

    else if (8 == depth_) {
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < sz_.width(); ++x) {
                pm.raw_[dst] &= ~mask;
                if (0 != raw_[src++]) { pm.raw_[dst] |= mask; }
                if (0x80 == mask) { mask = 0x01; ++dst; }
                else { mask <<= 1; }
            }
        }
    }

    else {
        for (unsigned y = 0; y < sz_.height(); ++y) {
            std::size_t src = y*stride_;
            std::size_t dst = y*pm.stride_;
            std::size_t mask = 0x01;

            for (unsigned x = 0; x < sz_.width(); ++x) {
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

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

Pixmap_xcb::Pixmap_xcb(unsigned depth, const Size & sz):
    Pixmap_impl()
{
    store_ = new Pix_store(depth, sz);
}

Pixmap_xcb::~Pixmap_xcb() {
    drop_cache();
    delete store_;
}

// Overrides pure Pixmap_impl.
Size Pixmap_xcb::size() const {
    return store_->sz_;
}

// Overrides pure Pixmap_impl.
const uint8_t * Pixmap_xcb::raw() const {
    return store_->raw_.data();
}

// Overrides pure Pixmap_impl.
int Pixmap_xcb::depth() const {
    return store_->depth_;
}

// Overrides pure Pixmap_impl.
std::size_t Pixmap_xcb::bytes() const {
    return store_->raw_.size();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::resize(const Size & sz) {
    store_->resize(sz);
    drop_cache();
    signal_changed_();
}

// Overrides pure Pixmap_impl.
Color Pixmap_xcb::get_pixel(const Point & pt) const {
    if (1 == depth()) {
        return store_->get_pixel(pt) ? Color("White") : Color("Black");
    }

    else if (8 == depth()) {
        return Color::from_gray8(store_->get_pixel(pt));
    }

    else if (24 == depth()) {
        return Color::from_rgb24(store_->get_pixel(pt));
    }

    else {
        return Color::from_argb32(store_->get_pixel(pt));
    }

    return Color();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) {
    if (1 == depth()) {
        for (; 0 != nrs; --nrs, ++rs) {
            store_->fill_rectangle(rs->origin(), rs->size(), 0 != c.rgb24() ? 0xff : 0x00);
        }
    }

    else if (8 == depth()) {
        for (; 0 != nrs; --nrs, ++rs) {
            store_->fill_rectangle(rs->origin(), rs->size(), c.gray8());
        }
    }

    else if (24 == depth()) {
        for (; 0 != nrs; --nrs, ++rs) {
            store_->fill_rectangle(rs->origin(), rs->size(), c.rgb24());
        }
    }

    else if (32 == depth()) {
        for (; 0 != nrs; --nrs, ++rs) {
            store_->fill_rectangle(rs->origin(), rs->size(), c.argb32());
        }
    }

    drop_cache();
    signal_changed_();
}

// Overrides pure Pixmap_impl.
void Pixmap_xcb::set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) {
    store_->set_argb32(pt, buffer, nbytes);
    drop_cache();
    signal_changed_();
}

void Pixmap_xcb::set_display(Display_xcb_ptr dp) {
    if (!dp_ || dp_ != dp) {
        drop_cache();
        dp_ = dp;
    }
}

xcb_pixmap_t Pixmap_xcb::create_xcb_pixmap(xcb_drawable_t drw, unsigned depth) {
    if (dp_) {
        pixmap_ = xcb_generate_id(dp_->conn());
        xcb_create_pixmap(dp_->conn(), 0 == depth ? dp_->depth() : depth, pixmap_, drw, size().width(), size().height());
        gc_ = new Context_xcb(dp_, pixmap_);
        return pixmap_;
    }

    return XCB_NONE;
}

xcb_pixmap_t Pixmap_xcb::create_mask_xcb_pixmap(xcb_drawable_t drw) {
    if (dp_) {
        mask_pixmap_ = xcb_generate_id(dp_->conn());
        xcb_create_pixmap(dp_->conn(), 1, mask_pixmap_, drw, size().width(), size().height());
        gcm_ = new Context_xcb(dp_, mask_pixmap_);
        return mask_pixmap_;
    }

    return XCB_NONE;
}

xcb_render_picture_t Pixmap_xcb::create_xcb_render_picture(xcb_render_pictformat_t pict_format) {
    if (dp_ && XCB_NONE != pixmap_) {
        picture_ = xcb_generate_id(dp_->conn());
        const uint32_t v[1] = { 0 };
        xcb_render_create_picture(dp_->conn(), picture_, pixmap_, XCB_NONE != pict_format ? pict_format : dp_->pictformat(), 1, v);
    }

    return picture_;
}

xcb_render_picture_t Pixmap_xcb::create_mask_xcb_render_picture() {
    if (dp_ && XCB_NONE != mask_pixmap_) {
        mask_picture_ = xcb_generate_id(dp_->conn());
        const uint32_t v[1] = { 0 };
        xcb_render_create_picture(dp_->conn(), mask_picture_, mask_pixmap_, dp_->pictformat(1), 1, v);
        return mask_picture_;
    }

    return XCB_NONE;
}

void Pixmap_xcb::drop_cache() {
    if (dp_) {
        if (XCB_NONE != mask_picture_) {
            xcb_render_free_picture(dp_->conn(), mask_picture_);
            mask_picture_ = XCB_NONE;
        }

        if (XCB_NONE != picture_) {
            xcb_render_free_picture(dp_->conn(), picture_);
            picture_ = XCB_NONE;
        }

        if (XCB_NONE != mask_pixmap_) {
            xcb_free_pixmap(dp_->conn(), mask_pixmap_);
            mask_pixmap_ = XCB_NONE;
        }

        if (XCB_NONE != pixmap_) {
            xcb_free_pixmap(dp_->conn(), pixmap_);
            pixmap_ = XCB_NONE;
        }

        if (gc_) {
            delete gc_;
            gc_ = nullptr;
        }

        if (gcm_) {
            delete gcm_;
            gcm_ = nullptr;
        }
    }
}

} // namespace tau

//END
