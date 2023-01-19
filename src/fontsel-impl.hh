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

#ifndef TAU_FONT_SELECTOR_IMPL_HH
#define TAU_FONT_SELECTOR_IMPL_HH

#include <tau/action.hh>
#include <table-impl.hh>

namespace tau {

class Fontsel_impl: public Table_impl {
public:

    Fontsel_impl();
    explicit Fontsel_impl(const ustring & spec, const ustring & sample=ustring());

    void select(const ustring & spec);
    ustring spec() const;

    void set_sample(const ustring & sample);
    ustring sample() const { return sample_; }

    Action & focus_next_action() { return next_; }
    Action & focus_previous_action() { return prev_; }
    Action & cancel_action() { return cancel_; }
    signal<void(const ustring &)> & signal_font_selected() { return signal_font_selected_; }
    signal<void(const ustring &)> & signal_font_activated() { return signal_font_activated_; }

private:

    List_text_ptr   families_;
    List_text_ptr   faces_;
    Counter_ptr     counter_;
    Text_ptr        fontspec_;
    Text_ptr        psname_;
    Entry_ptr       entry_;
    Button_ptr      apply_;
    unsigned        hsample_ = 0;
    ustring         sample_;    // User defined sample string.
    ustring         family_;
    ustring         face_;
    ustring         aspec_;     // Spec which was applied.
    ustring         uspec_;     // Font specification defined by user.
    ustring         uface_;     // Font face defined by user.

    Action          zin_    { ustring("<Ctrl>= <Ctrl>+") }; // Increase font size action.
    Action          zout_   { ustring("<Ctrl>-") }; // Decrease size action.
    Action          next_   { "Tab", fun(this, &Fontsel_impl::focus_next) }; // Focus next action.
    Action          prev_   { "<Shift>Tab <Shift>LeftTab", fun(this, &Fontsel_impl::focus_previous) }; // Focus previous action.
    Action          cancel_ { "Escape Cancel", "Cancel", ustring("dialog-cancel") };

    signal<void(const ustring &)> signal_font_selected_;
    signal<void(const ustring &)> signal_font_activated_;

private:

    void init();
    void update_font();
    void update_apply();
    void update_tooltips();

    void on_display();
    void on_family_selected(int row, const ustring & str);
    void on_family_activated(int row, const ustring & str);
    void on_face_selected(int row, const ustring & str);
    void on_face_activated(int row, const ustring & str);
    void on_counter_value_changed(double value);
    void on_sample_requisition_changed();
    void on_apply_clicked();
    void on_zin();
    void on_zout();
    void on_entry_changed(const ustring & s);
    void on_entry_activate(const ustring & s);
    void focus_next();
    void focus_previous();
};

using Fontsel_ptr = std::shared_ptr<Fontsel_impl>;
using Fontsel_cptr = std::shared_ptr<const Fontsel_impl>;

} // namespace tau

#endif // TAU_FONT_SELECTOR_IMPL_HH
