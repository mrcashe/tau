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

#include <tau/curve.hh>
#include <tau/matrix.hh>

namespace tau {

Curve::Curve(const Vector & end):
    order_(1),
    end_(end)
{
}

Curve::Curve(const Vector & cp1, const Vector & end):
    order_(2),
    cp1_(cp1),
    end_(end)
{
}

Curve::Curve(const Vector & cp1, const Vector & cp2, const Vector & end):
    order_(3),
    cp1_(cp1),
    cp2_(cp2),
    end_(end)
{
}

unsigned Curve::order() const {
    return order_;
}

Vector Curve::end() const {
    return end_;
}

Vector Curve::cp1() const {
    return cp1_;
}

Vector Curve::cp2() const {
    return cp2_;
}

void Curve::set_cp1(const Vector & v) {
    cp1_ = v;
}

void Curve::set_cp2(const Vector & v) {
    cp2_ = v;
}

void Curve::assign(const Vector & end) {
    order_ = 1;
    end_ = end;
}

void Curve::assign(const Vector & cp1, const Vector & end) {
    order_ = 2;
    cp1_ = cp1;
    end_ = end;
}

void Curve::assign(const Vector & cp1, const Vector & cp2, const Vector & end) {
    order_ = 3;
    cp1_ = cp1;
    cp2_ = cp2;
    end_ = end;
}

void Curve::scale(const Vector & v) {
    end_ *= v;
    cp1_ *= v;
    cp2_ *= v;
}

void Curve::scale(double s) {
    scale(Vector(s, s, s));
}

void Curve::scale(double sx, double sy, double sz) {
    scale(Vector(sx, sy, sz));
}

void Curve::translate(const Vector & v) {
    end_ += v;
    cp1_ += v;
    cp2_ += v;
}

void Curve::translate(double ox, double oy, double oz) {
    translate(Vector(ox, oy, oz));
}

void Curve::transform(const Matrix & mat) {
    end_ *= mat;
    cp1_ *= mat;
    cp2_ *= mat;
}

Curve & operator*=(Curve & cv, const Matrix & mat) {
    cv.transform(mat);
    return cv;
}

} // namespace tau

//END
