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

#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/timeval.hh>
#include <font-impl.hh>
#include <posix/font-face-posix.hh>
#include <posix/font-file-posix.hh>
#include "font-utils-posix.hh"
#include <list>
#include <unordered_map>
#include <vector>

#include <iostream>

using Mutex = std::recursive_mutex;
using Lock = std::lock_guard<Mutex>;

namespace {

using Facemap = std::map<tau::ustring, tau::ustring>;

struct Family_holder {
    tau::ustring        family;
    Facemap             faces;
};

struct Font_holder {
    tau::Font_ptr       font;
    tau::Timeval        tv;
};

struct Registry {
    tau::ustring        path;
    tau::ustring        family;
    tau::ustring        face;
    tau::Font_face_ptr  faceptr;
    tau::Font_file_ptr  ttf;
};

using Families  = std::unordered_map<std::string, Family_holder>;
using Cache     = std::unordered_map<std::string, Font_holder>;
using Registrar = std::unordered_map<std::string, Registry>;

Mutex           mutex_;
Families        families_;
Cache           cache_;
tau::ustring    normal_;
Registrar       reg_;

std::vector<std::string> list_font_family_keys() {
    std::vector<std::string> v;
    for (auto & famh: families_) { v.push_back(famh.first); }
    return v;
}

std::string font_family_key(const tau::ustring & family_name) {
    return tau::str_toupper(tau::str_trim(family_name));
}

std::string partial_key(const tau::ustring & family, const tau::ustring & face) {
    return tau::str_toupper(tau::str_trim(tau::str_format(family, ' ', face)));
}

tau::Font_face_ptr create_font_face_nested(const tau::ustring & family, const tau::ustring & facename) {
    Lock lock(mutex_);
    auto i = reg_.find(partial_key(family, facename));

    if (i != reg_.end()) {
        if (!i->second.faceptr) {
            if (!i->second.ttf) { i->second.ttf = tau::Font_file::create(i->second.path); }
            i->second.faceptr = i->second.ttf->face(i->second.ttf, family, facename);
        }

        return i->second.faceptr;
    }

    return nullptr;
}

void init_font_dir(const tau::ustring & dir) {
    if (tau::file_is_dir(dir)) {
        for (auto & fp: tau::path_find(dir)) {
            if (!tau::file_is_dir(fp)) {
                if (tau::str_has_suffix(tau::str_tolower(fp), ".ttf")) {
                    try {
                        if (reg_.end() == std::find_if(reg_.begin(), reg_.end(), [&fp](auto & p) { return fp == p.second.path; } )) {
                            tau::Font_file_ptr ttf = tau::Font_file::create(fp);
                            std::vector<tau::ustring> famv, facev;

                            for (auto & fam: ttf->list_families()) {
                                for (auto & face: ttf->list_faces(fam)) {
                                    auto specv = tau::font_spec_explode(tau::font_spec_build(fam, face));
                                    auto famf = tau::font_family_from_spec(specv);
                                    auto facef = tau::font_face_from_spec(specv);
                                    famv.push_back(famf);
                                    facev.push_back(facef);

                                    if (tau::str_similar(facef, "Oblique")) { facev.push_back("Italic"); }
                                    if (tau::str_similar(facef, "Normal")) { facev.push_back("Regular"); }
                                    if (tau::str_similar(facef, "Book")) { facev.push_back("Regular"); }
                                }
                            }

                            for (auto & fam: famv) {
                                for (auto & face: facev) {
                                    std::string key = partial_key(fam, face);
                                    Registry reg;
                                    reg.path = fp;
                                    reg.family = fam;
                                    reg.face = face;
                                    reg_[key] = reg;
                                    std::cout << reg_.size() << " " << key << '\n';

                                    key = font_family_key(fam);
                                    auto i = families_.find(key);
                                    Family_holder * hol;

                                    if (i == families_.end()) {
                                        Family_holder hfam;
                                        hfam.family = fam;
                                        auto p = families_.emplace(std::make_pair(key, hfam));
                                        hol = &(p.first->second);
                                    }

                                    else {
                                        hol = &(i->second);
                                    }

                                    if (hol->faces.end() == hol->faces.find(face)) {
                                        hol->faces[face] = fp;
                                    }
                                }
                            }
                        }
                    }

                    catch (tau::bad_font & bf) {
                        std::cerr << "** " << bf.what() << std::endl;
                    }
                }
            }
        }
    }
}

void init_fonts() {
    Lock lock(mutex_);

    if (families_.empty()) {
        init_font_dir(tau::path_build(tau::path_prefix_dir(), "fonts"));
        init_font_dir(tau::path_build(tau::path_user_home_dir(), ".fonts"));
        init_font_dir("/usr/share/fonts");
        init_font_dir("/usr/local/share/fonts");
    }
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

// static
ustring Font::normal() {
    init_fonts();
    Lock lock(mutex_);

    if (normal_.empty()) {
        auto keys = list_font_family_keys();
        const ustring fonts = "Droid Sans:DejaVu Serif Book:Noto Sans:Ubuntu:Free Sans";

        for (const ustring & s: str_explode(fonts, ':')) {
            std::string key = font_family_key(font_family_from_spec(s));

            for (auto & k: keys) {
                if (key == k) {
                    normal_ = str_format(s, ' ', 10);
                    return normal_;
                }
            }
        }
    }

    return normal_;
}

// static
std::vector<ustring> Font::list_families() {
    init_fonts();
    std::vector<ustring> v;
    Lock lock(mutex_);

    for (auto & reg: reg_) {
        if (!str_similar(reg.second.family, v)) {
            v.push_back(reg.second.family);
        }
    }

    //for (auto & hfam: families_) { v.push_back(hfam.family); }
    return v;
}

std::vector<ustring> Font::list_faces(const ustring & family) {
    init_fonts();
    std::vector<ustring> v;
    Lock lock(mutex_);

    auto i = families_.find(font_family_key(family));

    if (i != families_.end()) {
        for (auto & p: i->second.faces) {
            if (!str_similar(p.first, v)) {
                v.push_back(p.first);
            }
        }
    }

    return v;
}

Font_face_ptr create_font_face(const ustring & spec) {
    init_fonts();

    auto specv = font_spec_explode(spec);
    ustring family = font_family_from_spec(specv);
    // std::cout << "cff: " << font_face_from_spec(specv) << '\n';
    Font_face_ptr facep = create_font_face_nested(family, font_face_from_spec(specv));
    if (facep) { return facep; }

    // Not found, trying to append face specification to family.

    auto famv = str_explode(family, ' ');

    if (!famv.empty()) {
        auto facev = str_explode(font_face_from_spec(specv), ' ');
        std::vector<ustring> fv;

        for (unsigned n = 0; n < facev.size(); ++n) {
            fv.push_back(facev[n]);
            facep = create_font_face_nested(famv[0]+" "+str_implode(fv, ' '), "Regular");
            if (facep) { return facep; }
        }
    }

    // Not found, trying to replace "Italic" face by "Oblique".

    auto i = specv.begin();

    while (i != specv.end()) {
        if (str_similar(*i, "Italic")) {
            break;
        }

        ++i;
    }

    if (i != specv.end()) {
        *i = "Oblique";
        facep = create_font_face_nested(family, font_face_from_spec(specv));
        if (facep) { return facep; }
    }

    return nullptr;
}

void cache_font(Font_ptr font, const ustring & spec) {
    if (font) {
        uint64_t size = std::round(64.0*font->size());
        std::string key = str_format(str_toupper(font_size_remove(spec)), ' ', size, ' ', font->dpi());
        Font_holder hol;
        hol.font = font;
        hol.tv = Timeval::now();
        Lock lock(mutex_);
        cache_[key] = hol;
    }
}

Font_ptr uncache_font(const ustring & spec, unsigned dpi) {
    Font_ptr font;
    uint64_t size = std::round(64.0*font_size_from_spec(spec));
    std::string key = str_format(str_toupper(font_size_remove(spec)), ' ', size, ' ', dpi);

    Lock lock(mutex_);
    auto iter = cache_.find(key);

    if (iter != cache_.end()) {
        font = iter->second.font;
        iter->second.tv = Timeval::now();
    }

    return font;
}

void cleanup_font_cache() {
    Timeval now = Timeval::now();
    Lock lock(mutex_);

    for (auto iter = cache_.begin(); iter != cache_.end(); ++iter) {
        auto & fh = iter->second;

        if (1 == fh.font.use_count()) {
            if (180000000 < now-fh.tv) {
                cache_.erase(iter);
                break;
            }
        }

        else {
            fh.tv = Timeval::now();
        }
    }
}

} // namespace tau

//END
