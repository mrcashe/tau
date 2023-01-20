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

#include <tau/exception.hh>
#include <tau/fileinfo.hh>
#include <tau/font.hh>
#include <tau/locale.hh>
#include <tau/timeval.hh>
#include <tau/sys.hh>
#include <tau/string.hh>
#include <locale-impl.hh>
#include "theme-posix.hh"
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <glob.h>
#include <pwd.h>
#include <unistd.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace tau {

const char32_t path_slash_ = U'/';
const char32_t path_sep_ = U':';

sys_error::sys_error(const ustring & extra_msg) {
    gerror_ = errno;
    msg_ = strerror(gerror_);
    if (!extra_msg.empty()) { msg_ += " ("+extra_msg+")"; }
}

struct tm Timeval::gmtime() const {
    time_t t(usec_/1000000);
    struct tm res;
    gmtime_r(&t, &res);
    return res;
}

struct tm Timeval::localtime() const {
    time_t t(usec_/1000000);
    struct tm res;
    localtime_r(&t, &res);
    return res;
}

std::string locale_spec() {
    const char * s;

    s = getenv("LC_ALL");
    if (s && '\0' != *s) { return s; }

    s = getenv("LC_MESSAGES");
    if (s && '\0' != *s) { return s; }

    s = getenv("LANGUAGE");
    if (s && '\0' != *s) { return s; }

    s = getenv("LANG");
    if (s && '\0' != *s) { return s; }

    return "C";
}

std::string filename_encoding() {
    return locale_encoding();
}

bool path_is_absolute(const ustring & path) {
    return '/' == *path.begin();
}

ustring path_home() {
    const char * home = getenv("HOME");

    if (!home || '\0' == *home) {
        struct passwd pwd;
        struct passwd * pw = nullptr;
        char buffer[16384];
        const char * logname = getenv("LOGNAME");

        if (!logname || '\0' == *logname) {
            int result = getpwnam_r(logname, &pwd, buffer, sizeof(buffer), &pw);
            if (0 == result && pw && pw->pw_uid == getuid()) { return Locale().decode(pw->pw_dir); }
        }

        int result = getpwuid_r(getuid(), &pwd, buffer, sizeof(buffer), &pw);
        if (0 == result && pw) { return Locale().decode(pw->pw_dir); }
        return ustring(); // FIXME What to do in that case?
    }

    return Locale().decode(home);
}

ustring user_name() {
    struct passwd pwd;
    struct passwd * pw = nullptr;
    char buffer[16384];
    const char * logname = getenv("LOGNAME");

    if (logname) {
        int result = getpwnam_r(logname, &pwd, buffer, sizeof(buffer), &pw);
        if (0 == result && pw && pw->pw_uid == getuid()) { return Locale().decode(pw->pw_name); }
    }

    int result = getpwuid_r(getuid(), &pwd, buffer, sizeof(buffer), &pw);
    if (0 == result && pw) { return Locale().decode(pw->pw_name); }
    return "somebody";
}

ustring path_user_data_dir() {
    const char * env = getenv("XDG_DATA_HOME");

    if (env && '\0' != *env) {
        ustring dir = Locale().decode(env);
        if (path_is_absolute(dir)) { return dir; }
    }

    ustring dir = path_home();

    if (!dir.empty()) {
        return path_build(dir, ".local", "share");
    }

    return path_build(path_build(path_tmp(), user_name()), ".local", "share");
}

ustring path_user_config_dir() {
    const char * env = getenv("XDG_CONFIG_HOME");

    if (env && '\0' != *env) {
        ustring dir = Locale().decode(env);
        if (path_is_absolute(dir)) { return dir; }
    }

    ustring dir = path_home();

    if (!dir.empty()) {
        return path_build(dir, ".config");
    }

    return path_build(path_tmp(), user_name(), ".config");
}

ustring path_user_cache_dir() {
    const char * env = getenv("XDG_CACHE_HOME");

    if (env && '\0' != *env) {
        ustring dir = Locale().decode(env);
        if (path_is_absolute(dir)) { return dir; }
    }

    ustring dir = path_home();

    if (!dir.empty()) {
        return path_build(dir, ".cache");
    }

    return path_build(path_tmp(), user_name(), ".cache");
}

ustring path_user_runtime_dir() {
    const char * env = getenv("XDG_RUNTIME_DIR");

    if (env && '\0' != *env) {
        ustring dir = Locale().decode(env);
        if (path_is_absolute(dir)) { return dir; }
    }

    return path_user_cache_dir();
}

ustring path_tmp() {
    const char * env = getenv("TMPDIR");

    if (env && '\0' != *env) {
        ustring dir = Locale().decode(env);
        if (path_is_absolute(dir)) { return dir; }
    }

    return "/tmp";
}

ustring path_cwd() {
    const char * pwd = getenv("PWD");

    if (pwd && '\0' != *pwd) {
        return Locale().decode(pwd);
    }

    char wd[1+PATH_MAX];

    if (getcwd(wd, PATH_MAX)) { return
        Locale().decode_filename(wd);
    }

    return ustring();
}

std::vector<ustring> path_list(const ustring & path) {
    DIR * d = opendir(Locale().encode_filename(path).c_str());
    if (!d) { throw sys_error(); }
    struct dirent * de;
    std::vector<ustring> v;

    do {
        de = readdir(d);
        if (de) { v.push_back(Locale().decode_filename(de->d_name)); }
    } while (de);

    closedir(d);
    return v;
}

std::vector<ustring> path_glob(const ustring & mask) {
    std::vector<ustring> v;

    glob_t gl;
    int result = glob(Locale().encode_filename(mask).c_str(), GLOB_NOSORT|GLOB_TILDE, 0, &gl);

    if (GLOB_NOSPACE == result) {
        throw std::bad_alloc();
    }

    else if (GLOB_ABORTED == result) {
        throw internal_error("glob(): GLOB_ABORTED returned");
    }

    else if (0 == result) {
        for (std::size_t i = 0; i < gl.gl_pathc; ++i) {
            v.push_back(Locale().decode_filename(gl.gl_pathv[i]));
        }
    }

    globfree(&gl);
    return v;
}

void path_mkdir(const ustring & path) {
    if (file_exists(path) && !file_is_dir(path)) {
        throw user_error(str_format("file '", path, "' exists but not a directory"));
    }

    if (!file_is_dir(path)) {
        ustring parent = path_dirname(path);
        if (!file_exists(parent)) { path_mkdir(parent); }
        if (0 != mkdir(Locale().encode_filename(path).c_str(), 0755)) { throw sys_error(path); }
    }
}

ustring path_dirname(const ustring & path) {
    ustring::size_type pos = path.find_last_of("/\\");
    if (ustring::npos != pos) { return 0 != pos ? path.substr(0, pos) : "/"; }
    return ".";
}

ustring path_real(const ustring & path) {
    Locale loc;
    char * res_path = realpath(loc.encode_filename(path).c_str(), nullptr);

    if (res_path) {
        ustring res = loc.decode_filename(res_path);
        std::free(res_path);
        return res;
    }

    return path;
}

bool path_match(const ustring & pattern, const ustring & path) {
    Locale loc;
    return 0 == fnmatch(loc.encode_filename(pattern).c_str(), loc.encode_filename(path).c_str(), FNM_PATHNAME|FNM_PERIOD);
}

std::vector<ustring> path_which(const ustring & cmd) {
    std::vector<ustring> v;

    if (ustring::npos == cmd.find_first_of("/\\")) {
        char * env = getenv("PATH");

        if (env) {
            auto vv = str_explode(Locale().decode_filename(env), ':');

            for (const ustring & s: vv) {
                ustring path = path_build(s, cmd);
                if (Fileinfo(path).is_exec()) { v.push_back(path); }
            }
        }
    }

    return v;
}

void msleep(unsigned time_ms) {
    usleep(1000*time_ms);
}

// static
std::vector<ustring> Font::list_families() {
    return Theme_posix::root_posix()->list_families();
}

// static
std::vector<ustring> Font::list_faces(const ustring & family) {
    return Theme_posix::root_posix()->list_faces(family);
}

} // namespace tau

//END
