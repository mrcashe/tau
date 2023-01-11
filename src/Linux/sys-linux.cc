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
#include <tau/string.hh>
#include <unistd.h>
#include <fstream>
#include <map>

namespace tau {

ustring path_self() {
    char buffer[4096];
    *buffer = '\0';
    int len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    return -1 != len ? ustring(buffer, len) : ustring();
}

std::vector<ustring> list_removable_drives() {
    std::vector<ustring> v;
    std::ifstream is("/proc/self/mounts");

    if (is.good()) {
        std::map<ustring, ustring> devs;
        char s[2048];

        while (!is.eof()) {
            is.getline(s, sizeof(s)-1);
            auto w = str_explode(s, ' ');

            if (w.size() > 1 && str_has_prefix(w[0], "/dev/")) {
                ustring dev = path_notdir(w[0]);
                while (std::isdigit(*dev.rbegin())) { dev.erase(dev.size()-1); }
                if (devs.end() == devs.find(dev)) { devs[dev] = w[1]; }
            }
        }

        for (auto & pair: devs) {
            if ("/" != pair.first) {
                std::ifstream iss(path_build("/sys/block", pair.first, "removable"));

                if (iss.good()) {
                    char t[16];
                    iss.getline(t, 15);
                    if (0 != std::atoi(t)) { v.push_back(pair.second); }
                }
            }
        }
    }

    return v;
}

} // namespace tau

//END
