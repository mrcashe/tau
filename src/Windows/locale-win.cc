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

#include "types-win.hh"
#include <locale-impl.hh>
#include <tau/string.hh>

namespace tau {

std::string locale_spec() {
    LCID lcid = GetThreadLocale();
    char iso639[10];
    if (!GetLocaleInfo (lcid, LOCALE_SISO639LANGNAME, iso639, sizeof (iso639))) { return "C"; }
    char iso3166[10];
    if (!GetLocaleInfo (lcid, LOCALE_SISO3166CTRYNAME, iso3166, sizeof (iso3166))) { return "C"; }
    LANGID langid = LANGIDFROMLCID(lcid);
    std::string s;

    switch (PRIMARYLANGID(langid)) {
        case LANG_AZERI:
            switch (SUBLANGID(langid)) {
                case SUBLANG_AZERI_LATIN: s = "@Latn"; break;
                case SUBLANG_AZERI_CYRILLIC: s = "@Cyrl"; break;
            }

        break;

        case LANG_SERBIAN: // LANG_CROATIAN == LANG_SERBIAN
            switch (SUBLANGID(langid)) {
                case SUBLANG_SERBIAN_LATIN:
                case 0x06: // Serbian (Latin) - Bosnia and Herzegovina
                    s = "@Latn";
                    break;
            }

        break;

        case LANG_UZBEK:
            switch (SUBLANGID(langid)) {
                case SUBLANG_UZBEK_LATIN: s = "@Latn"; break;
                case SUBLANG_UZBEK_CYRILLIC: s = "@Cyrl"; break;
            }

        break;
    }

    return str_format(iso639, '_', iso3166, ".CP", GetACP(), s);
}

std::string filename_encoding() {
    return str_format("CP", GetOEMCP());
}

} // namespace tau

//END
