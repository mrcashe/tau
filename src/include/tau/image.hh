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

/// @file image.hh Image class.

#ifndef TAU_IMAGE_HH
#define TAU_IMAGE_HH

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// An image (picture).
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Image: public Widget {
public:

    /// @name Constructors and operators
    /// @{

    /// Default constructor.
    Image();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Image(const Image & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Image & operator=(const Image & other) = default;

    /// Constructor with implementation pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to construct the object
    /// from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Image(Widget_ptr wp);

    /// Constructor with pixmap.
    /// @param pixmap the pixmap.
    /// @param transparent set transparent or opaque mode.
    Image(Pixmap pixmap, bool transparent=false);

    /// Constructor with pixmap name.
    /// @param pixmap_name
    /// @param transparent set transparent or opaque mode.
    Image(const ustring & pixmap_name, bool transparent=false);

    /// @}
    /// Set pixmap.
    /// @param pixmap the pixmap.
    /// @param transparent set transparent or opaque mode.
    void set_pixmap(Pixmap pixmap, bool transparent=false);

    /// Add pixmap to animation.
    /// @param pixmap the pixmap.
    /// @param ms delay in milliseconds to show mentioned pixmap.
    /// @note This method does not provide transparent() flag setting,
    ///       so set it by calling one of the set_transparent()
    ///       or unset_transparent() methods.
    ///       The default value is not transparent (opaque).
    void add_pixmap(Pixmap pixmap, unsigned ms=0);

    /// Set transparent mode.
    /// The default value is not transparent (opaque).
    void set_transparent();

    /// Set opaque mode.
    /// The default value is not transparent (opaque).
    void unset_transparent();

    /// Test if transparent.
    /// The default value is not transparent (opaque).
    bool transparent() const;

    /// Set delay.
    /// @param ms delay in milliseconds to show mentioned pixmap.
    /// @sa delay().
    void set_delay(unsigned ms);

    /// Get delay in milliseconds.
    /// @return delay in milliseconds.
    unsigned delay() const;

    /// Set operator used to render pixmap.
    void set_oper(Oper op);

    /// Get operator used to render pixmap.
    Oper oper() const;
};

} // namespace tau

#endif // TAU_IMAGE_HH
