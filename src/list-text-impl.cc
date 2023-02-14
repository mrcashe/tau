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

#include <tau/string.hh>
#include <list-text-impl.hh>
#include <text-impl.hh>
#include <iostream>

namespace tau {

List_text_impl::List_text_impl(Align align):
    List_impl(),
    align_(align)
{
    signal_row_selected_.connect(fun(this, &List_text_impl::on_row_selected));
    signal_row_activated_.connect(fun(this, &List_text_impl::on_row_activated));
    signal_row_removed_.connect(fun(this, &List_text_impl::on_row_removed));
    signal_row_moved_.connect(fun(this, &List_text_impl::on_row_moved));
}

void List_text_impl::set_text_align(Align align) {
    if (align_ != align) {
        align_ = align;
        for (Holder & hol: holders_) { hol.text->set_text_align(align_, ALIGN_CENTER); }
    }
}

int List_text_impl::prepend(const ustring & str) {
    Text_ptr tp = std::make_shared<Text_impl>(str, align_, ALIGN_CENTER);
    int row = prepend_row(tp, true);
    holders_.emplace(holders_.begin(), row, tp, str);
    if (1 == holders_.size()) { refresh_selection(); }
    return row;
}

int List_text_impl::insert(const ustring & str, int row) {
    Text_ptr tp = std::make_shared<Text_impl>(str, align_, ALIGN_CENTER);
    row = insert_row(tp, row, true);
    holders_.emplace(holders_.begin(), row, tp, str);
    if (1 == holders_.size()) { refresh_selection(); }
    return row;
}

int List_text_impl::append(const ustring & str) {
    Text_ptr tp = std::make_shared<Text_impl>(str, align_, ALIGN_CENTER);
    int row = append_row(tp);
    holders_.emplace_back(row, tp, str);
    if (1 == holders_.size()) { refresh_selection(); }
    return row;
}

int List_text_impl::insert_before(const ustring & str, const ustring & other) {
    int row = INT_MIN;

    for (const Holder & hol: holders_) {
        if (hol.str == other) {
            row = hol.row;
            break;
        }
    }

    if (INT_MIN == row) { return prepend(str); }
    return insert(str, row);
}

int List_text_impl::insert_after(const ustring & str, const ustring & other) {
    int row = INT_MIN;

    if (!holders_.empty()) {
        for (std::size_t n = holders_.size(); 0 != n; --n) {
            const Holder & hol = holders_[n-1];

            if (hol.str == other) {
                row = hol.row;
                break;
            }
        }
    }

    if (INT_MIN == row) { return append(str); }
    return insert(str, ++row);
}

int List_text_impl::select(const ustring & str, bool similar) {
    for (const Holder & hol: holders_) {
        if (similar) {
            if (str_similar(hol.str, str)) {
                return select(hol.row);
            }
        }

        else {
            if (hol.str == str) {
                return select(hol.row);
            }
        }
    }

    return INT_MIN;
}

ustring List_text_impl::at(int row) const {
    for (const Holder & hol: holders_) {
        if (hol.row == row) {
            return hol.str;
        }
    }

    return ustring();
}

ustring List_text_impl::selection() const {
    int row = selected_row();
    auto i = std::find_if(holders_.begin(), holders_.end(), [row](auto & hol) { return row == hol.row; } );
    return i != holders_.end() ? i->str : ustring();
}

void List_text_impl::on_row_selected(int row) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [row](auto & hol) { return row == hol.row; } );

    if (i != holders_.end()) {
        selrow_ = row;
        if (!i->str.empty()) { signal_text_selected_(row, i->str); }
    }
}

void List_text_impl::on_row_activated(int row) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [row](auto & hol) { return row == hol.row; } );

    if (i != holders_.end()) {
        selrow_ = row;

        if (!i->str.empty()) {
            signal_text_selected_(row, i->str);
            signal_text_activated_(row, i->str);
        }
    }
}

void List_text_impl::on_row_removed(int row) {
    auto i = std::find_if(holders_.begin(), holders_.end(), [row](auto & hol) { return row == hol.row; } );

    if (i != holders_.end()) {
        const ustring str = i->str;
        if (row == selrow_) { selrow_ = INT_MIN; }
        holders_.erase(i);
        signal_text_removed_(row, str);
    }
}

void List_text_impl::on_row_moved(int old_br, int new_br) {
    for (auto i = holders_.begin(); i != holders_.end(); ++i) {
        if (i->row == old_br) {
            i->row = new_br;
            signal_text_moved_(old_br, new_br, i->str);
            break;
        }
    }
}

void List_text_impl::refresh_selection() {
    int row = selected_row();

    if (INT_MIN != row) {
        for (const Holder & hol: holders_) {
            if (hol.row == row) {
                selrow_ = row;
                signal_text_selected_(row, hol.str);
                break;
            }
        }
    }
}

void List_text_impl::remove_text(const ustring & str) {
    bool avail;

    do {
        avail = false;
        auto i = std::find_if(holders_.begin(), holders_.end(), [str](const Holder & hol) { return hol.str == str; });

        if (i != holders_.end()) {
            avail = true;
            remove(i->row);
        }
    } while (avail);
}

// Overrides List_impl.
// Overrides Table_impl.
void List_text_impl::clear() {
    List_impl::clear();
    for (const Holder & hol: holders_) { signal_text_removed_(hol.row, hol.str); }
    holders_.clear();
    selrow_ = INT_MIN;
}

int List_text_impl::find(const ustring & str) const {
    for (const Holder & hol: holders_) {
        if (hol.str == str) {
            return hol.row;
        }
    }

    return INT_MIN;
}

bool List_text_impl::contains(const ustring & str) const {
    return INT_MIN != find(str);
}

} // namespace tau

//END
