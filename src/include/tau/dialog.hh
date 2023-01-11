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

/// @file dialog.hh The Dialog class.

#ifndef TAU_DIALOG_HH
#define TAU_DIALOG_HH

#include <tau/toplevel.hh>

namespace tau {

/// %Dialog window.
/// @ingroup window_group
class Dialog: public Toplevel {
public:

    /// Constructor with widget.
    /// Constructs %Dialog with top level window obtained from the w.
    /// In case widget w not inserted into hierarchy with Toplevel at the top,
    /// an %exception throws. To prevent %exception throwing, test Toplevel
    /// existance with Widget::has_window() method.
    /// @throw graphics_error if unable to obtain top level window.
    Dialog(Widget & w, const Rect & bounds=Rect());

    /// Constructor with widget and title.
    /// Constructs %Dialog with top level window obtained from the w.
    /// In case widget w not inserted into hierarchy with Toplevel at the top,
    /// an %exception throws. To prevent %exception throwing, test Toplevel
    /// existance with Widget::has_window() method.
    /// @throw graphics_error if unable to obtain top level window.
    Dialog(Widget & w, const ustring & title, const Rect & bounds=Rect());

    /// Constructor with window.
    /// @throw graphics_error if unable to obtain Display.
    Dialog(Toplevel & wnd, const Rect & bounds=Rect());

    /// Constructor with window and title.
    /// @throw graphics_error if unable to obtain Display.
    Dialog(Toplevel & wnd, const ustring & title, const Rect & bounds=Rect());

    /// Run dialog.
    void run();

    /// Quit dialog.
    void quit();

    /// Test if dialog running.
    bool running() const;
};

} // namespace tau

#endif // TAU_DIALOG_HH
