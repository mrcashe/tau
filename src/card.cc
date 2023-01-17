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

#include <tau/card.hh>
#include <card-impl.hh>

#define CARD_IMPL (std::static_pointer_cast<Card_impl>(impl))

namespace tau {

Card::Card():
    Container(std::make_shared<Card_impl>())
{
}

Card::Card(Widget_ptr wp):
    Container(std::dynamic_pointer_cast<Card_impl>(wp))
{
}

void Card::insert(Widget & w) {
    CARD_IMPL->insert(w.ptr());
}

void Card::remove_current() {
    CARD_IMPL->remove_current();
}

void Card::remove(Widget & w) {
    CARD_IMPL->remove(w.ptr().get());
}

void Card::clear() {
    CARD_IMPL->clear();
}

void Card::show_next() {
    CARD_IMPL->show_next();
}

void Card::show_previous() {
    CARD_IMPL->show_previous();
}

bool Card::empty() const {
    return CARD_IMPL->empty();
}

} // namespace tau

//END
