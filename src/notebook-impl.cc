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

#include <tau/input.hh>
#include <tau/string.hh>
#include <absolute-impl.hh>
#include <card-impl.hh>
#include <frame-impl.hh>
#include <notebook-impl.hh>
#include <roller-impl.hh>
#include <text-impl.hh>
#include <iostream>

namespace {

tau::Orientation or_from_tab_pos(tau::Tab_pos tab_pos) {
    switch (tab_pos) {
        case tau::TAB_LEFT: return tau::OR_RIGHT;
        case tau::TAB_RIGHT: return tau::OR_LEFT;
        case tau::TAB_BOTTOM: return tau::OR_UP;
        default: return tau::OR_DOWN;
    }
}

} // anonymous namespace

namespace tau {

Notebook_impl::Notebook_impl(Tab_pos tab_pos):
    Box_impl(or_from_tab_pos(tab_pos)),
    card_(std::make_shared<Card_impl>())
{
    roller_ = std::make_shared<Roller_impl>(horizontal() ? OR_DOWN : OR_RIGHT);
    roller_->set_step(32);
    abs_ = std::make_shared<Absolute_impl>();

    frame_ = std::make_shared<Frame_impl>();
    frame_->set_border_style(BORDER_SOLID);
    frame_->set_border_color(sel_color());

    roller_->insert(abs_);
    append(roller_, true);
    frame_->insert(card_);
    append(frame_, false);

    signal_size_changed().connect(fun(this, &Notebook_impl::update_tabs));
    signal_size_changed().connect(fun(this, &Notebook_impl::update_roller));
    signal_take_focus().connect(fun(card_, &Card_impl::take_focus));
    frame_->style().get("button/background").signal_changed().connect(fun(this, &Notebook_impl::on_frame_background_changed));
    roller_->signal_mouse_wheel().connect(fun(this, &Notebook_impl::on_mouse_wheel), true);
    abs_->signal_size_changed().connect(fun(this, &Notebook_impl::update_roller));
}

Color Notebook_impl::sel_color() {
    Color c = frame_->style().color("button/background");
    c.darker(0.18);
    return c;
}

void Notebook_impl::on_frame_background_changed() {
    frame_->set_border_color(sel_color());
}

int Notebook_impl::remove_page(Widget_ptr wp) {
    return remove_page(page_number(wp));
}

int Notebook_impl::page_number(Widget_cptr cwp) const {
    std::size_t current = 0;

    for (auto & p: pages_) {
        if (cwp == p.wp) {
            return current;
        }

        ++current;
    }

    return -1;
}

int Notebook_impl::remove_page(int page) {
    if (page >= 0) {
        std::size_t upage = page;
        int current = current_page();

        if (upage < pages_.size()) {
            auto & p = pages_[upage];
            abs_->remove(p.frame.get());
            card_->remove(p.wp.get());
            p.size1_cx.drop();
            p.size2_cx.drop();
            p.requisition_cx.drop();
            p.hints_cx.drop();
            p.mouse_down_cx.drop();
            p.mouse_up_cx.drop();
            p.mouse_motion_cx.drop();
            p.show_cx.drop();
            p.visible_cx.drop();
            p.hide_cx.drop();
            pages_.erase(pages_.begin()+upage);
            signal_page_removed_(page);
            for (int n = pages_.size(); n > page; --n) { signal_page_reordered_(n, n-1); }
            if (page == current) { update_current(); }
            update_frame_border();
            update_tabs();
            return upage;
        }
    }

    return -1;
}

int Notebook_impl::current_page() const {
    for (unsigned n = 0; n < pages_.size(); ++n) {
        if (!pages_[n].wp->hidden()) {
            return n;
        }
    }

    return -1;
}

void Notebook_impl::clear_pages() {
    unsigned n = pages_.size();
    pages_.clear();
    for (; n > 0; --n) { signal_page_removed_(n-1); }
    card_->clear();
    abs_->clear();
    update_frame_border();
    update_tabs();
}

int Notebook_impl::append_page(Widget_ptr wp, Widget_ptr tp) {
    int current = current_page(), nth_page = pages_.size();
    pages_.emplace_back();
    init_page(nth_page, wp, tp);
    if (!nth_page) { update_frame_border(); }
    update_tabs();
    signal_page_added_(nth_page);
    if (current < 0) { update_current(); signal_page_changed_(current_page()); }
    return nth_page;
}

int Notebook_impl::append_page(Widget_ptr wp) {
    return append_page(wp, str_format("Page ", 1+pages_.size()));
}

int Notebook_impl::append_page(Widget_ptr wp, const ustring & title, Widget_ptr * rptr) {
    auto tp = std::make_shared<Text_impl>(title);
    tp->hint_margin(2);
    if (rptr) { *rptr = tp; }
    return append_page(wp, tp);
}

void Notebook_impl::prepend_page(Widget_ptr wp, Widget_ptr tp) {
    int current = current_page();
    pages_.emplace(pages_.begin());
    init_page(0, wp, tp);
    signal_page_added_(0);
    if (current < 0) { update_frame_border(); }
    update_tabs();
    for (std::size_t n = pages_.size()-1; n > 0; --n) { signal_page_reordered_(n-1, n); }
    if (current < 0) { update_current(); signal_page_changed_(current_page()); }
}

void Notebook_impl::prepend_page(Widget_ptr wp) {
    prepend_page(wp, str_format("Page ", 1+pages_.size()));
}

Widget_ptr Notebook_impl::prepend_page(Widget_ptr wp, const ustring & title) {
    auto tp = std::make_shared<Text_impl>(title);
    tp->hint_margin(2);
    prepend_page(wp, tp);
    return tp;
}

int Notebook_impl::insert_page(Widget_ptr wp, int nth_page, Widget_ptr tp) {
    int current = current_page();
    if (nth_page < 0) { nth_page = 0; }
    if (std::size_t(nth_page) >= pages_.size()) { nth_page = pages_.empty() ? 0 : pages_.size(); }
    pages_.emplace(pages_.begin()+nth_page);
    init_page(nth_page, wp, tp);
    for (std::size_t n = pages_.size()-1; n > std::size_t(nth_page); --n) { signal_page_reordered_(n-1, n); }
    if (current < 0) { update_frame_border(); }
    update_tabs();
    signal_page_added_(nth_page);
    if (current < 0) { update_current(); signal_page_changed_(current_page()); }
    return nth_page;
}

int Notebook_impl::insert_page(Widget_ptr wp, int nth_page) {
    return insert_page(wp, nth_page, str_format("Page ", 1+pages_.size()));
}

int Notebook_impl::insert_page(Widget_ptr wp, int nth_page, const ustring & title, Widget_ptr * rptr) {
    auto tp = std::make_shared<Text_impl>(title);
    tp->hint_margin(2);
    if (rptr) { *rptr = tp; }
    return insert_page(wp, nth_page, tp);
}

int Notebook_impl::insert_page_after(Widget_ptr wp, Widget_ptr after_this, Widget_ptr tp) {
    int nth_page = page_number(after_this);
    if (nth_page < 0) { nth_page = pages_.size(); }
    return insert_page(wp, nth_page, tp);
}

int Notebook_impl::insert_page_before(Widget_ptr wp, Widget_ptr before_this, Widget_ptr tp) {
    int nth_page = page_number(before_this);
    if (nth_page < 0) { nth_page = 0; }
    if (nth_page > 0) { --nth_page; }
    return insert_page(wp, nth_page, tp);
}

int Notebook_impl::insert_page_after(Widget_ptr wp, Widget_ptr after_this) {
    return insert_page_after(wp, after_this, str_format("Page ", 1+pages_.size()));
}

int Notebook_impl::insert_page_after(Widget_ptr wp, Widget_ptr after_this, const ustring & title, Widget_ptr * rptr) {
    auto tp = std::make_shared<Text_impl>(title);
    tp->hint_margin(2);
    if (rptr) { *rptr = tp; }
    return insert_page_after(wp, after_this, tp);
}

int Notebook_impl::insert_page_before(Widget_ptr wp, Widget_ptr before_this) {
    return insert_page_before(wp, before_this, str_format("Page ", 1+pages_.size()));
}

int Notebook_impl::insert_page_before(Widget_ptr wp, Widget_ptr before_this, const ustring & title, Widget_ptr * rptr) {
    auto tp = std::make_shared<Text_impl>(title);
    tp->hint_margin(2);
    if (rptr) { *rptr = tp; }
    return insert_page_before(wp, before_this, tp);
}

void Notebook_impl::update_frame_border() {
    if (!empty() && tabs_visible_) {
        switch (orientation()) {
            case OR_RIGHT:
                roller_->hint_margin_left(2);
                frame_->set_border_left(spc_);
                break;

            case OR_LEFT:
                roller_->hint_margin_right(2);
                frame_->set_border_right(spc_);
                break;

            case OR_UP:
                roller_->hint_margin_bottom(2);
                frame_->set_border_bottom(spc_);
                break;

            case OR_DOWN:
                roller_->hint_margin_top(2);
                frame_->set_border_top(spc_);
                break;
        }
    }

    else {
        roller_->hint_margin(0);
        frame_->set_border(0);
    }
}

void Notebook_impl::init_page(unsigned nth_page, Widget_ptr wp, Widget_ptr tp) {
    Page & p = pages_[nth_page];
    p.wp = wp;
    p.title = tp;
    p.frame = std::make_shared<Frame_impl>();

    if (horizontal()) {
        if (OR_RIGHT == orientation()) {
            p.frame->set_border_left(1);
            p.frame->set_border_left_style(BORDER_OUTSET);
        }

        else {
            p.frame->set_border_right(1);
            p.frame->set_border_right_style(BORDER_OUTSET);
        }

        p.frame->set_border_top(1);
        p.frame->set_border_bottom(1);
        p.frame->set_border_top_style(BORDER_OUTSET);
        p.frame->set_border_bottom_style(BORDER_OUTSET);
    }

    else {
        if (OR_DOWN == orientation()) {
            p.frame->set_border_top(1);
            p.frame->set_border_top_style(BORDER_OUTSET);
        }

        else {
            p.frame->set_border_bottom(1);
            p.frame->set_border_bottom_style(BORDER_OUTSET);
        }

        p.frame->set_border_left(1);
        p.frame->set_border_right(1);
        p.frame->set_border_left_style(BORDER_OUTSET);
        p.frame->set_border_right_style(BORDER_OUTSET);
    }

    p.frame->insert(tp);
    p.size1_cx = p.frame->signal_size_changed().connect(fun(this, &Notebook_impl::update_tabs));
    p.size2_cx = p.frame->signal_size_changed().connect(fun(this, &Notebook_impl::update_roller));
    p.requisition_cx = p.frame->signal_requisition_changed().connect(fun(this, &Notebook_impl::update_tabs));
    p.hints_cx = p.frame->signal_hints_changed().connect(fun(this, &Notebook_impl::update_tabs));
    p.mouse_down_cx = p.frame->signal_mouse_down().connect(tau::bind(fun(this, &Notebook_impl::on_tab_mouse_down), p.frame.get()));
    p.mouse_up_cx = p.frame->signal_mouse_up().connect(tau::bind(fun(this, &Notebook_impl::on_tab_mouse_up), p.frame.get()));
    p.mouse_motion_cx = p.frame->signal_mouse_motion().connect(tau::bind(fun(this, &Notebook_impl::on_tab_mouse_motion), p.frame.get()));

    p.show_cx = p.wp->signal_show().connect(fun(this, &Notebook_impl::update_current));
    p.visible_cx = p.wp->signal_visible().connect(fun(this, &Notebook_impl::update_roller));
    p.hide_cx = p.wp->signal_hide().connect(fun(this, &Notebook_impl::update_current));

    card_->insert(wp);
    abs_->put(p.frame, Point());
    update_frame_border();
    if (tabs_visible_) { roller_->show(); }
    else { roller_->hide(); }
}

void Notebook_impl::update_roller() {
    int current = current_page();

    if (current >= 0) {
        unsigned n = current;

        if (n < pages_.size()) {
            Page & p = pages_[n];
            Point org = p.frame->origin();
            Rect va = abs_->visible_area();

            if (horizontal()) {
                unsigned h = p.frame->size().height();

                if (0 != h && h < va.height()) {
                    if (org.y() < va.top()) {
                        roller_->roll_to(org.y());
                    }

                    else {
                        int y1 = h+org.y();

                        if (y1 > va.bottom()) {
                            int y = y1-va.height();
                            roller_->roll_to(y);
                        }
                    }
                }
            }

            else {
                unsigned w = p.frame->size().width();

                if (0 != w && w < va.width()) {
                    if (org.x() < va.left()) {
                        roller_->roll_to(org.x());
                    }

                    else {
                        int x1 = w+org.x();

                        if (x1 > va.right()) {
                            int x = x1-va.width();
                            roller_->roll_to(x);
                        }
                    }
                }
            }
        }
    }
}

void Notebook_impl::update_current() {
    int current = -1;

    for (unsigned i = 0; i < pages_.size(); ++i) {
        Page & pg = pages_[i];
        Color c = style().color("button/background");

        if (pg.wp.get() != card_->current()) {
            pg.frame->style().color("background") = c;
        }

        else {
            pg.frame->style().color("background") = c.lighten(0.1);
            current = i;
        }
    }

    if (current >= 0 && current != last_) {
        signal_page_changed_(current);
        last_ = current;
    }
}

void Notebook_impl::update_tabs() {
    if (!shut_ && ! in_arrange_) {
        in_arrange_ = true;

        if (!empty() && tabs_visible_) {
            unsigned aw = 0, ah = 0, wmax = 0, hmax = 0, spc = 0;

            for (auto & p: pages_) {
                Size req = child_requisition(p.frame.get());
                wmax = std::max(wmax, req.width());
                hmax = std::max(hmax, req.height());
                ++spc;
            }

            spc = spc > 1 ? (spc-1)*spc_ : 0;
            aw = horizontal() ? wmax : spc;
            ah = horizontal() ? spc : hmax;

            for (auto & p: pages_) {
                if (homogeneous_tabs_) {
                    abs_->resize(p.frame.get(), wmax, hmax);
                    if (horizontal()) { ah += hmax; aw = std::max(aw, wmax); }
                    else { aw += wmax; ah = std::max(ah, hmax); }
                }

                else {
                    Size req = child_requisition(p.frame.get());

                    if (horizontal()) {
                        ah += req.height();
                        abs_->resize(p.frame.get(), wmax, req.height());
                    }

                    else {
                        aw += req.width();
                        abs_->resize(p.frame.get(), req.width(), hmax);
                    }
                }
            }

            abs_->show();
            abs_->hint_min_size(aw, ah);
            int offset = 0;

            for (auto & p: pages_) {
                abs_->show();

                if (horizontal()) {
                    if (p.frame.get() != drag_) { abs_->move(p.frame.get(), 0, offset); }
                    offset += p.frame->size().height();
                }

                else {
                    if (p.frame.get() != drag_) { abs_->move(p.frame.get(), offset, 0); }
                    offset += p.frame->size().width();
                }

                offset += spc_;
            }
        }

        // empty() || !tabs_visible_
        else {
            abs_->hide();
        }

        in_arrange_ = false;
    }
}

Size Notebook_impl::child_requisition(Widget_impl * wp) {
    Size req = wp->required_size();
    req.update(wp->size_hint(), true);
    req.update_max(wp->min_size_hint());
    req.update_max(16, 8);
    req.update_min(wp->max_size_hint(), true);
    return req;
}

int Notebook_impl::show_next() {
    if (!pages_.empty()) {
        unsigned current = std::max(0, current_page());

        if (pages_.size() > 1) {
            if (1+current < pages_.size() || rollover_allowed_) {
                if (++current >= pages_.size()) { current = 0; }
                pages_[current].wp->show();
            }
        }

        return current;
    }

    return -1;
}

int Notebook_impl::show_previous() {
    if (!pages_.empty()) {
        unsigned current = std::max(0, current_page());

        if (pages_.size() > 1) {
            if (current > 0 || rollover_allowed_) {
                current = current > 0 ? current-1 : pages_.size()-1;
                pages_[current].wp->show();
            }
        }

        return current;
    }

    return -1;
}

int Notebook_impl::show_page(int nth_page) {
    if (nth_page >= 0) {
        unsigned n = nth_page;

        if (n < pages_.size()) {
            pages_[n].wp->show();
            return n;
        }
    }

    return -1;
}

void Notebook_impl::on_tab_mouse_motion(int mm, const Point & pt, Widget_impl * wi) {
    if (mm & MM_LEFT) {
        Point ppt = wi->to_parent(pt);
        int moving = -1, moved = -1, current = current_page();

        for (unsigned i = 0; i < pages_.size(); ++i) {
            if (pages_[i].frame.get() == wi) {
                moving = i;
                break;
            }
        }

        if (drag_ && moving >= 0) {
            if (horizontal()) {
                abs_->move(drag_, 0, ppt.y());

                if (ppt.y() > abs_->visible_area().bottom()) {
                    roller_->roll_to(ppt.y()-abs_->size().height());
                }

                else if (ppt.y() < abs_->visible_area().top()) {
                    roller_->roll_to(ppt.y());
                }

                for (unsigned i = 0; i < pages_.size(); ++i) {
                    if (pages_[i].frame.get() != wi) {
                        Point origin = pages_[i].frame->origin();
                        unsigned w = abs_->size().width(), h = pages_[i].frame->size().height();

                        if (Rect(origin, Size(w, h)).contains(ppt)) {
                            moved = i;
                            break;
                        }
                    }
                }
            }

            else {
                abs_->move(drag_, ppt.x(), 0);

                if (ppt.x() > abs_->visible_area().right()) {
                    roller_->roll_to(ppt.x()-abs_->size().width());
                }

                else if (ppt.x() < abs_->visible_area().left()) {
                    roller_->roll_to(ppt.x());
                }

                for (unsigned i = 0; i < pages_.size(); ++i) {
                    if (pages_[i].frame.get() != wi) {
                        Point origin = pages_[i].frame->origin();
                        unsigned h = abs_->size().height(), w = pages_[i].frame->size().width();

                        if (Rect(origin, Size(w, h)).contains(ppt)) {
                            moved = i;
                            break;
                        }
                    }
                }
            }

            if (moving != moved) {
                if (moved >= 0) {
                    std::swap(pages_[moving], pages_[moved]);
                    signal_page_reordered_(moving, moved);
                    update_tabs();
                    if (current >= 0 && std::min(moved, moving) <= current) { update_current(); }
                }
            }
        }

        else if (reorder_allowed_ && pages_.size() > 1) {
            drag_ = wi;
            drag_->grab_mouse();
        }
    }

    else {
        if (auto wp = drag_) {
            drag_ = nullptr;
            wp->ungrab_mouse();
            update_tabs();
        }
    }
}

bool Notebook_impl::on_tab_mouse_down(int mbt, int mm, const Point & pt, Widget_impl * wi) {
    if (MBT_LEFT == mbt) {
        auto i = std::find_if(pages_.begin(), pages_.end(), [wi](Page & pg) { return wi == pg.frame.get(); } );
        if (i != pages_.end()) { i->wp->show(); }
        return true;
    }

    return false;
}

bool Notebook_impl::on_tab_mouse_up(int mbt, int mm, const Point & pt, Widget_impl * wi) {
    if (MBT_LEFT == mbt) {
        if (wi == drag_) {
            wi->ungrab_mouse();
            drag_ = nullptr;
            update_tabs();
        }

        return true;
    }

    return false;
}

bool Notebook_impl::on_mouse_wheel(int d, int mm, const Point & pt) {
    if (d > 0) { show_next(); }
    else { show_previous(); }
    return true;
}

void Notebook_impl::reorder_page(Widget_ptr wp, int nth_page) {
    reorder_page(page_number(wp), nth_page);
}

void Notebook_impl::reorder_page(int old_page, int new_page) {
    if (reorder_allowed_) {
        int size = pages_.size();//, current = current_page();

            // std::cout << "reorder " << old_page << "->" << new_page << " " << size << '\n';
        if (old_page >= 0 && new_page >= 0 && old_page < size && new_page < size && old_page != new_page) {
            std::swap(pages_[old_page], pages_[new_page]);
            signal_page_reordered_(old_page, new_page);
            update_tabs();
            update_current();
        }
    }
}

void Notebook_impl::allow_reorder() {
    reorder_allowed_ = true;
}

void Notebook_impl::disallow_reorder() {
    reorder_allowed_ = false;
}

void Notebook_impl::show_tabs() {
    if (!tabs_visible_) {
        tabs_visible_ = true;
        update_frame_border();
        roller_->show();
    }
}

void Notebook_impl::hide_tabs() {
    if (tabs_visible_) {
        tabs_visible_ = false;
        update_frame_border();
        roller_->hide();
    }
}

void Notebook_impl::set_homogeneous_tabs() {
    if (!homogeneous_tabs_) {
        homogeneous_tabs_ = true;
        update_tabs();
    }
}

void Notebook_impl::unset_homogeneous_tabs() {
    if (homogeneous_tabs_) {
        homogeneous_tabs_ = false;
        update_tabs();
    }
}

} // namespace tau

//END
