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

#ifndef TAU_DISPLAY_WIN_HH
#define TAU_DISPLAY_WIN_HH

#include "types-win.hh"
#include <display-impl.hh>
#include "loop-win.hh"
#include <map>
#include <thread>
#include <unordered_map>

namespace tau {

class Display_win: public Display_impl {
public:

    static Display_win_ptr this_win_display();

    Display_win(std::thread::id tid, const ustring & args);
   ~Display_win();

    // Overrides pure Display_impl.
    Loop_ptr loop() override { return loop_; }

    // Overrides pure Display_impl.
    Loop_cptr loop() const override { return loop_; }

    // Overrides pure Display_impl.
    int depth() const override;

    // Overrides pure Display_impl.
    Rect cursor_area() const override;

    // Overrides pure Display_impl.
    Point where_mouse() const override;

    // Overrides pure Display_impl.
    bool can_paste_text() const override;

    // Overrides pure Display_impl.
    void paste_text() override;

    // Overrides pure Display_impl.
    void copy_text(const ustring & str) override;

    // Overrides pure Display_impl.
    void grab_mouse(Window_impl * wndi) override;

    // Overrides pure Display_impl.
    void ungrab_mouse() override;

    // Overrides pure Display_impl.
    void grab_window_focus(Window_impl * wii) override;

    // Overrides pure Display_impl.
    bool grab_modal(Window_impl * wii) override;

    // Overrides pure Display_impl.
    bool end_modal(Window_impl * wii) override;

    // Overrides pure Display_impl.
    Toplevel_ptr create_toplevel(Display_ptr dp, const Rect & ubounds) override;

    // Overrides pure Display_impl.
    Dialog_ptr create_dialog(Window_impl * wii, const Rect & bounds=Rect()) override;

    // Overrides pure Display_impl.
    Popup_ptr create_popup(Display_ptr dp, Widget_impl * wpp, const Point & upos, Gravity gravity) override;

    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

protected:

    // Overrides pure Display_impl.
    void done() override;

private:

    struct Font_holder {
        ustring             family_name;
        std::list<unsigned> charsets;
        std::list<ustring>  faces;
    };

    using Font_map = std::unordered_map<std::string, Font_holder>;
    using Winmap = std::map<HWND, Winface_win_ptr>;

    Winmap              winmap_;
    ustring             copied_text_;
    Painter_win_ptr     pr_;
    HINSTANCE           hinstance_;
    Loop_win_ptr        loop_;

private:

    Winface_win_ptr find(HWND hwnd);
    LRESULT handle(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
    void register_window(Winface_win_ptr wf);
    Cursor_ptr lookup_cursor(const ustring & name);

    bool handle_close(HWND hwnd, WPARAM w, LPARAM l);
    bool handle_mouse_down(HWND hwnd, int mbt, WPARAM w, LPARAM l);
    bool handle_mouse_double_click(HWND hwnd, int mbt, WPARAM w, LPARAM l);
    bool handle_mouse_up(HWND hwnd, int mbt, WPARAM w, LPARAM l);

    bool handle_key(HWND hwnd, WPARAM w, LPARAM l, bool press);
    bool handle_syskey(HWND hwnd, WPARAM w, LPARAM l, bool press);
    bool handle_syscommand(HWND hwnd, WPARAM w, LPARAM l);
    bool handle_devicechange(HWND hwnd, WPARAM w, LPARAM l);

    char32_t translate_vk(char32_t vk, char32_t km);

    void on_window_close(HWND hwnd);
    void on_enable_window(HWND hwnd);
};

} // namespace tau

#endif // TAU_DISPLAY_WIN_HH
