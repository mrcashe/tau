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

#include <tau/script.hh>
#include <tau/string.hh>

namespace tau {

class Script_data {
public:

    const char * code;
    const char * name;
    int          id;
};

static const Script_data datas[] = {
    { // 0
        .code   = "Zzzz",
        .name   = "Unknown",
        .id     = 999,
    },
    { // 1
        .code   = "Zyyy",
        .name   = "Common",
        .id     = 998,
    },
    { // 2
        .code   = "Zinh",
        .name   = "Inherited",
        .id     = 994,
    },
    { // 3
        .code   = "Arab",
        .name   = "Arabic",
        .id     = 160,
    },
    { // 4
        .code   = "Armn",
        .name   = "Armenian",
        .id     = 230,
    },
    { // 5
        .code   = "Beng",
        .name   = "Bengali",
        .id     = 325,
    },
    { // 6
        .code   = "Bopo",
        .name   = "Bopomofo",
        .id     = 285,
    },
    { // 7
        .code   = "Cher",
        .name   = "Cherokee",
        .id     = 445,
    },
    { // 8
        .code   = "Copt",
        .name   = "Coptic",
        .id     = 204,
    },
    { // 9
        .code   = "Cyrl",
        .name   = "Cyrillic",
        .id     = 220,
    },
    { // 10
        .code   = "Dsrt",
        .name   = "Deseret",
        .id     = 250,
    },
    { // 11
        .code   = "Deva",
        .name   = "Devanagari",
        .id     = 315,
    },
    { // 12
        .code   = "Ethi",
        .name   = "Ethiopic",
        .id     = 430,
    },
    { // 13
        .code   = "Geor",
        .name   = "Georgian",
        .id     = 240,
    },
    { // 14
        .code   = "Goth",
        .name   = "Gothic",
        .id     = 206,
    },
    { // 15
        .code   = "Grek",
        .name   = "Greek",
        .id     = 200,
    },
    { // 16
        .code   = "Gujr",
        .name   = "Gujarati",
        .id     = 320,
    },
    { // 17
        .code   = "Guru",
        .name   = "Gurmukhi",
        .id     = 310,
    },
    { // 18
        .code   = "Hani",
        .name   = "Han",
        .id     = 500,
    },
    { // 19
        .code   = "Hang",
        .name   = "Hangul",
        .id     = 286,
    },
    { // 20
        .code   = "Hebr",
        .name   = "Hebrew",
        .id     = 125,
    },
    { // 21
        .code   = "Hira",
        .name   = "Hiragana",
        .id     = 410,
    },
    { // 22
        .code   = "Knda",
        .name   = "Kannada",
        .id     = 345,
    },
    { // 23
        .code   = "Kana",
        .name   = "Katakana",
        .id     = 411,
    },
    { // 24
        .code   = "Khmr",
        .name   = "Khmer",
        .id     = 355,
    },
    { // 25
        .code   = "Laoo",
        .name   = "Lao",
        .id     = 356,
    },
    { // 26
        .code   = "Latn",
        .name   = "Latin",
        .id     = 215,
    },
    { // 27
        .code   = "Mlym",
        .name   = "Malayalam",
        .id     = 347,
    },
    { // 28
        .code   = "Mong",
        .name   = "Mongolian",
        .id     = 145,
    },
    { // 29
        .code   = "Mymr",
        .name   = "Myanmar",
        .id     = 350,
    },
    { // 30
        .code   = "Ogam",
        .name   = "Ogham",
        .id     = 212,
    },
    { // 31
        .code   = "Ital",
        .name   = "Old Italic",
        .id     = 212,
    },
    { // 32
        .code   = "Orya",
        .name   = "Oriya",
        .id     = 327,
    },
    { // 33
        .code   = "Runr",
        .name   = "Runic",
        .id     = 211,
    },
    { // 34
        .code   = "Sinh",
        .name   = "Sinhala",
        .id     = 348,
    },
    { // 35
        .code   = "Syrc",
        .name   = "Syriac",
        .id     = 135,
    },
    { // 36
        .code   = "Taml",
        .name   = "Tamil",
        .id     = 346,
    },
    { // 37
        .code   = "Telu",
        .name   = "Telugu",
        .id     = 340,
    },
    { // 38
        .code   = "Thaa",
        .name   = "Thaana",
        .id     = 170,
    },
    { // 39
        .code   = "Thai",
        .name   = "Thai",
        .id     = 352,
    },
    { // 40
        .code   = "Tibt",
        .name   = "Tibetian",
        .id     = 330,
    },
    { // 41
        .code   = "Cans",
        .name   = "Canadian Aboriginal",
        .id     = 440,
    },
    { // 42
        .code   = "Yiii",
        .name   = "Yi",
        .id     = 460,
    },
    { // 43
        .code   = "Tglg",
        .name   = "Tagalog",
        .id     = 370,
    },
    { // 44
        .code   = "Hano",
        .name   = "Hanundo",
        .id     = 371,
    },
    { // 45
        .code   = "Buhd",
        .name   = "Buhid",
        .id     = 372,
    },
    { // 46
        .code   = "Tagb",
        .name   = "Tagbanwa",
        .id     = 373,
    },
    { // 47
        .code   = "Brai",
        .name   = "Braille",
        .id     = 570,
    },
    { // 48
        .code   = "Cprt",
        .name   = "Cypriot",
        .id     = 403,
    },
    { // 49
        .code   = "Limb",
        .name   = "Limbu",
        .id     = 336,
    },
    { // 50
        .code   = "Osma",
        .name   = "Osmanya",
        .id     = 260,
    },
    { // 51
        .code   = "Shaw",
        .name   = "Shavian",
        .id     = 281,
    },
    { // 52
        .code   = "Linb",
        .name   = "Linear B",
        .id     = 401,
    },
    { // 53
        .code   = "Tale",
        .name   = "Thai Le",
        .id     = 353,
    },
    { // 54
        .code   = "Ugar",
        .name   = "Ugaritic",
        .id     = 040,
    },
    { // 55
        .code   = "Talu",
        .name   = "New Tai Lue",
        .id     = 354,
    },
    { // 56
        .code   = "Bugi",
        .name   = "Buginese",
        .id     = 367,
    },
    { // 57
        .code   = "Glag",
        .name   = "Glagolitic",
        .id     = 225,
    },
    { // 58
        .code   = "Tfng",
        .name   = "Tifinagh",
        .id     = 120,
    },
    { // 59
        .code   = "Sylo",
        .name   = "Syloti Nagry",
        .id     = 316,
    },
    { // 60
        .code   = "Xpeo",
        .name   = "Old Persian",
        .id     = 030,
    },
    { // 61
        .code   = "Khar",
        .name   = "Kharoshthi",
        .id     = 305,
    },
    { // 62
        .code   = "Bali",
        .name   = "Balinese",
        .id     = 360,
    },
    { // 63
        .code   = "Xsux",
        .name   = "Cuneiform",
        .id     = 020,
    },
    { // 64
        .code   = "Phnx",
        .name   = "Phoenitian",
        .id     = 115,
    },
    { // 65
        .code   = "Phag",
        .name   = "Phags-pa",
        .id     = 331,
    },
    { // 66
        .code   = "Nkoo",
        .name   = "N'Ko",
        .id     = 165,
    },
    { // 67
        .code   = "Kali",
        .name   = "Kayah Li",
        .id     = 357,
    },
    { // 68
        .code   = "Lepc",
        .name   = "LepchaLi",
        .id     = 335,
    },
    { // 69
        .code   = "Rjng",
        .name   = "Rejang",
        .id     = 363,
    },
    { // 70
        .code   = "Sund",
        .name   = "Sundanese",
        .id     = 362,
    },
    { // 71
        .code   = "Saur",
        .name   = "Saurashtra",
        .id     = 344,
    },
    { // 72
        .code   = "Cham",
        .name   = "Cham",
        .id     = 358,
    },
    { // 73
        .code   = "Olck",
        .name   = "Ol Chiki",
        .id     = 261,
    },
    { // 74
        .code   = "Vaii",
        .name   = "Vai",
        .id     = 470,
    },
    { // 75
        .code   = "Cari",
        .name   = "Carian",
        .id     = 201,
    },
    { // 76
        .code   = "Lyci",
        .name   = "Lucian",
        .id     = 202,
    },
    { // 77
        .code   = "Lydi",
        .name   = "Ludian",
        .id     = 116,
    },
    { // 78
        .code   = "Batk",
        .name   = "Batak",
        .id     = 365,
    },
    { // 79
        .code   = "Brah",
        .name   = "Brahmi",
        .id     = 300,
    },
    { // 80
        .code   = "Mand",
        .name   = "Mandaic",
        .id     = 140,
    },
    { // 81
        .code   = "Cakm",
        .name   = "Chakma",
        .id     = 349,
    },
    { // 82
        .code   = "Merc",
        .name   = "Meroitic Cursive",
        .id     = 101,
    },
    { // 83
        .code   = "Mero",
        .name   = "Meroitic Hieroglyphs",
        .id     = 100,
    },
    { // 84
        .code   = "Plrd",
        .name   = "Miao",
        .id     = 282,
    },
    { // 85
        .code   = "Shrd",
        .name   = "Sharada",
        .id     = 319,
    },
    { // 86
        .code   = "Sora",
        .name   = "Sora Sompeng",
        .id     = 398,
    },
    { // 87
        .code   = "Takr",
        .name   = "Takri",
        .id     = 321,
    },
    {
        .code   = nullptr,
        .name   = nullptr,
        .id     = -1
    }
};

Script::Script():
    data(&datas[0])
{
}

Script::Script(const Script_data * datap):
    data(datap)
{
}

std::string Script::name() const {
    return data->name;
}

std::string Script::code() const {
    return data->code;
}

int Script::id() const {
    return data->id;
}

Script Script::from_name(const std::string & name) {
    std::string uname = str_toupper(str_trim(name));

    for (const Script_data * datap = datas; datap->name; ++datap) {
        if (str_toupper(datap->name) == uname) {
            return Script(datap);
        }
    }

    return Unknown();
}

Script Script::from_code(const std::string & code) {
    std::string ucode = str_toupper(str_trim(code));

    for (const Script_data * datap = datas; datap->name; ++datap) {
        if (str_toupper(datap->code) == ucode) {
            return Script(datap);
        }
    }

    return Unknown();
}

Script Script::from_id(int id) {
    for (const Script_data * datap = datas; datap->name; ++datap) {
        if (datap->id == id) {
            return Script(datap);
        }
    }

    return Unknown();
}

bool Script::operator==(const Script & other) const {
    return data == other.data;
}

bool Script::operator!=(const Script & other) const {
    return data != other.data;
}

Script Script::Unknown() {
    return Script(&datas[0]);
}

Script Script::Common() {
    return Script(&datas[1]);
}

Script Script::Inherited() {
    return Script(&datas[2]);
}

Script Script::Arabic() {
    return Script(&datas[3]);
}

Script Script::Armenian() {
    return Script(&datas[4]);
}

Script Script::Bengali() {
    return Script(&datas[5]);
}

Script Script::Bopomofo() {
    return Script(&datas[6]);
}

Script Script::Cherokee() {
    return Script(&datas[7]);
}

Script Script::Coptic() {
    return Script(&datas[8]);
}

Script Script::Cyrillic() {
    return Script(&datas[9]);
}

Script Script::Deseret() {
    return Script(&datas[10]);
}

Script Script::Devanagari() {
    return Script(&datas[11]);
}

Script Script::Ethiopic() {
    return Script(&datas[12]);
}

Script Script::Georgian() {
    return Script(&datas[13]);
}

Script Script::Gothic() {
    return Script(&datas[14]);
}

Script Script::Greek() {
    return Script(&datas[15]);
}

Script Script::Gujarati() {
    return Script(&datas[16]);
}

Script Script::Gurmukhi() {
    return Script(&datas[17]);
}

Script Script::Han() {
    return Script(&datas[18]);
}

Script Script::Hangul() {
    return Script(&datas[19]);
}

Script Script::Hebrew() {
    return Script(&datas[20]);
}

Script Script::Hiragana() {
    return Script(&datas[21]);
}

Script Script::Kannada() {
    return Script(&datas[22]);
}

Script Script::Katakana() {
    return Script(&datas[23]);
}

Script Script::Khmer() {
    return Script(&datas[24]);
}

Script Script::Lao() {
    return Script(&datas[25]);
}

Script Script::Latin() {
    return Script(&datas[26]);
}

Script Script::Malayalam() {
    return Script(&datas[27]);
}

Script Script::Mongolian() {
    return Script(&datas[28]);
}

Script Script::Myanmar() {
    return Script(&datas[29]);
}

Script Script::Ogham() {
    return Script(&datas[30]);
}

Script Script::Old_Italic() {
    return Script(&datas[31]);
}

Script Script::Oriya() {
    return Script(&datas[32]);
}

Script Script::Runic() {
    return Script(&datas[33]);
}

Script Script::Sinhala() {
    return Script(&datas[34]);
}

Script Script::Syriac() {
    return Script(&datas[35]);
}

Script Script::Tamil() {
    return Script(&datas[36]);
}

Script Script::Telugu() {
    return Script(&datas[37]);
}

Script Script::Thaana() {
    return Script(&datas[38]);
}

Script Script::Thai() {
    return Script(&datas[39]);
}

Script Script::Tibetian() {
    return Script(&datas[40]);
}

Script Script::Canadian_aboriginal() {
    return Script(&datas[41]);
}

Script Script::Yi() {
    return Script(&datas[42]);
}

Script Script::Tagalog() {
    return Script(&datas[43]);
}

Script Script::Hanundo() {
    return Script(&datas[44]);
}

Script Script::Buhid() {
    return Script(&datas[45]);
}

Script Script::Tagbanwa() {
    return Script(&datas[46]);
}

Script Script::Braille() {
    return Script(&datas[47]);
}

Script Script::Cypriot() {
    return Script(&datas[48]);
}

Script Script::Limbu() {
    return Script(&datas[49]);
}

Script Script::Osmanya() {
    return Script(&datas[50]);
}

Script Script::Shavian() {
    return Script(&datas[51]);
}

Script Script::Linear_B() {
    return Script(&datas[52]);
}

Script Script::Thai_Le() {
    return Script(&datas[53]);
}

Script Script::Ugaritic() {
    return Script(&datas[54]);
}

Script Script::New_Tai_Lue() {
    return Script(&datas[55]);
}

Script Script::Buginese() {
    return Script(&datas[56]);
}

Script Script::Glagolitic() {
    return Script(&datas[57]);
}

Script Script::Tifinagh() {
    return Script(&datas[58]);
}

Script Script::Syloti_Nagry() {
    return Script(&datas[59]);
}

Script Script::Old_Persian() {
    return Script(&datas[60]);
}

Script Script::Kharoshthi() {
    return Script(&datas[61]);
}

Script Script::Balinese() {
    return Script(&datas[62]);
}

Script Script::Cuneiform() {
    return Script(&datas[63]);
}

Script Script::Phoenitian() {
    return Script(&datas[64]);
}

Script Script::Phags_pa() {
    return Script(&datas[65]);
}

Script Script::NKo() {
    return Script(&datas[66]);
}

Script Script::Kayah_Li() {
    return Script(&datas[67]);
}

Script Script::Lepcha() {
    return Script(&datas[68]);
}

Script Script::Rejang() {
    return Script(&datas[69]);
}

Script Script::Sundanese() {
    return Script(&datas[70]);
}

Script Script::Saurashtra() {
    return Script(&datas[71]);
}

Script Script::Cham() {
    return Script(&datas[72]);
}

Script Script::Ol_Chiki() {
    return Script(&datas[73]);
}

Script Script::Vai() {
    return Script(&datas[74]);
}

Script Script::Carian() {
    return Script(&datas[75]);
}

Script Script::Lycian() {
    return Script(&datas[76]);
}

Script Script::Lydian() {
    return Script(&datas[77]);
}

Script Script::Batak() {
    return Script(&datas[78]);
}

Script Script::Brahmi() {
    return Script(&datas[79]);
}

Script Script::Mandaic() {
    return Script(&datas[80]);
}

Script Script::Chakma() {
    return Script(&datas[81]);
}

Script Script::Meroitic_Cursive() {
    return Script(&datas[82]);
}

Script Script::Meroitic_Hieroglyphs() {
    return Script(&datas[83]);
}

Script Script::Miao() {
    return Script(&datas[84]);
}

Script Script::Sharada() {
    return Script(&datas[85]);
}

Script Script::Sora_Sompeng() {
    return Script(&datas[86]);
}

Script Script::Takri() {
    return Script(&datas[87]);
}

} // namespace tau

//END
