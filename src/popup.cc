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
#include <tau/string.hh>
#include <tau/popup.hh>
#include <tau/toplevel.hh>
#include <display-impl.hh>
#include <popup-impl.hh>
#include <toplevel-impl.hh>

namespace tau {

#define POPUP_IMPL (std::static_pointer_cast<Popup_impl>(impl))

Popup::Popup(Toplevel & tp, const Point & origin, Gravity gravity):
    Window(nullptr)
{
    Toplevel_ptr tpl = std::static_pointer_cast<Toplevel_impl>(tp.ptr());
    auto dp = Display_impl::this_display();
    impl = dp->create_popup(dp, tpl.get(), origin, gravity);
}

Popup::Popup(Widget_ptr wp):
    Window(std::dynamic_pointer_cast<Popup_impl>(wp))
{
}

Popup & Popup::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Popup_impl>(wp)) {
        throw user_error(str_format(this, " Popup::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

void Popup::close() {
    POPUP_IMPL->close();
}

void Popup::set_gravity(Gravity gravity) {
    POPUP_IMPL->set_gravity(gravity);
}

Gravity Popup::gravity() const {
    return POPUP_IMPL->gravity();
}

} // namespace tau

//END
