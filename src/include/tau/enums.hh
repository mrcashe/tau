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

/// @file enums.hh Enums.

#ifndef TAU_ENUMS_HH
#define TAU_ENUMS_HH

namespace tau {

/// The alignment of widget or text.
/// Used by multiple widgets.
/// @ingroup enum_group
enum Align {
    /// Top or left alignment.
    ALIGN_START,

    /// Center alignment.
    ALIGN_CENTER,

    /// Bottom or right alignment.
    ALIGN_END,

    /// Fill alignment.
    ALIGN_FILL
};

/// The border around some widgets.
/// @ingroup enum_group
enum Border_style {
    /// No border.
    BORDER_NONE,

    /// Solid border.
    BORDER_SOLID,

    /// Double line border.
    BORDER_DOUBLE,

    /// Dotted line border.
    BORDER_DOTTED,

    /// Dashed line border.
    BORDER_DASHED,

    /// Pushed in border.
    BORDER_INSET,

    /// Poped out border.
    BORDER_OUTSET,

    /// Etched in border.
    BORDER_GROOVE,

    /// Etched out border.
    BORDER_RIDGE
};

/// The orientation.
/// Used by multiple widgets.
/// @ingroup enum_group
enum Orientation {
    /// Down-to-Up orientation.
    OR_UP,

    /// Up-to-Down orientation.
    OR_DOWN,

    /// Right-to-Left orientation.
    OR_LEFT,

    /// Left-to-Right orientation.
    OR_RIGHT,

    /// Synonymous OR_UP.
    OR_NORTH = OR_UP,

    /// Synonymous OR_DOWN.
    OR_SOUTH = OR_DOWN,

    /// Synonymous OR_LEFT.
    OR_WEST  = OR_LEFT,

    /// Synonymous OR_RIGHT.
    OR_EAST  = OR_RIGHT,

    /// Synonymous OR_UP.
    ORIENTATION_UP = OR_UP,

    /// Synonymous OR_DOWN.
    ORIENTATION_DOWN = OR_DOWN,

    /// Synonymous OR_LEFT.
    ORIENTATION_LEFT = OR_LEFT,

    /// Synonymous OR_RIGHT.
    ORIENTATION_RIGHT = OR_RIGHT,

    /// Synonymous OR_UP.
    ORIENTATION_NORTH = OR_UP,

    /// Synonymous OR_DOWN.
    ORIENTATION_SOUTH = OR_DOWN,

    /// Synonymous OR_LEFT.
    ORIENTATION_WEST = OR_LEFT,

    /// Synonymous OR_RIGHT.
    ORIENTATION_EAST = OR_RIGHT

};

/// The window gravity.
/// @ingroup enum_group
enum Gravity {
    /// Top left corner.
    GRAVITY_TOP_LEFT,

    /// Top right corner.
    GRAVITY_TOP_RIGHT,

    /// Bottom left corner.
    GRAVITY_BOTTOM_LEFT,

    /// Bottom right corner.
    GRAVITY_BOTTOM_RIGHT,

    /// Left gravity.
    GRAVITY_LEFT,

    /// Right gravity.
    GRAVITY_RIGHT,

    /// Top gravity.
    GRAVITY_TOP,

    /// Bottom gravity.
    GRAVITY_BOTTOM,

    /// Center.
    GRAVITY_CENTER,

    /// Synonymous GRAVITY_TOP_LEFT.
    GRAVITY_NONE = GRAVITY_TOP_LEFT,

    /// Synonymous GRAVITY_TOP_LEFT.
    GRAVITY_LEFT_TOP = GRAVITY_TOP_LEFT,

    /// Synonymous GRAVITY_TOP_RIGHT.
    GRAVITY_RIGHT_TOP = GRAVITY_TOP_RIGHT,

    /// Synonymous GRAVITY_BOTTOM_LEFT.
    GRAVITY_LEFT_BOTTOM = GRAVITY_BOTTOM_LEFT,

    /// Synonymous GRAVITY_BOTTOM_RIGHT.
    GRAVITY_RIGHT_BOTTOM = GRAVITY_BOTTOM_RIGHT,

    /// Synonymous GRAVITY_TOP_LEFT.
    GRAVITY_NORTH_WEST = GRAVITY_TOP_LEFT,

    /// Synonymous GRAVITY_TOP_RIGHT.
    GRAVITY_NORTH_EAST = GRAVITY_TOP_RIGHT,

    /// Synonymous GRAVITY_BOTTOM_LEFT.
    GRAVITY_SOUTH_WEST = GRAVITY_BOTTOM_LEFT,

    /// Synonymous GRAVITY_BOTTOM_RIGHT.
    GRAVITY_SOUTH_EAST = GRAVITY_BOTTOM_RIGHT,

    /// Synonymous GRAVITY_LEFT.
    GRAVITY_WEST = GRAVITY_LEFT,

    /// Synonymous GRAVITY_RIGHT.
    GRAVITY_EAST = GRAVITY_RIGHT,

    /// Synonymous GRAVITY_TOP.
    GRAVITY_NORTH = GRAVITY_TOP,

    /// Synonymous GRAVITY_BOTTOM.
    GRAVITY_SOUTH = GRAVITY_BOTTOM
};

/// Line cap styles.
/// @ingroup enum_group
enum Cap_style {
    SQUARE_CAP,
    FLAT_CAP,
    ROUND_CAP
};

/// Line join styles.
/// @ingroup enum_group
enum Join_style {
    BEVEL_JOIN,
    MITER_JOIN,
    ROUND_JOIN
};

/// Line styles.
/// @ingroup enum_group
enum Line_style {
    SOLID_LINE,
    DASH_LINE,
    DOT_LINE,
    DASH_DOT_LINE,
    DASH_DOT_DOT_LINE,
    CUSTOM_DASH_LINE
};

/// Operators used while drawing.
/// @ingroup enum_group
enum Oper {
    OPER_COPY,
    OPER_CLEAR,
    OPER_SOURCE,
    OPER_NOT,
    OPER_XOR,
    OPER_SET
};

/// %Icon sizes (in pixels) used when getting icons.
/// @ingroup enum_group
enum Icon_size {
    /// Default icon size.
    DEFAULT_ICON    = 0,

    /// Smallest icon.
    SMALLEST_ICON   = 1,

    /// Default icon size is  8 pixels.
    NANO_ICON       = 1,

    /// Default icon size is 12 pixels.
    TINY_ICON       = 2,

    /// Default icon size is 16 pixels.
    SMALL_ICON      = 3,

    /// Default icon size is 22 pixels.
    MEDIUM_ICON     = 4,

    /// Default icon size is 32 pixels.
    LARGE_ICON      = 5,

    /// Default icon size is 48 pixels.
    HUGE_ICON       = 6,

    /// Largest icon.
    LARGEST_ICON    = 6
};

/// Check style.
/// @ingroup enum_group
enum Check_style {
    /// V-style rectangular check.
    CHECK_VSTYLE,

    /// X-style rectangular check.
    CHECK_XSTYLE,

    /// Circular (Radio-like) check.
    CHECK_RSTYLE
};

/// File and directory flags.
/// @ingroup enum_group
enum File_flags {

    /// Nothing specified/happend.
    FILE_NOTHING        = 0x00000000,

    /// File or directory was accessed.
    FILE_ACCESSED       = 0x00000001,

    /// File or directory metadata changed.
    FILE_ATTRIB         = 0x00000002,

    /// File or directory was changed.
    FILE_CHANGED        = 0x00000004,

    /// File or directory was created.
    FILE_CREATED        = 0x00000008,

    /// File or directory was opened.
    FILE_OPENED         = 0x00000010,

    /// File or directory was closed.
    FILE_CLOSED         = 0x00000020,

    /// File or directory was deleted.
    FILE_DELETED        = 0x00000040,

    /// File or directory was moved into watched directory.
    FILE_MOVED_IN       = 0x00000080,

    /// File or directory was moved out of watched directory.
    FILE_MOVED_OUT      = 0x00000100,

    /// Watched file or directory was moved.
    FILE_SELF_MOVED     = 0x00000200,

    /// Watched file or directory was deleted.
    FILE_SELF_DELETED   = 0x00000400,

    /// All file events mask.
    FILE_EVENTS         = 0x000007ff,

    /// Partition was mounted.
    FILE_MOUNT          = 0x00008000,

    /// Partition was unmounted.
    FILE_UMOUNT         = 0x00004000,

    /// Mounted/unmounted partition is removable.
    FILE_REMOVABLE      = 0x00002000
};

/// Fileman modes.
/// @ingroup enum_group
enum Fileman_mode {

    /// File manager in browse mode.
    FILEMAN_BROWSE,

    /// File manager in open mode.
    FILEMAN_OPEN,

    /// File manager in save mode.
    FILEMAN_SAVE
};

/// Label position used by the Frame class.
/// @ingroup enum_group
enum Label_pos {

    /// Horizontal label at the top face of the frame.
    LABEL_TOP,

    /// Horizontal label at the bottom face of the frame.
    LABEL_BOTTOM,

    /// Vertical label at the left face of the frame.
    LABEL_LEFT,

    /// Vertical label at the right face of the frame.
    LABEL_RIGHT
};

/// Notebook tab positions.
/// @ingroup enum_group
enum Tab_pos {

    /// Tabs at the top side.
    TAB_TOP,

    /// Tabs at the bottom side.
    TAB_BOTTOM,

    /// Tabs at the left side.
    TAB_LEFT,

    /// Tabs at the right side.
    TAB_RIGHT
};

/// Separator style.
/// @ingroup enum_group
enum Separator_style {
    /// Etched in separator.
    SEPARATOR_GROOVE,

    /// Etched out separator.
    SEPARATOR_RIDGE,

    /// Separator looking like a solid line.
    SEPARATOR_SOLID,

    /// Separator looking like a handle.
    SEPARATOR_HANDLE
};

/// Text word wrap modes.
/// @ingroup enum_group
enum Wrap_mode {
    /// Word wrap set off.
    WRAP_NONE,

    /// Insert ellipsis at the start of line.
    WRAP_ELLIPSIZE_START,

    /// Insert ellipsis at the center of line.
    WRAP_ELLIPSIZE_CENTER,

    /// Insert ellipsis at the end of line.
    WRAP_ELLIPSIZE_END

};

/// Action items to be used.
/// @ingroup enum_group
enum Action_items {

    /// Do not use anything.
    ACTION_NOTHING  = 0,

    /// Use label provided by action.
    ACTION_LABEL    = 1 << 0,

    /// Use accelerators provided by action.
    ACTION_ACCEL    = 1 << 1,

    /// Use tooltip provided by action.
    ACTION_TOOLTIP  = 1 << 2,

    /// Use icon provided by action.
    ACTION_ICON     = 1 << 3,

    /// Use everything provided by action.
    ACTION_ALL = (ACTION_LABEL|ACTION_ACCEL|ACTION_TOOLTIP|ACTION_ICON),

    /// Don't use label.
    ACTION_NO_LABEL = (ACTION_ALL & ~ACTION_LABEL),

    /// Don't use accelerator(s).
    ACTION_NO_ACCEL = (ACTION_ALL & ~ACTION_ACCEL),

    /// Don't use tooltip.
    ACTION_NO_TOOLTIP = (ACTION_ALL & ~ACTION_TOOLTIP),

    /// Don't use icon.
    ACTION_NO_ICON = (ACTION_ALL & ~ACTION_ICON)
};

} // namespace tau

#endif // TAU_ENUMS_HH
