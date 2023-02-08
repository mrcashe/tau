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

/// @file doc.hh Doc class and related classes.

#ifndef TAU_DOC_HH
#define TAU_DOC_HH

#include <tau/types.hh>
#include <tau/ustring.hh>
#include <vector>

namespace tau {

/// A document element.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Element {
public:

    /// Creates a pure element.
    Element() = default;

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Element(const Element & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Element & operator=(const Element & other) = default;

    /// Constructor with an opaque implementation object.
    Element(Element_ptr eptr);

    /// Test if pure.
    operator bool() const;

    /// Reset implementation pointer.
    void reset();

    /// Get implementation object.
    Element_ptr ptr();

    /// Get implementation object.
    Element_cptr ptr() const;

    /// Test for attributes existence.
    bool has_attributes() const;

    /// List attributes.
    std::vector<ustring> attributes() const;

    /// Test if certain attribute exists.
    bool has_attribute(const ustring & attr_name) const;

    /// Get attribute by name.
    ustring attribute(const ustring & attr_name) const;

    /// Set attribute value.
    void set_attribute(const ustring & attr_name, const ustring & attr_value);

    /// Remove attribute.
    void remove_attribute(const ustring & attr_name);

    /// Remove all attributes.
    void clear_attributes();

protected:

    /// @private
    Element_ptr impl;
};

/// A document element that contains character data.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Data_element: public Element {
public:

    /// Create a pure data element.
    Data_element() = default;

    /// Create data element from the implementation pointer.
    Data_element(Data_element_ptr eptr);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Data_element(const Data_element & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Data_element & operator=(const Data_element & other) = default;

    /// Get containing data.
    const uint8_t * data() const;

    /// Get size in bytes.
    std::size_t bytes() const;

    /// Assign new data.
    void assign(const uint8_t * pdata, std::size_t nbytes);
};

/// Document declaration element.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Decl_element: public Element {
public:

    /// Create a pure declaration.
    Decl_element() = default;

    /// Create declaration element from the implementation pointer.
    Decl_element(Decl_element_ptr eptr);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Decl_element(const Decl_element & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Decl_element & operator=(const Decl_element & other) = default;

    /// Get encoding.
    ustring encoding() const;

    /// Get major version number.
    unsigned version_major() const;

    /// Get minor version number.
    unsigned version_minor() const;

    /// Test if standalone.
    bool standalone() const;
};

/// A document element that contains processing instruction.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Inst_element: public Element {
public:

    /// Create a pure instruction.
    Inst_element() = default;

    /// Create instruction element from the implementation object.
    Inst_element(Inst_element_ptr eptr);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Inst_element(const Inst_element & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Inst_element & operator=(const Inst_element & other) = default;

    /// Get name.
    ustring name() const;
};

/// A document element that can own other elements.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Node_element: public Element {
public:

    /// Create a pure node.
    Node_element() = default;

    /// Create node element from the implementation object.
    Node_element(Node_element_ptr eptr);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Node_element(const Node_element & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Node_element & operator=(const Node_element & other) = default;

    /// Get node name.
    ustring name() const;

    /// Append node.
    Node_element append_node(const ustring & name);

    /// Append text element.
    Text_element append_text(const ustring & text);

    /// Append data element.
    Data_element append_data(const ustring & text);

    /// List owned elements.
    std::vector<Element> elements();

    /// List nodes with given name.
    std::vector<Node_element> nodes(const ustring & name);

    /// Test if empty.
    bool empty() const;

    /// Remove all elements.
    void clear();
};

/// A document element that contains text.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Text_element: public Element {
public:

    /// Create a pure text element.
    Text_element() = default;

    /// Create text element from the implementation object.
    Text_element(Text_element_ptr eptr);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Text_element(const Text_element & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Text_element & operator=(const Text_element & other) = default;

    /// Get containing text.
    ustring text() const;

    /// Assign new value.
    void assign(const ustring & text);

    /// Assign new value.
    void assign(const std::u32string & text);
};

/// Document type.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Doctype {
public:

    /// Creates a pure document type.
    Doctype() = default;

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Doctype(const Doctype & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Doctype & operator=(const Doctype & other) = default;

    /// Create document type from the implementation pointer.
    Doctype(Doctype_ptr dptr);

    /// Test if pure.
    operator bool() const;

    /// Reset implementation pointer.
    void reset();

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

private:

    Doctype_ptr impl;
};

/// Document.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup text_group
class Doc {
public:

    /// Create a pure document, without implementation.
    Doc() = default;

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Doc(const Doc & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Doc & operator=(const Doc & other) = default;

    /// Construct from implementation pointer.
    Doc(Doc_ptr dptr);

    /// Test if pure.
    operator bool() const;

    /// Reset implementation pointer.
    void reset();

    /// Gets implementation pointer.
    Doc_ptr ptr();

    /// Gets implementation pointer.
    Doc_cptr ptr() const;

    /// Create an XML document with corresponding declaration.
    static Doc create_xml(bool standalone, const ustring & encoding=ustring("UTF-8"), int version_major=1, int version_minor=0);

    /// @name Input/Output
    /// @{

    /// Load document from file.
    static Doc load_from_file(const ustring & path);

    /// Load from buffer.
    void load(Buffer buf);

    /// Save to buffer.
    /// Append document content to the buffer.
    /// @param buf the buffer.
    /// @param indent_size number of spaces to be added for node indentation.
    void save(Buffer buf, int indent_size=0) const;

    /// Save to file.
    /// Save document content to the file.
    /// @param path path to file.
    /// @param indent_size number of spaces to be added for node indentation.
    void save_to_file(const ustring & path, int indent_size=0) const;

    /// Special form of save_to_file() method.
    ///
    /// If document was created using load_from_file() method, this call will
    /// save it back. If document was not loaded from file that way, the user_error
    /// will be thrown.
    ///
    /// @param indent_size number of spaces to be added for node indentation.
    ///
    /// @throw user_error if document wasn't created using load_from_file.
    void save(int indent_size=0);

    /// @}
    /// Get document declaration element.
    Decl_element decl();

    /// Get document type object.
    Doctype doctype();

    /// Get root element.
    Node_element root();

    /// Create root element.
    /// @param root_name the name to be given for root element.
    /// @throw bad_doc when root element already exist or root_name is invalid.
    Node_element create_root(const ustring & root_name);

    /// List processing instructions.
    std::vector<Inst_element> instructions();

    /// Set entity value.
    void set_entity(const ustring & name, const ustring & value);

    /// Remove entity.
    void remove_entity(const ustring & name);

    /// Get entity value.
    ustring entity(const ustring & name) const;

    /// Determine if entity defined.
    bool has_entity(const ustring & name) const;

private:

    Doc_ptr impl;
};

} // namespace tau

#endif // TAU_DOC_HH
