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

/// @file base64.hh Base-64 encoder and decoder classes.

#ifndef TAU_BASE64_HH
#define TAU_BASE64_HH

#include <string>
#include <vector>

namespace tau {

/// Base-64 encoder.
/// @ingroup base64_group
class Base64_encoder {
public:

    /// Constructor.
    Base64_encoder(bool break_lines=true);

    /// Encode single byte.
    void encode(char c);

    /// Encode multiple bytes.
    void encode(const char * src, std::size_t nbytes);

    /// Get result.
    std::string result() const;

    /// Reset current state.
    void reset();

private:

    bool                 break_lines_;
    std::size_t          nchars_;
    std::string          out_;
    std::vector<uint8_t> acc_;
};

/// Base-64 decoder.
/// @ingroup base64_group
class Base64_decoder {
public:

    Base64_decoder();

    /// Decode single character.
    void decode(char c);

    /// Decode string.
    void decode(const char * src);

    /// Decode STL string.
    void decode(const std::string & src);

    /// Get result.
    std::vector<char> result() const;

    /// Reset current state.
    void reset();

private:

    unsigned            v_;
    std::string         acc_;
    std::vector<char>   out_;
};

} // namespace tau

#endif // TAU_BASE64_HH
