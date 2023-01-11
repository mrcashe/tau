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
    void set_icon(const ustring & icon_name, unsigned icon_size);

    /// Show button relief.
    void show_relief();

    /// Hide button relief.
    void hide_relief();

    /// Determines is relief visible.
    bool relief_visible() const;

};

/// Push button.
/// @ingroup widget_group
class Button: public Button_base {
public:

    /// Default constructor.
    Button();

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
    /// @param action the Action to be used.
    /// @param use_label use label provided by action, if available.
    Button(Action & action, bool use_label=true);

    /// Constructor with Action and icon size.
    /// @param action the Action to be used.
    /// @param icon_size the icon size.
    /// @param use_label use label provided by action, if available.
    Button(Action & action, int icon_size, bool use_label=true);

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
/// @ingroup widget_group
class Toggle: public Button_base {
public:

    /// Default constructor.
    Toggle();

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
    /// @param action the Action to be used.
    /// @param use_label use label provided by action, if available.
    Toggle(Toggle_action & action, bool use_label=true);

    /// Constructor with Action and icon size.
    /// @param action the Action to be used.
    /// @param icon_size the icon size.
    /// @param use_label use label provided by action, if available.
    Toggle(Toggle_action & action, int icon_size, bool use_label=true);

    /// Toggle button.
    void toggle();

    /// Get toggle state.
    bool toggled() const;

    /// Signal emitted when button toggled.
    signal<void(bool)> & signal_toggle();
};

} // namespace tau

#endif // TAU_BUTTON_HH
