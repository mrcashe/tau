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
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

namespace {

tau::Key_file kstate_;
std::vector<std::thread> threads_;

} // anonymous namespace

void run();

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct Main: tau::Toplevel {
    tau::Loop           loop_;
    tau::Notebook       notebook_ { tau::TAB_RIGHT };

    tau::Action         escape_action_ { "Escape", tau::fun(this, &Main::close) };
    tau::Action         next_page_action_ { "<Alt>Down", tau::fun(this, &Main::on_next_page) };
    tau::Action         prev_page_action_ { "<Alt>Up", tau::fun(this, &Main::on_prev_page) };
    tau::Action         n_action_ { "F12", tau::fun(this, &Main::new_thread) };

    tau::Counter        ymax_ { 0, 1599, 0 };
    tau::Counter        xmax_ { 0, 1599, 0 };
    tau::Counter        ymin_ { 0,  999, 0 };
    tau::Counter        xmin_ { 0,  999, 0 };

    std::size_t         prev_row_ = 0;
    tau::Progress       progress_;
    std::vector<tau::ustring> color_names_;
    tau::Widget         color_cont_;

    struct Color_widgets {
        tau::Text       name { tau::ALIGN_START };
        tau::Widget     w;
        tau::Text       value;
    };

    std::vector<Color_widgets> color_widgets_;

    int init_sysinfo_page(int pg) {
        tau::Table table;
        int page = notebook_.append_page(table, pages_[pg].title);

        tau::Scroller scr;
        table.put(scr,  0, 0, 1, 1);

        tau::Slider vs(scr, tau::ORIENTATION_DOWN, true);
        table.put(vs,  1,  0, 1, 1,  true, false);

        tau::Slider hs(scr, tau::ORIENTATION_EAST, true);
        table.put(hs,  0,  1, 1, 1,  false, true);

        tau::Text text(tau::str_sysinfo(), tau::ALIGN_START, tau::ALIGN_START);
        text.style().font(tau::STYLE_FONT).set(tau::Font::mono());
        text.style().redirect(tau::STYLE_WHITESPACE_BACKGROUND, tau::STYLE_BACKGROUND);
        text.allow_select();
        scr.insert(text);

        return page;
    }

    int init_list_page(int pg) {
        tau::List list;

        for (int i = 0; i < 64; ++i) {
            tau::Text t(tau::str_format("Row ", i));
            int row = list.append_row(t);

            for (int j = -1; j < 3; ++j) {
                tau::Text tt(tau::str_format("Row ", i, ':', j));
                list.insert(row, tt, j);
            }
        }

        return notebook_.append_page(list, pages_[pg].title);
    }

    void on_twins_ratio(double ratio, const tau::ustring & key) {
        kstate_.set_double(kstate_.root(), key, ratio);
    }

    int init_twins_page(int pg) {
        tau::Text label(pages_[pg].title);
        label.set_tooltip("This page shows\nthe Twins container example");
        tau::Frame frame(pages_[pg].title, tau::BORDER_SOLID, 1, 8);
        frame.hint_margin(5);
        frame.set_border_color(tau::Color("DeepSkyBlue"));
        frame.style().get("background").set("Lavender");
        int page = notebook_.append_page(frame, label);

        tau::Table table;
        table.set_column_spacing(5);
        table.set_row_spacing(5);
        table.hint_margin(4);
        table.style().get("background").set("PeachPuff");
        frame.insert(table);

        {   tau::Twins twins(tau::OR_DOWN, kstate_.get_double(kstate_.root(), "ratio_1", 0.4));
            twins.signal_ratio_changed().connect(tau::bind(tau::fun(this, &Main::on_twins_ratio), "ratio_1"));
            table.put(twins, 0, 0);
            tau::Text first("First@OR_DOWN"), second("Second@OR_DOWN");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        {   tau::Twins twins(tau::OR_UP, kstate_.get_double(kstate_.root(), "ratio_2", 0.4));
            twins.signal_ratio_changed().connect(tau::bind(tau::fun(this, &Main::on_twins_ratio), "ratio_2"));
            table.put(twins, 0, 1);
            tau::Text first("First@OR_UP"), second("Second@OR_UP");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        {   tau::Twins twins(tau::OR_LEFT, kstate_.get_double(kstate_.root(), "ratio_3", 0.4));
            twins.signal_ratio_changed().connect(tau::bind(tau::fun(this, &Main::on_twins_ratio), "ratio_3"));
            table.put(twins, 1, 0);
            tau::Text first("First@OR_LEFT"), second("Second@OR_LEFT");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        {   tau::Twins twins(tau::OR_RIGHT, kstate_.get_double(kstate_.root(), "ratio_4", 0.4));
            twins.signal_ratio_changed().connect(tau::bind(tau::fun(this, &Main::on_twins_ratio), "ratio_4"));
            table.put(twins, 1, 1);
            tau::Text first("First@OR_RIGHT"), second("Second@OR_RIGHT");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        return page;
    }

    void on_bps_changed(const tau::ustring & s) {
        kstate_.set_string(kstate_.root(), "bps", s);
    }

    int init_controls_page(int pg) {
        tau::Table table;
        table.signal_take_focus().connect(tau::fun(std::function<bool()>([]() { return true; } )), true);
        table.set_column_spacing(5);
        table.set_row_spacing(5);
        table.hint_margin(4);
        table.align_column(7, tau::ALIGN_CENTER);
        int row = 0, page = notebook_.append_page(table, pages_[pg].title);

        {
            tau::Text label("tau::Button", tau::ALIGN_START);
            table.put(label, 0, row, 1, 1, true);
            tau::Icon ico(tau::ICON_DOCUMENT_SAVE, tau::MEDIUM_ICON);
            tau::Button push_button(ico);
            table.put(push_button, 7, row, 1, 1, true, true);
        }

        ++row;
        {
            tau::Text label("tau::Toggle", tau::ALIGN_START);
            table.put(label, 0, row);
            tau::Icon ico2(tau::ICON_CONFIGURE, tau::SMALL_ICON);
            tau::Toggle toggle_button(ico2, "Toggle button");
            table.put(toggle_button, 7, row, 1, 1, true, true);
        }

        ++row;
        {
            tau::Text label("tau::Counter", tau::ALIGN_START);
            table.put(label, 0, row);
            tau::Counter counter(kstate_.get_integer(kstate_.root(), "counter"), 247, 1);
            counter.append("rpm", 2, 2);
            table.put(counter, 7, row, 1, 1, true, true);
            auto on_changed = [](double val) { kstate_.set_integer(kstate_.root(), "counter", val); };
            counter.signal_value_changed().connect(tau::fun(std::function<void(double)>(on_changed)));
        }

        ++row;
        {
            tau::Text label("tau::Check(tau::CHECK_XSTYLE)", tau::ALIGN_START);
            table.put(label, 0, row, 4, 1);
            tau::Check check(tau::CHECK_XSTYLE, tau::BORDER_SOLID, kstate_.get_boolean(kstate_.root(), "xcheck"));
            table.put(check, 7, row, 1, 1, true, true);
            auto on_changed = [](bool checked) { kstate_.set_boolean(kstate_.root(), "xcheck", checked); };
            check.signal_check().connect(tau::bind(tau::fun(std::function<void(bool)>(on_changed)), true));
            check.signal_uncheck().connect(tau::bind(tau::fun(std::function<void(bool)>(on_changed)), false));
        }

        ++row;
        {
            tau::Text label("tau::Check(tau::CHECK_VSTYLE)", tau::ALIGN_START);
            table.put(label, 0, row, 4, 1);
            tau::Check check(tau::CHECK_VSTYLE, kstate_.get_boolean(kstate_.root(), "vcheck"));
            table.put(check, 7, row, 1, 1, true, true);
            auto on_changed = [](bool checked) { kstate_.set_boolean(kstate_.root(), "vcheck", checked); };
            check.signal_check().connect(tau::bind(tau::fun(std::function<void(bool)>(on_changed)), true));
            check.signal_uncheck().connect(tau::bind(tau::fun(std::function<void(bool)>(on_changed)), false));
        }

        ++row;
        {
            tau::Text label("tau::Check(tau::CHECK_RSTYLE)", tau::ALIGN_START);
            table.put(label, 0, row, 4, 1);

            tau::Check check(tau::CHECK_RSTYLE, true);
            table.put(check, 6, row, 1, 1, true, true);

            tau::Check check1(tau::CHECK_RSTYLE);
            check1.join(check);
            table.put(check1, 7, row, 1, 1, true, true);
        }

        ++row;
        {
            tau::Text label("tau::Cycle", tau::ALIGN_START);
            table.put(label, 0, row, 1, 1);
            tau::Cycle cycle;
            table.put(cycle, 7, row, 1, 1, true, true);
            cycle.prepend("Speed:", 2, 4);
            cycle.append("bps", 2, 2);

            const long bauds[] = { 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200, 230400, 460800, 0 };
            tau::Widget_ptr wp;
            auto current = kstate_.get_string(kstate_.root(), "bps");

            for (int i = 0; 0 != bauds[i]; ++i) {
                auto s = tau::str_format(bauds[i]);
                tau::Text label(s);
                label.signal_select().connect(tau::bind(tau::fun(this, &Main::on_bps_changed), s));
                label.style().get(tau::STYLE_BACKGROUND).set(tau::COLOR_YELLOW);
                cycle.add(label);
                if (s == current) { wp = label.ptr(); }
            }

            if (wp) {
                tau::Widget w(wp);
                w.show();
            }
        }

        ++row;
        {
            tau::Text label("tau::Progress", tau::ALIGN_START);
            table.put(label, 0, row, 1, 1);
            progress_.set_format("%$%% complete");
            progress_.set_precision(1);
            progress_.style().font("font").add_face("Bold");
            progress_.set_border_style(tau::BORDER_SOLID);
            table.put(progress_, 1, row, 7, 1, false, true);
        }

        return page;
    }

    int init_colors_page(int pg) {
        color_names_ = tau::Color::list_css_names();
        color_widgets_.resize(color_names_.size());
        tau::Table table;
        color_cont_ = table;
        table.set_column_spacing(6);
        table.set_row_spacing(5);
        tau::Scroller scroller;
        scroller.insert(table);
        scroller.hint_margin(3, 3, 4, 4);
        tau::Slider slider(scroller);
        tau::Box box(tau::OR_RIGHT, 2);
        tau::Text label(pages_[pg].title);
        label.set_tooltip("This page shows\nnamed colors");
        int page = notebook_.append_page(box, label);
        box.append(scroller);
        box.append(slider, true);
        std::size_t row = 0;

        for (auto & cname: color_names_) {
            color_widgets_[row].w.hint_min_size(16, 0);
            table.put(color_widgets_[row].name, 0, row, 1, 1, true, true);
            table.put(color_widgets_[row].w, 1, row, 1, 1);
            table.put(color_widgets_[row].value, 2, row);
            set_row_color(row, cname);
            ++row;
        }

        return page;
    }

    int init_cursors_page(int pg) {
        tau::Table xtable;
        int page = notebook_.append_page(xtable, pages_[pg].title);

        tau::Table table;
        table.set_column_spacing(5);
        table.set_row_spacing(5);
        tau::Scroller scroller;
        scroller.insert(table);

        tau::Slider hslider(scroller, tau::OR_EAST, true);
        xtable.put(hslider, 0, 1, 1, 1, false, true);

        tau::Slider vslider(scroller, tau::OR_SOUTH, true);
        xtable.put(vslider, 1, 0, 1, 1, true, false);

        xtable.put(scroller, 0, 0);
        tau::ustring path = tau::path_build(tau::path_share(), "cursors");

        if (tau::file_is_dir(path)) {
            for (auto & s: tau::path_glob(tau::path_build(path, "*"))) {
                try {
                    auto sizes = tau::Cursor::list_sizes(s);

                    if (!sizes.empty()) {
                        tau::Table::Span rng = table.span();
                        tau::Text title_text(tau::path_notdir(s));
                        table.put(title_text, -2, rng.ymax, 1, sizes.size(), false, true);

                        for (auto n: sizes) {
                            if (auto cur = tau::Cursor::load_from_file(s, n)) {
                                tau::Image img;
                                img.set_transparent();
                                table.put(img, 0, rng.ymax, 1, 1, true, true);
                                std::size_t n_frames = cur.frame_count();
                                tau::Size sz;

                                for (std::size_t fr = 0; fr < n_frames; ++fr) {
                                    if (tau::Pixmap pix = cur.pixmap(fr)) {
                                        img.add_pixmap(pix, cur.delay(fr));
                                        sz |= pix.size();
                                    }
                                }

                                tau::Text size_text(tau::str_format("Size: ", cur.size(), "\n(", sz.width(), "x" , sz.height(), " px)"));
                                table.put(size_text, -1, rng.ymax, 1, 1, true);
                                ++rng.ymax;
                            }
                        }
                    }
                }

                catch (tau::exception & x) {
                    std::cerr << "** " << s << ": " << x.what() << std::endl;
                }
            }
        }

        return page;
    }

    void on_colorsel(const tau::Color & c) {
        kstate_.set_string(kstate_.root(), "colorsel", c.html());
    }

    int init_colorsel_page(int pg) {
        tau::Colorsel colorsel(tau::Color(kstate_.get_string(kstate_.root(), "colorsel", tau::COLOR_BLUE)));
        colorsel.cancel_action().disable();
        colorsel.hint_margin(4);
        colorsel.signal_color_changed().connect(tau::fun(this, &Main::on_colorsel));
        return notebook_.append_page(colorsel, pages_[pg].title);
    }

    struct Page {
        tau::ustring        title;
        int                 page;
        tau::slot<int(int)> x;
    };

    std::vector<Page>   pages_ {
        { "Controls",   0, tau::fun(this, &Main::init_controls_page) },
        { "List",       1, tau::fun(this, &Main::init_list_page) },
        { "Cursors",    2, tau::fun(this, &Main::init_cursors_page) },
        { "Colors",     3, tau::fun(this, &Main::init_colors_page) },
        { "Twins",      4, tau::fun(this, &Main::init_twins_page) },
        { "Colorsel",   5, tau::fun(this, &Main::init_colorsel_page) },
        { "Sysinfo",    6, tau::fun(this, &Main::init_sysinfo_page) }
    };

    void new_thread() {
        threads_.emplace_back(std::thread(run));
    }

    void save_pages() {
        tau::Key_section & sect = kstate_.section("pages");

        for (auto & pg: pages_) {
            kstate_.set_integer(sect, pg.title, pg.page);
            if (pg.page == notebook_.current_page()) { kstate_.set_string(sect, "current", pg.title); }
        }
    }

    void on_page_reordered(int old_page, int new_page) {
        for (auto & pg: pages_) {
            if (pg.page == old_page) { pg.page = new_page; }
            else if (pg.page == new_page) { pg.page = old_page; }
        }

        save_pages();
    }

    void on_page_changed(int n) {
        save_pages();
    }

    void set_row_color(std::size_t row, const tau::ustring & cname) {
        tau::Color c(cname);
        color_widgets_[row].name.assign(cname);
        color_widgets_[row].w.style().get(tau::STYLE_BACKGROUND).set(cname);
        color_widgets_[row].value.assign(c.html());
    }

    void on_minmax_changed(double) {
        unsigned xmin = xmin_.value(), ymin = ymin_.value(), xmax = xmax_.value(), ymax = ymax_.value();

        if (xmin >= 200 && ymin >= 200) {
            hint_min_size(xmin, ymin);
            std::vector<long long> v = { xmin, ymin };
            kstate_.set_integers(kstate_.section("main"), "min_size", v);
        }

        else {
            kstate_.remove_key(kstate_.section("main"), "min_size");
        }

        if (xmax >= 300 && ymax >= 300) {
            hint_max_size(xmax, ymax);
            std::vector<long long> v = { xmax, ymax };
            kstate_.set_integers(kstate_.section("main"), "max_size", v);
        }

        else {
            kstate_.remove_key(kstate_.section("main"), "max_size");
        }
    }

    void on_timer() {
        static int div = 0;

        if (progress_.visible() && 0 == div) {
            double value = progress_.value();
            value += 1.25;
            if (value > progress_.max_value()) { value = 0.0; }
            progress_.set_value(value);
        }

        if (color_cont_.visible()) {
            std::size_t row = (double(rand())/RAND_MAX)*color_widgets_.size();
            set_row_color(row, color_names_[prev_row_]);
            prev_row_ = row;
        }

        if (++div == 8) { div = 0; }
    }

    void on_next_page() {
        notebook_.show_next();
    }

    void on_prev_page() {
        notebook_.show_previous();
    }

    void on_geometry_changed() {
        std::vector<long long> v = { position().x(), position().y(), size().iwidth(), size().iheight() };
        kstate_.set_integers(kstate_.section("main"), "geometry", v);
    }

    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    Main(const tau::Rect & bounds):
        Toplevel(bounds)
    {
        {
            auto v = kstate_.get_integers(kstate_.section("main"), "min_size");
            if (v.size() > 1) { hint_min_size(v[0], v[1]); }
        }

        {
            auto v = kstate_.get_integers(kstate_.section("main"), "max_size");
            if (v.size() > 1) { hint_max_size(v[0], v[1]); }
        }

        tau::Box box0(tau::OR_SOUTH);
        insert(box0);
        notebook_.hint_margin(8);
        box0.append(notebook_);

        tau::Box ctlbox(tau::OR_WEST, 4);
        ctlbox.hint_margin(2, 2, 8, 2);
        ctlbox.style().get(tau::STYLE_WHITESPACE_BACKGROUND).set("BlanchedAlmond");
        box0.append(ctlbox, true);

        {
            tau::Frame frm(tau::BORDER_INSET);
            frm.hint_margin(2);
            ctlbox.append(frm, true);

            tau::Box box(tau::OR_LEFT, 4);
            box.hint_margin(2);
            frm.insert(box);

            box.append(ymax_);
            ymax_.set_tooltip("Sets maximal window height, in pixels");
            ymax_.append("px", 2, 2);
            ymax_.prepend("h:", 2, 2);
            ymax_.set_value(max_size_hint().height());
            ymax_.set_step_value(10);
            ymax_.signal_value_changed().connect(tau::fun(this, &Main::on_minmax_changed));

            box.append(xmax_);
            xmax_.set_tooltip("Sets maximal window width, in pixels");
            xmax_.append("px", 2, 2);
            xmax_.prepend("w:", 2, 2);
            xmax_.set_value(max_size_hint().width());
            xmax_.set_step_value(10);
            xmax_.signal_value_changed().connect(tau::fun(this, &Main::on_minmax_changed));

            tau::Text label("Max:", tau::ALIGN_END);
            box.append(label, true);
        }

        {
            tau::Frame frm(tau::BORDER_INSET);
            frm.hint_margin(2);
            ctlbox.append(frm, true);

            tau::Box box(tau::OR_LEFT, 4);
            box.hint_margin(2);
            frm.insert(box);

            box.append(ymin_);
            ymin_.set_tooltip("Sets minimal window height, in pixels");
            ymin_.append("px", 2, 2);
            ymin_.prepend("h:", 2, 2);
            ymin_.set_value(min_size_hint().height());
            ymin_.set_step_value(10);
            ymin_.signal_value_changed().connect(tau::fun(this, &Main::on_minmax_changed));

            box.append(xmin_);
            xmin_.set_tooltip("Sets minimal window width, in pixels");
            xmin_.append("px", 2, 2);
            xmin_.prepend("w:", 2, 2);
            xmin_.set_value(min_size_hint().width());
            xmin_.set_step_value(10);
            xmin_.signal_value_changed().connect(tau::fun(this, &Main::on_minmax_changed));

            tau::Text label("Min:", tau::ALIGN_END);
            box.append(label, true);
        }

        connect_action(escape_action_);
        connect_action(next_page_action_);
        connect_action(prev_page_action_);
        connect_action(n_action_);

        signal_position_changed().connect(tau::fun(this, &Main::on_geometry_changed));
        signal_size_changed().connect(tau::fun(this, &Main::on_geometry_changed));
        loop_.signal_alarm(107, true).connect(tau::fun(this, &Main::on_timer));

        set_icon("tau", 48);
        tau::Key_section & sect = kstate_.section("pages");
        std::map<int, int> m;
        int ifb = pages_.size();

        for (auto & pg: pages_) {
            m[kstate_.get_integer(sect, pg.title, ifb++)] = pg.page;
        }

        for (auto & mp: m) {
            pages_[mp.second].page = pages_[mp.second].x(mp.second);
        }

        tau::ustring ctitle = kstate_.get_string(sect, "current", pages_[0].title);

        for (auto & pg: pages_) {
            if (pg.title == ctitle) {
                notebook_.show_page(pg.page);
                break;
            }
        }

        notebook_.take_focus();
        notebook_.signal_page_changed().connect(fun(this, &Main::on_page_changed));
        notebook_.signal_page_reordered().connect(fun(this, &Main::on_page_reordered));
    }
};

void run() {
    try {
        auto v = kstate_.get_integers(kstate_.section("main"), "geometry");
        tau::Rect bounds;
        if (v.size() > 3) { bounds.set(tau::Point(v[0], v[1]), tau::Size(v[2], v[3])); }
        Main wnd(bounds);
        wnd.set_title("TAU Demo");
        tau::Loop().run();
    }

    catch (tau::exception & x) {
        std::cerr << "** taudemo: run(): tau::exception thrown: " << x.what() << std::endl;
    }

    catch (std::exception & x) {
        std::cerr << "** taudemo: run(): std::exception thrown: " << x.what() << std::endl;
    }

    catch (...) {
        std::cerr << "** taudemo: run(): unknown exception thrown: " << std::endl;
    }
}

int main(int argc, char * argv[]) {
    auto p = tau::path_build(tau::path_user_data_dir(), tau::program_name(), "state.ini");
    tau::path_mkdir(tau::path_dirname(p));
    kstate_ = tau::Key_file(p);
    tau::Timer timer(tau::fun(kstate_, static_cast<void (tau::Key_file::*)()>(&tau::Key_file::save)));
    kstate_.signal_changed().connect(tau::bind(tau::fun(timer, &tau::Timer::start), 6789, false));
    run();
    for (auto & thr: threads_) { thr.join(); }
    kstate_.save();
    return 0;
}

//END
