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

/// @file fontsel.hh The Fontsel class.

#ifndef TAU_FONTSEL_HH
#define TAU_FONTSEL_HH

#include <tau/ustring.hh>
#include <tau/widget.hh>

namespace tau {

/// %Font selection widget.
///
/// This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
class Fontsel: public Widget {
public:

    /// Default constructor.
    Fontsel();

    /// Constructor with font specification.
    /// @param spec the font specification, e.g. "Arial Bold 10".
    /// @param sample the sample text to be displayed. If empty, the default one will be used.
    Fontsel(const ustring & spec, const ustring & sample=ustring());

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Fontsel(const Fontsel & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Fontsel & operator=(const Fontsel & other) = default;

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
    Fontsel(Widget_ptr wp);

    /// Select specified font.
    void select(const ustring & spec);

    /// Get selected font.
    ustring spec() const;

    /// Set sample text.
    /// @param sample the sample text to be displayed. If empty, the default one will be used.
    void set_sample(const ustring & sample);

    /// Get sample text.
    ustring sample() const;

    /// Get focus_next action.
    Action & focus_next_action();

    /// Get focus_previous action.
    Action & focus_previous_action();

    /// Get "Cancel" action.
    Action & cancel_action();

    /// Signal "Font Selected".
    /// Emits when focus moved to the new font.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_font_selected(const ustring & spec);
    /// ~~~~~~~~~~~~~~~
    signal<void(ustring)> & signal_font_selected();

    /// Signal "Font Activated".
    /// Emits when font family of font face double clicked or font size changed or "Apply" button pressed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_font_activated(const ustring & spec);
    /// ~~~~~~~~~~~~~~~
    signal<void(ustring)> & signal_font_activated();
};

} // namespace tau

#endif // TAU_FONTSEL_HH
