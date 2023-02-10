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
#include <tau/string.hh>
#include <cstring>
#include <iostream>
#include <sstream>

namespace {

const std::vector<tau::ustring> faces_  = {
    "Regular", "Normal", "Book", "Italic", "Oblique", "Roman", "Medium", "Thin",
    "Bold", "ExtraLight", "Light", "Heavy", "Plain", "Initials", "Demi", "Condensed",
    "SemiBold", "ExtraBold", "BoldItalic", "BoldOblique", "LightOblique", "Mono"
};

const std::string digits_ = ".,-+=0123456789eE";

bool is_size(const tau::ustring & s) {
    return tau::ustring::npos == s.find_first_not_of(digits_);
}

bool is_face(const tau::ustring & s) {
    std::size_t n = 0;
    for (n = 0; U'=' == s[n]; ++n) {}
    return tau::str_similar(s.substr(n), faces_);
}

} // anonymous namespace

namespace tau {

std::vector<ustring> font_spec_explode(const ustring & spec) {
    return str_explode(str_trim(spec), ' ');
}

double font_size_from_spec(const std::vector<ustring> & specv, double fallback) {
    static const char * signs = "+-=";
    double pts = 0.0;

    for (std::string s: specv) {
        if (is_size(s)) {
            std::size_t pos, rpos = 0, len = s.size();
            for (pos = s.find(','); pos < len; pos = s.find(',')) { s[pos] = '.'; }
            pos = std::strchr(signs, s[0]) ? 1 : 0;

            while (pos < len) {
                char sgn = s[rpos];
                std::size_t sign = s.find_first_of(signs, pos), end = std::min(sign, len);
                if ('=' == sgn) { ++rpos; }
                double d; std::istringstream is(s.substr(rpos, end-rpos)); is >> d;
                if ('=' == sgn) { return d > 0.0 ? d : fallback; }
                pts += d;
                pos = end+1;
                rpos = sign;
            }
        }
    }

    return pts > 0.0 ? pts : fallback;
}

double font_size_from_spec(const ustring & spec, double fallback) {
    return font_size_from_spec(font_spec_explode(spec), fallback);
}

ustring font_face_from_spec(const std::vector<ustring> & specv) {
    std::vector<ustring> rv;
    bool mono = false;
    std::size_t pos;

    for (const ustring & s: specv) {
        if (is_face(s)) {
            for (pos = 0; pos < s.size() && U'=' == s[pos]; ++pos) {}
            if (str_similar("Mono", s.substr(pos))) { mono = true; }
        }
    }

    if (mono) { rv.push_back("Mono"); }

    for (const ustring & s: specv) {
        if (is_face(s)) {
            for (pos = 0; pos < s.size() && U'=' == s[pos]; ++pos) {}
            ustring t(s.substr(pos));
            if (!str_similar(t, "Mono Regular", U' ') && !str_similar(t, rv)) { rv.push_back(t); }
        }
    }

    return rv.empty() ? faces_[0] : str_implode(rv, ' ');
}

ustring font_face_from_spec(const ustring & spec) {
    return font_face_from_spec(font_spec_explode(spec));
}

ustring font_family_from_spec(const std::vector<ustring> & specv) {
    std::vector<ustring> rv;
    std::copy_if(specv.begin(), specv.end(), std::back_inserter(rv), [](const ustring & s) { return !is_size(s) && !is_face(s); } );
    return str_implode(rv, ' ');
}

ustring font_family_from_spec(const ustring & spec) {
    return font_family_from_spec(font_spec_explode(spec));
}

ustring font_spec_build(const std::vector<ustring> & specv) {
    double pts = font_size_from_spec(specv);
    ustring face = font_face_from_spec(specv);
    if (face == faces_[0]) { face.clear(); }
    else { face.insert(0, 1, ' '); }
    return font_family_from_spec(specv)+face+(pts >= 0.0 ? str_format(' ', pts) : "");
}

ustring font_spec_build(const ustring & family, const ustring & face, double pt) {
    std::vector<ustring> v;
    v.push_back(family);
    for (auto & s: str_explode(font_face_from_spec(face))) { v.push_back(s); }
    if (pt >= 0) { v.push_back(str_format(pt)); }
    return font_spec_build(v);
}

ustring font_size_change(const ustring & font_spec, double pts) {
    ustring family = font_family_from_spec(font_spec), face = font_face_from_spec(font_spec);
    return font_spec_build(family, face, pts);
}

ustring font_size_at_least(const ustring & font_spec, double min_size_pt) {
    double pts = font_size_from_spec(font_spec);
    return pts < min_size_pt ? font_size_change(font_spec, min_size_pt) : font_spec;
}

ustring font_size_as_max(const ustring & font_spec, double max_size_pt) {
    double pts = font_size_from_spec(font_spec);
    return pts > max_size_pt ? font_size_change(font_spec, max_size_pt) : font_spec;
}

ustring font_size_remove(const ustring & spec) {
    return font_spec_build(font_family_from_spec(spec), font_face_from_spec(spec));
}

ustring font_face_add(const ustring & spec, const ustring & face_elements) {
    auto specv = font_spec_explode(spec);
    for (auto & s: str_explode(face_elements)) { specv.push_back(s); }
    return font_spec_build(specv);
}

ustring font_face_set(const ustring & spec, const ustring & face) {
    double pt = font_size_from_spec(spec);
    ustring family = font_family_from_spec(spec);
    return font_spec_build(family, face, pt);
}

} // namespace tau

//END
