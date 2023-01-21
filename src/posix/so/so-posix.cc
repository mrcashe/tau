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

#include <tau/string.hh>
#include <theme-impl.hh>
#include <sys-impl.hh>
#include <iostream>

namespace tau {

void Loop_impl::boot_linkage() {
    sysinfo_.shared = true;

    static const char * pfxs = "/usr/lib:/usr/local/lib";
    const ustring solink = str_format("libtau.so.", Major_, '.', Minor_, '.', Micro_);
    auto v = str_explode(pfxs, ':');
    v.insert(v.begin(), path_dirname(path_self()));

    for (auto & s: str_explode(pfxs, ':')) {
        v.push_back(path_build(s, sysinfo().target));
        v.push_back(str_format(s, sysinfo().abits));
    }

    v.push_back(path_build("/opt", program_name(), "lib"));
    v.push_back(path_build(path_prefix(), "lib"));

    for (auto & s: v) {
        ustring sopath = path_build(s, solink);
        if (file_exists(sopath)) { sysinfo_.sopath = sopath; break; }
    }
}

} // namespace tau

//END
