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

#ifndef TAU_DOC_IMPL_HH
#define TAU_DOC_IMPL_HH

#include <tau/doc.hh>
#include <types-impl.hh>
#include <algorithm>
#include <map>

namespace tau {

struct Element_impl {
    virtual ~Element_impl() {}
    std::map<ustring, ustring> attrs;

    ustring attribute(const ustring & attr_name) const {
        auto i = std::find_if(attrs.begin(), attrs.end(), [&attr_name](const std::pair<ustring, ustring> & p) { return attr_name == p.first; } );
        return i != attrs.end() ? i->second : ustring();
    }

    void set_attribute(const ustring & attr_name, const ustring & attr_value) {
        auto i = std::find_if(attrs.begin(), attrs.end(), [&attr_name](const std::pair<ustring, ustring> & p) { return attr_name == p.first; } );

        if (i != attrs.end()) {
            i->second = attr_value;
            return;
        }

        attrs[attr_name] = attr_value;
    }

    void remove_attribute(const ustring & attr_name) {
        attrs.erase(attr_name);
    }

};

struct Text_element_impl: Element_impl {
    std::u32string          str;

    Text_element_impl(const ustring & s):
        str(s)
    {
    }
};

struct Data_element_impl: Element_impl {
    std::vector<uint8_t>    data;
};

struct Decl_element_impl: Element_impl {
    ustring                 encoding;
    unsigned                version_major = 1;
    unsigned                version_minor = 0;
    bool                    standalone = true;
};

struct Inst_element_impl: Element_impl {
    ustring                 name;

    Inst_element_impl(const ustring & n):
        name(n)
    {
    }
};

struct Node_element_impl: Element_impl {
    ustring                 name;
    std::list<Element_ptr>  elems;

    Node_element_impl(const ustring & n=ustring()):
        name(n)
    {
    }

    Node_element_ptr append_node(const ustring & name) {
        auto node = std::make_shared<Node_element_impl>(name);
        elems.push_back(node);
        return node;
    }


    Text_element_ptr append_text(const ustring & s) {
        auto elem = std::make_shared<Text_element_impl>(s);
        elems.push_back(elem);
        return elem;
    }
};

#define TEXT_IMPL (std::static_pointer_cast<Text_element_impl>(impl))
#define DATA_IMPL (std::static_pointer_cast<Data_element_impl>(impl))
#define INST_IMPL (std::static_pointer_cast<Inst_element_impl>(impl))
#define DECL_IMPL (std::static_pointer_cast<Decl_element_impl>(impl))
#define NODE_IMPL (std::static_pointer_cast<Node_element_impl>(impl))

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct Doctype_impl {
    Doctype_impl(const ustring & name):
        name_(name)
    {
    }

    ustring     name_;                  // Root element name.
    ustring     location_;              // External DTD location.
    ustring     owner_;                 // Public DTD owner.
    ustring     description_;           // Public DTD description.
    ustring     lang_;                  // Public DTD language.
    bool        public_ = false;

    void set_public(const ustring & name, const ustring & loc);
    void set_private(const ustring & loc) { location_ = loc; public_ = false; }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class Doc_impl {
public:

    Doc_impl() = default;
    static Doc_ptr create_xml(bool standalone, const ustring & encoding=ustring("UTF-8"), int version_major=1, int version_minor=0);
    static Doc_ptr load_from_file(const ustring & path);

    void load(Buffer buffer);
    Node_element_ptr create_root(const ustring & root_name);
    void set_entity(const ustring & name, const ustring & value);
    void remove_entity(const ustring & name);
    ustring entity(const ustring & name) const;
    bool has_entity(const ustring & name) const;
    void save(Buffer buf, int indent_size) const;
    void save_to_file(const ustring & path, int indent_size) const;
    void save(int indent_size);

    Decl_element_ptr decl() { return decl_; }
    Doctype_ptr doctype() { return doctype_; }
    Node_element_ptr root() { return root_; }
    std::vector<Inst_element_ptr> instructions() { return inst_; }

    static bool xml_element_name_valid(const ustring & name);
    static bool xml_attr_name_valid(const ustring & name);

protected:

    Node_element_ptr            root_;
    Decl_element_ptr            decl_;
    Doctype_ptr                 doctype_;
    std::vector<Inst_element_ptr> inst_;
    std::map<ustring, ustring>  entities_;
    ustring                     path_;      // Path to the file which was used by load_from_file().

private:

    void expand_entities(ustring & s);
    void save_element(Element_ptr elem, Buffer buf, int indent, int indent_size) const;
};

} // namespace tau

#endif // TAU_DOC_IMPL_HH
