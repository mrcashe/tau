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

#include <tau/exception.hh>
#include <tau/font.hh>
#include <tau/language.hh>
#include <tau/painter.hh>
#include <counter-impl.hh>
#include <button-impl.hh>
#include <entry-impl.hh>
#include <frame-impl.hh>
#include <fontsel-impl.hh>
#include <list-text-impl.hh>
#include <theme-impl.hh>
#include <iostream>

namespace tau {

Fontsel_impl::Fontsel_impl():
    Table_impl()
{
    init();
}

Fontsel_impl::Fontsel_impl(const ustring & spec, const ustring & sample):
    Table_impl(),
    sample_(sample)
{
    init();
    select(spec);
}

void Fontsel_impl::init() {
    set_column_spacing(4);
    set_row_spacing(4);
    set_column_margin(4, 6, 0);

    Frame_ptr frame = std::make_shared<Frame_impl>("Family:", ALIGN_CENTER, BORDER_RIDGE);
    families_ = std::make_shared<List_text_impl>();
    families_->signal_text_selected().connect(fun(this, &Fontsel_impl::on_family_selected));
    families_->signal_text_activated().connect(fun(this, &Fontsel_impl::on_family_activated));
    frame->insert(families_);
    families_->hint_min_size(0, 192);
    put(frame, 0, 0, 3, 2);

    frame = std::make_shared<Frame_impl>("Face:", ALIGN_CENTER, BORDER_RIDGE);
    faces_ = std::make_shared<List_text_impl>();
    faces_->signal_text_selected().connect(fun(this, &Fontsel_impl::on_face_selected));
    faces_->signal_text_activated().connect(fun(this, &Fontsel_impl::on_face_activated));
    frame->insert(faces_);
    put(frame, 3, 0, 2, 1);

    counter_ = std::make_shared<Counter_impl>(10, 200, 1);
    counter_->prepend("Size:", 2, 4);
    counter_->append("pt", 4, 2);
    counter_->signal_value_changed().connect(fun(this, &Fontsel_impl::on_counter_value_changed));
    put(counter_, 3, 1, 2, 1, false, true);

    frame = std::make_shared<Frame_impl>("Sample:", ALIGN_CENTER, BORDER_RIDGE);
    frame->hint_margin(0, 0, 4, 4);
    put(frame, 0, 2, 5, 1, false, true);

    entry_ = std::make_shared<Entry_impl>(ALIGN_CENTER, BORDER_NONE);
    entry_->hint_margin(3);
    entry_->signal_requisition_changed().connect(fun(this, &Fontsel_impl::on_sample_requisition_changed), true);
    entry_->hint_min_size(0, 52);
    entry_->hint_max_size(0, 122);
    entry_->signal_activate().connect(fun(this, &Fontsel_impl::on_entry_activate));
    entry_->signal_focus_in().connect(fun(entry_, &Entry_impl::select_all));
    entry_->signal_focus_out().connect(fun(entry_, &Entry_impl::unselect));
    entry_->signal_changed().connect(fun(this, &Fontsel_impl::on_entry_changed));
    entry_->assign(sample_.empty() ? Language().sample() : sample_);
    entry_->set_tooltip("Click on the sample text to edit it");
    frame->insert(entry_);

    Text_ptr tp = std::make_shared<Text_impl>("Font name:", ALIGN_START);
    put(tp, 0, 3, 1, 1, false, true);
    fontspec_ = std::make_shared<Text_impl>(ALIGN_END);
    put(fontspec_, 1, 3, 3, 1, false, true);

    tp = std::make_shared<Text_impl>("PostScript name:", ALIGN_START);
    put(tp, 0, 4, 1, 1, false, true);
    psname_ = std::make_shared<Text_impl>(ALIGN_END);
    put(psname_, 1, 4, 3, 1, false, true);

    apply_ = std::make_shared<Button_impl>("Apply", "dialog-ok", MEDIUM_ICON);
    apply_->signal_click().connect(fun(this, &Fontsel_impl::on_apply_clicked));
    apply_->disable();
    put(apply_, 4, 3, 1, 1, true, true);
    align(apply_.get(), ALIGN_FILL, ALIGN_FILL);

    auto btn = std::make_shared<Button_impl>(cancel_);
    put(btn, 4, 4, 1, 1, true, true);
    align(btn.get(), ALIGN_FILL, ALIGN_FILL);

    auto theme = Theme_impl::root();
    auto pa = theme->find_action(FOCUS_NEXT_ACTION);
    if (pa) { next_.connect_master_action(*pa); }

    pa = theme->find_action(FOCUS_PREVIOUS_ACTION);
    if (pa) { prev_.connect_master_action(*pa); }

    zin_.connect(fun(counter_, &Counter_impl::increase));
    zout_.connect(fun(counter_, &Counter_impl::decrease));
    cancel_.connect(fun(this, &Widget_impl::quit_dialog));

    connect_action(zin_);
    connect_action(zout_);
    connect_action(cancel_);
    connect_action(next_);
    connect_action(prev_);

    signal_display().connect(fun(this, &Fontsel_impl::on_display));
    signal_take_focus().connect(fun(families_, &Widget_impl::take_focus), true);
}

void Fontsel_impl::select(const ustring & spec) {
    uspec_ = spec;
    uface_ = font_face_from_spec(spec);
    ustring family = font_family_from_spec(spec);
    ustring face = font_face_from_spec(spec);
    double pt = font_size_from_spec(spec);
    if (pt < 1) { pt = font_size_from_spec(Font::normal()); }
    if (!families_->contains(family)) { family = font_family_from_spec(Font::normal()); }
    families_->select(family);
    if (!faces_->contains(face)) { face = font_face_from_spec(Font::normal()); }
    faces_->select(face);
    counter_->set_value(pt);
    update_font();
}

ustring Fontsel_impl::spec() const {
    return font_spec_build(family_, face_, counter_->value());
}

void Fontsel_impl::set_sample(const ustring & sample) {
    sample_ = sample;
    if (!sample_.empty()) { entry_->assign(sample_); }
}

void Fontsel_impl::on_display() {
    auto v = Font::list_families();
    std::sort(v.begin(), v.end());
    for (auto & s: v) { families_->append(s); }
    ustring s = uspec_.empty() ? Font::normal() : uspec_;
    aspec_ = s;
    select(s);
    update_tooltips();
}

// An exception may be thrown during this call!
void Fontsel_impl::update_font() {
    try {
        if (!family_.empty() && !face_.empty() && counter_->value() >= 1) {
            ustring spc = font_spec_build(family_, face_, counter_->value());
            fontspec_->assign(spc);

            if (Painter pr = entry_->painter()) {
                Font font = pr.select_font(spc);
                ustring psname = font.psname();
                psname_->assign(psname.empty() ? "Not available" : psname);
            }

            entry_->style().font("font") = spc;
        }
    }

    catch (exception & x) {
        std::cerr << "** Fontsel_impl::update_font(): " << x.what() << std::endl;
    }
}

void Fontsel_impl::on_face_selected(int row, const ustring & str) {
    face_ = str;
    update_apply();
    update_font();
    signal_font_selected_(spec());
}

void Fontsel_impl::on_face_activated(int row, const ustring & str) {
    face_ = str;
    ustring s = spec();

    if (!s.empty()) {
        aspec_ = s;
        update_apply();
        signal_font_activated_(s);
    }
}

void Fontsel_impl::on_family_selected(int row, const ustring & str) {
    family_ = str;
    ustring face = faces_->selection();
    faces_->clear_list();
    auto v = Font::list_faces(str);
    std::sort(v.begin(), v.end());
    for (auto & s: v) { faces_->append(s); }
    if (str_similar(face, v)) { faces_->select(face, true); }
    else if (str_similar(uface_, v)) { faces_->select(uface_, true); }
    else { faces_->select_first(); }
}

void Fontsel_impl::on_family_activated(int row, const ustring & str) {
    family_ = str;
    ustring s = spec();

    if (!s.empty()) {
        aspec_ = s;
        update_font();
        update_apply();
        signal_font_activated_(s);
    }
}

void Fontsel_impl::on_counter_value_changed(double value) {
    hsample_ = 0;
    entry_->hint_size(0, 0);

    if (!spec().empty()) {
        update_font();
        update_apply();
    }
}

void Fontsel_impl::on_apply_clicked() {
    ustring s = spec();

    if (!s.empty()) {
        update_apply();
        signal_font_activated_(s);
    }
}

void Fontsel_impl::on_sample_requisition_changed() {
    std::size_t h = entry_->required_size().height();
    if (0 != hsample_ && h > hsample_) { entry_->hint_size(0, h); }
    hsample_ = h;
}

void Fontsel_impl::update_tooltips() {
    auto table = std::make_shared<Table_impl>();
    table->set_column_spacing(8);
    table->hint_margin(4, 4, 2, 2);
    table->align_column(0, ALIGN_START);
    table->align_column(1, ALIGN_END);
    ustring spc = font_size_change(style().font("font").spec(), 7);

    auto tp = std::make_shared<Text_impl>("++size:", ALIGN_END);
    table->put(tp, 0, 0, 1, 1, false, true);
    tp->style().font("font") = spc;

    tp = std::make_shared<Text_impl>("--size:", ALIGN_END);
    table->put(tp, 0, 1, 1, 1, false, true);
    tp->style().font("font") = spc;

    ustring iaccels, oaccels;

    for (auto & accel: zin_.accels()) {
        if (!iaccels.empty()) { iaccels += ' '; }
        iaccels += accel.label();
    }

    for (auto & accel: zout_.accels()) {
        if (!oaccels.empty()) { oaccels += ' '; }
        oaccels += accel.label();
    }

    spc = font_set_face(spc, "Bold");
    tp = std::make_shared<Text_impl>(iaccels, ALIGN_END);
    table->put(tp, 1, 0, 1, 1, true, true);
    tp->style().font("font") = spc;

    tp = std::make_shared<Text_impl>(oaccels, ALIGN_END);
    table->put(tp, 1, 1, 1, 1, true, true);
    tp->style().font("font") = spc;

    counter_->set_tooltip(table);
}

void Fontsel_impl::update_apply() {
    ustring s = spec();

    if (!s.empty()) {
        if (s != aspec_) { apply_->enable(); }
        else { apply_->disable(); }
    }
}

void Fontsel_impl::on_entry_changed(const ustring & s) {
    sample_ = s;
}

void Fontsel_impl::focus_next() {
    if (has_focus()) {
        if (families_->has_focus()) { faces_->take_focus(); }
        else if (faces_->has_focus()) { counter_->take_focus(); }
        else { families_->take_focus(); }
    }
}

void Fontsel_impl::focus_previous() {
    if (has_focus()) {
        if (counter_->has_focus()) { faces_->take_focus(); }
        else if (families_->has_focus()) { counter_->take_focus(); }
        else { families_->take_focus(); }
    }
}

void Fontsel_impl::on_entry_activate(const ustring & s) {
    families_->take_focus();
}

} // namespace tau

//END
