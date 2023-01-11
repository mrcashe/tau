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

#ifndef TAU_CURSOR_XCB_HH
#define TAU_CURSOR_XCB_HH

#include "types-xcb.hh"
#include <tau/signal.hh>
#include <cursor-impl.hh>

namespace tau {

class Cursor_xcb: public Cursor_impl, public trackable {
public:

    Cursor_xcb();
    Cursor_xcb(Display_xcb * dp, xcb_cursor_t cid);
   ~Cursor_xcb();

    xcb_cursor_t xid() const { return cid_; }
    xcb_cursor_t upload(Display_xcb * dp, xcb_drawable_t drw);

protected:

    // Overrides Cursor_impl.
    bool has_sys_handle() const override;

    // Overrides Cursor_impl.
    void sys_update() override;

private:

    void on_display_quit();
    void free_cursor();
    xcb_cursor_t create_xcursor(const Cur & cur, xcb_drawable_t drw);

private:

    Display_xcb *       dp_ = nullptr;
    xcb_cursor_t        cid_ = XCB_NONE;
    std::vector<xcb_cursor_t> anim_;
};

} // namespace tau

#endif // TAU_CURSOR_XCB_HH
