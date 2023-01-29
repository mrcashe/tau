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

#include <tau/color.hh>
#include <tau/string.hh>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <unordered_map>

namespace {

struct { const char * name; uint32_t rgb24; } named_colors[] = {
    { tau::COLOR_ALICE_BLUE,                 0xF0F8FF },
    { tau::COLOR_ANTIQUE_WHITE,              0xFAEBD7 },
    { tau::COLOR_AQUA,                       0x00FFFF },
    { tau::COLOR_AQUAMARINE,                 0x7FFFD4 },
    { tau::COLOR_AZURE,                      0xF0FFFF },
    { tau::COLOR_BEIGE,                      0xF5F5DC },
    { tau::COLOR_BISQUE,                     0xFFE4C4 },
    { tau::COLOR_BLACK,                      0x000000 },
    { tau::COLOR_BLANCHED_ALMOND,            0xFFEBCD },
    { tau::COLOR_BLUE,                       0x0000FF },
    { tau::COLOR_BLUE_VIOLET,                0x8A2BE2 },
    { tau::COLOR_BROWN,                      0xA52A2A },
    { tau::COLOR_BURLY_WOOD,                 0xDEB887 },
    { tau::COLOR_CADET_BLUE,                 0x5F9EA0 },
    { tau::COLOR_CHARTREUSE,                 0x7FFF00 },
    { tau::COLOR_CHOCOLATE,                  0xD2691E },
    { tau::COLOR_CORAL,                      0xFF7F50 },
    { tau::COLOR_CORNFLOWER_BLUE,            0x6495ED },
    { tau::COLOR_CORNSILK,                   0xFFF8DC },
    { tau::COLOR_CRIMSON,                    0xDC143C },
    { tau::COLOR_CYAN,                       0x00FFFF },
    { tau::COLOR_DARK_BLUE,                  0x00008B },
    { tau::COLOR_DARK_CYAN,                  0x008B8B },
    { tau::COLOR_DARK_GOLDEN_ROD,            0xB8860B },
    { tau::COLOR_DARK_GRAY,                  0xA9A9A9 },
    { tau::COLOR_DARK_GREEN,                 0x006400 },
    { tau::COLOR_DARK_KHAKI,                 0xBDB76B },
    { tau::COLOR_DARK_MAGENTA,               0x8B008B },
    { tau::COLOR_DARK_OLIVE_GREEN,           0x556B2F },
    { tau::COLOR_DARK_ORANGE,                0xFF8C00 },
    { tau::COLOR_DARK_ORCHID,                0x9932CC },
    { tau::COLOR_DARK_RED,                   0x8B0000 },
    { tau::COLOR_DARK_SALMON,                0xE9967A },
    { tau::COLOR_DARK_SEA_GREEN,             0x8FBC8F },
    { tau::COLOR_DARK_SLATE_BLUE,            0x483D8B },
    { tau::COLOR_DARK_SLATE_GRAY,            0x2F4F4F },
    { tau::COLOR_DARK_TURQUOISE,             0x00CED1 },
    { tau::COLOR_DARK_VIOLET,                0x9400D3 },
    { tau::COLOR_DEEP_PINK,                  0xFF1493 },
    { tau::COLOR_DEEP_SKY_BLUE,              0x00BFFF },
    { tau::COLOR_DIM_GRAY,                   0x696969 },
    { tau::COLOR_DODGER_BLUE,                0x1E90FF },
    { tau::COLOR_FIRE_BRICK,                 0xB22222 },
    { tau::COLOR_FLORAL_WHITE,               0xFFFAF0 },
    { tau::COLOR_FOREST_GREEN,               0x228B22 },
    { tau::COLOR_FUCHSIA,                    0xFF00FF },
    { tau::COLOR_GAINSBORO,                  0xDCDCDC },
    { tau::COLOR_GHOST_WHITE,                0xF8F8FF },
    { tau::COLOR_GOLD,                       0xFFD700 },
    { tau::COLOR_GOLDEN_ROD,                 0xDAA520 },
    { tau::COLOR_GRAY,                       0x808080 },
    { tau::COLOR_GREEN,                      0x008000 },
    { tau::COLOR_GREEN_YELLOW,               0xADFF2F },
    { tau::COLOR_HONEY_DEW,                  0xF0FFF0 },
    { tau::COLOR_HOT_PINK,                   0xFF69B4 },
    { tau::COLOR_INDIAN_RED,                 0xCD5C5C },
    { tau::COLOR_INDIGO,                     0x4B0082 },
    { tau::COLOR_IVORY,                      0xFFFFF0 },
    { tau::COLOR_KHAKI,                      0xF0E68C },
    { tau::COLOR_LAVENDER,                   0xE6E6FA },
    { tau::COLOR_LAVENDER_BLUSH,             0xFFF0F5 },
    { tau::COLOR_LAWN_GREEN,                 0x7CFC00 },
    { tau::COLOR_LEMON_CHIFFON,              0xFFFACD },
    { tau::COLOR_LIGHT_BLUE,                 0xADD8E6 },
    { tau::COLOR_LIGHT_CORAL,                0xF08080 },
    { tau::COLOR_LIGHT_CYAN,                 0xE0FFFF },
    { tau::COLOR_LIGHT_GOLDEN_ROD_YELLOW,    0xFAFAD2 },
    { tau::COLOR_LIGHT_GRAY,                 0xD3D3D3 },
    { tau::COLOR_LIGHT_GREEN,                0x90EE90 },
    { tau::COLOR_LIGHT_PINK,                 0xFFB6C1 },
    { tau::COLOR_LIGHT_SALMON,               0xFFA07A },
    { tau::COLOR_LIGHT_SEA_GREEN,            0x20B2AA },
    { tau::COLOR_LIGHT_SKY_BLUE,             0x87CEFA },
    { tau::COLOR_LIGHT_SLATE_GRAY,           0x778899 },
    { tau::COLOR_LIGHT_STEEL_BLUE,           0xB0C4DE },
    { tau::COLOR_LIGHT_YELLOW,               0xFFFFE0 },
    { tau::COLOR_LIME,                       0x00FF00 },
    { tau::COLOR_LIME_GREEN,                 0x32CD32 },
    { tau::COLOR_LINEN,                      0xFAF0E6 },
    { tau::COLOR_MAGENTA,                    0xFF00FF },
    { tau::COLOR_MAROON,                     0x800000 },
    { tau::COLOR_MEDIUM_AQUA_MARINE,         0x66CDAA },
    { tau::COLOR_MEDIUM_BLUE,                0x0000CD },
    { tau::COLOR_MEDIUM_ORCHID,              0xBA55D3 },
    { tau::COLOR_MEDIUM_PURPLE,              0x9370DB },
    { tau::COLOR_MEDIUM_SEA_GREEN,           0x3CB371 },
    { tau::COLOR_MEDIUM_SLATE_BLUE,          0x7B68EE },
    { tau::COLOR_MEDIUM_SPRING_GREEN,        0x00FA9A },
    { tau::COLOR_MEDIUM_TURQUOISE,           0x48D1CC },
    { tau::COLOR_MEDIUM_VIOLET_RED,          0xC71585 },
    { tau::COLOR_MIDNIGHT_BLUE,              0x191970 },
    { tau::COLOR_MINT_CREAM,                 0xF5FFFA },
    { tau::COLOR_MISTY_ROSE,                 0xFFE4E1 },
    { tau::COLOR_MOCCASIN,                   0xFFE4B5 },
    { tau::COLOR_NAVAJO_WHITE,               0xFFDEAD },
    { tau::COLOR_NAVY,                       0x000080 },
    { tau::COLOR_OLD_LACE,                   0xFDF5E6 },
    { tau::COLOR_OLIVE,                      0x808000 },
    { tau::COLOR_OLIVE_DRAB,                 0x6B8E23 },
    { tau::COLOR_ORANGE,                     0xFFA500 },
    { tau::COLOR_ORANGE_RED,                 0xFF4500 },
    { tau::COLOR_ORCHID,                     0xDA70D6 },
    { tau::COLOR_PALE_GOLDEN_ROD,            0xEEE8AA },
    { tau::COLOR_PALE_GREEN,                 0x98FB98 },
    { tau::COLOR_PALE_TURQUOISE,             0xAFEEEE },
    { tau::COLOR_PALE_VIOLET_RED,            0xDB7093 },
    { tau::COLOR_PAPAYA_WHIP,                0xFFEFD5 },
    { tau::COLOR_PEACH_PUFF,                 0xFFDAB9 },
    { tau::COLOR_PERU,                       0xCD853F },
    { tau::COLOR_PINK,                       0xFFC0CB },
    { tau::COLOR_PLUM,                       0xDDA0DD },
    { tau::COLOR_POWDER_BLUE,                0xB0E0E6 },
    { tau::COLOR_PURPLE,                     0x800080 },
    { tau::COLOR_RED,                        0xFF0000 },
    { tau::COLOR_ROSY_BROWN,                 0xBC8F8F },
    { tau::COLOR_ROYAL_BLUE,                 0x4169E1 },
    { tau::COLOR_SADDLE_BROWN,               0x8B4513 },
    { tau::COLOR_SALMON,                     0xFA8072 },
    { tau::COLOR_SANDY_BROWN,                0xF4A460 },
    { tau::COLOR_SEA_GREEN,                  0x2E8B57 },
    { tau::COLOR_SEA_SHELL,                  0xFFF5EE },
    { tau::COLOR_SIENNA,                     0xA0522D },
    { tau::COLOR_SILVER,                     0xC0C0C0 },
    { tau::COLOR_SKY_BLUE,                   0x87CEEB },
    { tau::COLOR_SLATE_BLUE,                 0x6A5ACD },
    { tau::COLOR_SLATE_GRAY,                 0x708090 },
    { tau::COLOR_SNOW,                       0xFFFAFA },
    { tau::COLOR_SPRING_GREEN,               0x00FF7F },
    { tau::COLOR_STEEL_BLUE,                 0x4682B4 },
    { tau::COLOR_TAN,                        0xD2B48C },
    { tau::COLOR_TEAL,                       0x008080 },
    { tau::COLOR_THISTLE,                    0xD8BFD8 },
    { tau::COLOR_TOMATO,                     0xFF6347 },
    { tau::COLOR_TURQUOISE,                  0x40E0D0 },
    { tau::COLOR_VIOLET,                     0xEE82EE },
    { tau::COLOR_WHEAT,                      0xF5DEB3 },
    { tau::COLOR_WHITE,                      0xFFFFFF },
    { tau::COLOR_WHITE_SMOKE,                0xF5F5F5 },
    { tau::COLOR_YELLOW,                     0xFFFF00 },
    { tau::COLOR_YELLOW_GREEN,               0x9ACD32 },
    { nullptr,                               0x000000 }
};

std::unordered_map<std::string, uint32_t> named;

int int_from_hex(char c) {
    return (c >= '0' && c <= '9') ? (c-'0') : ((toupper(c) >= 'A' && toupper(c) <= 'F') ? (c-'A'+'\x0a') : 0);
}

// c must be hexadecimal digit.
double color_from_char(char c) {
    return static_cast<double>(int_from_hex(c))/16.0;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

const char * COLOR_ALICE_BLUE                   = "AliceBlue";
const char * COLOR_ANTIQUE_WHITE                = "AntiqueWhite";
const char * COLOR_AQUA                         = "Aqua";
const char * COLOR_AQUAMARINE                   = "Aquamarine";
const char * COLOR_AZURE                        = "Azure";
const char * COLOR_BEIGE                        = "Beige";
const char * COLOR_BISQUE                       = "Bisque";
const char * COLOR_BLACK                        = "Black";
const char * COLOR_BLANCHED_ALMOND              = "BlanchedAlmond";
const char * COLOR_BLUE                         = "Blue";
const char * COLOR_BLUE_VIOLET                  = "BlueViolet";
const char * COLOR_BROWN                        = "Brown";
const char * COLOR_BURLY_WOOD                   = "BurlyWood";
const char * COLOR_CADET_BLUE                   = "CadetBlue";
const char * COLOR_CHARTREUSE                   = "Chartreuse";
const char * COLOR_CHOCOLATE                    = "Chocolate";
const char * COLOR_CORAL                        = "Coral";
const char * COLOR_CORNFLOWER_BLUE              = "CornflowerBlue";
const char * COLOR_CORNSILK                     = "Cornsilk";
const char * COLOR_CRIMSON                      = "Crimson";
const char * COLOR_CYAN                         = "Cyan";
const char * COLOR_DARK_BLUE                    = "DarkBlue";
const char * COLOR_DARK_CYAN                    = "DarkCyan";
const char * COLOR_DARK_GOLDEN_ROD              = "DarkGoldenRod";
const char * COLOR_DARK_GRAY                    = "DarkGray";
const char * COLOR_DARK_GREEN                   = "DarkGreen";
const char * COLOR_DARK_KHAKI                   = "DarkKhaki";
const char * COLOR_DARK_MAGENTA                 = "DarkMagenta";
const char * COLOR_DARK_OLIVE_GREEN             = "DarkOliveGreen";
const char * COLOR_DARK_ORANGE                  = "DarkOrange";
const char * COLOR_DARK_ORCHID                  = "DarkOrchid";
const char * COLOR_DARK_RED                     = "DarkRed";
const char * COLOR_DARK_SALMON                  = "DarkSalmon";
const char * COLOR_DARK_SEA_GREEN               = "DarkSeaGreen";
const char * COLOR_DARK_SLATE_BLUE              = "DarkSlateBlue";
const char * COLOR_DARK_SLATE_GRAY              = "DarkSlateGray";
const char * COLOR_DARK_TURQUOISE               = "DarkTurquoise";
const char * COLOR_DARK_VIOLET                  = "DarkViolet";
const char * COLOR_DEEP_PINK                    = "DeepPink";
const char * COLOR_DEEP_SKY_BLUE                = "DeepSkyBlue";
const char * COLOR_DIM_GRAY                     = "DimGray";
const char * COLOR_DODGER_BLUE                  = "DodgerBlue";
const char * COLOR_FIRE_BRICK                   = "FireBrick";
const char * COLOR_FLORAL_WHITE                 = "FloralWhite";
const char * COLOR_FOREST_GREEN                 = "ForestGreen";
const char * COLOR_FUCHSIA                      = "Fuchsia";
const char * COLOR_GAINSBORO                    = "Gainsboro";
const char * COLOR_GHOST_WHITE                  = "GhostWhite";
const char * COLOR_GOLD                         = "Gold";
const char * COLOR_GOLDEN_ROD                   = "GoldenRod";
const char * COLOR_GRAY                         = "Gray";
const char * COLOR_GREEN                        = "Green";
const char * COLOR_GREEN_YELLOW                 = "GreenYellow";
const char * COLOR_HONEY_DEW                    = "HoneyDew";
const char * COLOR_HOT_PINK                     = "HotPink";
const char * COLOR_INDIAN_RED                   = "IndianRed";
const char * COLOR_INDIGO                       = "Indigo";
const char * COLOR_IVORY                        = "Ivory";
const char * COLOR_KHAKI                        = "Khaki";
const char * COLOR_LAVENDER                     = "Lavender";
const char * COLOR_LAVENDER_BLUSH               = "LavenderBlush";
const char * COLOR_LAWN_GREEN                   = "LawnGreen";
const char * COLOR_LEMON_CHIFFON                = "LemonChiffon";
const char * COLOR_LIGHT_BLUE                   = "LightBlue";
const char * COLOR_LIGHT_CORAL                  = "LightCoral";
const char * COLOR_LIGHT_CYAN                   = "LightCyan";
const char * COLOR_LIGHT_GOLDEN_ROD_YELLOW      = "LightGoldenRodYellow";
const char * COLOR_LIGHT_GRAY                   = "LightGray";
const char * COLOR_LIGHT_GREEN                  = "LightGreen";
const char * COLOR_LIGHT_PINK                   = "LightPink";
const char * COLOR_LIGHT_SALMON                 = "LightSalmon";
const char * COLOR_LIGHT_SEA_GREEN              = "LightSeaGreen";
const char * COLOR_LIGHT_SKY_BLUE               = "LightSkyBlue";
const char * COLOR_LIGHT_SLATE_GRAY             = "LightSlateGray";
const char * COLOR_LIGHT_STEEL_BLUE             = "LightSteelBlue";
const char * COLOR_LIGHT_YELLOW                 = "LightYellow";
const char * COLOR_LIME                         = "Lime";
const char * COLOR_LIME_GREEN                   = "LimeGreen";
const char * COLOR_LINEN                        = "Linen";
const char * COLOR_MAGENTA                      = "Magenta";
const char * COLOR_MAROON                       = "Maroon";
const char * COLOR_MEDIUM_AQUA_MARINE           = "MediumAquaMarine";
const char * COLOR_MEDIUM_BLUE                  = "MediumBlue";
const char * COLOR_MEDIUM_ORCHID                = "MediumOrchid";
const char * COLOR_MEDIUM_PURPLE                = "MediumPurple";
const char * COLOR_MEDIUM_SEA_GREEN             = "MediumSeaGreen";
const char * COLOR_MEDIUM_SLATE_BLUE            = "MediumSlateBlue";
const char * COLOR_MEDIUM_SPRING_GREEN          = "MediumSpringGreen";
const char * COLOR_MEDIUM_TURQUOISE             = "MediumTurquoise";
const char * COLOR_MEDIUM_VIOLET_RED            = "MediumVioletRed";
const char * COLOR_MIDNIGHT_BLUE                = "MidnightBlue";
const char * COLOR_MINT_CREAM                   = "MintCream";
const char * COLOR_MISTY_ROSE                   = "MistyRose";
const char * COLOR_MOCCASIN                     = "Moccasin";
const char * COLOR_NAVAJO_WHITE                 = "NavajoWhite";
const char * COLOR_NAVY                         = "Navy";
const char * COLOR_OLD_LACE                     = "OldLace";
const char * COLOR_OLIVE                        = "Olive";
const char * COLOR_OLIVE_DRAB                   = "OliveDrab";
const char * COLOR_ORANGE                       = "Orange";
const char * COLOR_ORANGE_RED                   = "OrangeRed";
const char * COLOR_ORCHID                       = "Orchid";
const char * COLOR_PALE_GOLDEN_ROD              = "PaleGoldenRod";
const char * COLOR_PALE_GREEN                   = "PaleGreen";
const char * COLOR_PALE_TURQUOISE               = "PaleTurquoise";
const char * COLOR_PALE_VIOLET_RED              = "PaleVioletRed";
const char * COLOR_PAPAYA_WHIP                  = "PapayaWhip";
const char * COLOR_PEACH_PUFF                   = "PeachPuff";
const char * COLOR_PERU                         = "Peru";
const char * COLOR_PINK                         = "Pink";
const char * COLOR_PLUM                         = "Plum";
const char * COLOR_POWDER_BLUE                  = "PowderBlue";
const char * COLOR_PURPLE                       = "Purple";
const char * COLOR_RED                          = "Red";
const char * COLOR_ROSY_BROWN                   = "RosyBrown";
const char * COLOR_ROYAL_BLUE                   = "RoyalBlue";
const char * COLOR_SADDLE_BROWN                 = "SaddleBrown";
const char * COLOR_SALMON                       = "Salmon";
const char * COLOR_SANDY_BROWN                  = "SandyBrown";
const char * COLOR_SEA_GREEN                    = "SeaGreen";
const char * COLOR_SEA_SHELL                    = "SeaShell";
const char * COLOR_SIENNA                       = "Sienna";
const char * COLOR_SILVER                       = "Silver";
const char * COLOR_SKY_BLUE                     = "SkyBlue";
const char * COLOR_SLATE_BLUE                   = "SlateBlue";
const char * COLOR_SLATE_GRAY                   = "SlateGray";
const char * COLOR_SNOW                         = "Snow";
const char * COLOR_SPRING_GREEN                 = "SpringGreen";
const char * COLOR_STEEL_BLUE                   = "SteelBlue";
const char * COLOR_TAN                          = "Tan";
const char * COLOR_TEAL                         = "Teal";
const char * COLOR_THISTLE                      = "Thistle";
const char * COLOR_TOMATO                       = "Tomato";
const char * COLOR_TURQUOISE                    = "Turquoise";
const char * COLOR_VIOLET                       = "Violet";
const char * COLOR_WHEAT                        = "Wheat";
const char * COLOR_WHITE                        = "White";
const char * COLOR_WHITE_SMOKE                  = "WhiteSmoke";
const char * COLOR_YELLOW                       = "Yellow";
const char * COLOR_YELLOW_GREEN                 = "YellowGreen";

Color::Color(double red, double green, double blue, double alpha) {
    set(red, green, blue, alpha);
}

Color::Color(const ustring & text, double alpha) {
    set(text, alpha);
}

double Color::red() const {
    return red_;
}

double Color::green() const {
    return green_;
}

double Color::blue() const {
    return blue_;
}

double Color::alpha() const {
    return alpha_;
}

double Color::hue() const {
    return hue_;
}

double Color::saturation() const {
    return sat_;
}

double Color::value() const {
    return value_;
}

Color & Color::operator=(const ustring & text) {
    set(text);
    return *this;
}

bool Color::operator==(const Color & other) const {
    return red_ == other.red_ && green_ == other.green_ && blue_ == other.blue_ && alpha_ == other.alpha_;
}

bool Color::operator!=(const Color & other) const {
    return red_ != other.red_ || green_ != other.green_ || blue_ != other.blue_ || alpha_ != other.alpha_;
}

void Color::set(double red, double green, double blue, double alpha) {
    red_ = std::max(0.0, std::min(1.0, red));
    green_ = std::max(0.0, std::min(1.0, green));
    blue_ = std::max(0.0, std::min(1.0, blue));
    alpha_ = std::max(0.0, std::min(1.0, alpha));
    calc_hsv();
}

// Color textual representations supported are:
// 1. #RGB
// 2. #RRGGBB
void Color::set(const ustring & str, double alpha) {
    if ('#' == *str.begin()) {
        switch (str.size()) {
            case 4:
                set_red(color_from_char(str[1]));
                set_green(color_from_char(str[2]));
                set_blue(color_from_char(str[3]));
                set_alpha(alpha);
                return;

            case 7:
                set_red((1.0/255.0)*static_cast<double>((int_from_hex(str[1]) << 4) + int_from_hex(str[2])));
                set_green((1.0/255.0)*static_cast<double>((int_from_hex(str[3]) << 4) + int_from_hex(str[4])));
                set_blue((1.0/255.0)*static_cast<double>((int_from_hex(str[5]) << 4) + int_from_hex(str[6])));
                set_alpha(alpha);
                return;
        }
    }

    if (named.empty()) {
        for (int i = 0; nullptr != named_colors[i].name; ++i) {
            named[str_toupper(named_colors[i].name)] = named_colors[i].rgb24;
        }
    }

    auto i = named.find(str_toupper(str));
    if (i != named.end()) { set_rgb24(i->second, alpha); return; }
    calc_hsv();
}

void Color::set_rgb24(uint32_t rgb24, double alpha) {
    uint8_t r = rgb24 >> 16, g = rgb24 >> 8, b = rgb24;
    set(0.00392*r, 0.00392*g, 0.00392*b, alpha);
}

void Color::set_hsv(double hue, double sat, double value) {
    hue_ = hue;
    sat_ = std::max(0.0, std::min(1.0, sat));
    value_ = std::max(0.0, std::min(1.0, value));
    calc_rgb();
}

void Color::set_hsv(double hue, double saturation, double value, double alpha) {
    set_hsv(hue, saturation, value);
    set_alpha(alpha);
}

void Color::set_red(double red) {
    red_ = std::max(0.0, std::min(1.0, red));
    calc_hsv();
}

void Color::set_green(double green) {
    green_ = std::max(0.0, std::min(1.0, green));
    calc_hsv();
}

void Color::set_blue(double blue) {
    blue_ = std::max(0.0, std::min(1.0, blue));
    calc_hsv();
}

void Color::set_alpha(double alpha) {
    alpha_ = std::max(0.0, std::min(1.0, alpha));
}

void Color::set_hue(double hue) {
    hue_ = hue;
    calc_rgb();
}

void Color::set_saturation(double sat) {
    sat_ = std::max(0.0, std::min(1.0, sat));
    calc_rgb();
}

void Color::set_value(double value) {
    value_ = std::max(0.0, std::min(1.0, value));
    calc_rgb();
}

void Color::calc_rgb() {
    // achromatic case
    if (sat_ == 0.0 || hue_ < 0.0) {
        red_ = green_ = blue_ = value_;
    }

    // chromatic case
    else {
        double h = hue_ >= 360.0 ? 0 : hue_/60.0;
        int i = int(h);
        double f = h-i;
        double p = value_*(1.0-sat_);

        if (i & 1) {
            double q = value_*(1.0-(sat_*f));

            switch (i) {
                case 1:
                    red_   = q;
                    green_ = value_;
                    blue_  = p;
                    break;

                case 3:
                    red_   = p;
                    green_ = q;
                    blue_  = value_;
                    break;

                case 5:
                    red_   = value_;
                    green_ = p;
                    blue_  = q;
                    break;
            }
        }

        else {
            double t = value_*(1.0-(sat_*(1.0-f)));

            switch (i) {
                case 0:
                    red_   = value_;
                    green_ = t;
                    blue_  = p;
                    break;

                case 2:
                    red_   = p;
                    green_ = value_;
                    blue_  = t;
                    break;

                case 4:
                    red_   = t;
                    green_ = p;
                    blue_  = value_;
                    break;
            }
        }
    }
}

void Color::calc_hsv() {
    double cmax = std::max(red_, std::max(green_, blue_));
    double cmin = std::min(red_, std::min(green_, blue_));
    double d = cmax-cmin;

    if (0.0 != d) {
        double hue = 0.0;

        if (green_ == cmax) {
            hue = 2.0+((blue_-red_)/d);
        }

        else if (blue_ == cmax) {
            hue = 4.0+((red_-green_)/d);
        }

        else {
            hue = green_ < blue_ ? 6.0+((green_-blue_)/d) : (green_-blue_)/d;
        }

        hue_ = 60.0*hue;
    }

    // Achromatic case, set hue < 0.
    else {
        hue_ = -1.0;
    }

    sat_ = 0.0 == cmax ? 0.0 : 1.0-(cmin/cmax);
    value_ = cmax;
}

void Color::lighter(double factor) {
    double v = std::max(1.0/255, value());
    v *= 1.0+std::max(0.0, factor);
    set_hsv(hue_, sat_, v);
}

void Color::darker(double factor) {
    double v = value();
    v *= 1.0-std::max(0.0, std::min(1.0, factor));
    set_hsv(hue_, sat_, v);
}

Color Color::lighten(double factor) const {
    Color c(*this);
    c.lighter(factor);
    return c;
}

Color Color::darken(double factor) const {
    Color c(*this);
    c.darker(factor);
    return c;
}

std::vector<ustring> Color::list_css_names() {
    std::vector<ustring> l;
    for (int i = 0; named_colors[i].name; ++i) { l.emplace_back(named_colors[i].name); }
    return l;
}

uint32_t Color::argb32() const {
    uint32_t a, r, g, b;
    a = static_cast<uint32_t>(255.0*alpha()) << 24;
    r = static_cast<uint32_t>(255.0*red()) << 16;
    g = static_cast<uint32_t>(255.0*green()) << 8;
    b = static_cast<uint32_t>(255.0*blue());
    return a+r+g+b;
}

uint32_t Color::abgr32() const {
    uint32_t a, r, g, b;
    a = static_cast<uint32_t>(255.0*alpha()) << 24;
    b = static_cast<uint32_t>(255.0*blue()) << 16;
    g = static_cast<uint32_t>(255.0*green()) << 8;
    r = static_cast<uint32_t>(255.0*red());
    return a+r+g+b;
}

uint32_t Color::rgb24() const {
    uint32_t r, g, b;
    r = static_cast<uint32_t>(255.0*red()) << 16;
    g = static_cast<uint32_t>(255.0*green()) << 8;
    b = static_cast<uint32_t>(255.0*blue());
    return r+g+b;
}

uint32_t Color::bgr24() const {
    uint32_t r, g, b;
    b = static_cast<uint32_t>(255.0*blue()) << 16;
    g = static_cast<uint32_t>(255.0*green()) << 8;
    r = static_cast<uint32_t>(255.0*red());
    return r+g+b;
}

uint8_t Color::gray8() const {
    return 255.0*gray();
}

uint32_t Color::gray24() const {
    uint32_t rb = gray8();
    uint32_t gg = rb << 8;
    uint32_t br = rb << 16;
    return rb+gg+br;
}

double Color::gray() const {
    return 0.299*red_+0.587*green_+0.114*blue_;         // form wikipedia.org
}

Color Color::from_argb32(uint32_t argb32) {
    uint8_t a = argb32 >> 24, r = argb32 >> 16, g = argb32 >> 8, b = argb32;
    return Color(0.00392*r, 0.00392*g, 0.00392*b, 0.00392*a);
}

Color Color::from_rgb24(uint32_t rgb24, double alpha) {
    uint8_t r = rgb24 >> 16, g = rgb24 >> 8, b = rgb24;
    return Color(0.00392*r, 0.00392*g, 0.00392*b, alpha);
}

Color Color::from_gray(double gray, double alpha) {
    return Color(gray, gray, gray, alpha);
}

Color Color::from_gray8(uint8_t gray, double alpha) {
    double g = 0.00392*gray;
    return Color(g, g, g, alpha);
}

ustring Color::html() const {
    ustring s(1, '#');

    s += str_format(std::setfill('0'), std::setw(2), std::hex, unsigned(255*red_));
    s += str_format(std::setfill('0'), std::setw(2), std::hex, unsigned(255*green_));
    s += str_format(std::setfill('0'), std::setw(2), std::hex, unsigned(255*blue_));

    return str_toupper(s);
}

Color Color::inverted() const {
    return Color(1.0-red_, 1.0-green_, 1.0-blue_, alpha_);
}

Color Color::inactive() const {
    double h = hue(), s = saturation(), v = value();

    if (h >= 0.0) {
        h = fabs(h-240);

        if (h < 40) {
            if (v > 0.7) { v *= 0.7; s *= 0.45; }
            else { s *= 0.7; v *= 1.2; }
        }

        else {
            v = v > 0.6 ?  v*0.8 : v+0.15;
        }
    }

    else { // gray
        v = v > 0.1 ? (v > 0.6 ?  v*0.8 : 1.2*v) : v+0.2;
    }

    Color c(*this);
    c.set_saturation(s);
    c.set_value(v);
    return c;
}

} // namespace tau

//END
