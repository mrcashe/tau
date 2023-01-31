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

/// @file twins.hh The Twins container class.

#ifndef TAU_TWINS_HH
#define TAU_TWINS_HH

#include <tau/enums.hh>
#include <tau/container.hh>

namespace tau {

/// A widget with two adjustable panes.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
/// @ingroup widget_group
class Twins: public Container {
public:

    /// @name Constructor and operators
    /// @{

    /// Default constructor.
    Twins(Orientation orient=OR_RIGHT, double ratio=0.5);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Twins(const Twins & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Twins & operator=(const Twins & other) = default;

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
    Twins(Widget_ptr wp);

    /// Assign implementation.
    ///
    /// @since 0.4.0
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Twins & operator=(Widget_ptr wp);

    /// @}
    /// Insert first widget.
    /// @throw user_error if w already inserted into another container.
    /// @throw internal_error if w has pure implementation pointer.
    void insert_first(Widget & w);

    /// Insert second widget.
    /// @throw user_error if w already inserted into another container.
    /// @throw internal_error if w has pure implementation pointer.
    void insert_second(Widget & w);

    /// Test if has first child.
    bool has_first() const;

    /// Test if has second child.
    bool has_second() const;

    /// Test if empty.
    bool empty() const;

    /// Remove first child.
    void remove_first();

    /// Remove second child.
    void remove_second();

    /// Remove both children.
    void clear();

    /// Set the ratio of the size of the first widget to the second.
    void set_ratio(double ratio);

    /// Get the ratio of the size of the first widget to the second
    double ratio() const;

    /// Signal emitted when user changed ratio by moving handle.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_ratio_changed(double ratio);
    /// ~~~~~~~~~~~~~~~
    signal<void(double)> & signal_ratio_changed();
};

} // namespace tau

#endif // TAU_TWINS_HH
