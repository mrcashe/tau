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

#include <tau/color.hh>
#include <tau/string.hh>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <unordered_map>

namespace {

struct { const char * name; uint32_t rgb24; } named_colors[] = {
    { "AliceBlue",              0xF0F8FF },
    { "AntiqueWhite",           0xFAEBD7 },
    { "Aqua",                   0x00FFFF },
    { "Aquamarine",             0x7FFFD4 },
    { "Azure",                  0xF0FFFF },
    { "Beige",                  0xF5F5DC },
    { "Bisque",                 0xFFE4C4 },
    { "Black",                  0x000000 },
    { "BlanchedAlmond",         0xFFEBCD },
    { "Blue",                   0x0000FF },
    { "BlueViolet",             0x8A2BE2 },
    { "Brown",                  0xA52A2A },
    { "BurlyWood",              0xDEB887 },
    { "CadetBlue",              0x5F9EA0 },
    { "Chartreuse",             0x7FFF00 },
    { "Chocolate",              0xD2691E },
    { "Coral",                  0xFF7F50 },
    { "CornflowerBlue",         0x6495ED },
    { "Cornsilk",               0xFFF8DC },
    { "Crimson",                0xDC143C },
    { "Cyan",                   0x00FFFF },
    { "DarkBlue",               0x00008B },
    { "DarkCyan",               0x008B8B },
    { "DarkGoldenRod",          0xB8860B },
    { "DarkGray",               0xA9A9A9 },
    { "DarkGreen",              0x006400 },
    { "DarkKhaki",              0xBDB76B },
    { "DarkMagenta",            0x8B008B },
    { "DarkOliveGreen",         0x556B2F },
    { "DarkOrange",             0xFF8C00 },
    { "DarkOrchid",             0x9932CC },
    { "DarkRed",                0x8B0000 },
    { "DarkSalmon",             0xE9967A },
    { "DarkSeaGreen",           0x8FBC8F },
    { "DarkSlateBlue",          0x483D8B },
    { "DarkSlateGray",          0x2F4F4F },
    { "DarkTurquoise",          0x00CED1 },
    { "DarkViolet",             0x9400D3 },
    { "DeepPink",               0xFF1493 },
    { "DeepSkyBlue",            0x00BFFF },
    { "DimGray",                0x696969 },
    { "DodgerBlue",             0x1E90FF },
    { "FireBrick",              0xB22222 },
    { "FloralWhite",            0xFFFAF0 },
    { "ForestGreen",            0x228B22 },
    { "Fuchsia",                0xFF00FF },
    { "Gainsboro",              0xDCDCDC },
    { "GhostWhite",             0xF8F8FF },
    { "Gold",                   0xFFD700 },
    { "GoldenRod",              0xDAA520 },
    { "Gray",                   0x808080 },
    { "Green",                  0x008000 },
    { "GreenYellow",            0xADFF2F },
    { "HoneyDew",               0xF0FFF0 },
    { "HotPink",                0xFF69B4 },
    { "IndianRed",              0xCD5C5C },
    { "Indigo",                 0x4B0082 },
    { "Ivory",                  0xFFFFF0 },
    { "Khaki",                  0xF0E68C },
    { "Lavender",               0xE6E6FA },
    { "LavenderBlush",          0xFFF0F5 },
    { "LawnGreen",              0x7CFC00 },
    { "LemonChiffon",           0xFFFACD },
    { "LightBlue",              0xADD8E6 },
    { "LightCoral",             0xF08080 },
    { "LightCyan",              0xE0FFFF },
    { "LightGoldenRodYellow",   0xFAFAD2 },
    { "LightGray",              0xD3D3D3 },
    { "LightGreen",             0x90EE90 },
    { "LightPink",              0xFFB6C1 },
    { "LightSalmon",            0xFFA07A },
    { "LightSeaGreen",          0x20B2AA },
    { "LightSkyBlue",           0x87CEFA },
    { "LightSlateGray",         0x778899 },
    { "LightSteelBlue",         0xB0C4DE },
    { "LightYellow",            0xFFFFE0 },
    { "Lime",                   0x00FF00 },
    { "LimeGreen",              0x32CD32 },
    { "Linen",                  0xFAF0E6 },
    { "Magenta",                0xFF00FF },
    { "Maroon",                 0x800000 },
    { "MediumAquaMarine",       0x66CDAA },
    { "MediumBlue",             0x0000CD },
    { "MediumOrchid",           0xBA55D3 },
    { "MediumPurple",           0x9370DB },
    { "MediumSeaGreen",         0x3CB371 },
    { "MediumSlateBlue",        0x7B68EE },
    { "MediumSpringGreen",      0x00FA9A },
    { "MediumTurquoise",        0x48D1CC },
    { "MediumVioletRed",        0xC71585 },
    { "MidnightBlue",           0x191970 },
    { "MintCream",              0xF5FFFA },
    { "MistyRose",              0xFFE4E1 },
    { "Moccasin",               0xFFE4B5 },
    { "NavajoWhite",            0xFFDEAD },
    { "Navy",                   0x000080 },
    { "OldLace",                0xFDF5E6 },
    { "Olive",                  0x808000 },
    { "OliveDrab",              0x6B8E23 },
    { "Orange",                 0xFFA500 },
    { "OrangeRed",              0xFF4500 },
    { "Orchid",                 0xDA70D6 },
    { "PaleGoldenRod",          0xEEE8AA },
    { "PaleGreen",              0x98FB98 },
    { "PaleTurquoise",          0xAFEEEE },
    { "PaleVioletRed",          0xDB7093 },
    { "PapayaWhip",             0xFFEFD5 },
    { "PeachPuff",              0xFFDAB9 },
    { "Peru",                   0xCD853F },
    { "Pink",                   0xFFC0CB },
    { "Plum",                   0xDDA0DD },
    { "PowderBlue",             0xB0E0E6 },
    { "Purple",                 0x800080 },
    { "Red",                    0xFF0000 },
    { "RosyBrown",              0xBC8F8F },
    { "RoyalBlue",              0x4169E1 },
    { "SaddleBrown",            0x8B4513 },
    { "Salmon",                 0xFA8072 },
    { "SandyBrown",             0xF4A460 },
    { "SeaGreen",               0x2E8B57 },
    { "SeaShell",               0xFFF5EE },
    { "Sienna",                 0xA0522D },
    { "Silver",                 0xC0C0C0 },
    { "SkyBlue",                0x87CEEB },
    { "SlateBlue",              0x6A5ACD },
    { "SlateGray",              0x708090 },
    { "Snow",                   0xFFFAFA },
    { "SpringGreen",            0x00FF7F },
    { "SteelBlue",              0x4682B4 },
    { "Tan",                    0xD2B48C },
    { "Teal",                   0x008080 },
    { "Thistle",                0xD8BFD8 },
    { "Tomato",                 0xFF6347 },
    { "Turquoise",              0x40E0D0 },
    { "Violet",                 0xEE82EE },
    { "Wheat",                  0xF5DEB3 },
    { "White",                  0xFFFFFF },
    { "WhiteSmoke",             0xF5F5F5 },
    { "Yellow",                 0xFFFF00 },
    { "YellowGreen",            0x9ACD32 },
    { nullptr,                  0x000000 }
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

Color::Color(double red, double green, double blue, double alpha) {
    set(red, green, blue, alpha);
}

Color::Color(const ustring & text, double alpha) {
    set(text, alpha);
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
