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

#ifndef TAU_PIXMAP_IMPL_HH
#define TAU_PIXMAP_IMPL_HH

#include <tau/painter.hh>
#include <tau/pixmap.hh>
#include <tau/ustring.hh>

namespace tau {

class Pixmap_impl {
protected:

    Pixmap_impl() = default;

public:

    Pixmap_impl(const Pixmap_impl & other) = delete;
    Pixmap_impl & operator=(const Pixmap_impl & other) = delete;
    virtual ~Pixmap_impl() { signal_destroy_(); }

    static Pixmap wrap(Pixmap_ptr pp) { return Pixmap(pp); }
    static Painter wrap(Painter_ptr pp) { return Painter(pp); }
    static Pixmap_ptr strip(Pixmap pix) { return pix.impl; }

    virtual void fill_rectangles(const Rect * rs, std::size_t nrs, const Color & c) = 0;

    static ustring list_file_suffixes() { return "xpm:bmp:ico:png"; }

    Vector ppi() const { return ppi_; }

    void set_ppi(const Vector & value) { ppi_ = value; }

    signal<void()> & signal_destroy() { return signal_destroy_; }

    signal<void()> & signal_changed() { return signal_changed_; }

    void copy(Pixmap_cptr other);

    void put_pixel(const Point & pt, const Color & c);
    void put_pixel(int x, int y, const Color & c) { put_pixel(Point(x, y), c); }

    // This method is platform depended.
    static Pixmap_ptr create(int depth, const Size & size=Size());

    static Pixmap_ptr create(int depth, unsigned width, unsigned height) {
        return create(depth, Size(width, height));
    }

    static Pixmap_ptr create(int depth, Pixmap_cptr other) {
        Pixmap_ptr pix = create(depth, other->size());
        pix->copy(other);
        return pix;
    }

    static Pixmap_ptr dup(Pixmap_cptr other);

    static Pixmap_ptr load_xpm_from_file(const ustring & path);
    static Pixmap_ptr load_xpm_from_string(const ustring & s);
    static Pixmap_ptr load_bmp_from_file(const ustring & path);
    static Pixmap_ptr load_bmp_from_memory(const char * raw, std::size_t bytes, bool has_mask);
    static Pixmap_ptr load_png_from_file(const ustring & path);
    static Pixmap_ptr load_ico_from_file(const ustring & path);
    static Pixmap_ptr load_from_file(const ustring & path);

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual Painter painter() = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual Size size() const = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual int depth() const = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual std::size_t bytes() const = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual const uint8_t * raw() const = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual void resize(const Size & sz) = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual Color get_pixel(const Point & pt) const = 0;

    // Overriden by Pixmap_xcb.
    // Overriden by Pixmap_win.
    virtual void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) = 0;

protected:

    Vector       ppi_ { 72.0, 72.0 };
    signal<void()> signal_changed_;
    signal<void()> signal_destroy_;
};

} // namespace tau

#endif // TAU_PIXMAP_IMPL_HH
