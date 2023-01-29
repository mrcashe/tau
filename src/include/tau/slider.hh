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

/// @file slider.hh The Slider class.

#ifndef TAU_SLIDER_HH
#define TAU_SLIDER_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// A slider.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Slider: public Widget {
public:

    /// @name Constructor and operators
    /// @{

    /// Default constructor with automatic orientation recognition.
    Slider();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Slider(const Slider & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Slider & operator=(const Slider & other) = default;

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
    Slider(Widget_ptr wp);

    /// Constructor with orientation.
    Slider(Orientation orient, bool autohide=false);

    /// Constructor with Scroller and automatic orientation.
    Slider(Scroller & scroller);

    /// Constructor with Scroller and orientation.
    Slider(Scroller & scroller, Orientation orient, bool autohide=false);

    /// @}
    /// Get orientation.
    Orientation orientation() const;

    /// Set orientation.
    void set_orientation(Orientation orient);

    /// Show buttons.
    /// Buttons are visible by default.
    void show_buttons();

    /// Hide buttons.
    /// Buttons are visible by default.
    void hide_buttons();

    /// Test if buttons visible.
    /// Buttons are visible by default.
    bool buttons_visible() const;

    /// Set slider location.
    void set_location(double loc);

    /// Get slider location.
    double location() const;

    /// Set range.
    void set_range(double range);

    /// Get range.
    double range() const;

    /// Set location and range.
    void set(double loc, double range);

    signal<void(double)> & signal_location_changed();
};

} // namespace tau

#endif // TAU_SLIDER_HH
