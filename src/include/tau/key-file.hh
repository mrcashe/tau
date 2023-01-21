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

/// @file key-file.hh The Key_file class.

#ifndef TAU_KEY_FILE_HH
#define TAU_KEY_FILE_HH

#include <tau/types.hh>
#include <tau/signal.hh>
#include <tau/ustring.hh>
#include <istream>
#include <vector>

namespace tau {

/// This class lets you parse, edit or create files containing groups of key-value pairs.
///
/// @ingroup text_group
class Key_file: public trackable {
public:

    /// Default constructor.
    /// @param list_sep list separator character.
    /// @param comment_sep list separator character.
    Key_file(char32_t list_sep=0, char32_t comment_sep=0);

    /// Constructor with stream.
    /// @param is the input stream.
    /// @param list_sep list separator character.
    /// @param comment_sep list separator character.
    /// If the input stream is isn't good, the constructor does not do anything.
    Key_file(std::istream & is, char32_t list_sep=0, char32_t comment_sep=0);

    /// Copy constructor.
    Key_file(const Key_file & other);

    /// Copy operator,
    Key_file & operator=(const Key_file & other);

    /// Destructor.
   ~Key_file();

    /// @name Input/Output
    /// @{

    /// Create from file.
    static Key_file load_from_file(const ustring & path);

    /// Load from stream.
    void load(std::istream & is);

    /// Save to stream.
    void save(std::ostream & os);

    /// Save to file.
    void save_to_file(const ustring & path);

    /// Special form of save_to_file() method.
    ///
    /// If key file was created using load_from_file() method, this call will
    /// save it back. If key file wasn't loaded from file that way, the user_error
    /// will be trown. If file specified for load_from_file() does not exist, save()
    /// anyway should try to perform write operation.
    ///
    /// @throw user_error if key file wasn't created using load_from_file.
    void save();

    /// @}
    /// Set comment separator.
    /// @note By default, the comment separator is a '#'.
    void set_comment_separator(char32_t comment_sep);

    /// Sets the character which is used to separate values in lists.
    /// @note By default, the list separator is a ':' (colon).
    void set_list_separator(char32_t list_sep);

    /// Gets list separator.
    char32_t list_separator() const;

    /// Gets comment separator.
    char32_t comment_separator() const;

    /// Get root section.
    Key_section & root();

    /// Get specified section.
    /// @param sect_name section name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare byte-to-byte.
    Key_section & section(const ustring & sect, bool similar=false);

    /// Gets a list of all sections in the Key_file.
    std::vector<ustring> list_sections() const;

    /// Gets a list of all keys within given section.
    std::vector<ustring> list_keys(const Key_section & sect) const;

    /// Test if empty.
    bool empty() const;

    /// Looks whether the key file has the section sect_name.
    /// @param sect_name section name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare byte-to-byte.
    bool has_section(const ustring & sect_name, bool similar=false) const;

    /// Looks whether the key file has the key key_name in the section named sect_name.
    /// @param sect_name section name.
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare byte-to-byte.
    bool has_key(const ustring & sect_name, const ustring & key_name, bool similar=false) const;

    /// Looks whether the key file has the key key_name in the section sect.
    /// @param sect section.
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare byte-to-byte.
    bool has_key(const Key_section & sect, const ustring & key_name, bool similar=false) const;

    /// Gets exact key name by similar name.
    ustring key_name(const Key_section & sect, const ustring similar_name) const;

    /// Places comment above sect.
    void set_comment(Key_section & sect, const ustring & comment);

    /// Get comment above section sect.
    ustring comment(Key_section & sect);

    /// Associates a new string value with key key under sect.
    void set_string(Key_section & sect, const ustring & key, const ustring & value);

    /// Sets a list of string values for key key under sect.
    void set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec);

    /// Associates a new boolean value with key key under sect.
    void set_boolean(Key_section & sect, const ustring & key, bool value);

    /// Sets a list of booleans for the key key under sect.
    void set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec);

    /// Associates a new integer value with key key under sect.
    void set_integer(Key_section & sect, const ustring & key, long long value);

    /// Sets a list of integers for the key key under sect.
    void set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec);

    /// Associates a new double value with key key under sect.
    void set_double(Key_section & sect, const ustring & key, double value);

    /// Sets a list of doubles for the key under sect.
    void set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec);

    /// Gets string value associated with key key under sect section.
    /// @return key value or fallback value if key not found.
    ustring get_string(Key_section & sect, const ustring & key, const ustring & fallback=ustring());

    /// Gets a list of strings associated with key key under sect section.
    /// @return list of values or empty list if not found.
    std::vector<ustring> get_strings(Key_section & sect, const ustring & key);

    /// Gets boolean value associated with key key under sect section.
    /// @return key value or fallback if key not found.
    bool get_boolean(Key_section & sect, const ustring & key, bool fallback=false);

    /// Gets a list of booleans associated with key key under sect section.
    /// @return list of values or empty list if not found.
    std::vector<bool> get_booleans(Key_section & sect, const ustring & key);

    /// Gets integer value associated with key key under sect section.
    /// @return key value or fallback if key not found.
    long long get_integer(Key_section & sect, const ustring & key, long long fallback=0);

    /// Gets a list of integers associated with key key under sect section.
    /// @return list of values or empty list if not found.
    std::vector<long long> get_integers(Key_section & sect, const ustring & key);

    /// Gets double value associated with key key under sect section.
    /// @return key value or fallback if key not found.
    double get_double(Key_section & sect, const ustring & key, double fallback=0.0);

    /// Gets a list of doubles associated with key key under sect section.
    /// @return list of values or empty list if not found.
    std::vector<double> get_doubles(Key_section & sect, const ustring & key);

    /// Remove key key form the section sect.
    /// @param sect section.
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare byte-to-byte.
    void remove_key(Key_section & sect, const ustring & key, bool similar=false);

    /// Remove section.
    /// @param sect_name section name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare byte-to-byte.
    void remove_section(const ustring & sect_name, bool similar=false);

    /// Remove all sections.
    void clear();

    /// Gets "signal_changed".
    signal<void()> & signal_changed();

private:

    Key_file_impl * impl;
};

} // namespace tau

#endif // TAU_KEY_FILE_HH
