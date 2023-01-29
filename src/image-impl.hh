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

#ifndef TAU_IMAGE_IMPL_HH
#define TAU_IMAGE_IMPL_HH

#include <tau/enums.hh>
#include <tau/pixmap.hh>
#include <widget-impl.hh>

namespace tau {

class Image_impl: public Widget_impl {
public:

    Image_impl();
    explicit Image_impl(Pixmap_ptr pix, bool transparent=false);
    explicit Image_impl(const ustring & pixmap_name, bool transparent=false);

    void add_pixmap(Pixmap_ptr pix, unsigned delay=0);
    void set_pixmap(Pixmap_ptr pix, bool transparent=false);
    void set_transparent();
    void unset_transparent();
    void set_delay(unsigned delay);
    void set_oper(Oper op);

    unsigned delay() const {
        return delay_;
    }

    bool transparent() const {
        return transparent_;
    }

    Oper oper() const {
        return oper_;
    }

protected:

    struct Film_frame {
        Pixmap_ptr      pix;
        unsigned        delay = 0;
        connection      changed_cx;
    };

    using Film = std::vector<Film_frame>;

    void init();
    void clear();
    void paint_pixmap(Painter pr);
    void redraw();
    void start_timer_if_needed();
    unsigned calc_delay() const;

    bool on_paint(Painter pr, const Rect & inval);
    void on_parent();
    void on_timer();
    void on_enable();
    void on_disable();
    void on_pix_changed(std::size_t index);

protected:

    Film        film_;
    Pixmap_ptr  gray_;
    Rect        irect_;
    std::size_t cur_ = 0;
    unsigned    delay_ = 0;
    Timer       timer_ { fun(this, &Image_impl::on_timer) };
    bool        transparent_ = false;
    ustring     pixmap_name_;
    Oper        oper_ = OPER_COPY;
};

} // namespace tau

#endif // TAU_IMAGE_IMPL_HH
