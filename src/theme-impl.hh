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

#ifndef TAU_THEME_IMPL_HH
#define TAU_THEME_IMPL_HH

#include <tau/enums.hh>
#include <tau/action.hh>
#include <tau/cursor.hh>
#include <tau/pixmap.hh>
#include <tau/signal.hh>
#include <tau/style.hh>
#include <tau/timeval.hh>
#include <tau/theme.hh>
#include <tau/ustring.hh>
#include <loop-impl.hh>
#include <atomic>
#include <list>
#include <map>
#include <mutex>
#include <vector>
#include <unordered_map>

namespace tau {

class Theme_impl {
public:

    virtual ~Theme_impl() {}
    Theme_impl(const Theme_impl & other) = delete;
    Theme_impl(Theme_impl && other) = delete;
    Theme_impl & operator=(const Theme_impl & other) = delete;
    Theme_impl & operator=(Theme_impl && other) = delete;

    static Theme_ptr root();

    void add_icon_dir(const ustring & dir);
    void add_pixmap_dir(const ustring & dir);
    void add_cursor_dir(const ustring & dir);

    Cursor_ptr  find_cursor(const ustring & names, int size);
    Pixmap_cptr find_pixmap(const ustring & names);
    Pixmap_cptr find_icon(const ustring & names, int icon_size, const ustring & context=ustring());
    Pixmap_ptr  get_icon(const ustring & names, int icon_size, const ustring & context=ustring());

    std::vector<ustring> list_icon_themes() const;
    std::vector<ustring> list_cursor_themes() const;

    void set_cursor_theme(const ustring & names);       // Accepts colon separated list of themes.

    void set_icon_theme(const ustring & names);         // Accepts colon separated list of themes.
    ustring cursor_theme() const;
    ustring icon_theme() const;
    int icon_pixels(int icon_size) const;               // Convert Icon_size enum to the icon size in pixels.

    ustring font_normal() { return font_normal_; }
    ustring font_mono() { return font_mono_; }

    Master_action * find_action(const std::string & name);
    void init_window_style(Style & st);
    void init_style(Style & st);

    void take_cursor_lookup_slot(slot<Cursor_ptr(ustring)> s);

    signal<void()> & signal_cursors_changed();
    signal<void()> & signal_icons_changed();

public:

    struct Cursor_holder {
        Cursor_ptr      cursor;
        Timeval         tv;
    };

    using Cursor_cache  = std::unordered_map<std::string, Cursor_holder>;

protected:

    using Mutex         = std::recursive_mutex;
    using Lock          = std::lock_guard<std::recursive_mutex>;

    Mutex               mmx_;   // Member mutex.
    std::atomic_int     icursor_ { -1 };
    std::atomic_int     iicon_ { -1 };
    int                 cursor_size_ = 24;
    int                 icon_sizes_[1+LARGEST_ICON-SMALLEST_ICON] = { 8, 12, 16, 22, 32, 48 };
    int                 def_icon_ = MEDIUM_ICON;
    ustring             font_normal_;
    ustring             font_mono_;

private:

    Cursor_cache        cursor_cache_;
    Loop_impl *         cleanup_loop_ = nullptr;

protected:

    Theme_impl() = default;

    // Overriden by Theme_posix.
    // Overriden by Theme_win.
    virtual void boot();

    // Overriden by Theme_posix.
    virtual void sweep();

    void boot_icon_themes(const ustring & themes);
    void boot_cursor_themes(const ustring & themes);
    void boot_fallback_theme(const ustring & theme);

private:

    bool feed_icon_dir(const ustring & stop_after_theme=ustring()) const;
    bool feed_icon_root(const ustring & root, const ustring & stop_after_theme=ustring()) const;

    void cache_cursor(Cursor_cache & cache, Cursor_ptr cursor, const ustring & name, int size);
    Cursor_ptr uncache_cursor(Cursor_cache & cache, const ustring & name, int size);
    int find_cursor_theme(const ustring & name) const;
    int find_cursor_theme_nolock(const ustring & name) const;
    Cursor_ptr find_cursor_in_theme(int ctheme, const std::vector<ustring> & unames, std::vector<int> & seen, int size);

    int find_icon_theme(const ustring & name) const;
    int find_icon_theme_nolock(const ustring & name) const;
    Pixmap_ptr find_icon_in_theme(int itheme, const std::vector<ustring> & unames, const ustring & context, std::vector<int> & seen, int size);

    Pixmap_ptr find_picto(const ustring & names, ustring & ret_name);

    void update_this_thread();
    void on_loop_quit(Loop_impl * loop);
    void boot_linkage(); // Linkage dependent method: shared (posix/so/so-posix.cc) or static (posix/a/a-posix.cc).
};

} // namespace tau

#endif // TAU_THEME_IMPL_HH
