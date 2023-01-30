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

#include <tau.hh>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <unordered_map>

namespace {

tau::Key_file               kstate_;
std::vector<tau::ustring>   args_;
int                         line_ = -1;
int                         col_ = -1;
std::vector<long long>      geom_;

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct Main: tau::Toplevel {
    struct Page {
        tau::Table      table;
        tau::Table      tab;
        tau::Text       title { tau::ALIGN_START, tau::ALIGN_CENTER };
        tau::Text       tooltip;
        tau::Edit       edit;
        tau::Box        status_box { tau::OR_RIGHT, 3 };
        tau::Text       row_value { "0" };
        tau::Text       rows_value { "0" };
        tau::Text       col_value { "0" };
        tau::Text       uni_value { "U+0000" };
        tau::Text       enc_label { "UTF-8" };
        tau::Text       insert_label { "INSERT" };
        tau::Text       replace_label { "REPLACE" };
        tau::Action     zin_action;
        tau::Action     zout_action;
        tau::Timer      motion_timer;
        tau::Icon       ico { "window-close", tau::SMALL_ICON };
        tau::Icon       save_ico { "document-save", tau::SMALL_ICON };
        tau::ustring    path;
        tau::connection meta_cx;
        tau::connection encoding_cx;
        tau::connection on_edit_focus_in_cx;
        tau::connection enable_redo_cx;
        tau::connection on_edit_focus_out_cx;
        tau::connection disable_redo_cx;
        std::size_t     lines = 0;
        int             page;
        uint64_t        metaid = 0;
        unsigned        font_size = 0;
        tau::Fileinfo   finfo;
    };

    using Pages = std::list<Page>;

    struct Meta_holder {
        tau::Timeval    atime { 0 };
        tau::ustring    path;
        uint64_t        id = 0;
    };

    using Metas = std::unordered_map<std::string, Meta_holder>;

    tau::Loop           loop_;
    tau::ustring        datadir_;
    Pages               pages_;
    Metas               metas_;
    tau::ustring        font_spec_;
    unsigned            font_size_ = 10;
    unsigned            max_recent_ = 24;
    tau::Pixmap_ptr     save_pix_;
    bool                fileop_in_dialogs_ = false;
    tau::connection     show_cx_;
    tau::connection     session_cx_;
    tau::signal<bool()> signal_modified_;
    tau::Key_file       ksession_;

    tau::Menubar        menubar_;
    tau::Box            toolbar_ { tau::OR_RIGHT, 6 };
    tau::Menubox        recent_menu_;
    tau::Notebook       notebook_ { tau::TAB_TOP };
    tau::Card           card_;
    tau::Widget *       wpop_ = nullptr;

    tau::Action         escape_action_ { "Escape", tau::fun(this, &Main::on_escape) };
    tau::Action         file_quit_action_ { "<Ctrl>Q", "Quit", "application-exit;window-close", tau::fun(this, &Main::on_menu_file_quit) };

    tau::Action         file_new_action_ { "<Ctrl>N", "New", "document-new", tau::fun(this, &Main::on_menu_file_new) };
    tau::Action         file_open_action_ { "<Ctrl>O", "Open", "document-open", "Open a File", tau::fun(this, &Main::on_menu_file_open) };
    tau::Action         file_save_action_ { "<Ctrl>S", "Save", "document-save", "Save a File", tau::fun(this, &Main::on_menu_file_save) };
    tau::Action         file_save_as_action_ { "<Ctrl><Shift>S", "Save As", "document-save-as", "Save the file with\na different name", tau::fun(this, &Main::on_menu_file_save_as) };
    tau::Action         file_save_all_action_ { "<Ctrl>L", "Save All", "document-save-all", tau::fun(this, &Main::on_menu_file_save_all) };
    tau::Action         file_close_action_ { "<Ctrl>W", "Close", "window-close", tau::fun(this, &Main::on_menu_file_close) };
    tau::Action         file_close_all_action_ { 0, 0, "Close All", tau::fun(this, &Main::on_menu_file_close_all) };
    tau::Action         file_close_others_action_ { "", "Close Others", tau::fun(this, &Main::close_others) };

    tau::Master_action  edit_undo_master_action_ { "<Alt>BackSpace", "Undo", "edit-undo" };
    tau::Action         edit_undo_action_ { edit_undo_master_action_, tau::fun(this, &Main::on_menu_edit_undo) };
    tau::Master_action  edit_redo_master_action_ { "<Alt>Enter", "Redo", "edit-redo" };
    tau::Action         edit_redo_action_ { edit_redo_master_action_, tau::fun(this, &Main::on_menu_edit_redo) };
    tau::Action         edit_unselect_action_ { 0, 0, "Unselect", tau::fun(this, &Main::on_menu_edit_unselect) };
    tau::Master_action  edit_select_all_master_action_ { U'A', tau::KM_CONTROL, "Select All" };
    tau::Action         edit_select_all_action_ { edit_select_all_master_action_, tau::fun(this, &Main::on_menu_edit_select_all) };
    tau::Master_action  edit_cut_master_action_ { "<Ctrl>X <Shift>Delete", "Cut", "edit-cut" };
    tau::Action         edit_cut_action_ { edit_cut_master_action_, tau::fun(this, &Main::on_menu_edit_copy) };
    tau::Master_action  edit_copy_master_action_ { "<Ctrl>C <Ctrl>Insert", "Copy", "edit-copy" };
    tau::Action         edit_copy_action_ { edit_copy_master_action_, tau::fun(this, &Main::on_menu_edit_cut) };
    tau::Master_action  edit_paste_master_action_ { "<Ctrl>V <Shift>Insert", "Paste", "edit-paste" };
    tau::Action         edit_paste_action_ { edit_paste_master_action_, tau::fun(this, &Main::on_menu_edit_paste) };

    tau::Master_action  view_increase_font_master_action_ { "<Ctrl>+ <Ctrl>=", "Increase Font", "zoom-in" };
    tau::Action         view_zin_action_ { view_increase_font_master_action_, tau::fun(this, &Main::on_menu_increase_font) };
    tau::Master_action  view_decrease_font_master_action_ { "<Ctrl>-", "Decrease Font", "zoom-out" };
    tau::Action         view_zout_action_ { view_decrease_font_master_action_ , tau::fun(this, &Main::on_menu_decrease_font) };
    tau::Action         view_next_page_action_ { tau::KC_RIGHT, tau::KM_ALT, "Next Page", tau::ICON_GO_NEXT, tau::fun(this, &Main::on_menu_next_doc) };
    tau::Action         view_prev_page_action_ { tau::KC_LEFT, tau::KM_ALT, "Previous Page", tau::ICON_GO_PREVIOUS, tau::fun(this, &Main::on_menu_prev_doc) };

    tau::Action         settings_action_ { U'P', tau::KM_CONTROL, "Settings", tau::fun(this, &Main::on_menu_settings) };

    Main(const tau::Rect & bounds=tau::Rect()):
        Toplevel(bounds)
    {
        datadir_ = tau::path_build(tau::path_user_data_dir(), tau::program_name());
        tau::path_mkdir(datadir_);
        ksession_.load(tau::path_build(datadir_, "session.ini"));
        font_spec_ = kstate_.get_string(kstate_.root(), "font", tau::Font::mono());
        font_size_ = kstate_.get_integer(kstate_.root(), "font-size", 10);

        connect_action(escape_action_);
        connect_action(file_quit_action_);
        connect_action(file_new_action_);
        connect_action(file_open_action_);
        file_save_action_.disable();
        connect_action(file_save_action_);
        connect_action(file_save_as_action_);
        file_save_as_action_.disable();
        connect_action(file_save_all_action_);
        file_save_all_action_.disable();
        connect_action(file_close_action_);
        file_close_action_.disable();
        connect_action(file_close_all_action_);
        file_close_all_action_.disable();
        connect_action(file_close_others_action_);
        file_close_others_action_.disable();

        edit_undo_action_.disable();
        connect_action(edit_undo_action_);
        edit_redo_action_.disable();
        connect_action(edit_redo_action_);

        edit_unselect_action_.disable();
        connect_action(edit_unselect_action_);

        edit_select_all_master_action_.disable();
        connect_action(edit_select_all_action_);

        edit_copy_master_action_.disable();
        connect_action(edit_copy_action_);

        edit_cut_master_action_.disable();
        connect_action(edit_cut_action_);

        if (!display().can_paste_text()) { edit_paste_master_action_.disable(); }
        connect_action(edit_paste_action_);

        view_increase_font_master_action_.disable();
        connect_action(view_zin_action_);

        view_decrease_font_master_action_.disable();
        connect_action(view_zout_action_);

        connect_action(view_next_page_action_);
        view_next_page_action_.disable();

        connect_action(view_prev_page_action_);
        view_prev_page_action_.disable();

        connect_action(settings_action_);

        signal_menu().connect(tau::fun(this, &Main::on_menu));
        signal_close().connect(tau::fun(this, &Main::save_metas));
        signal_close().connect(tau::fun(this, &Main::save_session));

        notebook_.signal_page_added().connect(fun(this, &Main::on_notebook_page_added));
        notebook_.signal_page_removed().connect(fun(this, &Main::on_notebook_page_removed));
        notebook_.signal_page_reordered().connect(fun(this, &Main::on_notebook_page_reordered));
        notebook_.signal_page_changed().connect(fun(this, &Main::on_notebook_page_changed));
        notebook_.signal_visible().connect(fun(this, &Main::update_title));

        tau::Display().signal_can_paste().connect(fun(this, &Main::on_can_paste));

        tau::Box box0(tau::OR_DOWN);
        insert(box0);
        populate_main_menu(menubar_);
        box0.append(menubar_, true);
        tau::Bin bin0;
        box0.append(bin0, true);
        toolbar_.hint_margin(3);
        bin0.insert(toolbar_);
        card_.insert(notebook_);
        box0.append(card_);

        {
            tau::Button button(file_open_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
        }

        {
            tau::Button button(file_save_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
        }

        {
            tau::Button button(file_save_as_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
            tau::Separator sep;
            toolbar_.append(sep, true);
        }

        {
            tau::Button button(edit_undo_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
        }

        {
            tau::Button button(edit_redo_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
            tau::Separator sep;
            toolbar_.append(sep, true);
        }

        {
            tau::Button button(edit_copy_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
        }

        {
            tau::Button button(edit_cut_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
        }

        {
            tau::Button button(edit_paste_action_, tau::ACTION_NO_LABEL);
            toolbar_.append(button, true);
        }

        update_title();
        set_icon("tau", 48);
        show_cx_ = signal_show().connect(tau::fun(this, &Main::on_show));
        signal_size_changed().connect(tau::fun(this, &Main::on_geometry));
        signal_position_changed().connect(tau::fun(this, &Main::on_geometry));
        menubar_.signal_quit().connect(tau::fun(this, &Main::on_menu_quit));
    }

   ~Main() {
        if (wpop_) {
            delete wpop_;
        }
    }

    void on_menu_quit() {
        if (notebook_.visible()) {
            auto i = std::find_if(pages_.begin(), pages_.end(), [this](auto & pg) { return notebook_.current_page() == pg.page; } );
            if (i != pages_.end()) { i->edit.grab_focus(); }
        }
    }

    void on_geometry() {
        tau::Point pos = position();
        tau::Size  sz  = size();
        geom_[0] = pos.x();
        geom_[1] = pos.y();
        geom_[2] = sz.iwidth();
        geom_[3] = sz.iheight();
        kstate_.set_integers(kstate_.root(), "geometry", geom_);
    }

    void open_recent(const tau::ustring & path) {
        int page = open_file(path);
        if (page >= 0) { notebook_.show_page(page); }
    }

    void on_show() {
        show_cx_.drop();

        if (!args_.empty()) {
            if (args_.size() > 1) {
                for (auto & s: args_) {
                    open_file(s);
                }
            }

            else {
                int page = open_file(args_.front());

                if (page >= 0 && (line_ > 0 || col_ > 0)) {
                    for (auto & pg: pages_) {
                        if (pg.page == page) {
                            pg.edit.move_to(line_ > 0 ? line_-1 : 0, col_ > 0 ? col_-1 : 0);
                            break;
                        }
                    }
                }
            }
        }

        else {
            load_session();
        }
    }

    int open_file(const tau::ustring & path) {
        for (auto & pg: pages_) {
            if (pg.path == path) {
                return pg.page;
            }
        }

        try {
            set_cursor("wait:watch");
            tau::Buffer buffer = tau::Buffer::load_from_file(path);
            Page & pg = new_editor(buffer);
            pg.path = path;
            pg.title.assign(tau::path_notdir(path));
            pg.tooltip.assign(path);
            pg.tab.set_tooltip(pg.tooltip);
            pg.metaid = find_metaid(path);

            if (0 == pg.metaid) {
                tau::Timeval now = tau::Timeval::now();
                pg.metaid = now;
                Meta_holder hol;
                hol.atime = now;
                hol.id = now;
                hol.path = path;
                metas_[tau::path_real(path)] = hol;
            }

            else {
                const tau::ustring name = tau::str_format(pg.metaid, ".ini");
                const tau::ustring mpath = tau::path_build(tau::path_build(tau::path_user_data_dir(), tau::program_name(), "meta"), name);
                tau::Key_file kf(mpath);
                std::size_t row = kf.get_integer(kf.section("pos"), "row");
                std::size_t col = kf.get_integer(kf.section("pos"), "col");
                pg.edit.move_to(row, col);
                pg.font_size = kf.get_integer(kf.section("font"), "size");
                pg.edit.style().font("font").set(tau::font_size_change(font_spec_, 0 != pg.font_size ? pg.font_size : font_size_));
            }

            pg.finfo = tau::Fileinfo(path);
            pg.finfo.signal_watch(tau::FILE_EVENTS).connect(tau::bind(tau::fun(this, &Main::on_watch), std::cref(pg)));
            pg.page = notebook_.append_page(pg.table, pg.tab);
            loop_.signal_alarm(16754).connect(tau::fun(this, &Main::save_metas));
            session_cx_.drop();
            session_cx_ = loop_.signal_alarm(22118).connect(tau::fun(this, &Main::save_session));
            touch_recent(pg.metaid);
            unset_cursor();
            return pg.page;
        }

        catch (tau::exception & x) {
            std::cerr << "** Main::open_file(): tau::exception caught: " << x.what() << std::endl;
        }

        catch (std::exception & x) {
            std::cerr << "** Main::open_file(): std::exception caught: " << x.what() << std::endl;
        }

        catch (...) {
            std::cerr << "** Main::open_file(): unknown exception caught: " << std::endl;
        }

        unset_cursor();
        return -1;
    }

    void update_recent_menu() {
        load_metas();
        recent_menu_.clear();
        auto v = kstate_.get_integers(kstate_.root(), "recent");
        std::map<uint64_t, uint64_t> ids;
        std::vector<uint64_t> ats;

        for (uint64_t id: v) {
            for (auto & p: metas_) {
                if (p.second.id == id) {
                    uint64_t at = p.second.atime;
                    ids[at] = id;
                    ats.push_back(at);
                    break;
                }
            }
        }

        std::sort(ats.begin(), ats.end(), std::greater<uint64_t>());

        for (uint64_t at: ats) {
            uint64_t id = ids[at];

            for (auto & p: metas_) {
                if (p.second.id == id) {
                    tau::Slot_menu_item item(tau::path_notdir(p.first), tau::bind(tau::fun(this, &Main::open_recent), p.second.path), "unknown");
                    tau::Text tooltip("@"+tau::path_dirname(p.second.path));
                    tooltip.set_wrap_mode(tau::WRAP_ELLIPSIZE_END);
                    tooltip.style().font("font").resize(7);
                    item.set_tooltip(tooltip);
                    recent_menu_.append(item);
                    break;
                }
            }
        }
    }

    void touch_recent(uint64_t rid) {
        load_metas();

        for (auto & p: metas_) {
            if (p.second.id == rid) {
                p.second.atime = tau::Timeval::now();
                break;
            }
        }

        std::map<uint64_t, uint64_t> ids;
        std::vector<uint64_t> ats;

        for (auto & p: metas_) {
            uint64_t at = p.second.atime;
            ats.push_back(at);
            ids[at] = p.second.id;
        }

        std::sort(ats.begin(), ats.end(), std::greater<uint64_t>());
        if (ats.size() > max_recent_) { ats.resize(max_recent_); }
        std::vector<long long> v;
        for (uint64_t at: ats) { v.push_back(ids[at]); }
        kstate_.set_integers(kstate_.root(), "recent", v);
        update_recent_menu();
    }

    bool on_menu() {
        menubar_.activate();
        return true;
    }

    void on_watch(unsigned mask, const tau::ustring & path, const Page & page) {
        std::cout << "on_watch " << std::hex << mask << std::dec << ", " << path << std::endl;
    }

    void set_font(const tau::ustring & spec) {
        const tau::ustring rspec = tau::font_size_remove(spec);

        if (font_spec_ != rspec) {
            font_spec_ = rspec;
            kstate_.set_integer(kstate_.root(), "font-size", tau::font_size_from_spec(spec));
            kstate_.set_string(kstate_.root(), "font", rspec);

            for (Page & pg: pages_) {
                pg.edit.style().font("font").set(tau::font_size_change(font_spec_, 0 != pg.font_size ? pg.font_size : font_size_));
            }
        }
    }

    void load_session() {
        load_metas();
        std::vector<int> v;

        for (auto & s: ksession_.list_sections()) {
            if (tau::str_has_prefix(s, "Page_")) {
                try { v.push_back(std::stoi(s.substr(5))); }
                catch (...) {}
            }
        }

        std::sort(v.begin(), v.end());

        for (auto & i: v) {
            auto & sect = ksession_.section(tau::str_format("Page_", i));
            uint64_t metaid = ksession_.get_integer(sect, "metaid");
            auto j = std::find_if(metas_.begin(), metas_.end(), [metaid](auto & p) { return p.second.id == metaid; } );
            if (j != metas_.end()) { open_file(j->second.path); }
        }

        uint64_t current_metaid = ksession_.get_integer(ksession_.root(), "current");
        auto k = std::find_if(pages_.begin(), pages_.end(), [current_metaid](auto & pg) { return pg.metaid == current_metaid; } );
        if (k != pages_.end()) { notebook_.show_page(k->page); }
    }

    void save_session() {
        session_cx_.drop();
        int page = 1, npages = notebook_.page_count();
        uint64_t current_metaid = 0;

        for (int n = 0; n < npages; ++n) {
            for (Page & pg: pages_) {
                if (n == pg.page && !pg.path.empty()) {
                    auto & sect = ksession_.section(tau::str_format("Page_", page++));
                    ksession_.set_integer(sect, "metaid", pg.metaid);
                    if (pg.page == notebook_.current_page()) { current_metaid = pg.metaid; }
                }
            }
        }

        if (0 != current_metaid) {
            ksession_.set_integer(ksession_.root(), "current", current_metaid);
        }

        try {
            ksession_.save();
            // std::cout << "save_session() -> " << '\n';
        }

        catch (tau::exception & x) {
            std::cerr << "** Main::save_session(): tau::exception caught: " << x.what() << std::endl;
        }

        catch (std::exception & x) {
            std::cerr << "** Main::save_session(): std::exception caught: " << x.what() << std::endl;
        }

        catch (...) {
            std::cerr << "** Main::save_session(): unknown exception caught" << std::endl;
        }
    }

    void load_metas() {
        if (metas_.empty()) {
            try {
                tau::Key_file kf(tau::path_build(datadir_, "metas.ini"));

                for (const tau::ustring & s: kf.list_sections()) {
                    if (tau::Fileinfo(s).is_regular()) {
                        tau::Key_section & sect = kf.section(s);
                        auto now = tau::Timeval::now();
                        Meta_holder hol;
                        hol.path = kf.get_string(sect, "path", s);
                        hol.atime = kf.get_integer(sect, "atime", now);
                        hol.id = kf.get_integer(sect, "id", now);
                        metas_[s] = hol;
                    }
                }
            }

            catch (tau::exception & x) {
                std::cerr << "** Main::load_metas(): tau::exception caught: " << x.what() << std::endl;
            }

            catch (std::exception & x) {
                std::cerr << "** Main::load_metas(): std::exception caught: " << x.what() << std::endl;
            }

            catch (...) {
                std::cerr << "** Main::load_metas(): unknown exception caught" << std::endl;
            }
        }
    }

    uint64_t find_metaid(const tau::ustring & path) {
        load_metas();
        auto iter = metas_.find(tau::path_real(path));
        return metas_.end() != iter ? iter->second.id : 0;
    }

    // Connected to editor's signal_focus_in().
    void update_pos(Page & pg) {
        tau::Buffer_citer i = pg.edit.caret();

        if (i) {
            pg.row_value.assign(tau::str_format(1+i.row()));
            pg.rows_value.assign(tau::str_format(std::max(std::size_t(1), pg.edit.buffer().rows())));
            pg.col_value.assign(tau::str_format(1+i.col()));
            pg.uni_value.assign(tau::key_code_to_string(*i));
        }

        else {
            pg.row_value.assign("0");
            pg.col_value.assign("0");
            pg.uni_value.assign("U+0000");
        }
    }

    // Connected to editor's signal_focus_in().
    void on_edit_focus_in(Page & pg) {
        if (pg.edit.modified()) { file_save_action_.enable(); }
        else { file_save_action_.disable(); }
        if (pg.edit.undo_action().enabled()) { edit_undo_action_.enable(); }
        else { edit_undo_action_.disable(); }
        if (pg.edit.redo_action().enabled()) { edit_redo_action_.enable(); }
        else { edit_redo_action_.disable(); }
        pg.on_edit_focus_in_cx = pg.edit.undo_action().signal_enable().connect(tau::fun(edit_undo_action_, &tau::Action::enable));
        pg.on_edit_focus_out_cx = pg.edit.undo_action().signal_disable().connect(tau::fun(edit_undo_action_, &tau::Action::disable));
        pg.enable_redo_cx = pg.edit.redo_action().signal_enable().connect(tau::fun(edit_redo_action_, &tau::Action::enable));
        pg.disable_redo_cx = pg.edit.redo_action().signal_disable().connect(tau::fun(edit_redo_action_, &tau::Action::disable));
    }

    // Connected to editor's signal_focus_out().
    void on_edit_focus_out(Page & pg) {
        edit_undo_action_.disable();
        edit_redo_action_.disable();
        pg.on_edit_focus_in_cx.drop();
        pg.enable_redo_cx.drop();
        pg.on_edit_focus_out_cx.drop();
        pg.disable_redo_cx.drop();
    }

    void on_utimer(Page & pg) {
        update_pos(pg);
        pg.meta_cx.drop();
        pg.meta_cx = loop_.signal_alarm(7735).connect(tau::bind(tau::fun(this, &Main::save_metadata), std::ref(pg)));
    }

    void on_caret_motion(Page & pg) {
        pg.motion_timer.start(50);
    }

    Page & new_editor(tau::Buffer buffer) {
        pages_.emplace_back();
        Page & pg = pages_.back();
        pg.motion_timer.signal_alarm().connect(tau::bind(tau::fun(this, &Main::on_utimer), std::ref(pg)));

        pg.tab.set_column_spacing(2);
        pg.tab.hint_margin(2, 2, 0, 0);
        pg.tab.put(pg.save_ico, -1, 0, 1, 1, true, true);
        pg.save_ico.hide();
        pg.title.hint_min_size(22, 0);
        pg.title.hint_max_size(142, 0);
        pg.title.set_wrap_mode(tau::WRAP_ELLIPSIZE_CENTER);
        pg.tab.put(pg.title, 0, 0, 1, 1, false, true);
        tau::Button btn(pg.ico);
        btn.hide_relief();
        btn.signal_click().connect(tau::bind(tau::fun(this, &Main::on_page_button_click), std::ref(pg)));
        pg.tab.put(btn, 1, 0, 1, 1, true, true);

        pg.tooltip.hint_max_size(320, 0);
        pg.tooltip.set_wrap_mode(tau::WRAP_ELLIPSIZE_END);
        pg.tooltip.style().font("font").resize(7);

        pg.edit.assign(buffer);
        pg.encoding_cx = pg.edit.buffer().signal_encoding_changed().connect(tau::bind(tau::fun(this, &Main::on_buffer_encoding_changed), std::ref(pg)));
        pg.edit.buffer().signal_changed().connect(tau::bind(tau::fun(this, &Main::on_edit_changed), std::ref(pg)));

        pg.edit.style().font("font").set(tau::font_size_change(font_spec_, font_size_));
        pg.edit.style().get("font").signal_changed().connect(tau::bind(tau::fun(this, &Main::on_edit_font_changed), std::ref(pg)));
        pg.edit.signal_caret_motion().connect(tau::bind(tau::fun(this, &Main::on_caret_motion), std::ref(pg)));
        pg.edit.signal_focus_in().connect(tau::bind(tau::fun(this, &Main::update_pos), std::ref(pg)));
        pg.edit.signal_focus_in().connect(tau::bind(tau::fun(this, &Main::on_edit_focus_in), std::ref(pg)));
        pg.edit.signal_focus_out().connect(tau::bind(tau::fun(this, &Main::on_edit_focus_out), std::ref(pg)));
        pg.edit.signal_selection_changed().connect(tau::bind(tau::fun(this, &Main::on_edit_selection_changed), std::ref(pg)));
        pg.edit.signal_modified().connect(tau::bind(tau::fun(this, &Main::on_modified), std::ref(pg)));
        pg.edit.insert_action().connect(tau::bind(tau::fun(this, &Main::on_edit_insert_toggled), std::ref(pg)));
        signal_modified_.connect(tau::fun(pg.edit, &tau::Edit::modified));
        pg.edit.cancel_action().disable();

        pg.zin_action.set_master_action(view_increase_font_master_action_);
        pg.edit.connect_action(pg.zin_action);
        pg.zin_action.connect(tau::bind(tau::fun(this, &Main::on_edit_increase_font), std::ref(pg)));

        pg.zout_action.set_master_action(view_decrease_font_master_action_);
        pg.edit.connect_action(pg.zout_action);
        pg.zout_action.connect(tau::bind(tau::fun(this, &Main::on_edit_decrease_font), std::ref(pg)));

        pg.edit.select_all_action().set_master_action(edit_select_all_master_action_);
        pg.edit.copy_action().set_master_action(edit_copy_master_action_);
        pg.edit.cut_action().set_master_action(edit_cut_master_action_);
        pg.edit.paste_action().set_master_action(edit_paste_master_action_);
        pg.edit.undo_action().set_master_action(edit_undo_master_action_);
        pg.edit.redo_action().set_master_action(edit_redo_master_action_);

        tau::Scroller scroller;
        tau::Slider vslider(scroller), hslider(scroller);
        scroller.insert(pg.edit);
        pg.table.put(scroller, 0, 0);
        pg.table.put(vslider, 1, 0, 1, 1, true, false);
        pg.table.put(hslider, 0, 1, 1, 1, false, true);
        pg.table.put(pg.status_box, 0, 2, 2, 1, false, true);

        // Status bar.
        pg.status_box.hint_margin(2);
        pg.status_box.style().font("font").enlarge(-2);

        {   // Current line label.
            tau::Frame frame(tau::BORDER_INSET);
            pg.status_box.append(frame, true);
            tau::Box box(tau::OR_RIGHT, 3);
            frame.insert(box);
            tau::Text line("Line");
            line.hint_margin(0, 4, 0, 0);
            box.append(line, true);
            box.append(pg.row_value, true);
            pg.row_value.set_tooltip("Shows current line number\nwithin text file.");

            tau::Text of("of ");
            box.append(of, true);
            box.append(pg.rows_value, true);
            pg.rows_value.set_tooltip("Shows total line count\nwithin text file.");
            pg.lines = pg.edit.buffer().rows();
            pg.rows_value.assign(tau::str_format(pg.lines));
        }

        {   // Current position label.
            tau::Frame frame(tau::BORDER_INSET);
            pg.status_box.append(frame, true);
            tau::Box box(tau::OR_RIGHT, 3);
            frame.insert(box);
            tau::Text pos("Pos:");
            box.append(pos, true);
            box.append(pg.col_value, true);
            pg.col_value.set_tooltip("Shows current character index\nwithin current line");
        }

        {   // Current unicode label.
            tau::Frame frame(tau::BORDER_INSET);
            pg.status_box.append(frame, true);
            frame.insert(pg.uni_value);
            pg.uni_value.set_tooltip("Shows current character\nUnicode value.");
        }

        {   // Input mode indicator.
            tau::Frame frame(tau::BORDER_INSET);
            pg.status_box.append(frame, true);
            tau::Card cd;
            cd.set_tooltip("Click here to change\nthe input mode");
            cd.signal_mouse_down().connect(tau::bind(tau::fun(this, &Main::on_insert_card), std::ref(pg)));
            cd.insert(pg.insert_label);
            cd.insert(pg.replace_label);
            frame.insert(cd);
        }

        {   // Text encoding label.
            tau::Frame frame(tau::BORDER_INSET);
            pg.status_box.append(frame, true);
            pg.enc_label.assign(buffer.encoding().name());
            frame.insert(pg.enc_label);
            pg.enc_label.set_tooltip("Shows text encoding.");
        }

        update_pos(pg);
        return pg;
    }

    bool on_insert_card(int mbt, int mm, const tau::Point & pt, Page & pg) {
        if (tau::MBT_LEFT == mbt) {
            auto & action = pg.edit.insert_action();
            action.toggle();
            if (action.get()) { pg.replace_label.show(); }
            else { pg.insert_label.show(); }
            return true;
        }

        return false;
    }

    void on_menu_file_new() {
        Page & pg = new_editor(tau::Buffer());
        pg.page = notebook_.append_page(pg.table, pg.tab);
        pg.title.assign("New File");
        pg.metaid = tau::Timeval::now();
        notebook_.show_page(pg.page);
    }

    void save_metadata(Page & pg) {
        pg.meta_cx.drop();

        if (!pg.path.empty()) {
            tau::Key_file kf;
            kf.set_integer(kf.section("pos"), "row", pg.edit.caret().row());
            kf.set_integer(kf.section("pos"), "col", pg.edit.caret().col());
            if (0 != pg.font_size) { kf.set_integer(kf.section("font"), "size", pg.font_size); }

            try {
                tau::ustring dir = tau::path_build(tau::path_user_data_dir(), tau::program_name(), "meta");
                tau::path_mkdir(dir);
                tau::ustring name = tau::str_format(pg.metaid, ".ini");
                tau::ustring path = tau::path_build(dir, name);
                kf.save(path);
                // std::cout << "save_metadata " << path << '\n';
            }

            catch (tau::exception & x) {
                std::cerr << "** Main::save_metadata(): tau::exception caught: " << x.what() << std::endl;
            }

            catch (std::exception & x) {
                std::cerr << "** Main::save_metadata(): std::exception caught: " << x.what() << std::endl;
            }

            catch (...) {
                std::cerr << "** Main::save_metadata(): unknown exception caught" << std::endl;
            }
        }
    }

    void save_metas() {
        for (Page & pg: pages_) {
            save_metadata(pg);
        }

        if (!metas_.empty()) {
            tau::Key_file kf;

            for (auto & p: metas_) {
                const Meta_holder & hol = p.second;
                auto & sect = kf.section(p.first);
                kf.set_integer(sect, "atime", hol.atime);
                kf.set_integer(sect, "id", hol.id);
                kf.set_string(sect, "path", hol.path);
            }

            try {
                const tau::ustring dir = tau::path_build(tau::path_user_data_dir(), tau::program_name());
                tau::path_mkdir(dir);
                const tau::ustring path = tau::path_build(dir, "metas.ini");
                kf.save(path);
                // std::cout << "save_metas() -> " << path << '\n';
            }

            catch (tau::exception & x) {
                std::cerr << "** Main::save_metas(): tau::exception caught: " << x.what() << std::endl;
            }

            catch (std::exception & x) {
                std::cerr << "** Main::save_metas(): std::exception caught: " << x.what() << std::endl;
            }

            catch (...) {
                std::cerr << "** Main::save_metas(): unknown exception caught" << std::endl;
            }
        }
    }

    void on_notebook_page_added(int page) {
        if (!pages_.empty()) {
            file_close_all_action_.enable();
        }

        if (pages_.size() > 1) {
            view_next_page_action_.enable();
            view_prev_page_action_.enable();
        }
    }

    void on_notebook_page_removed(int page) {
        if (pages_.size() < 2) {
            view_next_page_action_.disable();
            view_prev_page_action_.disable();
        }

        if (pages_.empty()) {
            file_close_all_action_.disable();
        }

        for (auto iter = pages_.begin(); iter != pages_.end(); ++iter) {
            Page & pg = *iter;

            if (page == pg.page) {
                pg.encoding_cx.drop();
                if (!pg.path.empty()) { save_metadata(pg); }
                pages_.erase(iter);
                break;
            }
        }

        update_save_all();
    }

    void on_notebook_page_reordered(int old_page, int new_page) {
        for (Page & pg: pages_) {
            if (old_page == pg.page) {
                pg.page = new_page;
                break;
            }
        }
    }

    void on_notebook_page_changed(int page) {
        update_title();

        for (Page & pg: pages_) {
            if (pg.page == page) {
                pg.edit.take_focus();
                file_close_action_.enable();
                if (pages_.size() > 1) { file_close_others_action_.enable(); }
                else { file_close_others_action_.disable(); }
                file_save_as_action_.enable();
                edit_select_all_master_action_.enable();

                if (pg.edit.has_selection()) {
                    edit_copy_master_action_.enable();
                    edit_cut_master_action_.enable();
                }

                else {
                    edit_copy_master_action_.disable();
                    edit_cut_master_action_.disable();
                }

                view_increase_font_master_action_.enable();
                view_decrease_font_master_action_.enable();
                session_cx_.drop();
                session_cx_ = loop_.signal_alarm(16754).connect(tau::fun(this, &Main::save_session));
                return;
            }
        }

        file_close_others_action_.disable();
        file_close_action_.disable();
        file_save_action_.disable();
        file_save_as_action_.disable();
        edit_copy_master_action_.disable();
        edit_cut_master_action_.disable();
        edit_select_all_master_action_.disable();
        view_increase_font_master_action_.disable();
        view_decrease_font_master_action_.disable();
    }

    void quit() {
        close();
    }

    void show_pop(Widget * w) {
        menubar_.disable();
        toolbar_.disable();
        wpop_ = w;
        card_.insert(*w);
        w->show();
        w->take_focus();
    }

    void delete_widget(tau::Widget * w) {
        delete w;
    }

    void close_pop() {
        if (wpop_) {
            card_.remove(*wpop_);
            delete wpop_;
            wpop_ = nullptr;
        }

        menubar_.enable();
        toolbar_.enable();
        notebook_.show();
        notebook_.take_focus();
    }

    void on_escape() {
        if (full_screened()) {
            unfullscreen();
        }

        else if (notebook_.hidden()) {
            close_pop();
        }

        else {
            quit();
        }
    }

    void close_page(Page & pg) {
        if (0 != pg.metaid) {
            if (!pg.path.empty()) { save_metadata(pg); }
            session_cx_.drop();
            session_cx_ = loop_.signal_alarm(9054).connect(tau::fun(this, &Main::save_session));
            notebook_.remove_page(pg.table);
        }
    }

    void close_others() {
        if (notebook_.page_count() > 1) {
            uint64_t metaid = 0;
            int page = notebook_.current_page();

            for (Page & pg: pages_) {
                if (page == pg.page) {
                    metaid = pg.metaid;
                    break;
                }
            }

            if (0 != metaid) {
                bool done;

                do {
                    done = true;

                    for (Page & pg: pages_) {
                        if (metaid != pg.metaid) {
                            close_page(pg);
                            done = false;
                            break;
                        }
                    }
                } while (!done);
            }
        }
    }

    void close_all() {
        while (!pages_.empty()) {
            close_page(pages_.back());
        }
    }

    void on_buffer_encoding_changed(const tau::Encoding & enc, Page & pg) {
        pg.enc_label.assign(enc.name());
    }

    void on_edit_insert_toggled(bool toggled, Page & pg) {
        if (toggled) { pg.replace_label.show(); }
        else { pg.insert_label.show(); }
    }

    void on_edit_changed(Page & pg) {
        std::size_t lines = pg.edit.buffer().rows();

        if (lines != pg.lines) {
            pg.lines = lines;
            pg.rows_value.assign(tau::str_format(pg.lines));
        }

        pg.meta_cx.drop();
        pg.meta_cx = loop_.signal_alarm(7439).connect(tau::bind(tau::fun(this, &Main::save_metadata), std::ref(pg)));
    }

    void on_edit_selection_changed(Page & pg) {
        if (!notebook_.hidden()) {
            if (notebook_.current_page() == pg.page) {
                if (pg.edit.has_selection()) {
                    edit_copy_master_action_.enable();
                    edit_cut_master_action_.enable();
                    edit_unselect_action_.enable();
                }

                else {
                    edit_copy_master_action_.disable();
                    edit_cut_master_action_.disable();
                    edit_unselect_action_.disable();
                }
            }
        }
    }

    void update_save_all() {
        if (signal_modified_()) { file_save_all_action_.enable(); }
        else { file_save_all_action_.disable(); }
    }

    void on_modified(bool modified, Page & pg) {
        if (modified) { pg.save_ico.show(); }
        else { pg.save_ico.hide(); }

        if (!pg.path.empty() && pg.edit.focused()) {
            if (modified) { file_save_action_.enable(); }
            else { file_save_action_.disable(); }
        }

        update_save_all();
    }

    void save_page(Page & pg) {
        if (!pg.path.empty()) {
            save_metadata(pg);
            pg.edit.buffer().save_to_file(pg.path);
            pg.save_ico.hide();
        }
    }

    void on_page_button_click(Page & pg) {
        close_page(pg);
    }

    void load_fileman(tau::Fileman & fman) {
        tau::Key_section & sect = kstate_.section("navigator");
        fman.load_state(kstate_, sect);
    }

    void save_fileman(tau::Fileman & fman) {
        tau::Key_section & sect = kstate_.section("navigator");
        fman.save_state(kstate_, sect);
    }

    void on_saver_apply(tau::Fileman * fman) {
        save_fileman(*fman);
        if (notebook_.hidden()) { close_pop(); }
    }

    void on_loader_apply(tau::Fileman & fman) {
        auto filenames = fman.selection();
        if (notebook_.hidden()) { card_.remove_current(); }
        int first_page = -1, exist_page = -1;

        for (const tau::ustring & f: filenames) {
            tau::ustring path = path_build(fman.uri(), f);

            for (const Page & pg: pages_) {
                if (pg.path == path) {
                    exist_page = pg.page;
                    break;
                }
            }

            if (-1 == exist_page) {
                int page = open_file(path);
                if (-1 == first_page) { first_page = page; }
            }
        }

        if (-1 == first_page) {
            if (-1 != exist_page) {
                notebook_.show_page(exist_page);
                update_title();
            }
        }

        else {
            notebook_.show_page(first_page);
            update_title();
        }

        save_fileman(fman);
    }

    void on_menu_file_open() {
        tau::ustring path;
        int page = notebook_.current_page();

        if (page >= 0) {
            for (const Page & pg: pages_) {
                if (pg.page == page) {
                    path = tau::path_dirname(pg.path);
                    break;
                }
            }
        }

        if (path.empty()) { path = tau::path_home(); }

        if (fileop_in_dialogs_) {
            tau::Fileman fman(tau::FILEMAN_OPEN, path);
            load_fileman(fman);
            tau::Navigator navi(fman.navigator_ptr());
            navi.allow_multiple_select();
            tau::Rect bounds;
            auto gv = kstate_.get_integers(kstate_.section("open_dialog"), "geometry");
            if (gv.size() > 3) { bounds.set(gv[0], gv[1], tau::Size(gv[2], gv[3])); }
            tau::Dialog dlg(*this, "Open a file", bounds);
            dlg.insert(fman);
            fman.apply_action().connect(tau::bind(fun(this, &Main::on_loader_apply), std::ref(fman)));
            fman.cancel_action().connect(tau::bind(fun(this, &Main::save_fileman), std::ref(fman)));
            dlg.show();
            fman.take_focus();
            dlg.grab_modal();
            dlg.run();
            bounds.set(dlg.position()-position(), dlg.size());
            gv.resize(4);
            gv[0] = bounds.x();
            gv[1] = bounds.y();
            gv[2] = bounds.width();
            gv[3] = bounds.height();
            kstate_.set_integers(kstate_.section("open_dialog"), "geometry", gv);
        }

        else if (!notebook_.hidden()) {
            tau::Fileman * fman = new tau::Fileman(tau::FILEMAN_OPEN, path);
            load_fileman(*fman);
            tau::Navigator navi(fman->navigator_ptr());
            navi.allow_multiple_select();
            fman->apply_action().connect(tau::bind(fun(this, &Main::on_loader_apply), std::ref(*fman)));
            fman->apply_action().connect(tau::fun(this, &Main::close_pop));
            fman->cancel_action().connect(tau::bind(fun(this, &Main::save_fileman), std::ref(*fman)));
            fman->cancel_action().connect(tau::fun(this, &Main::close_pop));
            set_title(tau::program_name()+": Open a File");
            show_pop(fman);
            fman->take_focus();
        }
    }

    void on_menu_file_save() {
        int page = notebook_.current_page();

        for (Page & pg: pages_) {
            if (pg.page == page) {
                save_page(pg);
                break;
            }
        }
    }

    void on_menu_file_save_as() {
        if (!notebook_.hidden()) {
            tau::ustring path;
            int page = notebook_.current_page();

            if (page >= 0) {
                for (Page & pg: pages_) {
                    if (pg.page == page) {
                        path = tau::path_dirname(pg.path);
                        break;
                    }
                }
            }

            tau::Fileman * fman = new tau::Fileman(tau::FILEMAN_SAVE, path.empty() ? tau::path_home() : path);
            load_fileman(*fman);
            fman->apply_action().connect(tau::bind(fun(this, &Main::on_saver_apply), fman));
            fman->cancel_action().connect(tau::bind(fun(this, &Main::save_fileman), std::ref(*fman)));
            fman->cancel_action().connect(tau::fun(this, &Main::close_pop));
            set_title(tau::program_name()+": Save File As");
            show_pop(fman);
            fman->take_focus();
        }
    }

    void on_menu_file_save_all() {
        for (Page & pg: pages_) { save_page(pg); }
    }

    void on_menu_file_quit() {
        quit();
    }

    void on_menu_file_close_all() {
        close_all();
    }

    void on_menu_file_close() {
        int page = notebook_.current_page();

        for (Page & pg: pages_) {
            if (pg.page == page) {
                close_page(pg);
                break;
            }
        }
    }

    void on_menu_edit_undo() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.undo_action().exec();
                    break;
                }
            }
        }
    }

    void on_menu_edit_redo() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.redo_action().exec();
                    break;
                }
            }
        }
    }

    void on_menu_edit_cut() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.cut_action().exec();
                    break;
                }
            }
        }
    }

    void on_menu_edit_copy() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.copy_action().exec();
                    break;
                }
            }
        }
    }

    void on_menu_edit_paste() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.paste_action().exec();
                    break;
                }
            }
        }
    }

    void on_menu_edit_select_all() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.select_all();
                    break;
                }
            }
        }
    }

    void on_menu_edit_unselect() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    pg.edit.unselect();
                    break;
                }
            }
        }
    }

    void on_menu_next_doc() {
        notebook_.show_next();
    }

    void on_menu_prev_doc() {
        notebook_.show_previous();
    }

    void on_edit_font_changed(Page & pg) {
        pg.meta_cx.drop();
        pg.meta_cx = loop_.signal_alarm(5767).connect(tau::bind(tau::fun(this, &Main::save_metadata), std::ref(pg)));
        pg.font_size = pg.edit.style().font("font").size();
    }

    void show_font_tooltip(Page & pg) {
        tau::Text text(pg.edit.style().font("font"));
        text.style().font("font").resize(18);
        pg.edit.show_tooltip(text, tau::Rect(pg.edit.size()).center(), tau::GRAVITY_CENTER, 2678);
        pg.edit.signal_focus_out().connect(tau::fun(pg.edit, &tau::Widget::hide_tooltip));
        pg.edit.style().get("font").signal_changed().connect(tau::fun(pg.edit, &tau::Widget::hide_tooltip));
    }

    void on_edit_increase_font(Page & pg) {
        auto fs = pg.edit.style().font("font");
        double pts = fs.size();
        if (pts < 100.0) { fs.grow(1.0); show_font_tooltip(pg); }
    }

    void on_edit_decrease_font(Page & pg) {
        auto fs = pg.edit.style().font("font");
        double pts = fs.size();
        if (pts >= 2.0) { fs.grow(-1.0); show_font_tooltip(pg); }
    }

    void on_menu_increase_font() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    on_edit_increase_font(pg);
                    break;
                }
            }
        }
    }

    void on_menu_decrease_font() {
        if (!notebook_.hidden()) {
            for (Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    on_edit_decrease_font(pg);
                    break;
                }
            }
        }
    }

    void on_can_paste() {
        if (display().can_paste_text()) { edit_paste_master_action_.enable(); }
        else  { edit_paste_master_action_.disable(); }
    }

    void update_title() {
        if (!notebook_.hidden()) {
            tau::ustring title = tau::program_name();

            if (0 != notebook_.page_count()) {
                int page = notebook_.current_page();

                if (-1 != page) {
                    for (auto & pg: pages_) {
                        if (pg.page == page) {
                            title += ": ";
                            if (!pg.path.empty()) { title += tau::path_notdir(pg.path); }
                            else { title += "(unnamed)"; }
                            pg.edit.take_focus();
                            break;
                        }
                    }
                }
            }

            set_title(title);
        }
    }

    void populate_main_menu(tau::Menu & mainmenu) {
        tau::Menubox edit_menu;
        mainmenu.append("Edit", edit_menu);
        edit_menu.append(edit_undo_action_);
        edit_menu.append(edit_redo_action_);
        edit_menu.append_separator();
        edit_menu.append(edit_cut_action_);
        edit_menu.append(edit_copy_action_);
        edit_menu.append(edit_paste_action_);
        edit_menu.append_separator();
        edit_menu.append(edit_select_all_action_);
        edit_menu.append(edit_unselect_action_);

        tau::Menubox file_menu;
        mainmenu.prepend("File", file_menu);
        file_menu.prepend(file_new_action_);
        file_menu.append_separator();
        file_menu.append(file_open_action_);
        tau::Submenu_item recent_item("Open Recent", recent_menu_, "document-open-recent:document-open");
        update_recent_menu();
        file_menu.append(recent_item);
        file_menu.append_separator();
        file_menu.append(file_save_action_);
        file_menu.append(file_save_as_action_);
        file_menu.append(file_save_all_action_);
        file_menu.append_separator();
        file_menu.append(file_close_action_);
        file_menu.append(file_close_all_action_);
        file_menu.append(file_close_others_action_);
        file_menu.append_separator();
        file_menu.append(file_quit_action_);

        tau::Menubox view_menu;
        mainmenu.append("View", view_menu);
        view_menu.append(view_next_page_action_);
        view_menu.append(view_prev_page_action_);
        view_menu.append_separator();
        view_menu.append(view_zin_action_);
        view_menu.append(view_zout_action_);

        mainmenu.append(settings_action_);
    }

    void on_menu_settings() {
        if (!notebook_.hidden()) {
            set_title(tau::program_name()+": Settings");

            // Box with the nested scroller box and the button box.
            tau::Box * box0 = new tau::Box(tau::OR_DOWN, 4);
            box0->hint_margin(4);
            show_pop(box0);

            // Nested box with scroller and slider.
            tau::Box box1(tau::OR_RIGHT, 2);
            box0->append(box1);
            tau::Frame frame(tau::BORDER_INSET);
            box1.append(frame);
            tau::Scroller scroller;
            frame.insert(scroller);
            tau::Slider slider(scroller, tau::OR_SOUTH, true);
            box1.append(slider, true);

            // Button box at the bottom.
            tau::Box button_box(tau::OR_RIGHT);
            button_box.set_align(tau::ALIGN_CENTER);
            box0->append(button_box, true);
            tau::Button close_button("Close");
            close_button.signal_click().connect(fun(this, &Main::close_pop));
            button_box.append(close_button, true);

            // Information tape as a box.
            tau::Box tape_box(tau::OR_DOWN);
            tape_box.hint_margin(4, 4, 2, 2);
            scroller.insert(tape_box);

            // Font selector.
            {
                tau::Frame frame(tau::BORDER_SOLID);
                frame.set_border(4, 0, 0, 0);
                frame.hint_margin(0, 0, 4, 4);
                tape_box.append(frame, true);

                tau::Box ibox(tau::OR_DOWN, 6);
                ibox.hint_margin_left(4);
                frame.insert(ibox);
                tau::Text title("Default editor font", tau::ALIGN_START);
                title.style().font("font").add_face("Bold");
                title.style().font("font").enlarge(4);
                ibox.append(title, true);
                tau::Text remark("This font used by text editor", tau::ALIGN_START);
                remark.style().font("font").enlarge(-2);
                ibox.append(remark, true);
            }

            unsigned font_size = font_size_;

            for (const Page & pg: pages_) {
                if (pg.page == notebook_.current_page()) {
                    if (0 != pg.font_size) { font_size = pg.font_size; }
                    break;
                }
            }

            tau::Fontsel fsel(tau::font_size_change(font_spec_, font_size));
            fsel.hint_margin(4, 4, 4, 0);
            fsel.hint_max_size(0, 3*size().height()/4);
            tape_box.append(fsel, true);
            fsel.signal_font_activated().connect(tau::fun(this, &Main::set_font));
            fsel.cancel_action().connect(tau::fun(this, &Main::close_pop));
            fsel.take_focus();
        }
    }
};

int main(int argc, char * argv[]) {
    tau::Locale::set();

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if ('-' != arg[0]) {
            tau::ustring path = tau::Encoding().decode(arg);
            if (!tau::path_is_absolute(path)) { path = tau::path_build(tau::path_cwd(), path); }
            if (args_.end() == std::find(args_.begin(), args_.end(), path)) { args_.push_back(path); }
        }

        else {
            if (("-l" == arg || "--line" == arg) && i+1 < argc) {
                line_ = std::stoi(argv[++i]);
            }

            else if (("-c" == arg || "--column" == arg) && i+1 < argc) {
                col_ = std::stoi(argv[++i]);
            }
        }
    }

    try {
        auto state_path = tau::path_build(tau::path_user_data_dir(), tau::program_name(), "state.ini");
        tau::path_mkdir(tau::path_dirname(state_path));
        kstate_.load(state_path);
        geom_ = kstate_.get_integers(kstate_.root(), "geometry");
        if (4 != geom_.size()) { geom_ = { 0, 0, 0, 0 }; }
        tau::Rect bounds;
        bounds.set(tau::Point(geom_[0], geom_[1]), tau::Size(geom_[2], geom_[3]));
        Main w(bounds);
        tau::Timer timer(tau::fun(kstate_, static_cast<void (tau::Key_file::*)()>(&tau::Key_file::save)));
        kstate_.signal_changed().connect(tau::bind(tau::fun(timer, &tau::Timer::start), 7401, false));
        tau::Loop().run();
        kstate_.set_integers(kstate_.root(), "geometry", geom_);
        kstate_.save();
    }

    catch (tau::exception & x) {
        std::cerr << "** tau::exception thrown: " << x.what() << std::endl;
    }

    return 0;
}

//END
