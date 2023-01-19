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

#include <tau/display.hh>
#include <display-impl.hh>

namespace tau {

Display::Display():
    impl(Display_impl::this_display())
{
}

Display::Display(Display_ptr dp):
    impl(dp)
{
}

int Display::id() const {
    return impl->id();
}

Size Display::size_px() const {
    return impl->size_px();
}

Size Display::size_mm() const {
    return impl->size_mm();
}

int Display::dpi() const {
    return impl->dpi();
}

int Display::depth() const {
    return impl->depth();
}

bool Display::can_paste_text() const {
    return impl->can_paste_text();
}

void Display::paste_text() {
    impl->paste_text();
}

void Display::copy_text(const ustring & str) {
    impl->copy_text(str);
}

void Display::allow_screensaver() {
    impl->allow_screensaver();
}

void Display::disallow_screensaver() {
    impl->disallow_screensaver();
}

bool Display::screensaver_allowed() const {
    return impl->screensaver_allowed();
}

signal<void()> & Display::signal_can_paste() {
    return impl->signal_can_paste();
}

signal<void(const ustring &)> & Display::signal_paste_text() {
    return impl->signal_paste_text();
}

// static
Display Display::open(const ustring & args) {
    return Display(Display_impl::open(args));
}

} // namespace tau

//END
