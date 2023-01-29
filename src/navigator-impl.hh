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

#ifndef TAU_NAVIGATOR_IMPL_HH
#define TAU_NAVIGATOR_IMPL_HH

#include <tau/string.hh>
#include <tau/fileinfo.hh>
#include <bin-impl.hh>

namespace tau {

class Navigator_impl: public Bin_impl {
public:

    Navigator_impl(const ustring & uri=ustring());
   ~Navigator_impl();

    ustring uri() const;
    void set_uri(const ustring & uri);
    void refresh();

    void sort_by(const ustring & col);
    ustring sorted_by() const { return sort_by_; }

    void sort_forward();
    void sort_backward();

    bool sorted_backward() const {
        return sort_backward_;
    }

    void hidden_visible_files();
    void hide_hidden_files();

    bool hidden_files_visible() const {
        return hidden_visible_;
    }

    void show_info(const ustring & items, char32_t sep=U':');
    void hide_info(const ustring & items, char32_t sep=U':');
    bool info_visible(const ustring & item) const;
    ustring visible_info_items(char32_t sep=U':') const;
    ustring invisible_info_items(char32_t sep=U':') const;

    void allow_multiple_select();
    void disallow_multiple_select();

    bool multiple_select_allowed() const {
        return multiple_select_allowed_;
    }

    void allow_dir_select();
    void disallow_dir_select();

    bool dir_select_allowed() const {
        return dir_select_allowed_;
    }

    void set_filter(const ustring & patterns);

    ustring filter() const {
        return str_implode(filters_, ',');
    }

    signal<void(const ustring &)> & signal_file_select() {
        return signal_file_select_;
    }

    signal<void(const ustring &)> & signal_file_unselect() {
        return signal_file_unselect_;
    }

    signal<void(const ustring &)> & signal_file_activate() {
        return signal_file_activate_;
    }

    signal<void(const ustring &)> & signal_dir_changed() {
        return signal_dir_changed_;
    }

private:

    // The record holding single file information.
    struct Rec {
        ustring                 name;
        ustring                 type;
        Fileinfo                fi;
        bool                    hidden = false;
        bool                    filtered = false;
        int                     br = INT_MIN;
    };

    using Recs = std::vector<Rec>;

    // The structure holding directory entry information.
    struct Holder {
        ustring                 path;
        Recs                    recs;           // File/dir records.
        std::vector<unsigned>   indice;         // Index table.
        bool                    prep = false;   // Preprocess (filter & sort) done.
        Fileinfo                finfo;
        Timer                   mon_timer;      // File monitor timer.
    };

    using Holders = std::list<Holder *>;

    List_ptr                    list_;                  // File panel as list.
    Holder *                    holder_ = nullptr;
    Holders                     hcache_;
    ustring                     user_path_;
    std::vector<ustring>        filters_;
    Timer                       timer_ { fun(this, &Navigator_impl::on_timer) };
    Pixmap_cptr                 dir_icon_;
    Pixmap_cptr                 unknown_icon_;

    ustring                     sort_by_ = "name";
    bool                        sort_backward_ = false;
    bool                        multiple_select_allowed_ = false;
    bool                        dir_select_allowed_ = false;
    bool                        dirs_only_visible_ = false;
    bool                        hidden_visible_ = false;
    bool                        bytes_visible_ = true;
    bool                        date_visible_ = true;

    connection                  paint_cx_;

    signal<void(const ustring &)> signal_file_select_;
    signal<void(const ustring &)> signal_file_unselect_;
    signal<void(const ustring &)> signal_file_activate_;
    signal<void(const ustring &)> signal_dir_changed_;

private:

    void new_dir(const ustring & path);
    void show_record(Rec & rec);
    void read_dir(Holder * hol);
    void preprocess(Holder & hol);
    void show_current_dir();
    int  find_row(const ustring & name);
    ustring name_from_row(int br);
    ustring format_file_time(const Fileinfo & fi) const;
    Holder * find_cached_holder(const ustring & path);
    void select_name(const ustring & name);
    void limit_name_column();

    bool on_paint(Painter pr, const Rect & inval);
    void on_timer();
    void on_file_select(const ustring & filename);
    void on_file_unselect(const ustring & filename);
    void on_list_activate(int br);
    bool on_list_mark_validate(int br);
    void on_list_header_click(int column);
    void on_list_header_width_changed(int column);
    void on_file_monitor(unsigned event, const ustring & filename, const ustring & dirname);
    void on_file_monitor_timer(const ustring & dirname);
    void on_unparent();
};

} // namespace tau

#endif //TAU_NAVIGATOR_IMPL_HH
