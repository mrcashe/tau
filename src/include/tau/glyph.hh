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

/// @file glyph.hh The Glyph class.

#ifndef TAU_GLYPH_HH
#define TAU_GLYPH_HH

#include <tau/contour.hh>
#include <tau/geometry.hh>

namespace tau {

/// A glyph represents a unit of rendered content within a Font.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// Often, there is a one-to-one correspondence between characters to be drawn and
/// corresponding glyphs (e.g., often, the character "A" is rendered using a single glyph),
/// but other times multiple glyphs are used to render a single character
/// (e.g., use of accents) or a single glyph can be used to render multiple characters
/// (e.g., ligatures). Typically, a glyph is defined by one or more shapes such as a path,
/// possibly with additional information such as rendering hints that help a font engine
/// to produce legible text in small sizes.
/// @ingroup font_group
class Glyph {
public:

    /// Default constructor make an empty glyph.
    Glyph();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Glyph(const Glyph & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Glyph & operator=(const Glyph & other) = default;

    /// Test if empty.
    operator bool() const;

    /// Reset underlying implementation.
    void reset();

    /// Get (xmin::ymin) pair.
    Vector min() const;

    /// Get (xmax::ymax) pair.
    Vector max() const;

    /// Get bearing.
    Vector bearing() const;

    /// Get advance.
    Vector advance() const;

    /// Get pixmap bounds.
    Rect bounds() const;

    /// Get contours.
    std::vector<Contour> contours() const;

private:

    Glyph(Glyph_ptr gl);
    Glyph_ptr impl;
    friend class Glyph_impl;
};

} // namespace tau

#endif // TAU_GLYPH_HH
