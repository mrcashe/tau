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

/// @file colorsel.hh The Colorsel class.

#ifndef TAU_COLORSEL_HH
#define TAU_COLORSEL_HH

#include <tau/color.hh>
#include <tau/widget.hh>

namespace tau {

/// %Color selection widget.
/// @ingroup widget_group
class Colorsel: public Widget {
public:

    /// Constructor.
    Colorsel(const Color & initial_color=Color());

    /// Set color.
    void set_color(const Color & color);

    /// Get selected color.
    Color color() const;

    /// Signal emitted when "Apply" button pressed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_apply();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_apply();

    /// Signal emitted when "Cancel" button pressed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_cancel();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_cancel();

    /// Signal emitted when color changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_color_changed(const Color & color);
    /// ~~~~~~~~~~~~~~~
    signal<void(Color)> & signal_color_changed();
};

} // namespace tau

#endif // TAU_COLORSEL_HH
