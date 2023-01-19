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

#include <tau/buffer.hh>
#include <tau/sys.hh>
#include <tau/key-file.hh>
#include <tau/string.hh>
#include <cstdlib>
#include <iostream>
#include <list>

namespace tau {

struct Key_section {
    ustring comment_;
    using Pair = std::pair<ustring, ustring>;
    using Storage = std::list<Pair>;
    Storage elems_;

    Key_section() = default;
    Key_section(const Key_section & other) = default;
    Key_section & operator=(const Key_section & other) = default;

    // Return true if changed.
    bool set_value(const ustring & name, const ustring & val) {
        auto i = std::find_if(elems_.begin(), elems_.end(), [name](const Pair & pair) { return name == pair.first; });

        if (i == elems_.end()) {
            elems_.emplace_back(name, val);
            return true;
        }

        else {
            if (i->second != val) {
                i->second = val;
                return true;
            }
        }

        return false;
    }

    bool has_key(const ustring & key_name, bool similar) const {
        return similar
            ? std::any_of(elems_.begin(), elems_.end(), [key_name](const Pair & pair) { return str_similar(key_name, pair.first); } )
            : elems_.end() != std::find_if(elems_.begin(), elems_.end(), [key_name](const Pair & pair) { return key_name == pair.first; } );
    }

    Storage::iterator find(const ustring & key_name, bool similar=false) {
        if (similar) {
            return std::find_if(elems_.begin(), elems_.end(), [key_name](auto & p) { return str_similar(key_name, p.first); } );
        }

        else {
            return std::find_if(elems_.begin(), elems_.end(), [key_name](auto & p) { return key_name == p.first; } );
        }
    }

    ustring key_name(const ustring & name) const {
        auto i = std::find_if(elems_.begin(), elems_.end(), [name](auto & p) { return str_similar(name, p.first); } );
        return i != elems_.end() ? i->first : ustring();
    }

    void stream_out(ustring & os, char csep) {
        if (!comment_.empty()) {
            auto v = str_explode(comment_, str_newlines());

            for (auto & s: v) {
                os += csep;
                os += ' ';
                os += s;
                os += '\n';
            }
        }

        if (!elems_.empty()) {
            for (auto & e: elems_) {
                os += e.first;
                os += '=';
                os += e.second;
                os += '\n';
            }

            os += '\n';
        }
    }

    void clear() {
        elems_.clear();
        comment_.clear();
    }

    bool empty() const {
        return comment_.empty() && elems_.empty();
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct Key_file_impl {
    char32_t csep_ = '#';
    char32_t lsep_ = path_sep_;
    Key_section root_;
    using Pair = std::pair<ustring, Key_section>;
    using Storage = std::list<Pair>;
    Storage sections_;
    signal<void()> signal_changed_;

    Key_file_impl() = default;

    void copy(const Key_file_impl & other) {
        csep_ = other.csep_;
        root_ = other.root_;
        sections_ = other.sections_;
    }

    void stream_out(ustring & os) {
        root_.stream_out(os, csep_);

        for (auto & p: sections_) {
            os += '[';
            os += p.first;
            os += ']';
            os += '\n';
            p.second.stream_out(os, csep_);
        }
    }

    Storage::iterator find(const ustring & sect_name) {
        return std::find_if(sections_.begin(), sections_.end(), [sect_name](const Pair & pair) { return sect_name == pair.first; } );
    }

    bool has_section(const ustring & sect_name, bool similar) const {
        return similar
            ? std::any_of(sections_.begin(), sections_.end(), [sect_name](const Pair & pair) { return str_similar(sect_name, pair.first); } )
            : sections_.end() != std::find_if(sections_.begin(), sections_.end(), [sect_name](const Pair & pair) { return sect_name == pair.first; } );
    }

    bool has_key(const ustring & sect_name, const ustring & key_name, bool similar) const {
        if (similar) {
            for (auto & sect: sections_) {
                if (str_similar(sect_name, sect.first)) {
                    return sect.second.has_key(key_name, true);
                }
            }

            return false;
        }

        else {
            auto i = std::find_if(sections_.begin(), sections_.end(), [sect_name](const Pair & pair) { return sect_name == pair.first; } );
            return i != sections_.end() && i->second.has_key(key_name, false);
        }
    }

    Key_section & section(const ustring & sect_name, bool similar) {
        if (similar) {
            for (auto & p: sections_) {
                if (str_similar(sect_name, p.first)) {
                    return p.second;
                }
            }
        }

        else {
            auto i = std::find_if(sections_.begin(), sections_.end(), [sect_name](const Pair & pair) { return sect_name == pair.first; } );
            if (i != sections_.end()) { return i->second; }
        }

        sections_.emplace_back(sect_name, Key_section());
        signal_changed_();
        return sections_.back().second;
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Key_file::Key_file(char32_t list_sep, char32_t comment_sep):
    trackable(),
    impl_(new Key_file_impl)
{
    set_list_separator(list_sep);
    set_comment_separator(comment_sep);
}

Key_file::Key_file(std::istream & is, char32_t list_sep, char32_t comment_sep):
    impl_(new Key_file_impl)
{
    set_list_separator(list_sep);
    set_comment_separator(comment_sep);
    load(is);
}

void Key_file::load(std::istream & is) {
    if (is.good()) {
        char32_t csep = ' ';
        Buffer buf;
        ustring sect;

        buf.insert(buf.cend(), is);
        for (Buffer_citer i = buf.cbegin(); i != buf.cend(); i.move_forward_line()) {
            i.skip_blanks();

            if (' ' == csep && ('#' == *i || ';' == *i)) {
                csep = *i;
            }

            if (csep != *i) {
                Buffer_citer ii(i);
                ii.move_to_eol();
                ustring s = str_trim(buf.text(i, ii));
                ustring::size_type pos;

                if ('[' == s[0]) {
                    pos = s.find(']', 1);
                    if (ustring::npos != pos) {
                        ustring name = s.substr(1, pos-1);
                        sect = name;
                    }
                }

                pos = s.find('=');
                if (ustring::npos != pos) {
                    ustring name = str_trim(s.substr(0, pos)), val  = str_trim(s.substr(pos+1));

                    if (!name.empty() && !val.empty()) {
                        Key_section & sct = sect.empty() ? impl_->root_ : impl_->section(sect, false);
                        if (sct.set_value(name, val)) { impl_->signal_changed_(); }
                    }
                }
            }
        }

        impl_->csep_ = csep;
    }
}

Key_file::Key_file(const Key_file & other):
    impl_(new Key_file_impl)
{
    impl_->copy(*other.impl_);
}

Key_file & Key_file::operator=(const Key_file & other) {
    if (this != &other) {
        impl_->copy(*other.impl_);
    }

    return *this;
}

Key_file::~Key_file() {
    delete impl_;
}

void Key_file::save(std::ostream & os) {
    ustring s;
    impl_->stream_out(s);
    os << s;
}

void Key_file::set_comment_separator(char32_t comment_sep) {
    if (0 != comment_sep) {
        impl_->csep_ = comment_sep;
    }
}

void Key_file::set_list_separator(char32_t list_sep) {
    if (0 != list_sep) {
        impl_->lsep_ = list_sep;
    }
}

char32_t Key_file::list_separator() const {
    return impl_->lsep_;
}

char32_t Key_file::comment_separator() const {
    return impl_->csep_;
}

std::vector<ustring> Key_file::list_sections() const {
    std::vector<ustring> v;
    for (auto & p: impl_->sections_) { v.push_back(p.first); }
    return v;
}

std::vector<ustring> Key_file::list_keys(const Key_section & sect) const {
    std::vector<ustring> v;
    for (auto & p: sect.elems_) { v.push_back(p.first); }
    return v;
}

bool Key_file::has_section(const ustring & sect_name, bool similar) const {
    return impl_->has_section(sect_name, similar);
}

bool Key_file::has_key(const ustring & sect_name, const ustring & key_name, bool similar) const {
    return impl_->has_key(sect_name, key_name, similar);
}

bool Key_file::has_key(const Key_section & sect, const ustring & key_name, bool similar) const {
    return sect.has_key(key_name, similar);
}

ustring Key_file::key_name(const Key_section & sect, const ustring similar_name) const {
    return sect.key_name(similar_name);
}

Key_section & Key_file::root() {
    return impl_->root_;
}

Key_section & Key_file::section(const ustring & sect, bool similar) {
    return impl_->section(sect, similar);
}

void Key_file::set_comment(Key_section & sect, const ustring & comment) {
    sect.comment_ = comment;
}

void Key_file::set_string(Key_section & sect, const ustring & key, const ustring & value) {
    if (sect.set_value(key, value)) { impl_->signal_changed_(); }
}

void Key_file::set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec) {
    if (sect.set_value(key, str_implode(vec, impl_->lsep_))) { impl_->signal_changed_(); }
}

void Key_file::set_boolean(Key_section & sect, const ustring & key, bool value) {
    if (sect.set_value(key, value ? "true" : "false")) { impl_->signal_changed_(); }
}

void Key_file::set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec) {
    std::vector<ustring> sv;
    for (auto i: vec) { sv.push_back(i ? "true" : "false"); }
    if (sect.set_value(key, str_implode(sv, impl_->lsep_))) { impl_->signal_changed_(); }
}

void Key_file::set_integer(Key_section & sect, const ustring & key, long long value) {
    if (sect.set_value(key, str_format(value))) { impl_->signal_changed_(); }
}

void Key_file::set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec) {
    std::vector<ustring> sv;
    for (auto i: vec) { sv.push_back(str_format(i)); }
    if (sect.set_value(key, str_implode(sv, impl_->lsep_))) { impl_->signal_changed_(); }
}

void Key_file::set_double(Key_section & sect, const ustring & key, double value) {
    if (sect.set_value(key, str_format(value))) { impl_->signal_changed_(); }
}

void Key_file::set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec) {
    std::vector<ustring> sv;
    for (auto i: vec) { sv.push_back(str_format(i)); }
    if (sect.set_value(key, str_implode(sv, impl_->lsep_))) { impl_->signal_changed_(); }
}

ustring Key_file::comment(Key_section & sect) {
    return sect.comment_;
}

ustring Key_file::get_string(Key_section & sect, const ustring & key, const ustring & fallback) {
    auto iter = sect.find(key);
    return iter == sect.elems_.end() ? fallback : iter->second;
}

std::vector<ustring> Key_file::get_strings(Key_section & sect, const ustring & key) {
    std::vector<ustring> v;
    auto iter = sect.find(key);

    if (iter != sect.elems_.end()) {
        v = str_explode(iter->second, impl_->lsep_);
    }

    return v;
}

bool Key_file::get_boolean(Key_section & sect, const ustring & key, bool fallback) {
    auto iter = sect.find(key);
    if (iter == sect.elems_.end()) { return fallback; }
    try { return 0 != std::stoll(iter->second, nullptr, 0); } catch (...) {}
    return str_similar(iter->second, "true yes y on", ' ');
}

std::vector<bool> Key_file::get_booleans(Key_section & sect, const ustring & key) {
    std::vector<bool> v;

    auto iter = sect.find(key);
    if (iter != sect.elems_.end()) {
        for (auto s: str_explode(iter->second, impl_->lsep_)) {
            try { v.push_back(0 != std::stoll(s, nullptr, 0)); } catch (...) {}
            v.push_back(str_similar(s, "true yes y on", ' '));
        }
    }

    return v;
}

long long Key_file::get_integer(Key_section & sect, const ustring & key, long long fallback) {
    auto iter = sect.find(key);

    if (iter != sect.elems_.end()) {
        try { return std::stoll(iter->second, nullptr, 0); }
        catch (std::exception & x) { std::cerr << "** Key_file::get_integer(): an exception caught: " << x.what() << std::endl; }
    }

    return fallback;
}

std::vector<long long> Key_file::get_integers(Key_section & sect, const ustring & key) {
    std::vector<long long> v;

    auto iter = sect.find(key);
    if (iter != sect.elems_.end()) {
        for (const ustring & s: str_explode(iter->second, impl_->lsep_)) {
            try { v.push_back(std::stoll(s, nullptr, 0)); }
            catch (std::exception & x) { std::cerr << "** Key_file::get_integer(): an exception caught: " << x.what() << std::endl; }
        }
    }

    return v;
}

double Key_file::get_double(Key_section & sect, const ustring & key, double fallback) {
    auto iter = sect.find(key);
    if (iter == sect.elems_.end()) { return fallback; }
    std::string s = iter->second;
    for (std::size_t pos = s.find(','); ustring::npos != pos; pos = s.find(',')) { s[pos] = '.'; }
    double d; std::istringstream is(s); is >> d;
    return d;
}

std::vector<double> Key_file::get_doubles(Key_section & sect, const ustring & key) {
    std::vector<double> v;

    auto iter = sect.find(key);
    if (iter != sect.elems_.end()) {
        for (std::string s: str_explode(iter->second, impl_->lsep_)) {
            for (std::size_t pos = s.find(','); ustring::npos != pos; pos = s.find(',')) { s[pos] = '.'; }
            double d; std::istringstream is(s); is >> d;
            v.push_back(d);
        }
    }

    return v;
}

void Key_file::remove_key(Key_section & sect, const ustring & key, bool similar) {
    auto iter = sect.find(key, similar);
    if (iter != sect.elems_.end()) { sect.elems_.erase(iter); impl_->signal_changed_(); }
}

void Key_file::remove_section(const ustring & sect_name, bool similar) {
    if (similar) {
        auto i = std::find_if(impl_->sections_.begin(), impl_->sections_.end(), [&sect_name](auto & p) { return str_similar(sect_name, p.first); } );
        if (i != impl_->sections_.end()) { impl_->sections_.erase(i); impl_->signal_changed_(); }
    }

    else {
        auto i = impl_->find(sect_name);
        if (i != impl_->sections_.end()) { impl_->sections_.erase(i); impl_->signal_changed_(); }
    }
}

void Key_file::clear() {
    if (!impl_->root_.empty()) { impl_->root_.clear(); impl_->signal_changed_(); }
    if (!impl_->sections_.empty()) { impl_->sections_.clear(); impl_->signal_changed_(); }
}

bool Key_file::empty() const {
    return impl_->root_.empty() && impl_->sections_.empty();
}

signal<void()> & Key_file::signal_changed() {
    return impl_->signal_changed_;
}

} // namespace tau

//END
