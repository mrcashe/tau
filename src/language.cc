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

#include <tau/language.hh>
#include <locale-impl.hh>
#include <tau/string.hh>
#include <iostream>

namespace tau {

struct Language_data {
    const char * code;
    const char * ename;
    const char * scr;
    const char * name;
    const char * sample;
};

Language_data datas[] = {
    {
        .code   = "",
        .ename  = "C",
        .scr    = "Latn"
    },
    {
        .code   = "aa:aar:aars",
        .ename  = "Afar",
        .scr    = "Latn",
        .name   = "Afaraf"
    },
    {
        .code   = "ab:abk:abks",
        .ename  = "Abkhaz",
        .scr    = "Cyrl",
        .name   = "аҧсуа бызшәа, аҧсшәа"
    },
    {
        .code   = "af:afr:afrs",
        .ename  = "Afrikaans",
        .scr    = "Latn",
        .name   = "Afrikaans",
        .sample = "Ek kan glas eet, maar dit doen my nie skade nie."
    },
    {
        .code   = "ak:aka",
        .ename  = "Akan",
        .scr    = "Latn",
        .name   = "Akan"
    },
    {
        .code   = "am:amh",
        .ename  = "Amharic",
        .scr    = "Ethi",
        .name   = "አማርኛ"
    },
    {
        .code   = "an:arg",
        .ename  = "Aragonese",
        .scr    = "Latn",
        .name   = "aragonés"
    },
    {
        .code   = "ar:ara",
        .ename  = "Arabic",
        .scr    = "Arab",
        .name   = "العربية",
        .sample = "نص حكيم له سر قاطع وذو شأن عظيم مكتوب على ثوب أخضر ومغلف بجلد أزرق."
    },
    {
        .code   = "as:asm",
        .ename  = "Assamese",
        .scr    = "Beng",
        .name   = "অসমীয়া"
    },
    {
        .code   = "av:ava",
        .ename  = "Avaric",
        .scr    = "Cyrl",
        .name   = "авар мацӀ, магӀарул мацӀ"
    },
    {
        .code   = "ay:aym",
        .ename  = "Aymara",
        .scr    = "Latn",
        .name   = "aymar aru"
    },
    {
        .code   = "azj:aze:az:az-az",
        .ename  = "North Azerbaijani",
        .scr    = "Latn",
        .name   = "azərbaycan dili",
        .sample = "Zəfər, jaketini də papağını da götür, bu axşam hava çox soyuq olacaq."
    },
    {
        .code   = "azb:az-ir",
        .ename  = "South Azerbaijani",
        .scr    = "Arab"
    },
    {
        .code   = "ba:bak",
        .ename  = "Bashkir",
        .scr    = "Cyrl",
        .name   = "башҡорт теле"
    },
    {
        .code   = "be:bel",
        .ename  = "Belarusian",
        .scr    = "Cyrl",
        .name   = "беларуская мова"
    },
    {
        .code   = "bg:bul:buls",
        .ename  = "Bulgarian",
        .scr    = "Cyrl",
        .name   = "български език",
        .sample = "За миг бях в чужд плюшен скърцащ фотьойл."
    },
    {
        .code   = "bh:bih",
        .ename  = "Bihari",
        .scr    = "Deva",
        .name   = "भोजपुरी"
    },
    {
        .code   = "bho",
        .ename  = "Bhojpuri",
        .scr    = "Deva",
        .name   = "भोजपुरी"
    },
    {
        .code   = "bi:bis",
        .ename  = "Bislama",
        .scr    = "Latn",
        .name   = "Bislama"
    },
    {
        .code   = "bin",
        .ename  = "Edo, Bini",
        .scr    = "Latn"
    },
    {
        .code   = "bm:bam",
        .ename  = "Bambara",
        .scr    = "Latn",
        .name   = "bamanankan"
    },
    {
        .code   = "bn:ben",
        .ename  = "Bengali, Bangla",
        .scr    = "Beng",
        .name   = "বাংলা"
    },
    {
        .code   = "bo:bod:tib",
        .ename  = "Tibetian Standard",
        .scr    = "Tibt",
        .name   = "བོད་ཡིག"
    },
    {
        .code   = "br:bre",
        .ename  = "Breton",
        .scr    = "Latn",
        .name   = "brezhoneg"
    },
    {
        .code   = "brx:sit",
        .ename  = "Bodo",
        .scr    = "Deva",
        .name   = "बोड़ो"
    },
    {
        .code   = "bs:bos",
        .ename  = "Bosnian",
        .scr    = "Latn",
        .name   = "bosanski jezik",
        .sample = "Fin džip, gluh jež i čvrst konjić dođoše bez moljca."
    },
    {
        .code   = "bua",
        .ename  = "Buryat",
        .scr    = "Cyrl",
        .name   = "Буряад хэлэн"
    },
    {
        .code   = "byn",
        .ename  = "Bilen",
        .scr    = "Ethi",
        .name   = "ብሊና"
    },
    {
        .code   = "ca:cat",
        .ename  = "Catalan",
        .scr    = "Latn",
        .name   = "català",
        .sample = "Jove xef, porti whisky amb quinze glaçons d'hidrogen, coi!"
    },
    {
        .code   = "ce:che",
        .ename  = "Chechen",
        .scr    = "Cyrl",
        .name   = "Нохчийн мотт"
    },
    {
        .code   = "ch:cha",
        .ename  = "Chamorro",
        .scr    = "Latn",
        .name   = "Finu' Chamorro"
    },
    {
        .code   = "chm",
        .ename  = "Mari",
        .scr    = "Cyrl",
        .name   = "Марий йылме"
    },
    {
        .code   = "chr",
        .ename  = "Cherokee",
        .scr    = "Cher",
        .name   = " ᏣᎳᎩ"
    },
    {
        .code   = "ckb",
        .ename  = "Central Kurdish",
        .scr    = "Arab",
        .name   = "سۆرانی، کوردیی ناوەندی"
    },
    {
        .code   = "co:cos",
        .ename  = "Corsican",
        .scr    = "Latn",
        .name   = "Corsu"
    },
    {
        .code   = "crh",
        .ename  = "Crimean Tatar",
        .scr    = "Latn",
        .name   = "Qırımtatar"
    },
    {
        .code   = "cs:ces:cze",
        .ename  = "Czech",
        .scr    = "Latn",
        .name   = "čeština",
        .sample = "Příliš žluťoučký kůň úpěl ďábelské ódy."
    },
    {
        .code   = "csb",
        .ename  = "Kashubian",
        .scr    = "Latn",
        .name   = "Kaszëbsczi"
    },
    {
        .code   = "cu:chu",
        .ename  = "Church Slavonic",
        .scr    = "Cyrl",
        .name   = "ѩзыкъ словѣньскъ"
    },
    {
        .code   = "cv:chv",
        .ename  = "Chuvash",
        .scr    = "Cyrl:Latn",
        .name   = "чӑваш чӗлхи"
    },
    {
        .code   = "cy:cym:wel",
        .ename  = "Welsh",
        .scr    = "Latn",
        .name   = "Cymraeg"
    },
    {
        .code   = "da:dan",
        .ename  = "Danish",
        .scr    = "Latn",
        .name   = "dansk",
        .sample = "Quizdeltagerne spiste jordbær med fløde, mens cirkusklovnen Walther spillede på xylofon."
    },
    {
        .code   = "de:deu:ger",
        .ename  = "German",
        .scr    = "Latn",
        .name   = "Deutsch",
        .sample = "Zwölf Boxkämpfer jagen Viktor quer über den großen Sylter Deich."
    },
    {
        .code   = "doi",
        .ename  = "Dogri",
        .scr    = "Deva",
        .name   = "डोगरी"
    },
    {
        .code   = "dv:div",
        .ename  = "Divehi",
        .scr    = "Thaa",
        .name   = "ދިވެހި"
    },
    {
        .code   = "dz:dzo",
        .ename  = "Dzongkha",
        .scr    = "Tibt",
        .name   = "རྫོང་ཁ"
    },
    {
        .code   = "ee:ewe",
        .ename  = "Ewe",
        .scr    = "Latn",
        .name   = "Eʋegbe"
    },
    {
        .code   = "el:ell:gre",
        .ename  = "Greek",
        .scr    = "Grek",
        .name   = "ελληνικά",
        .sample = "Θέλει αρετή και τόλμη η ελευθερία. (Ανδρέας Κάλβος)"
    },
    {
        .code   = "en:eng",
        .ename  = "English",
        .scr    = "Latn",
        .name   = "English",
        .sample = "The quick brown fox jumps over the lazy dog."
    },
    {
        .code   = "eo:epo",
        .ename  = "Esperanto",
        .scr    = "Latn",
        .name   = "Esperanto",
        .sample = "Eĥoŝanĝo ĉiuĵaŭde."
    },
    {
        .code   = "es:spa",
        .ename  = "Spanish",
        .scr    = "Latn",
        .name   = "Español",
        .sample = "Jovencillo emponzoñado de whisky: ¡qué figurota exhibe!"
    },
    {
        .code   = "et:est",
        .ename  = "Estonian",
        .scr    = "Latn",
        .name   = "Eesti",
        .sample = "See väike mölder jõuab rongile hüpata."
    },
    {
        .code   = "eu:eus:baq",
        .ename  = "Basque",
        .scr    = "Latn",
        .name   = "euskara",
        .sample = "Kristala jan dezaket, ez dit minik ematen."
    },
    {
        .code   = "fa:fas:per",
        .ename  = "Persian",
        .scr    = "Arab",
        .name   = "فارسی",
        .sample = "«الا یا اَیُّها السّاقی! اَدِرْ کَأساً وَ ناوِلْها!» که عشق آسان نمود اوّل، ولی افتاد مشکل‌ها!"
    },
    {
        .code   = "fat",
        .ename  = "Fante",
        .scr    = "Latn",
        .name   = "Fante"
    },
    {
        .code   = "ff:ful",
        .ename  = "Fulah",
        .scr    = "Latn",
        .name   = "Fulfulde"
    },
    {
        .code   = "fi:fin",
        .ename  = "Finnish",
        .scr    = "Latn",
        .name   = "Suomi",
        .sample = "Viekas kettu punaturkki laiskan koiran takaa kurkki."
    },
    {
        .code   = "fil",
        .ename  = "Filipino",
        .scr    = "Latn",
        .name   = "Wikang Filipino"
    },
    {
        .code   = "fj:fij",
        .ename  = "Fijian",
        .scr    = "Latn",
        .name   = "vosa Vakaviti"
    },
    {
        .code   = "fo:fao",
        .ename  = "Faroese",
        .scr    = "Latn",
        .name   = "føroyskt"
    },
    {
        .code   = "fr:fra:fre",
        .ename  = "French",
        .scr    = "Latn",
        .name   = "Français",
        .sample = "Voix ambiguë d'un cœur qui, au zéphyr, préfère les jattes de kiwis."
    },
    {
        .code   = "fur",
        .ename  = "Friulian",
        .scr    = "Latn",
        .name   = "Furlane"
    },
    {
        .code   = "fy:fry",
        .ename  = "Western Frisian",
        .scr    = "Latn",
        .name   = "Frysk"
    },
    {
        .code   = "ga:gle",
        .ename  = "Irish",
        .scr    = "Latn",
        .name   = "Gaeilge",
        .sample = "D'ḟuascail Íosa Úrṁac na hÓiġe Beannaiṫe pór Éaḃa agus Áḋaiṁ."
    },
    {
        .code   = "gd:gla",
        .ename  = "Gaelic",
        .scr    = "Latn",
        .name   = "Gàidhlig",
        .sample = "S urrainn dhomh gloinne ithe; cha ghoirtich i mi."
    },
    {
        .code   = "gez",
        .ename  = "Ge'ez",
        .scr    = "Ethi",
        .name   = "ግዕዝ"
    },
    {
        .code   = "gl:glg",
        .ename  = "Galician",
        .scr    = "Latn",
        .name   = "Gallego",
        .sample = "Eu podo xantar cristais e non cortarme."
    },
    {
        .code   = "gn:grn",
        .ename  = "Guaraní",
        .scr    = "Latn",
        .name   = "Avañe'ẽ"
    },
    {
        .code   = "gu:guj",
        .ename  = "Gujarati",
        .scr    = "Gujr",
        .name   = "ગુજરાતી",
        .sample = "હું કાચ ખાઇ શકુ છુ અને તેનાથી મને દર્દ નથી થતુ."
    },
    {
        .code   = "gv:glv",
        .ename  = "Manx",
        .scr    = "Latn",
        .name   = "Gaelg",
        .sample = "Foddym gee glonney agh cha jean eh gortaghey mee."
    },
    {
        .code   = "ha:hau",
        .ename  = "Hausa",
        .scr    = "Latn",
        .name   = "هَوُسَ"
    },
    {
        .code   = "haw",
        .ename  = "Hawaiian",
        .scr    = "Latn",
        .name   = "Ōlelo Hawaiʻi",
        .sample = "Hiki iaʻu ke ʻai i ke aniani; ʻaʻole nō lā au e ʻeha."
    },
    {
        .code   = "he:heb",
        .ename  = "Hebrew",
        .scr    = "Hebr",
        .name   = "עברית",
        .sample = "דג סקרן שט לו בים זך אך לפתע פגש חבורה נחמדה שצצה כך."
    },
    {
        .code   = "hi:hin",
        .ename  = "Hindi",
        .scr    = "Deva",
        .name   = "हिन्दी",
        .sample = "नहीं नजर किसी की बुरी नहीं किसी का मुँह काला जो करे सो उपर वाला"
    },
    {
        .code   = "hne",
        .ename  = "Chhattisgarhi",
        .scr    = "Deva",
        .name   = "छत्तीसगढ़ी"
    },
    {
        .code   = "ho:hmo",
        .ename  = "Hiri Motu",
        .scr    = "Latn",
        .name   = "Hiri Motu"
    },
    {
        .code   = "hr:hrv",
        .ename  = "Croatian",
        .scr    = "Latn",
        .name   = "Hrvatski",
        .sample = "Ja mogu jesti staklo i ne boli me."
    },
    {
        .code   = "hsb",
        .ename  = "Upper Sorbian",
        .scr    = "Latn",
        .name   = "Hornjoserbšćina"
    },
    {
        .code   = "ht:hat",
        .ename  = "Haitian",
        .scr    = "Latn",
        .name   = "Kreyòl ayisyen"
    },
    {
        .code   = "hu:hun",
        .ename  = "Hungarian",
        .scr    = "Latn",
        .name   = "Magyar",
        .sample = "Egy hűtlen vejét fülöncsípő, dühös mexikói úr Wesselényinél mázol Quitóban."
    },
    {
        .code   = "hy:hye",
        .ename  = "Armenian",
        .scr    = "Armn",
        .name   = "Հայերեն",
        .sample = "Կրնամ ապակի ուտել և ինծի անհանգիստ չըներ։"
    },
    {
        .code   = "hz:her",
        .ename  = "Herero",
        .scr    = "Latn",
        .name   = "Otjiherero"
    },
    {
        .code   = "ia:ina",
        .ename  = "Interlingua",
        .scr    = "Latn",
        .name   = "Interlingua"
    },
    {
        .code   = "id:ind",
        .ename  = "Indonesian",
        .scr    = "Latn",
        .name   = "Bahasa Indonesia"
    },
    {
        .code   = "ie:ile",
        .ename  = "Interlingue",
        .scr    = "Latn",
        .name   = "Interlingue"
    },
    {
        .code   = "ig:ibo",
        .ename  = "Igbo",
        .scr    = "Latn",
        .name   = "Asụsụ Igbo"
    },
    {
        .code   = "ii:iii",
        .ename  = "Sichuan Yi",
        .scr    = "Yiii",
        .name   = "ꆈꌠꉙ"
    },
    {
        .code   = "ik:ipk",
        .ename  = "Inupiaq",
        .scr    = "Cyrl",
        .name   = "Iñupiaq"
    },
    {
        .code   = "io:ido",
        .ename  = "Ido",
        .scr    = "Latn",
        .name   = "Ido"
    },
    {
        .code   = "is:isl:ice",
        .ename  = "Icelandic",
        .scr    = "Latn",
        .name   = "Íslenska",
        .sample = "Kæmi ný öxi hér ykist þjófum nú bæði víl og ádrepa"
    },
    {
        .code   = "it:ita",
        .ename  = "Italian",
        .scr    = "Latn",
        .name   = "Italiano",
        .sample = "Ma la volpe, col suo balzo, ha raggiunto il quieto Fido."
    },
    {
        .code   = "iu:iku",
        .ename  = "Inuktitut",
        .scr    = "Cans",
        .name   = "ᐃᓄᒃᑎᑐᑦ"
    },
    {
        .code   = "ja:jpn",
        .ename  = "Japanese",
        .scr    = "Hani:Kana:Hira",
        .name   = "日本語",
        .sample = "いろはにほへと ちりぬるを 色は匂へど 散りぬるを"
    },
    {
        .code   = "jv:jav",
        .ename  = "Javanese",
        .scr    = "Latn",
        .name   = "Basa Jawa",
        .sample = "Aku isa mangan beling tanpa lara."
    },
    {
        .code   = "ka:kat:geo",
        .ename  = "Georgian",
        .scr    = "Geor",
        .name   = "ქართული",
        .sample = "მინას ვჭამ და არა მტკივა."
    },
    {
        .code   = "kaa",
        .ename  = "Kara-Kalpak",
        .scr    = "Cyrl",
        .name   = "Қарақалпақ"
    },
    {
        .code   = "kab",
        .ename  = "Kabyle",
        .scr    = "Latn",
        .name   = "Taqbaylit"
    },
    {
        .code   = "ki:kik",
        .ename  = "Kikuyu",
        .scr    = "Latn",
        .name   = "Gĩkũyũ"
    },
    {
        .code   = "kj:kua",
        .ename  = "Kuanyama",
        .scr    = "Latn",
        .name   = "Kuanyama"
    },
    {
        .code   = "kk:kaz",
        .ename  = "Kazakh",
        .scr    = "Cyrl",
        .name   = "қазақ тілі"
    },
    {
        .code   = "kl:kal",
        .ename  = "Kalaallisut",
        .scr    = "Latn",
        .name   = "Kalaallisut"
    },
    {
        .code   = "km:khm",
        .ename  = "Central Khmer",
        .scr    = "Khmr",
        .name   = "ខ្មែរ"
    },
    {
        .code   = "kl:kal",
        .ename  = "Kalaallisut",
        .scr    = "Latn",
        .name   = "Kalaallisut"
    },
    {
        .code   = "kmr",
        .ename  = "Nothern Kurdish",
        .scr    = "Latn",
        .name   = "Kurmancî"
    },
    {
        .code   = "ko:kor",
        .ename  = "Korean",
        .scr    = "Hang",
        .name   = "한국어",
        .sample = "다람쥐 헌 쳇바퀴에 타고파"
    },
    {
        .code   = "kok",
        .ename  = "Konkani",
        .scr    = "Deva",
        .name   = "कोंकणी"
    },
    {
        .code   = "kr:kau",
        .ename  = "Kanuri",
        .scr    = "Latn",
        .name   = "Kanuri"
    },
    {
        .code   = "ks:kas",
        .ename  = "Kashmiri",
        .scr    = "Arab",
        .name   = "कश्मीरी"
    },
    {
        .code   = "kum",
        .ename  = "Kumyk",
        .scr    = "Cyrl",
        .name   = "къумукъ тил"
    },
    {
        .code   = "kv:kom",
        .ename  = "Komi",
        .scr    = "Cyrl",
        .name   = "коми кыв"
    },
    {
        .code   = "kw:cor",
        .ename  = "Cornish",
        .scr    = "Latn",
        .name   = "Kernewek",
        .sample = "Mý a yl dybry gwéder hag éf ny wra ow ankenya."
    },
    {
        .code   = "kwm",
        .ename  = "Kwambi",
        .scr    = "Latn",
        .name   = "Kwambi"
    },
    {
        .code   = "ky:kir",
        .ename  = "Kirghiz",
        .scr    = "Cyrl",
        .name   = "Кыргызча"
    },
    {
        .code   = "la:lat",
        .ename  = "Latin",
        .scr    = "Latn",
        .name   = "Latine",
        .sample = "Sic surgens, dux, zelotypos quam karus haberis"
    },
    {
        .code   = "lb:ltz",
        .ename  = "Luxembourgish",
        .scr    = "Latn",
        .name   = "Lëtzebuergesch"
    },
    {
        .code   = "lez",
        .ename  = "Lezgian",
        .scr    = "Cyrl",
        .name   = "Лезги чӏал"
    },
    {
        .code   = "lg:lug",
        .ename  = "Ganda",
        .scr    = "Latn",
        .name   = "Luganda"
    },
    {
        .code   = "li:lim",
        .ename  = "Limburgish",
        .scr    = "Latn",
        .name   = "Limburgs"
    },
    {
        .code   = "ln:lin",
        .ename  = "Lingala",
        .scr    = "Latn",
        .name   = "Lingála"
    },
    {
        .code   = "lo:lao",
        .ename  = "Lao",
        .scr    = "Laoo",
        .name   = "ພາສາລາວ"
    },
    {
        .code   = "lt:lit",
        .ename  = "Lithuanian",
        .scr    = "Latn",
        .name   = "Lietuvių kalba",
        .sample = "Įlinkdama fechtuotojo špaga sublykčiojusi pragręžė apvalų arbūzą."
    },
    {
        .code   = "lv:lav",
        .ename  = "Latvian",
        .scr    = "Latn",
        .name   = "Latviešu Valoda",
        .sample = "Sarkanās jūrascūciņas peld pa jūru."
    },
    {
        .code   = "mai",
        .ename  = "Maithili",
        .scr    = "Deva",
        .name   = "মৈথিলী/मैथिली"
    },
    {
        .code   = "mg:mlg",
        .ename  = "Malagasy",
        .scr    = "Latn",
        .name   = "Fiteny malagasy"
    },
    {
        .code   = "mh:mah",
        .ename  = "Marshallese",
        .scr    = "Latn",
        .name   = "Kajin M̧ajeļ"
    },
    {
        .code   = "mi:mri:mao",
        .ename  = "Maori",
        .scr    = "Latn",
        .name   = "te reo Māori"
    },
    {
        .code   = "mk:mkd:mac",
        .ename  = "Macedonian",
        .scr    = "Cyrl",
        .name   = "Македонски јазик",
        .sample = "Можам да јадам стакло, а не ме штета."
    },
    {
        .code   = "ml:mal",
        .ename  = "Malayalam",
        .scr    = "Mlym",
        .name   = "മലയാളം",
        .sample = "വേദനയില്ലാതെ കുപ്പിചില്ലു് എനിയ്ക്കു് കഴിയ്ക്കാം."
    },
    {
        .code   = "mn:mon",
        .ename  = "Mongolian",
        .scr    = "Cyrl",
        .name   = "Монгол хэл"
    },
    {
        .code   = "mni",
        .ename  = "Meitei",
        .scr    = "Beng",
        .name   = "মনিপুরি"
    },
    {
        .code   = "mr:mar",
        .ename  = "Marathi",
        .scr    = "Deva",
        .name   = "मराठी",
        .sample = "मी काच खाऊ शकतो, मला ते दुखत नाही."

    },
    {
        .code   = "ms:msa:may",
        .ename  = "Malay",
        .scr    = "Latn",
        .name   = "Bahasa Melayu",
        .sample = "Saya boleh makan kaca dan ia tidak mencederakan saya."
    },
    {
        .code   = "mt:mlt",
        .ename  = "Maltese",
        .scr    = "Latn",
        .name   = "Malti"
    },
    {
        .code   = "my:mya:bur",
        .ename  = "Burmese",
        .scr    = "Mymr",
        .name   = "ဗမာစာ  "
    },
    {
        .code   = "na:nau",
        .ename  = "Nauru",
        .scr    = "Latn",
        .name   = "Dorerin Naoero"
    },
    {
        .code   = "nb:nob",
        .ename  = "Norwegian Bokmål",
        .scr    = "Latn",
        .name   = "Norsk Bokmål",
        .sample = "Vår sære Zulu fra badeøya spilte jo whist og quickstep i min taxi."
    },
    {
        .code   = "nds",
        .ename  = "Low German",
        .scr    = "Latn",
        .name   = "Plattdeutsch"
    },
    {
        .code   = "ne:nep",
        .ename  = "Nepali",
        .scr    = "Deva",
        .name   = "नेपाली"
    },
    {
        .code   = "ng:ndo",
        .ename  = "Ndonga",
        .scr    = "Latn",
        .name   = "Owambo"
    },
    {
        .code   = "nl:nld:dut",
        .ename  = "Dutch",
        .scr    = "Latn",
        .name   = "Nederlands",
        .sample = "Pa's wijze lynx bezag vroom het fikse aquaduct."
    },
    {
        .code   = "nn:nno",
        .ename  = "Norwegian Nynorsk",
        .scr    = "Latn",
        .name   = "Norsk Nynorsk",
        .sample = "Eg kan eta glas utan å skada meg."
    },
    {
        .code   = "no:nor",
        .ename  = "Norwegian",
        .scr    = "Latn",
        .name   = "Norsk",
        .sample = "Vår sære Zulu fra badeøya spilte jo whist og quickstep i min taxi."
    },
    {
        .code   = "nr:nbl",
        .ename  = "South Ndebele",
        .scr    = "Latn",
        .name   = "isiNdebele"
    },
    {
        .code   = "nv:nav",
        .ename  = "Navajo",
        .scr    = "Latn",
        .name   = "Diné bizaad",
        .sample = "Tsésǫʼ yishą́ągo bííníshghah dóó doo shił neezgai da."
    },
    {
        .code   = "ny:nya",
        .ename  = "Chichewa",
        .scr    = "Latn",
        .name   = "chiCheŵa"
    },
    {
        .code   = "oc:oci",
        .ename  = "Occitan",
        .scr    = "Latn",
        .name   = "Occitan",
        .sample = "Pòdi manjar de veire, me nafrariá pas."
    },
    {
        .code   = "om:orm",
        .ename  = "Oromo",
        .scr    = "Latn",
        .name   = "Afaan Oromoo"
    },
    {
        .code   = "or:ori",
        .ename  = "Oriya",
        .scr    = "Orya",
        .name   = "ଓଡ଼ିଆ",
        .sample = "ମୁଁ କାଚ ଖାଇପାରେ ଏବଂ ତାହା ମୋର କ୍ଷତି କରିନଥାଏ।."
    },
    {
        .code   = "os:oss",
        .ename  = "Ossetian",
        .scr    = "Cyrl",
        .name   = "ирон æвзаг"
    },
    {
        .code   = "ota",
        .ename  = "Turkish, Ottoman",
        .scr    = "Arab",
        .name   = "لسان عثمانى"
    },
    {
        .code   = "pa:pan",
        .ename  = "Panjabi",
        .scr    = "Guru",
        .name   = "ਪੰਜਾਬੀ",
        .sample = "ਮੈਂ ਗਲਾਸ ਖਾ ਸਕਦਾ ਹਾਂ ਅਤੇ ਇਸ ਨਾਲ ਮੈਨੂੰ ਕੋਈ ਤਕਲੀਫ ਨਹੀਂ."
    },
    {
        .code   = "pl:pol",
        .ename  = "Polish",
        .scr    = "Latn",
        .name   = "Język Polski",
        .sample = "Pchnąć w tę łódź jeża lub ośm skrzyń fig."
    },
    {
        .code   = "ps:pus",
        .ename  = "Pashto",
        .scr    = "Arab",
        .name   = "پښتو"
    },
    {
        .code   = "pt:por",
        .ename  = "Portuguese",
        .scr    = "Latn",
        .name   = "Português",
        .sample = "Vejam a bruxa da raposa Salta-Pocinhas e o cão feliz que dorme regalado."
    },
    {
        .code   = "qu:que",
        .ename  = "Quechua",
        .scr    = "Latn",
        .name   = "Runa Simi"
    },
    {
        .code   = "rm:roh",
        .ename  = "Romansh",
        .scr    = "Latn",
        .name   = "Rumantsch Grischun"
    },
    {
        .code   = "rn:run",
        .ename  = "Rundi",
        .scr    = "Latn",
        .name   = "Ikirundi"
    },
    {
        .code   = "ro:ron:rum",
        .ename  = "Romanian",
        .scr    = "Latn",
        .name   = "Română",
        .sample = "Fumegând hipnotic sașiul azvârle mreje în bălți."

    },
    {
        .code   = "ru:rus",
        .ename  = "Russian",
        .scr    = "Cyrl",
        .name   = "Русский",
        .sample = "В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!"
    },
    {
        .code   = "rw:kin",
        .ename  = "Kinyarwanda",
        .scr    = "Latn",
        .name   = "Ikinyarwanda"

    },
    {
        .code   = "sa:san",
        .ename  = "Sanskrit",
        .scr    = "Deva",
        .name   = "संस्कृतम्",
        .sample = "काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥"

    },
    {
        .code   = "sah",
        .ename  = "Yakut",
        .scr    = "Cyrl",
        .name   = "Саха тыла"

    },
    {
        .code   = "sat",
        .ename  = "Santali",
        .scr    = "Deva",
        .name   = "ᱥᱟᱱᱛᱟᱲᱤ"

    },
    {
        .code   = "sc:srd",
        .ename  = "Sardinian",
        .scr    = "Latn",
        .name   = "Sardu"

    },
    {
        .code   = "sco",
        .ename  = "Scots",
        .scr    = "Latn",
        .name   = "Scots"

    },
    {
        .code   = "sd:snd",
        .ename  = "Sindhi",
        .scr    = "Arab",
        .name   = "सिन्धी"

    },
    {
        .code   = "sdh",
        .ename  = "Southern Kurdish",
        .scr    = "Arab",
        .name   = "کوردی خوارگ"
    },
    {
        .code   = "se:sme",
        .ename  = "Northern Sami",
        .scr    = "Latn",
        .name   = "Davvisámegiella"

    },
    {
        .code   = "sg:sag",
        .ename  = "Sango",
        .scr    = "Latn",
        .name   = "Yângâ tî sängö"

    },
    {
        .code   = "si:sin",
        .ename  = "Sinhala",
        .scr    = "Sinh",
        .name   = "සිංහල"

    },
    {
        .code   = "sk:slk:slo",
        .ename  = "Slovak",
        .scr    = "Latn",
        .name   = "Slovenčina",
        .sample = "Starý kôň na hŕbe kníh žuje tíško povädnuté ruže, na stĺpe sa ďateľ učí kvákať novú ódu o živote."
    },
    {
        .code   = "sl:slv",
        .ename  = "Slovenian",
        .scr    = "Latn",
        .name   = "Slovenski Jezik",
        .sample = "Šerif bo za vajo spet kuhal domače žgance."
    },
    {
        .code   = "sm:smo",
        .ename  = "Samoan",
        .scr    = "Latn",
        .name   = "Gagana fa'a Samoa"
    },
    {
        .code   = "sma",
        .ename  = "Southern Sami",
        .scr    = "Latn",
        .name   = "Åarjelsaemien gïele"
    },
    {
        .code   = "smj",
        .ename  = "Lule Sami",
        .scr    = "Latn",
        .name   = "Julevsámegiella"
    },
    {
        .code   = "smn",
        .ename  = "Inari Sami",
        .scr    = "Latn",
        .name   = "Anarâškielâ"
    },
    {
        .code   = "sms",
        .ename  = "Skolt Sami",
        .scr    = "Latn",
        .name   = "Sääʹmǩiõll"
    },
    {
        .code   = "sn:sna",
        .ename  = "Shona",
        .scr    = "Latn",
        .name   = "chiShona"
    },
    {
        .code   = "so:som",
        .ename  = "Somali",
        .scr    = "Latn",
        .name   = "Soomaaliga"
    },
    {
        .code   = "sq:sqi:alb",
        .ename  = "Albanian",
        .scr    = "Latn",
        .name   = "Shqip",
        .sample = "Unë mund të ha qelq dhe nuk më gjen gjë."
    },
    {
        .code   = "sr:srp",
        .ename  = "Serbian",
        .scr    = "Cyrl",
        .name   = "Српски језик",
        .sample = "Чешће цeђење мрeжастим џаком побољшава фертилизацију генских хибрида."
    },
    {
        .code   = "ss:ssw",
        .ename  = "Swati",
        .scr    = "Latn",
        .name   = "SiSwati"
    },
    {
        .code   = "st:sot",
        .ename  = "Southern Sotho",
        .scr    = "Latn",
        .name   = "Sesotho"
    },
    {
        .code   = "su:sun",
        .ename  = "Sundanese",
        .scr    = "Latn",
        .name   = "Basa Sunda"
    },
    {
        .code   = "sv:swe",
        .ename  = "Swedish",
        .scr    = "Latn",
        .name   = "Svenska",
        .sample = "Flygande bäckasiner söka strax hwila på mjuka tuvor."
    },
    {
        .code   = "sw:swa",
        .ename  = "Swahili",
        .scr    = "Latn",
        .name   = "Kiswahili"
    },
    {
        .code   = "syr",
        .ename  = "Syriac",
        .scr    = "Syrc",
        .name   = "ܠܶܫܳܢܳܐ ܣܽܘܪܝܳܝܳܐ"
    },
    {
        .code   = "ta:tam",
        .ename  = "Tamil",
        .scr    = "Taml",
        .name   = "தமிழ்",
        .sample = "நான் கண்ணாடி சாப்பிடுவேன், அதனால் எனக்கு ஒரு கேடும் வராது."
    },
    {
        .code   = "te:tel",
        .ename  = "Telugu",
        .scr    = "Telu",
        .name   = "తెలుగు",
        .sample = "నేను గాజు తినగలను అయినా నాకు యేమీ కాదు."
    },
    {
        .code   = "tg:tgk",
        .ename  = "Tajik",
        .scr    = "Cyrl",
        .name   = "тоҷикӣ"
    },
    {
        .code   = "th:tha",
        .ename  = "Thai",
        .scr    = "Thai",
        .name   = "ไทย",
        .sample = "เป็นมนุษย์สุดประเสริฐเลิศคุณค่า - กว่าบรรดาฝูงสัตว์เดรัจฉาน - จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร - ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า - หัดอภัยเหมือนกีฬาอัชฌาสัย - ปฏิบัติประพฤติกฎกำหนดใจ - พูดจาให้จ๊ะ ๆ จ๋า ๆ น่าฟังเอยฯ"
    },
    {
        .code   = "ti:tir",
        .ename  = "Tigrinya",
        .scr    = "Ethi",
        .name   = "ትግርኛ"
    },
    {
        .code   = "tig",
        .ename  = "Tigre",
        .scr    = "Ethi",
        .name   = "ትግረ"
    },
    {
        .code   = "tk:tuk",
        .ename  = "Turkmen",
        .scr    = "Latn",
        .name   = "Türkmen"
    },
    {
        .code   = "tl:tgl",
        .ename  = "Tagalog",
        .scr    = "Latn",
        .name   = "Wikang Tagalog",
        .sample = "Kaya kong kumain nang bubog at hindi ako masaktan."
    },
    {
        .code   = "tn:tsn",
        .ename  = "Tswana",
        .scr    = "Latn",
        .name   = "Setswana"
    },
    {
        .code   = "to:ton",
        .ename  = "Tonga",
        .scr    = "Latn",
        .name   = "Faka Tonga"
    },
    {
        .code   = "tr:tur",
        .ename  = "Turkish",
        .scr    = "Latn",
        .name   = "Türkçe",
        .sample = "Pijamalı hasta yağız şoföre çabucak güvendi."
    },
    {
        .code   = "ts:tso",
        .ename  = "Tsonga",
        .scr    = "Latn",
        .name   = "Xitsonga"
    },
    {
        .code   = "tt:tat",
        .ename  = "Tatar",
        .scr    = "Cyrl",
        .name   = "Татар теле"
    },
    {
        .code   = "tw:twi",
        .ename  = "Twi",
        .scr    = "Latn",
        .name   = "Twi",
        .sample = "Metumi awe tumpan, ɜnyɜ me hwee."
    },
    {
        .code   = "ti:tah",
        .ename  = "Tahitian",
        .scr    = "Latn",
        .name   = "Reo Tahiti"
    },
    {
        .code   = "tyv",
        .ename  = "Tuvan",
        .scr    = "Cyrl",
        .name   = "Тыва дыл"
    },
    {
        .code   = "ug:uig",
        .ename  = "Uighur",
        .scr    = "Arab",
        .name   = "ئۇيغۇرچە"
    },
    {
        .code   = "uk:ukr",
        .ename  = "Ukrainian",
        .scr    = "Cyrl",
        .name   = "Українська",
        .sample = "Чуєш їх, доцю, га? Кумедна ж ти, прощайся без ґольфів!"
    },
    {
        .code   = "ur:urd",
        .ename  = "Urdu",
        .scr    = "Arab",
        .name   = "اردو",
        .sample = "میں کانچ کھا سکتا ہوں اور مجھے تکلیف نہیں ہوتی ۔"
    },
    {
        .code   = "uz:uzb",
        .ename  = "Uzbek",
        .scr    = "Latn",
        .name   = "Oʻzbek"
    },
    {
        .code   = "ve:ven",
        .ename  = "Venda",
        .scr    = "Latn",
        .name   = "Tshivenḓa"
    },
    {
        .code   = "vi:vie",
        .ename  = "Vietnamese",
        .scr    = "Latn",
        .name   = "Tiếng Việt",
        .sample = "Con sói nâu nhảy qua con chó lười."
    },
    {
        .code   = "vo:vol",
        .ename  = "Volapük",
        .scr    = "Latn",
        .name   = "Volapük"
    },
    {
        .code   = "vot",
        .ename  = "Votic",
        .scr    = "Latn",
        .name   = "Vađđa ceeli"
    },
    {
        .code   = "wa:wln",
        .ename  = "Walloon",
        .scr    = "Latn",
        .name   = "Walon",
        .sample = "Dji pou magnî do vêre, çoula m' freut nén må."
    },
    {
        .code   = "wo:wol",
        .ename  = "Wolof",
        .scr    = "Latn",
        .name   = "Wollof"
    },
    {
        .code   = "xh:xho",
        .ename  = "Xhosa",
        .scr    = "Latn",
        .name   = "isiXhosa"
    },
    {
        .code   = "yi:yid",
        .ename  = "Yiddish",
        .scr    = "Hebr",
        .name   = "ייִדיש",
        .sample = "איך קען עסן גלאָז און עס טוט מיר נישט װײ."
    },
    {
        .code   = "yo:yor",
        .ename  = "Yoruba",
        .scr    = "Latn",
        .name   = "Yorùbá",
        .sample = "Mo lè je̩ dígí, kò ní pa mí lára."
    },
    {
        .code   = "za:zha",
        .ename  = "Zhuang",
        .scr    = "Latn",
        .name   = "Saɯ cueŋƅ"
    },
    {
        .code   = "zh:zho:chi",
        .ename  = "Chinese",
        .scr    = "Hani",
        .name   = "中文",
        .sample = "我能吞下玻璃而不傷身體。"
    },
    {
        .code   = "zu:zul",
        .ename  = "Zulu",
        .scr    = "Latn",
        .name   = "isiZulu"
    },
    {
        .code   = nullptr
    }
};

Language::Language(const std::string & code):
    data(datas)
{
    if (code.empty()) {
        if (locale_ptr_) {
            data = locale_ptr_->lang.data;
        }
    }

    else {
        for (Language_data * datap = datas; datap->code; ++datap) {
            if (str_similar(code, datap->code, ':')) {
                data = datap;
                break;
            }
        }
    }
}

Language::Language(const Language & other):
    data(other.data)
{
}

Language & Language::operator=(const Language & other) {
    if (this != &other) {
        data = other.data;
    }

    return *this;
}

Language::~Language() {}

// static
Language Language::system() {
    return sys_locale_ptr_ ?  sys_locale_ptr_->lang : Language();
}

std::string Language::ename() const {
    return data->ename;
}

std::string Language::code() const {
    const std::vector<ustring> v = str_explode(data->code, ':');
    std::size_t i = v.size(), len = 0;

    for (std::size_t j = 0; j < v.size(); ++j) {
        if (ustring::npos == v[j].find_first_of("-_")) {
            std::size_t l = v[j].size();
            if (l > len) { i = j; len = l; }
        }
    }

    return i < v.size() ? v[i] : "";
}

std::string Language::shortest() const {
    std::vector<ustring> v;
    if (data->code) { v = str_explode(data->code, ':'); }
    std::size_t i = v.size(), len = 0;

    for (const ustring & s: v) {
        len = std::max(len, s.size());
    }

    for (std::size_t j = 0; j < v.size(); ++j) {
        if (ustring::npos == v[j].find_first_of("-_")) {
            std::size_t l = v[j].size();
            if (l < len) { i = j; len = l; }
        }
    }

    return i < v.size() ? v[i] : "";
}

std::string Language::code2() const {
    const std::vector<ustring> v = str_explode(data->code, ':');

    for (std::size_t i = 0; i < v.size(); ++i) {
        if (2 == v[i].size()) {
            return v[i];
        }
    }

    return v.empty() ? "" : v[0];
}

std::string Language::code3() const {
    const std::vector<ustring> v = str_explode(data->code, ':');

    for (std::size_t i = 0; i < v.size(); ++i) {
        if (3 == v[i].size()) {
            return v[i];
        }
    }

    return v.empty() ? "" : v[0];
}

ustring Language::name() const {
    return data->name ? data->name : data->ename;
}

bool Language::operator==(const Language & other) const {
    return other.data == data;
}

bool Language::operator!=(const Language & other) const {
    return other.data != data;
}

ustring Language::sample() const {
    return data->sample ? data->sample : "The quick brown fox jumps over the lazy dog.";
}

std::vector<Script> Language::scripts() const {
    std::vector<Script> v;
    for (const ustring & s: str_explode(data->scr, str_blanks())) { v.push_back(Script::from_code(s)); }
    return v;
}

} // namespace tau

//END
