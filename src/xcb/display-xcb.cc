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

#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/pixmap.hh>
#include <dialog-impl.hh>
#include <event-impl.hh>
#include <loop-impl.hh>
#include <popup-impl.hh>
#include <theme-impl.hh>
#include "cursor-xcb.hh"
#include "display-xcb.hh"
#include "font-xcb.hh"
#include "winface-xcb.hh"
#include <xcb/screensaver.h>
#include <xcb/xfixes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <climits>
#include <cstring>
#include <map>
#include <iostream>

namespace {

// The state field is a mask of the buttons held down during the event.
// It is a bitwise OR of any of the following (from the xcbbuttonmaskt
// and xcbmodmaskt enumerations):
// XCB_BUTTON_MASK_1
// XCB_BUTTON_MASK_2
// XCB_BUTTON_MASK_3
// XCB_BUTTON_MASK_4
// XCB_BUTTON_MASK_5
// XCB_MOD_MASK_SHIFT
// XCB_MOD_MASK_LOCK
// XCB_MOD_MASK_CONTROL
// XCB_MOD_MASK_1
// XCB_MOD_MASK_2
// XCB_MOD_MASK_3
// XCB_MOD_MASK_4
// XCB_MOD_MASK_5
static int mm_from_state(uint16_t state) {
    int mm = 0;

    if (state & XCB_BUTTON_MASK_1) mm |= tau::MM_LEFT;
    if (state & XCB_BUTTON_MASK_2) mm |= tau::MM_MIDDLE;
    if (state & XCB_BUTTON_MASK_3) mm |= tau::MM_RIGHT;
    if (state & XCB_MOD_MASK_SHIFT) mm |= tau::MM_SHIFT;
    if (state & XCB_MOD_MASK_CONTROL) mm |= tau::MM_CONTROL;

    return mm;
}

const std::map<uint32_t, char32_t> kc_translate_ = {
    { XKB_KEY_space,                            tau::KC_SPACE                       },
    { XKB_KEY_KP_Space,                         tau::KC_SPACE                       },
    { XKB_KEY_Tab,                              tau::KC_TAB                         },
    { XKB_KEY_KP_Tab,                           tau::KC_TAB                         },
    { XKB_KEY_KP_Enter,                         tau::KC_ENTER                       },
    { XKB_KEY_BackSpace,                        tau::KC_BACKSPACE                   },
    { XKB_KEY_Linefeed,                         tau::KC_LINEFEED                    },
    { XKB_KEY_Clear,                            tau::KC_CLEAR                       },
    { XKB_KEY_Return,                           tau::KC_ENTER                       },
    { XKB_KEY_Pause,                            tau::KC_PAUSE                       },
    { XKB_KEY_Scroll_Lock,                      tau::KC_SCROLL_LOCK                 },
    { XKB_KEY_Sys_Req,                          tau::KC_SYSREQ                      },
    { XKB_KEY_Escape,                           tau::KC_ESCAPE                      },
    { XKB_KEY_Delete,                           tau::KC_DELETE                      },
    { XKB_KEY_Insert,                           tau::KC_INSERT                      },
    { XKB_KEY_Home,                             tau::KC_HOME                        },
    { XKB_KEY_Left,                             tau::KC_LEFT                        },
    { XKB_KEY_Up,                               tau::KC_UP                          },
    { XKB_KEY_Right,                            tau::KC_RIGHT                       },
    { XKB_KEY_Down,                             tau::KC_DOWN                        },
    { XKB_KEY_Page_Up,                          tau::KC_PAGE_UP                     },
    { XKB_KEY_Page_Down,                        tau::KC_PAGE_DOWN                   },
    { XKB_KEY_XF86ScrollUp,                     tau::KC_SCROLL_UP                   },  // Scroll window/contents up
    { XKB_KEY_XF86ScrollDown,                   tau::KC_SCROLL_DOWN                 },  // Scrool window/contentd down
    { XKB_KEY_End,                              tau::KC_END                         },
    { XKB_KEY_Begin,                            tau::KC_BEGIN                       },
    { XKB_KEY_Select,                           tau::KC_SELECT                      },
    { XKB_KEY_Print,                            tau::KC_PRINT                       },
    { XKB_KEY_Execute,                          tau::KC_EXECUTE                     },
    { XKB_KEY_XF86Terminal,                     tau::KC_TERMINAL                    },  // Launch terminal emulator
    { XKB_KEY_Insert,                           tau::KC_INSERT                      },
    { XKB_KEY_Undo,                             tau::KC_UNDO                        },
    { XKB_KEY_Redo,                             tau::KC_REDO                        },
    { XKB_KEY_Menu,                             tau::KC_MENU                        },
    { XKB_KEY_Find,                             tau::KC_FIND                        },
    { XKB_KEY_Cancel,                           tau::KC_CANCEL                      },
    { XKB_KEY_XF86Clear,                        tau::KC_CLEAR                       },  // Clear window, screen
    { XKB_KEY_XF86Copy,                         tau::KC_COPY                        },  // Copy selection
    { XKB_KEY_XF86Cut,                          tau::KC_CUT                         },  // Cut selection
    { XKB_KEY_XF86Paste,                        tau::KC_PASTE                       },  // Paste
    { XKB_KEY_Help,                             tau::KC_HELP                        },
    { XKB_KEY_Break,                            tau::KC_BREAK                       },
    { XKB_KEY_Mode_switch,                      tau::KC_MODE_SWITCH                 },
    { XKB_KEY_Num_Lock,                         tau::KC_NUM_LOCK                    },
    { XKB_KEY_KP_Home,                          tau::KC_HOME                        },
    { XKB_KEY_KP_Left,                          tau::KC_LEFT                        },
    { XKB_KEY_KP_Up,                            tau::KC_UP                          },
    { XKB_KEY_KP_Right,                         tau::KC_RIGHT                       },
    { XKB_KEY_KP_Down,                          tau::KC_DOWN                        },
    { XKB_KEY_KP_Page_Up,                       tau::KC_PAGE_UP                     },
    { XKB_KEY_KP_Page_Down,                     tau::KC_PAGE_DOWN                   },
    { XKB_KEY_KP_End,                           tau::KC_END                         },
    { XKB_KEY_KP_Begin,                         tau::KC_BEGIN                       },
    { XKB_KEY_KP_Insert,                        tau::KC_INSERT                      },
    { XKB_KEY_KP_Delete,                        tau::KC_DELETE                      },
    { XKB_KEY_KP_Equal,                         U'='                                },
    { XKB_KEY_KP_Multiply,                      U'*'                                },
    { XKB_KEY_KP_Add,                           U'+'                                },
    { XKB_KEY_KP_Separator,                     tau::KC_SEPARATOR                   },
    { XKB_KEY_KP_Subtract,                      U'-'                                },
    { XKB_KEY_KP_Decimal,                       tau::KC_DECIMAL                     },
    { XKB_KEY_KP_Divide,                        U'/'                                },
    { XKB_KEY_KP_0,                             U'0'                                },
    { XKB_KEY_KP_1,                             U'1'                                },
    { XKB_KEY_KP_2,                             U'2'                                },
    { XKB_KEY_KP_3,                             U'3'                                },
    { XKB_KEY_KP_4,                             U'4'                                },
    { XKB_KEY_KP_5,                             U'5'                                },
    { XKB_KEY_KP_6,                             U'6'                                },
    { XKB_KEY_KP_7,                             U'7'                                },
    { XKB_KEY_KP_8,                             U'8'                                },
    { XKB_KEY_KP_9,                             U'9'                                },
    { XKB_KEY_KP_F1,                            tau::KC_F1                          },
    { XKB_KEY_KP_F2,                            tau::KC_F2                          },
    { XKB_KEY_KP_F3,                            tau::KC_F3                          },
    { XKB_KEY_KP_F4,                            tau::KC_F4                          },
    { XKB_KEY_F1,                               tau::KC_F1                          },
    { XKB_KEY_F2,                               tau::KC_F2                          },
    { XKB_KEY_F3,                               tau::KC_F3                          },
    { XKB_KEY_F4,                               tau::KC_F4                          },
    { XKB_KEY_F5,                               tau::KC_F5                          },
    { XKB_KEY_F6,                               tau::KC_F6                          },
    { XKB_KEY_F7,                               tau::KC_F7                          },
    { XKB_KEY_F8,                               tau::KC_F8                          },
    { XKB_KEY_F9,                               tau::KC_F9                          },
    { XKB_KEY_F10,                              tau::KC_F10                         },
    { XKB_KEY_F11,                              tau::KC_F11                         },
    { XKB_KEY_F12,                              tau::KC_F12                         },
    { XKB_KEY_F13,                              tau::KC_F13                         },
    { XKB_KEY_F14,                              tau::KC_F14                         },
    { XKB_KEY_F15,                              tau::KC_F15                         },
    { XKB_KEY_F16,                              tau::KC_F16                         },
    { XKB_KEY_F17,                              tau::KC_F17                         },
    { XKB_KEY_F18,                              tau::KC_F18                         },
    { XKB_KEY_F19,                              tau::KC_F19                         },
    { XKB_KEY_F20,                              tau::KC_F20                         },
    { XKB_KEY_F21,                              tau::KC_F21                         },
    { XKB_KEY_F22,                              tau::KC_F22                         },
    { XKB_KEY_F23,                              tau::KC_F23                         },
    { XKB_KEY_F24,                              tau::KC_F24                         },
    { XKB_KEY_F25,                              tau::KC_F25                         },
    { XKB_KEY_F26,                              tau::KC_F26                         },
    { XKB_KEY_F27,                              tau::KC_F27                         },
    { XKB_KEY_F28,                              tau::KC_F28                         },
    { XKB_KEY_F29,                              tau::KC_F29                         },
    { XKB_KEY_F30,                              tau::KC_F30                         },
    { XKB_KEY_F31,                              tau::KC_F31                         },
    { XKB_KEY_F32,                              tau::KC_F32                         },
    { XKB_KEY_F33,                              tau::KC_F33                         },
    { XKB_KEY_F34,                              tau::KC_F34                         },
    { XKB_KEY_F35,                              tau::KC_F35                         },
    { XKB_KEY_Shift_L,                          tau::KC_LSHIFT                      },
    { XKB_KEY_Shift_R,                          tau::KC_RSHIFT                      },
    { XKB_KEY_Control_L,                        tau::KC_LCONTROL                    },
    { XKB_KEY_Control_R,                        tau::KC_RCONTROL                    },
    { XKB_KEY_Caps_Lock,                        tau::KC_CAPS_LOCK                   },
    { XKB_KEY_Shift_Lock,                       tau::KC_SHIFT_LOCK                  },
    { XKB_KEY_Meta_L,                           tau::KC_LMETA                       },
    { XKB_KEY_Meta_R,                           tau::KC_RMETA                       },
    { XKB_KEY_Alt_L,                            tau::KC_LALT                        },
    { XKB_KEY_Alt_R,                            tau::KC_RALT                        },
    { XKB_KEY_Super_L,                          tau::KC_LSUPER                      },
    { XKB_KEY_Super_R,                          tau::KC_RSUPER                      },
    { XKB_KEY_Hyper_L,                          tau::KC_LHYPER                      },
    { XKB_KEY_Hyper_R,                          tau::KC_RHYPER                      },
    { XKB_KEY_Mode_switch,                      tau::KC_MODE_SWITCH                 },
    { XKB_KEY_ISO_Lock,                         tau::KC_ISO_LOCK                    },
    { XKB_KEY_ISO_Level2_Latch,                 tau::KC_LEVEL2_LATCH                },
    { XKB_KEY_ISO_Level3_Shift,                 tau::KC_LEVEL3_SHIFT                },
    { XKB_KEY_ISO_Level3_Latch,                 tau::KC_LEVEL3_LATCH                },
    { XKB_KEY_ISO_Level3_Lock,                  tau::KC_LEVEL3_LOCK                 },
    { XKB_KEY_ISO_Level5_Shift,                 tau::KC_LEVEL5_SHIFT                },
    { XKB_KEY_ISO_Level5_Latch,                 tau::KC_LEVEL5_LATCH                },
    { XKB_KEY_ISO_Level5_Lock,                  tau::KC_LEVEL5_LOCK                 },
    { XKB_KEY_ISO_Group_Shift,                  tau::KC_GROUP_SHIFT                 },
    { XKB_KEY_ISO_Group_Latch,                  tau::KC_GROUP_LATCH                 },
    { XKB_KEY_ISO_Group_Lock,                   tau::KC_GROUP_LOCK                  },
    { XKB_KEY_ISO_Next_Group,                   tau::KC_NEXT_GROUP                  },
    { XKB_KEY_ISO_Next_Group_Lock,              tau::KC_NEXT_GROUP_LOCK             },
    { XKB_KEY_ISO_Prev_Group,                   tau::KC_PREV_GROUP                  },
    { XKB_KEY_ISO_Prev_Group_Lock,              tau::KC_PREV_GROUP_LOCK             },
    { XKB_KEY_ISO_First_Group,                  tau::KC_FIRST_GROUP                 },
    { XKB_KEY_ISO_First_Group_Lock,             tau::KC_FIRST_GROUP_LOCK            },
    { XKB_KEY_ISO_Last_Group,                   tau::KC_LAST_GROUP                  },
    { XKB_KEY_ISO_Last_Group_Lock,              tau::KC_LAST_GROUP_LOCK             },
    { XKB_KEY_ISO_Left_Tab,                     tau::KC_LEFT_TAB                    },
    { XKB_KEY_ISO_Move_Line_Up,                 tau::KC_MOVE_LINE_UP                },
    { XKB_KEY_ISO_Move_Line_Down,               tau::KC_MOVE_LINE_DOWN              },
    { XKB_KEY_ISO_Partial_Line_Up,              tau::KC_PARTIAL_LINE_UP             },
    { XKB_KEY_ISO_Partial_Line_Down,            tau::KC_PARTIAL_LINE_DOWN           },
    { XKB_KEY_ISO_Partial_Space_Left,           tau::KC_PARTIAL_SPACE_LEFT          },
    { XKB_KEY_ISO_Partial_Space_Right,          tau::KC_PARTIAL_SPACE_RIGHT         },
    { XKB_KEY_ISO_Set_Margin_Left,              tau::KC_SET_MARGIN_LEFT             },
    { XKB_KEY_ISO_Set_Margin_Right,             tau::KC_SET_MARGIN_RIGHT            },
    { XKB_KEY_ISO_Release_Margin_Left,          tau::KC_RELEASE_MARGIN_LEFT         },
    { XKB_KEY_ISO_Release_Margin_Right,         tau::KC_RELEASE_MARGIN_RIGHT        },
    { XKB_KEY_ISO_Release_Both_Margins,         tau::KC_RELEASE_BOTH_MARGINS        },
    { XKB_KEY_ISO_Fast_Cursor_Left,             tau::KC_FAST_CURSOR_LEFT            },
    { XKB_KEY_ISO_Fast_Cursor_Right,            tau::KC_FAST_CURSOR_RIGHT           },
    { XKB_KEY_ISO_Fast_Cursor_Up,               tau::KC_FAST_CURSOR_UP              },
    { XKB_KEY_ISO_Fast_Cursor_Down,             tau::KC_FAST_CURSOR_DOWN            },
    { XKB_KEY_ISO_Continuous_Underline,         tau::KC_CONTINUOUS_UNDERLINE        },
    { XKB_KEY_ISO_Discontinuous_Underline,      tau::KC_DISCONTINUOUS_UNDERLINE     },
    { XKB_KEY_ISO_Emphasize,                    tau::KC_EMPHASIZE                   },
    { XKB_KEY_ISO_Center_Object,                tau::KC_CENTER_OBJECT               },
    { XKB_KEY_ISO_Enter,                        tau::KC_ENTER                       },
    { XKB_KEY_Terminate_Server,                 tau::KC_TERMINATE_SERVER            },
    { XKB_KEY_XF86Launch0,                      tau::KC_LAUNCH0                     },  // Launch Application
    { XKB_KEY_XF86Launch1,                      tau::KC_LAUNCH1                     },  // Launch Application
    { XKB_KEY_XF86Launch2,                      tau::KC_LAUNCH2                     },  // Launch Application
    { XKB_KEY_XF86Launch3,                      tau::KC_LAUNCH3                     },  // Launch Application
    { XKB_KEY_XF86Launch4,                      tau::KC_LAUNCH4                     },  // Launch Application
    { XKB_KEY_XF86Launch5,                      tau::KC_LAUNCH5                     },  // Launch Application
    { XKB_KEY_XF86Launch6,                      tau::KC_LAUNCH6                     },  // Launch Application
    { XKB_KEY_XF86Launch7,                      tau::KC_LAUNCH7                     },  // Launch Application
    { XKB_KEY_XF86Launch8,                      tau::KC_LAUNCH8                     },  // Launch Application
    { XKB_KEY_XF86Launch9,                      tau::KC_LAUNCH9                     },  // Launch Application
    { XKB_KEY_XF86LaunchA,                      tau::KC_LAUNCHA                     },  // Launch Application
    { XKB_KEY_XF86LaunchB,                      tau::KC_LAUNCHB                     },  // Launch Application
    { XKB_KEY_XF86LaunchC,                      tau::KC_LAUNCHC                     },  // Launch Application
    { XKB_KEY_XF86LaunchD,                      tau::KC_LAUNCHD                     },  // Launch Application
    { XKB_KEY_XF86LaunchE,                      tau::KC_LAUNCHE                     },  // Launch Application
    { XKB_KEY_XF86LaunchF,                      tau::KC_LAUNCHF                     },  // Launch Application
    { XKB_KEY_XF86Close,                        tau::KC_CLOSE                       },  // Close window
    { XKB_KEY_XF86Video,                        tau::KC_VIDEO                       },  // Launch video player
    { XKB_KEY_XF86Music,                        tau::KC_MUSIC                       },  // Launch music application
    { XKB_KEY_XF86Phone,                        tau::KC_PHONE                       },  // Launch phone; dial number
    { XKB_KEY_XF86Reload,                       tau::KC_RELOAD                      },  // Reload web page, file, etc.
    { XKB_KEY_XF86PowerOff,                     tau::KC_POWER_OFF                   },  // Power off system entirely
    { XKB_KEY_XF86Sleep,                        tau::KC_SLEEP                       },  // Put system to sleep
    { XKB_KEY_XF86WakeUp,                       tau::KC_WAKE_UP                     },  // Wake up system from sleep
    { XKB_KEY_XF86Save,                         tau::KC_SAVE                        },  // Save (file, document, state
    { XKB_KEY_XF86Send,                         tau::KC_SEND                        },  // Send mail, file, object
    { XKB_KEY_XF86Reply,                        tau::KC_REPLY                       },  // Reply e.g., mail
    { XKB_KEY_XF86ZoomIn,                       tau::KC_ZOOM_IN                     },  // Zoom in view, map, etc.
    { XKB_KEY_XF86ZoomOut,                      tau::KC_ZOOM_OUT                    },  // Zoom out view, map, etc.
    { XKB_KEY_XF86Battery,                      tau::KC_BATTERY                     },  // Display battery information
    { XKB_KEY_XF86Bluetooth,                    tau::KC_BLUETOOTH                   },  // Enable/disable Bluetooth
    { XKB_KEY_XF86WLAN,                         tau::KC_WLAN                        },  // Enable/disable WLAN
    { XKB_KEY_XF86AudioRaiseVolume,             tau::KC_VOLUME_UP                   },
    { XKB_KEY_XF86AudioLowerVolume,             tau::KC_VOLUME_DOWN                 },
    { XKB_KEY_XF86AudioMute,                    tau::KC_VOLUME_MUTE                 },
    { XKB_KEY_XF86AudioPlay,                    tau::KC_MEDIA_PLAY                  },
    { XKB_KEY_XF86AudioStop,                    tau::KC_MEDIA_STOP                  },
    { XKB_KEY_XF86AudioNext,                    tau::KC_MEDIA_NEXT                  },
    { XKB_KEY_XF86AudioPrev,                    tau::KC_MEDIA_PREVIOUS              },
    { XKB_KEY_SunAudioRaiseVolume,              tau::KC_VOLUME_UP                   },
    { XKB_KEY_SunAudioLowerVolume,              tau::KC_VOLUME_DOWN                 },
    { XKB_KEY_SunAudioMute,                     tau::KC_VOLUME_MUTE                 },
    { XKB_KEY_Multi_key,                        tau::KC_MULTI_KEY                   },
    { XKB_KEY_Codeinput,                        tau::KC_CODE_INPUT                  },
    { XKB_KEY_SingleCandidate,                  tau::KC_SINGLE_CANDIDATE            },
    { XKB_KEY_MultipleCandidate,                tau::KC_MULTIPLE_CANDIDATE          },
    { XKB_KEY_PreviousCandidate,                tau::KC_PREVIOUS_CANDIDATE          },
    { XKB_KEY_Kanji,                            tau::KC_KANJI                       },
    { XKB_KEY_Muhenkan,                         tau::KC_MUHENKAN                    },
    { XKB_KEY_Henkan,                           tau::KC_HENKAN                      },
    { XKB_KEY_Romaji,                           tau::KC_ROMAJI                      },
    { XKB_KEY_Hiragana,                         tau::KC_HIRAGANA                    },
    { XKB_KEY_Katakana,                         tau::KC_KATAKANA                    },
    { XKB_KEY_Hiragana_Katakana,                tau::KC_HIRAGANA_KATAKANA           },
    { XKB_KEY_Zenkaku,                          tau::KC_ZENKAKU                     },
    { XKB_KEY_Hankaku,                          tau::KC_HANKAKU                     },
    { XKB_KEY_Zenkaku_Hankaku,                  tau::KC_ZENKAKU_HANKAKU             },
    { XKB_KEY_Touroku,                          tau::KC_TOUROKU                     },
    { XKB_KEY_Massyo,                           tau::KC_MASSYO                      },
    { XKB_KEY_Kana_Lock,                        tau::KC_KANA_LOCK                   },
    { XKB_KEY_Kana_Shift,                       tau::KC_KANA_SHIFT                  },
    { XKB_KEY_Eisu_Shift,                       tau::KC_EISU_SHIFT                  },
    { XKB_KEY_Eisu_toggle,                      tau::KC_EISU_TOGGLE                 },
    { XKB_KEY_braille_blank,                    0x00002800                          }, // BRAILLE PATTERN BLANK
    { XKB_KEY_braille_dots_1,                   0x00002801                          }, // BRAILLE PATTERN DOTS-1
    { XKB_KEY_braille_dots_2,                   0x00002802                          }, // BRAILLE PATTERN DOTS-2
    { XKB_KEY_braille_dots_12,                  0x00002803                          }, // BRAILLE PATTERN DOTS-12
    { XKB_KEY_braille_dots_3,                   0x00002804                          }, // BRAILLE PATTERN DOTS-3
    { XKB_KEY_braille_dots_13,                  0x00002805                          }, // BRAILLE PATTERN DOTS-13
    { XKB_KEY_braille_dots_23,                  0x00002806                          }, // BRAILLE PATTERN DOTS-23
    { XKB_KEY_braille_dots_123,                 0x00002807                          }, // BRAILLE PATTERN DOTS-123
    { XKB_KEY_braille_dots_4,                   0x00002808                          }, // BRAILLE PATTERN DOTS-4
    { XKB_KEY_braille_dots_14,                  0x00002809                          }, // BRAILLE PATTERN DOTS-14
    { XKB_KEY_braille_dots_24,                  0x0000280A                          }, // BRAILLE PATTERN DOTS-24
    { XKB_KEY_braille_dots_124,                 0x0000280B                          }, // BRAILLE PATTERN DOTS-124
    { XKB_KEY_braille_dots_34,                  0x0000280C                          }, // BRAILLE PATTERN DOTS-34
    { XKB_KEY_braille_dots_134,                 0x0000280D                          }, // BRAILLE PATTERN DOTS-134
    { XKB_KEY_braille_dots_234,                 0x0000280E                          }, // BRAILLE PATTERN DOTS-234
    { XKB_KEY_braille_dots_1234,                0x0000280F                          }, // BRAILLE PATTERN DOTS-1234
    { XKB_KEY_braille_dots_5,                   0x00002810                          }, // BRAILLE PATTERN DOTS-5
    { XKB_KEY_braille_dots_15,                  0x00002811                          }, // BRAILLE PATTERN DOTS-15
    { XKB_KEY_braille_dots_25,                  0x00002812                          }, // BRAILLE PATTERN DOTS-25
    { XKB_KEY_braille_dots_125,                 0x00002813                          }, // BRAILLE PATTERN DOTS-125
    { XKB_KEY_braille_dots_35,                  0x00002814                          }, // BRAILLE PATTERN DOTS-35
    { XKB_KEY_braille_dots_135,                 0x00002815                          }, // BRAILLE PATTERN DOTS-135
    { XKB_KEY_braille_dots_235,                 0x00002816                          }, // BRAILLE PATTERN DOTS-235
    { XKB_KEY_braille_dots_1235,                0x00002817                          }, // BRAILLE PATTERN DOTS-1235
    { XKB_KEY_braille_dots_45,                  0x00002818                          }, // BRAILLE PATTERN DOTS-45
    { XKB_KEY_braille_dots_145,                 0x00002819                          }, // BRAILLE PATTERN DOTS-145
    { XKB_KEY_braille_dots_245,                 0x0000281A                          }, // BRAILLE PATTERN DOTS-245
    { XKB_KEY_braille_dots_1245,                0x0000281B                          }, // BRAILLE PATTERN DOTS-1245
    { XKB_KEY_braille_dots_345,                 0x0000281C                          }, // BRAILLE PATTERN DOTS-345
    { XKB_KEY_braille_dots_1345,                0x0000281D                          }, // BRAILLE PATTERN DOTS-1345
    { XKB_KEY_braille_dots_2345,                0x0000281E                          }, // BRAILLE PATTERN DOTS-2345
    { XKB_KEY_braille_dots_12345,               0x0000281F                          }, // BRAILLE PATTERN DOTS-12345
    { XKB_KEY_braille_dots_6,                   0x00002820                          }, // BRAILLE PATTERN DOTS-6
    { XKB_KEY_braille_dots_16,                  0x00002821                          }, // BRAILLE PATTERN DOTS-16
    { XKB_KEY_braille_dots_26,                  0x00002822                          }, // BRAILLE PATTERN DOTS-26
    { XKB_KEY_braille_dots_126,                 0x00002823                          }, // BRAILLE PATTERN DOTS-126
    { XKB_KEY_braille_dots_36,                  0x00002824                          }, // BRAILLE PATTERN DOTS-36
    { XKB_KEY_braille_dots_136,                 0x00002825                          }, // BRAILLE PATTERN DOTS-136
    { XKB_KEY_braille_dots_236,                 0x00002826                          }, // BRAILLE PATTERN DOTS-236
    { XKB_KEY_braille_dots_1236,                0x00002827                          }, // BRAILLE PATTERN DOTS-1236
    { XKB_KEY_braille_dots_46,                  0x00002828                          }, // BRAILLE PATTERN DOTS-46
    { XKB_KEY_braille_dots_146,                 0x00002829                          }, // BRAILLE PATTERN DOTS-146
    { XKB_KEY_braille_dots_246,                 0x0000282A                          }, // BRAILLE PATTERN DOTS-246
    { XKB_KEY_braille_dots_1246,                0x0000282B                          }, // BRAILLE PATTERN DOTS-1246
    { XKB_KEY_braille_dots_346,                 0x0000282C                          }, // BRAILLE PATTERN DOTS-346
    { XKB_KEY_braille_dots_1346,                0x0000282D                          }, // BRAILLE PATTERN DOTS-1346
    { XKB_KEY_braille_dots_2346,                0x0000282E                          }, // BRAILLE PATTERN DOTS-2346
    { XKB_KEY_braille_dots_12346,               0x0000282F                          }, // BRAILLE PATTERN DOTS-12346
    { XKB_KEY_braille_dots_56,                  0x00002830                          }, // BRAILLE PATTERN DOTS-56
    { XKB_KEY_braille_dots_156,                 0x00002831                          }, // BRAILLE PATTERN DOTS-156
    { XKB_KEY_braille_dots_256,                 0x00002832                          }, // BRAILLE PATTERN DOTS-256
    { XKB_KEY_braille_dots_1256,                0x00002833                          }, // BRAILLE PATTERN DOTS-1256
    { XKB_KEY_braille_dots_356,                 0x00002834                          }, // BRAILLE PATTERN DOTS-356
    { XKB_KEY_braille_dots_1356,                0x00002835                          }, // BRAILLE PATTERN DOTS-1356
    { XKB_KEY_braille_dots_2356,                0x00002836                          }, // BRAILLE PATTERN DOTS-2356
    { XKB_KEY_braille_dots_12356,               0x00002837                          }, // BRAILLE PATTERN DOTS-12356
    { XKB_KEY_braille_dots_456,                 0x00002838                          }, // BRAILLE PATTERN DOTS-456
    { XKB_KEY_braille_dots_1456,                0x00002839                          }, // BRAILLE PATTERN DOTS-1456
    { XKB_KEY_braille_dots_2456,                0x0000283A                          }, // BRAILLE PATTERN DOTS-2456
    { XKB_KEY_braille_dots_12456,               0x0000283B                          }, // BRAILLE PATTERN DOTS-12456
    { XKB_KEY_braille_dots_3456,                0x0000283C                          }, // BRAILLE PATTERN DOTS-3456
    { XKB_KEY_braille_dots_13456,               0x0000283D                          }, // BRAILLE PATTERN DOTS-13456
    { XKB_KEY_braille_dots_23456,               0x0000283E                          }, // BRAILLE PATTERN DOTS-23456
    { XKB_KEY_braille_dots_123456,              0x0000283F                          }, // BRAILLE PATTERN DOTS-123456
    { XKB_KEY_braille_dots_7,                   0x00002840                          }, // BRAILLE PATTERN DOTS-7
    { XKB_KEY_braille_dots_17,                  0x00002841                          }, // BRAILLE PATTERN DOTS-17
    { XKB_KEY_braille_dots_27,                  0x00002842                          }, // BRAILLE PATTERN DOTS-27
    { XKB_KEY_braille_dots_127,                 0x00002843                          }, // BRAILLE PATTERN DOTS-127
    { XKB_KEY_braille_dots_37,                  0x00002844                          }, // BRAILLE PATTERN DOTS-37
    { XKB_KEY_braille_dots_137,                 0x00002845                          }, // BRAILLE PATTERN DOTS-137
    { XKB_KEY_braille_dots_237,                 0x00002846                          }, // BRAILLE PATTERN DOTS-237
    { XKB_KEY_braille_dots_1237,                0x00002847                          }, // BRAILLE PATTERN DOTS-1237
    { XKB_KEY_braille_dots_47,                  0x00002848                          }, // BRAILLE PATTERN DOTS-47
    { XKB_KEY_braille_dots_147,                 0x00002849                          }, // BRAILLE PATTERN DOTS-147
    { XKB_KEY_braille_dots_247,                 0x0000284A                          }, // BRAILLE PATTERN DOTS-247
    { XKB_KEY_braille_dots_1247,                0x0000284B                          }, // BRAILLE PATTERN DOTS-1247
    { XKB_KEY_braille_dots_347,                 0x0000284C                          }, // BRAILLE PATTERN DOTS-347
    { XKB_KEY_braille_dots_1347,                0x0000284D                          }, // BRAILLE PATTERN DOTS-1347
    { XKB_KEY_braille_dots_2347,                0x0000284E                          }, // BRAILLE PATTERN DOTS-2347
    { XKB_KEY_braille_dots_12347,               0x0000284F                          }, // BRAILLE PATTERN DOTS-12347
    { XKB_KEY_braille_dots_57,                  0x00002850                          }, // BRAILLE PATTERN DOTS-57
    { XKB_KEY_braille_dots_157,                 0x00002851                          }, // BRAILLE PATTERN DOTS-157
    { XKB_KEY_braille_dots_257,                 0x00002852                          }, // BRAILLE PATTERN DOTS-257
    { XKB_KEY_braille_dots_1257,                0x00002853                          }, // BRAILLE PATTERN DOTS-1257
    { XKB_KEY_braille_dots_357,                 0x00002854                          }, // BRAILLE PATTERN DOTS-357
    { XKB_KEY_braille_dots_1357,                0x00002855                          }, // BRAILLE PATTERN DOTS-1357
    { XKB_KEY_braille_dots_2357,                0x00002856                          }, // BRAILLE PATTERN DOTS-2357
    { XKB_KEY_braille_dots_12357,               0x00002857                          }, // BRAILLE PATTERN DOTS-12357
    { XKB_KEY_braille_dots_457,                 0x00002858                          }, // BRAILLE PATTERN DOTS-457
    { XKB_KEY_braille_dots_1457,                0x00002859                          }, // BRAILLE PATTERN DOTS-1457
    { XKB_KEY_braille_dots_2457,                0x0000285A                          }, // BRAILLE PATTERN DOTS-2457
    { XKB_KEY_braille_dots_12457,               0x0000285B                          }, // BRAILLE PATTERN DOTS-12457
    { XKB_KEY_braille_dots_3457,                0x0000285C                          }, // BRAILLE PATTERN DOTS-3457
    { XKB_KEY_braille_dots_13457,               0x0000285D                          }, // BRAILLE PATTERN DOTS-13457
    { XKB_KEY_braille_dots_23457,               0x0000285E                          }, // BRAILLE PATTERN DOTS-23457
    { XKB_KEY_braille_dots_123457,              0x0000285F                          }, // BRAILLE PATTERN DOTS-123457
    { XKB_KEY_braille_dots_67,                  0x00002860                          }, // BRAILLE PATTERN DOTS-67
    { XKB_KEY_braille_dots_167,                 0x00002861                          }, // BRAILLE PATTERN DOTS-167
    { XKB_KEY_braille_dots_267,                 0x00002862                          }, // BRAILLE PATTERN DOTS-267
    { XKB_KEY_braille_dots_1267,                0x00002863                          }, // BRAILLE PATTERN DOTS-1267
    { XKB_KEY_braille_dots_367,                 0x00002864                          }, // BRAILLE PATTERN DOTS-367
    { XKB_KEY_braille_dots_1367,                0x00002865                          }, // BRAILLE PATTERN DOTS-1367
    { XKB_KEY_braille_dots_2367,                0x00002866                          }, // BRAILLE PATTERN DOTS-2367
    { XKB_KEY_braille_dots_12367,               0x00002867                          }, // BRAILLE PATTERN DOTS-12367
    { XKB_KEY_braille_dots_467,                 0x00002868                          }, // BRAILLE PATTERN DOTS-467
    { XKB_KEY_braille_dots_1467,                0x00002869                          }, // BRAILLE PATTERN DOTS-1467
    { XKB_KEY_braille_dots_2467,                0x0000286A                          }, // BRAILLE PATTERN DOTS-2467
    { XKB_KEY_braille_dots_12467,               0x0000286B                          }, // BRAILLE PATTERN DOTS-12467
    { XKB_KEY_braille_dots_3467,                0x0000286C                          }, // BRAILLE PATTERN DOTS-3467
    { XKB_KEY_braille_dots_13467,               0x0000286D                          }, // BRAILLE PATTERN DOTS-13467
    { XKB_KEY_braille_dots_23467,               0x0000286E                          }, // BRAILLE PATTERN DOTS-23467
    { XKB_KEY_braille_dots_123467,              0x0000286F                          }, // BRAILLE PATTERN DOTS-123467
    { XKB_KEY_braille_dots_567,                 0x00002870                          }, // BRAILLE PATTERN DOTS-567
    { XKB_KEY_braille_dots_1567,                0x00002871                          }, // BRAILLE PATTERN DOTS-1567
    { XKB_KEY_braille_dots_2567,                0x00002872                          }, // BRAILLE PATTERN DOTS-2567
    { XKB_KEY_braille_dots_12567,               0x00002873                          }, // BRAILLE PATTERN DOTS-12567
    { XKB_KEY_braille_dots_3567,                0x00002874                          }, // BRAILLE PATTERN DOTS-3567
    { XKB_KEY_braille_dots_13567,               0x00002875                          }, // BRAILLE PATTERN DOTS-13567
    { XKB_KEY_braille_dots_23567,               0x00002876                          }, // BRAILLE PATTERN DOTS-23567
    { XKB_KEY_braille_dots_123567,              0x00002877                          }, // BRAILLE PATTERN DOTS-123567
    { XKB_KEY_braille_dots_4567,                0x00002878                          }, // BRAILLE PATTERN DOTS-4567
    { XKB_KEY_braille_dots_14567,               0x00002879                          }, // BRAILLE PATTERN DOTS-14567
    { XKB_KEY_braille_dots_24567,               0x0000287A                          }, // BRAILLE PATTERN DOTS-24567
    { XKB_KEY_braille_dots_124567,              0x0000287B                          }, // BRAILLE PATTERN DOTS-124567
    { XKB_KEY_braille_dots_34567,               0x0000287C                          }, // BRAILLE PATTERN DOTS-34567
    { XKB_KEY_braille_dots_134567,              0x0000287D                          }, // BRAILLE PATTERN DOTS-134567
    { XKB_KEY_braille_dots_234567,              0x0000287E                          }, // BRAILLE PATTERN DOTS-234567
    { XKB_KEY_braille_dots_1234567,             0x0000287F                          }, // BRAILLE PATTERN DOTS-1234567
    { XKB_KEY_braille_dots_8,                   0x00002880                          }, // BRAILLE PATTERN DOTS-8
    { XKB_KEY_braille_dots_18,                  0x00002881                          }, // BRAILLE PATTERN DOTS-18
    { XKB_KEY_braille_dots_28,                  0x00002882                          }, // BRAILLE PATTERN DOTS-28
    { XKB_KEY_braille_dots_128,                 0x00002883                          }, // BRAILLE PATTERN DOTS-128
    { XKB_KEY_braille_dots_38,                  0x00002884                          }, // BRAILLE PATTERN DOTS-38
    { XKB_KEY_braille_dots_138,                 0x00002885                          }, // BRAILLE PATTERN DOTS-138
    { XKB_KEY_braille_dots_238,                 0x00002886                          }, // BRAILLE PATTERN DOTS-238
    { XKB_KEY_braille_dots_1238,                0x00002887                          }, // BRAILLE PATTERN DOTS-1238
    { XKB_KEY_braille_dots_48,                  0x00002888                          }, // BRAILLE PATTERN DOTS-48
    { XKB_KEY_braille_dots_148,                 0x00002889                          }, // BRAILLE PATTERN DOTS-148
    { XKB_KEY_braille_dots_248,                 0x0000288A                          }, // BRAILLE PATTERN DOTS-248
    { XKB_KEY_braille_dots_1248,                0x0000288B                          }, // BRAILLE PATTERN DOTS-1248
    { XKB_KEY_braille_dots_348,                 0x0000288C                          }, // BRAILLE PATTERN DOTS-348
    { XKB_KEY_braille_dots_1348,                0x0000288D                          }, // BRAILLE PATTERN DOTS-1348
    { XKB_KEY_braille_dots_2348,                0x0000288E                          }, // BRAILLE PATTERN DOTS-2348
    { XKB_KEY_braille_dots_12348,               0x0000288F                          }, // BRAILLE PATTERN DOTS-12348
    { XKB_KEY_braille_dots_58,                  0x00002890                          }, // BRAILLE PATTERN DOTS-58
    { XKB_KEY_braille_dots_158,                 0x00002891                          }, // BRAILLE PATTERN DOTS-158
    { XKB_KEY_braille_dots_258,                 0x00002892                          }, // BRAILLE PATTERN DOTS-258
    { XKB_KEY_braille_dots_1258,                0x00002893                          }, // BRAILLE PATTERN DOTS-1258
    { XKB_KEY_braille_dots_358,                 0x00002894                          }, // BRAILLE PATTERN DOTS-358
    { XKB_KEY_braille_dots_1358,                0x00002895                          }, // BRAILLE PATTERN DOTS-1358
    { XKB_KEY_braille_dots_2358,                0x00002896                          }, // BRAILLE PATTERN DOTS-2358
    { XKB_KEY_braille_dots_12358,               0x00002897                          }, // BRAILLE PATTERN DOTS-12358
    { XKB_KEY_braille_dots_458,                 0x00002898                          }, // BRAILLE PATTERN DOTS-458
    { XKB_KEY_braille_dots_1458,                0x00002899                          }, // BRAILLE PATTERN DOTS-1458
    { XKB_KEY_braille_dots_2458,                0x0000289A                          }, // BRAILLE PATTERN DOTS-2458
    { XKB_KEY_braille_dots_12458,               0x0000289B                          }, // BRAILLE PATTERN DOTS-12458
    { XKB_KEY_braille_dots_3458,                0x0000289C                          }, // BRAILLE PATTERN DOTS-3458
    { XKB_KEY_braille_dots_13458,               0x0000289D                          }, // BRAILLE PATTERN DOTS-13458
    { XKB_KEY_braille_dots_23458,               0x0000289E                          }, // BRAILLE PATTERN DOTS-23458
    { XKB_KEY_braille_dots_123458,              0x0000289F                          }, // BRAILLE PATTERN DOTS-123458
    { XKB_KEY_braille_dots_68,                  0x000028A0                          }, // BRAILLE PATTERN DOTS-68
    { XKB_KEY_braille_dots_168,                 0x000028A1                          }, // BRAILLE PATTERN DOTS-168
    { XKB_KEY_braille_dots_268,                 0x000028A2                          }, // BRAILLE PATTERN DOTS-268
    { XKB_KEY_braille_dots_1268,                0x000028A3                          }, // BRAILLE PATTERN DOTS-1268
    { XKB_KEY_braille_dots_368,                 0x000028A4                          }, // BRAILLE PATTERN DOTS-368
    { XKB_KEY_braille_dots_1368,                0x000028A5                          }, // BRAILLE PATTERN DOTS-1368
    { XKB_KEY_braille_dots_2368,                0x000028A6                          }, // BRAILLE PATTERN DOTS-2368
    { XKB_KEY_braille_dots_12368,               0x000028A7                          }, // BRAILLE PATTERN DOTS-12368
    { XKB_KEY_braille_dots_468,                 0x000028A8                          }, // BRAILLE PATTERN DOTS-468
    { XKB_KEY_braille_dots_1468,                0x000028A9                          }, // BRAILLE PATTERN DOTS-1468
    { XKB_KEY_braille_dots_2468,                0x000028AA                          }, // BRAILLE PATTERN DOTS-2468
    { XKB_KEY_braille_dots_12468,               0x000028AB                          }, // BRAILLE PATTERN DOTS-12468
    { XKB_KEY_braille_dots_3468,                0x000028AC                          }, // BRAILLE PATTERN DOTS-3468
    { XKB_KEY_braille_dots_13468,               0x000028AD                          }, // BRAILLE PATTERN DOTS-13468
    { XKB_KEY_braille_dots_23468,               0x000028AE                          }, // BRAILLE PATTERN DOTS-23468
    { XKB_KEY_braille_dots_123468,              0x000028AF                          }, // BRAILLE PATTERN DOTS-123468
    { XKB_KEY_braille_dots_568,                 0x000028B0                          }, // BRAILLE PATTERN DOTS-568
    { XKB_KEY_braille_dots_1568,                0x000028B1                          }, // BRAILLE PATTERN DOTS-1568
    { XKB_KEY_braille_dots_2568,                0x000028B2                          }, // BRAILLE PATTERN DOTS-2568
    { XKB_KEY_braille_dots_12568,               0x000028B3                          }, // BRAILLE PATTERN DOTS-12568
    { XKB_KEY_braille_dots_3568,                0x000028B4                          }, // BRAILLE PATTERN DOTS-3568
    { XKB_KEY_braille_dots_13568,               0x000028B5                          }, // BRAILLE PATTERN DOTS-13568
    { XKB_KEY_braille_dots_23568,               0x000028B6                          }, // BRAILLE PATTERN DOTS-23568
    { XKB_KEY_braille_dots_123568,              0x000028B7                          }, // BRAILLE PATTERN DOTS-123568
    { XKB_KEY_braille_dots_4568,                0x000028B8                          }, // BRAILLE PATTERN DOTS-4568
    { XKB_KEY_braille_dots_14568,               0x000028B9                          }, // BRAILLE PATTERN DOTS-14568
    { XKB_KEY_braille_dots_24568,               0x000028BA                          }, // BRAILLE PATTERN DOTS-24568
    { XKB_KEY_braille_dots_124568,              0x000028BB                          }, // BRAILLE PATTERN DOTS-124568
    { XKB_KEY_braille_dots_34568,               0x000028BC                          }, // BRAILLE PATTERN DOTS-34568
    { XKB_KEY_braille_dots_134568,              0x000028BD                          }, // BRAILLE PATTERN DOTS-134568
    { XKB_KEY_braille_dots_234568,              0x000028BE                          }, // BRAILLE PATTERN DOTS-234568
    { XKB_KEY_braille_dots_1234568,             0x000028BF                          }, // BRAILLE PATTERN DOTS-1234568
    { XKB_KEY_braille_dots_78,                  0x000028C0                          }, // BRAILLE PATTERN DOTS-78
    { XKB_KEY_braille_dots_178,                 0x000028C1                          }, // BRAILLE PATTERN DOTS-178
    { XKB_KEY_braille_dots_278,                 0x000028C2                          }, // BRAILLE PATTERN DOTS-278
    { XKB_KEY_braille_dots_1278,                0x000028C3                          }, // BRAILLE PATTERN DOTS-1278
    { XKB_KEY_braille_dots_378,                 0x000028C4                          }, // BRAILLE PATTERN DOTS-378
    { XKB_KEY_braille_dots_1378,                0x000028C5                          }, // BRAILLE PATTERN DOTS-1378
    { XKB_KEY_braille_dots_2378,                0x000028C6                          }, // BRAILLE PATTERN DOTS-2378
    { XKB_KEY_braille_dots_12378,               0x000028C7                          }, // BRAILLE PATTERN DOTS-12378
    { XKB_KEY_braille_dots_478,                 0x000028C8                          }, // BRAILLE PATTERN DOTS-478
    { XKB_KEY_braille_dots_1478,                0x000028C9                          }, // BRAILLE PATTERN DOTS-1478
    { XKB_KEY_braille_dots_2478,                0x000028CA                          }, // BRAILLE PATTERN DOTS-2478
    { XKB_KEY_braille_dots_12478,               0x000028CB                          }, // BRAILLE PATTERN DOTS-12478
    { XKB_KEY_braille_dots_3478,                0x000028CC                          }, // BRAILLE PATTERN DOTS-3478
    { XKB_KEY_braille_dots_13478,               0x000028CD                          }, // BRAILLE PATTERN DOTS-13478
    { XKB_KEY_braille_dots_23478,               0x000028CE                          }, // BRAILLE PATTERN DOTS-23478
    { XKB_KEY_braille_dots_123478,              0x000028CF                          }, // BRAILLE PATTERN DOTS-123478
    { XKB_KEY_braille_dots_578,                 0x000028D0                          }, // BRAILLE PATTERN DOTS-578
    { XKB_KEY_braille_dots_1578,                0x000028D1                          }, // BRAILLE PATTERN DOTS-1578
    { XKB_KEY_braille_dots_2578,                0x000028D2                          }, // BRAILLE PATTERN DOTS-2578
    { XKB_KEY_braille_dots_12578,               0x000028D3                          }, // BRAILLE PATTERN DOTS-12578
    { XKB_KEY_braille_dots_3578,                0x000028D4                          }, // BRAILLE PATTERN DOTS-3578
    { XKB_KEY_braille_dots_13578,               0x000028D5                          }, // BRAILLE PATTERN DOTS-13578
    { XKB_KEY_braille_dots_23578,               0x000028D6                          }, // BRAILLE PATTERN DOTS-23578
    { XKB_KEY_braille_dots_123578,              0x000028D7                          }, // BRAILLE PATTERN DOTS-123578
    { XKB_KEY_braille_dots_4578,                0x000028D8                          }, // BRAILLE PATTERN DOTS-4578
    { XKB_KEY_braille_dots_14578,               0x000028D9                          }, // BRAILLE PATTERN DOTS-14578
    { XKB_KEY_braille_dots_24578,               0x000028DA                          }, // BRAILLE PATTERN DOTS-24578
    { XKB_KEY_braille_dots_124578,              0x000028DB                          }, // BRAILLE PATTERN DOTS-124578
    { XKB_KEY_braille_dots_34578,               0x000028DC                          }, // BRAILLE PATTERN DOTS-34578
    { XKB_KEY_braille_dots_134578,              0x000028DD                          }, // BRAILLE PATTERN DOTS-134578
    { XKB_KEY_braille_dots_234578,              0x000028DE                          }, // BRAILLE PATTERN DOTS-234578
    { XKB_KEY_braille_dots_1234578,             0x000028DF                          }, // BRAILLE PATTERN DOTS-1234578
    { XKB_KEY_braille_dots_678,                 0x000028E0                          }, // BRAILLE PATTERN DOTS-678
    { XKB_KEY_braille_dots_1678,                0x000028E1                          }, // BRAILLE PATTERN DOTS-1678
    { XKB_KEY_braille_dots_2678,                0x000028E2                          }, // BRAILLE PATTERN DOTS-2678
    { XKB_KEY_braille_dots_12678,               0x000028E3                          }, // BRAILLE PATTERN DOTS-12678
    { XKB_KEY_braille_dots_3678,                0x000028E4                          }, // BRAILLE PATTERN DOTS-3678
    { XKB_KEY_braille_dots_13678,               0x000028E5                          }, // BRAILLE PATTERN DOTS-13678
    { XKB_KEY_braille_dots_23678,               0x000028E6                          }, // BRAILLE PATTERN DOTS-23678
    { XKB_KEY_braille_dots_123678,              0x000028E7                          }, // BRAILLE PATTERN DOTS-123678
    { XKB_KEY_braille_dots_4678,                0x000028E8                          }, // BRAILLE PATTERN DOTS-4678
    { XKB_KEY_braille_dots_14678,               0x000028E9                          }, // BRAILLE PATTERN DOTS-14678
    { XKB_KEY_braille_dots_24678,               0x000028EA                          }, // BRAILLE PATTERN DOTS-24678
    { XKB_KEY_braille_dots_124678,              0x000028EB                          }, // BRAILLE PATTERN DOTS-124678
    { XKB_KEY_braille_dots_34678,               0x000028EC                          }, // BRAILLE PATTERN DOTS-34678
    { XKB_KEY_braille_dots_134678,              0x000028ED                          }, // BRAILLE PATTERN DOTS-134678
    { XKB_KEY_braille_dots_234678,              0x000028EE                          }, // BRAILLE PATTERN DOTS-234678
    { XKB_KEY_braille_dots_1234678,             0x000028EF                          }, // BRAILLE PATTERN DOTS-1234678
    { XKB_KEY_braille_dots_5678,                0x000028F0                          }, // BRAILLE PATTERN DOTS-5678
    { XKB_KEY_braille_dots_15678,               0x000028F1                          }, // BRAILLE PATTERN DOTS-15678
    { XKB_KEY_braille_dots_25678,               0x000028F2                          }, // BRAILLE PATTERN DOTS-25678
    { XKB_KEY_braille_dots_125678,              0x000028F3                          }, // BRAILLE PATTERN DOTS-125678
    { XKB_KEY_braille_dots_35678,               0x000028F4                          }, // BRAILLE PATTERN DOTS-35678
    { XKB_KEY_braille_dots_135678,              0x000028F5                          }, // BRAILLE PATTERN DOTS-135678
    { XKB_KEY_braille_dots_235678,              0x000028F6                          }, // BRAILLE PATTERN DOTS-235678
    { XKB_KEY_braille_dots_1235678,             0x000028F7                          }, // BRAILLE PATTERN DOTS-1235678
    { XKB_KEY_braille_dots_45678,               0x000028F8                          }, // BRAILLE PATTERN DOTS-45678
    { XKB_KEY_braille_dots_145678,              0x000028F9                          }, // BRAILLE PATTERN DOTS-145678
    { XKB_KEY_braille_dots_245678,              0x000028FA                          }, // BRAILLE PATTERN DOTS-245678
    { XKB_KEY_braille_dots_1245678,             0x000028FB                          }, // BRAILLE PATTERN DOTS-1245678
    { XKB_KEY_braille_dots_345678,              0x000028FC                          }, // BRAILLE PATTERN DOTS-345678
    { XKB_KEY_braille_dots_1345678,             0x000028FD                          }, // BRAILLE PATTERN DOTS-1345678
    { XKB_KEY_braille_dots_2345678,             0x000028FE                          }, // BRAILLE PATTERN DOTS-2345678
    { XKB_KEY_braille_dots_12345678,            0x000028FF                          }  // BRAILLE PATTERN DOTS-12345678
};

} // anonymous namespace

namespace tau {

class Toplevel_xcb: public Toplevel_impl {
public:

    Toplevel_xcb(Winface_xcb_ptr winface, const Rect & ubounds):
        Toplevel_impl(winface, ubounds)
    {
    }
};

class Popup_xcb: public Popup_impl {
public:

    Popup_xcb(Winface_xcb_ptr winface, Window_ptr wpp, const Point & upos, Gravity gravity):
        Popup_impl(winface, upos, wpp, gravity)
    {
    }
};

class Dialog_xcb: public Dialog_impl {
public:

    Dialog_xcb(Winface_xcb_ptr winface, Window_impl * wipp, const Rect & ubounds):
        Dialog_impl(winface, wipp, ubounds)
    {
    }
};

void Display_xcb::open(const ustring & args) {
    init_connection();

    if (scr_) {
        size_px_.update(scr_->width_in_pixels, scr_->height_in_pixels);
        size_mm_.update(scr_->width_in_millimeters, scr_->height_in_millimeters);
        double xdpi = 0.0, ydpi = 0.0;

        if (0 != size_mm_.width()) {
            xdpi = 25.4*size_px_.width();
            xdpi /= size_mm_.width();
        }

        if (0 != size_mm_.height()) {
            ydpi = 25.4*size_px_.height();
            ydpi /= size_mm_.height();
        }

        dpi_ = std::max(1, int(std::min(xdpi, ydpi)));
    }

    init_xrender();
    init_xkb();
    init_xsync();
    init_xfixes();
    init_whidden();

    utf8_string_atom_ = atom("UTF8_STRING");
    targets_atom_ = atom("TARGETS");
    clipboard_atom_ = atom("CLIPBOARD");
    abcd_atom_ = atom("_ABCD");

    loop()->signal_quit().connect(fun(this, &Display_xcb::on_loop_quit));
    xcb_event_ = loop()->create_event(fun(this, &Display_xcb::on_xcb_event));
    Theme_impl::root()->take_cursor_lookup_slot(fun(this, &Display_xcb::lookup_cursor));
    xcb_thr_ = std::thread([this] { this->xcb_thread(); });
}

Display_xcb::~Display_xcb() {
    if (xkbcontext_) {
        xkb_context_unref(xkbcontext_);
        xkbcontext_ = nullptr;
    }

    if (xkbkeymap_) {
        xkb_keymap_unref(xkbkeymap_);
        xkbkeymap_ = nullptr;
    }

    if (xkbstate_) {
        xkb_state_unref(xkbstate_);
        xkbstate_ = nullptr;
    }

    if (cx_) {
        if (XCB_NONE != whidden_) {
            xcb_destroy_window(cx_, whidden_);
            whidden_ = XCB_NONE;
        }

        for (auto & p: solid_fills_) {
            xcb_render_free_picture(cx_, p.second);
        }

        xcb_disconnect(cx_);
        cx_ = nullptr;
    }

    atoms_.clear();
    ratoms_.clear();
}

void Display_xcb::init_connection() {
    int nscreen;
    cx_ = xcb_connect(nullptr, &nscreen);
    if (!cx_) { throw graphics_error("Display_xcb: xcb_connect() returned NULL"); }

    // Get first screen.
    auto sc = xcb_setup_roots_iterator(xcb_get_setup(cx_));
    if (!sc.data) { throw graphics_error("Display_xcb: screen not found"); }
    scr_ = sc.data;

    // Find visual for our screen.
    for (auto depths = xcb_screen_allowed_depths_iterator(scr_); depths.rem; xcb_depth_next (&depths)) {
        for (auto visuals = xcb_depth_visuals_iterator(depths.data); visuals.rem; xcb_visualtype_next(&visuals)) {
            if (scr_->root_visual == visuals.data->visual_id) {
                visualid_ = visuals.data->visual_id;
                break;
            }
        }
    }

    if (0 == visualid_) {
        throw graphics_error("Display_xcb: failed to init visual");
    }
}

void Display_xcb::init_xfixes() {
    xcb_xfixes_query_version_cookie_t ck = xcb_xfixes_query_version(cx_, 255, 255);
    xcb_generic_error_t * e = nullptr;

    if (xcb_xfixes_query_version_reply_t * reply = xcb_xfixes_query_version_reply(cx_, ck, &e)) {
        xfixes_version_ = reply->major_version;
        xfixes_version_ <<= 8;
        xfixes_version_ += reply->minor_version;
        std::free(reply);
    }

    if (e) { std::free(e); }
}

void Display_xcb::init_xsync() {
    xcb_sync_initialize_cookie_t ck = xcb_sync_initialize(cx_, 0, 0);
    xcb_generic_error_t * e = nullptr;

    if (xcb_sync_initialize_reply_t * reply = xcb_sync_initialize_reply(cx_, ck, &e)) {
        xsync_version_ = reply->major_version;
        xsync_version_ <<= 8;
        xsync_version_ += reply->minor_version;
        std::free(reply);
    }

    if (e) { std::free(e); }
}

void Display_xcb::init_xkb() {
    int result = xkb_x11_setup_xkb_extension(cx_,
                                             XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
                                             XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
                                             nullptr, nullptr, nullptr, nullptr);

    if (1 != result) {
        throw graphics_error("Display_xcb: failed to setup xkb extension");
    }

    int32_t kbd_id = xkb_x11_get_core_keyboard_device_id(cx_);
    if (-1 == kbd_id) {
        throw graphics_error("Display_xcb: failed to obtain core keyboard device");
    }

    xkbcontext_ = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!xkbcontext_) {
        throw graphics_error("Display_xcb: failed to create xkb context");
    }

    xkbkeymap_ = xkb_x11_keymap_new_from_device(xkbcontext_, cx_, kbd_id, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if (!xkbkeymap_) {
        throw graphics_error("Display_xcb: failed to create xkb keymap");
    }

    xkbstate_ = xkb_state_new(xkbkeymap_);
    if (!xkbstate_) {
        throw graphics_error("Display_xcb: failed to create xkb state");
    }
}

void Display_xcb::init_xrender() {
    // Query XRender presence.
    auto version_ck = xcb_render_query_version(cx_, XCB_RENDER_MAJOR_VERSION, XCB_RENDER_MINOR_VERSION);
    auto formats_ck = xcb_render_query_pict_formats(cx_);
    auto * version = xcb_render_query_version_reply(cx_, version_ck, nullptr);
    auto * formats = xcb_render_query_pict_formats_reply(cx_, formats_ck, nullptr);

    if (!version || !formats) {
        if (formats) { free(formats); }
        if (version) { free(version); }
        throw graphics_error("Display_xcb: XRender not found");
    }

    // Build visual -> pict format id map.
    for (auto screens = xcb_render_query_pict_formats_screens_iterator(formats); screens.rem; xcb_render_pictscreen_next(&screens)) {
        for (auto depths = xcb_render_pictscreen_depths_iterator(screens.data); depths.rem; xcb_render_pictdepth_next(&depths)) {
            for (auto visuals = xcb_render_pictdepth_visuals_iterator(depths.data); visuals.rem; xcb_render_pictvisual_next(&visuals)) {
                visual_formats_[visuals.data->visual] = visuals.data->format;
            }
        }
    }

    // Build pict format id -> pict format description map.
    for (auto iter = xcb_render_query_pict_formats_formats_iterator(formats); iter.rem; xcb_render_pictforminfo_next(&iter)) {
        xcb_render_pictforminfo_t * info = iter.data;
        Pict_format pf;
        pf.depth = info->depth;
        pf.red_shift = info->direct.red_shift;
        pf.red_mask = info->direct.red_mask;
        pf.green_shift = info->direct.green_shift;
        pf.green_mask = info->direct.green_mask;
        pf.blue_shift = info->direct.blue_shift;
        pf.blue_mask = info->direct.blue_mask;
        pf.alpha_shift = info->direct.alpha_shift;
        pf.alpha_mask = info->direct.alpha_mask;
        pict_formats_[info->id] = pf;
    }

    // Build depth -> pict format id map for fast access.
    for (auto iter: pict_formats_) {
        const Pict_format & pf = iter.second;

        if (1 == pf.depth) {
            if (depth_formats_.end() == depth_formats_.find(1)) {
                depth_formats_[1] = iter.first;
            }
        }

        else if (8 == pf.depth) {
            if (depth_formats_.end() == depth_formats_.find(8)) {
                depth_formats_[8] = iter.first;
            }
        }

        else if (24 == pf.depth) {
            if (16 == pf.red_shift && 8 == pf.green_shift && 0 == pf.blue_shift) {
                if (depth_formats_.end() == depth_formats_.find(24)) {
                    depth_formats_[24] = iter.first;
                }
            }
        }
        else if (32 == pf.depth) {
            if (24 == pf.alpha_shift && 16 == pf.red_shift && 8 == pf.green_shift && 0 == pf.blue_shift) {
                if (depth_formats_.end() == depth_formats_.find(32)) {
                    depth_formats_[32] = iter.first;
                }
            }
        }
    }

    // Check pict format existance for depths 1, 8, 24 and 32.
    if (depth_formats_.end() == depth_formats_.find(1)) {
        throw graphics_error("Display_xcb: missing 1 bit depth format");
    }

    if (depth_formats_.end() == depth_formats_.find(8)) {
        throw graphics_error("Display_xcb: missing 8 bit depth format");
    }

    if (depth_formats_.end() == depth_formats_.find(24)) {
        throw graphics_error("Display_xcb: missing 24 bit depth format");
    }

    if (depth_formats_.end() == depth_formats_.find(32)) {
        throw graphics_error("Display_xcb: missing 32 bit depth format");
    }

    free(formats);
    free(version);
}

// Overrides Display_impl.
void Display_xcb::allow_screensaver() {
    Display_impl::allow_screensaver();
    if (screensaver_allowed()) { xcb_screensaver_suspend(cx_, 0); }
}

// Overrides Display_impl.
void Display_xcb::disallow_screensaver() {
    Display_impl::disallow_screensaver();
    xcb_screensaver_suspend(cx_, 1);
}

// Overrides pure Display_impl.
int Display_xcb::depth() const {
    return scr_ ? scr_->root_depth : 0;
}

Cursor_ptr Display_xcb::lookup_cursor(const ustring & name) {
    xcb_cursor_context_t * ctx;

    if (xcb_cursor_context_new(cx_, scr_, &ctx) >= 0) {
        xcb_cursor_t cid = xcb_cursor_load_cursor(ctx, name.c_str());
        xcb_cursor_context_free(ctx);
        if (XCB_NONE != cid) { return std::make_shared<Cursor_xcb>(this, cid); }
    }

    return nullptr;
}

xcb_visualid_t Display_xcb::visualid() const {
    return visualid_;
}

xcb_render_pictformat_t Display_xcb::pictformat(unsigned depth) {
    // First, try to find format in the depth->format map.
    auto i = depth_formats_.find(depth);
    if (depth_formats_.end() != i) { return i->second; }

    // Else try to find it among all available formats.
    for (auto j: pict_formats_) {
        if (depth == j.second.depth) {
            return j.first;
        }
    }

    return 0;
}

xcb_render_pictformat_t Display_xcb::pictformat() {
    auto iter = visual_formats_.find(visualid_);
    return iter != visual_formats_.end() ? iter->second : 0;
}

xcb_atom_t Display_xcb::atom(const std::string & name) {
    xcb_atom_t result = XCB_ATOM_NONE;

    if (!name.empty() && cx_) {
        auto iter = atoms_.find(name);
        if (iter != atoms_.end()) { return iter->second; }

        xcb_intern_atom_cookie_t ck = xcb_intern_atom(cx_, 0, name.size(), name.c_str());
        xcb_intern_atom_reply_t * reply = xcb_intern_atom_reply(cx_, ck, nullptr);
        if (!reply) { throw graphics_error("Display_xcb: failed to create atom "+name); }
        result = reply->atom;
        atoms_[name] = result;
        ratoms_[result] = name;
        free(reply);
    }

    return result;
}

std::string Display_xcb::ratom(xcb_atom_t atom) {
    if (XCB_ATOM_NONE != atom) {
        auto iter = ratoms_.find(atom);
        if (iter != ratoms_.end()) { return iter->second; }

        if (cx_) {
            auto ck = xcb_get_atom_name(cx_, atom);
            xcb_get_atom_name_reply_t * reply = xcb_get_atom_name_reply(cx_, ck, nullptr);

            if (!reply) {
                std::cerr << "Display_xcb: xcb_get_atom_name(" << atom << ") failed" << std::endl;
            }

            else if (char * s = xcb_get_atom_name_name(reply)) {
                std::string name(s, reply->name_len);

                if (!name.empty()) {
                    atoms_[name] = atom;
                    ratoms_[atom] = name;
                }

                free(reply);
                return name;
            }
        }
    }

    return std::string();
}

void Display_xcb::on_loop_quit() {
    int fd = xcb_get_file_descriptor(cx_);
    shutdown(fd, SHUT_RDWR);
    Timeval ts = Timeval::future(2000000);

    while (Timeval::now() < ts) {
        if (!xcb_thread_running_) {
            break;
        }
    }

    if (xcb_thread_running_) {
        std::cerr << "!! Display_xcb: force killing xcb thread" << std::endl;
        pthread_cancel(xcb_thr_.native_handle());
        xcb_thread_running_ = false;
    }

    xcb_event_.reset();
    xcb_thr_.join();
    done();
}

void Display_xcb::xcb_thread() {
    xcb_thread_running_ = true;
    xcb_generic_event_t * event;

    while (loop()->alive() && cx_) {
        if (xcb_connection_has_error(cx_)) {
            std::cerr << "** Display_xcb: xcb_thread() quits due to connection error" << std::endl;
            break;
        }

        event = xcb_wait_for_event(cx_);
        if (!event) { break; }

        {
            std::lock_guard<std::mutex> lk(xcb_mx_);
            xcb_events_.push_back(event);
        }

        xcb_event_->emit();
    }

    xcb_thread_running_ = false;
}

void Display_xcb::on_xcb_event() {
    xcb_generic_event_t * event = nullptr;

    for (;;) {
        {
            std::lock_guard<std::mutex> lk(xcb_mx_);

            if (!xcb_events_.empty()) {
                event = xcb_events_.front();
                xcb_events_.pop_front();
            }
        }

        if (!event) { break; }
        handle_xcb_event(event);
        free(event);
        event = nullptr;
    }
}

void Display_xcb::handle_xcb_event(xcb_generic_event_t * event) {
    uint8_t response = 0x7f & event->response_type;

    switch (response) {
        // 0 and 1 response types reserved for errors and replies.
        case 0:
        case 1:
            break;

        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
            handle_kbd(XCB_KEY_PRESS == response, reinterpret_cast<xcb_key_press_event_t *>(event));
            break;

        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
            handle_button(XCB_BUTTON_PRESS == response, reinterpret_cast<xcb_button_press_event_t *>(event));
            break;

            // Mouse motion.
        case XCB_MOTION_NOTIFY:
        {
            auto motion = reinterpret_cast<xcb_motion_notify_event_t *>(event);

            if (auto wf = find(motion->event)) {
                Point pt(motion->event_x, motion->event_y);
                Window_impl * wii = wf->self();

                if (modal_window_) {
                    if (mouse_grabber_ != wii) {
                        pt = wii->to_screen(pt)-modal_window_->to_screen();
                    }

                    wii = modal_window_;
                }

                else if (mouse_grabber_ && mouse_grabber_ != wii) {
                    pt = wii->to_screen(pt)-mouse_grabber_->to_screen();
                    wii = mouse_grabber_;
                }

                set_mouse_owner(wii, pt);
                wii->signal_mouse_motion()(mm_from_state(motion->state), pt);
            }
        }
            break;

        case XCB_ENTER_NOTIFY:
        {
            auto enter = reinterpret_cast<xcb_enter_notify_event_t *>(event);

            if (auto wf = find(enter->event)) {
                if (!mouse_grabber_) {
                    if (!modal_window_ || wf->self() == modal_window_) {
                        const Point pt(enter->event_x, enter->event_y);
                        set_mouse_owner(wf->self(), pt);
                    }
                }
            }
        }
            break;

        case XCB_LEAVE_NOTIFY:
        {
            auto leave = reinterpret_cast<xcb_enter_notify_event_t *>(event);

            if (auto wf = find(leave->event)) {
                if (!mouse_grabber_) {
                    if (!modal_window_ || wf->self() == modal_window_) {
                        if (wf->self() == mouse_owner_) { reset_mouse_owner(); }
                    }
                }
            }
        }
            break;

        case XCB_FOCUS_IN:
            if (auto wf = find(reinterpret_cast<xcb_focus_in_event_t *>(event)->event)) { focus_window(wf->self()); }
            break;

        case XCB_FOCUS_OUT:
            if (auto wf = find(reinterpret_cast<xcb_focus_out_event_t *>(event)->event)) { unfocus_window(wf->self()); }
            break;

        case XCB_MAP_NOTIFY:
        {
            auto map = reinterpret_cast<xcb_map_notify_event_t *>(event);
            if (auto wf = find(map->event)) { wf->handle_map(map); }
        }
            break;

        case XCB_UNMAP_NOTIFY:
        {
            auto unmap = reinterpret_cast<xcb_unmap_notify_event_t *>(event);
            if (auto wf = find(unmap->event)) { wf->handle_unmap(unmap); }
            break;
        }

        case XCB_KEYMAP_NOTIFY:
            break;

        case XCB_EXPOSE:
        {
            auto expose = reinterpret_cast<xcb_expose_event_t *>(event);
            if (auto wf = find(expose->window)) { wf->handle_expose(expose); }
        }
            break;

        case XCB_CONFIGURE_NOTIFY:
        {
            auto configure = reinterpret_cast<xcb_configure_notify_event_t *>(event);
            if (auto wf = find(configure->window)) { wf->handle_configure(configure); }
        }
            break;

        case XCB_DESTROY_NOTIFY:
        {
            auto destroy = reinterpret_cast<xcb_destroy_notify_event_t *>(event);
            if (auto wf = find(destroy->event)) { wf->self()->close(); }
        }
            break;

        case XCB_PROPERTY_NOTIFY:
        {
            auto prop = reinterpret_cast<xcb_property_notify_event_t *>(event);
            if (auto wf = find(prop->window)) { wf->handle_property(prop); }
        }
            break;

        case XCB_REPARENT_NOTIFY:
            //handle_reparent(reinterpret_cast<xcb_reparent_notify_event_t *>(event));
            break;

            //  xcb_visibility_notify_event_t:
            //  uint8_t      response_type;
            //  uint8_t      pad0;
            //  uint16_t     sequence;
            //  xcb_window_t window;
            //  uint8_t      state;
            //  uint8_t      pad1[3];
            //
            //  States:
            //  XCB_VISIBILITY_UNOBSCURED = 0,
            //  XCB_VISIBILITY_PARTIALLY_OBSCURED = 1,
            //  XCB_VISIBILITY_FULLY_OBSCURED = 2
        case XCB_VISIBILITY_NOTIFY:
            break;

        case XCB_CLIENT_MESSAGE:
            handle_client(reinterpret_cast<xcb_client_message_event_t *>(event));
            break;

        case XCB_SELECTION_NOTIFY:
            handle_selection_notify(reinterpret_cast<xcb_selection_notify_event_t *>(event));
            break;

        case XCB_SELECTION_REQUEST:
            handle_selection_request(reinterpret_cast<xcb_selection_request_event_t *>(event));
            break;

        case XCB_SELECTION_CLEAR:
            handle_selection_clear(reinterpret_cast<xcb_selection_clear_event_t *>(event));
            break;

        default:
            std::cerr << "** Display_xcb: unhandled response_type " << uint16_t(response) << std::endl;
    }
}

static uint32_t extract_modifiers(xcb_key_press_event_t * event) {
    uint32_t mods = 0;

    if (event->state & XCB_MOD_MASK_SHIFT) { mods |= KM_SHIFT; }
    if (event->state & XCB_MOD_MASK_CONTROL) { mods |= KM_CONTROL; }
    if (event->state & XCB_MOD_MASK_1) { mods |= KM_ALT; }

    return mods;
}

void Display_xcb::handle_kbd(bool press, xcb_key_press_event_t * event) {
    xkb_keycode_t key = event->detail;
    uint32_t km = extract_modifiers(event);
    uint32_t xkeycode = 0;
    const xkb_keysym_t * syms;
    if (0 != xkb_state_key_get_syms(xkbstate_, key, &syms)) { xkeycode = *syms; }
    uint32_t uni = xkb_state_key_get_utf32(xkbstate_, key);
    Window_impl * wii = modal_window_ ? modal_window_ : focused_;

    if (wii) {
        uint32_t kc = 0;

        if (uni < 0x000020 || uni == 0x007f || uni > 0x10ffff) {
            auto iter = kc_translate_.find(xkeycode);
            if (iter != kc_translate_.end()) { kc = iter->second; }
        }

        else {
            kc = uni;
            km &= ~KM_SHIFT;
        }

        if (0 != kc) {
            if (KC_F1 == kc && 0 == km) {
                if (press) {
                    if (auto tpl = dynamic_cast<Toplevel_impl *>(wii)) {
                        if (!tpl->signal_help()()) {
                            tpl->handle_key_down(kc, km);
                        }
                    }
                }
            }

            else if (KC_F10 == kc && 0 == km) {
                if (press) {
                    if (auto tpl = dynamic_cast<Toplevel_impl *>(wii)) {
                        if (!tpl->signal_menu()()) {
                            tpl->handle_key_down(kc, km);
                        }
                    }
                }
            }
        }

        else if ((KM_CONTROL & km) && xkeycode >= 0x20 && xkeycode <= 0x7f) {
            if (xkeycode >= 'a' && xkeycode <= 'z') {
                kc = xkeycode-0x20;
            }

            else {
                kc = xkeycode;
            }
        }

        if (0 != kc) {
            if (press) {
                if (!wii->handle_accel(kc, km)) {
                    wii->handle_key_down(kc, km);
                }
            }

            else {
                wii->handle_key_up(kc, km);
            }
        }

        else {
            std::cerr << "!! Display_xcb: unhandled key code " << std::hex << xkeycode << ':' << km << std::dec << std::endl;
        }
    }

    xkb_state_update_key(xkbstate_, key, press ? XKB_KEY_DOWN: XKB_KEY_UP);
}

void Display_xcb::handle_button(bool press, xcb_button_press_event_t * event) {
    if (auto wf = find(event->event)) {
        Window_impl * wii = wf->self();
        Point pt(event->event_x, event->event_y);
        unsigned button = event->detail;

        if (modal_window_) {
            if (mouse_grabber_ != wii) { pt = wii->to_screen(pt)-modal_window_->to_screen(); }
            wii = modal_window_;
        }

        else if (mouse_grabber_ && mouse_grabber_ != wii) {
            pt = wii->to_screen(pt)-mouse_grabber_->to_screen();
            wii = mouse_grabber_;
        }

        set_mouse_owner(wii, pt);
        unsigned mm = mm_from_state(event->state);

        if (press) {
            Timeval now = Timeval::now();

            if (button > 0 && button < 4) {
                if (button == last_button_ && last_clicker_ == wii && now-click_ts_ < dclick_time_) {
                    wii->signal_mouse_double_click()(button, mm, pt);
                }

                else {
                    wii->signal_mouse_down()(button, mm, pt);
                }

                click_ts_ = now;
                last_button_ = button;
                last_clicker_ = wii;
            }

            else if (4 == button) {
                wii->signal_mouse_wheel()(-1, mm, pt);
            }

            else if (5 == button) {
                wii->signal_mouse_wheel()(+1, mm, pt);
            }
        }

        else {
            wii->signal_mouse_up()(button, mm, pt);
        }
    }
}

xcb_window_t Display_xcb::root() const {
    return scr_ ? scr_->root : XCB_NONE;
}

Winface_xcb_ptr Display_xcb::find(xcb_window_t xid) {
    auto i = winmap_.find(xid);
    return i != winmap_.end() ? i->second : nullptr;
}

// The format member is set to 8, 16, or 32 and specifies whether the data
// should be viewed as a list of bytes, shorts, or longs.
void Display_xcb::handle_client(xcb_client_message_event_t * event) {
    ustring type = ratom(event->type);

    if ("WM_PROTOCOLS" == type) {
        if (32 == event->format) {
            ustring atom = ratom(event->data.data32[0]);

            if ("_NET_WM_PING" == atom) {
                xcb_client_message_event_t reply = *event;
                reply.window = root();
                xcb_send_event(cx_, false, root(), XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (const char *)&reply);
                xcb_flush(cx_);
                return;
            }

            else if (auto wf = find(event->window)) {
                wf->handle_client(event);
            }

            else {
                std::cerr << "!! Display_xcb: unhandled WM_PROTOCOLS message " << atom << std::endl;
            }
        }
    }
}

void Display_xcb::handle_selection_notify(xcb_selection_notify_event_t * event) {
    if (event->requestor == whidden_ && clipboard_atom_ == event->selection && abcd_atom_ == event->property) {
        xcb_get_property_cookie_t ck = xcb_get_property(cx_, 0, whidden_, abcd_atom_, utf8_string_atom_, 0, 0xffffffff);
        xcb_generic_error_t * e = nullptr;
        xcb_get_property_reply_t * reply = xcb_get_property_reply(cx_, ck, &e);

        if (reply) {
            if (0 != reply->value_len) {
                const char * p = reinterpret_cast<const char *>(reply);
                ustring s(p+sizeof(xcb_get_property_reply_t), reply->value_len);
                signal_paste_text_(s);
            }

            std::free(reply);
        }

        if (e) { std::free(e); }
    }
}

void Display_xcb::handle_selection_request(xcb_selection_request_event_t * event) {
    if (event->owner == whidden_ && event->selection == clipboard_atom_) {
        xcb_selection_notify_event_t respond;
        respond.response_type = XCB_SELECTION_NOTIFY;
        respond.requestor = event->requestor;
        respond.selection = event->selection;
        respond.target = event->target;
        respond.property = event->property;
        respond.time = event->time;

        if (XCB_NONE == event->property) {
            respond.property = XCB_NONE;
        }

        else {
            if (targets_atom_ == event->target) {
                std::vector<xcb_atom_t> atoms;
                atoms.push_back(targets_atom_);
                atoms.push_back(XCB_ATOM_STRING);
                atoms.push_back(utf8_string_atom_);
                xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, event->requestor, event->property, XCB_ATOM_ATOM, 32, atoms.size(), atoms.data());
            }

            else if (utf8_string_atom_ == event->target || XCB_ATOM_STRING == event->target) {
                xcb_change_property(cx_, XCB_PROP_MODE_REPLACE, event->requestor, event->property, utf8_string_atom_, 8, copy_.bytes(), copy_.c_str());
            }

            else {
                respond.property = XCB_NONE;
            }
        }

        xcb_send_event(cx_, 1, event->requestor, 0, (const char *)(&respond));
        xcb_flush(cx_);
    }
}

void Display_xcb::handle_selection_clear(xcb_selection_clear_event_t * event) {
    copy_.clear();
}

// Overrides pure Display_impl.
Toplevel_ptr Display_xcb::create_toplevel(Display_ptr dp, const Rect & ubounds) {
    if (dp.get() != this) { throw graphics_error("Display_xcb: got incompatible Display pointer"); }
    Rect wr;

    if (ubounds) {
        Size sz(std::max(100U, ubounds.width()), std::max(100U, ubounds.height()));
        wr.set(ubounds.origin(), sz);
    }

    else {
        Size sz(size_px().width()/2, size_px().height()/2);
        Point org(sz.width()/2, sz.height()/2);
        wr.set(org, sz);
    }

    auto wdp = std::static_pointer_cast<Display_xcb>(dp);
    auto wf = std::make_shared<Winface_xcb>(wdp, root());
    auto wip = std::make_shared<Toplevel_xcb>(wf, wr);
    wf->init(wip.get(), wr.origin(), wr.size());
    wip->signal_close().connect(tau::bind(fun(this, &Display_xcb::on_window_close), wf->wid()));
    winmap_[wf->wid()] = wf;
    add_window(wip);

    xcb_size_hints_t hints;
    std::memset(&hints, 0, sizeof hints);
    xcb_icccm_size_hints_set_position(&hints, 1, wr.x(), wr.y());
    xcb_icccm_size_hints_set_size(&hints, 1, wr.width(), wr.height());
    xcb_icccm_set_wm_size_hints(cx_, wf->wid(), atom("WM_NORMAL_HINTS"), &hints);

    return wip;
}

// Overrides pure Display_impl.
Dialog_ptr Display_xcb::create_dialog(Window_impl * wii, const Rect & ubounds) {
    auto dp = std::dynamic_pointer_cast<Display_xcb>(wii->display());
    if (dp.get() != this) { throw graphics_error("Display_xcb: got incompatible Display pointer"); }
    Rect wr;

    if (ubounds) {
        Size sz(std::max(100U, ubounds.width()), std::max(100U, ubounds.height()));
        wr.set(ubounds.origin(), sz);
    }

    else {
        Size sz(wii->size().width()/2, wii->size().height()/2);
        Point org(wii->position().x()+sz.width()/2, wii->position().y()+sz.height()/2);
        wr.set(org, sz);
    }

    auto wf = std::make_shared<Winface_xcb>(dp, root());
    auto wip = std::make_shared<Dialog_xcb>(wf, wii, wr);
    wf->init(wip.get(), wr.origin(), wr.size());
    xcb_icccm_set_wm_transient_for(cx_, wf->wid(), WINFACE_XCB(wii)->wid());
    wf->hide_taskbar_hint();
    wip->signal_close().connect(tau::bind(fun(this, &Display_xcb::on_window_close), wf->wid()));
    winmap_[wf->wid()] = wf;
    add_window(wip);
    // FIXME what about _NET_WM_STATE_MODAL?
    // FIXME what about _NET_WM_WINDOW_TYPE_DIALOG?
    return wip;
}

// Overrides pure Display_impl.
Popup_ptr Display_xcb::create_popup(Display_ptr dp, Widget_impl * wi, const Point & upos, Gravity gravity) {
    if (dp.get() != this) { throw graphics_error("Display_xcb: got incompatible Display pointer"); }
    auto wpp = winptr(wi);
    if (!wpp) { throw graphics_error("Display_xcb: create_popup(): parent Window not found"); }

    auto wdp = std::static_pointer_cast<Display_xcb>(dp);
    auto wf = std::make_shared<Winface_xcb>(wdp, WINFACE_XCB(wpp)->wid());
    auto wip = std::make_shared<Popup_xcb>(wf, wpp, upos, gravity);
    wf->init(wip.get(), upos, Size(1));

    wpp->signal_close().connect(fun(wip, &Window_impl::close));
    wip->signal_close().connect(tau::bind(fun(this, &Display_xcb::on_window_close), wf->wid()));
    winmap_[wf->wid()] = wf;
    add_window(wip);
    return wip;
}

void Display_xcb::on_window_close(xcb_window_t wid) {
    winmap_.erase(wid);
}

int Display_xcb::request_check(xcb_void_cookie_t ck) {
    int code = -1;

    if (cx_) {
        code = 0;

        if (auto err = xcb_request_check(cx_, ck)) {
            code = err->error_code;
            std::free(err);
        }
    }

    return code;
}

// Private.
bool Display_xcb::query_pointer(xcb_window_t wid, Point & pt) const {
    auto ck = xcb_query_pointer(cx_, wid);
    xcb_generic_error_t * err = nullptr;
    auto reply = xcb_query_pointer_reply(cx_, ck, &err);

    if (!err) {
        if (reply) {
            pt.set(reply->root_x, reply->root_y);
            std::free(reply);
            return true;
        }
    }

    else {
        std::free(err);
    }

    return false;
}

// Overrides pure Display_impl.
Point Display_xcb::where_mouse() const {
    Point pt;

    for (auto iter = winmap_.begin(); iter != winmap_.end(); ++iter) {
        if (query_pointer(iter->second->wid(), pt)) {
            return pt;
        }
    }

    query_pointer(root(), pt);
    return pt;
}

// Overrides pure Display_impl.
Rect Display_xcb::cursor_area() const {
    Rect r;

    if (0 != xfixes_version_) {
        auto ck = xcb_xfixes_get_cursor_image(cx_);
        xcb_generic_error_t * err = nullptr;

        if (auto reply = xcb_xfixes_get_cursor_image_reply(cx_, ck, &err)) {
            r.set(-reply->xhot, -reply->yhot, Size(reply->width, reply->height));
            std::free(reply);
        }

        if (err) {
            std::free(err);
        }
    }

    return r;
}

std::vector<ustring> Display_xcb::list_xrender_filters(xcb_render_picture_t picture) const {
    std::vector<ustring> filters;

    auto filters_ck = xcb_render_query_filters(cx_, picture);
    xcb_generic_error_t * filters_err = nullptr;

    if (auto reply = xcb_render_query_filters_reply(cx_, filters_ck, &filters_err)) {
        int n_filters = xcb_render_query_filters_filters_length(reply);

        if (n_filters > 0) {
            for (auto iter = xcb_render_query_filters_filters_iterator(reply); iter.rem; xcb_str_next(&iter)) {
                xcb_str_t * str = iter.data;
                unsigned name_len = str->name_len;

                if (0 != name_len) {
                    const char * name = reinterpret_cast<const char *>(str);
                    filters.push_back(ustring(name+1, name_len));
                }
            }
        }
    }

    if (filters_err) {
        std::free(filters_err);
    }

    return filters;
}

xcb_render_picture_t Display_xcb::solid_fill(const Color & c) {
    uint32_t argb = c.argb32();
    auto i = solid_fills_.find(argb);
    if (solid_fills_.end() != i) { return i->second; }
    xcb_render_picture_t xid = xcb_generate_id(cx_);
    xcb_render_create_solid_fill(cx_, xid, x11_render_color(c));

    if (solid_fills_.size() > 32767) {
        auto j = solid_fills_.begin();
        xcb_render_free_picture(cx_, j->second);
        solid_fills_.erase(j);
    }

    solid_fills_[argb] = xid;
    return xid;
}

// Overrides pure Display_impl.
bool Display_xcb::can_paste_text() const {
    return true;
}

void Display_xcb::init_whidden() {
    xcb_window_t wid = xcb_generate_id(cx_);
    uint32_t vals[1] = { 0 };
    vals[0] |= XCB_EVENT_MASK_PROPERTY_CHANGE;
    auto ck = xcb_create_window_checked(cx_, XCB_COPY_FROM_PARENT, wid, root(), 0, 0, 1, 1, 0, XCB_WINDOW_CLASS_INPUT_ONLY, XCB_COPY_FROM_PARENT, XCB_CW_EVENT_MASK, vals);
    int err = request_check(ck);
    if (0 != err) { throw graphics_error("Display_xcb: failed to create window"); }
    whidden_ = wid;
}

xcb_window_t Display_xcb::selection_owner() {
    xcb_window_t owner = XCB_NONE;
    auto ck = xcb_get_selection_owner(cx_, clipboard_atom_);
    xcb_generic_error_t * e = nullptr;
    auto * reply = xcb_get_selection_owner_reply(cx_, ck, &e);

    if (reply) {
        owner = reply->owner;
        std::free(reply);
    }

    if (e) { std::free(e); }
    return owner;
}

// Overrides pure Display_impl.
void Display_xcb::paste_text() {
    xcb_convert_selection(cx_, whidden_, clipboard_atom_, utf8_string_atom_, abcd_atom_, XCB_CURRENT_TIME);
    xcb_flush(cx_);
}

// Overrides pure Display_impl.
void Display_xcb::copy_text(const ustring & str) {
    copy_ = str;
    xcb_set_selection_owner(cx_, whidden_, clipboard_atom_, XCB_CURRENT_TIME);
    xcb_flush(cx_);
}

void Display_xcb::set_net_wm_state(xcb_window_t wid, bool add, const std::string & atom1, const std::string & atom2) {
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.window = wid;
    event.sequence = 0;
    event.format = 32;
    event.type = atom("_NET_WM_STATE");
    event.data.data32[0] = add ? NET_WM_STATE_ADD : NET_WM_STATE_REMOVE;
    event.data.data32[1] = atom(atom1);
    event.data.data32[2] = atom(atom2);
    event.data.data32[3] = 1; // source indication
    event.data.data32[4] = 0;
    xcb_send_event(cx_, false, root(), XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, reinterpret_cast<char *>(&event));
    xcb_flush(cx_);
}

// Overrides pure Display_impl.
bool Display_xcb::grab_modal(Window_impl * wii) {
    if (wii == modal_window_) { return true; }
    if (modal_window_) { return false; }
    modal_window_ = wii;
    xcb_window_t wid = WINFACE_XCB(wii)->wid();
    set_net_wm_state(wid, true, "_NET_WM_STATE_MODAL");
    xcb_set_input_focus(cx_, XCB_INPUT_FOCUS_PARENT, wid, XCB_CURRENT_TIME);
    xcb_flush(cx_);
    return true;
}

// Overrides pure Display_impl.
bool Display_xcb::end_modal(Window_impl * wii) {
    if (modal_window_ == wii) {
        set_net_wm_state(WINFACE_XCB(modal_window_)->wid(), false, "_NET_WM_STATE_MODAL");
        modal_window_ = nullptr;
        return true;
    }

    return false;
}

// Overrides pure Display_impl.
void Display_xcb::grab_window_focus(Window_impl * wii) {
    if (!modal_window_ || wii == modal_window_) {
        xcb_set_input_focus(cx_, XCB_INPUT_FOCUS_PARENT, WINFACE_XCB(wii)->wid(), XCB_CURRENT_TIME);
        xcb_flush(cx_);
    }
}

// Overrides pure Display_impl.
void Display_xcb::grab_mouse(Window_impl * wii) {
    mouse_grabber_ = wii;
    WINFACE_XCB(wii)->grab_mouse();
}

// Overrides pure Display_impl.
void Display_xcb::ungrab_mouse() {
    xcb_ungrab_pointer(cx_, XCB_CURRENT_TIME);
    xcb_flush(cx_);
    mouse_grabber_ = nullptr;
}

} // namespace tau

//END
