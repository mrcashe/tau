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

/// @file menu.hh Menu classes.

#ifndef TAU_MENU_HH
#define TAU_MENU_HH

#include <tau/enums.hh>
#include <tau/container.hh>

namespace tau {

/// %Menu item base.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Menu_item: public Widget {
public:

    /// Get label.
    ustring label() const;

protected:

    Menu_item(Widget_ptr wp);
};

/// %Menu item with Action.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Action_menu_item: public Menu_item {
public:

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Action_menu_item(const Action_menu_item & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Action_menu_item & operator=(const Action_menu_item & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Action_menu_item(Widget_ptr wp);

    /// Constructor with Action.
    Action_menu_item(Action & action);
};

/// %Menu item with Toggle_action.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Toggle_menu_item: public Menu_item {
public:

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Toggle_menu_item(const Toggle_menu_item & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Toggle_menu_item & operator=(const Toggle_menu_item & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Toggle_menu_item(Widget_ptr wp);

    /// Constructor with Toggle_action.
    Toggle_menu_item(Toggle_action & toggle_action, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

    /// Set check style.
    /// @param check_style the check style.
    void set_check_style(Check_style check_style);

    /// Get check style.
    Check_style check_style() const;

    /// Set border style.
    void set_border_style(Border_style border_style);

    /// Get border style.
    Border_style border_style() const;

    /// Set border width.
    void set_border_width(unsigned npx);

    /// Get border width.
    unsigned border_width() const;
};

/// %Menu item with other menu.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Submenu_item: public Menu_item {
public:

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Submenu_item(const Submenu_item & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Submenu_item & operator=(const Submenu_item & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Submenu_item(Widget_ptr wp);

    /// Constructor with label and submenu.
    Submenu_item(const ustring & label, Menu & menu);

    /// Constructor with label, submenu and icon name.
    Submenu_item(const ustring & label, Menu & menu, const ustring & icon_name);

    /// Set label.
    void set_label(const ustring & label);
};

/// %Menu item with slot.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Slot_menu_item: public Menu_item {
public:

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Slot_menu_item(const Slot_menu_item & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Slot_menu_item & operator=(const Slot_menu_item & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Slot_menu_item(Widget_ptr wp);

    /// Constructor with label and slot.
    Slot_menu_item(const ustring & label, const slot<void()> & slot_activate);

    /// Constructor with label, slot and icon name.
    Slot_menu_item(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name);

    /// Set label.
    void set_label(const ustring & label);
};

/// %Menu item with Check.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Check_menu_item: public Menu_item {
public:

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Check_menu_item(const Check_menu_item & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Check_menu_item & operator=(const Check_menu_item & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Check_menu_item(Widget_ptr wp);

    /// Constructor with label and initial state.
    /// @param label the label.
    /// @param checked the initial state.
    Check_menu_item(const ustring & label, bool checked=false);

    /// Constructor with label, check style and initial state.
    /// @param label the label.
    /// @param check_style the check style.
    /// @param checked the initial state.
    Check_menu_item(const ustring & label, Check_style check_style, bool checked=false);

    /// Constructor with label, border style and initial state.
    /// @param label the label.
    /// @param border_style the border style.
    /// @param checked the initial state.
    Check_menu_item(const ustring & label, Border_style border_style, bool checked=false);

    /// Constructor with label, check and border style and initial state.
    /// @param label the label.
    /// @param check_style the check style.
    /// @param border_style the border style.
    /// @param checked the initial state.
    Check_menu_item(const ustring & label, Check_style check_style, Border_style border_style, bool checked=false);

    /// Set label.
    void set_label(const ustring & label);

    /// Set check style.
    /// @param check_style the check style.
    void set_check_style(Check_style check_style);

    /// Get check style.
    Check_style check_style() const;

    /// Set border style.
    void set_border_style(Border_style border_style);

    /// Get border style.
    Border_style border_style() const;

    /// Set border width.
    void set_border_width(unsigned npx);

    /// Get border width.
    unsigned border_width() const;

    /// Set checked state.
    void check();

    /// Reset checked state.
    void uncheck();

    /// Toggle check.
    void toggle();

    /// Get current check value.
    bool checked() const;

    /// Join same radio group which other item belongs.
    void join(Check_menu_item & other);

    /// Signal emitted when check state changed to true.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// bool on_check();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_check();

    /// Signal emitted when check state changed to false.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// bool on_uncheck();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_uncheck();
};

/// Basic menu class.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
/// @ingroup widget_group
class Menu: public Container {
public:

    /// Append widget.
    void append(Widget & w);

    /// Append Action_menu_item.
    void append(Action & action);

    /// Append Toggle_menu_item.
    void append(Toggle_action & action, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

    /// Append Submenu_item.
    void append(const ustring & label, Menu & menu);

    /// Append Slot_menu_item.
    void append(const ustring & label, const slot<void()> & slot_activate);

    /// Append Slot_menu_item with icon.
    void append(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name);

    /// Append Separator.
    void append_separator(Separator_style separator_style=SEPARATOR_GROOVE);

    /// Prepend widget.
    void prepend(Widget & w);

    /// Prepend Action_menu_item.
    void prepend(Action & action);

    /// Prepend Toggle_menu_item.
    void prepend(Toggle_action & action, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

    /// Prpend Submenu_item.
    void prepend(const ustring & label, Menu & menu);

    /// Prepend Slot_menu_item.
    void prepend(const ustring & label, const slot<void()> & slot_activate);

    /// Prepend Slot_menu_item with icon.
    void prepend(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name);

    /// Prepend Separator.
    void prepend_separator(Separator_style separator_style=SEPARATOR_GROOVE);

    /// Insert widget before other widget.
    void insert_before(Widget & w, const Widget & other);

    /// Insert Action_menu_item before other widget.
    void insert_before(Action & action, const Widget & other);

    /// Insert Toggle_menu_item before other widget.
    void insert_before(Toggle_action & action, const Widget & other, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

    /// Insert Submenu_item before other widget.
    void insert_before(const ustring & label, Menu & menu, const Widget & other);

    /// Insert Slot_menu_item before other widget.
    void insert_before(const ustring & label, const slot<void()> & slot_activate, const Widget & other);

    /// Insert Slot_menu_item with icon before other widget.
    void insert_before(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name, const Widget & other);

    /// Insert Separator before other widget.
    void insert_separator_before(const Widget & other, Separator_style separator_style=SEPARATOR_GROOVE);

    /// Insert widget after other widget.
    void insert_after(Widget & w, const Widget & other);

    /// Insert Action_menu_item after other widget.
    void insert_after(Action & action, const Widget & other);

    /// Insert Toggle_menu_item after other widget.
    void insert_after(Toggle_action & action, const Widget & other, Check_style check_style=CHECK_VSTYLE, Border_style border_style=BORDER_INSET);

    /// Insert Submenu_item after other widget.
    void insert_after(const ustring & label, Menu & menu, const Widget & other);

    /// Insert Slot_menu_item after other widget.
    void insert_after(const ustring & label, const slot<void()> & slot_activate, const Widget & other);

    /// Insert Slot_menu_item with icon after other widget.
    void insert_after(const ustring & label, const slot<void()> & slot_activate, const ustring & icon_name, const Widget & other);

    /// Insert Separator after other widget.
    void insert_separator_after(const Widget & other, Separator_style separator_style=SEPARATOR_GROOVE);

    /// Remove widget.
    void remove(Widget & w);

    /// Remove all widgets.
    void clear();

protected:

    Menu(Widget_ptr wp);
};

/// %Popup menu.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
/// @ingroup widget_group
class Menubox: public Menu {
public:

    /// Constructor.
    Menubox();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Menubox(const Menubox & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Menubox & operator=(const Menubox & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Menubox(Widget_ptr wp);

    /// %Popup menu.
    void popup(Toplevel & parent, const Point & position, Gravity gravity=GRAVITY_NONE);
};

/// %Menu with items arranged horizontally.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
/// @ingroup widget_group
class Menubar: public Menu {
public:

    /// Constructor.
    Menubar();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Menubar(const Menubar & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Menubar & operator=(const Menubar & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Menubar(Widget_ptr wp);

    /// Activate menu.
    void activate();
};

} // namespace tau

#endif // TAU_MENU_HH
