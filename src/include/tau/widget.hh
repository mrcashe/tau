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

/// @file widget.hh The Widget class.

#ifndef TAU_WIDGET_HH
#define TAU_WIDGET_HH

#include <tau/enums.hh>
#include <tau/geometry.hh>
#include <tau/object.hh>

namespace tau {

/// Base class of all user interface objects and a simplest widget.
///
/// The widget is the atom of the user interface: it receives mouse,
/// keyboard and other events from the window system, and paints a representation
/// of itself on the screen. Every widget is rectangular. A widget is clipped by
/// its parent.
///
/// A widget that is not embedded in a parent widget is called a window.
/// Usually, windows have a frame and a title bar, although it is also possible
/// to create windows without such decoration.
///
/// The Widget class built using so called "pImpl" paradigm: the opaque implementation
/// class Widget_impl is hidden behind Widget class. The smart pointer is used to
/// hold an implementation class.
///
/// Such a construction architecture leads to far-reaching consequences. When you
/// insert widget into Container, the smart pointer holding Widget_impl copied to
/// container's data, but container knows nothing about Widget itself. Thus,
/// the implementation class defines the functionality of the widget. You can only
/// change the behavior of the widget using the signal handlers that are emitted
/// by the widget.
///
/// Another issue is we can not obtain widget parent from %Widget itself.
/// This is a consequence of the fact that we do not store a pointer to a possibly
/// inherited object in the data structures of the library. However, this is a
/// small drawback with the proper construction of the program code - the nice
/// programmer must always know about where parent is.
///
/// The Widget class is fully functional. You can construct it, you can derive it.
/// Typical application of the Widget is drawing area. Any derived class can be used
/// as drawing area.
///
/// @ingroup widget_group
class Widget: public trackable {
public:

    /// @name Constructors, operators, shared pointers
    /// @{

    /// Default constructor.
    /// Creates basic widget commonly used for drawing.
    Widget();

    /// Destructor.
   ~Widget();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Widget(const Widget & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Widget & operator=(const Widget & other) = default;

    /// Constructor with widget implementation shared pointer.
    ///
    /// @warning Unlike some other classes (Painter as an example), the whole
    /// @ref widget_group "widget stack" is unable to run with pure implementation
    /// pointer, so attempting to construct widget from a pure (@b nullptr) pointer
    /// will cause throwing an user_error exception!
    /// That exception also will be thrown if user tries to
    /// construct derived class from incompatible implementation shared pointer.
    ///
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Widget(Widget_ptr wp);

    /// Gets underlying shared pointer to implementation.
    Widget_ptr ptr();

    /// Gets underlying shared pointer to implementation.
    Widget_cptr ptr() const;

    /// Assign implementation.
    ///
    /// @since 0.4.0
    /// @throw user_error in case of pure implementation pointer or incompatible
    /// implementation pointer class.
    Widget & operator=(Widget_ptr wp);

    /// @}
    /// @name Size, position and coordinates
    /// Methods related to widget size, position and coordinates control.
    ///
    /// The library limits the user to the placement of widgets.
    /// The position and size of the widget are completely controlled by the parent container.
    /// Most containers allocate space automatically. And only the Absolute container allows
    /// you to freely manipulate the position and size of your widgets.
    ///
    /// %Widget can inform its parent container about preferred widget size by using set of
    /// <em>hint-methods</em>. For example, the Image widget changes its hints when the pixmap under it
    /// grows up or down. The parent container may or may not take in account that hints. For example,
    /// the Bin container always allocates all available space to its single child
    /// regardless of its size hints. In contrast, the Box always using size hints.
    /// Changing size hints may cause the parent to recalculate available space.
    ///
    /// The Window does not have its own container, so some hints ineffective for windows.
    /// @{

    /// Get widget size in pixels.
    /// Remember, you can not assign an arbitrary size to the widget. The only way to do so
    /// is to put your widget inside of an Absolute container.
    /// In case widget is a Window, the returning value is a size of window client area,
    /// excluding its caption and a frame.
    /// @return size allocated by parent container or by user for windows.
    Size size() const;

    /// Get widget origin within its parent container.
    /// Remember, you can not assign an arbitrary origin to the widget. The only way to do so
    /// is to put your widget inside of an Absolute container.
    /// In case widget is a Window, the returning value is a offset of window client area
    /// from the screen top left corner.
    /// @return an origin within the parent or within the screen for windows.
    Point origin() const;

    /// Get size requisition.
    /// Returns size requisition, previously set by require_size() methods.
    /// @return size requisition.
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa signal_hints_changed()
    /// @sa signal_requisistion_changed()
    Size required_size() const;

protected:

    /// Set size requisition.
    /// Tells parent container about widget size requisition.
    /// @note This call might emit signal_requisition_changed().
    /// @param size required size, values of zero mean "don't care".
    /// @return @b true if changed.
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa required_size()
    /// @sa signal_requisistion_changed()
    bool require_size(const Size & size);

    /// Set size requisition.
    /// Tells parent container about widget size requisition.
    /// @note This call might emit signal_requisition_changed().
    /// @param width required width, value of zero mean "don't care".
    /// @param height required width, value of zero mean "don't care".
    /// @return @b true if changed.
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa required_size()
    /// @sa signal_requisistion_changed()
    bool require_size(unsigned width, unsigned height);

public:

    /// Signal is emitted when widget owner (parent) changes widget coordinates.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_origin_changed();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_origin_changed();

    /// Signal is emitted after widget's size had changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_size_changed();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_size_changed();

    /// Signal emitted when requisition changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_requisition_changed();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_requisition_changed();

    /// Translate coordinates to screen coordinate system.
    ///
    /// @return point translated to screen coordinate system.
    Point to_screen(const Point & pt=Point()) const;

    /// Translate coordinates to root window coordinate system.
    ///
    /// The root window is a window that have not parent.
    /// All child windows have that window as parent window.
    /// @return point translated to root window coordinate system.
    Point to_root(const Point & pt=Point()) const;

    /// Translate coordinates to owning window coordinate system.
    ///
    /// @return point translated to owning window coordinate system.
    Point to_window(const Point & pt=Point()) const;

    /// Translate coordinates to immediate parent container coordinate system.
    ///
    /// @return point translated to parent container coordinate system.
    Point to_parent(const Point & pt=Point()) const;

    /// Translate coordinates to known parent container coordinate system.
    ///
    /// @return point translated to known container coordinate system.
    Point to_parent(const Container & known_cont, const Point & pt=Point()) const;

    /// @}
    /// @name Hints
    ///
    /// Widget can have different hints, such as:
    /// - Minimal size hint.
    /// - Maximal size hint.
    /// - Exact size hint.
    /// - Margin hint.
    ///
    /// Each widget has HTML-like margins surrounded it. By default, all margins are
    /// set to zero width (no margins). If widget has assigned margins, that margins
    /// are reserved by owning Container and belongs to that container.
    ///
    /// Most of containers are take in account widget margins, but some of them are not.
    /// As an example, see Scroller and Absolute. Some compound containers does not
    /// support margins.
    ///
    /// Changing hints emits signal_hints_changed().
    ///
    /// @{

    /// Set exact size hint.
    /// Tells parent container about preferred widget size.
    ///
    /// @note This call might emit signal_hints_changed().
    /// @param size preferred size, values of zero mean "don't care".
    /// @return @b true if hint changed.
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa size_hint()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    bool hint_size(const Size & size);

    /// Set exact size hint.
    /// Tells parent container about preferred widget size.
    /// @note This call might emit signal_hints_changed().
    /// @param width preferred width, value of zero mean "don't care".
    /// @param height preferred width, value of zero mean "don't care".
    /// @return @b true if hint changed.
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa size_hint()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    bool hint_size(unsigned width, unsigned height);

    /// Get size hint.
    /// Returns size hint previously set by hint_size().
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    Size size_hint() const;

    /// Set minimal size hint.
    /// Tells parent container about minimal widget size.
    /// @note This call might emit signal_hints_changed().
    /// @param size preferred minimal size in pixels, values of zero mean "don't care".
    /// @return @b true if hint changed.
    /// @sa hint_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    bool hint_min_size(const Size & size);

    /// Set minimal size hint.
    /// Tells parent container about minimal widget size.
    /// @note This call might emit signal_hints_changed().
    /// @param width preferred width, value of zero mean "don't care".
    /// @param height preferred width, value of zero mean "don't care".
    /// @return @b true if hint changed.
    /// @sa hint_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    bool hint_min_size(unsigned width, unsigned height);

    /// Get minimal size hint previously set by hint_min_size().
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    Size min_size_hint() const;

    /// Set maximal size hint.
    /// Tells parent container about maximal widget size.
    /// @note This call might emit signal_hints_changed().
    /// @param size required maximal size in pixels, values of zero mean "don't care".
    /// @return @b true if hint changed.
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    bool hint_max_size(const Size & size);

    /// Set maximal size hint.
    /// Tells parent container about maximal widget size.
    /// @note This call might emit signal_hints_changed().
    /// @param width preferred maximal width, value of zero mean "don't care".
    /// @param height required maximal width, value of zero mean "don't care".
    /// @return @b true if hint changed.
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_size()
    /// @sa min_size_hint()
    /// @sa max_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    bool hint_max_size(unsigned width, unsigned height);

    /// Get maximal size hint previously set by hint_max_size().
    /// @sa hint_size()
    /// @sa hint_min_size()
    /// @sa hint_max_size()
    /// @sa min_size_hint()
    /// @sa require_size()
    /// @sa required_size()
    /// @sa signal_hints_changed()
    Size max_size_hint() const;

    /// Set left margin.
    /// @return @b true if hint changed.
    /// @note This call might emit signal_hints_changed().
    bool hint_margin_left(unsigned left);

    /// Set right margin.
    /// @return @b true if hint changed.
    /// @note This call might emit signal_hints_changed().
    bool hint_margin_right(unsigned right);

    /// Set top margin.
    /// @return @b true if hint changed.
    /// @note This call might emit signal_hints_changed().
    bool hint_margin_top(unsigned top);

    /// Set bottom margin.
    /// @return @b true if hint changed.
    /// @note This call might emit signal_hints_changed().
    bool hint_margin_bottom(unsigned bottom);

    /// Set all margins to the same value.
    /// @return @b true if hint changed.
    /// @note This call might emit signal_hints_changed().
    bool hint_margin(unsigned all);

    /// Set all margins.
    /// @return @b true if hint changed.
    /// @note This call might emit signal_hints_changed().
    bool hint_margin(unsigned left, unsigned right, unsigned top, unsigned bottom);

    /// Get left margin hint.
    unsigned margin_left_hint() const;

    /// Get right margin hint.
    unsigned margin_right_hint() const;

    /// Get top margin hint.
    unsigned margin_top_hint() const;

    /// Get bottom margin hint.
    unsigned margin_bottom_hint() const;

    /// Get margin hints.
    /// @return width = margin_left_hint()+margin_right_hint(), height = margin_top_hint()+margin_bootom_hint().
    Size margin_hint() const;

    /// Get margin origin.
    /// @return x = margin_left_hint(), y = margin_top_hint().
    Point margin_origin() const;

    /// Signal emitted when hints changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_hints_changed();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_hints_changed();

    /// @}
    /// @name Scrolling (Panning)
    ///
    /// Each widget has builtin scrolling capabilities. However, that capabilities
    /// might be realized only when the widget inserted into special container
    /// named Scroller. If widget does not has Scroller as an immediate parent, the
    /// scrolling will not work.
    ///
    /// @{

    /// Get current scroll position.
    /// @sa scroll_to() scroll_to_x() scroll_to_y() signal_scroll_changed() Scroller
    Point scroll_position() const;

    /// Ask immediate parent to change scroll position.
    ///
    /// @note This call takes effect only when widget inserted into Scroller container.
    ///
    /// @sa scroll_position() signal_scroll_changed() Scroller
    void scroll_to(const Point & pt);

    /// Ask immediate parent to change scroll position.
    ///
    /// @note This call takes effect only when widget inserted into Scroller container.
    ///
    /// @sa scroll_position() signal_scroll_changed() Scroller
    void scroll_to(int x, int y);

    /// Ask immediate parent to change scroll position.
    ///
    /// @note This call takes effect only when widget inserted into Scroller container.
    ///
    /// @sa scroll_position() signal_scroll_changed() Scroller
    void scroll_to_x(int x);

    /// Ask immediate parent to change scroll position.
    ///
    /// @note This call takes effect only when widget inserted into Scroller container.
    ///
    /// @sa scroll_position() signal_scroll_changed() Scroller
    void scroll_to_y(int y);

    /// Signal is emitted after widget's scroll position had changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_scroll_changed();
    /// ~~~~~~~~~~~~~~~
    ///
    /// @sa scroll_to() scroll_to_x() scroll_to_y() Scroller::pan_to() Scroller::signal_pan_changed()
    signal<void()> & signal_scroll_changed();

    /// Test if widget inserted into Scroller.
    bool scrollable() const;

    /// @}
    /// @name Visibility
    /// @{

    /// Hide widget.
    /// From the construction of widget, all widgets (except Window and its descendants) are visible.
    /// The Window is hidden from the construction and needs to call show() to be exposed.
    ///
    /// The widget can be hidden in two cases:
    /// - when hide() method called from outside of widget (by the user)
    /// - when disappear() method called from inside of widget realization.
    /// After one of these methods called, the owning container deallocates space occupied by widget.
    ///
    /// signal_hide() emitted by this method if widget was not hidden before method call.
    /// signal_invisible() emitted by this method if widget was visible before method call.
    ///
    /// @sa hide()
    /// @sa disappear()
    /// @sa appear()
    /// @sa hidden()
    /// @sa visible()
    /// @sa signal_show()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    void hide();

    /// Show widget.
    /// Along with appear(), shows the widget.
    ///
    /// signal_show() emitted by this method if widget was hidden before method call.
    /// signal_visible() emitted by this method if widget was not visible before method call.
    ///
    /// @sa hide()
    /// @sa disappear()
    /// @sa appear()
    /// @sa hidden()
    /// @sa visible()
    /// @sa signal_show()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    void show();

protected:

    /// Hide widget.
    /// Along with hide(), hides the widget.
    ///
    /// In contrast to hide(), this method is protected and intended to internal widget realization.
    /// For example, the Menu_item uses this method to hide itself when menu item becomes invisible.
    ///
    /// signal_hide() emitted by this method if widget was not hidden before method call.
    /// signal_invisible() emitted by this method if widget was visible before method call.
    ///
    /// @sa show()
    /// @sa hide()
    /// @sa hidden()
    /// @sa visible()
    /// @sa appear()
    /// @sa signal_show()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    void disappear();

    /// Show widget.
    /// Along with show(), shows the widget.
    ///
    /// In contrast to show(), this method is protected and intended to internal widget realization.
    /// For example, the Menu_item uses this method to show itself when menu item becomes visible.
    ///
    /// signal_show() emitted by this method if widget was hidden before method call.
    /// signal_visible() emitted by this method if widget was not visible before method call.
    ///
    /// @sa show()
    /// @sa hide()
    /// @sa hidden()
    /// @sa visible()
    /// @sa disappear()
    /// @sa signal_show()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    void appear();

public:

    /// Determines if widget hidden.
    /// The widget can be hidden in two cases:
    /// - when hide() method called from outside of widget (by the user)
    /// - when disappear() method called from inside of widget realization.
    ///
    /// @sa show()
    /// @sa hide()
    /// @sa visible()
    /// @sa appear()
    /// @sa disappear()
    /// @sa signal_show()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    bool hidden() const;

    /// Determines if widget and it's parent are visible.
    ///
    /// The widget is visible, if:
    /// -   widget is not hidden by calling hide() or disappear().
    /// -   widget owner is visible (if Container owns widget) or Display (in case widget is Window) allows window
    ///     to be shown.
    ///
    /// @sa show()
    /// @sa hide()
    /// @sa hidden()
    /// @sa appear()
    /// @sa disappear()
    /// @sa signal_show()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    bool visible() const;

    /// Signal is emitted when hide() or disappear() called.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_hide();
    /// ~~~~~~~~~~~~
    /// @sa show()
    /// @sa hide()
    /// @sa hidden()
    /// @sa visible()
    /// @sa appear()
    /// @sa disappear()
    /// @sa signal_show()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    signal<void()> & signal_hide();

    /// Signal is emitted when show() or appear() called.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_show();
    /// ~~~~~~~~~~~~
    /// @sa show()
    /// @sa hide()
    /// @sa hidden()
    /// @sa visible()
    /// @sa appear()
    /// @sa disappear()
    /// @sa signal_hide()
    /// @sa signal_visible()
    /// @sa signal_invisible()
    signal<void()> & signal_show();

    /// Signal is emitted when widget becomes visible.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_visible();
    /// ~~~~~~~~~~~~
    ///
    /// The widget might become visible when:
    /// -   show() or appear() was called.
    /// -   Parent Container became visible.
    /// -   The widget just inserted into visible Container.
    /// -   In case widget is Window and owning Display shows the window.
    ///
    /// Do not be misunderstood with signal_show().
    /// @sa signal_show()
    /// @sa signal_invisible()
    signal<void()> & signal_visible();

    /// Signal is emitted when widget becomes invisible.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_invisible();
    /// ~~~~~~~~~~~~
    ///
    /// The widget might become invisible when:
    /// - hide() or disappear() was called.
    /// - Parent Container became invisible.
    /// - The widget will be removed from parent Container.
    /// - In case widget is Window and owning Display hides the window.
    ///
    /// Do not be misunderstood with signal_hide().
    /// @sa signal_hide()
    /// @sa signal_visible()
    signal<void()> & signal_invisible();

    /// Get visible area taking in account scroll position.
    /// When widget has Scroller container as an immediate owner, this method
    /// returns rectangle with the origin equals to scroll_position() and the size
    /// of owning container.
    /// When owning widget is not Scroller, the returned origin is <b>0:0</b> and
    /// rectangle size is equals to widget size.
    Rect visible_area() const;

    /// Get part of the widget currently obscured by user.
    /// @since 0.4.0
    Rect obscured_area() const;

    /// Test if any part of the widget is currently obscured by user.
    bool obscured() const;

    /// @}
    /// @name Sensitivity
    /// @{

    /// Enable widget sensitivity.
    ///
    /// From the construction, the widget is enabled.
    ///
    /// @sa disable()
    /// @sa freeze()
    /// @sa thaw()
    /// @sa disabled()
    /// @sa enabled()
    void enable();

    /// Disable widget sensitivity.
    ///
    /// From the construction, the widget is enabled.
    ///
    /// @sa enable()
    /// @sa freeze()
    /// @sa thaw()
    /// @sa disabled()
    /// @sa enabled()
    void disable();

protected:

    /// Disable widget sensitivity.
    ///
    /// Along with disable(), disables the widget sensitivity.
    /// In contrast to disable(), this method is protected and
    /// intended for internal widget realization. For example, the
    /// Menu_item uses this method to disable itself when menu item
    /// becomes disabled.
    ///
    /// @sa enable()
    /// @sa disable()
    /// @sa thaw()
    /// @sa disabled()
    /// @sa enabled()
    void freeze();

    /// Enable widget sensitivity.
    ///
    /// Along with enable(), disables the widget sensitivity.
    /// In contrast to enable(), this method is protected and
    /// intended to internal widget realization. For example, the
    /// Menu_item uses this method to enable itself when menu item
    /// becomes enabled.
    ///
    /// @sa enable()
    /// @sa disable()
    /// @sa freeze()
    /// @sa disabled()
    /// @sa enabled()
    void thaw();

public:

    /// Determines if widget and it's parent are enabled.
    /// The widget enabled if it not disabled by calling disable() or freeze()
    /// and, in addition, it's parent container is also enabled.
    /// From the construction, the widget is enabled.
    /// @sa enable()
    /// @sa disable()
    /// @sa freeze()
    /// @sa thaw()
    /// @sa disabled()
    bool enabled() const;

    /// Determines if widget is disabled.
    /// The widget can be disabled by calling these methods:
    /// 1. disable()
    /// 2. freeze()
    /// From the construction, the widget is enabled.
    /// @sa enable()
    /// @sa disable()
    /// @sa freeze()
    /// @sa thaw()
    /// @sa enabled()
    bool disabled() const;

    /// Signal is emitted when widget becomes enabled.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_enable();
    /// ~~~~~~~~~~~~
    /// @sa enable()
    /// @sa thaw()
    signal<void()> & signal_enable();

    /// Signal is emitted when widget becomes disabled.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_disable();
    /// ~~~~~~~~~~~~
    /// @sa disable()
    /// @sa freeze()
    signal<void()> & signal_disable();

    /// @}
    /// @name Focus
    /// Each widget can have a keyboard focus.
    /// By default, focusing of a widget disallowed. But you can allow it
    /// or disallow at any time.
    /// @{

    /// Allow widget focusing.
    /// By default, focusing is disallowed.
    void allow_focus();

    /// Disallow widget focusing.
    /// By default, focusing is disallowed.
    void disallow_focus();

    /// Test if focusable.
    bool focusable() const;

    /// Causes widget to have the keyboard focus.
    /// @return @b true on success.
    /// This call might fail in the following cases:
    /// 1. The widget hierarchy is incomplete and has no Window at the top.
    /// 2. There are modal focus elsewhere.
    /// 3. The widget does not allowed to gain focus, ie allow_focus() was not called.
    /// @sa grab_modal()
    /// @sa drop_focus()
    bool grab_focus();

    /// Release keyboard focus.
    /// After this call there are no focused widgets within owning window.
    /// This method can not release a modal focus.
    void drop_focus();

    /// Determines if the widget is on the focus path within its parents.
    /// This method also returns @b true if widget has a modal focus.
    bool focused() const;

    /// Take focus.
    /// @sa signal_take_focus()
    bool take_focus();

    /// Causes widget to have modal focus.
    /// After widget gained modal focus, only call to end_modal() can allow
    /// other widgets to gain any type of focus.
    /// To make this call successful, you must allow focus by calling allow_focus().
    /// @return @b true on success.
    ///
    /// This call might fail in the following cases:
    /// - The widget hierarchy is incomplete and has no Window at the top.
    /// - There are another modal focus elsewhere.
    /// - The widget does not allowed to gain focus, ie allow_focus() was not called.
    ///
    /// @sa end_modal()
    /// @sa has_modal()
    /// @sa allow_focus()
    bool grab_modal();

    /// Causes widget to release modal focus.
    /// @sa grab_modal()
    /// @sa grab_focus()
    /// @sa has_modal()
    /// @sa focused()
    void end_modal();

    /// Determines if the widget on the modal focus path within its parents.
    bool has_modal() const;

    /// Signal is emitted when widget gains keyboard focus.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_focus_in();
    /// ~~~~~~~~~~~~
    /// @sa grab_focus()
    /// @sa drop_focus()
    signal<void()> & signal_focus_in();

    /// Signal is emitted when widget loses keyboard focus.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_focus_out();
    /// ~~~~~~~~~~~~
    /// @sa grab_focus()
    /// @sa drop_focus()
    signal<void()> & signal_focus_out();

    /// Signal is emitted when widget invited to grab focus.
    /// This signal emitted by calling take_focus() method.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// bool on_take_focus();
    /// ~~~~~~~~~~~~
    /// Return @b true from here to stop further signal processing.
    /// @sa take_focus()
    signal<bool()> & signal_take_focus();

    /// @}
    /// @name Mouse
    /// @{

    /// Determines if mouse pointer is within widget bounds.
    bool hover() const;

    /// Get mouse pointer position relatively to the widget origin.
    /// The resulting point may be placed outside of widget drawing area.
    Point where_mouse() const;

    /// Actively grabs control of the pointer. Further pointer events are reported only to the grabbing widget.
    /// @return @b true on success.
    bool grab_mouse();

    /// Releases the mouse pointer and any queued events if you actively grabbed the pointer before using grab_mouse.
    /// @return @b true on success.
    bool ungrab_mouse();

    /// Determines is mouse pointer grabbed by widget.
    bool grabs_mouse() const;

    /// Set cursor.
    void set_cursor(Cursor cursor);

    /// Set cursor by name and, optionally, size.
    /// @param name the cursor name.
    /// @param size the cursor size in pixels, 0 for default size.
    void set_cursor(const ustring & name, unsigned size=0);

    /// Read back cursor set by set_cursor().
    /// @note You can not read system cursor image here.
    Cursor cursor();

    /// Unset cursor.
    /// Unsetting cursor causes using parent's cursor.
    void unset_cursor();

    /// Show cursor.
    /// By default, the cursor is not hidden.
    /// @sa hide_cursor()
    /// @sa cursor_visible()
    void show_cursor();

    /// Hide cursor.
    /// By default, the cursor is not hidden.
    /// @sa show_cursor()
    /// @sa cursor_visible()
    void hide_cursor();

    /// Determines whether mouse cursor is hidden.
    /// The cursor can be hidden only by calling hide_cursor().
    /// By default, the cursor is not hidden.
    /// @sa hide_cursor()
    /// @sa show_cursor()
    bool cursor_hidden() const;

    /// Determine whether mouse cursor is visible or not.
    /// The cursor visible when it is not hidden by calling hide_cursor()
    /// and parent's cursor visible too.
    /// @sa hide_cursor()
    /// @sa show_cursor()
    bool cursor_visible() const;

    /// Signal is emitted when mouse button pressed.
    /// Slot prototype:
    /// ~~~~
    /// bool on_mouse_down(int mbt, int mm, const Point & position) {
    ///     // ... do something ...
    ///                     // Return true if you handle this signal
    ///     return true;    // or return false if your are not in interest of this signal.
    /// }
    /// ~~~~
    ///
    /// Here is:
    /// - @b mbt is mouse button number, see #Mouse_buttons enum.
    /// - @b mm  is a mouse modifier mask, see #Mouse_modifiers enum.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from the signal handler to disable further signal emission.
    signal<bool(int, int, Point)> & signal_mouse_down();

    /// Signal is emitted when the mouse button double pressed.
    /// Slot prototype:
    /// ~~~~
    /// bool on_mouse_double_click(int mbt, int mm, const Point & position) {
    ///     // ... do something ...
    ///                     // Return true if you handle this signal
    ///     return true;    // or return false if your are not in interest of this signal.
    /// }
    /// ~~~~
    ///
    /// Here is:
    /// - @b mbt is mouse button number, see #Mouse_buttons enum.
    /// - @b mm  is a mouse modifier mask, see #Mouse_modifiers enum.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from the signal handler to disable further signal emission.
    signal<bool(int, int, Point)> & signal_mouse_double_click();

    /// Signal is emitted when the mouse button released.
    /// Slot prototype:
    /// ~~~~
    /// bool on_mouse_up(int mbt, int mm, const Point & position) {
    ///     // ... do something ...
    ///                     // Return true if you handle this signal
    ///     return true;    // or return false if your are not in interest of this signal.
    /// }
    /// ~~~~
    ///
    /// Here is:
    /// - @b mbt is mouse button number, see #Mouse_buttons enum.
    /// - @b mm  is a mouse modifier mask, see #Mouse_modifiers enum.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from the signal handler to disable further signal emission.
    signal<bool(int, int, Point)> & signal_mouse_up();

    /// Signal is emitted while the mouse pointer moves.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_mouse_motion(int mm, const Point & position);
    /// ~~~~~~~~~~~~
    ///
    /// Here is:
    /// - @b mm  is a mouse modifier mask, see #Mouse_modifiers enum.
    signal<void(int, Point)> & signal_mouse_motion();

    /// Signal is emitted when the mouse pointer entered widget.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_mouse_enter(const Point & position);
    /// ~~~~~~~~~~~~
    signal<void(Point)> & signal_mouse_enter();

    /// Signal is emitted when the mouse pointer leaved widget.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_mouse_leave();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_mouse_leave();

    /// Signal is emitted when the mouse wheel rotates.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// bool on_mouse_wheel(int delta, int mm, const Point & position);
    /// ~~~~~~~~~~~~
    ///
    ///
    /// Here is:
    /// - @b delta  the positive value is for right or down direction,
    ///             the negative one is for left or up direction.
    /// - @b mm     is a mouse modifier mask, see #Mouse_modifiers enum.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from the signal handler to disable further signal emission.
    signal<bool(int, int, Point)> & signal_mouse_wheel();

    /// @}
    /// @name Tool tips
    /// @{

    /// Set tool tip as text.
    void set_tooltip(const ustring & tooltip_text);

    /// Set tool tip as Widget.
    void set_tooltip(Widget & tooltip_widget);

    /// Test if widget has tool tip.
    bool has_tooltip() const;

    /// Unset tool tip.
    void unset_tooltip();

    /// Show tool tip with specified text.
    void show_tooltip(const ustring & tooltip_text);

    /// Show tool tip as widget.
    void show_tooltip(Widget & tooltip_widget);

    /// Show tool tip with specified text at specified position and during specified time.
    void show_tooltip(const ustring & tooltip_text, const Point & pt, Gravity gravity, unsigned time_ms=0);

    /// Show tool tip as widget at specified position and during specified time.
    void show_tooltip(Widget & tooltip_widget, const Point & pt, Gravity gravity, unsigned time_ms=0);

    /// Hide tool tip.
    /// Hides currently displayed tooltip.
    void hide_tooltip();

    /// @}
    /// @name Painting
    /// All painting operations are performed with the help of a Painter object.
    ///
    /// You can obtain painter object in two ways:
    /// -   Wait until signal_backpaint() and signal_paint() arrives with bound painter.
    /// -   Get painter by calling painter() method.
    ///
    /// ### System Repaint Event ###
    ///
    /// The system generated rendering event are divided into two phases.
    /// Two successive signals are emitted by the library:
    ///
    /// -   signal_backpaint() comes first and emitting over entire hierarchy of widgets.
    ///     Widget class has default signal_backpaint() handler connected to the signal
    ///     during Widget class construction. That handler do nothing if <i>%style().get("background").is_set()</i>
    ///     (see Style::get() and Style_item::is_set() methods) returns @b false.
    ///     If it return @b true, the handler simply fill all accessible drawing area
    ///     by background color set by user with calling <i>%style().get("background").set()</i> method
    ///     (see Style_item::set()). Thus, if no one widget within hierarchy have its
    ///     background set, only top level window renders its background and all other widgets within
    ///     hierarchy shares its background with owning window. The Window class has its background set
    ///     by Display during window creation procedure. The programmer can unset widget background
    ///     by calling <i>%style().get("background").unset()</i> call (see Style_item::unset()).
    /// -   After all background handlers invoked, signal_paint() arrives. This signal used to draw foreground
    ///     only. The generic Widget class has no default handler for that signal, but every widget having its
    ///     graphical content, connect its handlers to render itself.
    /// @{

    /// Signal is emitted when a widget is supposed to render it's background.
    /// This %signal emitted when graphical system sends event for repaint,
    /// such as ExposeEvent in X11 or WM_PAINT message on Windows.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from signal handler to disable other handlers.
    ///
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// bool on_backpaint(Painter_ptr pr, const Rect & inval) {
    ///     // ... do painting here ...
    ///     return false;   // Return false to allow other possible handlers.
    /// }
    /// ~~~~~~~~~~~~~~~
    ///
    /// Here is:
    /// -   @b pr    the Painter used for painting operations.
    /// -   @b inval the rectangle to be repainted (you can ignore it for simplicity).
    ///
    /// @note the invalidated rectangle, @e inval, is translated by scroll_position() during
    /// this signal emission, so coordinates shown by it, is logical, not physical.
    ///
    /// @sa signal_paint()
    /// @sa painter()
    /// @sa invalidate()
    /// @sa Window::update()
    signal<bool(Painter, Rect)> & signal_backpaint();

    /// Signal is emitted when a widget is supposed to render itself.
    /// This %signal emitted when graphical system sends event for repaint,
    /// such as ExposeEvent in X11 or WM_PAINT message on Windows.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from signal handler to disable other handlers.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// bool on_paint(Painter_ptr pr, const Rect & inval) {
    ///     // ... do painting here ...
    ///     return false;   // Return false to allow other possible handlers.
    /// }
    /// ~~~~~~~~~~~~
    /// Here is:
    /// -   @b pr    the Painter used for painting operations.
    /// -   @b inval the rectangle to be repainted (you can ignore it for simplicity).
    ///
    /// @note the invalidated rectangle, @e inval, is translated by scroll_position() during
    /// this signal emission, so coordinates shown by it, is logical, not physical.
    ///
    /// @sa signal_backpaint()
    /// @sa painter()
    /// @sa invalidate()
    /// @sa Window::update()
    signal<bool(Painter, Rect)> & signal_paint();

    /// Obtains a painter object.
    ///
    /// Gets so called private painter.
    /// The rendering of the widget can be done in three ways:
    /// -   By calling invalidate() method and than waiting for signal_backpaint() and
    ///     signal_paint() emission.
    /// -   The graphical system can emit two above signals when other window overlaps owning window or
    ///     owning window has been restored or its size has been changed.
    /// -   The widget can redraw itself at any time by using this method.
    ///
    /// The returned painter object can be empty in case Display object is unavailable, so the programmer
    /// can use this method after it receives signal_display() %signal.
    ///
    /// The main difference of the painter object returned by this method and painter object used by
    /// signal_backpaint() and signal_paint() signal is a system clip rectangle,
    /// which stored into painter. When painting done using signal's public painter, that clip rectangle
    /// is set to intersection of invalidate rectangle and widget bounds, so isn't entire widget area
    /// may be accessible by the painter. The painter, returned by this method, has full access to entire
    /// widget drawing area. Also, in contrast to signal_paint() and signal_backpaint() painters,
    /// when using private painter, the programmer must repaint widget background manually.
    ///
    /// The painter, returned by this method, can be stored as class member data and will be valid until
    /// widget will be removed from its owning container.
    ///
    /// Since returned painter can be empty, it's necessarily to test it by calling Painter::operator bool().
    ///
    /// Sample:
    /// ~~~~~~~~~~~~~~~~~~~~~~~~
    /// auto pr = this->painter();
    ///
    /// if (pr) {
    ///     // ... good painter ...
    /// }
    ///
    /// else {
    ///     // ... unusable painter ...
    /// }
    /// ~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// @return a painter object.
    /// @sa signal_backpaint()
    /// @sa signal_paint()
    /// @sa invalidate()
    /// @sa Window::update()
    Painter painter();

    /// Invalidate widget drawing area.
    ///
    /// Mark widget drawing area as needed to be repainted.
    /// Using this method enqueues signal_backpaint() and signal_paint() emission in some nearest future.
    /// The time point of that future is system dependent, the typical timeout is a few dozens milliseconds.
    ///
    /// @param r the invalidated region, the entire widget drawing area if empty.
    /// @sa signal_backpaint()
    /// @sa signal_paint()
    /// @sa painter()
    /// @sa Window::update()
    void invalidate(const Rect & r=Rect());

    /// @}
    /// @name Keyboard
    /// @{

    /// Signal is emitted when the key is pressed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// bool on_key_down(char32_t kc, int km);
    /// ~~~~~~~~~~~~
    ///
    /// Here is:
    /// - @b kc is a Unicode value or one of the #Key_codes enum values.
    /// - @b km is a bit mask of #Key_modifiers enum values.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from signal hander to disable other handlers.
    /// @sa signal_key_up()
    signal<bool(char32_t, int)> & signal_key_down();

    /// Signal is emitted when the key is released.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// bool on_key_up(char32_t kc, int km);
    /// ~~~~~~~~~~~~
    ///
    /// Here is:
    /// - @b kc is a Unicode value or one of the #Key_codes enum values.
    /// - @b km is a bit mask of #Key_modifiers enum values.
    ///
    /// Multiple slots may be connected to the signal.
    /// Return @b true from signal handler to disable other handlers.
    /// @sa signal_key_down()
    signal<bool(char32_t, int)> & signal_key_up();

    /// Signal is emitted when Unicode characters entered from keyboard or somehow.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// bool on_input(const ustring & s);
    /// ~~~~~~~~~~~~~~~
    ///
    /// Here is:
    /// - @b s is a Unicode string.
    ///
    /// Return @b true from signal handler if you done with input.
    signal<bool(const ustring &)> & signal_input();

    /// Connect a keyboard accelerator.
    /// Connection is done by connecting accelerator to the internal implementation
    /// signal managing keyboard accelerators.
    ///
    /// The @b prepend parameter may be used in case the user wants to override an
    /// existing accelerator. If it @b true, the newly connected accelerator will be
    /// placed at the front of the internal signal and will be activated earlier
    /// than existing accelerators.
    ///
    /// The returning @b connection object can be used to suspend accelerator
    /// appearance by calling connection::block() method and than resume appearance
    /// by calling connection::unblock() method or accelerator can be completely
    /// disabled by calling connection::disconnect() method.
    ///
    /// @param accel an accelerator to be connected.
    /// @param prepend if @b true, add accelerator to the front of accelerator list.
    /// @return a connection object.
    /// @sa connect_action()
    connection connect_accel(Accel & accel, bool prepend=false);

    /// Connect an action.
    /// Connects all accelerators managed by action.
    /// If some accelerators within given action will be added, changed or removed,
    /// the widget will handle that changes.
    /// If connected action will be destroyed, all accelerator will no longer works.
    /// @param action an action to be connected.
    /// @param prepend if @b true, add accelerators to the front of accelerator list.
    /// @sa connect_accel()
    void connect_action(Action_base & action, bool prepend=false);

    /// Lookup action.
    /// Searches an Action_base corresponding given specification within whole hierarchy
    /// starting with current widget.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @return the Action_base pointer or @b nullptr if not found.
    /// @warning Do not keep, @b delete or @a free() returned pointer!
    Action_base * lookup_action(char32_t kc, int km);

    /// @}
    /// @name Control
    /// @{

    /// Get display.
    /// Obtains Display object which widget belongs to.
    /// The returning object may be pure in case widget not yet inserted into hierarchy.
    Display display();

    /// Get display.
    /// Obtains Display object which widget belongs to.
    /// The returning object may be pure in case widget not yet inserted into hierarchy.
    const Display display() const;

    /// Get style.
    Style & style();

    /// Get style.
    const Style & style() const;

    /// Causes top level window to close, if that window is Dialog.
    void quit_dialog();

    /// Test if widget has parent container.
    /// @overload has_parent() const
    bool has_parent() const;

    /// Test if widget is ancestor of specified container.
    /// @since 0.4.0
    /// @overload has_parent(const Widget & w) const
    bool has_parent(const Widget & w) const;

    /// Test if widget inserted into window.
    bool has_window() const;

    /// Signal is emitted when object destroyed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_destroy();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_destroy();

    /// Signal is emitted when widget inserted into container.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_parent();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_parent();

    /// Signal is emitted when Display became accessible for widget.
    /// From the construction, the widget haven't an access to the owning display.
    /// After widget inserted into hierarchy using some sort of insertion function, provided
    /// by the owning Container, it can gain an access to the owning display. When display
    /// became visible by owning container, it (owning container) emits signal_display()
    /// on each of its children.
    ///
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_display() {
    ///     // At this line a Display object is visible and Widget::display() call returns non-empty result.
    /// }
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_display();

    /// Signal is emitted when widget removed from it's parent container.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_unparent();
    /// ~~~~~~~~~~~~
    signal<void()> & signal_unparent();

    /// Signal is emitted when widget inserted into selection.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_select();
    /// ~~~~~~~~~~~~
    /// The widget can be inserted into container utilized function of selection,
    /// such as List. In that case the container emits this signal on each widget
    /// entering selection.
    /// @sa signal_unselect()
    signal<void()> & signal_select();

    /// Signal is emitted when widget removed from selection.
    /// Slot prototype:
    /// ~~~~~~~~~~~~
    /// void on_unselect();
    /// ~~~~~~~~~~~~
    /// The widget can be inserted into container utilized function of selection,
    /// such as List. In that case the container emits this signal on each widget
    /// leaving selection.
    /// @sa signal_select()
    signal<void()> & signal_unselect();

    /// @}

protected:

    /// @private
    Widget(std::nullptr_t);

    /// @private
    Widget_ptr impl;

    /// @private
    friend Widget_impl;
};

} // namespace tau

#endif // TAU_WIDGET_HH
