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
    double red() const;

    /// Get green component, range is 0.0...1.0.
    double green() const;

    /// Get blue component, range is 0.0...1.0.
    double blue() const;

    /// Get alpha channel value, range is 0.0...1.0.
    double alpha() const;

    /// Get hue component, range is 0.0...1.0.
    double hue() const;

    /// Get saturation component, range is 0.0...1.0.
    double saturation() const;

    /// Get value component, range is 0.0...1.0.
    double value() const;

    /// Assignment from HTML-like string.
    Color & operator=(const ustring & text);

    /// Compare operator.
    bool operator==(const Color & other) const;

    /// Compare operator.
    bool operator!=(const Color & other) const;

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

/// @name Const access string variables providing CSS color names
/// @sa @ref color_sect
/// @{

/// The actual value is "AliceBlue".
/// @ingroup paint_group
extern const char * COLOR_ALICE_BLUE;

/// The actual value is "AntiqueWhite".
/// @ingroup paint_group
extern const char * COLOR_ANTIQUE_WHITE;

/// The actual value is "Aqua".
/// @ingroup paint_group
extern const char * COLOR_AQUA;

/// The actual value is "Aquamarine".
/// @ingroup paint_group
extern const char * COLOR_AQUAMARINE;

/// The actual value is "Azure".
/// @ingroup paint_group
extern const char * COLOR_AZURE;

/// The actual value is "Beige".
/// @ingroup paint_group
extern const char * COLOR_BEIGE;

/// The actual value is "Bisque".
/// @ingroup paint_group
extern const char * COLOR_BISQUE;

/// The actual value is "Black".
/// @ingroup paint_group
extern const char * COLOR_BLACK;

/// The actual value is "BlanchedAlmond".
/// @ingroup paint_group
extern const char * COLOR_BLANCHED_ALMOND;

/// The actual value is "Blue".
/// @ingroup paint_group
extern const char * COLOR_BLUE;

/// The actual value is "BlueViolet".
/// @ingroup paint_group
extern const char * COLOR_BLUE_VIOLET;

/// The actual value is "Brown".
/// @ingroup paint_group
extern const char * COLOR_BROWN;

/// The actual value is "BurlyWood".
/// @ingroup paint_group
extern const char * COLOR_BURLY_WOOD;

/// The actual value is "CadetBlue".
/// @ingroup paint_group
extern const char * COLOR_CADET_BLUE;

/// The actual value is "Chartreuse".
/// @ingroup paint_group
extern const char * COLOR_CHARTREUSE;

/// The actual value is "Chocolate".
/// @ingroup paint_group
extern const char * COLOR_CHOCOLATE;

/// The actual value is "Coral".
/// @ingroup paint_group
extern const char * COLOR_CORAL;

/// The actual value is "CornflowerBlue".
/// @ingroup paint_group
extern const char * COLOR_CORNFLOWER_BLUE;

/// The actual value is "Cornsilk".
/// @ingroup paint_group
extern const char * COLOR_CORNSILK;

/// The actual value is "Crimson".
/// @ingroup paint_group
extern const char * COLOR_CRIMSON;

/// The actual value is "Cyan".
/// @ingroup paint_group
extern const char * COLOR_CYAN;

/// The actual value is "DarkBlue".
/// @ingroup paint_group
extern const char * COLOR_DARK_BLUE;

/// The actual value is "DarkCyan".
/// @ingroup paint_group
extern const char * COLOR_DARK_CYAN;

/// The actual value is "DarkGoldenRod".
/// @ingroup paint_group
extern const char * COLOR_DARK_GOLDEN_ROD;

/// The actual value is "DarkGray".
/// @ingroup paint_group
extern const char * COLOR_DARK_GRAY;

/// The actual value is "DarkGreen".
/// @ingroup paint_group
extern const char * COLOR_DARK_GREEN;

/// The actual value is "DarkKhaki".
/// @ingroup paint_group
extern const char * COLOR_DARK_KHAKI;

/// The actual value is "DarkMagenta".
/// @ingroup paint_group
extern const char * COLOR_DARK_MAGENTA;

/// The actual value is "DarkOliveGreen".
/// @ingroup paint_group
extern const char * COLOR_DARK_OLIVE_GREEN;

/// The actual value is "DarkOrange".
/// @ingroup paint_group
extern const char * COLOR_DARK_ORANGE;

/// The actual value is "DarkOrchid".
/// @ingroup paint_group
extern const char * COLOR_DARK_ORCHID;

/// The actual value is "DarkRed".
/// @ingroup paint_group
extern const char * COLOR_DARK_RED;

/// The actual value is "DarkSalmon".
/// @ingroup paint_group
extern const char * COLOR_DARK_SALMON;

/// The actual value is "DarkSeaGreen".
/// @ingroup paint_group
extern const char * COLOR_DARK_SEA_GREEN;

/// The actual value is "DarkSlateBlue".
/// @ingroup paint_group
extern const char * COLOR_DARK_SLATE_BLUE;

/// The actual value is "DarkSlateGray".
/// @ingroup paint_group
extern const char * COLOR_DARK_SLATE_GRAY;

/// The actual value is "DarkTurquoise".
/// @ingroup paint_group
extern const char * COLOR_DARK_TURQUOISE;

/// The actual value is "DarkViolet".
/// @ingroup paint_group
extern const char * COLOR_DARK_VIOLET;

/// The actual value is "DeepPink".
/// @ingroup paint_group
extern const char * COLOR_DEEP_PINK;

/// The actual value is "DeepSkyBlue".
/// @ingroup paint_group
extern const char * COLOR_DEEP_SKY_BLUE;

/// The actual value is "DimGray".
/// @ingroup paint_group
extern const char * COLOR_DIM_GRAY;

/// The actual value is "DodgerBlue".
/// @ingroup paint_group
extern const char * COLOR_DODGER_BLUE;

/// The actual value is "FireBrick".
/// @ingroup paint_group
extern const char * COLOR_FIRE_BRICK;

/// The actual value is "FloralWhite".
/// @ingroup paint_group
extern const char * COLOR_FLORAL_WHITE;

/// The actual value is "ForestGreen".
/// @ingroup paint_group
extern const char * COLOR_FOREST_GREEN;

/// The actual value is "Fuchsia".
/// @ingroup paint_group
extern const char * COLOR_FUCHSIA;

/// The actual value is "Gainsboro".
/// @ingroup paint_group
extern const char * COLOR_GAINSBORO;

/// The actual value is "GhostWhite".
/// @ingroup paint_group
extern const char * COLOR_GHOST_WHITE;

/// The actual value is "Gold".
/// @ingroup paint_group
extern const char * COLOR_GOLD;

/// The actual value is "GoldenRod".
/// @ingroup paint_group
extern const char * COLOR_GOLDEN_ROD;

/// The actual value is "Gray".
/// @ingroup paint_group
extern const char * COLOR_GRAY;

/// The actual value is "Green".
/// @ingroup paint_group
extern const char * COLOR_GREEN;

/// The actual value is "GreenYellow".
/// @ingroup paint_group
extern const char * COLOR_GREEN_YELLOW;

/// The actual value is "HoneyDew".
/// @ingroup paint_group
extern const char * COLOR_HONEY_DEW;

/// The actual value is "HotPink".
/// @ingroup paint_group
extern const char * COLOR_HOT_PINK;

/// The actual value is "IndianRed".
/// @ingroup paint_group
extern const char * COLOR_INDIAN_RED;

/// The actual value is "Indigo".
/// @ingroup paint_group
extern const char * COLOR_INDIGO;

/// The actual value is "Ivory".
/// @ingroup paint_group
extern const char * COLOR_IVORY;

/// The actual value is "Khaki".
/// @ingroup paint_group
extern const char * COLOR_KHAKI;

/// The actual value is "Lavender".
/// @ingroup paint_group
extern const char * COLOR_LAVENDER;

/// The actual value is "LavenderBlush".
/// @ingroup paint_group
extern const char * COLOR_LAVENDER_BLUSH;

/// The actual value is "LawnGreen".
/// @ingroup paint_group
extern const char * COLOR_LAWN_GREEN;

/// The actual value is "LemonChiffon".
/// @ingroup paint_group
extern const char * COLOR_LEMON_CHIFFON;

/// The actual value is "LightBlue".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_BLUE;

/// The actual value is "LightCoral".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_CORAL;

/// The actual value is "LightCyan".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_CYAN;

/// The actual value is "LightGoldenRodYellow".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_GOLDEN_ROD_YELLOW;

/// The actual value is "LightGray".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_GRAY;

/// The actual value is "LightGreen".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_GREEN;

/// The actual value is "LightPink".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_PINK;

/// The actual value is "LightSalmon".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_SALMON;

/// The actual value is "LightSeaGreen".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_SEA_GREEN;

/// The actual value is "LightSkyBlue".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_SKY_BLUE;

/// The actual value is "LightSlateGray".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_SLATE_GRAY;

/// The actual value is "LightSteelBlue".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_STEEL_BLUE;

/// The actual value is "LightYellow".
/// @ingroup paint_group
extern const char * COLOR_LIGHT_YELLOW;

/// The actual value is "Lime".
/// @ingroup paint_group
extern const char * COLOR_LIME;

/// The actual value is "LimeGreen".
/// @ingroup paint_group
extern const char * COLOR_LIME_GREEN;

/// The actual value is "Linen".
/// @ingroup paint_group
extern const char * COLOR_LINEN;

/// The actual value is "Magenta".
/// @ingroup paint_group
extern const char * COLOR_MAGENTA;

/// The actual value is "Maroon".
/// @ingroup paint_group
extern const char * COLOR_MAROON;

/// The actual value is "MediumAquaMarine".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_AQUA_MARINE;

/// The actual value is "MediumBlue".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_BLUE;

/// The actual value is "MediumOrchid".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_ORCHID;

/// The actual value is "MediumPurple".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_PURPLE;

/// The actual value is "MediumSeaGreen".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_SEA_GREEN;

/// The actual value is "MediumSlateBlue".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_SLATE_BLUE;

/// The actual value is "MediumSpringGreen".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_SPRING_GREEN;

/// The actual value is "MediumTurquoise".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_TURQUOISE;

/// The actual value is "MediumVioletRed".
/// @ingroup paint_group
extern const char * COLOR_MEDIUM_VIOLET_RED;

/// The actual value is "MidnightBlue".
/// @ingroup paint_group
extern const char * COLOR_MIDNIGHT_BLUE;

/// The actual value is "MintCream".
/// @ingroup paint_group
extern const char * COLOR_MINT_CREAM;

/// The actual value is "MistyRose".
/// @ingroup paint_group
extern const char * COLOR_MISTY_ROSE;

/// The actual value is "Moccasin".
/// @ingroup paint_group
extern const char * COLOR_MOCCASIN;

/// The actual value is "NavajoWhite".
/// @ingroup paint_group
extern const char * COLOR_NAVAJO_WHITE;

/// The actual value is "Navy".
/// @ingroup paint_group
extern const char * COLOR_NAVY;

/// The actual value is "OldLace".
/// @ingroup paint_group
extern const char * COLOR_OLD_LACE;

/// The actual value is "Olive".
/// @ingroup paint_group
extern const char * COLOR_OLIVE;

/// The actual value is "OliveDrab".
/// @ingroup paint_group
extern const char * COLOR_OLIVE_DRAB;

/// The actual value is "Orange".
/// @ingroup paint_group
extern const char * COLOR_ORANGE;

/// The actual value is "OrangeRed".
/// @ingroup paint_group
extern const char * COLOR_ORANGE_RED;

/// The actual value is "Orchid".
/// @ingroup paint_group
extern const char * COLOR_ORCHID;

/// The actual value is "PaleGoldenRod".
/// @ingroup paint_group
extern const char * COLOR_PALE_GOLDEN_ROD;

/// The actual value is "PaleGreen".
/// @ingroup paint_group
extern const char * COLOR_PALE_GREEN;

/// The actual value is "PaleTurquoise".
/// @ingroup paint_group
extern const char * COLOR_PALE_TURQUOISE;

/// The actual value is "PaleVioletRed".
/// @ingroup paint_group
extern const char * COLOR_PALE_VIOLET_RED;

/// The actual value is "PapayaWhip".
/// @ingroup paint_group
extern const char * COLOR_PAPAYA_WHIP;

/// The actual value is "PeachPuff".
/// @ingroup paint_group
extern const char * COLOR_PEACH_PUFF;

/// The actual value is "Peru".
/// @ingroup paint_group
extern const char * COLOR_PERU;

/// The actual value is "Pink".
/// @ingroup paint_group
extern const char * COLOR_PINK;

/// The actual value is "Plum".
/// @ingroup paint_group
extern const char * COLOR_PLUM;

/// The actual value is "PowderBlue".
/// @ingroup paint_group
extern const char * COLOR_POWDER_BLUE;

/// The actual value is "Purple".
/// @ingroup paint_group
extern const char * COLOR_PURPLE;

/// The actual value is "Red".
/// @ingroup paint_group
extern const char * COLOR_RED;

/// The actual value is "RosyBrown".
/// @ingroup paint_group
extern const char * COLOR_ROSY_BROWN;

/// The actual value is "RoyalBlue".
/// @ingroup paint_group
extern const char * COLOR_ROYAL_BLUE;

/// The actual value is "SaddleBrown".
/// @ingroup paint_group
extern const char * COLOR_SADDLE_BROWN;

/// The actual value is "Salmon".
/// @ingroup paint_group
extern const char * COLOR_SALMON;

/// The actual value is "SandyBrown".
/// @ingroup paint_group
extern const char * COLOR_SANDY_BROWN;

/// The actual value is "SeaGreen".
/// @ingroup paint_group
extern const char * COLOR_SEA_GREEN;

/// The actual value is "SeaShell".
/// @ingroup paint_group
extern const char * COLOR_SEA_SHELL;

/// The actual value is "Sienna".
/// @ingroup paint_group
extern const char * COLOR_SIENNA;

/// The actual value is "Silver".
/// @ingroup paint_group
extern const char * COLOR_SILVER;

/// The actual value is "SkyBlue".
/// @ingroup paint_group
extern const char * COLOR_SKY_BLUE;

/// The actual value is "SlateBlue".
/// @ingroup paint_group
extern const char * COLOR_SLATE_BLUE;

/// The actual value is "SlateGray".
/// @ingroup paint_group
extern const char * COLOR_SLATE_GRAY;

/// The actual value is "Snow".
/// @ingroup paint_group
extern const char * COLOR_SNOW;

/// The actual value is "SpringGreen".
/// @ingroup paint_group
extern const char * COLOR_SPRING_GREEN;

/// The actual value is "SteelBlue".
/// @ingroup paint_group
extern const char * COLOR_STEEL_BLUE;

/// The actual value is "Tan".
/// @ingroup paint_group
extern const char * COLOR_TAN;

/// The actual value is "Teal".
/// @ingroup paint_group
extern const char * COLOR_TEAL;

/// The actual value is "Thistle".
/// @ingroup paint_group
extern const char * COLOR_THISTLE;

/// The actual value is "Tomato".
/// @ingroup paint_group
extern const char * COLOR_TOMATO;

/// The actual value is "Turquoise".
/// @ingroup paint_group
extern const char * COLOR_TURQUOISE;

/// The actual value is "Violet".
/// @ingroup paint_group
extern const char * COLOR_VIOLET;

/// The actual value is "Wheat".
/// @ingroup paint_group
extern const char * COLOR_WHEAT;

/// The actual value is "White".
/// @ingroup paint_group
extern const char * COLOR_WHITE;

/// The actual value is "WhiteSmoke".
/// @ingroup paint_group
extern const char * COLOR_WHITE_SMOKE;

/// The actual value is "Yellow".
/// @ingroup paint_group
extern const char * COLOR_YELLOW;

/// The actual value is "YellowGreen".
/// @ingroup paint_group
extern const char * COLOR_YELLOW_GREEN;

/// @}

} // namespace tau

#endif // TAU_COLOR_HH
