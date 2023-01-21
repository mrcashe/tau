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

#include <tau/fileinfo.hh>
#include <tau/locale.hh>
#include <tau/string.hh>
#include <tau/string.hh>
#include <sys-impl.hh>
#include <errno.h>
#include <cstdlib>
#include <iomanip>
#include <ostream>
#include <mutex>

namespace {

using Mutex = std::recursive_mutex;
using Lock = std::lock_guard<Mutex>;

Mutex           mx_;
tau::ustring    prefix_;
tau::ustring    share_;

} // anonymous namespace

namespace tau {

const Sysinfo & sysinfo() {
    return sysinfo_;
}

ustring str_sysinfo() {
    ustring s;
    const Sysinfo & si = sysinfo();

    s += str_format("Major:          ", si.Major,    '\n');
    s += str_format("Minor:          ", si.Minor,    '\n');
    s += str_format("Micro:          ", si.Micro,    '\n');
    s += str_format("Platform:       ", si.plat,     '\n');
    s += str_format("Target:         ", si.target,   '\n');
    s += str_format("Address Bits:   ", si.abits,    '\n');
    s += str_format("int Bits:       ", si.ibits,    '\n');
    s += str_format("long Bits:      ", si.lbits,    '\n');
    s += str_format("long long Bits: ", si.llbits,   '\n');
    s += str_format("intmax_t Bits:  ", si.mbits,    '\n');
    s += str_format("Linkage:        ", (si.shared ? "shared" : "static"), '\n');
    if (si.shared) { s += str_format("Shared path:    ", (si.sopath.empty() ? "NOT FOUND" : si.sopath), '\n'); }

    return s;
}

ustring path_build(const ustring & s1, const ustring & s2) {
    const ustring dels = "/\\";
    char32_t sep = '/';

    std::size_t spos = s1.find_first_of(dels);
    if (spos != ustring::npos) { sep = s1[spos]; }
    else { spos = s2.find_first_of(dels); if (spos != ustring::npos) sep = s2[spos]; }
    std::vector<ustring> v1 = str_explode(s1, dels), v2 = str_explode(s2, dels), v3;

    for (const ustring & s: v1) {
        if (!s.empty()) {
            v3.push_back(s);
        }
    }

    for (const ustring & s: v2) {
        if (!s.empty()) {
            v3.push_back(s);
        }
    }

    ustring res;
    if (!s1.empty() && ustring::npos != dels.find(s1[0])) { res += sep; }
    res += str_implode(v3, sep);
    return res;
}

ustring path_build(const ustring & s1, const ustring & s2, const ustring & s3) {
    return path_build(path_build(s1, s2), s3);
}

ustring path_basename(const ustring & path) {
    std::size_t begin, end;
    begin = path.find_last_of("/\\");
    if (ustring::npos == begin) { begin = 0; }
    else { ++begin; }
    end = path.find_last_of('.');
    if (ustring::npos == end || end < begin) { return path.substr(begin); }
    return path.substr(begin, end-begin);
}

ustring path_suffix(const ustring & path) {
    ustring fn = path_notdir(path);
    std::size_t pos = fn.find_last_of(".");
    return pos != ustring::npos ? fn.substr(pos+1) : ustring();
}

ustring path_notdir(const ustring & path) {
    std::size_t size = path.size();

    if (size > 1) {
        if (3 == size && ':' == path[1] && ('\\' == path[2] || '/' == path[2])) {
            return path.substr(0, 2);
        }

        else {
            ustring::size_type pos = path.find_last_of("/\\");
            return pos != ustring::npos ? path.substr(pos+1) : path;
        }
    }

    return path;
}

ustring path_prefix() {
    Lock lock(mx_);

    if (prefix_.empty()) {
        ustring s = path_dirname(path_self());
        ustring name = str_tolower(path_notdir(s));

        if ("bin" == name || "lib" == name) {
            prefix_ = path_dirname(s);
        }

        else {
            prefix_ = s;
        }
    }

    return prefix_;
}

ustring path_share() {
    Lock lock(mx_);

    if (share_.empty()) {
        ustring pfx = path_prefix(), s = path_build(pfx, "share", program_name());
        if (file_is_dir(s)) { share_ = s; return share_; }
        s = path_build(pfx, "share", str_format("tau-", Major_, '.', Minor_));
        if (file_is_dir(s)) { share_ = s; return share_; }
        s = path_build(pfx, "share");
        if (file_is_dir(s)) { share_ = s; return share_; }
        share_ = pfx;
    }

    return share_;
}

ustring program_name() {
    return path_basename(path_self());
}

bool file_exists(const ustring & path) {
    return Fileinfo(path).exists();
}

bool file_is_dir(const ustring & path) {
    return Fileinfo(path).is_dir();
}

std::vector<ustring> path_find(const ustring & dir, const ustring & mask) {
    std::vector<ustring> v;

    for (const ustring & p: path_glob(path_build(dir, "*"))) {
        if (file_is_dir(p)) {
            for (const ustring & s: path_find(p, mask)) {
                v.push_back(s);
            }
        }
    }

    for (const ustring & p: path_glob(path_build(dir, path_notdir(mask)))) {
        if (!file_is_dir(p)) {
            v.push_back(p);
        }
    }

    return v;
}

} // namespace tau

//END
