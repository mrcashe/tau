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

#include <tau/dialog.hh>
#include <dialog-impl.hh>
#include <display-impl.hh>

namespace tau {

#define DIALOG_IMPL (std::static_pointer_cast<Dialog_impl>(impl))

Dialog::Dialog(Widget & w, const Rect & bounds):
    Toplevel(nullptr)
{
    Widget_ptr wp = w.ptr();
    auto root = wp->root();
    auto dp = Display_impl::this_display();
    impl = dp->create_dialog(root, bounds);
}

Dialog::Dialog(Widget & w, const ustring & title, const Rect & bounds):
    Toplevel(nullptr)
{
    Widget_ptr wp = w.ptr();
    auto root = wp->root();
    auto dp = Display_impl::this_display();
    impl = dp->create_dialog(root, bounds);
    DIALOG_IMPL->set_title(title);
}

Dialog::Dialog(Toplevel & wnd, const Rect & bounds):
    Toplevel(nullptr)
{
    Toplevel_ptr tpl = std::static_pointer_cast<Toplevel_impl>(wnd.ptr());
    auto dp = Display_impl::this_display();
    impl = dp->create_dialog(tpl.get(), bounds);
}

Dialog::Dialog(Toplevel & wnd, const ustring & title, const Rect & bounds):
    Toplevel(nullptr)
{
    Toplevel_ptr tpl = std::static_pointer_cast<Toplevel_impl>(wnd.ptr());
    auto dp = Display_impl::this_display();
    impl = dp->create_dialog(tpl.get(), bounds);
    DIALOG_IMPL->set_title(title);
}

Dialog::Dialog(Widget_ptr wp):
    Toplevel(std::dynamic_pointer_cast<Dialog_impl>(wp))
{
}

void Dialog::run() {
    DIALOG_IMPL->run();
}

void Dialog::quit() {
    DIALOG_IMPL->quit();
}

bool Dialog::running() const {
    return DIALOG_IMPL->running();
}

} // namespace tau

//END
