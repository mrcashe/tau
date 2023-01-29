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

#ifndef TAU_ROLLER_HH
#define TAU_ROLLER_HH

/// @file roller.hh The Roller class.

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// Single dimension scrolling container with automatically hiding scrolling arrows.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
/// @ingroup widget_group
class Roller: public Widget {
public:

    /// Constructor.
    /// @param orient Orientation.
    /// @param autohide Automatically show or hide scroll buttons.
    Roller(Orientation orient, bool autohide=true);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Roller(const Roller & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Roller & operator=(const Roller & other) = default;

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
    Roller(Widget_ptr wp);

    /// Insert child.
    /// @throw user_error if widget already inserted into another container.
    /// @throw internal_error if w has pure implementation pointer.
    void insert(Widget & w);

    /// Remove child.
    void clear();

    /// Roll to specified position.
    void roll_to(int pos);

    /// Get current offset.
    int offset() const;

    /// Automatically show or hide scroll buttons.
    void set_autohide();

    /// Show scroll buttons, do not hide automatically.
    void unset_autohide();

    /// Test if autohide option is set.
    bool autohide() const;

    /// Set position increment step.
    void set_step(int step);

    /// Get position increment step.
    int step() const;

    /// Set widget for up/left button.
    void set_start(Widget & w);

    /// Set widget for down/right button.
    void set_end(Widget & w);
};

} // namespace tau

#endif // TAU_ROLLER_HH
