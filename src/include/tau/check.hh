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

/// @file check.hh The Check class.

#ifndef TAU_CHECK_HH
#define TAU_CHECK_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// %Widget that can be checked or not.
/// @ingroup widget_group
class Check: public Widget {
public:

    /// Default constructor.
    /// @param checked the initial state.
    Check(bool checked=false);

    /// Constructor with check style.
    /// @param check_style the check style.
    /// @param checked the initial state.
    Check(Check_style check_style, bool checked=false);

    /// Constructor with border style.
    /// @param border_style the border style.
    /// @param checked the initial state.
    Check(Border_style border_style, bool checked=false);

    /// Constructor with check and border style.
    /// @param check_style the check style.
    /// @param border_style the border style.
    /// @param checked the initial state.
    Check(Check_style check_style, Border_style border_style, bool checked=false);

    /// Set check style.
    /// @param cs the check style.
    void set_check_style(Check_style cs);

    /// Get check style.
    Check_style check_style() const;

    /// Set border style.
    void set_border_style(Border_style bs);

    /// Get border style.
    Border_style border_style() const;

    /// Set border width.
    void set_border_width(unsigned npx);

    /// Get border width.
    unsigned border_width() const;

    /// Set checked state.
    void check();

    /// Reset checked state.
    void uncheck();

    /// Toggle check.
    void toggle();

    /// Get current check value.
    bool checked() const;

    /// Join same radio group which other check belongs.
    void join(Check & other);

    /// Test if joined radio group.
    bool joined() const;

    /// Signal emitted when check state changed to true.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_check();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_check();

    /// Signal emitted when check state changed to false.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_uncheck();
    /// ~~~~~~~~~~~~~~
    signal<void()> & signal_uncheck();
};

} // namespace tau

#endif // TAU_CHECK_HH
