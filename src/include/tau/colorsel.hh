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

/// @file colorsel.hh The Colorsel class.

#ifndef TAU_COLORSEL_HH
#define TAU_COLORSEL_HH

#include <tau/color.hh>
#include <tau/widget.hh>

namespace tau {

/// %Color selection widget.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Colorsel: public Widget {
public:

    /// @name Constructors
    /// @{

    /// Constructor.
    Colorsel(const Color & initial_color=Color());

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Colorsel(const Colorsel & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Colorsel & operator=(const Colorsel & other) = default;

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
    Colorsel(Widget_ptr wp);

    /// @}
    /// Set color.
    void set_color(const Color & color);

    /// Get selected color.
    Color color() const;

    /// Get "Apply" action.
    Action & apply_action();

    /// Get "Apply" action.
    const Action & apply_action() const;

    /// Get "Cancel" action.
    Action & cancel_action();

    /// Get "Cancel" action.
    const Action & cancel_action() const;

    /// Signal emitted when color changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_color_changed(const Color & color);
    /// ~~~~~~~~~~~~~~~
    signal<void(const Color &)> & signal_color_changed();
};

} // namespace tau

#endif // TAU_COLORSEL_HH
