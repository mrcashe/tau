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

namespace tau {

Element::Element(Element_data * edata):
    pdata(edata)
{
}

Element::~Element() {
    delete pdata;
}

bool Element::has_attributes() const {
    return !pdata->attrs.empty();
}

bool Element::has_attribute(const ustring & attr_name) const {
    return std::any_of(pdata->attrs.begin(), pdata->attrs.end(), [&attr_name](const std::pair<ustring, ustring> & p) { return attr_name == p.first; } );
}

ustring Element::attribute(const ustring & attr_name) const {
    auto i = std::find_if(pdata->attrs.begin(), pdata->attrs.end(), [&attr_name](const std::pair<ustring, ustring> & p) { return attr_name == p.first; } );
    return i != pdata->attrs.end() ? i->second : ustring();
}

void Element::set_attribute(const ustring & attr_name, const ustring & attr_value) {
    auto i = std::find_if(pdata->attrs.begin(), pdata->attrs.end(), [&attr_name](const std::pair<ustring, ustring> & p) { return attr_name == p.first; } );

    if (i != pdata->attrs.end()) {
        i->first = attr_name;
        i->second = attr_value;
        return;
    }

    pdata->attrs.emplace_back(std::make_pair(attr_name, attr_value));
}

void Element::remove_attribute(const ustring & attr_name) {
    for (auto i = pdata->attrs.begin(); i != pdata->attrs.end(); ++i) {
        if (i->first == attr_name) {
            pdata->attrs.erase(i);
            return;
        }
    }
}

std::vector<ustring> Element::list_attributes() const {
    std::vector<ustring> v;

    for (auto & attr: pdata->attrs) {
        v.push_back(attr.first);
    }

    return v;
}

void Element::clear_attributes() {
    pdata->attrs.clear();
}

Text_element::Text_element():
    Element(new Text_element_data)
{
}

ustring Text_element::text() const {
    return TEXT_DATA->text;
}

void Text_element::assign(const ustring & text) {
    TEXT_DATA->text = text;
}

Data_element::Data_element():
    Element(new Data_element_data)
{
}

const std::vector<char32_t> & Data_element::data() const {
    return DATA_DATA->chars;
}

void Data_element::assign(const ustring & str) {
    DATA_DATA->chars.resize(str.size());
    std::size_t i = 0;
    for (char32_t uc: str) { DATA_DATA->chars[i++] = uc; }
}

void Data_element::assign(const std::vector<char32_t> & chars) {
    DATA_DATA->chars = chars;
}

Inst_element::Inst_element():
    Element(new Inst_element_data)
{
}

ustring Inst_element::name() const {
    return INST_DATA->name;
}

Decl_element::Decl_element():
    Element(new Decl_element_data)
{
}

ustring Decl_element::encoding() const {
    return DECL_DATA->encoding;
}

unsigned Decl_element::version_major() const {
    return DECL_DATA->version_major;
}

unsigned Decl_element::version_minor() const {
    return DECL_DATA->version_minor;
}

bool Decl_element::standalone() const {
    return DECL_DATA->standalone;
}

Node_element::Node_element(const ustring & name):
    Element(new Node_element_data)
{
    NODE_DATA->name = name;
}

Node_element::Node_element(Element_data * ndata):
    Element(ndata)
{
}

Node_element::~Node_element() {
}

ustring Node_element::name() const {
    return NODE_DATA->name;
}

Node_element_ptr Node_element::append_node(const ustring & name) {
    auto node = Node_element_ptr(new Node_element(name));
    NODE_DATA->elems.push_back(node);
    return node;
}

Text_element_ptr Node_element::append_text(const ustring & text) {
    auto elem = Text_element_ptr(new Text_element);
    auto edata = static_cast<Text_element_data *>(elem->pdata);
    edata->text = text;
    NODE_DATA->elems.push_back(elem);
    return elem;
}

std::list<Element_ptr> Node_element::elements() {
    std::list<Element_ptr> v;
    v = NODE_DATA->elems;
    return v;
}

std::vector<Node_element_ptr> Node_element::nodes(const ustring & name) {
    std::vector<Node_element_ptr> v;

    for (auto ep: NODE_DATA->elems) {
        if (auto np = std::dynamic_pointer_cast<Node_element>(ep)) {
            if (name.empty() || name == np->name()) {
                v.push_back(np);
            }
        }
    }

    return v;
}

bool Node_element::empty() const {
    return NODE_DATA->elems.empty();
}

void Node_element::clear() {
    NODE_DATA->elems.clear();
}

Doctype::Doctype() {}
Doctype::~Doctype() {}

ustring Doctype::name() const {
    return name_v();
}

bool Doctype::is_public() const {
    return is_public_v();
}

ustring Doctype::location() const {
    return location_v();
}

ustring Doctype::owner() const {
    return owner_v();
}

ustring Doctype::description() const {
    return description_v();
}

ustring Doctype::lang() const {
    return lang_v();
}

Doc::Doc() {}
Doc::~Doc() {}

void Doc::load(Buffer buf) {
    load_v(buf);
}

Decl_element_ptr Doc::decl() {
    return decl_v();
}

Doctype_ptr Doc::doctype() {
    return doctype_v();
}

Node_element_ptr Doc::root() {
    return root_v();
}

Node_element_ptr Doc::create_root(const ustring & root_name) {
    return create_root_v(root_name);
}

std::list<Inst_element_ptr> Doc::instrs() {
    return instrs_v();
}

void Doc::set_entity(const ustring & name, const ustring & value) {
    set_entity_v(name, value);
}

void Doc::remove_entity(const ustring & name) {
    remove_entity_v(name);
}

ustring Doc::entity(const ustring & name) const {
    return entity_v(name);
}

bool Doc::has_entity(const ustring & name) const {
    return has_entity_v(name);
}

void Doc::save(Buffer buf, unsigned indent_size) const {
    return save_v(buf, indent_size);
}

} // namespace tau

//END
