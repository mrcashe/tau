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

/// @file encoding.hh The Encoding class.

#ifndef TAU_ENCODING_HH
#define TAU_ENCODING_HH

#include <tau/ustring.hh>

namespace tau {

class Encoding_data;

/// The encoding.
/// @ingroup i18n_group
class Encoding {
public:

    /// Default constructor.
    Encoding(const std::string & name=std::string());

    /// Copy constructor.
    Encoding(const Encoding & other);

    /// Copy operator.
    Encoding & operator=(const Encoding & other);

    /// Destructor.
   ~Encoding();

    /// Compare operator.
    bool operator==(const Encoding & other) const;

    /// Compare operator.
    bool operator!=(const Encoding & other) const;

    /// Validate encoding.
    operator bool() const;

    /// Gets encoding name.
    std::string name() const;

    /// Test if unicode.
    bool is_unicode() const;

    /// Test if multibyte.
    bool is_multibyte() const;

    /// Test if UTF-8.
    bool is_utf8() const;

    /// Test if little endian.
    bool is_little_endian() const;

    /// Convert string to UTF-8.
    ustring decode(const std::string & s) const;

    /// Convert string from UTF-8.
    std::string encode(const ustring & s) const;

private:

    Encoding_data * data;
};

} // namespace tau

#endif // TAU_ENCODING_HH
