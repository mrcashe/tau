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

#ifndef TAU_TIMEVAL_HH
#define TAU_TIMEVAL_HH

#include <tau/locale.hh>
#include <cstdint>
#include <sys/time.h>

/// @file timeval.hh Timeval class.

namespace tau {

/// The time value.
/// @ingroup time_group
class Timeval {
public:

    Timeval();

    explicit Timeval(uint64_t usec);

    Timeval(const Timeval & other) = default;

    Timeval & operator=(const Timeval & other) = default;

    static Timeval future(uint64_t us);

    static Timeval now();

    Timeval & operator=(uint64_t value_us);

    operator uint64_t() const;

    operator timeval() const;

    struct tm gmtime() const;

    struct tm localtime() const;

    /// Format date and time.
    /// Method is similar to @b strftime() by functionality.
    /// @param format @em strftime -like specificators, see man 3 strftime.
    /// @param locale the locale to use in format.
    ///
    /// The format specification is a null-terminated string and may contain
    /// special character sequences called "conversion specifications",
    /// each of which is introduced by a '\' character and terminated by
    /// some other character known as a "conversion specifier character".
    /// All other character sequences are "ordinary character sequences".
    ///
    /// The characters of ordinary character sequences (including the null byte)
    /// are copied verbatim from @b format to @b s. However, the characters
    /// of conversion specifications are replaced as follows:
    ///
    /// %a The abbreviated weekday name according to the locale.
    /// %A The full weekday name according to the locale.
    /// %b The abbreviated month name according to the current locale.
    /// %B The full month name according to the current locale.
    /// %c The preferred date and time representation for the current locale.
    /// %C The century number (year/100) as a 2-digit integer. (SU)
    /// %d The day of the month as a decimal number (range 01 to 31).
    /// %D Equivalent to %m/%d/%y.
    /// %e Like %d, the day of the month as a decimal number, but a leading
    ///    zero is replaced by a space.
    /// %E Modifier: use alternative format, see below.
    /// %F Equivalent to %Y-%m-%d (the ISO 8601 date format).
    /// %G The ISO 8601 week-based year (see NOTES) with century as a decimal number.
    ///    The 4-digit year corresponding to the ISO week number (see  %V).
    ///    This has the same format and value as %Y except that if the ISO week number
    ///    belongs to the previous or next year, that year is used instead.
    /// %g Like %G, but without century, that is, with a 2-digit year (00-99).
    /// %h Equivalent to %b.
    /// %H The hour as a decimal number using a 24-hour clock (range 00 to 23).
    /// %I The hour as a decimal number using a 12-hour clock (range 01 to 12).
    /// %j The day of the year as a decimal number (range 001 to 366).
    /// %k The hour (24-hour clock) as a decimal number (range 0 to 23);
    ///    single digits are preceded by a blank. (See also %H).
    /// %l The hour (12-hour clock) as a decimal number (range 1 to 12);
    ///    single digits are preceded by a blank. (See also %I).
    /// %m The month as a decimal number (range 01 to 12).
    /// %M The minute as a decimal number (range 00 to 59).
    /// %n A newline character.
    /// %O Modifier: use alternative format, see below.
    /// %p Either "AM" or "PM" according to the given time value, or the
    ///    corresponding strings for the current locale.
    ///    Noon is treated as "PM" and midnight as "AM".
    /// %P Like %p but in lowercase: "am" or "pm" or a corresponding
    ///    string for the current locale. (GNU)
    /// %r The time in a.m. or p.m. notation.
    ///    In the POSIX locale this is equivalent to "%I:%M:%S %p".
    /// %R The time in 24-hour notation (%H:%M).
    ///    For a version including the seconds, see %T below.
    /// %s The number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
    /// %S The second as a decimal number (range 00 to 60).
    ///    (The range is up to 60 to allow for occasional leap seconds.)
    /// %t A tab character.
    /// %T The time in 24-hour notation (%H:%M:%S).
    /// %u The day of the week as a decimal, range 1 to 7, Monday being 1.
    ///    See also %w.
    /// %U The week number of the current year as a decimal number,
    ///    range 00 to 53, starting with the first Sunday as the first day
    ///    of week 01. See also %V and %W.
    /// %V The ISO 8601 week number (see NOTES) of the current year as a decimal number,
    ///    range 01 to 53, where week 1 is the first week that has at least
    ///    4 days in the new year. See also %U and %W.
    /// %w The day of the week as a decimal, range 0 to 6, Sunday being 0.
    ///    See also %u.
    /// %W The week number of the current year as a decimal number,
    ///    range 00 to 53, starting with the first Monday as the first day of week 01.
    /// %x The preferred date representation for the current locale without the time.
    /// %X The preferred time representation for the current locale without the date.
    /// %y The year as a decimal number without a century (range 00 to 99).
    /// %Y The year as a decimal number including the century.
    /// %z The +hhmm or -hhmm numeric timezone (that is, the hour and minute offset from UTC).
    /// %Z The timezone or name or abbreviation.
    /// %+ Not in Linux/glibc, but is in some BSDs (according to their man pages).
    ///    The date and time in date (1) format.
    /// %% A literal '%' character.
    ///
    /// Some conversion specifications can be modified by preceding the
    /// conversion specifier character by the E or O modifier
    /// to indicate that an alternative format should be used.
    ustring str(const ustring & format, const Locale & locale=Locale());

private:

    uint64_t usec_;
};

} // namespace tau

#endif // TAU_TIMEVAL_HH
