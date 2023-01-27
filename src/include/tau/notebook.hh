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

#ifndef TAU_NOTEBOOK_HH
#define TAU_NOTEBOOK_HH

/// @file notebook.hh The Notebook class definition.

#include <tau/enums.hh>
#include <tau/widget.hh>

namespace tau {

/// %Container which shows one of its children at a time, in tabbed windows.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// The %Notebook widget is a Container whose children are pages that can be
/// switched between using tab labels along one gravity.
/// The gravity at which the tabs for switching pages in the notebook are drawn
/// is set in the class constructor.
/// @ingroup container_group
/// @ingroup widget_group
class Notebook: public Widget {
public:

    /// @name Constructors, operators
    /// @{

    /// Default constructor.
    /// @param tab_pos the tab position.
    Notebook(Tab_pos tab_pos=TAB_TOP);

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Notebook(const Notebook & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Notebook & operator=(const Notebook & other) = default;

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
    Notebook(Widget_ptr wp);

    /// @}
    /// Appends a page to notebook.
    /// @param w the widget to be appended.
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int append_page(Widget & w);

    /// Appends a page to notebook.
    /// @param w the widget to be appended.
    /// @param title the title to be used in tab.
    /// @param rptr where to put returned pointer to the created Text widget [optional].
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int append_page(Widget & w, const ustring & title, Widget_ptr * rptr=nullptr);

    /// Appends a page to notebook.
    /// @param w the widget to be appended.
    /// @param title_widget the widget to be drawn in the tab.
    /// @return a number of added page.
    /// @throw user_error if any widget already inserted into another container.
    int append_page(Widget & w, Widget & title_widget);

    /// Prepends a page to notebook.
    /// @param w the widget to be prepended.
    /// @throw user_error if widget already inserted into another container.
    void prepend_page(Widget & w);

    /// Prepends a page to notebook.
    /// @param w the widget to be prepended.
    /// @param title the title to be used in tab.
    /// @return pointer to the created Text widget.
    /// @throw user_error if widget already inserted into another container.
    Widget_ptr prepend_page(Widget & w, const ustring & title);

    /// Prepends a page to notebook.
    /// @param w the widget to be prepended.
    /// @param title_widget the widget to be drawn in the tab.
    /// @throw user_error if any widget already inserted into another container.
    void prepend_page(Widget & w, Widget & title_widget);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param nth_page the page number for the new page.
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int insert_page(Widget & w, int nth_page);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param nth_page the page number for the new page.
    /// @param title the title to be used in tab.
    /// @param rptr where to put returned pointer to the created Text widget [optional].
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int insert_page(Widget & w, int nth_page, const ustring & title, Widget_ptr * rptr=nullptr);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param nth_page the page number for the new page.
    /// @param title_widget the widget to be drawn in the tab.
    /// @return a number of added page.
    /// @throw user_error if any widget already inserted into another container.
    int insert_page(Widget & w, int nth_page, Widget & title_widget);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param after_this the new widget will be inserted after this widget.
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int insert_page_after(Widget & w, Widget & after_this);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param after_this the new widget will be inserted after this widget.
    /// @param title the title to be used in tab.
    /// @param rptr where to put returned pointer to the created Text widget [optional].
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int insert_page_after(Widget & w, Widget & after_this, const ustring & title, Widget_ptr * rptr=nullptr);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param after_this the new widget will be inserted after this widget.
    /// @param title_widget the widget to be drawn in the tab.
    /// @return a number of added page.
    /// @throw user_error if any widget already inserted into another container.
    int insert_page_after(Widget & w, Widget & after_this, Widget & title_widget);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param before_this the new widget will be inserted before this widget.
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int insert_page_before(Widget & w, Widget & before_this);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param before_this the new widget will be inserted before this widget.
    /// @param title the title to be used in tab.
    /// @param rptr where to put returned pointer to the created Text widget [optional].
    /// @return a number of added page.
    /// @throw user_error if widget already inserted into another container.
    int insert_page_before(Widget & w, Widget & before_this, const ustring & title, Widget_ptr * rptr=nullptr);

    /// Inserts a page to notebook.
    /// @param w the widget to be inserted.
    /// @param before_this the new widget will be inserted before this widget.
    /// @param title_widget the widget to be drawn in the tab.
    /// @return a number of added page.
    /// @throw user_error if any widget already inserted into another container.
    int insert_page_before(Widget & w, Widget & before_this, Widget & title_widget);

    /// Removes a page from notebook.
    /// @return a number of deleted page or -1 if page not found.
    int remove_page(Widget & w);

    /// Removes a page from notebook.
    /// @return a number of deleted page or -1 if page not found.
    int remove_page(int nth_page);

    /// Get current page number.
    /// @return -1 if empty or current page number.
    int current_page() const;

    /// Finds the index of the page which contains the given child widget.
    /// @return -1 if not found or page number.
    int page_number(const Widget & w) const;

    /// Get page count.
    std::size_t page_count() const;

    /// Test if empty.
    bool empty() const;

    /// Remove all children.
    void clear();

    /// Show next page.
    /// @return page number of the shown page.
    /// @sa allow_rollover().
    /// @sa disallow_rollover().
    /// @sa rollover_allowed().
    int show_next();

    /// Show previous page.
    /// @return page number of the shown page.
    /// @sa allow_rollover().
    /// @sa disallow_rollover().
    /// @sa rollover_allowed().
    int show_previous();

    /// Show specified page.
    /// @return page number of the shown page.
    int show_page(int nth_page);

    /// Allow page reordering by user using mouse.
    /// @sa disallow_reorder().
    /// @sa reorder_allowed().
    /// @note allowed by default.
    void allow_reorder();

    /// Disallow page reordering by user using mouse.
    /// @sa allow_reorder().
    /// @sa reorder_allowed().
    /// @note allowed by default.
    void disallow_reorder();

    /// Determines whether reordering enabled.
    /// @sa allow_reorder().
    /// @sa disallow_reorder().
    /// @note allowed by default.
    bool reorder_allowed() const;

    /// Allow tabs rollover.
    /// When allowed:
    /// When show_next() or show_previous() invoked and current tab is first or last,
    /// the next current page will be last or first, correspondingly.
    /// When disallowed, show_next() and show_previous() will do nothing on first and
    /// last pages.
    /// @note allowed by default.
    /// @sa show_next().
    /// @sa show_previous().
    /// @sa disallow_rollover().
    /// @sa rollover_allowed().
    void allow_rollover();

    /// Disallow tabs rollover.
    /// @note allowed by default.
    /// @sa show_next().
    /// @sa show_previous().
    /// @sa allow_rollover().
    /// @sa rollover_allowed().
    void disallow_rollover();

    /// Test if rollover allowed.
    /// @sa show_next().
    /// @sa show_previous().
    /// @sa allow_rollover().
    /// @sa disallow_rollover().
    /// @note allowed by default.
    bool rollover_allowed() const;

    /// Reorders the page containing w, so that it appears in page nth_page.
    /// @sa allow_reorder().
    /// @sa disallow_reorder().
    /// @sa reorder_allowed().
    void reorder_page(Widget & w, int nth_page);

    /// Reorders the page old_page, so that it appears in page new_page.
    /// @sa allow_reorder().
    /// @sa disallow_reorder().
    /// @sa reorder_allowed().
    void reorder_page(int old_page, int new_page);

    /// Shows tabs.
    /// @note Tabs are visible by default.
    void show_tabs();

    /// Hide tabs.
    /// @note Tabs are visible by default.
    void hide_tabs();

    /// Determines if tabs visible.
    bool tabs_visible() const;

    /// Set homogeneous tabs.
    void set_homogeneous_tabs();

    /// Unset homogeneous tabs.
    void unset_homogeneous_tabs();

    /// Test if tabs are homogeneous.
    bool homogeneous_tabs() const;

    /// Signal emitted when page added.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_page_added(int number);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_page_added();

    /// Signal emitted when page removed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_page_removed(int number);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_page_removed();

    /// Signal emitted when current page changed.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_page_changed(int number);
    /// ~~~~~~~~~~~~~~
    signal<void(int)> & signal_page_changed();

    /// Signal emitted when page reordered.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~
    /// void on_page_reordered(int old_number, int new_number);
    /// ~~~~~~~~~~~~~~
    signal<void(int, int)> & signal_page_reordered();
};

} // namespace tau

#endif // TAU_NOTEBOOK_HH
