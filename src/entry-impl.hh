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

#ifndef TAU_ENTRY_IMPL_HH
#define TAU_ENTRY_IMPL_HH

#include <tau/action.hh>
#include <tau/input.hh>
#include <edit-impl.hh>
#include <frame-impl.hh>
#include <scroller-impl.hh>

namespace tau {

class Entry_impl: public Frame_impl {
public:

    Entry_impl(Border_style border_style=BORDER_INSET);
    Entry_impl(Align text_align, Border_style border_style=BORDER_INSET);
    Entry_impl(const ustring & text, Border_style border_style=BORDER_INSET);
    Entry_impl(const ustring & text, Align text_align, Border_style border_style=BORDER_INSET);

    void allow_edit();
    void disallow_edit();
    bool edit_allowed() const;
    void set_text_align(Align align);
    Align text_align() const;
    void assign(const ustring & text);
    ustring text() const;
    const Buffer & buffer() const;
    bool empty() const;
    void clear();
    void select(Buffer_citer b, Buffer_citer e);
    void select_all();
    bool has_selection() const { return edit_->has_selection(); }
    void unselect();
    void move_to(const Buffer_citer & pos);
    void move_to(std::size_t row, std::size_t col);
    Buffer_citer caret() const;
    Size text_size(const ustring & s) { return edit_->text_size(s); }
    void reserve(unsigned width) { scroller_->hint_min_size(width, 0); }
    void limit(unsigned width) { scroller_->hint_max_size(width, 0); }

    void append(Widget_ptr wp, bool shrink=false);
    Widget_ptr append(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);
    void prepend(Widget_ptr wp, bool shrink=false);
    Widget_ptr prepend(const ustring & text, unsigned margin_left=0, unsigned margin_right=0);

    Action & cancel_action() { return edit_->cancel_action(); }
    const Action & cancel_action() const { return edit_->cancel_action(); }

    signal<void(const ustring &)> & signal_activate() { return signal_activate_; }
    signal<void(const ustring &)> & signal_changed() { return signal_changed_; }
    signal<bool(const ustring &)> & signal_validate() { return signal_validate_; }

private:

    Box_ptr         box_;
    Edit_ptr        edit_;
    Scroller_ptr    scroller_;
    ustring         shadow_;
    Buffer          buffer_;
    Action          enter_action_ { KC_ENTER, KM_NONE, fun(this, &Entry_impl::on_enter) };
    connection      changed_cx_;

    signal<void(const ustring &)> signal_changed_;
    signal<bool(const ustring &)> signal_validate_;
    signal<void(const ustring &)> signal_activate_;

private:

    void init(Align text_align);
    void on_enter();
    void on_buffer_changed();
    bool on_mouse_down(int mbt, int mm, const Point & where);
};

} // namespace tau

#endif // TAU_ENTRY_IMPL_HH
