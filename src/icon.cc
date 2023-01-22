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

#include <tau/icon.hh>
#include <icon-impl.hh>

namespace tau {

#define ICON_IMPL (std::static_pointer_cast<Icon_impl>(impl))

Icon::Icon():
    Widget(std::make_shared<Icon_impl>())
{
}

Icon::Icon(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Icon_impl>(wp))
{
}

Icon::Icon(const ustring & icon_name, int icon_size):
    Widget(std::make_shared<Icon_impl>(icon_name, icon_size))
{
}

Icon::Icon(Action_base & action, int icon_size, Action_items items):
    Widget(std::make_shared<Icon_impl>(action, icon_size, items))
{
}

void Icon::assign(const ustring & icon_name, int icon_size) {
    ICON_IMPL->assign(icon_name, icon_size);
}

void Icon::set_icon_name(const ustring & icon_name) {
    ICON_IMPL->set_icon_name(icon_name);
}

ustring Icon::icon_name() const {
    return ICON_IMPL->icon_name();
}

void Icon::set_icon_size(unsigned icon_size) {
    ICON_IMPL->set_icon_size(icon_size);
}

int Icon::icon_size() const {
    return ICON_IMPL->icon_size();
}

} // namespace tau

//END
