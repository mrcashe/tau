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

#include <tau/input.hh>
#include <tau/string.hh>
#include <iomanip>
#include <cctype>
#include <map>
#include <algorithm>
#include <iostream>
#include <forward_list>
#include <unordered_map>

namespace {

const struct { char32_t kc; const char * name; } keydb_[] = {
    { tau::KC_ENTER,                    tau::KC_ENTER_NAME                      },
    { tau::KC_RETURN,                   tau::KC_RETURN_NAME                     },
    { tau::KC_CANCEL,                   tau::KC_CANCEL_NAME                     },
    { tau::KC_BREAK,                    tau::KC_BREAK_NAME                      },
    { tau::KC_SPACE,                    tau::KC_SPACE_NAME                      },
    { tau::KC_BACKSPACE,                tau::KC_BACKSPACE_NAME                  },
    { tau::KC_TAB,                      tau::KC_TAB_NAME                        },
    { tau::KC_LINEFEED,                 tau::KC_LINEFEED_NAME                   },
    { tau::KC_CLEAR,                    tau::KC_CLEAR_NAME                      },
    { tau::KC_ESCAPE,                   tau::KC_ESCAPE_NAME                     },
    { tau::KC_LSHIFT,                   tau::KC_LSHIFT_NAME                     },
    { tau::KC_RSHIFT,                   tau::KC_RSHIFT_NAME                     },
    { tau::KC_SHIFT,                    tau::KC_SHIFT_NAME                      },
    { tau::KC_LCONTROL,                 tau::KC_LCONTROL_NAME                   },
    { tau::KC_RCONTROL,                 tau::KC_RCONTROL_NAME                   },
    { tau::KC_CONTROL,                  tau::KC_CONTROL_NAME                    },
    { tau::KC_LALT,                     tau::KC_LALT_NAME                       },
    { tau::KC_RALT,                     tau::KC_RALT_NAME                       },
    { tau::KC_ALT,                      tau::KC_ALT_NAME                        },
    { tau::KC_LMETA,                    tau::KC_LMETA_NAME                      },
    { tau::KC_RMETA,                    tau::KC_RMETA_NAME                      },
    { tau::KC_META,                     tau::KC_META_NAME                       },
    { tau::KC_LWIN,                     tau::KC_LWIN_NAME                       },
    { tau::KC_RWIN,                     tau::KC_RWIN_NAME                       },
    { tau::KC_WIN,                      tau::KC_WIN_NAME                        },
    { tau::KC_LSUPER,                   tau::KC_LSUPER_NAME                     },
    { tau::KC_RSUPER,                   tau::KC_RSUPER_NAME                     },
    { tau::KC_SUPER,                    tau::KC_SUPER_NAME                      },
    { tau::KC_LMENU,                    tau::KC_LMENU_NAME                      },
    { tau::KC_RMENU,                    tau::KC_RMENU_NAME                      },
    { tau::KC_MENU,                     tau::KC_MENU_NAME                       },
    { tau::KC_LHYPER,                   tau::KC_LHYPER_NAME                     },
    { tau::KC_RHYPER,                   tau::KC_RHYPER_NAME                     },
    { tau::KC_HYPER,                    tau::KC_HYPER_NAME                      },
    { tau::KC_GROUP,                    tau::KC_GROUP_NAME                      },
    { tau::KC_NUM_LOCK,                 tau::KC_NUM_LOCK_NAME                   },
    { tau::KC_SCROLL_LOCK,              tau::KC_SCROLL_LOCK_NAME                },
    { tau::KC_CAPS_LOCK,                tau::KC_CAPS_LOCK_NAME                  },
    { tau::KC_SHIFT_LOCK,               tau::KC_SHIFT_LOCK_NAME                 },
    { tau::KC_F1,                       tau::KC_F1_NAME                         },
    { tau::KC_F2,                       tau::KC_F2_NAME                         },
    { tau::KC_F3,                       tau::KC_F3_NAME                         },
    { tau::KC_F4,                       tau::KC_F4_NAME                         },
    { tau::KC_F5,                       tau::KC_F5_NAME                         },
    { tau::KC_F6,                       tau::KC_F6_NAME                         },
    { tau::KC_F7,                       tau::KC_F7_NAME                         },
    { tau::KC_F8,                       tau::KC_F8_NAME                         },
    { tau::KC_F9,                       tau::KC_F9_NAME                         },
    { tau::KC_F10,                      tau::KC_F10_NAME                        },
    { tau::KC_F11,                      tau::KC_F11_NAME                        },
    { tau::KC_F12,                      tau::KC_F12_NAME                        },
    { tau::KC_F13,                      tau::KC_F13_NAME                        },
    { tau::KC_F14,                      tau::KC_F14_NAME                        },
    { tau::KC_F15,                      tau::KC_F15_NAME                        },
    { tau::KC_F16,                      tau::KC_F16_NAME                        },
    { tau::KC_F17,                      tau::KC_F17_NAME                        },
    { tau::KC_F18,                      tau::KC_F18_NAME                        },
    { tau::KC_F19,                      tau::KC_F19_NAME                        },
    { tau::KC_F20,                      tau::KC_F20_NAME                        },
    { tau::KC_F21,                      tau::KC_F21_NAME                        },
    { tau::KC_F22,                      tau::KC_F22_NAME                        },
    { tau::KC_F23,                      tau::KC_F23_NAME                        },
    { tau::KC_F24,                      tau::KC_F24_NAME                        },
    { tau::KC_F25,                      tau::KC_F25_NAME                        },
    { tau::KC_F26,                      tau::KC_F26_NAME                        },
    { tau::KC_F27,                      tau::KC_F27_NAME                        },
    { tau::KC_F28,                      tau::KC_F28_NAME                        },
    { tau::KC_F29,                      tau::KC_F29_NAME                        },
    { tau::KC_F30,                      tau::KC_F30_NAME                        },
    { tau::KC_F31,                      tau::KC_F31_NAME                        },
    { tau::KC_F32,                      tau::KC_F32_NAME                        },
    { tau::KC_F33,                      tau::KC_F33_NAME                        },
    { tau::KC_F34,                      tau::KC_F34_NAME                        },
    { tau::KC_F35,                      tau::KC_F35_NAME                        },
    { tau::KC_LEFT,                     tau::KC_LEFT_NAME                       },
    { tau::KC_RIGHT,                    tau::KC_RIGHT_NAME                      },
    { tau::KC_UP,                       tau::KC_UP_NAME                         },
    { tau::KC_DOWN,                     tau::KC_DOWN_NAME                       },
    { tau::KC_HOME,                     tau::KC_HOME_NAME                       },
    { tau::KC_END,                      tau::KC_END_NAME                        },
    { tau::KC_PAGE_UP,                  tau::KC_PAGE_UP_NAME                    },
    { tau::KC_PAGE_DOWN,                tau::KC_PAGE_DOWN_NAME                  },
    { tau::KC_SCROLL_UP,                tau::KC_SCROLL_UP_NAME                  },
    { tau::KC_SCROLL_DOWN,              tau::KC_SCROLL_DOWN_NAME                },
    { tau::KC_INSERT,                   tau::KC_INSERT_NAME                     },
    { tau::KC_DELETE,                   tau::KC_DELETE_NAME                     },
    { tau::KC_BEGIN,                    tau::KC_BEGIN_NAME                      },
    { tau::KC_UNDO,                     tau::KC_UNDO_NAME                       },
    { tau::KC_REDO,                     tau::KC_REDO_NAME                       },
    { tau::KC_FIND,                     tau::KC_FIND_NAME                       },
    { tau::KC_PAUSE,                    tau::KC_PAUSE_NAME                      },
    { tau::KC_PRINT,                    tau::KC_PRINT_NAME                      },
    { tau::KC_SYSREQ,                   tau::KC_SYSREQ_NAME                     },
    { tau::KC_HELP,                     tau::KC_HELP_NAME                       },
    { tau::KC_LDIR,                     tau::KC_LDIR_NAME                       },
    { tau::KC_RDIR,                     tau::KC_RDIR_NAME                       },
    { tau::KC_MULTI,                    tau::KC_MULTI_NAME                      },
    { tau::KC_CODE_INPUT,               tau::KC_CODE_INPUT_NAME                 },
    { tau::KC_SINGLE_CANDIDATE,         tau::KC_SINGLE_CANDIDATE_NAME           },
    { tau::KC_MULTIPLE_CANDIDATE,       tau::KC_MULTIPLE_CANDIDATE_NAME         },
    { tau::KC_PREVIOUS_CANDIDATE,       tau::KC_PREVIOUS_CANDIDATE_NAME         },
    { tau::KC_KANJI,                    tau::KC_KANJI_NAME                      },
    { tau::KC_MUHENKAN,                 tau::KC_MUHENKAN_NAME                   },
    { tau::KC_HENKAN,                   tau::KC_HENKAN_NAME                     },
    { tau::KC_ROMAJI,                   tau::KC_ROMAJI_NAME                     },
    { tau::KC_HIRAGANA,                 tau::KC_HIRAGANA_NAME                   },
    { tau::KC_KATAKANA,                 tau::KC_KATAKANA_NAME                   },
    { tau::KC_HIRAGANA_KATAKANA,        tau::KC_HIRAGANA_KATAKANA_NAME          },
    { tau::KC_ZENKAKU,                  tau::KC_ZENKAKU_NAME                    },
    { tau::KC_HANKAKU,                  tau::KC_HANKAKU_NAME                    },
    { tau::KC_ZENKAKU_HANKAKU,          tau::KC_ZENKAKU_HANKAKU_NAME            },
    { tau::KC_TOUROKU,                  tau::KC_TOUROKU_NAME                    },
    { tau::KC_MASSYO,                   tau::KC_MASSYO_NAME                     },
    { tau::KC_KANA_LOCK,                tau::KC_KANA_LOCK_NAME                  },
    { tau::KC_KANA_SHIFT,               tau::KC_KANA_SHIFT_NAME                 },
    { tau::KC_EISU_SHIFT,               tau::KC_EISU_SHIFT_NAME                 },
    { tau::KC_EISU_TOGGLE,              tau::KC_EISU_TOGGLE_NAME                },
    { tau::KC_HANGUL,                   tau::KC_HANGUL_NAME                     },
    { tau::KC_HANGUL_START,             tau::KC_HANGUL_START_NAME               },
    { tau::KC_HANGUL_END,               tau::KC_HANGUL_END_NAME                 },
    { tau::KC_HANGUL_HANJA,             tau::KC_HANGUL_HANJA_NAME               },
    { tau::KC_HANGUL_JAMO,              tau::KC_HANGUL_JAMO_NAME                },
    { tau::KC_HANGUL_ROMAJA,            tau::KC_HANGUL_ROMAJA_NAME              },
    { tau::KC_HANGUL_JEONJA,            tau::KC_HANGUL_JEONJA_NAME              },
    { tau::KC_HANGUL_BANJA,             tau::KC_HANGUL_BANJA_NAME               },
    { tau::KC_HANGUL_PREHANJA,          tau::KC_HANGUL_PREHANJA_NAME            },
    { tau::KC_HANGUL_POSTHANJA,         tau::KC_HANGUL_POSTHANJA_NAME           },
    { tau::KC_HANGUL_SPECIAL,           tau::KC_HANGUL_SPECIAL_NAME             },
    { tau::KC_MODE_SWITCH,              tau::KC_MODE_SWITCH_NAME                },
    { tau::KC_SELECT,                   tau::KC_SELECT_NAME                     },
    { tau::KC_YES,                      tau::KC_YES_NAME                        },
    { tau::KC_NO,                       tau::KC_NO_NAME                         },
    { tau::KC_COPY,                     tau::KC_COPY_NAME                       },
    { tau::KC_CUT,                      tau::KC_CUT_NAME                        },
    { tau::KC_PASTE,                    tau::KC_PASTE_NAME                      },
    { tau::KC_BACK,                     tau::KC_BACK_NAME                       },
    { tau::KC_FORWARD,                  tau::KC_FORWARD_NAME                    },
    { tau::KC_STOP,                     tau::KC_STOP_NAME                       },
    { tau::KC_REFRESH,                  tau::KC_REFRESH_NAME                    },
    { tau::KC_VOLUME_UP,                tau::KC_VOLUME_UP_NAME                  },
    { tau::KC_VOLUME_DOWN,              tau::KC_VOLUME_DOWN_NAME                },
    { tau::KC_VOLUME_MUTE,              tau::KC_VOLUME_MUTE_NAME                },
    { tau::KC_BASS_BOOST,               tau::KC_BASS_BOOST_NAME                 },
    { tau::KC_BASS_UP,                  tau::KC_BASS_UP_NAME                    },
    { tau::KC_BASS_DOWN,                tau::KC_BASS_DOWN_NAME                  },
    { tau::KC_TREBLE_UP,                tau::KC_TREBLE_UP_NAME                  },
    { tau::KC_TREBLE_DOWN,              tau::KC_TREBLE_DOWN_NAME                },
    { tau::KC_MEDIA_PLAY,               tau::KC_MEDIA_PLAY_NAME                 },
    { tau::KC_MEDIA_PAUSE,              tau::KC_MEDIA_PAUSE_NAME                },
    { tau::KC_MEDIA_STOP,               tau::KC_MEDIA_STOP_NAME                 },
    { tau::KC_MEDIA_PREVIOUS,           tau::KC_MEDIA_PREVIOUS_NAME             },
    { tau::KC_MEDIA_NEXT,               tau::KC_MEDIA_NEXT_NAME                 },
    { tau::KC_MEDIA_RECORD,             tau::KC_MEDIA_RECORD_NAME               },
    { tau::KC_HOME_PAGE,                tau::KC_HOME_PAGE_NAME                  },
    { tau::KC_FAVORITES,                tau::KC_FAVORITES_NAME                  },
    { tau::KC_SEARCH,                   tau::KC_SEARCH_NAME                     },
    { tau::KC_STANDBY,                  tau::KC_STANDBY_NAME                    },
    { tau::KC_OPEN_URL,                 tau::KC_OPEN_URL_NAME                   },
    { tau::KC_LAUNCH_MAIL,              tau::KC_LAUNCH_MAIL_NAME                },
    { tau::KC_LAUNCH_MEDIA,             tau::KC_LAUNCH_MEDIA_NAME               },
    { tau::KC_LAUNCH0,                  tau::KC_LAUNCH0_NAME                    },
    { tau::KC_LAUNCH1,                  tau::KC_LAUNCH1_NAME                    },
    { tau::KC_LAUNCH2,                  tau::KC_LAUNCH2_NAME                    },
    { tau::KC_LAUNCH3,                  tau::KC_LAUNCH3_NAME                    },
    { tau::KC_LAUNCH4,                  tau::KC_LAUNCH4_NAME                    },
    { tau::KC_LAUNCH5,                  tau::KC_LAUNCH5_NAME                    },
    { tau::KC_LAUNCH6,                  tau::KC_LAUNCH6_NAME                    },
    { tau::KC_LAUNCH7,                  tau::KC_LAUNCH7_NAME                    },
    { tau::KC_LAUNCH8,                  tau::KC_LAUNCH8_NAME                    },
    { tau::KC_LAUNCH9,                  tau::KC_LAUNCH9_NAME                    },
    { tau::KC_LAUNCHA,                  tau::KC_LAUNCHA_NAME                    },
    { tau::KC_LAUNCHB,                  tau::KC_LAUNCHB_NAME                    },
    { tau::KC_LAUNCHC,                  tau::KC_LAUNCHC_NAME                    },
    { tau::KC_LAUNCHD,                  tau::KC_LAUNCHD_NAME                    },
    { tau::KC_LAUNCHE,                  tau::KC_LAUNCHE_NAME                    },
    { tau::KC_LAUNCHF,                  tau::KC_LAUNCHF_NAME                    },
    { tau::KC_EXECUTE,                  tau::KC_EXECUTE_NAME                    },
    { tau::KC_TERMINAL,                 tau::KC_TERMINAL_NAME                   },
    { tau::KC_POWER_OFF,                tau::KC_POWER_OFF_NAME                  },
    { tau::KC_WAKE_UP,                  tau::KC_WAKE_UP_NAME                    },
    { tau::KC_SAVE,                     tau::KC_SAVE_NAME                       },
    { tau::KC_SEND,                     tau::KC_SEND_NAME                       },
    { tau::KC_REPLY,                    tau::KC_REPLY_NAME                      },
    { tau::KC_ZOOM_IN,                  tau::KC_ZOOM_IN_NAME                    },
    { tau::KC_ZOOM_OUT,                 tau::KC_ZOOM_OUT_NAME                   },
    { tau::KC_SLEEP,                    tau::KC_SLEEP_NAME                      },
    { tau::KC_TERMINATE_SERVER,         tau::KC_TERMINATE_SERVER_NAME           },
    { tau::KC_CLOSE,                    tau::KC_CLOSE_NAME                      },
    { tau::KC_VIDEO,                    tau::KC_VIDEO_NAME                      },
    { tau::KC_MUSIC,                    tau::KC_MUSIC_NAME                      },
    { tau::KC_PHONE,                    tau::KC_PHONE_NAME                      },
    { tau::KC_RELOAD,                   tau::KC_RELOAD_NAME                     },
    { tau::KC_BATTERY,                  tau::KC_BATTERY_NAME                    },
    { tau::KC_BLUETOOTH,                tau::KC_BLUETOOTH_NAME                  },
    { tau::KC_WLAN,                     tau::KC_WLAN_NAME                       },
    { tau::KC_ISO_LOCK,                 tau::KC_ISO_LOCK_NAME                   },
    { tau::KC_LEVEL2_LATCH,             tau::KC_LEVEL2_LATCH_NAME               },
    { tau::KC_LEVEL3_SHIFT,             tau::KC_LEVEL3_SHIFT_NAME               },
    { tau::KC_LEVEL3_LATCH,             tau::KC_LEVEL3_LATCH_NAME               },
    { tau::KC_LEVEL3_LOCK,              tau::KC_LEVEL3_LOCK_NAME                },
    { tau::KC_LEVEL5_SHIFT,             tau::KC_LEVEL5_SHIFT_NAME               },
    { tau::KC_LEVEL5_LATCH,             tau::KC_LEVEL5_LATCH_NAME               },
    { tau::KC_LEVEL5_LOCK,              tau::KC_LEVEL5_LOCK_NAME                },
    { tau::KC_GROUP_SHIFT,              tau::KC_GROUP_SHIFT_NAME                },
    { tau::KC_GROUP_LATCH,              tau::KC_GROUP_LATCH_NAME                },
    { tau::KC_GROUP_LOCK,               tau::KC_GROUP_LOCK_NAME                 },
    { tau::KC_NEXT_GROUP,               tau::KC_NEXT_GROUP_NAME                 },
    { tau::KC_NEXT_GROUP_LOCK,          tau::KC_NEXT_GROUP_LOCK_NAME            },
    { tau::KC_PREV_GROUP,               tau::KC_PREV_GROUP_NAME                 },
    { tau::KC_PREV_GROUP_LOCK,          tau::KC_PREV_GROUP_LOCK_NAME            },
    { tau::KC_FIRST_GROUP,              tau::KC_FIRST_GROUP_NAME                },
    { tau::KC_FIRST_GROUP_LOCK,         tau::KC_FIRST_GROUP_LOCK_NAME           },
    { tau::KC_LAST_GROUP,               tau::KC_LAST_GROUP_NAME                 },
    { tau::KC_LAST_GROUP_LOCK,          tau::KC_LAST_GROUP_LOCK_NAME            },
    { tau::KC_LEFT_TAB,                 tau::KC_LEFT_TAB_NAME                   },
    { tau::KC_MOVE_LINE_UP,             tau::KC_MOVE_LINE_UP_NAME               },
    { tau::KC_MOVE_LINE_DOWN,           tau::KC_MOVE_LINE_DOWN_NAME             },
    { tau::KC_PARTIAL_LINE_UP,          tau::KC_PARTIAL_LINE_UP_NAME            },
    { tau::KC_PARTIAL_LINE_DOWN,        tau::KC_PARTIAL_LINE_DOWN_NAME          },
    { tau::KC_PARTIAL_SPACE_LEFT,       tau::KC_PARTIAL_SPACE_LEFT_NAME         },
    { tau::KC_PARTIAL_SPACE_RIGHT,      tau::KC_PARTIAL_SPACE_RIGHT_NAME        },
    { tau::KC_SET_MARGIN_LEFT,          tau::KC_SET_MARGIN_LEFT_NAME            },
    { tau::KC_SET_MARGIN_RIGHT,         tau::KC_SET_MARGIN_RIGHT_NAME           },
    { tau::KC_RELEASE_MARGIN_LEFT,      tau::KC_RELEASE_MARGIN_LEFT_NAME        },
    { tau::KC_RELEASE_MARGIN_RIGHT,     tau::KC_RELEASE_MARGIN_RIGHT_NAME       },
    { tau::KC_RELEASE_BOTH_MARGINS,     tau::KC_RELEASE_BOTH_MARGINS_NAME       },
    { tau::KC_FAST_CURSOR_LEFT,         tau::KC_FAST_CURSOR_LEFT_NAME           },
    { tau::KC_FAST_CURSOR_RIGHT,        tau::KC_FAST_CURSOR_RIGHT_NAME          },
    { tau::KC_FAST_CURSOR_UP,           tau::KC_FAST_CURSOR_UP_NAME             },
    { tau::KC_FAST_CURSOR_DOWN,         tau::KC_FAST_CURSOR_DOWN_NAME           },
    { tau::KC_CONTINUOUS_UNDERLINE,     tau::KC_CONTINUOUS_UNDERLINE_NAME       },
    { tau::KC_DISCONTINUOUS_UNDERLINE,  tau::KC_DISCONTINUOUS_UNDERLINE_NAME    },
    { tau::KC_EMPHASIZE,                tau::KC_EMPHASIZE_NAME                  },
    { tau::KC_CENTER_OBJECT,            tau::KC_CENTER_OBJECT_NAME              },
    { tau::KC_BRAILLE_DOT_1,            tau::KC_BRAILLE_DOT_1_NAME              },
    { tau::KC_BRAILLE_DOT_2,            tau::KC_BRAILLE_DOT_2_NAME              },
    { tau::KC_BRAILLE_DOT_3,            tau::KC_BRAILLE_DOT_3_NAME              },
    { tau::KC_BRAILLE_DOT_4,            tau::KC_BRAILLE_DOT_4_NAME              },
    { tau::KC_BRAILLE_DOT_5,            tau::KC_BRAILLE_DOT_5_NAME              },
    { tau::KC_BRAILLE_DOT_6,            tau::KC_BRAILLE_DOT_6_NAME              },
    { tau::KC_BRAILLE_DOT_7,            tau::KC_BRAILLE_DOT_7_NAME              },
    { tau::KC_BRAILLE_DOT_8,            tau::KC_BRAILLE_DOT_8_NAME              },
    { tau::KC_BRAILLE_DOT_9,            tau::KC_BRAILLE_DOT_9_NAME              },
    { tau::KC_BRAILLE_DOT_10,           tau::KC_BRAILLE_DOT_10_NAME             },
    { 0,                                nullptr                                 }
};

uint32_t keycode_from_keyname(const std::string & name) {
    static std::unordered_map<std::string, uint32_t> map;

    if (map.empty()) {
        for (unsigned i = 0; keydb_[i].name; ++i) {
            tau::ustring kname = tau::str_tolower(keydb_[i].name);
            map[kname] = keydb_[i].kc;
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
        for (unsigned i = 0; keydb_[i].name; ++i) {
            map[keydb_[i].kc] = keydb_[i].name;
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

const char * KM_NONE_NAME                           = "NONE";
const char * KM_SHIFT_NAME                          = "SHIFT";
const char * KM_CONTROL_NAME                        = "CONTROL";
const char * KM_ALT_NAME                            = "ALT";
const char * KM_META_NAME                           = "META";
const char * KM_WIN_NAME                            = "WIN";
const char * KM_SUPER_NAME                          = "SUPER";
const char * KM_MENU_NAME                           = "MENU";
const char * KM_HYPER_NAME                          = "HYPER";
const char * KM_GROUP_NAME                          = "GROUP";
const char * KM_NUMLOCK_NAME                        = "NUMLOCK";
const char * KM_SCROLL_NAME                         = "SCROLL";
const char * KM_CAPS_NAME                           = "CAPS";

const char * KC_NONE_NAME                           = "None";
const char * KC_BACKSPACE_NAME                      = "BackSpace";
const char * KC_TAB_NAME                            = "Tab";
const char * KC_LINEFEED_NAME                       = "LineFeed";
const char * KC_ENTER_NAME                          = "Enter";
const char * KC_RETURN_NAME                         = "Return";
const char * KC_ESCAPE_NAME                         = "Escape";
const char * KC_SPACE_NAME                          = "Space";
const char * KC_DELETE_NAME                         = "Delete";
const char * KC_F1_NAME                             = "F1";
const char * KC_F2_NAME                             = "F2";
const char * KC_F3_NAME                             = "F3";
const char * KC_F4_NAME                             = "F4";
const char * KC_F5_NAME                             = "F5";
const char * KC_F6_NAME                             = "F6";
const char * KC_F7_NAME                             = "F7";
const char * KC_F8_NAME                             = "F8";
const char * KC_F9_NAME                             = "F9";
const char * KC_F10_NAME                            = "F10";
const char * KC_F11_NAME                            = "F11";
const char * KC_F12_NAME                            = "F12";
const char * KC_F13_NAME                            = "F13";
const char * KC_F14_NAME                            = "F14";
const char * KC_F15_NAME                            = "F15";
const char * KC_F16_NAME                            = "F16";
const char * KC_F17_NAME                            = "F17";
const char * KC_F18_NAME                            = "F18";
const char * KC_F19_NAME                            = "F19";
const char * KC_F20_NAME                            = "F20";
const char * KC_F21_NAME                            = "F21";
const char * KC_F22_NAME                            = "F22";
const char * KC_F23_NAME                            = "F23";
const char * KC_F24_NAME                            = "F24";
const char * KC_F25_NAME                            = "F25";
const char * KC_F26_NAME                            = "F26";
const char * KC_F27_NAME                            = "F27";
const char * KC_F28_NAME                            = "F28";
const char * KC_F29_NAME                            = "F29";
const char * KC_F30_NAME                            = "F30";
const char * KC_F31_NAME                            = "F31";
const char * KC_F32_NAME                            = "F32";
const char * KC_F33_NAME                            = "F33";
const char * KC_F34_NAME                            = "F34";
const char * KC_F35_NAME                            = "F35";
const char * KC_LEFT_NAME                           = "Left";
const char * KC_RIGHT_NAME                          = "Right";
const char * KC_UP_NAME                             = "Up";
const char * KC_DOWN_NAME                           = "Down";
const char * KC_HOME_NAME                           = "Home";
const char * KC_END_NAME                            = "End";
const char * KC_PAGE_UP_NAME                        = "PageUp";
const char * KC_PAGE_DOWN_NAME                      = "PageDown";
const char * KC_SCROLL_UP_NAME                      = "ScrollUp";
const char * KC_SCROLL_DOWN_NAME                    = "ScrollDown";
const char * KC_INSERT_NAME                         = "Insert";
const char * KC_BEGIN_NAME                          = "Begin";
const char * KC_CANCEL_NAME                         = "Cancel";
const char * KC_BREAK_NAME                          = "Break";
const char * KC_CLEAR_NAME                          = "Clear";
const char * KC_PAUSE_NAME                          = "Pause";
const char * KC_PRINT_NAME                          = "Print";
const char * KC_SYSREQ_NAME                         = "SysReq";
const char * KC_HELP_NAME                           = "Help";
const char * KC_UNDO_NAME                           = "Undo";
const char * KC_REDO_NAME                           = "Redo";
const char * KC_FIND_NAME                           = "Find";
const char * KC_SELECT_NAME                         = "Select";
const char * KC_NO_NAME                             = "No";
const char * KC_YES_NAME                            = "Yes";
const char * KC_COPY_NAME                           = "Copy";
const char * KC_CUT_NAME                            = "Cut";
const char * KC_PASTE_NAME                          = "Paste";
const char * KC_LDIR_NAME                           = "DirectionL";
const char * KC_RDIR_NAME                           = "DirectionR";
const char * KC_CODE_INPUT_NAME                     = "CodeInput";
const char * KC_MULTI_NAME                          = "Multi";
const char * KC_SINGLE_CANDIDATE_NAME               = "SingleCandidate";
const char * KC_MULTIPLE_CANDIDATE_NAME             = "MultipleCandidate";
const char * KC_PREVIOUS_CANDIDATE_NAME             = "PreviousCandidate";
const char * KC_MODE_SWITCH_NAME                    = "ModeSwitch";
const char * KC_SEPARATOR_NAME                      = "Separator";
const char * KC_DECIMAL_NAME                        = "Decimal";
const char * KC_ISO_LOCK_NAME                       = "ISO_Lock";
const char * KC_LEVEL2_LATCH_NAME                   = "Level2_Latch";
const char * KC_LEVEL3_SHIFT_NAME                   = "Level3_Shift";
const char * KC_LEVEL3_LATCH_NAME                   = "Level3_Latch";
const char * KC_LEVEL3_LOCK_NAME                    = "Level3_Lock";
const char * KC_LEVEL5_SHIFT_NAME                   = "Level5_Shift";
const char * KC_LEVEL5_LATCH_NAME                   = "Level5_Latch";
const char * KC_LEVEL5_LOCK_NAME                    = "Level5_Lock";
const char * KC_GROUP_SHIFT_NAME                    = "GroupShift";
const char * KC_GROUP_LATCH_NAME                    = "GroupLatch";
const char * KC_GROUP_LOCK_NAME                     = "GroupLock";
const char * KC_NEXT_GROUP_NAME                     = "NextGroup";
const char * KC_NEXT_GROUP_LOCK_NAME                = "NextGroupLock";
const char * KC_PREV_GROUP_NAME                     = "PrevGroup";
const char * KC_PREV_GROUP_LOCK_NAME                = "PrevGroupLock";
const char * KC_FIRST_GROUP_NAME                    = "FirstGroup";
const char * KC_FIRST_GROUP_LOCK_NAME               = "FirstGroupLock";
const char * KC_LAST_GROUP_NAME                     = "LastGroup";
const char * KC_LAST_GROUP_LOCK_NAME                = "LastGroupLock";
const char * KC_LEFT_TAB_NAME                       = "LeftTab";
const char * KC_MOVE_LINE_UP_NAME                   = "MoveLineUp";
const char * KC_MOVE_LINE_DOWN_NAME                 = "MoveLineDown";
const char * KC_PARTIAL_SPACE_LEFT_NAME             = "PartialSpaceLeft";
const char * KC_PARTIAL_SPACE_RIGHT_NAME            = "PartialSpaceRight";
const char * KC_PARTIAL_LINE_UP_NAME                = "PartialLineUp";
const char * KC_PARTIAL_LINE_DOWN_NAME              = "PartialLineDown";
const char * KC_SET_MARGIN_LEFT_NAME                = "SetMarginLeft";
const char * KC_SET_MARGIN_RIGHT_NAME               = "SetMarginRight";
const char * KC_RELEASE_MARGIN_LEFT_NAME            = "ReleaseMarginLeft";
const char * KC_RELEASE_MARGIN_RIGHT_NAME           = "ReleaseMarginRight";
const char * KC_RELEASE_BOTH_MARGINS_NAME           = "ReleaseBothMargins";
const char * KC_FAST_CURSOR_LEFT_NAME               = "FastCursorLeft";
const char * KC_FAST_CURSOR_RIGHT_NAME              = "FastCursorRight";
const char * KC_FAST_CURSOR_UP_NAME                 = "FastCursorUp";
const char * KC_FAST_CURSOR_DOWN_NAME               = "FastCursorDown";
const char * KC_CONTINUOUS_UNDERLINE_NAME           = "ContinuousUnderline";
const char * KC_DISCONTINUOUS_UNDERLINE_NAME        = "DiscontinuousUnderline";
const char * KC_EMPHASIZE_NAME                      = "Emphasize";
const char * KC_CENTER_OBJECT_NAME                  = "CenterObject";
const char * KC_SEND_NAME                           = "Send";
const char * KC_REPLY_NAME                          = "Reply";
const char * KC_ZOOM_IN_NAME                        = "ZoomIn";
const char * KC_ZOOM_OUT_NAME                       = "ZoomOut";
const char * KC_FORWARD_NAME                        = "Forward";
const char * KC_BACK_NAME                           = "Back";
const char * KC_STOP_NAME                           = "Stop";
const char * KC_REFRESH_NAME                        = "Refresh";
const char * KC_HOME_PAGE_NAME                      = "HomePage";
const char * KC_FAVORITES_NAME                      = "Favorites";
const char * KC_SEARCH_NAME                         = "Search";
const char * KC_STANDBY_NAME                        = "Standby";
const char * KC_OPEN_URL_NAME                       = "OpenURL";
const char * KC_LAUNCH_MAIL_NAME                    = "LaunchMail";
const char * KC_LAUNCH_MEDIA_NAME                   = "LaunchMedia";
const char * KC_LAUNCH0_NAME                        = "Launch0";
const char * KC_LAUNCH1_NAME                        = "Launch1";
const char * KC_LAUNCH2_NAME                        = "Launch2";
const char * KC_LAUNCH3_NAME                        = "Launch3";
const char * KC_LAUNCH4_NAME                        = "Launch4";
const char * KC_LAUNCH5_NAME                        = "Launch5";
const char * KC_LAUNCH6_NAME                        = "Launch6";
const char * KC_LAUNCH7_NAME                        = "Launch7";
const char * KC_LAUNCH8_NAME                        = "Launch8";
const char * KC_LAUNCH9_NAME                        = "Launch9";
const char * KC_LAUNCHA_NAME                        = "LaunchA";
const char * KC_LAUNCHB_NAME                        = "LaunchB";
const char * KC_LAUNCHC_NAME                        = "LaunchC";
const char * KC_LAUNCHD_NAME                        = "LaunchD";
const char * KC_LAUNCHE_NAME                        = "LaunchE";
const char * KC_LAUNCHF_NAME                        = "LaunchF";
const char * KC_EXECUTE_NAME                        = "Execute";
const char * KC_TERMINAL_NAME                       = "Terminal";
const char * KC_POWER_OFF_NAME                      = "PowerOff";
const char * KC_SLEEP_NAME                          = "Sleep";
const char * KC_WAKE_UP_NAME                        = "WakeUp";
const char * KC_TERMINATE_SERVER_NAME               = "TerminateServer";
const char * KC_CLOSE_NAME                          = "Close";
const char * KC_PHONE_NAME                          = "Phone";
const char * KC_RELOAD_NAME                         = "Reload";
const char * KC_SAVE_NAME                           = "Save";
const char * KC_VIDEO_NAME                          = "Video";
const char * KC_MUSIC_NAME                          = "Music";
const char * KC_BATTERY_NAME                        = "Battery";
const char * KC_BLUETOOTH_NAME                      = "Bluetooth";
const char * KC_WLAN_NAME                           = "WLAN";
const char * KC_VOLUME_UP_NAME                      = "VolumeUp";
const char * KC_VOLUME_DOWN_NAME                    = "VolumeDown";
const char * KC_VOLUME_MUTE_NAME                    = "VolumeMute";
const char * KC_BASS_BOOST_NAME                     = "BassBoost";
const char * KC_BASS_UP_NAME                        = "BassUp";
const char * KC_BASS_DOWN_NAME                      = "BassDown";
const char * KC_TREBLE_UP_NAME                      = "TrebleUp";
const char * KC_TREBLE_DOWN_NAME                    = "TrebleDown";
const char * KC_MEDIA_PLAY_NAME                     = "MediaPlay";
const char * KC_MEDIA_PAUSE_NAME                    = "MediaPause";
const char * KC_MEDIA_STOP_NAME                     = "MediaStop";
const char * KC_MEDIA_PREVIOUS_NAME                 = "MediaPrevious";
const char * KC_MEDIA_NEXT_NAME                     = "MediaNext";
const char * KC_MEDIA_RECORD_NAME                   = "MediaRecord";
const char * KC_KANJI_NAME                          = "Kanji";
const char * KC_MUHENKAN_NAME                       = "Muhenkan";
const char * KC_HENKAN_NAME                         = "Henkan";
const char * KC_ROMAJI_NAME                         = "Romaji";
const char * KC_HIRAGANA_NAME                       = "Hiragana";
const char * KC_KATAKANA_NAME                       = "Katakana";
const char * KC_HIRAGANA_KATAKANA_NAME              = "HiraganaKatakana";
const char * KC_ZENKAKU_NAME                        = "Zenkaku";
const char * KC_HANKAKU_NAME                        = "Hankaku";
const char * KC_ZENKAKU_HANKAKU_NAME                = "ZenkakuHankaku";
const char * KC_TOUROKU_NAME                        = "Touroku";
const char * KC_MASSYO_NAME                         = "Massyo";
const char * KC_KANA_LOCK_NAME                      = "KanaLock";
const char * KC_KANA_SHIFT_NAME                     = "KanaShift";
const char * KC_EISU_SHIFT_NAME                     = "EisuShift";
const char * KC_EISU_TOGGLE_NAME                    = "EisuToggle";
const char * KC_HANGUL_NAME                         = "Hangul";
const char * KC_HANGUL_START_NAME                   = "HangulStart";
const char * KC_HANGUL_END_NAME                     = "HangulEnd";
const char * KC_HANGUL_HANJA_NAME                   = "HangulHanja";
const char * KC_HANGUL_JAMO_NAME                    = "HangulJamo";
const char * KC_HANGUL_ROMAJA_NAME                  = "HangulRomaja";
const char * KC_HANGUL_JEONJA_NAME                  = "HangulJeonja";
const char * KC_HANGUL_BANJA_NAME                   = "HangulBanja";
const char * KC_HANGUL_PREHANJA_NAME                = "HangulPrehanja";
const char * KC_HANGUL_POSTHANJA_NAME               = "HangulPosthanja";
const char * KC_HANGUL_SPECIAL_NAME                 = "HangulSpecial";
const char * KC_BRAILLE_DOT_1_NAME                  = "BrailleDot_1";
const char * KC_BRAILLE_DOT_2_NAME                  = "BrailleDot_2";
const char * KC_BRAILLE_DOT_3_NAME                  = "BrailleDot_3";
const char * KC_BRAILLE_DOT_4_NAME                  = "BrailleDot_4";
const char * KC_BRAILLE_DOT_5_NAME                  = "BrailleDot_5";
const char * KC_BRAILLE_DOT_6_NAME                  = "BrailleDot_6";
const char * KC_BRAILLE_DOT_7_NAME                  = "BrailleDot_7";
const char * KC_BRAILLE_DOT_8_NAME                  = "BrailleDot_8";
const char * KC_BRAILLE_DOT_9_NAME                  = "BrailleDot_9";
const char * KC_BRAILLE_DOT_10_NAME                 = "BrailleDot_10";
const char * KC_LSHIFT_NAME                         = "ShiftL";
const char * KC_RSHIFT_NAME                         = "ShiftR";
const char * KC_SHIFT_NAME                          = "Shift";
const char * KC_LCONTROL_NAME                       = "ControlL";
const char * KC_RCONTROL_NAME                       = "ControlR";
const char * KC_CONTROL_NAME                        = "Control";
const char * KC_LALT_NAME                           = "AltL";
const char * KC_RALT_NAME                           = "AltR";
const char * KC_ALT_NAME                            = "Alt";
const char * KC_LMETA_NAME                          = "MetaL";
const char * KC_RMETA_NAME                          = "MetaR";
const char * KC_META_NAME                           = "Meta";
const char * KC_LWIN_NAME                           = "WinL";
const char * KC_RWIN_NAME                           = "WinR";
const char * KC_WIN_NAME                            = "Win";
const char * KC_LSUPER_NAME                         = "SuperL";
const char * KC_RSUPER_NAME                         = "SuperR";
const char * KC_SUPER_NAME                          = "Super";
const char * KC_LMENU_NAME                          = "MenuL";
const char * KC_RMENU_NAME                          = "MenuR";
const char * KC_MENU_NAME                           = "Menu";
const char * KC_LHYPER_NAME                         = "HyperL";
const char * KC_RHYPER_NAME                         = "HyperR";
const char * KC_HYPER_NAME                          = "Hyper";
const char * KC_GROUP_NAME                          = "Group";
const char * KC_NUM_LOCK_NAME                       = "NumLock";
const char * KC_SCROLL_LOCK_NAME                    = "ScrollLock";
const char * KC_CAPS_LOCK_NAME                      = "CapsLock";
const char * KC_SHIFT_LOCK_NAME                     = "ShiftLock";

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
    static const std::forward_list<char32_t> mods = {
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
