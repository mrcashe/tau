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

#ifndef TAU_PIXMAP_XCB_HH
#define TAU_PIXMAP_XCB_HH

#include "types-xcb.hh"
#include <pixmap-impl.hh>
#include "gcontext-xcb.hh"
#include "display-xcb.hh"
#include "pixmap-painter-xcb.hh"

namespace tau {

struct Pix_store {
    Size                    sz_;
    int                     depth_ = 1;            // layer count.
    uint8_t                 format_;
    std::size_t             stride_ = 0;           // bytes per line.
    std::vector<uint8_t>    raw_;                  // raw data.

    explicit Pix_store(int depth, const Size & sz=Size());

    void resize(const Size & sz);
    uint32_t get_pixel(const Point & pt) const;
    void put_pixel(const Point & pt, uint32_t rgb);
    void fill_rectangle(const Point & pt, const Size & sz, uint32_t on);
    void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes);

    void to_mono(Pix_store & xp) const;
    void to_gray(Pix_store & xp) const;
    void to_true(Pix_store & xp) const;
    void to_full(Pix_store & xp) const;

    void convert(Pix_store & xp) const;
};

struct Pixmap_sys {
    Display_xcb_ptr         dp_;
    xcb_connection_t *      cx_  = nullptr;
    xcb_pixmap_t            pixmap_ = XCB_NONE;
    xcb_pixmap_t            mask_pixmap_ = XCB_NONE;
    xcb_render_picture_t    picture_ = XCB_NONE;
    xcb_render_picture_t    mask_picture_ = XCB_NONE;
    Pix_store *             store_ = nullptr;
    Context_xcb *           gc_  = nullptr;
    Context_xcb *           gcm_ = nullptr;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Pixmap_xcb: public Pixmap_impl {
public:

    explicit Pixmap_xcb(int depth, const Size & sz=Size());
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
    void put_pixel_v(const Point & pt, const Color & c) override;

    // Overrides pure Pixmap_impl.
    Color get_pixel(const Point & pt) const override;

    // Overrides pure Pixmap_impl.
    void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) override;

    // Overrides pure Pixmap_impl.
    void fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) override;

    void set_display(Display_xcb_ptr dp) const;

    void draw(xcb_drawable_t drw, xcb_render_picture_t pict, Oper op, const Point & pix_origin, const Size & pix_size, const Point & pt, bool transparent) const;

private:

    mutable Pixmap_sys sys;

private:

    void drop_cache() const;
    void put(uint8_t format, xcb_drawable_t drw, const Context_xcb * gc, const Size & sz, const Point & dst_pos, uint8_t left_pad, uint8_t depth, uint32_t data_len, const uint8_t * data) const;
};

} // namespace tau

#endif // TAU_PIXMAP_XCB_HH
