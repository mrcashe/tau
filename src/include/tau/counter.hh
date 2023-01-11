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

/// @file counter.hh Counter class.

#ifndef TAU_COUNTER_HH
#define TAU_COUNTER_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// @ingroup widget_group
/// Numeric value editor.
class Counter: public Widget {
public:

    /// Constructor with values.
    /// @param value assigned value
    /// @param max_value maximal value
    /// @param min_value minimal value
    Counter(double value=0.0, double max_value=0.0, double min_value=0.0);

    /// Constructor with border style and values.
    /// @param value assigned value
    /// @param max_value maximal value
    /// @param min_value minimal value
    Counter(Border_style bs, double value=0.0, double max_value=0.0, double min_value=0.0);

    /// Set border style.
    void set_border_style(Border_style bs);

    /// Get border style.
    Border_style border_style() const;

    /// Allow value edition.
    void allow_edit();

    /// Disallow value edition.
    void disallow_edit();

    /// Show increment and decrement buttons.
    /// @note The buttons are visible by default.
    void show_buttons();

    /// Hide increment and decrement buttons.
    /// @note The buttons are visible by default.
    void hide_buttons();

    /// Determines if increment and decrement buttons visible.
    bool buttons_visible() const;

    /// Test whether value edition allowed or not.
    bool edit_allowed() const;

    /// Set new value.
    /// @note This method does not emit signal_value_changed()
    void set_value(double value);

    /// Get current value.
    double value() const;

    /// Get text currently entered.
    ustring text() const;

    /// Set minimal value.
    /// @note Calling this method may cause value to be changed and signal_value_changed() emited.
    void set_min_value(double min_value);

    /// Get minimal value.
    double min_value() const;

    /// Set maximal value.
    /// @note Calling this method may cause value to be changed and signal_value_changed() emited.
    void set_max_value(double max_value);

    /// Get maximal value.
    double max_value() const;

    /// Set increment/decrement step value.
    /// The initial value is 1.0.
    /// @note step_value must be positive non-zero value.
    void set_step_value(double step_value);

    /// Get increment/decrement step value.
    double step_value() const;

    /// Sets the decimal precision to be used on value formatting.
    /// @param prec the precision.
    /// The initial precision is 0.
    void set_precision(int prec);

    /// Sets the decimal precision used on value formatting.
    int precision() const;

    /// Turns the decimal fixed format on.
    /// The initial fixed flag is true (on).
    void set_fixed();

    /// Turns the decimal fixed format off.
    /// The initial fixed flag is true (on).
    void unset_fixed();

    /// Determines whether decimal format fixed or not.
    bool fixed() const;

    /// Sets the field width to be used on value formatting.
    /// The initial value is 0.
    void set_width(int width);

    /// Gets the field width used on value formatting.
    int width() const;

    /// Sets fill character.
    /// The initial value is ' ' (space).
    void set_fill(char c);

    /// Gets fill character.
    char fill() const;

    /// Enable capital letters.
    /// The initial value is off (disabled).
    void set_caps();

    /// Disable capital letters.
    /// The initial value is off (disabled).
    void unset_caps();

    /// Determines whether capital letters enabled or not.
    bool caps() const;

    /// Set base.
    /// @param base the base value.
    /// Only base values of 2, 8 and 16 allowed. For other values ten will be assumed.
    /// The initial value is 10.
    void set_base(int base);

    /// Get base.
    int base() const;

    /// Increase value by 1 step value.
    void increase();

    /// Increase value by base step values.
    void increase_page();

    /// Decrease value by 1 step value.
    void decrease();

    /// Decrease value by base step values.
    void decrease_page();

    /// Append widget after counting value.
    /// @throw user_error if widget already inserted into another container.
    void append(Widget & w, bool shrink=false);

    /// Append text after counting value.
    /// @param text text to be appended.
    /// @param margin_left_hint left margin.
    /// @param margin_right_hint right margin.
    void append(const ustring & text, unsigned margin_left_hint=0, unsigned margin_right_hint=0);

    /// Prepend widget before counting value.
    /// @throw user_error if widget already inserted into another container.
    void prepend(Widget & w, bool shrink=false);

    /// Prepend text before counting value.
    /// @param text text to be prepended.
    /// @param margin_left_hint left margin.
    /// @param margin_right_hint right margin.
    void prepend(const ustring & text, unsigned margin_left_hint=0, unsigned margin_right_hint=0);

    /// Emits when entered text changed.
    signal<void(double)> & signal_value_changed();
};

} // namespace tau

#endif // TAU_COUNTER_HH
