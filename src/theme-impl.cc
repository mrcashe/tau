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

#include <tau/exception.hh>
#include <tau/fileinfo.hh>
#include <tau/font.hh>
#include <tau/icon.hh>
#include <tau/key-file.hh>
#include <tau/locale.hh>
#include <tau/sys.hh>
#include <tau/string.hh>
#include <cursor-impl.hh>
#include <event-impl.hh>
#include <pixmap-impl.hh>
#include <theme-impl.hh>

#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>

namespace {

struct Cursor_theme {
    std::vector<tau::ustring>   roots;
    std::vector<tau::ustring>   inherits;
    std::vector<int>            inherited;
    tau::ustring                name;
};

struct Icon_dir {
    tau::ustring                path;
    tau::ustring                type;
    tau::ustring                context;
    bool                        scalable = false;
    bool                        files_listed = false;
    std::list<tau::ustring>     files;
    int                         size = 0;
    unsigned                    scale = 1;
    int                         threshold = 2;
    int                         min_size = 0;
    int                         max_size = 0;
};

struct Icon_theme {
    std::vector<tau::ustring>   roots;
    std::vector<tau::ustring>   inherits;
    std::vector<Icon_dir>       dirs;
    std::vector<int>            inherited;
    tau::ustring                name;
    tau::ustring                name_i18n;
    tau::ustring                comment;
    tau::ustring                comment_i18n;
    tau::ustring                example;
    bool                        hidden = false;
};

using Smutex        = std::recursive_mutex;
using Slock         = std::lock_guard<Smutex>;

using Dirs          = std::list<tau::ustring>;
using Cursor_map    = std::unordered_map<std::string, std::string>; // Maps Name->Path.
using Pixmap_map    = std::unordered_map<std::string, std::string>; // Maps Name->Path.

using Cursor_themes = std::vector<Cursor_theme>;
using Icon_themes   = std::vector<Icon_theme>;

Smutex              mx_;
std::atomic_int     nicon_dirs_ { 0 };
std::atomic_int     ifallback_ { -1 };
Dirs                icon_dirs_;
Dirs                pixmap_dirs_;
Dirs                cursor_dirs_;
Cursor_themes       cursor_themes_;
Icon_themes         icon_themes_;
Cursor_map          cursor_map_;
Pixmap_map          pixmap_map_;

tau::ustring        icon_theme_names_;
tau::ustring        fallback_theme_name_;

using Actions = std::unordered_map<std::string, tau::Master_action>;

struct Thread {
    tau::slot<tau::Cursor_ptr(tau::ustring)> lookup;
    tau::Theme_impl::Cursor_cache cursor_cache;

    tau::Event_ptr  event_cursor_theme_changed;
    tau::Event_ptr  event_icon_theme_changed;
    Actions         actions;
};

using Threads = std::map<std::thread::id, Thread>;

Threads             threads_;

const char * picto_down_xpm_ =
    "/* XPM */\n"
    "static char * picto_down_xpm[] = {\n"
    "\"12 12 5 1\",\n"
    "\"  c None\",\n"
    "\". c #2E3436\",\n"
    "\"+ c #2E3336\",\n"
    "\"@ c #2D3435\",\n"
    "\"# c #2E3437\",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\" .        . \",\n"
    "\" +.      .+ \",\n"
    "\"  @.    .#  \",\n"
    "\"   @.  .#   \",\n"
    "\"    ....    \",\n"
    "\"     @#     \",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"            \"};\n"
;

const char * picto_left_xpm_ =
    "/* XPM */\n"
    "static char * picto_left_xpm[] = {\n"
    "\"12 12 6 1\",\n"
    "\"  c None\",\n"
    "\". c #2D3536\",\n"
    "\"+ c #2E3336\",\n"
    "\"@ c #2D3435\",\n"
    "\"# c #2E3436\",\n"
    "\"$ c #2D3437\",\n"
    "\"            \",\n"
    "\"       .+   \",\n"
    "\"      @#    \",\n"
    "\"     @#     \",\n"
    "\"    @#      \",\n"
    "\"   @#       \",\n"
    "\"   @#       \",\n"
    "\"    @#      \",\n"
    "\"     @#     \",\n"
    "\"      @#    \",\n"
    "\"       $+   \",\n"
    "\"            \"};\n"
;

const char * picto_right_xpm_ =
    "/* XPM */\n"
    "static char * picto_right_xpm[] = {\n"
    "\"12 12 5 1\",\n"
    "\"  c None\",\n"
    "\". c #2E3436\",\n"
    "\"+ c #2E3435\",\n"
    "\"@ c #2F3436\",\n"
    "\"# c #2E3437\",\n"
    "\"            \",\n"
    "\"   .+       \",\n"
    "\"    .@      \",\n"
    "\"     .@     \",\n"
    "\"      .@    \",\n"
    "\"       .@   \",\n"
    "\"       .#   \",\n"
    "\"      .#    \",\n"
    "\"     .#     \",\n"
    "\"    .#      \",\n"
    "\"   ..       \",\n"
    "\"            \"};\n"
;

const char * picto_up_xpm_ =
    "/* XPM */\n"
    "static char * picto_up_xpm[] = {\n"
    "\"12 12 6 1\",\n"
    "\"  c None\",\n"
    "\". c #2D3435\",\n"
    "\"+ c #2F3436\",\n"
    "\"@ c #2E3436\",\n"
    "\"# c #2D3437\",\n"
    "\"$ c #2E3336\",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"     .+     \",\n"
    "\"    @@@@    \",\n"
    "\"   .@  @+   \",\n"
    "\"  .@    @+  \",\n"
    "\" #@      @# \",\n"
    "\" $        $ \",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"            \"};\n"
;

const char * picto_close_xpm_ =
    "/* XPM */\n"
    "static char * picto_close_xpm[] = {\n"
    "\"12 12 10 1\",\n"
    "\"  c None\",\n"
    "\". c #2E3436\",\n"
    "\"+ c #2D3537\",\n"
    "\"@ c #2D3436\",\n"
    "\"# c #2E3537\",\n"
    "\"$ c #2E3337\",\n"
    "\"% c #2F3435\",\n"
    "\"& c #2D3435\",\n"
    "\"* c #2E3435\",\n"
    "\"= c #2F3436\",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"   .+  ..   \",\n"
    "\"   @.@#.$   \",\n"
    "\"    %..&    \",\n"
    "\"    *..%    \",\n"
    "\"   #.==.@   \",\n"
    "\"   .@  @.   \",\n"
    "\"            \",\n"
    "\"            \",\n"
    "\"            \"};\n"
;

const char * picto_inc_xpm_ =
    "/* XPM */\n"
    "static char * picto_inc_xpm[] = {\n"
    "\"7 3 2 1\",\n"
    "\"  c None\",\n"
    "\". c #000000\",\n"
    "\"   .   \",\n"
    "\"  ...  \",\n"
    "\" ..... \"};\n"
;

const char * picto_dec_xpm_ =
    "/* XPM */\n"
    "static char * picto_dec_xpm[] = {\n"
    "\"7 3 2 1\",\n"
    "\"  c None\",\n"
    "\". c #000000\",\n"
    "\" ..... \",\n"
    "\"  ...  \",\n"
    "\"   .   \"};\n"
;

const struct { const char * name, * xpm; } pictos_[] = {
    { tau::ICON_PICTO_UP,       picto_up_xpm_ },
    { tau::ICON_PICTO_DOWN,     picto_down_xpm_ },
    { tau::ICON_PICTO_LEFT,     picto_left_xpm_ },
    { tau::ICON_PICTO_RIGHT,    picto_right_xpm_ },
    { tau::ICON_PICTO_CLOSE,    picto_close_xpm_ },
    { tau::ICON_PICTO_INC,      picto_inc_xpm_ },
    { tau::ICON_PICTO_DEC,      picto_dec_xpm_ },
    { nullptr,                  nullptr },
};

struct Action_def {
    const char *    name;
    const char *    label;
    const char *    icon_name;
    const char *    tooltip;
    const char *    accels;
};

const Action_def action_defs_[] = {
    //     name                   label              icon            tooltip             accels
    { tau::ACTION_FOCUS_NEXT,     "Focus Next",      "go-next",      "Focus Next",       "Tab"                       },
    { tau::ACTION_FOCUS_PREVIOUS, "Focus Previous",  "go-previous",  "Focus Previous",   "<Shift>Tab <Shift>LeftTab" },
    { tau::ACTION_PAN_LEFT,       "Pan Left",        "",             "Pan Left",         "<Ctrl><Alt>Left"           },
    { tau::ACTION_PAN_RIGHT,      "Pan Right",       "",             "Pan Right",        "<Ctrl><Alt>Right"          },
    { tau::ACTION_PAN_UP,         "Pan Up",          "",             "Pan Up",           "<Ctrl>Up"                  },
    { tau::ACTION_PAN_DOWN,       "Pan Down",        "",             "Pan Down",         "<Ctrl>Down"                },
    { nullptr,                    nullptr,           nullptr,        nullptr,            nullptr                     }
};

// Default style items.
const struct { const char * name, * value; } items_[] = {
    { tau::STYLE_FONT,                  ""              },
    { tau::STYLE_FOREGROUND,            "Black"         },
    { tau::STYLE_MENU_FOREGROUND,       "DarkSlateGray" },
    { tau::STYLE_SLIDER_FOREGROUND,     "SteelBlue"     },
    { tau::STYLE_PROGRESS_FOREGROUND,   "Blue"          },
    { tau::STYLE_ACCEL_FOREGROUND,      "Magenta"       },
    { tau::STYLE_PROGRESS_BACKGROUND,   "DeepSkyBlue"   },
    { tau::STYLE_BACKGROUND,            "LightGray"     },
    { tau::STYLE_WHITESPACE_BACKGROUND, "Snow"          },
    { tau::STYLE_MENU_BACKGROUND,       "Silver"        },
    { tau::STYLE_SELECT_BACKGROUND,     "DeepSkyBlue"   },
    { tau::STYLE_TOOLTIP_BACKGROUND,    "Aquamarine"    },
    { tau::STYLE_BUTTON_BACKGROUND,     "Gainsboro"     },
    { tau::STYLE_SLIDER_BACKGROUND,     "DarkGray"      },
    { nullptr,                          nullptr         }
};

struct Pixmap_holder {
    tau::Pixmap_ptr pixmap;
    tau::Timeval    tv;
};

using Pixmap_cache = std::unordered_map<std::string, Pixmap_holder>;

Pixmap_cache        icon_cache_;
Pixmap_cache        pixmap_cache_;

void cache_icon(tau::Pixmap_ptr icon, const tau::ustring & name, const tau::ustring & context, int size) {
    Pixmap_holder hol;
    hol.pixmap = icon;
    hol.tv = tau::Timeval::now();
    tau::ustring key = tau::str_toupper(tau::str_format(name, '-', (context.empty() ? "ANY" : context), '-', size));
    Slock lk(mx_);
    icon_cache_[key] = hol;
}

tau::Pixmap_cptr uncache_icon(const tau::ustring & name, const tau::ustring & context, int size) {
    tau::ustring key = tau::str_toupper(str_format(name, '-', (context.empty() ? "ANY" : context), '-', size));
    Slock lk(mx_);
    auto iter = icon_cache_.find(key);

    if (iter != icon_cache_.end()) {
        iter->second.tv = tau::Timeval::now();
        return iter->second.pixmap;
    }

    return nullptr;
}

void cache_pixmap(tau::Pixmap_ptr pixmap, const tau::ustring & name) {
    Pixmap_holder hol;
    hol.pixmap = pixmap;
    hol.tv = tau::Timeval::now();
    tau::ustring key = tau::str_toupper(tau::str_trim(name));
    Slock ml(mx_);
    pixmap_cache_[key] = hol;
}

tau::Pixmap_cptr uncache_pixmap(const tau::ustring & name) {
    tau::ustring key = tau::str_toupper(str_trim(name));
    Slock ml(mx_);
    auto iter = pixmap_cache_.find(key);

    if (iter != pixmap_cache_.end()) {
        iter->second.tv = tau::Timeval::now();
        return iter->second.pixmap;
    }

    return nullptr;
}

tau::Pixmap_ptr find_icon_in_dir(Icon_dir & dir, const std::vector<tau::ustring> & unames, const tau::ustring & context, int size) {
    if (!dir.files_listed) {
        try {
            for (auto & file: path_list(dir.path)) {
                if (tau::Fileinfo(tau::path_build(dir.path, file)).is_regular()) {
                    tau::ustring suf = tau::path_suffix(file);

                    if (tau::str_similar(suf, tau::str_explode(tau::Pixmap_impl::list_file_suffixes(), ":;"))) {
                        dir.files.push_back(file);
                    }
                }
            }

            dir.files_listed = true;
        }

        catch (tau::exception & x) {
            std::cerr << "** Theme_impl::find_icon_in_dir: dir=" << dir.path << ": " << x.what() << std::endl;
            return nullptr;
        }

        catch (...) {
            std::cerr << "** Theme_impl::find_icon_in_dir: dir=" << dir.path << ": unknown exception caught" << std::endl;
            return nullptr;
        }
    }

    std::list<tau::ustring> blacklist;

    for (auto & file: dir.files) {
        tau::ustring basename = tau::path_basename(file);

        for (auto & uname: unames) {
            if (tau::str_similar(basename, uname)) {
                tau::ustring path = tau::path_build(dir.path, file);

                try {
                    if (auto pixmap = tau::Pixmap_impl::load_from_file(path)) {
                        cache_icon(pixmap, uname, context, size);
                        for (auto & s: blacklist) { dir.files.remove(s); }
                        return pixmap;
                    }
                }

                catch (tau::exception & x) {
                    std::cerr << "** Theme_impl::find_icon_in_dir: file=" << path << ": " << x.what() << std::endl;
                    blacklist.push_back(file);
                    break;
                }

                catch (std::exception & x) {
                    std::cerr << "** Theme_impl::find_icon_in_dir: file=" << path << ": " << x.what() << std::endl;
                    blacklist.push_back(file);
                    break;
                }

                catch (...) {
                    std::cerr << "** Theme_impl::find_icon_in_dir: file=" << path << ": unknown exception caught" << std::endl;
                    blacklist.push_back(file);
                    break;
                }
            }
        }
    }

    for (auto & s: blacklist) { dir.files.remove(s); }
    return nullptr;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

const char * ACTION_FOCUS_NEXT = "focus-next";
const char * ACTION_FOCUS_PREVIOUS = "focus-previous";
const char * ACTION_PAN_LEFT = "pan-left";
const char * ACTION_PAN_RIGHT = "pan-right";
const char * ACTION_PAN_UP = "pan-up";
const char * ACTION_PAN_DOWN = "pan-down";

// Overriden by Theme_posix.
// Overriden by Theme_win.
void Theme_impl::boot() {
    boot_linkage();

    auto tid = std::this_thread::get_id();
    update_this_thread();

    ustring prefix = path_prefix(),
        share = path_build(prefix, "share", program_name()),
        lib_share = path_build(prefix, "share", str_format("tau-", Major_, '.', Minor_));

    add_cursor_dir(path_build(prefix, "cursors"));
    add_pixmap_dir(path_build(prefix, "pixmaps"));
    add_icon_dir(path_build(prefix, "icons"));

    add_cursor_dir(path_build(prefix, "share", "cursors"));
    add_pixmap_dir(path_build(prefix, "share", "pixmaps"));
    add_icon_dir(path_build(prefix, "share", "icons"));

    add_cursor_dir(path_build(share, "cursors"));
    add_pixmap_dir(path_build(share, "pixmaps"));
    add_icon_dir(path_build(share, "icons"));

    add_cursor_dir(path_build(lib_share, "cursors"));
    add_pixmap_dir(path_build(lib_share, "pixmaps"));
    add_icon_dir(path_build(lib_share, "icons"));

    auto i = threads_.find(tid);
    if (i == threads_.end()) { throw internal_error("Theme_impl::init_actions(): unable to setup thread"); }
    Thread & thr = i->second;

    // Init actions.
    for (const Action_def * ap = action_defs_; ap->name; ++ap) {
        auto p = thr.actions.emplace(std::make_pair(std::string(ap->name), Master_action()));
        Master_action & ma = p.first->second;
        ma.set_label(ap->label);
        ma.set_icon_name(ap->icon_name);
        ma.set_tooltip(ap->tooltip);
        ma.add_accels(ap->accels);
    }
}

void Theme_impl::boot_cursor_themes(const ustring & names) {
    set_cursor_theme(names);
}

void Theme_impl::boot_icon_themes(const ustring & names) {
    set_icon_theme(names);
}

void Theme_impl::boot_fallback_theme(const ustring & theme) {
    fallback_theme_name_ = theme;
}

void Theme_impl::sweep() {
    Timeval now = Timeval::now();
    Lock ml(mmx_);

    if (cursor_cache_.size() > 100) {
        std::list<std::string> outdated;

        for (auto i = cursor_cache_.begin(); i != cursor_cache_.end(); ++i) {
            if (now-i->second.tv > 300000000) {
                outdated.push_back(i->first);
            }

            for (const std::string & key: outdated) {
                cursor_cache_.erase(key);
            }
        }
    }

    if (pixmap_cache_.size() > 100) {
        std::list<std::string> outdated;

        for (auto i = pixmap_cache_.begin(); i != pixmap_cache_.end(); ++i) {
            if (now-i->second.tv > 300000000) {
                outdated.push_back(i->first);
            }

            for (const std::string & key: outdated) {
                pixmap_cache_.erase(key);
            }
        }
    }

    if (icon_cache_.size() > 400) {
        std::list<std::string> outdated;

        for (auto i = icon_cache_.begin(); i != icon_cache_.end(); ++i) {
            if (now-i->second.tv > 900000000) {
                outdated.push_back(i->first);
            }

            for (const std::string & key: outdated) {
                icon_cache_.erase(key);
            }
        }
    }
}

void Theme_impl::add_icon_dir(const ustring & dir) {
    if (file_is_dir(dir)) {
        Lock sl(mx_);

        if (icon_dirs_.end() == std::find(icon_dirs_.begin(), icon_dirs_.end(), dir)) {
            icon_dirs_.push_back(dir);
            nicon_dirs_++;
        }
    }
}

void Theme_impl::add_pixmap_dir(const ustring & dir) {
    if (file_is_dir(dir)) {
        Lock sl(mx_);

        if (pixmap_dirs_.end() == std::find(pixmap_dirs_.begin(), pixmap_dirs_.end(), dir)) {
            pixmap_dirs_.push_back(dir);
        }
    }
}

void Theme_impl::add_cursor_dir(const ustring & dir) {
    if (file_is_dir(dir)) {
        Lock sl(mx_);

        if (cursor_dirs_.end() == std::find(cursor_dirs_.begin(), cursor_dirs_.end(), dir)) {
            cursor_dirs_.push_back(dir);
        }
    }
}

bool Theme_impl::feed_icon_root(const ustring & root, const ustring & stop_after) const {
    ustring this_filename = path_notdir(root);
    std::vector<ustring> subdirs;

    for (const ustring & file: path_list(root)) {
        if ("." != file && ".." != file) {
            if (file_is_dir(path_build(root, file))) {
                subdirs.push_back(file);
            }
        }
    }

    Key_file kf(path_build(root, "index.theme"), ',');
    kf.lock();
    Key_section * theme_sect = kf.has_section("Icon Theme") ? &kf.section("Icon Theme") : nullptr;
    ustring theme_name = theme_sect ? kf.get_string(*theme_sect, "Name", this_filename) : this_filename;
    int itheme = -1;
    std::vector<ustring> inherits;

    if (theme_sect) {
        inherits = str_explode(kf.get_string(*theme_sect, "Inherits"), ',');

        if (kf.has_key(*theme_sect, "Directories")) {
            Lock sl(mx_);
            itheme = find_icon_theme_nolock(theme_name);

            if (itheme < 0 || !str_similar(root, icon_themes_[itheme].roots)) {
                if (itheme < 0) {
                    itheme = icon_themes_.size();
                    icon_themes_.emplace_back();
                    icon_themes_[itheme].name = theme_name;
                }

                auto & iref = icon_themes_[itheme];
                iref.roots.push_back(root);
                if (ifallback_ < 0 && str_similar(theme_name, fallback_theme_name_)) { ifallback_ = itheme; }

                for (const ustring & s: inherits) {
                    iref.inherits.push_back(s);
                    int inherited = find_icon_theme_nolock(s);
                    if (inherited >= 0) { iref.inherited.push_back(inherited); }
                }

                // Save new pointer to the themes that inherit this new theme.
                for (Icon_theme & theme: icon_themes_) {
                    if (&theme != &iref) {
                        if (str_similar(theme_name, theme.inherits)) {
                            if (theme.inherited.end() == std::find(theme.inherited.begin(), theme.inherited.end(), itheme)) {
                                theme.inherited.push_back(itheme);
                            }
                        }
                    }
                }

                iref.comment += kf.get_string(*theme_sect, "Comment");
                iref.hidden = kf.get_boolean(*theme_sect, "Hidden");
                iref.example += kf.get_string(*theme_sect, "Example");

                for (const ustring & dir: kf.get_strings(*theme_sect, "Directories")) {
                    if (kf.has_section(dir)) {
                        ustring path = path_build(root, dir);

                        if (file_is_dir(path)) {
                            Icon_dir idir;
                            Key_section & sect = kf.section(dir);
                            idir.path = path;
                            idir.context = kf.get_string(sect, "Context");
                            idir.size = kf.get_integer(sect, "Size");
                            idir.scale = kf.get_integer(sect, "Scale", 1);
                            idir.threshold = kf.get_integer(sect, "Threshold", 2);
                            idir.min_size = kf.get_integer(sect, "MinSize");
                            idir.max_size = kf.get_integer(sect, "MaxSize");
                            idir.type = kf.get_string(sect, "Type");
                            if (str_similar(idir.type, "Scalable")) { idir.scalable = true; }
                            iref.dirs.push_back(idir);
                        }
                    }
                }

                // Update icon_theme_names_.
                if (!iref.hidden) {
                    auto v = str_explode(icon_theme_names_, ":;");
                    if (v.end() == std::find(v.begin(), v.end(), theme_name)) { v.push_back(theme_name); }
                    icon_theme_names_ = str_implode(v, ':');
                }
            }
        }
    }

    int ctheme = -1;

    if (subdirs.end() != std::find(subdirs.begin(), subdirs.end(), "cursors")) {
        ustring cursors_root = path_build(root, "cursors");
        //std::cout << "Cursor theme " << theme_name << "\n";
        Lock sl(mx_);
        ctheme = find_cursor_theme_nolock(theme_name);

        if (ctheme < 0 || !str_similar(cursors_root, cursor_themes_[ctheme].roots)) {
            if (ctheme < 0) {
                ctheme = cursor_themes_.size();
                cursor_themes_.emplace_back();
                cursor_themes_[ctheme].name = theme_name;
            }

            Cursor_theme & rctheme = cursor_themes_[ctheme];
            rctheme.roots.push_back(cursors_root);

            for (const ustring & s: inherits) {
                rctheme.inherits.push_back(s);
                int inherited = find_cursor_theme_nolock(s);
                if (inherited >= 0) { rctheme.inherited.push_back(inherited); }
            }

            // Save new pointer to the themes that inherit this new theme.
            for (Cursor_theme & theme: cursor_themes_) {
                if (&theme != &rctheme) {
                    if (str_similar(theme_name, theme.inherits)) {
                        if (theme.inherited.end() == std::find(theme.inherited.begin(), theme.inherited.end(), ctheme)) {
                            theme.inherited.push_back(ctheme);
                        }
                    }
                }
            }
        }
    }

    if (itheme >= 0 || ctheme >= 0) {
        if (str_similar(theme_name, str_explode(stop_after, ":;"))) {
            return true;
        }
    }

    for (const ustring & file: subdirs) {
        if (feed_icon_root(path_build(root, file), stop_after)) { // recursive call.
            return true;
        }
    }

    return false;
}

bool Theme_impl::feed_icon_dir(const ustring & stop_after) const {
    ustring dir;

    {   Lock sl(mx_);

        if (!icon_dirs_.empty()) {
            dir = icon_dirs_.front();
        }
    }

    if (!dir.empty()) {
        if (feed_icon_root(dir, stop_after)) {
            return true;
        }


        {
            Lock lock(mx_);
            icon_dirs_.remove(dir);
            nicon_dirs_ = icon_dirs_.size();
        }
    }

    return false;
}

int Theme_impl::find_cursor_theme(const tau::ustring & name) const {
    Lock sl(mx_);
    return find_cursor_theme_nolock(name);
}

int Theme_impl::find_cursor_theme_nolock(const tau::ustring & names) const {
    auto v = str_explode(names, ":;");
    Cursor_theme * p = cursor_themes_.data();
    for (std::size_t n = 0; n < cursor_themes_.size(); n++, p++) { if (str_similar(p->name, v)) return n; }
    return -1;
}

std::vector<ustring> Theme_impl::list_cursor_themes() const {
    while (0 != nicon_dirs_) { feed_icon_dir(); }
    std::vector<ustring> v;
    Lock sl(mx_);
    for (const Cursor_theme & theme: cursor_themes_) { v.push_back(theme.name); }
    return v;
}

void Theme_impl::set_cursor_theme(const ustring & names) {
    int save = icursor_, icursor = find_cursor_theme(names);
    while (icursor < 0 && 0 != nicon_dirs_) { feed_icon_dir(names); icursor = find_cursor_theme(names); }

    if (icursor >= 0 && icursor != save) {
        {
            Lock ml(mmx_);
            icursor_ = icursor;
            cursor_cache_.clear();

            Lock sl(mx_);
            for (auto pair: threads_) { pair.second.event_cursor_theme_changed->emit(); }
        }
    }
}

Cursor_ptr Theme_impl::find_cursor_in_theme(int ic, const std::vector<ustring> & unames, std::vector<int> & seen, int size) {
    auto & rctheme = cursor_themes_[ic];
    std::vector<ustring> roots; { Lock sl(mx_); roots = rctheme.roots; }

    for (const ustring & root: roots) {
        if (seen.end() == std::find(seen.begin(), seen.end(), ic)) {
            seen.push_back(ic);

            try {
                for (const ustring & file: path_glob(path_build(root, "*"))) {
                    ustring base = str_trim(str_toupper(path_basename(file)));

                    for (const ustring & name: unames) {
                        if (name == base) {
                            try {
                                if (auto cursor = Cursor_impl::load_from_file(file, size)) {
                                    Lock ml(mmx_);
                                    cache_cursor(cursor_cache_, cursor, name, size);
                                    return cursor;
                                }
                            }

                            catch (...) { break; }
                        }
                    }
                }
            }

            catch (...) { break; }
        }
    }

    std::vector<int> inherited; { Lock sl(mx_); inherited = rctheme.inherited; }

    for (int ii: inherited) {
        if (auto cursor = find_cursor_in_theme(ii, unames, seen, size)) { // recursive call.
            return cursor;
        }
    }

    return nullptr;
}

Cursor_ptr Theme_impl::find_cursor(const ustring & names, int size) {
    auto tid = std::this_thread::get_id();
    if (0 == size) { size = cursor_size_; }
    std::vector<ustring> unames;
    for (const ustring & s: str_explode(names, ":;")) { unames.push_back(str_trim(str_toupper(s))); }

    {
        Lock ml(mmx_);
        auto i = threads_.find(tid);

        for (const ustring & name: unames) {
            // Try to uncache cursor from process wide cache.
            if (auto cursor = uncache_cursor(cursor_cache_, name, size)) {
                return cursor;
            }

            mmx_.unlock(); Lock sl(mx_);
            // Trying to uncache cursor for thread cache.
            if (i != threads_.end()) {
                if (auto cursor = uncache_cursor(i->second.cursor_cache, name, size)) {
                    return cursor;
                }
            }
        }
    }

    if (icursor_ >= 0) {
        std::vector<int> seen;

        if (auto cursor = find_cursor_in_theme(icursor_, unames, seen, size)) {
            return cursor;
        }
    }

    // Now try to search in cursor map.
    for (const ustring & name: unames) {
        ustring path;

        {
            Lock sl(mx_);
            auto i = cursor_map_.find(name);
            if (i != cursor_map_.end()) { path = i->second; }
        }

        if (!path.empty()) {
            try {
                if (auto cursor = Cursor_impl::load_from_file(path, size)) {
                    Lock ml(mmx_);
                    cache_cursor(cursor_cache_, cursor, name, size);
                    return cursor;
                }
            }

            catch (...) {
                Lock sl(mx_);
                cursor_map_.erase(name);
            }
        }
    }

    // Search in booted directories.
    Dirs dirs; { Lock sl(mx_); dirs = cursor_dirs_; }

    for (const ustring & dir: dirs) {
        try {
            for (const ustring & path: path_find(dir, "*")) {
                ustring base = str_trim(str_toupper(path_basename(path)));

                // Copy to cursor map.
                {
                    Lock sl(mx_);
                    if (cursor_map_.find(base) == cursor_map_.end()) {
                        cursor_map_[base] = path;
                    }
                }

                for (const ustring & name: unames) {
                    if (base == name) {
                        try {
                            if (auto cursor = Cursor_impl::load_from_file(path, size)) {
                                Lock ml(mmx_);
                                cache_cursor(cursor_cache_, cursor, name, size);
                                return cursor;
                            }
                        }

                        catch (...) { break; }
                    }
                }
            }

            // All cursor paths from current directory are copied
            // to the cursor map, so we can erase that directory.
            {
                Lock sl(mx_);
                auto i = std::find(cursor_dirs_.begin(), cursor_dirs_.end(), dir);
                if (i != cursor_dirs_.end()) { cursor_dirs_.erase(i); }
            }
        }

        catch (...) {}
    }

    // Trying to obtain cursor from Display.
    {
        Lock sl(mx_);
        auto i = threads_.find(tid);

        if (i != threads_.end()) {
            Thread & thr = i->second;

            // Display_xcb uses case sensitive cursor lookup, so don't uppercase cursor name here!
            for (const ustring & s: str_explode(names, ":;")) {
                if (auto cursor = thr.lookup(s)) {
                    cache_cursor(thr.cursor_cache, cursor, str_trim(str_toupper(s)), size);
                    return cursor;
                }
            }
        }
    }

    return nullptr;
}

void Theme_impl::cache_cursor(Cursor_cache & cache, Cursor_ptr cursor, const ustring & name, int size) {
    Cursor_holder hol;
    hol.cursor = cursor;
    hol.tv = Timeval::now();
    ustring key = str_format(name, '_', size);
    cache[key] = hol;
}

Cursor_ptr Theme_impl::uncache_cursor(Cursor_cache & cache, const ustring & name, int size) {
    ustring key = str_format(name, '_', size);
    auto iter = cache.find(key);

    if (iter != cache.end()) {
        iter->second.tv = Timeval::now();
        return iter->second.cursor;
    }

    return nullptr;
}

Pixmap_cptr Theme_impl::find_pixmap(const ustring & names) {
    auto v = str_explode(names, ":;");

    // Trying to uncache first given variant.
    if (!v.empty()) {
        auto pixmap = uncache_pixmap(v[0]);
        if (pixmap) { return pixmap; }
    }

    for (const ustring & name: v) {
        ustring key = str_toupper(str_trim(name)), path;
        { Lock sl(mx_); auto i = pixmap_map_.find(key); if (i != pixmap_map_.end()) path = i->second; }

        if (!path.empty()) {
            try {
                if (auto pixmap = Pixmap_impl::load_from_file(path)) {
                    cache_pixmap(pixmap, name);
                    return pixmap;
                }
            }

            catch (...) { break; }
        }
    }

    std::list<ustring> dirs;
    { Lock sl(mx_); dirs = pixmap_dirs_; }

    for (const ustring & dir: dirs) {
        try {
            for (const ustring & path: path_find(dir, "*")) {
                ustring base = str_toupper(str_trim(path_basename(path)));

                {
                    Lock lock(mx_);
                    auto i = pixmap_map_.find(base);
                    if (i == pixmap_map_.end()) { pixmap_map_[base] = path; }
                }

                for (const ustring & name: v) {
                    if (str_toupper(str_trim(name)) == base) {
                        try {
                            if (auto pixmap = Pixmap_impl::load_from_file(path)) {
                                cache_pixmap(pixmap, name);
                                return pixmap;
                            }
                        }

                        catch (exception & x) {
                            std::cerr << "** Theme_impl::find_pixmap(): " << x.what() << std::endl;
                            break;
                        }
                    }
                }
            }

            {
                Lock sl(mx_);
                auto i = std::find(pixmap_dirs_.begin(), pixmap_dirs_.end(), dir);
                if (i != pixmap_dirs_.end()) { pixmap_dirs_.erase(i); }
            }
        }

        catch (exception & x) {
            std::cerr << "** Theme_impl::find_pixmap(): " << x.what() << std::endl;
            break;
        }
    }

    // Trying to uncache all variants
    if (v.size() > 1) {
        for (const ustring & name: v) {
            auto pixmap = uncache_pixmap(name);
            if (pixmap) { return pixmap; }
        }
    }

    // Try to search into "picto-*".
    ustring ret_name;

    if (auto pixmap = find_picto(names, ret_name)) {
        cache_pixmap(pixmap, ret_name);
        return pixmap;
    }

    return nullptr;
}

int Theme_impl::find_icon_theme(const ustring & names) const {
    Lock lock(mx_);
    return find_icon_theme_nolock(names);
}

int Theme_impl::find_icon_theme_nolock(const ustring & names) const {
    auto v = str_explode(names, ":;");
    Icon_theme * p = icon_themes_.data();

    for (std::size_t n = 0; n != icon_themes_.size(); n++, p++) {
        if (str_similar(p->name, v)) {
            return n;
        }
    }

    return -1;
}

void Theme_impl::set_icon_theme(const ustring & names) {
    int save = iicon_, iicon = find_icon_theme(names);

    while (iicon < 0 && 0 != nicon_dirs_) {
        feed_icon_dir(names);
        iicon = find_icon_theme(names);
    }

    if (iicon >= 0 && iicon != save) {
        {
            Lock ml(mmx_);
            iicon_ = iicon;
            icon_cache_.clear();
            for (auto pair: threads_) { pair.second.event_icon_theme_changed->emit(); }
        }
    }
}

std::vector<ustring> Theme_impl::list_icon_themes() const {
    while (0 != nicon_dirs_) { feed_icon_dir(); }
    ustring names; { Lock lock(mx_); names = icon_theme_names_; }
    return str_explode(names, ":;");
}

int Theme_impl::icon_pixels(int icon_size) const {
    if (icon_size >= SMALLEST_ICON && icon_size <= LARGEST_ICON) {
        return icon_sizes_[icon_size-SMALLEST_ICON];
    }

    else if (DEFAULT_ICON == icon_size) {
        return def_icon_;
    }

    return std::max(0, icon_size);
}

Pixmap_ptr Theme_impl::find_icon_in_theme(int iicon, const std::vector<ustring> & unames, const ustring & context, std::vector<int> & seen, int size) {
    auto & ref = icon_themes_[iicon]; // Don't need to lock here.

    if (seen.end() == std::find(seen.begin(), seen.end(), iicon)) {
        seen.push_back(iicon);
        int best = -1, dmin = INT_MAX, dindex = 0;
        Lock sl(mx_);

        for (auto & dir: ref.dirs) {
            if (!dir.scalable) {
                if (!context.empty() && !str_similar(dir.context, context)) {
                    continue;
                }

                if (0 != dir.size) {
                    int ds = dir.size >= size ? dir.size-size : size-dir.size;
                    if (ds < dmin) { dmin = ds, best = dindex; }
                }
            }

            dindex++;
        }

        if (best >= 0) {
            if (auto pixmap = find_icon_in_dir(ref.dirs[best], unames, context, size)) {
                return pixmap;
            }
        }

        dindex = 0;
        for (auto & dir: ref.dirs) {
            if (dindex != best) {
                if (auto pixmap = find_icon_in_dir(dir, unames, context, size)) {
                    return pixmap;
                }
            }

            dindex++;
        }
    }

    std::vector<int> inherited; { Lock lock(mx_); inherited = ref.inherited; }

    for (int ii: inherited) {
        if (auto pixmap = find_icon_in_theme(ii, unames, context, seen, size)) { // recursive call.
            return pixmap;
        }
    }

    return nullptr;
}

Pixmap_cptr Theme_impl::find_icon(const ustring & names, int size, const ustring & context) {
    size = icon_pixels(size);
    std::vector<ustring> unames;

    for (const ustring & s: str_explode(names, ":;")) {
        unames.push_back(str_trim(str_toupper(s)));
    }

    // Try to uncache only first icon.
    if (!unames.empty()) {
        if (auto pixmap = uncache_icon(unames[0], context, size)) {
            return pixmap;
        }
    }

    // Search in icon theme, if it set.
    if (iicon_ >= 0) {
        std::vector<int> seen;

        if (auto pixmap = find_icon_in_theme(iicon_, unames, context, seen, size)) {
            return pixmap;
        }

        int ifb = ifallback_;

        if (ifb >= 0 && (seen.end() == std::find(seen.begin(), seen.end(), ifb))) {
            if (auto pixmap = find_icon_in_theme(ifb, unames, context, seen, size)) {
                return pixmap;
            }
        }

        if (!context.empty()) {
            seen.clear();

            if (auto pixmap = find_icon_in_theme(iicon_, unames, ustring(), seen, size)) {
                return pixmap;
            }

            if (ifb >= 0 && (seen.end() == std::find(seen.begin(), seen.end(), ifb))) {
                if (auto pixmap = find_icon_in_theme(ifb, unames, ustring(), seen, size)) {
                    return pixmap;
                }
            }
        }
    }

    // Try to uncache all given variants.
    if (unames.size() > 1) {
        for (const ustring & name: unames) {
            if (auto pixmap = uncache_icon(name, context, size)) {
                return pixmap;
            }
        }
    }

    ustring ret_name;

    if (auto pixmap = find_picto(names, ret_name)) {
        cache_icon(pixmap, ret_name, context, size);
        return pixmap;
    }

    return find_pixmap(names);
}

Pixmap_ptr Theme_impl::find_picto(const ustring & names, ustring & ret_name) {
    ustring picto;

    for (const ustring & s: str_explode(names, ":;")) {
        if (str_has_prefix(s, "picto-", true)) {
            picto = s;
            break;
        }
    }

    if (!picto.empty()) {
        const char * xpm = nullptr;

        for (std::size_t i = 0; pictos_[i].name; ++i) {
            if (str_similar(picto, pictos_[i].name)) {
                xpm = pictos_[i].xpm;
                break;
            }
        }

        if (xpm) {
            if (auto pix = Pixmap_impl::load_xpm_from_string(xpm)) {
                ret_name = picto;
                return pix;
            }
        }
    }

    return nullptr;
}

Pixmap_ptr Theme_impl::get_icon(const ustring & names, int size, const ustring & context) {
    if (auto pixmap = find_icon(names, size, context)) { return pixmap->dup(); }
    size = icon_pixels(size);
    auto pix = Pixmap_impl::create(1, size);
    cache_icon(pix, str_explode(names, ":;").front(), context, size);
    return pix;
}

ustring Theme_impl::cursor_theme() const {
    if (-1 != icursor_) {
        Lock sl(mx_);
        return cursor_themes_[icursor_].name;
    }

    return ustring();
}

ustring Theme_impl::icon_theme() const {
    if (-1 != iicon_) {
        Lock sl(mx_);
        return icon_themes_[iicon_].name;
    }

    return ustring();
}

void Theme_impl::take_cursor_lookup_slot(slot<Cursor_ptr(ustring)> s) {
    update_this_thread();
    Lock ml(mx_);
    auto i = threads_.find(std::this_thread::get_id());
    if (i != threads_.end()) { i->second.lookup = s; }
}

void Theme_impl::update_this_thread() {
    auto tid = std::this_thread::get_id();
    Lock lk(mx_);
    auto i = threads_.find(tid);
    if (i != threads_.end()) { return; }

    if (auto loop = Loop_impl::that_loop(tid)) {
        loop->signal_quit().connect(tau::bind(fun(this, &Theme_impl::on_loop_quit), loop.get()));

        if (!cleanup_loop_) {
            cleanup_loop_ = loop.get();
            loop->signal_alarm(61678, true).connect(fun(this, &Theme_impl::sweep));
        }

        auto pair = threads_.emplace(std::make_pair(tid, Thread()));
        Thread & thr = pair.first->second;
        thr.event_cursor_theme_changed = loop->create_event();
        thr.event_icon_theme_changed = loop->create_event();
    }
}

void Theme_impl::on_loop_quit(Loop_impl * loop) {
    auto tid = std::this_thread::get_id();

    Lock lk(mx_);
    threads_.erase(tid);
    if (cleanup_loop_ != loop) { return; }
    cleanup_loop_ = nullptr;
    if (threads_.empty()) { return; }
    auto i = threads_.begin();
    Loop_ptr lptr = Loop_impl::that_loop(i->first);

    if (lptr) {
        // Switching cleanup to the first existing loop.
        cleanup_loop_ = lptr.get();
        lptr->signal_alarm(59321, true).connect(fun(this, &Theme_impl::sweep));
    }
}

signal<void()> & Theme_impl::signal_cursors_changed() {
    auto tid = std::this_thread::get_id();
    update_this_thread();
    Lock lk(mx_);
    auto i = threads_.find(tid);
    if (i == threads_.end()) { throw internal_error("Theme_impl::signal_cursors_changed(): unable to setup thread"); }
    Thread & thr = i->second;
    return thr.event_cursor_theme_changed->signal_ready();
}

signal<void()> & Theme_impl::signal_icons_changed() {
    auto tid = std::this_thread::get_id();
    update_this_thread();
    Lock lk(mx_);
    auto i = threads_.find(tid);
    if (i == threads_.end()) { throw internal_error("Theme_impl::signal_icons_changed(): unable to setup thread"); }
    Thread & thr = i->second;
    return thr.event_icon_theme_changed->signal_ready();
}

Master_action * Theme_impl::find_action(const std::string & name) {
    auto tid = std::this_thread::get_id();
    update_this_thread();
    Lock lk(mx_);
    auto i = threads_.find(tid);
    if (i == threads_.end()) { throw internal_error("Theme_impl::find_action(): unable to setup thread"); }
    Thread & thr = i->second;
    auto j = thr.actions.find(name);
    if (j != thr.actions.end()) { return &j->second; }
    return nullptr;
}

void Theme_impl::init_style(Style & st) {
    for (std::size_t i = 0; items_[i].name; ++i) { st.set(items_[i].name); }
}

void Theme_impl::init_window_style(Style & st) {
    for (std::size_t i = 0; items_[i].name; ++i) { st.set(items_[i].name, items_[i].value); }
    st.set("font", Font::normal());
}

} // namespace tau

//END
