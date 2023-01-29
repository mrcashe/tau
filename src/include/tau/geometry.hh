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

#ifndef TAU_GEOMETRY_HH
#define TAU_GEOMETRY_HH

/// @file geometry.hh Vector, Point, Size, Rect classes and related stuff.

#include <tau/types.hh>
#include <algorithm>
#include <cmath>
#include <vector>

namespace tau {

/// The 2D/3D point with floating point coordinates.
/// @ingroup geometry_group
class Vector {
public:

    /// Default constructor.
    Vector() = default;

    /// Copy cnstructor.
    Vector(const Vector & other) = default;

    /// Copy operator.
    Vector & operator=(const Vector & other) = default;

    /// Constructor with Point.
    Vector(const Point & pt);

    /// 2D constructor.
    Vector(double x, double y);

    /// 3D constructor.
    Vector(double x, double y, double z);

    /// Get x coordinate.
    double x() const;

    /// Get y coordinate.
    double y() const;

    /// Get z coordinate.
    double z() const;

    /// Add other vector.
    void operator+=(const Vector & other);

    /// Subtract other vector.
    void operator-=(const Vector & other);

    /// Multiply by scale factor.
    void operator*=(double s);

    /// Multiply by other vector.
    void operator*=(const Vector & other);

    /// Divide by scale factor.
    void operator/=(double s);

    /// Divide by other vector.
    void operator/=(const Vector & other);

    /// Set x value.
    void set_x(double x);

    /// Set y value.
    void set_y(double y);

    /// Set z value.
    void set_z(double z);

    /// Set x and y values.
    void set(double x, double y);

    /// Set all values.
    void set(double x, double y, double z);

    /// Set from Point.
    void set(const Point & pt);

    /// Set from Size.
    void set(const Size & sz);

    /// Reset coordinates to 0.0.
    void reset();

    /// Returns the length of the vector.
    double length() const;

    /// Returns normalized vector.
    Vector normalized() const;

private:

    double x_ = 0.0;
    double y_ = 0.0;
    double z_ = 0.0;
};

/// @relates Vector
Vector operator+(const Vector & lhs, const Vector & rhs);

/// @relates Vector
Vector operator-(const Vector & lhs, const Vector & rhs);

/// @relates Vector
Vector operator*(const Vector & lhs, const Vector & rhs);

/// @relates Vector
Vector operator*(const Vector & v, double s);

/// @relates Vector
Vector operator*(double s, const Vector & v);

/// @relates Vector
Vector operator/(const Vector & lhs, const Vector & rhs);

/// @relates Vector
Vector operator/(const Vector & lhs, double s);

/// The 2D/3D point with integer coordinates.
/// @ingroup geometry_group
class Point {
public:

    /// Default constructor.
    Point() = default;

    /// Copy constructor.
    Point(const Point & other) = default;

    /// Copy operator.
    Point & operator=(const Point & other) = default;

    /// 2D constructor.
    Point(int x, int y);

    /// 3D constructor.
    Point(int x, int y, int z);

    /// Constructor with Vector.
    Point(const Vector & vec);

    /// Assignment from Vector.
    Point & operator=(const Vector & vec);

    /// 2D setter.
    Point & set(int x, int y);

    /// 3D setter.
    Point & set(int x, int y, int z);

    /// Set values from other point.
    Point & set(const Point & pt);

    /// Set values from Vector.
    Point & set(const Vector & vec);

    /// 2D updater.
    /// @return true if values changed.
    bool update(int x, int y);

    /// 3D updater.
    /// @return true if values changed.
    bool update(int x, int y, int z);

    /// Update from other point.
    /// @return true if values changed.
    bool update(const Point & py);

    /// Update from Vector.
    /// @return true if values changed.
    bool update(const Vector & vec);

    /// Update x coordinate.
    /// @return true if value changed.
    bool update_x(int x);

    /// Update y coordinate.
    /// @return true if value changed.
    bool update_y(int y);

    /// Update z coordinate.
    /// @return true if value changed.
    bool update_z(int z);

    /// Move the point along the x and y axes.
    /// @return true if values changed.
    bool translate(int dx, int dy);

    /// Move the point along the x, y and z axes.
    /// @return true if values changed.
    bool translate(int dx, int dy, int dz);

    /// Move the point along the x and y axes.
    /// @return true if values changed.
    bool translate(const Point & other);

    /// X coordinate accessor.
    int x() const;

    /// Y coordinate accessor.
    int y() const;

    /// Z coordinate accessor.
    int z() const;

    /// Reset coordinates to 0.
    void reset();

    /// Transpose coordinates.
    Point & operator+=(const Point & p);

    /// Transpose coordinates.
    Point & operator-=(const Point & p);

    /// Scale coordinates.
    Point & operator*=(double m);

    /// Scale coordinates.
    Point & operator*=(const Vector & v);

private:

    int x_ = 0;
    int y_ = 0;
    int z_ = 0;
};

/// @relates Point
Point operator*(const Point & p, double m);

/// @relates Point
Point operator*(double m, const Point & p);

/// @relates Point
Point operator+(const Point & p, const Point & q);

/// @relates Point
Point operator-(const Point & p, const Point & q);

/// @relates Point
Point operator-(const Point & p);

/// @relates Point
bool operator==(const Point & p, const Point & q);

/// @relates Point
bool operator!=(const Point & p, const Point & q);

/// The 2D/3D size with unsigned integer values.
/// @ingroup geometry_group
class Size {
public:

    Size() = default;
    Size(const Size & other) = default;
    Size & operator=(const Size & other) = default;

    Size(unsigned width, unsigned height);

    Size(unsigned width, unsigned height, unsigned depth);

    Size(unsigned all);

    /// Constructs from Vector.
    Size(const Vector & v);

    void set(const Size & other);

    void set(unsigned width, unsigned height);

    void set(unsigned width, unsigned height, unsigned depth);

    unsigned width() const;

    unsigned height() const;

    unsigned depth() const;

    int iwidth() const;

    int iheight() const;

    int idepth() const;

    /// Update width.
    /// @return @b true if changed.
    bool update_width(unsigned width);

    /// Update height.
    /// @return @b true if changed.
    bool update_height(unsigned height);

    /// Update depth.
    /// @return @b true if changed.
    bool update_depth(unsigned depth);

    /// Update width, height and depth.
    bool update(unsigned all);

    /// Update width and height.
    /// @return @b true if changed.
    bool update(unsigned width, unsigned height);

    /// Update width, height and depth.
    /// @return @b true if changed.
    bool update(unsigned width, unsigned height, unsigned depth);

    /// Update all.
    /// @param sz other size.
    /// @param nz update values only if new values not equals to 0.
    /// @return @b true if changed.
    bool update(const Size & sz, bool nz=false);

    /// Update all.
    /// @return @b true if changed.
    bool update(const Vector & vec);

    /// Update if given value greater.
    /// @return @b true if changed.
    bool update_max_width(unsigned width);

    /// Update if given value greater.
    /// @return @b true if changed.
    bool update_max_height(unsigned height);

    /// Update if given value greater.
    /// @return @b true if changed.
    bool update_max_depth(unsigned depth);

    /// Update if given values greater.
    bool update_max(unsigned all);

    /// Update width and height if given values greater.
    /// @return @b true if changed.
    bool update_max(unsigned width, unsigned height);

    /// Update width, height and depth if given values greater.
    /// @return @b true if changed.
    bool update_max(unsigned width, unsigned height, unsigned depth);

    /// Update all if given values greater.
    /// @param sz other size.
    /// @param nz update values only if new values not equals to 0.
    /// @return @b true if changed.
    bool update_max(const Size & sz, bool nz=false);

    /// Update all if given values greater.
    /// @return @b true if changed.
    bool update_max(const Vector & vec);

    /// Update width if given value less.
    /// @return @b true if changed.
    bool update_min_width(unsigned width);

    /// Update height if given value less.
    /// @return @b true if changed.
    bool update_min_height(unsigned height);

    /// Update depth if given value less.
    /// @return @b true if changed.
    bool update_min_depth(unsigned depth);

    /// Update width, height and depth if given values less.
    bool update_min(unsigned all);

    /// Update width and height if given values less.
    /// @return @b true if changed.
    bool update_min(unsigned width, unsigned height);

    /// Update width, height and depth if given values less.
    /// @return @b true if changed.
    bool update_min(unsigned width, unsigned height, unsigned depth);

    /// Update all if given value less.
    /// @param sz other size.
    /// @param nz update values only if new values not equals to 0.
    /// @return @b true if changed.
    bool update_min(const Size & sz, bool nz=false);

    /// Update all if given value less.
    /// @return @b true if changed.
    bool update_min(const Vector & vec);

    /// Increase 2D size.
    /// @return @b true if changed.
    bool increase(unsigned dx, unsigned dy);

    /// Increase 3D size.
    /// @return @b true if changed.
    bool increase(unsigned dx, unsigned dy, unsigned dz);

    /// Increase size.
    /// @return @b true if changed.
    bool increase(const Size & sz);

    /// Decrease 2D size.
    /// @return @b true if changed.
    bool decrease(unsigned dx, unsigned dy);

    /// Decrease 3D size.
    /// @return @b true if changed.
    bool decrease(unsigned dx, unsigned dy, unsigned dz);

    /// Decrease size.
    /// @return @b true if changed.
    bool decrease(const Size & sz);

    /// Grow 2D size.
    /// @return @b true if changed.
    bool grow(int dx, int dy);

    /// Grow 3D size.
    /// @return @b true if changed.
    bool grow(int dx, int dy, int dz);

    /// Make increased size.
    Size increased(unsigned dx, unsigned dy) const;

    /// Make increased size.
    Size increased(unsigned dx, unsigned dy, unsigned dz) const;

    /// Make increased size.
    Size increased(const Size & sz) const;

    /// Make decreased size.
    Size decreased(unsigned dx, unsigned dy) const;

    /// Make decreased size.
    Size decreased(unsigned dx, unsigned dy, unsigned dz) const;

    /// Make decreased size.
    Size decreased(const Size & sz) const;

    /// Make grown size.
    Size grown(int dx, int dy) const;

    /// Make grown size.
    Size grown(int dx, int dy, int dz) const;

    /// 2D check empty.
    bool empty() const;

    /// 3D check empty.
    bool empty3() const;

    /// Reset values to 0.
    void reset();

    /// Return minimal of width(), height().
    unsigned min() const;

    /// Return maximal of width(), height().
    unsigned max() const;

    /// Return minimal of width(), height(), depth().
    unsigned min3() const;

    /// Return maximal of width(), height(), depth().
    unsigned max3() const;

    /// Test if not empty.
    operator bool() const;

    /// Add other Size.
    Size & operator+=(const Size & size);

    /// Subtract other Size.
    Size & operator-=(const Size & size);

    Size & operator|=(const Size & sz);

    Size & operator&=(const Size & sz);

    Size & operator*=(double m);

private:

    unsigned width_  = 0;
    unsigned height_ = 0;
    unsigned depth_  = 0;
};

/// @relates Size
bool operator==(const Size & s, const Size & t);

/// @relates Size
bool operator!=(const Size & s, const Size & t);

/// @relates Size
Size operator+(const Size & lhs, const Size & rhs);

/// @relates Size
Size operator|(const Size & lhs, const Size & rhs);

/// @relates Size
Size operator&(const Size & lhs, const Size & rhs);

/// @relates Size
Size operator-(const Size & lhs, const Size & rhs);

/// @relates Point
Point operator+(const Point & p, const Size & sz);

/// @relates Point
Point operator+(const Size & sz, const Point & p);

/// @relates Point
Point operator-(const Point & p, const Size & sz);

/// The rectangle with integer coordinates.
/// @ingroup geometry_group
class Rect {
public:

    Rect() = default;

    /// Constructor with coordinates and size.
    Rect(int x, int y, const Size & size);

    /// Constructor with coordinates and size.
    Rect(const Point & org, const Size & size);

    /// Constructor with coordinates.
    Rect(int x1, int y1, int x2, int y2);

    /// Constructor with coordinates.
    Rect(const Point & a, const Point & b);

    /// Constructor with Size and coordinates at (0, 0).
    Rect(const Size & size);

    /// Constructor with width and height and coordinates at (0, 0).
    Rect(unsigned width, unsigned height);

    /// Get origin.
    Point origin() const;

    /// Get left coordinate.
    int left() const;

    /// Get right coordinate.
    int right() const;

    /// Get top coordinate.
    int top() const;

    /// Get bottom coordinate.
    int bottom() const;

    /// Get left coordinate.
    /// Same as left().
    int x() const;

    /// Get top coordinate.
    /// Same as top().
    int y() const;

    /// Get size.
    Size size() const;

    /// Get width.
    unsigned width() const;

    /// Get height.
    unsigned height() const;

    /// Get signed width.
    int iwidth() const;

    /// Get signed height.
    int iheight() const;

    /// Assign new value.
    void set(const Rect & r);

    /// Assign new value.
    void set(const Point & org, const Size & size);

    /// Assign new value.
    void set(int x, int y, const Size & size);

    /// Assign new value.
    void set(const Size & size);

    /// Assign new value.
    void set(int x1, int y1, int x2, int y2);

    /// Assign new value.
    void set(const Point & a, const Point & b);

    /// Reset origin and size.
    void reset();

    /// Test if empty.
    bool empty() const;

    /// Get top left point.
    Point top_left() const;

    /// Get top right point.
    Point top_right() const;

    /// Get bottom left point.
    Point bottom_left() const;

    /// Get bottom right point.
    Point bottom_right() const;

    /// Get center point.
    Point center() const;

    void center_to(int x, int y);
    void center_to(const Point & pt);
    Rect moved(const Point & p) const;
    Rect moved(int x, int y) const;
    bool contains(int x, int y) const;
    bool contains(const Point & p) const;
    bool contains(const Rect & r) const;

    /// Intersect with other rectangle.
    void intersect(const Rect & other);

    /// Unite with other rectangle.
    void unite(const Rect & other);

    /// Make intersection between this rectangle and other one.
    Rect intersected(const Rect & other) const;

    /// Make union of two rectangles.
    Rect united(const Rect & other) const;

    void move_to(const Point & p);

    /// Assign new origin.
    void move_to(int x, int y);

    /// Move relatively.
    void translate(const Point & p);

    /// Move relatively.
    void translate(int dx, int dy);

    /// Move relatively.
    void translate(const Size & sz);

    Rect translated(const Point & p) const;

    Rect translated(const Size & sz) const;

    Rect translated(int x, int y) const;

    /// Resize.
    void resize(unsigned width, unsigned height);

    /// Resize.
    void resize(const Size & size);

    bool update_left(int x);

    bool update_top(int y);

    bool update_width(unsigned width);

    bool update_height(unsigned height);

    bool update_origin(const Point & pt);

    bool update_origin(int x, int y);

    bool update_size(const Size & sz);

    bool update_size(unsigned width, unsigned height);

    /// Increase size.
    void increase(unsigned dx, unsigned dy);

    /// Increase size.
    void increase(const Size & sz);

    /// Decrease size.
    void decrease(unsigned dx, unsigned dy);

    /// Decrease size.
    void decrease(const Size & sz);

    /// Make increased rectangle.
    Rect increased(unsigned dx, unsigned dy) const;

    /// Make increased rectangle.
    Rect increased(const Size & sz) const;

    /// Make decreased rectangle.
    Rect decreased(unsigned dx, unsigned dy) const;

    /// Make decreased rectangle.
    Rect decreased(const Size & sz) const;

    /// Grow size up or down.
    void grow(int dx, int dy);

    /// Make grown rectangle.
    Rect grown(int dx, int dy) const;

    /// Assign new value.
    Rect & operator=(const Rect & r);

    /// Unite with other rectangle.
    void operator|=(const Rect & other);

    /// Intersect with other rectangle.
    void operator&=(const Rect & other);

    /// Unite with size.
    void operator|=(const Size & size);

    /// Test if not empty.
    operator bool() const;

private:

    Point org_;
    Size  sz_;
};

/// @relates Rect
Rect operator|(const Rect & r1, const Rect & r2);

/// @relates Rect
Rect operator&(const Rect & r1, const Rect & r2);

/// @relates Rect
bool operator==(const Rect & r1, const Rect & r2);

/// @relates Rect
bool operator!=(const Rect & r1, const Rect & r2);

} // namespace tau

#endif // TAU_GEOMETRY_HH
