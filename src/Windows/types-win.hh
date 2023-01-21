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

#ifndef TAU_TYPES_WIN_HH
#define TAU_TYPES_WIN_HH

#include <tau/painter.hh>
#include <tau/string.hh>
#include <tau/geometry.hh>
#include <types-impl.hh>
#include <windows.h>
#include <windowsx.h>
#include <memory>

namespace tau {

class Dialog_win;
using Dialog_win_ptr = std::shared_ptr<Dialog_win>;
using Dialog_win_cptr = std::shared_ptr<const Dialog_win>;

class Display_win;
using Display_win_ptr = std::shared_ptr<Display_win>;
using Display_win_cptr = std::shared_ptr<const Display_win>;

class Font_win;
using Font_win_ptr = std::shared_ptr<Font_win>;
using Font_win_cptr = std::shared_ptr<const Font_win>;

class Loop_win;
using Loop_win_ptr = std::shared_ptr<Loop_win>;
using Loop_win_cptr = std::shared_ptr<const Loop_win>;

class Painter_win;
using Painter_win_ptr = std::shared_ptr<Painter_win>;
using Painter_win_cptr = std::shared_ptr<const Painter_win>;

class Pixmap_win;
using Pixmap_win_ptr = std::shared_ptr<Pixmap_win>;
using Pixmap_win_cptr = std::shared_ptr<const Pixmap_win>;

class Popup_win;
using Popup_win_ptr = std::shared_ptr<Popup_win>;
using Popup_win_cptr = std::shared_ptr<const Popup_win>;

class Theme_win;
using Theme_win_ptr = std::shared_ptr<Theme_win>;
using Theme_win_cptr = std::shared_ptr<const Theme_win>;

class Toplevel_win;
using Toplevel_win_ptr = std::shared_ptr<Toplevel_win>;
using Toplevel_win_cptr = std::shared_ptr<const Toplevel_win>;

class Winface_win;
using Winface_win_ptr = std::shared_ptr<Winface_win>;
using Winface_win_cptr = std::shared_ptr<const Winface_win>;

std::wstring str_to_wstring(const ustring & s);
std::wstring str32_to_wstring(const std::u32string & s);
ustring str_from_wstring(const std::wstring & ws);
ustring ustr_error(DWORD error);
RECT to_winrect(const Rect & r);
Rect from_winrect(const RECT & rect);
POINT to_winpoint(const Point & pt);
int winrop(Oper op);
std::vector<ustring> enum_dc_fonts(HDC hdc);
Vector pointfx_to_vector(const POINTFX & pfx);
bool operator!=(const POINTFX & pfx1, const POINTFX & pfx2);
void setup_sysinfo_win();

} // namespace tau

#endif // TAU_TYPES_WIN_HH
