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

/// @file edit.hh The Edit class.

#ifndef TAU_EDIT_HH
#define TAU_EDIT_HH

#include <tau/text.hh>

namespace tau {

/// %Text editor.
/// @ingroup widget_group
class Edit: public Text {
public:

    /// Default constructor.
    Edit();

    /// Constructor with text alignment.
    Edit(Align halign, Align valign=ALIGN_START);

    /// Constructor with text and text alignment.
    Edit(const ustring & text, Align halign=ALIGN_START, Align valign=ALIGN_START);

    /// Constructor with buffer and text alignment.
    Edit(Buffer buf, Align halign=ALIGN_START, Align valign=ALIGN_START);

    /// Allow edit.
    void allow_edit();

    /// Disallow edit.
    void disallow_edit();

    /// Determines whether edit allowed.
    bool edit_allowed() const;

    /// Enter text at current position.
    void enter_text(const ustring & text);

    /// Determines if undo action possible.
    bool can_undo() const;

    /// Determines if redo action possible.
    bool can_redo() const;

    /// Get current undo index.
    std::size_t undo_index() const;

    /// Split next undo record.
    /// This method can be used to establish the file save position.
    void split_undo();

    /// @name Access to the established actions.
    /// @{

    /// Gets "Cut" action (Ctrl+X, Ctrl+Delete").
    Action & cut_action();

    /// Gets "Paste" action (Ctrl+V, Shift+Insert).
    Action & paste_action();

    /// Gets "Enter" action (Enter).
    Action & enter_action();

    /// Gets "Delete" action (Delete).
    Action & del_action();

    /// Gets "BackSpace" action (Backspace).
    Action & backspace_action();

    /// Gets "Undo" action (Alt+Backspace").
    Action & undo_action();

    /// Gets "Redo" action (Alt+Enter").
    Action & redo_action();

    /// Gets "Tab" action (Alt+Enter").
    Action & tab_action();

    /// Gets "Insert/Replace" action.
    Toggle_action & insert_action();

    /// @}
};

} // namespace tau

#endif // TAU_EDIT_HH
