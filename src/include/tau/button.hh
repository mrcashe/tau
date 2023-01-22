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

/// @file button.hh The Button_base, Button and Toggle classes.

#ifndef TAU_BUTTON_HH
#define TAU_BUTTON_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// %Button base.
/// @ingroup widget_group
class Button_base: public Widget {
protected:

    Button_base(Widget_ptr wp);

public:

    /// Set label.
    void set_label(const ustring & label);

    /// Set image widget.
    void set_image(Widget & img);

    /// Set icon.
    void set_icon(const ustring & icon_name, int icon_size);

    /// Show button relief.
    void show_relief();

    /// Hide button relief.
    void hide_relief();

    /// Determines is relief visible.
    bool relief_visible() const;
};

/// Push button.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Button: public Button_base {
public:

    /// @name Constructors
    /// @{

    /// Default constructor.
    /// Constructs basic %Button.
    Button();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Button(const Button & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Button & operator=(const Button & other) = default;

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
    Button(Widget_ptr wp);

    /// Constructor with label.
    Button(const ustring & label);

    /// Constructor with image.
    Button(Widget & img);

    /// Constructor with image and label.
    Button(Widget & img, const ustring & label);

    /// Constructor with icon.
    Button(const ustring & icon_name, int icon_size);

    /// Constructor with label and icon.
    Button(const ustring & label, const ustring & icon_name, int icon_size);

    /// Constructor with Action.
    /// Suitable for tool buttons. Uses @b MEDIUM_ICON size (see #Icon_size enum).
    /// @param action the action to be used.
    /// @param items action items to be used, see #Action_items enum.
    Button(Action & action, Action_items items=ACTION_ALL);

    /// Constructor with Action and icon size.
    /// @param action the action to be used.
    /// @param icon_size the icon size.
    /// @param items action items to be used, see #Action_items enum.
    Button(Action & action, int icon_size, Action_items items=ACTION_ALL);

    /// @}
    /// Enable autorepeat of mouse click.
    /// Disabled by default.
    void enable_repeat();

    /// Disable autorepeat of mouse click.
    void disable_repeat();

    /// Determines is autorepeat enabled.
    bool repeat_enabled() const;

    /// Set repeat period.
    /// @param delay_ms period in milliseconds before first repeat.
    /// @param interval_ms period in milliseconds starting second repeat. If 0, delay_ms used.
    void set_repeat_delay(unsigned delay_ms, unsigned interval_ms=0);

    /// Get repeat delay.
    unsigned repeat_delay() const;

    /// Get repeat intrval.
    unsigned repeat_interval() const;

    /// Signal emitted when button clicked.
    signal<void()> & signal_click();
};

/// %Toggle button.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Toggle: public Button_base {
public:

    /// @name Constructors
    /// @{

    /// Default constructor.
    /// Constructs basic %Toggle.
    Toggle();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Toggle(const Toggle & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Toggle & operator=(const Toggle & other) = default;

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
    Toggle(Widget_ptr wp);

    /// Constructor with label.
    Toggle(const ustring & label);

    /// Constructor with image.
    Toggle(Widget & img);

    /// Constructor with image and label.
    Toggle(Widget & img, const ustring & label);

    /// Constructor with icon.
    Toggle(const ustring & icon_name, int icon_size);

    /// Constructor with label and icon.
    Toggle(const ustring & label, const ustring & icon_name, int icon_size);

    /// Constructor with Action.
    /// Suitable for tool buttons. Uses @b MEDIUM_ICON size (see #Icon_size enum).
    /// @param action the action to be used.
    /// @param items action items to be used, see #Action_items enum.
    Toggle(Toggle_action & action, Action_items items=ACTION_ALL);

    /// Constructor with Action and icon size.
    /// @param action the action to be used.
    /// @param icon_size the icon size.
    /// @param items action items to be used, see #Action_items enum.
    Toggle(Toggle_action & action, int icon_size, Action_items items=ACTION_ALL);

    /// @}
    /// Toggle button.
    void toggle();

    /// Get toggle state.
    bool get() const;

    /// Signal emitted when button toggled.
    signal<void(bool)> & signal_toggle();
};

} // namespace tau

#endif // TAU_BUTTON_HH
