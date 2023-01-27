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
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <tau/sys.hh>
#include <tau/key-file.hh>
#include <tau/string.hh>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>

namespace {

const char * true_words_  = "true yes y on";
const char * bool_words_  = "true yes y on false no n off none";

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

struct Key_section {
    ustring comment_;
    using Pair = std::pair<ustring, ustring>;
    using Storage = std::list<Pair>;
    Storage elems_;

    Key_section() = default;
    Key_section(const Key_section & other) = default;
    Key_section(Key_section && other) = default;
    Key_section & operator=(const Key_section & other) = default;
    Key_section & operator=(Key_section && other) = default;

    std::vector<ustring> list_keys() const {
        std::vector<ustring> v;
        for (auto & p: elems_) { v.push_back(p.first); }
        return v;
    }

    // Return true if changed.
    bool set_value(const ustring & name, const ustring & val) {
        auto i = std::find_if(elems_.begin(), elems_.end(), [name](auto & p) { return name == p.first; } );

        if (i == elems_.end()) {
            elems_.emplace_back(std::make_pair(name, val));
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
            ? std::any_of(elems_.begin(), elems_.end(), [key_name](auto & p) { return str_similar(key_name, p.first); } )
            : elems_.end() != std::find_if(elems_.begin(), elems_.end(), [key_name](auto & p) { return key_name == p.first; } );
    }

    Storage::iterator find(const ustring & key, bool similar=false) {

        if (similar) {
            return std::find_if(elems_.begin(), elems_.end(), [key](auto & p) { return str_similar(key, p.first); } );
        }

        else {
            auto i = std::find_if(elems_.begin(), elems_.end(), [key](auto & p) { return key == p.first; } );
            return i;
        }
    }

    ustring key_name(const ustring & name) const {
        auto i = std::find_if(elems_.begin(), elems_.end(), [name](auto & p) { return str_similar(name, p.first); } );
        return i != elems_.end() ? i->first : ustring();
    }

    void stream_out(ustring & os, char csep) const {
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
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;
    using Pair = std::pair<ustring, Key_section>;
    using Storage = std::list<Pair>;

    mutable Mutex mx_;
    bool locked_ = false;
    char32_t csep_ = '#';
    char32_t lsep_ = path_sep_;
    Key_section root_;
    Storage sections_;
    ustring path_;
    signal<void()> signal_changed_;

    Key_file_impl() = default;

    Key_file_impl(const Key_file_impl & other):
        locked_(other.locked_),
        csep_(other.csep_),
        lsep_(other.lsep_),
        root_(other.root_),
        sections_(other.sections_),
        path_(other.path_)
    {
    }

    Key_file_impl(Key_file_impl && other):
        locked_(other.locked_),
        csep_(other.csep_),
        lsep_(other.lsep_),
        root_(std::move(other.root_)),
        sections_(std::move(other.sections_)),
        path_(std::move(other.path_))
    {
    }

    Key_file_impl & operator=(const Key_file_impl & other) {
        if (this != &other) {
            locked_ = other.locked_;
            csep_ = other.csep_;
            lsep_ = other.lsep_;
            root_ = other.root_;
            sections_ = other.sections_;
            path_ = other.path_;
        }

        return *this;
    }

    Key_file_impl & operator=(Key_file_impl && other) {
        locked_ = other.locked_;
        csep_ = other.csep_;
        lsep_ = other.lsep_;
        root_ = std::move(other.root_);
        sections_ = std::move(other.sections_);
        path_ = std::move(other.path_);
        return *this;
    }

    void stream_out(ustring & os) const {
        Lock lock(mx_);
        root_.stream_out(os, csep_);

        for (auto & p: sections_) {
            os += '[';
            os += p.first;
            os += ']';
            os += '\n';
            p.second.stream_out(os, csep_);
        }
    }

    std::vector<ustring> list_sections() const {
        std::vector<ustring> v;
        Lock lock(mx_);
        for (auto & p: sections_) { v.push_back(p.first); }
        return v;
    }

    Storage::iterator find(const ustring & sect_name, bool similar=false) {
        if (similar) {
            return std::find_if(sections_.begin(), sections_.end(), [sect_name](auto & p) { return str_similar(p.first, sect_name); } );
        }

        else {
            return std::find_if(sections_.begin(), sections_.end(), [sect_name](auto & p) { return p.first == sect_name; } );
        }
    }

    Storage::const_iterator find(const ustring & sect_name, bool similar=false) const {
        if (similar) {
            return std::find_if(sections_.begin(), sections_.end(), [sect_name](auto & p) { return str_similar(p.first, sect_name); } );
        }

        else {
            return std::find_if(sections_.begin(), sections_.end(), [sect_name](auto & p) { return p.first == sect_name; } );
        }
    }

    bool has_section(const ustring & sect_name, bool similar) const {
        Lock lock(mx_);
        return find(sect_name, similar) != sections_.end();
    }

    bool has_key(const ustring & sect_name, const ustring & key_name, bool similar) const {
        Lock lock(mx_);
        auto i = find(sect_name, similar);
        return i != sections_.end() && i->second.has_key(key_name, similar);
    }

    Key_section & section(const ustring & sect_name, bool similar=false) {
        Lock lock(mx_);
        auto i = find(sect_name, similar);
        if (i != sections_.end()) { return i->second; }
        sections_.emplace_back(std::make_pair(sect_name, Key_section()));
        signal_changed_();
        return sections_.back().second;
    }

    bool empty() const {
        Lock lock(mx_);
        return root_.empty() && sections_.empty();
    }

    void clear() {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            if (!root_.empty()) { root_.clear(); changed = true; }
            if (!sections_.empty()) { sections_.clear(); changed = true; }
        }

        if (changed) { signal_changed_(); }
    }

    void remove_section(const ustring & sect_name, bool similar) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            auto i = find(sect_name, similar);
            if (i != sections_.end()) { sections_.erase(i); changed = true; }
        }

        if (changed) { signal_changed_(); }
    }

    void remove_key(Key_section & sect, const ustring & key, bool similar) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            auto iter = sect.find(key, similar);
            if (iter != sect.elems_.end()) { sect.elems_.erase(iter); changed = true; }
        }

        if (changed) { signal_changed_(); }
    }

    std::vector<double> get_doubles(Key_section & sect, const ustring & key) {
        std::vector<double> v;
        Lock lock(mx_);
        auto iter = sect.find(key);

        if (iter != sect.elems_.end()) {
            for (std::string s: str_explode(iter->second, lsep_)) {
                for (std::size_t pos = s.find(','); ustring::npos != pos; pos = s.find(',')) { s[pos] = '.'; }
                double d; std::istringstream is(s); is >> d;
                v.push_back(d);
            }
        }

        return v;
    }

    double get_double(Key_section & sect, const ustring & key, double fallback) {
        Lock lock(mx_);
        auto iter = sect.find(key);
        if (iter == sect.elems_.end()) { return fallback; }
        std::string s = iter->second;
        for (std::size_t pos = s.find(','); ustring::npos != pos; pos = s.find(',')) { s[pos] = '.'; }
        double d; std::istringstream is(s); is >> d;
        return d;
    }

    std::vector<long long> get_integers(Key_section & sect, const ustring & key) {
        std::vector<long long> v;
        Lock lock(mx_);
        auto iter = sect.find(key);

        if (iter != sect.elems_.end()) {
            for (const ustring & s: str_explode(iter->second, lsep_)) {
                try { v.push_back(std::stoll(s, nullptr, 0)); }
                catch (std::exception & x) { std::cerr << "** Key_file::get_integer(): an exception caught: " << x.what() << std::endl; }
            }
        }

        return v;
    }

    long long get_integer(Key_section & sect, const ustring & key, long long fallback) {
        Lock lock(mx_);
        auto i = sect.find(key);

        if (i != sect.elems_.end()) {
            try { return std::stoll(i->second, nullptr, 0); }
            catch (std::exception & x) { std::cerr << "** Key_file::get_integer(): an exception caught: " << x.what() << std::endl; }
        }

        return fallback;
    }

    std::vector<bool> get_booleans(Key_section & sect, const ustring & key) {
        std::vector<bool> v;
        Lock lock(mx_);
        auto iter = sect.find(key);

        if (iter != sect.elems_.end()) {
            for (const ustring & s: str_explode(iter->second, lsep_)) {
                if (!str_similar(s, bool_words_, U' ')) {
                    bool yes = false;
                    try { yes = 0 != std::stoll(s, nullptr, 0); } catch (...) {}
                    v.push_back(yes);
                }

                else {
                    v.push_back(str_similar(s, true_words_, U' '));
                }
            }
        }

        return v;
    }

    bool get_boolean(Key_section & sect, const ustring & key, bool fallback) {
        Lock lock(mx_);
        auto iter = sect.find(key);
        if (iter == sect.elems_.end()) { return fallback; }

        if (!str_similar(iter->second, bool_words_, U' ')) {
            try { return 0 != std::stoll(iter->second, nullptr, 0); }
            catch (...) { return false; }
        }

        return str_similar(iter->second, true_words_, U' ');
    }

    std::vector<ustring> get_strings(Key_section & sect, const ustring & key) {
        Lock lock(mx_);
        std::vector<ustring> v;
        auto iter = sect.find(key);

        if (iter != sect.elems_.end()) {
            v = str_explode(iter->second, lsep_);
        }

        return v;
    }

    ustring get_string(Key_section & sect, const ustring & key, const ustring & fallback) {
        Lock lock(mx_);
        auto iter = sect.find(key);
        return iter == sect.elems_.end() ? fallback : iter->second;
    }

    ustring comment(Key_section & sect) {
        Lock lock(mx_);
        return sect.comment_;
    }

    void set_comment(Key_section & sect, const ustring & comment) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);

            if (comment != sect.comment_) {
                sect.comment_ = comment;
                changed = true;
            }
        }

        if (changed) { signal_changed_(); }
    }

    void set_string(Key_section & sect, const ustring & key, const ustring & value) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, value);
        }

        if (changed) { signal_changed_(); }
    }

    void set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, str_implode(vec, lsep_));
        }

        if (changed) { signal_changed_(); }
    }

    void set_boolean(Key_section & sect, const ustring & key, bool value) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, value ? "true" : "false");
        }

        if (changed) { signal_changed_(); }
    }

    void set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            std::vector<ustring> sv;
            for (bool b: vec) { sv.push_back(b ? "true" : "false"); }
            changed = sect.set_value(key, str_implode(sv, lsep_));
        }

        if (changed) { signal_changed_(); }
    }

    void set_integer(Key_section & sect, const ustring & key, long long value) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, str_format(value));
        }

        if (changed) { signal_changed_(); }
    }

    void set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            std::vector<ustring> sv;
            for (long long ll: vec) { sv.push_back(str_format(ll)); }
            changed = sect.set_value(key, str_implode(sv, lsep_));
        }

        if (changed) { signal_changed_(); }
    }

    void set_double(Key_section & sect, const ustring & key, double value) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, str_format(value));
        }

        if (changed) { signal_changed_(); }
    }

    void set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            std::vector<ustring> sv;
            for (double d: vec) { sv.push_back(str_format(d)); }
            changed = sect.set_value(key, str_implode(sv, lsep_));
        }

        if (changed) { signal_changed_(); }
    }

    std::vector<ustring> list_keys(const Key_section & sect) const {
        Lock lock(mx_);
        return sect.list_keys();
    }

    bool has_key(const Key_section & sect, const ustring & key_name, bool similar) const {
        Lock lock(mx_);
        return sect.has_key(key_name, similar);
    }

    ustring key_name(const Key_section & sect, const ustring similar_name) const {
        Lock lock(mx_);
        return sect.key_name(similar_name);
    }

    void load(std::istream & is) {
        bool changed = false;

        if (!locked_ && is.good()) {
            Lock lock(mx_);
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
                    std::size_t pos;

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
                            Key_section & sct = sect.empty() ? root_ : section(sect);
                            if (sct.set_value(name, val)) { changed = true; }
                        }
                    }
                }
            }

            csep_ = csep;
        }

        if (changed) { signal_changed_(); }
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Key_file::Key_file(char32_t lsep, char32_t csep):
    trackable(),
    impl(new Key_file_impl)
{
    set_list_separator(lsep);
    set_comment_separator(csep);
}

Key_file::Key_file(std::istream & is, char32_t lsep, char32_t csep):
    trackable(),
    impl(new Key_file_impl)
{
    set_list_separator(lsep);
    set_comment_separator(csep);
    try { load(is); } catch (...) {}
}

Key_file::Key_file(const ustring & path, char32_t lsep, char32_t csep):
    trackable(),
    impl(new Key_file_impl)
{
    set_list_separator(lsep);
    set_comment_separator(csep);
    impl->path_ = path;

    try {
        auto & io = Locale().iocharset();
        std::ifstream is(io.is_utf8() ? std::string(path) : io.encode(path), std::ios::binary);
        load(is);
    }

    catch (...) {}
}

Key_file::Key_file(const Key_file & other):
    impl(new Key_file_impl(*other.impl))
{
}

Key_file & Key_file::operator=(const Key_file & other) {
    if (this != &other) {
        impl = new Key_file_impl(*other.impl);
    }

    return *this;
}

Key_file::Key_file(Key_file && other):
    impl(new Key_file_impl(std::move(*other.impl)))
{
}

Key_file & Key_file::operator=(Key_file && other) {
    *impl = std::move(*other.impl);
    return *this;
}

Key_file::~Key_file() {
    delete impl;
}

void Key_file::load(std::istream & is) {
    impl->load(is);
}

void Key_file::load(const ustring & path) {
    auto & io = Locale().iocharset();
    std::ifstream is(io.is_utf8() ? std::string(path) : io.encode(path));
    load(is);
    impl->path_ = path;
}

void Key_file::save(std::ostream & os) const {
    if (os.good()) {
        ustring s;
        impl->stream_out(s);
        os << s;
    }
}

void Key_file::save(const ustring & path) {
    auto & io = Locale().iocharset();
    std::ofstream os(io.is_utf8() ? std::string(path) : io.encode(path));
    save(os);
}

void Key_file::save() {
    if (impl->path_.empty()) { throw user_error("Key_file::save(): was not loaded from file"); }
    save(impl->path_);
}

void Key_file::set_comment_separator(char32_t comment_sep) {
    if (0 != comment_sep) {
        impl->csep_ = comment_sep;
    }
}

void Key_file::set_list_separator(char32_t list_sep) {
    if (0 != list_sep) {
        impl->lsep_ = list_sep;
    }
}

char32_t Key_file::list_separator() const {
    return impl->lsep_;
}

char32_t Key_file::comment_separator() const {
    return impl->csep_;
}

std::vector<ustring> Key_file::list_sections() const {
    return impl->list_sections();
}

std::vector<ustring> Key_file::list_keys(const Key_section & sect) const {
    return impl->list_keys(sect);
}

bool Key_file::has_section(const ustring & sect_name, bool similar) const {
    return impl->has_section(sect_name, similar);
}

bool Key_file::has_key(const ustring & sect_name, const ustring & key_name, bool similar) const {
    return impl->has_key(sect_name, key_name, similar);
}

bool Key_file::has_key(const Key_section & sect, const ustring & key_name, bool similar) const {
    return impl->has_key(sect, key_name, similar);
}

ustring Key_file::key_name(const Key_section & sect, const ustring similar_name) const {
    return impl->key_name(sect, similar_name);
}

Key_section & Key_file::root() {
    return impl->root_;
}

Key_section & Key_file::section(const ustring & sect, bool similar) {
    return impl->section(sect, similar);
}

void Key_file::set_comment(Key_section & sect, const ustring & comment) {
    impl->set_comment(sect, comment);
}

void Key_file::set_string(Key_section & sect, const ustring & key, const ustring & value) {
    if (sect.set_value(key, value)) { impl->signal_changed_(); }
}

void Key_file::set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec) {
    if (sect.set_value(key, str_implode(vec, impl->lsep_))) { impl->signal_changed_(); }
}

void Key_file::set_boolean(Key_section & sect, const ustring & key, bool value) {
    if (sect.set_value(key, value ? "true" : "false")) { impl->signal_changed_(); }
}

void Key_file::set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec) {
    std::vector<ustring> sv;
    for (bool b: vec) { sv.push_back(b ? "true" : "false"); }
    if (sect.set_value(key, str_implode(sv, impl->lsep_))) { impl->signal_changed_(); }
}

void Key_file::set_integer(Key_section & sect, const ustring & key, long long value) {
    if (sect.set_value(key, str_format(value))) { impl->signal_changed_(); }
}

void Key_file::set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec) {
    std::vector<ustring> sv;
    for (long long ll: vec) { sv.push_back(str_format(ll)); }
    if (sect.set_value(key, str_implode(sv, impl->lsep_))) { impl->signal_changed_(); }
}

void Key_file::set_double(Key_section & sect, const ustring & key, double value) {
    if (sect.set_value(key, str_format(value))) { impl->signal_changed_(); }
}

void Key_file::set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec) {
    std::vector<ustring> sv;
    for (double d: vec) { sv.push_back(str_format(d)); }
    if (sect.set_value(key, str_implode(sv, impl->lsep_))) { impl->signal_changed_(); }
}

ustring Key_file::comment(Key_section & sect) {
    return impl->comment(sect);
}

ustring Key_file::get_string(Key_section & sect, const ustring & key, const ustring & fallback) {
    return impl->get_string(sect, key, fallback);
}

std::vector<ustring> Key_file::get_strings(Key_section & sect, const ustring & key) {
    return impl->get_strings(sect, key);
}

bool Key_file::get_boolean(Key_section & sect, const ustring & key, bool fallback) {
    return impl->get_boolean(sect, key, fallback);
}

std::vector<bool> Key_file::get_booleans(Key_section & sect, const ustring & key) {
    return impl->get_booleans(sect, key);
}

long long Key_file::get_integer(Key_section & sect, const ustring & key, long long fallback) {
    return impl->get_integer(sect, key, fallback);
}

std::vector<long long> Key_file::get_integers(Key_section & sect, const ustring & key) {
    return impl->get_integers(sect, key);
}

double Key_file::get_double(Key_section & sect, const ustring & key, double fallback) {
    return impl->get_double(sect, key, fallback);
}

std::vector<double> Key_file::get_doubles(Key_section & sect, const ustring & key) {
    return impl->get_doubles(sect, key);
}

void Key_file::remove_key(Key_section & sect, const ustring & key, bool similar) {
    impl->remove_key(sect, key, similar);
}

void Key_file::remove_section(const ustring & sect_name, bool similar) {
    impl->remove_section(sect_name, similar);
}

void Key_file::clear() {
    impl->clear();
}

bool Key_file::empty() const {
    return impl->empty();
}

void Key_file::lock() {
    impl->locked_ = true;
}

void Key_file::unlock() {
    impl->locked_ = false;
}

bool Key_file::locked() const {
    return impl->locked_;
}

signal<void()> & Key_file::signal_changed() {
    return impl->signal_changed_;
}

std::istream & operator>>(std::istream & is, Key_file & kf) {
    kf.load(is); return is;
}

std::ostream & operator<<(std::ostream & os, const Key_file & kf) {
    kf.save(os); return os;
}

} // namespace tau

//END
