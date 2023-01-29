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

/// @file cursor.hh The Cursor class.

#ifndef TAU_CURSOR_HH
#define TAU_CURSOR_HH

#include <tau/geometry.hh>
#include <tau/types.hh>
#include <vector>

namespace tau {

/// A mouse cursor.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup input_group
class Cursor {
public:

    /// @name Constructors
    /// @{

    /// Creates a pure (without implementation) cursor.
    Cursor();

    /// Create cursor from given pixmap and hot spot position.
    /// @param pix the pixmap.
    /// @param hotspot the hot spot position within pixmap.
    Cursor(Pixmap pix, const Point & hotspot=Point());

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Cursor(const Cursor & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Cursor & operator=(const Cursor & other) = default;

    /// @}
    /// Test if pure.
    operator bool() const;

    /// Reset underlying implementation (make pure).
    void reset();

    /// Create cursor from file with specified cursor size in pixels.
    static Cursor load_from_file(const ustring & path, unsigned size_px);

    /// Lists available cursor sizes within specified file.
    static std::vector<unsigned> list_sizes(const ustring & path);

    /// Gets cursor size in pixels.
    unsigned size() const;

    /// Assign cursor image to single frame.
    /// @param pix the pixmap.
    /// @param hotspot the hot spot position within pixmap.
    void assign(Pixmap pix, const Point & hotspot=Point());

    /// Adds a frame at the end.
    /// @param pix the pixmap.
    /// @param delay_ms delay for this frame in milliseconds.
    /// @param hotspot the hot spot position within pixmap.
    /// @return the frame index.
    int append(Pixmap pix, unsigned delay_ms=0, const Point & hotspot=Point());

    /// Adds a frame at the begin.
    /// @param pix the pixmap.
    /// @param delay_ms delay for this frame in milliseconds.
    /// @param hotspot the hot spot position within pixmap.
    void prepend(Pixmap pix, unsigned delay_ms=0, const Point & hotspot=Point());

    /// Inserts a frame.
    /// @param nth_frame the frame index to be assigned for new frame.
    /// @param pix the pixmap.
    /// @param delay_ms delay for this frame in milliseconds.
    /// @param hotspot the hot spot position within pixmap.
    /// @return the frame index.
    /// @note if specified frame index does not exist, the new frame will be appended.
    int insert(int nth_frame, Pixmap pix, unsigned delay_ms=0, const Point & hotspot=Point());

    /// Gets frame count.
    int frame_count() const;

    /// Gets pixmap for specified frame index.
    Pixmap pixmap(int nth_frame=0) const;

    /// Gets hot spot coordinates for specified frame index.
    Point hotspot(int nth_frame=0) const;

    /// Gets delay in milliseconds for specified frame index.
    unsigned delay(int nth_frame) const;

    /// Gets default delay in milliseconds.
    unsigned delay() const;

    /// Change pixmap for specified frame.
    /// @param pix the new pixmap.
    /// @param nth_frame the frame index.
    /// @note if specified frame does not exist, nothing will be done.
    void set_pixmap(Pixmap pix, int nth_frame);

    /// Change delay for specified frame.
    /// @param delay_ms the delay in milliseconds.
    /// @param nth_frame the frame index.
    /// @note if specified frame does not exist, nothing will be done.
    void set_delay(unsigned delay_ms, int nth_frame);

    /// Change hot spot position for specified frame.
    /// @param hotspot the new hot spot position.
    /// @param nth_frame the frame index.
    /// @note if specified frame does not exist, nothing will be done.
    void set_hotspot(const Point & hotspot, int nth_frame=0);

    /// Change default delay.
    /// @param delay_ms the delay in milliseconds.
    void set_delay(unsigned delay_ms);

private:

    Cursor(Cursor_ptr cp);
    Cursor_ptr impl;
    friend class Cursor_impl;
};

} // namespace tau

#endif // TAU_CURSOR_HH
