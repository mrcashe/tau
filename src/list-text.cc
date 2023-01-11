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

#include <tau/list-text.hh>
#include <list-text-impl.hh>

#define LIST_TEXT_IMPL (std::static_pointer_cast<List_text_impl>(impl))

namespace tau {

List_text::List_text(Align align):
    Widget(std::make_shared<List_text_impl>(align))
{
}

void List_text::set_text_align(Align align) {
    LIST_TEXT_IMPL->set_text_align(align);
}

Align List_text::text_align() const {
    return LIST_TEXT_IMPL->text_align();
}

int List_text::prepend(const ustring & str) {
    return LIST_TEXT_IMPL->prepend(str);
}

int List_text::insert(const ustring & str, int row) {
    return LIST_TEXT_IMPL->insert(str, row);
}

int List_text::insert_before(const ustring & str, const ustring & other) {
    return LIST_TEXT_IMPL->insert_before(str, other);
}

int List_text::insert_after(const ustring & str, const ustring & other) {
    return LIST_TEXT_IMPL->insert_after(str, other);
}

int List_text::append(const ustring & str) {
    return LIST_TEXT_IMPL->append(str);
}

void List_text::select(int row) {
    LIST_TEXT_IMPL->select(row);
}

void List_text::select(const ustring & str, bool similar) {
    LIST_TEXT_IMPL->select(str, similar);
}

int List_text::select_first() {
    return LIST_TEXT_IMPL->select_first();
}

int List_text::select_last() {
    return LIST_TEXT_IMPL->select_last();
}

void List_text::remove(int row) {
    LIST_TEXT_IMPL->remove(row);
}

void List_text::remove_text(const ustring & str) {
    LIST_TEXT_IMPL->remove_text(str);
}

ustring List_text::at(int row) const {
    return LIST_TEXT_IMPL->at(row);
}

bool List_text::empty() const {
    return LIST_TEXT_IMPL->empty();
}

void List_text::clear() {
    LIST_TEXT_IMPL->clear_list();
}

void List_text::unselect() {
    LIST_TEXT_IMPL->unselect();
}

void List_text::allow_multiple_select() {
    LIST_TEXT_IMPL->allow_multiple_select();
}

void List_text::disallow_multiple_select() {
    LIST_TEXT_IMPL->disallow_multiple_select();
}

bool List_text::multiple_select_allowed() const {
    return LIST_TEXT_IMPL->multiple_select_allowed();
}

void List_text::show_headers() {
    LIST_TEXT_IMPL->show_headers();
}

void List_text::hide_headers() {
    LIST_TEXT_IMPL->hide_headers();
}

bool List_text::headers_visible() const {
    return LIST_TEXT_IMPL->headers_visible();
}

void List_text::show_header(int column) {
    LIST_TEXT_IMPL->show_header(column);
}

void List_text::show_header(int column, const ustring & title, Align align) {
    LIST_TEXT_IMPL->show_header(column, title, align);
}

void List_text::show_header(int column, Widget & w) {
    LIST_TEXT_IMPL->show_header(column, w.ptr());
}

void List_text::hide_header(int column) {
    LIST_TEXT_IMPL->hide_header(column);
}

void List_text::show_sort_marker(int column, bool descend) {
    LIST_TEXT_IMPL->show_sort_marker(column, descend);
}

void List_text::hide_sort_marker() {
    LIST_TEXT_IMPL->hide_sort_marker();
}

void List_text::set_column_spacing(unsigned spacing) {
    LIST_TEXT_IMPL->set_column_spacing(spacing);
}

void List_text::set_row_spacing(unsigned spacing) {
    LIST_TEXT_IMPL->set_row_spacing(spacing);
}

unsigned List_text::column_spacing() const {
    return LIST_TEXT_IMPL->column_spacing();
}

unsigned List_text::row_spacing() const {
    return LIST_TEXT_IMPL->row_spacing();
}

void List_text::set_column_width(int column, unsigned width) {
    LIST_TEXT_IMPL->set_column_width(column, width);
}

unsigned List_text::column_width(int column) const {
    return LIST_TEXT_IMPL->column_width(column);
}

void List_text::set_row_height(int row, unsigned height) {
    LIST_TEXT_IMPL->set_row_height(row, height);
}

unsigned List_text::row_height(int row) const {
    return LIST_TEXT_IMPL->row_height(row);
}

void List_text::set_min_column_width(int column, unsigned width) {
    LIST_TEXT_IMPL->set_min_column_width(column, width);
}

unsigned List_text::min_column_width(int column) const {
    return LIST_TEXT_IMPL->min_column_width(column);
}

void List_text::set_min_row_height(int row, unsigned height) {
    LIST_TEXT_IMPL->set_min_row_height(row, height);
}

unsigned List_text::min_row_height(int row) const {
    return LIST_TEXT_IMPL->min_row_height(row);
}

void List_text::set_max_column_width(int column, unsigned width) {
    LIST_TEXT_IMPL->set_max_column_width(column, width);
}

unsigned List_text::max_column_width(int column) const {
    return LIST_TEXT_IMPL->max_column_width(column);
}

void List_text::set_max_row_height(int row, unsigned height) {
    LIST_TEXT_IMPL->set_max_row_height(row, height);
}

unsigned List_text::max_row_height(int row) const {
    return LIST_TEXT_IMPL->max_row_height(row);
}

void List_text::set_column_margin(int x, unsigned left, unsigned right) {
    LIST_TEXT_IMPL->set_column_margin(x, left, right);
}

void List_text::get_column_margin(int x, unsigned & left, unsigned & right) const {
    LIST_TEXT_IMPL->get_column_margin(x, left, right);
}

ustring List_text::selection() const {
    return LIST_TEXT_IMPL->selection();
}

int List_text::find(const ustring & str) const {
    return LIST_TEXT_IMPL->find(str);
}

bool List_text::contains(const ustring & str) const {
    return LIST_TEXT_IMPL->contains(str);
}

signal<void(int, ustring)> & List_text::signal_text_selected() {
    return LIST_TEXT_IMPL->signal_text_selected();
}

signal<void(int, ustring)> & List_text::signal_text_activated() {
    return LIST_TEXT_IMPL->signal_text_activated();
}

signal<void(int, ustring)> & List_text::signal_text_removed() {
    return LIST_TEXT_IMPL->signal_text_removed();
}

signal<void(int, int, ustring)> & List_text::signal_text_moved() {
    return LIST_TEXT_IMPL->signal_text_moved();
}

signal<bool(int)> & List_text::signal_mark_validate() {
    return LIST_TEXT_IMPL->signal_mark_validate();
}

signal<void(int)> & List_text::signal_header_click() {
    return LIST_TEXT_IMPL->signal_header_click();
}

signal<void(int)> & List_text::signal_header_width_changed() {
    return LIST_TEXT_IMPL->signal_header_width_changed();
}

} // namespace tau

//END
