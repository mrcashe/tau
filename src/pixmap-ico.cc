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

#include <pixmap-impl.hh>
#include <tau/exception.hh>
#include <tau/sys.hh>
#include <tau/fileinfo.hh>
#include <tau/locale.hh>
#include <fstream>
#include <iostream>

namespace {

inline uint32_t u32(const char * b) {
    return uint8_t(b[0])|(uint32_t(uint8_t(b[1])) << 8)|(uint32_t(uint8_t(b[2])) << 16)|(uint32_t(uint8_t(b[3])) << 24);
}

inline uint16_t u16(const char * b) {
    return uint8_t(b[0])|(uint16_t(uint8_t(b[1])) << 8);
}

} // anonymous namespace

namespace tau {

Pixmap_ptr Pixmap_impl::load_ico_from_file(const ustring & path) {
    std::string lfp = Locale().encode_filename(path);
    std::ifstream is(lfp, std::ios::binary);
    if (!is.good()) { throw sys_error(path); }

    char buf[22];
    is.read(buf, 22);
    if (std::streamsize(22) != is.gcount()) { throw bad_pixmap("corrupted ICO header"); }
    if (0x00010000 != u32(buf)) { throw bad_pixmap("invalid ICO magic"); }
    uint16_t img_count = u16(buf+4);
    if (0 == img_count) { throw bad_pixmap("invalid ICO image count"); }

    //unsigned w = buf[6] , h = buf[7], ncolors = buf[8];
    std::streamsize nbytes = u32(buf+14);
    std::streamoff ofs = u32(buf+18);

    is.seekg(ofs);
    if (std::streampos(ofs) != is.tellg()) { throw bad_pixmap("ICO file seek failed"); }

    if (nbytes < 32768) {
        char v[nbytes];
        is.read(v, nbytes);
        if (nbytes != is.gcount()) { throw bad_pixmap("corrupted ICO data"); }
        return Pixmap_impl::load_bmp_from_memory(v, nbytes, true);
    }

    auto v = std::make_unique<char[]>(nbytes);
    is.read(v.get(), nbytes);
    if (nbytes != is.gcount()) { throw bad_pixmap("corrupted ICO data"); }
    return Pixmap_impl::load_bmp_from_memory(v.get(), nbytes, true);
}

} // namespace tau

//END
