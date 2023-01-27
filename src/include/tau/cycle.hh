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

/// @file cycle.hh The Cycle class.

#ifndef TAU_CYCLE_HH
#define TAU_CYCLE_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// %Container that show its children by cycle.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// To obtain which child is selected now, connect to child Widget::signal_select()
/// or (and) Widget::signal_unselect().
///
/// @ingroup container_group
/// @ingroup widget_group
class Cycle: public Widget {
public:

    /// @name Constructors
    /// @{

    /// Default constructor.
    Cycle(Border_style bs=BORDER_INSET);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Cycle(const Cycle & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Cycle & operator=(const Cycle & other) = default;

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
    Cycle(Widget_ptr wp);

    /// @}
    /// Set border style.
    void set_border_style(Border_style bs);

    /// Get border style.
    Border_style border_style() const;

    /// Add cycling widget.
    void add(Widget & w);

    /// Append widget after cycling widget.
    /// @throw user_error if w already inserted into another container.
    /// @throw internal_error if w has pure implementation pointer.
    void append(Widget & w, bool shrink=false);

    /// Append text after cycling widget.
    /// @param text text to be appended.
    /// @param margin_left left margin.
    /// @param margin_right right margin.
    /// @return a pointer to the created Text widget.
    Widget_ptr append(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    /// Prepend widget before cycling widget.
    /// @throw user_error if w already inserted into another container.
    /// @throw internal_error if w has pure implementation pointer.
    void prepend(Widget & w, bool shrink=false);

    /// Prepend text before cycling widget.
    /// @param text text to be prepended.
    /// @param margin_left left margin.
    /// @param margin_right right margin.
    /// @return a pointer to the created Text widget.
    Widget_ptr prepend(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    /// Remove any widget.
    void remove(Widget & w);
};

} // namespace tau

#endif // TAU_CYCLE_HH
