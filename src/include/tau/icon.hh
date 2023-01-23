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

/// @file icon.hh The Icon class.

#ifndef TAU_ICON_HH
#define TAU_ICON_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// An icon (image) displaying widget.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup paint_group
/// @ingroup widget_group
class Icon: public Widget {
public:

    /// @name Constructors and operators
    /// @{

    /// Default constructor.
    Icon();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Icon(const Icon & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Icon & operator=(const Icon & other) = default;

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
    Icon(Widget_ptr wp);

    /// Constructor with icon_name and icon size.
    Icon(const ustring & icon_name, int icon_size);

    /// @param action the action to be used.
    /// @param icon_size the icon size.
    /// @param items action items to be used, see #Action_items enum.
    Icon(Action_base & action, int icon_size, Action_items items=ACTION_ALL);

    /// @}
    /// Set icon name and icon size.
    void assign(const ustring & icon_name, int icon_size);

    /// Set icon name.
    void set_icon_name(const ustring & icon_name);

    /// Get icon name.
    ustring icon_name() const;

    /// Set icon size.
    void set_icon_size(unsigned icon_size);

    /// Get icon size.
    int icon_size() const;
};

/// @name Standard Icon Names
/// Here is the string constants used for icon loading from icon themes.
/// You can pass those constants to either @link tau::Theme::find_icon() @endlink
/// or @link tau::Theme::get_icon() @endlink methods to obtain @link tau::Pixmap @endlink
/// object containing image bits.
///
/// Using those constants prevent typos during icon name input and generating errors at compile time.
/// @{

/// Configure.
/// The actual value is "configure".
/// @ingroup paint_group
extern const char * ICON_CONFIGURE;

/// The actual value is "dialog-cancel".
/// @ingroup paint_group
extern const char * ICON_DIALOG_CANCEL;

/// The actual value is "dialog-ok".
/// @ingroup paint_group
extern const char * ICON_DIALOG_OK;

/// The actual value is "drive-optical".
/// @ingroup paint_group
extern const char * ICON_DRIVE_OPTICAL;

/// The actual value is "drive-harddisk".
/// @ingroup paint_group
extern const char * ICON_DRIVE_HARDDISK;

/// The actual value is "drive-removable-media".
/// @ingroup paint_group
extern const char * ICON_DRIVE_REMOVABLE_MEDIA;

/// The actual value is "folder".
/// @ingroup paint_group
extern const char * ICON_FOLDER;

/// The actual value is "folder-new".
/// @ingroup paint_group
extern const char * ICON_FOLDER_NEW;

/// The actual value is "folder-remote".
/// @ingroup paint_group
extern const char * ICON_FOLDER_REMOTE;

/// The actual value is "go-home".
/// @ingroup paint_group
extern const char * ICON_GO_HOME;

/// The actual value is "go-next".
/// @ingroup paint_group
extern const char * ICON_GO_NEXT;

/// The actual value is "go-previous".
/// @ingroup paint_group
extern const char * ICON_GO_PREVIOUS;

/// The actual value is "go-up".
/// @ingroup paint_group
extern const char * ICON_GO_UP;

/// The actual value is "picto-dec".
/// @ingroup paint_group
extern const char * ICON_PICTO_DEC;

/// The actual value is "picto-inc".
/// @ingroup paint_group
extern const char * ICON_PICTO_INC;

/// The actual value is "picto-close".
/// @ingroup paint_group
extern const char * ICON_PICTO_CLOSE;

/// The actual value is "picto-down".
/// @ingroup paint_group
extern const char * ICON_PICTO_DOWN;

/// The actual value is "picto-left".
/// @ingroup paint_group
extern const char * ICON_PICTO_LEFT;

/// The actual value is "picto-right".
/// @ingroup paint_group
extern const char * ICON_PICTO_RIGHT;

/// The actual value is "picto-up".
/// @ingroup paint_group
extern const char * ICON_PICTO_UP;

/// The actual value is "unknown".
/// @ingroup paint_group
extern const char * ICON_UNKNOWN;

/// The actual value is "view-refresh".
/// @ingroup paint_group
extern const char * ICON_VIEW_REFRESH;

/// @}

///

} // namespace tau

#endif // TAU_ICON_HH
