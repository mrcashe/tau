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

#include <tau/input.hh>
#include <tau/string.hh>
#include <iomanip>
#include <cctype>
#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <iostream>

namespace {

const struct { char32_t kc_; const char * name_; } keydb_[] = {
    { tau::KC_ENTER,                    "Enter"                     },
    { tau::KC_RETURN,                   "Return"                    },
    { tau::KC_CANCEL,                   "Cancel"                    },
    { tau::KC_BREAK,                    "Break"                     },
    { tau::KC_SPACE,                    "Space"                     },
    { tau::KC_BACKSPACE,                "BackSpace"                 },
    { tau::KC_TAB,                      "Tab"                       },
    { tau::KC_LINEFEED,                 "LineFeed"                  },
    { tau::KC_CLEAR,                    "Clear"                     },
    { tau::KC_ESCAPE,                   "Escape"                    },
    { tau::KC_LSHIFT,                   "ShiftL"                    },
    { tau::KC_RSHIFT,                   "ShiftR"                    },
    { tau::KC_SHIFT,                    "Shift"                     },
    { tau::KC_LCONTROL,                 "ControlL"                  },
    { tau::KC_RCONTROL,                 "ControlR"                  },
    { tau::KC_CONTROL,                  "Control"                   },
    { tau::KC_LALT,                     "AltL"                      },
    { tau::KC_RALT,                     "AltR"                      },
    { tau::KC_ALT,                      "Alt"                       },
    { tau::KC_LMETA,                    "MetaL"                     },
    { tau::KC_RMETA,                    "MetaR"                     },
    { tau::KC_META,                     "Meta"                      },
    { tau::KC_LWIN,                     "WinL"                      },
    { tau::KC_RWIN,                     "WinR"                      },
    { tau::KC_WIN,                      "Win"                       },
    { tau::KC_LSUPER,                   "SuperL"                    },
    { tau::KC_RSUPER,                   "SuperR"                    },
    { tau::KC_SUPER,                    "Super"                     },
    { tau::KC_LMENU,                    "MenuL"                     },
    { tau::KC_RMENU,                    "MenuR"                     },
    { tau::KC_MENU,                     "Menu"                      },
    { tau::KC_LHYPER,                   "HyperL"                    },
    { tau::KC_RHYPER,                   "HyperR"                    },
    { tau::KC_HYPER,                    "Hyper"                     },
    { tau::KC_GROUP,                    "Group"                     },
    { tau::KC_NUM_LOCK,                 "NumLock"                   },
    { tau::KC_SCROLL_LOCK,              "ScrollLock"                },
    { tau::KC_CAPS_LOCK,                "CapsLock"                  },
    { tau::KC_SHIFT_LOCK,               "ShiftLock"                 },
    { tau::KC_F1,                       "F1"                        },
    { tau::KC_F2,                       "F2"                        },
    { tau::KC_F3,                       "F3"                        },
    { tau::KC_F4,                       "F4"                        },
    { tau::KC_F5,                       "F5"                        },
    { tau::KC_F6,                       "F6"                        },
    { tau::KC_F7,                       "F7"                        },
    { tau::KC_F8,                       "F8"                        },
    { tau::KC_F9,                       "F9"                        },
    { tau::KC_F10,                      "F10"                       },
    { tau::KC_F11,                      "F11"                       },
    { tau::KC_F12,                      "F12"                       },
    { tau::KC_F13,                      "F13"                       },
    { tau::KC_F14,                      "F14"                       },
    { tau::KC_F15,                      "F15"                       },
    { tau::KC_F16,                      "F16"                       },
    { tau::KC_F17,                      "F17"                       },
    { tau::KC_F18,                      "F18"                       },
    { tau::KC_F19,                      "F19"                       },
    { tau::KC_F20,                      "F20"                       },
    { tau::KC_F21,                      "F21"                       },
    { tau::KC_F22,                      "F22"                       },
    { tau::KC_F23,                      "F23"                       },
    { tau::KC_F24,                      "F24"                       },
    { tau::KC_F25,                      "F25"                       },
    { tau::KC_F26,                      "F26"                       },
    { tau::KC_F27,                      "F27"                       },
    { tau::KC_F28,                      "F28"                       },
    { tau::KC_F29,                      "F29"                       },
    { tau::KC_F30,                      "F30"                       },
    { tau::KC_F31,                      "F31"                       },
    { tau::KC_F32,                      "F32"                       },
    { tau::KC_F33,                      "F33"                       },
    { tau::KC_F34,                      "F34"                       },
    { tau::KC_F35,                      "F35"                       },
    { tau::KC_LEFT,                     "Left"                      },
    { tau::KC_RIGHT,                    "Right"                     },
    { tau::KC_UP,                       "Up"                        },
    { tau::KC_DOWN,                     "Down"                      },
    { tau::KC_HOME,                     "Home"                      },
    { tau::KC_END,                      "End"                       },
    { tau::KC_PAGE_UP,                  "PageUp"                    },
    { tau::KC_PAGE_DOWN,                "PageDown"                  },
    { tau::KC_SCROLL_UP,                "ScrollUp"                  },
    { tau::KC_SCROLL_DOWN,              "ScrollDown"                },
    { tau::KC_INSERT,                   "Insert"                    },
    { tau::KC_DELETE,                   "Delete"                    },
    { tau::KC_BEGIN,                    "Begin"                     },
    { tau::KC_UNDO,                     "Undo"                      },
    { tau::KC_REDO,                     "Redo"                      },
    { tau::KC_FIND,                     "Find"                      },
    { tau::KC_PAUSE,                    "Pause"                     },
    { tau::KC_PRINT,                    "Print"                     },
    { tau::KC_SYSREQ,                   "SysReq"                    },
    { tau::KC_HELP,                     "Help"                      },
    { tau::KC_LDIR,                     "DirectionL"                },
    { tau::KC_RDIR,                     "DirectionR"                },
    { tau::KC_MULTI_KEY,                "Multi"                     },
    { tau::KC_CODE_INPUT,               "CodeInput"                 },
    { tau::KC_SINGLE_CANDIDATE,         "SingleCandidate"           },
    { tau::KC_MULTIPLE_CANDIDATE,       "MultipleCandidate"         },
    { tau::KC_PREVIOUS_CANDIDATE,       "PreviousCandidate"         },
    { tau::KC_KANJI,                    "Kanji"                     },
    { tau::KC_MUHENKAN,                 "Muhenkan"                  },
    { tau::KC_HENKAN,                   "Henkan"                    },
    { tau::KC_ROMAJI,                   "Romaji"                    },
    { tau::KC_HIRAGANA,                 "Hiragana"                  },
    { tau::KC_KATAKANA,                 "Katakana"                  },
    { tau::KC_HIRAGANA_KATAKANA,        "HiraganaKatakana"          },
    { tau::KC_ZENKAKU,                  "Zenkaku"                   },
    { tau::KC_HANKAKU,                  "Hankaku"                   },
    { tau::KC_ZENKAKU_HANKAKU,          "ZenkakuHankaku"            },
    { tau::KC_TOUROKU,                  "Touroku"                   },
    { tau::KC_MASSYO,                   "Massyo"                    },
    { tau::KC_KANA_LOCK,                "KanaLock"                  },
    { tau::KC_KANA_SHIFT,               "KanaShift"                 },
    { tau::KC_EISU_SHIFT,               "EisuShift"                 },
    { tau::KC_EISU_TOGGLE,              "EisuToggle"                },
    { tau::KC_HANGUL,                   "Hangul"                    },
    { tau::KC_HANGUL_START,             "HangulStart"               },
    { tau::KC_HANGUL_END,               "HangulEnd"                 },
    { tau::KC_HANGUL_HANJA,             "HangulHanja"               },
    { tau::KC_HANGUL_JAMO,              "HangulJamo"                },
    { tau::KC_HANGUL_ROMAJA,            "HangulRomaja"              },
    { tau::KC_HANGUL_JEONJA,            "HangulJeonja"              },
    { tau::KC_HANGUL_BANJA,             "HangulBanja"               },
    { tau::KC_HANGUL_PREHANJA,          "HangulPreHanja"            },
    { tau::KC_HANGUL_POSTHANJA,         "HangulPostHanja"           },
    { tau::KC_HANGUL_SPECIAL,           "HangulSpecial"             },
    { tau::KC_MODE_SWITCH,              "ModeSwitch"                },
    { tau::KC_SELECT,                   "Select"                    },
    { tau::KC_YES,                      "Yes"                       },
    { tau::KC_NO,                       "No"                        },
    { tau::KC_COPY,                     "Copy"                      },
    { tau::KC_CUT,                      "Cut"                       },
    { tau::KC_PASTE,                    "Paste"                     },
    { tau::KC_BACK,                     "Back"                      },
    { tau::KC_FORWARD,                  "Forward"                   },
    { tau::KC_STOP,                     "Stop"                      },
    { tau::KC_REFRESH,                  "Refresh"                   },
    { tau::KC_VOLUME_UP,                "VolumeUp"                  },
    { tau::KC_VOLUME_DOWN,              "VolumeDown"                },
    { tau::KC_VOLUME_MUTE,              "VolumeMute"                },
    { tau::KC_BASS_BOOST,               "BassBoost"                 },
    { tau::KC_BASS_UP,                  "BassUp"                    },
    { tau::KC_BASS_DOWN,                "BassDown"                  },
    { tau::KC_TREBLE_UP,                "TrebleUp"                  },
    { tau::KC_TREBLE_DOWN,              "TrebleDown"                },
    { tau::KC_MEDIA_PLAY,               "MediaPlay"                 },
    { tau::KC_MEDIA_PAUSE,              "MediaPause"                },
    { tau::KC_MEDIA_STOP,               "MediaStop"                 },
    { tau::KC_MEDIA_PREVIOUS,           "MediaPrevious"             },
    { tau::KC_MEDIA_NEXT,               "MediaNext"                 },
    { tau::KC_MEDIA_RECORD,             "MediaRecord"               },
    { tau::KC_HOME_PAGE,                "HomePage"                  },
    { tau::KC_FAVORITES,                "Favorites"                 },
    { tau::KC_SEARCH,                   "Search"                    },
    { tau::KC_STANDBY,                  "Standby"                   },
    { tau::KC_OPEN_URL,                 "OpenURL"                   },
    { tau::KC_LAUNCH_MAIL,              "LaunchMail"                },
    { tau::KC_LAUNCH_MEDIA,             "LaunchMedia"               },
    { tau::KC_LAUNCH0,                  "Launch0"                   },
    { tau::KC_LAUNCH1,                  "Launch1"                   },
    { tau::KC_LAUNCH2,                  "Launch2"                   },
    { tau::KC_LAUNCH3,                  "Launch3"                   },
    { tau::KC_LAUNCH4,                  "Launch4"                   },
    { tau::KC_LAUNCH5,                  "Launch5"                   },
    { tau::KC_LAUNCH6,                  "Launch6"                   },
    { tau::KC_LAUNCH7,                  "Launch7"                   },
    { tau::KC_LAUNCH8,                  "Launch8"                   },
    { tau::KC_LAUNCH9,                  "Launch9"                   },
    { tau::KC_LAUNCHA,                  "LaunchA"                   },
    { tau::KC_LAUNCHB,                  "LaunchB"                   },
    { tau::KC_LAUNCHC,                  "LaunchC"                   },
    { tau::KC_LAUNCHD,                  "LaunchD"                   },
    { tau::KC_LAUNCHE,                  "LaunchE"                   },
    { tau::KC_LAUNCHF,                  "LaunchF"                   },
    { tau::KC_EXECUTE,                  "Execute"                   },
    { tau::KC_TERMINAL,                 "Terminal"                  },
    { tau::KC_POWER_OFF,                "PowerOff"                  },
    { tau::KC_WAKE_UP,                  "WakeUp"                    },
    { tau::KC_SAVE,                     "Save"                      },
    { tau::KC_SEND,                     "Send"                      },
    { tau::KC_REPLY,                    "Reply"                     },
    { tau::KC_ZOOM_IN,                  "ZoomIn"                    },
    { tau::KC_ZOOM_OUT,                 "ZoomOut"                   },
    { tau::KC_SLEEP,                    "Sleep"                     },
    { tau::KC_TERMINATE_SERVER,         "TerminateServer"           },
    { tau::KC_CLOSE,                    "Close"                     },
    { tau::KC_VIDEO,                    "Video"                     },
    { tau::KC_MUSIC,                    "Music"                     },
    { tau::KC_PHONE,                    "Phone"                     },
    { tau::KC_RELOAD,                   "Reload"                    },
    { tau::KC_BATTERY,                  "Battery"                   },
    { tau::KC_BLUETOOTH,                "Bluetooth"                 },
    { tau::KC_WLAN,                     "WLAN"                      },
    { tau::KC_ISO_LOCK,                 "ISO_Lock"                  },
    { tau::KC_LEVEL2_LATCH,             "Level2_Latch"              },
    { tau::KC_LEVEL3_SHIFT,             "Level3_Shift"              },
    { tau::KC_LEVEL3_LATCH,             "Level3_Latch"              },
    { tau::KC_LEVEL3_LOCK,              "Level3_Lock"               },
    { tau::KC_LEVEL5_SHIFT,             "Level5_Shift"              },
    { tau::KC_LEVEL5_LATCH,             "Level5_Latch"              },
    { tau::KC_LEVEL5_LOCK,              "Level5_Lock"               },
    { tau::KC_GROUP_SHIFT,              "GroupShift"                },
    { tau::KC_GROUP_LATCH,              "GroupLatch"                },
    { tau::KC_GROUP_LOCK,               "GroupLock"                 },
    { tau::KC_NEXT_GROUP,               "NextGroup"                 },
    { tau::KC_NEXT_GROUP_LOCK,          "NextGroupLock"             },
    { tau::KC_PREV_GROUP,               "PrevGroup"                 },
    { tau::KC_PREV_GROUP_LOCK,          "PrevGroupLock"             },
    { tau::KC_FIRST_GROUP,              "FirstGroup"                },
    { tau::KC_FIRST_GROUP_LOCK,         "FirstGroupLock"            },
    { tau::KC_LAST_GROUP,               "LastGroup"                 },
    { tau::KC_LAST_GROUP_LOCK,          "LastGroupLock"             },
    { tau::KC_LEFT_TAB,                 "LeftTab"                   },
    { tau::KC_MOVE_LINE_UP,             "MoveLineUp"                },
    { tau::KC_MOVE_LINE_DOWN,           "MoveLineDown"              },
    { tau::KC_PARTIAL_LINE_UP,          "PartialLineUp"             },
    { tau::KC_PARTIAL_LINE_DOWN,        "PartialLineDown"           },
    { tau::KC_PARTIAL_SPACE_LEFT,       "PartialSpaceLeft"          },
    { tau::KC_PARTIAL_SPACE_RIGHT,      "PartialSpaceRight"         },
    { tau::KC_SET_MARGIN_LEFT,          "SetMarginLeft"             },
    { tau::KC_SET_MARGIN_RIGHT,         "SetMarginRight"            },
    { tau::KC_RELEASE_MARGIN_LEFT,      "ReleaseMarginLeft"         },
    { tau::KC_RELEASE_MARGIN_RIGHT,     "ReleaseMarginRight"        },
    { tau::KC_RELEASE_BOTH_MARGINS,     "ReleaseBothMargins"        },
    { tau::KC_FAST_CURSOR_LEFT,         "FastCursorLeft"            },
    { tau::KC_FAST_CURSOR_RIGHT,        "FastCursorRight"           },
    { tau::KC_FAST_CURSOR_UP,           "FastCursorUp"              },
    { tau::KC_FAST_CURSOR_DOWN,         "FastCursorDown"            },
    { tau::KC_CONTINUOUS_UNDERLINE,     "ContinuousUnderline"       },
    { tau::KC_DISCONTINUOUS_UNDERLINE,  "DiscontinuousUnderline"    },
    { tau::KC_EMPHASIZE,                "Emphasize"                 },
    { tau::KC_CENTER_OBJECT,            "CenterObject"              },
    { tau::KC_BRAILLE_DOT_1,            "Braille_Dot_1"             },
    { tau::KC_BRAILLE_DOT_2,            "Braille_Dot_2"             },
    { tau::KC_BRAILLE_DOT_3,            "Braille_Dot_3"             },
    { tau::KC_BRAILLE_DOT_4,            "Braille_Dot_4"             },
    { tau::KC_BRAILLE_DOT_5,            "Braille_Dot_5"             },
    { tau::KC_BRAILLE_DOT_6,            "Braille_Dot_6"             },
    { tau::KC_BRAILLE_DOT_7,            "Braille_Dot_7"             },
    { tau::KC_BRAILLE_DOT_8,            "Braille_Dot_8"             },
    { tau::KC_BRAILLE_DOT_9,            "Braille_Dot_9"             },
    { tau::KC_BRAILLE_DOT_10,           "Braille_Dot_10"            },
    { 0,                                nullptr                     }
};

uint32_t keycode_from_keyname(const std::string & name) {
    static std::unordered_map<std::string, uint32_t> map;

    if (map.empty()) {
        for (unsigned i = 0; keydb_[i].name_; ++i) {
            tau::ustring kname = tau::str_tolower(keydb_[i].name_);
            map[kname] = keydb_[i].kc_;
        }
    }

    auto iter = map.find(tau::str_tolower(name));
    if (map.end() != iter) {
        return iter->second;
    }

    return 0;
}

std::string keyname_from_keycode(uint32_t kc) {
    static std::map<uint32_t, std::string> map;

    if (map.empty()) {
        for (unsigned i = 0; keydb_[i].name_; ++i) {
            map[keydb_[i].kc_] = keydb_[i].name_;
        }
    }

    auto iter = map.find(kc);

    if (map.end() != iter) {
        return iter->second;
    }

    return std::string();
}

char32_t xread(const std::string & str) {
    char32_t r = 0;
    std::string::size_type pos = 0, len = str.size();

    while (pos < len) {
        char c = std::toupper(str[pos++]);
        if (!std::isxdigit(c)) { return r; }
        c -= '0';
        if (c > 9) c -= 7;
        r <<= 4;
        r += c;
    }

    return r;
}

}

namespace tau {

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ustring key_code_to_string(char32_t kc) {
    std::string kn = keyname_from_keycode(kc);

    if (kn.empty()) {
        if (kc <= 0x10ffff) {
            kn = str_format("U+", std::setw(6), std::setfill('0'), std::hex, kc);
        }

        else {
            kn = str_format("0x", std::setw(8), std::setfill('0'), std::hex, kc);
        }
    }

    return kn;
}

char32_t key_code_from_string(const ustring & str) {
    ustring::size_type len = str.size(), pos = str.find_first_not_of(" \t");

    if (1 == str.size()) {
        return str[0];
    }

    if ('0' == str[pos] && 1+pos < len && 'X' == toupper(str[1+pos])) {
        return xread(str.substr(2+pos));
    }

    if ('U' == toupper(str[pos])) {
        if (1+pos < len && '+' == str[1+pos]) {
            return xread(str.substr(2+pos));
        }

        char32_t kc = xread(str.substr(1+pos));
        if (0 != kc) return kc;
    }

    return keycode_from_keyname(str);
}

ustring key_modifiers_to_string(int km) {
    ustring result;

    if (km & KM_SHIFT) {
        result += "<SHIFT>";
    }

    if (km & KM_CONTROL) {
        result += "<CONTROL>";
    }

    if (km & KM_ALT) {
        result += "<ALT>";
    }

    if (km & KM_META) {
        result += "<META>";
    }

    if (km & KM_SUPER) {
        result += "<SUPER>";
    }

    if (km & KM_HYPER) {
        result += "<HYPER>";
    }

    return result;
}

int key_modifiers_from_string(const ustring & str) {
    int km = 0;
    std::size_t pos = 0, end;

    while (ustring::npos != pos) {
        pos = str.find_first_of('<', pos);

        if (ustring::npos != pos) {
            end = str.find_first_of('>', pos);
            if (ustring::npos == end) break;
            ustring acc = str_toupper(str.substr(1+pos, end-pos-1));

            if ("SHIFT" == acc) {
                km |= KM_SHIFT;
            }

            else if ("CONTROL" == acc || "CTRL" == acc) {
                km |= KM_CONTROL;
            }

            else if ("ALT" == acc) {
                km |= KM_ALT;
            }

            else if ("META" == acc) {
                km |= KM_META;
            }

            else if ("SUPER" == acc) {
                km |= KM_SUPER;
            }

            else if ("HYPER" == acc) {
                km |= KM_HYPER;
            }

            pos = 1+end;
        }
    }

    return km;
}

ustring key_spec_to_label(char32_t kc, int km) {
    ustring res;

    if (KM_CONTROL & km) {
        res += "Ctrl+";
    }

    if (KM_SHIFT & km) {
        res += "Shift+";
    }

    if (KM_ALT & km) {
        res += "Alt+";
    }

    if (KM_META & km) {
        res += "Meta+";
    }

    if (KM_SUPER & km) {
        res += "Super+";
    }

    if (KM_HYPER & km) {
        res += "Hyper+";
    }

    if (KM_CONTROL & km) {
        if (kc >= 'a' && kc <= 'z') {
            res += char32_t(kc-0x20);
        }

        else if (kc >= 0x20 && kc < 0x7f) {
            res += char32_t(kc);
        }

        else {
            res += key_code_to_string(kc);
        }
    }

    else {
        if (kc >= 0x20 && kc < 0x7f) {
            res += char32_t(kc);
        }

        else {
            res += key_code_to_string(kc);
        }
    }

    return res;
}

ustring key_spec_to_string(char32_t kc, int km) {
    return key_modifiers_to_string(km)+key_code_to_string(kc);
}

void key_spec_from_string(const ustring & spec, char32_t & kc, int & km) {
    km = 0;
    kc = 0;

    ustring::size_type pos;
    pos = spec.find_last_of('>');

    if (ustring::npos != pos) {
        km = key_modifiers_from_string(spec.substr(0, 1+pos));
        kc = key_code_from_string(spec.substr(1+pos));
    }

    else {
        kc = key_code_from_string(spec);
    }
}

bool key_code_is_modifier(char32_t kc) {
    static const std::list<char32_t> mods = {
        KC_LSHIFT, KC_RSHIFT, KC_SHIFT, KC_LCONTROL, KC_RCONTROL, KC_CONTROL,
        KC_LALT, KC_RALT, KC_ALT, KC_LWIN, KC_RWIN, KC_WIN, KC_LSUPER, KC_RSUPER,
        KC_SUPER, KC_LMENU, KC_RMENU, KC_MENU, KC_LHYPER, KC_RHYPER, KC_HYPER, KC_GROUP,
        KC_NUM_LOCK, KC_SCROLL_LOCK, KC_CAPS_LOCK, KC_SHIFT_LOCK
    };

    auto iter = std::find(mods.begin(), mods.end(), kc);
    return mods.end() != iter;
}

} // namespace tau

//END
