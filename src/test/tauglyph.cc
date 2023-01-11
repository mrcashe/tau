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
#include <climits>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>

tau::Point pos_ = { 200, 200 };
tau::Size  sz_  = { 700, 500 };

class Main: public tau::Toplevel {
    tau::Key_file &     kf_;
    tau::ustring        font_spec_;
    char32_t            gchar_ = 32;
    tau::Matrix         mat_;

    tau::Action         escape_action_ { "Escape", tau::fun(this, &Main::close) };
    tau::Action         prev_action_ { "Left", tau::fun(this, &Main::on_glyph_prev) };
    tau::Action         next_action_ { "Right", tau::fun(this, &Main::on_glyph_next) };
    tau::Action         zin_;
    tau::Action         zout_;
    tau::Action         view_ctr_action_ { "F2", tau::fun(this, &Main::on_view_ctr) };
    tau::Action         view_pix_action_ { "F3", tau::fun(this, &Main::on_view_pix) };
    tau::Widget         area_;
    tau::Painter        painter_;

    tau::Text           font_label_;
    tau::Text           char_label_;
    tau::Text           code_label_;
    tau::Text           gxbear_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gybear_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gxmin_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gymin_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gxmax_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gymax_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gxadv_label_ { "0.00", tau::ALIGN_END };
    tau::Text           gyadv_label_ { "0.00", tau::ALIGN_END };

    tau::Toggle         ctr_button_ { "View Contour" };
    tau::Toggle         pix_button_ { "View Pixmap" };
    bool                ctr_visible_ = true;
    bool                pix_visible_ = true;

    tau::Glyph          glyph_;
    tau::Font           font_;
    tau::Pixmap         gpix_;

    tau::Absolute       absolute_;
    tau::ustring        test_text_ { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

    int                 ascent_ = 0;
    int                 descent_ = 0;
    int                 npx_ = 0;
    int                 npy_ = 0;
    int                 gxmin_ = 0;
    int                 gymin_ = 0;
    int                 gxmax_ = 0;
    int                 gymax_ = 0;
    double              scale_ = 1.0;
    double              x0_ = 0.0;
    double              y0_ = 0.0;
    double              ox_ = 0.0;
    double              oy_ = 0.0;

public:

    Main(tau::Key_file & kf):
        Toplevel(tau::Rect(pos_, sz_)),
        kf_(kf)
    {
        set_title("TAU Glyph Viewer");
        area_.hint_min_size(60);
        gchar_ = kf_.get_integer(kf_.section("main"), "gchar", 32);
        ctr_visible_ = kf_.get_boolean(kf_.section("view"), "contour", true);
        pix_visible_ = kf_.get_boolean(kf_.section("view"), "pixmap", true);
        if (ctr_visible_) { ctr_button_.toggle(); }
        if (pix_visible_) { pix_button_.toggle(); }

        kf_.set_boolean(kf_.section("view"), "contour", ctr_visible_);
        signal_position_changed().connect(tau::fun(this, &Main::on_geometry_changed));
        signal_size_changed().connect(tau::fun(this, &Main::on_geometry_changed));

        connect_action(escape_action_);
        connect_action(prev_action_);
        connect_action(next_action_);

        zin_.add_accels("<Ctrl>+ <Ctrl>=");
        zin_.connect(tau::fun(this, &Main::increase_font));
        connect_action(zin_);

        zout_.add_accels("<Ctrl>-");
        zout_.connect(tau::fun(this, &Main::decrease_font));
        connect_action(zout_);

        connect_action(view_ctr_action_);
        connect_action(view_pix_action_);

        area_.signal_paint().connect(tau::fun(this, &Main::on_area_paint));
        area_.signal_mouse_wheel().connect(tau::fun(this, &Main::on_area_mouse_wheel));
        area_.signal_size_changed().connect(tau::fun(this, &Main::update_font));
        area_.signal_size_changed().connect(tau::fun(this, &Main::fetch_area_painter));
        area_.signal_parent().connect(tau::fun(this, &Main::fetch_area_painter));

        font_spec_ = kf_.get_string(kf.section("main"), "font");
        if (!font_spec_.empty()) { area_.style().font("font").set(font_spec_); }
        else { font_spec_ = style().font("font").spec(); }

        // Top box (vertical).
        tau::Box box0(tau::OR_DOWN);

        // Central box (horizontal) including view area (left) and info pane (right).
        tau::Box cbox(tau::OR_RIGHT, 4);
        box0.append(cbox);

        {
            area_.hint_margin(2);
            tau::Frame frame("Glyph View", tau::BORDER_GROOVE);
            frame.insert(area_);
            frame.hint_margin(4, 2, 0, 0);
            cbox.append(frame);
        }

        // Info box (vertical) inserted into central box.
        tau::Box ibox(tau::OR_DOWN, 7);
        ibox.set_align(tau::ALIGN_CENTER);
        ibox.hint_margin(0, 4, 0, 0);
        cbox.append(ibox, true);

        {   // Glyphinfo table being inserted into info frame.
            tau::Table itable;
            itable.set_column_spacing(2);
            itable.set_row_spacing(2);
            itable.hint_margin(2);

            {   // Info frame inserted into info box.
                tau::Frame frame("Glyph Info", tau::BORDER_GROOVE);
                frame.insert(itable);
                ibox.append(frame, true);
            }

            {   // Glyph minimal labels.
                tau::Text label("Minimal", tau::ALIGN_START, tau::ALIGN_CENTER);
                itable.put(label, 0, 0, 1, 2, true);
                tau::Text x("x:"); itable.put(x, 1, 0, 1, 1, true, true);
                tau::Text y("y:"); itable.put(y, 1, 1, 1, 1, true, true);
                itable.put(gxmin_label_, 2, 0, 1, 1);
                itable.put(gymin_label_, 2, 1, 1, 1);
            }

            {   // Glyph maximal labels.
                tau::Text label("Maximal", tau::ALIGN_START, tau::ALIGN_CENTER);
                itable.put(label, 0, 2, 1, 2, true);
                tau::Text x("x:"); itable.put(x, 1, 2, 1, 1, true, true);
                tau::Text y("y:"); itable.put(y, 1, 3, 1, 1, true, true);
                itable.put(gxmax_label_, 2, 2, 1, 1);
                itable.put(gymax_label_, 2, 3, 1, 1);
            }

            {   // Glyph advance labels.
                tau::Text label("Advance", tau::ALIGN_START, tau::ALIGN_CENTER);
                itable.put(label, 0, 4, 1, 2, true);
                tau::Text x("x:"); itable.put(x, 1, 4, 1, 1, true, true);
                tau::Text y("y:"); itable.put(y, 1, 5, 1, 1, true, true);
                itable.put(gxadv_label_, 2, 4, 1, 1);
                itable.put(gyadv_label_, 2, 5, 1, 1);
            }

            {   // Glyph bearing labels.
                tau::Text label("Bearing", tau::ALIGN_START, tau::ALIGN_CENTER);
                itable.put(label, 0, 6, 1, 2, true);
                tau::Text x("x:"); itable.put(x, 1, 6, 1, 1, true, true);
                tau::Text y("y:"); itable.put(y, 1, 7, 1, 1, true, true);
                itable.put(gxbear_label_, 2, 6, 1, 1);
                itable.put(gybear_label_, 2, 7, 1, 1);
            }
        }

        // View Contour button.
        ctr_button_.hint_margin(0, 0, 2, 2);
        ctr_button_.signal_toggle().connect(fun(this, &Main::on_ctr_button_toggle));
        ibox.append(ctr_button_, true);

        // View Pixmap button.
        pix_button_.hint_margin(0, 0, 2, 2);
        pix_button_.signal_toggle().connect(fun(this, &Main::on_pix_button_toggle));
        ibox.append(pix_button_, true);

        {   // Test text absolute container.
            tau::Box abox(tau::OR_RIGHT);
            abox.style().get("background").set("Black");
            abox.set_align(tau::ALIGN_CENTER);
            absolute_.hint_min_size(tau::Size(0, 10));
            abox.append(absolute_, true);
            box0.append(abox, true);
        }

        {   // Status bar frame.
            tau::Frame frame(tau::BORDER_RIDGE);
            frame.hint_margin(4, 4, 0, 0);
            box0.append(frame, true);

            // Status box (horizontal).
            tau::Box status(tau::OR_RIGHT, 3);
            status.style().font("font").enlarge(-2);
            status.hint_margin(2);
            frame.insert(status);

            {   // Font status item.
                tau::Text txt("Font:");
                status.append(txt, true);
                tau::Frame frame(tau::BORDER_INSET);
                frame.set_border(4, 2, 2, 2);
                status.append(frame, true);
                font_label_.hint_margin(4, 4, 0, 0);
                font_label_.assign(font_spec_);
                frame.insert(font_label_);
            }

            {   // Character status item.
                tau::Text txt("Character:");
                status.append(txt, true);
                tau::Frame frame(tau::BORDER_INSET);
                frame.set_border(4, 2, 2, 2);
                status.append(frame, true);
                char_label_.hint_margin(4, 4, 0, 0);
                char_label_.assign(tau::ustring(1, gchar_));
                frame.insert(char_label_);
            }

            {   // Character code status item.
                tau::Text txt("Char Code:");
                status.append(txt, true);
                tau::Frame frame(tau::BORDER_INSET);
                frame.set_border(4, 2, 2, 2);
                status.append(frame, true);
                code_label_.hint_margin(4, 4, 0, 0);
                code_label_.assign(tau::key_code_to_string(gchar_));
                frame.insert(code_label_);
            }
        }

        insert(box0);
        update_glyph();
        set_icon("tau-48");
    }

private:

    tau::Pixmap raster_glyph(tau::Glyph glyph) {
        tau::Rect r = glyph.bounds();
        tau::Pixmap pix(32, r.size());

        if (auto pr = pix.painter()) {
            pr.set_brush(tau::Color("Blue"));
            pr.paint();
            pr.move_to(-r.left(), std::ceil(glyph.max().y()));
            pr.glyph(glyph);
            pr.set_brush(tau::Color("Gold"));
            pr.fill();
        }

        return pix;
    }

    void on_geometry_changed() {
        sz_ = size();
        pos_  = position();
    }

    void increase_font() {
        tau::ustring spec = area_.style().font("font").spec();
        double pt = tau::font_size_from_spec(spec);

        if (pt < 100.0) {
            pt += 1.0;
            spec = tau::font_size_change(spec, pt);
            font_spec_ = spec;
            update_font();
        }
    }

    void decrease_font() {
        tau::ustring spec = area_.style().font("font").spec();
        double pt = tau::font_size_from_spec(spec);

        if (pt >= 6.0) {
            pt -= 1.0;
            spec = tau::font_size_change(spec, pt);
            font_spec_ = spec;
            update_font();
        }
    }

    void update_test_text() {
        absolute_.clear();

        int xx = 0;
        unsigned hmax = 0;
        std::vector<tau::Pixmap> pixx;
        std::vector<tau::Point> pts;

        for (char32_t wc: test_text_) {
            tau::Painter pr = absolute_.painter();

            if (pr) {
                tau::Font font = pr.select_font(font_spec_);

                if (font) {
                    if (tau::Glyph glyph = font.glyph(wc)) {
                        tau::Pixmap pix = raster_glyph(glyph);
                        pixx.push_back(pix);
                        hmax = std::max(hmax, pix.size().height());
                        int x = xx+std::floor(glyph.bearing().x());
                        //int y = std::floor(glyph->max().y());
                        pts.emplace_back(x, 0);
                        tau::Vector adv = glyph.advance();
                        int x_off = std::ceil(adv.x());
                        xx += x_off;
                    }
                }
            }
        }

        xx = 0;
        absolute_.hint_size(tau::Size(0, std::max(10U, hmax)));

        for (auto pix: pixx) {
            tau::Image img(pix, false);
            absolute_.put(img, pts[xx++]);
        }
    }

    void update_font() {
        font_label_.assign(font_spec_);
        area_.style().font("font").set(font_spec_);
        kf_.set_string(kf_.section("main"), "font", font_spec_);

        if (auto pr = painter()) {
            font_ = pr.select_font(font_spec_);

            if (font_) {
                update_test_text();
                ascent_  = std::ceil(font_.ascent());
                descent_ = std::floor(font_.descent());
                npy_ = ascent_-descent_;
                glyph_ = pr.font().glyph(gchar_);
                update_glyphinfo();
                gpix_ = tau::Pixmap(32);

                std::size_t width = area_.size().width();
                std::size_t height = area_.size().height();
                y0_ = npy_+descent_;
                tau::Vector fmin = glyph_.min()-tau::Vector(1, 1);
                tau::Vector fmax = glyph_.max()+tau::Vector(1, 1);
                npx_ = std::max(1, int(std::ceil(fmax.x())-std::ceil(fmin.x())));
                x0_ = -std::ceil(fmin.x());
                scale_ = std::min(width/npx_, height/npy_);

                tau::Vector gmin = glyph_.min();
                tau::Vector gmax = glyph_.max();
                gxmin_ = x0_+std::floor(gmin.x());
                gymin_ = y0_-std::floor(gmin.y());
                gxmax_ = x0_+std::ceil(gmax.x());
                gymax_ = y0_-std::ceil(gmax.y());
                ox_ = 0.5*(width-(npx_*scale_));
                oy_ = 0.5*(height-(npy_*scale_));

                mat_ = tau::Matrix();
                mat_.translate(ox_, oy_);
                mat_.scale(scale_);

                redraw_area();
            }
        }
    }

    void redraw_area() {
        area_.invalidate();
    }

    void update_glyph() {
        kf_.set_integer(kf_.section("main"), "gchar", gchar_);
        char_label_.assign(tau::ustring(1, gchar_));
        code_label_.assign(tau::key_code_to_string(gchar_));
        update_font();
    }

    void next_glyph() {
        char32_t gc = gchar_;

        if (0x11ffff != gc) {
            if (0x007e == gc) {
                gc = 0x00a0;
            }

            else {
                ++gc;
            }

            if (gc != gchar_) {
                gchar_ = gc;
                update_glyph();
            }
        }
    }

    void prev_glyph() {
        char32_t gc = gchar_;

        if (0x0000 != gc) {
            if (0x00a0 == gc) {
                gc = 0x007e;
            }

            else {
                --gc;
            }

            if (gc != gchar_) {
                gchar_ = gc;
                update_glyph();
            }
        }
    }

    void on_glyph_prev() {
        prev_glyph();
    }

    void on_glyph_next() {
        next_glyph();
    }

    void fetch_area_painter() {
        if (auto pr = area_.painter()) {
            painter_ = pr;
        }
    }

    bool on_area_mouse_wheel(int delta, uint16_t mb_all, const tau::Point & where) {
        if ((tau::MM_CONTROL|tau::MM_RIGHT) & mb_all) {
            if (delta > 0) {
                increase_font();
            }

            else if (delta < 0) {
                decrease_font();
            }
        }

        else {
            if (delta > 0) {
                next_glyph();
            }

            else if (delta < 0) {
                prev_glyph();
            }
        }

        return true;
    }

    void update_glyphinfo() {
        if (glyph_) {
            gxmin_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.min().x()));
            gymin_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.min().y()));
            gxmax_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.max().x()));
            gymax_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.max().y()));
            gxadv_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.advance().x()));
            gyadv_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.advance().y()));
            gxbear_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.bearing().x()));
            gybear_label_.assign(tau::str_format(std::fixed, std::setprecision(2), glyph_.bearing().y()));
        }

        else {
            gxmin_label_.assign("0.00");
            gymin_label_.assign("0.00");
            gxmax_label_.assign("0.00");
            gymax_label_.assign("0.00");
            gxadv_label_.assign("0.00");
            gyadv_label_.assign("0.00");
            gxbear_label_.assign("0.00");
            gybear_label_.assign("0.00");
        }
    }

    void draw_contours(tau::Painter pr) {
        for (auto & ctr: glyph_.contours()) {
            double bd = 3/scale_;
            tau::Vector cur = ctr.start(), end;
            tau::Color line_color = tau::Color("DarkOrange");
            tau::Color cv_color = tau::Color("Orange");

            for (auto curve: ctr) {
                end = curve.end();
                pr.rectangle(x0_+cur.x()+bd, y0_-cur.y()+bd, x0_+cur.x()-bd, y0_-cur.y()-bd);
                pr.set_pen(tau::Pen(line_color, 2, tau::SOLID_LINE, tau::ROUND_CAP));
                pr.fill();

                if (2 == curve.order()) {
                    tau::Vector cp = curve.cp1();
                    pr.rectangle(x0_+cp.x()+bd, y0_-cp.y()+bd, x0_+cp.x()-bd, y0_-cp.y()-bd);
                    pr.set_brush(cv_color);
                    pr.fill();

                    pr.move_to(x0_+cur.x(), y0_-cur.y());
                    pr.conic_to(x0_+cp.x(), y0_-cp.y(), x0_+end.x(), y0_-end.y());
                    pr.set_pen(tau::Pen(cv_color, 2, tau::SOLID_LINE, tau::ROUND_CAP));
                    pr.stroke();
                }

                else {
                    pr.move_to(x0_+cur.x(), y0_-cur.y());
                    pr.line_to(x0_+end.x(), y0_-end.y());
                    pr.set_pen(tau::Pen(line_color, 2, tau::SOLID_LINE, tau::ROUND_CAP));
                    pr.stroke();
                }

                cur = end;
            }
        }
    }

    void draw_pixmap(tau::Painter pr) {
        for (int x = 0; x < npx_; ++x) {
            double bd = 0.05;

            for (int y = 0; y < npy_; ++y) {
                tau::Color c = tau::Color("PowderBlue");

                if (!gpix_.empty()) {
                    if (x >= gxmin_ && x < gxmax_ && y >= gymax_ && y < gymin_) {
                        int px = x-gxmin_, py = y-gymax_;
                        c = gpix_.get_pixel(px, py);
                    }
                }

                pr.rectangle(x+bd, y+bd, x-bd+1.0, y-bd+1.0);
                pr.set_brush(c);
                pr.fill();
            }
        }
    }

    void draw_grid(tau::Painter pr) {
        tau::Matrix m = mat_.inverted();
        tau::Vector vn = m*tau::Vector(0, 0);
        tau::Vector vk = m*tau::Vector(area_.size().width(), area_.size().height());

        pr.push();
        pr.move_to(vn.x(), y0_);
        pr.line_to(vk.x(), y0_);
        pr.move_to(x0_, vn.y());
        pr.line_to(x0_, vk.y());
        pr.set_pen(tau::Pen(tau::Color("Black"), 4, tau::DASH_LINE));
        pr.stroke();

        if (vk.y() > vn.y()) {
            for (double y = std::floor(vn.y()); y < vk.y(); y += 1.0) {
                pr.move_to(vn.x(), y);
                pr.line_to(vk.x(), y);
            }
        }

        else {
            for (double y = std::floor(vk.y()); y < vn.y(); y += 1.0) {
                pr.move_to(vn.x(), y);
                pr.line_to(vk.x(), y);
            }
        }

        for (double x = std::ceil(vn.x()); x < vk.x(); x += 1.0) {
            pr.move_to(x, vn.y());
            pr.line_to(x, vk.y());
        }

        pr.set_pen(tau::Pen(tau::Color("Gray"), 0, tau::DASH_LINE));
        pr.stroke();
        pr.pop();
    }

    bool on_area_paint(tau::Painter pr, const tau::Rect & inval) {
        if (glyph_) {
            pr.matrix() = mat_;
            draw_grid(pr);

            if (pix_visible_) {
                gpix_ = raster_glyph(glyph_);
                draw_pixmap(pr);
            }

            if (ctr_visible_) {
                draw_contours(pr);
            }
        }

        return true;
    }

    void on_view_ctr() {
        if (ctr_visible_) {
            ctr_visible_ = false;
            if (ctr_button_.toggled()) { ctr_button_.toggle(); }
        }

        else {
            ctr_visible_ = true;
            if (!ctr_button_.toggled()) { ctr_button_.toggle(); }
        }

        kf_.set_boolean(kf_.section("view"), "contour", ctr_visible_);
        redraw_area();
    }

    void on_view_pix() {
        if (pix_visible_) {
            pix_visible_ = false;
            if (pix_button_.toggled()) { pix_button_.toggle(); }
        }

        else {
            pix_visible_ = true;
            if (!pix_button_.toggled()) { pix_button_.toggle(); }
        }

        kf_.set_boolean(kf_.section("view"), "pixmap", pix_visible_);
        redraw_area();
    }

    void on_ctr_button_toggle(bool toggled) {
        ctr_visible_ = toggled;
        kf_.set_boolean(kf_.section("view"), "contour", ctr_visible_);
        redraw_area();
    }

    void on_pix_button_toggle(bool toggled) {
        pix_visible_ = toggled;
        kf_.set_boolean(kf_.section("view"), "pixmap", pix_visible_);
        redraw_area();
    }

};

int main(int argc, char * argv[]) {
    try {
        tau::ustring conf_path = tau::path_build(tau::path_user_config_dir(), tau::program_name(), "state.ini");
        std::ifstream is(conf_path.c_str(), std::ios::in);
        tau::Key_file kf(is);

        auto v = kf.get_integers(kf.section("main"), "geometry");
        if (v.size() > 3) {
            pos_.set(v[0], v[1]);
            sz_.update(v[2], v[3]);
        }

        Main wnd(kf);
        tau::Loop().run();

        kf.set_comment(kf.root(), "This is automatically generated file, do not edit");
        std::vector<long long> iv = { pos_.x(), pos_.y(), sz_.iwidth(), sz_.iheight() };
        kf.set_integers(kf.section("main"), "geometry", iv);

        path_mkdir(path_dirname(conf_path));
        std::ofstream os(conf_path.c_str(), std::ios::out);
        kf.save(os);
    }

    catch (tau::exception & x) {
        std::cerr << "** tau::exception thrown: " << x.what() << std::endl;
    }

    return 0;
}


//END
