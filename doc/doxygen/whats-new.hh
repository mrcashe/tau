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

namespace tau {

/**

@page whats_new Whats New in 0.4.0

@section broken_text_methods_040 Broken Text methods

- Text::move_left_action() -> renamed to next_char_action()
- Text::move_right_action() -> renamed to next_char_action()
- Text::move_up_action() -> renamed to previous_line_action()
- Text::move_down_action() -> renamed to next_line_action()
- Text::select_left_action() -> renamed to select_previous_char_action()
- Text::select_right_action() -> renamed to select_next_char_action()
- Text::select_word_left_action() ->renamed to select_previous_word_action()
- Text::move_word_right_action() -> renamed to next_word_action()
- Text::select_word_right_action() -> renamed to select_next_word_action()
- Text::page_up_action() -> renamed to Text::previous_page_action()
- Text::page_down_action() -> renamed to Text::next_page_action()
- Text::select_up_action() -> renamed to Text::select_previous_line_action()
- Text::select_down_action() -> renamed to Text::select_next_line_action()
- Text::select_previous_page_action() -> renamed to Text::select_previous_page_action()
- Text::select_next_page_action() -> renamed to Text::select_next_page_action()

@section broken_list_methods_040 Broken List and List_text methods

- int List::select_row(int row) -> renamed to int List::select(int row).
- void List_text::select(int row) -> returned value changed to int.
- void List_text::select(const ustring & str, bool similar) -> returned value changed to int.

@section broken_list_methods_040 Broken Scroller methods

- Scroller::page_down_action() -> renamed to Scroller::next_page_action()
- Scroller::page_up_action() -> renamed to Scroller::previous_page_action()

@section new_widget_assignments_040 New Widget assignment operators

- Absolute::operator=(Widget_ptr wp)
- Bin::operator=(Widget_ptr wp)
- Box::operator=(Widget_ptr wp)
- Button::operator=(Widget_ptr wp)
- Toggle::operator=(Widget_ptr wp)
- Card::operator=(Widget_ptr wp)
- Check::operator=(Widget_ptr wp)
- Colorsel::operator=(Widget_ptr wp)
- Counter::operator=(Widget_ptr wp)
- Cycle_text::operator=(Widget_ptr wp)
- Cycle::operator=(Widget_ptr wp)
- Dialog::operator=(Widget_ptr wp)
- Edit::operator=(Widget_ptr wp)
- Entry::operator=(Widget_ptr wp)
- Fileman::operator=(Widget_ptr wp)
- Fontsel::operator=(Widget_ptr wp)
- Frame::operator=(Widget_ptr wp)
- Icon::operator=(Widget_ptr wp)
- Image::operator=(Widget_ptr wp)
- List_text::operator=(Widget_ptr wp)
- List::operator=(Widget_ptr wp)
- Action_menu_item::operator=(Widget_ptr wp)
- Toggle_menu_item::operator=(Widget_ptr wp)
- Submenu_item::operator=(Widget_ptr wp)
- Slot_menu_item::operator=(Widget_ptr wp)
- Check_menu_item::operator=(Widget_ptr wp)
- Menubox::operator=(Widget_ptr wp)
- Menubar::operator=(Widget_ptr wp)
- Navigator::operator=(Widget_ptr wp)
- Notebook::operator=(Widget_ptr wp)
- Popup::operator=(Widget_ptr wp)
- Progress::operator=(Widget_ptr wp)
- Roller::operator=(Widget_ptr wp)
- Scroller::operator=(Widget_ptr wp)
- Separator::operator=(Widget_ptr wp)
- Slider::operator=(Widget_ptr wp)
- Table::operator=(Widget_ptr wp)
- Text::operator=(Widget_ptr wp)
- Toplevel::operator=(Widget_ptr wp)
- Twins::operator=(Widget_ptr wp)
- Widget::operator=(Widget_ptr wp)

@section new_widget_methods_040 New Widget methods

- Widget::has_parent(const Widget & w) const

@section new_table_methods_040 New Table methods

- Table::Table(unsigned xspacing, unsigned yspacing)
- Table::Table(unsigned spacing)
- Table::set_spacing(unsigned spacing)
- Table::has_selection()
- Table::has_marks()

@section new_list_methods_040 New List & List_text methods

- List::select_next()
- List::select_previous()
- List::remove(Widget & w)
- List::cancel_action()
- List::enter_action()
- List::previous_action()
- List::next_action()
- List::previous_page_action()
- List::next_page_action()
- List::home_action()
- List::end_action()
- List::select_previous_action()
- List::select_next_action()
- List::select_previous_page_action()
- List::select_next_page_action()
- List::select_home_action()
- List::select_end_action()

- List_text::select_next()
- List_text::select_previous()
- List_text::cancel_action()
- List_text::enter_action()
- List_text::previous_action()
- List_text::next_action()
- List_text::previous_page_action()
- List_text::next_page_action()
- List_text::home_action()
- List_text::end_action()
- List_text::select_previous_action()
- List_text::select_next_action()
- List_text::select_previous_page_action()
- List_text::select_next_page_action()
- List_text::select_home_action()
- List_text::select_end_action()

@section new_roller_methods_040 New Roller methods

- Roller::roll_to(Widget & w)

@section new_action_methods_040 New Action and Toggle_action methods

- Action::connected()
- Toggle_action::connected()

@section new_style_methods_040 New Style methods

- Style::signal_changed()
- Style::unredirect()

@section new_keyfile_methods_040 New Key_file methods

- Key_file::flush()
- Key_file::changed()
- Key_file::comment()
- Key_file::get_string(const ustring & key, const ustring & fallback)
- Key_file::get_strings(const ustring & key)
- Key_file::get_boolean(const ustring & key, bool fallback)
- Key_file::get_booleans(const ustring & key)
- Key_file::get_integer(const ustring & key, long long fallback)
- Key_file::get_integers(const ustring & key)
- Key_file::get_double(const ustring & key, double fallback)
- Key_file::get_doubles(const ustring & key)
- Key_file::set_comment(const ustring & comment)
- Key_file::set_string(const ustring & key, const ustring & value)
- Key_file::set_strings(const ustring & key, const std::vector<ustring> & vec)
- Key_file::set_boolean(const ustring & key, bool value)
- Key_file::set_booleans(const ustring & key, const std::vector<bool> & vec)
- Key_file::set_integer(const ustring & key, long long value)
- Key_file::set_integers(const ustring & key, const std::vector<long long> & vec)
- Key_file::set_double(const ustring & key, double value)
- Key_file::set_doubles(const ustring & key, const std::vector<double> & vec)
- Key_file::has_key(const ustring & key_name, bool similar) const
- Key_file::key_name(const ustring & similar_name) const
- Key_file::remove_key(const ustring & key_name, bool similar)

@section new_color_methods_040 New Color methods

- Color::alpha_blend()
- Color::alpha_blended()

@section new_buffer_citer_methods_040 New Buffer_citer methods

- Buffer_citer::length()
- Buffer_citer::move_to(Buffer_citer other)

@section new_icons_040 New constant access strings with standard icon names

- @link ICON_DOCUMENT_NEW                 @endlink
- @link ICON_DOCUMENT_OPEN_RECENT         @endlink
- @link ICON_DOCUMENT_OPEN                @endlink
- @link ICON_DOCUMENT_SAVE_ALL            @endlink
- @link ICON_DOCUMENT_SAVE_AS             @endlink
- @link ICON_DOCUMENT_SAVE                @endlink

**/

} // namespace tau

