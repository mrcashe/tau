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

#ifndef TAU_SYS_IMPL_HH
#define TAU_SYS_IMPL_HH

#include <tau/sys.hh>
#include <tau/sysinfo.hh>

namespace tau {

extern Sysinfo sysinfo_;

template <class T>
struct v_allocator {
    using value_type = T;

    static constexpr std::size_t delta_ = 256;
    std::vector<T>  v_;
    std::size_t     pos_ = 0;

    v_allocator(std::size_t size=0) noexcept { v_.resize(size ? size : delta_); }
    v_allocator(const v_allocator & other) noexcept {}
    template <class U> v_allocator (const v_allocator<U> &) noexcept {}
    void deallocate (T *, std::size_t) {}

    T * allocate (std::size_t n) {
        if (pos_ == v_.size()) { v_.resize(pos_+delta_); }
        return v_.data()+pos_++;
    }
};

}

#endif // TAU_SYS_IMPL_HH
