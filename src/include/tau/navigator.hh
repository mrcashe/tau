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

#ifndef TAU_NAVIGATOR_HH
#define TAU_NAVIGATOR_HH

/// @file navigator.hh The %Navigator (filesystem navigation widget) class.

#include <tau/ustring.hh>
#include <tau/widget.hh>

namespace tau {

/// Filesystem navigation widget.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Navigator: public Widget {
public:

    /// @name Constructors, operators
    /// @{

    /// Constructor with path.
    Navigator(const ustring & uri=ustring());

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Navigator(const Navigator & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Navigator & operator=(const Navigator & other) = default;

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
    Navigator(Widget_ptr wp);

    /// @}
    /// Set current URI.
    void set_uri(const ustring & uri);

    /// Get current URI.
    ustring uri() const;

    /// Reload current directory.
    void refresh();

    /// Set sort column name.
    void sort_by(const ustring & col);

    /// Get sort column name.
    ustring sorted_by() const;

    /// Sort file list forward.
    void sort_forward();

    /// Sort file list backward.
    void sort_backward();

    /// Determine if file list sorted backward.
    bool sorted_backward() const;

    /// Show info items.
    /// @param items item list
    /// @param sep list separator
    /// @sa hide_info()
    /// @sa info_visible()
    /// @sa visible_info_items()
    /// @sa invisible_info_items()
    void show_info(const ustring & items, char32_t sep=U':');

    /// Hide info items.
    /// @param items item list
    /// @param sep list separator
    /// @sa show_info()
    /// @sa info_visible()
    /// @sa visible_info_items()
    /// @sa invisible_info_items()
    void hide_info(const ustring & items, char32_t sep=U':');

    /// Test if info item visible.
    /// @param item the item name
    /// @sa show_info()
    /// @sa hide_info()
    /// @sa visible_info_items()
    /// @sa invisible_info_items()
    bool info_visible(const ustring & item) const;

    /// List visible info items.
    /// @param sep list separator
    /// @return item list
    /// @sa show_info()
    /// @sa hide_info()
    /// @sa info_visible()
    /// @sa invisible_info_items()
    ustring visible_info_items(char32_t sep=U':') const;

    /// List invisible info items.
    /// @param sep list separator
    /// @return item list
    /// @sa show_info()
    /// @sa hide_info()
    /// @sa info_visible()
    /// @sa visible_info_items()
    ustring invisible_info_items(char32_t sep=U':') const;

    /// Allow multiple select.
    /// Disallowed by default.
    void allow_multiple_select();

    /// Disallow multiple select.
    /// Disallowed by default.
    void disallow_multiple_select();

    /// Test if multiple select allowed.
    /// Disallowed by default.
    bool multiple_select_allowed() const;

    /// Allow directory select.
    /// Disallowed by default.
    void allow_dir_select();

    /// Disallow directory select.
    /// Disallowed by default.
    void disallow_dir_select();

    /// Determine if directory select allowed.
    bool dir_select_allowed() const;

    /// Set filter.
    void set_filter(const ustring & patterns);

    /// Get current filter.
    ustring filter() const;

    /// Signal emitted when file selected.
    /// The bound string represents a name of the currently selected file or directory.
    signal<void(const ustring &)> & signal_file_select();

    /// Signal emitted when file unselected.
    /// The bound string represents a name of the unselected file or directory.
    signal<void(const ustring &)> & signal_file_unselect();

    /// Signal emitted when directory had changed.
    /// The bound string represents a full path to the current directory.
    signal<void(const ustring &)> & signal_dir_changed();

    /// Signal emitted when:
    /// 1. The user presses "ENTER" on file.
    /// 2. The user double clicks on file.
    /// The bound string represents a full path to the clicked or ENTERed file or directory.
    signal<void(const ustring &)> & signal_file_activate();
};

} // namespace tau

#endif //TAU_NAVIGATOR_HH
