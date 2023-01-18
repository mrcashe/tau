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

/// @file card.hh The Card class.

#ifndef TAU_CARD_HH
#define TAU_CARD_HH

#include <tau/container.hh>

namespace tau {

/// %Container that can display only one its child at a time.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// To show or hide particular child, simply call Widget::show() or
/// Widget::hide() on that child.
///
/// You also can cycle over children using Card::show_next() and
/// Card::show_previous() methods.
///
/// @ingroup container_group
/// @ingroup widget_group
class Card: public Container {
public:

    /// @name Constructors
    /// @{

    /// Default constructor.
    /// Constructs basic %Card @ref container_group "container".
    Card();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Card(const Card & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Card & operator=(const Card & other) = default;

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
    Card(Widget_ptr wp);

    /// @}
    /// Add child into container.
    /// @throw user_error if widget already inserted into another container.
    /// @throw internal_error if w has pure implementation pointer.
    ///
    /// @sa remove()
    /// @sa remove_current()
    /// @sa clear()
    void insert(Widget & w);

    /// Remove current child.
    ///
    /// @sa insert()
    /// @sa remove()
    /// @sa clear()
    void remove_current();

    /// Remove child from the container.
    ///
    /// @sa insert()
    /// @sa remove_current()
    /// @sa clear()
    void remove(Widget & w);

    /// Remove all children.
    ///
    /// @sa insert()
    /// @sa remove_current()
    /// @sa remove()
    void clear();

    /// Test if empty.
    bool empty() const;

    /// Show next child.
    void show_next();

    /// Show previous child.
    void show_previous();
};

} // namespace tau

#endif // TAU_CARD_HH
