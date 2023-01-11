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

#ifndef TAU_CYCLE_TEXT_IMPL_HH
#define TAU_CYCLE_TEXT_IMPL_HH

#include <cycle-impl.hh>

namespace tau {

class Cycle_text_impl: public Cycle_impl {
public:

    Cycle_text_impl(Border_style bs=BORDER_INSET);
    explicit Cycle_text_impl(Align align);
    Cycle_text_impl(Border_style bs, Align align);

    void add_text(const ustring & text, const ustring & tooltip=ustring());
    void select_text(const ustring & text);
    void remove_text(const ustring & text);

    ustring selection() const {
        return current_;
    }

    void set_text_align(Align align);

    Align text_align() const {
        return align_;
    }

    void allow_edit();
    void disallow_edit();

    bool edit_allowed() const {
        return edit_allowed_;
    }

    signal<void(ustring)> & signal_selection_changed() {
        return signal_selection_changed_;
    }

private:

    void on_text_activate(const ustring & s);
    void on_text_select(const Entry_impl * entry);
    void on_child_show(Entry_impl * entry);
    void on_child_hide(Entry_impl * entry);

private:

    struct Holder {
        Entry_impl *        entry;
        connection          show_cx;
        connection          hide_cx;
    };

    using Holders = std::list<Holder>;

    Holders                 holders_;
    ustring                 current_;
    Align                   align_;
    bool                    edit_allowed_ = false;
    signal<void(ustring)>   signal_selection_changed_;
};

} // namespace tau

#endif // TAU_CYCLE_TEXT_IMPL_HH
