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

#include <tau/string.hh>
#include <tau/style.hh>
#include <display-impl.hh>
#include <edit-impl.hh>
#include <iostream>

namespace tau {

Edit_impl::Edit_impl():
    Text_impl(ALIGN_START, ALIGN_START)
{
    init();
}

Edit_impl::Edit_impl(Align halign, Align valign):
    Text_impl(halign, valign)
{
    init();
}

Edit_impl::Edit_impl(const ustring & s, Align halign, Align valign):
    Text_impl(s, halign, valign)
{
    init();
}

Edit_impl::Edit_impl(Buffer buf, Align halign, Align valign):
    Text_impl(buf, halign, valign)
{
    init();
}

void Edit_impl::init() {
    connect_buffer();
    allow_edit();

    style_.redirect("background-whitespace", "background");
    signal_key_down_.connect(fun(this, &Edit_impl::on_key_down));
    signal_display().connect(fun(this, &Edit_impl::on_display));

    connect_action(insert_action_);
    connect_action(cut_action_);
    connect_action(paste_action_);
    connect_action(del_action_);
    connect_action(backspace_action_);
    connect_action(enter_action_);
    connect_action(undo_action_);
    connect_action(redo_action_);
    connect_action(tab_action_);
}

void Edit_impl::connect_buffer() {
    edit_insert_cx_ = buffer_.signal_insert().connect(fun(this, &Edit_impl::on_edit_insert), true);
    edit_replace_cx_ = buffer_.signal_replace().connect(fun(this, &Edit_impl::on_edit_replace), true);
    edit_erase_cx_ = buffer_.signal_erase().connect(fun(this, &Edit_impl::on_edit_erase), true);
}

void Edit_impl::allow_edit() {
    allow_select();
    enable_caret();
    edit_allowed_ = true;
}

void Edit_impl::disallow_edit() {
    edit_allowed_ = false;
    disable_caret();
    disallow_select();
}

// Overrides Text_impl.
void Edit_impl::assign(Buffer buf) {
    edit_insert_cx_.disconnect();
    edit_replace_cx_.disconnect();
    edit_erase_cx_.disconnect();

    undo_.clear();
    undo_index_ = 0;
    Text_impl::assign(buf);
    connect_buffer();
}

// Overrides Text_impl.
void Edit_impl::assign(const ustring & s) {
    undo_.clear();
    undo_index_ = 0;
    Text_impl::assign(s);
}

bool Edit_impl::on_key_down(char32_t kc, uint32_t km) {
    if (edit_allowed_) {
        if (0 == (km & (KM_CONTROL|KM_ALT))) {
            if (char32_is_newline(kc) || char32_is_unicode(kc)) {
                enter_text(ustring(1, kc));
            }
        }

        return true;
    }

    return false;
}

void Edit_impl::enter() {
    if (edit_allowed_) {
        enter_text(newline_);
    }
}

void Edit_impl::del_char() {
    if (edit_allowed_) {
        Buffer_iter e(caret_);
        if (e.eol()) { e.move_forward_line(); }
        else { ++e; }
        del_range(caret_, e);
    }
}

void Edit_impl::del() {
    if (edit_allowed_) {
        if (sel_ && esel_ && esel_ != sel_) { del_selection(); }
        else { del_char(); }
    }
}

void Edit_impl::backspace() {
    if (edit_allowed_) {
        if (has_selection()) {
            del_selection();
        }

        else {
            Buffer_iter i(caret_);
            move_left();
            if (caret_ < i) { del_char(); }
        }
    }
}

void Edit_impl::del_selection() {
    if (edit_allowed_ && sel_ && esel_) {
        del_range(sel_, esel_);
        unselect();
    }
}

void Edit_impl::enter_text(const ustring & str) {
    if (edit_allowed_ && !buffer_.locked() && !str.empty()) {
        del_selection();
        if (insert_) { buffer_.insert(caret_, str); }
        else { buffer_.replace(caret_, str); }
    }
}

void Edit_impl::del_range(Buffer_iter b, Buffer_iter e) {
    if (edit_allowed_ && !buffer_.locked() && b && e && b != e) {
        if (e < b) { std::swap(b, e); }
        unselect();
        cutoff_redo();
        buffer().erase(b, e);
    }
}

void Edit_impl::on_insert(bool replace) {
    if (edit_allowed_) {
        insert_ = !replace;
        signal_caret_motion_();
    }
}

void Edit_impl::cut() {
    if (edit_allowed_) {
        if (sel_ && esel_) {
            if (auto dp = display()) {
                dp->copy_text(selection());
                del_selection();
            }
        }
    }
}

void Edit_impl::paste() {
    if (edit_allowed_) {
        if (auto dp = display()) {
            if (dp->can_paste_text()) {
                del_selection();
                dp->paste_text();
            }
        }
    }
}

void Edit_impl::tab() {
    if (edit_allowed_) {
        enter_text(ustring(1, '\t'));
    }
}

void Edit_impl::on_display() {
    if (auto dp = display()) {
        paste_text_cx_ = dp->signal_paste_text().connect(fun(this, &Edit_impl::on_paste_text));
    }
}

void Edit_impl::on_paste_text(const ustring & s) {
    if (has_focus()) {
        enter_text(s);
    }
}

bool Edit_impl::can_undo() const {
    return edit_allowed_ && 0 != undo_index_;
}

bool Edit_impl::can_redo() const {
    return edit_allowed_ && !undo_.empty() && undo_index_ < undo_.size();
}

void Edit_impl::cutoff_redo() {
    if (can_redo()) {
        undo_.erase(undo_.begin()+undo_index_, undo_.end());
    }
}

void Edit_impl::undo() {
    if (edit_allowed_) {
        if (can_undo()) {
            Undo & u = undo_[--undo_index_];
            edit_insert_cx_.block();
            edit_replace_cx_.block();
            edit_erase_cx_.block();

            if (UNDO_ERASE == u.type) {
                buffer_.insert(buffer_.iter(u.row1, u.col1), u.str1);
            }

            else if (UNDO_INSERT == u.type) {
                buffer_.erase(buffer_.iter(u.row1, u.col1), buffer_.iter(u.row2, u.col2));
            }

            else if (UNDO_REPLACE == u.type) {
                buffer_.replace(buffer_.iter(u.row1, u.col1), u.str1);
            }

            edit_insert_cx_.unblock();
            edit_replace_cx_.unblock();
            edit_erase_cx_.unblock();
            signal_changed_();
        }
    }
}

void Edit_impl::redo() {
    if (edit_allowed_) {
        if (can_redo()) {
            Undo & u = undo_[undo_index_++];
            edit_insert_cx_.block();
            edit_replace_cx_.block();
            edit_erase_cx_.block();

            if (UNDO_ERASE == u.type) {
                Buffer_iter b = buffer_.iter(u.row1, u.col1), e = buffer_.iter(u.row2, u.col2);

                if (buffer_.length(b, e) != u.str1.size()) {
                    e = b;

                    for (char32_t wc: u.str1) {
                        if (wc != *e) { break; }
                        ++e;
                    }
                }

                buffer_.erase(b, e);
            }

            else if (UNDO_INSERT == u.type) {
                buffer_.insert(buffer_.iter(u.row1, u.col1), u.str1);
            }

            else if (UNDO_REPLACE == u.type) {
                buffer_.replace(buffer_.iter(u.row1, u.col1), u.str2);
            }

            edit_insert_cx_.unblock();
            edit_replace_cx_.unblock();
            edit_erase_cx_.unblock();
            signal_changed_();
        }
    }
}

void Edit_impl::split_undo() {
    split_undo_ = true;
}

void Edit_impl::on_edit_insert(Buffer_iter b, Buffer_iter e) {
    std::u32string str = buffer_.text32(b, e);

    if (!str.empty()) {
        cutoff_redo();

        if (!split_undo_ && !undo_.empty() && UNDO_INSERT == undo_.back().type && b.row() == undo_.back().row2 && b.col() == undo_.back().col2) {
            Undo & u = undo_.back();
            u.str1 += str;
            u.row2 = e.row();
            u.col2 = e.col();
        }

        else {
            undo_.emplace_back();
            Undo & u = undo_.back();
            u.row1 = b.row();
            u.col1 = b.col();
            u.row2 = e.row();
            u.col2 = e.col();
            u.str1 = str;
            u.type = UNDO_INSERT;
            split_undo_ = false;
            ++undo_index_;
        }
    }
}

void Edit_impl::on_edit_replace(Buffer_iter b, Buffer_iter e, const std::u32string & replaced) {
    std::u32string str = buffer_.text32(b, e);

    if (!str.empty()) {
        cutoff_redo();

        if (!split_undo_ && !undo_.empty() && UNDO_REPLACE == undo_.back().type && b.row() == undo_.back().row2 && b.col() == undo_.back().col2) {
            Undo & u = undo_.back();
            u.str1 += replaced;
            u.str2 += str;
            u.row2 = e.row();
            u.col2 = e.col();
        }

        else {
            undo_.emplace_back();
            Undo & u = undo_.back();
            u.row1 = b.row();
            u.col1 = b.col();
            u.row2 = e.row();
            u.col2 = e.col();
            u.str1 = replaced;
            u.str2 = str;
            u.type = UNDO_REPLACE;
            split_undo_ = false;
            ++undo_index_;
        }
    }
}

void Edit_impl::on_edit_erase(Buffer_iter b, Buffer_iter e, const std::u32string & erased) {
    cutoff_redo();

    if (!split_undo_ && !undo_.empty() && UNDO_ERASE == undo_.back().type) {
        Undo & u = undo_.back();

        if (e.row() == u.row2 && e.col() == u.col2) {
            u.str1 += erased;
            return;
        }

        if (e.row() == u.row1 && e.col() == u.col1) {
            u.str1 = erased+u.str1;
            u.row1 = b.row();
            u.col1 = b.col();
            return;
        }
    }

    undo_.emplace_back();
    Undo & u = undo_.back();
    u.row1 = b.row();
    u.col1 = b.col();
    u.row2 = e.row();
    u.col2 = e.col();
    u.str1 = erased;
    u.type = UNDO_ERASE;
    split_undo_ = false;
    ++undo_index_;
}

} // namespace tau

//END
