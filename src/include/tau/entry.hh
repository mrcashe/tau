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

/// @file entry.hh The Entry (input line) class.

#ifndef TAU_ENTRY_HH
#define TAU_ENTRY_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// Single line text editor with decorations.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Entry: public Widget {
public:

    /// @name Constructors, operators
    /// @{

    /// Default constructor.
    Entry(Border_style border_style=BORDER_INSET);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Entry(const Entry & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Entry & operator=(const Entry & other) = default;

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
    Entry(Widget_ptr wp);

    /// Constructor with horizontal text alignment.
    Entry(Align text_align, Border_style border_style=BORDER_INSET);

    /// Constructor with text.
    Entry(const ustring & text, Border_style border_style=BORDER_INSET);

    /// Constructor with text and horizontal text alignment.
    Entry(const ustring & text, Align text_align, Border_style border_style=BORDER_INSET);

    /// @}
    /// Assign text.
    void assign(const ustring & s);

    /// Get text.
    ustring text() const;

    /// Get text size in pixels.
    Size text_size(const ustring & s);

    /// Get buffer.
    const Buffer & buffer() const;

    /// Allow edit.
    void allow_edit();

    /// Disallow edit.
    void disallow_edit();

    /// Determines whether edit allowed.
    bool edit_allowed() const;

    /// Test if empty.
    bool empty() const;

    /// Clear input.
    void clear();

    /// Set horizontal text alignment.
    void set_text_align(Align align);

    /// Get horizontal text alignment.
    Align text_align() const;

    /// Select all.
    void select_all();

    /// Select text.
    void select(Buffer_citer b, Buffer_citer e);

    /// Test if has selection.
    bool has_selection() const;

    /// Unselect all.
    void unselect();

    /// Set caret position.
    void move_to(const Buffer_citer & pos);

    /// Set caret position.
    void move_to(std::size_t row, std::size_t col);

    /// Get caret position.
    Buffer_citer caret() const;

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

    /// Gets "Cancel" action.
    Action & cancel_action();

    /// Signal emitted when entered text changed.
    ///
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_changed(const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<void(const ustring &)> & signal_changed();

    /// Signal emitted when user pressed "ENTER".
    ///
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_activate(const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<void(const ustring &)> & signal_activate();

    /// Signal emitted when validation enabled and new input pending.
    /// Return true from signal handler to decline changes.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// bool is_wrong_input(const ustring & text);
    /// ~~~~~~~~~~~~~~
    signal<bool(const ustring &)> & signal_validate();

};

} // namespace tau

#endif // TAU_ENTRY_HH
