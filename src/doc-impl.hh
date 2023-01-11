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
#include <map>

namespace tau {

class Element_data {
public:

    virtual ~Element_data() {}
    std::list<std::pair<ustring, ustring>>      attrs;
};

class Text_element_data: public Element_data {
public:

    ustring                                     text;
};

class Data_element_data: public Element_data {
public:

    std::vector<char32_t>                       chars;
};

class Decl_element_data: public Element_data {
public:

    ustring                                     encoding;
    unsigned                                    version_major = 1;
    unsigned                                    version_minor = 0;
    bool                                        standalone = true;
};

class Inst_element_data: public Element_data {
public:

    ustring                                     name;
};

class Node_element_data: public Element_data {
public:

    ustring                                     name;
    std::list<Element_ptr>                      elems;
};

#define TEXT_DATA (static_cast<Text_element_data *>(pdata))
#define DATA_DATA (static_cast<Data_element_data *>(pdata))
#define INST_DATA (static_cast<Inst_element_data *>(pdata))
#define DECL_DATA (static_cast<Decl_element_data *>(pdata))
#define NODE_DATA (static_cast<Node_element_data *>(pdata))

class Doctype_impl: public Doctype {
public:

    Doctype_impl(const ustring & name):
        name_(name)
    {
    }

    // Overrides pure Doctype.
    ustring name_v() const override {
        return name_;
    }

    // Overrides pure Doctype.
    ustring location_v() const override {
        return location_;
    }

    // Overrides pure Doctype.
    ustring owner_v() const override {
        return owner_;
    }

    // Overrides pure Doctype.
    ustring description_v() const override {
        return description_;
    }

    // Overrides pure Doctype.
    ustring lang_v() const override {
        return lang_;
    }

    void set_private(const ustring & loc) {
        location_ = loc;
        public_ = false;
    }

    void set_public(const ustring & name, const ustring & loc);

    /// Overrides pure Doctype.
    bool is_public_v() const override {
        return public_;
    }

private:

    ustring     name_;                  // Root element name.
    ustring     location_;              // External DTD location.
    ustring     owner_;                 // Public DTD owner.
    ustring     description_;           // Public DTD description.
    ustring     lang_;                  // Public DTD language.
    bool        public_ = false;
};

class Doc_impl: public Doc {
public:

    Doc_impl();

    // Overrides pure Doc.
    void load_v(Buffer buffer) override;

    // Overrides pure Doc.
    Decl_element_ptr decl_v() override;

    // Overrides pure Doc.
    Doctype_ptr doctype_v() override;

    // Overrides pure Doc.
    Node_element_ptr root_v() override;

    // Overrides pure Doc.
    Node_element_ptr create_root_v(const ustring & root_name) override;

    // Overrides pure Doc.
    std::list<Inst_element_ptr> instrs_v() override;

    // Overrides pure Doc.
    void set_entity_v(const ustring & name, const ustring & value) override;

    // Overrides pure Doc.
    void remove_entity_v(const ustring & name) override;

    // Overrides pure Doc.
    ustring entity_v(const ustring & name) const override;

    // Overrides pure Doc.
    bool has_entity_v(const ustring & name) const override;

    // Overrides pure Doc.
    void save_v(Buffer buf, unsigned indent_size) const override;

    // Used by Doc::create_xml().
    void assign_decl(Decl_element_ptr decl) {
        decl_ = decl;
    }

    static bool xml_element_name_valid(const ustring & name);
    static bool xml_attr_name_valid(const ustring & name);

private:

    void expand_entities(ustring & s);
    void save_element(Element_ptr elem, Buffer buf, unsigned indent, unsigned indent_size) const;

protected:

    Node_element_ptr                    root_;
    Decl_element_ptr                    decl_;
    Doctype_ptr                         doctype_;
    std::list<Inst_element_ptr>         instrs_;
    std::map<ustring, ustring>          entities_;
};

} // namespace tau

#endif // TAU_DOC_IMPL_HH
