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

#include <tau/image.hh>
#include <tau/exception.hh>
#include <tau/string.hh>
#include <image-impl.hh>
#include <pixmap-impl.hh>

namespace tau {

#define IMAGE_IMPL (std::static_pointer_cast<Image_impl>(impl))

Image::Image():
    Widget(std::make_shared<Image_impl>())
{
}

Image::Image(Widget_ptr wp):
    Widget(std::dynamic_pointer_cast<Image_impl>(wp))
{
}

Image & Image::operator=(Widget_ptr wp) {
    if (!std::dynamic_pointer_cast<Image_impl>(wp)) {
        throw user_error(str_format(this, " Image::operator=(Widget_ptr): got pure or incompatible implementation pointer"));
    }

    impl = wp;
    return *this;
}

Image::Image(Pixmap pix, bool transparent):
    Widget(std::make_shared<Image_impl>(Pixmap_impl::strip(pix), transparent))
{
}

Image::Image(const ustring & pixmap_name, bool transparent):
    Widget(std::make_shared<Image_impl>(pixmap_name, transparent))
{
}

void Image::set_pixmap(Pixmap pix, bool transparent) {
    IMAGE_IMPL->set_pixmap(Pixmap_impl::strip(pix), transparent);
}

void Image::add_pixmap(Pixmap pix, unsigned ms) {
    IMAGE_IMPL->add_pixmap(Pixmap_impl::strip(pix), ms);
}

void Image::set_delay(unsigned ms) {
    IMAGE_IMPL->set_delay(ms);
}

unsigned Image::delay() const {
    return IMAGE_IMPL->delay();
}

void Image::set_oper(Oper op) {
    IMAGE_IMPL->set_oper(op);
}

Oper Image::oper() const {
    return IMAGE_IMPL->oper();
}

void Image::set_transparent() {
    IMAGE_IMPL->set_transparent();
}

void Image::unset_transparent() {
    IMAGE_IMPL->unset_transparent();
}

bool Image::transparent() const {
    return IMAGE_IMPL->transparent();
}

} // namespace tau

//END
