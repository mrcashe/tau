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
/// @ingroup container_group
class Cycle: public Widget {
public:

    /// Default constructor.
    Cycle(Border_style bs=BORDER_INSET);

    /// Set border style.
    void set_border_style(Border_style bs);

    /// Get border style.
    Border_style border_style() const;

    /// Add cycling widget.
    void add(Widget & w);

    /// Append widget after cycling widget.
    /// @throw user_error if widget already inserted into another container.
    void append(Widget & w, bool shrink=false);

    /// Append text after cycling widget.
    /// @param text text to be appended.
    /// @param margin_left left margin.
    /// @param margin_right right margin.
    void append(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    /// Prepend widget before cycling widget.
    /// @throw user_error if widget already inserted into another container.
    void prepend(Widget & w, bool shrink=false);

    /// Prepend text before cycling widget.
    /// @param text text to be prepended.
    /// @param margin_left left margin.
    /// @param margin_right right margin.
    void prepend(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    /// Remove any widget.
    void remove(Widget & w);

};

} // namespace tau

#endif // TAU_CYCLE_HH
