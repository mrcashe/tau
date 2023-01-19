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
#include <tau/font.hh>
#include <tau/input.hh>
#include <tau/sys.hh>
#include <tau/toplevel.hh>
#include <theme-impl.hh>
#include "cursor-win.hh"
#include "display-win.hh"
#include "painter-win.hh"
#include "pixmap-win.hh"
#include "window-win.hh"
#include <dbt.h>
#include <shlobj.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <mutex>

namespace {

using Mutex = std::recursive_mutex;
using Lock = std::lock_guard<Mutex>;
using Wndproc_map = std::map<HWND, tau::Display_win *>;
using Displays = std::map<std::thread::id, tau::Display_win_ptr>;

Mutex           smx_;
Wndproc_map     wndproc_map_;
ATOM            toplevel_class_atom_ = 0;
ATOM            dialog_class_atom_ = 0;
ATOM            popup_class_atom_ = 0;
Displays        dps_;
std::atomic_int dpcnt_;

struct Key_pair {
    char32_t c1 = 0, c2 = 0;

    Key_pair() = default;
    Key_pair(char32_t cc1, char32_t cc2): c1(cc1), c2(cc2) {}
    Key_pair(char32_t cc1): c1(cc1), c2(cc1) {}
};

using Key_map = std::map<uint8_t, Key_pair>;

static const Key_map vks_ = {
    { VK_CANCEL,                { tau::KC_CANCEL } },
    { VK_BACK,                  { tau::KC_BACKSPACE } },
    { VK_TAB,                   { tau::KC_TAB } },
    { VK_CLEAR,                 { tau::KC_CLEAR } },
    { VK_RETURN,                { tau::KC_ENTER } },
    { VK_SHIFT,                 { tau::KC_SHIFT } },
    { VK_CONTROL,               { tau::KC_CONTROL } },
    { VK_MENU,                  { tau::KC_ALT } },
    { VK_PAUSE,                 { tau::KC_PAUSE } },
    { VK_CAPITAL,               { tau::KC_CAPS_LOCK } },
    { VK_ESCAPE,                { tau::KC_ESCAPE } },
    { VK_MODECHANGE,            { tau::KC_MODE_SWITCH } },
    { VK_PRIOR,                 { tau::KC_PAGE_UP } },
    { VK_NEXT,                  { tau::KC_PAGE_DOWN } },
    { VK_END,                   { tau::KC_END } },
    { VK_HOME,                  { tau::KC_HOME } },
    { VK_LEFT,                  { tau::KC_LEFT } },
    { VK_UP,                    { tau::KC_UP } },
    { VK_RIGHT,                 { tau::KC_RIGHT } },
    { VK_DOWN,                  { tau::KC_DOWN } },
    { VK_SELECT,                { tau::KC_SELECT } },
    { VK_PRINT,                 { tau::KC_PRINT } },
    { VK_EXECUTE,               { tau::KC_EXECUTE } },
    { VK_SNAPSHOT,              { tau::KC_PRINT } },
    { VK_INSERT,                { tau::KC_INSERT } },
    { VK_DELETE,                { tau::KC_DELETE } },
    { VK_HELP,                  { tau::KC_HELP } },
    { VK_LWIN,                  { tau::KC_LWIN } },
    { VK_RWIN,                  { tau::KC_RWIN } },
    { VK_APPS,                  { tau::KC_MENU } },
    { VK_SLEEP,                 { tau::KC_SLEEP } },
    { VK_NUMPAD0,               { '0' } },
    { VK_NUMPAD1,               { '1' } },
    { VK_NUMPAD2,               { '2' } },
    { VK_NUMPAD3,               { '3' } },
    { VK_NUMPAD4,               { '4' } },
    { VK_NUMPAD5,               { '5' } },
    { VK_NUMPAD6,               { '6' } },
    { VK_NUMPAD7,               { '7' } },
    { VK_NUMPAD8,               { '8' } },
    { VK_NUMPAD9,               { '9' } },
    { VK_MULTIPLY,              { '*' } },
    { VK_ADD,                   { '+' } },
    { VK_SEPARATOR,             { ',' } },
    { VK_SUBTRACT,              { '-' } },
    { VK_DECIMAL,               { '.' } },
    { VK_DIVIDE,                { '/' } },
    { VK_F1,                    { tau::KC_F1 } },
    { VK_F2,                    { tau::KC_F2 } },
    { VK_F3,                    { tau::KC_F3 } },
    { VK_F4,                    { tau::KC_F4 } },
    { VK_F5,                    { tau::KC_F5 } },
    { VK_F6,                    { tau::KC_F6 } },
    { VK_F7,                    { tau::KC_F7 } },
    { VK_F8,                    { tau::KC_F8 } },
    { VK_F9,                    { tau::KC_F9 } },
    { VK_F10,                   { tau::KC_F10 } },
    { VK_F11,                   { tau::KC_F11 } },
    { VK_F12,                   { tau::KC_F12 } },
    { VK_F13,                   { tau::KC_F13 } },
    { VK_F14,                   { tau::KC_F14 } },
    { VK_F15,                   { tau::KC_F15 } },
    { VK_F16,                   { tau::KC_F16 } },
    { VK_F17,                   { tau::KC_F17 } },
    { VK_F18,                   { tau::KC_F18 } },
    { VK_F19,                   { tau::KC_F19 } },
    { VK_F20,                   { tau::KC_F20 } },
    { VK_F21,                   { tau::KC_F21 } },
    { VK_F22,                   { tau::KC_F22 } },
    { VK_F23,                   { tau::KC_F23 } },
    { VK_F24,                   { tau::KC_F24 } },
    { VK_NUMLOCK,               { tau::KC_NUM_LOCK } },
    { VK_SCROLL,                { tau::KC_SCROLL_LOCK } },
    { VK_LSHIFT,                { tau::KC_LSHIFT } },
    { VK_RSHIFT,                { tau::KC_RSHIFT } },
    { VK_LCONTROL,              { tau::KC_LCONTROL } },
    { VK_RCONTROL,              { tau::KC_RCONTROL } },
    { VK_LMENU,                 { tau::KC_LMENU } },
    { VK_RMENU,                 { tau::KC_RMENU } },
    { VK_OEM_CLEAR,             { tau::KC_CLEAR } },
    { 0xee,                     { tau::KC_RWIN } },
    { VK_OEM_PLUS,              { '=',  '+' } },
    { VK_OEM_MINUS,             { '-',  '_' } },
    { VK_OEM_1,                 { ';',  ':' } },
    { VK_OEM_2,                 { '/',  '?' } },
    { VK_OEM_3,                 { '`',  '~' } },
    { VK_OEM_4,                 { '[',  '{' } },
    { VK_OEM_5,                 { '\\', '|' } },
    { VK_OEM_6,                 { ']',  '}' } },
    { VK_OEM_7,                 { '\'', '"' } },
    { VK_OEM_COMMA,             { ',',  '<' } },
    { VK_OEM_PERIOD,            { '.',  '>' } },
    { '0',                      { '0',  ')' } },
    { '1',                      { '1',  '!' } },
    { '2',                      { '2',  '@' } },
    { '3',                      { '3',  '#' } },
    { '4',                      { '4',  '$' } },
    { '5',                      { '5',  '%' } },
    { '6',                      { '6',  '^' } },
    { '7',                      { '7',  '&' } },
    { '8',                      { '8',  '*' } },
    { '9',                      { '9',  '(' } },
    { 'A',                      { 'A',  'a' } },
    { 'B',                      { 'B',  'b' } },
    { 'C',                      { 'C',  'c' } },
    { 'D',                      { 'D',  'd' } },
    { 'E',                      { 'E',  'e' } },
    { 'F',                      { 'F',  'f' } },
    { 'G',                      { 'G',  'g' } },
    { 'H',                      { 'H',  'h' } },
    { 'I',                      { 'I',  'i' } },
    { 'J',                      { 'J',  'j' } },
    { 'K',                      { 'K',  'k' } },
    { 'L',                      { 'L',  'l' } },
    { 'M',                      { 'M',  'm' } },
    { 'N',                      { 'N',  'n' } },
    { 'O',                      { 'O',  'o' } },
    { 'P',                      { 'P',  'p' } },
    { 'Q',                      { 'Q',  'q' } },
    { 'R',                      { 'R',  'r' } },
    { 'S',                      { 'S',  's' } },
    { 'T',                      { 'T',  't' } },
    { 'U',                      { 'U',  'u' } },
    { 'V',                      { 'V',  'v' } },
    { 'W',                      { 'W',  'w' } },
    { 'X',                      { 'X',  'x' } },
    { 'Y',                      { 'Y',  'y' } },
    { 'Z',                      { 'Z',  'z' } }
};

const struct { const char * name; int id; } std_cursors_[] = {
    { "half-busy",          32650 },
    { "left_ptr_watch",     32650 },
    { "all-scroll",         32646 },
    { "size-all",           32646 },
    { "fleur",              32646 },
    { "arrow",              32512 },
    { "left_ptr",           32512 },
    { "default",            32512 },
    { "text",               32513 },
    { "ibeam",              32513 },
    { "cross",              32515 },
    { "tcross",             32515 },
    { "crosshair",          32515 },
    { "crossed_circle",     32648 },
    { "not-allowed",        32648 },
    { "no-drop",            32648 },
    { "forbidden",          32648 },
    { "hand",               32649 },
    { "hand1",              32649 },
    { "hand2",              32649 },
    { "openhand",           32649 },
    { "pointer",            32649 },
    { "pointing_hand",      32649 },
    { "help",               32651 },
    { "whats_this",         32651 },
    { "question_arrow",     32651 },
    { "hourglass",          32514 },
    { "wait",               32514 },
    { "watch",              32514 },
    { "nesw-resize",        32643 },
    { "sw-resize",          32643 },
    { "size_bdiag",         32643 },
    { "ne-resize",          32643 },
    { "ns-resize",          32645 },
    { "n-resize",           32645 },
    { "s-resize",           32645 },
    { "sb_v_double_arrow",  32645 },
    { "v_double_arrow",     32645 },
    { "size_ver",           32645 },
    { "nw-resize",          32642 },
    { "se-resize",          32642 },
    { "nwse-resize",        32642 },
    { "size_fdiag",         32642 },
    { "e-resize",           32644 },
    { "ew-resize",          32644 },
    { "sb_h_double_arrow",  32644 },
    { "h_double_arrow",     32644 },
    { "w-resize",           32644 },
    { "size_hor",           32644 },
    { "up_arrow",           32516 },
    { "center_ptr",         32516 },
    { nullptr, 0 }
};

tau::Display_win_ptr new_display(std::thread::id tid, const tau::ustring & args) {
    auto dp = std::make_shared<tau::Display_win>(tid, args);
    Lock lk(smx_);
    dps_[tid] = dp;
    ++dpcnt_;
    return dp;
}

} // anonymous namespace

namespace tau {

Display_win::Display_win(std::thread::id tid, const ustring &):
    Display_impl(),
    hinstance_(GetModuleHandle(NULL)),
    loop_(Loop_win::this_win_loop())
{
    loop_->signal_quit().connect(fun(this, &Display_win::done));

    if (HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL)) {
        size_px_.update_width(GetDeviceCaps(hdc, HORZRES));
        size_px_.update_height(GetDeviceCaps(hdc, VERTRES));
        size_mm_.update_width(GetDeviceCaps(hdc, HORZSIZE));
        size_mm_.update_height(GetDeviceCaps(hdc, VERTSIZE));

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
        DeleteDC(hdc);
    }

    Theme_impl::root()->take_cursor_lookup_slot(fun(this, &Display_win::lookup_cursor));
}

Display_win::~Display_win() {
    Lock lock(smx_);
    bool cont = false;

    do {
        for (auto iter = wndproc_map_.begin(); iter != wndproc_map_.end(); ++iter) {
            if (iter->second == this) {
                cont = true;
                wndproc_map_.erase(iter);
            }
        }
    } while (cont);
}

// Overrides pure Display_impl.
void Display_win::done() {
    Lock lk(smx_);
    dps_.erase(tid_);
}

// Overrides pure Display_impl.
Toplevel_ptr Display_win::create_toplevel(Display_ptr dp, const Rect & ubounds) {
    if (dp.get() != this) { throw graphics_error("Display_win: got incompatible Display pointer"); }
    auto wdp = std::static_pointer_cast<Display_win>(dp);
    LPCWSTR class_name = L"tau::Toplevel";
    ATOM atom; { Lock lk(smx_); atom = toplevel_class_atom_; }

    if (!atom) {
        WNDCLASSEXW wcl;
        memset(&wcl, 0, sizeof(wcl));
        wcl.cbSize = sizeof(wcl);
        wcl.style = CS_DBLCLKS;
        wcl.lpfnWndProc = wnd_proc;
        wcl.hInstance = hinstance_;
        wcl.lpszClassName = class_name;
        wcl.hCursor = NULL;
        atom = RegisterClassExW(&wcl);
        if (!atom) { throw sys_error("Display_win: RegisterClassExW() failed"); }
        Lock lk(smx_); toplevel_class_atom_ = atom;
    }

    int x, y, width, height;

    if (ubounds) {
        x = ubounds.x(), y = ubounds.y();
        width = ubounds.width(), height = ubounds.height();
    }

    else {
        Size sz(size_px().width()/2, size_px().height()/2);
        width = sz.width(), height = sz.height();
        x = sz.width() >> 1, y = sz.height() >> 1;
    }

    HWND hwnd = CreateWindowExW(0, class_name, NULL, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, x, y, width, height, NULL, NULL, hinstance_, NULL);
    if (!hwnd) { throw sys_error("Display_win: CreateWindowExW() failed"); }
    { Lock lock(smx_); wndproc_map_[hwnd] = this; }

    RECT wr, cr;
    GetWindowRect(hwnd, &wr);
    GetClientRect(hwnd, &cr);
    unsigned border_width  = (wr.right-wr.left)-(cr.right-cr.left);
    unsigned border_height = (wr.bottom-wr.top)-(cr.bottom-cr.top);
    MoveWindow(hwnd, x, y, border_width+width, border_height+height, false);

    auto wf = std::make_shared<Winface_win>(wdp, hwnd);
    auto wip = std::make_shared<Toplevel_win>(wf, Rect(x, y, Size(width, height)));
    wf->init(wip.get());
    register_window(wf);
    add_window(wip);
    return wip;
}

// Overrides pure Display_impl.
Dialog_ptr Display_win::create_dialog(Window_impl * wii, const Rect & ubounds) {
    if (!wii) { throw graphics_error("Display_win::create_dialog(): got pure window pointer"); }
    auto wdp = std::static_pointer_cast<Display_win>(wii->display());
    if (wdp.get() != this) { throw graphics_error("Display_win: got incompatible Display pointer"); }

    LPCWSTR class_name = L"tau::Dialog";
    ATOM atom; { Lock lk(smx_); atom = dialog_class_atom_; }

    if (!atom) {
        WNDCLASSEXW wcl;
        memset(&wcl, 0, sizeof(wcl));
        wcl.cbSize = sizeof(wcl);
        wcl.style = CS_DBLCLKS;
        wcl.lpfnWndProc = wnd_proc;
        wcl.hInstance = hinstance_;
        wcl.lpszClassName = class_name;
        wcl.hCursor = NULL;
        atom = RegisterClassExW(&wcl);
        if (!atom) { throw sys_error("Display_win: RegisterClassExW() failed"); }
        Lock lk(smx_); dialog_class_atom_ = atom;
    }

    int x, y, width, height;

    if (ubounds) {
        x = ubounds.x(), y = ubounds.y();
        width = ubounds.width(), height = ubounds.height();
    }

    else {
        Size sz(wii->size().width()/2, wii->size().height()/2);
        width = sz.width(), height = sz.height();
        x = wii->position().x()+sz.width()/2, y = wii->position().y()+sz.height()/2;
    }

    HWND hwnd = CreateWindowExW(0, class_name, NULL, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
                                x, y, width, height, NULL, NULL, hinstance_, NULL);

    if (!hwnd) { throw sys_error("Display_win: CreateWindowExW() failed"); }
    { Lock lock(smx_); wndproc_map_[hwnd] = this; }

    RECT wr, cr;
    GetWindowRect(hwnd, &wr);
    GetClientRect(hwnd, &cr);
    unsigned border_width  = (wr.right-wr.left)-(cr.right-cr.left);
    unsigned border_height = (wr.bottom-wr.top)-(cr.bottom-cr.top);
    MoveWindow(hwnd, x, y, border_width+width, border_height+height, false);

    HWND hwndp = WINFACE_WIN(wii)->handle();
    auto wf = std::make_shared<Winface_win>(wdp, hwnd);
    auto wip = std::make_shared<Dialog_win>(wf, wii, ubounds);
    wf->init(wip.get());
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    EnableWindow(hwndp, FALSE);
    wip->signal_close().connect(tau::bind(fun(this, &Display_win::on_enable_window), hwndp));
    register_window(wf);
    add_window(wip);
    return wip;
}

// Overrides pure Display_impl.
Popup_ptr Display_win::create_popup(Display_ptr dp, Widget_impl * wi, const Point & upos, Gravity gravity) {
    if (dp.get() != this) { throw graphics_error("Display_win: got incompatible Display pointer"); }
    auto wdp = std::static_pointer_cast<Display_win>(dp);
    auto wpp = winptr(wi);
    if (!wpp) { throw graphics_error("Display_win: can not find parent window"); }

    LPCWSTR class_name = L"tau::Popup";
    ATOM atom; { Lock lk(smx_); atom = popup_class_atom_; }

    if (!atom) {
        WNDCLASSEXW wcl;
        memset(&wcl, 0, sizeof(wcl));
        wcl.cbSize = sizeof(wcl);
        wcl.style = CS_DBLCLKS;
        wcl.lpfnWndProc = wnd_proc;
        wcl.hInstance = hinstance_;
        wcl.lpszClassName = class_name;
        wcl.hCursor = NULL;
        atom = RegisterClassExW(&wcl);
        if (!atom) { throw sys_error("Display_win: RegisterClassExW() failed"); }
        Lock lk(smx_); popup_class_atom_ = atom;
    }

    // WS_CLIPSIBLINGS doesn't work on Win7!
    HWND hwnd = CreateWindowExW(WS_EX_NOPARENTNOTIFY, class_name, NULL, WS_CHILD, upos.x(), upos.y(), 10, 10, WINFACE_WIN(wpp)->handle(), NULL, hinstance_, NULL);
    if (!hwnd) { throw sys_error("Display_win: CreateWindowExW() failed"); }
    { Lock lock(smx_); wndproc_map_[hwnd] = this; }
    auto wf = std::make_shared<Winface_win>(wdp, hwnd);
    auto wip = std::make_shared<Popup_win>(wf, wpp, upos, gravity);
    wf->init(wip.get());
    register_window(wf);
    add_window(wip);
    return wip;
}

void Display_win::on_enable_window(HWND hwnd) {
    EnableWindow(hwnd, TRUE);
}

void Display_win::register_window(Winface_win_ptr wf) {
    HWND hwnd = wf->handle();
    winmap_[hwnd] = wf;
    wf->self()->signal_close().connect(tau::bind(fun(this, &Display_win::on_window_close), hwnd));
}

// static void dump_msg(UINT msg, WPARAM wp, LPARAM lp) {
//     std::cout << str_format("message ", std::hex, std::setw(3), std::setfill('0'), msg);
//     std::cout << ' ';
//     std::cout << str_format(std::hex, std::setw(8), std::setfill('0'), wp);
//     std::cout << ' ';
//     std::cout << str_format(std::hex, std::setw(8), std::setfill('0'), lp);
//     std::cout << std::endl;
// }

// wparam indicates whether various virtual keys are down.
// This parameter can be one or more of the following values.
// MK_CONTROL  0x0008       The CTRL key is down.
// MK_LBUTTON  0x0001       The left mouse button is down.
// MK_MBUTTON  0x0010       The middle mouse button is down.
// MK_RBUTTON  0x0002       The right mouse button is down.
// MK_SHIFT    0x0004       The SHIFT key is down.
// MK_XBUTTON1 0x0020       The first X button is down.
// MK_XBUTTON2 0x0040       The second X button is down.
static unsigned mm_from_wp(WPARAM w) {
    unsigned mm = 0;

    if (w & MK_CONTROL) mm |= MM_CONTROL;
    if (w & MK_SHIFT) mm |= MM_SHIFT;
    if (w & MK_LBUTTON) mm |= MM_LEFT;
    if (w & MK_MBUTTON) mm |= MM_MIDDLE;
    if (w & MK_RBUTTON) mm |= MM_RIGHT;

    return mm;
}

LRESULT Display_win::handle(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CLOSE:
            return handle_close(hwnd, wp, lp) ? DefWindowProcW(hwnd, msg, wp, lp) : 0;

        case WM_LBUTTONDOWN:
            return handle_mouse_down(hwnd, MBT_LEFT, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MBUTTONDOWN:
            return handle_mouse_down(hwnd, MBT_MIDDLE, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_RBUTTONDOWN:
            return handle_mouse_down(hwnd, MBT_RIGHT, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_LBUTTONUP:
            return handle_mouse_up(hwnd, MBT_LEFT, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MBUTTONUP:
            return handle_mouse_up(hwnd, MBT_MIDDLE, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_RBUTTONUP:
            return handle_mouse_up(hwnd, MBT_RIGHT, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_LBUTTONDBLCLK:
            return handle_mouse_double_click(hwnd, MBT_LEFT, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MBUTTONDBLCLK:
            return handle_mouse_double_click(hwnd, MBT_MIDDLE, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_RBUTTONDBLCLK:
            return handle_mouse_double_click(hwnd, MBT_RIGHT, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_DESTROY:
            if (auto wf = find(hwnd)) { wf->self()->close(); }
            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_KEYDOWN:

        case WM_KEYUP:
            return handle_key(hwnd, wp, lp, WM_KEYDOWN == msg) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_SYSKEYDOWN:

        case WM_SYSKEYUP:
            return handle_syskey(hwnd, wp, lp, WM_SYSKEYDOWN == msg) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_CHAR:
            if (!char32_is_control(wp)) {
                char32_t wc = wp;

                if (auto wf = find(hwnd)) {
                    if (!wf->self()->handle_accel(wc, 0)) {
                        if (char32_is_unicode(wc)) {
                            if (wf->self()->handle_input(ustring(1, wc))) {
                                return 0;
                            }
                        }

                        wf->self()->handle_key_down(wc, 0);
                    }

                    return 0;
                }
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MOUSEMOVE:
            if (auto wf = find(hwnd)) {
                Window_impl * wii = wf->self();
                Point pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));

                if (modal_window_) {
                    if (mouse_grabber_ != wii) { pt = wii->to_screen(pt)-modal_window_->to_screen(); }
                    wii = modal_window_;
                }

                auto p = set_mouse_owner(wii, pt);

                if (p == wii) {
                    p->signal_mouse_motion()(mm_from_wp(wp), pt);
                    wf->track_mouse_event();
                    return TRUE;
                }
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MOUSEWHEEL:
            if (auto wf = find(hwnd)) {
                Window_impl * wii = wf->self();
                int d = GET_WHEEL_DELTA_WPARAM(wp);
                Point pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));

                if (modal_window_) {
                    if (mouse_grabber_ != wii) { pt = wii->to_screen(pt)-modal_window_->to_screen(); }
                    wii = modal_window_;
                }

                pt -= wii->position();
                pt -= wii->client_area().origin();
                auto p = set_mouse_owner(wii, pt);

                if (p == wii) {
                    wii->signal_mouse_wheel()(d < 0 ? +1 : -1, mm_from_wp(wp), pt);
                    return TRUE;
                }
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MOUSELEAVE:
            if (auto wf = find(hwnd)) {
                if (wf->self() == mouse_owner_) {
                    reset_mouse_owner();
                    wf->untrack_mouse_event();
                }

                return TRUE;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_SYSCOMMAND:
            return handle_syscommand(hwnd, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_GETMINMAXINFO:
            if (auto wf = find(hwnd)) {
                Size min = wf->self()->min_size_hint(), max = wf->self()->max_size_hint();

                if (min || max) {
                    MINMAXINFO * mmi = reinterpret_cast<MINMAXINFO *>(lp);
                    mmi->ptMinTrackSize.x = min.width();
                    mmi->ptMinTrackSize.y = min.height();
                    mmi->ptMaxTrackSize.x = max.width();
                    mmi->ptMaxTrackSize.y = max.height();
                    return 0;
                }
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_SETFOCUS:
            if (auto wf = find(hwnd)) {
                focus_window(wf->self());
                return 0;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_KILLFOCUS:
            if (auto wf = find(hwnd)) {
                if (unfocus_window(wf->self())) {
                    return 0;
                }
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

            // Client area size changed.
        case WM_SIZE:
            if (auto wf = find(hwnd)) {
                Size size(LOWORD(lp), HIWORD(lp));
                POINT co = { 0, 0 };
                ClientToScreen(hwnd, &co);
                Point pco(co.x, co.y);
                pco -= wf->self()->position();
                wf->self()->handle_client_area(Rect(pco, size));
                wf->self()->update_size(size);
                //std::cout << "WM_SIZE: w=" << size.width() << ":" << size.height() << " pco=" << pco.x() << ":" << pco.y() << "\n";
                return TRUE;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_MOVE:
            if (auto wf = find(hwnd)) {
                //std::cout << "WM_MOVE: " << LOWORD(lp) << ":" << HIWORD(lp) << "\n";

                if (!wf->self()->parent_window()) {
                    RECT wr;
                    GetWindowRect(hwnd, &wr);
                    Point wpt(wr.left, wr.top);
                    wf->self()->update_position(wpt);
                }

                else {
                    int x = static_cast<short>(LOWORD(lp));
                    int y = static_cast<short>(HIWORD(lp));
                    wf->self()->update_position(Point(x, y));
                }

                return TRUE;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_PAINT:
            if (auto wf = find(hwnd)) {
                Window_impl * wii = wf->self();
                if (!pr_) { pr_ = std::make_shared<Painter_win>(); }
                pr_->capture(wii);
                PAINTSTRUCT ps;
                pr_->begin_paint(wf.get(), &ps);
                Rect inval = from_winrect(ps.rcPaint);
                pr_->pclip(inval);
                Painter pr(wii->wrap_painter(pr_));
                wii->handle_backpaint(pr, inval);
                wii->handle_paint(pr, inval);
                pr_->end_paint();
                pr_->wreset();
                return TRUE;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_HELP:
            if (auto wf = find(hwnd)) {
                if (auto tpl = dynamic_cast<Toplevel_impl *>(wf->self())) {
                    if (!tpl->signal_help()()) {
                        tpl->signal_key_down()(KC_F1, 0);
                        tpl->signal_key_up()(KC_F1, 0);
                    }
                }
            }

            else {
                DefWindowProcW(hwnd, msg, wp, lp);
            }

            return TRUE;

        case WM_SHOWWINDOW:
            if (auto wf = find(hwnd)) {
                wf->handle_visibility(0 != wp);
                return TRUE;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);

        case WM_DEVICECHANGE:
            return handle_devicechange(hwnd, wp, lp) ? 0 : DefWindowProcW(hwnd, msg, wp, lp);

        case WM_CREATE:
            return 0;

        case WM_ERASEBKGND:
        case WM_ACTIVATE:
        case WM_CHILDACTIVATE:
        case WM_GETICON:
        case WM_INITMENU:
        case WM_CONTEXTMENU:
        case WM_MENUSELECT:
        case WM_MENUCHAR:
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
        case WM_IME_SELECT:
        case WM_MOUSEACTIVATE:
        case WM_QUERYNEWPALETTE:
        case WM_SETTEXT:
        case WM_GETTEXT:
        case WM_SETCURSOR:
        case WM_SYSCHAR:
        case WM_ENTERMENULOOP:
        case WM_EXITMENULOOP:
        case WM_ACTIVATEAPP:
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
        case WM_NCHITTEST:
        case WM_NCPAINT:
        case WM_NCCALCSIZE:
        case WM_NCMOUSEMOVE:
        case WM_NCDESTROY:
        case WM_NCCREATE:
        case WM_NCACTIVATE:
            //std::cout << "MSG = " << msg << std::endl;
            return DefWindowProcW(hwnd, msg, wp, lp);

        default:
            //dump_msg(msg, wp, lp);
            return DefWindowProcW(hwnd, msg, wp, lp);
    }
}

bool Display_win::handle_mouse_down(HWND hwnd, int mbt, WPARAM wp, LPARAM lp) {
    if (auto wf = find(hwnd)) {
        Window_impl * wii = wf->self();
        Point pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));

        if (modal_window_) {
            if (mouse_grabber_ != wii) { pt = wii->to_screen(pt)-modal_window_->to_screen(); }
            wii = modal_window_;
        }

        auto p = set_mouse_owner(wii, pt);

        if (p == wii) {
            wii->signal_mouse_down()(mbt, mm_from_wp(wp) & ~(MM_LEFT|MM_MIDDLE|MM_RIGHT), pt);
            return true;
        }
    }

    return false;
}

bool Display_win::handle_mouse_double_click(HWND hwnd, int mbt, WPARAM wp, LPARAM lp) {
    if (auto wf = find(hwnd)) {
        Window_impl * wii = wf->self();
        Point pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));

        if (modal_window_) {
            if (mouse_grabber_ != wii) { pt = wii->to_screen(pt)-modal_window_->to_screen(); }
            wii = modal_window_;
        }

        auto p = set_mouse_owner(wii, pt);

        if (p == wii) {
            wii->signal_mouse_double_click()(mbt, mm_from_wp(wp) & ~(MM_LEFT|MM_MIDDLE|MM_RIGHT), pt);
            return true;
        }
    }

    return false;
}

bool Display_win::handle_mouse_up(HWND hwnd, int mbt, WPARAM wp, LPARAM lp) {
    if (auto wf = find(hwnd)) {
        Window_impl * wii = wf->self();
        Point pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));

        if (modal_window_) {
            if (mouse_grabber_ != wii) { pt = wii->to_screen(pt)-modal_window_->to_screen(); }
            wii = modal_window_;
        }

        auto p = set_mouse_owner(wii, pt);

        if (p == wii) {
            wii->signal_mouse_up()(mbt, mm_from_wp(wp) & ~(MM_LEFT|MM_MIDDLE|MM_RIGHT), pt);
            return true;
        }
    }

    return false;
}

static uint32_t modifiers_state() {
    uint32_t km = 0;

    if (0x8000 & GetKeyState(VK_SHIFT)) km |= KM_SHIFT;
    if (0x8000 & GetKeyState(VK_CONTROL)) km |= KM_CONTROL;
    if (0x8000 & GetKeyState(VK_MENU)) km |= KM_ALT;

    return km;
}

char32_t Display_win::translate_vk(char32_t vk, char32_t km) {
    auto iter = vks_.find(vk);
    return iter != vks_.end() ? ((KM_SHIFT & km) ? iter->second.c2 : iter->second.c1) : 0;
}

bool Display_win::handle_key(HWND hwnd, WPARAM w, LPARAM l, bool press) {
    if (auto wf = find(hwnd)) {
        uint32_t km = modifiers_state();
        uint32_t kc = translate_vk(w, km);

        if (0 == kc) {
            if (km & KM_CONTROL && w >= 0x0041 && w <= 0x005a) {
                kc = w-0x0040;
            }
        }

        if (0 != kc) {
            if (char32_is_control(kc) || km & (KM_CONTROL|KM_ALT)) {

                if (press) {
                    if (!wf->self()->handle_accel(kc, km)) {
                        wf->self()->handle_key_down(kc, km);
                    }
                }

                else {
                    wf->self()->handle_key_up(kc, km);
                }

                return true;
            }
        }
    }

    return false;
}

bool Display_win::handle_syskey(HWND hwnd, WPARAM w, LPARAM l, bool press) {
    if (auto wf = find(hwnd)) {
        uint32_t km = modifiers_state();

        if (l & 0x20000000) {
            uint32_t kc = translate_vk(w, km);
            if (0 == kc) { kc = w; }

            if (press) {
                if (!wf->self()->handle_accel(kc, km)) {
                    wf->self()->handle_key_down(kc, km);
                }
            }

            else {
                wf->self()->handle_key_up(kc, km);
            }

            return true;
        }
    }

    return false;
}

bool Display_win::handle_syscommand(HWND hwnd, WPARAM wp, LPARAM lp) {
    switch (wp) {
        // Closes the window.
        case SC_CLOSE:
            return handle_close(hwnd, wp, lp);

        // Changes the cursor to a question mark with a pointer.
        // If the user then clicks a control in the dialog box, the control receives a WM_HELP message.
        case SC_CONTEXTHELP:
            //std::cout << "WM_SYSCOMMAND context help\n";
            return false;

        // Selects the default item; the user double-clicked the window menu.
        case SC_DEFAULT:
            //std::cout << "WM_SYSCOMMAND default\n";
            return false;

        // Activates the window associated with the application-specified hot key.
        // The lParam parameter identifies the window to activate.
        case SC_HOTKEY:
            //std::cout << "WM_SYSCOMMAND hotkey\n";
            return false;

        // Scrolls horizontally.
        case SC_HSCROLL:
            //std::cout << "WM_SYSCOMMAND hscroll\n";
            return false;

        // Retrieves the window menu as a result of a keystroke.
        // For more instr_formation, see the Remarks section.
        case SC_KEYMENU:
            if (auto wf = find(hwnd)) {
                if (auto tpl = dynamic_cast<Toplevel_impl *>(wf->self())) {
                    if (!tpl->signal_menu()()) {
                        tpl->signal_key_down()(KC_F1, 0);
                        tpl->signal_key_up()(KC_F1, 0);
                    }
                }

                return TRUE;
            }

            return FALSE;

        // Maximizes the window.
        case SC_MAXIMIZE:
            if (auto wf = find(hwnd)) { wf->handle_maximize(); }
            return false;

        // Minimizes the window.
        case SC_MINIMIZE:
            if (auto wf = find(hwnd)) { wf->handle_minimize(); }
            return false;

        // Restores the window to its normal position and size.
        case SC_RESTORE:
            if (auto wf = find(hwnd)) { wf->handle_restore(); }
            return false;

        // Executes the screen saver application specified in the [boot] section of the Display.ini file.
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            return !screensaver_allowed();

        // Retrieves the window menu as a result of a mouse click.
        case SC_MOUSEMENU:
            //std::cout << "WM_SYSCOMMAND mouse menu\n";
            return false;

        // Moves the window.
        case SC_MOVE:
            //std::cout << "WM_SYSCOMMAND move\n";
            return false;

        // Moves to the next window.
        case SC_NEXTWINDOW:
            //std::cout << "WM_SYSCOMMAND next window\n";
            return false;

        // Moves to the prev window.
        case SC_PREVWINDOW:
            //std::cout << "WM_SYSCOMMAND prev window\n";
            return false;

        // Sizes the window.
        case SC_SIZE:
            //std::cout << "WM_SYSCOMMAND size\n";
            return false;

        // Activates the Start menu.
        case SC_TASKLIST:
            //std::cout << "WM_SYSCOMMAND task list\n";
            return false;

        // Scrolls vertically.
        case SC_VSCROLL:
            //std::cout << "WM_SYSCOMMAND vscroll\n";
            return false;
    }

    return false;
}

bool Display_win::handle_close(HWND hwnd, WPARAM, LPARAM) {
    if (auto wf = find(hwnd)) {
        auto tpl = dynamic_cast<Toplevel_impl *>(wf->self());
        if (tpl) { tpl->handle_close(); }
        return true;
    }

    return false;
}

bool Display_win::handle_devicechange(HWND hwnd, WPARAM wp, LPARAM lp) {
    if (DBT_DEVICEARRIVAL == wp) { loop_->handle_mount(); }
    else if (DBT_DEVICEREMOVECOMPLETE == wp) { loop_->handle_umount(); }
    return true;
}

Winface_win_ptr Display_win::find(HWND hwnd) {
    auto i = winmap_.find(hwnd);
    return i != winmap_.end() ? i->second : nullptr;
}

void Display_win::on_window_close(HWND hwnd) {
    winmap_.erase(hwnd);
    Lock lock(smx_);
    wndproc_map_.erase(hwnd);
}

// Overrides pure Display_impl.
int Display_win::depth() const {
    return 24;
}

Cursor_ptr Display_win::lookup_cursor(const ustring & name) {
    for (int i = 0; std_cursors_[i].name; ++i) {
        if (str_similar(name, std_cursors_[i].name)) {
            HCURSOR hcursor = LoadCursor(NULL, MAKEINTRESOURCE(std_cursors_[i].id));
            return std::make_shared<Cursor_win>(hcursor);
        }
    }

    return nullptr;
}

// Overrides pure Display_impl.
Point Display_win::where_mouse() const {
    POINT wpt = { 0, 0 };
    GetCursorPos(&wpt);
    return Point(wpt.x, wpt.y);
}

// Overrides pure Display_impl.
Rect Display_win::cursor_area() const {
    Rect r;

    if (HCURSOR hc = GetCursor()) {
        ICONINFO ii = { 0 };

        if (0 != GetIconInfo(hc, &ii)) {
            BITMAP bi = { 0 };

            if (0 != GetObject(ii.hbmMask, sizeof(BITMAP), &bi)) {
                r.set(-ii.xHotspot, -ii.yHotspot, Size(bi.bmWidth, abs(bi.bmHeight)/(ii.hbmColor ? 1 : 2)));
            }

            DeleteObject(ii.hbmColor);
            DeleteObject(ii.hbmMask);
        }
    }

    return r;
}

// Overrides pure Display_impl.
void Display_win::grab_mouse(Window_impl * wii) {
    if (!modal_window_ || wii == modal_window_) {
        mouse_grabber_ = wii;
        SetCapture(WINFACE_WIN(wii)->handle());
    }
}

// Overrides pure Display_impl.
void Display_win::ungrab_mouse() {
    ReleaseCapture();
    mouse_grabber_ = nullptr;
}

// Overrides pure Display_impl.
bool Display_win::grab_modal(Window_impl * wii) {
    if (wii == modal_window_) { return true; }
    if (modal_window_) { return false; }
    modal_window_ = wii;
    SetFocus(WINFACE_WIN(wii)->handle());
    return true;
}

// Overrides pure Display_impl.
bool Display_win::end_modal(Window_impl * wii) {
    if (modal_window_ == wii) {
        modal_window_ = nullptr;
        return true;
    }

    return false;
}

// Overrides pure Display_impl.
void Display_win::grab_window_focus(Window_impl * wii) {
    if (!modal_window_ || wii == modal_window_) {
        SetFocus(WINFACE_WIN(wii)->handle());
    }
}

// Overrides pure Display_impl.
bool Display_win::can_paste_text() const {
    return !copied_text_.empty();
}

// Overrides pure Display_impl.
void Display_win::paste_text() {
}

// Overrides pure Display_impl.
void Display_win::copy_text(const ustring & str) {
    copied_text_ = str;
    signal_can_paste_();
}

// static
LRESULT CALLBACK Display_win::wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Display_win * dpp = nullptr;

    {
        Lock lk(smx_);
        auto iter = wndproc_map_.find(hwnd);
        if (wndproc_map_.end() != iter) { dpp = iter->second; }
    }

    if (dpp) {
        return dpp->handle(hwnd, msg, wp, lp);
    }

    else {
        if (WM_CREATE == msg) { return 0; }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// static
Display_win_ptr Display_win::this_win_display() {
    std::thread::id tid = std::this_thread::get_id();
    Lock lk(smx_);
    auto i = dps_.find(tid);
    if (dps_.end() != i) { return i->second; }
    smx_.unlock();
    return new_display(tid, ustring());
}

// static
Display_ptr Display_impl::open(const ustring & args) {
    std::thread::id tid = std::this_thread::get_id();
    Lock lk(smx_);
    auto i = dps_.find(tid);
    if (dps_.end() != i) { return i->second; }
    smx_.unlock();
    return new_display(tid, args);
}

// static
Display_ptr Display_impl::this_display() {
    return Display_win::this_win_display();
}

} // namespace tau

//END
