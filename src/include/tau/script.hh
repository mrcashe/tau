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

/// @file script.hh The Script class.

#ifndef TAU_SCRIPT_HH
#define TAU_SCRIPT_HH

#include <string>

namespace tau {

class Script_data;

/// A class representing Unicode script according ISO 15924 standard.
/// In Unicode, a script is a collection of letters and other written signs
/// used to represent textual information in one or more writing systems.
/// @ingroup i18n_group
class Script {
public:

    /// Constructs script from current locale.
    Script();

    /// Copy constructor.
    Script(const Script & other) = default;

    /// Copy operator.
    Script & operator=(const Script & other) = default;

    /// Compare operator.
    bool operator==(const Script & other) const;

    /// Compare operator.
    bool operator!=(const Script & other) const;

    /// Gets ISO 15924 script name.
    /// @return the script name, such as "Arabic", "Cyrillic" etc.
    std::string name() const;

    /// Gets ISO 15924 script code.
    /// @return the script code, such as "Arab", "Cyrl", etc.
    std::string code() const;

    /// Gets ISO 15924 script number.
    /// @return the script number, such as 160 for "Arabic" or 220 for "Cyrillic".
    int id() const;

    /// Create script from it's name.
    static Script from_name(const std::string & name);

    /// Create script from it's code.
    static Script from_code(const std::string & code);

    /// Create script from it's id.
    static Script from_id(int id);

    /// Gets unknown script.
    /// Has "Zzzz" code and id of 999.
    static Script Unknown();

    /// Gets undetermined script.
    /// Has "Zyyy" code and id of 998.
    static Script Common();

    /// Gets Inherited script.
    /// Has "Zinh" code and id of 994.
    static Script Inherited();

    /// Gets Arabic script.
    /// Has "Arab" code and id of 160.
    static Script Arabic();

    /// Gets Armenian script.
    /// Has "Armn" code and id of 230.
    static Script Armenian();

    /// Gets Bengali script.
    /// Has "Beng" code and id of 325.
    static Script Bengali();

    /// Gets Bopomofo script.
    /// Has "Bopo" code and id of 285.
    static Script Bopomofo();

    /// Gets Cherokee script.
    /// Has "Cher" code and id of 445.
    static Script Cherokee();

    /// Gets Coptic script.
    /// Has "Copt" code and id of 204.
    static Script Coptic();

    /// Gets Cyrillic script.
    /// Has "Cyrl" code and id of 220.
    static Script Cyrillic();

    /// Gets Deseret script.
    /// Has "Dsrt" code and id of 250.
    static Script Deseret();

    /// Gets Devanagari script.
    /// Has "Deva" code and id of 315.
    static Script Devanagari();

    /// Gets Ethiopic script.
    /// Has "Ethi" code and id of 430.
    static Script Ethiopic();

    /// Gets Georgian script.
    /// Has "Geor" code and id of 240.
    static Script Georgian();

    /// Gets Gothic script.
    /// Has "Goth" code and id of 206.
    static Script Gothic();

    /// Gets Greek script.
    /// Has "Grek" code and id of 200.
    static Script Greek();

    /// Gets Gujarati script.
    /// Has "Gujr" code and id of 320.
    static Script Gujarati();

    /// Gets Gurmukhi script.
    /// Has "Guru" code and id of 310.
    static Script Gurmukhi();

    /// Gets Han script.
    /// Has "Hani" code and id of 500.
    static Script Han();

    /// Gets Hangul script.
    /// Has "Hang" code and id of 286.
    static Script Hangul();

    /// Gets Hebrew script.
    /// Has "Hebr" code and id of 125.
    static Script Hebrew();

    /// Gets Hiragana script.
    /// Has "Hira" code and id of 410.
    static Script Hiragana();

    /// Gets Kannada script.
    /// Has "Knda" code and id of 345.
    static Script Kannada();

    /// Gets Katakana script.
    /// Has "Kana" code and id of 411.
    static Script Katakana();

    /// Gets Khmer script.
    /// Has "Khmr" code and id of 355.
    static Script Khmer();

    /// Gets Lao script.
    /// Has "Laoo" code and id of 356.
    static Script Lao();

    /// Gets Latin script.
    /// Has "Latn" code and id of 215.
    static Script Latin();

    /// Gets Malayalam script.
    /// Has "Mlym" code and id of 347.
    static Script Malayalam();

    /// Gets Mongolian script.
    /// Has "Mong" code and id of 145.
    static Script Mongolian();

    /// Gets Myanmar script.
    /// Has "Mymr" code and id of 350.
    static Script Myanmar();

    /// Gets Ogham script.
    /// Has "Ogam" code and id of 212.
    static Script Ogham();

    /// Gets Old Italic script.
    /// Has "Ital" code and id of 210.
    static Script Old_Italic();

    /// Gets Oriya script.
    /// Has "Orya" code and id of 327.
    static Script Oriya();

    /// Gets Runic script.
    /// Has "Runr" code and id of 211.
    static Script Runic();

    /// Gets Sinhala script.
    /// Has "Sinh" code and id of 348.
    static Script Sinhala();

    /// Gets Syriac script.
    /// Has "Syrc" code and id of 135.
    static Script Syriac();

    /// Gets Tamil script.
    /// Has "Taml" code and id of 346.
    static Script Tamil();

    /// Gets Telugu script.
    /// Has "Telu" code and id of 340.
    static Script Telugu();

    /// Gets Thaana script.
    /// Has "Thaa" code and id of 170.
    static Script Thaana();

    /// Gets Thai script.
    /// Has "Thai" code and id of 352.
    static Script Thai();

    /// Gets Tibetian script.
    /// Has "Tibt" code and id of 330.
    static Script Tibetian();

    /// Gets Canadian Aboriginal script.
    /// Has "Cans" code and id of 440.
    static Script Canadian_aboriginal();

    /// Gets Yi script.
    /// Has "Yiii" code and id of 460.
    static Script Yi();

    /// Gets Tagalog script.
    /// Has "Tglg" code and id of 370.
    static Script Tagalog();

    /// Gets Hanundo script.
    /// Has "Hano" code and id of 371.
    static Script Hanundo();

    /// Gets Buhid script.
    /// Has "Buhd" code and id of 372.
    static Script Buhid();

    /// Gets Tagbanwa script.
    /// Has "Tagb" code and id of 373.
    static Script Tagbanwa();

    /// Gets Braille script.
    /// Has "Brai" code and id of 570.
    static Script Braille();

    /// Gets Cypriot script.
    /// Has "Cprt" code and id of 403.
    static Script Cypriot();

    /// Gets Limbu script.
    /// Has "Limb" code and id of 336.
    static Script Limbu();

    /// Gets Osmanya script.
    /// Has "Osma" code and id of 260.
    static Script Osmanya();

    /// Gets Shavian script.
    /// Has "Shaw" code and id of 281.
    static Script Shavian();

    /// Gets Linear B script.
    /// Has "Linb" code and id of 401.
    static Script Linear_B();

    /// Gets Thai Le script.
    /// Has "Tale" code and id of 353.
    static Script Thai_Le();

    /// Gets Ugaritic script.
    /// Has "Ugar" code and id of 040.
    static Script Ugaritic();

    /// Gets New Tai Lue script.
    /// Has "Talu" code and id of 354.
    static Script New_Tai_Lue();

    /// Gets New Tai Lue script.
    /// Has "Bugi" code and id of 367.
    static Script Buginese();

    /// Gets Glagolitic script.
    /// Has "Glag" code and id of 225.
    static Script Glagolitic();

    /// Gets Tifinagh script.
    /// Has "Tfng" code and id of 120.
    static Script Tifinagh();

    /// Gets Syloti Nagry script.
    /// Has "Sylo" code and id of 316.
    static Script Syloti_Nagry();

    /// Gets Old Persian script.
    /// Has "Xpeo" code and id of 030.
    static Script Old_Persian();

    /// Gets Kharoshthi script.
    /// Has "Khar" code and id of 305.
    static Script Kharoshthi();

    /// Gets Balinese script.
    /// Has "Bali" code and id of 360.
    static Script Balinese();

    /// Gets Cuneiform script.
    /// Has "Xsux" code and id of 020.
    static Script Cuneiform();

    /// Gets Phoenitian script.
    /// Has "Phnx" code and id of 115.
    static Script Phoenitian();

    /// Gets Phags-pa script.
    /// Has "Phag" code and id of 331.
    static Script Phags_pa();

    /// Gets N'Ko script.
    /// Has "Nkoo" code and id of 165.
    static Script NKo();

    /// Gets Kayah Li script.
    /// Has "Kali" code and id of 357.
    static Script Kayah_Li();

    /// Gets Lepcha script.
    /// Has "Lepc" code and id of 335.
    static Script Lepcha();

    /// Gets Rejang script.
    /// Has "Rjng" code and id of 363.
    static Script Rejang();

    /// Gets Sundanese script.
    /// Has "Sund" code and id of 362.
    static Script Sundanese();

    /// Gets Saurashtra script.
    /// Has "Saur" code and id of 344.
    static Script Saurashtra();

    /// Gets Cham script.
    /// Has "Chan" code and id of 358.
    static Script Cham();

    /// Gets Ol Chiki script.
    /// Has "Olck" code and id of 261.
    static Script Ol_Chiki();

    /// Gets Vai script.
    /// Has "Vaii" code and id of 470.
    static Script Vai();

    /// Gets Carian script.
    /// Has "Cari" code and id of 201.
    static Script Carian();

    /// Gets Lycian script.
    /// Has "Lyci" code and id of 202.
    static Script Lycian();

    /// Gets Lydian script.
    /// Has "Lydi" code and id of 116.
    static Script Lydian();

    /// Gets Batak script.
    /// Has "Batk" code and id of 365.
    static Script Batak();

    /// Gets Brahmi script.
    /// Has "Brah" code and id of 300.
    static Script Brahmi();

    /// Gets Mandaic script.
    /// Has "Mand" code and id of 140.
    static Script Mandaic();

    /// Gets Chakma script.
    /// Has "Cakm" code and id of 349.
    static Script Chakma();

    /// Gets Meroitic Cursive script.
    /// Has "Merc" code and id of 101.
    static Script Meroitic_Cursive();

    /// Gets Meroitic Hieroglyphs script.
    /// Has "Mero" code and id of 100.
    static Script Meroitic_Hieroglyphs();

    /// Gets Miao script.
    /// Has "Plrd" code and id of 282.
    static Script Miao();

    /// Gets Sharada script.
    /// Has "Shrd" code and id of 319.
    static Script Sharada();

    /// Gets Sora Sompeng script.
    /// Has "Sora" code and id of 398.
    static Script Sora_Sompeng();

    /// Gets Takri script.
    /// Has "Takr" code and id of 321.
    static Script Takri();

private:

    const Script_data * data;
    Script(const Script_data * datap);
};

} // namespace tau

#endif // TAU_SCRIPT_HH
