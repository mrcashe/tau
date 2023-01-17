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

#include <tau/buffer.hh>
#include <tau/doc.hh>
#include <doc-impl.hh>
#include <cstring>

namespace tau {

Element::Element(Element_ptr eptr):
    impl(eptr)
{
}

Element::operator bool() const {
    return nullptr != impl;
}

void Element::reset() {
    impl.reset();
}

Element_ptr Element::ptr() {
    return impl;
}

Element_cptr Element::ptr() const {
    return impl;
}

bool Element::has_attributes() const {
    return impl && !impl->attrs.empty();
}

bool Element::has_attribute(const ustring & attr_name) const {
    return impl && std::any_of(impl->attrs.begin(), impl->attrs.end(), [&attr_name](const std::pair<ustring, ustring> & p) { return attr_name == p.first; } );
}

ustring Element::attribute(const ustring & attr_name) const {
    return impl ? impl->attribute(attr_name) : ustring();
}

void Element::set_attribute(const ustring & attr_name, const ustring & attr_value) {
    if (impl) {
        impl->set_attribute(attr_name, attr_value);
    }
}

void Element::remove_attribute(const ustring & attr_name) {
    if (impl) {
        impl->remove_attribute(attr_name);
    }
}

std::vector<ustring> Element::attributes() const {
    std::vector<ustring> v;

    if (impl) {
        for (auto & attr: impl->attrs) {
            v.push_back(attr.first);
        }
    }

    return v;
}

void Element::clear_attributes() {
    if (impl) {
        impl->attrs.clear();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Text_element::Text_element(Text_element_ptr eptr):
    Element(eptr)
{
}

ustring Text_element::text() const {
    return TEXT_IMPL->str;
}

void Text_element::assign(const ustring & str) {
    TEXT_IMPL->str = str;
}

void Text_element::assign(const std::u32string & str) {
    TEXT_IMPL->str = str;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Data_element::Data_element(Data_element_ptr eptr):
    Element(eptr)
{
}

const uint8_t * Data_element::data() const {
    return DATA_IMPL->data.data();
}

std::size_t Data_element::bytes() const {
    return DATA_IMPL->data.size();
}

void Data_element::assign(const uint8_t * pdata, std::size_t nbytes) {
    DATA_IMPL->data.resize(nbytes);
    std::memcpy(DATA_IMPL->data.data(), pdata, nbytes);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Inst_element::Inst_element(Inst_element_ptr eptr):
    Element(eptr)
{
}

ustring Inst_element::name() const {
    return INST_IMPL->name;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Decl_element::Decl_element(Decl_element_ptr eptr):
    Element(eptr)
{
}

ustring Decl_element::encoding() const {
    return DECL_IMPL->encoding;
}

unsigned Decl_element::version_major() const {
    return DECL_IMPL->version_major;
}

unsigned Decl_element::version_minor() const {
    return DECL_IMPL->version_minor;
}

bool Decl_element::standalone() const {
    return DECL_IMPL->standalone;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Node_element::Node_element(Node_element_ptr eptr):
    Element(eptr)
{
}

ustring Node_element::name() const {
    return NODE_IMPL->name;
}

Node_element Node_element::append_node(const ustring & name) {
    return NODE_IMPL->append_node(name);
}

Text_element Node_element::append_text(const ustring & str) {
    return NODE_IMPL->append_text(str);
}

std::vector<Element> Node_element::elements() {
    std::vector<Element> v;
    v.reserve(NODE_IMPL->elems.size());
    for (auto eptr: NODE_IMPL->elems) { v.push_back(eptr); }
    return v;
}

std::vector<Node_element> Node_element::nodes(const ustring & name) {
    std::vector<Node_element> v;

    for (auto eptr: NODE_IMPL->elems) {
        if (auto nptr = std::dynamic_pointer_cast<Node_element_impl>(eptr)) {
            if (name.empty() || name == nptr->name) {
                v.push_back(Node_element(nptr));
            }
        }
    }

    return v;
}

bool Node_element::empty() const {
    return NODE_IMPL->elems.empty();
}

void Node_element::clear() {
    NODE_IMPL->elems.clear();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Doctype::Doctype(Doctype_ptr dptr):
    impl(dptr)
{
}

ustring Doctype::name() const {
    return impl ? impl->name_ : ustring();
}

bool Doctype::is_public() const {
    return impl && impl->public_;
}

ustring Doctype::location() const {
    return impl ? impl->location_ : ustring();
}

ustring Doctype::owner() const {
    return impl ? impl->owner_ : ustring();
}

ustring Doctype::description() const {
    return impl ? impl->description_ : ustring();
}

ustring Doctype::lang() const {
    return impl ? impl->lang_ : ustring();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Doc::Doc(Doc_ptr dptr):
    impl(dptr)
{
}

Doc::operator bool() const {
    return nullptr != impl;
}

Doc_ptr Doc::ptr() {
    return impl;
}

Doc_cptr Doc::ptr() const {
    return impl;
}

void Doc::reset() {
    impl.reset();
}

Decl_element Doc::decl() {
    return impl ? impl->decl() : Decl_element();
}

Doctype Doc::doctype() {
    return impl ? impl->doctype() : Doctype();
}

Node_element Doc::root() {
    return impl ? impl->root() : Node_element();
}

Node_element Doc::create_root(const ustring & root_name) {
    return impl ? create_root(root_name) : Node_element();
}

std::vector<Inst_element> Doc::instructions() {
    std::vector<Inst_element> v;

    if (impl) {
        for (auto i: impl->instructions()) {
            v.push_back(i);
        }
    }

    return v;
}

void Doc::set_entity(const ustring & name, const ustring & value) {
    if (impl) { set_entity(name, value); }
}

void Doc::remove_entity(const ustring & name) {
    if (impl) { impl->remove_entity(name); }
}

ustring Doc::entity(const ustring & name) const {
    return impl ? impl->entity(name) : ustring();
}

bool Doc::has_entity(const ustring & name) const {
    return impl && impl->has_entity(name);
}

void Doc::save(Buffer buf, int indent_size) const {
    if (impl) { impl->save(buf, indent_size); }
}

void Doc::save_to_file(const ustring & path, int indent_size) const {
    if (impl) { impl->save_to_file(path, indent_size); }
}

// static
Doc Doc::create_xml(bool standalone, const ustring & encoding, int major, int minor) {
    return Doc(Doc_impl::create_xml(standalone, encoding, major, minor));
}

// static
Doc Doc::load_from_file(const ustring & path) {
    return Doc(Doc_impl::load_from_file(path));
}

} // namespace tau

//END
