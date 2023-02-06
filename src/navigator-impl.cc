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

#include <tau/icon.hh>
#include <tau/locale.hh>
#include <tau/loop.hh>
#include <tau/navigator.hh>
#include <tau/string.hh>
#include <tau/sys.hh>
#include <display-impl.hh>
#include <image-impl.hh>
#include <list-impl.hh>
#include <navigator-impl.hh>
#include <pixmap-impl.hh>
#include <text-impl.hh>
#include <theme-impl.hh>
#include <window-impl.hh>
#include <iostream>

namespace tau {

const char * NAVIGATOR_INFO_NAME = "name";
const char * NAVIGATOR_INFO_BYTES = "bytes";
const char * NAVIGATOR_INFO_DATE = "date";
const char * NAVIGATOR_INFO_HIDDEN = "hidden";
const char * NAVIGATOR_INFO_PLACES = "places";

Navigator_impl::Navigator_impl(const ustring & uri):
    Bin_impl()
{
    uri_ = file_exists(uri) || file_is_dir(path_dirname(uri)) ? uri : path_cwd();
    list_ = std::make_shared<List_impl>();
    list_->set_column_spacing(3);
    list_->show_header(0, "Name");
    list_->show_header(1, "Size");
    list_->show_header(2, "Date");
    list_->show_sort_marker(0);
    list_->set_min_column_width(0, 80);
    list_->signal_row_activated().connect(fun(this, &Navigator_impl::on_list_activate));
    list_->signal_mark_validate().connect(fun(this, &Navigator_impl::on_list_mark_validate));
    list_->signal_header_click().connect(fun(this, &Navigator_impl::on_list_header_click));
    list_->signal_header_width_changed().connect(fun(this, &Navigator_impl::on_list_header_width_changed));
    list_->signal_size_changed().connect(fun(this, &Navigator_impl::limit_name_column));
    insert(list_);

    signal_display().connect(fun(this, &Navigator_impl::on_display));
    signal_unparent().connect(fun(this, &Navigator_impl::on_unparent));
}

void Navigator_impl::cleanup() {
    for (auto * hol: hcache_) {
        if (hol == holder_) { holder_ = nullptr; }
        delete hol;
    }

    if (holder_) { delete holder_; holder_ = nullptr; }
    hcache_.clear();
}

Navigator_impl::~Navigator_impl() {
    cleanup();
}

void Navigator_impl::on_unparent() {
    cleanup();
    if (list_) { list_->clear(); clear(); list_.reset(); }
}

void Navigator_impl::on_display() {
    set_uri(uri_);
    show_current_dir();
}

int Navigator_impl::find_row(const ustring & name) {
    if (holder_) {
        for (auto & rec: holder_->recs_) {
            if (name == rec.name_) {
                return rec.row_;
            }
        }
    }

    return INT_MIN;
}

ustring Navigator_impl::name_from_row(int row) {
    if (holder_) {
        for (auto & rec: holder_->recs_) {
            if (row == rec.row_) {
                return rec.name_;
            }
        }
    }

    return ustring();
}

void Navigator_impl::new_dir(const ustring & path) {
    if (!holder_ || holder_->path_ != path) {
        if (holder_) {
            if (hcache_.size() >= 32) {
                Holder * h = hcache_.front();
                hcache_.pop_front();
                delete h;
            }

            if (!find_cached_holder(holder_->path_)) {
                hcache_.push_back(holder_);
            }
        }

        holder_ = find_cached_holder(path);

        if (!holder_) {
            holder_ = new Holder;
            holder_->path_ = path;
            read_dir(holder_);
            preprocess(*holder_);
            holder_->finfo_ = Fileinfo(path);
            int event_mask = FILE_CREATED|FILE_DELETED|FILE_MOVED_IN|FILE_MOVED_OUT;
            holder_->wcx_ = holder_->finfo_.signal_watch(event_mask).connect(bind(fun(this, &Navigator_impl::on_watch), path));
        }

        if (holder_) {
            if (!holder_->prep_) { preprocess(*holder_); }
            signal_dir_changed_(path);
            show_current_dir();
        }
    }
}

void Navigator_impl::show_record(Rec & rec) {
    if (list_ && !list_->running() && !rec.filtered_ && (hidden_visible_ || !rec.hidden_)) {

        // Show file/dir name.
        Text_ptr txt = std::make_shared<Text_impl>(rec.name_, ALIGN_START);
        // txt->set_wrap_mode(WRAP_ELLIPSIZE_CENTER);
        txt->signal_select().connect(tau::bind(fun(this, &Navigator_impl::on_file_select), rec.name_));
        txt->signal_unselect().connect(tau::bind(fun(this, &Navigator_impl::on_file_unselect), rec.name_));
        rec.row_ = list_->append_row(txt);
        list_->align(txt, ALIGN_START, ALIGN_CENTER);
        Pixmap_cptr ico;

        // Show modify date/time.
        if (date_visible_) {
            txt = std::make_shared<Text_impl>(format_file_time(rec.fi_));
            list_->insert(rec.row_, txt, 1, true);
            list_->align(txt, ALIGN_END, ALIGN_CENTER);
        }

        if (rec.fi_.is_dir()) {
            if (!dir_icon_) { dir_icon_ = Theme_impl::root()->find_icon(ICON_FOLDER, SMALL_ICON); }
            ico = dir_icon_;
        }

        else {
            if (!unknown_icon_) { unknown_icon_ = Theme_impl::root()->find_icon(ICON_UNKNOWN, SMALL_ICON); }
            ico = unknown_icon_;

            // Do not show file size for directory.
            if (bytes_visible_) {
                txt = std::make_shared<Text_impl>(str_bytes(rec.fi_.bytes()));
                list_->insert(rec.row_, txt, 0, true);
                list_->align(txt, ALIGN_END, ALIGN_CENTER);
            }
        }

        if (ico) {
            Image_ptr img = std::make_shared<Image_impl>(ico, true);
            list_->insert(rec.row_, img, -1, true);
            list_->align(img, ALIGN_START, ALIGN_CENTER);
        }
    }
}

void Navigator_impl::show_current_dir() {
    if (list_ && !list_->running()) {
        set_cursor("wait:watch");
        list_->clear();
        // unsigned nn = 0;

        if (holder_) {
            for (unsigned & n: holder_->indice_) {
                Rec & rec = holder_->recs_[n];

                if (rec.fi_.is_dir()) {
                    show_record(rec);
                    // std::cout << " shown " << ++nn << " of " << holder_->recs.size() << ": " << rec.name << '\n';
                }
            }

            if (!dirs_only_visible_) {
                for (unsigned & n: holder_->indice_) {
                    Rec & rec = holder_->recs_[n];

                    if (!rec.fi_.is_dir()) {
                        show_record(rec);
                        // std::cout << " shown " << ++nn << " of " << holder_->recs.size() << ": " << rec.name << '\n';
                    }
                }
            }
        }

        unset_cursor();
    }
}

// May throw!
void Navigator_impl::read_dir(Holder * hol) {
    auto fs = path_list(hol->path_);
    hol->recs_.clear();

    for (const ustring & name: fs) {
        if ("." != name && ".." != name) {
            ustring path = path_build(hol->path_, name);

            try {
                Fileinfo fi(path);
                Rec rec;
                rec.name_ = name;
                rec.fi_ = fi;
                rec.hidden_ = fi.is_hidden();
                hol->recs_.push_back(rec);
            }

            catch (...) {}
        }
    }
}

void Navigator_impl::preprocess(Holder & hol) {
    hol.prep_ = true;

    for (Rec & rec: hol.recs_) {
        rec.filtered_ = false;

        if (!rec.fi_.is_dir()) {
            if (!filters_.empty()) {
                rec.filtered_ = true;

                for (const ustring & pattern: filters_) {
                    if (path_match(pattern, rec.name_)) {
                        rec.filtered_ = false;
                        break;
                    }
                }
            }
        }
    }

    hol.indice_.resize(hol.recs_.size());
    for (unsigned n = 0; n < hol.indice_.size(); ++n) { hol.indice_[n] = n; }

    if (NAVIGATOR_INFO_NAME == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r2].name_ < hol.recs_[r1].name_; } );
        }

        else {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r1].name_ < hol.recs_[r2].name_; } );
        }
    }

    else if (NAVIGATOR_INFO_DATE == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r2].fi_.mtime() < hol.recs_[r1].fi_.mtime(); } );
        }

        else {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r1].fi_.mtime() < hol.recs_[r2].fi_.mtime(); } );
        }
    }

    else if (NAVIGATOR_INFO_BYTES == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r2].fi_.bytes() < hol.recs_[r1].fi_.bytes(); } );
        }

        else {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r1].fi_.bytes() < hol.recs_[r2].fi_.bytes(); } );
        }
    }

    else if ("file" == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r2].type_ < hol.recs_[r1].type_; } );
        }

        else {
            std::sort(hol.indice_.begin(), hol.indice_.end(), [hol](unsigned r1, unsigned r2) { return hol.recs_[r1].type_ < hol.recs_[r2].type_; } );
        }
    }
}

Navigator_impl::Holder * Navigator_impl::find_cached_holder(const ustring & path) {
    Holder * hol = nullptr;
    auto iter = std::find_if(hcache_.begin(), hcache_.end(), [path](Holder * h) { return path == h->path_; });
    if (hcache_.end() != iter) { hol = *iter; }
    return hol;
}

void Navigator_impl::select_name(const ustring & name) {
    if (list_ && !list_->running()) {
        int br = find_row(name);
        if (br > INT_MIN) { list_->select_row(br); }
    }
}

void Navigator_impl::refresh() {
    if (holder_ && list_ && !list_->running()) {
        int selected_row = list_->selected_row();
        ustring filename;
        if (INT_MIN != selected_row) { filename = name_from_row(selected_row); }
        read_dir(holder_);
        preprocess(*holder_);
        show_current_dir();
        if (!filename.empty()) { select_name(filename); }
    }
}

void Navigator_impl::set_uri(const ustring & uri) {
    try {
        ustring path = path_is_absolute(uri) ? uri : path_build(path_cwd(), uri);

        if (!file_is_dir(path)) {
            ustring p = path_dirname(path);
            if (file_is_dir(p)) { uri_ = p; }
        }

        else {
            uri_ = path;
        }

        new_dir(uri_);
        if (file_exists(path)) { select_name(path_notdir(path)); }
    }

    catch (...) {
        uri_ = path_cwd();
        new_dir(uri_);
    }
}

void Navigator_impl::on_list_activate(int br) {
    ustring name = name_from_row(br);

    if (!name.empty()) {
        try {
            ustring p = path_build(uri(), name);
            Fileinfo fi(p);

            if (fi.is_dir()) {
                set_uri(p);
            }

            else {
                signal_file_activate_(p);
            }
        }

        catch (...) {}
    }
}

bool Navigator_impl::on_list_mark_validate(int row) {
    if (!dir_select_allowed_ && !dirs_only_visible_) {
        if (holder_) {
            for (const Rec & rec: holder_->recs_) {
                if (rec.row_ == row) {
                    return rec.fi_.is_dir();
                }
            }
        }
    }

    return false;
}

void Navigator_impl::on_list_header_click(int column) {
    switch (column) {
        case 0:
            if (NAVIGATOR_INFO_NAME != sort_by_) { sort_by(NAVIGATOR_INFO_NAME); }
            else if (sorted_backward()) { sort_forward(); }
            else { sort_backward(); }
            break;

        case 1:
            if (NAVIGATOR_INFO_BYTES != sort_by_) { sort_by(NAVIGATOR_INFO_BYTES); }
            else if (sorted_backward()) { sort_forward(); }
            else { sort_backward(); }
            break;

        case 2:
            if (NAVIGATOR_INFO_DATE != sort_by_) { sort_by(NAVIGATOR_INFO_DATE); }
            else if (sorted_backward()) { sort_forward(); }
            else { sort_backward(); }
            break;
    }
}

void Navigator_impl::on_list_header_width_changed(int column) {
    if (list_) {
        if (0 == column) {
            list_->set_max_column_width(0, 0);
        }
    }
}

void Navigator_impl::limit_name_column() {
    if (list_ && 0 == list_->column_width(0)) {
        unsigned w = 0;
        if (bytes_visible_ || date_visible_) { w = (7*list_->size().width())/10; }
        list_->set_max_column_width(0, w);
    }
}

void Navigator_impl::on_file_select(const ustring & filename) {
    signal_file_select_(filename);
}

void Navigator_impl::on_file_unselect(const ustring & filename) {
    signal_file_unselect_(filename);
}

void Navigator_impl::sort_forward() {
    if (sort_backward_) {
        sort_backward_ = false;

        if (list_) {
            if (NAVIGATOR_INFO_NAME == sort_by_) { list_->show_sort_marker(0); }
            else if (NAVIGATOR_INFO_BYTES == sort_by_) { list_->show_sort_marker(1); }
            else if (NAVIGATOR_INFO_DATE == sort_by_) { list_->show_sort_marker(2); }
        }

        if (holder_) {
            for (auto hol: hcache_) { hol->prep_ = false; }
            preprocess(*holder_);
            show_current_dir();
        }
    }
}

void Navigator_impl::sort_backward() {
    if (!sort_backward_) {
        sort_backward_ = true;

        if (list_) {
            if (NAVIGATOR_INFO_NAME == sort_by_) { list_->show_sort_marker(0, true); }
            else if (NAVIGATOR_INFO_BYTES == sort_by_) { list_->show_sort_marker(1, true); }
            else if (NAVIGATOR_INFO_DATE == sort_by_) { list_->show_sort_marker(2, true); }
        }

        if (holder_) {
            for (auto hol: hcache_) { hol->prep_ = false; }
            preprocess(*holder_);
            show_current_dir();
        }
    }
}

void Navigator_impl::hidden_visible_files() {
    if (!hidden_visible_) {
        hidden_visible_ = true;
        show_current_dir();
    }
}

void Navigator_impl::hide_hidden_files() {
    if (hidden_visible_) {
        hidden_visible_ = false;
        show_current_dir();
    }
}

ustring Navigator_impl::format_file_time(const Fileinfo & fi) const {
    Timeval mtime = fi.mtime(), now = Timeval::now();
    tm tm_file = mtime.localtime(), tm_now = now.localtime();
    return mtime.str((tm_file.tm_year == tm_now.tm_year ? "%e %b %R" : "%x"));
}

void Navigator_impl::allow_multiple_select() {
    multiple_select_allowed_ = true;
    if (list_) { list_->allow_multiple_select(); }
}

void Navigator_impl::disallow_multiple_select() {
    multiple_select_allowed_ = false;
    if (list_) { list_->disallow_multiple_select(); }
}

void Navigator_impl::allow_dir_select() {
    dir_select_allowed_ = true;
}

void Navigator_impl::disallow_dir_select() {
    dir_select_allowed_ = false;
}

void Navigator_impl::set_filter(const ustring & filters) {
    filters_.clear();

    if ("*" != filters) {
        for (const ustring & s: str_explode(filters, ":;,")) {
            filters_.push_back(str_trim(s));
        }
    }

    for (auto hol: hcache_) { hol->prep_ = false; }

    if (holder_) {
        preprocess(*holder_);
        show_current_dir();
    }
}

void Navigator_impl::on_watch(unsigned event, const ustring & filename, const ustring & dirname) {
    Holder * holder = nullptr;

    if (holder_ && dirname == holder_->path_) {
        holder = holder_;
    }

    for (Holder * h: hcache_) {
        if (h->path_ == dirname) {
            holder = h;
            break;
        }
    }

    if (holder) {
        if (holder->wtimer_.signal_alarm().empty()) {
            holder->wtimer_.signal_alarm().connect(tau::bind(fun(this, &Navigator_impl::on_watch_timer), dirname));
        }

        holder->wtimer_.start(251);
    }
}

void Navigator_impl::on_watch_timer(const ustring & dirname) {
    Holder * holder = nullptr;

    if (holder_ && dirname == holder_->path_) {
        holder = holder_;
    }

    for (Holder * h: hcache_) {
        if (h->path_ == dirname) {
            holder = h;
            break;
        }
    }

    if (holder) {
        if (holder_ == holder) {
            refresh();
        }

        else {
            read_dir(holder);
            preprocess(*holder);
        }
    }
}

void Navigator_impl::sort_by(const ustring & col) {
    int list_col = -1;

    if (NAVIGATOR_INFO_NAME == col) {
        list_col = 0;
        sort_by_ = col;
    }

    else if (NAVIGATOR_INFO_BYTES == col) {
        list_col = 1;
        sort_by_ = col;
    }

    else if (NAVIGATOR_INFO_DATE == col) {
        list_col = 2;
        sort_by_ = col;
    }

    if (list_col < 0) { sort_by_ = ""; }

    list_->show_sort_marker(list_col, sorted_backward());

    if (holder_) {
        for (auto hol: hcache_) { hol->prep_ = false; }
        preprocess(*holder_);
        show_current_dir();
    }
}

void Navigator_impl::show_info(const ustring & items, char32_t sep) {
    for (const ustring & s: str_explode(items, sep)) {
        if (str_similar(NAVIGATOR_INFO_DATE, s)) { date_visible_ = true; }
        else if (str_similar(NAVIGATOR_INFO_BYTES, s)) { bytes_visible_ = true; }
        else if (str_similar(NAVIGATOR_INFO_HIDDEN, s)) { hidden_visible_ = true; }
    }

    show_current_dir();
    limit_name_column();
}

void Navigator_impl::hide_info(const ustring & items, char32_t sep) {
    for (const ustring & s: str_explode(items, sep)) {
        if (str_similar(NAVIGATOR_INFO_DATE, s)) { date_visible_ = false; }
        else if (str_similar(NAVIGATOR_INFO_BYTES, s)) { bytes_visible_ = false; }
        else if (str_similar(NAVIGATOR_INFO_HIDDEN, s)) { hidden_visible_ = false; }
    }

    show_current_dir();
    limit_name_column();
}

bool Navigator_impl::info_visible(const ustring & s) const {
    if (str_similar(NAVIGATOR_INFO_NAME, s)) { return true; }
    else if (str_similar(NAVIGATOR_INFO_BYTES, s)) { return bytes_visible_; }
    else if (str_similar(NAVIGATOR_INFO_DATE, s)) { return date_visible_; }
    else if (str_similar(NAVIGATOR_INFO_HIDDEN, s)) { return hidden_visible_; }
    return false;
}

ustring Navigator_impl::visible_info_items(char32_t sep) const {
    ustring res = NAVIGATOR_INFO_NAME;
    if (bytes_visible_) { res += sep; res += NAVIGATOR_INFO_BYTES; }
    if (date_visible_) { res += sep; res += NAVIGATOR_INFO_DATE; }
    if (hidden_visible_) { res += sep; res += NAVIGATOR_INFO_HIDDEN; }
    return res;
}

ustring Navigator_impl::invisible_info_items(char32_t sep) const {
    ustring res;

    if (!bytes_visible_) { res += NAVIGATOR_INFO_BYTES; }
    if (!date_visible_) { if (!res.empty()) { res += sep; } res += NAVIGATOR_INFO_DATE; }
    if (!hidden_visible_) { if (!res.empty()) { res += sep; } res += NAVIGATOR_INFO_HIDDEN; }

    return res;
}

} // namespace tau

//END
