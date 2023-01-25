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

/// @file matrix.hh The Matrix class.

#ifndef TAU_MATRIX_HH
#define TAU_MATRIX_HH

#include <tau/geometry.hh>

namespace tau {

/// Transformation matrix.
/// @ingroup geometry_group
class Matrix {
public:

    /// Default constructor.
    Matrix() = default;

    /// Copy constructor.
    Matrix(const Matrix & other) = default;

    /// Copy operator.
    Matrix & operator=(const Matrix & other) = default;

    /// Constructor with transformation parameters.
    Matrix(double xx, double xy, double yx, double yy, double x0, double y0);

    /// Reset to identity.
    void reset();

    /// Translate by coordinates.
    void translate(double dx, double dy);

    /// Shear.
    void shear(double horiz, double vert);

    /// Rotate.
    void rotate(double radians);

    /// Scale.
    void scale(double sx, double sy);

    /// Scale.
    void scale(double s);

    /// Get inverted matrix.
    /// If it impossible to get inverted matrix, returns itself.
    Matrix inverted() const;

    /// Transform coordinates.
    Vector transform(const Vector & vec) const;

    /// Transform distance.
    Vector transform_distance(double dx, double dy) const;

    /// Transform distance.
    Vector transform_distance(const Vector & vec) const { return transform_distance(vec.x(), vec.y()); }

    /// Test if has unity scale.
    bool has_unity_scale() const;

    /// Test if identity.
    bool is_identity() const;

    /// Get parameter.
    double xx() const;

    /// Get parameter.
    double xy() const;

    /// Get parameter.
    double yx() const;

    /// Get parameter.
    double yy() const;

    /// Get parameter.
    double x0() const;

    /// Get parameter.
    double y0() const;

private:

    double xx_ = 1.0;
    double xy_ = 0.0;
    double yx_ = 0.0;
    double yy_ = 1.0;
    double x0_ = 0.0;
    double y0_ = 0.0;
};

/// Scale Vector by Matrix.
/// @ingroup geometry_group
/// @relates Matrix
Vector operator*(const Vector & vec, const Matrix & mat);

/// Scale Vector by Matrix.
/// @ingroup geometry_group
/// @relates Matrix
Vector operator*(const Matrix & mat, const Vector & vec);

/// Scale Vector by Matrix.
/// @ingroup geometry_group
/// @relates Matrix
Vector & operator*=(Vector & vec, const Matrix & mat);

} // namespace tau

#endif // TAU_MATRIX_HH
