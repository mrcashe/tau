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

#include <tau/exception.hh>
#include <tau/fileinfo.hh>
#include <tau/font.hh>
#include <tau/timeval.hh>
#include <tau/string.hh>
#include <locale-impl.hh>
#include <sys-impl.hh>
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

bool path_is_absolute(const ustring & path) {
    return '/' == *path.begin();
}

ustring path_home() {
    ustring home(str_env("HOME"));
    if (!home.empty()) { return home; }

    Encoding enc;
    struct passwd pwd;
    struct passwd * pw = nullptr;
    char buffer[16384];
    const char * logname = getenv("LOGNAME");

    if (!logname || '\0' == *logname) {
        int result = getpwnam_r(logname, &pwd, buffer, sizeof(buffer), &pw);

        if (0 == result && pw && pw->pw_uid == getuid()) {
            return enc.is_utf8() ? ustring(pw->pw_dir) : enc.decode(pw->pw_dir);
        }
    }

    int result = getpwuid_r(getuid(), &pwd, buffer, sizeof(buffer), &pw);
    if (0 == result && pw) { return enc.is_utf8() ? ustring(pw->pw_dir) : enc.decode(pw->pw_dir); }
    return ustring(); // FIXME What to do in that case?
}

ustring user_name() {
    struct passwd pwd;
    struct passwd * pw = nullptr;
    char buffer[16384];
    const char * logname = getenv("LOGNAME");
    auto & io = Locale().iocharset();

    if (logname) {
        int result = getpwnam_r(logname, &pwd, buffer, sizeof(buffer), &pw);
        if (0 == result && pw && pw->pw_uid == getuid()) { return io.is_utf8() ? ustring(pw->pw_name) : io.decode(pw->pw_name); }
    }

    int result = getpwuid_r(getuid(), &pwd, buffer, sizeof(buffer), &pw);
    if (0 == result && pw) { return io.is_utf8() ? ustring(pw->pw_name) : io.decode(pw->pw_name); }
    return "somebody";
}

ustring path_user_data_dir() {
    ustring dir = str_env("XDG_DATA_HOME");
    if (path_is_absolute(dir)) { return dir; }
    dir = path_home();
    if (!dir.empty()) { return path_build(dir, ".local", "share"); }
    return path_build(path_build(path_tmp(), user_name()), ".local", "share");
}

ustring path_user_config_dir() {
    ustring dir = str_env("XDG_CONFIG_HOME");
    if (path_is_absolute(dir)) { return dir; }
    dir = path_home();
    if (!dir.empty()) { return path_build(dir, ".config"); }
    return path_build(path_tmp(), user_name(), ".config");
}

ustring path_user_cache_dir() {
    ustring dir = str_env("XDG_CACHE_HOME");
    if (path_is_absolute(dir)) { return dir; }
    dir = path_home();
    if (!dir.empty()) { return path_build(dir, ".cache"); }
    return path_build(path_tmp(), user_name(), ".cache");
}

ustring path_user_runtime_dir() {
    ustring dir = str_env("XDG_RUNTIME_DIR");
    if (path_is_absolute(dir)) { return dir; }
    return path_user_cache_dir();
}

ustring path_tmp() {
    ustring dir = str_env("TMPDIR");
    if (path_is_absolute(dir)) { return dir; }
    return "/tmp";
}

ustring path_cwd() {
    ustring dir = str_env("PWD");

    if (dir.empty()) {
        char wd[1+PATH_MAX];

        if (getcwd(wd, PATH_MAX)) {
            Encoding enc;
            dir.assign(enc.is_utf8() ? ustring(wd) : enc.decode(wd));
        }
    }

    return dir;
}

std::vector<ustring> path_list(const ustring & path) {
    auto & io = Locale().iocharset();
    std::string lfp = io.is_utf8() ? std::string(path) : io.encode(path);
    DIR * d = opendir(lfp.c_str());
    if (!d) { throw sys_error(); }
    struct dirent * de;
    std::vector<ustring> v;

    do {
        de = readdir(d);
        if (de) { v.push_back(io.is_utf8() ? ustring(de->d_name) : io.decode(de->d_name)); }
    } while (de);

    closedir(d);
    return v;
}

std::vector<ustring> path_glob(const ustring & mask) {
    std::vector<ustring> v;

    glob_t gl;
    auto & io = Locale().iocharset();
    std::string lm = io.is_utf8() ? std::string(mask) : io.encode(mask);
    int result = glob(lm.c_str(), GLOB_NOSORT|GLOB_TILDE, 0, &gl);

    if (GLOB_NOSPACE == result) {
        throw std::bad_alloc();
    }

    else if (GLOB_ABORTED == result) {
        throw internal_error("glob(): GLOB_ABORTED returned");
    }

    else if (0 == result) {
        for (std::size_t i = 0; i < gl.gl_pathc; ++i) {
            v.push_back(io.is_utf8() ? ustring(gl.gl_pathv[i]) : io.decode(gl.gl_pathv[i]));
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
        auto & io = Locale().iocharset();
        std::string lfp = io.is_utf8() ? std::string(path) : io.encode(path);
        if (0 != mkdir(lfp.c_str(), 0755)) { throw sys_error(path); }
    }
}

ustring path_dirname(const ustring & path) {
    ustring::size_type pos = path.find_last_of("/\\");
    if (ustring::npos != pos) { return 0 != pos ? path.substr(0, pos) : "/"; }
    return ".";
}

ustring path_real(const ustring & path) {
    auto & io = Locale().iocharset();
    std::string lfp = io.is_utf8() ? std::string(path) : io.encode(path);
    char * res_path = realpath(lfp.c_str(), nullptr);

    if (res_path) {
        ustring res = io.is_utf8() ? ustring(res_path) : io.decode(res_path);
        std::free(res_path);
        return res;
    }

    return path;
}

bool path_match(const ustring & pattern, const ustring & path) {
    Locale loc;
    auto & io = Locale().iocharset();
    std::string lp = io.is_utf8() ? std::string(pattern) : io.encode(pattern);
    std::string lfp = io.is_utf8() ? std::string(path) : io.encode(path);
    return 0 == fnmatch(lp.c_str(), lfp.c_str(), FNM_PATHNAME|FNM_PERIOD);
}

std::vector<ustring> path_which(const ustring & cmd) {
    std::vector<ustring> v;

    if (ustring::npos == cmd.find_first_of("/\\")) {
        for (const ustring & s: str_explode(str_env("PATH"), ':')) {
            ustring path = path_build(s, cmd);
            if (Fileinfo(path).is_exec()) { v.push_back(path); }
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

ustring usystem(const ustring & cmd) {
    ustring os, utmp =  path_build(path_tmp(), "tau-XXXXXX");
    char tmp[1+utmp.bytes()];
    std::strcpy(tmp, utmp.c_str());

    try {
        int fd = mkstemp(tmp);

        if (fd > 2) {
            auto & enc = Locale().encoding();
            auto us = str_format(cmd, " > ", &tmp[0]);

            if (-1 < std::system((enc.is_utf8() ? std::string(us) : enc.encode(us)).c_str())) {
                if (std::size_t bytes = Fileinfo(tmp).bytes()) {
                    char out[bytes];
                    ssize_t nread = read(fd, out, bytes);
                    std::string sout(out, nread);
                    if (0 < nread) { os.assign(str_trim(enc.is_utf8() ? ustring(sout) : enc.decode(sout))); }
                }
            }

            close(fd);
            unlink(tmp);
        }
    }

    catch (std::exception & x) {
        std::cerr << "** tau::usystem(" << cmd << "): " << x.what() << std::endl;
    }

    catch (...) {
        std::cerr << "** tau::usystem(" << cmd << "): unknown exception caught" << std::endl;
    }

    return os;
}

void setup_sysinfo_posix() {
    ustring os = usystem("uname -a");
    sysinfo_.uname = os;
    auto v = str_explode(os);

    if (v.size() > 2) {
        auto w = str_explode(v[2], '.');
        if (w.size() > 0) { sysinfo_.osmajor = std::atoi(w[0].c_str()); }
        if (w.size() > 1) { sysinfo_.osminor = std::atoi(w[1].c_str()); }
    }

    sysinfo_.locale = Locale().code();
    sysinfo_.iocharset = Locale().iocharset().name();
}

void Locale::init1() {
    data->spec = str_env("LANG", str_env("LANGUAGE", "C"));
    auto begin = data->spec.find('.');

    if (std::string::npos != begin) {
        ++begin;
        auto end = data->spec.find('@');
        data->iocharset = data->enc = Encoding(data->spec.substr(begin, end-begin));
    }
}

} // namespace tau

//END
