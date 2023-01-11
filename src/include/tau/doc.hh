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

/// @file doc.hh Doc class and related classes.

#ifndef TAU_DOC_HH
#define TAU_DOC_HH

#include <tau/types.hh>
#include <tau/ustring.hh>
#include <list>
#include <vector>

namespace tau {

/// A document element.
/// @ingroup text_group
class Element {
public:

    virtual ~Element();

    bool has_attributes() const;
    std::vector<ustring> list_attributes() const;
    bool has_attribute(const ustring & attr_name) const;
    ustring attribute(const ustring & attr_name) const;
    void set_attribute(const ustring & attr_name, const ustring & attr_value);
    void remove_attribute(const ustring & attr_name);
    void clear_attributes();

protected:

    /// @private
    Element(Element_data * edata);

    /// %Element is non-copyable class.
    Element(const Element & other) = delete;

    /// %Element is non-copyable class.
    Element & operator=(const Element & other) = delete;

    /// @private
    Element_data * pdata;
};

/// A document element that contains text.
/// @ingroup text_group
class Text_element: public Element {
public:

    /// Get containing text.
    ustring text() const;

    /// Assign new value.
    void assign(const ustring & text);

private:

    friend Node_element;
    Text_element();
};

/// A document element that contains character data.
/// @ingroup text_group
class Data_element: public Element {
public:

    /// Get containing data.
    const std::vector<char32_t> & data() const;

    /// Assign new value.
    void assign(const ustring & text);

    /// Assign new value.
    void assign(const std::vector<char32_t> & chars);

private:

    friend Node_element;
    Data_element();
};

/// A document element that contains processing instruction.
/// @ingroup text_group
class Inst_element: public Element {
public:

    ustring name() const;

private:

    friend Doc;
    friend class Doc_impl;
    Inst_element();
};

/// Document declaration element.
class Decl_element: public Element {
public:

    /// Get encoding.
    ustring encoding() const;

    /// Get major version number.
    unsigned version_major() const;

    /// Get minor version number.
    unsigned version_minor() const;

    /// Test if standalone.
    bool standalone() const;

private:

    friend Doc;
    friend class Doc_impl;
    Decl_element();

};

/// A document element that can own other elements.
/// @ingroup text_group
class Node_element: public Element {
public:

   ~Node_element();

    /// Get node name.
    ustring name() const;

    /// Append node.
    Node_element_ptr append_node(const ustring & name);

    /// Append text element.
    Text_element_ptr append_text(const ustring & text);

    /// Append data element.
    Data_element_ptr append_data(const ustring & text);

    /// List owned elements.
    std::list<Element_ptr> elements();

    /// List nodes with given name.
    std::vector<Node_element_ptr> nodes(const ustring & name);

    /// Test if empty.
    bool empty() const;

    /// Remove all elements.
    void clear();

protected:

    /// @private
    friend class Doc_impl;

    /// @private
    Node_element(const ustring & name);

    /// @private
    Node_element(Element_data * ndata);

};

/// Document type.
/// @ingroup text_group
class Doctype {
public:

    virtual ~Doctype();

    /// Get root element name.
    ustring name() const;

    /// Test if public.
    bool is_public() const;

    /// Get location of the external DTD.
    ustring location() const;

    /// Get public DTD owner.
    ustring owner() const;

    /// Get public DTD description.
    ustring description() const;

    /// Get public DTD language.
    ustring lang() const;

protected:

    /// @private
    friend class Doc_impl;

    /// @private
    Doctype();

    /// @private
    virtual ustring name_v() const = 0;

    /// @private
    virtual bool is_public_v() const = 0;

    /// @private
    virtual ustring location_v() const = 0;

    /// @private
    virtual ustring owner_v() const = 0;

    /// @private
    virtual ustring description_v() const = 0;

    /// @private
    virtual ustring lang_v() const = 0;
};

/// Document.
/// @ingroup text_group
class Doc {
public:

    virtual ~Doc();

    /// Create an empty document.
    static Doc_ptr create();

    /// Create an XML document with corresponding declaration.
    static Doc_ptr create_xml(bool standalone, const ustring & encoding=ustring("UTF-8"), unsigned version_major=1, unsigned version_minor=0);

    /// Load from buffer.
    void load(Buffer buf);

    /// Save to buffer.
    /// Append document content to the buffer.
    /// @param buf the buffer.
    /// @param indent_size number of spaces to be added for node indentation.
    void save(Buffer buf, unsigned indent_size=0) const;

    /// Get document declaration element.
    Decl_element_ptr decl();

    /// Get document type object.
    Doctype_ptr doctype();

    /// Get root element.
    Node_element_ptr root();

    /// Create root element.
    /// @param root_name the name to be given for root element.
    /// @throw bad_doc when root element already exist or root_name is invalid.
    Node_element_ptr create_root(const ustring & root_name);

    /// List processing instructions.
    std::list<Inst_element_ptr> instrs();

    /// Set entity value.
    void set_entity(const ustring & name, const ustring & value);

    /// Remove entity.
    void remove_entity(const ustring & name);

    /// Get entity value.
    ustring entity(const ustring & name) const;

    /// Determine if entity defined.
    bool has_entity(const ustring & name) const;

protected:

    /// @private
    Doc();

    /// @private
    virtual void load_v(Buffer buf) = 0;

    /// @private
    virtual Decl_element_ptr decl_v() = 0;

    /// @private
    virtual Doctype_ptr doctype_v() = 0;

    /// @private
    virtual Node_element_ptr root_v() = 0;

    /// @private
    virtual Node_element_ptr create_root_v(const ustring & root_name) = 0;

    /// @private
    virtual std::list<Inst_element_ptr> instrs_v() = 0;

    /// @private
    virtual void set_entity_v(const ustring & name, const ustring & value) = 0;

    /// @private
    virtual void remove_entity_v(const ustring & name) = 0;

    /// @private
    virtual ustring entity_v(const ustring & name) const = 0;

    /// @private
    virtual bool has_entity_v(const ustring & name) const = 0;

    /// @private
    virtual void save_v(Buffer buf, unsigned indent_size) const = 0;
};

} // namespace tau

#endif // TAU_DOC_HH
