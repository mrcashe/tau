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

#ifndef TAU_PIXMAP_WIN_HH
#define TAU_PIXMAP_WIN_HH

#include "types-win.hh"
#include "pixmap-painter-win.hh"
#include <pixmap-impl.hh>

namespace tau {

class Pixmap_win: public Pixmap_impl {
public:

    explicit Pixmap_win(unsigned depth, const Size & sz=Size());

    // Overrides pure Pixmap_impl.
    Painter painter() override { return wrap(std::make_shared<Pixmap_painter_win>(this)); }

    // Overrides pure Pixmap_impl.
    Size size() const override;

    // Overrides pure Pixmap_impl.
    int depth() const override;

    // Overrides pure Pixmap_impl.
    std::size_t bytes() const override;

    // Overrides pure Pixmap_impl.
    const uint8_t * raw() const override;

    // Overrides pure Pixmap_impl.
    void resize(const Size & size) override {
        resize_priv(size);
    }

    // Overrides pure Pixmap_impl.
    void fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) override;

    // Overrides pure Pixmap_impl.
    Color get_pixel(const Point & pt) const override;

    // Overrides pure Pixmap_impl.
    void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) override;

    HBITMAP create_bitmap(HDC dc) const;

private:

    unsigned bpp() const;
    void put_pixel_impl(const Point & pt, uint32_t rgb);
    uint32_t get_pixel_impl(const Point & pt) const;
    void resize_priv(const Size & sz);

private:

    unsigned             depth_;
    std::size_t          stride_;
    std::vector<uint8_t> raw_;
    BITMAPINFO           info_;
};

} // namespace tau

#endif // TAU_PIXMAP_WIN_HH
