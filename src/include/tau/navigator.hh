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
/// @ingroup widget_group
class Navigator: public Widget {
public:

    /// Constructor with path.
    Navigator(const ustring & path=ustring());

    /// Change directory.
    void chdir(const ustring & path);

    /// Reload current directory.
    void reload();

    /// Get current directory.
    ustring dir() const;

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

    /// Show hidden files.
    void show_hidden_files();

    /// Hide hidden files.
    void hide_hidden_files();

    /// Test hidden files visible.
    bool hidden_files_visible() const;

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

    /// Set show directories only.
    void set_show_dirs_only();

    /// Unset show directories only.
    void unset_show_dirs_only();

    /// Test if directories only visible.
    bool dirs_only_visible() const;

    /// Set filter.
    void set_filter(const ustring & patterns);

    /// Get current filter.
    ustring filter() const;

    /// Signal emitted when file selected.
    /// The bound string represents a name of the currently selected file or directory.
    signal<void(ustring)> & signal_file_select();

    /// Signal emitted when file unselected.
    /// The bound string represents a name of the unselected file or directory.
    signal<void(ustring)> & signal_file_unselect();

    /// Signal emitted when directory had changed.
    /// The bound string represents a full path to the current directory.
    signal<void(ustring)> & signal_dir_changed();

    /// Signal emitted when:
    /// 1. The user presses "ENTER" on file.
    /// 2. The user double clicks on file.
    /// The bound string represents a full path to the clicked or ENTERed file or directory.
    signal<void(ustring)> & signal_file_activate();
};

} // namespace tau

#endif //TAU_NAVIGATOR_HH
