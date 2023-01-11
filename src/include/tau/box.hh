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

/// @file box.hh The Box container class.

#ifndef TAU_BOX_HH
#define TAU_BOX_HH

#include <tau/enums.hh>
#include <tau/container.hh>

namespace tau {

/// Single dimension container.
/// The %Box arranges its multiple children in line. Depending on the orientation,
/// widgets are located from right to left, from left to right, from top to bottom
/// or from bottom to top.
///
/// Depending on whether the vertical box or horizontal, all widgets have the same
/// height (for horizontal boxes) or width (for vertical boxes).
///
/// The %Box supports spacing - an additional space between children.
///
/// Each child can be shrank or expanded. The shrank widget occupies just is much space
/// as it is set by its size hints. The expanded widget occupies all available free space.
///
/// The %Box supports alignment. When shrank widgets do not occupy the entire space
/// completely, they can be distributed accordingly.
/// @ingroup container_group
class Box: public Container {
public:

    /// The constructor.
    /// @param orient the box orientation.
    /// @param spacing the amount of space being allocated between widgets.
    Box(Orientation orient, unsigned spacing=0);

    /// Set spacing.
    /// @param spacing the amount of space being allocated between widgets.
    void set_spacing(unsigned spacing);

    /// Get spacing.
    unsigned spacing() const;

    /// Set align.
    void set_align(Align align);

    /// Get align.
    Align align() const;

    /// Append widget.
    /// @throw user_error if widget already inserted into another container.
    void append(Widget & w, bool shrink=false);

    /// Prepend widget.
    /// @throw user_error if widget already inserted into another container.
    void prepend(Widget & w, bool shrink=false);

    /// Place widget before other widget.
    /// @throw user_error if widget already inserted into another container.
    void insert_before(Widget & w, const Widget & other, bool shrink=false);

    /// Place widget after other widget.
    /// @throw user_error if widget already inserted into another container.
    void insert_after(Widget & w, const Widget & other, bool shrink=false);

    /// Remove widget.
    void remove(Widget & w);

    /// Remove single widget placed before specified widget.
    void remove_before(const Widget & w);

    /// Remove single widget placed after specified widget.
    void remove_after(const Widget & w);

    /// Remove all widgets.
    void clear();

    /// Test if the box empty.
    bool empty() const;

    /// Test if certain widget is shrunk.
    bool shrunk(const Widget & w) const;

    /// Shrink certain widget.
    void shrink(Widget & w);

    /// Expand certain widget.
    void expand(Widget & w);

    /// Shrink all widgets.
    void shrink_all();

    /// Expand all widgets.
    void expand_all();

    /// Change orientation.
    void set_orientation(Orientation orient);

    /// Get focus_next action.
    Action & focus_next_action();

    /// Get focus_previous action.
    Action & focus_previous_action();

    /// Signal orientation changed.
    signal<void()> & signal_orientation_changed();
};

} // namespace tau

#endif // TAU_BOX_HH
