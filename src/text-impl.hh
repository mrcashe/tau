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

   ~Text_impl();

    void assign(Buffer buf);
    void assign(const ustring & text);
    bool empty() const;
    ustring text() const;

    // Overriden by Edit_impl.
    virtual void clear();

    void allow_select();
    void disallow_select();
    bool select_allowed() const { return select_allowed_; }

    bool has_selection() const;
    ustring selection() const;
    void select(Buffer_citer b, Buffer_citer e);
    void select_all();
    void unselect();

    void set_spacing(unsigned spc);
    void set_text_align(Align xalign, Align yalign);

    void get_text_align(Align & xalign, Align & yalign) {
        xalign = xalign_;
        yalign = yalign_;
    }

    Size text_size(const ustring & s);
    Size text_size(const std::u32string & s);

    void move_to(const Buffer_citer pos);
    void move_to(std::size_t row, std::size_t col);
    Buffer_citer caret() const;
    Buffer_citer iter(std::size_t row, std::size_t col) const;
    std::size_t rows() const;

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
    void set_wrap_mode(Wrap_mode wrap_mode);
    Wrap_mode wrap_mode() const { return wrap_; }
    bool caret_enabled() const { return caret_enabled_; }

    Action & move_left_action();
    Action & select_left_action();
    Action & move_right_action();
    Action & select_right_action();
    Action & move_up_action();
    Action & select_up_action();
    Action & move_down_action();
    Action & select_down_action();
    Action & move_word_left_action();
    Action & select_word_left_action();
    Action & move_word_right_action();
    Action & select_word_right_action();
    Action & move_home_action();
    Action & select_home_action();
    Action & move_to_eol_action();
    Action & select_to_eol_action();
    Action & move_to_sof_action();
    Action & select_to_sof_action();
    Action & move_to_eof_action();
    Action & select_to_eof_action();
    Action & move_page_up_action();
    Action & move_page_down_action();
    Action & select_page_up_action();
    Action & select_page_down_action();
    Action & select_all_action();
    Action & copy_action();
    Action & cancel_action();

    signal<void()> & signal_caret_motion();
    signal<void()> & signal_selection_changed();
    signal<void()> & signal_click();

protected:

    Buffer              buffer_;
    Buffer_citer        caret_;                 // Current caret position.
    Buffer_citer        sel_;                   // Selection start.
    Buffer_citer        esel_;                  // Selection end.
    Painter             ppr_;                   // Private painter.
    bool                insert_ = true;

protected:

    void scroll_to_caret();
    void hint_x();
    void init_actions();
    void refresh_caret();

    // Overriden by Edit_impl.
    virtual void init_buffer();

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

    struct Actions {
        Action          move_left_action_ { KC_LEFT, KM_NONE };
        Action          select_left_action_ { KC_LEFT, KM_SHIFT };
        Action          move_right_action_ { KC_RIGHT, KM_NONE };
        Action          select_right_action_ { KC_RIGHT, KM_SHIFT };
        Action          move_up_action_ { KC_UP, KM_NONE };
        Action          select_up_action_ { KC_UP, KM_SHIFT };
        Action          move_down_action_ { KC_DOWN, KM_NONE };
        Action          select_down_action_ { KC_DOWN, KM_SHIFT };
        Action          move_word_left_action_ { KC_LEFT, KM_CONTROL };
        Action          select_word_left_action_ { KC_LEFT, KM_CONTROL+KM_SHIFT };
        Action          move_word_right_action_ { KC_RIGHT, KM_CONTROL };
        Action          select_word_right_action_ { KC_RIGHT, KM_CONTROL+KM_SHIFT };
        Action          move_home_action_ { KC_HOME, KM_NONE };
        Action          select_home_action_ { KC_HOME, KM_SHIFT };
        Action          move_to_eol_action_ { KC_END, KM_NONE };
        Action          select_to_eol_action_ { KC_END, KM_SHIFT };
        Action          move_to_sof_action_ { KC_HOME, KM_CONTROL };
        Action          select_to_sof_action_ { KC_HOME, KM_CONTROL+KM_SHIFT };
        Action          move_to_eof_action_ { KC_END, KM_CONTROL };
        Action          select_to_eof_action_ { KC_END, KM_CONTROL+KM_SHIFT };
        Action          move_page_up_action_ { KC_PAGE_UP, KM_NONE };
        Action          select_page_up_action_ { KC_PAGE_UP, KM_SHIFT };
        Action          move_page_down_action_ { KC_PAGE_DOWN, KM_NONE };
        Action          select_page_down_action_ { KC_PAGE_DOWN, KM_SHIFT };
        Action          select_all_action_ { U'A', KM_CONTROL };
        Action          copy_action_ { ustring("<Ctrl>C <Ctrl>Insert") };
        Action          cancel_action_ { ustring("Escape Cancel") };
    };

    Actions *           actions_ = nullptr;
    signal<void()> *    signal_selection_changed_ = nullptr;
    signal<void()> *    signal_caret_motion_ = nullptr;
    signal<void()> *    signal_click_ = nullptr;

    struct Frag {
        std::size_t     start_      = 0;
        std::size_t     ncols_      = 0;
        int             width_      = 0;
        int             font_       = 0;
    };

    // Fragments.
    using Frags = std::list<Frag>;

    // Character positions (in pixels).
    using Poss  = std::vector<int>;

    // Row structure.
    struct Row {
        std::size_t     ncols_      = 0;            // Number of columns (characters).
        int             width_      = 0;            // Width in pixels.
        int             ascent_     = 0;            // Ascent in pixels.
        int             descent_    = 0;            // Descent in pixels.
        int             ybase_      = 0;            // Baseline within entire widget area.
        int             ox_         = 0;            // Offset within X coordinate.
        std::u32string  ellipsized_;
        Frags           frags_;
        Poss            poss_;
    };

    using Rows = std::vector<Row>;
    using R_iter = Rows::iterator;
    using R_citer = Rows::const_iterator;

    Loop_ptr            loop_;
    Rows                rows_;
    Buffer_citer        msel_;                      // Mouse selection start.
    Buffer_citer        emsel_;                     // Mouse selection last.
    bool                caret_visible_:  1;
    bool                caret_exposed_:  1;
    bool                caret_refresh_:  1;
    bool                caret_enabled_:  1;
    bool                select_allowed_: 1;

    std::vector<Font>   fonts_;
    int                 xhint_ = 0;                 // Desired x offset for up/down caret navigation.
    int                 font_height_ = 0;
    int                 text_width_ = 0;
    int                 tab_width_ = 8;
    int                 space_width_ = 0;
    int                 ellipsis_width_ = 0;
    int                 text_height_ = 0;
    int                 oy_ = 0;                    // Vertical offset.
    Rect                va_;                        // Visible area.
    Rect                rcaret_;                    // Caret rectangle.
    Color               ccaret_;                    // Caret color.
    Align               xalign_ = ALIGN_CENTER;
    Align               yalign_ = ALIGN_CENTER;
    Wrap_mode           wrap_ = WRAP_NONE;
    std::u32string      ellipsis_ = U"...";// U"…";
    std::size_t         spacing_ = 0;

    connection          caret_cx_ { true };
    connection          insert_cx_ { true };
    connection          replace_cx_ { true };
    connection          erase_cx_ { true };
    connection          insert_move_cx_ { true };
    connection          replace_move_cx_ { true };
    connection          erase_move_cx_ { true };
    connection          mouse_down_cx_ { true };
    connection          mouse_up_cx_ { true };
    connection          mouse_motion_cx_ { true };

private:

    void init();
    void draw_caret(Painter pr);
    void expose_caret();
    void wipe_caret();
    void show_caret();
    void hide_caret();

    Buffer_citer iter_from_point(const Point & pt);
    int x_at_col(const Row & row, std::size_t col) const;
    std::size_t col_at_x(const Row & row, int x) const;
    std::size_t hinted_pos(std::size_t ri);
    int  calc_height(R_citer first, R_citer last);
    int  calc_width(R_citer first, R_citer last);
    void calc_row(R_iter i, Painter pr=Painter());
    void calc_rows();
    bool align_rows(R_iter first, R_iter last);
    void align_all();
    void load_rows(R_iter first, R_iter last);
    void translate_rows(R_iter first, R_iter last, int dy);
    void insert_range(Buffer_citer b, Buffer_citer e);
    void paint_row(R_citer ri, std::size_t pos, Painter pr);
    void paint_ellipsized(const Row & row, Painter pr);
    void redraw(const Rect & r, Painter pr=Painter());
    Painter wipe_area(int x1, int y1, int x2, int y2, Painter pr=Painter());
    Painter priv_painter();
    Font select_font(Painter pr=Painter());

    void update_caret();
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

    void on_display();
};

} // namespace tau

#endif // TAU_TEXT_IMPL_HH
