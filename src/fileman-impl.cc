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

#include <tau/exception.hh>
#include <tau/key-file.hh>
#include <tau/sys.hh>
#include <tau/painter.hh>
#include <button-impl.hh>
#include <check-impl.hh>
#include <cycle-text-impl.hh>
#include <display-impl.hh>
#include <entry-impl.hh>
#include <fileman-impl.hh>
#include <frame-impl.hh>
#include <icon-impl.hh>
#include <list-impl.hh>
#include <loop-impl.hh>
#include <menu-impl.hh>
#include <roller-impl.hh>
#include <scroller-impl.hh>
#include <separator-impl.hh>
#include <slider-impl.hh>
#include <dialog-impl.hh>
#include <iostream>

namespace tau {

Fileman_impl::Fileman_impl(Fileman_mode fm_mode, const ustring & path):
    Twins_impl(OR_WEST, 0.75),
    mode_(fm_mode),
    navi_(std::make_shared<Navigator_impl>(path))
{
    auto loop = Loop_impl::this_loop();

    // List removables.
    for (const ustring & s: loop->mounts()) {
        if (Fileinfo(s).is_removable()) {
            removables_.push_back(s);
        }
    }

    loop->signal_mount().connect(fun(this, &Fileman_impl::on_mount));

    dir_creation_allowed_ = (FILEMAN_OPEN != mode_);

    table_ = std::make_shared<Table_impl>();
    table_->hint_margin(3, 3, 0, 0);
    table_->set_row_spacing(3);
    table_->set_column_margin(1, 0, 3);
    table_->set_row_margin(2, 0, 3);
    insert_first(table_);

    // Path buttons.
    Roller_ptr roller = std::make_shared<Roller_impl>(OR_RIGHT);
    table_->put(roller, 0, -1, 4, 1, false, true);
    pathbox_ = std::make_shared<Box_impl>(OR_RIGHT);
    roller->insert(pathbox_);

    // Navigator inside of frame.
    navi_->signal_dir_changed().connect(fun(this, &Fileman_impl::on_dir_changed));
    navi_->signal_file_select().connect(fun(this, &Fileman_impl::on_file_select));
    navi_->signal_file_unselect().connect(fun(this, &Fileman_impl::on_file_unselect));
    navi_->signal_file_activate().connect(fun(this, &Fileman_impl::on_file_activate));
    navi_->hint_margin(2, 0, 2, 2);
    Frame_ptr frame = std::make_shared<Frame_impl>(BORDER_INSET);
    frame->insert(navi_);
    table_->put(frame, 0, 0, 3, 1, false, false);

    // File name or path entry.
    Text_ptr tp = std::make_shared<Text_impl>("Name:", tau::ALIGN_END);
    tp->hint_margin(3, 3, 0, 0);
    table_->put(tp, 0, 1, 1, 1, true, true);
    table_->align(tp.get(), ALIGN_END, ALIGN_CENTER);
    entry_ = std::make_shared<Entry_impl>();
    table_->put(entry_, 1, 1, FILEMAN_BROWSE == mode_ ? 2 : 1, 1, false, true);
    entry_->signal_activate().connect(fun(this, &Fileman_impl::on_entry_activate));
    entry_->signal_changed().connect(fun(this, &Fileman_impl::on_entry_changed));
    entry_->signal_mouse_down().connect(fun(this, &Fileman_impl::on_entry_mouse_down));

    // Filters.
    tp = std::make_shared<Text_impl>("Filter:", tau::ALIGN_END);
    tp->hint_margin(3, 3, 0, 0);
    table_->put(tp, 0, 2, 1, 1, true, true);
    table_->align(tp.get(), ALIGN_END, ALIGN_CENTER);
    filters_ = std::make_shared<Cycle_impl>();
    table_->put(filters_, 1, 2, FILEMAN_BROWSE == mode_ ? 2 : 1, 1, false, true);
    add_filter("*", "All Files");

    // Buttons "Open"/"Save" & "Cancel".
    if (FILEMAN_BROWSE != mode_) {
        apply_action_.set_label(FILEMAN_SAVE == mode_ ? "Save" : "Load");
        Button_ptr button = std::make_shared<Button_impl>(apply_action_);
        table_->put(button, 2, 1, 1, 1, true, true);
        table_->align(button.get(), ALIGN_FILL, ALIGN_CENTER);

        button = std::make_shared<Button_impl>(cancel_action_);
        table_->put(button, 2, 2, 1, 1, true, true);
        table_->align(button.get(), ALIGN_FILL, ALIGN_CENTER);
    }

    auto tools = std::make_shared<Box_impl>(OR_DOWN, 4);
    tools->hint_margin(2, 2, 0, 0);
    table_->put(tools, 3, 0, 1, 1, true, false);

    up_button_ = std::make_shared<Button_impl>(updir_action_, ACTION_NO_LABEL);
    tools->append(up_button_, true);
    tools->append(std::make_shared<Separator_impl>(), true);
    tools->append(std::make_shared<Button_impl>(prev_action_, ACTION_NO_LABEL), true);
    tools->append(std::make_shared<Button_impl>(next_action_, ACTION_NO_LABEL), true);
    tools->append(std::make_shared<Separator_impl>(), true);
    tools->append(std::make_shared<Button_impl>(refresh_action_, ACTION_NO_LABEL), true);
    tools->append(std::make_shared<Toggle_impl>(hidden_action_, ACTION_NO_LABEL), true);
    tools->append(std::make_shared<Button_impl>(mkdir_action_, ACTION_NO_LABEL), true);
    tools->append(std::make_shared<Separator_impl>(), true);
    conf_button_ = std::make_shared<Button_impl>(configure_action_, ACTION_NO_LABEL);
    tools->append(conf_button_, true);

    prev_action_.disable();
    next_action_.disable();
    apply_action_.disable();
    if (!dir_creation_allowed_) { mkdir_action_.disable(); mkdir_action_.hide(); }
    refresh_action_.connect(fun(navi_, &Navigator_impl::refresh));

    connect_action(configure_action_);
    connect_action(cancel_action_);
    connect_action(next_action_);
    connect_action(prev_action_);
    connect_action(updir_action_);
    connect_action(apply_action_);
    connect_action(refresh_action_);
    connect_action(mkdir_action_);
    connect_action(hidden_action_);

    signal_display_.connect(fun(this, &Fileman_impl::on_display));
    signal_take_focus_.connect(fun(navi_, &Widget_impl::take_focus), true);
}

void Fileman_impl::on_display() {
    if (places_visible_) {
        init_places();
        fill_places();
    }
}

ustring Fileman_impl::uri() const {
    return navi_->uri();
}

void Fileman_impl::set_uri(const ustring & uri) {
    add_to_history(uri);
    navi_->set_uri(uri);
}

ustring Fileman_impl::entry() const {
    return entry_->text();
}

std::vector<ustring> Fileman_impl::selection() const {
    return selection_;
}

void Fileman_impl::on_file_activate(const ustring & path) {
    selection_.clear();
    selection_.push_back(path_notdir(path));
    apply();
}

void Fileman_impl::entry_from_selection() {
    if (selection_.size() > 1) {
        ustring s;

        for (const ustring & name: selection_) {
            s += '"';
            s += name;
            s += "\" ";
        }

        entry_->assign(s);
    }

    else {
        if (selection_.empty()) {
            entry_->clear();
        }

        else {
            entry_->assign(selection_.front());
        }
    }
}

void Fileman_impl::on_file_select(const ustring & filename) {
    if (navi_->multiple_select_allowed()) {
        auto iter = std::find(selection_.begin(), selection_.end(), filename);
        if (iter == selection_.end()) { selection_.push_back(filename); }
    }

    else {
        selection_.clear();
        selection_.push_back(filename);
    }

    ustring p = path_build(navi_->uri(), filename);

    if (file_is_dir(p) && !navi_->dir_select_allowed()) { apply_action_.disable(); }
    else { apply_action_.enable(); }

    entry_from_selection();
    entry_->move_to(entry_->buffer().cend());
}

void Fileman_impl::on_file_unselect(const ustring & filename) {
    if (navi_->multiple_select_allowed()) {
        auto iter = std::find(selection_.begin(), selection_.end(), filename);
        if (iter != selection_.end()) { selection_.erase(iter); }
    }

    entry_from_selection();
}

void Fileman_impl::on_dir_changed(const ustring & path) {
    entry_->clear();
    apply_action_.disable();
    pathbox_->clear();

    if (up_button_) {
        ustring pdir = path_dirname(path);
        if (pdir.empty() || pdir == path) { up_button_->disable(); }
        else { up_button_->enable(); }
    }

    if (prev_avail()) { prev_action_.enable(); }
    else { prev_action_.disable(); }

    if (next_avail()) { next_action_.enable(); }
    else { next_action_.disable(); }

    if (!path.empty()) {
        ustring p = path, q;

        for (;;) {
            Button_ptr button = std::make_shared<Button_impl>(path_notdir(p));
            button->hint_min_size(Size(14, 0));
            button->signal_click().connect(tau::bind(fun(this, &Fileman_impl::set_uri), p));
            button->hide_relief();
            pathbox_->prepend(button, true);
            q = path_dirname(p);
            if (q == p) { break; }
            pathbox_->prepend(std::make_shared<Text_impl>(ustring(1, path_slash_)), true);
            p = q;
        }
    }
}

void Fileman_impl::apply() {
    if (FILEMAN_SAVE == mode_) {
        if (!overwrite_allowed_) {
            ustring path = path_build(uri(), entry_->text());

            if (file_exists(path)) {
                if (auto dp = display()) {
                    if (auto toplevel = root()) {
                        Box_ptr box = std::make_shared<Box_impl>(OR_DOWN, 5);
                        box->set_align(ALIGN_CENTER);
                        box->hint_margin(8);

                        Text_ptr tp = std::make_shared<Text_impl>(str_format("File ", entry_->text(), " exist."));
                        box->append(tp, true);

                        tp = std::make_shared<Text_impl>("Are your sure to overwrite it?");
                        box->append(tp, true);

                        Box_ptr bbox = std::make_shared<Box_impl>(OR_RIGHT, 12);
                        bbox->set_align(ALIGN_CENTER);
                        bbox->hint_margin(0, 0, 10, 0);
                        box->append(bbox, true);

                        Button_ptr yes = std::make_shared<Button_impl>("Yes", "dialog-ok", MEDIUM_ICON);
                        yes->signal_click().connect(fun(yes, &Widget_impl::quit_dialog));
                        yes->signal_click().connect(fun(this, &Widget_impl::quit_dialog));
                        yes->signal_click().connect(fun(signal_apply_));
                        bbox->append(yes, true);

                        Button_ptr no = std::make_shared<Button_impl>("No", "dialog-cancel", MEDIUM_ICON);
                        no->signal_click().connect(fun(yes, &Widget_impl::quit_dialog));
                        bbox->append(no, true);

                        auto dlg = dp->create_dialog(toplevel);
                        dlg->set_title("Confirm File Overwrite");
                        dlg->insert(box);
                        dlg->show();
                        dlg->run();
                        return;
                    }
                }
            }
        }
    }

    quit_dialog();
    signal_apply_();
}

void Fileman_impl::on_apply() {
    if (FILEMAN_OPEN == mode_) {
        if (!selection_.empty()) {
            apply();
        }
    }

    else {
        if (!entry_->empty()) {
            apply();
        }
    }
}

void Fileman_impl::on_cancel() {
    entry_->clear();
    selection_.clear();
    quit_dialog();
}

void Fileman_impl::on_entry_changed(const ustring & s) {
    if (selection_.size() > 1) {
        apply_action_.enable();
    }

    else {
        ustring p = path_build(uri(), s);

        if (("." == s || ".." == s || ustring::npos != s.find_first_of("/\\")) || s.empty()
            || ((!navi_->dir_select_allowed()) && file_is_dir(p)))
        {
            apply_action_.disable();
        }

        else {
            if (FILEMAN_OPEN == mode_) {
                if (!file_exists(p)) {
                    apply_action_.disable();
                }

                else {
                    if (!navi_->multiple_select_allowed()) {
                        auto iter = std::find(selection_.begin(), selection_.end(), s);
                        if (selection_.end() == iter) { selection_.push_back(s); }
                    }

                    apply_action_.enable();
                }
            }

            else {
                apply_action_.enable();
            }
        }
    }
}

void Fileman_impl::on_entry_activate(const ustring & s) {
    if (!s.empty()) {
        if ("." == s || ".." == s || ustring::npos != s.find_first_of("/\\")) {
            ustring p = path_is_absolute(s) ? s : path_real(path_build(uri(), s));
            if (file_exists(p)) { entry_->clear(); set_uri(p); }
        }

        else {
            ustring p = path_build(uri(), s);

            if (file_is_dir(p)) {
                if (navi_->dir_select_allowed()) {
                    apply();
                }

                else {
                    entry_->clear();
                    set_uri(p);
                }
            }

            else {
                if (FILEMAN_OPEN == mode_) {
                    if (file_exists(p)) {
                        if (!navi_->multiple_select_allowed()) {
                            auto iter = std::find(selection_.begin(), selection_.end(), s);
                            if (selection_.end() == iter) { selection_.push_back(s); }
                        }

                        apply();
                    }
                }

                else if (FILEMAN_SAVE == mode_) {
                    apply();
                }
            }
        }
    }
}

bool Fileman_impl::on_entry_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        if (!entry_->empty()) {
            entry_->move_to(entry_->buffer().cend());
            entry_->select_all();
        }

        return true;
    }

    return false;
}

void Fileman_impl::on_mkdir() {
    if (auto dp = display()) {
        if (auto toplevel = root()) {
            Box_ptr box = std::make_shared<Box_impl>(OR_DOWN);
            box->hint_margin(8);
            box->set_align(ALIGN_CENTER);

            Text_ptr tp = std::make_shared<Text_impl>("Create new folder in:", ALIGN_START, ALIGN_CENTER);
            box->append(tp, true);

            tp = std::make_shared<Text_impl>(uri(), ALIGN_START, ALIGN_CENTER);
            box->append(tp, true);

            Entry_ptr ent = std::make_shared<Entry_impl>();
            box->append(ent, true);
            ent->cancel_action().connect(fun(ent, &Widget_impl::quit_dialog));
            ent->signal_activate().connect(tau::bind(fun(this, &Fileman_impl::on_mkdir_activate), ent.get()));
            ent->signal_changed().connect(fun(this, &Fileman_impl::on_mkdir_changed));

            Box_ptr bbox = std::make_shared<Box_impl>(OR_RIGHT, 12);
            bbox->set_align(ALIGN_CENTER);
            bbox->hint_margin(0, 0, 10, 4);
            box->append(bbox, true);

            mkdir_ok_button_ = std::make_shared<Button_impl>("OK", "dialog-ok", SMALL_ICON);
            bbox->append(mkdir_ok_button_, true);
            mkdir_ok_button_->signal_click().connect(tau::bind(fun(this, &Fileman_impl::on_mkdir_apply), ent.get()));
            mkdir_ok_button_->disable();

            Button_ptr cancel_button = std::make_shared<Button_impl>("Cancel", "dialog-cancel", SMALL_ICON);
            bbox->append(cancel_button, true);
            cancel_button->signal_click().connect(fun(cancel_button, &Widget_impl::quit_dialog));

            auto dlg = dp->create_dialog(toplevel);
            dlg->set_title("Create Folder");
            dlg->insert(box);
            dlg->show();
            ent->take_focus();
            dlg->run();
            mkdir_ok_button_.reset();
        }
    }
}

void Fileman_impl::mkdir(const ustring & path) {
    if (!path.empty()) {
        try {
            path_mkdir(path);
            set_uri(path);
            entry_->clear();
        }

        catch (tau::exception & x) {
            std::cerr << "** Fileman_impl::mkdir(): tau::exception thrown: " << x.what() << std::endl;
        }
    }
}

void Fileman_impl::on_mkdir_apply(Entry_impl * entry) {
    ustring path = path_build(uri(), entry->text());
    mkdir(path);
    entry->quit_dialog();
}

void Fileman_impl::on_mkdir_activate(const ustring & dirname, Entry_impl * entry) {
    ustring path = path_build(uri(), dirname);
    mkdir(path);
    entry->quit_dialog();
}

void Fileman_impl::on_mkdir_changed(const ustring & s) {
    if (s.empty() || ".." == s || "." == s || "../" == s || "./" == s || "..\\" == s || ".\\" == s) {
        mkdir_ok_button_->disable();
    }

    else {
        ustring p = path_build(uri(), s);
        if (file_exists(p)) { mkdir_ok_button_->disable(); }
        else { mkdir_ok_button_->enable(); }
    }
}

void Fileman_impl::init_places() {
    auto frame = std::make_shared<Frame_impl>(BORDER_GROOVE);
    frame->hint_margin_right(3);
    insert_second(frame);

    Box_ptr vbox = std::make_shared<Box_impl>(OR_DOWN, 3);
    vbox->hint_margin(3);
    frame->insert(vbox);

    Box_ptr hbox = std::make_shared<Box_impl>(OR_RIGHT, 8);
    vbox->append(hbox, true);
    hbox->append(std::make_shared<Text_impl>("Places", ALIGN_START, ALIGN_CENTER));

    Button_ptr button = std::make_shared<Button_impl>(std::make_shared<Icon_impl>("picto-close", 12));
    button->hide_relief();
    button->signal_click().connect(fun(this, &Fileman_impl::hide_places));
    hbox->append(button, true);

    places_list_ = std::make_shared<List_impl>();
    vbox->append(places_list_);
}

void Fileman_impl::show_places() {
    places_visible_ = true;

    if (!has_second()) {
        init_places();
        fill_places();
    }
}

void Fileman_impl::hide_places() {
    remove_second();
    places_visible_ = false;
    places_list_.reset();
}

void Fileman_impl::on_mount(int flags, const ustring & mp) {
    if (FILE_UMOUNT & flags) {
        if (str_similar(mp, removables_)) {
            removables_.erase(std::find(removables_.begin(), removables_.end(), mp));
            fill_places();
        }
    }

    else if (FILE_MOUNT & flags) {
        if (Fileinfo(mp).is_removable()) {
            removables_.push_back(mp);
            fill_places();
        }
    }
}

void Fileman_impl::add_filter(const ustring & patterns, const ustring & title) {
    if (filters_->empty()) { navi_->set_filter(patterns); }
    ustring s = title.empty() ? patterns : str_format(patterns, " – ", title);
    Text_ptr tp = std::make_shared<Text_impl>(s, ALIGN_START);
    tp->signal_select().connect(tau::bind(fun(navi_, &Navigator_impl::set_filter), patterns));
    filters_->add(tp);
}

void Fileman_impl::allow_overwrite() {
    overwrite_allowed_ = true;
}

void Fileman_impl::disallow_overwrite() {
    overwrite_allowed_ = false;
}

void Fileman_impl::on_configure() {
    Menubox_ptr menu = std::make_shared<Menubox_impl>();
    Menubox_ptr sort_menu = std::make_shared<Menubox_impl>();
    Submenu_ptr sort_item = std::make_shared<Submenu_impl>("Sort", sort_menu);
    menu->append(sort_item);

    Check_menu_ptr sort_name = std::make_shared<Check_menu_impl>("By Name", CHECK_RSTYLE, "name" == navi_->sorted_by());
    sort_name->signal_check().connect(tau::bind(fun(navi_, &Navigator_impl::sort_by), "name"));
    sort_menu->append(sort_name);

    Check_menu_ptr sort_size = std::make_shared<Check_menu_impl>("By Size", CHECK_RSTYLE, "bytes" == navi_->sorted_by());
    sort_size->join(sort_name);
    sort_size->signal_check().connect(tau::bind(fun(navi_, &Navigator_impl::sort_by), "bytes"));
    sort_menu->append(sort_size);

    Check_menu_ptr sort_date = std::make_shared<Check_menu_impl>("By Date", CHECK_RSTYLE, "date" == navi_->sorted_by());
    sort_date->join(sort_name);
    sort_date->signal_check().connect(tau::bind(fun(navi_, &Navigator_impl::sort_by), "date"));
    sort_menu->append(sort_date);

    Check_menu_ptr sort_unsorted = std::make_shared<Check_menu_impl>("Keep Unsorted", CHECK_RSTYLE, "" == navi_->sorted_by());
    sort_unsorted->join(sort_name);
    sort_unsorted->signal_check().connect(tau::bind(fun(navi_, &Navigator_impl::sort_by), ""));
    sort_menu->append(sort_unsorted);

    Menubox_ptr columns_menu = std::make_shared<Menubox_impl>();
    Submenu_ptr columns_item = std::make_shared<Submenu_impl>("Columns", columns_menu);
    menu->append(columns_item);

    Check_menu_ptr columns_size = std::make_shared<Check_menu_impl>("Show File Size", CHECK_VSTYLE, info_visible("bytes"));
    columns_size->signal_check().connect(tau::bind(fun(this, &Fileman_impl::show_info), "bytes", U':'));
    columns_size->signal_uncheck().connect(tau::bind(fun(this, &Fileman_impl::hide_info), "bytes", U':'));
    columns_menu->append(columns_size);

    Check_menu_ptr columns_date = std::make_shared<Check_menu_impl>("Show File Date", CHECK_VSTYLE, info_visible("date"));
    columns_date->signal_check().connect(tau::bind(fun(this, &Fileman_impl::show_info), "date", U':'));
    columns_date->signal_uncheck().connect(tau::bind(fun(this, &Fileman_impl::hide_info), "date", U':'));
    columns_menu->append(columns_date);

    sort_menu->append_separator();

    Check_menu_ptr sort_back = std::make_shared<Check_menu_impl>("Sort Descent", CHECK_VSTYLE, navi_->sorted_backward());
    sort_back->signal_check().connect(fun(navi_, &Navigator_impl::sort_backward));
    sort_back->signal_uncheck().connect(fun(navi_, &Navigator_impl::sort_forward));
    sort_menu->append(sort_back);

    menu->append_separator();

    Check_menu_ptr show_hidden = std::make_shared<Check_menu_impl>("Show Hidden Files", CHECK_VSTYLE, info_visible("hidden"));
    show_hidden->signal_check().connect(tau::bind(fun(this, &Fileman_impl::on_show_hidden), true));
    show_hidden->signal_uncheck().connect(tau::bind(fun(this, &Fileman_impl::on_show_hidden), false));
    menu->append(show_hidden);

    Check_menu_ptr show_places = std::make_shared<Check_menu_impl>("Show Places", CHECK_VSTYLE, places_visible_);
    show_places->signal_check().connect(fun(this, &Fileman_impl::show_places));
    show_places->signal_uncheck().connect(fun(this, &Fileman_impl::hide_places));
    menu->append(show_places);

    if (root()) {
        Point pos = conf_button_->to_root();
        menu->popup(root(), menu, pos, GRAVITY_TOP_RIGHT, nullptr);
    }
}

bool Fileman_impl::next_avail() const {
    return 1+ihistory_ < history_.size();
}

bool Fileman_impl::prev_avail() const {
    return ihistory_ > 0;
}

void Fileman_impl::add_to_history(const ustring & path) {
    for (unsigned n = ihistory_; n < history_.size(); ++n) { history_.pop_back(); }
    history_.push_back(path);
    ihistory_ = history_.size();
}

void Fileman_impl::next() {
    if (1+ihistory_ < history_.size()) {
        bool p_avail = prev_avail();
        ++ihistory_;
        navi_->set_uri(history_[ihistory_]);
        if (!next_avail()) { next_action_.disable(); }
        if (!p_avail && prev_avail()) { prev_action_.enable(); }
    }
}

void Fileman_impl::prev() {
    if (ihistory_ > 0) {
        bool n_avail = next_avail();
        --ihistory_;
        if (ihistory_ < history_.size()) { navi_->set_uri(history_[ihistory_]); }
        if (!prev_avail()) { prev_action_.disable(); }
        if (!n_avail && next_avail()) { next_action_.enable(); }
    }
}

void Fileman_impl::updir() {
    ustring p = path_dirname(navi_->uri());
    if (p != uri()) { set_uri(p); }
}

void Fileman_impl::on_show_hidden(bool show) {
    if (show) { show_info("hidden"); }
    else { hide_info("hidden"); }
}

void Fileman_impl::load_state(Key_file & kf, Key_section & sect) {
    show_info(kf.get_string(sect, "visible_info_items"), kf.list_separator());
    hide_info(kf.get_string(sect, "invisible_info_items"), kf.list_separator());
    navi_->sort_by(kf.get_string(sect, "sort_by", "name"));
    set_ratio(kf.get_double(sect, "ratio", ratio()));
    if (kf.get_boolean(sect, "sort_backward")) { navi_->sort_backward(); }
}

void Fileman_impl::save_state(Key_file & kf, Key_section & sect) {
    kf.set_string(sect, "visible_info_items", visible_info_items(kf.list_separator()));
    kf.set_string(sect, "invisible_info_items", invisible_info_items(kf.list_separator()));
    kf.set_string(sect, "sort_by", navi_->sorted_by());
    kf.set_boolean(sect, "sort_backward", navi_->sorted_backward());
    kf.set_double(sect, "ratio", ratio());
}

void Fileman_impl::show_info(const ustring & items, char32_t sep) {
    for (const ustring & s: str_explode(items, sep)) {
        if (str_similar("hidden", s)) { hidden_action_.set(true); }
        else if (str_similar("places", s)) { show_places(); }
    }

    navi_->show_info(items, sep);
}

void Fileman_impl::hide_info(const ustring & items, char32_t sep) {
    for (const ustring & s: str_explode(items, sep)) {
        if (str_similar("hidden", s)) { hidden_action_.set(false); }
        else if (str_similar("places", s)) { hide_places(); }
    }

    navi_->hide_info(items, sep);
}

bool Fileman_impl::info_visible(const ustring & item) const {
    if (str_similar(item, "places")) { return places_visible_; }
    return navi_->info_visible(item);
}

ustring Fileman_impl::visible_info_items(char32_t sep) const {
    ustring s = navi_->visible_info_items(sep);
    if (places_visible_) { s += sep; s += "places"; }
    return s;
}

ustring Fileman_impl::invisible_info_items(char32_t sep) const {
    ustring s = navi_->invisible_info_items(sep);
    if (!places_visible_) { s += sep; s += "places"; }
    return s;
}

} // namespace tau

//END
