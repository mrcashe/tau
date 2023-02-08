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
#include <box-impl.hh>
#include <entry-impl.hh>
#include <iostream>

namespace tau {

Entry_impl::Entry_impl(Border_style border_style):
    Frame_impl(border_style)
{
    init(ALIGN_START);
}

Entry_impl::Entry_impl(Align text_align, Border_style border_style):
    Frame_impl(border_style)
{
    init(text_align);
}

Entry_impl::Entry_impl(const ustring & text, Border_style border_style):
    Frame_impl(border_style)
{
    init(ALIGN_START);
    assign(text);
}

Entry_impl::Entry_impl(const ustring & text, Align text_align, Border_style border_style):
    Frame_impl(border_style)
{
    init(text_align);
    assign(text);
}

void Entry_impl::init(Align text_align) {
    box_ = std::make_shared<Box_impl>(OR_RIGHT);
    insert(box_);
    if (BORDER_NONE != border_left_style()) { box_->hint_margin(1); }

    box_->style().redirect(STYLE_WHITESPACE_BACKGROUND, STYLE_BACKGROUND);
    box_->focus_next_action().disable();
    box_->focus_previous_action().disable();

    scroller_ = std::make_shared<Scroller_impl>();
    box_->append(scroller_);
    scroller_->pan_left_action().disable();
    scroller_->pan_right_action().disable();
    scroller_->pan_up_action().disable();
    scroller_->pan_down_action().disable();
    scroller_->page_up_action().disable();
    scroller_->page_down_action().disable();
    scroller_->home_action().disable();
    scroller_->end_action().disable();

    edit_ = std::make_shared<Edit_impl>(buffer_, text_align, ALIGN_CENTER);
    edit_->style().unset(STYLE_BACKGROUND);
    scroller_->insert(edit_);
    edit_->enter_action().disable();
    edit_->tab_action().disable();
    edit_->move_page_down_action().disable();
    edit_->move_page_up_action().disable();
    edit_->move_down_action().disable();
    edit_->move_up_action().disable();
    changed_cx_ = buffer_.signal_changed().connect(fun(this, &Entry_impl::on_buffer_changed));
    edit_->signal_focus_out().connect(fun(edit_, &Edit_impl::unselect));
    edit_->cancel_action().connect(fun(this, &Widget_impl::drop_focus));

    connect_action(enter_action_);
    signal_take_focus().connect(fun(edit_, &Widget_impl::take_focus), true);
    box_->signal_mouse_down().connect(fun(this, &Entry_impl::on_mouse_down), true);
}

void Entry_impl::assign(const ustring & s) {
    ustring::size_type eol = s.find_first_of(str_newlines());
    ustring sh = ustring::npos != eol ? s.substr(0, eol) : s;
    changed_cx_.block();
    buffer_.assign(sh);
    shadow_ = sh;
    changed_cx_.unblock();
}

void Entry_impl::allow_edit() {
    edit_->allow_edit();
}

void Entry_impl::disallow_edit() {
    edit_->disallow_edit();
}

bool Entry_impl::edit_allowed() const {
    return edit_->edit_allowed();
}

void Entry_impl::set_text_align(Align align) {
    edit_->set_text_align(align, ALIGN_CENTER);
}

Align Entry_impl::text_align() const {
    Align xalign, yalign;
    edit_->get_text_align(xalign, yalign);
    return xalign;
}

ustring Entry_impl::text() const {
    return edit_->text();
}

const Buffer & Entry_impl::buffer() const {
    return buffer_;
}

bool Entry_impl::empty() const {
    return edit_->empty();
}

void Entry_impl::clear() {
    edit_->clear();
}

void Entry_impl::select_all() {
    edit_->select_all();
}

void Entry_impl::select(Buffer_citer b, Buffer_citer e) {
    edit_->select(b, e);
}

void Entry_impl::unselect() {
    edit_->unselect();
}

void Entry_impl::move_to(const Buffer_citer & pos) {
    edit_->move_to(pos);
}

void Entry_impl::move_to(std::size_t row, std::size_t col) {
    edit_->move_to(row, col);
}

Buffer_citer Entry_impl::caret() const {
    return edit_->caret();
}

void Entry_impl::append(Widget_ptr wp, bool shrink) {
    box_->prepend(wp, shrink);
    wp->disallow_focus();
}

Widget_ptr Entry_impl::append(const ustring & text, unsigned margin_left, unsigned margin_right) {
    Text_ptr tp = std::make_shared<Text_impl>(text);
    tp->hint_margin_left(margin_left);
    tp->hint_margin_right(margin_right);
    box_->append(tp, true);
    return tp;
}

void Entry_impl::prepend(Widget_ptr wp, bool shrink) {
    box_->append(wp, shrink);
    wp->disallow_focus();
}

Widget_ptr Entry_impl::prepend(const ustring & text, unsigned margin_left, unsigned margin_right) {
    Text_ptr tp = std::make_shared<Text_impl>(text);
    tp->hint_margin_left(margin_left);
    tp->hint_margin_right(margin_right);
    box_->prepend(tp, true);
    return tp;
}

void Entry_impl::on_buffer_changed() {
    ustring s = edit_->text();

    if (signal_validate_(s)) {
        assign(shadow_);
    }

    else {
        shadow_ = s;
        signal_changed_(s);
    }
}

void Entry_impl::on_enter() {
    grab_focus();
    signal_activate_(edit_->text());
}

bool Entry_impl::on_mouse_down(int mbt, int mm, const Point & where) {
    if (MBT_LEFT == mbt) {
        if (!edit_->focused()) {
            if (edit_->has_selection()) { edit_->unselect(); }
            else { edit_->select_all(); edit_->grab_focus(); }
        }

        return true;
    }

    return false;
}

} // namespace tau

//END
