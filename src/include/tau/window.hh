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

/// @file window.hh The abstract Window class.

#ifndef TAU_WINDOW_HH
#define TAU_WINDOW_HH

#include <tau/bin.hh>

namespace tau {

/// Base class for all windows.
/// @ingroup window_group
class Window: public Bin {
public:

    /// Get position relatively to it's parent.
    Point position() const;

    /// Move.
    /// This method only makes a request to window system.
    /// It's no guarantee, the window will be placed exactly at given position.
    /// Moreover, calling this before show() may not take an effect.
    void move(const Point & pt);

    /// Move.
    /// This method only makes a request to window system.
    /// It's no guarantee, the window will be placed exactly at given position.
    /// Moreover, calling this before show() may not take an effect.
    void move(int x, int y);

    /// Move and resize.
    /// This method only makes a request to window system.
    /// It's no guarantee, the window will be placed exactly at given position.
    /// Moreover, calling this before show() may not take an effect.
    void move(const Rect & r);

    /// Move.
    /// This method only makes a request to window system.
    /// It's no guarantee, the window will be placed exactly at given position.
    /// Moreover, calling this before show() may not take an effect.
    void move(int x, int y, unsigned width, unsigned height);

    /// Resize.
    /// @param sz desired widget size in pixels.
    void resize(const Size & sz);

    /// Resize.
    void resize(unsigned width, unsigned height);

    /// Flush any pending paint and resize requests.
    void update();

    /// Signal emitted when window moves across it's parent or screen.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// void on_position_changed();
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    signal<void()> & signal_position_changed();

    /// Signal emitted when window is going to close.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// void on_close();
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    signal<void()> & signal_close();

protected:

    /// @private
    Window(std::nullptr_t);

    /// @private
    Window(Widget_ptr wp);

    /// @private
    Window() = default;
};

} // namespace tau

#endif // TAU_WINDOW_HH
