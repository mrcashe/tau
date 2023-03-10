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

#ifndef TAU_FONT_FILE_HH
#define TAU_FONT_FILE_HH

#include <tau/ustring.hh>
#include "types-posix.hh"
#include <vector>

namespace tau {

class Font_file {
protected:

    Font_file() {}

public:

    static Font_file_ptr create(const ustring & fp);

    virtual ~Font_file() {}
    virtual ustring file_path() const = 0;
    virtual std::vector<ustring> list_families() const = 0;
    virtual std::vector<ustring> list_faces(const ustring & family) const = 0;
    virtual Font_face_ptr face(Font_file_ptr file, const ustring & family, const ustring & face) = 0;
    virtual Master_glyph_ptr glyph(const ustring & family, const ustring & face, char32_t wc) = 0;
    virtual std::vector<Master_glyph_ptr> glyphs(const ustring & family, const ustring & face, const std::u32string & str) = 0;

};

} // namespace tau

#endif // TAU_FONT_FILE_HH
