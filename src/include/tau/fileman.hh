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

/// @file fileman.hh The Fileman class.

#ifndef TAU_FILESEL_HH
#define TAU_FILESEL_HH

#include <tau/enums.hh>
#include <tau/ustring.hh>
#include <tau/widget.hh>

namespace tau {

/// File navigation and selection widget.
/// @ingroup widget_group
class Fileman: public Widget {
public:

    /// Constructor with mode and path.
    Fileman(Fileman_mode fm_mode, const ustring & path=ustring());

    /// Get current directory.
    ustring dir() const;

    /// Change current directory.
    void chdir(const ustring & dirpath);

    /// Gets selected filenames without path component.
    std::vector<ustring> selection() const;

    /// Gets text from entry.
    ustring entry() const;

    /// Show places.
    /// The places are visible by default.
    void show_places();

    /// Hide places.
    /// The places are visible by default.
    void hide_places();

    /// Determine if places are visible.
    /// The places are visible by default.
    bool places_visible() const;

    /// Add filter.
    void add_filter(const ustring & patterns, const ustring & title=ustring());

    /// Get current filter.
    ustring filter() const;

    /// Set sort item name.
    void sort_by(const ustring & item);

    /// Get sort item name.
    ustring sorted_by() const;

    /// Sort files forward.
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
    /// Disallowed by default.
    bool dir_select_allowed() const;

    /// Set show directories only.
    void set_show_dirs_only();

    /// Unset show directories only.
    void unset_show_dirs_only();

    /// Test if directories only visible.
    bool dirs_only_visible() const;

    /// Allow directory creation.
    /// Allowed by default in file save mode.
    void allow_dir_creation();

    /// Disallow directory creation.
    /// Allowed by default in file save mode.
    void disallow_dir_creation();

    /// Test if directory creation allowed.
    /// Allowed by default in file save mode.
    bool dir_creation_allowed() const;

    /// Allow file overwrite without a prompt.
    /// Disallowed by default.
    void allow_silent_overwrite();

    /// Disallow file overwrite without a prompt.
    /// Disallowed by default.
    void disallow_silent_overwrite();

    /// Test if file overwrite without a prompt allowed.
    bool silent_overwrite_allowed() const;

    /// Get ratio.
    double ratio() const;

    /// Set ratio.
    void set_ratio(double ratio);

    /// Get "Cancel" action.
    Action & cancel_action();

    /// Signal emitted when:
    /// - User double clicks on single file.
    /// - User presses "Enter" on single file.
    /// - "Apply" button has been clicked.
    ///
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_apply();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_apply();
};

} // namespace tau

#endif // TAU_FILESEL_HH
