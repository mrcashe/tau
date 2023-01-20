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

#include <tau/brush.hh>
#include <tau/painter.hh>
#include <tau/pen.hh>
#include <box-impl.hh>
#include <button-impl.hh>
#include <check-impl.hh>
#include <colorsel-impl.hh>
#include <counter-impl.hh>
#include <entry-impl.hh>
#include <frame-impl.hh>
#include <image-impl.hh>
#include <pixmap-impl.hh>
#include <scroller-impl.hh>
#include <slider-impl.hh>
#include <cctype>
#include <iostream>

namespace tau {

Colorsel_impl::Colorsel_impl(const Color & initial_color):
    Table_impl(),
    color_(initial_color)
{
    allow_focus();
    set_row_spacing(4);
    set_column_spacing(4);
    signal_take_focus_.connect(fun(this, &Colorsel_impl::grab_focus), true);

    hue_check_ = std::make_shared<Check_impl>(CHECK_RSTYLE);
    hue_check_->signal_check().connect(fun(this, &Colorsel_impl::fill1));
    hue_check_->signal_check().connect(fun(this, &Colorsel_impl::fill2));
    put(hue_check_, 0, 5, 1, 1, true, true);

    saturation_check_ = std::make_shared<Check_impl>(CHECK_RSTYLE);
    saturation_check_->signal_check().connect(fun(this, &Colorsel_impl::fill1));
    saturation_check_->signal_check().connect(fun(this, &Colorsel_impl::fill2));
    put(saturation_check_, 0, 6, 1, 1, true, true);
    saturation_check_->join(hue_check_);

    value_check_ = std::make_shared<Check_impl>(CHECK_RSTYLE, true);
    value_check_->signal_check().connect(fun(this, &Colorsel_impl::fill1));
    value_check_->signal_check().connect(fun(this, &Colorsel_impl::fill2));
    put(value_check_, 0, 7, 1, 1, true, true);
    value_check_->join(hue_check_);

    red_check_ = std::make_shared<Check_impl>(CHECK_RSTYLE);
    red_check_->signal_check().connect(fun(this, &Colorsel_impl::fill1));
    red_check_->signal_check().connect(fun(this, &Colorsel_impl::fill2));
    red_check_->hint_margin_left(8);
    put(red_check_, 3, 5, 1, 1, true, true);
    red_check_->join(hue_check_);

    green_check_ = std::make_shared<Check_impl>(CHECK_RSTYLE);
    green_check_->signal_check().connect(fun(this, &Colorsel_impl::fill1));
    green_check_->signal_check().connect(fun(this, &Colorsel_impl::fill2));
    green_check_->hint_margin_left(8);
    put(green_check_, 3, 6, 1, 1, true, true);
    green_check_->join(hue_check_);

    blue_check_ = std::make_shared<Check_impl>(CHECK_RSTYLE);
    blue_check_->signal_check().connect(fun(this, &Colorsel_impl::fill1));
    blue_check_->signal_check().connect(fun(this, &Colorsel_impl::fill2));
    blue_check_->hint_margin_left(8);
    put(blue_check_, 3, 7, 1, 1, true, true);
    blue_check_->join(hue_check_);

    Text_ptr text = std::make_shared<Text_impl>("Hue", ALIGN_START);
    text->hint_margin_left(4);
    put(text, 1, 5, 1, 1, false, true);

    text = std::make_shared<Text_impl>("Saturation", ALIGN_START);
    text->hint_margin_left(4);
    put(text, 1, 6, 1, 1, false, true);

    text = std::make_shared<Text_impl>("Value", ALIGN_START);
    text->hint_margin_left(4);
    put(text, 1, 7, 1, 1, false, true);

    text = std::make_shared<Text_impl>("Red", ALIGN_START);
    text->hint_margin_left(4);
    put(text, 4, 5, 1, 1, false, true);

    text = std::make_shared<Text_impl>("Green", ALIGN_START);
    text->hint_margin_left(4);
    put(text, 4, 6, 1, 1, false, true);

    text = std::make_shared<Text_impl>("Blue", ALIGN_START);
    text->hint_margin_left(4);
    put(text, 4, 7, 1, 1, false, true);

    hue_counter_ = std::make_shared<Counter_impl>(std::ceil(color_.hue()), 359.0);
    hue_counter_->set_fixed();
    hue_counter_->append("\u00b0");
    hue_counter_->signal_value_changed().connect(fun(this, &Colorsel_impl::on_hue_counter_changed));
    put(hue_counter_, 2, 5, 1, 1, true, true);
    align(hue_counter_.get(), ALIGN_FILL, ALIGN_CENTER);

    saturation_counter_ = std::make_shared<Counter_impl>(std::ceil(255*color_.saturation()), 255.0);
    saturation_counter_->set_fixed();
    saturation_counter_->signal_value_changed().connect(fun(this, &Colorsel_impl::on_saturation_counter_changed));
    put(saturation_counter_, 2, 6, 1, 1, true, true);
    align(saturation_counter_.get(), ALIGN_FILL, ALIGN_CENTER);

    value_counter_ = std::make_shared<Counter_impl>(std::ceil(255*color_.value()), 255.0);
    value_counter_->set_fixed();
    value_counter_->signal_value_changed().connect(fun(this, &Colorsel_impl::on_value_counter_changed));
    put(value_counter_, 2, 7, 1, 1, true, true);
    align(value_counter_.get(), ALIGN_FILL, ALIGN_CENTER);

    red_counter_ = std::make_shared<Counter_impl>(std::ceil(255*color_.red()), 255.0);
    red_counter_->set_fixed();
    red_counter_->signal_value_changed().connect(fun(this, &Colorsel_impl::on_red_counter_changed));
    put(red_counter_, 5, 5, 1, 1, true, true);
    align(red_counter_.get(), ALIGN_FILL, ALIGN_CENTER);

    green_counter_ = std::make_shared<Counter_impl>(std::ceil(255*color_.green()), 255.0);
    green_counter_->set_fixed();
    green_counter_->signal_value_changed().connect(fun(this, &Colorsel_impl::on_green_counter_changed));
    put(green_counter_, 5, 6, 1, 1, true, true);
    align(green_counter_.get(), ALIGN_FILL, ALIGN_CENTER);

    blue_counter_ = std::make_shared<Counter_impl>(std::ceil(255*color_.blue()), 255.0);
    blue_counter_->set_fixed();
    blue_counter_->signal_value_changed().connect(fun(this, &Colorsel_impl::on_blue_counter_changed));
    put(blue_counter_, 5, 7, 1, 1, true, true);
    align(blue_counter_.get(), ALIGN_FILL, ALIGN_CENTER);

    pm1_ = Pixmap_impl::create(24);
    pm2_ = Pixmap_impl::create(24);

    img1_ = std::make_shared<Image_impl>(pm1_);
    img1_->signal_size_changed().connect(fun(this, &Colorsel_impl::on_img1_size_changed));
    img1_->signal_mouse_down().connect(fun(this, &Colorsel_impl::on_img1_mouse_down));
    img1_->signal_mouse_motion().connect(fun(this, &Colorsel_impl::on_img1_mouse_motion));
    img1_->signal_paint().connect(fun(this, &Colorsel_impl::on_img1_paint));
    put(img1_, 0, 0, 5, 5, false, false);

    img2_ = std::make_shared<Image_impl>(pm2_);
    img2_->hint_margin(12, 12, 0, 0);
    img2_->signal_size_changed().connect(fun(this, &Colorsel_impl::on_img2_size_changed));
    img2_->signal_mouse_down().connect(fun(this, &Colorsel_impl::on_img2_mouse_down));
    img2_->signal_mouse_motion().connect(fun(this, &Colorsel_impl::on_img2_mouse_motion));
    img2_->signal_paint().connect(fun(this, &Colorsel_impl::on_img2_paint));
    put(img2_, 5, 0, 1, 5, false, false);

    Frame_ptr frame = std::make_shared<Frame_impl>(BORDER_INSET);
    put(frame, 6, 4, 2, 1, false, false);

    sample_ = std::make_shared<Widget_impl>();
    sample_->style().color("background") = color_;
    sample_->hint_margin(2);
    frame->insert(sample_);

    put(std::make_shared<Text_impl>("HTML: ", ALIGN_END), 6, 5, 1, 1, false, true);

    html_ = std::make_shared<Entry_impl>(color_.html(), ALIGN_CENTER, BORDER_NONE);
    html_cx_ = html_->signal_changed().connect(fun(this, &Colorsel_impl::on_html_changed));
    html_->signal_focus_in().connect(fun(this, &Colorsel_impl::on_html_focus_in));
    put(html_, 7, 5, 1, 1, false, true);

    // Reuse frame.
    frame = std::make_shared<Frame_impl>("Named Colors", ALIGN_CENTER, BORDER_GROOVE);
    put(frame, 6, 0, 2, 4, false, false);

    Box_ptr box = std::make_shared<Box_impl>(OR_RIGHT, 2);
    box->hint_margin(4);
    frame->insert(box);

    Scroller_ptr scroller = std::make_shared<Scroller_impl>();
    box->append(scroller);

    Slider_ptr vslider = std::make_shared<Slider_impl>(scroller, OR_SOUTH, true);
    box->append(vslider, true);

    ntable_ = std::make_shared<Table_impl>();
    ntable_->signal_size_changed().connect(fun(this, &Colorsel_impl::on_ntable_size_changed));
    scroller->insert(ntable_);

    Button_ptr button = std::make_shared<Button_impl>(apply_action_, ACTION_ALL);
    put(button, 7, 6, 1, 1, true, true);
    align(button.get(), ALIGN_FILL, ALIGN_CENTER);
    apply_action_.connect(fun(this, &Widget_impl::quit_dialog));
    connect_action(apply_action_);

    button = std::make_shared<Button_impl>(cancel_action_, ACTION_ALL);
    put(button, 7, 7, 1, 1, true, true);
    align(button.get(), ALIGN_FILL, ALIGN_CENTER);
    cancel_action_.connect(fun(this, &Widget_impl::quit_dialog));
    connect_action(cancel_action_);
}

void Colorsel_impl::set_color(const Color & color) {
    color_ = color;
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    update_all();
    signal_color_changed_(color_);
}

void Colorsel_impl::on_img1_size_changed() {
    Size sz = img1_->size();
    pm1_->resize(sz);
    buffer1_.resize(sz.width()*sz.height()*4);
    fill1();
}

void Colorsel_impl::on_img2_size_changed() {
    Size sz = img2_->size();
    pm2_->resize(sz);
    buffer2_.resize(sz.width()*sz.height()*4);
    fill2();
}

void Colorsel_impl::on_ntable_size_changed() {
    ntable_->clear();
    unsigned n_columns = ntable_->size().width()/18;

    if (n_columns > 0) {
        std::vector<ustring> names = Color::list_css_names();
        std::size_t cindex = 0;
        unsigned n_rows = names.size()/n_columns;
        if (0 != names.size()%n_columns) { ++n_rows; }

        for (unsigned y = 0; y < n_rows; ++y) {
            for (unsigned x = 0; cindex < names.size() && x < n_columns; ++x, ++cindex) {
                Widget_ptr wp = std::make_shared<Widget_impl>();
                wp->hint_size(16);
                wp->hint_margin(1);
                wp->style().color("background") = Color(names[cindex]);
                wp->set_tooltip(names[cindex]);
                wp->signal_mouse_down().connect(tau::bind(fun(this, &Colorsel_impl::on_named_mouse_down), wp.get()));
                ntable_->put(wp, x, y, 1, 1, true, true);
            }
        }
    }
}

void Colorsel_impl::on_hue_counter_changed(double v) {
    color_.set_hue(v);
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    red_counter_->set_value(std::ceil(255*color_.red()));
    green_counter_->set_value(std::ceil(255*color_.green()));
    blue_counter_->set_value(std::ceil(255*color_.blue()));
    sample_->style().color("background") = color_;
    fill1();
    fill2();
    signal_color_changed_(color_);
}

void Colorsel_impl::on_saturation_counter_changed(double v) {
    color_.set_saturation(v/255);
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    red_counter_->set_value(std::ceil(255*color_.red()));
    green_counter_->set_value(std::ceil(255*color_.green()));
    blue_counter_->set_value(std::ceil(255*color_.blue()));
    sample_->style().color("background") = color_;
    fill1();
    fill2();
    signal_color_changed_(color_);
}

void Colorsel_impl::on_value_counter_changed(double v) {
    color_.set_value(v/255);
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    red_counter_->set_value(std::ceil(255*color_.red()));
    green_counter_->set_value(std::ceil(255*color_.green()));
    blue_counter_->set_value(std::ceil(255*color_.blue()));
    sample_->style().color("background") = color_;
    fill1();
    fill2();
    signal_color_changed_(color_);
}

void Colorsel_impl::on_red_counter_changed(double v) {
    color_.set_red(v/255);
    hue_counter_->set_value(std::ceil(color_.hue()));
    saturation_counter_->set_value(std::ceil(255*color_.saturation()));
    value_counter_->set_value(std::ceil(255*color_.value()));
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    sample_->style().color("background") = color_;
    fill1();
    fill2();
    signal_color_changed_(color_);
}

void Colorsel_impl::on_green_counter_changed(double v) {
    color_.set_green(v/255);
    hue_counter_->set_value(std::ceil(color_.hue()));
    saturation_counter_->set_value(std::ceil(255*color_.saturation()));
    value_counter_->set_value(std::ceil(255*color_.value()));
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    sample_->style().color("background") = color_;
    fill1();
    fill2();
    signal_color_changed_(color_);
}

void Colorsel_impl::on_blue_counter_changed(double v) {
    color_.set_blue(v/255);
    hue_counter_->set_value(std::ceil(color_.hue()));
    saturation_counter_->set_value(std::ceil(255*color_.saturation()));
    value_counter_->set_value(std::ceil(255*color_.value()));
    html_cx_.block();
    html_->assign(color_.html());
    html_cx_.unblock();
    sample_->style().color("background") = color_;
    fill1();
    fill2();
    signal_color_changed_(color_);
}

void Colorsel_impl::update_all() {
    sample_->style().color("background") = color_;
    hue_counter_->set_value(std::ceil(color_.hue()));
    saturation_counter_->set_value(std::ceil(255*color_.saturation()));
    value_counter_->set_value(std::ceil(255*color_.value()));
    red_counter_->set_value(std::ceil(255*color_.red()));
    green_counter_->set_value(std::ceil(255*color_.green()));
    blue_counter_->set_value(std::ceil(255*color_.blue()));
    fill1();
    fill2();
}

bool Colorsel_impl::on_named_mouse_down(int mbt, int mm, const Point & pt, Widget_impl * wi) {
    if (MBT_LEFT == mbt) {
        color_ = wi->style().color("background");
        html_cx_.block();
        html_->assign(color_.html());
        html_cx_.unblock();
        update_all();
        signal_color_changed_(color_);
        return true;
    }

    return false;
}

bool Colorsel_impl::on_img1_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        color_ = pm1_->get_pixel(pt);
        html_cx_.block();
        html_->assign(color_.html());
        html_cx_.unblock();
        update_all();
        signal_color_changed_(color_);
        return true;
    }

    return false;
}

void Colorsel_impl::on_img1_mouse_motion(unsigned shifts, const Point & pt) {
    if (MM_LEFT & shifts) {
        color_ = pm1_->get_pixel(pt);
        html_cx_.block();
        html_->assign(color_.html());
        html_cx_.unblock();
        update_all();
        signal_color_changed_(color_);
    }
}

bool Colorsel_impl::on_img1_paint(Painter pr, const Rect & inval) {
    Size sz = pm1_->size();

    if (sz) {
        double units_per_pixel_x, units_per_pixel_y;
        int x, y;

        if (hue_check_->checked()) {
            units_per_pixel_x = 1.0/sz.width();             // Saturation axis is X.
            units_per_pixel_y = 1.0/sz.height();            // Value axis is Y.
            x = color_.saturation()/units_per_pixel_x;
            y = (1.0-color_.value())/units_per_pixel_y;
        }

        else if (saturation_check_->checked()) {
            units_per_pixel_x = 359.0/sz.width();           // Hue axis is X.
            units_per_pixel_y = 1.0/sz.height();            // Value axis is Y.
            x = color_.hue()/units_per_pixel_x;
            y = (1.0-color_.value())/units_per_pixel_y;
        }

        else if (value_check_->checked()) {
            units_per_pixel_x = 359.0/sz.width();           // Hue axis is X.
            units_per_pixel_y = 1.0/sz.height();            // Saturation axis is Y.
            x = color_.hue()/units_per_pixel_x;
            y = (1.0-color_.saturation())/units_per_pixel_y;
        }

        else if (red_check_->checked()) {
            units_per_pixel_x = 1.0/sz.width();             // Green axis is X.
            units_per_pixel_y = 1.0/sz.height();            // Blue axis is Y.
            x = color_.green()/units_per_pixel_x;
            y = (1.0-color_.blue())/units_per_pixel_y;
        }

        else if (green_check_->checked()) {
            units_per_pixel_x = 1.0/sz.width();             // Red axis is X.
            units_per_pixel_y = 1.0/sz.height();            // Blue axis is Y.
            x = color_.red()/units_per_pixel_x;
            y = (1.0-color_.blue())/units_per_pixel_y;
        }

        else {
            units_per_pixel_x = 1.0/sz.width();             // Red axis is X.
            units_per_pixel_y = 1.0/sz.height();            // Green axis is Y.
            x = color_.red()/units_per_pixel_x;
            y = (1.0-color_.green())/units_per_pixel_y;
        }

        pr.rectangle(x-2, y-2, x+2, y+2);
        pr.set_brush(Color("White"));
        pr.fill();
    }

    return false;
}

bool Colorsel_impl::on_img2_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt) {
        color_ = pm2_->get_pixel(pt);
        html_cx_.block();
        html_->assign(color_.html());
        html_cx_.unblock();
        update_all();
        signal_color_changed_(color_);
        return true;
    }

    return false;
}

void Colorsel_impl::on_img2_mouse_motion(unsigned shifts, const Point & pt) {
    if (MM_LEFT & shifts) {
        color_ = pm2_->get_pixel(pt);
        html_cx_.block();
        html_->assign(color_.html());
        html_cx_.unblock();
        update_all();
        signal_color_changed_(color_);
    }
}

bool Colorsel_impl::on_img2_paint(Painter pr, const Rect & inval) {
    Size sz = pm2_->size();

    if (sz) {
        double units_per_pixel;
        int y;

        if (hue_check_->checked()) {
            units_per_pixel = 359.0/sz.height();
            y = (359.0-color_.hue())/units_per_pixel;
        }

        else if (saturation_check_->checked()) {
            units_per_pixel = 1.0/sz.height();
            y = (1.0-color_.saturation())/units_per_pixel;
        }

        else if (value_check_->checked()) {
            units_per_pixel = 1.0/sz.height();
            y = (1.0-color_.value())/units_per_pixel;
        }

        else if (red_check_->checked()) {
            units_per_pixel = 1.0/sz.height();
            y = (1.0-color_.red())/units_per_pixel;
        }

        else if (green_check_->checked()) {
            units_per_pixel = 1.0/sz.height();
            y = (1.0-color_.green())/units_per_pixel;
        }

        else {
            units_per_pixel = 1.0/sz.height();
            y = (1.0-color_.blue())/units_per_pixel;
        }

        pr.set_pen(Color("White"));
        pr.move_to(0, y);
        pr.line_to(sz.width(), y);
        pr.stroke();
    }

    return false;
}

void Colorsel_impl::on_html_changed(const ustring & s) {
    if (!s.empty() && (4 == s.size() || 7 == s.size()) && '#' == s[0]) {
        for (ustring::size_type i = 1; i < s.size(); ++i) {
            if (!std::isxdigit(s[i])) {
                return;
            }
        }

        color_.set(s);
        update_all();
        signal_color_changed_(color_);
    }
}

void Colorsel_impl::on_html_focus_in() {
    Buffer buf = html_->buffer();
    auto e = buf.cend();
    html_->select(buf.citer(0, 1), e);
    html_->move_to(e);
}

void Colorsel_impl::fill1() {
    Size sz = pm1_->size();
    double units_per_pixel_x, units_per_pixel_y;
    unsigned x, y;
    std::size_t index = 0;

    if (hue_check_->checked()) {
        units_per_pixel_x = 1.0/sz.width();             // Saturation axis is X.
        units_per_pixel_y = 1.0/sz.height();            // Value axis is Y.

        for (y = 0; y < sz.height(); ++y) {
            for (x = 0; x < sz.width(); ++x) {
                Color c(color_);
                c.set_saturation(x*units_per_pixel_x);
                c.set_value(1.0-y*units_per_pixel_y);
                uint32_t argb = c.argb32();
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb;
            }
        }
    }

    else if (saturation_check_->checked()) {
        units_per_pixel_x = 359.0/sz.width();           // Hue axis is X.
        units_per_pixel_y = 1.0/sz.height();            // Value axis is Y.

        for (y = 0; y < sz.height(); ++y) {
            for (x = 0; x < sz.width(); ++x) {
                Color c(color_);
                c.set_hue(x*units_per_pixel_x);
                c.set_value(1.0-y*units_per_pixel_y);
                uint32_t argb = c.argb32();
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb;
            }
        }
    }

    else if (value_check_->checked()) {
        units_per_pixel_x = 359.0/sz.width();           // Hue axis is X.
        units_per_pixel_y = 1.0/sz.height();            // Saturation axis is Y.

        for (y = 0; y < sz.height(); ++y) {
            for (x = 0; x < sz.width(); ++x) {
                Color c(color_);
                c.set_hue(x*units_per_pixel_x);
                c.set_saturation(1.0-y*units_per_pixel_y);
                uint32_t argb = c.argb32();
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb;
            }
        }
    }

    else if (red_check_->checked()) {
        units_per_pixel_x = 1.0/sz.width();             // Green axis is X.
        units_per_pixel_y = 1.0/sz.height();            // Blue axis is Y.

        for (y = 0; y < sz.height(); ++y) {
            for (x = 0; x < sz.width(); ++x) {
                Color c(color_);
                c.set_green(x*units_per_pixel_x);
                c.set_blue(1.0-y*units_per_pixel_y);
                uint32_t argb = c.argb32();
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb;
            }
        }
    }

    else if (green_check_->checked()) {
        units_per_pixel_x = 1.0/sz.width();             // Red axis is X.
        units_per_pixel_y = 1.0/sz.height();            // Blue axis is Y.

        for (y = 0; y < sz.height(); ++y) {
            for (x = 0; x < sz.width(); ++x) {
                Color c(color_);
                c.set_red(x*units_per_pixel_x);
                c.set_blue(1.0-y*units_per_pixel_y);
                uint32_t argb = c.argb32();
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb;
            }
        }
    }

    else {
        units_per_pixel_x = 1.0/sz.width();             // Red axis is X.
        units_per_pixel_y = 1.0/sz.height();            // Green axis is Y.

        for (y = 0; y < sz.height(); ++y) {
            for (x = 0; x < sz.width(); ++x) {
                Color c(color_);
                c.set_red(x*units_per_pixel_x);
                c.set_green(1.0-y*units_per_pixel_y);
                uint32_t argb = c.argb32();
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb; argb >>= 8;
                buffer1_[index++] = argb;
            }
        }
    }

    pm1_->set_argb32(Point(), buffer1_.data(), buffer1_.size());
}

void Colorsel_impl::fill2() {
    Size sz = pm2_->size();
    double units_per_pixel;
    unsigned x, y;
    std::size_t index = 0;

    if (hue_check_->checked()) {
        units_per_pixel = 359.0/sz.height();

        for (y = 0; y < sz.height(); ++y) {
            Color c(color_);
            c.set_hue(359.0-units_per_pixel*y);
            uint32_t argb = c.argb32();

            for (x = 0; x < sz.width(); ++x) {
                buffer2_[index++] = argb;
                buffer2_[index++] = argb >> 8;
                buffer2_[index++] = argb >> 16;
                buffer2_[index++] = argb >> 24;
            }
        }
    }

    else if (saturation_check_->checked()) {
        units_per_pixel = 1.0/sz.height();

        for (y = 0; y < sz.height(); ++y) {
            Color c(color_);
            c.set_saturation(1.0-units_per_pixel*y);
            uint32_t argb = c.argb32();

            for (x = 0; x < sz.width(); ++x) {
                buffer2_[index++] = argb;
                buffer2_[index++] = argb >> 8;
                buffer2_[index++] = argb >> 16;
                buffer2_[index++] = argb >> 24;
            }
        }
    }

    else if (value_check_->checked()) {
        units_per_pixel = 1.0/sz.height();

        for (y = 0; y < sz.height(); ++y) {
            Color c(color_);
            c.set_value(1.0-units_per_pixel*y);
            uint32_t argb = c.argb32();

            for (x = 0; x < sz.width(); ++x) {
                buffer2_[index++] = argb;
                buffer2_[index++] = argb >> 8;
                buffer2_[index++] = argb >> 16;
                buffer2_[index++] = argb >> 24;
            }
        }
    }

    else if (red_check_->checked()) {
        units_per_pixel = 1.0/sz.height();

        for (y = 0; y < sz.height(); ++y) {
            Color c(color_);
            c.set_red(1.0-units_per_pixel*y);
            uint32_t argb = c.argb32();

            for (x = 0; x < sz.width(); ++x) {
                buffer2_[index++] = argb;
                buffer2_[index++] = argb >> 8;
                buffer2_[index++] = argb >> 16;
                buffer2_[index++] = argb >> 24;
            }
        }
    }

    else if (green_check_->checked()) {
        units_per_pixel = 1.0/sz.height();

        for (y = 0; y < sz.height(); ++y) {
            Color c(color_);
            c.set_green(1.0-units_per_pixel*y);
            uint32_t argb = c.argb32();

            for (x = 0; x < sz.width(); ++x) {
                buffer2_[index++] = argb;
                buffer2_[index++] = argb >> 8;
                buffer2_[index++] = argb >> 16;
                buffer2_[index++] = argb >> 24;
            }
        }
    }

    else {
        units_per_pixel = 1.0/sz.height();

        for (y = 0; y < sz.height(); ++y) {
            Color c(color_);
            c.set_blue(1.0-units_per_pixel*y);
            uint32_t argb = c.argb32();

            for (x = 0; x < sz.width(); ++x) {
                buffer2_[index++] = argb;
                buffer2_[index++] = argb >> 8;
                buffer2_[index++] = argb >> 16;
                buffer2_[index++] = argb >> 24;
            }
        }
    }

    pm2_->set_argb32(Point(), buffer2_.data(), buffer2_.size());
}

} // namespace tau

//END
