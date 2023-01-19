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

/// @file progress.hh The Progress class.

#ifndef TAU_PROGRESS_HH
#define TAU_PROGRESS_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// A widget which indicates progress visually.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Progress: public Widget {
public:

    /// Constructor with direction.
    /// @param vertical create vertical(true) or horizontal(false) progress bar.
    Progress(bool vertical=false);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Progress(const Progress & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Progress & operator=(const Progress & other) = default;

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
    Progress(Widget_ptr wp);

    /// Set border style.
    /// The default border style is BORDER_INSET.
    /// @param bs the border style.
    void set_border_style(Border_style bs);

    /// Get border style.
    /// The default border style is BORDER_INSET.
    /// @return border style.
    Border_style border_style() const;

    /// Set border color.
    /// @param color the color.
    void set_border_color(const Color & color);

    /// Set border color.
    void unset_border_color();

    /// Get border color.
    /// @return the border color.
    Color border_color() const;

    /// Set border width.
    /// @param npx border width in pixels.
    void set_border(unsigned npx);

    /// Get border width.
    /// @return border width in pixels.
    unsigned border() const;

    /// Set text align.
    /// @param align the align.
    /// ALIGN_START, ALIGN_CENTER and ALIGN_END supported.
    /// The default value is ALIGN_CENTER.
    /// @sa text_align.
    void set_text_align(Align align);

    /// Get text align.
    /// @return the text align.
    /// The default value is ALIGN_CENTER.
    /// @sa set_text_align.
    Align text_align() const;

    /// Set new value.
    /// @param value the value.
    /// The value cannot be less than minimal value
    /// and greater than maximal value.
    /// @sa set_min_value().
    void set_value(double value);

    /// Get current value.
    double value() const;

    /// Set minimal value.
    /// The default value is 0.0.
    /// Minimal value can be positive or negative.
    /// @param min_value the minimal value.
    void set_min_value(double min_value);

    /// Get minimal value.
    /// The default value is 0.0.
    /// Minimal value can be positive or negative.
    double min_value() const;

    /// Set maximal value.
    /// @param max_value the maximal value.
    /// The default value is 100.0.
    /// Maximal value can be positive or negative.
    void set_max_value(double max_value);

    /// Get maximal value.
    /// The default value is 100.0.
    /// Maximal value can be positive or negative.
    double max_value() const;

    /// Sets the decimal precision to be used on value formatting.
    /// @param prec the precision.
    /// The initial precision is 0.
    void set_precision(int prec);

    /// Sets the decimal precision used on value formatting.
    /// The initial precision is 0.
    int precision() const;

    /// Set format string.
    /// @param fmt the format string.
    /// Sets format string using during text generation procedure.
    /// Use "%$" qualifier to define current value.
    /// Use "%_" qualifier to define minimal value.
    /// Use "%^" qualifier to define maximal value.
    /// Use "%%" qualifier to define '%' character.
    /// Use an empty string to suppress message generation.
    /// The default value is "%$%%".
    /// @sa format.
    void set_format(const ustring & fmt);

    /// Get format string.
    /// @return the format string.
    /// The default value is "%$%%".
    /// @sa set_format.
    ustring format() const;
};

} // namespace tau

#endif // TAU_PROGRESS_HH
