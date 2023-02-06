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
#include <tau/cursor.hh>
#include <tau/loop.hh>
#include <tau/sys.hh>
#include <tau/key-file.hh>
#include <tau/locale.hh>
#include <tau/style.hh>
#include <dialog-impl.hh>
#include <display-impl.hh>
#include <loop-impl.hh>
#include <pixmap-impl.hh>
#include <theme-impl.hh>
#include <window-impl.hh>
#include <fstream>
#include <iostream>

namespace tau {

// Overrides Display.
Size Display_impl::size_px() const {
    return size_px_;
}

// Overrides Display.
Size Display_impl::size_mm() const {
    return size_mm_;
}

// Overrides Display.
unsigned Display_impl::dpi() const {
    return dpi_;
}

Window_impl * Display_impl::set_mouse_owner(Window_impl * wii, const Point & pt) {
    Window_impl * mo = mouse_owner_;
    mouse_owner_ = nullptr;

    if (mo && (mo != wii || !wii->enabled())) {
        mo->handle_mouse_leave();
    }

    if (wii->enabled()) {
        mouse_owner_ = wii;
        if (mo != wii) { wii->handle_mouse_enter(pt); }
    }

    return mouse_owner_;
}

void Display_impl::reset_mouse_owner() {
    Window_impl * wii = mouse_owner_;
    mouse_owner_ = nullptr;
    if (wii) { wii->handle_mouse_leave(); }
}

Window_ptr Display_impl::winptr(Widget_impl * wi) {
    for (auto wip: windows_) {
        if (wip.get() == wi) {
            return wip;
        }
    }

    return nullptr;
}

Window_cptr Display_impl::winptr(const Widget_impl * wi) const {
    for (auto wip: windows_) {
        if (wip.get() == wi) {
            return wip;
        }
    }

    return nullptr;
}

void Display_impl::add_window(Window_ptr wip) {
    windows_.push_back(wip);
    Window_impl * wii = wip.get();
    if (!loop()->running()) { loop()->signal_start().connect(fun(wii, &Window_impl::show)); }
    loop()->signal_quit().connect(fun(wii, &Window_impl::close));
    wii->signal_close().connect(tau::bind(fun(this, &Display_impl::remove_window), wii));
    wii->signal_enable().connect(tau::bind(fun(this, &Display_impl::on_window_sensitivity), wii));
    wii->signal_disable().connect(tau::bind(fun(this, &Display_impl::on_window_sensitivity), wii));
    Theme_impl::root()->init_window_style(wii->style());
    wii->handle_enable(true);
    wii->signal_parent()();
    wii->handle_display();
}

void Display_impl::remove_window(Window_impl * wii) {
    for (auto i = windows_.begin(); i != windows_.end(); ++i) {
        Window_ptr wip = *i;

        if (wip.get() == wii) {
            if (auto dip = std::dynamic_pointer_cast<Dialog_impl>(wip)) { dip->quit(); }
            wii->unparent();
            woff_.push_back(wip);
            if (!woff_timer_.running()) { woff_tv_ = 0; }
            woff_timer_.restart(11);
            windows_.erase(i);
            if (wii == modal_window_) { modal_window_  = nullptr; }
            if (wii == focused_) { focused_ = nullptr; }
            if (wii == mouse_grabber_) { mouse_grabber_ = nullptr; }
            if (wii == mouse_owner_) { mouse_owner_  = nullptr; }
            return;
        }
    }
}

void Display_impl::on_window_sensitivity(Window_impl * wii) {
    if (!wii->enabled()) {
        if (wii == mouse_owner_) { reset_mouse_owner(); }
        unfocus_window(wii);
    }
}

void Display_impl::focus_window(Window_impl * wii) {
    if (wii != focused_) {
        if (focused_) { focused_->signal_focus_out()(); }
        focused_ = nullptr;
        if (wii) { focused_ = wii; focused_->signal_focus_in()(); }
    }
}

bool Display_impl::unfocus_window(Window_impl * wii) {
    if (wii == focused_) {
        focused_ = nullptr;
        wii->signal_focus_out()();
        return true;
    }

    return false;
}

// Overriden by Display_xcb.
void Display_impl::allow_screensaver() {
    if (0 != screensaver_counter_) {
        screensaver_counter_--;
    }
}

// Overriden by Display_xcb.
void Display_impl::disallow_screensaver() {
    screensaver_counter_++;
}

void Display_impl::on_woff_timer() {
    Window_impl * running = nullptr;

    for (auto wip: woff_) {
        if (wip->running()) {
            running = wip.get();
            break;
        }
    }

    if (running) {
        woff_timer_.restart(45);
        Timeval now = Timeval::now();

        if (woff_tv_ && now-woff_tv_ >= 2000) {
            std::cerr << "** Display_impl: can not stop running Window_impl @" << running << std::endl;
        }

        woff_tv_ = now;
    }

    else {
        woff_tv_ = 0;
        woff_.clear();
        if (windows_.empty()) { Loop().quit(); }
    }
}

} // namespace tau

//END
