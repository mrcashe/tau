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

#include <button-impl.hh>
#include <menu-impl.hh>
#include <scroller-impl.hh>
#include <window-impl.hh>
#include <iostream>

namespace tau {

Menu_impl::Menu_impl(Orientation orient):
    Roller_impl(orient)
{
    freeze();
    style_.redirect(STYLE_MENU_BACKGROUND, STYLE_BACKGROUND);
    style_.redirect(STYLE_MENU_BACKGROUND, STYLE_BUTTON_BACKGROUND);
    style_.redirect(STYLE_MENU_FOREGROUND, STYLE_FOREGROUND);
    signal_focus_in().connect(fun(this, &Menu_impl::mark));
    connect_action(enter_action_);
    connect_action(cancel_action_);
}

Menu_item_ptr Menu_impl::current_item() {
    if (current_item_) {
        return current_item_;
    }

    int min_pos = INT_MAX;
    Menu_item_ptr min_item;

    for (auto ip: items_) {
        if (!ip->disabled() && !ip->hidden()) {
            int pos;
            if (OR_RIGHT == orientation()) { pos = ip->origin().x(); }
            else { pos = ip->origin().y(); }
            if (pos < min_pos) { min_pos = pos; min_item = ip; }
        }
    }

    return min_item;
}

Menu_item_ptr Menu_impl::next_item() {
    if (Menu_item_ptr cur = current_item()) {
        int min_plus = INT_MAX, max_minus = INT_MAX;
        Menu_item_ptr ip_min_plus, ip_max_minus;

        for (auto ip: items_) {
            if (ip != cur) {
                if (ip->enabled() && !ip->hidden()) {
                    int d;

                    if (OR_RIGHT == orientation()) {
                        d = ip->origin().x()-cur->origin().x();
                    }

                    else {
                        d = ip->origin().y()-cur->origin().y();
                    }

                    if (d >= 0) {
                        if (d < min_plus) {
                            min_plus = d;
                            ip_min_plus = ip;
                        }
                    }

                    else {
                        if (d < max_minus) {
                            max_minus = d;
                            ip_max_minus = ip;
                        }
                    }
                }
            }
        }

        if (ip_min_plus) {
            return ip_min_plus;
        }

        if (ip_max_minus) {
            return ip_max_minus;
        }
    }

    return Menu_item_ptr();
}

Menu_item_ptr Menu_impl::prev_item() {
    if (Menu_item_ptr cur = current_item()) {
        int max_plus = INT_MIN, min_minus = INT_MIN;
        Menu_item_ptr ip_max_plus, ip_min_minus;

        for (auto ip: items_) {
            if (ip != cur) {
                if (ip->enabled() && !ip->hidden()) {
                    int d;

                    if (OR_RIGHT == orientation()) {
                        d = ip->origin().x()-cur->origin().x();
                    }

                    else {
                        d = ip->origin().y()-cur->origin().y();
                    }

                    if (d >= 0) {
                        if (d > max_plus) {
                            max_plus = d;
                            ip_max_plus = ip;
                        }
                    }

                    else {
                        if (d > min_minus) {
                            min_minus = d;
                            ip_min_minus = ip;
                        }
                    }
                }
            }
        }

        if (ip_min_minus) {
            return ip_min_minus;
        }

        if (ip_max_plus) {
            return ip_max_plus;
        }
    }

    return nullptr;
}

void Menu_impl::select_next() {
    if (auto item = next_item()) {
        unselect_current();
        select_item(item);
    }
}

void Menu_impl::select_prev() {
    if (auto item = prev_item()) {
        unselect_current();
        select_item(item);
    }
}

bool Menu_impl::emit_current() {
    if (auto item = current_item()) {
        if (item->enabled()) {
            if (auto slot_impl = std::dynamic_pointer_cast<Slot_menu_impl>(item)) {
                pass_quit();
                slot_impl->signal_activate()();
                return true;
            }

            if (auto action_impl = std::dynamic_pointer_cast<Action_menu_impl>(item)) {
                action_impl->exec();
                pass_quit();
                return true;
            }

            if (auto check_impl = std::dynamic_pointer_cast<Check_menu_impl>(item)) {
                check_impl->toggle();
                return true;
            }

            if (auto toggle_impl = std::dynamic_pointer_cast<Toggle_menu_impl>(item)) {
                toggle_impl->toggle();
                return true;
            }
        }
    }

    return false;
}

bool Menu_impl::open_current() {
    if (!enabled() || !current_item_ || submenu_) { return false; }

    auto sub = std::dynamic_pointer_cast<Submenu_impl>(current_item_);
    if (!sub) { return false; }

    auto mp = std::dynamic_pointer_cast<Menubox_impl>(sub->menu());
    if (!mp || mp->disabled() || mp->parent() || !mp->has_enabled_items()) { return false; }

    auto toplevel = root();
    if (!toplevel) { return false; }

    Point pos;
    Gravity gravity;

    if (OR_RIGHT == orientation()) {
        pos = current_item_->to_window(Point(0, current_item_->size().height()));
        gravity = GRAVITY_TOP_LEFT;
    }

    else {
        Point origin = current_item_->origin();
        int y = origin.y()-margin_top_hint()-2;  // FIXME from where this '2'?
        Point p1 = to_root(Point(size().width()+margin_right_hint(), y));
        Point p2 = to_root(Point(-margin_left_hint(), y));

        if (toplevel->size().iwidth()-p1.x() >= p2.x()) {
            pos = p1;
            gravity = GRAVITY_TOP_LEFT;
        }

        else {
            pos = p2;
            gravity = GRAVITY_TOP_RIGHT;
        }
    }

    submenu_ = mp;
    end_modal();
    if (parent_) { parent_->ungrab_mouse(); }
    mp->popup(toplevel, mp, pos, gravity, this);
    return true;
}

void Menu_impl::activate_current() {
    if (current_item_) {
        if (!open_current()) {
            emit_current();
        }
    }
}

void Menu_impl::select_item(Menu_item_ptr item) {
    close_submenu();
    current_item_ = item;
    mark();
}

void Menu_impl::remove_item(Widget_impl * wp) {
    auto iter = std::find_if(items_.begin(), items_.end(), [wp](Widget_ptr wpp) { return wp == wpp.get(); } );
    if (items_.end() != iter) { items_.erase(iter); }
}

Menu_item_ptr Menu_impl::item_ptr(Menu_item_impl * item) {
    for (auto ip: items_) {
        if (item == ip.get()) {
            return ip;
        }
    }

    return Menu_item_ptr();
}

void Menu_impl::unselect_current() {
    close_submenu();
    unmark();
    current_item_ = nullptr;
}

void Menu_impl::on_item_enable() {
    thaw();
}

void Menu_impl::on_item_disable() {
    if (!has_enabled_items()) {
        freeze();
    }
}

void Menu_impl::mark() {
    unmark();

    if (current_item_) {
        marked_item_ = current_item_.get();
        mark_item(marked_item_, true);
    }
}

void Menu_impl::unmark() {
    if (marked_item_) {
        mark_item(marked_item_, false);
        marked_item_ = nullptr;
    }
}

bool Menu_impl::has_enabled_items() const {
    return std::any_of(items_.begin(), items_.end(), [](Widget_ptr wp) { return !wp->disabled(); } );
}

void Menu_impl::cancel() {
    auto pmenu = unset_parent_menu();
    end_modal();
    quit();
    if (pmenu) { pmenu->child_menu_cancel(); }
}

// Overriden by Menubox_impl.
void Menu_impl::quit() {
    close_submenu();
    unselect_current();
    unset_parent_menu();
    signal_quit_();
}

void Menu_impl::close_submenu() {
    if (auto sm = submenu_) {
        submenu_.reset();
        sm->quit();
    }
}

void Menu_impl::reset_submenu() {
    if (auto sm = submenu_) {
        submenu_.reset();
        sm->unset_parent_menu();
    }
}

Menu_impl * Menu_impl::unset_parent_menu() {
    auto p = pmenu_;
    pmenu_ = nullptr;
    return p;
}

void Menu_impl::pass_quit() {
    auto pmenu = unset_parent_menu();
    end_modal();
    quit();
    if (pmenu) { pmenu->pass_quit(); }
}

} // namespace tau

//END
