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

/// @file twins.hh The Twins container class.

#ifndef TAU_TWINS_HH
#define TAU_TWINS_HH

#include <tau/enums.hh>
#include <tau/container.hh>

namespace tau {

/// A widget with two adjustable panes.
/// @ingroup container_group
class Twins: public Container {
public:

    /// Default constructor.
    Twins(Orientation orient, double ratio=0.5);

    /// Insert first widget.
    /// @throw user_error if widget already inserted into another container.
    void insert_first(Widget & w);

    /// Insert second widget.
    /// @throw user_error if widget already inserted into another container.
    void insert_second(Widget & w);

    /// Test if has first child.
    bool has_first() const;

    /// Test if has second child.
    bool has_second() const;

    /// Test if empty.
    bool empty() const;

    /// Remove first child.
    void remove_first();

    /// Remove second child.
    void remove_second();

    /// Remove both children.
    void clear();

    /// Set the ratio of the size of the first widget to the second.
    void set_ratio(double ratio);

    /// Get the ratio of the size of the first widget to the second
    double ratio() const;

    /// Signal emitted when user changed ratio by moving handle.
    /// Slot protorype:
    /// ~~~~~~~~~~~~~~~
    /// void on_ratio_changed(double ratio);
    /// ~~~~~~~~~~~~~~~
    signal<void(double)> & signal_ratio_changed();
};

} // namespace tau

#endif // TAU_TWINS_HH
