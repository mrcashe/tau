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

#include <tau/matrix.hh>
#include <cmath>

namespace tau {

Matrix::Matrix(double xx, double xy, double yx, double yy, double x0, double y0):
    xx_(xx),
    xy_(xy),
    yx_(yx),
    yy_(yy),
    x0_(x0),
    y0_(y0)
{
}

void Matrix::reset() {
    xx_ = yy_ = 1.0;
    xy_ = yx_ = x0_ = y0_ = 0.0;
}

void Matrix::translate(double dx, double dy) {
    if (1.0 != dx) { x0_ += xx_*dx; y0_ += dx*xy_; }
    if (1.0 != dy) { y0_ += yy_*dy; x0_ += dy*yx_; }
}

void Matrix::rotate(double radians) {
    double s = sin(radians);
    double c = cos(radians);

    double xx =  c*xx_+s*yx_;
    double xy =  c*xy_+s*yy_;
    double yx = -s*xx_+c*yx_;
    double yy = -s*xy_+c*yy_;

    xx_ = xx;
    xy_ = xy;
    yx_ = yx;
    yy_ = yy;
}

void Matrix::shear(double horiz, double vert) {
    double xx = yx_*vert;
    double xy = yy_*vert;
    double yx = xx_*horiz;
    double yy = xy_*horiz;

    xx_ += xx;
    xy_ += xy;
    yx_ += yx;
    yy_ += yy;
}

void Matrix::scale(double sx, double sy) {
    if (1.0 != sx) {
        xx_ *= sx;
        xy_ *= sx;
    }

    if (1.0 != sy) {
        yx_ *= sy;
        yy_ *= sy;
    }
}

void Matrix::scale(double s) {
    if (1.0 != s) {
        xx_ *= s;
        xy_ *= s;
        yx_ *= s;
        yy_ *= s;
    }
}

Vector Matrix::transform_distance(double dx, double dy) const {
    double x = xx_*dx+yx_*dy;
    double y = yy_*dy+xy_*dx;
    return Vector(x, y);
}

Vector Matrix::transform_distance(const Vector & vec) const {
    return transform_distance(vec.x(), vec.y());
}

Vector Matrix::transform(const Vector & vec) const {
    double x = xx_*vec.x()+yx_*vec.y()+x0_;
    double y = yy_*vec.y()+xy_*vec.x()+y0_;
    return Vector(x, y);
}

Matrix Matrix::inverted() const {
    double det = xx_*yy_-xy_*yx_;

    if (det) {
        double idet = 1.0/det;
        double xx = yy_*idet;
        double xy = xy_*idet;
        double yx = yx_*idet;
        double yy = xx_*idet;
        double x0 = idet*(yx_*y0_-yy_*x0_);
        double y0 = idet*(xy_*x0_-xx_*y0_);
        return Matrix(xx, xy, yx, yy, x0, y0);
    }

    return Matrix();
}

bool Matrix::is_identity() const {
    return 1.0 == xx_ && 1.0 == yy_ && 0.0 == xy_ && 0.0 == yx_ && 0.0 == x0_ && 0.0 == y0_;
}

bool Matrix::has_unity_scale() const {
    if (0.0 == xy_ && 0.0 == yx_) {
        if (!(1.0 == xx_ || -1.0 == xx_)) {
            return false;
        }

        if (!(1.0 == yy_ || -1.0 == yy_)) {
            return false;
        }
    }

    else if (0.0 == xx_ && 0.0 == yy_) {
        if (!(1.0 == xy_ || -1.0 == xy_)) {
            return false;
        }

        if (!(1.0 == yx_ || -1.0 == yx_)) {
            return false;
        }
    }

    else {
        return false;
    }

    return true;
}

double Matrix::xx() const {
    return xx_;
}

double Matrix::xy() const {
    return xy_;
}

double Matrix::yx() const {
    return yx_;
}

double Matrix::yy() const {
    return yy_;
}

double Matrix::x0() const {
    return x0_;
}

double Matrix::y0() const {
    return y0_;
}

Vector operator*(const Vector & vec, const Matrix & mat) {
    return mat.transform(vec);
}

Vector operator*(const Matrix & mat, const Vector & vec) {
    return mat.transform(vec);
}

Vector & operator*=(Vector & vec, const Matrix & mat) {
    vec = mat.transform(vec);
    return vec;
}

} // namespace tau

//END
