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
#include <iostream>

namespace tau {

std::wstring str_to_wstring(const ustring & str) {
    std::wstring ws;

    for (std::size_t i = 0; i < str.size(); ++i) {
        char32_t wc = str[i];

        if (wc >= 0x00110000 || (wc >= 0x0000d800 && wc < 0x0000e000)) { // illegal sequence.
            break;
        }

        if (wc < 0x00010000) {
            ws += char16_t(wc);
        }

        else {
            ws += (wc-0x00010000) / 0x400 + 0xd800;
            ws += (wc-0x00010000) % 0x400 + 0xdc00;
        }
    }

    return ws;
}

std::wstring str32_to_wstring(const std::u32string & str) {
    std::wstring ws;

    for (std::size_t i = 0; i < str.size(); ++i) {
        char32_t wc = str[i];

        if (wc >= 0x00110000 || (wc >= 0x0000d800 && wc < 0x0000e000)) { // illegal sequence.
            break;
        }

        if (wc < 0x00010000) {
            ws += char16_t(wc);
        }

        else {
            ws += (wc-0x00010000) / 0x400 + 0xd800;
            ws += (wc-0x00010000) % 0x400 + 0xdc00;
        }
    }

    return ws;
}

ustring str_from_wstring(const std::wstring & ws) {
    ustring s;
    wchar_t high_surr = 0;

    for (unsigned i = 0; i < ws.size(); ++i) {
        wchar_t wc = ws[i];

        if (0xdc00 <= wc && wc <= 0xdfff) {
            if (0 == high_surr) {
                // illegal sequence
                return s;
            }

            s += static_cast<char32_t>((high_surr-0xd800)*0x400+wc-0xdc00+0x10000);
            high_surr = 0;
        }

        else {
            if (0 != high_surr) {
                // illegal sequence
                return s;
            }

            if (0xd800 <= wc && wc <= 0xdbff) { // high surrogate
                high_surr = wc;
            }

            else {
                s += static_cast<char32_t>(wc);
            }
        }
    }

    return s;
}

ustring ustr_error(DWORD error) {
    LPWSTR bufp = NULL;
    ustring result;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL, error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                   (LPWSTR)&bufp, 0, NULL);

    if (bufp) {
        result = str_trimright(str_from_wstring(std::wstring(bufp)));
        LocalFree(bufp);
    }

    return result;
}

RECT to_winrect(const Rect & r) {
    RECT wr;
    wr.left = r.left();
    wr.right = 1+r.right();
    wr.top = r.top();
    wr.bottom = 1+r.bottom();
    return wr;
}

Rect from_winrect(const RECT & rect) {
    return Rect(Point(rect.left, rect.top), Point(rect.right, rect.bottom));
}

static int CALLBACK efunc(const LOGFONTW * lf, const TEXTMETRICW * tm, DWORD ftype, LPARAM lp) {
    if (TRUETYPE_FONTTYPE == ftype) {
        std::vector<ustring> * vsp = reinterpret_cast<std::vector<ustring> *>(lp);
        vsp->push_back(str_from_wstring(std::wstring(lf->lfFaceName)));
    }

    return 1;
}

// Fonts with @ before face name are enabled for vertical text.
std::vector<ustring> enum_dc_fonts(HDC hdc) {
    LOGFONTW lf;
    memset(&lf, 0, sizeof(lf));
    std::vector<ustring> sv;
    EnumFontFamiliesExW(hdc, &lf, efunc, DWORD(&sv), 0);
    return sv;
}

Vector pointfx_to_vector(const POINTFX & pfx) {
    double x = pfx.x.value+(1.0/65536.0)*pfx.x.fract;
    double y = pfx.y.value+(1.0/65536.0)*pfx.y.fract;
    return Vector(x, y);
}

bool operator!=(const POINTFX & pfx1, const POINTFX & pfx2) {
    return pfx1.x.value != pfx2.x.value || pfx1.x.fract != pfx2.x.fract ||
           pfx1.y.value != pfx2.y.value || pfx1.y.fract != pfx2.y.fract;
}

POINT to_winpoint(const Point & pt) {
    POINT xp;
    xp.x = pt.x();
    xp.y = pt.y();
    return xp;
}

int winrop(Oper op) {
    static const struct { Oper op; int r2; } opers[] = {
        { OPER_COPY,    R2_COPYPEN      },
        { OPER_CLEAR,   R2_BLACK        },
        { OPER_SOURCE,  R2_COPYPEN      },
        { OPER_XOR,     R2_XORPEN       },
        { OPER_SET,     R2_WHITE        },
        { OPER_NOT,     R2_NOT          },
        { OPER_COPY,    R2_NOP          }
    };

    for (int i = 0; R2_NOP != opers[i].r2; ++i) {
        if (opers[i].op == op) {
            return opers[i].r2;
        }
    }

    return R2_COPYPEN;
}

} // namespace tau

//END
