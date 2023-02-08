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
#include <tau/exception.hh>
#include <tau/font.hh>
#include <tau/string.hh>
#include <tau/style.hh>
#include <iostream>

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
    struct Item {
        std::string name_;
        Style_item  si_;
        ustring     value_;
        ustring     pvalue_;
        ustring     fmt_;
        Item *      from_ = nullptr;
        std::vector<Item *> redirs_;
        std::vector<Item *> predirs_;

        signal<void()> * signal_changed_ = nullptr;

        Item(const std::string & name, Style_impl * sty, const ustring & value):
            name_(name),
            si_(sty),
            value_(value)
        {
        }

       ~Item() {
            if (signal_changed_) { delete signal_changed_; }
        }
    };

    using Items = std::forward_list<Item>;
    using Children = std::vector<Style_impl *>;

    Style_wptr      parent_;
    Items           items_;
    Children        children_;
    Style_item      mt_;

    Style_impl() = default;

   ~Style_impl() {
       unparent();
    }

    Style_impl(const Style_impl & other):
        items_(other.items_)
    {
    }

    Style_impl & operator=(const Style_impl & other) {
        if (this != &other) {
            items_ = other.items_;
        }

        return *this;
    }

    Style_impl(Style_impl && other):
        items_(std::move(other.items_))
    {
    }

    Style_impl & operator=(Style_impl && other) {
        items_ = std::move(other.items_);
        return *this;
    }

    Item * ifind(const std::string & name) {
        auto i = std::find_if(items_.begin(), items_.end(), [name](auto & ip) { return str_similar(name, ip.name_); } );
        if (i != items_.end()) { return &(*i); }
        return nullptr;
    }

    const Item * ifind(const std::string & name) const {
        auto i = std::find_if(items_.begin(), items_.end(), [name](auto & ip) { return str_similar(name, ip.name_); } );
        if (i != items_.end()) { return &(*i); }
        return nullptr;
    }

    Item * ifind(Style_item * si) {
        auto i = std::find_if(items_.begin(), items_.end(), [si](auto & ip) { return si == &ip.si_; } );
        if (i != items_.end()) { return &(*i); }
        return nullptr;
    }

    const Item * ifind(const Style_item * si) const {
        auto i = std::find_if(items_.begin(), items_.end(), [si](auto & ip) { return si == &ip.si_; } );
        if (i != items_.end()) { return &(*i); }
        return nullptr;
    }

    signal<void()> & signal_changed(Item * ip) {
        if (!ip->signal_changed_) { ip->signal_changed_ = new signal<void()>; }
        return *ip->signal_changed_;
    }

    void unparent() {
        if (auto pp = parent_.lock()) {
            for (auto & i: items_) {
                std::vector<std::string> v;
                for (auto ip: i.predirs_) { v.push_back(ip->name_); }
                for (auto & s: v) { unredirect(s); }
            }

            auto j = std::find(pp->children_.begin(), pp->children_.end(), this);
            if (j != pp->children_.end()) { pp->children_.erase(j); }
            parent_.reset();
        }
    }

    void set_parent(Style_ptr parent) {
        parent_ = parent;
        parent->children_.push_back(this);

        for (auto & ip: items_) {
            if (!ip.from_) {
                if (auto pi = parent->ifind(ip.name_)) {
                    pset(ip, parent->iget(*pi));
                    for (auto ri: pi->redirs_) { redirect(pi->name_, ri->name_, true); }
                    for (auto ri: pi->predirs_) { redirect(pi->name_, ri->name_, true); }
                }
            }
        }
    }

    void pset(Item & i, const ustring & pvalue) {
        ustring val = i.pvalue_;
        i.pvalue_ = pvalue;

        if (pvalue != val) {
            for (auto sty: children_) { sty->npset(i.name_, iget(i)); }
            if (i.signal_changed_) { (*i.signal_changed_)(); }
        }

        for (auto ip: i.redirs_) { pset(*ip, pvalue); }
        for (auto ip: i.predirs_) { pset(*ip, pvalue); }
    }

    void iset(Item & i, const ustring & val) {
        if (val.find(U'%') < val.size()) {
            if (i.fmt_ != val) {
                i.fmt_ = val;
                for (auto sty: children_) { sty->npset(i.name_, iget(i)); }
                if (i.signal_changed_) { (*i.signal_changed_)(); }
            }
        }

        else {
            if (i.value_ != val) {
                i.value_ = val;
                for (auto sty: children_) { sty->npset(i.name_, iget(i)); }
                if (i.signal_changed_) { (*i.signal_changed_)(); }
            }
        }

        for (auto ip: i.redirs_) { iset(*ip, val); }
        for (auto ip: i.predirs_) { iset(*ip, val); }
    }

    ustring iget(const Item & i) const {
        if (i.fmt_.empty()) {
            return i.value_.empty() ? i.pvalue_ : i.value_;
        }

        ustring res;
        std::size_t pos, len = i.fmt_.size(), fspec, end;

        for (pos = 0; pos < len; pos = end) {
            fspec = i.fmt_.find(U'%', pos);
            end = std::min(len, fspec);
            res += i.fmt_.substr(pos, end-pos);

            if (fspec < len) {
                switch (i.fmt_[fspec+1]) {
                    case U'%': res += U'%'; break;
                    case U'v': res += i.value_.empty() ? i.pvalue_ : i.value_; break;
                }

                end += 2;
            }
        }

        return res;
    }

    void redirect(const std::string & src, const std::string & dest, bool from_parent=false) {
        auto i = ifind(src), j = ifind(dest);

        if (i && j && i != j && !j->si_.is_set()) {
            if (from_parent) { i->predirs_.push_back(j); }
            else { i->redirs_.push_back(j); }
            j->from_ = i;
            j->fmt_ = i->fmt_;
            pset(*j, i->pvalue_);
            iset(*j, i->value_);

            for (auto sty: children_) { sty->redirect(src, dest, true); }
        }
    }

    void unredirect(const std::string & name) {
        if (auto ip = ifind(name)) {
            if (ip->from_) {
                auto i = std::find(ip->from_->redirs_.begin(), ip->from_->redirs_.end(), ip);
                if (i != ip->from_->redirs_.end()) { ip->from_->redirs_.erase(i); }
                i = std::find(ip->from_->predirs_.begin(), ip->from_->predirs_.end(), ip);
                if (i != ip->from_->predirs_.end()) { ip->from_->predirs_.erase(i); }
                ip->from_ = nullptr;

                if (auto pp = parent_.lock()) {
                    if (auto pi = pp->ifind(ip->name_)) {
                        pset(*ip, pp->iget(*pi));
                    }
                }
            }
        }
    }

    void iunset(Item & i) {
        ustring val = i.value_;
        i.value_.clear();
        unredirect(i.name_);

        if (!val.empty()) {
            for (auto sty: children_) { sty->npset(i.name_, iget(i)); }
            if (i.signal_changed_) { (*i.signal_changed_)(); }
        }
    }

    Style_item * set(const std::string & name, const ustring & value) {
        if (auto ip = ifind(name)) {
            iset(*ip, value);
            return &ip->si_;
        }

        items_.emplace_front(name, this, value);

        if (auto pp = parent_.lock()) {
            if (auto pi = pp->ifind(name)) {
                pset(items_.front(), pp->iget(*pi));
            }
        }

        return &items_.front().si_;
    }

    void npset(const std::string & name, const ustring & pvalue) {
        if (auto ip = ifind(name)) {
            if (!ip->from_) {
                pset(*ip, pvalue);
            }
        }
    }

    void unset(const std::string & name) {
        if (auto ip = ifind(name)) {
            ip->si_.unset();
        }
    }

    void unset() {
        for (auto & p: items_) {
            p.si_.unset();
        }
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Style_item::Style_item(Style_impl * sty):
    sty_(sty)
{
}

ustring Style_item::get() const {
    if (sty_) {
        if (auto ip = sty_->ifind(this)) {
            return sty_->iget(*ip);
        }
    }

    return ustring();
}

void Style_item::set(const ustring & val) {
    if (sty_) {
        if (auto ip = sty_->ifind(this)) {
            sty_->iset(*ip, val);
        }
    }
}

ustring & Style_item::format() {
    if (sty_) {
        if (auto ip = sty_->ifind(this)) {
            return ip->fmt_;
        }
    }

    throw internal_error("Style_item::format(): have pure sty_");
}

signal<void()> & Style_item::signal_changed() {
    if (sty_) {
        if (auto ip = sty_->ifind(this)) {
            return sty_->signal_changed(ip);
        }
    }

    throw internal_error("Style_item::signal_changed(): have pure sty_");
}

void Style_item::unset() {
    if (sty_) {
        if (auto ip = sty_->ifind(this)) {
            sty_->iunset(*ip);
        }
    }
}

bool Style_item::is_set() const {
    if (sty_) {
        if (auto ip = sty_->ifind(this)) {
            return ip->from_ || !ip->redirs_.empty() || !ip->predirs_.empty() || !ip->value_.empty();
        }
    }

    return false;
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

    si_.format() = str_implode(fv, ' ');
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
    set(fmt);
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

Style::Style():
    impl(std::make_shared<Style_impl>())
{
}

Style::~Style() {}

Style::Style(const Style & other) {
    impl = std::make_shared<Style_impl>(*(other.impl));
}

Style & Style::operator=(const Style & other) {
    if (this != &other) {
        impl = std::make_shared<Style_impl>(*(other.impl));
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
    auto ip = impl->ifind(name);
    return ip ? ip->si_ : impl->mt_;
}

const Style_item & Style::get(const std::string & name) const {
    auto ip = impl->ifind(name);
    return ip ? ip->si_ : impl->mt_;
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
    auto ip = impl->ifind(name);
    return ip ? Font_style(ip->si_) : Font_style(impl->mt_);
}

Color_style Style::color(const std::string & name) {
    auto ip = impl->ifind(name);
    return ip ? Color_style(ip->si_) : Color_style(impl->mt_);
}

} // namespace tau

//END
