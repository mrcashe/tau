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

#include <tau/sys.hh>
#include <display-impl.hh>
#include <icon-impl.hh>
#include <list-impl.hh>
#include <navigator-impl.hh>
#include <fileman-impl.hh>
#include <text-impl.hh>
#include <cstring>
#include <fstream>

namespace tau {

class Fileman_freebsd: public Fileman_impl {
public:

    Fileman_freebsd(Fileman_mode fs_type):
        Fileman_impl(fs_type)
    {
    }

    Fileman_freebsd(Fileman_mode fs_type, const ustring & path):
        Fileman_impl(fs_type, path)
    {
    }

private:

    std::vector<ustring> removables_;
    int                  removables_row_ = -1;

protected:

    void fill_places() override {
        if (places_list_) {
            places_list_->clear_list();

            Text_ptr tp = std::make_shared<Text_impl>("Root Folder", ALIGN_START, ALIGN_CENTER);
            tp->signal_select().connect(tau::bind(fun(signal_places_chdir_), "/"));
            int br = places_list_->append_row(tp, true);

            Icon_ptr ico = std::make_shared<Icon_impl>("drive-harddisk", SMALL_ICON);
            ico->signal_select().connect(tau::bind(fun(signal_places_chdir_), "/"));
            places_list_->insert(br, ico, -1, true);

            tp = std::make_shared<Text_impl>("Home Folder", ALIGN_START, ALIGN_CENTER);
            tp->signal_select().connect(tau::bind(fun(signal_places_chdir_), path_user_home_dir()));
            br = places_list_->append_row(tp, true);

            ico = std::make_shared<Icon_impl>("go-home:folder", SMALL_ICON);
            ico->signal_select().connect(tau::bind(fun(signal_places_chdir_), path_user_home_dir()));
            places_list_->insert(br, ico, -1, true);

            removables_ = list_removable_drives();

            if (!removables_.empty()) {
                tp = std::make_shared<Text_impl>("Removable Drives");
                tp->hint_margin_top(5);
                removables_row_ = places_list_->append(tp);

                for (const ustring & mpoint: removables_) {
                    tp = std::make_shared<Text_impl>(path_notdir(mpoint));
                    tp->signal_select().connect(tau::bind(fun(signal_places_chdir_), mpoint));
                    br = places_list_->append_row(tp, true);

                    ico = std::make_shared<Icon_impl>("drive-removable-media:drive-harddisk", SMALL_ICON);
                    ico->signal_select().connect(tau::bind(fun(signal_places_chdir_), mpoint));
                    places_list_->insert(br, ico, -1, true);
                }
            }
        }
    }
};

// static
Fileman_ptr Fileman_impl::create(Fileman_mode fs_type) {
    return std::make_shared<Fileman_freebsd>(fs_type);
}

// static
Fileman_ptr Fileman_impl::create(Fileman_mode fs_type, const ustring & path) {
    return std::make_shared<Fileman_freebsd>(fs_type, path);
}

} // namespace tau

//END
