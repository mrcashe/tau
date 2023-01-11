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

/// @file theme.hh The Theme class.

#ifndef TAU_THEME_HH
#define TAU_THEME_HH

#include <tau/signal.hh>
#include <tau/types.hh>
#include <tau/ustring.hh>
#include <vector>

namespace tau {

/// %Theme engine.
/// @ingroup sys_group
class Theme {
public:

    /// Default constructor.
    Theme();

    /// Add icon directory.
    void add_icon_dir(const ustring & dir);

    /// Add pixmap directory.
    void add_pixmap_dir(const ustring & dir);

    /// Add cursor directory.
    void add_cursor_dir(const ustring & dir);

    /// Find mouse cursor by name.
    /// Accepts colon separated list of cursors.
    /// @param names colon separated cursor name list.
    /// @param size the cursor size in pixels.
    /// @note if size is zero, the default cursor size used.
    /// @return an empty cursor if not found.
    Cursor find_cursor(const ustring & names, int size);

    /// Find some pixmap by it's base name.
    /// Accepts colon separated list of pixmaps.
    /// @return an empty pixmap if not found.
    Pixmap find_pixmap(const ustring & names);

    /// Find icon.
    /// Accepts colon separated list of icons.
    /// @return an empty pixmap if not found.
    Pixmap find_icon(const ustring & names, int icon_size, const ustring & context=ustring());

    /// Get icon.
    /// Accepts colon separated list of icons.
    /// @note never return an empty pixmap.
    /// @return found icon or black pixmap with requested size in case requested icon not found.
    Pixmap get_icon(const ustring & names, int icon_size, const ustring & context=ustring());

    /// List installed icon theme names.
    std::vector<ustring> list_icon_themes() const;

    /// List installed cursor theme names.
    std::vector<ustring> list_cursor_themes() const;

    /// Change cursor theme.
    /// Accepts colon separated list of themes.
    void set_cursor_theme(const ustring & names);

    /// Change icon theme.
    /// Accepts colon separated list of themes.
    void set_icon_theme(const ustring & names);

    /// Get current cursor theme name.
    ustring cursor_theme() const;

    /// Get current icon theme name.
    ustring icon_theme() const;

    /// Find library provided Master_action.
    /// Some actions are used by library itself and can be used by end user too.
    /// This method tries to obtain a standard action by it's name.
    /// These actions provide key bindings, standard icon names, labels and tooltips.
    /// The standard action names, used and provided by the library, see @ref actions_page
    /// action list.
    ///
    /// @param name an action name.
    /// @return the pointer to the Master_action or <b>nullptr</b> if not found.
    ///
    /// @warning do not keep, @b delete or @e free() returned pointer, that pointer belongs
    /// to associative container and may be invalidated at any moment!
    Master_action * find_action(const ustring & name);

    /// Get signal for cursor theme changing event.
    signal<void()> & signal_cursors_changed();

    /// Get signal for icon theme changing event.
    signal<void()> & signal_icons_changed();

private:

    Theme_ptr impl;
};

} // namespace tau

#endif // TAU_THEME_HH
