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

#ifndef TAU_LOCALE_IMPL_HH
#define TAU_LOCALE_IMPL_HH

#include <tau/locale.hh>

namespace tau {

// Get X/Open style locale specification.
std::string locale_spec();

// Get language component from X/Open style locale specification.
std::string locale_language(const std::string & locale);

// Get territory component from X/Open style locale specification.
std::string locale_territory(const std::string & locale);

// Get codeset component from X/Open style locale specification.
std::string locale_encoding(const std::string & locale);

// Get modifier component from X/Open style locale specification.
std::string locale_modifier(const std::string & locale);

// Get language component from X/Open style locale specification.
std::string locale_language();

// Get territory component from X/Open style locale specification.
std::string locale_territory();

// Get codeset component from X/Open style locale specification.
std::string locale_encoding();

// Get modifier component from X/Open style locale specification.
std::string locale_modifier();

// Check codeset is UTF-8.
bool locale_is_utf8(const std::string & locale);

// Check locale is UTF-8.
bool locale_is_utf8();

// Get encoding used for file names.
std::string filename_encoding();

} // namespace tau

#endif // TAU_LOCALE_IMPL_HH
