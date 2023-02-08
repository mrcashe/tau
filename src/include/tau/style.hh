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

/// @file style.hh The Style class.

#ifndef TAU_STYLE_HH
#define TAU_STYLE_HH

#include <tau/color.hh>
#include <tau/signal.hh>
#include <tau/types.hh>

namespace tau {

/// Basic style item.
/// @ingroup style_group
class Style_item: public trackable {
public:

    /// @private
    Style_item(Style_impl * sty=nullptr);

    /// Copy constructor.
    Style_item(const Style_item & other) = default;

    /// Copy operator.
    Style_item & operator=(const Style_item & other) = default;

    /// Get value.
    ustring get() const;

    /// Set value or format.
    void set(const ustring & val=ustring());

    /// Test if value set.
    bool is_set() const;

    /// Unset value.
    void unset();

    /// Get "signal_changed".
    signal<void()> & signal_changed();

private:

    friend Style_impl;
    friend Font_style;

    Style_impl * sty_ = nullptr;

private:

    ustring & format();
};

/// Font style item.
/// @ingroup style_group
class Font_style {
public:

    Font_style(Style_item & si);

    /// Get font specification string.
    ustring spec() const;

    /// Set font specification.
    void set(const ustring & spec);

    /// Get font specification string.
    operator ustring() const;

    /// Set font specification.
    void operator=(const ustring & s);

    /// Get font size in points.
    double size() const;

    /// Set font size.
    /// @param pts font size in points.
    void resize(double pts);

    /// Enlarge font size.
    /// @param pts font size in points.
    void enlarge(double pts);

    /// Grow font size.
    /// @param pts font size in points.
    void grow(double pts);

    /// Add face elements to the font.
    /// @param face_elements element to be added to font face specification.
    /// If current font specification is "Arial Bold" and the face_elements is
    /// "Italic", the result will be "Arial Bold Italic".
    void add_face(const ustring & face_elements);

    /// Set face.
    /// @param face face specification.
    /// If current font specification is "Arial Bold" and the face is
    /// "Italic", the result will be "Arial Italic".
    void set_face(const ustring & face);

private:

    Style_item & si_;

private:

    void clear_size();
};

/// Color style item.
/// @ingroup style_group
class Color_style {
public:

    Color_style(Style_item & si);

    /// Get color value.
    Color get() const;

    /// Set new color value.
    /// @param color the color.
    void set(const Color & color);

    /// Get color value.
    operator Color() const;

    /// Set new color value.
    Color_style & operator=(const Color & color);

private:

    Style_item & si_;
};

/// CSS-like style.
/// @ingroup style_group
/// @sa @ref style_sect
class Style {
public:

    /// Default constructor.
    Style();

    /// Destructor.
   ~Style();

    /// Copy constructor.
    Style(const Style & other);

    /// Copy operator.
    Style & operator=(const Style & other);

    /// Set parent style.
    void set_parent(Style & parent);

    /// Unset parent style.
    void unparent();

    /// Assign or reassign a value to the item.
    /// @sa @ref style_sect
    Style_item & set(const std::string & name, const ustring & value=ustring());

    /// Get item by name.
    /// @sa @ref style_sect
    Style_item & get(const std::string & name);

    /// Get constant item by name.
    /// @sa @ref style_sect
    const Style_item & get(const std::string & name) const;

    /// Get wrapped Style_item into Font_style.
    /// @sa @ref style_sect
    Font_style font(const std::string & name);

    /// Get wrapped Style_item into Color_style.
    /// @sa @ref style_sect
    Color_style color(const std::string & name);

    /// Redirect item.
    /// @param src item that will replace @a dest
    /// @param dest item that will be replaced
    /// @sa @ref style_sect
    void redirect(const std::string & src, const std::string & dest);

    /// Unset specified item.
    /// @sa @ref style_sect
    void unset(const std::string & name);

    /// Unset all items.
    void unset();

private:

    Style_ptr impl;
};

/// @name String variables providing style names
/// @{

/// Font style name.
/// The actual value is "font".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_FONT;

/// Foreground style name.
/// The actual value is "foreground".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_FOREGROUND;

/// Menu foreground style name.
/// The actual value is "menu/foreground".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_MENU_FOREGROUND;

/// Slider foreground style name.
/// The actual value is "slider/foreground".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_SLIDER_FOREGROUND;

/// Progress foreground style name.
/// The actual value is "progress/foreground".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_PROGRESS_FOREGROUND;

/// Accel foreground style name.
/// The actual value is "accel/foreground".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_ACCEL_FOREGROUND;

/// Background style name.
/// The actual value is "background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_BACKGROUND;

/// Progress background style name.
/// The actual value is "progress/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_PROGRESS_BACKGROUND;

/// Whitespace background style name.
/// The actual value is "whitespace/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_WHITESPACE_BACKGROUND;

/// Menu background style name.
/// The actual value is "menu/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_MENU_BACKGROUND;

/// Selection background style name.
/// The actual value is "select/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_SELECT_BACKGROUND;

/// Tooltip background style name.
/// The actual value is "tooltip/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_TOOLTIP_BACKGROUND;

/// Button background style name.
/// The actual value is "button/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_BUTTON_BACKGROUND;

/// Slider background style name.
/// The actual value is "slider/background".
/// @ingroup style_group
/// @sa @ref style_sect
extern const char * STYLE_SLIDER_BACKGROUND;

/// @}

} // namespace tau

#endif // TAU_STYLE_HH
