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

#ifndef TAU_ABSOLUTE_HH
#define TAU_ABSOLUTE_HH

/// @file absolute.hh The Absolute container class.

#include <tau/container.hh>

namespace tau {

/// A container widget that allows freely move and resize its children.
/// @ingroup container_group
class Absolute: public Container {
public:

    /// Default constructor.
    Absolute();

    /// Constructor with implementation pointer.
    Absolute(Widget_ptr wp);

    /// Put widget at the specified position and give them specified size.
    /// @param w widget to be put.
    /// @param pos position where widget will be placed.
    /// @param size size in pixel or automatic allocation if corresponding components are zero.
    /// @throw user_error if widget already inserted into another container.
    void put(Widget & w, const Point & pos, const Size & size=Size());

    /// Put widget at the specified position and give them specified size.
    /// @param w widget to be put.
    /// @param pos position where widget will be placed.
    /// @param width width in pixels or automatic allocation along X axis if zero.
    /// @param height height in pixels or automatic allocation along Y axis if zero.
    /// @throw user_error if widget already inserted into another container.
    void put(Widget & w, const Point & pos, unsigned width, unsigned height);

    /// Put widget at the specified position and give them specified size.
    /// @param w widget to be put.
    /// @param x X coordinate in pixels where widget will be placed.
    /// @param y Y coordinate in pixels where widget will be placed.
    /// @param size size in pixel or automatic allocation if corresponding components are zero.
    /// @throw user_error if widget already inserted into another container.
    void put(Widget & w, int x, int y, const Size & size=Size());

    /// Put widget at the specified position and give them specified size.
    /// @param w widget to be put.
    /// @param x X coordinate in pixels where widget will be placed.
    /// @param y Y coordinate in pixels where widget will be placed.
    /// @param width width in pixels or automatic allocation along X axis if zero.
    /// @param height height in pixels or automatic allocation along Y axis if zero.
    /// @throw user_error if widget already inserted into another container.
    void put(Widget & w, int x, int y, unsigned width, unsigned height);

    /// Move child.
    /// @param w widget to be moved.
    /// @param pos position where widget will be placed.
    void move(Widget & w, const Point & pos);

    /// Move child.
    /// @param w widget to be moved.
    /// @param x X coordinate of destination.
    /// @param y Y coordinate of destination.
    void move(Widget & w, int x, int y);

    /// Move child relatively.
    /// @param w widget to be moved.
    /// @param offset offset from current position to a new position.
    void move_rel(Widget & w, const Point & offset);

    /// Move child relatively.
    /// @param w widget to be moved.
    /// @param dx X component of offset to a new position.
    /// @param dy Y component of offset to a new position.
    void move_rel(Widget & w, int dx, int dy);

    /// Resize child.
    /// @param w widget to be resized.
    /// @param size size in pixel or automatic allocation if corresponding components are zero.
    void resize(Widget & w, const Size & size);

    /// Resize child.
    /// @param w widget to be resized.
    /// @param width width in pixels or automatic allocation along X axis if zero.
    /// @param height height in pixels or automatic allocation along Y axis if zero.
    void resize(Widget & w, unsigned width, unsigned height);

    /// Remove child.
    /// @param w widget to be removed.
    void remove(Widget & w);

    /// Remove all children.
    void clear();

    /// Test if empty.
    bool empty() const;
};

} // namespace tau

#endif // TAU_ABSOLUTE_HH
