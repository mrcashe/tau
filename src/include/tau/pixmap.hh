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

/// @file pixmap.hh The Pixmap class and standard pixmap names.

#ifndef TAU_PIXMAP_HH
#define TAU_PIXMAP_HH

#include <tau/types.hh>
#include <tau/geometry.hh>
#include <tau/signal.hh>

namespace tau {

/// Object containing image data.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup pixmap_group
/// See also @ref icon_sect
class Pixmap {
public:

    /// Default constructor creates an empty pixmap.
    Pixmap();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Pixmap(const Pixmap & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Pixmap & operator=(const Pixmap & other) = default;

    /// Create pixmap with specified depth and size.
    Pixmap(int depth, const Size & size=Size());

    /// Create pixmap with specified depth and size.
    Pixmap(int depth, unsigned width, unsigned height);

    /// Test if pure.
    operator bool() const;

    /// Reset underlying implementation.
    void reset();

    /// Get size (width and height) in pixels.
    Size size() const;

    /// Get color depth.
    /// Only four values are possible:
    /// - 1 for monochrome (A1 color model).
    /// - 8 for grayscale (A8 color model).
    /// - 24 for True Color (RGB24 color model).
    /// - 32 for Full Color (ARGB32 color model).
    int depth() const;

    /// Get size in bytes.
    std::size_t bytes() const;

    /// Get raw data.
    const uint8_t * raw() const;

    /// Get pixels per inch.
    Vector ppi() const;

    /// Set pixels per inch.
    void set_ppi(const Vector & value);

    /// Resize the pixmap.
    void resize(unsigned width, unsigned height);

    /// Resize the pixmap.
    void resize(const Size & sz);

    /// Assign pixel color.
    void put_pixel(int x, int y, const Color & c);

    /// Assign pixel color.
    void put_pixel(const Point & pt, const Color & c);

    /// Get pixel color at the specified location.
    Color get_pixel(int x, int y) const;

    /// Get pixel color at the specified location.
    Color get_pixel(const Point & pt) const;

    /// Fill from the memory.
    /// @param x the initial x coordinate.
    /// @param y the initial y coordinate.
    /// @param buffer the source bytes.
    /// @param nbytes the byte count in the source buffer.
    ///
    /// The data must be organized at 4 bytes per pixel.
    /// The least significant byte contains the blue channel,
    /// followed by the green, red and alpha channel.
    void set_argb32(int x, int y, const uint8_t * buffer, std::size_t nbytes);

    /// Fill from the memory.
    /// @param pt the initial coordinates.
    /// @param buffer the source bytes.
    /// @param nbytes the byte count in the source buffer.
    ///
    /// The data must be organized at 4 bytes per pixel.
    /// The least significant byte contains the blue channel,
    /// followed by the green, red and alpha channel.
    void set_argb32(const Point & pt, const uint8_t * buffer, std::size_t nbytes);

    /// Obtain a Painter.
    Painter painter();

    /// Test if empty.
    /// @return true, if pixmap has zero length data.
    bool empty() const;

    /// Load content from other pixmap.
    void copy(const Pixmap other);

    /// Duplicate pixmap.
    Pixmap dup() const;

    /// Load content from a file.
    static Pixmap load_from_file(const ustring & path);

    /// Signal emitted when some changes applied to pixmap.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_pixmap_changed();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_changed();

private:

    Pixmap(Pixmap_ptr pp);
    Pixmap_ptr impl;
    friend class Pixmap_impl;
};

} // namespace tau

#endif // TAU_PIXMAP_HH
