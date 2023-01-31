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

    /// @name Constructors and operators
    /// @{

    /// Default constructor.
    ///
    /// @param list_sep list separator character.
    /// @param comment_sep list separator character.
    /// @overload Key_file(char32_t list_sep, char32_t comment_sep)
    Key_file(char32_t list_sep=U'\0', char32_t comment_sep=U'\0');

    /// Constructor with stream.
    ///
    /// If the input stream isn't good(), the constructor does nothing.
    ///
    /// @param is the input stream.
    /// @param list_sep list separator character.
    /// @param comment_sep list separator character.
    /// @overload Key_file(std::istream & is, char32_t list_sep, char32_t comment_sep)
    Key_file(std::istream & is, char32_t list_sep=U'\0', char32_t comment_sep=U'\0');

    /// Constructor with file name.
    ///
    /// Given path saved and can be used later to save content using
    /// save() method (without arguments).
    ///
    /// @param path path to the file.
    /// @param list_sep list separator character.
    /// @param comment_sep list separator character.
    /// @overload Key_file(const ustring & path, char32_t list_sep, char32_t comment_sep)
    Key_file(const ustring & path, char32_t list_sep=U'\0', char32_t comment_sep=U'\0');

    /// Copy constructor.
    /// @overload Key_file(const Key_file & other)
    Key_file(const Key_file & other);

    /// Copy operator,
    Key_file & operator=(const Key_file & other);

    /// Move constructor.
    /// @overload Key_file(Key_file && other)
    Key_file(Key_file && other);

    /// Move operator,
    Key_file & operator=(Key_file && other);

    /// Destructor.
    /// Since 0.4.0 calls flush().
   ~Key_file();

    /// @}
    /// @name Input/Output
    /// @{

    /// Load from stream.
    /// @throw sys_error in case of OS error.
    /// @overload load(std::istream & is)
    void load(std::istream & is);

    /// Load from file.
    ///
    /// If file was successfuly loaded, given path stored and can be used later
    /// to save file using save() method (without arguments).
    ///
    /// @overload load(const ustring & path)
    /// @throw sys_error in case of OS error.
    void load(const ustring & path);

    /// Save to stream.
    /// Resets "changed" state (since 0.4.0).
    /// @overload save(std::ostream & os) const
    /// @throw sys_error in case of OS error.
    /// @sa changed()
    /// @sa signal_changed()
    void save(std::ostream & os) const;

    /// Save to file.
    /// If directory, containing destination file, does not exist, this call
    /// attempts to create it using path_mkdir() call.
    ///
    /// Resets "changed" state (since 0.4.0).
    /// @overload save(const ustring & path)
    /// @throw sys_error in case of OS error.
    /// @sa changed()
    /// @sa signal_changed()
    void save(const ustring & path);

    /// Special form of the save(const ustring &) method.
    ///
    /// If key file was loaded from file using load(const ustring &) method,
    /// or was constructed using Key_file(const ustring &, char32_t, char32_t),
    /// this call will save it back. If key file wasn't loaded from file that way, the user_error
    /// will be thrown. If file specified for load(const ustring &) or Key_file(const ustring &, char32_t, char32_t)
    /// does not exist, save() anyway should try to perform write operation.
    ///
    /// If directory, containing destination file, does not exist, this call
    /// attempts to create it using path_mkdir() call.
    ///
    /// Resets "changed" state (since 0.4.0).
    /// @overload save()
    /// @throw user_error if key file wasn't loaded from file.
    /// @throw sys_error in case of OS error.
    /// @sa Key_file(const ustring &, char32_t, char32_t)
    /// @sa load(const ustring &)
    /// @sa save(const ustring &)
    /// @sa flush()
    /// @sa changed()
    /// @sa signal_changed()
    void save();

    /// Flush file to the disk.
    ///
    /// If key file was loaded from file using load(const ustring &) method,
    /// or was constructed using Key_file(const ustring &, char32_t, char32_t),
    /// this call will save it back in case the key file had changed. If key file was not loaded from the disk
    /// one of above methods or file specified for load(const ustring &)
    /// or Key_file(const ustring &, char32_t, char32_t) does not exist, this call have no effect.
    ///
    /// If directory, containing destination file, does not exist, this call
    /// attempts to create it using path_mkdir() call.
    ///
    /// Resets "changed" state.
    ///
    /// Does not throw exceptions in case of error, only issues messages to the @a std::cerr.
    ///
    /// @since 0.4.0
    /// @sa Key_file(const ustring &, char32_t, char32_t)
    /// @sa load(const ustring &)
    /// @sa save(const ustring &)
    /// @sa changed()
    /// @sa signal_changed()
    void flush();

    /// @}
    /// @name Getters
    /// @{

    /// Get comment above sect section.
    /// @overload comment(Key_section & sect)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    ustring comment(Key_section & sect);

    /// Get comment above root section.
    /// @overload comment()
    /// @since 0.4.0
    ustring comment();

    /// Gets string value associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @overload get_string(Key_section & sect, const ustring & key, const ustring & fallback)
    /// @return key value or fallback value if key not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    ustring get_string(Key_section & sect, const ustring & key, const ustring & fallback=ustring());

    /// Gets string value associated with key under root section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @return key value or fallback value if key not found.
    /// @overload get_string(const ustring & key, const ustring & fallback)
    /// @since 0.4.0
    ustring get_string(const ustring & key, const ustring & fallback=ustring());

    /// Gets a list of strings associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @overload get_strings(Key_section & sect, const ustring & key)
    /// @return list of values or empty list if not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    std::vector<ustring> get_strings(Key_section & sect, const ustring & key);

    /// Gets a list of strings associated with key under root section.
    /// @param key key name.
    /// @return list of values or empty list if not found.
    /// @overload get_strings(const ustring & key)
    /// @since 0.4.0
    std::vector<ustring> get_strings(const ustring & key);

    /// Gets boolean value associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @overload get_boolean(Key_section & sect, const ustring & key, bool fallback)
    /// @return key value or fallback if key not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    bool get_boolean(Key_section & sect, const ustring & key, bool fallback=false);

    /// Gets boolean value associated with key under root section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @return key value or fallback if key not found.
    /// @overload get_boolean(const ustring & key, bool fallback)
    /// @since 0.4.0
    bool get_boolean(const ustring & key, bool fallback=false);

    /// Gets a list of Boolean's associated with key key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @overload get_booleans(Key_section & sect, const ustring & key)
    /// @return list of values or empty list if not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    std::vector<bool> get_booleans(Key_section & sect, const ustring & key);

    /// Gets a list of Boolean's associated with key key under root section.
    /// @param key key name.
    /// @return list of values or empty list if not found.
    /// @overload get_booleans(const ustring & key)
    /// @since 0.4.0
    std::vector<bool> get_booleans(const ustring & key);

    /// Gets integer value associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @overload get_integer(Key_section & sect, const ustring & key, long long fallback)
    /// @return key value or fallback if key not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    long long get_integer(Key_section & sect, const ustring & key, long long fallback=0);

    /// Gets integer value associated with key under root section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @return key value or fallback if key not found.
    /// @overload get_integer(const ustring & key, long long fallback)
    /// @since 0.4.0
    long long get_integer(const ustring & key, long long fallback=0);

    /// Gets a list of integers associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @overload get_integers(Key_section & sect, const ustring & key)
    /// @return list of values or empty list if not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    std::vector<long long> get_integers(Key_section & sect, const ustring & key);

    /// Gets a list of integers associated with key under root section.
    /// @param key key name.
    /// @return list of values or empty list if not found.
    /// @overload get_integers(const ustring & key)
    /// @since 0.4.0
    std::vector<long long> get_integers(const ustring & key);

    /// Gets double value associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @overload get_double(Key_section & sect, const ustring & key, double fallback)
    /// @return key value or fallback if key not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    double get_double(Key_section & sect, const ustring & key, double fallback=0.0);

    /// Gets double value associated with key under root section.
    /// @param key key name.
    /// @param fallback value to be returned if key not found.
    /// @return key value or fallback if key not found.
    /// @overload get_double(const ustring & key, double fallback)
    /// @since 0.4.0
    double get_double(const ustring & key, double fallback=0.0);

    /// Gets a list of doubles associated with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @overload get_doubles(Key_section & sect, const ustring & key)
    /// @return list of values or empty list if not found.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    std::vector<double> get_doubles(Key_section & sect, const ustring & key);

    /// Gets a list of doubles associated with key under root section.
    /// @param key key name.
    /// @return list of values or empty list if not found.
    /// @overload get_doubles(const ustring & key)
    /// @since 0.4.0
    std::vector<double> get_doubles(const ustring & key);

    /// @}
    /// @name Setters
    /// @{

    /// Places comment above sect section.
    /// @overload set_comment(Key_section & sect, const ustring & comment)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_comment(Key_section & sect, const ustring & comment);

    /// Places comment above root section.
    /// @overload set_comment(const ustring & comment)
    /// @since 0.4.0
    void set_comment(const ustring & comment);

    /// Associates a new string value with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_string(Key_section & sect, const ustring & key, const ustring & value)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_string(Key_section & sect, const ustring & key, const ustring & value);

    /// Associates a new string value with key under root section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_string(const ustring & key, const ustring & value)
    /// @since 0.4.0
    void set_string(const ustring & key, const ustring & value);

    /// Sets a list of string values for key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_strings(Key_section & sect, const ustring & key, const std::vector<ustring> & vec);

    /// Sets a list of string values for key under root section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_strings(const ustring & key, const std::vector<ustring> & vec)
    /// @since 0.4.0
    void set_strings(const ustring & key, const std::vector<ustring> & vec);

    /// Associates a new boolean value with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_boolean(Key_section & sect, const ustring & key, bool value)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_boolean(Key_section & sect, const ustring & key, bool value);

    /// Associates a new boolean value with key under root section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_boolean(const ustring & key, bool value)
    /// @since 0.4.0
    void set_boolean(const ustring & key, bool value);

    /// Sets a list of Boolean's for the key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_booleans(Key_section & sect, const ustring & key, const std::vector<bool> & vec);

    /// Sets a list of Boolean's for the key under root section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_booleans(const ustring & key, const std::vector<bool> & vec)
    /// @since 0.4.0
    void set_booleans(const ustring & key, const std::vector<bool> & vec);

    /// Associates a new integer value with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_integer(Key_section & sect, const ustring & key, long long value)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_integer(Key_section & sect, const ustring & key, long long value);

    /// Associates a new integer value with key under root section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_integer(const ustring & key, long long value)
    /// @since 0.4.0
    void set_integer(const ustring & key, long long value);

    /// Sets a list of integers for the key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_integers(Key_section & sect, const ustring & key, const std::vector<long long> & vec);

    /// Sets a list of integers for the key under root section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_integers(const ustring & key, const std::vector<long long> & vec)
    /// @since 0.4.0
    void set_integers(const ustring & key, const std::vector<long long> & vec);

    /// Associates a new double value with key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_double(Key_section & sect, const ustring & key, double value)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_double(Key_section & sect, const ustring & key, double value);

    /// Associates a new double value with key under root section.
    /// @param key key name.
    /// @param value value to be saved.
    /// @overload set_double(const ustring & key, double value)
    /// @since 0.4.0
    void set_double(const ustring & key, double value);

    /// Sets a list of doubles for the key under sect section.
    /// @param sect section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void set_doubles(Key_section & sect, const ustring & key, const std::vector<double> & vec);

    /// Sets a list of doubles for the key under root section.
    /// @param key key name.
    /// @param vec vector of values.
    /// @overload set_doubles(const ustring & key, const std::vector<double> & vec)
    /// @since 0.4.0
    void set_doubles(const ustring & key, const std::vector<double> & vec);

    /// @{
    /// @name Controls
    /// @{

    /// Set comment separator.
    /// @note By default, the comment separator is a '#'.
    void set_comment_separator(char32_t comment_sep);

    /// Sets the character which is used to separate values in lists.
    /// @note By default, the list separator is path_sep_.
    void set_list_separator(char32_t list_sep);

    /// Gets list separator.
    char32_t list_separator() const;

    /// Gets comment separator.
    char32_t comment_separator() const;

    /// Get root section.
    Key_section & root();

    /// Get specified section.
    /// @param sect_name section name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare sect_name byte-to-byte.
    Key_section & section(const ustring & sect_name, bool similar=false);

    /// Gets a list of all sections in the Key_file.
    std::vector<ustring> list_sections() const;

    /// Gets a list of all keys within given section.
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    std::vector<ustring> list_keys(const Key_section & sect) const;

    /// Test if empty.
    bool empty() const;

    /// Looks whether the key file has the section sect_name.
    ///
    /// @param sect_name section name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare sect_name byte-to-byte.
    bool has_section(const ustring & sect_name, bool similar=false) const;

    /// Looks whether the key file has the key key_name in the section named sect_name.
    ///
    /// @param sect_name section name.
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare both names byte-to-byte.
    /// @overload has_key(const ustring & sect_name, const ustring & key_name, bool similar) const
    bool has_key(const ustring & sect_name, const ustring & key_name, bool similar=false) const;

    /// Looks whether the key file has the key key_name in the section sect.
    ///
    /// @param sect section.
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare key_name byte-to-byte.
    /// @overload has_key(const Key_section & sect, const ustring & key_name, bool similar) const
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    bool has_key(const Key_section & sect, const ustring & key_name, bool similar=false) const;

    /// Looks whether the key file has the key key_name in the root section.
    ///
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare key_name byte-to-byte.
    /// @overload has_key(const ustring & key_name, bool similar) const
    /// @since 0.4.0
    bool has_key(const ustring & key_name, bool similar=false) const;

    /// Gets exact key name by similar name within specified section.
    /// @param sect section.
    /// @param similar_name key name.
    /// @overload key_name(const Key_section & sect, const ustring & similar_name) const
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    ustring key_name(const Key_section & sect, const ustring & similar_name) const;

    /// Gets exact key name by similar name within root section.
    /// @param similar_name key name.
    /// @overload key_name(const ustring & similar_name) const
    /// @since 0.4.0
    ustring key_name(const ustring & similar_name) const;

    /// Remove key key form the sect section.
    ///
    /// @param sect section.
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare key_name byte-to-byte.
    /// @overload remove_key(Key_section & sect, const ustring & key_name, bool similar)
    /// @throw user_error if sect is foreign (belongs to another Key_file) (since 0.4.0).
    void remove_key(Key_section & sect, const ustring & key_name, bool similar=false);

    /// Remove key key form the root section.
    ///
    /// @param key_name key name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare key_name byte-to-byte.
    /// @overload remove_key(const ustring & key_name, bool similar)
    /// @since 0.4.0
    void remove_key(const ustring & key_name, bool similar=false);

    /// Remove section.
    ///
    /// @param sect_name section name.
    /// @param similar if @b true, use str_similar() for lookup (case insensitive), else compare sect_name byte-to-byte.
    void remove_section(const ustring & sect_name, bool similar=false);

    /// Remove all sections.
    void clear();

    /// Lock file (disable modification).
    void lock();

    /// Unlock file (enable modification).
    void unlock();

    /// Test if locked.
    bool locked() const;

    /// Test if changed.
    /// @since 0.4.0
    bool changed() const;

    /// Gets "signal_changed".
    signal<void()> & signal_changed();

    /// @}

private:

    Key_file_impl * impl;
};

/// Stream Key_file in.
/// @relates Key_file
std::istream & operator>>(std::istream & is, Key_file & kf);

/// Stream Key_file out.
/// @relates Key_file
std::ostream & operator<<(std::ostream & os, const Key_file & kf);

} // namespace tau

#endif // TAU_KEY_FILE_HH
