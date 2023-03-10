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

/// @file display.hh The Display class.

#ifndef TAU_DISPLAY_HH
#define TAU_DISPLAY_HH

#include <tau/object.hh>
#include <tau/ustring.hh>

namespace tau {

/// A display.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup sys_group
class Display: public Object {
public:

    /// Default constructor.
    Display();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Display(const Display & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Display & operator=(const Display & other) = default;

    /// Open display with optional arguments.
    static Display open(const ustring & args=ustring());

    /// Gets unique id.
    int id() const;

    /// Get size in pixels.
    Size size_px() const;

    /// Get size in millimeters.
    Size size_mm() const;

    /// Get dots per inch ratio.
    int dpi() const;

    /// Get number of bits per pixel.
    int depth() const;

    bool can_paste_text() const;

    void paste_text();

    void copy_text(const ustring & str);

    void allow_screensaver();
    void disallow_screensaver();
    bool screensaver_allowed() const;

    /// @name Accessors to established signals.
    /// @{

    signal<void()> & signal_can_paste();

    /// Signal emitted when text pasted from clipboard.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_paste_text(const ustring & path).
    /// ~~~~~~~~~~~~~~~
    signal<void(const ustring &)> & signal_paste_text();

    /// @}

private:

    Display_ptr impl;
    Display(Display_ptr dp);
    friend class Display_impl;
};

} // namespace tau

#endif // TAU_DISPLAY_HH
