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
    table_->signal_mouse_down().connect(fun(this, &Menubox_impl::on_table_mouse_down));
    table_->signal_mouse_motion().connect(fun(this, &Menubox_impl::on_table_mouse_motion));
    insert(table_);

    connect_action(up_action_);
    connect_action(down_action_);
    connect_action(left_action_);
    connect_action(right_action_);
}

void Menubox_impl::on_left() {
    if (auto pmenu = unset_parent_menu()) {
        end_modal();
        pmenu->child_menu_left();
        quit();
    }
}

void Menubox_impl::on_right() {
    if (!open_current()) {
        if (auto pmenu = unset_parent_menu()) {
            end_modal();
            pmenu->child_menu_right();
            quit();
        }
    }
}

void Menubox_impl::on_up() {
    select_prev();
}

void Menubox_impl::on_down() {
    select_next();
}

// "ESCAPE" came from the child menu.
void Menubox_impl::child_menu_escape() {
    reset_submenu();
    grab_modal();
}

void Menubox_impl::child_menu_left() {
    reset_submenu();
    grab_modal();
}

void Menubox_impl::child_menu_right() {
    reset_submenu();
    grab_modal();
}

void Menubox_impl::mark_item(Menu_item_ptr ip, bool select) {
    if (select) {
        Table::Span rng = table_->span(ip.get());

        if (rng.ymax > rng.ymin) {
            table_->select_row(rng.ymin);
        }
    }

    else {
        table_->unselect();
    }
}

// Quit menu if user press mouse button somewhere outside any menu.
bool Menubox_impl::on_popup_mouse_down(int mbt, int mm, const Point & pt) {
    if (auto wip = window()) {
        if (!Rect(wip->size()).contains(pt)) {
            end_modal();
            wip->ungrab_mouse();
//                 Point spt = pp->to_screen(pt);
//
//                 for (Menu_ptr mi = parent_menu(); mi; mi = mi->parent_menu()) {
//                     Point worg = mi->to_screen(Point());
//
//                     if (Rect(worg, mi->size()).contains(spt)) {
//                         end_modal();
//                         mi->signal_mouse_down()(nth_button, mm, spt-worg);
//                         return true;
//                     }
//                 }

            child_menu_quit();
        }
    }

    return false;
}

void Menubox_impl::popup(Window_impl * root, Widget_ptr self, const Point & origin, Gravity gravity, Menu_impl * pmenu) {
    if (root) {
        if (auto dp = root->display()) {
            pmenu_ = pmenu;
            auto wip = dp->create_popup(dp, root, origin, gravity);
            wip->style().redirect("background-menu", "background");
            wip->insert(self);
            wip->signal_mouse_down().connect(fun(this, &Menubox_impl::on_popup_mouse_down), true);
            wip->show();
            grab_modal();
            wip->grab_mouse();
            select_item(current_item());
        }
    }
}

void Menubox_impl::quit() {
    Menu_impl::quit();
    if (auto wip = window()) { wip->close(); }
}

bool Menubox_impl::on_table_mouse_down(int mbt, int mm, const Point & pt) {
    if (MBT_LEFT == mbt && !(mm & (MM_CONTROL|MM_SHIFT))) {
        for (auto ip: items_) {
            if (ip->enabled() && ip->visible()) {
                int y = pt.y(), ymin = ip->origin().y(), ymax = ymin+ip->size().height();

                if (y >= ymin && y < ymax) {
                    if (current_item_ != ip) { unselect_current(); select_item(ip); }
                    end_modal();
                    if (auto wip = window()) { wip->ungrab_mouse(); }
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
    if (auto ip = std::dynamic_pointer_cast<Menu_item_impl>(wp)) {
        wp->hint_margin(2, 2, 0, 0);
        table_->put(wp, 1, row, 1, 1, true, true);
        table_->set_column_margin(1, 2, 2);
        table_->set_row_margin(row, 2, 2);
        table_->align(wp.get(), ALIGN_START, ALIGN_CENTER);

        if (auto img_part = std::dynamic_pointer_cast<Menu_image>(ip)) {
            if (Widget_ptr image = img_part->image()) {
                image->hint_margin(2, 2, 0, 0);
                if (ip->disabled()) { image->disable(); }
                table_->put(image, 0, row, 1, 1, true, true);
                table_->set_column_margin(0, 2, 2);
                table_->set_row_margin(row, 2, 2);
                ip->signal_enable().connect(fun(image, &Widget_impl::enable));
                ip->signal_disable().connect(fun(image, &Widget_impl::disable));
            }
        }

        if (auto submenu = std::dynamic_pointer_cast<Submenu_impl>(ip)) {
            Widget_ptr arrow = submenu->arrow();
            arrow->hint_margin(2, 0, 0, 0);
            if (ip->disabled()) { arrow->disable(); }
            table_->put(arrow, 2, row, 1, 1, true, true);
            table_->set_column_margin(2, 2, 2);
            table_->set_row_margin(row, 2, 2);
            table_->align(arrow.get(), ALIGN_END, ALIGN_CENTER);
            ip->signal_enable().connect(fun(arrow, &Widget_impl::enable));
            ip->signal_disable().connect(fun(arrow, &Widget_impl::disable));
        }

        else if (auto action_item = std::dynamic_pointer_cast<Action_menu_impl>(ip)) {
            Text_ptr accel_text = action_item->accel_label();
            accel_text->hint_margin(2, 0, 0, 0);
            if (ip->disabled()) { accel_text->disable(); }
            table_->put(accel_text, 2, row, 1, 1, true, true);
            table_->set_column_margin(2, 2, 2);
            table_->set_row_margin(row, 2, 2);
            table_->align(accel_text.get(), ALIGN_END, ALIGN_CENTER);
            ip->signal_enable().connect(fun(accel_text, &Widget_impl::enable));
            ip->signal_disable().connect(fun(accel_text, &Widget_impl::disable));
        }

        else if (auto toggle_item = std::dynamic_pointer_cast<Toggle_menu_impl>(ip)) {
            Text_ptr accel_text = toggle_item->accel_label();
            accel_text->hint_margin(2, 0, 0, 0);
            if (ip->disabled()) { accel_text->disable(); }
            table_->put(accel_text, 2, row, 1, 1, true, true);
            table_->set_column_margin(2, 2, 2);
            table_->set_row_margin(row, 2, 2);
            table_->align(accel_text.get(), ALIGN_END, ALIGN_CENTER);
            ip->signal_enable().connect(fun(accel_text, &Widget_impl::enable));
            ip->signal_disable().connect(fun(accel_text, &Widget_impl::disable));

            if (Check_ptr check = toggle_item->check_ptr()) {
                check->hint_margin(2, 2, 0, 0);
                if (ip->disabled()) { check->disable(); }
                table_->put(check, 0, row, 1, 1, true, true);
                table_->set_column_margin(0, 2, 2);
                table_->set_row_margin(row, 2, 2);
                ip->signal_enable().connect(fun(check, &Widget_impl::enable));
                ip->signal_disable().connect(fun(check, &Widget_impl::disable));
            }
        }

        else if (auto check_item = std::dynamic_pointer_cast<Check_menu_impl>(ip)) {
            if (Check_ptr check = check_item->check_ptr()) {
                check->hint_margin(2, 2, 0, 0);
                if (ip->disabled()) { check->disable(); }
                table_->put(check, 0, row, 1, 1, true, true);
                table_->set_column_margin(0, 2, 2);
                table_->set_row_margin(row, 2, 2);
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
        table_->set_column_margin(rng.xmin, 2, 2);
        table_->set_row_margin(row, 2, 2);
    }

    widgets_.push_back(wp);
    update_separators();
}

void Menubox_impl::add_item(Menu_item_ptr ip) {
    items_.push_back(ip);
    ip->signal_enable().connect(fun(this, &Menu_impl::on_item_enable));
    ip->signal_disable().connect(fun(this, &Menu_impl::on_item_disable));
    if (!ip->disabled()) { thaw(); }
}

// Overrides pure Menu_impl.
void Menubox_impl::append_widget(Widget_ptr wp) {
    Table::Span rng = table_->span();
    put_widget(wp, rng.ymax > rng.ymin ? rng.ymax : 0);
}

// Overrides pure Menu_impl.
void Menubox_impl::prepend_widget(Widget_ptr wp) {
    Table::Span rng = table_->span();
    put_widget(wp, rng.ymax > rng.ymin ? rng.ymin-1 : 0);
}

// Overrides pure Menu_impl.
void Menubox_impl::insert_widget_before(Widget_ptr wp, Widget_cptr other) {
    Table::Span rng = table_->span(other.get());

    if (rng.ymax > rng.ymin) {
        table_->insert_rows(rng.ymin, 1);
        put_widget(wp, rng.ymin);
    }

    else {
        prepend_widget(wp);
    }
}

// Overrides pure Menu_impl.
void Menubox_impl::insert_widget_after(Widget_ptr wp, Widget_cptr other) {
    Table::Span rng = table_->span(other.get());

    if (rng.ymax > rng.ymin) {
        table_->insert_rows(rng.ymin+1, 1);
        put_widget(wp, rng.ymin+1);
    }

    else {
        append_widget(wp);
    }
}

// Overrides pure Menu_impl.
void Menubox_impl::remove_widget(Widget_ptr wp) {
    auto i = std::find(widgets_.begin(), widgets_.end(), wp);
    if (i != widgets_.end()) { widgets_.erase(i); }
    remove_item(wp);
    Table::Span rng = table_->span(wp.get());
    if (rng.ymax > rng.ymin) { table_->remove_rows(rng.ymin, 1); }
}

// Overrides pure Menu_impl.
void Menubox_impl::clear_widgets() {
    widgets_.clear();
    items_.clear();
    table_->clear();
}

void Menubox_impl::update_separators() {
    if (table_) {
        Table::Span trng = table_->span();

        for (Widget_ptr wp: widgets_) {
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
