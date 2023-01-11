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

/// @file cycle-text.hh The Cycle_text class.

#ifndef TAU_CYCLE_TEXT_HH
#define TAU_CYCLE_TEXT_HH

#include <tau/enums.hh>
#include <tau/widget.hh>
#include <tau/ustring.hh>

namespace tau {

/// Cycle specification for text.
/// @ingroup container_group
class Cycle_text: public Widget {
public:

    /// Constructor with border style.
    /// @param bs the border style
    Cycle_text(Border_style bs=BORDER_INSET);

    /// Constructor with alignment.
    /// @param align horizontal text align.
    Cycle_text(Align align);

    /// Constructor with border style and alignment.
    /// @param bs the border style
    /// @param align horizontal text align.
    Cycle_text(Border_style bs, Align align);

    /// Set border style.
    void set_border_style(Border_style bs);

    /// Get border style.
    Border_style border_style() const;

    /// Set horizontal text alignment.
    void set_text_align(Align align);

    /// Get horizontal text alignment.
    Align text_align() const;

    /// Allow edit.
    void allow_edit();

    /// Disallow edit.
    void disallow_edit();

    /// Determines whether edit allowed.
    bool edit_allowed() const;

    /// Add cycling text.
    /// @param text text to shown.
    /// @param tooltip an optional tooltip for that text.
    void add_text(const ustring & text, const ustring & tooltip=ustring());

    /// Select specified text.
    /// @note signal_selection_changed() does not emitted during this call.
    void select_text(const ustring & text);

    /// Get current selection.
    ustring selection() const;

    /// Remove text.
    void remove_text(const ustring & text);

    /// Remove all.
    void clear();

    /// Test if empty.
    bool empty() const;

    /// Append widget after cycling text.
    /// @throw user_error if widget already inserted into another container.
    void append(Widget & w, bool shrink=false);

    /// Append text after cycling text.
    /// @param text text to be appended.
    /// @param margin_left left margin.
    /// @param margin_right right margin.
    void append(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    /// Prepend widget before cycling text.
    /// @throw user_error if widget already inserted into another container.
    void prepend(Widget & w, bool shrink=false);

    /// Prepend text before cycling text.
    /// @param text text to be prepended.
    /// @param margin_left left margin.
    /// @param margin_right right margin.
    void prepend(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    /// Remove widget.
    void remove(Widget & w);

    /// Emits when selection changes.
    signal<void(ustring)> & signal_selection_changed();
};

} // namespace tau

#endif // TAU_CYCLE_TEXT_HH
