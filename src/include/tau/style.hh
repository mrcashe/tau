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

    /// Default constructor.
    Style_item() = default;

    /// Copy constructor.
    Style_item(const Style_item & other) = default;

    /// Copy operator.
    Style_item & operator=(const Style_item & other) = default;

    /// Constructor with value.
    Style_item(const ustring & val);

    /// Get value.
    ustring get() const;

    /// Set value or format.
    void set(const ustring & val=ustring());

    /// Test if value set.
    bool is_set() const;

    /// Unset value.
    void unset();

    /// Get format string.
    ustring format() const {
        return fmt_;
    }

    /// Get "signal_changed".
    signal<void()> & signal_changed() {
        return signal_changed_;
    }

private:

    friend Style_impl;

    ustring     value_;
    ustring     pvalue_;
    ustring     fmt_;
    bool        set_ = false;
    connection  cx_;

    signal<void(ustring)> signal_value_changed_;
    signal<void()> signal_changed_;

private:

    void set_pvalue(const ustring & pvalue);
};

/// Font style item.
/// @ingroup style_group
class Font_style {
public:

    Font_style(Style_item & si):
        si_(si)
    {
    }

    /// Get font specification string.
    ustring spec() const;

    /// Set font specification.
    void set(const ustring & spec);

    /// Get font specification string.
    operator ustring() const {
        return spec();
    }

    /// Set font specification.
    void operator=(const ustring & s) {
        set(s);
    }

    /// Get font size in points.
    double size() const;

    /// Set font size.
    /// @param pt size in points.
    void resize(double pts);

    /// Enlarge font size.
    /// @param pt size in points.
    void enlarge(double pts);

    /// Grow font size.
    /// @param pt size in points.
    void grow(double pts);

    /// Add face modificator to the font.
    /// @param face_elements element to be added to font face specification.
    /// If current font specificaion is "Arial Bold" and the face_elements is
    /// "Italic", the result will be "Arial Bold Italic".
    void add_face(const ustring & face_elements);

    /// Set face.
    /// @param face face specification.
    /// If current font specificaion is "Arial Bold" and the face is
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

    Color_style(Style_item & si):
        si_(si)
    {
    }

    /// Get color value.
    Color get() const;

    /// Set new color value.
    /// @param color the color.
    void set(const Color & color);

    /// Get color value.
    operator Color() const {
        return get();
    }

    /// Set new color value.
    Color_style & operator=(const Color & color) {
        set(color);
        return *this;
    }

private:

    Style_item & si_;
};

/// CSS-like style.
/// @ingroup style_group
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

    /// Find item by name.
    Style_item * find(const std::string & name);

    /// Assign or reassign a value to the item.
    Style_item & set(const std::string & name, const ustring & value=ustring());

    /// Get item by name.
    Style_item & get(const std::string & name);

    /// Get const item by name.
    const Style_item & get(const std::string & name) const;

    /// Get wrapped Style_item into Font_style.
    Font_style font(const std::string & name);

    /// Get wrapped Style_item into Color_style.
    Color_style color(const std::string & name);

    /// Redirect item.
    /// @param src item that will replace @a dest
    /// @param dest item that will be replaced
    void redirect(const std::string & src, const std::string & dest);

    /// Unset specified item.
    void unset(const std::string & name);

    /// Unset all items.
    void unset();

private:

    Style_impl * impl;
};

} // namespace tau

#endif // TAU_STYLE_HH
