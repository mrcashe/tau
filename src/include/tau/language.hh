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

/// @file language.hh Language class.

#ifndef TAU_LANGUAGE_HH
#define TAU_LANGUAGE_HH

#include <tau/types.hh>
#include <tau/script.hh>
#include <vector>

namespace tau {

struct Language_data;

/// ISO language.
/// @ingroup i18n_group
class Language {
public:

    /// Construct from any ISO code or from current locale if iso_code is empty.
    Language(const std::string & iso_code=std::string());

    /// Copy constructor.
    Language(const Language & other);

    /// Copy operator.
    Language & operator=(const Language & other);

    /// Destructor.
   ~Language();

    /// Compare operator.
    bool operator==(const Language & other) const;

    /// Compare operator.
    bool operator!=(const Language & other) const;

    /// Gets longest availabale ISO 639 code.
    std::string code() const;

    /// Gets shortest availabale ISO 639 code.
    std::string shortest() const;

    /// Gets ISO 639-1 two-letter code.
    std::string code2() const;

    /// Gets ISO 639-3 three-letter code.
    std::string code3() const;

    /// Gets English name.
    std::string ename() const;

    /// Gets native name.
    ustring name() const;

    /// Gets scripts.
    std::vector<Script> scripts() const;

    /// Gets sample string.
    ustring sample() const;

private:

    Language_data * data;
};

} // namespace tau

#endif // TAU_LANGUAGE_HH
