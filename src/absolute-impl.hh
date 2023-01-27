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

#ifndef TAU_ABSOLUTE_IMPL_HH
#define TAU_ABSOLUTE_IMPL_HH

#include <container-impl.hh>

namespace tau {

class Absolute_impl: public Container_impl {
public:

    Absolute_impl();
   ~Absolute_impl() { signal_destroy_(); }

   void put(Widget_ptr wp, const Point & pos, const Size & size=Size());
   void put(Widget_ptr wp, const Point & pos, unsigned width, unsigned height) { put(wp, pos, Size(width, height)); }
   void put(Widget_ptr wp, int x, int y, const Size & size=Size()) { put(wp, Point(x, y), size); }
   void put(Widget_ptr wp, int x, int y, unsigned width, unsigned height) { put(wp, Point(x, y), Size(width, height)); }

   void move(Widget_impl * wi, const Point & move_to);
   void move(Widget_impl * wi, int x, int y) { move(wi, Point(x, y)); }

   void move_rel(Widget_impl * wi, const Point & dpt);
   void move_rel(Widget_impl * wi, int dx, int dy) { move_rel(wi, Point(dx, dy)); }

   void resize(Widget_impl * wi, const Size & sz);
   void resize(Widget_impl * wi, unsigned width, unsigned height) { resize(wi, Size(width, height)); }

   void remove(Widget_impl * wi);
   void clear();
   bool empty() const;

private:

    struct Holder {
        Widget_ptr      wp;
        Point           pos;
        Size            size;
        connection      hints_cx;
        connection      req_cx;
        connection      hide_cx;
        connection      show_cx;
    };

    using Holders = std::list<Holder>;
    Holders             holders_;

private:

    void arrange();
    void update_requisition();
    void rm_child(Holder & hol);
    Size child_requisition(const Holder & hol);

    void on_child_requisition(Widget_impl * wi);
    void on_child_hide(Widget_impl * wi);
    void on_child_show(Widget_impl * wi);
    bool on_take_focus();
};

} // namespace tau

#endif // TAU_ABSOLUTE_IMPL_HH
