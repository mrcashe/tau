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

#include <tau/color.hh>
#include <tau/display.hh>
#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/painter.hh>
#include <pixmap-impl.hh>

namespace tau {

Pixmap::Pixmap() {}

Pixmap::Pixmap(Pixmap_ptr pp): impl(pp) {}

Pixmap::Pixmap(int depth, const Size & size):
    impl(Pixmap_impl::create(depth, size))
{
}

Pixmap::Pixmap(int depth, unsigned width, unsigned height):
    impl(Pixmap_impl::create(depth, Size(width, height)))
{
}

Pixmap::operator bool() const { return nullptr != impl; }

void Pixmap::reset() { impl.reset(); }

bool Pixmap::empty() const {
    return impl ? impl->size().empty() : true;
}

Size Pixmap::size() const {
    return impl ? impl->size() : Size();
}

int Pixmap::depth() const {
    return impl ? impl->depth() : -1;
}

std::size_t Pixmap::bytes() const {
    return impl ? impl->bytes() : 0;
}

const uint8_t * Pixmap::raw() const {
    return impl ? impl->raw() : nullptr;
}

Vector Pixmap::ppi() const {
    return impl ? impl->ppi() : Vector();
}

void Pixmap::set_ppi(const Vector & value) {
    if (impl) { impl->set_ppi(value); }
}

void Pixmap::resize(unsigned width, unsigned height) {
    if (impl) { impl->resize(Size(width, height)); }
}

void Pixmap::resize(const Size & sz) {
    if (impl) { impl->resize(sz); }
}

void Pixmap::put_pixel(int x, int y, const Color & c) {
    if (impl) { impl->put_pixel(Point(x, y), c); }
}

void Pixmap::put_pixel(const Point & pt, const Color & c) {
    if (impl) { impl->put_pixel(pt, c); }
}

signal<void()> & Pixmap::signal_changed() {
    if (impl) { return impl->signal_changed(); }
    throw user_error("Pixmap::signal_changed(): unable to return reference to the signal_changed_ due to empty Pixmap");
}

Color Pixmap::get_pixel(int x, int y) const {
    return impl ? impl->get_pixel(Point(x, y)) : Color();
}

Color Pixmap::get_pixel(const Point & pt) const {
    return impl ? impl->get_pixel(pt) : Color();
}

void Pixmap::copy(const Pixmap other) {
    if (impl) { impl->copy(other.impl); }
}

void Pixmap::set_argb32(int x, int y, const uint8_t * buffer, std::size_t nbytes) {
    if (impl) { impl->set_argb32(Point(x, y), buffer, nbytes); }
}

void Pixmap::set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes) {
    if (impl) { impl->set_argb32(pt, buffer, nbytes); }
}

Painter Pixmap::painter() {
    return impl ? impl->painter() : Painter();
}

} // namespace tau

//END
