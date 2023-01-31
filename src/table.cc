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

#include <tau/exception.hh>
#include <tau/string.hh>
#include <tau/table.hh>
#include <table-impl.hh>

namespace tau {

#define TABLE_IMPL (std::static_pointer_cast<Table_impl>(impl))

Table::Table():
    Container(std::make_shared<Table_impl>())
{
}

Table::Table(Widget_ptr wp):
    Container(std::dynamic_pointer_cast<Table_impl>(wp))
{
}

Table & Table::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Table_impl>(wp)) {
        throw user_error(str_format(this, " Table::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

void Table::put(Widget & w, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh) {
    TABLE_IMPL->put(w.ptr(), x, y, xspan, yspan, xsh, ysh);
}

Table::Span Table::span() const {
    return TABLE_IMPL->span();
}

void Table::get_column_span(int row, int & xmin, int & xmax) {
    return TABLE_IMPL->get_column_span(row, xmin, xmax);
}

void Table::get_row_span(int col, int & ymin, int & ymax) {
    return TABLE_IMPL->get_row_span(col, ymin, ymax);
}

void Table::set_column_spacing(unsigned xspacing) {
    TABLE_IMPL->set_column_spacing(xspacing);
}

void Table::set_row_spacing(unsigned yspacing) {
    TABLE_IMPL->set_row_spacing(yspacing);
}

void Table::set_spacing(unsigned xspacing, unsigned yspacing) {
    TABLE_IMPL->set_spacing(xspacing, yspacing);
}

unsigned Table::column_spacing() const {
    return TABLE_IMPL->column_spacing();
}

unsigned Table::row_spacing() const {
    return TABLE_IMPL->row_spacing();
}

void Table::remove(Widget & w) {
    TABLE_IMPL->remove(w.ptr().get());
}

void Table::remove(int xmin, int ymin, int xmax, int ymax) {
    TABLE_IMPL->remove(xmin, ymin, xmax, ymax);
}

void Table::respan(Widget & w, int x, int y, unsigned xspan, unsigned yspan) {
    TABLE_IMPL->respan(w.ptr().get(), x, y, xspan, yspan);
}

void Table::respan(Widget & w, int x, int y, unsigned xspan, unsigned yspan, bool xsh, bool ysh) {
    TABLE_IMPL->respan(w.ptr().get(), x, y, xspan, yspan, xsh, ysh);
}

Table::Span Table::span(const Widget & w) const {
    return TABLE_IMPL->span(w.ptr().get());
}

void Table::insert_columns(int x, unsigned n_columns) {
    TABLE_IMPL->insert_columns(x, n_columns);
}

void Table::insert_rows(int y, unsigned n_rows) {
    TABLE_IMPL->insert_rows(y, n_rows);
}

void Table::remove_columns(int x, unsigned n_columns) {
    TABLE_IMPL->remove_columns(x, n_columns);
}

void Table::remove_rows(int y, unsigned n_rows) {
    TABLE_IMPL->remove_rows(y, n_rows);
}

void Table::align_columns(Align xalign) {
    TABLE_IMPL->align_columns(xalign);
}

Align Table::columns_align() const {
    return TABLE_IMPL->columns_align();
}

void Table::align_rows(Align yalign) {
    TABLE_IMPL->align_columns(yalign);
}

Align Table::rows_align() const {
    return TABLE_IMPL->rows_align();
}

void Table::align(Widget & w, Align xalign, Align yalign) {
    TABLE_IMPL->align(w.ptr().get(), xalign, yalign);
}

void Table::unalign(Widget & w) {
    TABLE_IMPL->unalign(w.ptr().get());
}

void Table::align_column(int x, Align column_align) {
    TABLE_IMPL->align_column(x, column_align);
}

Align Table::column_align(int x) const {
    return TABLE_IMPL->column_align(x);
}

void Table::unalign_column(int x) {
    TABLE_IMPL->unalign_column(x);
}

void Table::align_row(int y, Align column_align) {
    TABLE_IMPL->align_row(y, column_align);
}

Align Table::row_align(int y) const {
    return TABLE_IMPL->row_align(y);
}

void Table::unalign_row(int y) {
    TABLE_IMPL->unalign_row(y);
}

void Table::get_align(const Widget & w, Align & xalign, Align & yalign) const {
    TABLE_IMPL->get_align(w.ptr().get(), xalign, yalign);
}

void Table::clear() {
    TABLE_IMPL->clear();
}

void Table::select_row(int nth_row) {
    TABLE_IMPL->select_row(nth_row);
}

void Table::select_column(int nth_col) {
    TABLE_IMPL->select_column(nth_col);
}

void Table::select(int x, int y, unsigned xspan, unsigned yspan) {
    TABLE_IMPL->select(x, y, xspan, yspan);
}

void Table::unselect() {
    TABLE_IMPL->unselect();
}

Table::Span Table::selection() const {
    return TABLE_IMPL->selection();
}

void Table::mark(int x, int y, unsigned width, unsigned height) {
    TABLE_IMPL->mark(x, y, width, height);
}

void Table::mark_column(int x) {
    TABLE_IMPL->mark_column(x);
}

void Table::mark_row(int y) {
    TABLE_IMPL->mark_row(y);
}

void Table::unmark(int xmin, int ymin, int xmax, int ymax) {
    TABLE_IMPL->unmark(xmin, ymin, xmax, ymax);
}

void Table::unmark_column(int x) {
    TABLE_IMPL->unmark_column(x);
}

void Table::unmark_row(int y) {
    TABLE_IMPL->unmark_row(y);
}

void Table::unmark_all() {
    TABLE_IMPL->unmark_all();
}

std::vector<Table::Span> Table::marks() const {
    return TABLE_IMPL->marks();
}

void Table::set_column_margin(int x, unsigned left, unsigned right) {
    TABLE_IMPL->set_column_margin(x, left, right);
}

void Table::set_row_margin(int y, unsigned top, unsigned bottom) {
    TABLE_IMPL->set_row_margin(y, top, bottom);
}

void Table::get_column_margin(int x, unsigned & left, unsigned & right) const {
    TABLE_IMPL->get_column_margin(x, left, right);
}

void Table::get_row_margin(int y, unsigned & top, unsigned & bottom) const {
    TABLE_IMPL->get_row_margin(y, top, bottom);
}

void Table::set_columns_margin(unsigned left, unsigned right) {
    TABLE_IMPL->set_columns_margin(left, right);
}

void Table::set_rows_margin(unsigned top, unsigned bottom) {
    TABLE_IMPL->set_rows_margin(top, bottom);
}

void Table::get_columns_margin(unsigned & left, unsigned & right) const {
    TABLE_IMPL->get_columns_margin(left, right);
}

void Table::get_rows_margin(unsigned & top, unsigned & bottom) const {
    TABLE_IMPL->get_rows_margin(top, bottom);
}

Rect Table::bounds(int x, int y, unsigned xspan, unsigned yspan) const {
    return TABLE_IMPL->bounds(x, y, xspan, yspan);
}

void Table::get_column_bounds(int col, int & xmin, int & xmax) {
    TABLE_IMPL->get_column_bounds(col, xmin, xmax);
}

void Table::get_row_bounds(int row, int & ymin, int & ymax) {
    TABLE_IMPL->get_row_bounds(row, ymin, ymax);
}

void Table::set_column_width(int x, unsigned width) {
    TABLE_IMPL->set_column_width(x, width);
}

unsigned Table::column_width(int x) const {
    return TABLE_IMPL->column_width(x);
}

void Table::set_row_height(int y, unsigned height) {
    TABLE_IMPL->set_row_height(y, height);
}

unsigned Table::row_height(int y) const {
    return TABLE_IMPL->row_height(y);
}

void Table::set_min_column_width(int x, unsigned width) {
    TABLE_IMPL->set_min_column_width(x, width);
}

unsigned Table::min_column_width(int x) const {
    return TABLE_IMPL->min_column_width(x);
}

void Table::set_min_row_height(int y, unsigned height) {
    TABLE_IMPL->set_min_row_height(y, height);
}

unsigned Table::min_row_height(int y) const {
    return TABLE_IMPL->min_row_height(y);
}

void Table::set_max_column_width(int column, unsigned width) {
    TABLE_IMPL->set_max_column_width(column, width);
}

unsigned Table::max_column_width(int column) const {
    return TABLE_IMPL->max_column_width(column);
}

void Table::set_max_row_height(int row, unsigned height) {
    TABLE_IMPL->set_max_row_height(row, height);
}

unsigned Table::max_row_height(int row) const {
    return TABLE_IMPL->max_row_height(row);
}

signal<void(int)> & Table::signal_column_bounds_changed() {
    return TABLE_IMPL->signal_column_bounds_changed();
}

signal<void(int)> & Table::signal_row_bounds_changed() {
    return TABLE_IMPL->signal_row_bounds_changed();
}

signal<void()> & Table::signal_selection_changed() {
    return TABLE_IMPL->signal_selection_changed();
}

} // namespace tau

//END
