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
#include <tau/icon.hh>
#include <navigator-impl.hh>
#include <table-impl.hh>
#include <twins-impl.hh>

namespace tau {

class Fileman_impl: public Twins_impl {
protected:

    Fileman_impl(Fileman_mode fs_type, const ustring & path=ustring());

public:

    static Fileman_ptr create(Fileman_mode fs_type, const ustring & path=ustring());

    ustring uri() const;
    void set_uri(const ustring & dirpath);
    std::vector<ustring> selection() const;
    ustring entry() const;
    Navigator_ptr navigator() { return navi_; }

    void add_filter(const ustring & patterns, const ustring & title=ustring());
    ustring filter() const { return navi_->filter(); }

    void show_info(const ustring & items, char32_t sep=U':');
    void hide_info(const ustring & items, char32_t sep=U':');
    bool info_visible(const ustring & item) const;
    ustring visible_info_items(char32_t sep=U':') const;
    ustring invisible_info_items(char32_t sep=U':') const;

    void allow_overwrite();
    void disallow_overwrite();
    bool overwrite_allowed() const { return overwrite_allowed_; }

    void load_state(Key_file & kf, Key_section & sect);
    void save_state(Key_file & kf, Key_section & sect);

    Action & apply_action() { return user_apply_action_; }
    const Action & apply_action() const { return user_apply_action_; }
    Action & cancel_action() { return cancel_action_; }
    const Action & cancel_action() const { return cancel_action_; }

protected:

    Fileman_mode            mode_;
    Table_ptr               table_;
    List_ptr                places_list_;           // Places list.
    Cycle_ptr               filters_;
    bool                    places_visible_ = true; // Places visible by default.
    bool                    dir_creation_allowed_;  // Directory creation allowed.
    bool                    overwrite_allowed_ = false; // Allow overwrite without a prompt.
    std::vector<ustring>    removables_;

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

    Action                  user_apply_action_ { KC_NONE, KM_NONE };
    Action                  apply_action_ { KC_NONE, KM_NONE, "Apply", ICON_DIALOG_OK, fun(this, &Fileman_impl::on_apply) };
    Action                  configure_action_ { KC_NONE, KM_NONE, "Configure", ICON_CONFIGURE, "Configure file open widget", fun(this, &Fileman_impl::on_configure) };
    Action                  cancel_action_ { "Escape Cancel", "Cancel", ICON_DIALOG_CANCEL, fun(this, &Fileman_impl::on_cancel) };
    Action                  next_action_ { KC_RIGHT, KM_ALT, "Next Folder", ICON_GO_NEXT, "Go to next folder", fun(this, &Fileman_impl::next) };
    Action                  prev_action_ { KC_LEFT, KM_ALT, "Previous Folder", ICON_GO_PREVIOUS, "Go to previous folder", fun(this, &Fileman_impl::prev) };
    Action                  updir_action_ { KC_UP, KM_ALT, "Up Folder", ICON_GO_UP, "Go to parent folder", fun(this, &Fileman_impl::updir) };
    Action                  refresh_action_ { "F5", "Refresh", ICON_VIEW_REFRESH, ustring("Reload current folder") };
    Action                  mkdir_action_ { "F7", "Create Directory", ICON_FOLDER_NEW, "Create a new folder", fun(this, &Fileman_impl::on_mkdir) };
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
    void show_places();
    void hide_places();

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
