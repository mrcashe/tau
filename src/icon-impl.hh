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

#ifndef TAU_ICON_IMPL_HH
#define TAU_ICON_IMPL_HH

#include <tau/enums.hh>
#include <tau/pixmap.hh>
#include <image-impl.hh>

namespace tau {

class Icon_impl: public Image_impl {
public:

    Icon_impl();
    Icon_impl(const ustring & icon_name, unsigned icon_size);
    Icon_impl(Action_base & action, unsigned icon_size);

    void assign(const ustring & icon_name, unsigned icon_size);
    void set_icon_name(const ustring & icon_name);
    ustring icon_name() const;
    void set_icon_size(unsigned icon_size);
    unsigned icon_size() const;

private:

    void init();
    void init_action(Action_base & action);
    void on_display();
    void update_pixmap();
    void on_action_tooltip(const ustring & tooltip);

private:

    ustring     icon_name_;
    unsigned   icon_size_;
};

} // namespace tau

#endif // TAU_ICON_IMPL_HH
