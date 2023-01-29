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

/// @file container.hh The Container class.

#ifndef TAU_CONTAINER_HH
#define TAU_CONTAINER_HH

#include <tau/widget.hh>

namespace tau {

/// An abstract widget's @ref container_group "container" base.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup container_group
/// @ingroup widget_group
class Container: public Widget {
public:

    /// Force children arrange.
    void queue_arrange();

    /// Get focus owner.
    Widget_ptr focus_owner();

    /// Get focus owner.
    Widget_cptr focus_owner() const;

    /// Get children.
    std::vector<Widget_ptr> children() const;

    /// Signal emitted when some child (or children) added to or removed from container.
    ///
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_children_changed();
    /// ~~~~~~~~~~~~~~~
    signal<void()> & signal_children_changed();

protected:

    /// @throw user_error if widget already inserted into another container.
    void make_child(Widget & w);

    void unparent_child(Widget & w);

    bool update_child_bounds(Widget & w, const Rect & bounds);
    bool update_child_bounds(Widget & w, const Point & origin, const Size & sz);
    bool update_child_bounds(Widget & w, int x, int y, const Size & sz);
    bool update_child_bounds(Widget & w, int x, int y, unsigned width, unsigned height);

    signal<void()> & signal_arrange();

    /// @private
    Container(Widget_ptr wp);

    /// @private
    Container(std::nullptr_t);
};

} // namespace tau

#endif // TAU_CONTAINER_HH
