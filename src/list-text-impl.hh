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

#ifndef TAU_LIST_TEXT_IMPL_HH
#define TAU_LIST_TEXT_IMPL_HH

#include <list-impl.hh>
#include <climits>

namespace tau {

class List_text_impl: public List_impl {
public:

    List_text_impl(Align align=ALIGN_START);

    int  prepend(const ustring & str);
    int  insert(const ustring & str, int row);
    int  insert_before(const ustring & str, const ustring & other);
    int  insert_after(const ustring & str, const ustring & other);
    int  append(const ustring & str);

    void remove_text(const ustring & str);

    void select(int row);
    void select(const ustring & str, bool similar=false);

    void set_text_align(Align align);

    ustring at(int row) const;

    // Overrides List_impl.
    // Overrides Table_impl.
    void clear() override;

    Align text_align() const {
        return align_;
    }

    ustring selection() const {
        return selection_;
    }

    int find(const ustring & str) const;
    bool contains(const ustring & str) const;

    signal<void(int, const ustring &)> & signal_text_selected() {
        return signal_text_selected_;
    }

    signal<void(int, const ustring &)> & signal_text_activated() {
        return signal_text_activated_;
    }

    signal<void(int, const ustring &)> & signal_text_removed() {
        return signal_text_removed_;
    }

    signal<void(int, int, const ustring &)> & signal_text_moved() {
        return signal_text_moved_;
    }

private:

    void on_row_selected(int br);
    void on_row_activated(int br);
    void on_row_removed(int br);
    void on_row_moved(int old_br, int new_br);
    void refresh_selection();

private:

    struct Holder {
        int             row;
        Text_ptr        text;
        ustring         str;

        Holder(int i, Text_ptr tp, const ustring & s): row(i), text(tp), str(s) {}
    };

    using Holders = std::vector<Holder>;

    Align               align_;
    ustring             selection_;
    Holders             holders_;
    int                 selrow_ = INT_MIN;

    signal<void(int, const ustring &)> signal_text_selected_;
    signal<void(int, const ustring &)> signal_text_activated_;
    signal<void(int, const ustring &)> signal_text_removed_;
    signal<void(int, int, const ustring &)> signal_text_moved_;
};

} // namespace tau

#endif // TAU_LIST_TEXT_IMPL_HH
