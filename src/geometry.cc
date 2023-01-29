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

#include <tau/geometry.hh>

namespace tau {

Vector::Vector(double x, double y):
    x_(x),
    y_(y)
{
}

/// 3D constructor.
Vector::Vector(double x, double y, double z):
    x_(x),
    y_(y),
    z_(z)
{
}

Vector::Vector(const Point & pt):
    x_(pt.x()),
    y_(pt.y()),
    z_(pt.z())
{
}

void Vector::set(const Point & pt) {
    x_ = pt.x();
    y_ = pt.y();
    z_ = pt.z();
}

void Vector::set(const Size & sz) {
    x_ = sz.width();
    y_ = sz.height();
    z_ = sz.depth();
}

double Vector::x() const {
    return x_;
}

/// Get y coordinate.
double Vector::y() const {
    return y_;
}

/// Get z coordinate.
double Vector::z() const {
    return z_;
}

/// Add other vector.
void Vector::operator+=(const Vector & other) {
    x_ += other.x();
    y_ += other.y();
    z_ += other.z();
}

/// Subtract other vector.
void Vector::operator-=(const Vector & other) {
    x_ -= other.x();
    y_ -= other.y();
    z_ -= other.z();
}

/// Multiply by scale factor.
void Vector::operator*=(double s) {
    x_ *= s;
    y_ *= s;
    z_ *= s;
}

/// Multiply by other vector.
void Vector::operator*=(const Vector & other) {
    x_ *= other.x();
    y_ *= other.y();
    z_ *= other.z();
}

/// Divide by scale factor.
void Vector::operator/=(double s) {
    x_ /= s;
    y_ /= s;
    z_ /= s;
}

/// Divide by other vector.
void Vector::operator/=(const Vector & other) {
    x_ /= other.x();
    y_ /= other.y();
    z_ /= other.z();
}

double Vector::length() const {
    return std::sqrt(x_*x_ + y_*y_ + z_*z_);
}

Vector Vector::normalized() const {
    double l = length();
    return 0.0 != l ? (*this)/l : Vector();
}

void Vector::set_x(double x) {
    x_ = x;
}

/// Set y value.
void Vector::set_y(double y) {
    y_ = y;
}

/// Set z value.
void Vector::set_z(double z) {
    z_ = z;
}

/// Set x and y values.
void Vector::set(double x, double y) {
    x_ = x;
    y_ = y;
}

/// Set all values.
void Vector::set(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
}

void Vector::reset() {
    x_ = y_ = z_ = 0.0;
}

Vector operator+(const Vector & lhs, const Vector & rhs) {
    return Vector(lhs.x()+rhs.x(), lhs.y()+rhs.y(), lhs.z()+rhs.z());
}

Vector operator-(const Vector & lhs, const Vector & rhs) {
    return Vector(lhs.x()-rhs.x(), lhs.y()-rhs.y(), lhs.z()-rhs.z());
}

Vector operator*(const Vector & lhs, const Vector & rhs) {
    return Vector(lhs.x()*rhs.x(), lhs.y()*rhs.y(), lhs.z()*rhs.z());
}

Vector operator*(const Vector & v, double s) {
    return Vector(v.x()*s, v.y()*s, v.z()*s);
}

Vector operator*(double s, const Vector & v) {
    return Vector(v.x()*s, v.y()*s, v.z()*s);
}

Vector operator/(const Vector & lhs, const Vector & rhs) {
    Vector v(lhs);
    v /= rhs;
    return v;
}

Vector operator/(const Vector & lhs, double s) {
    Vector v(lhs);
    v /= s;
    return v;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Point::Point(int x, int y):
    x_(x),
    y_(y)
{
}

/// 3D constructor.
Point::Point(int x, int y, int z):
    x_(x),
    y_(y),
    z_(z)
{
}

Point::Point(const Vector & vec):
    x_(vec.x()),
    y_(vec.y()),
    z_(vec.z())
{
}

Point & Point::set(int x, int y) {
    x_ = x;
    y_ = y;
    return *this;
}

Point & Point::set(int x, int y, int z) {
    x_ = x;
    y_ = y;
    z_ = z;
    return *this;
}

Point & Point::set(const Point & p) {
    x_ = p.x_;
    y_ = p.y_;
    z_ = p.z_;
    return *this;
}

Point & Point::set(const Vector & vec) {
    x_ = vec.x();
    y_ = vec.y();
    z_ = vec.z();
    return *this;
}

bool Point::update(int x, int y) {
    if (x != x_ || y != y_) {
        x_ = x;
        y_ = y;
        return true;
    }

    return false;
}

bool Point::update(int x, int y, int z) {
    if (x != x_ || y != y_ || z != z_) {
        x_ = x;
        y_ = y;
        z_ = z;
        return true;
    }

    return false;
}

bool Point::update(const Point & p) {
    if (p.x() != x_ || p.y() != y_ || p.z() != z_) {
        x_ = p.x();
        y_ = p.y();
        z_ = p.z();
        return true;
    }

    return false;
}

bool Point::update(const Vector & vec) {
    return update(std::round(vec.x()), std::round(vec.y()), std::round(vec.z()));
}

bool Point::update_x(int x) {
    if (x != x_) {
        x_ = x;
        return true;
    }

    return false;
}

bool Point::update_y(int y) {
    if (y != y_) {
        y_ = y;
        return true;
    }

    return false;
}

bool Point::update_z(int z) {
    if (z != z_) {
        z_ = z;
        return true;
    }

    return false;
}

bool Point::translate(int dx, int dy) {
    return update(dx+x_, dy+y_);
}

bool Point::translate(int dx, int dy, int dz) {
    return update(dx+x_, dy+y_, dz+z_);
}

bool Point::translate(const Point & other) {
    return update(other.x()+x_, other.y()+y_, other.z()+z_);
}

Point & Point::operator=(const Vector & vec) {
    set(vec);
    return *this;
}

int Point::x() const {
    return x_;
}

/// Y coordinate accessor.
int Point::y() const {
    return y_;
}

/// Z coordinate accessor.
int Point::z() const {
    return z_;
}

void Point::reset() {
    x_ = y_ = z_ = 0;
}

Point & Point::operator+=(const Point & p) {
    x_ += p.x_;
    y_ += p.y_;
    z_ += p.z_;
    return *this;
}

/// Transpose coordinates.
Point & Point::operator-=(const Point & p) {
    x_ -= p.x_;
    y_ -= p.y_;
    z_ -= p.z_;
    return *this;
}

/// Scale coordinates.
Point & Point::operator*=(double m) {
    x_ = (static_cast<double>(x_)*m);
    y_ = (static_cast<double>(y_)*m);
    z_ = (static_cast<double>(z_)*m);
    return *this;
}

/// Scale coordinates.
Point & Point::operator*=(const Vector & v) {
    x_ = (static_cast<double>(x_)*v.x());
    y_ = (static_cast<double>(y_)*v.y());
    z_ = (static_cast<double>(z_)*v.z());
    return *this;
}

Point operator*(const Point & p, double m) {
    return Point(p)*=m;
}

Point operator*(double m, const Point & p) {
    return Point(p)*=m;
}

Point operator+(const Point & p, const Point & q) {
    return Point(p.x()+q.x(), p.y()+q.y());
}

Point operator-(const Point & p, const Point & q) {
    return Point(p.x()-q.x(), p.y()-q.y(), p.z()-q.z());
}

Point operator-(const Point & p) {
    return Point(-p.x(), -p.y(), -p.z());
}

bool operator==(const Point & p, const Point & q) {
    return p.x() == q.x() && p.y() == q.y() && p.z() == q.z();
}

bool operator!=(const Point & p, const Point & q) {
    return !(p == q);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Size::Size(unsigned width, unsigned height):
    width_(width),
    height_(height)
{
}

Size::Size(unsigned width, unsigned height, unsigned depth):
    width_(width),
    height_(height),
    depth_(depth)
{
}

Size::Size(unsigned all):
    width_(all),
    height_(all),
    depth_(all)
{
}

Size::Size(const Vector & v) {
    update(v);
}

bool Size::update_width(unsigned width) {
    if (width_ != width) {
        width_ = width;
        return true;
    }

    return false;
}

bool Size::update_height(unsigned height) {
    if (height_ != height) {
        height_ = height;
        return true;
    }

    return false;
}

bool Size::update_depth(unsigned depth) {
    if (depth_ != depth) {
        depth_ = depth;
        return true;
    }

    return false;
}

bool Size::update(unsigned all) {
    bool changed = update_width(all);
    if (update_height(all)) { changed = true; }
    if (update_depth(all)) { changed = true; }
    return changed;
}

bool Size::update(unsigned width, unsigned height) {
    bool changed = update_width(width);
    if (update_height(height)) { changed = true; }
    return changed;
}

bool Size::update(unsigned width, unsigned height, unsigned depth) {
    bool changed = update_width(width);
    if (update_height(height)) { changed = true; }
    if (update_depth(depth)) { changed = true; }
    return changed;
}

bool Size::update(const Size & size, bool nz) {
    bool changed = nz ? size.width() && update_width(size.width()) : update_width(size.width());
    if (nz ? size.height() && update_height(size.height()) : update_height(size.height())) { changed = true; }
    if (nz ? size.depth() && update_depth(size.depth()) : update_depth(size.depth())) { changed = true; }
    return changed;
}

bool Size::update(const Vector & vec) {
    bool changed = update_width(std::max(0, static_cast<int>(vec.x())));
    if (update_height(std::max(0, static_cast<int>(vec.y())))) { changed = true; }
    if (update_depth(std::max(0, static_cast<int>(vec.z())))) { changed = true; }
    return changed;
}

bool Size::update_max_width(unsigned width) {
    return update_width(std::max(width_, width));
}

bool Size::update_max_height(unsigned height) {
    return update_height(std::max(height_, height));
}

bool Size::update_max_depth(unsigned depth) {
    return update_depth(std::max(depth_, depth));
}

bool Size::update_max(unsigned all) {
    bool changed = update_max_width(all);
    if (update_max_height(all)) { changed = true; }
    if (update_max_depth(all)) { changed = true; }
    return changed;
}

bool Size::update_max(unsigned width, unsigned height) {
    bool changed = update_max_width(width);
    if (update_max_height(height)) { changed = true; }
    return changed;
}

bool Size::update_max(unsigned width, unsigned height, unsigned depth) {
    bool changed = update_max_width(width);
    if (update_max_height(height)) { changed = true; }
    if (update_max_depth(depth)) { changed = true; }
    return changed;
}

bool Size::update_max(const Size & size, bool nz) {
    bool changed = nz ? size.width() && update_max_width(size.width()) : update_max_width(size.width());
    if (nz ? size.height() && update_max_height(size.height()) : update_max_height(size.height())) { changed = true; }
    if (nz ? size.depth() && update_max_depth(size.depth()) : update_max_depth(size.depth())) { changed = true; }
    return changed;
}

bool Size::update_max(const Vector & vec) {
    bool changed = update_max_width(std::max(0, static_cast<int>(vec.x())));
    if (update_max_height(std::max(0, static_cast<int>(vec.y())))) { changed = true; }
    if (update_max_depth(std::max(0, static_cast<int>(vec.z())))) { changed = true; }
    return changed;
}

bool Size::update_min_width(unsigned width) {
    return update_width(std::min(width_, width));
}

bool Size::update_min_height(unsigned height) {
    return update_height(std::min(height_, height));
}

bool Size::update_min_depth(unsigned depth) {
    return update_depth(std::min(depth_, depth));
}

bool Size::update_min(unsigned all) {
    bool changed = update_min_width(all);
    if (update_min_height(all)) { changed = true; }
    if (update_min_depth(all)) { changed = true; }
    return changed;
}

bool Size::update_min(unsigned width, unsigned height) {
    bool changed = update_min_width(width);
    if (update_min_height(height)) { changed = true; }
    return changed;
}

bool Size::update_min(unsigned width, unsigned height, unsigned depth) {
    bool changed = update_min_width(width);
    if (update_min_height(height)) { changed = true; }
    if (update_min_depth(depth)) { changed = true; }
    return changed;
}

bool Size::update_min(const Size & size, bool nz) {
    bool changed = nz ? size.width() && update_min_width(size.width()) : update_min_width(size.width());
    if (nz ? size.height() && update_min_height(size.height()) : update_min_height(size.height())) { changed = true; }
    if (nz ? size.depth() && update_min_depth(size.depth()) : update_min_depth(size.depth())) { changed = true; }
    return changed;
}

bool Size::update_min(const Vector & vec) {
    bool changed = update_min_width(std::max(0, static_cast<int>(vec.x())));
    if (update_min_height(std::max(0, static_cast<int>(vec.y())))) { changed = true; }
    if (update_min_depth(std::max(0, static_cast<int>(vec.z())))) { changed = true; }
    return changed;
}

bool Size::increase(unsigned dx, unsigned dy) {
    return update(dx+width_, dy+height_);
}

bool Size::increase(unsigned dx, unsigned dy, unsigned dz) {
    return update(dx+width_, dy+height_, dz+depth_);
}


bool Size::increase(const Size & size) {
    return update(size.width()+width_, size.height()+height_, size.depth()+depth_);
}

bool Size::decrease(unsigned dx, unsigned dy) {
    return update(width_ > dx ? width_-dx : 0, height_ > dy ? height_-dy : 0);
}

bool Size::decrease(unsigned dx, unsigned dy, unsigned dz) {
    return update(width_ > dx ? width_-dx : 0, height_ > dy ? height_-dy : 0, depth_ > dz ? depth_-dz : 0);
}

bool Size::decrease(const Size & size) {
    return update(width_ > size.width() ? width_-size.width() : 0,
                  height_ > size.height() ? height_-size.height() : 0,
                  depth_ > size.depth() ? depth_-size.depth() : 0);
}

bool Size::grow(int dx, int dy) {
    return update(dx >= 0 ? width_+dx : (width_ > unsigned(dx) ? width_-dx : 0),
                  dy >= 0 ? height_+dy : (height_ > unsigned(dy) ? height_-dy : 0));
}

bool Size::grow(int dx, int dy, int dz) {
    return update(dx >= 0 ? width_+dx : (width_ > unsigned(dx) ? width_-dx : 0),
                  dy >= 0 ? height_+dy : (height_ > unsigned(dy) ? height_-dy : 0),
                  dz >= 0 ? depth_+dz : (depth_ > unsigned(dz) ? depth_-dz : 0));
}

Size Size::increased(unsigned dx, unsigned dy) const {
    Size sz(*this);
    sz.increase(dx, dy);
    return sz;
}

Size Size::increased(unsigned dx, unsigned dy, unsigned dz) const {
    Size sz(*this);
    sz.increase(dx, dy, dz);
    return sz;
}

Size Size::increased(const Size & other) const {
    Size sz(*this);
    sz.increase(other);
    return sz;
}

Size Size::decreased(unsigned dx, unsigned dy) const {
    Size sz(*this);
    sz.decrease(dx, dy);
    return sz;
}

Size Size::decreased(unsigned dx, unsigned dy, unsigned dz) const {
    Size sz(*this);
    sz.decrease(dx, dy, dz);
    return sz;
}

Size Size::decreased(const Size & other) const {
    Size sz(*this);
    sz.decrease(other);
    return sz;
}

Size Size::grown(int dx, int dy) const {
    Size sz(*this);
    sz.grow(dx, dy);
    return sz;
}

Size Size::grown(int dx, int dy, int dz) const {
    Size sz(*this);
    sz.grow(dx, dy, dz);
    return sz;
}

void Size::set(const Size & other) {
    update(other);
}

void Size::set(unsigned width, unsigned height) {
    update(width, height);
}

void Size::set(unsigned width, unsigned height, unsigned depth) {
    update(width, height, depth);
}

unsigned Size::width() const {
    return width_;
}

unsigned Size::height() const {
    return height_;
}

unsigned Size::depth() const {
    return depth_;
}

int Size::iwidth() const {
    return width_;
}

int Size::iheight() const {
    return height_;
}

int Size::idepth() const {
    return depth_;
}

bool Size::empty() const {
    return 0 == width_ || 0 == height_;
}

bool Size::empty3() const {
    return 0 == width_ || 0 == height_ || 0 == depth_;
}

void Size::reset() {
    width_ = height_ = depth_ = 0;
}

unsigned Size::min() const {
    return std::min(width_, height_);
}

unsigned Size::max() const {
    return std::max(width_, height_);
}

unsigned Size::min3() const {
    return std::min(std::min(width_, height_), depth_);
}

unsigned Size::max3() const {
    return std::max(std::max(width_, height_), depth_);
}

Size::operator bool() const {
    return !empty();
}

Size & Size::operator+=(const Size & size) {
    width_  += size.width();
    height_ += size.height();
    depth_  += size.depth();
    return *this;
}

Size & Size::operator-=(const Size & size) {
    width_  -= size.width();
    height_ -= size.height();
    depth_  -= size.depth();
    return *this;
}

Size & Size::operator|=(const Size & sz) {
    width_  = std::max(width_,  sz.width_);
    height_ = std::max(height_, sz.height_);
    depth_  = std::max(depth_,  sz.depth_);
    return *this;
}

Size & Size::operator&=(const Size & sz) {
    width_  = std::min(width_,  sz.width_);
    height_ = std::min(height_, sz.height_);
    depth_  = std::min(depth_,  sz.depth_);
    return *this;
}

Size & Size::operator*=(double m) {
    width_  = static_cast<double>(width_)*m;
    height_ = static_cast<double>(height_)*m;
    depth_  = static_cast<double>(depth_)*m;
    return *this;
}

bool operator==(const Size & s, const Size & t) {
    return (s.width() == t.width()) && (s.height() == t.height());
}

bool operator!=(const Size & s, const Size & t) {
    return (s.width() != t.width()) || (s.height() != t.height());
}

Size operator+(const Size & lhs, const Size & rhs) {
    return Size(lhs).increased(rhs);
}

Size operator|(const Size & lhs, const Size & rhs) {
    Size sz(lhs);
    sz |= rhs;
    return sz;
}

Size operator&(const Size & lhs, const Size & rhs) {
    Size sz(lhs);
    sz &= rhs;
    return sz;
}

Size operator-(const Size & lhs, const Size & rhs) {
    return Size(lhs).decreased(rhs);
}

Point operator+(const Point & p, const Size & sz) {
    return Point(p.x()+sz.width(), p.y()+sz.height(), p.z()+sz.depth());
}

Point operator+(const Size & sz, const Point & p) {
    return Point(p.x()+sz.width(), p.y()+sz.height(), p.z()+sz.depth());
}

Point operator-(const Point & p, const Size & sz) {
    return Point(p.x()-sz.width(), p.y()-sz.height(), p.z()-sz.depth());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Rect::Rect(int x, int y, const Size & size) {
    set(x, y, size);
}

Rect::Rect(const Point & org, const Size & size) {
    set(org, size);
}

Rect::Rect(int x1, int y1, int x2, int y2) {
    set(x1, y1, x2, y2);
}

Rect::Rect(const Point & a, const Point & b) {
    set(a, b);
}

Rect::Rect(const Size & size) {
    set(size);
}

Rect::Rect(unsigned width, unsigned height) {
    set(0, 0, Size(width, height));
}

void Rect::set(int x1, int y1, int x2, int y2) {
    org_.set(std::min(x1, x2), std::min(y1, y2));
    sz_.set(1+abs(x2-x1), 1+abs(y2-y1));
    //sz_.set(abs(x2-x1), abs(y2-y1));
}

void Rect::set(const Point & org, const Size & size) {
    org_ = org;
    sz_ = size;
}

void Rect::set(int x, int y, const Size & sz) {
    org_.set(x, y);
    sz_ = sz;
}

void Rect::set(const Size & sz) {
    org_.reset();
    sz_ = sz;
}

void Rect::set(const Rect & r) {
    org_ = r.top_left();
    sz_ = r.size();
}

int Rect::right() const {
    return width() ? org_.x()+width()-1 : 0;
}

int Rect::bottom() const {
    return height() ? org_.y()+height()-1 : 0;
}

void Rect::reset() {
    org_.set(0, 0);
    sz_.reset();
}

Point Rect::top_left() const {
    return Point(left(), top());
}

Point Rect::top_right() const {
    return Point(right(), top());
}

Point Rect::bottom_left() const {
    return Point(left(), bottom());
}

Point Rect::bottom_right() const {
    return Point(right(), bottom());
}

Point Rect::center() const {
    return Point(left()+width()/2, top()+height()/2);
}

void Rect::center_to(int x, int y) {
    center_to(Point(x, y));
}

void Rect::center_to(const Point & pt) {
    move_to(pt.x()-(size().width()/2), pt.y()-(size().height()/2));
}

Rect Rect::moved(const Point & p) const {
    Rect r(*this);
    r.move_to(p);
    return r;
}

Rect Rect::moved(int x, int y) const {
    return moved(Point(x, y));
}

bool Rect::contains(int x, int y) const {
    return empty() ? false : (x >= left() && x <= right() && y >= top() && y <= bottom());
}

bool Rect::contains(const Point & p) const {
    return contains(p.x(), p.y());
}

bool Rect::contains(const Rect & r) const {
    return contains(r.top_left()) && contains(r.bottom_right());
}

void Rect::unite(const Rect & other) {
    if (other) {
        set(std::min(left(), other.left()), std::min(top(), other.top()), std::max(right(), other.right()), std::max(bottom(), other.bottom()));
    }
}

Rect Rect::united(const Rect & other) const {
    Rect res(*this);
    res.unite(other);
    return res;
}

void Rect::intersect(const Rect & other) {
    if (!empty() && !other.empty() && other.left() <= right() && other.right() >= left() && other.top() <= bottom() && other.bottom() >= top()) {
        set(std::max(left(), other.left()), std::max(top(), other.top()), std::min(right(), other.right()), std::min(bottom(), other.bottom()));
    }

    else {
        reset();
    }
}

Point Rect::origin() const {
    return org_;
}

int Rect::left() const {
    return org_.x();
}

int Rect::top() const {
    return org_.y();
}

int Rect::x() const {
    return left();
}

int Rect::y() const {
    return top();
}

Size Rect::size() const {
    return sz_;
}

unsigned Rect::width() const {
    return sz_.width();
}

unsigned Rect::height() const {
    return sz_.height();
}

int Rect::iwidth() const {
    return sz_.iwidth();
}

int Rect::iheight() const {
    return sz_.iheight();
}

void Rect::set(const Point & a, const Point & b) {
    set(a.x(), a.y(), b.x(), b.y());
}

bool Rect::empty() const {
    return sz_.empty();
}

void Rect::move_to(const Point & p) {
    org_ = p;
}

void Rect::move_to(int x, int y) {
    move_to(Point(x, y));
}

void Rect::translate(const Point & p) {
    org_ += p;
}

void Rect::translate(int dx, int dy) {
    translate(Point(dx, dy));
}

void Rect::translate(const Size & sz) {
    org_.set(org_.x()+sz.width(), org_.y()+sz.height());
}

Rect Rect::translated(const Point & p) const {
    Rect r(*this);
    r.translate(p);
    return r;
}

Rect Rect::translated(const Size & sz) const {
    Rect r(*this);
    r.translate(sz);
    return r;
}

Rect Rect::translated(int x, int y) const {
    return translated(Point(x, y));
}

void Rect::resize(unsigned width, unsigned height) {
    sz_.update(width, height);
}

void Rect::resize(const Size & size) {
    sz_ = size;
}

bool Rect::update_left(int x) {
    return org_.update_x(x);
}

bool Rect::update_top(int y) {
    return org_.update_y(y);
}

bool Rect::update_width(unsigned width) {
    return sz_.update_width(width);
}

bool Rect::update_height(unsigned height) {
    return sz_.update_height(height);
}

bool Rect::update_origin(const Point & pt) {
    return org_.update(pt);
}

bool Rect::update_origin(int x, int y) {
    return org_.update(x, y);
}

bool Rect::update_size(const Size & sz) {
    return sz_.update(sz);
}

bool Rect::update_size(unsigned width, unsigned height) {
    return sz_.update(width, height);
}

void Rect::increase(unsigned dx, unsigned dy) {
    sz_.increase(dx, dy);
}

void Rect::increase(const Size & sz) {
    sz_.increase(sz);
}

void Rect::decrease(unsigned dx, unsigned dy) {
    sz_.decrease(dx, dy);
}

void Rect::decrease(const Size & sz) {
    sz_.decrease(sz);
}

Rect Rect::increased(unsigned dx, unsigned dy) const {
    return Rect(org_, sz_.increased(dx, dy));
}

Rect Rect::increased(const Size & sz) const {
    return Rect(org_, sz_.increased(sz));
}

Rect Rect::decreased(unsigned dx, unsigned dy) const {
    return Rect(org_, sz_.decreased(dx, dy));
}

Rect Rect::decreased(const Size & sz) const {
    return Rect(org_, sz_.decreased(sz));
}

void Rect::grow(int dx, int dy) {
    sz_.grow(dx, dy);
}

Rect Rect::grown(int dx, int dy) const {
    return Rect(org_, sz_.grown(dx, dy));
}

Rect & Rect::operator=(const Rect & r) {
    set(r);
    return *this;
}

void Rect::operator|=(const Rect & other) {
    unite(other);
}

void Rect::operator&=(const Rect & other) {
    intersect(other);
}

void Rect::operator|=(const Size & size) {
    sz_ |= size;
}

Rect::operator bool() const {
    return !empty();
}

Rect Rect::intersected(const Rect & other) const {
    Rect res(*this);
    res.intersect(other);
    return res;
}

Rect operator|(const Rect & r1, const Rect & r2) {
    return r1.united(r2);
}

Rect operator&(const Rect & r1, const Rect & r2) {
    return r1.intersected(r2);
}

bool operator==(const Rect & r1, const Rect & r2) {
    return r1.top_left() != r2.top_left() && r1.bottom_right() != r2.bottom_right();
}

bool operator!=(const Rect & r1, const Rect & r2) {
    return r1.top_left() != r2.top_left() || r1.bottom_right() != r2.bottom_right();
}

} // namespace tau

//END
