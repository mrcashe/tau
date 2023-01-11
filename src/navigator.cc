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

#include <tau/navigator.hh>
#include <navigator-impl.hh>

namespace tau {

#define NAVIGATOR_IMPL (std::static_pointer_cast<Navigator_impl>(impl))

Navigator::Navigator():
    Widget(std::make_shared<Navigator_impl>())
{
}

Navigator::Navigator(const ustring & path):
    Widget(std::make_shared<Navigator_impl>(path))
{
}

ustring Navigator::dir() const {
    return NAVIGATOR_IMPL->dir();
}

void Navigator::chdir(const ustring & path) {
    NAVIGATOR_IMPL->chdir(path);
}

void Navigator::reload() {
    NAVIGATOR_IMPL->reload();
}

void Navigator::sort_by(const ustring & col) {
    NAVIGATOR_IMPL->sort_by(col);
}

ustring Navigator::sorted_by() const {
    return NAVIGATOR_IMPL->sorted_by();
}

void Navigator::sort_forward() {
    NAVIGATOR_IMPL->sort_forward();
}

void Navigator::sort_backward() {
    NAVIGATOR_IMPL->sort_backward();
}

bool Navigator::sorted_backward() const {
    return NAVIGATOR_IMPL->sorted_backward();
}

void Navigator::show_info(const ustring & items, char32_t sep) {
    NAVIGATOR_IMPL->show_info(items, sep);
}

void Navigator::hide_info(const ustring & items, char32_t sep) {
    NAVIGATOR_IMPL->hide_info(items, sep);
}

bool Navigator::info_visible(const ustring & item) const {
    return NAVIGATOR_IMPL->info_visible(item);
}

ustring Navigator::visible_info_items(char32_t sep) const {
    return NAVIGATOR_IMPL->visible_info_items(sep);
}

ustring Navigator::invisible_info_items(char32_t sep) const {
    return NAVIGATOR_IMPL->visible_info_items(sep);
}

void Navigator::show_hidden_files() {
    NAVIGATOR_IMPL->show_hidden_files();
}

void Navigator::hide_hidden_files() {
    NAVIGATOR_IMPL->hide_hidden_files();
}

bool Navigator::hidden_files_visible() const {
    return NAVIGATOR_IMPL->hidden_files_visible();
}

void Navigator::allow_multiple_select() {
    NAVIGATOR_IMPL->allow_multiple_select();
}

void Navigator::disallow_multiple_select() {
    NAVIGATOR_IMPL->disallow_multiple_select();
}

bool Navigator::multiple_select_allowed() const {
    return NAVIGATOR_IMPL->multiple_select_allowed();
}

void Navigator::allow_dir_select() {
    NAVIGATOR_IMPL->allow_dir_select();
}

void Navigator::disallow_dir_select() {
    NAVIGATOR_IMPL->disallow_dir_select();
}

bool Navigator::dir_select_allowed() const {
    return NAVIGATOR_IMPL->dir_select_allowed();
}

void Navigator::set_show_dirs_only() {
    NAVIGATOR_IMPL->set_show_dirs_only();
}

void Navigator::unset_show_dirs_only() {
    NAVIGATOR_IMPL->unset_show_dirs_only();
}

bool Navigator::dirs_only_visible() const {
    return NAVIGATOR_IMPL->dirs_only_visible();
}

void Navigator::set_filter(const ustring & patterns) {
    NAVIGATOR_IMPL->set_filter(patterns);
}

ustring Navigator::filter() const {
    return NAVIGATOR_IMPL->filter();
}

signal<void(ustring)> & Navigator::signal_file_select() {
    return NAVIGATOR_IMPL->signal_file_select();
}

signal<void(ustring)> & Navigator::signal_file_unselect() {
    return NAVIGATOR_IMPL->signal_file_unselect();
}

signal<void(ustring)> & Navigator::signal_file_activate() {
    return NAVIGATOR_IMPL->signal_file_activate();
}

signal<void(ustring)> & Navigator::signal_dir_changed() {
    return NAVIGATOR_IMPL->signal_dir_changed();
}

} // namespace tau

//END
