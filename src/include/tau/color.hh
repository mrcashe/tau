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

/// @file color.hh The Color class.

#ifndef TAU_COLOR_HH
#define TAU_COLOR_HH

#include <tau/ustring.hh>
#include <vector>

namespace tau {

/// ARGB and HSV model color.
/// @ingroup paint_group
class Color {
public:

    /// @name Constructors
    /// @{

    /// Default constructor.
    Color() = default;

    /// Copy constructor.
    Color(const Color & other) = default;

    /// Copy operator.
    Color & operator=(const Color & other) = default;

    /// Constructor with RGB, range is 0.0...1.0.
    Color(double red, double green, double blue, double alpha=1.0);

    /// Constructor with HTML-like string and alpha.
    Color(const ustring & str, double alpha=1.0);

    /// @}
    /// Assign value from HTML-like string and alpha.
    void set(const ustring & str, double alpha=1.0);

    /// Assign value form ARGB components, range is 0.0...1.0.
    void set(double red, double green, double blue, double alpha=1.0);

    /// Assign value from RGB24.
    void set_rgb24(uint32_t rgb24, double alpha=1.0);

    /// Set red component, range is 0.0...1.0.
    void set_red(double red);

    /// Set green component, range is 0.0...1.0.
    void set_green(double green);

    /// Set blue component, range is 0.0...1.0.
    void set_blue(double blue);

    /// Set alpha channel value, range is 0.0...1.0.
    void set_alpha(double alpha);

    /// Set HSV components, hue range is 0.0...360.0, other range is 0.0...1.0.
    void set_hsv(double hue, double saturation, double value);

    /// Set HSV components and alpha channel, hue range is 0.0...360.0, other range is 0.0...1.0.
    void set_hsv(double hue, double saturation, double value, double alpha);

    /// Set hue component, range is 0.0...360.0.
    void set_hue(double hue);

    /// Set saturation component, range is 0.0...1.0.
    void set_saturation(double sat);

    /// Set value component, range is 0.0...1.0.
    void set_value(double value);

    /// Get red component, range is 0.0...1.0.
    double red() const {
        return red_;
    }

    /// Get green component, range is 0.0...1.0.
    double green() const {
        return green_;
    }

    /// Get blue component, range is 0.0...1.0.
    double blue() const {
        return blue_;
    }

    /// Get alpha channel value, range is 0.0...1.0.
    double alpha() const {
        return alpha_;
    }

    /// Get hue component, range is 0.0...1.0.
    double hue() const {
        return hue_;
    }

    /// Get saturation component, range is 0.0...1.0.
    double saturation() const {
        return sat_;
    }

    /// Get value component, range is 0.0...1.0.
    double value() const {
        return value_;
    }

    /// Assignment from HTML-like string.
    Color & operator=(const ustring & text) {
        set(text);
        return *this;
    }

    /// Compare operator.
    bool operator==(const Color & other) const {
        return red_ == other.red_ && green_ == other.green_ && blue_ == other.blue_ && alpha_ == other.alpha_;
    }

    /// Compare operator.
    bool operator!=(const Color & other) const {
        return red_ != other.red_ || green_ != other.green_ || blue_ != other.blue_ || alpha_ != other.alpha_;
    }

    /// Get ARGB32 data as 32-bit value, range 0...255 for every component.
    uint32_t argb32() const;

    /// Get ABGR32 data as 32-bit value, range 0...255 for every component.
    uint32_t abgr32() const;

    /// Get RGB24 data as 32-bit value, range 0...255 for every component.
    uint32_t rgb24() const;

    /// Get BGR24 data as 32-bit value, range 0...255 for every component.
    uint32_t bgr24() const;

    /// Convert to 8-bit gray.
    uint8_t gray8() const;

    /// Convert to 24-bit gray.
    uint32_t gray24() const;

    /// Convert to gray.
    double gray() const;

    /// Get HTML-like string representation (smt. like #102030).
    ustring html() const;

    /// Get inverted color.
    Color inverted() const;

    /// Make lighter.
    void  lighter(double factor);

    /// Make darker.
    void  darker(double factor);

    /// Return lighten color.
    Color lighten(double factor) const;

    /// Return darken color.
    Color darken(double factor) const;

    /// Return low-contrast color (used for disabled GUI elements painting).
    Color inactive() const;

    /// Create from ARGB32 data.
    static Color from_argb32(uint32_t argb32);

    /// Create from RGB24 data.
    static Color from_rgb24(uint32_t rgb24, double alpha=1.0);

    /// Create from gray level, range is 0.0...1.0.
    static Color from_gray(double gray, double alpha=1.0);

    /// Create from 8-bit gray level, range is 0...255.
    static Color from_gray8(uint8_t gray, double alpha=1.0);

    /// List CSS color names.
    static std::vector<ustring> list_css_names();

private:

    void calc_hsv();
    void calc_rgb();

private:

    double red_         = 0.0;
    double green_       = 0.0;
    double blue_        = 0.0;
    double alpha_       = 1.0;
    double hue_         = 0.0;
    double sat_         = 0.0;
    double value_       = 0.0;
};

} // namespace tau

#endif // TAU_COLOR_HH
