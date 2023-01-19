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

/// @file pen.hh The Pen class.

#ifndef TAU_PEN_HH
#define TAU_PEN_HH

#include <tau/enums.hh>
#include <tau/types.hh>
#include <vector>

namespace tau {

/// A pen used for drawing.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup paint_group
class Pen {
public:

    /// Constructor with default parameters.
    Pen();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Pen(const Pen & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Pen & operator=(const Pen & other) = default;

    /// Move constructor.
    Pen(Pen && other);

    /// Construct cosmetic pen.
    ///
    /// Cosmetic pens are used to draw strokes that have a constant width regardless of any transformations
    /// applied to the QPainter they are used with. Drawing a shape with a cosmetic pen ensures that its
    /// outline will have the same thickness at different scale factors.
    Pen(const Color & color);

    /// Constructor with parameters.
    Pen(const Color & color, double width, Line_style ls=SOLID_LINE, Cap_style caps=SQUARE_CAP, Join_style js=BEVEL_JOIN);

    /// Set color.
    void set_color(const Color & color);

    /// Get color.
    Color color() const;

    /// Set line style.
    void set_line_style(Line_style ls);

    /// Get line style.
    Line_style line_style() const;

    /// Get line width.
    double line_width() const;

    /// Set line width.
    void set_line_width(double width);

    /// Get cap style.
    Cap_style cap_style() const;

    /// Set cap style.
    void set_cap_style(Cap_style caps);

    /// Get join style.
    Join_style join_style() const;

    /// Set join style.
    void set_join_style(Join_style js);

    /// Set dash pattern.
    ///
    /// Sets the dash pattern for this pen to the given pattern.
    /// The pattern must be specified as an even number of positive entries
    /// where the entries 1, 3, 5... are the dashes and 2, 4, 6... are the spaces.
    /// The dash pattern is specified in units of the pens width; e.g. a dash
    /// of length 5 in width 10 is 50 pixels long. Note that a pen with zero width
    /// is equivalent to a cosmetic pen with a width of 1 pixel.
    /// Each dash is also subject to cap styles so a dash of 1 with square cap set
    /// will extend 0.5 pixels out in each direction resulting in a total width of 2.
    /// Note that the default cap style is SQUARE_CAP, meaning that a square line end
    /// covers the end point and extends beyond it by half the line width.
    void set_dash_pattern(const std::vector<double> & dashes);

    /// Get dash pattern.
    std::vector<double> dash_pattern() const;

    /// Sets the dash offset.
    ///
    /// Sets the dash offset (the starting point on the dash pattern) for this pen
    /// to the offset specified. The offset is measured in terms of the units used
    /// to specify the dash pattern.
    void set_dash_offset(double offset);

    /// Returns the dash offset for the pen.
    double dash_offset() const;

    /// Sets the miter limit of this pen to the given limit.
    ///
    /// The miter limit describes how far a miter join can extend from the join point.
    /// This is used to reduce artifacts between line joins where the lines are close to parallel.
    /// This value does only have effect when the pen style is set to MITER_JOIN.
    /// The value is specified in units of the pen's width, e.g. a miter limit of 5 in
    /// width 10 is 50 pixels long. The default miter limit is 2, i.e. twice the pen width in pixels.
    void set_miter_limit(double limit);

    /// Returns the miter limit.
    double miter_limit() const;

private:

    friend class Painter_impl;
    mutable Pen_ptr impl;
    Pen(Pen_ptr pp);
};

} // namespace tau

#endif // TAU_PEN_HH
