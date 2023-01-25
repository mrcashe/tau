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

#include <tau/contour.hh>
#include <tau/curve.hh>

namespace tau {

Contour::Contour(const Vector & start):
    start_(start)
{
}

Contour::Contour(double sx, double sy, double sz):
    start_(sx, sy, sz)
{
}

Contour::Contour(Contour && ctr):
    start_(ctr.start_),
    curves_(std::move(ctr.curves_))
{
}

Contour & Contour::operator=(Contour && ctr) {
    start_ = ctr.start_;
    curves_ = std::move(ctr.curves_);
    return *this;
}

Contour Contour::from_rectangle(const Vector & v1, const Vector & v2) {
    double xmin = std::min(v1.x(), v2.x()), xmax = std::max(v1.x(), v2.x());
    double ymin = std::min(v1.y(), v2.y()), ymax = std::max(v1.y(), v2.y());

    Contour ctr(xmin, ymin);
    ctr.line_to(Vector(xmax, ymin));
    ctr.line_to(Vector(xmax, ymax));
    ctr.line_to(Vector(xmin, ymax));
    ctr.line_to(Vector(xmin, ymin));

    return ctr;
}

Contour Contour::from_rectangle(double x1, double y1, double x2, double y2) {
    return from_rectangle(Vector(x1, y1), Vector(x2, y2));
}

Vector Contour::start() const {
    return start_;
}

std::vector<Curve> & Contour::curves() {
    return curves_;
}

const std::vector<Curve> & Contour::curves() const {
    return curves_;
}

Curve & Contour::operator[](std::size_t index) {
    return curves_[index];
}

const Curve & Contour::operator[](std::size_t index) const {
    return curves_[index];
}

void Contour::line_to(const Vector & end) {
    curves_.push_back(Curve(end));
}

void Contour::conic_to(const Vector & cp1, const Vector & end) {
    curves_.push_back(Curve(cp1, end));
}

void Contour::cubic_to(const Vector & cp1, const Vector & cp2, const Vector & end) {
    curves_.push_back(Curve(cp1, cp2, end));
}

std::size_t Contour::size() const {
    return curves_.size();
}

bool Contour::empty() const {
    return curves_.empty();
}

Contour::iterator Contour::begin() {
    return curves_.begin();
}

Contour::const_iterator Contour::begin() const {
    return curves_.begin();
}

Contour::iterator Contour::end() {
    return curves_.end();
}

Contour::const_iterator Contour::end() const {
    return curves_.end();
}

void Contour::translate(const Vector & v) {
    start_ += v;
    for (auto & c: curves_) { c.translate(v); }
}

void Contour::translate(double ox, double oy, double oz) {
    translate(Vector(ox, oy, oz));
}

void Contour::scale(const Vector & v) {
    start_ *= v;
    for (auto & c: curves_) { c.scale(v); }
}

void Contour::scale(double s) {
    scale(Vector(s, s, s));
}

void Contour::scale(double sx, double sy, double sz) {
    scale(Vector(sx, sy, sz));
}

void Contour::transform(const Matrix & mat) {
    start_ *= mat;
    for (Curve & c: curves_) { c *= mat; }
}

unsigned Contour::order() const {
    unsigned o = 0;
    for (const Curve & c: curves_) { o = std::max(o, c.order()); }
    return o;
}

void Contour::revert() {
    if (!empty()) {
        Vector end = start_;
        std::vector<Curve> cvs;

        for (const Curve & cv: curves_) {
            if (3 == cv.order()) {
                cvs.emplace(cvs.begin(), cv.cp2(), cv.cp1(), end);
            }

            else if (2 == cv.order()) {
                //Vector cp = cv.end()-cv.cp1();
                cvs.emplace(cvs.begin(), cv.cp1(), end);
            }

            else {
                cvs.emplace(cvs.begin(), end);
            }

            end = cv.end();
        }

        start_ = end;
        curves_ = std::move(cvs);
    }
}

void Contour::line_to(double x, double y) {
    line_to(Vector(x, y));
}

void Contour::conic_to(double cx, double cy, double ex, double ey) {
    conic_to(Vector(cx, cy), Vector(ex, ey));
}

void Contour::cubic_to(double cx1, double cy1, double cx2, double cy2, double ex, double ey) {
    cubic_to(Vector(cx1, cy1), Vector(cx2, cy2), Vector(ex, ey));
}

Contour & operator*=(Contour & ctr, const Matrix & mat) {
    ctr.transform(mat);
    return ctr;
}

Contour & operator*=(const Matrix & mat, Contour & ctr) {
    ctr.transform(mat);
    return ctr;
}

} // namespace tau

//END

