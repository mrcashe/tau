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

#include <tau/buffer.hh>
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <buffer-impl.hh>
#include <fstream>
#include <iostream>

namespace tau {

Buffer::Buffer():
    impl(std::make_shared<Buffer_impl>())
{
}

Buffer::~Buffer() {}

Buffer::Buffer(const ustring & s):
    impl(std::make_shared<Buffer_impl>())
{
    assign(s);
}

Buffer::Buffer(const std::u32string & s):
    impl(std::make_shared<Buffer_impl>())
{
    assign(s);
}

Buffer::Buffer(std::istream & is):
    impl(std::make_shared<Buffer_impl>())
{
    insert(cend(), is);
}

void Buffer::assign(const ustring & str) {
    clear();
    insert(cend(), str);
}

void Buffer::assign(const std::u32string & str) {
    clear();
    insert(cend(), str);
}

void Buffer::assign(const Buffer other) {
    clear();
    insert(cend(), other.text());
}

Buffer_citer Buffer::replace(Buffer_citer i, const ustring & str) {
    return replace(i, std::u32string(str));
}

Buffer_citer Buffer::replace(Buffer_citer i, const std::u32string & str) {
    return impl->replace(i, str);
}

Buffer_citer Buffer::insert(Buffer_citer i, const std::u32string & str) {
    return impl->insert(i, str);
}

Buffer_citer Buffer::insert(Buffer_citer i, char32_t uc, std::size_t count) {
    return 0 != count ? insert(i, std::u32string(count, uc)) : i;
}

Buffer_citer Buffer::insert(Buffer_citer i, const ustring & str) {
    return insert(i, std::u32string(str));
}

Buffer_citer Buffer::erase(Buffer_citer b, Buffer_citer e) {
    return impl->erase(b, e);
}

void Buffer::save(std::ostream & os) {
    impl->save(os);
}

void Buffer::save_to_file(const ustring & path) {
    impl->save_to_file(path);
}

std::size_t Buffer::size() const {
    return impl->size();
}

std::size_t Buffer::length(Buffer_citer b, Buffer_citer e) const {
    return impl->length(b.row(), b.col(), e.row(), e.col());
}

std::size_t Buffer::rows() const {
    return impl->rows();
}

bool Buffer::empty() const {
    return impl->empty();
}

bool Buffer::changed() const {
    return impl->changed_;
}

Encoding Buffer::encoding() const {
    return impl->encoding_;
}

void Buffer::change_encoding(const Encoding & enc) {
    impl->change_encoding(enc);
}

Buffer_citer Buffer::citer(std::size_t row, std::size_t col) const {
    return Buffer_citer(Buffer_citer_impl::create(impl, row, col));
}

Buffer_citer Buffer::cbegin() const {
    return citer(0, 0);
}

Buffer_citer Buffer::cend() const {
    std::size_t row = 0, col = 0;

    if (0 != rows()) {
        row = rows()-1;
        col = impl->length(row);
    }

    return citer(row, col);
}

void Buffer::clear() {
    erase(cbegin(), cend());
}

ustring Buffer::text() const {
    return text(cbegin(), cend());
}

ustring Buffer::text(Buffer_citer b, Buffer_citer e) const {
    return impl->text(b.row(), b.col(), e.row(), e.col());
}

std::u32string Buffer::text32() const {
    return text32(cbegin(), cend());
}

std::u32string Buffer::text32(Buffer_citer b, Buffer_citer e) const {
    return impl->text(b.row(), b.col(), e.row(), e.col());
}

Buffer_citer Buffer::insert(Buffer_citer iter, std::istream & is) {
    return impl->insert(iter, is);
}

// static
Buffer Buffer::load_from_file(const ustring & path) {
    auto & io = Locale().iocharset();
    std::ifstream is(io.is_utf8() ? std::string(path) : io.encode(path), std::ios::binary);
    if (!is.good()) { throw sys_error(path); }
    Buffer buffer;
    buffer.insert(buffer.cend(), is);
    buffer.impl->path_ = path;
    return buffer;
}

void Buffer::save() {
    impl->save();
}

void Buffer::lock() {
    impl->lock();
}

void Buffer::unlock() {
    impl->unlock();
}

bool Buffer::locked() const {
    return impl->locked_;
}

void Buffer::enable_bom() {
    impl->enable_bom();
}

void Buffer::disable_bom() {
    impl->disable_bom();
}

bool Buffer::bom_enabled() const {
    return impl->bom_;
}

signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & Buffer::signal_erase() {
    return impl->signal_erase();
}

signal<void(Buffer_citer, Buffer_citer)> & Buffer::signal_insert() {
    return impl->signal_insert();
}

signal<void(Buffer_citer, Buffer_citer, const std::u32string &)> & Buffer::signal_replace() {
    return impl->signal_replace();
}

signal<void()> & Buffer::signal_changed() {
    return impl->signal_changed();
}

signal<void()> & Buffer::signal_flush() {
    return impl->signal_flush();
}

signal<void()> & Buffer::signal_lock() {
    return impl->signal_lock();
}

signal<void()> & Buffer::signal_unlock() {
    return impl->signal_unlock();
}

signal<void(const Encoding &)> & Buffer::signal_encoding_changed() {
    return impl->signal_encoding_changed();
}

signal<void()> & Buffer::signal_bom_changed() {
    return impl->signal_bom_changed();
}

} // namespace tau

//END
