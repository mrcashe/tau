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
#include <mutex>

namespace {

const char * true_words_  = "true yes y on";
const char * bool_words_  = "true yes y on false no n off none";

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

struct Key_file_impl;

struct Key_section {
    ustring comment_;
    using Pair = std::pair<ustring, ustring>;
    using Storage = std::list<Pair>;
    Storage elems_;
    const Key_file_impl * kf_ = nullptr;
    ustring name_;

    Key_section(const Key_file_impl * kf, const ustring & name): kf_(kf), name_(name) {}
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
    bool changed_ = false;
    signal<void()> * signal_changed_ = nullptr;

   ~Key_file_impl() {
        if (signal_changed_) { delete signal_changed_; }
    }

    Key_file_impl():
        root_(this, "root")
    {
    }

    Key_file_impl(const Key_file_impl & other):
        locked_(other.locked_),
        csep_(other.csep_),
        lsep_(other.lsep_),
        root_(other.root_),
        sections_(other.sections_),
        path_(other.path_),
        changed_(other.changed_)
    {
        own();
    }

    Key_file_impl(Key_file_impl && other):
        locked_(other.locked_),
        csep_(other.csep_),
        lsep_(other.lsep_),
        root_(std::move(other.root_)),
        sections_(std::move(other.sections_)),
        path_(std::move(other.path_)),
        changed_(other.changed_)
    {
        own();
    }

    Key_file_impl & operator=(const Key_file_impl & other) {
        if (this != &other) {
            locked_ = other.locked_;
            csep_ = other.csep_;
            lsep_ = other.lsep_;
            root_ = other.root_;
            sections_ = other.sections_;
            path_ = other.path_;
            changed_ = other.changed_;
            own();
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
        changed_ = other.changed_;
        own();
        return *this;
    }

    signal<void()> & signal_changed() {
        if (!signal_changed_) { signal_changed_ = new signal<void()>; }
        return *signal_changed_;
    }

    void set_changed() {
        changed_ = true;
        if (signal_changed_) { (*signal_changed_)(); }
    }

    void own() {
        root_.kf_ = this;
        for (auto & p: sections_) { p.second.kf_ = this; }
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
        sections_.emplace_back(sect_name, Key_section(this, sect_name));
        set_changed();
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

        if (changed) { set_changed(); }
    }

    void remove_section(const ustring & sect_name, bool similar) {
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            auto i = find(sect_name, similar);
            if (i != sections_.end()) { sections_.erase(i); changed = true; }
        }

        if (changed) { set_changed(); }
    }

    void remove_key(Key_section & sect, const ustring & key, bool similar) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            auto iter = sect.find(key, similar);
            if (iter != sect.elems_.end()) { sect.elems_.erase(iter); changed = true; }
        }

        if (changed) { set_changed(); }
    }

    std::vector<double> get_doubles(Key_section & sect, const ustring & key) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
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

    std::vector<double> get_doubles(const ustring & key) {
        return get_doubles(root_, key);
    }

    double get_double(Key_section & sect, const ustring & key, double fallback) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        auto iter = sect.find(key);
        if (iter == sect.elems_.end()) { return fallback; }
        std::string s = iter->second;
        for (std::size_t pos = s.find(','); ustring::npos != pos; pos = s.find(',')) { s[pos] = '.'; }
        double d; std::istringstream is(s); is >> d;
        return d;
    }

    double get_double(const ustring & key, double fallback) {
        return get_double(root_, key, fallback);
    }

    std::vector<long long> get_integers(Key_section & sect, const ustring & key) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
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

    std::vector<long long> get_integers(const ustring & key) {
        return get_integers(root_, key);
    }

    long long get_integer(Key_section & sect, const ustring & key, long long fallback) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        auto i = sect.find(key);

        if (i != sect.elems_.end()) {
            try { return std::stoll(i->second, nullptr, 0); }
            catch (std::exception & x) { std::cerr << "** Key_file::get_integer(): an exception caught: " << x.what() << std::endl; }
        }

        return fallback;
    }

    long long get_integer(const ustring & key, long long fallback) {
        return get_integer(root_, key, fallback);
    }

    std::vector<bool> get_booleans(Key_section & sect, const ustring & key) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
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

    std::vector<bool> get_booleans(const ustring & key) {
        return get_booleans(root_, key);
    }

    bool get_boolean(Key_section & sect, const ustring & key, bool fallback) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        auto iter = sect.find(key);
        if (iter == sect.elems_.end()) { return fallback; }

        if (!str_similar(iter->second, bool_words_, U' ')) {
            try { return 0 != std::stoll(iter->second, nullptr, 0); }
            catch (...) { return false; }
        }

        return str_similar(iter->second, true_words_, U' ');
    }

    bool get_boolean(const ustring & key, bool fallback) {
        return get_boolean(root_, key, fallback);
    }

    std::vector<ustring> get_strings(Key_section & sect, const ustring & key) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        std::vector<ustring> v;
        auto iter = sect.find(key);

        if (iter != sect.elems_.end()) {
            v = str_explode(iter->second, lsep_);
        }

        return v;
    }

    std::vector<ustring> get_strings(const ustring & key) {
        return get_strings(root_, key);
    }

    ustring get_string(Key_section & sect, const ustring & key, const ustring & fallback) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        auto iter = sect.find(key);
        return iter == sect.elems_.end() ? fallback : iter->second;
    }

    ustring get_string(const ustring & key, const ustring & fallback) {
        return get_string(root_, key, fallback);
    }

    ustring comment(Key_section & sect) {
        Lock lock(mx_);
        return sect.comment_;
    }

    ustring comment() {
        return comment(root_);
    }

    void set_comment(Key_section & sect, const ustring & comment) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);

            if (comment != sect.comment_) {
                sect.comment_ = comment;
                changed = true;
            }
        }

        if (changed) { set_changed(); }
    }

    void set_comment(const ustring & comment) {
        set_comment(root_, comment);
    }

    void set_string(Key_section & sect, const ustring & key, const ustring & value) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, value);
        }

        if (changed) { set_changed(); }
    }

    void set_string(const ustring & key, const ustring & value) {
        set_string(root_, key, value);
    }

    void set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, str_implode(vec, lsep_));
        }

        if (changed) { set_changed(); }
    }

    void set_strings(const ustring & key, const std::vector<ustring> & vec) {
        set_strings(root_, key, vec);
    }

    void set_boolean(Key_section & sect, const ustring & key, bool value) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, value ? "true" : "false");
        }

        if (changed) { set_changed(); }
    }

    void set_boolean(const ustring & key, bool value) {
        set_boolean(root_, key, value);
    }

    void set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            std::vector<ustring> sv;
            for (bool b: vec) { sv.push_back(b ? "true" : "false"); }
            changed = sect.set_value(key, str_implode(sv, lsep_));
        }

        if (changed) { set_changed(); }
    }

    void set_booleans(const ustring & key, const std::vector<bool> & vec) {
        set_booleans(root_, key, vec);
    }

    void set_integer(Key_section & sect, const ustring & key, long long value) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, str_format(value));
        }

        if (changed) { set_changed(); }
    }

    void set_integer(const ustring & key, long long value) {
        set_integer(root_, key, value);
    }

    void set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            std::vector<ustring> sv;
            for (long long ll: vec) { sv.push_back(str_format(ll)); }
            changed = sect.set_value(key, str_implode(sv, lsep_));
        }

        if (changed) { set_changed(); }
    }

    void set_integers(const ustring & key, const std::vector<long long> & vec) {
        set_integers(root_, key, vec);
    }

    void set_double(Key_section & sect, const ustring & key, double value) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            changed = sect.set_value(key, str_format(value));
        }

        if (changed) { set_changed(); }
    }

    void set_double(const ustring & key, double value) {
        set_double(root_, key, value);
    }

    void set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec) {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        bool changed = false;

        if (!locked_) {
            Lock lock(mx_);
            std::vector<ustring> sv;
            for (double d: vec) { sv.push_back(str_format(d)); }
            changed = sect.set_value(key, str_implode(sv, lsep_));
        }

        if (changed) { set_changed(); }
    }

    void set_doubles(const ustring & key, const std::vector<double> & vec) {
        set_doubles(root_, key, vec);
    }

    std::vector<ustring> list_keys(const Key_section & sect) const {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        return sect.list_keys();
    }

    std::vector<ustring> list_keys() const {
        return list_keys(root_);
    }

    bool has_key(const Key_section & sect, const ustring & key_name, bool similar) const {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        return sect.has_key(key_name, similar);
    }

    bool has_key(const ustring & key_name, bool similar) const {
        return has_key(root_, key_name, similar);
    }

    ustring key_name(const Key_section & sect, const ustring & similar_name) const {
        if (this != sect.kf_) { throw user_error(str_format(this, " Key_file: foreign section '", sect.name_, '\'')); }
        Lock lock(mx_);
        return sect.key_name(similar_name);
    }

    ustring key_name(const ustring & similar_name) const {
        return key_name(root_, similar_name);
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

        if (changed) { set_changed(); }
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
    flush();
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
        impl->changed_ = false;
    }
}

void Key_file::save(const ustring & path) {
    path_mkdir(path_dirname(path));
    auto & io = Locale().iocharset();
    std::ofstream os(io.is_utf8() ? std::string(path) : io.encode(path));
    save(os);
}

void Key_file::save() {
    if (impl->path_.empty()) { throw user_error("Key_file::save(): was not loaded from file"); }
    save(impl->path_);
}

void Key_file::flush() {
    if (!impl->locked_ && impl->changed_ && !impl->path_.empty()) {
        try {
            save(impl->path_);
        }

        catch (exception & x) {
            std::cerr << "** Key_file::flush(): failed to write to file " << impl->path_ << ": " << x.what() << std::endl;
        }

        catch (std::exception & x) {
            std::cerr << "** Key_file::flush(): failed to write to file " << impl->path_ << ": " << x.what() << std::endl;
        }

        catch (...) {
            std::cerr << "** Key_file::flush(): failed to write to file " << impl->path_ << std::endl;
        }
    }
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

bool Key_file::has_key(const ustring & key_name, bool similar) const {
    return impl->has_key(key_name, similar);
}

ustring Key_file::key_name(const Key_section & sect, const ustring & similar_name) const {
    return impl->key_name(sect, similar_name);
}

ustring Key_file::key_name(const ustring & similar_name) const {
    return impl->key_name(impl->root_, similar_name);
}

Key_section & Key_file::root() {
    return impl->root_;
}

Key_section & Key_file::section(const ustring & sect_name, bool similar) {
    return impl->section(sect_name, similar);
}

void Key_file::set_comment(Key_section & sect, const ustring & comment) {
    impl->set_comment(sect, comment);
}

void Key_file::set_comment(const ustring & comment) {
    impl->set_comment(comment);
}

void Key_file::set_string(Key_section & sect, const ustring & key, const ustring & value) {
    impl->set_string(sect, key, value);
}

void Key_file::set_string(const ustring & key, const ustring & value) {
    impl->set_string(key, value);
}

void Key_file::set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec) {
    impl->set_strings(sect, key, vec);
}

void Key_file::set_strings(const ustring & key, const std::vector<ustring> & vec) {
    impl->set_strings(key, vec);
}

void Key_file::set_boolean(Key_section & sect, const ustring & key, bool value) {
    impl->set_boolean(sect, key, value);
}

void Key_file::set_boolean(const ustring & key, bool value) {
    impl->set_boolean(key, value);
}

void Key_file::set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec) {
    impl->set_booleans(sect, key, vec);
}

void Key_file::set_booleans(const ustring & key, const std::vector<bool> & vec) {
    impl->set_booleans(key, vec);
}

void Key_file::set_integer(Key_section & sect, const ustring & key, long long value) {
    impl->set_integer(sect, key, value);
}

void Key_file::set_integer(const ustring & key, long long value) {
    impl->set_integer(key, value);
}

void Key_file::set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec) {
    impl->set_integers(sect, key, vec);
}

void Key_file::set_integers(const ustring & key, const std::vector<long long> & vec) {
    impl->set_integers(key, vec);
}

void Key_file::set_double(Key_section & sect, const ustring & key, double value) {
    impl->set_double(sect, key, value);
}

void Key_file::set_double(const ustring & key, double value) {
    impl->set_double(key, value);
}

void Key_file::set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec) {
    impl->set_doubles(sect, key, vec);
}

void Key_file::set_doubles(const ustring & key, const std::vector<double> & vec) {
    impl->set_doubles(key, vec);
}

ustring Key_file::comment(Key_section & sect) {
    return impl->comment(sect);
}

ustring Key_file::comment() {
    return impl->comment(root());
}

ustring Key_file::get_string(Key_section & sect, const ustring & key, const ustring & fallback) {
    return impl->get_string(sect, key, fallback);
}

ustring Key_file::get_string(const ustring & key, const ustring & fallback) {
    return impl->get_string(root(), key, fallback);
}

std::vector<ustring> Key_file::get_strings(Key_section & sect, const ustring & key) {
    return impl->get_strings(sect, key);
}

std::vector<ustring> Key_file::get_strings(const ustring & key) {
    return impl->get_strings(root(), key);
}

bool Key_file::get_boolean(Key_section & sect, const ustring & key, bool fallback) {
    return impl->get_boolean(sect, key, fallback);
}

bool Key_file::get_boolean(const ustring & key, bool fallback) {
    return impl->get_boolean(root(), key, fallback);
}

std::vector<bool> Key_file::get_booleans(Key_section & sect, const ustring & key) {
    return impl->get_booleans(sect, key);
}

std::vector<bool> Key_file::get_booleans(const ustring & key) {
    return impl->get_booleans(root(), key);
}

long long Key_file::get_integer(Key_section & sect, const ustring & key, long long fallback) {
    return impl->get_integer(sect, key, fallback);
}

long long Key_file::get_integer(const ustring & key, long long fallback) {
    return impl->get_integer(root(), key, fallback);
}

std::vector<long long> Key_file::get_integers(Key_section & sect, const ustring & key) {
    return impl->get_integers(sect, key);
}

std::vector<long long> Key_file::get_integers(const ustring & key) {
    return impl->get_integers(root(), key);
}

double Key_file::get_double(Key_section & sect, const ustring & key, double fallback) {
    return impl->get_double(sect, key, fallback);
}

double Key_file::get_double(const ustring & key, double fallback) {
    return impl->get_double(root(), key, fallback);
}

std::vector<double> Key_file::get_doubles(Key_section & sect, const ustring & key) {
    return impl->get_doubles(sect, key);
}

std::vector<double> Key_file::get_doubles(const ustring & key) {
    return impl->get_doubles(root(), key);
}

void Key_file::remove_key(Key_section & sect, const ustring & key, bool similar) {
    impl->remove_key(sect, key, similar);
}

void Key_file::remove_key(const ustring & key, bool similar) {
    impl->remove_key(root(), key, similar);
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

bool Key_file::changed() const {
    return impl->changed_;
}

signal<void()> & Key_file::signal_changed() {
    return impl->signal_changed();
}

std::istream & operator>>(std::istream & is, Key_file & kf) {
    kf.load(is); return is;
}

std::ostream & operator<<(std::ostream & os, const Key_file & kf) {
    kf.save(os); return os;
}

} // namespace tau

//END
