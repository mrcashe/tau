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

/// @file input.hh Keyboard and mouse enums and functions.

#ifndef TAU_INPUT_HH
#define TAU_INPUT_HH

#include <tau/ustring.hh>

namespace tau {

/// Key modifiers.
/// @ingroup input_group
enum Key_modifiers {
    KM_NONE                     = 0x00000000,   ///< None pressed
    KM_SHIFT                    = 0x00000001,   ///< Shift pressed
    KM_CONTROL                  = 0x00000002,   ///< Control pressed
    KM_ALT                      = 0x00000004,   ///< Alt pressed
    KM_META                     = 0x00000008,   ///< Meta pressed
    KM_WIN                      = 0x00000010,   ///< Windows pressed
    KM_SUPER                    = 0x00000020,   ///< Super pressed
    KM_MENU                     = 0x00000040,   ///< %Menu pressed
    KM_HYPER                    = 0x00000080,   ///< Hyper pressed
    KM_GROUP                    = 0x00000100,   ///< Group pressed
    KM_NUMLOCK                  = 0x00000200,   ///< NumLock pressed
    KM_SCROLL                   = 0x00000400,   ///< ScrollLock pressed
    KM_CAPS                     = 0x00000800    ///< Caps pressed
};

/// Key codes, 32 bit.
/// @ingroup input_group
enum Key_codes {
    KC_NONE                     = 0x00000000,   ///< None pressed or unknown
    KC_BACKSPACE                = 0x00000008,   ///< BackSpace
    KC_TAB                      = 0x00000009,   ///< Tab
    KC_LINEFEED                 = 0x0000000A,   ///< LineFeed
    KC_ENTER                    = 0x0000000D,   ///< Enter
    KC_RETURN                   = 0x0000000D,   ///< Synonymous Enter
    KC_ESCAPE                   = 0x0000001B,   ///< Escape
    KC_SPACE                    = 0x00000020,   ///< Space
    KC_DELETE                   = 0x0000007F,   ///< Delete

    // Functional keys.
    KC_F1                       = 0x10000001,   ///< Functional, F1
    KC_F2                       = 0x10000002,   ///< Functional, F2
    KC_F3                       = 0x10000003,   ///< Functional, F3
    KC_F4                       = 0x10000004,   ///< Functional, F4
    KC_F5                       = 0x10000005,   ///< Functional, F5
    KC_F6                       = 0x10000006,   ///< Functional, F6
    KC_F7                       = 0x10000007,   ///< Functional, F7
    KC_F8                       = 0x10000008,   ///< Functional, F8
    KC_F9                       = 0x10000009,   ///< Functional, F9
    KC_F10                      = 0x1000000A,   ///< Functional, F10
    KC_F11                      = 0x1000000B,   ///< Functional, F11
    KC_F12                      = 0x1000000C,   ///< Functional, F12
    KC_F13                      = 0x1000000D,   ///< Functional, F13
    KC_F14                      = 0x1000000E,   ///< Functional, F14
    KC_F15                      = 0x1000000F,   ///< Functional, F15
    KC_F16                      = 0x10000010,   ///< Functional, F16
    KC_F17                      = 0x10000011,   ///< Functional, F17
    KC_F18                      = 0x10000012,   ///< Functional, F18
    KC_F19                      = 0x10000013,   ///< Functional, F19
    KC_F20                      = 0x10000014,   ///< Functional, F20
    KC_F21                      = 0x10000015,   ///< Functional, F21
    KC_F22                      = 0x10000016,   ///< Functional, F22
    KC_F23                      = 0x10000017,   ///< Functional, F23
    KC_F24                      = 0x10000018,   ///< Functional, F24
    KC_F25                      = 0x10000019,   ///< Functional, F25
    KC_F26                      = 0x1000001A,   ///< Functional, F26
    KC_F27                      = 0x1000001B,   ///< Functional, F27
    KC_F28                      = 0x1000001C,   ///< Functional, F28
    KC_F29                      = 0x1000001D,   ///< Functional, F29
    KC_F30                      = 0x1000001E,   ///< Functional, F30
    KC_F31                      = 0x1000001F,   ///< Functional, F31
    KC_F32                      = 0x10000020,   ///< Functional, F32
    KC_F33                      = 0x10000021,   ///< Functional, F33
    KC_F34                      = 0x10000022,   ///< Functional, F34
    KC_F35                      = 0x10000023,   ///< Functional, F35

    KC_LEFT                     = 0x20000000,   ///< Left
    KC_RIGHT                    = 0x20000001,   ///< Right
    KC_UP                       = 0x20000008,   ///< Up
    KC_DOWN                     = 0x20000009,   ///< Down
    KC_HOME                     = 0x20000010,   ///< Home
    KC_END                      = 0x20000011,   ///< End
    KC_PAGE_UP                  = 0x20000020,   ///< Page Up
    KC_PAGE_DOWN                = 0x20000021,   ///< Page Down
    KC_SCROLL_UP                = 0x20000028,   ///< Scroll Up
    KC_SCROLL_DOWN              = 0x20000029,   ///< Scroll Down
    KC_INSERT                   = 0x20000030,   ///< Insert
    KC_BEGIN                    = 0x20000040,   ///< Begin
    KC_CANCEL                   = 0x10000050,   ///< Cancel
    KC_BREAK                    = 0x10000058,   ///< Break
    KC_CLEAR                    = 0x10000060,   ///< Clear
    KC_PAUSE                    = 0x20000070,   ///< Pause
    KC_PRINT                    = 0x20000080,   ///< Print
    KC_SYSREQ                   = 0x20000090,   ///< Sys Request
    KC_HELP                     = 0x200000A0,   ///< Help
    KC_UNDO                     = 0x200000B0,   ///< Undo
    KC_REDO                     = 0x200000B1,   ///< Redo
    KC_FIND                     = 0x200000C0,   ///< Find
    KC_SELECT                   = 0x200000D0,   ///< Select
    KC_NO                       = 0x200000E0,   ///< No
    KC_YES                      = 0x200000E1,   ///< Yes
    KC_COPY                     = 0x200000F0,   ///< Copy
    KC_CUT                      = 0x200000F2,   ///< Cut
    KC_PASTE                    = 0x200000F4,   ///< Paste

    KC_LDIR                     = 0x30000010,   ///< Left Direction
    KC_RDIR                     = 0x30000020,   ///< Right Direction
    KC_MULTI_KEY                = 0x30000030,   ///< Multi
    KC_CODE_INPUT               = 0x30000040,   ///< Code Input
    KC_SINGLE_CANDIDATE         = 0x30000050,   ///< Single Candidate
    KC_MULTIPLE_CANDIDATE       = 0x30000060,   ///< Multiple Candidate
    KC_PREVIOUS_CANDIDATE       = 0x30000070,   ///< Previous Candidate
    KC_MODE_SWITCH              = 0x30000080,   ///< Mode Switch
    KC_SEPARATOR                = 0x30000090,   ///< Often comma.
    KC_DECIMAL                  = 0x300000A0,   ///< Decimal
    KC_ISO_LOCK                 = 0x300000B0,   ///< ISO Lock
    KC_LEVEL2_LATCH             = 0x300000C0,   ///< Level2 Latch
    KC_LEVEL3_SHIFT             = 0x300000D0,   ///< Level3 Shift
    KC_LEVEL3_LATCH             = 0x300000E0,   ///< Level3 Latch
    KC_LEVEL3_LOCK              = 0x300000F0,   ///< Level3 Lock
    KC_LEVEL5_SHIFT             = 0x30000100,   ///< Level5 Shift
    KC_LEVEL5_LATCH             = 0x30000110,   ///< Level5 Latch
    KC_LEVEL5_LOCK              = 0x30000120,   ///< Level5 Lock
    KC_GROUP_SHIFT              = 0x30000130,   ///< Group Shift
    KC_GROUP_LATCH              = 0x30000140,   ///< Group Latch
    KC_GROUP_LOCK               = 0x30000150,   ///< Group Lock
    KC_NEXT_GROUP               = 0x30000160,   ///< Next Group
    KC_NEXT_GROUP_LOCK          = 0x30000170,   ///< Next Group Lock
    KC_PREV_GROUP               = 0x30000180,   ///< Prev Group
    KC_PREV_GROUP_LOCK          = 0x30000190,   ///< Prev Group Lock
    KC_FIRST_GROUP              = 0x300001A0,   ///< First Group
    KC_FIRST_GROUP_LOCK         = 0x300001B0,   ///< First Group Lock
    KC_LAST_GROUP               = 0x300001C0,   ///< Last Group
    KC_LAST_GROUP_LOCK          = 0x300001D0,   ///< Last Group Lock
    KC_LEFT_TAB                 = 0x300001E0,   ///< Left Tab
    KC_MOVE_LINE_UP             = 0x300001F0,   ///< Move Line Up
    KC_MOVE_LINE_DOWN           = 0x300001F1,   ///< Move Line Down
    KC_PARTIAL_SPACE_LEFT       = 0x30000200,   ///< Partial Space Left
    KC_PARTIAL_SPACE_RIGHT      = 0x30000201,   ///< Partial Space Right
    KC_PARTIAL_LINE_UP          = 0x30000210,   ///< Partial Line Up
    KC_PARTIAL_LINE_DOWN        = 0x30000211,   ///< Partial Line Down
    KC_SET_MARGIN_LEFT          = 0x30000220,   ///< Set Margin Left
    KC_SET_MARGIN_RIGHT         = 0x30000221,   ///< Set Margin Right
    KC_RELEASE_MARGIN_LEFT      = 0x30000230,   ///< Release Margin Left
    KC_RELEASE_MARGIN_RIGHT     = 0x30000231,   ///< Release Margin Right
    KC_RELEASE_BOTH_MARGINS     = 0x30000238,   ///< Release Both Margins
    KC_FAST_CURSOR_LEFT         = 0x30000240,   ///< Fast %Cursor Left
    KC_FAST_CURSOR_RIGHT        = 0x30000241,   ///< Fast %Cursor Right
    KC_FAST_CURSOR_UP           = 0x30000248,   ///< Fast %Cursor Up
    KC_FAST_CURSOR_DOWN         = 0x30000249,   ///< Fast %Cursor Down
    KC_CONTINUOUS_UNDERLINE     = 0x30000250,   ///< Continuous Underline
    KC_DISCONTINUOUS_UNDERLINE  = 0x30000251,   ///< Discontinuous Underline
    KC_EMPHASIZE                = 0x30000260,   ///< Emphasize
    KC_CENTER_OBJECT            = 0x30000270,   ///< Center Object
    KC_SEND                     = 0x30000280,   ///< Send mail, file, object
    KC_REPLY                    = 0x30000281,   ///< Reply e.g., mail
    KC_ZOOM_IN                  = 0x30000290,   ///< Zoom in view, map, etc.
    KC_ZOOM_OUT                 = 0x30000291,   ///< Zoom out view, map, etc.
    KC_FORWARD                  = 0x30000320,   ///< Forward
    KC_BACK                     = 0x30000321,   ///< Back
    KC_STOP                     = 0x30000340,   ///< Stop
    KC_REFRESH                  = 0x30000350,   ///< Refresh
    KC_HOME_PAGE                = 0x30000360,   ///< Home Page
    KC_FAVORITES                = 0x30000370,   ///< Favorites
    KC_SEARCH                   = 0x30000380,   ///< Search
    KC_STANDBY                  = 0x30000390,   ///< Standby
    KC_OPEN_URL                 = 0x300003A0,   ///< Open URL
    KC_LAUNCH_MAIL              = 0x300003B0,   ///< Launch Mail
    KC_LAUNCH_MEDIA             = 0x300003C0,   ///< Launch Media Player
    KC_LAUNCH0                  = 0x300003D0,   ///< Launch 0
    KC_LAUNCH1                  = 0x300003D1,   ///< Launch 1
    KC_LAUNCH2                  = 0x300003D2,   ///< Launch 2
    KC_LAUNCH3                  = 0x300003D3,   ///< Launch 3
    KC_LAUNCH4                  = 0x300003D4,   ///< Launch 4
    KC_LAUNCH5                  = 0x300003D5,   ///< Launch 5
    KC_LAUNCH6                  = 0x300003D6,   ///< Launch 6
    KC_LAUNCH7                  = 0x300003D7,   ///< Launch 7
    KC_LAUNCH8                  = 0x300003D8,   ///< Launch 8
    KC_LAUNCH9                  = 0x300003D9,   ///< Launch 9
    KC_LAUNCHA                  = 0x300003DA,   ///< Launch A
    KC_LAUNCHB                  = 0x300003DB,   ///< Launch B
    KC_LAUNCHC                  = 0x300003DC,   ///< Launch C
    KC_LAUNCHD                  = 0x300003DD,   ///< Launch D
    KC_LAUNCHE                  = 0x300003DE,   ///< Launch E
    KC_LAUNCHF                  = 0x300003DF,   ///< Launch F
    KC_EXECUTE                  = 0x300003F0,   ///< Execute
    KC_TERMINAL                 = 0x300003F8,   ///< Terminal
    KC_POWER_OFF                = 0x30000400,   ///< Power Off
    KC_SLEEP                    = 0x30000402,   ///< Sleep
    KC_WAKE_UP                  = 0x30000408,   ///< Wake Up
    KC_TERMINATE_SERVER         = 0x30000410,   ///< Terminate Server
    KC_CLOSE                    = 0x30000420,   ///< Close
    KC_PHONE                    = 0x30000430,   ///< Launch phone; dial number
    KC_RELOAD                   = 0x30000440,   ///< Reload web page, file, etc.
    KC_SAVE                     = 0x30000450,   ///< Save (file, document, state)
    KC_VIDEO                    = 0x30000460,   ///< Launch video player
    KC_MUSIC                    = 0x30000462,   ///< Launch music applicaton
    KC_BATTERY                  = 0x30000470,   ///< %Display battery information
    KC_BLUETOOTH                = 0x30000472,   ///< Enable/disable Bluetooth
    KC_WLAN                     = 0x30000474,   ///< Enable/disable WLAN

    // Multimedia
    KC_VOLUME_UP                = 0x37000010,   ///< Volume Up
    KC_VOLUME_DOWN              = 0x37000030,   ///< Volume Down
    KC_VOLUME_MUTE              = 0x37000050,   ///< Volume Mute
    KC_BASS_BOOST               = 0x37000070,   ///< Bass Boost
    KC_BASS_UP                  = 0x37000090,   ///< Bass Up
    KC_BASS_DOWN                = 0x370000B0,   ///< Bass Down
    KC_TREBLE_UP                = 0x370000D0,   ///< Treble Up
    KC_TREBLE_DOWN              = 0x370000F0,   ///< Treble Down
    KC_MEDIA_PLAY               = 0x37000110,   ///< Media Play
    KC_MEDIA_PAUSE              = 0x37000130,   ///< Media Pause
    KC_MEDIA_STOP               = 0x37000150,   ///< Media Stop
    KC_MEDIA_PREVIOUS           = 0x37000170,   ///< Media Previous
    KC_MEDIA_NEXT               = 0x37000190,   ///< Media Next
    KC_MEDIA_RECORD             = 0x370001B0,   ///< Media Record

    // Japanese keyboard support.
    KC_KANJI                    = 0x41000010,   ///< Kanji, Kanji convert
    KC_MUHENKAN                 = 0x41000020,   ///< Cancel Conversion
    KC_HENKAN                   = 0x41000030,   ///< Alias for Henkan_Mode
    KC_ROMAJI                   = 0x41000040,   ///< to Romaji
    KC_HIRAGANA                 = 0x41000050,   ///< to Hiragana
    KC_KATAKANA                 = 0x41000060,   ///< to Katakana
    KC_HIRAGANA_KATAKANA        = 0x41000070,   ///< Hiragana/Katakana toggle
    KC_ZENKAKU                  = 0x41000080,   ///< to Zenkaku
    KC_HANKAKU                  = 0x41000090,   ///< to Hankaku
    KC_ZENKAKU_HANKAKU          = 0x410000A0,   ///< Zenkaku/Hankaku toggle
    KC_TOUROKU                  = 0x410000B0,   ///< Add to Dictionary
    KC_MASSYO                   = 0x410000C0,   ///< Delete from Dictionary
    KC_KANA_LOCK                = 0x410000D0,   ///< Kana Lock
    KC_KANA_SHIFT               = 0x410000E0,   ///< Kana Shift
    KC_EISU_SHIFT               = 0x410000F0,   ///< Alphanumeric Shift
    KC_EISU_TOGGLE              = 0x41000100,   ///< Alphanumeric toggle

    // Korean keyboard support.
    KC_HANGUL                   = 0x42000010,   ///< Hangul start/stop(toggle)
    KC_HANGUL_START             = 0x42000020,   ///< Hangul start
    KC_HANGUL_END               = 0x42000030,   ///< Hangul end, English start
    KC_HANGUL_HANJA             = 0x42000040,   ///< Start Hangul->Hanja Conversion
    KC_HANGUL_JAMO              = 0x42000050,   ///< Hangul Jamo mode
    KC_HANGUL_ROMAJA            = 0x42000060,   ///< Hangul Romaja mode
    KC_HANGUL_JEONJA            = 0x42000080,   ///< Jeonja mode
    KC_HANGUL_BANJA             = 0x42000090,   ///< Banja mode
    KC_HANGUL_PREHANJA          = 0x420000A0,   ///< Pre Hanja conversion
    KC_HANGUL_POSTHANJA         = 0x420000B0,   ///< Post Hanja conversion
    KC_HANGUL_SPECIAL           = 0x420000F0,   ///< Special symbols

    // Braille
    KC_BRAILLE_DOT_1            = 0x50000010,   ///< Braille Dot 1
    KC_BRAILLE_DOT_2            = 0x50000020,   ///< Braille Dot 2
    KC_BRAILLE_DOT_3            = 0x50000030,   ///< Braille Dot 3
    KC_BRAILLE_DOT_4            = 0x50000040,   ///< Braille Dot 4
    KC_BRAILLE_DOT_5            = 0x50000050,   ///< Braille Dot 5
    KC_BRAILLE_DOT_6            = 0x50000060,   ///< Braille Dot 6
    KC_BRAILLE_DOT_7            = 0x50000070,   ///< Braille Dot 7
    KC_BRAILLE_DOT_8            = 0x50000080,   ///< Braille Dot 8
    KC_BRAILLE_DOT_9            = 0x50000090,   ///< Braille Dot 9
    KC_BRAILLE_DOT_10           = 0x500000a0,   ///< Braille Dot 10

    // Modifiers
    KC_LSHIFT                   = 0x80000110,   ///< Left Shift
    KC_RSHIFT                   = 0x80000120,   ///< Right Shift
    KC_SHIFT                    = 0x80000130,   ///< Shift
    KC_LCONTROL                 = 0x80000140,   ///< Left Control
    KC_RCONTROL                 = 0x80000150,   ///< Right Control
    KC_CONTROL                  = 0x80000160,   ///< Control
    KC_LALT                     = 0x80000170,   ///< Left Alt
    KC_RALT                     = 0x80000180,   ///< Right Alt
    KC_ALT                      = 0x80000190,   ///< Alt
    KC_LMETA                    = 0x800001A0,   ///< Left Meta
    KC_RMETA                    = 0x800001B0,   ///< Right Meta
    KC_META                     = 0x800001C0,   ///< Meta
    KC_LWIN                     = 0x800001D0,   ///< Left Windows
    KC_RWIN                     = 0x800001E0,   ///< Right Windows
    KC_WIN                      = 0x800001F0,   ///< Windows
    KC_LSUPER                   = 0x80000200,   ///< Left Super
    KC_RSUPER                   = 0x80000210,   ///< Right Super
    KC_SUPER                    = 0x80000220,   ///< Super
    KC_LMENU                    = 0x80000230,   ///< Left %Menu
    KC_RMENU                    = 0x80000240,   ///< Right %Menu
    KC_MENU                     = 0x80000250,   ///< %Menu
    KC_LHYPER                   = 0x80000260,   ///< Left Hyper
    KC_RHYPER                   = 0x80000270,   ///< Right Hyper
    KC_HYPER                    = 0x80000280,   ///< Hyper
    KC_GROUP                    = 0x80000290,   ///< Group
    KC_NUM_LOCK                 = 0x800002A0,   ///< Num Lock
    KC_SCROLL_LOCK              = 0x800002B0,   ///< Scroll Lock
    KC_CAPS_LOCK                = 0x800002C0,   ///< Caps Lock
    KC_SHIFT_LOCK               = 0x800002D0    ///< Shift Lock
};

/// Mouse buttons.
/// The better name for those enumerators should be @b "MB_xxx" but
/// @b "MB_" prefix clashes with Windows mouse button defines, so
/// I added "T" letter.
/// @ingroup input_group
enum Mouse_buttons {
    MBT_NONE                     = 0,           ///< Unspecified mouse button
    MBT_LEFT                     = 1,           ///< Left mouse button
    MBT_MIDDLE                   = 2,           ///< Middle mouse button
    MBT_RIGHT                    = 3            ///< Right mouse button
};

/// Mouse modifiers.
/// @ingroup input_group
enum Mouse_modifiers {
    MM_NONE                     = 0x0000,       ///< No modifiers
    MM_LEFT                     = 0x0001,       ///< Left mouse modifier
    MM_MIDDLE                   = 0x0002,       ///< Middle mouse modifier
    MM_RIGHT                    = 0x0004,       ///< Right mouse modifier
    MM_SHIFT                    = 0x0100,       ///< Shift mouse modifier
    MM_CONTROL                  = 0x0200        ///< Control mouse modifier
};

/// Convert UCS-4 character into string.
/// If kc is a member of #Key_codes enum, the result will be name of kc
/// (all available at the moment key names are summarized in @ref kc_sect).
/// Otherwise, the result will looks like "U+xxxx", where "xxxx" is key code
/// doesn't matter whether it Unicode character or not.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref kc_sect
ustring key_code_to_string(char32_t kc);

/// Gets key code from a string.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref kc_sect
char32_t key_code_from_string(const ustring & str);

/// Convert key modifiers into string representation.
/// Resulting string will contain something like this: "\<CONTROL\>\<SHIFT\>".
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref km_sect
ustring key_modifiers_to_string(int km);

/// Convert string representation to key modifiers.
/// To successful convert, the str must contain key modifiers textual representation
/// in form like this: "\<CONTROL\>\<Alt\>". The character case does not matter.
/// See complete list of all recognizable key modifier representation at @ref km_sect.
/// If no representations found, the @link KM_NONE @endlink value will be returned.
/// @param str the string possibly containing representation of any key modifiers.
/// @return bitwise mask of flags mentioned in #Key_modifiers enumeration.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref km_sect
int key_modifiers_from_string(const ustring & str);

/// Convert key specification (code+modifiers) into string representation.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref km_sect
/// @sa @ref kc_sect
ustring key_spec_to_string(char32_t kc, int km);

/// Convert key specification (code+modifiers) into human readable text representation.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref km_sect
/// @sa @ref kc_sect
ustring key_spec_to_label(char32_t kc, int km);

/// Extract key code and modifier from string.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref km_sect
/// @sa @ref kc_sect
void key_spec_from_string(const ustring & spec, char32_t & kc, int & km);

/// Check is key code a modifier key.
/// @ingroup input_group
/// @sa @ref kbd_sect
/// @sa @ref km_sect
/// @sa @ref kc_sect
bool key_code_is_modifier(char32_t kc);

/// @name Const access string variables providing key modifiers names
/// @sa @ref kbd_sect
/// @sa @ref km_sect
/// @{

/// Actual value is "NONE".
/// @ingroup input_group
extern const char * KM_NONE_NAME;

/// Actual value is "SHIFT".
/// @ingroup input_group
extern const char * KM_SHIFT_NAME;

/// Actual value is "CONTROL".
/// @ingroup input_group
extern const char * KM_CONTROL_NAME;

/// Actual value is "ALT".
/// @ingroup input_group
extern const char * KM_ALT_NAME;

/// Actual value is "META".
/// @ingroup input_group
extern const char * KM_META_NAME;

/// Actual value is "WIN".
/// @ingroup input_group
extern const char * KM_WIN_NAME;

/// Actual value is "SUPER".
/// @ingroup input_group
extern const char * KM_SUPER_NAME;

/// Actual value is "MENU".
/// @ingroup input_group
extern const char * KM_MENU_NAME;

/// Actual value is "HYPER".
/// @ingroup input_group
extern const char * KM_HYPER_NAME;

/// Actual value is "GROUP".
/// @ingroup input_group
extern const char * KM_GROUP_NAME;

/// Actual value is "NUMLOCK".
/// @ingroup input_group
extern const char * KM_NUMLOCK_NAME;

/// Actual value is "SCROLL".
/// @ingroup input_group
extern const char * KM_SCROLL_NAME;

/// Actual value is "CAPS".
/// @ingroup input_group
extern const char * KM_CAPS_NAME;

/// @}

/// @name Const access string variables providing key code names
/// @sa @ref kbd_sect
/// @sa @ref kc_sect
/// @{

/// Actual value is "None".
/// @ingroup input_group
extern const char * KC_NONE_NAME;

/// Actual value is "BackSpace".
/// @ingroup input_group
extern const char * KC_BACKSPACE_NAME;

/// Actual value is "Tab".
/// @ingroup input_group
extern const char * KC_TAB_NAME;

/// Actual value is "LineFeed".
/// @ingroup input_group
extern const char * KC_LINEFEED_NAME;

/// Actual value is "Enter".
/// @ingroup input_group
extern const char * KC_ENTER_NAME;

/// Actual value is "Return".
/// @ingroup input_group
extern const char * KC_RETURN_NAME;

/// Actual value is "Escape".
/// @ingroup input_group
extern const char * KC_ESCAPE_NAME;

/// Actual value is "Space".
/// @ingroup input_group
extern const char * KC_SPACE_NAME;

/// Actual value is "Delete".
/// @ingroup input_group
extern const char * KC_DELETE_NAME;

/// Actual value is "F1".
/// @ingroup input_group
extern const char * KC_F1_NAME;

/// Actual value is "F2".
/// @ingroup input_group
extern const char * KC_F2_NAME;

/// Actual value is "F3".
/// @ingroup input_group
extern const char * KC_F3_NAME;

/// Actual value is "F4".
/// @ingroup input_group
extern const char * KC_F4_NAME;

/// Actual value is "F5".
/// @ingroup input_group
extern const char * KC_F5_NAME;

/// Actual value is "F6".
/// @ingroup input_group
extern const char * KC_F6_NAME;

/// Actual value is "F7".
/// @ingroup input_group
extern const char * KC_F7_NAME;

/// Actual value is "F8".
/// @ingroup input_group
extern const char * KC_F8_NAME;

/// Actual value is "F9".
/// @ingroup input_group
extern const char * KC_F9_NAME;

/// Actual value is "F10".
/// @ingroup input_group
extern const char * KC_F10_NAME;

/// Actual value is "F11".
/// @ingroup input_group
extern const char * KC_F11_NAME;

/// Actual value is "F12".
/// @ingroup input_group
extern const char * KC_F12_NAME;

/// Actual value is "F13".
/// @ingroup input_group
extern const char * KC_F13_NAME;

/// Actual value is "F14".
/// @ingroup input_group
extern const char * KC_F14_NAME;

/// Actual value is "F15".
/// @ingroup input_group
extern const char * KC_F15_NAME;

/// Actual value is "F16".
/// @ingroup input_group
extern const char * KC_F16_NAME;

/// Actual value is "F17".
/// @ingroup input_group
extern const char * KC_F17_NAME;

/// Actual value is "F18".
/// @ingroup input_group
extern const char * KC_F18_NAME;

/// Actual value is "F19".
/// @ingroup input_group
extern const char * KC_F19_NAME;

/// Actual value is "F20".
/// @ingroup input_group
extern const char * KC_F20_NAME;

/// Actual value is "F21".
/// @ingroup input_group
extern const char * KC_F21_NAME;

/// Actual value is "F22".
/// @ingroup input_group
extern const char * KC_F22_NAME;

/// Actual value is "F23".
/// @ingroup input_group
extern const char * KC_F23_NAME;

/// Actual value is "F24".
/// @ingroup input_group
extern const char * KC_F24_NAME;

/// Actual value is "F25".
/// @ingroup input_group
extern const char * KC_F25_NAME;

/// Actual value is "F26".
/// @ingroup input_group
extern const char * KC_F26_NAME;

/// Actual value is "F27".
/// @ingroup input_group
extern const char * KC_F27_NAME;

/// Actual value is "F28".
/// @ingroup input_group
extern const char * KC_F28_NAME;

/// Actual value is "F29".
/// @ingroup input_group
extern const char * KC_F29_NAME;

/// Actual value is "F30".
/// @ingroup input_group
extern const char * KC_F30_NAME;

/// Actual value is "F31".
/// @ingroup input_group
extern const char * KC_F31_NAME;

/// Actual value is "F32".
/// @ingroup input_group
extern const char * KC_F32_NAME;

/// Actual value is "F33".
/// @ingroup input_group
extern const char * KC_F33_NAME;

/// Actual value is "F34".
/// @ingroup input_group
extern const char * KC_F34_NAME;

/// Actual value is "F35".
/// @ingroup input_group
extern const char * KC_F35_NAME;

/// Actual value is "Left".
/// @ingroup input_group
extern const char * KC_LEFT_NAME;

/// Actual value is "Right".
/// @ingroup input_group
extern const char * KC_RIGHT_NAME;

/// Actual value is "Up".
/// @ingroup input_group
extern const char * KC_UP_NAME;

/// Actual value is "Down".
/// @ingroup input_group
extern const char * KC_DOWN_NAME;

/// Actual value is "Home".
/// @ingroup input_group
extern const char * KC_HOME_NAME;

/// Actual value is "End".
/// @ingroup input_group
extern const char * KC_END_NAME;

/// Actual value is "PageUp".
/// @ingroup input_group
extern const char * KC_PAGE_UP_NAME;

/// Actual value is "PageDown".
/// @ingroup input_group
extern const char * KC_PAGE_DOWN_NAME;

/// Actual value is "ScrollUp".
/// @ingroup input_group
extern const char * KC_SCROLL_UP_NAME;

/// Actual value is "ScrollDown".
/// @ingroup input_group
extern const char * KC_SCROLL_DOWN_NAME;

/// Actual value is "Insert".
/// @ingroup input_group
extern const char * KC_INSERT_NAME;

/// Actual value is "Begin".
/// @ingroup input_group
extern const char * KC_BEGIN_NAME;

/// Actual value is "Cancel".
/// @ingroup input_group
extern const char * KC_CANCEL_NAME;

/// Actual value is "Break".
/// @ingroup input_group
extern const char * KC_BREAK_NAME;

/// Actual value is "Clear".
/// @ingroup input_group
extern const char * KC_CLEAR_NAME;

/// Actual value is "Pause".
/// @ingroup input_group
extern const char * KC_PAUSE_NAME;

/// Actual value is "Print".
/// @ingroup input_group
extern const char * KC_PRINT_NAME;

/// Actual value is "SysReq".
/// @ingroup input_group
extern const char * KC_SYSREQ_NAME;

/// Actual value is "Help".
/// @ingroup input_group
extern const char * KC_HELP_NAME;

/// Actual value is "Undo".
/// @ingroup input_group
extern const char * KC_UNDO_NAME;

/// Actual value is "Redo".
/// @ingroup input_group
extern const char * KC_REDO_NAME;

/// Actual value is "Find".
/// @ingroup input_group
extern const char * KC_FIND_NAME;

/// Actual value is "Select".
/// @ingroup input_group
extern const char * KC_SELECT_NAME;

/// Actual value is "No".
/// @ingroup input_group
extern const char * KC_NO_NAME;

/// Actual value is "Yes".
/// @ingroup input_group
extern const char * KC_YES_NAME;

/// Actual value is "Copy".
/// @ingroup input_group
extern const char * KC_COPY_NAME;

/// Actual value is "Cut".
/// @ingroup input_group
extern const char * KC_CUT_NAME;

/// Actual value is "Paste".
/// @ingroup input_group
extern const char * KC_PASTE_NAME;

/// Actual value is "DirectionL".
/// @ingroup input_group
extern const char * KC_LDIR_NAME;

/// Actual value is "DirectionR".
/// @ingroup input_group
extern const char * KC_RDIR_NAME;

/// Actual value is "Multi".
/// @ingroup input_group
extern const char * KC_MULTI_KEY_NAME;

/// Actual value is "CodeInput".
/// @ingroup input_group
extern const char * KC_CODE_INPUT_NAME;

/// Actual value is "SingleCandidate".
/// @ingroup input_group
extern const char * KC_SINGLE_CANDIDATE_NAME;

/// Actual value is "MultipleCandidate".
/// @ingroup input_group
extern const char * KC_MULTIPLE_CANDIDATE_NAME;

/// Actual value is "PreviousCandidate".
/// @ingroup input_group
extern const char * KC_PREVIOUS_CANDIDATE_NAME;

/// Actual value is "ModeSwitch".
/// @ingroup input_group
extern const char * KC_MODE_SWITCH_NAME;

/// Actual value is "Separator".
/// @ingroup input_group
extern const char * KC_SEPARATOR_NAME;

/// Actual value is "Decimal".
/// @ingroup input_group
extern const char * KC_DECIMAL_NAME;

/// Actual value is "ISO_Lock".
/// @ingroup input_group
extern const char * KC_ISO_LOCK_NAME;

/// Actual value is "Level2_Latch".
/// @ingroup input_group
extern const char * KC_LEVEL2_LATCH_NAME;

/// Actual value is "Level3_Shift".
/// @ingroup input_group
extern const char * KC_LEVEL3_SHIFT_NAME;

/// Actual value is "Level3_Latch".
/// @ingroup input_group
extern const char * KC_LEVEL3_LATCH_NAME;

/// Actual value is "Level3_Lock".
/// @ingroup input_group
extern const char * KC_LEVEL3_LOCK_NAME;

/// Actual value is "Level5_Shift".
/// @ingroup input_group
extern const char * KC_LEVEL5_SHIFT_NAME;

/// Actual value is "Level5_Latch".
/// @ingroup input_group
extern const char * KC_LEVEL5_LATCH_NAME;

/// Actual value is "Level5_Lock".
/// @ingroup input_group
extern const char * KC_LEVEL5_LOCK_NAME;

/// Actual value is "GroupShift".
/// @ingroup input_group
extern const char * KC_GROUP_SHIFT_NAME;

/// Actual value is "GroupLatch".
/// @ingroup input_group
extern const char * KC_GROUP_LATCH_NAME;

/// Actual value is "GroupLock".
/// @ingroup input_group
extern const char * KC_GROUP_LOCK_NAME;

/// Actual value is "NextGroup".
/// @ingroup input_group
extern const char * KC_NEXT_GROUP_NAME;

/// Actual value is "NextGroupLock".
/// @ingroup input_group
extern const char * KC_NEXT_GROUP_LOCK_NAME;

/// Actual value is "PrevGroup".
/// @ingroup input_group
extern const char * KC_PREV_GROUP_NAME;

/// Actual value is "PrevGroupLock".
/// @ingroup input_group
extern const char * KC_PREV_GROUP_LOCK_NAME;

/// Actual value is "FirstGroup".
/// @ingroup input_group
extern const char * KC_FIRST_GROUP_NAME;

/// Actual value is "FirstGroupLock".
/// @ingroup input_group
extern const char * KC_FIRST_GROUP_LOCK_NAME;

/// Actual value is "LastGroup".
/// @ingroup input_group
extern const char * KC_LAST_GROUP_NAME;

/// Actual value is "LastGroupLock".
/// @ingroup input_group
extern const char * KC_LAST_GROUP_LOCK_NAME;

/// Actual value is "LeftTab".
/// @ingroup input_group
extern const char * KC_LEFT_TAB_NAME;

/// Actual value is "MoveLineUp".
/// @ingroup input_group
extern const char * KC_MOVE_LINE_UP_NAME;

/// Actual value is "MoveLineDown".
/// @ingroup input_group
extern const char * KC_MOVE_LINE_DOWN_NAME;

/// Actual value is "PartialSpaceLeft".
/// @ingroup input_group
extern const char * KC_PARTIAL_SPACE_LEFT_NAME;

/// Actual value is "PartialSpaceRight".
/// @ingroup input_group
extern const char * KC_PARTIAL_SPACE_RIGHT_NAME;

/// Actual value is "PartialLineUp".
/// @ingroup input_group
extern const char * KC_PARTIAL_LINE_UP_NAME;

/// Actual value is "PartialLineDown".
/// @ingroup input_group
extern const char * KC_PARTIAL_LINE_DOWN_NAME;

/// Actual value is "SetMarginLeft".
/// @ingroup input_group
extern const char * KC_SET_MARGIN_LEFT_NAME;

/// Actual value is "SetMarginRight".
/// @ingroup input_group
extern const char * KC_SET_MARGIN_RIGHT_NAME;

/// Actual value is "ReleaseMarginLeft".
/// @ingroup input_group
extern const char * KC_RELEASE_MARGIN_LEFT_NAME;

/// Actual value is "ReleaseMarginRight".
/// @ingroup input_group
extern const char * KC_RELEASE_MARGIN_RIGHT_NAME;

/// Actual value is "ReleaseBothMargins".
/// @ingroup input_group
extern const char * KC_RELEASE_BOTH_MARGINS_NAME;

/// Actual value is "FastCursorLeft".
/// @ingroup input_group
extern const char * KC_FAST_CURSOR_LEFT_NAME;

/// Actual value is "FastCursorRight".
/// @ingroup input_group
extern const char * KC_FAST_CURSOR_RIGHT_NAME;

/// Actual value is "FastCursorUp".
/// @ingroup input_group
extern const char * KC_FAST_CURSOR_UP_NAME;

/// Actual value is "FastCursorDown".
/// @ingroup input_group
extern const char * KC_FAST_CURSOR_DOWN_NAME;

/// Actual value is "ContinuousUnderline".
/// @ingroup input_group
extern const char * KC_CONTINUOUS_UNDERLINE_NAME;

/// Actual value is "DiscontinuousUnderline".
/// @ingroup input_group
extern const char * KC_DISCONTINUOUS_UNDERLINE_NAME;

/// Actual value is "Emphasize".
/// @ingroup input_group
extern const char * KC_EMPHASIZE_NAME;

/// Actual value is "CenterObject".
/// @ingroup input_group
extern const char * KC_CENTER_OBJECT_NAME;

/// Actual value is "Send".
/// @ingroup input_group
extern const char * KC_SEND_NAME;

/// Actual value is "Reply".
/// @ingroup input_group
extern const char * KC_REPLY_NAME;

/// Actual value is "ZoomIn".
/// @ingroup input_group
extern const char * KC_ZOOM_IN_NAME;

/// Actual value is "ZoomOut".
/// @ingroup input_group
extern const char * KC_ZOOM_OUT_NAME;

/// Actual value is "Forward".
/// @ingroup input_group
extern const char * KC_FORWARD_NAME;

/// Actual value is "Back".
/// @ingroup input_group
extern const char * KC_BACK_NAME;

/// Actual value is "Stop".
/// @ingroup input_group
extern const char * KC_STOP_NAME;

/// Actual value is "Refresh".
/// @ingroup input_group
extern const char * KC_REFRESH_NAME;

/// Actual value is "HomePage".
/// @ingroup input_group
extern const char * KC_HOME_PAGE_NAME;

/// Actual value is "Favorites".
/// @ingroup input_group
extern const char * KC_FAVORITES_NAME;

/// Actual value is "Search".
/// @ingroup input_group
extern const char * KC_SEARCH_NAME;

/// Actual value is "Standby".
/// @ingroup input_group
extern const char * KC_STANDBY_NAME;

/// Actual value is "OpenURL".
/// @ingroup input_group
extern const char * KC_OPEN_URL_NAME;

/// Actual value is "LaunchMail".
/// @ingroup input_group
extern const char * KC_LAUNCH_MAIL_NAME;

/// Actual value is "LaunchMedia".
/// @ingroup input_group
extern const char * KC_LAUNCH_MEDIA_NAME;

/// Actual value is "Launch0".
/// @ingroup input_group
extern const char * KC_LAUNCH0_NAME;

/// Actual value is "Launch1".
/// @ingroup input_group
extern const char * KC_LAUNCH1_NAME;

/// Actual value is "Launch2".
/// @ingroup input_group
extern const char * KC_LAUNCH2_NAME;

/// Actual value is "Launch3".
/// @ingroup input_group
extern const char * KC_LAUNCH3_NAME;

/// Actual value is "Launch4".
/// @ingroup input_group
extern const char * KC_LAUNCH4_NAME;

/// Actual value is "Launch5".
/// @ingroup input_group
extern const char * KC_LAUNCH5_NAME;

/// Actual value is "Launch6".
/// @ingroup input_group
extern const char * KC_LAUNCH6_NAME;

/// Actual value is "Launch7".
/// @ingroup input_group
extern const char * KC_LAUNCH7_NAME;

/// Actual value is "Launch8".
/// @ingroup input_group
extern const char * KC_LAUNCH8_NAME;

/// Actual value is "Launch9".
/// @ingroup input_group
extern const char * KC_LAUNCH9_NAME;

/// Actual value is "LaunchA".
/// @ingroup input_group
extern const char * KC_LAUNCHA_NAME;

/// Actual value is "LaunchB".
/// @ingroup input_group
extern const char * KC_LAUNCHB_NAME;

/// Actual value is "LaunchC".
/// @ingroup input_group
extern const char * KC_LAUNCHC_NAME;

/// Actual value is "LaunchD".
/// @ingroup input_group
extern const char * KC_LAUNCHD_NAME;

/// Actual value is "LaunchE".
/// @ingroup input_group
extern const char * KC_LAUNCHE_NAME;

/// Actual value is "LaunchF".
/// @ingroup input_group
extern const char * KC_LAUNCHF_NAME;

/// Actual value is "Execute".
/// @ingroup input_group
extern const char * KC_EXECUTE_NAME;

/// Actual value is "Terminal".
/// @ingroup input_group
extern const char * KC_TERMINAL_NAME;

/// Actual value is "PowerOff".
/// @ingroup input_group
extern const char * KC_POWER_OFF_NAME;

/// Actual value is "Sleep".
/// @ingroup input_group
extern const char * KC_SLEEP_NAME;

/// Actual value is "WakeUp".
/// @ingroup input_group
extern const char * KC_WAKE_UP_NAME;

/// Actual value is "TerminateServer".
/// @ingroup input_group
extern const char * KC_TERMINATE_SERVER_NAME;

/// Actual value is "Close".
/// @ingroup input_group
extern const char * KC_CLOSE_NAME;

/// Actual value is "Phone".
/// @ingroup input_group
extern const char * KC_PHONE_NAME;

/// Actual value is "Reload".
/// @ingroup input_group
extern const char * KC_RELOAD_NAME;

/// Actual value is "Save".
/// @ingroup input_group
extern const char * KC_SAVE_NAME;

/// Actual value is "Video".
/// @ingroup input_group
extern const char * KC_VIDEO_NAME;

/// Actual value is "Music".
/// @ingroup input_group
extern const char * KC_MUSIC_NAME;

/// Actual value is "Battery".
/// @ingroup input_group
extern const char * KC_BATTERY_NAME;

/// Actual value is "Bluetooth".
/// @ingroup input_group
extern const char * KC_BLUETOOTH_NAME;

/// Actual value is "WLAN".
/// @ingroup input_group
extern const char * KC_WLAN_NAME;

/// Actual value is "VolumeUp".
/// @ingroup input_group
extern const char * KC_VOLUME_UP_NAME;

/// Actual value is "VolumeDown".
/// @ingroup input_group
extern const char * KC_VOLUME_DOWN_NAME;

/// Actual value is "VolumeMute".
/// @ingroup input_group
extern const char * KC_VOLUME_MUTE_NAME;

/// Actual value is "BassBoost".
/// @ingroup input_group
extern const char * KC_BASS_BOOST_NAME;

/// Actual value is "BassUp".
/// @ingroup input_group
extern const char * KC_BASS_UP_NAME;

/// Actual value is "BassDown".
/// @ingroup input_group
extern const char * KC_BASS_DOWN_NAME;

/// Actual value is "TrebleUp".
/// @ingroup input_group
extern const char * KC_TREBLE_UP_NAME;

/// Actual value is "TrebleDown".
/// @ingroup input_group
extern const char * KC_TREBLE_DOWN_NAME;

/// Actual value is "MediaPlay".
/// @ingroup input_group
extern const char * KC_MEDIA_PLAY_NAME;

/// Actual value is "MediaPause".
/// @ingroup input_group
extern const char * KC_MEDIA_PAUSE_NAME;

/// Actual value is "MediaStop".
/// @ingroup input_group
extern const char * KC_MEDIA_STOP_NAME;

/// Actual value is "MediaPrevious".
/// @ingroup input_group
extern const char * KC_MEDIA_PREVIOUS_NAME;

/// Actual value is "MediaNext".
/// @ingroup input_group
extern const char * KC_MEDIA_NEXT_NAME;

/// Actual value is "MediaRecord".
/// @ingroup input_group
extern const char * KC_MEDIA_RECORD_NAME;

/// Actual value is "Kanji".
/// @ingroup input_group
extern const char * KC_KANJI_NAME;

/// Actual value is "Muhenkan".
/// @ingroup input_group
extern const char * KC_MUHENKAN_NAME;

/// Actual value is "Henkan".
/// @ingroup input_group
extern const char * KC_HENKAN_NAME;

/// Actual value is "Romaji".
/// @ingroup input_group
extern const char * KC_ROMAJI_NAME;

/// Actual value is "Hiragana".
/// @ingroup input_group
extern const char * KC_HIRAGANA_NAME;

/// Actual value is "Katakana".
/// @ingroup input_group
extern const char * KC_KATAKANA_NAME;

/// Actual value is "HiraganaKatakana".
/// @ingroup input_group
extern const char * KC_HIRAGANA_KATAKANA_NAME;

/// Actual value is "Zenkaku".
/// @ingroup input_group
extern const char * KC_ZENKAKU_NAME;

/// Actual value is "Hankaku".
/// @ingroup input_group
extern const char * KC_HANKAKU_NAME;

/// Actual value is "ZenkakuHankaku".
/// @ingroup input_group
extern const char * KC_ZENKAKU_HANKAKU_NAME;

/// Actual value is "Touroku".
/// @ingroup input_group
extern const char * KC_TOUROKU_NAME;

/// Actual value is "Massyo".
/// @ingroup input_group
extern const char * KC_MASSYO_NAME;

/// Actual value is "KanaLock".
/// @ingroup input_group
extern const char * KC_KANA_LOCK_NAME;

/// Actual value is "KanaShift".
/// @ingroup input_group
extern const char * KC_KANA_SHIFT_NAME;

/// Actual value is "EisuShift".
/// @ingroup input_group
extern const char * KC_EISU_SHIFT_NAME;

/// Actual value is "EisuToggle".
/// @ingroup input_group
extern const char * KC_EISU_TOGGLE_NAME;

/// Actual value is "Hangul".
/// @ingroup input_group
extern const char * KC_HANGUL_NAME;

/// Actual value is "HangulStart".
/// @ingroup input_group
extern const char * KC_HANGUL_START_NAME;

/// Actual value is "HangulEnd".
/// @ingroup input_group
extern const char * KC_HANGUL_END_NAME;

/// Actual value is "HangulHanja".
/// @ingroup input_group
extern const char * KC_HANGUL_HANJA_NAME;

/// Actual value is "HangulJamo".
/// @ingroup input_group
extern const char * KC_HANGUL_JAMO_NAME;

/// Actual value is "HangulRomaja".
/// @ingroup input_group
extern const char * KC_HANGUL_ROMAJA_NAME;

/// Actual value is "HangulJeonja".
/// @ingroup input_group
extern const char * KC_HANGUL_JEONJA_NAME;

/// Actual value is "HangulBanja".
/// @ingroup input_group
extern const char * KC_HANGUL_BANJA_NAME;

/// Actual value is "HangulPrehanja".
/// @ingroup input_group
extern const char * KC_HANGUL_PREHANJA_NAME;

/// Actual value is "HangulPosthanja".
/// @ingroup input_group
extern const char * KC_HANGUL_POSTHANJA_NAME;

/// Actual value is "HangulSpecial".
/// @ingroup input_group
extern const char * KC_HANGUL_SPECIAL_NAME;

/// Actual value is "BrailleDot_1".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_1_NAME;

/// Actual value is "BrailleDot_2".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_2_NAME;

/// Actual value is "BrailleDot_3".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_3_NAME;

/// Actual value is "BrailleDot_4".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_4_NAME;

/// Actual value is "BrailleDot_5".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_5_NAME;

/// Actual value is "BrailleDot_6".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_6_NAME;

/// Actual value is "BrailleDot_7".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_7_NAME;

/// Actual value is "BrailleDot_8".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_8_NAME;

/// Actual value is "BrailleDot_9".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_9_NAME;

/// Actual value is "BrailleDot_10".
/// @ingroup input_group
extern const char * KC_BRAILLE_DOT_10_NAME;

/// Actual value is "ShiftL".
/// @ingroup input_group
extern const char * KC_LSHIFT_NAME;

/// Actual value is "ShiftR".
/// @ingroup input_group
extern const char * KC_RSHIFT_NAME;

/// Actual value is "Shift".
/// @ingroup input_group
extern const char * KC_SHIFT_NAME;

/// Actual value is "ControlL".
/// @ingroup input_group
extern const char * KC_LCONTROL_NAME;

/// Actual value is "ControlR".
/// @ingroup input_group
extern const char * KC_RCONTROL_NAME;

/// Actual value is "Control".
/// @ingroup input_group
extern const char * KC_CONTROL_NAME;

/// Actual value is "AltL".
/// @ingroup input_group
extern const char * KC_LALT_NAME;

/// Actual value is "AltR".
/// @ingroup input_group
extern const char * KC_RALT_NAME;

/// Actual value is "Alt".
/// @ingroup input_group
extern const char * KC_ALT_NAME;

/// Actual value is "MetaL".
/// @ingroup input_group
extern const char * KC_LMETA_NAME;

/// Actual value is "MetaR".
/// @ingroup input_group
extern const char * KC_RMETA_NAME;

/// Actual value is "Meta".
/// @ingroup input_group
extern const char * KC_META_NAME;

/// Actual value is "WinL".
/// @ingroup input_group
extern const char * KC_LWIN_NAME;

/// Actual value is "WinR".
/// @ingroup input_group
extern const char * KC_RWIN_NAME;

/// Actual value is "Win".
/// @ingroup input_group
extern const char * KC_WIN_NAME;

/// Actual value is "SuperL".
/// @ingroup input_group
extern const char * KC_LSUPER_NAME;

/// Actual value is "SuperR".
/// @ingroup input_group
extern const char * KC_RSUPER_NAME;

/// Actual value is "Super".
/// @ingroup input_group
extern const char * KC_SUPER_NAME;

/// Actual value is "MenuL".
/// @ingroup input_group
extern const char * KC_LMENU_NAME;

/// Actual value is "MenuR".
/// @ingroup input_group
extern const char * KC_RMENU_NAME;

/// Actual value is "Menu".
/// @ingroup input_group
extern const char * KC_MENU_NAME;

/// Actual value is "HyperL".
/// @ingroup input_group
extern const char * KC_LHYPER_NAME;

/// Actual value is "HyperR".
/// @ingroup input_group
extern const char * KC_RHYPER_NAME;

/// Actual value is "Hyper".
/// @ingroup input_group
extern const char * KC_HYPER_NAME;

/// Actual value is "Group".
/// @ingroup input_group
extern const char * KC_GROUP_NAME;

/// Actual value is "NumLock".
/// @ingroup input_group
extern const char * KC_NUM_LOCK_NAME;

/// Actual value is "ScrollLock".
/// @ingroup input_group
extern const char * KC_SCROLL_LOCK_NAME;

/// Actual value is "CapsLock".
/// @ingroup input_group
extern const char * KC_CAPS_LOCK_NAME;

/// Actual value is "ShiftLock".
/// @ingroup input_group
extern const char * KC_SHIFT_LOCK_NAME;

/// @}

} // namespace tau

#endif // TAU_INPUT_HH
