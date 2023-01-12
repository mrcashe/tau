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

#ifndef TAU_POPUP_IMPL_HH
#define TAU_POPUP_IMPL_HH

#include <window-impl.hh>

namespace tau {

class Popup_impl: public Window_impl {
    Point   upos_;
    Gravity gravity_ = GRAVITY_TOP_LEFT;

protected:

    Popup_impl(Winface_ptr winface, const Point & upos, Window_ptr wpp, Gravity gravity);

public:

    // Overrides Widget_impl.
    Window_impl * root() override;

    // Overrides Widget_impl.
    const Window_impl * root() const override;

    // Overrides Widget_impl.
    // Overrides Window_impl.
    Point to_root(const Point & pt=Point()) const override;

    // Overrides Widget_impl.
    void set_parent(Container_impl * parent) override;

    void set_gravity(Gravity gravity);
    Gravity gravity() const { return gravity_; }

private:

    void adjust();
    void on_show();
};

} // namespace tau

#endif // TAU_POPUP_IMPL_HH
