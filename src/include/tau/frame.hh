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

#ifndef TAU_FRAME_HH
#define TAU_FRAME_HH

/// @file frame.hh The Frame class definition.

#include <tau/enums.hh>
#include <tau/container.hh>

namespace tau {

/// Single child container with border and optional text label.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup widget_group
/// @ingroup container_group
class Frame: public Container {
public:

    /// @name Constructors and operators
    /// @{

    /// Default constructor.
    Frame();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Frame(const Frame & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Frame & operator=(const Frame & other) = default;

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
    Frame(Widget_ptr wp);

    /// Constructor with a label.
    Frame(const ustring & label);

    /// Constructor with a label and alignment.
    /// @note By default, the alignment is ALIGN_CENTER.
    Frame(const ustring & label, Align align);

    /// Constructor with border style, width and radius.
    Frame(Border_style bs, unsigned border_width=1, int border_radius=0);

    /// Constructor with label, border style, width and radius.
    Frame(const ustring & label, Border_style bs, unsigned border_width=1, int border_radius=0);

    /// Constructor with label, alignment, border style, width and radius.
    /// @note By default, the alignment is ALIGN_CENTER.
    Frame(const ustring & label, Align align, Border_style bs, unsigned border_width=1, int border_radius=0);

    /// @}
    /// Set owning widget.
    /// @throw user_error if widget already inserted into another container.
    void insert(Widget & w);

    /// Unset owning widget.
    void clear();

    /// Set label text.
    void set_label(const ustring & label, Align align=ALIGN_CENTER);

    /// Set label widget.
    /// @throw user_error if widget already inserted into another container.
    void set_label(Widget & widget);

    /// Remove label.
    void unset_label();

    /// Set label align.
    /// @note By default, the alignment is ALIGN_CENTER.
    void align_label(Align align);

    /// Get label align.
    Align label_align() const;

    /// Set label position.
    void set_label_position(Label_pos label_pos);

    /// Get label position.
    Label_pos label_position() const;

    /// Set border width.
    /// @param npx border width in pixels.
    void set_border(unsigned npx);

    /// Set border width in pixels.
    /// @param left left border width in pixels.
    /// @param right right border width in pixels.
    /// @param top top border width in pixels.
    /// @param bottom bottom border width in pixels.
    void set_border(unsigned left, unsigned right, unsigned top, unsigned bottom);

    /// Set left border width in pixels.
    /// @param npx left border width in pixels.
    void set_border_left(unsigned npx);

    /// Set right border width in pixels.
    /// @param npx right border width in pixels.
    void set_border_right(unsigned npx);

    /// Set top border width in pixels.
    /// @param npx top border width in pixels.
    void set_border_top(unsigned npx);

    /// Set bottom border width in pixels and style.
    /// @param npx bottom border width in pixels.
    void set_border_bottom(unsigned npx);

    /// Set left border width in pixels and style.
    /// @param px left border width in pixels.
    /// @param bs the border style.
    void set_border_left(unsigned px, Border_style bs);

    /// Set right border width in pixels and style.
    /// @param px right border width in pixels.
    /// @param bs the border style.
    void set_border_right(unsigned px, Border_style bs);

    /// Set top border width in pixels and style.
    /// @param px top border width in pixels.
    /// @param bs the border style.
    void set_border_top(unsigned px, Border_style bs);

    /// Set bottom border width in pixels and style.
    /// @param px bottom border width in pixels.
    /// @param bs the border style.
    void set_border_bottom(unsigned px, Border_style bs);

    /// Set border width and style.
    /// @param px border width in pixels.
    /// @param bs the border style.
    void set_border(unsigned px, Border_style bs);

    /// Set left border width, style and color.
    /// @param px border width in pixels.
    /// @param bs the border style.
    /// @param color the color.
    void set_border_left(unsigned px, Border_style bs, const Color & color);

    /// Set right border width, style and color.
    /// @param px border width in pixels.
    /// @param bs the border style.
    /// @param color the color.
    void set_border_right(unsigned px, Border_style bs, const Color & color);

    /// Set top border width, style and color.
    /// @param px border width in pixels.
    /// @param bs the border style.
    /// @param color the color.
    void set_border_top(unsigned px, Border_style bs, const Color & color);

    /// Set bottom border width, style and color.
    /// @param px border width in pixels.
    /// @param bs the border style.
    /// @param color the color.
    void set_border_bottom(unsigned px, Border_style bs, const Color & color);

    /// Set border width, style and color.
    /// @param px border width in pixels.
    /// @param bs the border style.
    /// @param color the color.
    void set_border(unsigned px, Border_style bs, const Color & color);

    /// Get left border width in pixels.
    unsigned border_left() const;

    /// Get right border width in pixels.
    unsigned border_right() const;

    /// Get top border width in pixels.
    unsigned border_top() const;

    /// Get bottom border width in pixels.
    unsigned border_bottom() const;

    /// Set all borders color.
    /// @param color the color.
    void set_border_color(const Color & color);

    /// Set all borders color.
    /// @param left left border color.
    /// @param right right border color.
    /// @param top top border color.
    /// @param bottom bottom border color.
    void set_border_color(const Color & left, const Color & right, const Color & top, const Color & bottom);

    /// Set left border color.
    /// @param color the color.
    void set_border_left_color(const Color & color);

    /// Set right border color.
    /// @param color the color.
    void set_border_right_color(const Color & color);

    /// Set top border color.
    /// @param color the color.
    void set_border_top_color(const Color & color);

    /// Set bottom border color.
    /// @param color the color.
    void set_border_bottom_color(const Color & color);

    /// Set default border color.
    void unset_border_color();

    /// Set default left border color.
    void unset_border_left_color();

    /// Set default right border color.
    void unset_border_right_color();

    /// Set default top border color.
    void unset_border_top_color();

    /// Set default bottom border color.
    void unset_border_bottom_color();

    /// Get left border color.
    Color border_left_color() const;

    /// Get right border color.
    Color border_right_color() const;

    /// Get top border color.
    Color border_top_color() const;

    /// Get bottom border color.
    Color border_bottom_color() const;

    /// Set border style.
    /// @param bs the border style.
    void set_border_style(Border_style bs);

    /// Set border style.
    /// @param left left border style.
    /// @param right right border style.
    /// @param top top border style.
    /// @param bottom bottom border style.
    void set_border_style(Border_style left, Border_style right, Border_style top, Border_style bottom);

    /// Set left border style.
    /// @param bs the border style.
    void set_border_left_style(Border_style bs);

    /// Set right border style.
    /// @param bs the border style.
    void set_border_right_style(Border_style bs);

    /// Set top border style.
    /// @param bs the border style.
    void set_border_top_style(Border_style bs);

    /// Set bottom border style.
    /// @param bs the border style.
    void set_border_bottom_style(Border_style bs);

    /// Get left border style.
    Border_style border_left_style() const;

    /// Get right border style.
    Border_style border_right_style() const;

    /// Get top border style.
    Border_style border_top_style() const;

    /// Get bottom border style.
    Border_style border_bottom_style() const;

    /// Set border radius.
    void set_border_radius(int radius);

    /// Get border radius.
    int border_radius() const;
};

} // namespace tau

#endif // TAU_FRAME_HH
