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

#include <tau/theme.hh>
#include <box-impl.hh>
#include <iostream>

namespace tau {

Box_impl::Box_impl(Orientation orient, unsigned spacing):
    Container_impl(),
    spacing_(spacing),
    orient_(orient)
{
    signal_arrange_.connect(fun(this, &Box_impl::arrange));
    signal_size_changed_.connect(fun(this, &Box_impl::arrange));
    signal_visible_.connect(fun(this, &Box_impl::arrange));
    signal_display_.connect(fun(this, &Box_impl::update_requisition));
    signal_take_focus_.connect(fun(this, &Box_impl::on_take_focus));
    signal_child_requisition_.connect(fun(this, &Box_impl::on_child_requisition));
    signal_child_hints_.connect(fun(this, &Box_impl::on_child_requisition));
    signal_child_show_.connect(fun(this, &Box_impl::on_child_show));
    signal_child_hide_.connect(fun(this, &Box_impl::on_child_hide));

    next_action_.set_master_action(ACTION_FOCUS_NEXT);
    prev_action_.set_master_action(ACTION_FOCUS_PREVIOUS);

    connect_action(next_action_);
    connect_action(prev_action_);
}

Box_impl::~Box_impl() {
    signal_destroy_();
    if (signal_orientation_changed_) { delete signal_orientation_changed_; }
}

void Box_impl::update_requisition() {
    nvisible_ = nsh_ = req_ = 0;
    unsigned x = 0, y = 0;

    for (Holder & hol: holders_) {
        if (!hol.wp_->hidden()) {
            ++nvisible_;

            hol.req_ = hol.wp_->required_size();
            hol.req_.update(hol.wp_->size_hint(), true);
            hol.req_.update_max(hol.wp_->min_size_hint());
            hol.req_.update_min(hol.wp_->max_size_hint(), true);
            Size mg(hol.wp_->margin_hint());

            if (horizontal()) {
                if (hol.sh_) {
                    ++nsh_;
                    req_ += hol.req_.width();
                }

                x += hol.req_.width()+mg.width();
                req_ += mg.width();
                y = std::max(y, (hol.req_.height()+mg.height()));
            }

            else {
                if (hol.sh_) {
                    ++nsh_;
                    req_ += hol.req_.height();
                }

                y += hol.req_.height()+mg.height();
                req_ += mg.height();
                x = std::max(x, (hol.req_.width()+mg.width()));
            }
        }
    }

    unsigned nspace = nvisible_ > 1 ? spacing_*(nvisible_-1) : 0;
    req_ += nspace;
    if (horizontal()) { x += nspace; }
    else { y += nspace; }
    require_size(x, y);
}

void Box_impl::arrange() {
    Rect  r, inval;
    Point origin;
    Size  sz;
    unsigned rem = 0, nextra = nvisible_-nsh_;
    int avail;

    if (horizontal()) {
        unsigned sw = size().width();
        avail = sw > req_ ? sw-req_ : 0;

        if (nextra > 0) {
            rem = avail > 0 ? avail%nextra : 0;        // Calculate rem before nextra!
            nextra = avail > 0 ? avail/nextra : 0;
        }

        int x = OR_RIGHT == orient_ ? 0 : size().width();

        if (avail > 0 && nsh_ == nvisible_) {
            if (OR_LEFT == orient_) {
                if (ALIGN_END == align_) { x -= avail; }
                else if (ALIGN_CENTER == align_) { x -= avail/2; }
            }

            else {
                if (ALIGN_END == align_) { x += avail; }
                else if (ALIGN_CENTER == align_) { x += avail/2; }
            }
        }

        for (Holder & hol: holders_) {
            if (!hol.wp_->hidden() && size().height()) {
                Size mg = hol.wp_->margin_hint();
                unsigned w;

                if (hol.sh_) {
                    w = hol.req_.width();
                }

                else {
                    w = nextra;
                    if (0 != rem) { ++w, --rem; }
                    w = std::max(1U, w); // If there are no free space, assign width=1 px.
                }

                origin.update((OR_RIGHT == orient_ ? x : x-w-mg.width()), 0);
                origin.translate(hol.wp_->margin_origin());
                sz.update(w, size().height()-mg.height());
                r.set(hol.wp_->origin(), hol.wp_->size());

                if (update_child_bounds(hol.wp_, origin, sz)) {
                    inval.unite(r);
                    inval.unite(Rect(origin, sz));
                }

                int d = spacing_+w+mg.width();
                x += OR_RIGHT == orient_ ? d : -d;
            }
        }
    }

    else {
        unsigned sh = size().height();
        avail = sh > req_ ? sh-req_ : 0;

        if (nextra > 0) {
            rem = avail > 0 ? avail%nextra : 0;        // Calculate rem before nextra!
            nextra = avail > 0 ? avail/nextra : 0;
        }

        int y = OR_DOWN == orient_ ? 0 : size().height();

        if (avail > 0 && nsh_ == nvisible_) {
            if (OR_UP == orient_) {
                if (ALIGN_END == align_) { y -= avail; }
                else if (ALIGN_CENTER == align_) { y -= avail/2; }
            }

            else {
                if (ALIGN_END == align_) { y += avail; }
                else if (ALIGN_CENTER == align_) { y += avail/2; }
            }
        }

        for (Holder & hol: holders_) {
            if (!hol.wp_->hidden() && size().width()) {
                Size mg = hol.wp_->margin_hint();
                unsigned h;

                if (hol.sh_) {
                    h = hol.req_.height();
                }

                else {
                    h = nextra;
                    if (0 != rem) { ++h, --rem; }
                    h = std::max(1U, h); // If there are no free space, assiign height=1 px.
                }

                origin.update(0, (OR_DOWN == orient_ ? y : y-h-mg.height()));
                origin.translate(hol.wp_->margin_origin());
                sz.update(size().width()-mg.width(), h);
                r.set(hol.wp_->origin(), hol.wp_->size());

                if (update_child_bounds(hol.wp_, origin, sz)) {
                    inval.unite(r);
                    inval.unite(Rect(origin, sz));
                }

                int d = spacing_+h+mg.height();
                y += OR_DOWN == orient_ ? d : -d;
            }
        }
    }

    if (inval) { invalidate(inval); }
}

void Box_impl::on_child_requisition(Widget_impl * wi) {
    if (!shut_) {
        update_requisition();
        queue_arrange();
    }
}

void Box_impl::on_child_hide(Widget_impl * wi) {
    if (!shut_) {
        wi->update_origin(INT_MIN, INT_MIN);
        wi->update_size(0, 0);
        update_requisition();
        queue_arrange();
        invalidate();
    }
}

void Box_impl::on_child_show(Widget_impl * wi) {
    if (!shut_) {
        update_requisition();
        queue_arrange();
        invalidate();
    }
}

void Box_impl::new_child(Holder & hol, Widget_ptr wp, bool shrink) {
    wp->update_origin(INT_MIN, INT_MIN);
    wp->update_size(0, 0);
    hol.wp_ = wp.get();
    hol.sh_ = shrink;
    make_child(wp);
    update_requisition();
    queue_arrange();
}

void Box_impl::rm_child(Holder & hol) {
    unparent_child(hol.wp_);
    update_requisition();
    queue_arrange();
    invalidate();
}

void Box_impl::append(Widget_ptr wp, bool shrink) {
    if (wp) {
        chk_parent(wp);
        holders_.emplace_back();
        new_child(holders_.back(), wp, shrink);
    }
}

void Box_impl::prepend(Widget_ptr wp, bool shrink) {
    if (wp) {
        chk_parent(wp);
        holders_.emplace_front();
        new_child(holders_.front(), wp, shrink);
    }
}

void Box_impl::insert_before(Widget_ptr wp, const Widget_impl * other, bool shrink) {
    if (wp) {
        chk_parent(wp);
        auto i = holders_.begin();

        while (i != holders_.end()) {
            if (other == i->wp_) { break; }
            ++i;
        }

        new_child(*holders_.emplace(i), wp, shrink);
    }
}

void Box_impl::insert_after(Widget_ptr wp, const Widget_impl * other, bool shrink) {
    if (wp) {
        chk_parent(wp);
        auto i = holders_.begin();

        while (i != holders_.end()) {
            if (other == i->wp_) { ++i; break; }
            ++i;
        }

        new_child(*holders_.emplace(i), wp, shrink);
    }
}

void Box_impl::remove(Widget_impl * wp) {
    if (wp) {
        for (auto i = holders_.begin(); i != holders_.end(); ++i) {
            if (wp == i->wp_) {
                rm_child(*i);
                holders_.erase(i);
                break;
            }
        }
    }
}

void Box_impl::remove_after(const Widget_impl * other) {
    if (other) {
        if (holders_.size() > 1) {
            auto i = holders_.begin();

            for (; i != holders_.end(); ++i) {
                if (other == i->wp_) {
                    break;
                }
            }

            if (holders_.end() != i) {
                ++i;

                if (holders_.end() != i) {
                    rm_child(*i);
                    holders_.erase(i);
                }
            }
        }
    }
}

void Box_impl::remove_before(const Widget_impl * other) {
    if (other) {
        if (holders_.size() > 1) {
            auto i = holders_.begin();

            for (; i != holders_.end(); ++i) {
                if (other == i->wp_) {
                    break;
                }
            }

            if (holders_.end() != i) {
                if (holders_.begin() != i) {
                    --i;
                    rm_child(*i);
                    holders_.erase(i);
                }
            }
        }
    }
}

void Box_impl::remove_front() {
    if (!holders_.empty()) {
        rm_child(holders_.front());
        holders_.pop_front();
    }
}

void Box_impl::remove_back() {
    if (!holders_.empty()) {
        rm_child(holders_.back());
        holders_.pop_back();
    }
}

void Box_impl::clear() {
    if (!holders_.empty()) {
        holders_.clear();
        unparent_all();
        update_requisition();
        invalidate();
    }
}

bool Box_impl::empty() const {
    return holders_.empty();
}

void Box_impl::set_align(Align align) {
    if (align_ != align) {
        align_ = align;
        if (!empty()) { update_requisition(); queue_arrange(); }
    }
}

bool Box_impl::shrunk(const Widget_impl * wp) const {
    for (auto iter = holders_.begin(); iter != holders_.end(); ++iter) {
        if (wp == iter->wp_) {
            return iter->sh_;
        }
    }

    return false;
}

void Box_impl::shrink(Widget_impl * wp) {
    for (auto iter = holders_.begin(); iter != holders_.end(); ++iter) {
        if (wp == iter->wp_) {
            if (!iter->sh_) {
                iter->sh_ = true;
                update_requisition();
                queue_arrange();
            }

            break;
        }
    }
}

void Box_impl::expand(Widget_impl * wp) {
    for (auto iter = holders_.begin(); iter != holders_.end(); ++iter) {
        if (wp == iter->wp_) {
            if (iter->sh_) {
                iter->sh_ = false;
                update_requisition();
                queue_arrange();
            }

            break;
        }
    }
}

void Box_impl::shrink_all() {
    bool changed = false;

    for (auto iter = holders_.begin(); iter != holders_.end(); ++iter) {
        if (!iter->sh_) {
            iter->sh_ = true;
            changed = true;
        }
    }

    if (changed) {
        update_requisition();
        queue_arrange();
    }
}

void Box_impl::expand_all() {
    bool changed = false;

    for (auto iter = holders_.begin(); iter != holders_.end(); ++iter) {
        if (iter->sh_) {
            iter->sh_ = false;
            changed = true;
        }
    }

    if (changed) {
        update_requisition();
        queue_arrange();
    }
}

bool Box_impl::horizontal() const {
    return OR_RIGHT == orientation() || OR_LEFT == orientation();
}

void Box_impl::set_orientation(Orientation orient) {
    if (orient_ != orient) {
        orient_ = orient;
        if (signal_orientation_changed_) { (*signal_orientation_changed_)(); }
        if (!empty()) { update_requisition(); queue_arrange(); }
    }
}

void Box_impl::set_spacing(unsigned spacing) {
    if (spacing_ != spacing) {
        spacing_ = spacing;
        if (!empty()) { update_requisition(); queue_arrange(); }
    }
}

bool Box_impl::on_take_focus() {
    if (focused_child_ && focused_child_->take_focus()) {
        return true;
    }

    if (OR_EAST == orient_ || OR_SOUTH == orient_) {
        auto i = std::find_if(holders_.begin(), holders_.end(), [](Holder & hol) { return hol.wp_->take_focus(); } );
        return i != holders_.end() ? true : grab_focus();
    }

    auto i = std::find_if(holders_.rbegin(), holders_.rend(), [](Holder & hol) { return hol.wp_->take_focus(); } );
    return i != holders_.rend() ? true : grab_focus();
}

void Box_impl::focus_next() {
    if (OR_EAST == orient_ || OR_SOUTH == orient_) { on_forward(); }
    else { on_reverse(); }
}

void Box_impl::focus_previous() {
    if (OR_WEST == orient_ || OR_NORTH == orient_) { on_forward(); }
    else { on_reverse(); }
}

bool Box_impl::on_forward() {
    auto wi = focused_child_;
    auto i = std::find_if(holders_.begin(), holders_.end(), [wi](const Holder & hol) { return hol.wp_ == wi; } );

    if (i != holders_.end()) {
        auto j = std::find_if(++i, holders_.end(), [](Holder & hol) { return hol.wp_->take_focus(); } );
        return j != holders_.end();
    }

    return false;
}

bool Box_impl::on_reverse() {
    auto wi = focused_child_;
    auto i = std::find_if(holders_.rbegin(), holders_.rend(), [wi](const Holder & hol) { return hol.wp_ == wi; } );

    if (i != holders_.rend()) {
        auto j = std::find_if(++i, holders_.rend(), [](Holder & hol) { return hol.wp_->take_focus(); } );
        return j != holders_.rend();
    }

    return false;
}

signal<void()> & Box_impl::signal_orientation_changed() {
    if (!signal_orientation_changed_) { signal_orientation_changed_ = new signal<void()>; }
    return *signal_orientation_changed_;
}

} // namespace tau

//END
