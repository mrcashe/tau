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

#ifndef TAU_TEXT_IMPL_HH
#define TAU_TEXT_IMPL_HH

#include <tau/accel.hh>
#include <tau/action.hh>
#include <tau/buffer.hh>
#include <tau/color.hh>
#include <tau/input.hh>
#include <tau/font.hh>
#include <tau/painter.hh>
#include <widget-impl.hh>

namespace tau {

class Text_impl: public Widget_impl {
public:

    Text_impl();
    explicit Text_impl(Align xalign, Align yalign=ALIGN_CENTER);
    explicit Text_impl(const ustring & text, Align xalign=ALIGN_CENTER, Align yalign=ALIGN_CENTER);
    explicit Text_impl(Buffer buf, Align xalign=ALIGN_CENTER, Align yalign=ALIGN_CENTER);

    // Overriden by Edit_impl.
    virtual void assign(const ustring & text);

    // Overriden by Edit_impl.
    virtual void assign(Buffer buf);

    bool empty() const;
    ustring text() const;
    void clear();

    void allow_select();
    void disallow_select();
    bool select_allowed() const { return select_allowed_; }

    bool has_selection() const;
    ustring selection() const;
    void select(Buffer_citer b, Buffer_citer e);
    void select_all();
    void unselect();

    void set_spacing(unsigned spc);
    void set_text_align(Align halign, Align valign);
    Size text_size(const ustring & s);
    Size text_size(const std::u32string & s);

    void move_to(const Buffer_citer pos);
    void move_to(std::size_t row, std::size_t col);
    Buffer_citer caret() const;
    Buffer_citer iter(std::size_t row, std::size_t col) const;
    std::size_t lines() const;

    void enable_caret();
    void disable_caret();

    int x_at_col(std::size_t ri, std::size_t col) const;
    std::size_t col_at_x(std::size_t ri, int x) const;
    std::size_t row_at_y(int y) const;
    int baseline(std::size_t ri) const;
    void get_row_bounds(std::size_t ri, int & top, int & bottom) const;

    Buffer buffer() { return buffer_; }
    const Buffer & buffer() const { return buffer_; }
    int spacing() const { return spacing_; }
    Align horizontal_text_align() const { return xalign_; }
    Align vertical_text_align() const { return yalign_; }
    void set_wrap_mode(Wrap_mode wrap_mode);
    Wrap_mode wrap_mode() const { return wrap_; }
    bool caret_enabled() const { return caret_enabled_; }

    Action & move_left_action() { return move_left_action_; }
    Action & select_left_action() { return select_left_action_; }
    Action & move_right_action() { return move_right_action_; }
    Action & select_right_action() { return select_right_action_; }
    Action & move_up_action() { return move_up_action_; }
    Action & select_up_action() { return select_up_action_; }
    Action & move_down_action() { return move_down_action_; }
    Action & select_down_action() { return select_down_action_; }
    Action & move_word_left_action() { return move_word_left_action_; }
    Action & select_word_left_action() { return select_word_left_action_; }
    Action & move_word_right_action() { return move_word_right_action_; }
    Action & select_word_right_action() { return select_word_right_action_; }
    Action & move_home_action() { return move_home_action_; }
    Action & select_home_action() { return select_home_action_; }
    Action & move_to_eol_action() { return move_to_eol_action_; }
    Action & select_to_eol_action() { return select_to_eol_action_; }
    Action & move_to_sof_action() { return move_to_sof_action_; }
    Action & select_to_sof_action() { return select_to_sof_action_; }
    Action & move_to_eof_action() { return move_to_eof_action_; }
    Action & select_to_eof_action() { return select_to_eof_action_; }
    Action & move_page_up_action() { return move_page_up_action_; }
    Action & move_page_down_action() { return move_page_down_action_; }
    Action & select_page_up_action() { return select_page_up_action_; }
    Action & select_page_down_action() { return select_page_down_action_; }
    Action & select_all_action() { return select_all_action_; }
    Action & copy_action() { return copy_action_; }
    Action & cancel_action() { return cancel_action_; }

    signal<void()> & signal_caret_motion() { return signal_caret_motion_; }
    signal<void()> & signal_selection_changed() { return signal_selection_changed_; }
    signal<void()> & signal_click() { return signal_click_; }

protected:

    Buffer              buffer_;
    Buffer_citer         caret_;                 // Current caret position.
    Buffer_citer         sel_;                   // Selection start.
    Buffer_citer         esel_;                  // Selection end.

    bool                insert_ = true;
    bool                select_allowed_ = false;
    bool                caret_enabled_ = false;

    Action              move_left_action_ { KC_LEFT, KM_NONE, fun(this, &Text_impl::move_left) };
    Action              select_left_action_ { KC_LEFT, KM_SHIFT, fun(this, &Text_impl::select_left) };
    Action              move_right_action_ { KC_RIGHT, KM_NONE, fun(this, &Text_impl::move_right) };
    Action              select_right_action_ { KC_RIGHT, KM_SHIFT, fun(this, &Text_impl::select_right) };
    Action              move_up_action_ { KC_UP, KM_NONE, fun(this, &Text_impl::move_up) };
    Action              select_up_action_ { KC_UP, KM_SHIFT, fun(this, &Text_impl::select_up) };
    Action              move_down_action_ { KC_DOWN, KM_NONE, fun(this, &Text_impl::move_down) };
    Action              select_down_action_ { KC_DOWN, KM_SHIFT, fun(this, &Text_impl::select_down) };
    Action              move_word_left_action_ { KC_LEFT, KM_CONTROL, fun(this, &Text_impl::move_word_left) };
    Action              select_word_left_action_ { KC_LEFT, KM_CONTROL+KM_SHIFT, fun(this, &Text_impl::select_word_left) };
    Action              move_word_right_action_ { KC_RIGHT, KM_CONTROL, fun(this, &Text_impl::move_word_right) };
    Action              select_word_right_action_ { KC_RIGHT, KM_CONTROL+KM_SHIFT, fun(this, &Text_impl::select_word_right) };
    Action              move_home_action_ { KC_HOME, KM_NONE, fun(this, &Text_impl::move_home) };
    Action              select_home_action_ { KC_HOME, KM_SHIFT, fun(this, &Text_impl::select_home) };
    Action              move_to_eol_action_ { KC_END, KM_NONE, fun(this, &Text_impl::move_to_eol) };
    Action              select_to_eol_action_ { KC_END, KM_SHIFT, fun(this, &Text_impl::select_to_eol) };
    Action              move_to_sof_action_ { KC_HOME, KM_CONTROL, fun(this, &Text_impl::move_to_sof) };
    Action              select_to_sof_action_ { KC_HOME, KM_CONTROL+KM_SHIFT, fun(this, &Text_impl::select_to_sof) };
    Action              move_to_eof_action_ { KC_END, KM_CONTROL, fun(this, &Text_impl::move_to_eof) };
    Action              select_to_eof_action_ { KC_END, KM_CONTROL+KM_SHIFT, fun(this, &Text_impl::select_to_eof) };
    Action              move_page_up_action_ { KC_PAGE_UP, KM_NONE, fun(this, &Text_impl::move_page_up) };
    Action              select_page_up_action_ { KC_PAGE_UP, KM_SHIFT, fun(this, &Text_impl::select_page_up) };
    Action              move_page_down_action_ { KC_PAGE_DOWN, KM_NONE, fun(this, &Text_impl::move_page_down) };
    Action              select_page_down_action_ { KC_PAGE_DOWN, KM_SHIFT, fun(this, &Text_impl::select_page_down) };
    Action              select_all_action_ { U'A', KM_CONTROL, fun(this, &Text_impl::select_all) };
    Action              copy_action_ { "<Ctrl>C <Ctrl>Insert", fun(this, &Text_impl::copy) };
    Action              cancel_action_ { "Escape Cancel", fun(this, &Widget_impl::drop_focus) };

    signal<void()>      signal_selection_changed_;
    signal<void()>      signal_caret_motion_;
    signal<void()>      signal_click_;

protected:

    void show_caret();
    void hide_caret();
    void move_caret(Buffer_citer i);
    void move_caret(std::size_t row, std::size_t col);
    void scroll_to_caret();
    void hint_x();

    void pan_up();
    void pan_down();
    void left();
    void move_left();
    void select_left();
    void right();
    void move_right();
    void select_right();
    void up();
    void move_up();
    void select_up();
    void down();
    void move_down();
    void select_down();
    void move_word_left();
    void select_word_left();
    void move_word_right();
    void select_word_right();
    void home();
    void move_home();
    void select_home();
    void move_to_sof();
    void select_to_sof();
    void move_to_eol();
    void select_to_eol();
    void move_to_eof();
    void select_to_eof();
    void page_up();
    void move_page_up();
    void select_page_up();
    void page_down();
    void move_page_down();
    void select_page_down();
    void copy();

private:

    struct Frag {
        std::size_t     start  = 0;
        std::size_t     ncols  = 0;
        int             width  = 0;
        int             font = 0;
    };

    // Fragments.
    using Frags = std::list<Frag>;

    // Character positions (in pixels).
    using Poss  = std::vector<int>;

    // Line structure.
    struct Line {
        std::size_t     ncols   = 0;            // Number of columns (characters).
        int             width   = 0;            // Width in pixels.
        int             ascent  = 0;            // Ascent in pixels.
        int             descent = 0;            // Descent in pixels.
        int             ybase   = 0;            // Baseline within entire widget area.
        int             ox      = 0;            // Offset within X coordinate.
        std::u32string  text;
        std::u32string  ellipsized;
        Frags           frags;
        Poss            poss;
    };

    using Lines = std::vector<Line>;

    Lines               lines_;
    Buffer_citer        msel_;                  // Mouse selection start.
    Buffer_citer        emsel_;                 // Mouse selection last.
    bool                caret_visible_ = false;
    bool                caret_exposed_ = false;

    Painter             pr_;
    std::vector<Font>   fonts_;
    int                 xhint_ = 0;             // Desired x offset for up/down caret navigation.
    int                 font_height_ = 0;
    int                 text_width_ = 0;
    int                 tab_width_ = 8;
    int                 space_width_ = 0;
    int                 ellipsis_width_ = 0;
    int                 text_height_ = 0;
    int                 oy_ = 0;                // Vertical offset.
    Rect                va_;                    // Visible area.
    Rect                rcaret_;                // Caret rectangle.
    Color               ccaret_;                // Caret color.
    Align               xalign_ = ALIGN_CENTER;
    Align               yalign_ = ALIGN_CENTER;
    Wrap_mode           wrap_ = WRAP_NONE;
    std::u32string      ellipsis_ = U"...";// U"…";
    std::size_t         spacing_ = 0;

    Timer               caret_timer_;
    connection          insert_cx_;
    connection          replace_cx_;
    connection          erase_cx_;
    connection          insert_move_cx_;
    connection          replace_move_cx_;
    connection          erase_move_cx_;

private:

    void init();
    void init_buffer();
    void draw_caret(Painter pr);
    void expose_caret();
    void wipe_caret();
    void refresh_caret();

    Buffer_citer iter_from_point(const Point & pt);
    int x_at_col(const Line & row, std::size_t col) const;
    std::size_t col_at_x(const Line & row, int x) const;
    std::size_t hinted_pos(std::size_t ri);
    void arrange_lines();
    int  calc_height(std::size_t first, std::size_t last);
    int  calc_width(std::size_t first, std::size_t last);
    void calc_line(Line & line);
    void calc_ellipsis(Line & line);
    void calc_all_ellipsis();
    bool align_lines(std::size_t first, std::size_t last);
    void align_all();
    void load_lines(std::size_t first, std::size_t last);
    void translate_lines(std::size_t first, std::size_t last, int dy);
    void insert_range(Buffer_citer b, Buffer_citer e);
    void paint_line(const Line & line, std::size_t ln, std::size_t pos, Painter pr);
    void paint_ellipsized(const Line & line, Painter pr);
    void redraw(const Rect & r, Painter pr=Painter());
    Painter wipe_area(int x1, int y1, int x2, int y2, Painter pr=Painter());
    Painter priv_painter();
    Font select_font(Painter pr=Painter());

    void update_caret();
    void update_painter();
    void update_font();
    void update_range(Buffer_citer begin, Buffer_citer end);
    void update_requisition();
    void update_selection(Buffer_citer i, Buffer_citer j);
    void update_va();

    bool on_paint(Painter pr, const Rect & inval);
    bool on_mouse_down(int mbt, int mm, const Point & pt);
    bool on_mouse_up(int mbt, int mm, const Point & pt);
    void on_mouse_motion(int mm, const Point & pt);
    void on_focus_in();
    void on_caret_timer();
    bool on_take_focus();

    void on_buffer_insert(Buffer_citer b, Buffer_citer e);
    void on_buffer_replace(Buffer_citer b, Buffer_citer e, const std::u32string & replaced);
    void on_buffer_erase(Buffer_citer b, Buffer_citer e, const std::u32string & erased);

    void on_buffer_insert_move(Buffer_citer b, Buffer_citer e);
    void on_buffer_replace_move(Buffer_citer b, Buffer_citer e, const std::u32string & replaced);
    void on_buffer_erase_move(Buffer_citer b, Buffer_citer e, const std::u32string & erased);
};

} // namespace tau

#endif // TAU_TEXT_IMPL_HH
