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

#include <tau/font.hh>
#include <font-impl.hh>
#include <glyph-impl.hh>
#include <theme-impl.hh>
#include <iostream>     // Needed for error logging!

namespace {

void log(const char * s) {
    std::cerr << "tau::Font::" << s << "() called on empty Font" << std::endl;
}

} // anonymous namespace

namespace tau {

Font::Font() {}

Font::~Font() {}

Font::Font(Font_ptr fp):
    impl(fp)
{
}

bool Font::operator==(const Font & other) const {
    return impl && other.impl && impl.get() == other.impl.get();
}

bool Font::operator!=(const Font & other) const {
    return !impl || !other.impl || impl.get() != other.impl.get();
}

Font::operator bool() const {
    return nullptr != impl;
}

void Font::reset() {
    impl.reset();
}

ustring Font::spec() const {
    if (impl) {
        return impl->spec();
    }

    else {
        log("spec");
        return ustring();
    }
}

ustring Font::psname() const {
    if (impl) {
        return impl->psname();
    }

    else {
        log("psname");
        return ustring();
    }
}

unsigned Font::dpi() const {
    if (impl) {
        return impl->dpi();
    }

    else {
        log("dpi");
        return 0;
    }
}

double Font::ascent() const {
    if (impl) {
        return impl->ascent();
    }

    else {
        log("ascent");
        return 0.0;
    }
}

double Font::descent() const {
    if (impl) {
        return impl->descent();
    }

    else {
        log("descent");
        return 0.0;
    }
}

double Font::linegap() const {
    if (impl) {
        return impl->linegap();
    }

    else {
        log("linegap");
        return 0.0;
    }
}

Vector Font::min() const {
    if (impl) {
        return impl->min();
    }

    else {
        log("min");
        return Vector();
    }
}

Vector Font::max() const {
    if (impl) {
        return impl->max();
    }

    else {
        log("max");
        return Vector();
    }
}

Glyph Font::glyph(char32_t wc) {
    if (impl) {
        return Glyph_impl::wrap(impl->glyph(wc));
    }

    else {
        log("glyph");
        return Glyph();
    }
}

// static
ustring Font::normal() {
    return Theme_impl::root()->font_normal();
}

// static
ustring Font::mono() {
    return Theme_impl::root()->font_mono();
}

} // namespace tau

//END
