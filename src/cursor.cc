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

#include <tau/cursor.hh>
#include <tau/locale.hh>
#include <cursor-impl.hh>
#include <pixmap-impl.hh>
#include <fstream>

namespace tau {

Cursor::Cursor() {}

Cursor::Cursor(Cursor_ptr cp): impl(cp) {}

Cursor::Cursor(Pixmap pix, const Point & hotspot):
    impl(Cursor_impl::create(Pixmap_impl::strip(pix), hotspot))
{
}

/// Test if empty.
Cursor::operator bool() const { return nullptr != impl; }

/// Reset underlying implementation.
void Cursor::reset() { impl.reset(); }

unsigned Cursor::size() const {
    return impl ? impl->size() : 0;
}

int Cursor::frame_count() const {
    return impl ? impl->frame_count() : -1;
}

Pixmap Cursor::pixmap(int index) const {
    return impl ? Pixmap_impl::wrap(impl->pixmap(index)) : Pixmap();
}

Point Cursor::hotspot(int nth_frame) const {
    return impl ? impl->hotspot(nth_frame) : Point();
}

unsigned Cursor::delay(int nth_frame) const {
    return impl ? impl->delay(nth_frame) : 0;
}

unsigned Cursor::delay() const {
    return impl ? impl->delay() : 0;
}

void Cursor::assign(Pixmap pix, const Point & hotspot) {
    if (impl) { impl->assign(Pixmap_impl::strip(pix), hotspot); }
}

int Cursor::append(Pixmap pix, unsigned delay_ms, const Point & hotspot) {
    return impl ? impl->append(Pixmap_impl::strip(pix), delay_ms, hotspot) : -1;
}

void Cursor::prepend(Pixmap pix, unsigned delay_ms, const Point & hotspot) {
    if (impl) { impl->prepend(Pixmap_impl::strip(pix), delay_ms, hotspot); }
}

int Cursor::insert(int nth_frame, Pixmap pix, unsigned delay_ms, const Point & hotspot) {
    return impl ? impl->insert(nth_frame, Pixmap_impl::strip(pix), delay_ms, hotspot) : -1;
}

void Cursor::set_pixmap(Pixmap pix, int nth_frame) {
    if (impl) { impl->set_pixmap(Pixmap_impl::strip(pix), nth_frame); }
}

void Cursor::set_delay(unsigned delay_ms, int nth_frame) {
    if (impl) { impl->set_delay(delay_ms, nth_frame); }
}

void Cursor::set_hotspot(const Point & hotspot, int nth_frame) {
    if (impl) { impl->set_hotspot(hotspot, nth_frame); }
}

void Cursor::set_delay(unsigned delay_ms) {
    if (impl) { impl->set_delay(delay_ms); }
}

// static
Cursor Cursor::load_from_file(const ustring & path, unsigned size_px) {
    return Cursor(Cursor_impl::load_from_file(path, size_px));
}

} // namespace tau

//END
