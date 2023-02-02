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

#include <dialog-impl.hh>
#include <loop-impl.hh>
#include <display-impl.hh>

namespace tau {

Dialog_impl::Dialog_impl(Winface_ptr winface, const Rect & ubounds):
    Toplevel_impl(winface, ubounds)
{
}

Dialog_impl::Dialog_impl(Winface_ptr winface, Window_impl * wipp, const Rect & ubounds):
    Toplevel_impl(winface, ubounds)
{
    wipp->signal_close().connect(fun(this, &Dialog_impl::close), true);
    wipp->signal_hide().connect(fun(this, &Dialog_impl::hide));
    wipp->signal_show().connect(fun(this, &Dialog_impl::show));
}

// Overrides Window_impl.
void Dialog_impl::close() {
    if (run_) { close_ = true; quit(); }
    else { Window_impl::close(); }
}

// Overrides Widget_impl.
void Dialog_impl::quit_dialog() {
    close();
}

void Dialog_impl::run() {
    if (!run_) {
        show();
        run_ = true;
        Loop_impl::this_loop()->run();
        run_ = false;
        hide();
        if (close_) { Window_impl::close(); }
    }
}

void Dialog_impl::quit() {
    if (run_) {
        Loop_impl::this_loop()->quit();
        run_ = false;
    }
}

} // namespace tau

//END
