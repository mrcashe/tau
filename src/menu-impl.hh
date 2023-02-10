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

    virtual void child_menu_cancel() = 0;
    virtual void child_menu_left() = 0;
    virtual void child_menu_right() = 0;

    void quit();

    Widget_ptr append_separator(Separator_style separator_style=SEPARATOR_GROOVE) {
        Separator_ptr sp = std::make_shared<Separator_impl>(separator_style);
        append(sp);
        return sp;
    }

    Widget_ptr prepend_separator(Separator_style separator_style=SEPARATOR_GROOVE) {
        Separator_ptr sp = std::make_shared<Separator_impl>(separator_style);
        prepend(sp);
        return sp;
    }

    Widget_ptr insert_separator_before(const Widget_impl * other, Separator_style separator_style=SEPARATOR_GROOVE) {
        Separator_ptr sp = std::make_shared<Separator_impl>(separator_style);
        insert_before(sp, other);
        return sp;
    }

    Widget_ptr insert_separator_after(const Widget_impl * other, Separator_style separator_style=SEPARATOR_GROOVE) {
        Separator_ptr sp = std::make_shared<Separator_impl>(separator_style);
        insert_after(sp, other);
        return sp;
    }

    Menu_impl * parent_menu() { return pmenu_; }
    const Menu_impl * parent_menu() const { return pmenu_; }

    Menu_impl * unset_parent_menu();
    void close_submenu();

    void on_item_enable();
    void on_item_disable();

    signal<void()> & signal_quit() { return signal_quit_; }

protected:

    using Items = std::list<Menu_item_ptr>;

    Items               items_;
    Orientation         submenu_or_ = OR_RIGHT;
    Menu_impl *         pmenu_ = nullptr;
    Menu_ptr            submenu_;
    Menu_item_ptr       current_item_;
    Menu_item_impl *    marked_item_ = nullptr;
    Action              enter_action_ { KC_ENTER, KM_NONE, fun(this, &Menu_impl::activate_current) };
    Action              cancel_action_ { "Escape Cancel", fun(this, &Menu_impl::cancel) };
    Action              home_action_ { KC_HOME, KM_NONE, fun(this, &Menu_impl::on_home) };
    Action              end_action_ { KC_END, KM_NONE, fun(this, &Menu_impl::on_end) };
    signal<void()>      signal_quit_;

protected:

    Menu_impl(Orientation orient);

    virtual void mark_item(Menu_item_impl * ip, bool select) = 0;

    void cancel();
    void select_item(Menu_item_ptr item);
    void remove_item(Widget_impl * wp);
    Menu_item_ptr current_item();
    Menu_item_ptr item_ptr(Menu_item_impl * item);
    Menu_item_ptr next_item();
    Menu_item_ptr prev_item();

    void unselect_current();
    void select_next();
    void select_prev();

    bool emit_current();
    bool open_current();
    void activate_current();
    void reset_submenu();
    void pass_quit();

private:

    void mark();
    void unmark();
    void step_scroller(bool increase);
    void on_home();
    void on_end();
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

    Menu_image():
        icon_(std::make_shared<Icon_impl>())
    {
    }

    explicit Menu_image(const ustring & icon_name):
        icon_(std::make_shared<Icon_impl>(icon_name, SMALL_ICON))
    {
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
    explicit Menu_item_impl(const ustring & label);
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

    explicit Action_menu_impl(Action & action);

    Action & action() {
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

    explicit Toggle_menu_impl(Toggle_action & toggle_action, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

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

    explicit Check_menu_impl(const ustring & label, bool checked=false);
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

    // Overrides Box_impl.
    void append(Widget_ptr wp, bool shrink=false) override;

    // Overrides Box_impl.
    void prepend(Widget_ptr wp, bool shrink=false) override;

    // Overrides Box_impl.
    void insert_before(Widget_ptr wp, const Widget_impl * other, bool shrink=false) override;

    // Overrides Box_impl.
    void insert_after(Widget_ptr wp, const Widget_impl * other, bool shrink=false) override;

    // Overrides Box_impl.
    void remove(Widget_impl * wp) override;

    // Overrides Box_impl.
    void clear() override;

    // Overrides pure Menu_impl.
    void child_menu_cancel() override;

    // Overrides pure Menu_impl.
    void child_menu_left() override;

    // Overrides pure Menu_impl.
    void child_menu_right() override;

    void activate();

protected:

    void mark_item(Menu_item_impl * ip, bool select) override;

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

    // Overrides Box_impl.
    void append(Widget_ptr wp, bool shrink=false) override;

    // Overrides Box_impl.
    void prepend(Widget_ptr wp, bool shrink=false) override;

    // Overrides Box_impl.
    void insert_before(Widget_ptr wp, const Widget_impl * other, bool shrink=false) override;

    // Overrides Box_impl.
    void insert_after(Widget_ptr wp, const Widget_impl * other, bool shrink=false) override;

    // Overrides Box_impl.
    void remove(Widget_impl * wp) override;

    // Overrides Box_impl.
    void clear() override;

    // Overrides pure Menu_impl.
    void child_menu_cancel() override;

    // Overrides pure Menu_impl.
    void child_menu_left() override;

    // Overrides pure Menu_impl.
    void child_menu_right() override;

    Window_ptr popup(Window_impl * parent_window, Widget_ptr self, const Point & position, Gravity gravity, Menu_impl * pmenu);
    Window_ptr popup(Window_impl * parent_window, Widget_ptr self, const Point & position, Menu_impl * pmenu);

protected:

    void mark_item(Menu_item_impl * ip, bool select) override;

private:

    using Widgets = std::vector<Widget_ptr>;

    Table_ptr   table_;
    Gravity     gravity_ = GRAVITY_NONE;
    Action      up_action_ { "Up", fun(this, &Menubox_impl::select_prev) };
    Action      down_action_ { "Down", fun(this, &Menubox_impl::select_next) };
    Action      left_action_ { "Left", fun(this, &Menubox_impl::on_left) };
    Action      right_action_ { "Right", fun(this, &Menubox_impl::on_right) };

private:

    void put_widget(Widget_ptr wp, int row);
    void add_item(Menu_item_ptr ip);
    void update_separators();

    bool on_popup_mouse_down(int mbt, int mm, const Point & pt);
    bool on_table_mouse_down(int mbt, int mm, const Point & pt);
    void on_table_mouse_motion(int mm, const Point & pt);
    void on_left();
    void on_right();
};

} // namespace tau

#endif // TAU_MENU_IMPL_HH
