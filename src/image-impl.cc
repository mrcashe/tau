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

#include <tau/brush.hh>
#include <tau/loop.hh>
#include <tau/painter.hh>
#include <image-impl.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <painter-impl.hh>
#include <pixmap-impl.hh>
#include <theme-impl.hh>
#include <iostream>

namespace tau {

Image_impl::Image_impl():
    Widget_impl()
{
    init();
}

Image_impl::Image_impl(Pixmap_cptr pix, bool transparent):
    Widget_impl()
{
    init();
    set_pixmap(pix, transparent);
}

Image_impl::Image_impl(Pixmap_ptr pix, bool transparent):
    Widget_impl()
{
    init();
    set_pixmap(pix, transparent);
}

Image_impl::Image_impl(const ustring & pixmap_name, bool transparent):
    Widget_impl(),
    transparent_(transparent),
    pixmap_name_(pixmap_name)
{
    init();
}

void Image_impl::init() {
    signal_paint_.connect(fun(this, &Image_impl::on_paint));
    signal_display_.connect(fun(this, &Image_impl::on_display));
    signal_enable_.connect(fun(this, &Image_impl::on_enable));
    signal_disable_.connect(fun(this, &Image_impl::on_disable));
}

void Image_impl::clear() {
    cfilm_.clear();
    film_.clear();
    cur_ = 0;
    redraw();
}

void Image_impl::add_pixmap(Pixmap_cptr pix, unsigned delay) {
    if (pix) {
        cfilm_.emplace_back();
        auto & ff = cfilm_.back();
        ff.pix = pix;
        ff.delay = delay;
        Size sz;
        for (auto & f: cfilm_) { sz |= f.pix->size(); }
        require_size(sz);
        redraw();
        start_timer_if_needed();
    }
}

void Image_impl::add_pixmap(Pixmap_ptr pix, unsigned delay) {
    if (pix) {
        film_.emplace_back();
        auto & ff = film_.back();
        ff.pix = pix;
        ff.delay = delay;
        ff.changed_cx = pix->signal_changed().connect(tau::bind(fun(this, &Image_impl::on_pix_changed), film_.size()-1));
        Size sz;
        for (auto & f: film_) { sz |= f.pix->size(); }
        require_size(sz);
        redraw();
        start_timer_if_needed();
    }
}

void Image_impl::set_pixmap(Pixmap_cptr pix, bool transparent) {
    transparent_ = transparent;

    if (pix) {
        clear();
        add_pixmap(pix);
    }
}

void Image_impl::set_pixmap(Pixmap_ptr pix, bool transparent) {
    transparent_ = transparent;

    if (pix) {
        clear();
        add_pixmap(pix);
    }
}

void Image_impl::reset_gray() {
    gray_.reset();
}

void Image_impl::create_gray() {
    if (!shut_ && !gray_ && (!cfilm_.empty() || !film_.empty())) {
        Pixmap_cptr orig = !cfilm_.empty() ? cfilm_.front().pix : film_.front().pix;

        if (32 == orig->depth() && transparent_) {
            gray_ = Pixmap_impl::create(32, orig->size());
            Color white(1.0, 1.0, 1.0, 0.33);

            for (int y = 0; y < orig->size().iheight(); ++y) {
                for (int x = 0; x < orig->size().iwidth(); ++x) {
                    Color c = orig->get_pixel(Point(x, y));
                    double a = c.alpha();
                    if (a > 0) { c.alpha_blend(white); }
                    double g = c.gray();
                    gray_->put_pixel(Point(x, y), Color(g, g, g, a));
                }
            }
        }

        else {
            gray_ = Pixmap_impl::create(8, orig);
        }
    }
}

void Image_impl::on_display() {
    if (cfilm_.empty() && !pixmap_name_.empty()) {
        if (auto pix = Theme_impl::root()->find_pixmap(pixmap_name_)) {
            set_pixmap(pix, transparent_);
        }
    }
}

void Image_impl::on_enable() {
    if (!shut_ && gray_ && !gray_cx_.empty() && (!cfilm_.empty() || !film_.empty())) {
        if (auto dp = display()) {
            if (auto loop = dp->loop()) {
                gray_cx_ = loop->signal_alarm(73054).connect(fun(this, &Image_impl::reset_gray));
            }
        }

        redraw();
    }
}

void Image_impl::on_disable() {
    gray_cx_.drop();
    if (!cfilm_.empty() || !film_.empty()) { create_gray(); redraw(); }
}

void Image_impl::on_timer() {
    std::size_t cur = cur_;

    if (!cfilm_.empty()) {
        if (cfilm_.size() > 1) {
            if (++cur_ >= cfilm_.size()) {
                cur_ = 0;
            }
        }
    }

    else if (film_.size() > 1) {
        if (++cur_ >= film_.size()) {
            cur_ = 0;
        }
    }

    if (cur != cur_) {
        timer_.start(calc_delay());
        redraw();
    }
}

void Image_impl::redraw() {
    if (visible()) {
        if (auto pr = painter()) {
            paint_pixmap(pr);
        }

        else {
            invalidate();
        }
    }
}

void Image_impl::on_pix_changed(std::size_t index) {
    if (index < film_.size() && cur_ == index) {
        gray_.reset();
        redraw();
    }
}

void Image_impl::paint_pixmap(Painter pr) {
    if (irect_) {
        pr.set_brush(Color(style().color(STYLE_BACKGROUND)));
        pr.rectangle(irect_.left(), irect_.top(), irect_.right(), irect_.bottom());
        pr.fill();
    }

    if (!enabled()) { create_gray(); }

    Pixmap_cptr pix = nullptr;
    if (!enabled()) { pix = gray_; }
    else if (!film_.empty() && cur_ < film_.size()) { pix = film_[cur_].pix; }
    else if (!cfilm_.empty() && cur_ < cfilm_.size()) { pix = cfilm_[cur_].pix; }

    if (pix) {
        auto pri = strip(pr);
        Rect r(pix->size()), rr(size());
        r.center_to(rr.center());
        irect_ = r;
        pri->set_oper(oper_);
        pri->move_to(r.origin());
        pri->pixmap(pix, transparent_);
        pri->fill();
    }
}

bool Image_impl::on_paint(Painter pr, const Rect & inval) {
    paint_pixmap(pr);
    return false;
}

void Image_impl::set_delay(unsigned delay) {
    if (delay_ != delay) {
        delay_ = delay;
        start_timer_if_needed();
    }
}

void Image_impl::start_timer_if_needed() {
    if (!cfilm_.empty()) {
        if (cfilm_.size() > 1) {
            timer_.start(calc_delay());
        }
    }

    else if (film_.size() > 1) {
        timer_.start(calc_delay());
    }
}

unsigned Image_impl::calc_delay() const {
    if (!cfilm_.empty()) {
        if (cur_ < cfilm_.size()) {
            auto & ff = cfilm_[cur_];
            if (0 != ff.delay) { return ff.delay; }
        }
    }

    if (cur_ < film_.size()) {
        auto & ff = film_[cur_];
        if (0 != ff.delay) { return ff.delay; }
    }

    if (0 != delay_) {
        return delay_;
    }

    return 1000;
}

void Image_impl::set_transparent() {
    if (!transparent_) {
        transparent_ = true;
        redraw();
    }
}

void Image_impl::unset_transparent() {
    if (transparent_) {
        transparent_ = false;
        redraw();
    }
}

void Image_impl::set_oper(Oper op) {
    if (oper_ != op) {
        oper_ = op;
        redraw();
    }
}

} // namespace tau

//END
