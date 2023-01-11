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
    Vector(double x, double y):
        x_(x),
        y_(y)
    {
    }

    /// 3D constructor.
    Vector(double x, double y, double z):
        x_(x),
        y_(y),
        z_(z)
    {
    }

    /// Get x coordinate.
    double x() const {
        return x_;
    }

    /// Get y coordinate.
    double y() const {
        return y_;
    }

    /// Get z coordinate.
    double z() const {
        return z_;
    }

    /// Add other vector.
    void operator+=(const Vector & other) {
        x_ += other.x();
        y_ += other.y();
        z_ += other.z();
    }

    /// Subtract other vector.
    void operator-=(const Vector & other) {
        x_ -= other.x();
        y_ -= other.y();
        z_ -= other.z();
    }

    /// Multiply by scale factor.
    void operator*=(double s) {
        x_ *= s;
        y_ *= s;
        z_ *= s;
    }

    /// Multiply by other vector.
    void operator*=(const Vector & other) {
        x_ *= other.x();
        y_ *= other.y();
        z_ *= other.z();
    }

    /// Divide by scale factor.
    void operator/=(double s) {
        x_ /= s;
        y_ /= s;
        z_ /= s;
    }

    /// Divide by other vector.
    void operator/=(const Vector & other) {
        x_ /= other.x();
        y_ /= other.y();
        z_ /= other.z();
    }

    /// Set x value.
    void set_x(double x) {
        x_ = x;
    }

    /// Set y value.
    void set_y(double y) {
        y_ = y;
    }

    /// Set z value.
    void set_z(double z) {
        z_ = z;
    }

    /// Set x and y values.
    void set(double x, double y) {
        x_ = x;
        y_ = y;
    }

    /// Set all values.
    void set(double x, double y, double z) {
        x_ = x;
        y_ = y;
        z_ = z;
    }

    /// Set from Point.
    void set(const Point & pt);

    /// Set from Size.
    void set(const Size & sz);

    /// Reset coordinates to 0.0.
    void reset() {
        x_ = y_ = z_ = 0.0;
    }

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
inline Vector operator+(const Vector & lhs, const Vector & rhs) {
    return Vector(lhs.x()+rhs.x(), lhs.y()+rhs.y(), lhs.z()+rhs.z());
}

/// @relates Vector
inline Vector operator-(const Vector & lhs, const Vector & rhs) {
    return Vector(lhs.x()-rhs.x(), lhs.y()-rhs.y(), lhs.z()-rhs.z());
}

/// @relates Vector
inline Vector operator*(const Vector & lhs, const Vector & rhs) {
    return Vector(lhs.x()*rhs.x(), lhs.y()*rhs.y(), lhs.z()*rhs.z());
}

/// @relates Vector
inline Vector operator*(const Vector & v, double s) {
    return Vector(v.x()*s, v.y()*s, v.z()*s);
}

/// @relates Vector
inline Vector operator*(double s, const Vector & v) {
    return Vector(v.x()*s, v.y()*s, v.z()*s);
}

/// @relates Vector
inline Vector operator/(const Vector & lhs, const Vector & rhs) {
    Vector v(lhs);
    v /= rhs;
    return v;
}

/// @relates Vector
inline Vector operator/(const Vector & lhs, double s) {
    Vector v(lhs);
    v /= s;
    return v;
}

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
    Point(int x, int y):
        x_(x),
        y_(y)
    {
    }

    /// 3D constructor.
    Point(int x, int y, int z):
        x_(x),
        y_(y),
        z_(z)
    {
    }

    /// Constructor with Vector.
    Point(const Vector & vec):
        x_(vec.x()),
        y_(vec.y()),
        z_(vec.z())
    {
    }

    /// Assignment from Vector.
    Point & operator=(const Vector & vec) {
        set(vec);
        return *this;
    }

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
    int x() const {
        return x_;
    }

    /// Y coordinate accessor.
    int y() const {
        return y_;
    }

    /// Z coordinate accessor.
    int z() const {
        return z_;
    }

    /// Reset coordinates to 0.
    void reset() {
        x_ = y_ = z_ = 0;
    }

    /// Transpose coordinates.
    Point & operator+=(const Point & p) {
        x_ += p.x_;
        y_ += p.y_;
        z_ += p.z_;
        return *this;
    }

    /// Transpose coordinates.
    Point & operator+=(const Size & sz);

    /// Transpose coordinates.
    Point & operator-=(const Point & p) {
        x_ -= p.x_;
        y_ -= p.y_;
        z_ -= p.z_;
        return *this;
    }

    /// Scale coordinates.
    Point & operator*=(double m) {
        x_ = (static_cast<double>(x_)*m);
        y_ = (static_cast<double>(y_)*m);
        z_ = (static_cast<double>(z_)*m);
        return *this;
    }

    /// Scale coordinates.
    Point & operator*=(const Vector & v) {
        x_ = (static_cast<double>(x_)*v.x());
        y_ = (static_cast<double>(y_)*v.y());
        z_ = (static_cast<double>(z_)*v.z());
        return *this;
    }

    /// Shift left.
    void operator<<=(unsigned nbits) {
        x_ <<= nbits;
        y_ <<= nbits;
        z_ <<= nbits;
    }

    /// Shift right.
    void operator>>=(unsigned nbits) {
        x_ <<= nbits;
        y_ <<= nbits;
        z_ <<= nbits;
    }

private:

    int x_ = 0;
    int y_ = 0;
    int z_ = 0;
};

/// @relates Point
inline Point operator*(const Point & p, double m) {
    return Point(p)*=m;
}

/// @relates Point
inline Point operator*(double m, const Point & p) {
    return Point(p)*=m;
}

/// @relates Point
inline Point operator+(const Point & p, const Point & q) {
    return Point(p.x()+q.x(), p.y()+q.y());
}

/// @relates Point
inline Point operator-(const Point & p, const Point & q) {
    return Point(p.x()-q.x(), p.y()-q.y(), p.z()-q.z());
}

/// @relates Point
inline Point operator-(const Point & p) {
    return Point(-p.x(), -p.y(), -p.z());
}

/// @relates Point
inline bool operator==(const Point & p, const Point & q) {
    return p.x() == q.x() && p.y() == q.y() && p.z() == q.z();
}

/// @relates Point
inline bool operator!=(const Point & p, const Point & q) {
    return !(p == q);
}

/// @relates Point
inline Point operator<<(const Point & p, unsigned nbits) {
    Point q(p);
    q <<= nbits;
    return q;
}

/// @relates Point
inline Point operator>>(const Point & p, unsigned nbits) {
    Point q(p);
    q >>= nbits;
    return q;
}

/// The 2D/3D size with unsigned integer values.
/// @ingroup geometry_group
class Size {
public:

    Size() = default;
    Size(const Size & other) = default;
    Size & operator=(const Size & other) = default;

    Size(unsigned width, unsigned height):
        width_(width),
        height_(height)
    {
    }

    Size(unsigned width, unsigned height, unsigned depth):
        width_(width),
        height_(height),
        depth_(depth)
    {
    }

    Size(unsigned all):
        width_(all),
        height_(all),
        depth_(all)
    {
    }

    /// Constructs from Vector.
    Size(const Vector & v) {
        update(v);
    }

    void set(const Size & other) {
        update(other);
    }

    void set(unsigned width, unsigned height) {
        update(width, height);
    }

    void set(unsigned width, unsigned height, unsigned depth) {
        update(width, height, depth);
    }

    static Size from_vector(const Vector & vec) {
        Size sz;
        sz.update(vec);
        return sz;
    }

    unsigned width() const {
        return width_;
    }

    unsigned height() const {
        return height_;
    }

    unsigned depth() const {
        return depth_;
    }

    int iwidth() const {
        return width_;
    }

    int iheight() const {
        return height_;
    }

    int idepth() const {
        return depth_;
    }

    /// Update width.
    /// @return true if changed.
    bool update_width(unsigned width);

    /// Update height.
    /// @return true if changed.
    bool update_height(unsigned height);

    /// Update depth.
    /// @return true if changed.
    bool update_depth(unsigned depth);

    /// Update width, height and depth.
    bool update(unsigned all);

    /// Update width and height.
    /// @return true if changed.
    bool update(unsigned width, unsigned height);

    /// Update width, height and depth.
    /// @return true if changed.
    bool update(unsigned width, unsigned height, unsigned depth);

    /// Update all.
    /// @param sz other size.
    /// @param nz update values only if new values not equals to 0.
    /// @return true if changed.
    bool update(const Size & sz, bool nz=false);

    /// Update all.
    /// @return true if changed.
    bool update(const Vector & vec);

    /// Update if given value greater.
    /// @return true if changed.
    bool update_max_width(unsigned width);

    /// Update if given value greater.
    /// @return true if changed.
    bool update_max_height(unsigned height);

    /// Update if given value greater.
    /// @return true if changed.
    bool update_max_depth(unsigned depth);

    /// Update if given values greater.
    bool update_max(unsigned all);

    /// Update width and height if given values greater.
    /// @return true if changed.
    bool update_max(unsigned width, unsigned height);

    /// Update width, height and depth if given values greater.
    /// @return true if changed.
    bool update_max(unsigned width, unsigned height, unsigned depth);

    /// Update all if given values greater.
    /// @param sz other size.
    /// @param nz update values only if new values not equals to 0.
    /// @return true if changed.
    bool update_max(const Size & sz, bool nz=false);

    /// Update all if given values greater.
    /// @return true if changed.
    bool update_max(const Vector & vec);

    /// Update width if given value less.
    /// @return true if changed.
    bool update_min_width(unsigned width);

    /// Update height if given value less.
    /// @return true if changed.
    bool update_min_height(unsigned height);

    /// Update depth if given value less.
    /// @return true if changed.
    bool update_min_depth(unsigned depth);

    /// Update width, height and depth if given values less.
    bool update_min(unsigned all);

    /// Update width and height if given values less.
    /// @return true if changed.
    bool update_min(unsigned width, unsigned height);

    /// Update width, height and depth if given values less.
    /// @return true if changed.
    bool update_min(unsigned width, unsigned height, unsigned depth);

    /// Update all if given value less.
    /// @param sz other size.
    /// @param nz update values only if new values not equals to 0.
    /// @return true if changed.
    bool update_min(const Size & sz, bool nz=false);

    /// Update all if given value less.
    /// @return true if changed.
    bool update_min(const Vector & vec);

    /// Increase 2D size.
    /// @return true if changed.
    bool increase(unsigned dx, unsigned dy);

    /// Increase 3D size.
    /// @return true if changed.
    bool increase(unsigned dx, unsigned dy, unsigned dz);

    /// Increase size.
    /// @return true if changed.
    bool increase(const Size & sz);

    /// Decrease 2D size.
    /// @return true if changed.
    bool decrease(unsigned dx, unsigned dy);

    /// Decrease 3D size.
    /// @return true if changed.
    bool decrease(unsigned dx, unsigned dy, unsigned dz);

    /// Decrease size.
    /// @return true if changed.
    bool decrease(const Size & sz);

    /// Grow 2D size.
    /// @return true if changed.
    bool grow(int dx, int dy);

    /// Grow 3D size.
    /// @return true if changed.
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
    bool empty() const {
        return 0 == width_ || 0 == height_;
    }

    /// 3D check empty.
    bool empty3() const {
        return 0 == width_ || 0 == height_ || 0 == depth_;
    }

    /// Reset values to 0.
    void reset() {
        width_ = height_ = depth_ = 0;
    }

    /// Return minimal of width(), height().
    unsigned min() const {
        return std::min(width_, height_);
    }

    /// Return maximal of width(), height().
    unsigned max() const {
        return std::max(width_, height_);
    }

    /// Return minimal of width(), height(), depth().
    unsigned min3() const {
        return std::min(std::min(width_, height_), depth_);
    }

    /// Return maximal of width(), height(), depth().
    unsigned max3() const {
        return std::max(std::max(width_, height_), depth_);
    }

    /// Test if not empty.
    operator bool() const {
        return !empty();
    }

    /// Add other Size.
    Size & operator+=(const Size & size) {
        width_  += size.width();
        height_ += size.height();
        depth_  += size.depth();
        return *this;
    }

    /// Subtract other Size.
    Size & operator-=(const Size & size) {
        width_  -= size.width();
        height_ -= size.height();
        depth_  -= size.depth();
        return *this;
    }

    Size & operator|=(const Size & sz) {
        width_  = std::max(width_,  sz.width_);
        height_ = std::max(height_, sz.height_);
        depth_  = std::max(depth_,  sz.depth_);
        return *this;
    }

    Size & operator&=(const Size & sz) {
        width_  = std::min(width_,  sz.width_);
        height_ = std::min(height_, sz.height_);
        depth_  = std::min(depth_,  sz.depth_);
        return *this;
    }

    Size & operator*=(double m) {
        width_  = static_cast<double>(width_)*m;
        height_ = static_cast<double>(height_)*m;
        depth_  = static_cast<double>(depth_)*m;
        return *this;
    }

private:

    unsigned width_  = 0;
    unsigned height_ = 0;
    unsigned depth_  = 0;
};

/// @relates Point
inline Point & Point::operator+=(const Size & sz) {
    x_ += sz.width();
    y_ += sz.height();
    z_ += sz.depth();
    return *this;
}

/// @relates Size
inline bool operator==(const Size & s, const Size & t) {
    return (s.width() == t.width()) && (s.height() == t.height());
}

/// @relates Size
inline bool operator!=(const Size & s, const Size & t) {
    return (s.width() != t.width()) || (s.height() != t.height());
}

/// @relates Size
inline Size operator+(const Size & lhs, const Size & rhs) {
    return Size(lhs).increased(rhs);
}

/// @relates Size
inline Size operator|(const Size & lhs, const Size & rhs) {
    Size sz(lhs);
    sz |= rhs;
    return sz;
}

/// @relates Size
inline Size operator&(const Size & lhs, const Size & rhs) {
    Size sz(lhs);
    sz &= rhs;
    return sz;
}

/// @relates Size
inline Size operator-(const Size & lhs, const Size & rhs) {
    return Size(lhs).decreased(rhs);
}

/// @relates Point
inline Point operator+(const Point & p, const Size & sz) {
    return Point(p.x()+sz.width(), p.y()+sz.height(), p.z()+sz.depth());
}

/// @relates Point
inline Point operator+(const Size & sz, const Point & p) {
    return Point(p.x()+sz.width(), p.y()+sz.height(), p.z()+sz.depth());
}

/// @relates Point
inline Point operator-(const Point & p, const Size & sz) {
    return Point(p.x()-sz.width(), p.y()-sz.height(), p.z()-sz.depth());
}

/// The rectangle with integer coordinates.
/// @ingroup geometry_group
class Rect {
public:

    Rect() = default;

    /// Constructor with coordinates and size.
    Rect(int x, int y, const Size & size) {
        set(x, y, size);
    }

    /// Constructor with coordinates and size.
    explicit Rect(const Point & org, const Size & size) {
        set(org, size);
    }

    /// Constructor with coordinates.
    Rect(int x1, int y1, int x2, int y2) {
        set(x1, y1, x2, y2);
    }

    /// Constructor with coordinates.
    explicit Rect(const Point & a, const Point & b) {
        set(a, b);
    }

    /// Constructor with Size and coordinates at (0, 0).
    explicit Rect(const Size & size) {
        set(size);
    }

    /// Constructor with width and height and coordinates at (0, 0).
    Rect(unsigned width, unsigned height) {
        set(0, 0, Size(width, height));
    }

    /// Get origin.
    Point origin() const {
        return org_;
    }

    /// Get left coordinate.
    int left() const {
        return org_.x();
    }

    /// Get right coordinate.
    int right() const;

    /// Get top coordinate.
    int top() const {
        return org_.y();
    }

    /// Get bottom coordinate.
    int bottom() const;

    /// Get left coordinate.
    /// Same as left().
    int x() const {
        return left();
    }

    /// Get top coordinate.
    /// Same as top().
    int y() const {
        return top();
    }

    /// Get size.
    Size size() const {
        return sz_;
    }

    /// Get width.
    unsigned width() const {
        return sz_.width();
    }

    /// Get height.
    unsigned height() const {
        return sz_.height();
    }

    /// Get signed width.
    int iwidth() const {
        return sz_.iwidth();
    }

    /// Get signed height.
    int iheight() const {
        return sz_.iheight();
    }

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
    void set(const Point & a, const Point & b) {
        set(a.x(), a.y(), b.x(), b.y());
    }

    /// Reset origin and size.
    void reset();

    /// Test if empty.
    bool empty() const {
        return sz_.empty();
    }

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

    void move_to(const Point & p) {
        org_ = p;
    }

    /// Assign new origin.
    void move_to(int x, int y) {
        move_to(Point(x, y));
    }

    /// Move relatively.
    void translate(const Point & p) {
        org_ += p;
    }

    /// Move relatively.
    void translate(int dx, int dy) {
        translate(Point(dx, dy));
    }

    /// Move relatively.
    void translate(const Size & sz) {
        org_.set(org_.x()+sz.width(), org_.y()+sz.height());
    }

    Rect translated(const Point & p) const {
        Rect r(*this);
        r.translate(p);
        return r;
    }

    Rect translated(const Size & sz) const {
        Rect r(*this);
        r.translate(sz);
        return r;
    }

    Rect translated(int x, int y) const {
        return translated(Point(x, y));
    }

    /// Resize.
    void resize(unsigned width, unsigned height) {
        sz_.update(width, height);
    }

    /// Resize.
    void resize(const Size & size) {
        sz_ = size;
    }

    bool update_left(int x) {
        return org_.update_x(x);
    }

    bool update_top(int y) {
        return org_.update_y(y);
    }

    bool update_width(unsigned width) {
        return sz_.update_width(width);
    }

    bool update_height(unsigned height) {
        return sz_.update_height(height);
    }

    bool update_origin(const Point & pt) {
        return org_.update(pt);
    }

    bool update_origin(int x, int y) {
        return org_.update(x, y);
    }

    bool update_size(const Size & sz) {
        return sz_.update(sz);
    }

    bool update_size(unsigned width, unsigned height) {
        return sz_.update(width, height);
    }

    /// Increase size.
    void increase(unsigned dx, unsigned dy) {
        sz_.increase(dx, dy);
    }

    /// Increase size.
    void increase(const Size & sz) {
        sz_.increase(sz);
    }

    /// Decrease size.
    void decrease(unsigned dx, unsigned dy) {
        sz_.decrease(dx, dy);
    }

    /// Decrease size.
    void decrease(const Size & sz) {
        sz_.decrease(sz);
    }

    /// Make increased rectangle.
    Rect increased(unsigned dx, unsigned dy) const {
        return Rect(org_, sz_.increased(dx, dy));
    }

    /// Make increased rectangle.
    Rect increased(const Size & sz) const {
        return Rect(org_, sz_.increased(sz));
    }

    /// Make decreased rectangle.
    Rect decreased(unsigned dx, unsigned dy) const {
        return Rect(org_, sz_.decreased(dx, dy));
    }

    /// Make decreased rectangle.
    Rect decreased(const Size & sz) const {
        return Rect(org_, sz_.decreased(sz));
    }

    /// Grow size up or down.
    void grow(int dx, int dy) {
        sz_.grow(dx, dy);
    }

    /// Make grown rectangle.
    Rect grown(int dx, int dy) const {
        return Rect(org_, sz_.grown(dx, dy));
    }

    /// Assign new value.
    Rect & operator=(const Rect & r) {
        set(r);
        return *this;
    }

    /// Unite with other rectangle.
    void operator|=(const Rect & other) {
        unite(other);
    }

    /// Intersect with other rectangle.
    void operator&=(const Rect & other) {
        intersect(other);
    }

    /// Unite with size.
    void operator|=(const Size & size) {
        sz_ |= size;
    }

    /// Test if not empty.
    operator bool() const {
        return !empty();
    }

private:

    Point org_;
    Size  sz_;
};

/// @relates Rect
inline Rect operator|(const Rect & r1, const Rect & r2) {
    return r1.united(r2);
}

/// @relates Rect
inline Rect operator&(const Rect & r1, const Rect & r2) {
    return r1.intersected(r2);
}

/// @relates Rect
inline bool operator==(const Rect & r1, const Rect & r2) {
    return r1.top_left() != r2.top_left() && r1.bottom_right() != r2.bottom_right();
}

/// @relates Rect
inline bool operator!=(const Rect & r1, const Rect & r2) {
    return r1.top_left() != r2.top_left() || r1.bottom_right() != r2.bottom_right();
}

} // namespace tau

#endif // TAU_GEOMETRY_HH
