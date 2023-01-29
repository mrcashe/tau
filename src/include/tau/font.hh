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

/// @file font.hh The Font class and font manipulating functions.

#ifndef TAU_FONT_HH
#define TAU_FONT_HH

#include <tau/types.hh>
#include <tau/geometry.hh>
#include <tau/ustring.hh>

namespace tau {

/// The scaled font.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// A font represents an organized collection of glyphs in which the various Glyph
/// representations will share a common look or styling such that, when a string of
/// characters is rendered together, the result is highly legible, conveys a particular
/// artistic style and provides consistent inter-character alignment and spacing.
/// @ingroup font_group
class Font {
public:

    /// Default constructor creates a pure font.
    Font();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Font(const Font & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Font & operator=(const Font & other) = default;

    /// Test if pure.
    operator bool() const;

    /// Reset underlying implementation.
    void reset();

    /// Compare operator.
    bool operator==(const Font & other) const;

    /// Compare operator.
    bool operator!=(const Font & other) const;

    /// List all available font families.
    static std::vector<ustring> list_families();

    /// List all available font faces for given font family.
    static std::vector<ustring> list_faces(const ustring & font_family);

    /// Get system default font specification.
    static ustring normal();

    /// Get system default monospace font specification.
    static ustring mono();

    /// Get font specification.
    ustring spec() const;

    /// Get PostScript font name.
    ustring psname() const;

    /// Get dots-per-inch.
    unsigned dpi() const;

    /// Get ascent.
    /// The distance from the baseline to the highest or
    /// upper grid coordinate used to place an outline point.
    double ascent() const;

    /// Get descent.
    /// The distance from the baseline to the lowest
    /// grid coordinate used to place an outline point.
    double descent() const;

    /// Get linegap.
    /// The distance that must be placed between two lines of text.
    double linegap() const;

    /// Get (xmin:ymin) pair.
    Vector min() const;

    /// Get (xmax:ymax) pair.
    Vector max() const;

    /// Get glyph for specified unicode character.
    Glyph glyph(char32_t wc);

private:

    Font_ptr impl;
    friend class Font_impl;
    Font(Font_ptr fp);
};

/// Split font specification into peaces.
std::vector<ustring> font_spec_explode(const ustring & spec);

/// Get font family from font specification.
/// @ingroup font_group
ustring font_family_from_spec(const ustring & spec);

/// Get font family from font specification.
/// @ingroup font_group
ustring font_family_from_spec(const std::vector<ustring> & specv);

/// Get font face from font specification.
/// @ingroup font_group
ustring font_face_from_spec(const ustring & spec);

/// Get font face from font specification.
/// @ingroup font_group
ustring font_face_from_spec(const std::vector<ustring> & specv);

/// Returns size component from font specification.
/// @param spec font specification.
/// @param fallback fallback value in case spec missing font size.
/// @ingroup font_group
double font_size_from_spec(const ustring & spec, double fallback=0.0);

/// Returns size component from font specification.
/// @param specv font specification vector.
/// @param fallback fallback value in case spec missing font size.
/// @ingroup font_group
double font_size_from_spec(const std::vector<ustring> & specv, double fallback=0.0);

/// Change size in font specification.
/// Changes size if it present and add font size if it initially absend.
/// @ingroup font_group
ustring font_size_change(const ustring & spec, double size_pt);

/// Change size in font specification.
/// Changes size if it less than given value or if it absend.
/// @ingroup font_group
ustring font_size_at_least(const ustring & spec, double min_size_pt);

/// Change size in font specification.
/// Changes size if it greater than given value or if it absend.
/// @ingroup font_group
ustring font_size_as_max(const ustring & spec, double max_size_pt);

/// Returns font specification without size component.
/// @ingroup font_group
ustring font_size_remove(const ustring & spec);

/// Build specification from components.
/// @ingroup font_group
ustring font_spec_build(const ustring & family, const ustring & face, double size_pt=0.0);

/// Build specification from components.
/// @ingroup font_group
ustring font_spec_build(const std::vector<ustring> & specv);

/// Add face modificator to the font.
/// @ingroup font_group
/// @param spec initial font specification.
/// @param face_elements elements to be added to font face specification.
/// If current font specificaion is "Arial Bold" and the face_elements is
/// "Italic", the result will be "Arial Bold Italic".
/// @return modified font specification.
ustring font_face_add(const ustring & spec, const ustring & face_elements);

/// Set face.
/// @ingroup font_group
/// @param spec initial font specification.
/// @param face the face specification.
/// If current font specificaion is "Arial Bold" and the face is
/// "Italic", the result will be "Arial Italic".
/// @return modified font specification.
ustring font_face_set(const ustring & spec, const ustring & face);

} // namespace tau

#endif // TAU_FONT_HH
