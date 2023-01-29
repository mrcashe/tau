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

#include <types-impl.hh>
#include <tau/font.hh>
#include <tau/string.hh>
#include <tau/style.hh>
#include <iostream>
#include <unordered_map>

namespace tau {

const char * STYLE_FONT = "font";
const char * STYLE_FOREGROUND = "foreground";
const char * STYLE_MENU_FOREGROUND = "menu/foreground";
const char * STYLE_SLIDER_FOREGROUND = "slider/foreground";
const char * STYLE_PROGRESS_FOREGROUND = "progress/foreground";
const char * STYLE_ACCEL_FOREGROUND = "accel/foreground";
const char * STYLE_BACKGROUND = "background";
const char * STYLE_PROGRESS_BACKGROUND = "progress/background";
const char * STYLE_WHITESPACE_BACKGROUND = "whitespace/background";
const char * STYLE_MENU_BACKGROUND = "menu/background";
const char * STYLE_SELECT_BACKGROUND = "select/background";
const char * STYLE_TOOLTIP_BACKGROUND = "tooltip/background";
const char * STYLE_BUTTON_BACKGROUND = "button/background";
const char * STYLE_SLIDER_BACKGROUND = "slider/background";

struct Style_impl {
    using Item_ptr = std::shared_ptr<Style_item>;
    using Item_cptr = std::shared_ptr<const Style_item>;
    using Items = std::unordered_map<std::string, Item_ptr>;

    Items           items_, redir_;
    Style_item      mt_;
    connection      find_cx_;
    signal<Style_item *(const std::string &)> signal_find_;

    Style_impl() = default;
    Style_impl(const Style_impl & other) = default;
    Style_impl & operator=(const Style_impl & other) = default;
    Style_impl(Style_impl && other) = default;
    Style_impl & operator=(Style_impl && other) = default;

    void set_parent(Style_impl * parent) {
        find_cx_ = signal_find_.connect(fun(parent, &Style_impl::find));

        for (auto & p: items_) {
            if (auto pi = parent->find(p.first)) {
                auto ip = p.second;
                ip->set_pvalue(pi->get());
                ip->cx_ = pi->signal_value_changed_.connect(fun(ip, &Style_item::set_pvalue));
            }
        }
    }

    void unparent() {
        if (!find_cx_.empty()) {
            for (auto & p: items_) { p.second->cx_.drop(); }
            find_cx_.drop();
        }
    }

    void unset(const std::string & name) {
        if (auto ip = find(name)) {
            ip->unset();
        }
    }

    void unset() {
        for (auto & p: items_) {
            p.second->unset();
        }
    }

    Item_ptr set(const std::string & name, const ustring & value) {
        auto i = items_.find(name);

        if (i == items_.end()) {
            Item_ptr item = std::make_shared<Style_item>(value);
            items_[name] = item;

            if (auto pi = signal_find_(name)) {
                item->set_pvalue(pi->get());
                item->cx_ = pi->signal_value_changed_.connect(fun(item, &Style_item::set_pvalue));
            }

            return item;
        }

        else {
            i->second->set(value);
            return i->second;
        }
    }

    Style_item * find(const std::string & name) {
        auto i = items_.find(name);
        if (i != items_.end()) { return i->second.get(); }
        i = redir_.find(name);
        return i != redir_.end() ? i->second.get() : nullptr;
    }

    void redirect(const std::string & src, const std::string & dest) {
        if (redir_.end() == redir_.find(dest)) {
            auto i = items_.find(src), j = items_.find(dest);

            if (i != items_.end() && j != items_.end()) {
                Item_ptr s = i->second, d = j->second;
                s->signal_value_changed_ += d->signal_value_changed_;
                s->signal_changed_ += d->signal_changed_;
                items_.erase(j);
                redir_[dest] = s;
                s->set_ = true;
                s->signal_value_changed_(s->get());
                s->signal_changed_();
            }
        }
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Style_item::Style_item(const ustring & value):
    trackable()
{
    set(value);
}

ustring Style_item::get() const {
    if (fmt_.empty()) {
        return value_.empty() ? pvalue_ : value_;
    }

    ustring res;
    std::size_t pos, len = fmt_.size(), fspec, end;

    for (pos = 0; pos < len; pos = end) {
        fspec = fmt_.find(U'%', pos);
        end = std::min(len, fspec);
        res += fmt_.substr(pos, end-pos);

        if (fspec < len) {
            switch (fmt_[fspec+1]) {
                case U'%': res += U'%'; break;
                case U'v': res += value_.empty() ? pvalue_ : value_; break;
            }

            end += 2;
        }
    }

    return res;
}

void Style_item::set(const ustring & val) {
    if (val.find(U'%') < val.size()) {
        fmt_ = val;
        signal_value_changed_(get());
        signal_changed_();
    }

    else {
        if (value_ != val) {
            value_ = val;
            signal_value_changed_(get());
            signal_changed_();
        }
    }
}

ustring Style_item::format() const {
    return fmt_;
}

signal<void()> & Style_item::signal_changed() {
    return signal_changed_;
}

bool Style_item::is_set() const {
    return set_ || !value_.empty();
}

void Style_item::unset() {
    set_ = false;
    ustring val = value_;
    value_.clear();

    if (!val.empty()) {
        signal_value_changed_(get());
        signal_changed_();
    }
}

void Style_item::set_pvalue(const ustring & pvalue) {
    ustring val  = pvalue_;
    pvalue_ = pvalue;

    if (set_ || (pvalue != val)) {
        signal_value_changed_(get());
        signal_changed_();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Font_style::Font_style(Style_item & si):
    si_(si)
{
}

ustring Font_style::spec() const {
    ustring val = si_.get(), fam = font_family_from_spec(val), face = font_face_from_spec(val);
    double pts = font_size_from_spec(val);
    return font_spec_build(fam, face, pts);
}

void Font_style::set(const ustring & spec) {
    si_.set(spec);
}

double Font_style::size() const {
    return font_size_from_spec(si_.get());
}

void Font_style::clear_size() {
    ustring fmt = si_.format();
    auto fv = str_explode(fmt, str_blanks());
    bool done;

    do {
        done = true;

        for (auto i = fv.begin(); i != fv.end(); ++i) {
            if (ustring::npos == i->find_first_not_of("+-=.,0123456789eE")) {
                done = false;
                fv.erase(i);
                break;
            }
        }
    } while (!done);

    fmt = str_implode(fv, ' ');
    set(fmt);
}

void Font_style::resize(double pts) {
    if (pts > 0.0) {
        clear_size();
        ustring fmt = si_.format();
        if (ustring::npos == fmt.find(U'%')) { fmt = "%v"; }
        fmt = str_format(fmt, " =", pts);
        set(fmt);
    }
}

void Font_style::grow(double pts) {
    ustring spc = spec();
    auto v = font_spec_explode(spc);

    if (!v.empty()) {
        clear_size();
        pts += font_size_from_spec(v);
        set(font_spec_build(font_family_from_spec(v), font_face_from_spec(v), pts));
    }
}

void Font_style::enlarge(double pts) {
    ustring fmt = si_.format();
    auto fv = str_explode(fmt, str_blanks());
    bool done;

    do {
        done = true;

        for (auto i = fv.begin(); i != fv.end(); ++i) {
            if (ustring::npos == i->find_first_not_of("=.,0123456789eE")) {
                done = false;
                fv.erase(i);
                break;
            }
        }
    } while (!done);

    fmt = str_implode(fv, ' ');
    if (ustring::npos == fmt.find(U'%')) { fmt = "%v"; }
    fmt = str_format(fmt, " ", std::showpos, pts);
    set(fmt);
}

void Font_style::add_face(const ustring & face_elements) {
    ustring fmt = si_.format();
    if (ustring::npos == fmt.find(U'%')) { fmt = "%v"; }
    fmt = str_format(fmt, " ", face_elements);
    si_.set(fmt);
}

void Font_style::set_face(const ustring & face) {
    set(font_face_set(spec(), face));
}

Font_style::operator ustring() const {
    return spec();
}

void Font_style::operator=(const ustring & s) {
    set(s);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Color_style::Color_style(Style_item & si):
    si_(si)
{
}

Color Color_style::get() const {
    return Color(si_.get());
}

void Color_style::set(const Color & color) {
    si_.set(color.html());
}

Color_style::operator Color() const {
    return get();
}

Color_style & Color_style::operator=(const Color & color) {
    set(color);
    return *this;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Style::Style() {
    impl = new Style_impl;
}

Style::~Style() {
    delete impl;
}

Style::Style(const Style & other) {
    impl = new Style_impl(*(other.impl));
}

Style & Style::operator=(const Style & other) {
    if (this != &other) {
        delete impl;
        impl = new Style_impl(*(other.impl));
    }

    return *this;
}

void Style::set_parent(Style & parent) {
    impl->set_parent(parent.impl);
}

void Style::unparent() {
    impl->unparent();
}

Style_item & Style::set(const std::string & name, const ustring & value) {
    return *impl->set(name, value);
}

Style_item & Style::get(const std::string & name) {
    auto ip = impl->find(name);
    return ip ? *ip : impl->mt_;
}

const Style_item & Style::get(const std::string & name) const {
    auto ip = impl->find(name);
    return ip ? *ip : impl->mt_;
}

void Style::redirect(const std::string & src, const std::string & dest) {
    impl->redirect(src, dest);
}

void Style::unset(const std::string & name) {
    impl->unset(name);
}

void Style::unset() {
    impl->unset();
}

Font_style Style::font(const std::string & name) {
    auto ip = impl->find(name);
    return ip ? Font_style(*ip) : Font_style(impl->mt_);
}

Color_style Style::color(const std::string & name) {
    auto ip = impl->find(name);
    return ip ? Color_style(*ip) : Color_style(impl->mt_);
}

} // namespace tau

//END
