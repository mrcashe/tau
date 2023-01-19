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

#ifndef TAU_CARD_IMPL_HH
#define TAU_CARD_IMPL_HH

#include <container-impl.hh>

namespace tau {

class Card_impl: public Container_impl {
public:

    Card_impl();
   ~Card_impl() { destroy_ = true; }

    void insert(Widget_ptr wp);
    void remove_current();
    void remove(Widget_impl * wi);
    void clear();
    bool empty() const;
    void show_next();
    void show_previous();
    Widget_impl * current();

private:

    struct Holder {
        Widget_impl *   wp;
        connection      req_cx;
        connection      hints_cx;
        connection      show_cx;
        connection      hide_cx;
    };

    using Holders = std::list<Holder>;

    Holders             holders_;
    Widget_impl *       showing_ = nullptr;
    Widget_impl *       hiding_  = nullptr;

private:

    Size child_requisition(const Holder & hol);
    void arrange();
    void update_requisition();
    bool on_take_focus();
    void rm_child(Holder & hol);

    void on_child_hide(Widget_impl * wi);
    void on_child_show(Widget_impl * wi);
};

} // namespace tau

#endif // TAU_CARD_IMPL_HH
