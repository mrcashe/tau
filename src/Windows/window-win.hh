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

#ifndef TAU_WINDOW_WIN_HH
#define TAU_WINDOW_WIN_HH

#include <dialog-impl.hh>
#include <popup-impl.hh>
#include "display-win.hh"
#include "winface-win.hh"

namespace tau {

struct Toplevel_win: public Toplevel_impl {
    Toplevel_win(Winface_win_ptr winface, const Rect & ubounds):
        Toplevel_impl(winface, ubounds)
    {
    }
};

struct Popup_win: public Popup_impl {
    Popup_win(Winface_win_ptr winface, Window_ptr wpp, const Point & upos, Gravity gravity):
        Popup_impl(winface, upos, wpp, gravity)
    {
    }
};

struct Dialog_win: public Dialog_impl {
    Dialog_win(Winface_win_ptr winface, Window_impl * wipp, const Rect & ubounds):
        Dialog_impl(winface, wipp, ubounds)
    {
    }
};

} // namespace tau

#endif // TAU_WINDOW_WIN_HH
