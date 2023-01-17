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

/// @file contour.hh The %Contour class.

#ifndef TAU_CONTOUR_HH
#define TAU_CONTOUR_HH

#include <tau/types.hh>
#include <tau/curve.hh>
#include <tau/geometry.hh>
#include <vector>

namespace tau {

/// %List of Bezier curves and a starting point.
///
/// @ingroup geometry_group
class Contour {
public:

    using list_type = std::vector<Curve>;
    using iterator = list_type::iterator;
    using const_iterator = list_type::const_iterator;

    /// @name Constructors
    /// @{

    /// Default constructor.
    /// @param start the start point.
    Contour(const Vector & start=Vector());

    /// Constructor with separated start coordinates.
    Contour(double sx, double sy, double sz=0.0);

    // Copy constructor.
    Contour(const Contour & other) = default;

    // Move constructor.
    Contour(Contour && other);

    /// Copy operator.
    Contour & operator=(const Contour & other) = default;

    /// Move operator.
    Contour & operator=(Contour && other);

    /// @}
    /// Create from rectangle.
    static Contour from_rectangle(const Vector & a, const Vector & b);

    /// Create from rectangle.
    static Contour from_rectangle(double x1, double y1, double x2, double y2);

    /// Get starting point.
    Vector start() const;

    /// Get access to curves.
    list_type & curves();

    /// Get access to curves.
    const list_type & curves() const;

    /// Get curve by index.
    Curve & operator[](std::size_t index);

    /// Get curve by index.
    const Curve & operator[](std::size_t index) const;

    /// Count curves.
    std::size_t size() const;

    /// Test if empty.
    bool empty() const;

    /// Add a 1st order curve.
    void line_to(const Vector & end);

    /// Add a 1st order curve.
    void line_to(double x, double y);

    /// Add quadratic curve.
    /// @param cp control point coordinates.
    /// @param end end point coordinates.
    void conic_to(const Vector & cp, const Vector & end);

    /// Add quadratic curve.
    /// @param cx control point x coordinate.
    /// @param cy control point y coordinate.
    /// @param ex end point x coordinate.
    /// @param ey end point y coordinate.
    void conic_to(double cx, double cy, double ex, double ey);

    /// Add cubic curve.
    /// @param cp1 control point 1 coordinates.
    /// @param cp2 control point 2 coordinates.
    /// @param end end point coordinates.
    void cubic_to(const Vector & cp1, const Vector & cp2, const Vector & end);

    /// Add cubic curve.
    /// @param cx1 control point 1 x coordinate.
    /// @param cy1 control point 1 y coordinate.
    /// @param cx2 control point 2 x coordinate.
    /// @param cy2 control point 2 y coordinate.
    /// @param ex end point x coordinate.
    /// @param ey end point y coordinate.
    void cubic_to(double cx1, double cy1, double cx2, double cy2, double ex, double ey);

    /// Translate contour by Vector.
    void translate(const Vector & pt);

    /// Translate contour.
    void translate(double ox, double oy=0.0, double oz=0.0);

    /// Scale contour.
    void scale(const Vector & vec);

    /// Scale contour.
    void scale(double s);

    /// Scale contour.
    void scale(double sx, double sy, double sz=1.0);

    /// Revert contour.
    void revert();

    /// Transform contour by matrix.
    void transform(const Matrix & mat);

    /// Get begin iterator.
    iterator begin();

    /// Get begin const iterator.
    const_iterator begin() const;

    /// Get end iterator.
    iterator end();

    /// Get end const iterator.
    const_iterator end() const;

    /// Get maximal order value.
    /// @return 0 if contour is empty or in range 1...3.
    unsigned order() const;

private:

    Vector start_;
    std::vector<Curve> curves_;
};

/// @relates Contour
inline Contour & operator*=(Contour & ctr, const Matrix & mat) {
    ctr.transform(mat);
    return ctr;
}

/// @relates Contour
inline Contour & operator*=(const Matrix & mat, Contour & ctr) {
    ctr.transform(mat);
    return ctr;
}

} // namespace tau

#endif // TAU_CONTOUR_HH
