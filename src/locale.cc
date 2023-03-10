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

#include <tau/encoding.hh>
#include <tau/exception.hh>
#include <tau/sys.hh>
#include <locale-impl.hh>
#include <iostream>

namespace {

const char * de_DE_abday =      "\u0053\u006F:"
                                "\u004D\u006F:"
                                "\u0044\u0069:"
                                "\u004D\u0069:"
                                "\u0044\u006F:"
                                "\u0046\u0072:"
                                "\u0053\u0061";

const char * de_DE_day =        "\u0053\u006F\u006E\u006E\u0074\u0061\u0067:"
                                "\u004D\u006F\u006E\u0074\u0061\u0067:"
                                "\u0044\u0069\u0065\u006E\u0073\u0074\u0061\u0067:"
                                "\u004D\u0069\u0074\u0074\u0077\u006F\u0063\u0068:"
                                "\u0044\u006F\u006E\u006E\u0065\u0072\u0073\u0074\u0061\u0067:"
                                "\u0046\u0072\u0065\u0069\u0074\u0061\u0067:"
                                "\u0053\u0061\u006D\u0073\u0074\u0061\u0067";

const char * de_DE_abmon =      "\u004A\u0061\u006E:"
                                "\u0046\u0065\u0062:"
                                "\u004D\u00E4\u0072:"
                                "\u0041\u0070\u0072:"
                                "\u004D\u0061\u0069:"
                                "\u004A\u0075\u006E:"
                                "\u004A\u0075\u006C:"
                                "\u0041\u0075\u0067:"
                                "\u0053\u0065\u0070:"
                                "\u004F\u006B\u0074:"
                                "\u004E\u006F\u0076:"
                                "\u0044\u0065\u007A";

const char * de_DE_mon =        "\u004A\u0061\u006E\u0075\u0061\u0072:"
                                "\u0046\u0065\u0062\u0072\u0075\u0061\u0072:"
                                "\u004D\u00E4\u0072\u007A:"
                                "\u0041\u0070\u0072\u0069\u006C:"
                                "\u004D\u0061\u0069:"
                                "\u004A\u0075\u006E\u0069:"
                                "\u004A\u0075\u006C\u0069:"
                                "\u0041\u0075\u0067\u0075\u0073\u0074:"
                                "\u0053\u0065\u0070\u0074\u0065\u006D\u0062\u0065\u0072:"
                                "\u004F\u006B\u0074\u006F\u0062\u0065\u0072:"
                                "\u004E\u006F\u0076\u0065\u006D\u0062\u0065\u0072:"
                                "\u0044\u0065\u007A\u0065\u006D\u0062\u0065\u0072:";

const char * de_DE_d_t_fmt =    "\u0025\u0061\u0020\u0025\u0064\u0020\u0025\u0062\u0020\u0025\u0059\u0020\u0025\u0054\u0020\u0025\u005A";
const char * de_DE_d_fmt =      "\u0025\u0064\u002E\u0025\u006D\u002E\u0025\u0059";
const char * de_DE_t_fmt =      "\u0025\u0054";
const char * de_DE_date_fmt =   "\u0025\u0061\u0020\u0025\u002D\u0064\u002E\u0020"
                                "\u0025\u0062\u0020\u0025\u0048\u003A\u0025\u004D\u003A\u0025\u0053"
                                "\u0020\u0025\u005A\u0020\u0025\u0059";

const char * en_GB_abday =      "\u0053\u0075\u006E:"
                                "\u004D\u006F\u006E:"
                                "\u0054\u0075\u0065:"
                                "\u0057\u0065\u0064:"
                                "\u0054\u0068\u0075:"
                                "\u0046\u0072\u0069:"
                                "\u0053\u0061\u0074";

const char * en_GB_day =        "\u0053\u0075\u006E\u0064\u0061\u0079:"
                                "\u004D\u006F\u006E\u0064\u0061\u0079:"
                                "\u0054\u0075\u0065\u0073\u0064\u0061\u0079:"
                                "\u0057\u0065\u0064\u006E\u0065\u0073\u0064\u0061\u0079:"
                                "\u0054\u0068\u0075\u0072\u0073\u0064\u0061\u0079:"
                                "\u0046\u0072\u0069\u0064\u0061\u0079:"
                                "\u0053\u0061\u0074\u0075\u0072\u0064\u0061\u0079";

const char * en_GB_abmon =      "\u004A\u0061\u006E:"
                                "\u0046\u0065\u0062:"
                                "\u004D\u0061\u0072:"
                                "\u0041\u0070\u0072:"
                                "\u004D\u0061\u0079:"
                                "\u004A\u0075\u006E:"
                                "\u004A\u0075\u006C:"
                                "\u0041\u0075\u0067:"
                                "\u0053\u0065\u0070:"
                                "\u004F\u0063\u0074:"
                                "\u004E\u006F\u0076:"
                                "\u0044\u0065\u0063";

const char * en_GB_mon =        "\u004A\u0061\u006E\u0075\u0061\u0072\u0079:"
                                "\u0046\u0065\u0062\u0072\u0075\u0061\u0072\u0079:"
                                "\u004D\u0061\u0072\u0063\u0068:"
                                "\u0041\u0070\u0072\u0069\u006C:"
                                "\u004D\u0061\u0079:"
                                "\u004A\u0075\u006E\u0065:"
                                "\u004A\u0075\u006C\u0079:"
                                "\u0041\u0075\u0067\u0075\u0073\u0074:"
                                "\u0053\u0065\u0070\u0074\u0065\u006D\u0062\u0065\u0072:"
                                "\u004F\u0063\u0074\u006F\u0062\u0065\u0072:"
                                "\u004E\u006F\u0076\u0065\u006D\u0062\u0065\u0072:"
                                "\u0044\u0065\u0063\u0065\u006D\u0062\u0065\u0072";

const char * en_GB_d_t_fmt =    "\u0025\u0061\u0020\u0025\u0064\u0020\u0025\u0062\u0020\u0025\u0059\u0020\u0025\u0054\u0020\u0025\u005A";
const char * en_GB_d_fmt =      "\u0025\u0064\u002F\u0025\u006D\u002F\u0025\u0079";
const char * en_GB_t_fmt =      "\u0025\u0054";
const char * en_GB_am_pm =      "\u0061\u006D:\u0070\u006D";
const char * en_GB_t_fmt_ampm = "\u0025\u006C\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u0050\u0020\u0025\u005A";
const char * en_GB_date_fmt =   "\u0025\u0061\u0020\u0025\u0065\u0020\u0025\u0062"
                                "\u0020\u0025\u0048\u003A\u0025\u004D\u003A\u0025\u0053\u0020"
                                "\u0025\u005A\u0020\u0025\u0059";

const char * en_US_am_pm =      "\u0041\u004D:\u0050\u004D";

const char * ru_RU_abday =      "\u0412\u0441:"
                                "\u041f\u043d:"
                                "\u0412\u0442:"
                                "\u0421\u0440:"
                                "\u0427\u0442:"
                                "\u041f\u0442:"
                                "\u0421\u0431";

const char * ru_RU_day =        "\u0412\u043E\u0441\u043A\u0440\u0435\u0441\u0435\u043D\u044C\u0435:"
                                "\u041F\u043E\u043D\u0435\u0434\u0435\u043B\u044C\u043D\u0438\u043A:"
                                "\u0412\u0442\u043E\u0440\u043D\u0438\u043A:"
                                "\u0421\u0440\u0435\u0434\u0430:"
                                "\u0427\u0435\u0442\u0432\u0435\u0440\u0433:"
                                "\u041F\u044F\u0442\u043D\u0438\u0446\u0430:"
                                "\u0421\u0443\u0431\u0431\u043E\u0442\u0430";

const char * ru_RU_abmon =      "\u044F\u043D\u0432:"
                                "\u0444\u0435\u0432:"
                                "\u043C\u0430\u0440:"
                                "\u0430\u043F\u0440:"
                                "\u043C\u0430\u0439:"
                                "\u0438\u044E\u043D:"
                                "\u0438\u044E\u043B:"
                                "\u0430\u0432\u0433:"
                                "\u0441\u0435\u043D:"
                                "\u043E\u043A\u0442:"
                                "\u043D\u043E\u044F:"
                                "\u0434\u0435\u043A";

const char * ru_RU_mon =        "\u042F\u043D\u0432\u0430\u0440\u044C:"
                                "\u0424\u0435\u0432\u0440\u0430\u043B\u044C:"
                                "\u041C\u0430\u0440\u0442:"
                                "\u0410\u043F\u0440\u0435\u043B\u044C:"
                                "\u041C\u0430\u0439:"
                                "\u0418\u044E\u043D\u044C:"
                                "\u0418\u044E\u043B\u044C:"
                                "\u0410\u0432\u0433\u0443\u0441\u0442:"
                                "\u0421\u0435\u043D\u0442\u044F\u0431\u0440\u044C:"
                                "\u041E\u043A\u0442\u044F\u0431\u0440\u044C:"
                                "\u041D\u043E\u044F\u0431\u0440\u044C:"
                                "\u0414\u0435\u043A\u0430\u0431\u0440\u044C";

const char * ru_RU_d_t_fmt =    "\u0025\u0061\u0020\u0025\u0064\u0020\u0025\u0062\u0020\u0025\u0059\u0020\u0025\u0054";
const char * ru_RU_d_fmt =      "\u0025\u0064\u002E\u0025\u006D\u002E\u0025\u0059";
const char * ru_RU_t_fmt =      "\u0025\u0054";

const Locale_static sdata_[] = {
    {   .code                   = "C",
        .int_curr_symbol        = nullptr,
        .currency_symbol        = nullptr,
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = nullptr,
        .mon_grouping           = nullptr,
        .positive_sign          = nullptr,
        .negative_sign          = nullptr,
        .int_frac_digits        = -1,
        .frac_digits            = -1,
        .p_cs_precedes          = -1,
        .p_sep_by_space         = -1,
        .n_cs_precedes          = -1,
        .n_sep_by_space         = -1,
        .p_sign_posn            = -1,
        .n_sign_posn            = -1,
        .decimal_point          = "\u002E",
        .thousands_sep          = nullptr,
        .grouping               = nullptr,
        .abday                  = en_GB_abday,
        .day                    = en_GB_day,
        .abmon                  = en_GB_abmon,
        .mon                    = en_GB_mon,
        .d_t_fmt                = "\u0025\u0061\u0020\u0025\u0062\u0020\u0025\u0065\u0020\u0025\u0048\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u0059",
        .d_fmt                  = "\u0025\u006D\u002F\u0025\u0064\u002F\u0025\u0079",
        .t_fmt                  = "\u0025\u0048\u003A\u0025\u004D\u003A\u0025\u0053",
        .am_pm                  = en_GB_am_pm,
        .t_fmt_ampm             = "\u0025\u0049\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u0070",
        .date_fmt               = "\u0025\u0061\u0020\u0025\u0062\u0020\u0025\u0065\u0020\u0025\u0048\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u005A\u0020\u0025\u0059",
        .first_weekday          = 2
    },
    {   .code                   = "de_DE",
        .int_curr_symbol        = "\u0045\u0055\u0052\u0020",
        .currency_symbol        = "\u20ac",
        .mon_decimal_point      = "\u002c",
        .mon_thousands_sep      = "\u002e",
        .mon_grouping           = "3:3",
        .positive_sign          = "",
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 0,
        .p_sep_by_space         = 1,
        .n_cs_precedes          = 0,
        .n_sep_by_space         = 1,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002c",
        .thousands_sep          = "\u002e",
        .grouping               = "3:3",
        .abday                  = de_DE_abday,
        .day                    = de_DE_day,
        .abmon                  = de_DE_abmon,
        .mon                    = de_DE_mon,
        .d_t_fmt                = de_DE_d_t_fmt,
        .d_fmt                  = de_DE_d_fmt,
        .t_fmt                  = de_DE_t_fmt,
        .am_pm                  = "",
        .t_fmt_ampm             = "",
        .date_fmt               = de_DE_date_fmt,
        .first_weekday          = 2
    },
    {   .code                   = "en_AU",
        .int_curr_symbol        = "\u0041\u0055\u0044\u0020",
        .currency_symbol        = "\u0024",
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = "\u002c",
        .mon_grouping           = "3:3",
        .positive_sign          = "",
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 1,
        .p_sep_by_space         = 0,
        .n_cs_precedes          = 1,
        .n_sep_by_space         = 0,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002E",
        .thousands_sep          = "\u002C",
        .grouping               = "3:3",
        .abday                  = en_GB_abday,
        .day                    = en_GB_day,
        .abmon                  = en_GB_abmon,
        .mon                    = en_GB_mon,
        .d_t_fmt                = en_GB_d_t_fmt,
        .d_fmt                  = en_GB_d_fmt,
        .t_fmt                  = en_GB_t_fmt,
        .am_pm                  = en_US_am_pm,
        .t_fmt_ampm             = "\u0025\u0049\u003A\u0025\u004D\u003A\u0025\u0053\u0020",
        .date_fmt               = en_GB_date_fmt,
        .first_weekday          = 2
    },
    {   .code                   = "en_CA",
        .int_curr_symbol        = "\u0043\u0041\u0044\u0020",
        .currency_symbol        = "\u0024",
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = "\u002c",
        .mon_grouping           = "3:3",
        .positive_sign          = "",
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 1,
        .p_sep_by_space         = 0,
        .n_cs_precedes          = 1,
        .n_sep_by_space         = 0,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002E",
        .thousands_sep          = "\u002C",
        .grouping               = "3:3",
        .abday                  = en_GB_abday,
        .day                    = en_GB_day,
        .abmon                  = en_GB_abmon,
        .mon                    = en_GB_mon,
        .d_t_fmt                = "\u0025\u0061\u0020\u0025\u0064\u0020\u0025\u0062\u0020\u0025\u0059\u0020\u0025\u0072\u0020\u0025\u005A",
        .d_fmt                  = "\u0025\u0059\u002D\u0025\u006D\u002D\u0025\u0064",
        .t_fmt                  = "\u0025\u0072",
        .am_pm                  = en_US_am_pm,
        .t_fmt_ampm             = "\u0025\u0049\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u0070",
        .date_fmt               = en_GB_date_fmt,
        .first_weekday          = 2
    },
    {   .code                   = "en_GB",
        .int_curr_symbol        = "\u0047\u0042\u0050\u0020",
        .currency_symbol        = "\u00a3",
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = "\u002c",
        .mon_grouping           = "3:3",
        .positive_sign          = "",
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 1,
        .p_sep_by_space         = 0,
        .n_cs_precedes          = 1,
        .n_sep_by_space         = 0,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002E",
        .thousands_sep          = "\u002C",
        .grouping               = "3:3",
        .abday                  = en_GB_abday,
        .day                    = en_GB_day,
        .abmon                  = en_GB_abmon,
        .mon                    = en_GB_mon,
        .d_t_fmt                = en_GB_d_t_fmt,
        .d_fmt                  = en_GB_d_fmt,
        .t_fmt                  = en_GB_t_fmt,
        .am_pm                  = en_GB_am_pm,
        .t_fmt_ampm             = en_GB_t_fmt_ampm,
        .date_fmt               = en_GB_date_fmt,
        .first_weekday          = 2
    },
    {   .code                   = "en_NZ",
        .int_curr_symbol        = "\u004e\u005a\u0044\u0020",
        .currency_symbol        = "\u0024",
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = "\u002c",
        .mon_grouping           = "3:3",
        .positive_sign          = "",
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 1,
        .p_sep_by_space         = 0,
        .n_cs_precedes          = 1,
        .n_sep_by_space         = 0,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002E",
        .thousands_sep          = "\u002C",
        .grouping               = "3:3",
        .abday                  = en_GB_abday,
        .day                    = en_GB_day,
        .abmon                  = en_GB_abmon,
        .mon                    = en_GB_mon,
        .d_t_fmt                = en_GB_d_t_fmt,
        .d_fmt                  = en_GB_d_fmt,
        .t_fmt                  = en_GB_t_fmt,
        .am_pm                  = en_US_am_pm,
        .t_fmt_ampm             = "\u0025\u0049\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u0070",
        .date_fmt               = en_GB_date_fmt,
        .first_weekday          = 2
    },
    {   .code                   = "en_US",
        .int_curr_symbol        = "\u0055\u0053\u0044\u0020",
        .currency_symbol        = "\u0024",
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = "\u002c",
        .mon_grouping           = "3:3",
        .positive_sign          = nullptr,
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 1,
        .p_sep_by_space         = 0,
        .n_cs_precedes          = 1,
        .n_sep_by_space         = 0,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002E",
        .thousands_sep          = "\u002C",
        .grouping               = "3:3",
        .abday                  = en_GB_abday,
        .day                    = en_GB_day,
        .abmon                  = en_GB_abmon,
        .mon                    = en_GB_mon,
        .d_t_fmt                = "\u0025\u0061\u0020\u0025\u0064\u0020\u0025\u0062\u0020\u0025\u0059\u0020\u0025\u0072\u0020\u0025\u005A",
        .d_fmt                  = "\u0025\u006D\u002F\u0025\u0064\u002F\u0025\u0059",
        .t_fmt                  = "\u0025\u0072",
        .am_pm                  = en_US_am_pm,
        .t_fmt_ampm             = "\u0025\u0049\u003A\u0025\u004D\u003A\u0025\u0053\u0020\u0025\u0070",
        .date_fmt               = en_GB_date_fmt,
        .first_weekday          = 2
    },
    {   .code                   = "ru_RU",
        .int_curr_symbol        = "\u0052\u0055\u0042\u0020",
        .currency_symbol        = "\u20bd",
        .mon_decimal_point      = "\u002e",
        .mon_thousands_sep      = "\u00a0",
        .mon_grouping           = "3:3",
        .positive_sign          = nullptr,
        .negative_sign          = "\u002d",
        .int_frac_digits        = 2,
        .frac_digits            = 2,
        .p_cs_precedes          = 0,
        .p_sep_by_space         = 1,
        .n_cs_precedes          = 0,
        .n_sep_by_space         = 1,
        .p_sign_posn            = 1,
        .n_sign_posn            = 1,
        .decimal_point          = "\u002c",
        .thousands_sep          = "\u00a0",
        .grouping               = "3:3",
        .abday                  = ru_RU_abday,
        .day                    = ru_RU_day,
        .abmon                  = ru_RU_abmon,
        .mon                    = ru_RU_mon,
        .d_t_fmt                = ru_RU_d_t_fmt,
        .d_fmt                  = ru_RU_d_fmt,
        .t_fmt                  = ru_RU_t_fmt,
        .am_pm                  = nullptr,
        .t_fmt_ampm             = nullptr,
        .date_fmt               = nullptr,
        .first_weekday          = 2
    },
    {   .code                   = nullptr }
};

const Locale_static * find_sdata(const tau::Language & lang, const tau::Territory & terr) {
    std::string code = lang.shortest()+"_"+terr.code2();

    for (const Locale_static * p = sdata_; p->code; ++p) {
        if (code == p->code) {
            return p;
        }
    }

    return sdata_;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

Locale_data sys_locale_ = {
    .spec       = "C",
    .lang       = Language("C"),
    .terr       = Territory(),
    .enc        = Encoding("ASCII"),
    .iocharset  = Encoding("ASCII"),
    .mod        = "",
    .sdata      = sdata_
};

Locale_data locale_ = {
    .spec       = "C",
    .lang       = Language("C"),
    .terr       = Territory(),
    .enc        = Encoding("ASCII"),
    .iocharset  = Encoding("ASCII"),
    .mod        = "",
    .sdata      = sdata_
};

Locale_data * sys_locale_ptr_ = nullptr;
Locale_data * locale_ptr_ = nullptr;

Locale::Locale(const std::string & spec):
    data(locale_ptr_)
{
    if (spec.empty()) {
        if (!data) {
            if (!sys_locale_ptr_) { data = sys_locale_ptr_ = &sys_locale_; init1(); }
            if (!locale_ptr_) { locale_ = sys_locale_; locale_ptr_ = &locale_; }
            data = locale_ptr_;
            init2();
        }
    }

    else {
        data = new Locale_data;
        data->spec = spec;
        data->iocharset = sys_locale_.iocharset;
        init2();
    }
}

Locale::Locale(Locale_data * dataptr):
    data(dataptr)
{
}

// static
Locale Locale::system() {
    return sys_locale_ptr_ ?  Locale(sys_locale_ptr_) : Locale();
}

void Locale::init2() {
    auto end = data->spec.find_first_of("-_");
    if (std::string::npos != end) { data->lang = Language(data->spec.substr(0, end)); }

    auto begin = data->spec.find_first_of("-_");

    if (std::string::npos != begin) {
        ++begin;
        end = data->spec.find('.');
        data->terr = Territory(data->spec.substr(begin, end-begin));
    }

    if (!data->enc) {
        begin = data->spec.find('.');

        if (std::string::npos != begin) {
            ++begin;
            end = data->spec.find('@');
            data->enc = data->spec.substr(begin, end-begin);
        }
    }

    begin = data->spec.find('@');
    if (std::string::npos != begin) { data->mod = data->spec.substr(1+begin); }
    if (!data->iocharset) { data->iocharset = data->enc; }
    data->sdata = find_sdata(data->lang, data->terr);
}

Locale::Locale(const Language & lang, const Territory & terr, const std::string & modifier):
    data(new Locale_data)
{
    data->lang = lang;
    data->terr = terr;
    data->mod = modifier;
    data->iocharset = sys_locale_.iocharset;
    data->sdata = find_sdata(data->lang, data->terr);
}

Locale::Locale(const Language & lang, const Territory & terr, const Encoding & enc, const std::string & modifier):
    data(new Locale_data)
{
    data->lang = lang;
    data->terr = terr;
    data->enc = enc;
    data->iocharset = sys_locale_.iocharset;
    data->mod = modifier;
    data->sdata = find_sdata(data->lang, data->terr);
}

Locale::Locale(const Locale & other):
    data(new Locale_data)
{
    data->lang = other.data->lang;
    data->terr = other.data->terr;
    data->enc = other.data->enc;
    data->iocharset = sys_locale_.iocharset;
    data->mod = other.data->mod;
    data->sdata = other.data->sdata;
}

Locale & Locale::operator=(const Locale & other) {
    if (this != &other) {
        if (data == sys_locale_ptr_) { data = new Locale_data; }
        data->lang = other.data->lang;
        data->terr = other.data->terr;
        data->enc = other.data->enc;
        data->iocharset = sys_locale_.iocharset;
        data->mod = other.data->mod;
        data->sdata = other.data->sdata;
    }

    return *this;
}

Locale::~Locale() {
    if ((data != sys_locale_ptr_) && (data != locale_ptr_)) { delete data; }
}

bool Locale::operator==(const Locale & other) const {
    return data->lang == other.data->lang && data->terr == other.data->terr &&
           data->enc == other.data->enc && data->mod == other.data->mod;
}

bool Locale::operator!=(const Locale & other) const {
    return data->lang != other.data->lang || data->terr != other.data->terr ||
           data->enc != other.data->enc || data->mod != other.data->mod;
}

// static
char * Locale::set(int category, const std::string & locale) {
    Locale lc;
    char * result = setlocale(category, locale.c_str());

    if (result) {
        if (!locale.empty() && lc.code() != locale) {
            lc.data->spec = locale;
            lc.init2();
        }
    }

    return result;
}

std::string Locale::code() const {
    std::string spec = data->sdata->code;

    if (data->enc) {
        spec += '.';
        spec += data->enc.name();
    }

    if (!data->mod.empty()) {
        spec += '@';
        spec += data->mod;
    }

    return spec;
}

const Language & Locale::language() const {
    return data->lang;
}

const Territory & Locale::territory() const {
    return data->terr;
}

const Encoding & Locale::encoding() const {
    return data->enc;
}

const Encoding & Locale::iocharset() const {
    return data->iocharset;
}

std::string Locale::modifier() const {
    return data->mod;
}

std::vector<Locale> Locale::variants() const {
    std::vector<Locale> v;

    std::string locale = code();
    std::string lang = data->lang.code2();
    std::string terr = data->lang.name();
    std::string enc = data->enc.name();
    std::string mod = data->mod;

    if (!lang.empty() && !terr.empty() && !enc.empty() && !mod.empty()) {
        v.push_back(Locale(str_format(lang, '_', terr, '.', enc, mod)));
    }

    if (!lang.empty() && !terr.empty() && !enc.empty()) {
        v.push_back(Locale(str_format(lang, '_', terr, '.', enc)));
    }

    if (!lang.empty() && !terr.empty()) {
        v.push_back(Locale(str_format(lang, '_', terr)));
    }

    if (!lang.empty()) {
        v.push_back(Locale(lang));
    }

    return v;
}

ustring Locale::int_curr_symbol() const {
    return data->sdata->int_curr_symbol ? data->sdata->int_curr_symbol : ustring();
}

ustring Locale::currency_symbol() const {
    return data->sdata->currency_symbol ? data->sdata->currency_symbol : ustring();
}

ustring Locale::mon_decimal_point() const {
    return data->sdata->mon_decimal_point ? data->sdata->mon_decimal_point : ustring();
}

ustring Locale::mon_thousands_sep() const {
    return data->sdata->mon_thousands_sep ? data->sdata->mon_thousands_sep : ustring();
}

std::vector<int> Locale::mon_grouping() const {
    std::vector<int> v;

    if (data->sdata->mon_grouping) {
        for (auto & s: str_explode(data->sdata->mon_grouping, ':')) {
            v.push_back(s[0]-'\0');
        }
    }

    return v;
}

ustring Locale::positive_sign() const {
    return data->sdata->positive_sign ? data->sdata->positive_sign : ustring();
}

ustring Locale::negative_sign() const {
    return data->sdata->negative_sign ? data->sdata->negative_sign : ustring();
}

int Locale::int_frac_digits() const {
    return data->sdata->int_frac_digits;
}

int Locale::frac_digits() const {
    return data->sdata->frac_digits;
}

int Locale::p_cs_precedes() const {
    return data->sdata->p_cs_precedes;
}

int Locale::p_sep_by_space() const {
    return data->sdata->p_sep_by_space;
}

int Locale::n_cs_precedes() const {
    return data->sdata->n_cs_precedes;
}

int Locale::n_sep_by_space() const {
    return data->sdata->n_sep_by_space;
}

int Locale::p_sign_posn() const {
    return data->sdata->p_sign_posn;
}

int Locale::n_sign_posn() const {
    return data->sdata->n_sign_posn;
}

ustring Locale::decimal_point() const {
    return data->sdata->decimal_point ? data->sdata->decimal_point : ustring();
}

ustring Locale::thousands_sep() const {
    return data->sdata->thousands_sep ? data->sdata->thousands_sep : ustring();
}

std::vector<int> Locale::grouping() const {
    std::vector<int> v;

    if (data->sdata->grouping) {
        for (auto & s: str_explode(data->sdata->grouping, ':')) {
            v.push_back(s[0]-'\0');
        }
    }

    return v;
}

std::vector<ustring> Locale::abday() const {
    std::vector<ustring> v;

    if (data->sdata->abday) {
        for (auto & s: str_explode(data->sdata->abday, ':')) {
            v.push_back(s);
        }
    }

    return v;
}

std::vector<ustring> Locale::day() const {
    std::vector<ustring> v;

    if (data->sdata->day) {
        for (auto & s: str_explode(data->sdata->day, ':')) {
            v.push_back(s);
        }
    }

    return v;
}

std::vector<ustring> Locale::abmon() const {
    std::vector<ustring> v;

    if (data->sdata->abmon) {
        for (auto & s: str_explode(data->sdata->abmon, ':')) {
            v.push_back(s);
        }
    }

    return v;
}

std::vector<ustring> Locale::mon() const {
    std::vector<ustring> v;

    if (data->sdata->mon) {
        for (auto & s: str_explode(data->sdata->mon, ':')) {
            v.push_back(s);
        }
    }

    return v;
}

ustring Locale::d_t_fmt() const {
    return data->sdata->d_t_fmt ? data->sdata->d_t_fmt : ustring();
}

ustring Locale::d_fmt() const {
    return data->sdata->d_fmt ? data->sdata->d_fmt : ustring();
}

ustring Locale::t_fmt() const {
    return data->sdata->t_fmt ? data->sdata->t_fmt : ustring();
}

std::vector<ustring> Locale::am_pm() const {
    std::vector<ustring> v;

    if (data->sdata->am_pm) {
        for (auto & s: str_explode(data->sdata->am_pm, ':')) {
            v.push_back(s);
        }
    }

    return v;
}

ustring Locale::t_fmt_ampm() const {
    return data->sdata->t_fmt_ampm ? data->sdata->t_fmt_ampm : ustring();
}

ustring Locale::date_fmt() const {
    return data->sdata->date_fmt ? data->sdata->date_fmt : ustring();
}

int Locale::first_weekday() const {
    return data->sdata->first_weekday;
}

} // namespace tau

//END
