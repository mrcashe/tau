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
    HRESULT hr;
    LPITEMIDLIST pidl = NULL;
    tau::ustring result;

    hr = SHGetSpecialFolderLocation(NULL, csidl, &pidl);
    if (hr == S_OK) {
        wchar_t path[MAX_PATH+1];

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

ustring ustr_error(DWORD error) {
    LPWSTR bufp = NULL;
    ustring result;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL, error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                   (LPWSTR)&bufp, 0, NULL);

    if (*bufp) {
        result = str_trimright(str_from_wstring(std::wstring(bufp)));
        LocalFree(bufp);
    }

    return result;
}

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
    struct tm res { 0 };
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
    EnumFontFamiliesExW(hdc, &lf, efunc, LPARAM(&sv), 0);
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
