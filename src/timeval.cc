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

#include <tau/string.hh>
#include <tau/timeval.hh>
#include <iomanip>

namespace {

void tz_set() {
    static bool called = false;

    if (!called) {
        called = true;
        tzset();
    }
}

inline int isleap(int year) {
    return 0 == year%4 && (0 != year%100 || 0 == year%400);
}

int iso_week_days(int yday, int wday) {
    int big_enough_multiple_of_7 = (2+366/7)*7;
    return 3+yday-(yday-wday+4+big_enough_multiple_of_7)%7;
}

int tm_diff(const struct tm * a, const struct tm * b) {
    int a4 = (a->tm_year >> 2) + (1900 >> 2) - ! (a->tm_year & 3);
    int b4 = (b->tm_year >> 2) + (1900 >> 2) - ! (b->tm_year & 3);
    int a100 = a4 / 25 - (a4 % 25 < 0);
    int b100 = b4 / 25 - (b4 % 25 < 0);
    int a400 = a100 >> 2;
    int b400 = b100 >> 2;
    int intervening_leap_days = (a4-b4)-(a100-b100)+(a400-b400);
    int years = a->tm_year - b->tm_year;
    int days = (365*years + intervening_leap_days + (a->tm_yday-b->tm_yday));

    return (60*(60*(24*days+(a->tm_hour-b->tm_hour))+(a->tm_min - b->tm_min))+(a->tm_sec - b->tm_sec));
}

tau::ustring str_format_time_int(const tau::ustring & fmt, const tau::Timeval & tv, const tau::Locale & loc) {
    tau::ustring res;
    struct tm tt = tv.localtime();
    char32_t ctx = 0;

    for (char32_t wc: fmt) {
        if (0 == ctx) {
            if ('%' == wc) { ctx = wc; }
            else { res += wc; }
            continue;
        }

        if ('%' == ctx && ('O' == wc || 'E' == wc)) {
            ctx = wc;
            continue;
        }

        switch (wc) {
            case '%':
                res += wc;
                break;

            case 'a':
            case 'A':
                if (tt.tm_wday >= 0 && tt.tm_wday < 7) {
                    auto v = 'a' == wc ? loc.abday() : loc.day();
                    if (size_t(tt.tm_wday) < v.size()) { res += v[tt.tm_wday]; }
                }

                break;

            case 'b':
            case 'h':
            case 'B':
                if (tt.tm_mon >= 0 && tt.tm_mon < 12) {
                    auto v = 'B' == wc ? loc.mon() : loc.abmon();
                    if (size_t(tt.tm_mon) < v.size()) { res += v[tt.tm_mon]; }
                }

                break;

            case 'c':
                res += str_format_time_int(loc.d_t_fmt(), tv, loc);
                break;

            case 'C':
                res += tau::str_format(std::setw(2), std::setfill('0'), (1900+tt.tm_year)%100);
                break;

            case 'd':
                res += tau::str_format(std::setw(2), std::setfill('0'), tt.tm_mday);
                break;

            case 'D':
                res += str_format_time_int("%m/%d/%y", tv, loc);
                break;

            case 'e':
                res += tau::str_format(std::setw(2), std::setfill(' '), tt.tm_mday);
                break;

            case 'F':
                res += str_format_time_int("%Y-%m-%d", tv, loc);
                break;

            case 'H':
            case 'k':
                res += tau::str_format(std::setw(2), std::setfill('H' == wc ? '0' : ' '), tt.tm_hour);
                break;

            case 'I':
            case 'l':
            {
                int hour = tt.tm_hour%12;
                if (0 == hour) { hour = 12; }
                res += tau::str_format(std::setw(2), std::setfill('I' == wc ? '0' : ' '), hour);
            }
            break;

            case 'M':
                res += tau::str_format(std::setw(2), std::setfill('0'), tt.tm_min);
                break;

            case 'm':
                res += tau::str_format(std::setw(2), std::setfill('0'), 1+tt.tm_mon);
                break;

            case 'S':
                res += tau::str_format(std::setw(2), std::setfill('0'), tt.tm_sec);
                break;

            case 'n':
                res += '\n';
                break;

            case 'p':
            case 'P':
            {
                auto v = loc.am_pm();

                if (v.size() > 1) {
                    tau::ustring s = v[tt.tm_hour > 11 ? 1 : 0];
                    if ('P' == wc) { tau::str_tolower(s); }
                    res += s;
                }
            }
            break;

            case 'r':
            {
                tau::ustring fmt = loc.t_fmt_ampm();
                if (fmt.empty()) { fmt = "%I:%M:%S %p"; }
                res += str_format_time_int(fmt, tv, loc);
            }
            break;

            case 'R':
                res += str_format_time_int("%H:%M", tv, loc);
                break;

            case 'T':
                res += str_format_time_int("%H:%M:%S", tv, loc);
                break;

            case 't':
                res += '\t';
                break;

            case 'u':
                res += tau::str_format((tt.tm_wday-1+7)%7+1);
                break;

            case 'U':
                res += tau::str_format((tt.tm_yday-tt.tm_wday+7)/7);
                break;

            case 'V':
            case 'g':
            case 'G':
            {
                int year = 1900+tt.tm_year;
                int days = iso_week_days(tt.tm_yday, tt.tm_wday);

                if (days < 0) {
                    --year;
                    days = iso_week_days(tt.tm_yday+(365+isleap(year)), tt.tm_wday);
                }

                else {
                    int d = iso_week_days(tt.tm_yday-(365+isleap(year)), tt.tm_wday);
                    if (0 <= d) { year++; days = d; }
                }

                if ('g' == wc) { res += tau::str_format((year%100+100)%100); }
                else if ('G' == wc) { res += tau::str_format(year); }
                else { res += tau::str_format(1+days/7); }
            }
            break;

            case 'w':
                res += tau::str_format(tt.tm_wday);
                break;

            case 'W':
                res += tau::str_format((tt.tm_yday-(7+tt.tm_wday-1)%7+7)/7);
                break;

            case 'x':
                res += str_format_time_int(loc.d_fmt(), tv, loc);
                break;

            case 'X':
                res += str_format_time_int(loc.t_fmt(), tv, loc);
                break;

            case 'y':
                res += tau::str_format(std::setw(2), std::setfill('0'), (1900+tt.tm_year)%100);
                break;

            case 'Y':
                res += tau::str_format(1900+tt.tm_year);
                break;

            case 'z':
                if (tt.tm_isdst >= 0) {
                    struct tm gtm = tv.gmtime();
                    int diff = tm_diff(&tt, &gtm);

                    if (diff < 0) {
                        res += '-';
                        diff = -diff;
                    }

                    else {
                        res += '+';
                    }

                    diff /= 60;
                    res += tau::str_format((diff/60)*100 + diff%60);
                }

                break;

            case 'Z':
                if (tt.tm_isdst >= 0) {
                    res += (tt.tm_isdst <= 1 ? tzname[tt.tm_isdst] : "?");
                }

                break;

            default:
                res += '%';
                if ('%' != ctx) { res += ctx; }
                res += wc;
        }

        ctx = 0;
    }

    return res;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

Timeval::Timeval():
    usec_(0)
{
    tz_set();
}

Timeval::Timeval(uint64_t usec):
    usec_(usec)
{
    tz_set();
}

Timeval Timeval::now() {
    ::timeval tv;
    ::gettimeofday(&tv, nullptr);
    return Timeval(uint64_t(1000000)*uint64_t(tv.tv_sec)+tv.tv_usec);
}

Timeval::operator timeval() const {
    ::timeval tv;
    tv.tv_sec  = usec_/1000000;
    tv.tv_usec = usec_%1000000;
    return tv;
}

ustring Timeval::str(const ustring & fmt, const Locale & locale) {
    return str_format_time_int(fmt, *this, locale);
}

} // namespace tau

//END
