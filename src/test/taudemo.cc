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

#include <tau.hh>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

tau::Key_file kf_;
std::vector<std::thread> threads_;

void run();

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Main: public tau::Toplevel {
    tau::Notebook       notebook_ { tau::TAB_RIGHT };

    tau::Action         escape_action_ { "Escape", tau::fun(this, &Main::close) };
    tau::Action         next_page_action_ { "<Alt>Down", tau::fun(this, &Main::on_next_page) };
    tau::Action         prev_page_action_ { "<Alt>Up", tau::fun(this, &Main::on_prev_page) };
    tau::Action         n_action_ { "F12", tau::fun(this, &Main::on_n) };

    tau::Counter        ymax_ { 0, 1599, 0 };
    tau::Counter        xmax_ { 0, 1599, 0 };
    tau::Counter        ymin_ { 0,  999, 0 };
    tau::Counter        xmin_ { 0,  999, 0 };

    struct Page {
        tau::ustring    title;
        int             def;
        int             page;
    };

    std::vector<Page>   pages_ {
        { "Controls",   0, 0,},
        { "List",       1, 1 },
        { "Cursors",    2, 2 },
        { "Colors",     3, 3 },
        { "Twins",      4, 4 },
        { "Colorsel",   5, 5 }
    };

    std::size_t                 prev_row_ = 0;
    tau::Progress               progress_;
    tau::Timer                  timer_;
    std::vector<tau::ustring>   color_names_;

    struct Color_widgets {
        tau::Text *   name;
        tau::Widget * w;
        tau::Text *   value;
    };

    std::vector<Color_widgets> color_widgets_;

public:

    Main(tau::Key_file & kf, const tau::Rect & bounds):
        Toplevel(bounds)
    {
        {
            auto v = kf_.get_integers(kf_.section("main"), "min_size");
            if (v.size() > 1) { hint_min_size(v[0], v[1]); }
        }

        {
            auto v = kf_.get_integers(kf_.section("main"), "max_size");
            if (v.size() > 1) { hint_max_size(v[0], v[1]); }
        }

        tau::Box box0(tau::OR_SOUTH);
        insert(box0);
        notebook_.hint_margin(8);
        box0.append(notebook_);

        tau::Box ctlbox(tau::OR_WEST, 4);
        ctlbox.hint_margin(2, 2, 8, 2);
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
            ymax_.style().get("background-whitespace").set("BlanchedAlmond");
            ymax_.append("px", 2, 2);
            ymax_.prepend("h:", 2, 2);
            ymax_.set_value(max_size_hint().height());
            ymax_.set_step_value(10);
            ymax_.signal_value_changed().connect(tau::fun(this, &Main::on_minmax_changed));

            box.append(xmax_);
            xmax_.set_tooltip("Sets maximal window width, in pixels");
            xmax_.style().get("background-whitespace").set("BlanchedAlmond");
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
            ymin_.style().get("background-whitespace").set("BlanchedAlmond");
            ymin_.append("px", 2, 2);
            ymin_.prepend("h:", 2, 2);
            ymin_.set_value(min_size_hint().height());
            ymin_.set_step_value(10);
            ymin_.signal_value_changed().connect(tau::fun(this, &Main::on_minmax_changed));

            box.append(xmin_);
            xmin_.set_tooltip("Sets minimal window width, in pixels");
            xmin_.style().get("background-whitespace").set("BlanchedAlmond");
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

        timer_ = tau::Timer(tau::fun(this, &Main::on_timer), 100, true);
        color_names_ = tau::Color::list_names();
        color_widgets_.resize(color_names_.size());

        set_icon("tau-48");

        init_controls_page(0);
        init_list_page(1);
        init_cursors_page(2);
        init_colors_page(3);
        init_twins_page(4);
        init_colorsel_page(5);

        tau::Key_section & sect = kf_.section("pages");

        for (auto & p: pages_) {
            unsigned pg = kf_.get_integer(sect, p.title, -1);

            if (pg >= 0 && pg < pages_.size()) {
                auto & p1 = pages_[pg];
                std::swap(p.page, p1.page);
                notebook_.reorder_page(p.page, pg);
            }
        }

        notebook_.signal_page_changed().connect(fun(this, &Main::on_page_changed));
        notebook_.signal_page_reordered().connect(fun(this, &Main::on_page_reordered));

        tau::ustring ctitle = kf_.get_string(sect, "current");
        int cpage = 1;

        for (auto & p: pages_) {
            if (p.title == ctitle) {
                cpage = p.page;
                //break;
            }
        }

        notebook_.show_page(cpage);
        notebook_.take_focus();
    }

   ~Main() {
       for (auto & cw: color_widgets_) { delete cw.name; delete cw.w; delete cw.value; }
    }

private:

    void on_n() {
        threads_.emplace_back(std::thread(run));
    }

    void on_page_reordered(int old_page, int new_page) {
        int done = 0;
        tau::Key_section & sect = kf_.section("pages");
        //std::cout << "reordered " << old_page << " " << new_page << '\n';

        for (auto & p: pages_) {
            if (p.page == old_page) {
                p.page = new_page;
                if (p.page != p.def) { kf_.set_integer(sect, p.title, new_page); }
                else { kf_.remove_key(sect, p.title); }
                ++done;
            }

            else if (p.page == new_page) {
                p.page = old_page;
                kf_.set_integer(kf_.section("pages"), p.title, old_page);
                if (p.page != p.def) { kf_.set_integer(sect, p.title, new_page); }
                else { kf_.remove_key(sect, p.title); }
                ++done;
            }

            if (2 == done) { break; }
        }
    }

    void on_page_changed(int n) {
        for (auto & p: pages_) {
            if (p.page == n) {
                //std::cout << "changed " << n << " " << p.title << '\n';
                kf_.set_string(kf_.section("pages"), "current", p.title);
                break;
            }
        }
    }

    void set_row_color(std::size_t row, const tau::ustring & cname) {
        tau::Color c(cname);
        color_widgets_[row].name->assign(cname);
        color_widgets_[row].w->style().get("background").set(cname);
        color_widgets_[row].value->assign(c.html());
    }

    void on_minmax_changed(double) {
        unsigned xmin = xmin_.value(), ymin = ymin_.value(), xmax = xmax_.value(), ymax = ymax_.value();

        if (xmin >= 200 && ymin >= 200) {
            hint_min_size(xmin, ymin);
            std::vector<long long> v = { xmin, ymin };
            kf_.set_integers(kf_.section("main"), "min_size", v);
        }

        else {
            kf_.remove_key(kf_.section("main"), "min_size");
        }

        if (xmax >= 300 && ymax >= 300) {
            hint_max_size(xmax, ymax);
            std::vector<long long> v = { xmax, ymax };
            kf_.set_integers(kf_.section("main"), "max_size", v);
        }

        else {
            kf_.remove_key(kf_.section("main"), "max_size");
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

        std::size_t row = (double(rand())/RAND_MAX)*color_widgets_.size();
        set_row_color(row, color_names_[prev_row_]);
        prev_row_ = row;
        if (++div == 8) { div = 0; }
    }

    void init_list_page(int pg) {
        tau::List list;

        for (int i = 0; i < 64; ++i) {
            tau::Text t(tau::str_format("Row ", i));
            int br = list.append_row(t);

            for (int j = -1; j < 3; ++j) {
                tau::Text tt(tau::str_format("Row ", i, ':', j));
                list.insert(br, tt, j);
            }
        }

        notebook_.append_page(list, pages_[pg].title);
        list.grab_focus();
    }

    void init_twins_page(int pg) {
        tau::Text label(pages_[pg].title);
        label.set_tooltip("This page shows\nthe Twins container example");
        tau::Frame frame(pages_[pg].title, tau::BORDER_SOLID, 1, 8);
        frame.hint_margin(5);
        frame.set_border_color(tau::Color("DeepSkyBlue"));
        frame.style().get("background").set("Lavender");
        notebook_.append_page(frame, label);

        tau::Table table;
        table.set_column_spacing(5);
        table.set_row_spacing(5);
        table.hint_margin(4);
        table.style().get("background").set("PeachPuff");
        frame.insert(table);

        {   tau::Twins twins(tau::OR_DOWN, 0.4);
            table.put(twins, 0, 0);
            tau::Text first("First@OR_DOWN"), second("Second@OR_DOWN");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        {   tau::Twins twins(tau::OR_UP, 0.4);
            table.put(twins, 0, 1);
            tau::Text first("First@OR_UP"), second("Second@OR_UP");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        {   tau::Twins twins(tau::OR_LEFT, 0.4);
            table.put(twins, 1, 0);
            tau::Text first("First@OR_LEFT"), second("Second@OR_LEFT");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }

        {   tau::Twins twins(tau::OR_RIGHT, 0.4);
            table.put(twins, 1, 1);
            tau::Text first("First@OR_RIGHT"), second("Second@OR_RIGHT");
            first.style().get("background").set("LightBlue");
            second.style().get("background").set("Lime");
            first.hint_margin(10);
            second.hint_margin(12);
            twins.insert_first(first);
            twins.insert_second(second);
        }
    }

    void init_controls_page(int pg) {
        tau::Table table;
        table.signal_take_focus().connect(tau::fun(std::function<bool()>([]() { return true; } )), true);
        table.set_column_spacing(5);
        table.set_row_spacing(5);
        table.hint_margin(4);
        table.align_column(7, tau::ALIGN_CENTER);
        notebook_.append_page(table, pages_[pg].title);

        int row = 0;

        {
            tau::Text label("tau::Button", tau::ALIGN_START);
            table.put(label, 0, row, 1, 1, true);
            tau::Icon ico("document-save", tau::MEDIUM_ICON);
            tau::Button push_button(ico);
            table.put(push_button, 7, row, 1, 1, true, true);
        }

        ++row;
        {
            tau::Text label("tau::Toggle", tau::ALIGN_START);
            table.put(label, 0, row);
            tau::Icon ico2("configure", tau::SMALL_ICON);
            tau::Toggle toggle_button(ico2, "Toggle button");
            table.put(toggle_button, 7, row, 1, 1, true, true);
        }

        ++row;
        {
            tau::Text label("tau::Counter", tau::ALIGN_START);
            table.put(label, 0, row);
            tau::Counter counter(kf_.get_integer(kf_.root(), "counter"), 247, 1);
            counter.append("rpm", 2, 2);
            table.put(counter, 7, row, 1, 1, true, true);
            auto on_changed = [](double val) { kf_.set_integer(kf_.root(), "counter", val); };
            counter.signal_value_changed().connect(tau::fun(std::function<void(double)>(on_changed)));
        }

        ++row;
        {
            tau::Text label("tau::Check(tau::CHECK_XSTYLE)", tau::ALIGN_START);
            table.put(label, 0, row, 4, 1);
            tau::Check check(tau::CHECK_XSTYLE, tau::BORDER_SOLID, kf_.get_boolean(kf_.root(), "xcheck"));
            table.put(check, 7, row, 1, 1, true, true);
            auto on_changed = [](bool checked) { kf_.set_boolean(kf_.root(), "xcheck", checked); };
            check.signal_check().connect(tau::bind(tau::fun(std::function<void(bool)>(on_changed)), true));
            check.signal_uncheck().connect(tau::bind(tau::fun(std::function<void(bool)>(on_changed)), false));
        }

        ++row;
        {
            tau::Text label("tau::Check(tau::CHECK_VSTYLE)", tau::ALIGN_START);
            table.put(label, 0, row, 4, 1);
            tau::Check check(tau::CHECK_VSTYLE, kf_.get_boolean(kf_.root(), "vcheck"));
            table.put(check, 7, row, 1, 1, true, true);
            auto on_changed = [](bool checked) { kf_.set_boolean(kf_.root(), "vcheck", checked); };
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

            for (int i = 0; 0 != bauds[i]; ++i) {
                tau::Text label(tau::str_format(bauds[i]));
                label.style().get("background").set("yellow");
                cycle.add(label);
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
    }

    void init_colors_page(int pg) {
        tau::Table table;
        table.set_column_spacing(6);
        table.set_row_spacing(5);
        tau::Scroller scroller;
        scroller.insert(table);
        scroller.hint_margin(3, 3, 4, 4);
        tau::Slider slider(scroller);
        tau::Box box(tau::OR_RIGHT, 2);
        tau::Text label(pages_[pg].title);
        label.set_tooltip("This page shows\nnamed colors");
        notebook_.append_page(box, label);
        box.append(scroller);
        box.append(slider, true);
        std::size_t row = 0;

        for (auto & cname: color_names_) {
            color_widgets_[row].name = new tau::Text(tau::ALIGN_START);
            color_widgets_[row].w = new tau::Widget;
            color_widgets_[row].w->hint_min_size(16, 0);
            color_widgets_[row].value = new tau::Text;

            table.put(*color_widgets_[row].name, 0, row, 1, 1, true, true);
            table.put(*color_widgets_[row].w, 1, row, 1, 1);
            table.put(*color_widgets_[row].value, 2, row);

            set_row_color(row, cname);
            ++row;
        }
    }

    void init_cursors_page(int pg) {
        tau::Table xtable;
        notebook_.append_page(xtable, pages_[pg].title);

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
        tau::ustring path = tau::path_build(tau::path_prefix_dir(), "share", "cursors");

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
    }

    void init_colorsel_page(int pg) {
        tau::Colorsel colorsel(tau::Color("Blue"));
        colorsel.hint_margin(4);
        notebook_.append_page(colorsel, pages_[pg].title);
    }

    void on_next_page() {
        notebook_.show_next();
    }

    void on_prev_page() {
        notebook_.show_prev();
    }

    void on_geometry_changed() {
        std::vector<long long> v = { position().x(), position().y(), size().iwidth(), size().iheight() };
        kf_.set_integers(kf_.section("main"), "geometry", v);
    }
};

void run() {
    try {
        auto v = kf_.get_integers(kf_.section("main"), "geometry");
        tau::Rect bounds;
        if (v.size() > 3) { bounds.set(tau::Point(v[0], v[1]), tau::Size(v[2], v[3])); }

        Main wnd(kf_, bounds);
        wnd.set_title("TAU Demo");
        tau::Loop().run();
    }

    catch (tau::exception & x) {
        std::cerr << "** taudemo: run(): tau::exception thrown: " << x.what() << std::endl;
    }
}

int main(int argc, char * argv[]) {
    tau::ustring conf_path = tau::path_build(tau::path_user_config_dir(), tau::program_name(), "state.ini");
    std::ifstream is(conf_path.c_str());
    kf_.load(is);

    //threads_.emplace_back(std::thread(run));
    run();
    for (auto & thr: threads_) { thr.join(); }

    try {
        path_mkdir(path_dirname(conf_path));
        std::ofstream os(conf_path.c_str());
        kf_.save(os);
    }

    catch (tau::exception & x) {
        std::cerr << "** taudemo: main(): tau::exception thrown: " << x.what() << std::endl;
    }

    return 0;
}

//END
