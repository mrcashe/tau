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

#include <card-impl.hh>
#include <climits>
#include <iostream>

namespace tau {

Card_impl::Card_impl():
    Container_impl()
{
    signal_arrange().connect(fun(this, &Card_impl::arrange));
    signal_take_focus().connect(fun(this, &Card_impl::on_take_focus));
    signal_size_changed().connect(fun(this, &Card_impl::arrange));
    signal_visible().connect(fun(this, &Card_impl::arrange));
    signal_display().connect(fun(this, &Card_impl::update_requisition));
}

void Card_impl::insert(Widget_ptr wp) {
    bool first = holders_.empty();
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    wp->hide();
    holders_.emplace_back();
    Holder & hol = holders_.back();
    hol.wp = wp.get();
    hol.hints_cx = wp->signal_hints_changed().connect(fun(this, &Card_impl::update_requisition));
    hol.req_cx = wp->signal_requisition_changed().connect(fun(this, &Card_impl::update_requisition));
    hol.show_cx = wp->signal_show().connect(tau::bind(fun(this, &Card_impl::on_child_show), wp.get()));
    hol.hide_cx = wp->signal_hide().connect(tau::bind(fun(this, &Card_impl::on_child_hide), wp.get()));
    make_child(wp);
    if (first) { wp->show(); wp->signal_select()(); }
    update_requisition();
    queue_arrange();
}

bool Card_impl::empty() const {
    return holders_.empty();
}

Widget_impl * Card_impl::current() {
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        if (!i->wp->hidden()) {
            return i->wp;
        }
    }

    return nullptr;
}

void Card_impl::remove_current() {
    auto wp = current();

    if (wp) {
        remove(wp);
    }
}

void Card_impl::rm_child(Holder & hol) {
    hol.wp->signal_unselect()();
    hol.wp->hide();
    hol.req_cx.disconnect();
    hol.hints_cx.disconnect();
    hol.show_cx.disconnect();
    hol.hide_cx.disconnect();
    unparent_child(hol.wp);
    hol.wp->update_origin(INT_MIN, INT_MIN);
    hol.wp->update_size(0, 0);
}

void Card_impl::remove(Widget_impl * wi) {
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = *i;

        if (hol.wp == wi) {
            rm_child(hol);
            holders_.erase(i);
            update_requisition();
            queue_arrange();
            return;
        }
    }
}

void Card_impl::clear() {
    for (auto & hol: holders_) { rm_child(hol); }
    holders_.clear();
    invalidate();
    update_requisition();
}

Size Card_impl::child_requisition(const Holder & hol) {
    Size req = hol.wp->required_size();
    req.update(hol.wp->size_hint(), true);
    req.update_max(hol.wp->min_size_hint());
    req.update_min(hol.wp->max_size_hint(), true);
    req.increase(hol.wp->margin_hint());
    return req;
}

void Card_impl::update_requisition() {
    Size req;
    for (Holder & hol: holders_) { req |= child_requisition(hol); }
    require_size(req);
}

void Card_impl::arrange() {
    bool changed = false;

    for (Holder & hol: holders_) {
        if (!hol.wp->hidden()) {
            if (hol.wp->update_origin(hol.wp->margin_origin())) { changed = true; }
            if (hol.wp->update_size(size()-hol.wp->margin_hint())) { changed = true; }
        }
    }

    if (changed) { invalidate(); }
}

void Card_impl::on_child_hide(Widget_impl * wi) {
    wi->update_origin(INT_MIN, INT_MIN);
    wi->update_size(0, 0);

    if (!destroy_) {
        if (holders_.size() > 1) {
            in_hide_ = true;

            if (!in_show_) {
                for (auto i = holders_.begin(); i != holders_.end(); ++i) {
                    if (i->wp == wi) {
                        auto j = i++;    // keep iter.

                        if (i != holders_.end()) { // try to show next child.
                            i->wp->show();
                        }

                        else if (j != holders_.begin()) {
                            --j;
                            j->wp->show();
                        }

                        break;
                    }
                }
            }

            in_hide_ = false;
        }

        if (!in_show_) { arrange(); }
    }
}

void Card_impl::on_child_show(Widget_impl * wi) {
    if (!destroy_) {
        if (holders_.size() > 1) {
            in_show_ = true;

            if (!in_hide_) {
                for (auto i = holders_.begin(); i != holders_.end(); ++i) {
                    if (i->wp != wi) { i->wp->hide(); }
                }
            }

            in_show_ = false;
        }

        if (!in_hide_) { arrange(); }
        if (focused()) { wi->take_focus(); }
    }
}

void Card_impl::show_next() {
    if (holders_.size() > 1) {
        if (auto cw = current()) {
            for (auto i = holders_.begin(); i != holders_.end(); ++i) {
                if (i->wp == cw) {
                    if (++i != holders_.end()) { i->wp->show(); }
                    else { holders_.front().wp->show(); }
                    return;
                }
            }
        }
    }
}

void Card_impl::show_previous() {
    if (holders_.size() > 1) {
        if (auto cw = current()) {
            for (auto i = holders_.rbegin(); i != holders_.rend(); ++i) {
                Holder & hol = *i;

                if (hol.wp == cw) {
                    if (++i != holders_.rend()) { i->wp->show(); }
                    else { holders_.back().wp->show(); }
                    return;
                }
            }
        }
    }
}

bool Card_impl::on_take_focus() {
    auto wp = current();

    if (wp && wp->take_focus()) {
        return true;
    }

    return grab_focus();
}

} // namespace tau

//END
