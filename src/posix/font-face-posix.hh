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

#ifndef TAU_FONT_FACE_HH
#define TAU_FONT_FACE_HH

#include "types-posix.hh"
#include <tau/contour.hh>
#include <tau/font.hh>
#include <glyph-impl.hh>
#include <mutex>
#include <map>

namespace tau {

class Master_glyph {
    std::vector<Contour> contours_;
    Rect bbox_;
    int  adv_ = 0;
    int  lsb_ = 0;

public:

    Master_glyph() = default;
    Master_glyph(const Master_glyph & other) = delete;
    Master_glyph & operator=(const Master_glyph & other) = delete;

    void set_bounds(const Point & top_left, const Point & bottom_right) {
        bbox_.set(top_left, bottom_right);
    }

    void set_advance(int adv) {
        adv_ = adv;
    }

    void set_lbearing(int lsb) {
        lsb_ = lsb;
    }

    void add_contour(const Contour & gctr) {
        if (!gctr.empty()) {
            contours_.push_back(gctr);
        }
    }

    Rect bounds() const {
        return bbox_;
    }

    int advance() const {
        return adv_;
    }

    int lbearing() const {
        return lsb_;
    }

    const std::vector<Contour> & contours() const {
        return contours_;
    }

    void merge(const Master_glyph & other) {
        for (auto ctr: other.contours_) {
            contours_.push_back(ctr);
        }
    }

    void translate(const Point & pt) {
        for (auto & ctr: contours_) {
            ctr.translate(pt);
        }
    }

    void scale(const Vector & vec) {
        for (auto & ctr: contours_) {
            ctr.scale(vec);
        }
    }

    Glyph_ptr glyph(const Vector & scale) {
        Glyph_ptr gl = std::make_shared<Glyph_impl>();

        gl->set_min(Vector(scale.x()*bbox_.left(), scale.y()*bbox_.top()));
        gl->set_max(Vector(scale.x()*bbox_.right(), scale.y()*bbox_.bottom()));
        gl->set_advance(Vector(scale.x()*adv_, 0.0));
        gl->set_bearing(Vector(scale.x()*lsb_, std::round(gl->min().y())));

        for (auto & mctr: contours_) {
            Contour ctr(mctr);
            ctr.scale(scale);
            gl->add_contour(ctr);
        }

        return gl;
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Font_face {
public:

    Font_face(Font_file_ptr file, Master_glyph_ptr zero);
    Font_face(const Font_face & other) = delete;
    Font_face(Font_face && other) = delete;
    Font_face & operator=(const Font_face & other) = delete;
    Font_face & operator=(Font_face && other) = delete;
   ~Font_face() {}

    Font_file_ptr font_file() { return file_; }
    Font_file_cptr font_file() const { return file_; }

    ustring family() const {
        return family_;
    }

    ustring facename() const {
        return facename_;
    }

    ustring fontname() const {
        return fontname_;
    }

    ustring psname() const {
        return psname_;
    }

    Rect bounds() const {
        return bbox_;
    }

    unsigned upm() const {
        return upm_;
    }

    int ascent() const {
        return ascent_;
    }

    int descent() const {
        return descent_;
    }

    int linegap() const {
        return linegap_;
    }

    int max_advance() const {
        return max_advance_;
    }

    int min_lsb() const {
        return min_lsb_;
    }

    int min_rsb() const {
        return min_rsb_;
    }

    int max_x_extent() const {
        return max_x_extent_;
    }

    bool caret_slope_rise() const {
        return caret_slope_rise_;
    }

    bool caret_slope_run() const {
        return caret_slope_run_;
    }

    Master_glyph_ptr glyph(char32_t wc);

    void set_family(const ustring & family);
    void set_facename(const ustring & facename);
    void set_fontname(const ustring & name);
    void set_psname(const ustring & psname);
    void set_bounds(const Rect & bounds);
    void set_upm(unsigned upm);
    void set_ascent(int16_t asc);
    void set_descent(int16_t desc);
    void set_linegap(int16_t lgap);
    void set_max_advance(uint16_t adv);
    void set_min_lsb(int16_t lsb);
    void set_min_rsb(int16_t rsb);
    void set_max_x_extent(int16_t ext);
    void set_caret_slope_rise(bool rise);
    void set_caret_slope_run(bool run);

private:

    void preload(char32_t first, char32_t last);

private:

    using Glyphs = std::map<char32_t, Master_glyph_ptr>;
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;

    ustring             family_;
    ustring             facename_;
    ustring             fontname_;
    ustring             psname_;

    Rect                bbox_;
    unsigned            upm_ = 0;
    int                 ascent_ = 0;
    int                 descent_ = 0;
    int                 linegap_ = 0;
    int                 max_advance_ = 0;
    int                 min_lsb_ = 0;
    int                 min_rsb_ = 0;
    int                 max_x_extent_ = 0;
    bool                caret_slope_rise_ = false;
    bool                caret_slope_run_ = false;
    mutable Mutex       mx_;
    Glyphs              glyphs_;
    Font_file_ptr       file_;
    Master_glyph_ptr    zero_;

    bool                xlatin_ = false;
    std::string         preloaded_;
};

} // namespace tau

#endif // TAU_FONT_FACE_HH
