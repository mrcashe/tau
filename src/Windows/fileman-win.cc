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
#include <tau/locale.hh>
#include <icon-impl.hh>
#include <list-impl.hh>
#include <navigator-impl.hh>
#include <fileman-impl.hh>
#include <text-impl.hh>
#include <shlobj.h>
#include <cstring>
#include <iostream>

namespace tau {

class Fileman_win: public Fileman_impl {
public:

    Fileman_win(Fileman_mode fm_mode, const ustring & path=ustring()):
        Fileman_impl(fm_mode, path)
    {
    }

protected:

    void fill_places() override {
        if (places_list_) {
            places_list_->clear();
            wchar_t ws[1024];

            if (S_OK == SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, ws)) {
                ustring path = str_from_wstring(std::wstring(ws));
                Text_ptr tp = std::make_shared<Text_impl>("My Documents", ALIGN_START);
                tp->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), path));
                tp->set_tooltip(path);
                int row = places_list_->append_row(tp, true);

                Icon_ptr ico = std::make_shared<Icon_impl>(ICON_FOLDER, SMALL_ICON);
                ico->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), path));
                places_list_->insert(row, ico, -1, true);
            }

            std::list<ustring> removables, cdroms, remotes, drives;
            DWORD nbytes = GetLogicalDriveStringsA(0, NULL);

            if (0 != nbytes) {
                Locale loc;
                char buf[1+nbytes];
                GetLogicalDriveStringsA(1+nbytes, buf);

                for (std::size_t i = 0; i < nbytes; ++i) {
                    if ('\0' != buf[i]) {
                        UINT dtype = GetDriveType(buf+i);

                        if (DRIVE_UNKNOWN != dtype && DRIVE_NO_ROOT_DIR != dtype) {
                            ustring name = loc.iocharset().encode(buf+i);
                            if (DRIVE_REMOVABLE == dtype) { removables.push_back(name); }
                            else if (DRIVE_CDROM == dtype) { cdroms.push_back(name); }
                            else if (DRIVE_REMOTE == dtype) { remotes.push_back(name); }
                            else { drives.push_back(name); }
                        }
                    }

                    while ('\0' != buf[i]) { ++i; }
                }
            }

            if (!drives.empty()) {
                Text_ptr tp = std::make_shared<Text_impl>("Drives");
                tp->hint_margin_top(5);
                places_list_->append(tp);

                for (const ustring & s: drives) {
                    tp = std::make_shared<Text_impl>(s, ALIGN_START);
                    tp->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    int row = places_list_->append_row(tp, true);

                    Icon_ptr ico = std::make_shared<Icon_impl>(ICON_DRIVE_HARDDISK, SMALL_ICON);
                    ico->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    places_list_->insert(row, ico, -1, true);
                }
            }

            if (!cdroms.empty()) {
                Text_ptr tp = std::make_shared<Text_impl>("CDROM Drives");
                tp->hint_margin_top(5);
                places_list_->append(tp);

                for (const ustring & s: cdroms) {
                    tp = std::make_shared<Text_impl>(s, ALIGN_START);
                    tp->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    int row = places_list_->append_row(tp, true);

                    Icon_ptr ico = std::make_shared<Icon_impl>(ICON_DRIVE_OPTICAL, SMALL_ICON);
                    ico->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    places_list_->insert(row, ico, -1, true);
                }
            }

            if (!remotes.empty()) {
                Text_ptr tp = std::make_shared<Text_impl>("Remote Drives");
                tp->hint_margin_top(5);
                places_list_->append(tp);

                for (const ustring & s: remotes) {
                    tp = std::make_shared<Text_impl>(s, ALIGN_START);
                    tp->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    int row = places_list_->append_row(tp, true);

                    Icon_ptr ico = std::make_shared<Icon_impl>(ustring(ICON_FOLDER_REMOTE)+";"+ICON_FOLDER, SMALL_ICON);
                    ico->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    places_list_->insert(row, ico, -1, true);
                }
            }

            if (!removables.empty()) {
                Text_ptr tp = std::make_shared<Text_impl>("Removable Drives");
                tp->hint_margin_top(5);
                places_list_->append(tp);

                for (const ustring & s: removables) {
                    tp = std::make_shared<Text_impl>(s, ALIGN_START);
                    tp->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    int row = places_list_->append_row(tp, true);

                    Icon_ptr ico = std::make_shared<Icon_impl>(ustring(ICON_DRIVE_REMOVABLE_MEDIA)+":"+ICON_DRIVE_HARDDISK, SMALL_ICON);
                    ico->signal_select().connect(tau::bind(fun(this, &Fileman_impl::set_uri), s));
                    places_list_->insert(row, ico, -1, true);
                }
            }
        }
    }
};

// static
Fileman_ptr Fileman_impl::create(Fileman_mode fm_mode, const ustring & path) {
    return std::make_shared<Fileman_win>(fm_mode, path);
}

} // namespace tau

//END
