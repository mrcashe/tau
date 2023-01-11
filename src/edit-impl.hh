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

#ifndef TAU_EDIT_IMPL_HH
#define TAU_EDIT_IMPL_HH

#include <text-impl.hh>

namespace tau {

class Edit_impl: public Text_impl {
public:

    Edit_impl();
    Edit_impl(Align halign, Align valign=ALIGN_START);
    Edit_impl(const ustring & s, Align halign=ALIGN_START, Align valign=ALIGN_START);
    Edit_impl(Buffer buf, Align halign=ALIGN_START, Align valign=ALIGN_START);

    // Overrides Text_impl.
    void assign(Buffer buf) override;

    // Overrides Text_impl.
    void assign(const ustring & text) override;

    void allow_edit();
    void disallow_edit();
    bool edit_allowed() const { return edit_allowed_; }
    bool can_undo() const;
    bool can_redo() const;
    void enter_text(const ustring & str);
    std::size_t undo_index() const { return undo_index_; }
    void split_undo();

    Action & enter_action() { return enter_action_; }
    Action & cut_action() { return cut_action_; }
    Action & paste_action() { return paste_action_; }
    Action & del_action() { return del_action_; }
    Action & backspace_action() { return backspace_action_; }
    Action & undo_action() { return undo_action_; }
    Action & redo_action() { return redo_action_; }
    Action & tab_action() { return tab_action_; }
    Toggle_action & insert_action() { return insert_action_; }

protected:

    enum Undo_type { UNDO_INSERT, UNDO_REPLACE, UNDO_ERASE };

    struct Undo {
        Undo_type       type;
        std::size_t     row1;
        std::size_t     col1;
        std::size_t     row2;
        std::size_t     col2;
        std::u32string  str1;
        std::u32string  str2;
    };

    using Undoes = std::vector<Undo>;

    Undoes              undo_;
    std::size_t         undo_index_ = 0;
    ustring             newline_ = "\u000a";
    bool                edit_allowed_ = true;
    bool                split_undo_ = false;

    Action              enter_action_ { KC_ENTER, KM_NONE, fun(this, &Edit_impl::enter) };
    Action              backspace_action_ { KC_BACKSPACE, KM_NONE, fun(this, &Edit_impl::backspace) };
    Action              cut_action_ { "<Ctrl>X <Shift>Delete", fun(this, &Edit_impl::cut) };
    Action              paste_action_ { "<Ctrl>V <Shift>Insert", fun(this, &Edit_impl::paste) };
    Action              del_action_ { KC_DELETE, KM_NONE, fun(this, &Edit_impl::del) };
    Action              undo_action_ { "<Alt>BackSpace <Ctrl>Z", fun(this, &Edit_impl::undo) };
    Action              redo_action_ { "<Alt>Enter", fun(this, &Edit_impl::redo) };
    Action              tab_action_ { KC_TAB, KM_NONE, fun(this, &Edit_impl::tab) };
    Toggle_action       insert_action_ { KC_INSERT, KM_NONE, fun(this, &Edit_impl::on_insert) };

protected:

    void init();
    void cutoff_redo();
    void connect_buffer();

    void backspace();
    void enter();
    void tab();
    void cut();
    void paste();
    void undo();
    void redo();

    void del();
    void del_char();
    void del_selection();
    void del_range(Buffer_iter b, Buffer_iter e);

private:

    connection          edit_insert_cx_;
    connection          edit_replace_cx_;
    connection          edit_erase_cx_;
    connection          paste_text_cx_;

private:

    bool on_key_down(char32_t kc, uint32_t km);
    void on_insert(bool replace);
    void on_display();
    void on_paste_text(const ustring & s);
    void on_edit_insert(Buffer_iter b, Buffer_iter e);
    void on_edit_replace(Buffer_iter b, Buffer_iter e, const std::u32string & replaced);
    void on_edit_erase(Buffer_iter b, Buffer_iter e, const std::u32string & erased);
};

} // namespace tau

#endif // TAU_EDIT_IMPL_HH
