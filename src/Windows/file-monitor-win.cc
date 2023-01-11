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
#include <tau/sys.hh>
#include "loop-win.hh"
#include <cstring>
#include <iostream>

namespace tau {

File_monitor_win::File_monitor_win(const ustring & path, int umask):
    File_monitor(),
    umask_(umask)
{
    isdir_ = file_is_dir(path);
    dirpath_ = isdir_ ? path : path_dirname(path);

    if (!isdir_) {
        filename_ = path_notdir(path);
        wfilename_ = str_to_wstring(filename_);
        WIN32_FILE_ATTRIBUTE_DATA attrib_data;

        if (!GetFileAttributesExW(str_to_wstring(path).c_str(), GetFileExInfoStandard, &attrib_data)) {
            throw sys_error("File_monitor: GetFileAttributesExW(): "+path);
        }

        attribs_ = attrib_data.dwFileAttributes;
    }

    filter_ = FILE_NOTIFY_CHANGE_FILE_NAME|
              FILE_NOTIFY_CHANGE_DIR_NAME|
              FILE_NOTIFY_CHANGE_ATTRIBUTES|
              FILE_NOTIFY_CHANGE_SIZE;

    hdir_ = CreateFileW(str_to_wstring(dirpath_).c_str(),
        GENERIC_READ|SYNCHRONIZE|FILE_LIST_DIRECTORY,
        FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
        NULL);

    if (INVALID_HANDLE_VALUE == hdir_) {
        throw sys_error("File_monitor: CreateFileW(): "+dirpath_);
    }

    buffer_.resize(32784);
    std::memset(&overlapped_, 0, sizeof overlapped_);
    overlapped_.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!ReadDirectoryChangesW(hdir_, buffer_.data(), buffer_.size(), FALSE, filter_, NULL, &overlapped_, NULL)) {
        throw sys_error("File_monitor: ReadDirectoryChangesW(): "+dirpath_);
    }

    signal_poll_.connect(fun(this, &File_monitor_win::notify));
}

File_monitor_win::~File_monitor_win() {
    CloseHandle(overlapped_.hEvent);
    CloseHandle(hdir_);
    signal_destroy_();
}

void File_monitor_win::notify() {
    ResetEvent(overlapped_.hEvent);
    std::size_t offset = 0;
    FILE_NOTIFY_INFORMATION * pfni;

    do {
        pfni = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buffer_.data()+offset);
        ustring filename;
        std::wstring wfilename;

        if (0 != pfni->FileNameLength) {
            wfilename = std::wstring(pfni->FileName, pfni->FileNameLength/2);
            filename = str_from_wstring(wfilename);
        }

        unsigned umask = 0;

        switch(pfni->Action) {
            case FILE_ACTION_ADDED:
                if (isdir_) {
                    if (FILE_CREATED & umask_) {
                        umask = FILE_CREATED;
                    }
                }

                break;

            case FILE_ACTION_REMOVED:
                if (isdir_) {
                    if (FILE_DELETED & umask_) {
                        umask = FILE_DELETED;
                    }
                }

                else {
                    if (0 == _wcsnicmp(wfilename_.c_str(), wfilename.c_str(), wcslen(wfilename.c_str()))) {
                        if (FILE_SELF_DELETED & umask_) {
                            umask = FILE_SELF_DELETED;
                            filename.clear();
                        }
                    }
                }

                break;

            case FILE_ACTION_MODIFIED:
                if (!isdir_) {
                    if (0 == _wcsnicmp(wfilename_.c_str(), wfilename.c_str(), wcslen(wfilename.c_str()))) {
                        filename.clear();
                        WIN32_FILE_ATTRIBUTE_DATA attrib_data;
                        GetFileAttributesExW(str_to_wstring(path_build(dirpath_, filename_)).c_str(), GetFileExInfoStandard, &attrib_data);

                        if (attribs_ != attrib_data.dwFileAttributes) {
                            attribs_ = attrib_data.dwFileAttributes;

                            if (FILE_ATTRIB & umask_) {
                                umask = FILE_ATTRIB;
                            }
                        }

                        if (0 == umask && FILE_CHANGED & umask_) {
                            umask = FILE_CHANGED;
                        }
                    }
                }
                break;

            case FILE_ACTION_RENAMED_OLD_NAME:
                if (FILE_MOVED_OUT & umask_) {
                    umask = FILE_MOVED_OUT;
                }
                break;

            case FILE_ACTION_RENAMED_NEW_NAME:
                if (FILE_MOVED_IN & umask_) {
                    umask = FILE_MOVED_IN;
                }
                break;
        }

        if (0 != umask) { signal_notify_(umask, path_is_absolute(filename) ? filename : path_build(dirpath_, filename)); }
        //std::cout << "monitor " << pfni->Action << " " << filename << std::endl;
        offset += pfni->NextEntryOffset;
    } while (0 != pfni->NextEntryOffset);

    ReadDirectoryChangesW(hdir_, buffer_.data(), buffer_.size(), FALSE, filter_, NULL, &overlapped_, NULL);
}

} // namespace tau

//END
