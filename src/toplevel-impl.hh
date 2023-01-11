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

#ifndef TAU_TOPLEVEL_IMPL_HH
#define TAU_TOPLEVEL_IMPL_HH

#include <window-impl.hh>

namespace tau {

class Toplevel_impl: public Window_impl {
protected:

    Toplevel_impl(Winface_ptr winface, const Rect & ubounds);

public:

    // Overrides Widget_impl.
    Window_impl * root() override { return this; }

    // Overrides Widget_impl.
    const Window_impl * root() const override { return this; }

    // Overrides Widget_impl.
    // Overrides Window_impl.
    Point to_root(const Point & pt=Point()) const override { return pt; }

    // Overrides Widget_impl.
    void set_parent(Container_impl * parent) override;

    void set_title(const ustring & title);
    void set_icon(Pixmap_ptr pix);
    void set_icon(const ustring & icon_name, unsigned icon_size=MEDIUM_ICON);
    void set_icon_from_file(const ustring & path);

    void minimize();
    void maximize();
    void restore();
    void fullscreen();
    void unfullscreen();

    bool maximized() const { return maximized_; }
    bool minimized() const { return minimized_; }
    bool fullscreened() const { return fullscreen_; }

    void enable_maximize();
    void disable_maximize();
    bool maximize_enabled() const;

    void enable_minimize();
    void disable_minimize();
    bool minimize_enabled() const;

    void show_frame();
    void hide_frame();
    bool frame_visible() const { return frame_visible_; }

    // Handle close [x] button or Alt-F4.
    void handle_close();
    void handle_fullscreen(bool yes);
    void handle_minimize();
    void handle_maximize();
    void handle_restore();

    signal<bool()> & signal_can_close() { return signal_can_close_; }
    signal<void()> & signal_minimize() { return signal_minimize_; }
    signal<void()> & signal_maximize() { return signal_maximize_; }
    signal<void()> & signal_restore() { return signal_restore_; }
    signal<void(bool)> & signal_fullscreen() { return signal_fullscreen_; }
    signal<bool()> & signal_menu() { return signal_menu_; }
    signal<bool()> & signal_help() { return signal_help_; }

protected:

    bool                maximized_ = false;
    bool                minimized_ = false;
    bool                fullscreen_ = false;
    bool                frame_visible_ = true;

    signal<bool()>      signal_can_close_;
    signal<void()>      signal_minimize_;
    signal<void()>      signal_maximize_;
    signal<void()>      signal_restore_;
    signal<void(bool)>  signal_fullscreen_;
    signal<bool()>      signal_menu_;
    signal<bool()>      signal_help_;

private:

    ustring             icon_name_;
    unsigned            icon_size_ = 0;
    connection          icon_theme_cx_;

private:

    void on_icon_theme_changed();
};

} // namespace tau

#endif // TAU_TOPLEVEL_IMPL_HH
