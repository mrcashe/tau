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

#ifndef TAU_FILE_SELECTOR_IMPL_HH
#define TAU_FILE_SELECTOR_IMPL_HH

#include <tau/action.hh>
#include <navigator-impl.hh>
#include <table-impl.hh>
#include <twins-impl.hh>

namespace tau {

class Fileman_impl: public Twins_impl {
protected:

    Fileman_impl(Fileman_mode fs_type, const ustring & path=ustring());

public:

    static Fileman_ptr create(Fileman_mode fs_type, const ustring & path=ustring());

    ustring dir() const;
    void chdir(const ustring & dirpath);
    std::vector<ustring> selection() const;
    ustring entry() const;

    void show_places();
    void hide_places();
    bool places_visible() const { return places_visible_; }

    void add_filter(const ustring & patterns, const ustring & title=ustring());
    ustring filter() const { return navi_->filter(); }

    void sort_by(const ustring & col) { navi_->sort_by(col); }
    ustring sorted_by() const { return navi_->sorted_by(); }
    void show_info(const ustring & items, char32_t sep=U':') { navi_->show_info(items, sep); }
    void hide_info(const ustring & items, char32_t sep=U':') { navi_->hide_info(items, sep); }
    bool info_visible(const ustring & item) const { return navi_->info_visible(item); }
    ustring visible_info_items(char32_t sep=U':') const { return navi_->visible_info_items(sep); }
    ustring invisible_info_items(char32_t sep=U':') const { return navi_->invisible_info_items(sep); }

    void sort_forward();
    void sort_backward();
    bool sorted_backward() const;

    void show_hidden_files();
    void hide_hidden_files();
    bool hidden_files_visible() const;

    void allow_multiple_select();
    void disallow_multiple_select();
    bool multiple_select_allowed() const;

    void allow_dir_select();
    void disallow_dir_select();
    bool dir_select_allowed() const;

    void set_show_dirs_only();
    void unset_show_dirs_only();
    bool dirs_only_visible() const;

    void allow_dir_creation();
    void disallow_dir_creation();
    bool dir_creation_allowed() const { return dir_creation_allowed_; }

    void allow_silent_overwrite();
    void disallow_silent_overwrite();
    bool silent_overwrite_allowed() const { return silent_overwrite_allowed_; }

    Action & cancel_action() { return cancel_action_; }

    signal<void()> & signal_apply() { return signal_apply_; }

protected:

    Fileman_mode            fm_mode_;
    Table_ptr               table_;
    List_ptr                places_list_;           // Places list.
    Cycle_ptr               filters_;
    bool                    places_visible_ = true; // Places visible by default.
    bool                    dir_creation_allowed_;  // Directory creation allowed.
    bool                    silent_overwrite_allowed_ = false; // Allow overwrite without a prompt.

    signal<void()>          signal_apply_;

protected:

    // Platform depended.
    virtual void fill_places() = 0;

private:

    using History = std::vector<ustring>;

    Box_ptr                 pathbox_;               // Path selection buttons here.
    std::vector<ustring>    selection_;             // Currently selected paths.
    Navigator_ptr           navi_;
    Entry_ptr               entry_;
    History                 history_;
    std::size_t             ihistory_ = 0;

    Button_ptr              up_button_;
    Button_ptr              conf_button_;
    Button_ptr              mkdir_ok_button_;

    Action                  configure_action_ { KC_NONE, KM_NONE, "Configure", "configure", "Configure file open widget", fun(this, &Fileman_impl::on_configure) };
    Action                  apply_action_ { KC_NONE, KM_NONE, "Apply", "dialog-ok", fun(this, &Fileman_impl::on_apply) };
    Action                  cancel_action_ { "Escape Cancel", "Cancel", "dialog-cancel", fun(this, &Fileman_impl::on_cancel) };
    Action                  next_action_ { KC_RIGHT, KM_ALT, "Next Folder", "go-next:go-right", "Go to next folder", fun(this, &Fileman_impl::next) };
    Action                  prev_action_ { KC_LEFT, KM_ALT, "Previous Folder", "go-previous:go-left", "Go to previous folder", fun(this, &Fileman_impl::prev) };
    Action                  updir_action_ { KC_UP, KM_ALT, "Up Folder", "go-up", "Go to parent folder", fun(this, &Fileman_impl::updir) };
    Action                  refresh_action_ { "F5", "Refresh", "view-refresh", ustring("Reload current folder") };
    Action                  mkdir_action_ { "F7", "Create Directory", "folder-new:folder", "Create a new folder", fun(this, &Fileman_impl::on_mkdir) };
    Toggle_action           hidden_action_ { "<Alt>.", "Show Hidden Files", "show-hidden", "Show Hidden Files", fun(this, &Fileman_impl::on_show_hidden) };

private:

    void init_places();
    void entry_from_selection();
    void mkdir(const ustring & path);
    void apply();
    bool next_avail() const;
    bool prev_avail() const;
    void add_to_history(const ustring & path);
    void next();
    void prev();
    void updir();

    void on_file_select(const ustring & filename);
    void on_file_unselect(const ustring & filename);
    void on_file_activate(const ustring & path);
    void on_dir_changed(const ustring & path);
    void on_entry_activate(const ustring & s);
    void on_entry_changed(const ustring & s);
    bool on_entry_mouse_down(int mbt, int mm, const Point & pt);
    void on_display();
    void on_apply();
    void on_cancel();
    void on_mount(int flags, const ustring & mpoint);
    void on_configure();
    void on_mkdir();
    void on_mkdir_apply(Entry_impl * entry);
    void on_mkdir_activate(const ustring & dirname, Entry_impl * entry);
    void on_mkdir_changed(const ustring & dirname);
    void on_show_hidden(bool show);
};

} // namespace tau

#endif //TAU_FILE_SELECTOR_IMPL_HH