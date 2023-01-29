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

#include <tau/list.hh>
#include <list-impl.hh>

#define LIST_IMPL (std::static_pointer_cast<List_impl>(impl))

namespace tau {

List::List():
    Widget(std::make_shared<List_impl>())
{
}

List::List(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<List_impl>(wp))
{
}

int List::prepend_row(Widget & w, bool shrink) {
    return LIST_IMPL->prepend_row(w.ptr(), shrink);
}

int List::prepend_row(Widget & w, Align align) {
    return LIST_IMPL->prepend_row(w.ptr(), align);
}

int List::insert_row(Widget & w, int position, bool shrink) {
    return LIST_IMPL->insert_row(w.ptr(), position, shrink);
}

int List::insert_row(Widget & w, int position, Align align) {
    return LIST_IMPL->insert_row(w.ptr(), position, align);
}

int List::append_row(Widget & w, bool shrink) {
    return LIST_IMPL->append_row(w.ptr(), shrink);
}

int List::append_row(Widget & w, Align align) {
    return LIST_IMPL->append_row(w.ptr(), align);
}

int List::prepend(Widget & w, bool shrink) {
    return LIST_IMPL->prepend(w.ptr(), shrink);
}

int List::prepend(Widget & w, Align align) {
    return LIST_IMPL->prepend(w.ptr(), align);
}

int List::insert(Widget & w, int position, bool shrink) {
    return LIST_IMPL->insert(w.ptr(), position, shrink);
}

int List::insert(Widget & w, int position, Align align) {
    return LIST_IMPL->insert(w.ptr(), position, align);
}

int List::append(Widget & w, bool shrink) {
    return LIST_IMPL->append(w.ptr(), shrink);
}

int List::append(Widget & w, Align align) {
    return LIST_IMPL->append(w.ptr(), align);
}

int List::prepend(int branch, Widget & w, bool shrink) {
    return LIST_IMPL->prepend(branch, w.ptr(), shrink);
}

int List::prepend(int branch, Widget & w, Align align) {
    return LIST_IMPL->prepend(branch, w.ptr(), align);
}

int List::insert(int branch, Widget & w, int position, bool shrink) {
    return LIST_IMPL->insert(branch, w.ptr(), position, shrink);
}

int List::insert(int branch, Widget & w, int position, Align align) {
    return LIST_IMPL->insert(branch, w.ptr(), position, align);
}

int List::append(int branch, Widget & w, bool shrink) {
    return LIST_IMPL->append(branch, w.ptr(), shrink);
}

int List::append(int branch, Widget & w, Align align) {
    return LIST_IMPL->append(branch, w.ptr(), align);
}

int List::select_row(int br) {
    return LIST_IMPL->select_row(br);
}

int List::select_front() {
    return LIST_IMPL->select_front();
}

int List::select_back() {
    return LIST_IMPL->select_back();
}

int List::selected_row() const {
    return LIST_IMPL->selected_row();
}

void List::remove(int br) {
    LIST_IMPL->remove(br);
}

bool List::empty() const {
    return LIST_IMPL->empty();
}

void List::clear() {
    LIST_IMPL->clear();
}

void List::unselect() {
    LIST_IMPL->unselect();
}

void List::allow_multiple_select() {
    LIST_IMPL->allow_multiple_select();
}

void List::disallow_multiple_select() {
    LIST_IMPL->disallow_multiple_select();
}

bool List::multiple_select_allowed() const {
    return LIST_IMPL->multiple_select_allowed();
}

void List::show_headers() {
    LIST_IMPL->show_headers();
}

void List::hide_headers() {
    LIST_IMPL->hide_headers();
}

bool List::headers_visible() const {
    return LIST_IMPL->headers_visible();
}

void List::show_header(int column) {
    LIST_IMPL->show_header(column);
}

void List::show_header(int column, const ustring & title, Align align) {
    LIST_IMPL->show_header(column, title, align);
}

void List::show_header(int column, Widget & w) {
    LIST_IMPL->show_header(column, w.ptr());
}

void List::hide_header(int column) {
    LIST_IMPL->hide_header(column);
}

void List::show_sort_marker(int column, bool descend) {
    LIST_IMPL->show_sort_marker(column, descend);
}

void List::hide_sort_marker() {
    LIST_IMPL->hide_sort_marker();
}

void List::set_column_spacing(unsigned spacing) {
    LIST_IMPL->set_column_spacing(spacing);
}

void List::set_row_spacing(unsigned spacing) {
    LIST_IMPL->set_row_spacing(spacing);
}

unsigned List::column_spacing() const {
    return LIST_IMPL->column_spacing();
}

unsigned List::row_spacing() const {
    return LIST_IMPL->row_spacing();
}

void List::set_column_width(int column, unsigned width) {
    LIST_IMPL->set_column_width(column, width);
}

unsigned List::column_width(int column) const {
    return LIST_IMPL->column_width(column);
}

void List::set_row_height(int row, unsigned height) {
    LIST_IMPL->set_row_height(row, height);
}

unsigned List::row_height(int row) const {
    return LIST_IMPL->row_height(row);
}

void List::set_min_column_width(int column, unsigned width) {
    LIST_IMPL->set_min_column_width(column, width);
}

unsigned List::min_column_width(int column) const {
    return LIST_IMPL->min_column_width(column);
}

void List::set_min_row_height(int row, unsigned height) {
    LIST_IMPL->set_min_row_height(row, height);
}

unsigned List::min_row_height(int row) const {
    return LIST_IMPL->min_row_height(row);
}

void List::set_max_column_width(int column, unsigned width) {
    LIST_IMPL->set_max_column_width(column, width);
}

unsigned List::max_column_width(int column) const {
    return LIST_IMPL->max_column_width(column);
}

void List::set_max_row_height(int row, unsigned height) {
    LIST_IMPL->set_max_row_height(row, height);
}

unsigned List::max_row_height(int row) const {
    return LIST_IMPL->max_row_height(row);
}

void List::set_column_margin(int x, unsigned left, unsigned right) {
    LIST_IMPL->set_column_margin(x, left, right);
}

void List::get_column_margin(int x, unsigned & left, unsigned & right) const {
    LIST_IMPL->get_column_margin(x, left, right);
}

signal<void(int)> & List::signal_row_selected() {
    return LIST_IMPL->signal_row_selected();
}

signal<void(int)> & List::signal_row_activated() {
    return LIST_IMPL->signal_row_activated();
}

signal<void(int)> & List::signal_row_removed() {
    return LIST_IMPL->signal_row_removed();
}

signal<void(int, int)> & List::signal_row_moved() {
    return LIST_IMPL->signal_row_moved();
}

signal<bool(int)> & List::signal_mark_validate() {
    return LIST_IMPL->signal_mark_validate();
}

signal<void(int)> & List::signal_header_click() {
    return LIST_IMPL->signal_header_click();
}

signal<void(int)> & List::signal_header_width_changed() {
    return LIST_IMPL->signal_header_width_changed();
}

} // namespace tau

//END
