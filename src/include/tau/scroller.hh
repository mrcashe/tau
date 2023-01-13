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

#ifndef TAU_SCROLLER_HH
#define TAU_SCROLLER_HH

/// @file scroller.hh The Scroller class.

#include <tau/container.hh>

namespace tau {

/// Container for a widget that can be scrolled by the user, allowing it to be
/// larger than the physical display.
/// @ingroup container_group
class Scroller: public Container {
public:

    /// Default constructor.
    Scroller();

    /// Set owning widget.
    /// @throw user_error if widget already inserted into another container.
    void insert(Widget & w);

    /// Unset owning widget.
    void clear();

    /// Get logical size in pixels.
    Size logical_size() const;

    /// Scroll child to X position.
    void pan_to_x(int x);

    /// Scroll child to Y position.
    void pan_to_y(int y);

    /// Scroll child to desired position.
    void pan_to(const Point & pos);

    /// Scroll child to desired position.
    void pan_to(int x, int y);

    /// Get child scroll value.
    Point offset() const;

    /// Get pan_left Action.
    /// @sa set_step()
    /// @sa step()
    Action & pan_left_action();

    /// Get pan_right Action.
    /// @sa set_step()
    /// @sa step()
    Action & pan_right_action();

    /// Get pan_up Action.
    /// @sa set_step()
    /// @sa step()
    Action & pan_up_action();

    /// Get pan_down Action.
    /// @sa set_step()
    /// @sa step()
    Action & pan_down_action();

    /// Get page_up Action.
    Action & page_up_action();

    /// Get page_down Action.
    Action & page_down_action();

    /// Get home Action.
    Action & home_action();

    /// Get end Action.
    Action & end_action();

    /// Set step values used by pan_left_action(), pan_right_action(), pan_up_action() and pan_down_action().
    /// @sa pan_left_action()
    /// @sa pan_right_action()
    /// @sa pan_up_action()
    /// @sa pan_down_action()
    /// @sa step()
    void set_step(const Point & step);

    /// Set step values used by pan_left_action(), pan_right_action(), pan_up_action() and pan_down_action().
    /// @sa pan_left_action()
    /// @sa pan_right_action()
    /// @sa pan_up_action()
    /// @sa pan_down_action()
    /// @sa step()
    void set_step(int xstep, int ystep);

    /// Get step values used by pan_left_action(), pan_right_action(), pan_up_action() and pan_down_action().
    /// @sa pan_left_action()
    /// @sa pan_right_action()
    /// @sa pan_up_action()
    /// @sa pan_down_action()
    /// @sa step()
    Point step() const;

    /// Signal emitted when child offset changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_offset_changed();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_offset_changed();

    /// Signal emitted when logical size changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_logical_size_changed();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_logical_size_changed();
};

} // namespace tau

#endif // TAU_SCROLLER_HH