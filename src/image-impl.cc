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
#include <tau/loop.hh>
#include <tau/painter.hh>
#include <image-impl.hh>
#include <pixmap-impl.hh>
#include <theme-impl.hh>

namespace tau {

Image_impl::Image_impl():
    Widget_impl()
{
    init();
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
    signal_paint().connect(fun(this, &Image_impl::on_paint));
    signal_parent().connect(fun(this, &Image_impl::on_parent));
    signal_enable().connect(fun(this, &Image_impl::on_enable));
    signal_disable().connect(fun(this, &Image_impl::on_disable));
}

void Image_impl::clear() {
    for (auto & ff: film_) { ff.changed_cx.drop(); }
    film_.clear();
    cur_ = 0;
    invalidate();
}

void Image_impl::on_pix_changed(std::size_t index) {
    if (index < film_.size() && cur_ == index) {
        gray_.reset();
        invalidate();
    }
}

void Image_impl::add_pixmap(Pixmap_ptr pix, unsigned delay) {
    if (pix) {
        film_.emplace_back();
        Film_frame & ff = film_.back();
        std::size_t index = film_.size()-1;
        ff.pix = pix;
        ff.delay = delay;
        ff.changed_cx = pix->signal_changed().connect(tau::bind(fun(this, &Image_impl::on_pix_changed), index));
    }

    Size sz;
    for (auto & ff: film_) { sz |= ff.pix->size(); }
    require_size(sz);
    invalidate();
    start_timer_if_needed();
}

void Image_impl::set_pixmap(Pixmap_ptr pix, bool transparent) {
    transparent_ = transparent;

    if (pix) {
        clear();
        add_pixmap(pix);
    }
}

void Image_impl::set_transparent() {
    if (!transparent_) {
        transparent_ = true;
        invalidate();
    }
}

void Image_impl::unset_transparent() {
    if (transparent_) {
        transparent_ = false;
        invalidate();
    }
}

void Image_impl::set_oper(Oper op) {
    if (oper_ != op) {
        oper_ = op;
        invalidate();
    }
}

void Image_impl::paint_pixmap(Painter pr) {
    if (cur_ < film_.size()) {
        if (!enabled() && !gray_) {
            Pixmap_ptr orig = film_[cur_].pix;

            if (32 == orig->depth() && transparent_) {
                gray_ = Pixmap_impl::create(32, orig->size());
                int width = orig->size().width(), height = orig->size().height();

                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        uint32_t px = orig->get_pixel(Point(x, y)).argb32();

                        if (0 == px >> 24) {
                            gray_->put_pixel(Point(x, y), Color::from_argb32(0));
                        }

                        else {
                            gray_->put_pixel(Point(x, y), Color::from_gray(Color::from_argb32(px).gray()));
                        }
                    }
                }
            }

            else {
                gray_ = Pixmap_impl::create(8, orig);
            }
        }

        if (irect_) {
            pr.set_brush(Color(style().color("background")));
            pr.rectangle(irect_.left(), irect_.top(), irect_.right(), irect_.bottom());
            pr.fill();
        }

        Pixmap_ptr pix = gray_ ? gray_ : film_[cur_].pix;
        Rect r(pix->size()), rr(size());
        r.center_to(rr.center());
        irect_ = r;
        pr.set_oper(oper_);
        pr.move_to(r.origin());
        pr.pixmap(pix, transparent_);
        pr.stroke();
    }
}

void Image_impl::on_timer() {
    if (film_.size() > 1) {
        if (++cur_ >= film_.size()) { cur_ = 0; }
        redraw();
        timer_.start(calc_delay());
    }
}

unsigned Image_impl::calc_delay() const {
    if (cur_ < film_.size()) {
        const Film_frame & ff = film_[cur_];
        if (0 != ff.delay) { return ff.delay; }
    }

    if (0 != delay_) {
        return delay_;
    }

    return 1000;
}

void Image_impl::start_timer_if_needed() {
    if (film_.size() > 1) {
        timer_.start(calc_delay());
    }
}

void Image_impl::on_parent() {
    if (film_.empty() && !pixmap_name_.empty()) {
        if (auto pix = Theme_impl::root()->find_pixmap(pixmap_name_)) {
            set_pixmap(pix->dup(), transparent_);
        }
    }
}

void Image_impl::on_enable() {
    if (!film_.empty()) {
        gray_.reset();
        invalidate();
    }
}

void Image_impl::on_disable() {
    if (!film_.empty()) {
        invalidate();
    }
}

bool Image_impl::on_paint(Painter pr, const Rect & inval) {
    paint_pixmap(pr);
    return false;
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

void Image_impl::set_delay(unsigned delay) {
    if (delay_ != delay) {
        delay_ = delay;
        start_timer_if_needed();
    }
}

} // namespace tau

//END
