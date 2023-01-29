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

/// @file painter.hh The Painter class.

#ifndef TAU_PAINTER_HH
#define TAU_PAINTER_HH

#include <tau/types.hh>
#include <tau/enums.hh>
#include <tau/ustring.hh>

namespace tau {

/// The painting engine.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup paint_group
class Painter {
public:

    /// Default constructor creates a pure %Painter.
    Painter();

    /// Copy constructor.
    Painter(const Painter & other) = default;

    /// Copy operator.
    Painter & operator=(const Painter & other) = default;

    /// Test if %Painter not pure.
    operator bool() const;

    /// Reset implementation pointer.
    void reset();

    /// Set font.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void set_font(Font font);

    /// Select font.
    /// @throw graphics_error on Unix if unable to select font_spec and default system font.
    /// @return may return an empty pointer in case %Painter is empty, so always check returned pointer.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Font select_font(const ustring & font_spec);

    /// Get current font.
    /// @return may return an empty pointer in case %Painter is empty, so always check returned pointer.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Font font();

    /// Get text size.
    /// @param s an UTF-8 string.
    /// @param orient text orientation (reserved, but not yet implemented).
    /// @return zeroed vector on empty %Painter or text size in pixels.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Vector text_size(const ustring & s, Orientation orient=ORIENTATION_RIGHT);

    /// Get text size.
    /// @param s an UTF-32 string.
    /// @param orient text orientation (reserved, but not yet implemented).
    /// @return zeroed vector on empty %Painter or text size in pixels.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Vector text_size(const std::u32string & s, Orientation orient=ORIENTATION_RIGHT);

    /// Draw text at current position.
    /// @param s text to be drawn.
    /// @param c color that will be used.
    /// @param orient text orientation (reserved, but not yet implemented).
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void text(const ustring & s, const Color & c, Orientation orient=ORIENTATION_RIGHT);

    /// Draw text at current position.
    /// @param s text to be drawn.
    /// @param c color that will be used.
    /// @param orient text orientation (reserved, but not yet implemented).
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void text(const std::u32string & s, const Color & c, Orientation orient=ORIENTATION_RIGHT);

    /// Draw glyph at current position.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void glyph(Glyph glyph);

    /// Get transformation matrix.
    /// @return may return a reference to a dummy Matrix in case %Painter is empty.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Matrix & matrix() const;

    /// Set Pen used for contour stroke.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void set_pen(Pen pen);

    /// Get pen used for contour stroke.
    /// @return may return default Pen in case %Painter is empty.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Pen pen() const;

    /// Set brush used for contour fill.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void set_brush(Brush brush);

    /// Get brush used for contour fill.
    /// @return may return default Brush in case %Painter is empty.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Brush brush() const;

    /// Fill all available area with current Brush.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void paint();

    /// Fill area defined by the current path with current Brush and clear the path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void fill();

    /// Fill area defined by the current path with current Brush and keep the path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void fill_preserve();

    /// Stroke contour defined by the current path with current Brush and clear the path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void stroke();

    /// Stroke contour defined by the current path with current Brush and keep the path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void stroke_preserve();

    /// Save current context.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void push();

    /// Restore previously saved context.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void pop();

    /// Clear current path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void clear();

    /// Set drawing operator.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void set_oper(Oper op);

    /// Get drawing operator.
    /// @return @b OPER_COPY (see #Oper enum) in case %Painter is pure or actual operator used for painting.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Oper oper() const;

    /// Add part of the Pixmap to the current path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void pixmap(Pixmap_ptr pix, const Point pix_origin, const Size & pix_size, bool transparent=false);

    /// Add entire Pixmap to the current path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void pixmap(Pixmap_ptr pix, bool transparent=false);

    /// Get current position.
    /// @return default Vector in case %Painter is pure.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    Vector position() const;

    /// Change current position.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void move_to(double x, double y);

    /// Change current position.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void move_to(const Vector & v);

    /// Change current position relatively.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void move_rel(double x, double y);

    /// Change current position relatively.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void move_rel(const Vector & v);

    /// Draw rectangle.
    /// @param x1 left coordinate.
    /// @param y1 top coordinate.
    /// @param x2 right coordinate.
    /// @param y2 bottom coordinate.
    /// @param radius rectangle corners radius (reserved but not yet realized).
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void rectangle(double x1, double y1, double x2, double y2, double radius=0.0);

    /// Draw rectangle.
    /// @param v1 top left coordinates.
    /// @param v2 bottom top coordinates.
    /// @param radius rectangle corners radius (reserved but not yet realized).
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void rectangle(const Vector & v1, const Vector & v2, double radius=0.0);

    /// Draw line from current position previously set by move_to() or move_rel() to the ending point.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void line_to(double x, double y);

    /// Draw line from current position previously set by move_to() or move_rel() to the ending point.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void line_to(const Vector & end);

    /// Draw line from current position previously set by move_to() or move_rel() to the ending point.
    /// @param dx X distance from current position to the end point.
    /// @param dy Y distance from current position to the end point.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void line_rel(double dx, double dy);

    /// Draw line from current position previously set by move_to() or move_rel() to the ending point.
    /// @param dv distance from current position to the end point.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void line_rel(const Vector & dv);

    /// Draw conic Bezier curve from current position previously set by move_to() or move_rel() to the ending point.
    /// @param cx control point X coordinate.
    /// @param cy control point Y coordinate.
    /// @param ex end point X coordinate.
    /// @param ey end point Y coordinate.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void conic_to(double cx, double cy, double ex, double ey);

    /// Draw conic Bezier curve from current position previously set by move_to() or move_rel() to the ending point.
    /// @param cp control point coordinate.
    /// @param end end point coordinate.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void conic_to(const Vector & cp, const Vector & end);

    /// Draw cubic Bezier curve from current position previously set by move_to() or move_rel() to the ending point.
    /// @param cx1 control point 1 X coordinate.
    /// @param cy1 control point 1 Y coordinate.
    /// @param cx2 control point 2 X coordinate.
    /// @param cy2 control point 2 Y coordinate.
    /// @param ex end point X coordinate.
    /// @param ey end point Y coordinate.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void cubic_to(double cx1, double cy1, double cx2, double cy2, double ex, double ey);

    /// Draw cubic Bezier curve from current position previously set by move_to() or move_rel() to the ending point.
    /// @param cp1 control point 1 coordinate.
    /// @param cp2 control point 2 coordinate.
    /// @param end end point coordinate.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void cubic_to(const Vector & cp1, const Vector & cp2, const Vector & end);

    /// Add arc subpath.
    /// @param center arc center point.
    /// @param radius arc radius.
    /// @param angle1 starting angle.
    /// @param angle2 ending angle.
    /// @param pie raster via arc center when filling.
    /// Positive angles indicate counterclockwise motion,
    /// and negative angles indicate clockwise motion.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void arc(const Vector & center, double radius, double angle1, double angle2, bool pie=false);

    /// Add arc subpath.
    /// @param cx arc center X point.
    /// @param cy arc center Y point.
    /// @param radius arc radius.
    /// @param angle1 starting angle.
    /// @param angle2 ending angle.
    /// @param pie raster via arc center when filling.
    /// Positive angles indicate counterclockwise motion,
    /// and negative angles indicate clockwise motion.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void arc(double cx, double cy, double radius, double angle1, double angle2, bool pie=false);

    /// Add full circle subpath.
    /// @param center center point.
    /// @param radius circle radius.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void circle(const Vector & center, double radius);

    /// Add full circle subpath.
    /// @param cx center X point.
    /// @param cy center Y point.
    /// @param radius circle radius.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void circle(double cx, double cy, double radius);

    /// Add contour to the current path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void contour(const Contour & ctr);

    /// Add contour to the current path.
    /// @note This method does not work on pure %Painter.
    /// @note Check %Painter is not pure before using it!
    void contour(Contour && ctr);

private:

    Painter_ptr impl;
    friend class Container_impl;
    friend class Pixmap_impl;
    friend class Widget_impl;
    Painter(Painter_ptr pp);
};

} // namespace tau

#endif // TAU_PAINTER_HH
