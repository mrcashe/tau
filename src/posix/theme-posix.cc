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

#include "theme-posix.hh"
#include "font-utils-posix.hh"
#include <mutex>
#include <iostream>

namespace {

using Mutex = std::recursive_mutex;
using Lock = std::lock_guard<Mutex>;

Mutex                mx_;
tau::Theme_posix_ptr root_;

} // anonymous namespace

namespace tau {

// static
Theme_posix_ptr Theme_posix::root_posix() {
    Lock lk(mx_);

    if (!root_) {
        root_ = std::make_shared<Theme_posix>();
        root_->boot();
    }

    return root_;
}

// static
Theme_ptr Theme_impl::root() {
    return Theme_posix::root_posix();
}

// Overrides Theme_impl.
void Theme_posix::boot() {
    Theme_impl::boot();

    add_pixmap_dir("/usr/share/pixmaps");
    add_pixmap_dir("/usr/share/icons");
    add_icon_dir("/usr/share/icons");

    add_pixmap_dir("/usr/local/share/pixmaps");
    add_pixmap_dir("/usr/local/share/icons");
    add_icon_dir("/usr/local/share/icons");

    boot_icon_themes("Tau");
    boot_cursor_themes("Oxygen Blue:Oxygen Yellow:Breeze:KDE Classic:Adwaita");
    boot_fallback_theme("Hicolor");
}

// Overrides Theme_impl.
void Theme_posix::sweep() {
    Theme_impl::sweep();
    cleanup_font_cache();
}

} // namespace tau

//END
