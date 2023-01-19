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

/// @file toplevel.hh The Toplevel class.

#ifndef TAU_TOPLEVEL_HH
#define TAU_TOPLEVEL_HH

#include <tau/enums.hh>
#include <tau/window.hh>

namespace tau {

/// %Toplevel window.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup window_group
class Toplevel: public Window {
public:

    /// @name Constructor and operators
    /// @{

    /// Default constructor.
    Toplevel(const Rect & bounds=Rect());

    /// Constructor with title (caption).
    Toplevel(const ustring & title, const Rect & bounds=Rect());

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Toplevel(const Toplevel & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Toplevel & operator=(const Toplevel & other) = default;

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
    Toplevel(Widget_ptr wp);

    /// @}
    /// Sets window title (caption).
    void set_title(const ustring & title);

    /// Sets window icon.
    void set_icon(Pixmap pix);

    /// Sets window icon.
    void set_icon(const ustring & icon_name, int icon_size=MEDIUM_ICON);

    /// Sets window icon from file.
    void set_icon_from_file(const ustring & path);

    /// Close window.
    void close();

    /// Retrieves the current maximized state of window.
    bool maximized() const;

    /// Retrieves the current minimized state of window.
    bool minimized() const;

    /// Occupy the screen entirely.
    void fullscreen();

    /// Deoccupy the screen.
    void unfullscreen();

    /// Test if fullscreened.
    bool fullscreened() const;

    /// Maximize (zoom) window.
    void maximize();

    /// Minimize (iconify) window.
    void minimize();

    /// Restore from minimized or maximized state.
    void restore();

    /// Show window frame, including caption(title), minimize and maximize buttons.
    /// @see hide_frame
    /// @see frame_visible
    /// @note visible by default.
    void show_frame();

    /// Hide window frame, including caption(title), minimize and maximize buttons.
    /// @see show_frame
    /// @see frame_visible
    /// @note visible by default.
    void hide_frame();

    /// Test if window frame visible.
    /// @see show_frame
    /// @see hide_frame
    /// @note visible by default.
    bool frame_visible() const;

    /// At the moment, works differently on Windows and X11.
    /// On Windows, simply shows window maximize button.
    /// On X11, enables window maximizing.
    /// @see maximize
    /// @see maximized
    /// @see maximize_enabled
    /// @see disable_maximize
    /// @note enabled by default.
    void enable_maximize();

    /// At the moment, works differently on Windows and X11.
    /// On Windows, simply hides window maximize button.
    /// On X11, disables window maximizing at all.
    /// @see maximize
    /// @see maximized
    /// @see maximize_enabled
    /// @see enable_maximize
    /// @note enabled by default.
    void disable_maximize();

    /// At the moment, works differently on Windows and X11.
    /// On Windows, simply tests if window maximize button visible.
    /// On X11, tests if window maximizing enabled.
    /// @see maximize
    /// @see maximized
    /// @see enable_maximize
    /// @see disable_maximize
    /// @note enabled by default.
    bool maximize_enabled() const;

    /// At the moment, works differently on Windows and X11.
    /// On Windows, simply hides window minimize button.
    /// On X11, disables window minimizing at all.
    /// @see minimize
    /// @see minimized
    /// @see minimize_enabled
    /// @see enable_minimize
    /// @note enabled by default.
    void enable_minimize();

    /// At the moment, works differently on Windows and X11.
    /// On Windows, simply hides window minimize button.
    /// On X11, disables window minimizing at all.
    /// @see minimize
    /// @see minimized
    /// @see minimize_enabled
    /// @see enable_minimize
    /// @note enabled by default.
    void disable_minimize();

    /// At the moment, works differently on Windows and X11.
    /// On Windows, simply tests if window minimize button visible.
    /// On X11, tests if window minimizing enabled.
    /// @see minimize
    /// @see minimized
    /// @see enable_minimize
    /// @see disable_minimize
    /// @note enabled by default.
    bool minimize_enabled() const;

    /// Emits when user clicks [x] window button or presses Alt+F4 or similar.
    /// Return true to prevent window close, false to allow.
    signal<bool()> & signal_about_close();

    signal<void()> & signal_maximize();

    signal<void()> & signal_minimize();

    signal<void()> & signal_restore();

    signal<void(bool)> & signal_fullscreen();

    /// Emits when MENU key pressed, typically F10.
    /// @return true if signal handled.
    signal<bool()> & signal_menu();

    /// Emits when HELP key pressed, typically F1.
    /// @return true if signal handled.
    signal<bool()> & signal_help();

protected:

    /// @private
    Toplevel(std::nullptr_t);
};

} // namespace tau

#endif // TAU_TOPLEVEL_HH
