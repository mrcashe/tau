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

/// @file sys.hh System functions.

#ifndef TAU_SYS_HH
#define TAU_SYS_HH

#include <tau/ustring.hh>
#include <vector>

namespace tau {

/// Major component of API version.
/// @ingroup sys_group
extern const int Major_;

/// Minor component of API version.
/// @ingroup sys_group
extern const int Minor_;

/// Micro component of API version.
/// @ingroup sys_group
extern const int Micro_;

/// Build path from two components.
/// @ingroup path_group
ustring path_build(const ustring & s1, const ustring & s2);

/// Build path from three components.
/// @ingroup path_group
ustring path_build(const ustring & s1, const ustring & s2, const ustring & s3);

/// Test if path absolute.
/// @ingroup path_group
bool path_is_absolute(const ustring & path);

/// Extracts all but the directory-part of the path.
/// If the file name contains no slash, it is left unchanged.
/// Otherwise, everything through the last slash is removed from it.
/// @ingroup path_group
ustring path_notdir(const ustring & path);

/// Get file extension.
/// @ingroup path_group
ustring path_suffix(const ustring & path);

/// Get file name excluding directory part and suffix.
/// @ingroup path_group
ustring path_basename(const ustring & path);

/// Get directory part of the path.
/// @ingroup path_group
ustring path_dirname(const ustring & path);

/// Get resolved path.
/// @ingroup path_group
ustring path_real(const ustring & path);

/// Get executable file path.
/// @ingroup path_group
ustring path_self();

/// Test if path matches against given template.
/// @ingroup path_group
bool path_match(const ustring & pattern, const ustring & path);

/// Get current directory.
/// @ingroup dir_group
ustring path_cwd();

/// Get temporary directory.
/// @ingroup path_group
ustring path_tmp();

/// Get program installation prefix directory.
/// @ingroup path_group
ustring path_prefix();

/// Get share directory path.
/// @ingroup path_group
ustring path_share();

/// Get user home directory path.
/// @ingroup path_group
ustring path_home();

/// Get user data directory path.
/// @ingroup path_group
ustring path_user_data_dir();

/// Get user configuration directory path.
/// @ingroup path_group
ustring path_user_config_dir();

/// Get user cache directory path.
/// @ingroup path_group
ustring path_user_cache_dir();

/// Get user runtime directory path.
/// @ingroup path_group
ustring path_user_runtime_dir();

/// Get full paths of shell commands.
/// @ingroup path_group
std::vector<ustring> path_which(const ustring & cmd);

/// @ingroup dir_group
/// Make directory with parents.
void path_mkdir(const ustring & path);

/// Lists directory.
/// Directory names are included in search result.
/// @ingroup dir_group
std::vector<ustring> path_glob(const ustring & mask);

/// Lists directory.
/// Directory names doesn't included in search result.
/// @ingroup dir_group
std::vector<ustring> path_list(const ustring & dir_path);

/// Perform recursive file search under given directory.
/// Directory names included in search result.
/// @ingroup dir_group
std::vector<ustring> path_find(const ustring & dir, const ustring & mask="*");

/// Get user name (login name).
/// @ingroup sys_group
ustring user_name();

/// Get program name.
/// @ingroup file_group
ustring program_name();

/// Test if file exist.
/// @ingroup file_group
bool file_exists(const ustring & path);

/// Test if file is directory.
/// @ingroup file_group
bool file_is_dir(const ustring & path);

/// Get environment value.
/// Gets UTF-8 encoded value obtained from environment
/// variable named @e env. If that variable is not defined,
/// fallback value returned which is empty by default.
/// @ingroup sys_group
ustring str_env(const std::string & env, const ustring & fallback=ustring());

/// Sleep for a while.
/// @param time_ms time of the sleep, in milliseconds.
/// @ingroup time_group
void msleep(unsigned time_ms);

/// A variable storing slash character using for path elements separation
/// ('/' for Unix and '\' for Windows).
/// @ingroup path_group
extern const char32_t path_slash_;

/// A variable storing path separator using for path separation in path lists
/// (':' for Unix and ';' for Windows).
/// @ingroup path_group
extern const char32_t path_sep_;

} // namespace tau

#endif // TAU_SYS_HH
