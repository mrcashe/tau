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

/// @file locale.hh The Locale class.

#ifndef TAU_LOCALE_HH
#define TAU_LOCALE_HH

#include <tau/types.hh>
#include <tau/language.hh>
#include <tau/encoding.hh>
#include <tau/ustring.hh>
#include <locale.h>
#include <vector>

namespace tau {

struct Locale_data;
struct Territory_data;

/// Represents territory based on ISO territory/country codes.
/// @ingroup i18n_group
class Territory {
public:

    /// Constructs from ISO code or from current locale if iso_code is empty.
    Territory(const std::string & iso_code=std::string());

    /// Copy constructor.
    Territory(const Territory & other);

    /// Copy operator.
    Territory & operator=(const Territory & other);

    /// Destructor.
   ~Territory();

    /// Create from system default territory.
    static Territory system();

    /// Compare operator.
    bool operator==(const Territory & other) const;

    /// Compare operator.
    bool operator!=(const Territory & other) const;

    /// Get localized name.
    /// @return English name if localized name is not available.
    ustring name() const;

    /// Get localized to specified language name.
    ustring name(const Language & lang) const;

    /// Get English name.
    std::string ename() const;

    /// Get 2-character code.
    std::string code2() const;

    /// Get 3-character code if available.
    std::string code3() const;

    /// Get numeric code.
    int numeric_code() const;

private:

    Territory_data * data;
};

/// %Locale.
/// @ingroup i18n_group
class Locale {
public:

    /// @name Constructors, operators and destructor.
    /// @{

    /// Construct from ISO code or from current locale (when iso_code is empty).
    Locale(const std::string & iso_code = std::string());

    /// Construct from components.
    Locale(const Language & lang, const Territory & terr, const std::string & modifier=std::string());

    /// Construct from components.
    Locale(const Language & lang, const Territory & terr, const Encoding & enc, const std::string & modifier=std::string());

    /// Copy constructor.
    Locale(const Locale & other);

    /// Copy operator.
    Locale & operator=(const Locale & other);

    /// Destructor.
   ~Locale();

    /// @}

    /// Create from system default locale.
    static Locale system();

    /// Wrapper around setlocale(3) libc function.
    static char * set(int category=LC_ALL, const std::string & locale=std::string());

    /// Compare operator.
    bool operator==(const Locale & other) const;

    /// Compare operator.
    bool operator!=(const Locale & other) const;

    /// Get ISO code.
    std::string code() const;

    /// Get language.
    const Language & language() const;

    /// Get territory.
    const Territory & territory() const;

    /// Get system encoding.
    const Encoding & encoding() const;

    /// Get encoding used for file names.
    const Encoding & iocharset() const;

    /// Get modifier.
    std::string modifier() const;

    /// Returns a list of derived variants of locale, which can be used to
    /// e.g. construct locale-dependent filenames or search paths. The returned
    /// list is sorted from most desirable to least desirable.
    /// This function handles territory, character set and extra locale modifiers.
    ///
    /// For example, if locale is "fr_BE", then the returned list
    /// is "fr_BE", "fr".
    /// @note borrowed from glib.
    std::vector<Locale> variants() const;

    /// Get international currency symbol.
    ustring int_curr_symbol() const;

    /// Get string that shall be used as the local currency symbol.
    ustring currency_symbol() const;

    /// Get decimal delimiter.
    /// Get string containing the symbol that shall be used as the decimal delimiter
    /// (radix character) in monetary formatted quantities.
    ustring mon_decimal_point() const;

    /// Get thousands separator.
    /// Get string containing the symbol that shall be used as a separator for groups
    /// of digits to the left of the decimal delimiter in formatted monetary quantities.
    ustring mon_thousands_sep() const;

    /// Define the size of each group of digits in formatted monetary quantities.
    /// Each integer specifies the number of digits in each group, with the initial integer
    /// defining the size of the group immediately preceding the decimal delimiter,
    /// and the following integers defining the preceding groups. If the last integer is not -1,
    /// then the size of the previous group (if any) shall be repeatedly used for the
    /// remainder of the digits. If the last integer is -1, then no further grouping shall be performed.
    std::vector<int> mon_grouping() const;

    /// Get string that shall be used to indicate a non-negative-valued formatted monetary quantity.
    ustring positive_sign() const;

    /// Get string that shall be used to indicate a negative-valued formatted monetary quantity.
    ustring negative_sign() const;

    /// An integer representing the number of fractional digits (those to the right of the decimal delimiter)
    /// to be written in a formatted monetary quantity using int_curr_symbol.
    int int_frac_digits() const;

    /// An integer representing the number of fractional digits (those to the right of the decimal delimiter)
    /// to be written in a formatted monetary quantity using currency_symbol.
    int frac_digits() const;

    /// An integer set to 1 if the currency_symbol precedes the value for a monetary quantity
    /// with a non-negative value, and set to 0 if the symbol succeeds the value.
    int p_cs_precedes() const;

    /// Set to a value indicating the separation of the currency_symbol, the sign string,
    /// and the value for a non-negative formatted monetary quantity.
    /// The values of p_sep_by_space, n_sep_by_space, int_p_sep_by_space, and int_n_sep_by_space
    /// are interpreted according to the following:
    /// 0       No space separates the currency symbol and value.
    /// 1       If the currency symbol and sign string are adjacent, a space separates them from the value; otherwise, a space separates the currency symbol from the value.
    /// 2       If the currency symbol and sign string are adjacent, a space separates them; otherwise, a space separates the sign string from the value.
    int p_sep_by_space() const;

    /// An integer set to 1 if the currency_symbol precedes the value for a monetary quantity
    /// with a negative value, and set to 0 if the symbol succeeds the value.
    int n_cs_precedes() const;

    /// Set to a value indicating the separation of the currency_symbol, the sign string,
    /// and the value for a negative formatted monetary quantity.
    int n_sep_by_space() const;

    /// An integer set to a value indicating the positioning of the positive_sign for a
    /// monetary quantity with a non-negative value. The following integer values shall be
    /// recognized for int_n_sign_posn, int_p_sign_posn, n_sign_posn, and p_sign_posn:
    /// 0       Parentheses enclose the quantity and the currency_symbol.
    /// 1       The sign string precedes the quantity and the currency_symbol.
    /// 2       The sign string succeeds the quantity and the currency_symbol.
    /// 3       The sign string precedes the currency_symbol.
    /// 4       The sign string succeeds the currency_symbol.
    int p_sign_posn() const;

    /// An integer set to a value indicating the positioning of the negative_sign
    /// for a negative formatted monetary quantity.
    int n_sign_posn() const;

    /// Get decimal point.
    /// Get string containing the symbol that shall be used as the decimal delimiter
    /// (radix character) in numeric, non-monetary formatted quantities. This keyword
    /// cannot be omitted and cannot be set to the empty string. In contexts where standards
    /// limit the decimal_point to a single byte, the result of specifying a multi-byte operand
    /// shall be unspecified.
    ustring decimal_point() const;

    /// Get string containing the symbol that shall be used as a separator for groups of digits
    /// to the left of the decimal delimiter in numeric, non-monetary formatted monetary quantities.
    /// In contexts where standards limit the thousands_sep to a single byte, the result of specifying
    /// a multi-byte operand shall be unspecified.
    ustring thousands_sep() const;

    /// Define the size of each group of digits in formatted non-monetary quantities.
    /// The operand is a sequence of integers separated by semicolons. Each integer specifies
    /// the number of digits in each group, with the initial integer defining the size of the group
    /// immediately preceding the decimal delimiter, and the following integers defining the preceding groups.
    /// If the last integer is not -1, then the size of the previous group (if any) shall be repeatedly used
    /// for the remainder of the digits. If the last integer is -1, then no further grouping shall be performed.
    std::vector<int> grouping() const;

    /// Define the abbreviated weekday names, corresponding to the conversion specification
    /// (conversion specification in the strftime(), wcsftime(), and strptime() functions).
    /// The first string shall be the abbreviated name of the day corresponding to Sunday,
    /// the second the abbreviated name of the day corresponding to Monday, and so on.
    std::vector<ustring> abday() const;

    /// Define the full weekday names, corresponding to the conversion specification.
    /// The first string is the full name of the day corresponding to Sunday,
    /// the second the full name of the day corresponding to Monday, and so on.
    std::vector<ustring> day() const;

    /// Define the abbreviated month names, corresponding to the %b conversion specification.
    /// The first string shall be the abbreviated name of the first month of the year (January),
    /// the second the abbreviated name of the second month, and so on.
    std::vector<ustring> abmon() const;

    /// Define the full month names, corresponding to the %B conversion specification.
    /// The first string shall be the full name of the first month of the year (January),
    /// the second the full name of the second month, and so on.
    std::vector<ustring> mon() const;

    /// Define the appropriate date and time representation, corresponding to the conversion specification.
    /// The operand shall consist of a string containing any combination of characters and
    /// conversion specifications. In addition, the string can contain escape sequences
    /// defined in the table in Escape Sequences and Associated Actions.
    ustring d_t_fmt() const;

    /// Define the appropriate date representation, corresponding to the %x conversion specification.
    /// The operand shall consist of a string containing any combination of characters and
    /// conversion specifications. In addition, the string can contain escape sequences
    /// defined in Escape Sequences and Associated Actions.
    ustring d_fmt() const;

    /// Define the appropriate time representation, corresponding to the %X conversion specification.
    /// The operand shall consist of a string containing any combination of characters and
    /// conversion specifications. In addition, the string can contain escape sequences defined
    /// in Escape Sequences and Associated Actions.
    ustring t_fmt() const;

    /// Define the appropriate representation of the ante-meridiem and post-meridiem strings,
    /// corresponding to the %p conversion specification. The operand shall consist of two strings.
    /// The first string shall represent the ante-meridiem designation,
    /// the last string the post-meridiem designation.
    std::vector<ustring> am_pm() const;

    /// Define the appropriate time representation in the 12-hour clock format with am_pm,
    /// corresponding to the conversion specification. The operand shall consist of a string
    /// and can contain any combination of characters and conversion specifications.
    /// If the string is empty, the 12-hour format is not supported in the locale.
    ustring t_fmt_ampm() const;

    /// The return value can be used as a format string for strftime to represent a date in a locale-specific way.
    ustring date_fmt() const;

    /// Get first weekday number: 1 for Sunday, 2 for Monday etc.
    int first_weekday() const;

private:

    Locale_data * data;
    void init1();
    void init2();
    Locale(Locale_data * dataptr);
};

} // namespace tau

#endif // TAU_LOCALE_HH
