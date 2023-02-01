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

#include <tau/color.hh>
#include <tau/exception.hh>
#include <tau/fileinfo.hh>
#include <tau/locale.hh>
#include <tau/sys.hh>
#include <pixmap-impl.hh>
#include <fstream>

namespace {

inline uint32_t u32(const char * b) {
    return uint8_t(b[0])|(uint32_t(uint8_t(b[1])) << 8)|(uint32_t(uint8_t(b[2])) << 16)|(uint32_t(uint8_t(b[3])) << 24);
}

inline uint32_t u24(const char * b) {
    return uint8_t(b[0])|(uint32_t(uint8_t(b[1])) << 8)|(uint32_t(uint8_t(b[2])) << 16);
}

inline uint16_t u16(const char * b) {
    return uint8_t(b[0])|(uint16_t(uint8_t(b[1])) << 8);
}

} // anonymous namespace

namespace tau {

Pixmap_ptr Pixmap_impl::load_bmp_from_memory(const char * raw, std::size_t bytes, bool has_mask) {
    std::size_t hdr_size = u32(raw);

    int iwidth, iheight;
    std::size_t bpp, comp, n_colors = 0;
    Vector ppi(72, 72);

    if (12 == hdr_size) {
        iwidth = u16(raw+4);
        iheight = u16(raw+6);
        bpp = u16(raw+10);
        if (bpp <= 8) { n_colors = 1 << bpp; }
    }

    else if (40 == hdr_size || 108 == hdr_size || 124 == hdr_size) {
        iwidth = u32(raw+4);
        iheight = u32(raw+8);
        bpp = u16(raw+14);
        comp = u32(raw+16);
        if (0 != comp && 3 != comp && 6 != comp) { throw bad_pixmap(str_format("BMP: compression type ", comp, " not supported")); }
        uint32_t xppm = u32(raw+24), yppm = u32(raw+28);
        ppi.set(double(xppm)/1000.0*25.4, double(yppm)/1000.0*25.4);
        n_colors = u32(raw+32);
        if (40 == hdr_size && (3 == comp || 6 == comp)) { hdr_size += 3 == comp ? 12 : 16; }
    }

    else {
        throw bad_pixmap(str_format("BMP: header size ", hdr_size, " not supported"));
    }

    if (iwidth > 0 && 0 != iheight) {
        if (has_mask) { iheight /= 2; }
        std::size_t height = abs(iheight);
        std::size_t stride = ~3 & ((iwidth*bpp+31)/8), bits_len = stride*height;
        std::size_t mask_stride = has_mask ? (~3 & (iwidth+31)/8) : 0, mask_bytes = height*mask_stride;
        std::size_t color_bytes = bytes-hdr_size-bits_len-mask_bytes;
        std::vector<uint32_t> colors;
        bool is_gray = true;

        if (0 != color_bytes && 0 == n_colors) {
            n_colors = color_bytes >> 2;
        }

        if (0 != n_colors) {
            colors.assign(n_colors, 0);
            const char * p = raw+hdr_size;

            for (unsigned n = 0; n < n_colors; ++n) {
                uint32_t c = u32(p);
                colors[n] = c;
                p += 4;
                uint8_t r = c >> 16, g = c >> 8, b = c;
                if (r != g || r != b || g != b) { is_gray = false; }
            }
        }

        const char * bits = raw+hdr_size+color_bytes;
        const char * mask = 0 != mask_bytes ? bits+bits_len : nullptr;
        Pixmap_ptr pix;

        if (1 == bpp) {
            int depth = has_mask ? 32 : 1;
            pix = Pixmap_impl::create(depth, iwidth, height);

            for (std::size_t y = 0; y < height; ++y) {
                const char * p = bits+stride*y;
                const char * m = mask ? mask+mask_stride*y : nullptr;

                for (int x = 0; x < iwidth; ++x) {
                    unsigned sh = 7-(7 & x), offset = x >> 3;
                    uint8_t index = (p[offset] & (1 << sh)) ? 1 : 0;
                    int yp = iheight >= 0 ? height-y-1 : y;
                    Color c;
                    if (m && (m[offset] & (1 << sh))) { c = Color("Black", 0.0); }
                    else { c = Color::from_rgb24(n_colors >= 2 ? colors[index] : (0 != index ? 0x00ffffff : 0)); }
                    pix->put_pixel(x, yp, c);
                }
            }
        }


        else if (4 == bpp) {
            int depth = has_mask ? 32 : (is_gray ? 8 : 24);
            pix = Pixmap_impl::create(depth, iwidth, height);

            for (std::size_t y = 0; y < height; ++y) {
                const char * m = mask ? mask+mask_stride*y : nullptr;
                int yp = iheight >= 0 ? height-y-1 : y;

                for (int x = 0; x < iwidth; ++x) {
                    const char * p = bits+(stride*y)+x/2;
                    unsigned sh = 7-(7 & x), mask_offset = x >> 3;
                    unsigned index = (1 & x) ? (*p & 0x0f) : (*p >> 4);
                    Color c;
                    if (m && (m[mask_offset] & (1 << sh))) { c = Color("Black", 0.0); }
                    else { c = index < n_colors ? Color::from_rgb24(colors[index]) : Color::from_gray8(index); }
                    pix->put_pixel(x, yp, c);
                }
            }
        }

        else if (8 == bpp) {
            int depth = has_mask ? 32 : (is_gray ? 8 : 24);
            pix = Pixmap_impl::create(depth, iwidth, height);

            for (std::size_t y = 0; y < height; ++y) {
                const char * m = mask ? mask+mask_stride*y : nullptr;
                int x = 0;

                while (x < iwidth) {
                    unsigned sh = 7-(7 & x), offset = x >> 3;
                    uint8_t index = *bits++;
                    int yp = iheight >= 0 ? height-y-1 : y;
                    Color c;
                    if (m && (m[offset] & (1 << sh))) { c = Color("Black", 0.0); }
                    else { c = n_colors >= 256 ? Color::from_rgb24(colors[index]) : Color::from_gray8(index); }
                    pix->put_pixel(x++, yp, c);
                }

                while (x & 3) { ++bits; ++x; }
            }
        }

        else if (24 == bpp) {
            int depth = has_mask ? 32 : 24;
            pix = Pixmap_impl::create(depth, iwidth, height);

            for (std::size_t y = 0; y < height; ++y) {
                int x = 0;

                while (x < iwidth) {
                    pix->put_pixel(x++, iheight >= 0 ? height-y-1 : y, Color::from_rgb24(u24(bits)));
                    bits += 3;
                }

                while (x & 3) { ++bits; ++x; }
            }
        }

        else if (32 == bpp) {
            pix = Pixmap_impl::create(32, iwidth, height);

            for (std::size_t y = 0; y < height; ++y) {
                pix->set_argb32(Point(0, y), reinterpret_cast<const uint8_t *>(bits), 4*iwidth); bits += 4*iwidth;
            }
        }

        else {
            throw bad_pixmap(str_format("BMP: bits per pixel of ", bpp, " unsupported"));
        }

        pix->set_ppi(ppi);
        return pix;
    }

    return Pixmap_impl::create(1);
}

Pixmap_ptr Pixmap_impl::load_bmp_from_file(const ustring & path) {
    auto & io = Locale().iocharset();
    std::string lfp = io.is_utf8() ? std::string(path) : io.encode(path);
    std::ifstream is(lfp, std::ios::binary);
    if (!is.good()) { throw sys_error(path); }

    char b[14];
    is.read(b, 14);
    if (std::streamsize(14) != is.gcount()) { throw bad_pixmap("corrupted BMP file header"); }
    if (0x42 != b[0] || 0x4d != b[1]) { throw bad_pixmap(str_format(path, ": corrupted BMP file signature")); }
    is.seekg(0, std::ios::end);
    const std::streamsize bytes = is.tellg()-std::streamoff(14);
    is.seekg(14);

    if (bytes < 32768) {
        char v[bytes];
        is.read(v, bytes);
        if (std::streamsize(bytes) != is.gcount()) { throw bad_pixmap("corrupted BMP file data"); }
        return load_bmp_from_memory(v, bytes, false);
    }

    else {
        auto v = std::make_unique<char[]>(bytes);
        is.read(v.get(), bytes);
        if (bytes != is.gcount()) { throw bad_pixmap("corrupted BMP file data"); }
        auto pix = load_bmp_from_memory(v.get(), bytes, false);
        return pix;
    }
}

} // namespace tau

//END
