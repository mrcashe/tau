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

#include <tau/loop.hh>
#include "cursor-xcb.hh"
#include "display-xcb.hh"
#include "pixmap-xcb.hh"

namespace tau {

Cursor_xcb::Cursor_xcb():
    Cursor_impl(),
    trackable()
{
}

Cursor_xcb::Cursor_xcb(Display_xcb * dp, xcb_cursor_t cid):
    Cursor_impl(),
    trackable(),
    dp_(dp),
    cid_(cid)
{
    Loop().signal_quit().connect(fun(this, &Cursor_xcb::on_display_quit));
}

Cursor_xcb::~Cursor_xcb() {
    free_cursor();
}

void Cursor_xcb::free_cursor() {
    if (dp_) {
        if (XCB_NONE != cid_) {
            xcb_free_cursor(dp_->conn(), cid_);
            cid_ = XCB_NONE;
        }

        for (xcb_cursor_t cid: anim_) {
            xcb_free_cursor(dp_->conn(), cid);
        }

        anim_.clear();
        xcb_flush(dp_->conn());
    }
}

void Cursor_xcb::on_display_quit() {
    dp_ = nullptr;
}

xcb_cursor_t Cursor_xcb::create_xcursor(const Cur & cur, xcb_drawable_t drw) {
    if (dp_) {
        if (auto pix = std::dynamic_pointer_cast<Pixmap_xcb>(cur.pix)) {
            if (32 == pix->depth()) {
                xcb_pixmap_t pixmap = xcb_generate_id(dp_->conn());
                xcb_create_pixmap(dp_->conn(), 32, pixmap, drw, pix->size().width(), pix->size().height());
                xcb_render_picture_t picture = xcb_generate_id(dp_->conn());
                const uint32_t v[1] = { 0 };
                xcb_render_create_picture(dp_->conn(), picture, pixmap, dp_->pictformat(32), 1, v);
                xcb_gcontext_t gc = xcb_generate_id(dp_->conn());
                xcb_create_gc(dp_->conn(), gc, pixmap, 0, nullptr);
                xcb_put_image(dp_->conn(), XCB_IMAGE_FORMAT_Z_PIXMAP, pixmap, gc, pix->size().width(), pix->size().height(), 0, 0, 0, 32, pix->bytes(), pix->raw());
                xcb_cursor_t cid = xcb_generate_id(dp_->conn());
                xcb_render_create_cursor(dp_->conn(), cid, picture, cur.hotspot.x(), cur.hotspot.y());
                xcb_render_free_picture(dp_->conn(), picture);
                xcb_free_gc(dp_->conn(), gc);
                xcb_free_pixmap(dp_->conn(), pixmap);
                return cid;
            }
        }
    }

    return XCB_NONE;
}

xcb_cursor_t Cursor_xcb::upload(Display_xcb * dp, xcb_drawable_t drw) {
    if (!dp_ || dp_ == dp) {
        dp_ = dp;
        Loop().signal_quit().connect(fun(this, &Cursor_xcb::on_display_quit));

        if (!frames_.empty()) {
            if (1 == frames_.size()) {
                auto & cur = frames_[0];
                cid_ = create_xcursor(cur, drw);
            }

            else {
                xcb_render_animcursorelt_t elt[frames_.size()];

                for (std::size_t n = 0; n < frames_.size(); ++n) {
                    auto & cur = frames_[n];
                    elt[n].cursor = create_xcursor(cur, drw);
                    elt[n].delay = cur.delay;
                }

                cid_ = xcb_generate_id(dp_->conn());
                xcb_render_create_anim_cursor(dp_->conn(), cid_, frames_.size(), elt);
            }
        }
    }

    return cid_;
}

// Overrides Cursor_impl.
bool Cursor_xcb::has_sys_handle() const {
    return XCB_NONE != cid_;
}

// Overrides Cursor_impl.
void Cursor_xcb::sys_update() {
    //TODO Add actual stuff.
}

} // namespace tau

//END
