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
/// @ingroup enum_group
enum Key_modifiers {
    KM_NONE                     = 0x00000000,
    KM_SHIFT                    = 0x00000001,
    KM_CONTROL                  = 0x00000002,
    KM_ALT                      = 0x00000004,
    KM_META                     = 0x00000008,
    KM_WIN                      = 0x00000010,
    KM_SUPER                    = 0x00000020,
    KM_MENU                     = 0x00000040,
    KM_HYPER                    = 0x00000080,
    KM_GROUP                    = 0x00000100,
    KM_NUMLOCK                  = 0x00000200,
    KM_SCROLL                   = 0x00000400,
    KM_CAPS                     = 0x00000800
};

/// Key codes, 32 bit.
/// @ingroup enum_group
enum Key_codes {
    KC_NONE                     = 0x00000000,
    KC_BACKSPACE                = 0x00000008,
    KC_TAB                      = 0x00000009,
    KC_LINEFEED                 = 0x0000000A,
    KC_ENTER                    = 0x0000000D,
    KC_RETURN                   = 0x0000000D,   // Synonymous KC_ENTER
    KC_ESCAPE                   = 0x0000001B,
    KC_SPACE                    = 0x00000020,
    KC_DELETE                   = 0x0000007F,

    // Function keys.
    KC_F1                       = 0x10000001,
    KC_F2                       = 0x10000002,
    KC_F3                       = 0x10000003,
    KC_F4                       = 0x10000004,
    KC_F5                       = 0x10000005,
    KC_F6                       = 0x10000006,
    KC_F7                       = 0x10000007,
    KC_F8                       = 0x10000008,
    KC_F9                       = 0x10000009,
    KC_F10                      = 0x1000000A,
    KC_F11                      = 0x1000000B,
    KC_F12                      = 0x1000000C,
    KC_F13                      = 0x1000000D,
    KC_F14                      = 0x1000000E,
    KC_F15                      = 0x1000000F,
    KC_F16                      = 0x10000010,
    KC_F17                      = 0x10000011,
    KC_F18                      = 0x10000012,
    KC_F19                      = 0x10000013,
    KC_F20                      = 0x10000014,
    KC_F21                      = 0x10000015,
    KC_F22                      = 0x10000016,
    KC_F23                      = 0x10000017,
    KC_F24                      = 0x10000018,
    KC_F25                      = 0x10000019,
    KC_F26                      = 0x1000001A,
    KC_F27                      = 0x1000001B,
    KC_F28                      = 0x1000001C,
    KC_F29                      = 0x1000001D,
    KC_F30                      = 0x1000001E,
    KC_F31                      = 0x1000001F,
    KC_F32                      = 0x10000020,
    KC_F33                      = 0x10000021,
    KC_F34                      = 0x10000022,
    KC_F35                      = 0x10000023,

    KC_LEFT                     = 0x20000000,
    KC_RIGHT                    = 0x20000001,
    KC_UP                       = 0x20000008,
    KC_DOWN                     = 0x20000009,
    KC_HOME                     = 0x20000010,
    KC_END                      = 0x20000011,
    KC_PAGE_UP                  = 0x20000020,
    KC_PAGE_DOWN                = 0x20000021,
    KC_SCROLL_UP                = 0x20000028,
    KC_SCROLL_DOWN              = 0x20000029,
    KC_INSERT                   = 0x20000030,
    KC_BEGIN                    = 0x20000040,
    KC_CANCEL                   = 0x10000050,
    KC_BREAK                    = 0x10000058,
    KC_CLEAR                    = 0x10000060,
    KC_PAUSE                    = 0x20000070,
    KC_PRINT                    = 0x20000080,
    KC_SYSREQ                   = 0x20000090,
    KC_HELP                     = 0x200000A0,
    KC_UNDO                     = 0x200000B0,
    KC_REDO                     = 0x200000B1,
    KC_FIND                     = 0x200000C0,
    KC_SELECT                   = 0x200000D0,
    KC_NO                       = 0x200000E0,
    KC_YES                      = 0x200000E1,
    KC_COPY                     = 0x200000F0,
    KC_CUT                      = 0x200000F2,
    KC_PASTE                    = 0x200000F4,

    KC_LDIR                     = 0x30000010,   // LEFT DIRECTION
    KC_RDIR                     = 0x30000020,   // RIGHT DIRECTION
    KC_MULTI_KEY                = 0x30000030,
    KC_CODE_INPUT               = 0x30000040,
    KC_SINGLE_CANDIDATE         = 0x30000050,
    KC_MULTIPLE_CANDIDATE       = 0x30000060,
    KC_PREVIOUS_CANDIDATE       = 0x30000070,
    KC_MODE_SWITCH              = 0x30000080,
    KC_SEPARATOR                = 0x30000090,   // Often comma.
    KC_DECIMAL                  = 0x300000A0,
    KC_ISO_LOCK                 = 0x300000B0,
    KC_LEVEL2_LATCH             = 0x300000C0,
    KC_LEVEL3_SHIFT             = 0x300000D0,
    KC_LEVEL3_LATCH             = 0x300000E0,
    KC_LEVEL3_LOCK              = 0x300000F0,
    KC_LEVEL5_SHIFT             = 0x30000100,
    KC_LEVEL5_LATCH             = 0x30000110,
    KC_LEVEL5_LOCK              = 0x30000120,
    KC_GROUP_SHIFT              = 0x30000130,
    KC_GROUP_LATCH              = 0x30000140,
    KC_GROUP_LOCK               = 0x30000150,
    KC_NEXT_GROUP               = 0x30000160,
    KC_NEXT_GROUP_LOCK          = 0x30000170,
    KC_PREV_GROUP               = 0x30000180,
    KC_PREV_GROUP_LOCK          = 0x30000190,
    KC_FIRST_GROUP              = 0x300001A0,
    KC_FIRST_GROUP_LOCK         = 0x300001B0,
    KC_LAST_GROUP               = 0x300001C0,
    KC_LAST_GROUP_LOCK          = 0x300001D0,
    KC_LEFT_TAB                 = 0x300001E0,
    KC_MOVE_LINE_UP             = 0x300001F0,
    KC_MOVE_LINE_DOWN           = 0x300001F1,
    KC_PARTIAL_SPACE_LEFT       = 0x30000200,
    KC_PARTIAL_SPACE_RIGHT      = 0x30000201,
    KC_PARTIAL_LINE_UP          = 0x30000210,
    KC_PARTIAL_LINE_DOWN        = 0x30000211,
    KC_SET_MARGIN_LEFT          = 0x30000220,
    KC_SET_MARGIN_RIGHT         = 0x30000221,
    KC_RELEASE_MARGIN_LEFT      = 0x30000230,
    KC_RELEASE_MARGIN_RIGHT     = 0x30000231,
    KC_RELEASE_BOTH_MARGINS     = 0x30000238,
    KC_FAST_CURSOR_LEFT         = 0x30000240,
    KC_FAST_CURSOR_RIGHT        = 0x30000241,
    KC_FAST_CURSOR_UP           = 0x30000248,
    KC_FAST_CURSOR_DOWN         = 0x30000249,
    KC_CONTINUOUS_UNDERLINE     = 0x30000250,
    KC_DISCONTINUOUS_UNDERLINE  = 0x30000251,
    KC_EMPHASIZE                = 0x30000260,
    KC_CENTER_OBJECT            = 0x30000270,
    KC_SEND                     = 0x30000280,   // Send mail, file, object
    KC_REPLY                    = 0x30000281,   // Reply e.g., mail
    KC_ZOOM_IN                  = 0x30000290,   // Zoom in view, map, etc.
    KC_ZOOM_OUT                 = 0x30000291,   // Zoom out view, map, etc.
    KC_FORWARD                  = 0x30000320,
    KC_BACK                     = 0x30000321,
    KC_STOP                     = 0x30000340,
    KC_REFRESH                  = 0x30000350,
    KC_HOME_PAGE                = 0x30000360,
    KC_FAVORITES                = 0x30000370,
    KC_SEARCH                   = 0x30000380,
    KC_STANDBY                  = 0x30000390,
    KC_OPEN_URL                 = 0x300003A0,
    KC_LAUNCH_MAIL              = 0x300003B0,
    KC_LAUNCH_MEDIA             = 0x300003C0,
    KC_LAUNCH0                  = 0x300003D0,
    KC_LAUNCH1                  = 0x300003D1,
    KC_LAUNCH2                  = 0x300003D2,
    KC_LAUNCH3                  = 0x300003D3,
    KC_LAUNCH4                  = 0x300003D4,
    KC_LAUNCH5                  = 0x300003D5,
    KC_LAUNCH6                  = 0x300003D6,
    KC_LAUNCH7                  = 0x300003D7,
    KC_LAUNCH8                  = 0x300003D8,
    KC_LAUNCH9                  = 0x300003D9,
    KC_LAUNCHA                  = 0x300003DA,
    KC_LAUNCHB                  = 0x300003DB,
    KC_LAUNCHC                  = 0x300003DC,
    KC_LAUNCHD                  = 0x300003DD,
    KC_LAUNCHE                  = 0x300003DE,
    KC_LAUNCHF                  = 0x300003DF,
    KC_EXECUTE                  = 0x300003F0,
    KC_TERMINAL                 = 0x300003F8,
    KC_POWER_OFF                = 0x30000400,
    KC_SLEEP                    = 0x30000402,
    KC_WAKE_UP                  = 0x30000408,
    KC_TERMINATE_SERVER         = 0x30000410,
    KC_CLOSE                    = 0x30000420,
    KC_PHONE                    = 0x30000430,   // Launch phone; dial number
    KC_RELOAD                   = 0x30000440,   // Reload web page, file, etc.
    KC_SAVE                     = 0x30000450,   // Save (file, document, state
    KC_VIDEO                    = 0x30000460,   // Launch video player
    KC_MUSIC                    = 0x30000462,   // Launch music applicaton
    KC_BATTERY                  = 0x30000470,   // Display battery information
    KC_BLUETOOTH                = 0x30000472,   // Enable/disable Bluetooth
    KC_WLAN                     = 0x30000474,   // Enable/disable WLAN

    // Multimedia
    KC_VOLUME_UP                = 0x37000010,
    KC_VOLUME_DOWN              = 0x37000030,
    KC_VOLUME_MUTE              = 0x37000050,
    KC_BASS_BOOST               = 0x37000070,
    KC_BASS_UP                  = 0x37000090,
    KC_BASS_DOWN                = 0x370000B0,
    KC_TREBLE_UP                = 0x370000D0,
    KC_TREBLE_DOWN              = 0x370000F0,
    KC_MEDIA_PLAY               = 0x37000110,
    KC_MEDIA_PAUSE              = 0x37000130,
    KC_MEDIA_STOP               = 0x37000150,
    KC_MEDIA_PREVIOUS           = 0x37000170,
    KC_MEDIA_NEXT               = 0x37000190,
    KC_MEDIA_RECORD             = 0x370001B0,

    // Japanese keyboard support.
    KC_KANJI                    = 0x41000010,   // Kanji, Kanji convert
    KC_MUHENKAN                 = 0x41000020,   // Cancel Conversion
    KC_HENKAN                   = 0x41000030,   // Alias for Henkan_Mode
    KC_ROMAJI                   = 0x41000040,   // to Romaji
    KC_HIRAGANA                 = 0x41000050,   // to Hiragana
    KC_KATAKANA                 = 0x41000060,   // to Katakana
    KC_HIRAGANA_KATAKANA        = 0x41000070,   // Hiragana/Katakana toggle
    KC_ZENKAKU                  = 0x41000080,   // to Zenkaku
    KC_HANKAKU                  = 0x41000090,   // to Hankaku
    KC_ZENKAKU_HANKAKU          = 0x410000A0,   // Zenkaku/Hankaku toggle
    KC_TOUROKU                  = 0x410000B0,   // Add to Dictionary
    KC_MASSYO                   = 0x410000C0,   // Delete from Dictionary
    KC_KANA_LOCK                = 0x410000D0,   // Kana Lock
    KC_KANA_SHIFT               = 0x410000E0,   // Kana Shift
    KC_EISU_SHIFT               = 0x410000F0,   // Alphanumeric Shift
    KC_EISU_TOGGLE              = 0x41000100,   // Alphanumeric toggle

    // Korean keyboard support.
    KC_HANGUL                   = 0x42000010,   // Hangul start/stop(toggle)
    KC_HANGUL_START             = 0x42000020,   // Hangul start
    KC_HANGUL_END               = 0x42000030,   // Hangul end, English start
    KC_HANGUL_HANJA             = 0x42000040,   // Start Hangul->Hanja Conversion
    KC_HANGUL_JAMO              = 0x42000050,   // Hangul Jamo mode
    KC_HANGUL_ROMAJA            = 0x42000060,   // Hangul Romaja mode
    KC_HANGUL_JEONJA            = 0x42000080,   // Jeonja mode
    KC_HANGUL_BANJA             = 0x42000090,   // Banja mode
    KC_HANGUL_PREHANJA          = 0x420000A0,   // Pre Hanja conversion
    KC_HANGUL_POSTHANJA         = 0x420000B0,   // Post Hanja conversion
    KC_HANGUL_SPECIAL           = 0x420000F0,   // Special symbols

    // Braille
    KC_BRAILLE_DOT_1            = 0x50000010,
    KC_BRAILLE_DOT_2            = 0x50000020,
    KC_BRAILLE_DOT_3            = 0x50000030,
    KC_BRAILLE_DOT_4            = 0x50000040,
    KC_BRAILLE_DOT_5            = 0x50000050,
    KC_BRAILLE_DOT_6            = 0x50000060,
    KC_BRAILLE_DOT_7            = 0x50000070,
    KC_BRAILLE_DOT_8            = 0x50000080,
    KC_BRAILLE_DOT_9            = 0x50000090,
    KC_BRAILLE_DOT_10           = 0x500000a0,

    // Modifiers
    KC_LSHIFT                   = 0x80000110,
    KC_RSHIFT                   = 0x80000120,
    KC_SHIFT                    = 0x80000130,
    KC_LCONTROL                 = 0x80000140,
    KC_RCONTROL                 = 0x80000150,
    KC_CONTROL                  = 0x80000160,
    KC_LALT                     = 0x80000170,
    KC_RALT                     = 0x80000180,
    KC_ALT                      = 0x80000190,
    KC_LMETA                    = 0x800001A0,
    KC_RMETA                    = 0x800001B0,
    KC_META                     = 0x800001C0,
    KC_LWIN                     = 0x800001D0,
    KC_RWIN                     = 0x800001E0,
    KC_WIN                      = 0x800001F0,
    KC_LSUPER                   = 0x80000200,
    KC_RSUPER                   = 0x80000210,
    KC_SUPER                    = 0x80000220,
    KC_LMENU                    = 0x80000230,
    KC_RMENU                    = 0x80000240,
    KC_MENU                     = 0x80000250,
    KC_LHYPER                   = 0x80000260,
    KC_RHYPER                   = 0x80000270,
    KC_HYPER                    = 0x80000280,
    KC_GROUP                    = 0x80000290,
    KC_NUM_LOCK                 = 0x800002A0,
    KC_SCROLL_LOCK              = 0x800002B0,
    KC_CAPS_LOCK                = 0x800002C0,
    KC_SHIFT_LOCK               = 0x800002D0
};

/// Mouse buttons.
/// @ingroup enum_group
enum Mouse_buttons {
    MBT_NONE                     = 0,           ///< Unspecified mouse button.
    MBT_LEFT                     = 1,           ///< Left mouse button.
    MBT_MIDDLE                   = 2,           ///< Middle mouse button.
    MBT_RIGHT                    = 3            ///< Right mouse button.
};

/// Mouse modifiers.
/// @ingroup enum_group
enum Mouse_modifiers {
    MM_NONE                     = 0x0000,       ///< No modifiers.
    MM_LEFT                     = 0x0001,       ///< Left mouse modifier.
    MM_MIDDLE                   = 0x0002,       ///< Middle mouse modifier.
    MM_RIGHT                    = 0x0004,       ///< Right mouse modifier.
    MM_SHIFT                    = 0x0100,       ///< Shift mouse modifier.
    MM_CONTROL                  = 0x0200        ///< Control mouse modifier.
};

/// Convert UCS-4 character into form "U+xxxx".
/// @ingroup input_group
ustring key_code_to_string(char32_t kc);

/// Gets key code from string.
/// @ingroup input_group
char32_t key_code_from_string(const ustring & str);

/// Convert key modifiers into string representation.
/// Resulting string will contain something like this: "<CONTROL><SHIFT>".
/// @ingroup input_group
ustring key_modifiers_to_string(int km);

/// Convert string representation to key modifiers.
/// @ingroup input_group
int key_modifiers_from_string(const ustring & str);

/// Convert key specification (code+modifiers) into string representation.
/// @ingroup input_group
ustring key_spec_to_string(char32_t kc, int km);

/// Convert key specification (code+modifiers) into human readable text representation.
/// @ingroup input_group
ustring key_spec_to_label(char32_t kc, int km);

/// Extract key code and modifier from string.
/// @ingroup input_group
void key_spec_from_string(const ustring & spec, char32_t & kc, int & km);

/// Check is key code a modifier key.
/// @ingroup input_group
bool key_code_is_modifier(char32_t kc);

} // namespace tau

#endif // TAU_INPUT_HH
