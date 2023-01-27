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

#include <card-impl.hh>
#include <cycle-text-impl.hh>
#include <entry-impl.hh>

namespace tau {

Cycle_text_impl::Cycle_text_impl(Border_style bs):
    Cycle_impl(bs),
    align_(ALIGN_CENTER)
{
    card_->hint_margin(2, 2, 0, 0);
}

Cycle_text_impl::Cycle_text_impl(Border_style bs, Align align):
    Cycle_impl(bs),
    align_(align)
{
    card_->hint_margin(2, 2, 0, 0);
}

Cycle_text_impl::Cycle_text_impl(Align align):
    Cycle_impl(BORDER_INSET),
    align_(align)
{
    card_->hint_margin(2, 2, 0, 0);
}

void Cycle_text_impl::set_text_align(Align align) {
    if (align_ != align) {
        align_ = align;
        for (Holder & hol: holders_) { hol.entry->set_text_align(align); }
    }
}

void Cycle_text_impl::allow_edit() {
    if (!edit_allowed_) {
        edit_allowed_ = true;
        for (Holder & hol: holders_) { hol.entry->allow_edit(); }
    }
}

void Cycle_text_impl::disallow_edit() {
    if (edit_allowed_) {
        edit_allowed_ = false;
        for (Holder & hol: holders_) { hol.entry->disallow_edit(); }
    }
}

void Cycle_text_impl::add_text(const ustring & text, const ustring & tooltip) {
    Entry_ptr tp = std::make_shared<Entry_impl>(text, align_, BORDER_NONE);
    tp->set_tooltip(tooltip);
    if (!edit_allowed_) { tp->disallow_edit(); }
    Cycle_impl::add(tp);
    holders_.emplace_back();
    Holder & hol = holders_.back();
    hol.entry = tp.get();
    hol.show_cx = tp->signal_show().connect(tau::bind(fun(this, &Cycle_text_impl::on_child_show), tp.get()));
    hol.hide_cx = tp->signal_show().connect(tau::bind(fun(this, &Cycle_text_impl::on_child_hide), tp.get()));
    tp->signal_activate().connect(fun(this, &Cycle_text_impl::on_text_activate));
    tp->signal_select().connect(tau::bind(fun(this, &Cycle_text_impl::on_text_select), tp.get()));
}

void Cycle_text_impl::select_text(const ustring & text) {
    for (Holder & hol: holders_) {
        ustring tp_text = hol.entry->text();

        if (tp_text == text) {
            current_ = text;
            hol.show_cx.block();
            hol.entry->show();
            hol.show_cx.unblock();
            return;
        }
    }
}

void Cycle_text_impl::remove_text(const ustring & text) {
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        Holder & hol = *i;
        ustring tp_text = hol.entry->text();

        if (tp_text == text) {
            if (text == current_) { current_.clear(); }
            hol.show_cx.drop();
            hol.hide_cx.drop();
            card_->remove(hol.entry);
            holders_.erase(i);
            return;
        }
    }
}

void Cycle_text_impl::on_text_select(const Entry_impl * entry) {
    ustring text = entry->text();

    if (current_ != text) {
        current_ = text;
        signal_selection_changed_(current_);
    }
}

void Cycle_text_impl::on_text_activate(const ustring & text) {
    if (current_ != text) {
        current_ = text;
        signal_selection_changed_(current_);
    }
}

void Cycle_text_impl::on_child_show(Entry_impl * entry) {
    entry->signal_select()();
}

void Cycle_text_impl::on_child_hide(Entry_impl * entry) {
    if (current_ == entry->text()) {
        entry->signal_unselect()();
    }
}

} // namespace tau

//END
