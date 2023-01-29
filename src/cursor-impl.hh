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

#ifndef TAU_CURSOR_IMPL_HH
#define TAU_CURSOR_IMPL_HH

#include <tau/cursor.hh>
#include <fstream>
#include <map>

namespace tau {

class Cursor_impl {
public:

    Cursor_impl() = default;
    virtual ~Cursor_impl() {}

    Cursor_impl(const Cursor_impl & other) = delete;
    Cursor_impl & operator=(const Cursor_impl & other) = delete;

    static Cursor_ptr create();

    static Cursor_ptr load_from_file(const ustring & path, unsigned size_px);

    static Cursor_ptr create(Pixmap_ptr pix, const Point & hotspot=Point());

    static Cursor wrap(Cursor_ptr cp) { return Cursor(cp); }

    static Cursor_ptr strip(Cursor c) { return c.impl; }

    unsigned size() const;

    int frame_count() const;

    Pixmap_ptr pixmap(int index) const;

    Point hotspot(int nth_frame=0) const;

    unsigned delay(int nth_frame) const;

    unsigned delay() const;

    void assign(Pixmap_cptr pix, const Point & hotspot);

    int append(Pixmap_cptr pix, unsigned delay_ms, const Point & hotspot);

    void prepend(Pixmap_cptr pix, unsigned delay_ms, const Point & hotspot);

    int insert(int nth_frame, Pixmap_cptr pix, unsigned delay_ms, const Point & hotspot);

    void set_pixmap(Pixmap_cptr pix, int nth_frame);

    void set_delay(unsigned delay_ms, int nth_frame);

    void set_hotspot(const Point & hotspot, int nth_frame);

    void set_delay(unsigned delay_ms);

    /// Load from file.
    void load(const ustring & path, unsigned size_px);

    /// List X11 cursor sizes.
    static std::vector<unsigned> list_xcursor_sizes(std::istream & is);

    /// List Windows .cur file sizes.
    static std::vector<unsigned> list_cur_sizes(std::ifstream & is);

    /// List Windows animated cursor sizes.
    static std::vector<unsigned> list_ani_sizes(std::ifstream & is);

protected:

    struct Cur {
        Pixmap_ptr  pix;
        Point       hotspot;
        unsigned    delay = 0;

        Cur() = default;

        Cur(Pixmap_ptr p, const Point & hs=Point(), unsigned d=0):
            pix(p),
            hotspot(hs),
            delay(d)
        {
        }
    };

    std::vector<Cur>    frames_;
    unsigned            delay_ = 0;

protected:

    virtual bool has_sys_handle() const = 0;
    virtual void sys_update() = 0;

private:

    // Remove all frames.
    void clear();

    // Check X11 cursor header.
    static bool check_xcursor_header(std::istream & is);

    // Load X11 cursor.
    void load_xcursor(std::ifstream & is, unsigned size_px);

    // Load Windows .cur file.
    void load_cur(std::ifstream & is, unsigned rate=0);

    // Check Windows animated cursor file header.
    static bool check_ani_header(std::ifstream & is);

    // Load Windows animated cursor file.
    void load_ani(std::ifstream & is);
};

} // namespace tau

#endif // TAU_CURSOR_IMPL_HH
