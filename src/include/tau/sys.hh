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

/// @ingroup path_group
/// Build path from two components.
ustring path_build(const ustring & s1, const ustring & s2);

/// @ingroup path_group
/// Build path from three components.
ustring path_build(const ustring & s1, const ustring & s2, const ustring & s3);

/// @ingroup path_group
/// Test if path absolute.
bool path_is_absolute(const ustring & path);

/// @ingroup path_group
/// Extracts all but the directory-part of the path.
/// If the file name contains no slash, it is left unchanged.
/// Otherwise, everything through the last slash is removed from it.
ustring path_notdir(const ustring & path);

/// @ingroup path_group
/// Get file extension.
ustring path_suffix(const ustring & path);

/// @ingroup path_group
/// Get file name excluding directory part and suffix.
ustring path_basename(const ustring & path);

/// @ingroup path_group
/// Get directory part of the path.
ustring path_dirname(const ustring & path);

/// @ingroup path_group
/// Get resolved path.
ustring path_real(const ustring & path);

/// @ingroup path_group
/// Get executable file path.
ustring path_self();

/// @ingroup path_group
/// Test if path matches against given template.
bool path_match(const ustring & pattern, const ustring & path);

/// @ingroup dir_group
/// Get current directory.
ustring path_cwd();

/// @ingroup path_group
/// Get temporary directory.
ustring path_tmp_dir();

/// @ingroup path_group
/// Get program installation prefix directory.
ustring path_prefix_dir();

/// @ingroup path_group
/// Get user home directory path.
ustring path_user_home_dir();

/// @ingroup path_group
/// Get user data directory path.
ustring path_user_data_dir();

/// @ingroup path_group
/// Get user configuration directory path.
ustring path_user_config_dir();

/// @ingroup path_group
/// Get user cache directory path.
ustring path_user_cache_dir();

/// @ingroup path_group
/// Get user runtime directory path.
ustring path_user_runtime_dir();

/// @ingroup path_group
/// Get full paths of shell commands.
std::vector<ustring> path_which(const ustring & cmd);

/// @ingroup dir_group
/// Make directory with parents.
void path_mkdir(const ustring & path);

/// @ingroup dir_group
/// Lists directory.
/// Directory names are included in search result.
std::vector<ustring> path_glob(const ustring & mask);

/// @ingroup dir_group
/// Lists directory.
/// Directory names doesn't included in search result.
std::vector<ustring> path_list(const ustring & dir_path);

/// @ingroup dir_group
/// Perform recursive file search under given directory.
/// Directory names included in search result.
std::vector<ustring> path_find(const ustring & dir, const ustring & mask="*");

/// @ingroup sys_group
/// Get user name (login name).
ustring user_name();

/// @ingroup file_group
/// Get program name.
ustring program_name();

/// @ingroup file_group
/// Test if file exist.
bool file_exists(const ustring & path);

/// @ingroup file_group
/// Test if file is directory.
bool file_is_dir(const ustring & path);

/// @ingroup file_group
/// Remove file from disk.
/// @throw sys_error
void file_unlink(const ustring & path);

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
