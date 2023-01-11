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

#include <tau/buffer.hh>
#include <tau/color.hh>
#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/locale.hh>
#include <pixmap-impl.hh>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>

namespace tau {

static Pixmap_ptr load_xpm_from_buffer(Buffer buf) {
    std::unordered_map<std::string, uint32_t> colors;
    std::string varname;                // variable name.
    std::size_t width = 0;              // image width.
    std::size_t height = 0;             // image height.
    std::size_t ncolors = 0;            // number of colors.
    std::size_t nchars = 0;             // number of characters per color.
    std::vector<uint32_t> pixels;       // actual pixels.
    std::size_t pindex = 0;             // pixel addition index.
    bool has_alpha = false;             // alpha channel found.

    for (Buffer_iter cur = buf.begin(); cur != buf.end(); cur.move_forward_line()) {
        Buffer_iter end(cur);
        end.move_to_eol();
        ustring s = buf.text(cur, end);

        if (0 == width && 0 == height) {
            ustring::size_type asterisk = s.find('*'), left_square = s.find('['), right_square = s.find(']');

            if (ustring::npos != asterisk && ustring::npos != left_square && ustring::npos != right_square) {
                if (asterisk < left_square && left_square < right_square) {
                    varname = str_trim(s.substr(1+asterisk, left_square-asterisk-1));
                }
            }

            else {
                ustring::size_type open = s.find('"');

                if (ustring::npos != open) {
                    ustring::size_type close = s.find('"', 1+open);

                    if (ustring::npos != close) {
                        auto v = str_explode(str_trim(s.substr(1+open, close-open-1)), ' ');

                        if (4 == v.size()) {
                            try {
                                width = std::stoi(v[0], nullptr, 0);
                                height = std::stoi(v[1], nullptr, 0);
                                ncolors = std::stoi(v[2], nullptr, 0);
                                nchars = std::stoi(v[3], nullptr, 0);
                                pixels.assign(width*height, 0);
                            }

                            catch (std::exception & x) {
                                std::cerr << "** load_xpm_from_buffer(): std::exception caught: " << x.what() << std::endl;
                                return nullptr;
                            }
                        }
                    }
                }
            }
        }

        else if (colors.size() < ncolors) {
            ustring::size_type open = s.find('"');

            if (ustring::npos != open) {
                ustring::size_type close = s.find('"', 1+open);

                if (ustring::npos != close) {
                    ustring ss = s.substr(1+open, close-open-1);
                    auto v = str_explode(str_trim(ss), " \t");

                    if (char32_isblank(*ss.begin())) {
                        v.insert(v.begin(), ustring(nchars, ' '));
                    }

                    if (3 == v.size()) {
                        if (v[0].size() == nchars && "c" == v[1]) {
                            uint32_t w = 0;

                            if ('#' == v[2][0]) {
                                try {
                                    w = std::stoi(v[2].substr(1), nullptr, 16);
                                }

                                catch (std::exception & x) {
                                    std::cerr << "** load_xpm_from_buffer(): std::exception caught: " << x.what() << std::endl;
                                }

                                w |= 0xff000000;
                            }

                            else if (str_similar(v[2], "none")) {
                                has_alpha = true;
                            }

                            colors[v[0]] = w;
                        }
                    }
                }
            }
        }

        else if (0 != pixels.size()) {
            ustring::size_type open = s.find('"');

            if (ustring::npos != open) {
                ustring::size_type close = s.find('"', 1+open);

                if (ustring::npos != close) {
                    if (close-open-1 == width*nchars) {
                        ++open;

                        for (std::size_t n = 0; n < width; ++n, open += nchars) {
                            ustring val(s.substr(open, nchars));
                            uint32_t px = colors[val];
                            pixels[pindex++] = px;
                        }
                    }
                }
            }
        }
    }

    if (0 != width && 0 != height && pindex == width*height) {
        Pixmap_ptr pix;
        Size sz(width, height);
        int depth = has_alpha ? 32 : (2 == ncolors ? 1 : 24);
        pix = Pixmap_impl::create(depth, sz);
        pindex = 0;

        for (std::size_t y = 0; y < height; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                uint32_t w = pixels[pindex++];
                pix->put_pixel(Point(x, y), Color::from_argb32(w));
            }
        }

        return pix;
    }

    return nullptr;
}

Pixmap_ptr Pixmap_impl::load_xpm_from_file(const ustring & path) {
    Buffer buf = Buffer::load_from_file(path);
    auto pix = load_xpm_from_buffer(buf);
    if (pix) { return pix; }
    throw bad_pixmap(path);
}

Pixmap_ptr Pixmap_impl::load_xpm_from_string(const ustring & s) {
    return load_xpm_from_buffer(Buffer(s));
}

} // namespace tau

//END
