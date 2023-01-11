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
#include <separator-impl.hh>

namespace tau {

Separator_impl::Separator_impl(Separator_style separator_style):
    Widget_impl(),
    separator_style_(separator_style)
{
    signal_paint().connect(fun(this, &Separator_impl::on_paint));
    init_separator_style();
}

void Separator_impl::init_separator_style() {
    if (SEPARATOR_GROOVE == separator_style_ || SEPARATOR_RIDGE == separator_style_) {
        require_size(2);
    }

    else if (SEPARATOR_HANDLE == separator_style_) {
        require_size(3);
    }

    else {
        require_size(1);
    }
}

void Separator_impl::set_separator_style(Separator_style separator_style) {
    if (separator_style_ != separator_style) {
        separator_style_ = separator_style;
        init_separator_style();
        invalidate();
    }
}

bool Separator_impl::on_paint(Painter pr, const Rect & inval) {
    Size sz = size();
    if (!sz) { return true; }

    if (SEPARATOR_GROOVE == separator_style_) {
        Color c = style().color("background");
        double f = 0.15;
        Color cdark  = c.darken(f);
        Color clight = c.lighten(f);

        if (sz.width() > sz.height()) {
            int x1 = 0;
            int x2 = sz.width();
            int y = sz.height()/2;
            if (0 != y) { --y; }

            pr.move_to(x1, y);
            pr.line_to(x2, y);
            pr.set_pen(cdark);
            pr.stroke();

            ++y;
            pr.move_to(x1, y);
            pr.line_to(x2, y);
            pr.set_pen(clight);
            pr.stroke();
        }

        else {
            int y1 = 0;
            int y2 = sz.height();
            int x = sz.width()/2;
            if (0 != x) { --x; }

            pr.move_to(x, y1);
            pr.line_to(x, y2);
            pr.set_pen(cdark);
            pr.stroke();

            ++x;
            pr.move_to(x, y1);
            pr.line_to(x, y2);
            pr.set_pen(clight);
            pr.stroke();
        }
    }

    else if (SEPARATOR_HANDLE == separator_style_) {
        if (sz.width() > sz.height()) {
            int xmax = sz.width(), ymin = 0, ymax = sz.height()-1, step = 2*(ymax-ymin);
            pr.move_to(0, ymin);

            for (int x = 0; x < xmax;) {
                x += step;
                pr.line_to(x, ymin);
                x += ymax-ymin;
                pr.line_to(x, ymax);
                x += step; pr.line_to(x, ymax);
                x += ymax-ymin;
                pr.line_to(x, ymin);
            }
        }

        else {
            int ymax = sz.height(), xmin = 0, xmax = sz.width()-1, step = 2*(xmax-xmin);
            pr.move_to(xmin, 0);

            for (int y = 0; y < ymax;) {
                y += step; pr.line_to(xmin, y);
                y += xmax-xmin; pr.line_to(xmax, y);
                y += step; pr.line_to(xmax, y);
                y += xmax-xmin; pr.line_to(xmin, y);
            }
        }

        pr.set_pen(Pen(Color(style().get("foreground").get()), 1));
        pr.stroke();
    }

    else { // SEPARATOR_SOLID assumed
        pr.rectangle(0, 0, sz.width(), sz.height());
        pr.set_brush(Color(style().get("foreground").get()));
        pr.fill();
    }

    return true;
}

} // namespace tau

//END

