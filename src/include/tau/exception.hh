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

/// @file exception.hh exception class and its descendants.

#ifndef TAU_EXCEPTION_HH
#define TAU_EXCEPTION_HH

#include <tau/encoding.hh>
#include <tau/string.hh>

namespace tau {

/// Base exception class.
/// @ingroup throw_group
class exception {
public:

    virtual ~exception() throw();
    virtual ustring what() const = 0;
};

/// Internal error exception.
/// @ingroup throw_group
class internal_error: public exception {
    ustring msg_;

public:

    explicit internal_error(const ustring & msg);
    ustring what() const override;
};

/// Library user error.
/// @ingroup throw_group
struct user_error: public internal_error {
    user_error(const ustring & msg);
};

/// Operating system error.
/// @ingroup throw_group
class sys_error: public exception {
    int     gerror_;    // Generic error code (from GetLastError() or errno).
    ustring msg_;

public:

    explicit sys_error(const ustring & extra_msg=ustring());
    ustring what() const override;
    int gerror() const;
};

/// Graphics system error.
/// @ingroup throw_group
struct graphics_error: public internal_error {
    explicit graphics_error(const ustring & msg);
};

/// Bad document.
/// @ingroup throw_group
struct bad_doc: public internal_error {
    explicit bad_doc(const ustring & msg);
};

/// Bad font.
/// @ingroup throw_group
struct bad_font: public internal_error {
    explicit bad_font(const ustring & msg);
};

/// Bad image.
/// @ingroup throw_group
struct bad_pixmap: public internal_error {
    explicit bad_pixmap(const ustring & path);
};

} // namespace tau

#endif // TAU_EXCEPTION_HH
