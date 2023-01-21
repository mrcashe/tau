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

#ifndef TAU_SYSINFO_HH
#define TAU_SYSINFO_HH

#include <tau/types.hh>
#include <tau/ustring.hh>

namespace tau {

/// System information structure.
/// @ingroup sys_group
struct Sysinfo {

/// Major version component, same as Major_.
int         Major;

/// Minor version component, same as Minor_.
int         Minor;

/// Micro version component, same as Micro_.
int         Micro;

/// Platform name.
///
/// Possible values are:
/// - Linux
/// - FreeBSD
/// - Windows
std::string plat;

/// Operating system name.
///
/// This is what uname(1) outputs on POSIX.
/// On Windows, so far GetVersionInfoExW() used.
ustring uname;

/// Operating system version numbers: major and minor.
/// Differ on various OSes.
int osmajor = 0, osminor = 0;

/// @name Distributive related info (actual for Linux only).
/// @{

/// Distributive name, such as "Ubuntu", "Mageia" etc.
ustring distrib;

/// Distributive major and ninor release numbers.
int distrib_major = 0, distrib_minor = 0;

/// Distributive codename.
ustring distrib_codename;

/// Distributive description.
ustring distrib_description;

/// @}
///
/// Target name.
///
/// Possible values are:
/// - x86_64-linux-gnu (g++)
/// - x86_64-pc-linux-gnu (clang++)
/// - i686-w64-mingw32.static
/// - i686-w64-mingw32.shared
/// - x86_64-w64-mingw32.static
/// - x86_64-w64-mingw32.shared
std::string target;

/// Address size, in bits.
///
/// Possible values are:
/// - 32
/// - 64
int         abits;

/// int type size, in bits.
///
/// Possible values are:
/// - 32
/// - 64
int         ibits;

/// long type size, in bits.
///
/// Possible values are:
/// - 32
/// - 64
int         lbits;

/// long long type size, in bits.
///
/// Possible values are:
/// - 32
/// - 64
int         llbits;

/// intmax_t, uintmax_t type size, in bits.
///
/// Possible values are:
/// - 32
/// - 64
int         mbits;

/// wchar_t type size, in bits.
///
/// Possible values are:
/// - 16
/// - 32
int         wcbits;

/// Linkage type
///
/// Possible values are:
///  - @b true if shared linkage;
///  - @b false if static linkage.
bool        shared;

/// Shared library path.
/// When .shared is @b false, this field is empty.
/// Else it @e must containg path to the shared library but it is possible it not.
ustring     sopath;

/// System locale.
std::string locale = "C";

/// Encoding for file i/o.
std::string iocharset;

};

/// Sysinfo is accessible using function.
/// @ingroup sys_group
//  Definition in $(confdir)/$(plat)/conf-$(plat).cc
const Sysinfo & sysinfo();

/// Get Sysinfo content as a text splited into lines.
/// @ingroup string_group
//  implementation in $(srcdir)/sys.cc
ustring str_sysinfo();

} // namespace tau

#endif // TAU_SYSINFO_HH
