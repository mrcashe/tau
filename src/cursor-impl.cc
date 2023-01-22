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
#include <tau/pixmap.hh>
#include <cursor-impl.hh>
#include <pixmap-impl.hh>
#include <climits>

namespace {

uint16_t u16(const char * b) {
    return uint8_t(b[0])|(uint16_t(uint8_t(b[1])) << 8);
}

uint32_t u32(const char * b) {
    return uint8_t(b[0])|(uint32_t(uint8_t(b[1])) << 8)|(uint32_t(uint8_t(b[2])) << 16)|(uint32_t(uint8_t(b[3])) << 24);
}

} // anonymous namespace

namespace tau {

//static
bool Cursor_impl::check_ani_header(std::ifstream & is) {
    char buf[12];
    is.read(buf, 12);
    if (std::streamsize(12) != is.gcount()) { return false; }
    if ("RIFF" != std::string(buf, 4)) { return false; }
    std::streamsize res_bytes = u32(buf+4);
    is.seekg(0, std::ios::end);
    if (res_bytes > is.tellg()) { return false; }
    if ("ACON" != std::string(buf+8, 4)) { return false; }
    is.seekg(12);
    return true;
}

//static
std::vector<unsigned> Cursor_impl::list_ani_sizes(std::ifstream & is) {
    std::vector<unsigned> v;
    bool is_cursor = false;

    if (check_ani_header(is)) {
        while (!is.eof()) {
            char buf[36];
            is.read(buf, 8);

            if (!is.eof()) {
                if (std::streamsize(8) != is.gcount()) { return v; }
                std::string chunk(buf, 4);
                std::streamoff cbytes = u32(buf+4), cnext = is.tellg()+cbytes;

                if ("LIST" == chunk) {
                    is.read(buf, 4);
                    if (std::streamsize(4) != is.gcount()) { return v; }
                    std::string sub_chunk(buf, 4);

                    if ("fram" == sub_chunk) {
                        while (is.tellg() < cnext) {
                            is.read(buf, 8);
                            if (std::streamsize(8) != is.gcount()) { return v; }
                            std::string ico_chunk(buf, 4);
                            if ("icon" != ico_chunk) { return v; }
                            std::streamoff ico_bytes = u32(buf+4), ico_next = is.tellg()+ico_bytes;

                            if (0 != ico_bytes) {
                                if (is_cursor) {
                                    is.read(buf, 6);
                                    if (std::streamsize(6) != is.gcount()) { return v; }
                                    if (0x00020000 != u32(buf)) { return v; }
                                    uint16_t img_count = u16(buf+4);
                                    if (0 == img_count) { return v; }

                                    for (uint16_t n = 0; n < img_count; ++n) {
                                        is.read(buf, 16);
                                        if (std::streamsize(16) != is.gcount()) { return v; }
                                        unsigned w = buf[0], h = buf[1], size = std::max(w, h);
                                        if (v.end() == std::find(v.begin(), v.end(), size)) { v.push_back(size); }
                                    }

                                    is.seekg(ico_next);
                                    if (ico_next != is.tellg()) { return v; }
                                }

                                // Raw bitmap yet not supported.
                                else {
                                    return v;
                                }
                            }
                        }
                    }

                    else {
                        is.seekg(cnext);
                    }

                    if (cnext != is.tellg()) { return v; }
                }

                else if ("anih" == chunk) {
                    if (36 != cbytes) {  return v; }
                    is.read(buf, 36);
                    if (std::streamsize(36) != is.gcount()) { throw bad_pixmap("currupted ANI header"); }
                    uint32_t flags = u32(buf+32);
                    is_cursor = 1 & flags;
                }

                else {
                    is.seekg(cnext);
                    if (cnext != is.tellg()) { return v; }
                }
            }
        }
    }

    return v;
}

void Cursor_impl::load_ani(std::ifstream & is) {
    char buf[36];
    std::size_t n_frames = 0;
    //std::size_t n_steps = 0;
    //std::size_t n_bits = 0;
    //std::size_t n_planes = 0;
    //std::size_t hdr_width = 0;
    //std::size_t hdr_height = 0;
    std::size_t rate = 0;
    bool is_cursor = false;
    std::vector<uint32_t> rates;

    if (!check_ani_header(is)) {
        throw bad_pixmap("corrupted ANI header");
    }

    while (!is.eof()) {
        is.read(buf, 8);

        if (!is.eof()) {
            std::string chunk(buf, 4);
            std::streamoff cbytes = u32(buf+4), cnext = is.tellg()+cbytes;

            if ("LIST" == chunk) {
                is.read(buf, 4);
                if (std::streamsize(4) != is.gcount()) { throw bad_pixmap("ANI file: currupted 'LIST' chunk"); }
                std::string sub_chunk(buf, 4);

                if ("fram" == sub_chunk) {
                    if (0 == n_frames) { throw bad_pixmap("ANI file: misplaced 'anih' chunk?"); }

                    while (is.tellg() < cnext) {
                        is.read(buf, 8);
                        if (std::streamsize(8) != is.gcount()) { throw bad_pixmap("ANI file: currupted 'fram' subchunk"); }
                        std::string ico_chunk(buf, 4);
                        if ("icon" != ico_chunk) { throw bad_pixmap("ANI file: currupted icon header"); }
                        std::streamoff ico_bytes = u32(buf+4), ico_next = is.tellg()+ico_bytes;

                        if (0 != ico_bytes) {
                            if (is_cursor) {
                                load_cur(is, rate);
                                is.seekg(ico_next);
                                if (ico_next != is.tellg()) { throw bad_pixmap("ANI file: currupted icon data"); }
                            }

                            else {
                                throw bad_pixmap("ANI file: raw bitmap format not supported");
                            }
                        }
                    }
                }

                else {
                    is.seekg(cnext);
                }

                if (cnext != is.tellg()) { throw bad_pixmap("ANI file: seek over LIST chunk failed"); }
            }

            else if ("rate" == chunk) {
                if (0 != cbytes) {
                    char rbuf[cbytes];
                    is.read(rbuf, cbytes);
                    if (cbytes != is.gcount()) { throw bad_pixmap("ANI file: failed to load rates"); }
                    for (int n = 0; n < cbytes/4; ++n) { rates.push_back(u32(rbuf+4*n)); }
                }
            }

            else if ("anih" == chunk) {
                if (36 != cbytes) {  throw bad_pixmap("ANI header size mismatch"); }
                is.read(buf, 36);
                if (std::streamsize(36) != is.gcount()) { throw bad_pixmap("currupted ANI header"); }
                n_frames = u32(buf+4);
                //n_steps = u32(buf+8);
                //hdr_width = u32(buf+12);
                //hdr_height = u32(buf+16);
                //n_bits = u32(buf+20);
                //n_planes = u32(buf+24);
                rate = u32(buf+28);
                uint32_t flags = u32(buf+32);
                is_cursor = 1 & flags;
            }

            else {
                throw bad_pixmap(str_format("unknow ANI file chunk '", chunk, "'"));
            }
        }
    }

    if (!rates.empty()) {
        for (std::size_t n = 0; n < frames_.size() && n < rates.size(); ++n) {
            frames_[n].delay = 33*rates[n];
        }
    }
}

// Static.
std::vector<unsigned> Cursor_impl::list_cur_sizes(std::ifstream & is) {
    std::vector<unsigned> v;
    char buf[16];

    is.read(buf, 4);
    if (std::streamsize(4) != is.gcount()) { return v; }
    if (0x00020000 != u32(buf)) { return v; }

    is.read(buf, 2);
    if (std::streamsize(2) != is.gcount()) { return v; }
    uint16_t img_count = u16(buf);
    if (0 == img_count) { return v; }

    for (uint16_t n = 0; n < img_count; ++n) {
        is.read(buf, 16);
        if (std::streamsize(16) != is.gcount()) { return v; }
        unsigned w = static_cast<uint8_t>(buf[0]), h = static_cast<uint8_t>(buf[1]), size = std::max(w, h);
        if (v.end() == std::find(v.begin(), v.end(), size)) { v.push_back(size); }
    }

    return v;
}

void Cursor_impl::load_cur(std::ifstream & is, unsigned rate) {
    std::streamoff fstart = is.tellg();
    char buf[16];

    is.read(buf, 6);
    if (std::streamsize(6) != is.gcount()) { throw bad_pixmap("corrupted CUR header"); }
    if (0x00020000 != u32(buf)) { throw bad_pixmap("invalid CUR magic"); }
    uint16_t img_count = u16(buf+4);
    if (0 == img_count) { throw bad_pixmap("invalid CUR image count"); }

    struct Rec { uint8_t w, h, ncolors; uint16_t x_spot, y_spot; std::streamsize bytes; std::streamoff ofs; };
    std::vector<Rec> recs;

    for (uint16_t n = 0; n < img_count; ++n) {
        is.read(buf, 16);
        if (std::streamsize(16) != is.gcount()) { throw bad_pixmap("corrupted CUR record"); }
        recs.emplace_back();
        Rec & rec = recs.back();
        rec.w = buf[0];
        rec.h = buf[1];
        rec.ncolors = buf[2];
        rec.x_spot = u16(buf+4);
        rec.y_spot = u16(buf+6);
        rec.bytes = u32(buf+8);
        rec.ofs = fstart+u32(buf+12);
    }

    for (const Rec & rec: recs) {
        if (0 != rec.ncolors) { throw bad_pixmap("palette not supported within CUR file format"); }
        is.seekg(rec.ofs);
        if (std::streampos(rec.ofs) != is.tellg()) { throw bad_pixmap("file seek failed"); }

        Cur cur;
        cur.hotspot.set(rec.x_spot, rec.y_spot);
        cur.delay = 33*rate;

        if (rec.bytes < 32768) {
            char v[rec.bytes];
            is.read(v, rec.bytes);
            if (rec.bytes != is.gcount()) { throw bad_pixmap("corrupted CUR data"); }
            cur.pix = Pixmap_impl::load_bmp_from_memory(v, rec.bytes, true);
        }

        else {
            auto v = std::make_unique<char[]>(rec.bytes);
            is.read(v.get(), rec.bytes);
            if (rec.bytes != is.gcount()) { throw bad_pixmap("corrupted CUR data"); }
            cur.pix = Pixmap_impl::load_bmp_from_memory(v.get(), rec.bytes, true);
        }

        frames_.push_back(cur);
    }
}

// Static.
bool Cursor_impl::check_xcursor_header(std::istream & is) {
    char buf[4];

    is.read(buf, 4);
    if (std::streamsize(4) != is.gcount()) { return false; }
    if ("Xcur" != std::string(buf, 4)) { return false; }

    uint32_t hdr_bytes;
    is.read(buf, 4);
    if (std::streamsize(4) != is.gcount()) { return false; }
    hdr_bytes = u32(buf);
    if (16 != hdr_bytes) { return false; }

    uint32_t hver;
    is.read(buf, 4);
    if (std::streamsize(4) != is.gcount()) { return false; }
    hver = u32(buf);

    // Version is in form HIGH.LOW.
    // Only 1.0 currently supported.
    if (0x10000 != hver) { return false; }

    return true;
}

// static
std::vector<unsigned> Cursor_impl::list_xcursor_sizes(std::istream & is) {
    std::vector<unsigned> v;

    if (check_xcursor_header(is)) {
        char buf[32];
        uint32_t ntoc;
        is.read(buf, 4);
        if (std::streamsize(4) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR header"); }
        ntoc = u32(buf);

        for (uint32_t n = 0; n < ntoc; ++n) {
            is.read(buf, 12);
            if (std::streamsize(12) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR table of contents"); }
            unsigned size = u32(buf+4);
            if (v.end() == std::find(v.begin(), v.end(), size)) { v.push_back(size); }
        }
    }

    return v;
}

void Cursor_impl::load_xcursor(std::ifstream & is, unsigned size_px) {
    if (!check_xcursor_header(is)) {
        throw bad_pixmap("bad XCURSOR header");
    }

    char buf[32];
    uint32_t ntoc;
    is.read(buf, 4);
    if (std::streamsize(4) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR header"); }
    ntoc = u32(buf);

    struct Toc { uint32_t type, subtype, position; };
    std::vector<Toc> tocs;
    unsigned closest = UINT_MAX, dmax = UINT_MAX;

    for (uint32_t n = 0; n < ntoc; ++n) {
        is.read(buf, 12);
        if (std::streamsize(12) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR table of contents"); }
        tocs.emplace_back();
        Toc & toc = tocs.back();
        toc.type = u32(buf);
        toc.subtype = u32(buf+4);
        toc.position = u32(buf+8);
        unsigned d = toc.subtype < size_px ? size_px-toc.subtype : toc.subtype-size_px;

        if (UINT_MAX == closest) {
            closest = toc.subtype;
            dmax = d;
        }

        else if (d < dmax) {
            closest = toc.subtype;
            dmax = d;
        }
    }

    if (UINT_MAX != closest) {
        for (const Toc & toc: tocs) {
            if (closest == toc.subtype) {
                is.seekg(toc.position);
                if (std::streampos(toc.position) != is.tellg()) { throw bad_pixmap("XCURSOR: file seek failed"); }
                is.read(buf, 16);
                if (std::streamsize(16) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR chunk"); }
                uint32_t version = u32(buf+12);
                if (1 != version) { throw bad_pixmap(str_format("unsupported XCURSOR chunk version ", version)); }
                uint32_t type = u32(buf+4);
                uint32_t subtype = u32(buf+8);
                if (toc.type != type || toc.subtype != subtype) { throw bad_pixmap("XCURSOR chunk type mismatch"); }

                if (0xfffd0002 == type) {
                    is.read(buf, 20);
                    if (std::streamsize(20) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR image header"); }
                    uint32_t width = u32(buf);
                    uint32_t height = u32(buf+4);
                    if (width > 0x7fff || height > 0x7fff) { throw bad_pixmap("corrupted XCURSOR image header"); }
                    uint32_t x_spot = u32(buf+8), y_spot = u32(buf+12), delay = u32(buf+16);
                    std::size_t nwords = width*height;
                    uint32_t v[nwords];
                    is.read(reinterpret_cast<char *>(v), 4*nwords);
                    if (std::streamsize(4*nwords) != is.gcount()) { throw bad_pixmap("corrupted XCURSOR data"); }

                    Cur cur;
                    cur.hotspot.set(x_spot, y_spot);
                    cur.delay = delay;
                    cur.pix = Pixmap_impl::create(32, width, height);
                    uint32_t * vp = v;

                    for (uint32_t y = 0; y < height; ++y) {
                        for (uint32_t x = 0; x < width; ++x) {
                            cur.pix->put_pixel(Point(x, y), Color::from_argb32(*vp++));
                        }
                    }

                    frames_.push_back(cur);
                }
            }
        }
    }
}

// static
void Cursor_impl::load(const ustring & path, unsigned size_px) {
    clear();
    std::ifstream is(Locale().io_encode(path), std::ios::binary);

    if (is.good()) {
        char buf[4];
        is.read(buf, 4);

        if (std::streamsize(4) == is.gcount()) {
            is.seekg(0);

            if ("Xcur" == std::string(buf, 4)) {
                load_xcursor(is, size_px);
                return;
            }

            if ("RIFF" == std::string(buf, 4)) {
                load_ani(is);
                return;
            }

            else if (0x00020000 == u32(buf)) {
                load_cur(is);
                return;
            }
        }
    }

    throw bad_pixmap(str_format(path, ": unknown cursor file format"));
}

void Cursor_impl::clear() {
    frames_.clear();
    delay_ = 0;
}

unsigned Cursor_impl::size() const {
    unsigned size = 0;

    for (unsigned n = 0; n < frames_.size(); ++n) {
        Pixmap_ptr pix = frames_[n].pix;
        if (pix) { size = std::max(size, pix->size().max()); }
    }

    return size;
}

int Cursor_impl::frame_count() const {
    return frames_.size();
}

Pixmap_ptr Cursor_impl::pixmap(int nth_frame) const {
    if (!frames_.empty() && nth_frame >= 0) {
        std::size_t max_index = std::min(frames_.size()-1, std::size_t(nth_frame));
        return frames_[max_index].pix;
    }

    return nullptr;
}

Point Cursor_impl::hotspot(int nth_frame) const {
    if (!frames_.empty() && nth_frame >= 0) {
        std::size_t max_index = std::min(frames_.size()-1, std::size_t(nth_frame));
        return frames_[max_index].hotspot;
    }

    return Point();
}

unsigned Cursor_impl::delay(int nth_frame) const {
    if (nth_frame >= 0) {
        std::size_t n = nth_frame;
        return n < frames_.size() ? frames_[n].delay : delay_;
    }

    return 0;
}

unsigned Cursor_impl::delay() const {
    return delay_;
}

void Cursor_impl::assign(Pixmap_cptr pix, const Point & hotspot) {
    clear();
    append(pix, 0, hotspot);
}

int Cursor_impl::append(Pixmap_cptr pix, unsigned delay_ms, const Point & hotspot) {
    frames_.emplace_back(Pixmap_impl::create(32, pix), hotspot, delay_ms);
    if (has_sys_handle()) { sys_update(); }
    return frames_.size()-1;
}

void Cursor_impl::prepend(Pixmap_cptr pix, unsigned delay_ms, const Point & hotspot) {
    frames_.emplace(frames_.begin(), Pixmap_impl::create(32, pix), hotspot, delay_ms);
    if (has_sys_handle()) { sys_update(); }
}

int Cursor_impl::insert(int nth_frame, Pixmap_cptr pix, unsigned delay_ms, const Point & hotspot) {
    int result = -1;

    if (nth_frame >= 0) {
        std::size_t n = nth_frame;

        if (n < frames_.size()) {
            frames_.emplace(frames_.begin()+nth_frame, Pixmap_impl::create(32, pix), hotspot, delay_ms);
            result = nth_frame;
        }

        else {
            frames_.emplace_back(Pixmap_impl::create(32, pix), hotspot, delay_ms);
            result = frames_.size()-1;
        }

        if (has_sys_handle()) {
            sys_update();
        }
    }

    return result;
}

void Cursor_impl::set_pixmap(Pixmap_cptr pix, int nth_frame) {
    if (nth_frame >= 0) {
        std::size_t n = nth_frame;

        if (n < frames_.size()) {
            Cur & c = frames_[n];
            c.pix = Pixmap_impl::create(32, pix);
            if (has_sys_handle()) { sys_update(); }
        }
    }
}

void Cursor_impl::set_delay(unsigned delay_ms, int nth_frame) {
    if (nth_frame >= 0) {
        std::size_t n = nth_frame;

        if (n < frames_.size()) {
            Cur & c = frames_[nth_frame];

            if (c.delay != delay_ms) {
                c.delay = delay_ms;
                if (has_sys_handle()) { sys_update(); }
            }
        }
    }
}

void Cursor_impl::set_hotspot(const Point & hotspot, int nth_frame) {
    if (nth_frame >= 0) {
        std::size_t n = nth_frame;

        if (n < frames_.size()) {
            Cur & c = frames_[nth_frame];

            if (c.hotspot != hotspot) {
                c.hotspot = hotspot;
                if (has_sys_handle()) { sys_update(); }
            }
        }
    }
}

void Cursor_impl::set_delay(unsigned delay_ms) {
    if (delay_ != delay_ms) {
        delay_ = delay_ms;

        if (frames_.size() > 1) {
            bool def_delay = true;

            for (auto & c: frames_) {
                if (0 != c.delay) {
                    def_delay = false;
                    break;
                }
            }

            if (!def_delay && has_sys_handle()) {
                sys_update();
            }
        }
    }
}

//static
Cursor_ptr Cursor_impl::create(Pixmap_ptr pix, const Point & hotspot) {
    auto cur = create();
    cur->assign(pix, hotspot);
    return cur;
}

// static
Cursor_ptr Cursor_impl::load_from_file(const ustring & path, unsigned size_px) {
    auto cur = create();
    cur->load(path, size_px);
    return cur;
}

// static
std::vector<unsigned> Cursor::list_sizes(const ustring & path) {
    std::ifstream is(Locale().io_encode(path), std::ios::binary);

    if (is.good()) {
        char buf[4];
        is.read(buf, 4);

        if (std::streamsize(4) == is.gcount()) {
            is.seekg(0);

            if ("Xcur" == std::string(buf, 4)) {
                return Cursor_impl::list_xcursor_sizes(is);
            }

            if ("RIFF" == std::string(buf, 4)) {
                return Cursor_impl::list_ani_sizes(is);
            }

            else if (0x00020000 == u32(buf)) {
                return Cursor_impl::list_cur_sizes(is);
            }
        }
    }

    return std::vector<unsigned>();
}

} // namespace tau

//END
