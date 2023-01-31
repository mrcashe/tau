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

#include <tau/painter.hh>
#include <display-impl.hh>
#include <menu-impl.hh>
#include <table-impl.hh>
#include <popup-impl.hh>
#include <iostream>

namespace tau {

Menubox_impl::Menubox_impl():
    Menu_impl(OR_DOWN)
{
    hint_margin(3);

    table_ = std::make_shared<Table_impl>();
    table_->set_columns_margin(2, 2);
    table_->signal_mouse_down().connect(fun(this, &Menubox_impl::on_table_mouse_down));
    table_->signal_mouse_motion().connect(fun(this, &Menubox_impl::on_table_mouse_motion));
    insert(table_);

    connect_action(up_action_);
    connect_action(down_action_);
    connect_action(left_action_);
    connect_action(right_action_);
}

void Menubox_impl::on_left() {
    bool q = true;

    if (GRAVITY_RIGHT == gravity_ || GRAVITY_TOP_RIGHT == gravity_ || GRAVITY_BOTTOM_RIGHT == gravity_) {
        q = !open_current();
    }

    if (q) {
        if (auto pmenu = unset_parent_menu()) {
            quit();
            pmenu->child_menu_left();
        }
    }
}

void Menubox_impl::on_right() {
    bool q = true;

    if (GRAVITY_LEFT == gravity_ || GRAVITY_TOP_LEFT == gravity_ || GRAVITY_BOTTOM_LEFT == gravity_) {
        q = !open_current();
    }

    if (q) {
        if (auto pmenu = unset_parent_menu()) {
            quit();
            pmenu->child_menu_right();
        }
    }
}

void Menubox_impl::child_menu_cancel() {
    close_submenu();
    grab_modal();
    grab_mouse();
}

void Menubox_impl::child_menu_left() {
    close_submenu();
    grab_modal();
    grab_mouse();
}

void Menubox_impl::child_menu_right() {
    close_submenu();
    grab_modal();
    grab_mouse();
}

void Menubox_impl::mark_item(Menu_item_impl * ip, bool select) {
    if (select) {
        Table::Span rng = table_->span(ip);

        if (rng.ymax > rng.ymin) {
            table_->select_row(rng.ymin);
        }
    }

    else {
        table_->unselect();
    }
}

Window_ptr Menubox_impl::popup(Window_impl * root, Widget_ptr self, const Point & origin, Menu_impl * pmenu) {
    Point pos;
    Gravity gravity;

    if (OR_RIGHT == orientation()) {
        pos = to_parent(root, origin);
        gravity = GRAVITY_TOP_LEFT;
    }

    else {
        int y = origin.y()-margin_top_hint()-2;  // FIXME from where this '2'?
        Point p1 = to_parent(root, Point(size().width()+margin_right_hint(), y));
        Point p2 = to_parent(root, Point(-margin_left_hint(), y));

        if (root->size().iwidth()-p1.x() >= p2.x()) {
            pos = p1;
            gravity = GRAVITY_TOP_LEFT;
        }

        else {
            pos = p2;
            gravity = GRAVITY_TOP_RIGHT;
        }
    }

    return popup(root, self, origin, gravity, pmenu);
}

Window_ptr Menubox_impl::popup(Window_impl * root, Widget_ptr self, const Point & origin, Gravity gravity, Menu_impl * pmenu) {
    if (root) {
        if (auto dp = root->display()) {
            gravity_ = gravity;
            pmenu_ = pmenu;
            auto wip = dp->create_popup(dp, root, origin, gravity);
            wip->insert(self);
            wip->style().redirect(STYLE_MENU_BACKGROUND, STYLE_BACKGROUND);
            wip->signal_mouse_down().connect(fun(this, &Menubox_impl::on_popup_mouse_down), true);
            signal_quit_.connect(fun(wip, &Window_impl::close));
            wip->show();
            grab_modal();
            grab_mouse();
            select_item(current_item());
            return wip;
        }
    }

    return nullptr;
}

// Quit menu if user press mouse button somewhere outside any menu.
bool Menubox_impl::on_popup_mouse_down(int mbt, int mm, const Point & pt) {
    if (auto wip = window()) {
        if (!Rect(wip->size()).contains(pt)) {
            end_modal();
            ungrab_mouse();
            Point spt = wip->to_screen(pt);

            for (auto m = parent_menu(); m; m = m->parent_menu()) {
                if (dynamic_cast<Menubox_impl *>(m)) {
                    Point worg = m->to_screen();

                    if (Rect(worg, m->size()).contains(spt)) {
                        m->close_submenu();
                        m->grab_modal();
                        m->grab_mouse();
                        m->signal_mouse_down()(mbt, mm, spt-worg);
                        return true;
                    }
                }
            }

            cancel();
        }
    }

    return false;
}

bool Menubox_impl::on_table_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt && !(mm & (MM_CONTROL|MM_SHIFT))) {
        for (auto ip: items_) {
            if (ip->enabled() && ip->visible()) {
                int y = pt.y(), ymin = ip->origin().y(), ymax = ymin+ip->size().height();

                if (y >= ymin && y < ymax) {
                    if (current_item_ != ip) { unselect_current(); select_item(ip); }
                    end_modal();
                    ungrab_mouse();
                    activate_current();
                    break;
                }
            }
        }

        return true;
    }

    return false;
}

void Menubox_impl::on_table_mouse_motion(int mm, const Point & pt) {
    for (auto ip: items_) {
        if (ip->enabled() && ip->visible()) {
            int y = pt.y(), ymin = ip->origin().y(), ymax = ymin+ip->size().height();

            if (y >= ymin && y < ymax) {
                if (current_item_ != ip) {
                    unselect_current();
                    select_item(ip);
                }

                break;
            }
        }
    }
}

void Menubox_impl::put_widget(Widget_ptr wp, int row) {
    table_->set_row_margin(row, 2, 2);

    if (auto ip = std::dynamic_pointer_cast<Menu_item_impl>(wp)) {
        wp->hint_margin(2, 2, 0, 0);
        table_->put(wp, 1, row, 1, 1, true, true);
        table_->align(wp.get(), ALIGN_START, ALIGN_CENTER);

        if (auto img_part = std::dynamic_pointer_cast<Menu_image>(ip)) {
            if (Widget_ptr image = img_part->image()) {
                image->hint_margin(2, 2, 0, 0);
                if (ip->disabled()) { image->disable(); }
                table_->put(image, 0, row, 1, 1, true, true);
                ip->signal_enable().connect(fun(image, &Widget_impl::enable));
                ip->signal_disable().connect(fun(image, &Widget_impl::disable));
            }
        }

        if (auto submenu = std::dynamic_pointer_cast<Submenu_impl>(ip)) {
            Widget_ptr arrow = submenu->arrow();
            arrow->hint_margin(2, 0, 0, 0);
            if (ip->disabled()) { arrow->disable(); }
            table_->put(arrow, 2, row, 1, 1, true, true);
            table_->align(arrow.get(), ALIGN_END, ALIGN_CENTER);
            ip->signal_enable().connect(fun(arrow, &Widget_impl::enable));
            ip->signal_disable().connect(fun(arrow, &Widget_impl::disable));
        }

        else if (auto action_item = std::dynamic_pointer_cast<Action_menu_impl>(ip)) {
            Text_ptr accel_text = action_item->accel_label();
            accel_text->hint_margin(2, 0, 0, 0);
            if (ip->disabled()) { accel_text->disable(); }
            table_->put(accel_text, 2, row, 1, 1, true, true);
            table_->align(accel_text.get(), ALIGN_END, ALIGN_CENTER);
            ip->signal_enable().connect(fun(accel_text, &Widget_impl::enable));
            ip->signal_disable().connect(fun(accel_text, &Widget_impl::disable));
        }

        else if (auto toggle_item = std::dynamic_pointer_cast<Toggle_menu_impl>(ip)) {
            Text_ptr accel_text = toggle_item->accel_label();
            accel_text->hint_margin(2, 0, 0, 0);
            if (ip->disabled()) { accel_text->disable(); }
            table_->put(accel_text, 2, row, 1, 1, true, true);
            table_->align(accel_text.get(), ALIGN_END, ALIGN_CENTER);
            ip->signal_enable().connect(fun(accel_text, &Widget_impl::enable));
            ip->signal_disable().connect(fun(accel_text, &Widget_impl::disable));

            if (Check_ptr check = toggle_item->check_ptr()) {
                check->hint_margin(2, 2, 0, 0);
                if (ip->disabled()) { check->disable(); }
                table_->put(check, 0, row, 1, 1, true, true);
                ip->signal_enable().connect(fun(check, &Widget_impl::enable));
                ip->signal_disable().connect(fun(check, &Widget_impl::disable));
            }
        }

        else if (auto check_item = std::dynamic_pointer_cast<Check_menu_impl>(ip)) {
            if (Check_ptr check = check_item->check_ptr()) {
                check->hint_margin(2, 2, 0, 0);
                if (ip->disabled()) { check->disable(); }
                table_->put(check, 0, row, 1, 1, true, true);
                ip->signal_enable().connect(fun(check, &Widget_impl::enable));
                ip->signal_disable().connect(fun(check, &Widget_impl::disable));
            }
        }

        add_item(ip);
    }

    else {
        unsigned w;
        Table::Span rng = table_->span();
        if (rng.xmax > rng.xmin) { w = rng.xmax-rng.xmin; }
        else { rng.xmin = 0, w = 1; }
        table_->put(wp, rng.xmin, row, w, 1, false, true);
    }

    update_separators();
}

void Menubox_impl::add_item(Menu_item_ptr ip) {
    items_.push_back(ip);
    ip->signal_enable().connect(fun(this, &Menu_impl::on_item_enable));
    ip->signal_disable().connect(fun(this, &Menu_impl::on_item_disable));
    if (!ip->disabled()) { thaw(); }
}

// Overrides Box_impl.
void Menubox_impl::append(Widget_ptr wp, bool) {
    Table::Span rng = table_->span();
    put_widget(wp, rng.ymax > rng.ymin ? rng.ymax : 0);
}

// Overrides Box_impl.
void Menubox_impl::prepend(Widget_ptr wp, bool) {
    Table::Span rng = table_->span();
    put_widget(wp, rng.ymax > rng.ymin ? rng.ymin-1 : 0);
}

// Overrides Box_impl.
void Menubox_impl::insert_before(Widget_ptr wp, const Widget_impl * other, bool) {
    Table::Span rng = table_->span(other);

    if (rng.ymax > rng.ymin) {
        table_->insert_rows(rng.ymin, 1);
        put_widget(wp, rng.ymin);
    }

    else {
        prepend(wp);
    }
}

// Overrides Box_impl.
void Menubox_impl::insert_after(Widget_ptr wp, const Widget_impl * other, bool) {
    Table::Span rng = table_->span(other);

    if (rng.ymax > rng.ymin) {
        table_->insert_rows(rng.ymin+1, 1);
        put_widget(wp, rng.ymin+1);
    }

    else {
        append(wp);
    }
}

// Overrides Box_impl.
void Menubox_impl::remove(Widget_impl * wp) {
    remove_item(wp);
    Table::Span rng = table_->span(wp);
    if (rng.ymax > rng.ymin) { table_->remove_rows(rng.ymin, 1); }
}

// Overrides Box_impl.
void Menubox_impl::clear() {
    items_.clear();
    table_->clear();
}

void Menubox_impl::update_separators() {
    if (table_) {
        Table::Span trng = table_->span();

        for (Widget_ptr wp: table_->children()) {
            if (auto sep = std::dynamic_pointer_cast<Separator_impl>(wp)) {
                int x, y, w, h;
                Table::Span wrng = table_->span(sep.get());

                if (trng.xmax > trng.xmin) {
                    x = trng.xmin;
                    w = trng.xmax-trng.xmin;
                }

                else {
                    x = 0;
                    w = 1;
                }

                if (wrng.ymax > wrng.ymin) {
                    y = wrng.ymin;
                    h = wrng.ymax-wrng.ymin;
                }

                else {
                    y = 0;
                    h = 1;
                }

                table_->respan(sep.get(), x, y, w, h);
            }
        }

        trng = table_->span();

        if (trng.xmax > trng.xmin && trng.ymax > trng.ymin) {
            bool fsep = true;

            for (int yy = trng.ymin; yy < trng.ymax; ++yy) {
                auto v = table_->children_within_range(trng.xmin, yy, trng.xmax, yy+1);

                if (1 == v.size()) {
                    if (auto sep = dynamic_cast<Separator_impl *>(v[0])) {
                        if (fsep) { sep->hide(); }
                        else { sep->show(); }
                        fsep = true;
                    }

                    else {
                        fsep = false;
                    }
                }

                else {
                    fsep = false;
                }
            }

            fsep = false;

            for (int yy = trng.ymax-1; yy >= trng.ymin; --yy) {
                auto v = table_->children_within_range(trng.xmin, yy, trng.xmax, trng.ymin+1);

                if (1 == v.size()) {
                    if (auto sep = dynamic_cast<Separator_impl *>(v[0])) {
                        if (fsep) { sep->hide(); }
                        fsep = true;
                    }

                    else {
                        break;
                    }
                }

                else {
                    break;
                }
            }
        }
    }
}

} // namespace tau

//END
