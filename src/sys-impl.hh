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

#if (defined(__GNUC__) && (__GNUC__ >= 11)) || (defined(__clang__) && (__clang_major__ >= 15))

template <class T>
struct v_allocator {
    using value_type = T;

    std::vector<T>  v_;
    std::size_t     pos_ = 0;

    v_allocator() noexcept: v_(256) {}
    v_allocator(const v_allocator &) noexcept {}
    template <class U> v_allocator (const v_allocator<U> &) noexcept {}
    void deallocate (value_type *, std::size_t) {}

    value_type * allocate(std::size_t n) {
        if (!n) { return nullptr; }
        if (pos_+n > v_.size()) { v_.resize(pos_+(std::max(n, 256LU))); }
        value_type * res = v_.data()+pos_;
        pos_ += n;
        return res;
    }
};

template <class T, class U>
bool operator==(v_allocator<T> const &, v_allocator<U> const &) noexcept {
    return true;
}

template <class T, class U>
bool operator!=(v_allocator<T> const & x, v_allocator<U> const & y) noexcept {
    return !(x == y);
}

#define TAU_HAS_VALLOCATOR 1
#else
#define TAU_HAS_VALLOCATOR 0
#endif

}

#endif // TAU_SYS_IMPL_HH
