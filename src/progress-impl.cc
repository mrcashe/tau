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
#include <tau/font.hh>
#include <tau/painter.hh>
#include <tau/string.hh>
#include <progress-impl.hh>
#include <iomanip>

namespace tau {

Progress_impl::Progress_impl(bool vertical):
    Frame_impl(BORDER_INSET),
    vertical_(vertical)
{
    area_ = std::make_shared<Widget_impl>();
    area_->signal_paint().connect(fun(this, &Progress_impl::on_area_paint));
    area_->signal_parent().connect(fun(this, &Progress_impl::on_area_hierarchy_changed));
    insert(area_);

    area_->style().get("progress/background").signal_changed().connect(fun(this, &Progress_impl::paint_now));
    area_->style().get("progress/foreground").signal_changed().connect(fun(this, &Progress_impl::paint_now));
    area_->style().get("background").signal_changed().connect(fun(this, &Progress_impl::paint_now));
}

void Progress_impl::on_area_hierarchy_changed() {
    if (auto dp = area_->display()) {
        format_str();
        calc_hints();
    }
}

void Progress_impl::set_value(double value) {
    double cval = std::max(std::min(value, max_value_), min_value_);

    if (value_ != cval) {
        value_ = cval;
        format_str();
        calc_hints();
        paint_now();
    }
}

void Progress_impl::set_min_value(double min_value) {
    if (min_value < max_value_) {
        if (min_value_ != min_value) {
            min_value_ = min_value;

            if (value_ < min_value_) {
                value_ = min_value_;
                format_str();
                calc_hints();
                paint_now();
            }
        }
    }
}

void Progress_impl::set_max_value(double max_value) {
    if (max_value > min_value_) {
        if (max_value_ != max_value) {
            max_value_ = max_value;

            if (value_ > max_value_) {
                value_ = max_value_;
                format_str();
                calc_hints();
                paint_now();
            }
        }
    }
}

void Progress_impl::set_precision(int prec) {
    if (precision_ != prec) {
        precision_ = prec;

        if (!fmt_.empty()) {
            format_str();
            calc_hints();
            paint_now();
        }
    }
}

void Progress_impl::set_format(const ustring & fmt) {
    if (fmt_ != fmt) {
        fmt_ = fmt;
        format_str();
        calc_hints();
        paint_now();
    }
}

void Progress_impl::set_text_align(Align align) {
    if (align != text_align_) {
        text_align_ = align;
        paint_now();
    }
}

void Progress_impl::paint_now() {
    if (Painter pr = area_->painter()) {
        redraw(pr);
    }
}

void Progress_impl::redraw(Painter pr) {
    Size asize = area_->size();

    if (asize) {
        Color cback = area_->style().get("background").get();
        pr.rectangle(0, 0, asize.width(), asize.height());
        pr.set_brush(cback);
        pr.fill();

        double range = max_value_-min_value_;

        if (range > 0) {
            double value = std::max(std::min(max_value_, value_), min_value_);
            double units_per_pixel;
            int x1, y1, x2, y2;

            if (vertical_) {
                units_per_pixel = range/asize.height();
                double dy2 = (value-min_value_)/units_per_pixel;
                x1 = 0, y1 = asize.height()-dy2, x2 = asize.width(), y2 = asize.height();
            }

            else {
                units_per_pixel = range/asize.width();
                double dx2 = (value-min_value_)/units_per_pixel;
                x1 = 0, y1 = 0, x2 = dx2, y2 = asize.height();
            }

            Color cbar = area_->style().get("progress/background").get();
            pr.rectangle(x1, y1, x2, y2);
            pr.set_brush(cbar);
            pr.fill();

            if (!msg_.empty()) {
                Font font = pr.select_font(area_->style().font("font").spec());
                int ascent = font ? std::ceil(font.ascent()) : text_size_.height();
                int x = 0, y = ascent+((asize.height()-text_size_.height())/2);

                if (asize.width() > text_size_.width()) {
                    if (ALIGN_END == text_align_) { x = asize.width()-text_size_.width(); }
                    else if (ALIGN_CENTER == text_align_) { x = (asize.width()-text_size_.width())/2; }
                }

                pr.move_to(x, y);
                Color ctext = area_->style().get("progress/foreground").get();
                pr.text(msg_, ctext);
                pr.stroke();
            }
        }
    }
}

bool Progress_impl::on_area_paint(Painter pr, const Rect & inval) {
    redraw(pr);
    return false;
}

void Progress_impl::format_str() {
    msg_.clear();
    ustring::size_type i = 0, n, len = fmt_.size();

    if (0 != len) {
        for (;;) {
            n = fmt_.find('%', i);

            if (ustring::npos == n) {
                msg_ += fmt_.substr(i);
                break;
            }

            msg_ += fmt_.substr(i, n-i);

            if (n < len-1) {
                char32_t wc = fmt_[n+1];

                switch (wc) {
                    case '%':
                        msg_ += '%';
                        i = n+2;
                        break;

                    case '$':
                        msg_ += tau::str_format(std::setprecision(precision_), std::fixed, value_);
                        i = n+2;
                        break;

                    case '_':
                        msg_ += tau::str_format(std::setprecision(precision_), std::fixed, min_value_);
                        i = n+2;
                        break;

                    case '^':
                        msg_ += tau::str_format(std::setprecision(precision_), std::fixed, max_value_);
                        i = n+2;
                        break;

                    default:
                        i = n+1;
                }
            }
        }
    }
}

void Progress_impl::calc_hints() {
    Size sz;

    if (!msg_.empty()) {
        if (Painter pr = painter()) {
            pr.select_font(style().font("font").spec());
            Vector v = pr.text_size(msg_);
            text_size_.set(std::ceil(v.x()), std::ceil(v.y()));
            sz.set(text_size_.width()+2, text_size_.height()+2);
        }
    }

    else {
        if (vertical_) { sz.set(4, 0); }
        else { sz.set(0, 4); }
    }

    area_->hint_size(sz);
}

} // namespace tau

//END
