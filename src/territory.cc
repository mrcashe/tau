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

#include <locale-impl.hh>
#include <tau/string.hh>

namespace tau {

struct Territory_data {
    const char *        code2;
    const char *        code3;
    int                 num;
    const char *        ename;
};

} // namespace tau

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace {

tau::Territory_data data_[] = {
    {   .code2          = "",
        .code3          = "",
        .num            = 0,
        .ename          = "Unknown" },
    {   .code2          = "AF",
        .code3          = "AFG",
        .num            = 4,
        .ename          = "Afghanistan" },
    {   .code2          = "AX",
        .code3          = "ALA",
        .num            = 248,
        .ename          = "Åland Islands" },
    {   .code2          = "AL",
        .code3          = "ALB",
        .num            = 8,
        .ename          = "Albania" },
    {   .code2          = "DZ",
        .code3          = "DZA",
        .num            = 12,
        .ename          = "Algeria" },
    {   .code2          = "AS",
        .code3          = "ASM",
        .num            = 16,
        .ename          = "American Samoa" },
    {   .code2          = "AD",
        .code3          = "AND",
        .num            = 20,
        .ename          = "Andorra" },
    {   .code2          = "AO",
        .code3          = "AGO",
        .num            = 24,
        .ename          = "Angola" },
    {   .code2          = "AI",
        .code3          = "AIA",
        .num            = 660,
        .ename          = "Anguilla" },
    {   .code2          = "AQ",
        .code3          = "ATA",
        .num            = 10,
        .ename          = "Antarctica" },
    {   .code2          = "AG",
        .code3          = "ATG",
        .num            = 28,
        .ename          = "Antigua and Barbuda" },
    {   .code2          = "AR",
        .code3          = "ARG",
        .num            = 32,
        .ename          = "Argentina" },
    {   .code2          = "AM",
        .code3          = "ARM",
        .num            = 51,
        .ename          = "Armenia" },
    {   .code2          = "AW",
        .code3          = "ABW",
        .num            = 533,
        .ename          = "Aruba" },
    {   .code2          = "AU",
        .code3          = "AUS",
        .num            = 36,
        .ename          = "Australia" },
    {   .code2          = "AT",
        .code3          = "AUT",
        .num            = 40,
        .ename          = "Austria" },
    {   .code2          = "AZ",
        .code3          = "AZE",
        .num            = 31,
        .ename          = "Azerbaijan" },
    {   .code2          = "BS",
        .code3          = "BHS",
        .num            = 44,
        .ename          = "Bahamas" },
    {   .code2          = "BH",
        .code3          = "BHR",
        .num            = 48,
        .ename          = "Bahrain" },
    {   .code2          = "BD",
        .code3          = "BGD",
        .num            = 50,
        .ename          = "Bangladesh" },
    {   .code2          = "BB",
        .code3          = "BRB",
        .num            = 52,
        .ename          = "Barbados" },
    {   .code2          = "BY",
        .code3          = "BLR",
        .num            = 112,
        .ename          = "Belarus" },
    {   .code2          = "BE",
        .code3          = "BEL",
        .num            = 56,
        .ename          = "Belgium" },
    {   .code2          = "BZ",
        .code3          = "BLZ",
        .num            = 84,
        .ename          = "Belize" },
    {   .code2          = "BJ",
        .code3          = "BEN",
        .num            = 204,
        .ename          = "Benin" },
    {   .code2          = "BM",
        .code3          = "BMU",
        .num            = 60,
        .ename          = "Bermuda" },
    {   .code2          = "BT",
        .code3          = "BTN",
        .num            = 64,
        .ename          = "Bhutan" },
    {   .code2          = "BO",
        .code3          = "BOL",
        .num            = 68,
        .ename          = "Bolivia (Plurinational State of)" },
    {   .code2          = "BQ",
        .code3          = "BES",
        .num            = 535,
        .ename          = "Bonaire, Sint Eustatius and Saba" },
    {   .code2          = "BA",
        .code3          = "BIH",
        .num            = 70,
        .ename          = "Bosnia and Herzegovina" },
    {   .code2          = "BW",
        .code3          = "BWA",
        .num            = 72,
        .ename          = "Botswana" },
    {   .code2          = "BV",
        .code3          = "BVT",
        .num            = 74,
        .ename          = "Bouvet Island" },
    {   .code2          = "BR",
        .code3          = "BRA",
        .num            = 76,
        .ename          = "Brazil" },
    {   .code2          = "IO",
        .code3          = "IOT",
        .num            = 86,
        .ename          = "British Indian Ocean Territory" },
    {   .code2          = "BN",
        .code3          = "BRN",
        .num            = 96,
        .ename          = "Brunei Darussalam" },
    {   .code2          = "BG",
        .code3          = "BGR",
        .num            = 100,
        .ename          = "Bulgaria" },
    {   .code2          = "BF",
        .code3          = "BFA",
        .num            = 854,
        .ename          = "Burkina Faso" },
    {   .code2          = "BI",
        .code3          = "BDI",
        .num            = 108,
        .ename          = "Burundi" },
    {   .code2          = "CV",
        .code3          = "CPV",
        .num            = 132,
        .ename          = "Cabo Verde" },
    {   .code2          = "KH",
        .code3          = "KHM",
        .num            = 116,
        .ename          = "Cambodia" },
    {   .code2          = "CM",
        .code3          = "CMR",
        .num            = 120,
        .ename          = "Cameroon" },
    {   .code2          = "CA",
        .code3          = "CAN",
        .num            = 124,
        .ename          = "Canada" },
    {   .code2          = "KY",
        .code3          = "CYM",
        .num            = 136,
        .ename          = "Cayman Islands" },
    {   .code2          = "CF",
        .code3          = "CAF",
        .num            = 140,
        .ename          = "Central African Republic" },
    {   .code2          = "TD",
        .code3          = "TCD",
        .num            = 148,
        .ename          = "Chad" },
    {   .code2          = "CL",
        .code3          = "CHL",
        .num            = 152,
        .ename          = "Chile" },
    {   .code2          = "CN",
        .code3          = "CHN",
        .num            = 156,
        .ename          = "China" },
    {   .code2          = "CX",
        .code3          = "CXR",
        .num            = 162,
        .ename          = "Christmas Island" },
    {   .code2          = "CC",
        .code3          = "CCK",
        .num            = 166,
        .ename          = "Cocos (Keeling) Islands" },
    {   .code2          = "CO",
        .code3          = "COL",
        .num            = 170,
        .ename          = "Colombia" },
    {   .code2          = "KM",
        .code3          = "COM",
        .num            = 174,
        .ename          = "Comoros" },
    {   .code2          = "CG",
        .code3          = "COG",
        .num            = 178,
        .ename          = "Congo" },
    {   .code2          = "CD",
        .code3          = "COD",
        .num            = 180,
        .ename          = "Congo (Democratic Republic of the)" },
    {   .code2          = "CK",
        .code3          = "COK",
        .num            = 184,
        .ename          = "Cook Islands" },
    {   .code2          = "CR",
        .code3          = "CRI",
        .num            = 188,
        .ename          = "Costa Rica" },
    {   .code2          = "CI",
        .code3          = "CIV",
        .num            = 384,
        .ename          = "Côte d'Ivoire" },
    {   .code2          = "HR",
        .code3          = "HRV",
        .num            = 191,
        .ename          = "Croatia" },
    {   .code2          = "CU",
        .code3          = "CUB",
        .num            = 192,
        .ename          = "Cuba" },
    {   .code2          = "CW",
        .code3          = "CUW",
        .num            = 531,
        .ename          = "Curaçao" },
    {   .code2          = "CY",
        .code3          = "CYP",
        .num            = 196,
        .ename          = "Cyprus" },
    {   .code2          = "CZ",
        .code3          = "CZE",
        .num            = 203,
        .ename          = "Czechia" },
    {   .code2          = "DK",
        .code3          = "DNK",
        .num            = 208,
        .ename          = "Denmark" },
    {   .code2          = "DJ",
        .code3          = "DJI",
        .num            = 262,
        .ename          = "Djibouti" },
    {   .code2          = "DM",
        .code3          = "DMA",
        .num            = 212,
        .ename          = "Dominica" },
    {   .code2          = "DO",
        .code3          = "DOM",
        .num            = 214,
        .ename          = "Dominican Republic" },
    {   .code2          = "EC",
        .code3          = "ECU",
        .num            = 218,
        .ename          = "Ecuador" },
    {   .code2          = "EG",
        .code3          = "EGY",
        .num            = 818,
        .ename          = "Egypt" },
    {   .code2          = "SV",
        .code3          = "SLV",
        .num            = 222,
        .ename          = "El Salvador" },
    {   .code2          = "GQ",
        .code3          = "GNQ",
        .num            = 226,
        .ename          = "Equatorial Guinea" },
    {   .code2          = "ER",
        .code3          = "ERI",
        .num            = 232,
        .ename          = "Eritrea" },
    {   .code2          = "EE",
        .code3          = "EST",
        .num            = 233,
        .ename          = "Estonia" },
    {   .code2          = "ET",
        .code3          = "ETH",
        .num            = 231,
        .ename          = "Ethiopia" },
    {   .code2          = "FK",
        .code3          = "FLK",
        .num            = 238,
        .ename          = "Falkland Islands (Malvinas)" },
    {   .code2          = "FO",
        .code3          = "FRO",
        .num            = 234,
        .ename          = "Faroe Islands" },
    {   .code2          = "FJ",
        .code3          = "FJI",
        .num            = 242,
        .ename          = "Fiji" },
    {   .code2          = "FI",
        .code3          = "FIN",
        .num            = 246,
        .ename          = "Finland" },
    {   .code2          = "FR",
        .code3          = "FRA",
        .num            = 250,
        .ename          = "France" },
    {   .code2          = "GF",
        .code3          = "GUF",
        .num            = 254,
        .ename          = "French Guiana" },
    {   .code2          = "PF",
        .code3          = "PYF",
        .num            = 258,
        .ename          = "French Polynesia" },
    {   .code2          = "TF",
        .code3          = "ATF",
        .num            = 260,
        .ename          = "French Southern Territories" },
    {   .code2          = "GA",
        .code3          = "GAB",
        .num            = 266,
        .ename          = "Gabon" },
    {   .code2          = "GM",
        .code3          = "GMB",
        .num            = 270,
        .ename          = "Gambia" },
    {   .code2          = "GE",
        .code3          = "GEO",
        .num            = 268,
        .ename          = "Georgia" },
    {   .code2          = "DE",
        .code3          = "DEU",
        .num            = 276,
        .ename          = "Germany" },
    {   .code2          = "GH",
        .code3          = "GHA",
        .num            = 288,
        .ename          = "Ghana" },
    {   .code2          = "GI",
        .code3          = "GIB",
        .num            = 292,
        .ename          = "Gibraltar" },
    {   .code2          = "GR",
        .code3          = "GRC",
        .num            = 300,
        .ename          = "Greece" },
    {   .code2          = "GL",
        .code3          = "GRL",
        .num            = 304,
        .ename          = "Greenland" },
    {   .code2          = "GD",
        .code3          = "GRD",
        .num            = 308,
        .ename          = "Grenada" },
    {   .code2          = "GP",
        .code3          = "GLP",
        .num            = 312,
        .ename          = "Guadeloupe" },
    {   .code2          = "GU",
        .code3          = "GUM",
        .num            = 316,
        .ename          = "Guam" },
    {   .code2          = "GT",
        .code3          = "GTM",
        .num            = 320,
        .ename          = "Guatemala" },
    {   .code2          = "GG",
        .code3          = "GGY",
        .num            = 831,
        .ename          = "Guernsey" },
    {   .code2          = "GN",
        .code3          = "GIN",
        .num            = 324,
        .ename          = "Guinea" },
    {   .code2          = "GW",
        .code3          = "GNB",
        .num            = 624,
        .ename          = "Guinea-Bissau" },
    {   .code2          = "GY",
        .code3          = "GUY",
        .num            = 328,
        .ename          = "Guyana" },
    {   .code2          = "HT",
        .code3          = "HTI",
        .num            = 332,
        .ename          = "Haiti" },
    {   .code2          = "HM",
        .code3          = "HMD",
        .num            = 334,
        .ename          = "Heard Island and McDonald Islands" },
    {   .code2          = "VA",
        .code3          = "VAT",
        .num            = 336,
        .ename          = "Holy See" },
    {   .code2          = "HN",
        .code3          = "HND",
        .num            = 340,
        .ename          = "Honduras" },
    {   .code2          = "HK",
        .code3          = "HKG",
        .num            = 344,
        .ename          = "Hong Kong" },
    {   .code2          = "HU",
        .code3          = "HUN",
        .num            = 348,
        .ename          = "Hungary" },
    {   .code2          = "IS",
        .code3          = "ISL",
        .num            = 352,
        .ename          = "Iceland" },
    {   .code2          = "IN",
        .code3          = "IND",
        .num            = 356,
        .ename          = "India" },
    {   .code2          = "ID",
        .code3          = "IDN",
        .num            = 360,
        .ename          = "Indonesia" },
    {   .code2          = "IR",
        .code3          = "IRN",
        .num            = 364,
        .ename          = "Iran (Islamic Republic of)" },
    {   .code2          = "IQ",
        .code3          = "IRQ",
        .num            = 368,
        .ename          = "Iraq" },
    {   .code2          = "IE",
        .code3          = "IRL",
        .num            = 372,
        .ename          = "Ireland" },
    {   .code2          = "IM",
        .code3          = "IMN",
        .num            = 833,
        .ename          = "Isle of Man" },
    {   .code2          = "IL",
        .code3          = "ISR",
        .num            = 376,
        .ename          = "Israel" },
    {   .code2          = "IT",
        .code3          = "ITA",
        .num            = 380,
        .ename          = "Italy" },
    {   .code2          = "JM",
        .code3          = "JAM",
        .num            = 388,
        .ename          = "Jamaica" },
    {   .code2          = "JP",
        .code3          = "JPN",
        .num            = 392,
        .ename          = "Japan" },
    {   .code2          = "JE",
        .code3          = "JEY",
        .num            = 832,
        .ename          = "Jersey" },
    {   .code2          = "JO",
        .code3          = "JOR",
        .num            = 400,
        .ename          = "Jordan" },
    {   .code2          = "KZ",
        .code3          = "KAZ",
        .num            = 398,
        .ename          = "Kazakhstan" },
    {   .code2          = "KE",
        .code3          = "KEN",
        .num            = 404,
        .ename          = "Kenya" },
    {   .code2          = "KI",
        .code3          = "KIR",
        .num            = 296,
        .ename          = "Kiribati" },
    {   .code2          = "KP",
        .code3          = "PRK",
        .num            = 408,
        .ename          = "Korea (Democratic People's Republic of)" },
    {   .code2          = "KR",
        .code3          = "KOR",
        .num            = 410,
        .ename          = "Korea (Republic of)" },
    {   .code2          = "KW",
        .code3          = "KWT",
        .num            = 414,
        .ename          = "Kuwait" },
    {   .code2          = "KG",
        .code3          = "KGZ",
        .num            = 417,
        .ename          = "Kyrgyzstan" },
    {   .code2          = "LA",
        .code3          = "LAO",
        .num            = 418,
        .ename          = "Lao People's Democratic Republic" },
    {   .code2          = "LV",
        .code3          = "LVA",
        .num            = 428,
        .ename          = "Latvia" },
    {   .code2          = "LB",
        .code3          = "LBN",
        .num            = 422,
        .ename          = "Lebanon" },
    {   .code2          = "LS",
        .code3          = "LSO",
        .num            = 426,
        .ename          = "Lesotho" },
    {   .code2          = "LR",
        .code3          = "LBR",
        .num            = 430,
        .ename          = "Liberia" },
    {   .code2          = "LY",
        .code3          = "LBY",
        .num            = 434,
        .ename          = "Libya" },
    {   .code2          = "LI",
        .code3          = "LIE",
        .num            = 438,
        .ename          = "Liechtenstein" },
    {   .code2          = "LT",
        .code3          = "LTU",
        .num            = 440,
        .ename          = "Lithuania" },
    {   .code2          = "LU",
        .code3          = "LUX",
        .num            = 442,
        .ename          = "Luxembourg" },
    {   .code2          = "MO",
        .code3          = "MAC",
        .num            = 446,
        .ename          = "Macao" },
    {   .code2          = "MK",
        .code3          = "MKD",
        .num            = 807,
        .ename          = "Macedonia (the former Yugoslav Republic of)" },
    {   .code2          = "MG",
        .code3          = "MDG",
        .num            = 450,
        .ename          = "Madagascar" },
    {   .code2          = "MW",
        .code3          = "MWI",
        .num            = 454,
        .ename          = "Malawi" },
    {   .code2          = "MY",
        .code3          = "MYS",
        .num            = 458,
        .ename          = "Malaysia" },
    {   .code2          = "MV",
        .code3          = "MDV",
        .num            = 462,
        .ename          = "Maldives" },
    {   .code2          = "ML",
        .code3          = "MLI",
        .num            = 466,
        .ename          = "Mali" },
    {   .code2          = "MT",
        .code3          = "MLT",
        .num            = 470,
        .ename          = "Malta" },
    {   .code2          = "MH",
        .code3          = "MHL",
        .num            = 584,
        .ename          = "Marshall Islands" },
    {   .code2          = "MQ",
        .code3          = "MTQ",
        .num            = 474,
        .ename          = "Martinique" },
    {   .code2          = "MR",
        .code3          = "MRT",
        .num            = 478,
        .ename          = "Mauritania" },
    {   .code2          = "MU",
        .code3          = "MUS",
        .num            = 480,
        .ename          = "Mauritius" },
    {   .code2          = "YT",
        .code3          = "MYT",
        .num            = 175,
        .ename          = "Mayotte" },
    {   .code2          = "MX",
        .code3          = "MEX",
        .num            = 484,
        .ename          = "Mexico" },
    {   .code2          = "FM",
        .code3          = "FSM",
        .num            = 583,
        .ename          = "Micronesia (Federated States of)" },
    {   .code2          = "MD",
        .code3          = "MDA",
        .num            = 498,
        .ename          = "Moldova (Republic of)" },
    {   .code2          = "MC",
        .code3          = "MCO",
        .num            = 492,
        .ename          = "Monaco" },
    {   .code2          = "MN",
        .code3          = "MNG",
        .num            = 496,
        .ename          = "Mongolia" },
    {   .code2          = "ME",
        .code3          = "MNE",
        .num            = 499,
        .ename          = "Montenegro" },
    {   .code2          = "MS",
        .code3          = "MSR",
        .num            = 500,
        .ename          = "Montserrat" },
    {   .code2          = "MA",
        .code3          = "MAR",
        .num            = 504,
        .ename          = "Morocco" },
    {   .code2          = "MZ",
        .code3          = "MOZ",
        .num            = 508,
        .ename          = "Mozambique" },
    {   .code2          = "MM",
        .code3          = "MMR",
        .num            = 104,
        .ename          = "Myanmar" },
    {   .code2          = "NA",
        .code3          = "NAM",
        .num            = 516,
        .ename          = "Namibia" },
    {   .code2          = "NR",
        .code3          = "NRU",
        .num            = 520,
        .ename          = "Nauru" },
    {   .code2          = "NP",
        .code3          = "NPL",
        .num            = 524,
        .ename          = "Nepal" },
    {   .code2          = "NL",
        .code3          = "NLD",
        .num            = 528,
        .ename          = "Netherlands" },
    {   .code2          = "NC",
        .code3          = "NCL",
        .num            = 540,
        .ename          = "New Caledonia" },
    {   .code2          = "NZ",
        .code3          = "NZL",
        .num            = 554,
        .ename          = "New Zealand" },
    {   .code2          = "NI",
        .code3          = "NIC",
        .num            = 558,
        .ename          = "Nicaragua" },
    {   .code2          = "NE",
        .code3          = "NER",
        .num            = 562,
        .ename          = "Niger" },
    {   .code2          = "NG",
        .code3          = "NGA",
        .num            = 566,
        .ename          = "Nigeria" },
    {   .code2          = "NU",
        .code3          = "NIU",
        .num            = 570,
        .ename          = "Niue" },
    {   .code2          = "NF",
        .code3          = "NFK",
        .num            = 574,
        .ename          = "Norfolk Island" },
    {   .code2          = "MP",
        .code3          = "MNP",
        .num            = 580,
        .ename          = "Northern Mariana Islands" },
    {   .code2          = "NO",
        .code3          = "NOR",
        .num            = 578,
        .ename          = "Norway" },
    {   .code2          = "OM",
        .code3          = "OMN",
        .num            = 512,
        .ename          = "Oman" },
    {   .code2          = "PK",
        .code3          = "PAK",
        .num            = 586,
        .ename          = "Pakistan" },
    {   .code2          = "PW",
        .code3          = "PLW",
        .num            = 585,
        .ename          = "Palau" },
    {   .code2          = "PS",
        .code3          = "PSE",
        .num            = 275,
        .ename          = "Palestine, State of" },
    {   .code2          = "PA",
        .code3          = "PAN",
        .num            = 591,
        .ename          = "Panama" },
    {   .code2          = "PG",
        .code3          = "PNG",
        .num            = 598,
        .ename          = "Papua New Guinea" },
    {   .code2          = "PY",
        .code3          = "PRY",
        .num            = 600,
        .ename          = "Paraguay" },
    {   .code2          = "PE",
        .code3          = "PER",
        .num            = 604,
        .ename          = "Peru" },
    {   .code2          = "PH",
        .code3          = "PHL",
        .num            = 608,
        .ename          = "Philippines" },
    {   .code2          = "PN",
        .code3          = "PCN",
        .num            = 612,
        .ename          = "Pitcairn" },
    {   .code2          = "PL",
        .code3          = "POL",
        .num            = 616,
        .ename          = "Poland" },
    {   .code2          = "PT",
        .code3          = "PRT",
        .num            = 620,
        .ename          = "Portugal" },
    {   .code2          = "PR",
        .code3          = "PRI",
        .num            = 630,
        .ename          = "Puerto Rico" },
    {   .code2          = "QA",
        .code3          = "QAT",
        .num            = 634,
        .ename          = "Qatar" },
    {   .code2          = "RE",
        .code3          = "REU",
        .num            = 638,
        .ename          = "Réunion" },
    {   .code2          = "RO",
        .code3          = "ROU",
        .num            = 642,
        .ename          = "Romania" },
    {   .code2          = "RU",
        .code3          = "RUS",
        .num            = 643,
        .ename          = "Russian Federation" },
    {   .code2          = "RW",
        .code3          = "RWA",
        .num            = 646,
        .ename          = "Rwanda" },
    {   .code2          = "BL",
        .code3          = "BLM",
        .num            = 652,
        .ename          = "Saint Barthélemy" },
    {   .code2          = "SH",
        .code3          = "SHN",
        .num            = 654,
        .ename          = "Saint Helena, Ascension and Tristan da Cunha" },
    {   .code2          = "KN",
        .code3          = "KNA",
        .num            = 659,
        .ename          = "Saint Kitts and Nevis" },
    {   .code2          = "LC",
        .code3          = "LCA",
        .num            = 662,
        .ename          = "Saint Lucia" },
    {   .code2          = "MF",
        .code3          = "MAF",
        .num            = 663,
        .ename          = "Saint Martin (French part)" },
    {   .code2          = "PM",
        .code3          = "SPM",
        .num            = 666,
        .ename          = "Saint Pierre and Miquelon" },
    {   .code2          = "VC",
        .code3          = "VCT",
        .num            = 670,
        .ename          = "Saint Vincent and the Grenadines" },
    {   .code2          = "WS",
        .code3          = "WSM",
        .num            = 882,
        .ename          = "Samoa" },
    {   .code2          = "SM",
        .code3          = "SMR",
        .num            = 674,
        .ename          = "San Marino" },
    {   .code2          = "ST",
        .code3          = "STP",
        .num            = 678,
        .ename          = "Sao Tome and Principe" },
    {   .code2          = "SA",
        .code3          = "SAU",
        .num            = 682,
        .ename          = "Saudi Arabia" },
    {   .code2          = "SN",
        .code3          = "SEN",
        .num            = 686,
        .ename          = "Senegal" },
    {   .code2          = "RS",
        .code3          = "SRB",
        .num            = 688,
        .ename          = "Serbia" },
    {   .code2          = "SC",
        .code3          = "SYC",
        .num            = 690,
        .ename          = "Seychelles" },
    {   .code2          = "SL",
        .code3          = "SLE",
        .num            = 694,
        .ename          = "Sierra Leone" },
    {   .code2          = "SG",
        .code3          = "SGP",
        .num            = 702,
        .ename          = "Singapore" },
    {   .code2          = "SX",
        .code3          = "SXM",
        .num            = 534,
        .ename          = "Sint Maarten (Dutch part)" },
    {   .code2          = "SK",
        .code3          = "SVK",
        .num            = 703,
        .ename          = "Slovakia" },
    {   .code2          = "SI",
        .code3          = "SVN",
        .num            = 705,
        .ename          = "Slovenia" },
    {   .code2          = "SB",
        .code3          = "SLB",
        .num            = 90,
        .ename          = "Solomon Islands" },
    {   .code2          = "SO",
        .code3          = "SOM",
        .num            = 706,
        .ename          = "Somalia" },
    {   .code2          = "ZA",
        .code3          = "ZAF",
        .num            = 710,
        .ename          = "South Africa" },
    {   .code2          = "GS",
        .code3          = "SGS",
        .num            = 239,
        .ename          = "South Georgia and the South Sandwich Islands" },
    {   .code2          = "SS",
        .code3          = "SSD",
        .num            = 728,
        .ename          = "South Sudan" },
    {   .code2          = "ES",
        .code3          = "ESP",
        .num            = 724,
        .ename          = "Spain" },
    {   .code2          = "LK",
        .code3          = "LKA",
        .num            = 144,
        .ename          = "Sri Lanka" },
    {   .code2          = "SD",
        .code3          = "SDN",
        .num            = 729,
        .ename          = "Sudan" },
    {   .code2          = "SR",
        .code3          = "SUR",
        .num            = 740,
        .ename          = "Suriname" },
    {   .code2          = "SJ",
        .code3          = "SJM",
        .num            = 744,
        .ename          = "Svalbard and Jan Mayen" },
    {   .code2          = "SZ",
        .code3          = "SWZ",
        .num            = 748,
        .ename          = "Swaziland" },
    {   .code2          = "SE",
        .code3          = "SWE",
        .num            = 752,
        .ename          = "Sweden" },
    {   .code2          = "CH",
        .code3          = "CHE",
        .num            = 756,
        .ename          = "Switzerland" },
    {   .code2          = "SY",
        .code3          = "SYR",
        .num            = 760,
        .ename          = "Syrian Arab Republic" },
    {   .code2          = "TW",
        .code3          = "TWN",
        .num            = 158,
        .ename          = "Taiwan, Province of China" },
    {   .code2          = "TJ",
        .code3          = "TJK",
        .num            = 762,
        .ename          = "Tajikistan" },
    {   .code2          = "TZ",
        .code3          = "TZA",
        .num            = 834,
        .ename          = "Tanzania, United Republic of" },
    {   .code2          = "TH",
        .code3          = "THA",
        .num            = 764,
        .ename          = "Thailand" },
    {   .code2          = "TL",
        .code3          = "TLS",
        .num            = 626,
        .ename          = "Timor-Leste" },
    {   .code2          = "TG",
        .code3          = "TGO",
        .num            = 768,
        .ename          = "Togo" },
    {   .code2          = "TK",
        .code3          = "TKL",
        .num            = 772,
        .ename          = "Tokelau" },
    {   .code2          = "TO",
        .code3          = "TON",
        .num            = 776,
        .ename          = "Tonga" },
    {   .code2          = "TT",
        .code3          = "TTO",
        .num            = 780,
        .ename          = "Trinidad and Tobago" },
    {   .code2          = "TN",
        .code3          = "TUN",
        .num            = 788,
        .ename          = "Tunisia" },
    {   .code2          = "TR",
        .code3          = "TUR",
        .num            = 792,
        .ename          = "Turkey" },
    {   .code2          = "TM",
        .code3          = "TKM",
        .num            = 795,
        .ename          = "Turkmenistan" },
    {   .code2          = "TC",
        .code3          = "TCA",
        .num            = 796,
        .ename          = "Turks and Caicos Islands" },
    {   .code2          = "TV",
        .code3          = "TUV",
        .num            = 798,
        .ename          = "Tuvalu" },
    {   .code2          = "UG",
        .code3          = "UGA",
        .num            = 800,
        .ename          = "Uganda" },
    {   .code2          = "UA",
        .code3          = "UKR",
        .num            = 804,
        .ename          = "Ukraine" },
    {   .code2          = "AE",
        .code3          = "ARE",
        .num            = 784,
        .ename          = "United Arab Emirates" },
    {   .code2          = "GB",
        .code3          = "GBR",
        .num            = 826,
        .ename          = "United Kingdom of Great Britain and Northern Ireland" },
    {   .code2          = "US",
        .code3          = "USA",
        .num            = 840,
        .ename          = "United States of America" },
    {   .code2          = "UM",
        .code3          = "UMI",
        .num            = 581,
        .ename          = "United States Minor Outlying Islands" },
    {   .code2          = "UY",
        .code3          = "URY",
        .num            = 858,
        .ename          = "Uruguay" },
    {   .code2          = "UZ",
        .code3          = "UZB",
        .num            = 860,
        .ename          = "Uzbekistan" },
    {   .code2          = "VU",
        .code3          = "VUT",
        .num            = 548,
        .ename          = "Vanuatu" },
    {   .code2          = "VE",
        .code3          = "VEN",
        .num            = 862,
        .ename          = "Venezuela (Bolivarian Republic of)" },
    {   .code2          = "VN",
        .code3          = "VNM",
        .num            = 704,
        .ename          = "Viet Nam" },
    {   .code2          = "VG",
        .code3          = "VGB",
        .num            = 92,
        .ename          = "Virgin Islands (British)" },
    {   .code2          = "VI",
        .code3          = "VIR",
        .num            = 850,
        .ename          = "Virgin Islands (U.S.)" },
    {   .code2          = "WF",
        .code3          = "WLF",
        .num            = 876,
        .ename          = "Wallis and Futuna" },
    {   .code2          = "EH",
        .code3          = "ESH",
        .num            = 732,
        .ename          = "Western Sahara" },
    {   .code2          = "YE",
        .code3          = "YEM",
        .num            = 887,
        .ename          = "Yemen" },
    {   .code2          = "ZM",
        .code3          = "ZMB",
        .num            = 894,
        .ename          = "Zambia" },
    {   .code2          = "ZW",
        .code3          = "ZWE",
        .num            = 716,
        .ename          = "Zimbabwe" },
    {   .code2          = nullptr }
};

tau::Territory_data * find_data(const std::string & code) {
    std::string ucode = tau::str_toupper(code);

    for (tau::Territory_data * p = data_; p->code2; ++p) {
        if (ucode == p->code2 || ucode == p->code3) {
            return p;
        }
    }

    return data_;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace tau {

Territory::Territory(const std::string & iso_code):
    data(data_)
{
    if (iso_code.empty()) {
        if (locale_ptr_) {
            data = locale_ptr_->terr.data;
        }
    }

    else {
        data = find_data(iso_code);
    }
}

Territory::Territory(const Territory & other):
    data(other.data)
{
}

Territory & Territory::operator=(const Territory & other) {
    if (this != &other) {
        data = other.data;
    }

    return *this;
}

Territory::~Territory() {}

// static
Territory Territory::system() {
    return sys_locale_ptr_ ?  sys_locale_ptr_->terr : Territory();
}

bool Territory::operator==(const Territory & other) const {
    return data == other.data;
}

bool Territory::operator!=(const Territory & other) const {
    return data != other.data;
}

ustring Territory::name() const {
    return data->ename;
}

ustring Territory::name(const Language & lang) const {
    return data->ename;
}

std::string Territory::ename() const {
    return data->ename;
}

std::string Territory::code2() const {
    return data->code2;
}

std::string Territory::code3() const {
    if ('\0' != *data->code3) { return data->code3; }
    return code2();
}

int Territory::numeric_code() const {
    return data->num;
}

} // namespace tau

//END
