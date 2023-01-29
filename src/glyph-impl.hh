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

#ifndef TAU_GLYPH_IMPL_HH
#define TAU_GLYPH_IMPL_HH

#include <tau/contour.hh>
#include <tau/glyph.hh>
#include <types-impl.hh>

namespace tau {

class Glyph_impl {
public:

    static Glyph wrap(Glyph_ptr gl) { return Glyph(gl); }
    static Glyph_ptr strip(Glyph g) { return g.impl; }

    Glyph_impl() = default;
    Vector bearing() const { return bearing_; }
    Vector advance() const { return advance_; }
    Vector min() const { return min_; }
    Vector max() const { return max_; }
    std::vector<Contour> contours() const { return contours_; }
    void set_min(const Vector & min) { min_ = min; }
    void set_max(const Vector & max) { max_ = max; }
    void set_advance(const Vector & adv) { advance_ = adv; }
    void set_bearing(const Vector & bear) { bearing_ = bear; }
    void add_contour(const Contour & ctr) { contours_.push_back(ctr); }

    Rect bounds() const {
        if (contours().empty()) {
            Size adv = advance();

            if (0 == adv.height() && 0 == adv.width()) {
                return Rect();
            }

            if (0 != adv.height() && 0 != adv.width()) {
                return Rect(0, 0, adv);
            }

            if (0 != adv.width()) {
                return Rect(0, 0, Size(adv.width(), 1));
            }

            return Rect(0, 0, Size(1, adv.height()));
        }

        int xpmin = std::floor(min_.x()), xpmax = std::ceil(max_.x());
        int ypmin = std::floor(min_.y()), ypmax = std::ceil(max_.y());
        return Rect(Point(xpmin, ypmin), Point(xpmax, ypmax));
    }

private:

    using Contours = std::vector<Contour>;

    Vector      min_, max_;     // Scaled real bounds ((xmin:ymin), (xmax:ymax)).
    Vector      advance_;       // Scaled real advance x & y.
    Vector      bearing_;       // Scaled real bearing left (x) & top (y).
    Contours    contours_;
};

} // namespace tau

#endif // TAU_GLYPH_IMPL_HH
