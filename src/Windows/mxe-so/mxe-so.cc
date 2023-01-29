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

#include <tau/string.hh>
#include <sys-impl.hh>
#include <Windows/loop-win.hh>
#include <iostream>
#include <shlobj.h>

namespace tau {

void Loop_impl::boot_linkage() {
    sysinfo_.shared = true;
    std::list<ustring> v(1, path_dirname(path_self()));
    wchar_t ws[2048];

    if (S_OK == SHGetFolderPathW(NULL, CSIDL_WINDOWS, NULL, 0, ws)) {
        ustring s = str_from_wstring(std::wstring(ws));
        v.push_back(s);
        v.push_back(path_build(s, "System"));
    }

    if (S_OK == SHGetFolderPathW(NULL, CSIDL_SYSTEM, NULL, 0, ws)) {
        v.push_back(str_from_wstring(std::wstring(ws)));
    }

    const ustring solink = str_format("libtau-", Major_, '.', Minor_, '-', sysinfo_.target, "-mxe.dll");

    for (auto & s: v) {
        ustring sopath = path_build(s, solink);
        if (file_exists(sopath)) { sysinfo_.sopath = sopath; break; }
    }

    setup_sysinfo_win();
}

} // namespace tau

//END
