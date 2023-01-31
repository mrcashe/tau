// ----------------------------------------------------------------------------
// Copyright © 2014-2023 Konstantin Shmelkov <mrcashe@gmail.com>.
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

/// @file fileman.hh The Fileman class.

#ifndef TAU_FILEMAN_HH
#define TAU_FILEMAN_HH

#include <tau/enums.hh>
#include <tau/ustring.hh>
#include <tau/widget.hh>

namespace tau {

/// File navigation and selection widget.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup file_group
/// @ingroup widget_group
class Fileman: public Widget {
public:

    /// @name Constructors, operators
    /// @{

    /// Constructor with mode and path.
    Fileman(Fileman_mode fm_mode, const ustring & path=ustring());

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Fileman(const Fileman & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Fileman & operator=(const Fileman & other) = default;

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
    Fileman(Widget_ptr wp);

    /// Assign implementation.
    ///
    /// @since 0.4.0
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Fileman & operator=(Widget_ptr wp);

    /// @}
    /// Get current URI.
    ustring uri() const;

    /// Change current URI.
    void set_uri(const ustring & uri);

    /// Get opaque implementation pointer to buil-in Navigator object.
    Widget_ptr navigator_ptr();

    /// Gets selected filenames without path component.
    std::vector<ustring> selection() const;

    /// Gets text from entry.
    ustring entry() const;

    /// Add filter.
    void add_filter(const ustring & patterns, const ustring & title=ustring());

    /// Get current filter.
    ustring filter() const;

    /// Show info items.
    /// @param items item list, see @ref navigator_info for meaning.
    /// @param sep list separator
    /// @sa hide_info() info_visible() visible_info_items() invisible_info_items()
    void show_info(const ustring & items, char32_t sep=U':');

    /// Hide info items.
    /// @param items item list, see @ref navigator_info for meaning.
    /// @param sep list separator
    /// @sa show_info() info_visible() visible_info_items() invisible_info_items()
    void hide_info(const ustring & items, char32_t sep=U':');

    /// Test if info item visible.
    /// @param item the item name, see @ref navigator_info for meaning.
    /// @sa show_info() hide_info() visible_info_items() invisible_info_items()
    bool info_visible(const ustring & item) const;

    /// %List visible info items.
    /// @param sep list separator
    /// @return item list, see @ref navigator_info for meaning.
    /// @sa show_info() hide_info() info_visible() invisible_info_items()
    ustring visible_info_items(char32_t sep=U':') const;

    /// %List invisible info items.
    /// @param sep list separator
    /// @return item list, see @ref navigator_info for meaning.
    /// @sa show_info() hide_info() info_visible() visible_info_items()
    ustring invisible_info_items(char32_t sep=U':') const;

    /// Allow file overwrite without a prompt.
    /// Disallowed by default.
    void allow_overwrite();

    /// Disallow file overwrite without a prompt.
    /// Disallowed by default.
    void disallow_overwrite();

    /// Test if file overwrite without a prompt allowed.
    bool overwrite_allowed() const;

    /// Load state from the Key_file.
    void load_state(Key_file & kf, Key_section & sect);

    /// Save state to the Key_file.
    void save_state(Key_file & kf, Key_section & sect);

    /// Get "Apply" action.
    Action & apply_action();

    /// Get "Cancel" action.
    const Action & apply_action() const;

    /// Get "Cancel" action.
    Action & cancel_action();

    /// Get "Cancel" action.
    const Action & cancel_action() const;
};

} // namespace tau

#endif // TAU_FILEMAN_HH
