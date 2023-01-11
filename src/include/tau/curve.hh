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

/// @file curve.hh The Curve class.

#ifndef TAU_CURVE_HH
#define TAU_CURVE_HH

#include <tau/geometry.hh>
#include <tau/matrix.hh>
#include <tau/types.hh>

namespace tau {

/// A Bezier curve.
/// @ingroup geometry_group
class Curve {
public:

    Curve(const Vector & end);
    Curve(const Vector & cp1, const Vector & end);
    Curve(const Vector & cp1, const Vector & cp2, const Vector & end);
    Curve(const Curve & other) = default;
    Curve & operator=(const Curve & other) = default;

    unsigned order() const;
    Vector end() const;
    Vector cp1() const;
    Vector cp2() const;

    void set(const Vector & end);
    void set(const Vector & cp1, const Vector & end);
    void set(const Vector & cp1, const Vector & cp2, const Vector & end);
    void set_cp1(const Vector & v);
    void set_cp2(const Vector & v);

    void scale(const Vector & v);
    void scale(double s);
    void scale(double sx, double sy, double sz=1.0);

    void translate(const Vector & v);
    void translate(double ox, double oy=0.0, double oz=0.0);

    void map(const Matrix & mat);

private:

    unsigned    order_;
    Vector      cp1_;
    Vector      cp2_;
    Vector      end_;
};

inline Curve & operator*=(Curve & cv, const Matrix & mat) {
    cv.map(mat);
    return cv;
}

} // namespace tau

#endif // TAU_CURVE_HH

