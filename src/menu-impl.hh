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

#ifndef TAU_MENU_IMPL_HH
#define TAU_MENU_IMPL_HH

#include <tau/enums.hh>
#include <tau/action.hh>
#include <check-impl.hh>
#include <icon-impl.hh>
#include <image-impl.hh>
#include <roller-impl.hh>
#include <separator-impl.hh>
#include <text-impl.hh>

namespace tau {

class Menu_impl: public Roller_impl {
public:

    bool has_enabled_items() const;

    virtual void append_widget(Widget_ptr wp) = 0;
    virtual void prepend_widget(Widget_ptr wp) = 0;
    virtual void insert_widget_before(Widget_ptr wp, Widget_cptr other) = 0;
    virtual void insert_widget_after(Widget_ptr wp, Widget_cptr other) = 0;
    virtual void remove_widget(Widget_ptr wp) = 0;
    virtual void clear_widgets() = 0;

    virtual void child_menu_escape() = 0;
    virtual void child_menu_left() = 0;
    virtual void child_menu_right() = 0;

    // Overriden by Menubox_impl.
    virtual void quit();

    void append_separator(Separator_style separator_style=SEPARATOR_GROOVE) {
        append_widget(std::make_shared<Separator_impl>(separator_style));
    }

    void prepend_separator(Separator_style separator_style=SEPARATOR_GROOVE) {
        prepend_widget(std::make_shared<Separator_impl>(separator_style));
    }

    void insert_separator_before(Widget_cptr other, Separator_style separator_style=SEPARATOR_GROOVE) {
        insert_widget_before(std::make_shared<Separator_impl>(separator_style), other);
    }

    void insert_separator_after(Widget_cptr other, Separator_style separator_style=SEPARATOR_GROOVE) {
        insert_widget_after(std::make_shared<Separator_impl>(separator_style), other);
    }

    void child_menu_quit();

    Menu_impl * parent_menu() { return pmenu_; }
    const Menu_impl * parent_menu() const { return pmenu_; }

    Menu_impl * unset_parent_menu();

    void on_item_enable();
    void on_item_disable();

protected:

    Menu_impl(Orientation orient);

    virtual void mark_item(Menu_item_ptr ip, bool select) = 0;

    void select_item(Menu_item_ptr item);
    void remove_item(Widget_ptr wp);
    Menu_item_ptr current_item();
    Menu_item_ptr item_ptr(Menu_item_impl * item);
    Menu_item_ptr next_item();
    Menu_item_ptr prev_item();

    void unselect_current();
    void select_next();
    void select_prev();

    bool emit_current();
    bool open_current();
    bool activate_current();
    void close_submenu();
    void reset_submenu();
    void pass_quit();

protected:

    using Items = std::list<Menu_item_ptr>;

    Items           items_;
    Orientation     submenu_or_ = OR_RIGHT;
    Menu_impl *     pmenu_ = nullptr;
    Menu_ptr        submenu_;
    Menu_item_ptr   current_item_;
    Menu_item_ptr   marked_item_;

private:

    Action          enter_action_ { "Enter", fun(this, &Menu_impl::on_enter) };
    Accel           escape_accel_ { "Escape", fun(this, &Menu_impl::on_escape) };

private:

    void on_enter();
    bool on_escape();
    void on_focus_in();

    void mark();
    void unmark();
    void step_scroller(bool increase);
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Menu_image {
public:

    Icon_ptr image() {
        return icon_;
    }

protected:

    Icon_ptr icon_;

    Menu_image() {
        icon_ = std::make_shared<Icon_impl>();
    }

    Menu_image(const ustring & icon_name) {
        icon_ = std::make_shared<Icon_impl>(icon_name, SMALL_ICON);
    }

    void assign_icon(const ustring & icon_name) {
        icon_->assign(icon_name, SMALL_ICON);
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Menu_item_impl: public Text_impl {
protected:

    Menu_item_impl();
    Menu_item_impl(const ustring & label);
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Submenu_impl: public Menu_item_impl, public Menu_image {
public:

    Submenu_impl(const ustring & label, Menu_ptr menu);
    Submenu_impl(const ustring & label, Menu_ptr menu, const ustring & image_name);

    Menu_ptr menu() {
        return menu_;
    }

    Menu_cptr menu() const {
        return menu_;
    }

    Widget_ptr arrow() {
        return arrow_;
    }

    Widget_cptr arrow() const {
        return arrow_;
    }

private:

    void init();
    void on_menu_enable();
    void on_menu_disable();

private:

    Menu_ptr    menu_;
    Widget_ptr  arrow_;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Action_menu_impl: public Menu_item_impl, public Menu_image {
public:

    Action_menu_impl(Action & action);

    Action & action() {
        return action_;
    }

    const Action & action() const {
        return action_;
    }

    Text_ptr accel_label() {
        return accel_label_;
    }

    void exec() {
        if (enabled() && !destroyed_) {
            action_.exec();
        }
    }

private:

    Action &    action_;
    bool        destroyed_ = false;
    Text_ptr    accel_label_;
    connection  accel_changed_cx_;

private:

    void update_icon();
    void update_label();
    void update_accels();
    void on_label_changed(const ustring & label);

    void on_accel_added(Accel & accel);
    void on_accel_changed(Accel & accel);
    void on_accel_removed(Accel & accel);
    void on_action_destroy();
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Toggle_menu_impl: public Menu_item_impl {
public:

    Toggle_menu_impl(Toggle_action & toggle_action, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

    Text_ptr accel_label() {
        return accel_label_;
    }

    Check_ptr check_ptr() {
        return check_;
    }

    Check_cptr check_ptr() const {
        return check_;
    }

    Toggle_action & action() {
        return action_;
    }

    const Toggle_action & action() const {
        return action_;
    }

    void toggle();

    void set_check_style(Check_style check_style);
    Check_style check_style() const;

    void set_border_style(Border_style border_style);
    Border_style border_style() const;

    void set_border_width(unsigned npx);
    unsigned border_width() const;

    signal<void()> & signal_quit() {
        return signal_quit_;
    }

private:

    void update_label();
    void update_accels();
    void on_label_changed(const ustring & label);
    void on_accel_added(Accel & accel);
    void on_accel_changed(Accel & accel);
    void on_accel_removed(Accel & accel);
    void on_toggle();
    void on_action_toggle(bool toggled);
    void on_action_destroy();

private:

    Toggle_action & action_;
    Check_ptr       check_;
    Text_ptr        accel_label_;
    signal<void()>  signal_quit_;
    connection      check_cx_;
    connection      uncheck_cx_;
    connection      accel_changed_cx_;
    bool            destroyed_ = false;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Slot_menu_impl: public Menu_item_impl, public Menu_image {
public:

    Slot_menu_impl(const ustring & label, const slot<void()> & slot_activate);
    Slot_menu_impl(const ustring & label, const slot<void()> & slot_activate, const ustring & image_name);
    Slot_menu_impl(const ustring & label, const slot<void()> & slot_activate, Pixmap_ptr pix);

    signal<void()> & signal_activate() {
        return signal_activate_;
    }

private:

    signal<void()> signal_activate_;

private:

    void init();
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Check_menu_impl: public Menu_item_impl {
public:

    Check_menu_impl(const ustring & label, bool checked=false);
    Check_menu_impl(const ustring & label, Check_style check_style, bool checked=false);
    Check_menu_impl(const ustring & label, Border_style border_style, bool checked=false);
    Check_menu_impl(const ustring & label, Check_style check_style, Border_style border_style, bool checked=false);

    Check_ptr check_ptr() { return check_; }
    Check_cptr check_ptr() const { return check_; }

    void set_check_style(Check_style check_style);
    Check_style check_style() const;

    void set_border_style(Border_style border_style);
    Border_style border_style() const;

    void set_border_width(unsigned npx);
    unsigned border_width() const;

    void check();
    void uncheck();
    void toggle();
    bool checked() const;
    void join(Check_menu_ptr other);

    signal<void()> & signal_check() { return signal_check_; }
    signal<void()> & signal_uncheck() { return signal_uncheck_; }
    signal<void()> & signal_quit() { return signal_quit_; }

private:

    Check_ptr    check_;

    signal<void()> signal_check_;
    signal<void()> signal_uncheck_;
    signal<void()> signal_quit_;

private:

    void init();

    void on_check();
    void on_uncheck();
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Menubar_impl: public Menu_impl {
public:

    Menubar_impl();

    // Overrides pure Menu_impl.
    void append_widget(Widget_ptr wp) override;

    // Overrides pure Menu_impl.
    void prepend_widget(Widget_ptr wp) override;

    // Overrides pure Menu_impl.
    void insert_widget_before(Widget_ptr wp, Widget_cptr other) override;

    // Overrides pure Menu_impl.
    void insert_widget_after(Widget_ptr wp, Widget_cptr other) override;

    // Overrides pure Menu_impl.
    void remove_widget(Widget_ptr wp) override;

    // Overrides pure Menu_impl.
    void clear_widgets() override;

    // Overrides pure Menu_impl.
    void child_menu_escape() override;

    // Overrides pure Menu_impl.
    void child_menu_left() override;

    // Overrides pure Menu_impl.
    void child_menu_right() override;

    void activate();

protected:

    void mark_item(Menu_item_ptr ip, bool select) override;

private:

    Box_ptr     box_;
    Action      left_action_ {"Left", fun(this, &Menubar_impl::on_left) };
    Action      right_action_ {"Right", fun(this, &Menubar_impl::on_right) };

private:

    void add_item(Menu_item_ptr ip);
    void on_left();
    void on_right();
    bool on_item_mouse_down(int mbt, int mm, const Point & pt, Menu_item_impl * ip);
    void on_item_mouse_enter(const Point & pt, Menu_item_impl * item);
    void on_mouse_leave();
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Menubox_impl: public Menu_impl {
public:

    Menubox_impl();

    // Overrides pure Menu_impl.
    void append_widget(Widget_ptr wp) override;

    // Overrides pure Menu_impl.
    void prepend_widget(Widget_ptr wp) override;

    // Overrides pure Menu_impl.
    void insert_widget_before(Widget_ptr wp, Widget_cptr other) override;

    // Overrides pure Menu_impl.
    void insert_widget_after(Widget_ptr wp, Widget_cptr other) override;

    // Overrides pure Menu_impl.
    void remove_widget(Widget_ptr wp) override;

    // Overrides pure Menu_impl.
    void clear_widgets() override;

    // Overrides pure Menu_impl.
    void child_menu_escape() override;

    // Overrides pure Menu_impl.
    void child_menu_left() override;

    // Overrides pure Menu_impl.
    void child_menu_right() override;

    // Overrides Menu_impl.
    void quit() override;

    void popup(Window_impl * parent_window, Widget_ptr self, const Point & position, Gravity gravity, Menu_impl * pmenu);

protected:

    void mark_item(Menu_item_ptr ip, bool select) override;

private:

    using Widgets = std::vector<Widget_ptr>;

    Widgets     widgets_;
    Table_ptr   table_;
    Action      up_action_ {"Up", fun(this, &Menubox_impl::on_up) };
    Action      down_action_ {"Down", fun(this, &Menubox_impl::on_down) };
    Action      left_action_ {"Left", fun(this, &Menubox_impl::on_left) };
    Action      right_action_ {"Right", fun(this, &Menubox_impl::on_right) };

private:

    void put_widget(Widget_ptr wp, int row);
    void add_item(Menu_item_ptr ip);
    void update_separators();

    bool on_popup_mouse_down(int mbt, int mm, const Point & pt);
    bool on_table_mouse_down(int mbt, int mm, const Point & pt);
    void on_table_mouse_motion(int mm, const Point & pt);
    void on_up();
    void on_down();
    void on_left();
    void on_right();
};

} // namespace tau

#endif // TAU_MENU_IMPL_HH
