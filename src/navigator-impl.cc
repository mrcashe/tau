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

Navigator_impl::Navigator_impl(const ustring & path):
    Bin_impl(),
    user_path_(path)
{
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

    signal_display().connect(fun(this, &Navigator_impl::show_current_dir));
    signal_unparent().connect(fun(this, &Navigator_impl::on_unparent));
    paint_cx_ = signal_paint().connect(fun(this, &Navigator_impl::on_paint));
}

Navigator_impl::~Navigator_impl() {
    for (auto * hol: hcache_) {
        if (hol == holder_) { holder_ = nullptr; }
        delete hol;
    }

    if (holder_) { delete holder_; }
}

int Navigator_impl::find_row(const ustring & name) {
    if (holder_) {
        for (auto & rec: holder_->recs) {
            if (name == rec.name) {
                return rec.br;
            }
        }
    }

    return INT_MIN;
}

ustring Navigator_impl::name_from_row(int br) {
    if (holder_) {
        for (auto & rec: holder_->recs) {
            if (br == rec.br) {
                return rec.name;
            }
        }
    }

    return ustring();
}

void Navigator_impl::new_dir(const ustring & path) {
    if (!holder_ || holder_->path != path) {
        if (holder_) {
            if (hcache_.size() >= 32) {
                Holder * h = hcache_.front();
                hcache_.pop_front();
                delete h;
            }

            if (!find_cached_holder(holder_->path)) {
                hcache_.push_back(holder_);
            }
        }

        holder_ = find_cached_holder(path);

        if (!holder_) {
            holder_ = new Holder;
            holder_->path = path;
            read_dir(holder_);
            preprocess(*holder_);
            holder_->finfo = Fileinfo(path);
            int event_mask = FILE_CREATED|FILE_DELETED|FILE_MOVED_IN|FILE_MOVED_OUT;
            holder_->finfo.signal_watch(event_mask).connect(tau::bind(fun(this, &Navigator_impl::on_file_monitor), path));
        }

        if (holder_) {
            if (!holder_->prep) { preprocess(*holder_); }
            signal_dir_changed_(path);
            show_current_dir();
        }
    }
}

ustring Navigator_impl::uri() const {
    return holder_ ? holder_->path : path_cwd();
}

void Navigator_impl::show_record(Rec & rec) {
    if (list_ && !list_->running() && !rec.filtered && (hidden_visible_ || !rec.hidden)) {

        // Show file/dir name.
        Text_ptr txt = std::make_shared<Text_impl>(rec.name, ALIGN_START);
        //txt->set_wrap_mode(WRAP_ELLIPSIZE_CENTER);
        txt->signal_select().connect(tau::bind(fun(this, &Navigator_impl::on_file_select), rec.name));
        txt->signal_unselect().connect(tau::bind(fun(this, &Navigator_impl::on_file_unselect), rec.name));
        rec.br = list_->append_row(txt);
        list_->align(txt, ALIGN_START, ALIGN_CENTER);
        Pixmap_cptr ico;

        // Show modify date/time.
        if (date_visible_) {
            txt = std::make_shared<Text_impl>(format_file_time(rec.fi));
            list_->insert(rec.br, txt, 1, true);
            list_->align(txt, ALIGN_END, ALIGN_CENTER);
        }

        if (rec.fi.is_dir()) {
            if (!dir_icon_) { dir_icon_ = Theme_impl::root()->find_icon(ICON_FOLDER, SMALL_ICON); }
            ico = dir_icon_;
        }

        else {
            if (!unknown_icon_) { unknown_icon_ = Theme_impl::root()->find_icon(ICON_UNKNOWN, SMALL_ICON); }
            ico = unknown_icon_;

            // Do not show file size for directory.
            if (bytes_visible_) {
                txt = std::make_shared<Text_impl>(str_bytes(rec.fi.bytes()));
                list_->insert(rec.br, txt, 0, true);
                list_->align(txt, ALIGN_END, ALIGN_CENTER);
            }
        }

        if (ico) {
            Image_ptr img = std::make_shared<Image_impl>(ico, true);
            list_->insert(rec.br, img, -1, true);
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
            for (unsigned & n: holder_->indice) {
                Rec & rec = holder_->recs[n];

                if (rec.fi.is_dir()) {
                    show_record(rec);
                    // std::cout << " shown " << ++nn << " of " << holder_->recs.size() << ": " << rec.name << '\n';
                }
            }

            if (!dirs_only_visible_) {
                for (unsigned & n: holder_->indice) {
                    Rec & rec = holder_->recs[n];

                    if (!rec.fi.is_dir()) {
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
    auto fs = path_list(hol->path);
    hol->recs.clear();

    for (const ustring & name: fs) {
        if ("." != name && ".." != name) {
            ustring path = path_build(hol->path, name);

            try {
                Fileinfo fi(path);
                Rec rec;
                rec.name = name;
                rec.fi = fi;
                rec.hidden = fi.is_hidden();
                hol->recs.push_back(rec);
            }

            catch (...) {}
        }
    }
}

void Navigator_impl::preprocess(Holder & hol) {
    hol.prep = true;

    for (Rec & rec: hol.recs) {
        rec.filtered = false;

        if (!rec.fi.is_dir()) {
            if (!filters_.empty()) {
                rec.filtered = true;

                for (const ustring & pattern: filters_) {
                    if (path_match(pattern, rec.name)) {
                        rec.filtered = false;
                        break;
                    }
                }
            }
        }
    }

    hol.indice.resize(hol.recs.size());
    for (unsigned n = 0; n < hol.indice.size(); ++n) { hol.indice[n] = n; }

    if (NAVIGATOR_INFO_NAME == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r2].name < hol.recs[r1].name; } );
        }

        else {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r1].name < hol.recs[r2].name; } );
        }
    }

    else if (NAVIGATOR_INFO_DATE == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r2].fi.mtime() < hol.recs[r1].fi.mtime(); } );
        }

        else {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r1].fi.mtime() < hol.recs[r2].fi.mtime(); } );
        }
    }

    else if (NAVIGATOR_INFO_BYTES == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r2].fi.bytes() < hol.recs[r1].fi.bytes(); } );
        }

        else {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r1].fi.bytes() < hol.recs[r2].fi.bytes(); } );
        }
    }

    else if ("file" == sort_by_) {
        if (sorted_backward()) {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r2].type < hol.recs[r1].type; } );
        }

        else {
            std::sort(hol.indice.begin(), hol.indice.end(), [hol](unsigned r1, unsigned r2) { return hol.recs[r1].type < hol.recs[r2].type; } );
        }
    }
}

Navigator_impl::Holder * Navigator_impl::find_cached_holder(const ustring & path) {
    Holder * hol = nullptr;
    auto iter = std::find_if(hcache_.begin(), hcache_.end(), [path](Holder * h) { return path == h->path; });
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

void Navigator_impl::set_uri(const ustring & path) {
    try {
        ustring fpath = path_is_absolute(path) ? path : path_build(path_cwd(), path);

        if (!file_is_dir(fpath)) {
            ustring p = path_dirname(fpath);

            if (file_is_dir(p)) {
                new_dir(p);
                if (file_exists(fpath)) { select_name(path_notdir(fpath)); }
            }
        }

        else {
            new_dir(fpath);
        }
    }

    catch (...) {
        new_dir(path_cwd());
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

bool Navigator_impl::on_list_mark_validate(int br) {
    if (!dir_select_allowed_ && !dirs_only_visible_) {
        if (holder_) {
            for (const Rec & rec: holder_->recs) {
                if (rec.br == br) {
                    return rec.fi.is_dir();
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
            for (auto hol: hcache_) { hol->prep = false; }
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
            for (auto hol: hcache_) { hol->prep = false; }
            preprocess(*holder_);
            show_current_dir();
        }
    }
}

bool Navigator_impl::on_paint(Painter pr, const Rect & inval) {
    paint_cx_.drop();
    timer_.start(97);
    return false;
}

void Navigator_impl::on_timer() {
    set_uri(user_path_.empty() ? path_cwd() : user_path_);
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

    for (auto hol: hcache_) { hol->prep = false; }

    if (holder_) {
        preprocess(*holder_);
        show_current_dir();
    }
}

void Navigator_impl::on_file_monitor(unsigned event, const ustring & filename, const ustring & dirname) {
    Holder * holder = nullptr;

    if (holder_ && dirname == holder_->path) {
        holder = holder_;
    }

    for (Holder * h: hcache_) {
        if (h->path == dirname) {
            holder = h;
            break;
        }
    }

    if (holder) {
        if (holder->mon_timer.signal_alarm().empty()) {
            holder->mon_timer.signal_alarm().connect(tau::bind(fun(this, &Navigator_impl::on_file_monitor_timer), dirname));
        }

        holder->mon_timer.start(251);
    }
}

void Navigator_impl::on_file_monitor_timer(const ustring & dirname) {
    Holder * holder = nullptr;

    if (holder_ && dirname == holder_->path) {
        holder = holder_;
    }

    for (Holder * h: hcache_) {
        if (h->path == dirname) {
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

void Navigator_impl::on_unparent() {
    //if (list_) { list_->clear(); clear(); list_.reset(); }
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
        for (auto hol: hcache_) { hol->prep = false; }
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
