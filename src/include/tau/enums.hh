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

/// @file enums.hh Enums.

#ifndef TAU_ENUMS_HH
#define TAU_ENUMS_HH

namespace tau {

/// The alignment of widget or text.
/// Used by multiple widgets.
/// @ingroup enum_group
enum Align {
    ALIGN_START,            ///< Top or left alignment.
    ALIGN_CENTER,           ///< Center alignment.
    ALIGN_END,              ///< Bottom or right alignment.
    ALIGN_FILL              ///< Fill alignment.
};

/// The border around some widgets.
/// @ingroup enum_group
enum Border_style {
    BORDER_NONE,            ///< No border.
    BORDER_SOLID,           ///< Solid border.
    BORDER_DOUBLE,          ///< Double line border.
    BORDER_DOTTED,          ///< Dotted line border.
    BORDER_DASHED,          ///< Dashed line border.
    BORDER_INSET,           ///< Pushed in border.
    BORDER_OUTSET,          ///< Poped out border.
    BORDER_GROOVE,          ///< Etched in border.
    BORDER_RIDGE            ///< Etched out border.
};

/// The orientation.
/// Used by multiple widgets.
/// @ingroup enum_group
enum Orientation {
    OR_UP,                          ///< Down-to-Up orientation.
    OR_DOWN,                        ///< Up-to-Down orientation.
    OR_LEFT,                        ///< Right-to-Left orientation.
    OR_RIGHT,                       ///< Left-to-Right orientation.
    OR_NORTH = OR_UP,               ///< Synonymous OR_UP.
    OR_SOUTH = OR_DOWN,             ///< Synonymous OR_DOWN.
    OR_WEST  = OR_LEFT,             ///< Synonymous OR_LEFT.
    OR_EAST  = OR_RIGHT,            ///< Synonymous OR_RIGHT.
    ORIENTATION_UP = OR_UP,         ///< Synonymous OR_UP.
    ORIENTATION_DOWN = OR_DOWN,     ///< Synonymous OR_DOWN.
    ORIENTATION_LEFT = OR_LEFT,     ///< Synonymous OR_LEFT.
    ORIENTATION_RIGHT = OR_RIGHT,   ///< Synonymous OR_RIGHT.
    ORIENTATION_NORTH = OR_UP,      ///< Synonymous OR_UP.
    ORIENTATION_SOUTH = OR_DOWN,    ///< Synonymous OR_DOWN.
    ORIENTATION_WEST = OR_LEFT,     ///< Synonymous OR_LEFT.
    ORIENTATION_EAST = OR_RIGHT,    ///< Synonymous OR_RIGHT.

};

/// The window gravity.
/// @ingroup enum_group
enum Gravity {
    GRAVITY_TOP_LEFT,                               ///< Top left corner.
    GRAVITY_TOP_RIGHT,                              ///< Top right corner.
    GRAVITY_BOTTOM_LEFT,                            ///< Bottom left corner.
    GRAVITY_BOTTOM_RIGHT,                           ///< Bottom right corner.
    GRAVITY_LEFT,                                   ///< Left gravity.
    GRAVITY_RIGHT,                                  ///< Right gravity.
    GRAVITY_TOP,                                    ///< Top gravity.
    GRAVITY_BOTTOM,                                 ///< Bottom gravity.
    GRAVITY_CENTER,                                 ///< Center.
    GRAVITY_NONE = GRAVITY_TOP_LEFT,                ///< Synonymous GRAVITY_TOP_LEFT.
    GRAVITY_LEFT_TOP = GRAVITY_TOP_LEFT,            ///< Synonymous GRAVITY_TOP_LEFT.
    GRAVITY_RIGHT_TOP = GRAVITY_TOP_RIGHT,          ///< Synonymous GRAVITY_TOP_RIGHT.
    GRAVITY_LEFT_BOTTOM = GRAVITY_BOTTOM_LEFT,      ///< Synonymous GRAVITY_BOTTOM_LEFT.
    GRAVITY_RIGHT_BOTTOM = GRAVITY_BOTTOM_RIGHT,    ///< Synonymous GRAVITY_BOTTOM_RIGHT.
    GRAVITY_NORTH_WEST = GRAVITY_TOP_LEFT,          ///< Synonymous GRAVITY_TOP_LEFT.
    GRAVITY_NORTH_EAST = GRAVITY_TOP_RIGHT,         ///< Synonymous GRAVITY_TOP_RIGHT.
    GRAVITY_SOUTH_WEST = GRAVITY_BOTTOM_LEFT,       ///< Synonymous GRAVITY_BOTTOM_LEFT.
    GRAVITY_SOUTH_EAST = GRAVITY_BOTTOM_RIGHT,      ///< Synonymous GRAVITY_BOTTOM_RIGHT.
    GRAVITY_WEST = GRAVITY_LEFT,                    ///< Synonymous GRAVITY_LEFT.
    GRAVITY_EAST = GRAVITY_RIGHT,                   ///< Synonymous GRAVITY_RIGHT.
    GRAVITY_NORTH = GRAVITY_TOP,                    ///< Synonymous GRAVITY_TOP.
    GRAVITY_SOUTH = GRAVITY_BOTTOM                  ///< Synonymous GRAVITY_BOTTOM.
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

/// %Icon sizes used when getting icons.
/// @ingroup enum_group
enum Icon_size {
    SMALLEST_ICON   = 0,    ///< Smallest icon.
    NANO_ICON       = 0,    ///< Default icon size is  8 pixels.
    TINY_ICON       = 1,    ///< Default icon size is 12 pixels.
    SMALL_ICON      = 2,    ///< Default icon size is 16 pixels.
    MEDIUM_ICON     = 3,    ///< Default icon size is 22 pixels.
    NORMAL_ICON     = 4,    ///< Default icon size is 32 pixels.
    LARGE_ICON      = 5,    ///< Default icon size is 48 pixels.
    LARGEST_ICON    = 5     ///< Largest icon.
};

/// Check style.
/// @ingroup enum_group
enum Check_style {
    CHECK_VSTYLE,           ///< V-style rectangular check.
    CHECK_XSTYLE,           ///< X-style rectangular check.
    CHECK_RSTYLE            ///< Circular (Radio-like) check.
};

/// File and directory flags.
/// @ingroup enum_group
enum File_flags {
    FILE_NOTHING        = 0x00000000,   ///< Nothing specified/happend.
    FILE_ACCESSED       = 0x00000001,   ///< File or directory was accessed.
    FILE_ATTRIB         = 0x00000002,   ///< File or directory metadata changed.
    FILE_CHANGED        = 0x00000004,   ///< File or directory was changed.
    FILE_CREATED        = 0x00000008,   ///< File or directory was created.
    FILE_OPENED         = 0x00000010,   ///< File or directory was opened.
    FILE_CLOSED         = 0x00000020,   ///< File or directory was closed.
    FILE_DELETED        = 0x00000040,   ///< File or directory was deleted.
    FILE_MOVED_IN       = 0x00000080,   ///< File or directory was moved into watched directory.
    FILE_MOVED_OUT      = 0x00000100,   ///< File or directory was moved out of watched directory.
    FILE_SELF_MOVED     = 0x00000200,   ///< Watched file or directory was moved.
    FILE_SELF_DELETED   = 0x00000400,   ///< Watched file or directory was deleted.
    FILE_EVENTS         = 0x000007ff,   ///< All file events mask.
    FILE_MOUNT          = 0x00008000,   ///< Partition was mounted.
    FILE_UMOUNT         = 0x00004000,   ///< Partition was unmounted.
    FILE_REMOVABLE      = 0x00002000    ///< Mounted/unmounted partition is removable.
};

/// File manager modes.
/// @ingroup enum_group
enum Fileman_mode {
    FILEMAN_BROWSE,         ///< File manager in browse mode.
    FILEMAN_OPEN,           ///< File manager in open mode.
    FILEMAN_SAVE            ///< File manager in save mode.
};

/// Label position used by the Frame class.
/// @ingroup enum_group
enum Label_pos {
    LABEL_TOP,              ///< Horizontal label at the top face of the frame.
    LABEL_BOTTOM,           ///< Horizontal label at the bottom face of the frame.
    LABEL_LEFT,             ///< Vertical label at the left face of the frame.
    LABEL_RIGHT             ///< Vertical label at the right face of the frame.
};

/// Tab positions.
/// @ingroup enum_group
enum Tab_pos {
    TAB_TOP,                ///< Tabs at the top side.
    TAB_BOTTOM,             ///< Tabs at the bottom side.
    TAB_LEFT,               ///< Tabs at the left side.
    TAB_RIGHT               ///< Tabs at the right side.
};

/// Separator style.
/// @ingroup enum_group
enum Separator_style {
    SEPARATOR_GROOVE,       ///< Etched in separator.
    SEPARATOR_RIDGE,        ///< Etched out separator.
    SEPARATOR_SOLID,        ///< Separator looking like a solid line.
    SEPARATOR_HANDLE        ///< Separator looking like a handle.
};

/// Word wrap modes.
/// @ingroup enum_group
enum Wrap_mode {
    WRAP_NONE,              ///< Word wrap set off.
    WRAP_ELLIPSIZE_START,   ///< Insert ellipsis at the start of line.
    WRAP_ELLIPSIZE_CENTER,  ///< Insert ellipsis at the center of line.
    WRAP_ELLIPSIZE_END      ///< Insert ellipsis at the end of line.
                            //   TO BE ADDED...
};

/// Action items to be used.
enum Action_items {
    ACTION_NOTHING  = 0,        ///< Do not use anything.
    ACTION_LABEL    = 1 << 0,   ///< Use label provided by action.
    ACTION_ACCEL    = 1 << 1,   ///< Use accelerators provided by action.
    ACTION_TOOLTIP  = 1 << 2,   ///< Use tootip provided by action.
    ACTION_ICON     = 1 << 3,   ///< Use icon provided by action.
    ACTION_ALL = (ACTION_LABEL|ACTION_ACCEL|ACTION_TOOLTIP|ACTION_ICON),    ///< Use everything provided by action.
    ACTION_NO_LABEL = (ACTION_ALL & ~ACTION_LABEL),     ///< Don't use label.
    ACTION_NO_ACCEL = (ACTION_ALL & ~ACTION_ACCEL),     ///< Don't use accelerator.
    ACTION_NO_TOOLTIP = (ACTION_ALL & ~ACTION_TOOLTIP), ///< Don't use tooltip.
    ACTION_NO_ICON = (ACTION_ALL & ~ACTION_ICON)        ///< Don't use icon.
};

} // namespace tau

#endif // TAU_ENUMS_HH
