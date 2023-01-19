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

#include <tau/notebook.hh>
#include <notebook-impl.hh>

#define NOTEBOOK_IMPL (std::static_pointer_cast<Notebook_impl>(impl))

namespace tau {

Notebook::Notebook(Tab_pos tab_pos):
    Widget(std::make_shared<Notebook_impl>(tab_pos))
{
}

Notebook::Notebook(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Notebook_impl>(wp))
{
}

int Notebook::append_page(Widget & w) {
    return NOTEBOOK_IMPL->append_page(w.ptr());
}

int Notebook::append_page(Widget & w, const ustring & title) {
    return NOTEBOOK_IMPL->append_page(w.ptr(), title);
}

int  Notebook::append_page(Widget & w, Widget & title_widget) {
    return NOTEBOOK_IMPL->append_page(w.ptr(), title_widget.ptr());
}

void Notebook::prepend_page(Widget & w) {
    NOTEBOOK_IMPL->prepend_page(w.ptr());
}

void Notebook::prepend_page(Widget & w, const ustring & title) {
    NOTEBOOK_IMPL->prepend_page(w.ptr(), title);
}

void Notebook::prepend_page(Widget & w, Widget & title_widget) {
    NOTEBOOK_IMPL->prepend_page(w.ptr(), title_widget.ptr());
}

int Notebook::insert_page(Widget & w, int nth_page) {
    return NOTEBOOK_IMPL->insert_page(w.ptr(), nth_page);
}

int Notebook::insert_page(Widget & w, int nth_page, const ustring & title) {
    return NOTEBOOK_IMPL->insert_page(w.ptr(), nth_page, title);
}

int Notebook::insert_page(Widget & w, int nth_page, Widget & title_widget) {
    return NOTEBOOK_IMPL->insert_page(w.ptr(), nth_page, title_widget.ptr());
}

int Notebook::insert_page_after(Widget & w, Widget & after_this) {
    return NOTEBOOK_IMPL->insert_page_after(w.ptr(), after_this.ptr());
}

int Notebook::insert_page_after(Widget & w, Widget & after_this, const ustring & title) {
    return NOTEBOOK_IMPL->insert_page_after(w.ptr(), after_this.ptr(), title);
}

int Notebook::insert_page_after(Widget & w, Widget & after_this, Widget & title_widget) {
    return NOTEBOOK_IMPL->insert_page_after(w.ptr(), after_this.ptr(), title_widget.ptr());
}

int Notebook::insert_page_before(Widget & w, Widget & before_this) {
    return NOTEBOOK_IMPL->insert_page_before(w.ptr(), before_this.ptr());
}

int Notebook::insert_page_before(Widget & w, Widget & before_this, const ustring & title) {
    return NOTEBOOK_IMPL->insert_page_before(w.ptr(), before_this.ptr(), title);
}

int Notebook::insert_page_before(Widget & w, Widget & before_this, Widget & title_widget) {
    return NOTEBOOK_IMPL->insert_page_before(w.ptr(), before_this.ptr(), title_widget.ptr());
}

int Notebook::remove_page(Widget & w) {
    return NOTEBOOK_IMPL->remove_page(w.ptr());
}

int Notebook::remove_page(int page) {
    return NOTEBOOK_IMPL->remove_page(page);
}

bool Notebook::empty() const {
    return NOTEBOOK_IMPL->empty();
}

void Notebook::clear() {
    NOTEBOOK_IMPL->clear_pages();
}

int Notebook::show_next() {
    return NOTEBOOK_IMPL->show_next();
}

int Notebook::show_previous() {
    return NOTEBOOK_IMPL->show_previous();
}

int Notebook::show_page(int nth_page) {
    return NOTEBOOK_IMPL->show_page(nth_page);
}

int Notebook::current_page() const {
    return NOTEBOOK_IMPL->current_page();
}

std::size_t Notebook::page_count() const {
    return NOTEBOOK_IMPL->page_count();
}

int Notebook::page_number(const Widget & w) const {
    return NOTEBOOK_IMPL->page_number(w.ptr());
}

void Notebook::reorder_page(Widget & w, int nth_page) {
    NOTEBOOK_IMPL->reorder_page(w.ptr(), nth_page);
}

void Notebook::reorder_page(int old_page, int new_page) {
    NOTEBOOK_IMPL->reorder_page(old_page, new_page);
}

void Notebook::allow_reorder() {
    NOTEBOOK_IMPL->allow_reorder();
}

void Notebook::disallow_reorder() {
    NOTEBOOK_IMPL->disallow_reorder();
}

bool Notebook::reorder_allowed() const {
    return NOTEBOOK_IMPL->reorder_allowed();
}

void Notebook::show_tabs() {
    NOTEBOOK_IMPL->show_tabs();
}

void Notebook::hide_tabs() {
    NOTEBOOK_IMPL->hide_tabs();
}

bool Notebook::tabs_visible() const {
    return NOTEBOOK_IMPL->tabs_visible();
}

void Notebook::set_homogeneous_tabs() {
    NOTEBOOK_IMPL->set_homogeneous_tabs();
}

void Notebook::unset_homogeneous_tabs() {
    NOTEBOOK_IMPL->unset_homogeneous_tabs();
}

bool Notebook::homogeneous_tabs() const {
    return NOTEBOOK_IMPL->homogeneous_tabs();
}

void Notebook::allow_rollover() {
    NOTEBOOK_IMPL->allow_rollover();
}

void Notebook::disallow_rollover() {
    NOTEBOOK_IMPL->disallow_rollover();
}

bool Notebook::rollover_allowed() const {
    return NOTEBOOK_IMPL->rollover_allowed();
}

signal<void(int)> & Notebook::signal_page_added() {
    return NOTEBOOK_IMPL->signal_page_added();
}

signal<void(int)> & Notebook::signal_page_removed() {
    return NOTEBOOK_IMPL->signal_page_removed();
}

signal<void(int)> & Notebook::signal_page_changed() {
    return NOTEBOOK_IMPL->signal_page_changed();
}

signal<void(int, int)> & Notebook::signal_page_reordered() {
    return NOTEBOOK_IMPL->signal_page_reordered();
}

} // namespace tau

//END
