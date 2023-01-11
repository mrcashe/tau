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
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <tau/sys.hh>
#include <tau/string.hh>
#include <tau/timeval.hh>
#include <sys/stat.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <lmcons.h>
#include <cctype>

namespace {

tau::ustring special_folder(int csidl) {
    wchar_t path[MAX_PATH+1];
    HRESULT hr;
    LPITEMIDLIST pidl = NULL;
    tau::ustring result;

    hr = SHGetSpecialFolderLocation(NULL, csidl, &pidl);
    if (hr == S_OK) {
        if (SHGetPathFromIDListW(pidl, path)) {
            result = tau::str_from_wstring(std::wstring(path));
        }

        CoTaskMemFree(pidl);
    }

    return result;
}

tau::ustring root_dir() {
    wchar_t ww[MAX_PATH];

    if (GetWindowsDirectoryW(ww, MAX_PATH)) {
        return tau::str_from_wstring(std::wstring(ww));
    }

    return "C:\\";
}

} // anon namespace

namespace tau {

const char32_t path_slash_ = U'\\';
const char32_t path_sep_ = U';';

sys_error::sys_error(const ustring & extra_msg) {
    DWORD error = GetLastError();
    ustring err = ustr_error(error);

    if (!err.empty()) {
        gerror_ = error;
        msg_ = err;
    }

    else {
        gerror_ = 0;
        msg_ = "No error";
    }

    if (!extra_msg.empty()) { msg_ += " ("+extra_msg+")"; }
}

struct tm Timeval::gmtime() const {
    time_t t(usec_/1000000);
    struct tm res;
    gmtime_s(&res, &t);
    return res;
}

struct tm Timeval::localtime() const {
    time_t t(usec_/1000000);
    struct tm res;
    localtime_s(&res, &t);
    return res;
}

bool path_is_absolute(const ustring & path) {
    if (path.size() > 2) {
        uint32_t wc = path[0];
        if (wc < 0x007f) {
            char c = toupper(wc);
            if ('A' <= c && c <= 'Z') {
                if (':' == path[1] && ('\\' == path[2] || '/' == path[2])) {
                    return true;
                }
            }
        }
    }

    return false;
}

ustring path_user_home_dir() {
    ustring p = special_folder(CSIDL_PROFILE);
    return p;
}

ustring user_name() {
    unsigned len = UNLEN+1;
    wchar_t buffer[UNLEN+1];

    if (GetUserNameW(buffer, (LPDWORD) &len)) {
        return str_from_wstring(std::wstring(buffer, len));
    }

    return "somebody";
}

ustring path_tmp_dir() {
    const char * tmp = getenv("TEMP");

    if (tmp && '\0' != *tmp) {
        return Locale().decode(tmp);
    }

    return root_dir();
}

ustring path_user_data_dir() {
    ustring data_dir = special_folder(CSIDL_LOCAL_APPDATA);

    if (!data_dir.empty()) {
        return data_dir;
    }

    return path_build(path_user_home_dir(), "Local Settings", "Application Data");
}

ustring path_user_config_dir() {
    return path_user_data_dir();
}

ustring path_user_cache_dir() {
    ustring data_dir = special_folder(CSIDL_INTERNET_CACHE);

    if (!data_dir.empty()) {
        return data_dir;
    }

    return path_build(path_user_home_dir(), "Local Settings", "Temporary Internet Files");
}

ustring path_user_runtime_dir() {
    return path_user_cache_dir();
}

ustring path_self() {
    wchar_t fn[4096];
    HMODULE hm = GetModuleHandle(NULL);
    DWORD n = GetModuleFileNameW(hm, fn, sizeof(fn)/2);
    return str_from_wstring(std::wstring(fn, n));
}

std::vector<ustring> path_list(const ustring & path) {
    WIN32_FIND_DATAW fdata;
    HANDLE h = FindFirstFileW(str_to_wstring(path_build(path, "*")).c_str(), &fdata);
    if (INVALID_HANDLE_VALUE == h) { throw sys_error(path); }

    std::vector<ustring> v;

    do {
        v.push_back(str_from_wstring(std::wstring(fdata.cFileName)));
    } while (FindNextFileW(h, &fdata));

    FindClose(h);
    return v;
}

std::vector<ustring> path_glob(const ustring & path) {
    ustring dirp = ustring::npos == path_basename(path).find_first_of("*?") ? path : path_dirname(path);
    WIN32_FIND_DATAW fdata;
    HANDLE h = FindFirstFileW(str_to_wstring(path).c_str(), &fdata);
    if (INVALID_HANDLE_VALUE == h) { throw sys_error(path); }

    std::vector<ustring> v;

    do {
        ustring s = str_from_wstring(std::wstring(fdata.cFileName));
        if ('.' != *s.begin()) {
            v.push_back(path_build(dirp, s));
        }
    } while (FindNextFileW(h, &fdata));

    FindClose(h);
    return v;
}

void path_mkdir(const ustring & path) {
    if (!file_is_dir(path)) {
        ustring parent = path_dirname(path);

        if (file_exists(parent)) {
            if (!file_is_dir(parent)) {
                throw user_error(str_format("file '", parent, "' exists but not a directory"));
            }

            if (0 != mkdir(Locale().encode(path).c_str())) {
                throw sys_error(path);
            }
        }

        path_mkdir(parent);
    }
}

ustring path_cwd() {
    DWORD len = GetCurrentDirectoryW(0, NULL);

    if (0 != len) {
        wchar_t wd[len+1];
        GetCurrentDirectoryW(len, wd);
        return str_from_wstring(std::wstring(wd));
    }

    return ustring();
}

ustring path_dirname(const ustring & path) {
    ustring::size_type pos = path.find_last_of("/\\");

    if (ustring::npos != pos) {
        if (0 == pos) { return "\\"; }

        if (2 == pos) {
            char c0 = toupper(path[0]);
            if (c0 >= 'A' && c0 <= 'Z' && ':' == path[1]) { return path.substr(0, 3); }
        }

        return path.substr(0, pos);
    }

    return ".";
}

ustring path_real(const ustring & path) {
    wchar_t buffer[MAX_PATH+1];
    wchar_t * pname;
    DWORD res = GetFullPathNameW(str_to_wstring(path).c_str(), MAX_PATH, buffer, &pname);

    if (0 != res) {
        return str_from_wstring(std::wstring(buffer));
    }

    return path;
}

bool file_is_hidden(const ustring & path) {
    DWORD attrs = GetFileAttributesW(str_to_wstring(path).c_str());

    if (INVALID_FILE_ATTRIBUTES != attrs) {
        return 0 != (FILE_ATTRIBUTE_HIDDEN & attrs);
    }

    return true;
}

std::vector<ustring> list_removable_drives() {
    std::vector<ustring> v;
    DWORD nbytes = GetLogicalDriveStringsA(0, NULL);

    if (0 != nbytes) {
        Locale loc;
        char buf[1+nbytes];
        GetLogicalDriveStringsA(1+nbytes, buf);

        for (std::size_t i = 0; i < nbytes; ++i) {
            if ('\0' != buf[i]) {
                UINT type = GetDriveType(buf+i);
                if (DRIVE_REMOVABLE == type) { v.push_back(loc.encode_filename(buf+i)); }
            }

            while ('\0' != buf[i]) { ++i; }
        }
    }

    return v;
}

bool path_match(const ustring & pattern, const ustring & path) {
    std::wstring wpattern = str_to_wstring(pattern), wpath = str_to_wstring(path);
    return PathMatchSpecW(wpath.c_str(), wpattern.c_str());
}

std::vector<ustring> path_which(const ustring & cmd) {
    std::vector<ustring> v;

    if (ustring::npos == cmd.find_first_of("/\\")) {
        char * env = getenv("PATH");

        if (env) {
            auto vv = str_explode(Locale().decode_filename(env), ':');

            for (const ustring & s: vv) {
                ustring path = path_build(s, cmd);
                if (file_is_executable(path)) { v.push_back(path); }
            }
        }
    }

    return v;
}

bool file_is_executable(const ustring & path) {
    if (!path.empty()) {
        std::wstring ws = str_to_wstring(path);
        DWORD type;

        if (GetBinaryTypeW(ws.c_str(), &type)) {
            return true;
        }
    }

    return false;
}

void file_unlink(const ustring & path) {
    std::wstring ws = str_to_wstring(path);
    if (!DeleteFileW(ws.c_str())) { throw sys_error(); }
}

void msleep(unsigned time_ms) {
    Sleep(time_ms);
}

} // namespace tau

//END
