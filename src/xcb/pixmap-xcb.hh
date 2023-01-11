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

#ifndef TAU_PIXMAP_XCB_HH
#define TAU_PIXMAP_XCB_HH

#include "types-xcb.hh"
#include <pixmap-impl.hh>
#include "gcontext-xcb.hh"
#include "display-xcb.hh"
#include "pixmap-painter-xcb.hh"

namespace tau {

struct Pix_store {
    Size                 sz_;
    unsigned             depth_ = 1;            // layer count.
    std::size_t          stride_ = 0;           // bytes per line.
    std::vector<uint8_t> raw_;                  // raw data.

    Pix_store(unsigned depth);
    Pix_store(unsigned depth, const Size & sz);

    void resize(const Size & sz);
    uint32_t get_pixel(const Point & pt) const;
    void put_pixel(const Point & pt, uint32_t rgb);
    void fill_rectangle(const Point & pt, const Size & sz, uint32_t on);
    void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes);
    uint32_t format() const;

    void to_mono(Pix_store & xp) const;
    void to_gray(Pix_store & xp) const;
    void to_true(Pix_store & xp) const;
    void to_full(Pix_store & xp) const;
};

class Pixmap_xcb: public Pixmap_impl {
public:

    Pixmap_xcb(unsigned depth, const Size & sz=Size());
   ~Pixmap_xcb();

    // Overrides pure Pixmap_impl.
    Painter painter() override { return wrap(std::make_shared<Pixmap_painter_xcb>(this)); }

    // Overrides pure Pixmap_impl.
    Size size() const override;

    // Overrides pure Pixmap_impl.
    int depth() const override;

    // Overrides pure Pixmap_impl.
    std::size_t bytes() const override;

    // Overrides pure Pixmap_impl.
    const uint8_t * raw() const override;

    // Overrides pure Pixmap_impl.
    void resize(const Size & sz) override;

    // Overrides pure Pixmap_impl.
    Color get_pixel(const Point & pt) const override;

    // Overrides pure Pixmap_impl.
    void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) override;

    // Overrides pure Pixmap_impl.
    void fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) override;

    void set_display(Display_xcb_ptr dp);
    Display_xcb_ptr display() { return dp_; }
    Display_xcb_cptr display() const { return dp_; }

    xcb_pixmap_t xcb_pixmap() const { return pixmap_; };
    xcb_pixmap_t create_xcb_pixmap(xcb_drawable_t drw, unsigned depth=0);

    xcb_render_picture_t xcb_render_picture() const { return picture_; }
    xcb_render_picture_t create_xcb_render_picture(xcb_render_pictformat_t pict_format=XCB_NONE);

    Pix_store * store() { return store_; }
    const Pix_store * store() const { return store_; }

    xcb_pixmap_t mask_xcb_pixmap() const { return mask_pixmap_; }
    xcb_pixmap_t create_mask_xcb_pixmap(xcb_drawable_t drw);

    xcb_render_picture_t mask_xcb_render_picture() const { return mask_picture_; }
    xcb_render_picture_t create_mask_xcb_render_picture();

    Context_xcb * gc() { return gc_; }
    const Context_xcb * gc() const { return gc_; }

    Context_xcb * mask_gc() { return gcm_; }
    const Context_xcb * mask_gc() const { return gcm_; }

    uint32_t format() const { return store_ ? store_->format() : XCB_NONE; }

private:

    void drop_cache();

private:

    Pix_store * store_ = nullptr;

    xcb_pixmap_t pixmap_ = XCB_NONE;
    xcb_pixmap_t mask_pixmap_ = XCB_NONE;

    xcb_render_picture_t picture_ = XCB_NONE;
    xcb_render_picture_t mask_picture_ = XCB_NONE;

    Context_xcb * gc_ = nullptr;
    Context_xcb * gcm_ = nullptr;

    Display_xcb_ptr dp_;
};

} // namespace tau

#endif // TAU_PIXMAP_XCB_HH
