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

#ifndef TAU_LOCALE_IMPL_HH
#define TAU_LOCALE_IMPL_HH

#include <tau/locale.hh>

struct Locale_static {
    const char *        code;
    const char *        int_curr_symbol;
    const char *        currency_symbol;
    const char *        mon_decimal_point;
    const char *        mon_thousands_sep;
    const char *        mon_grouping;
    const char *        positive_sign;
    const char *        negative_sign;
    int                 int_frac_digits;
    int                 frac_digits;
    int                 p_cs_precedes;
    int                 p_sep_by_space;
    int                 n_cs_precedes;
    int                 n_sep_by_space;
    int                 p_sign_posn;
    int                 n_sign_posn;
    const char *        decimal_point;
    const char *        thousands_sep;
    const char *        grouping;
    const char *        abday;
    const char *        day;
    const char *        abmon;
    const char *        mon;
    const char *        d_t_fmt;
    const char *        d_fmt;
    const char *        t_fmt;
    const char *        am_pm;
    const char *        t_fmt_ampm;
    const char *        date_fmt;
    int                 first_weekday;
};

namespace tau {

struct Locale_data {
    std::string         spec;
    Language            lang;
    Territory           terr;
    Encoding            enc;    // System encoding.
    Encoding            iocharset;   // Encoding used for file names.
    std::string         mod;
    const Locale_static * sdata;
};

extern Locale_data * sys_locale_ptr_;
extern Locale_data * locale_ptr_;

} // namespace tau

#endif // TAU_LOCALE_IMPL_HH
