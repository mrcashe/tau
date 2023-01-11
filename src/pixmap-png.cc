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

#include <tau/color.hh>
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <pixmap-impl.hh>
#include <cstdio>
#include <iostream>
#include <png.h>

namespace tau {

Pixmap_ptr Pixmap_impl::load_png_from_file(const ustring & path) {
    std::string lfp = Locale().encode_filename(path);
    FILE * is = std::fopen(lfp.c_str(), "rb");
    if (!is) { throw sys_error(); }
    uint8_t header[8];
    if (8 != std::fread(header, 1, 8, is)) { throw bad_pixmap(str_format(path, ": bad PNG header")); }
    if (png_sig_cmp(header, 0, 8)) { throw bad_pixmap(str_format(path, ": not a PNG file")); }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) { throw bad_pixmap(str_format(path, ": PNG: png_create_read_struct failed")); }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) { throw bad_pixmap(str_format(path, ": PNG: png_create_info_struct failed")); }

    if (setjmp(png_jmpbuf(png_ptr))) { throw bad_pixmap(str_format(path, ": PNG: error during init_io")); }
    png_init_io(png_ptr, is);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    unsigned ctype = png_get_color_type(png_ptr, info_ptr);
    unsigned bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (PNG_COLOR_TYPE_PALETTE == ctype) { png_set_palette_to_rgb(png_ptr); }
    if (16 == bit_depth) { png_set_strip_16(png_ptr); }
    png_read_update_info(png_ptr, info_ptr);
    ctype = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    if (8 != bit_depth) { throw bad_pixmap(str_format(path, ": PNG: failed to set 8 bit depth")); }
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    png_bytep * row_pointers = new png_bytep[height];
    for (int y = 0; y < height; ++y) { row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)]; }
    png_read_image(png_ptr, row_pointers);

    Pixmap_ptr pix;

    if (PNG_COLOR_TYPE_GRAY == ctype) {
        pix = Pixmap_impl::create(8, Size(width, height));
    }

    else if (PNG_COLOR_TYPE_RGB_ALPHA == ctype || PNG_COLOR_TYPE_GRAY_ALPHA == ctype) {
        pix = Pixmap_impl::create(32, Size(width, height));
    }

    else {
        pix = Pixmap_impl::create(24, Size(width, height));
    }

    pix->set_ppi(Vector(png_get_x_pixels_per_inch(png_ptr, info_ptr), png_get_y_pixels_per_inch(png_ptr, info_ptr)));

    for (int y = 0; y < height; ++y) {
        png_byte * row = row_pointers[y];

        for (int x = 0; x < width; ++x) {
            if (PNG_COLOR_TYPE_RGB_ALPHA == ctype) {
                png_byte * p = row+(x*4);
                uint32_t r = p[0], g = p[1], b = p[2], a = p[3];
                pix->put_pixel(x, y, Color::from_argb32((a << 24)|(r << 16)|(g << 8)|b));
            }

            else if (PNG_COLOR_TYPE_RGB == ctype) {
                png_byte * p = row+(x*3);
                uint32_t r = p[0], g = p[1], b = p[2];
                pix->put_pixel(x, y, Color::from_rgb24((r << 16)|(g << 8)|b));
            }

            else if (PNG_COLOR_TYPE_GRAY == ctype) {
                png_byte * p = row+x;
                pix->put_pixel(x, y, Color::from_gray8(*p));
            }

            else if (PNG_COLOR_TYPE_GRAY_ALPHA == ctype) {
                png_byte * p = row+(x*2);
                pix->put_pixel(x, y, Color::from_gray8(p[0], p[1]/255.0));
            }

            else {
                throw bad_pixmap(str_format(path, ": PNG: color type of ", ctype, " unsupported"));
            }
        }
    }

    for (int y = 0; y < height; ++y) { delete row_pointers[y]; }
    delete[] row_pointers;
    png_read_end(png_ptr, nullptr);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    std::fclose(is);
    return pix;
}

} // namespace tau

//END
