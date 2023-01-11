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

#include <tau/buffer.hh>
#include <tau/exception.hh>
#include <tau/locale.hh>
#include <doc-impl.hh>
#include <fstream>
#include <iostream>

namespace {

const std::map<tau::ustring, tau::ustring> ent_ = {
    { "quot",       "\u0022" },     // XML 1.0
    { "amp",        "\u0026" },     // XML 1.0
    { "apos",       "\u0027" },     // XML 1.0
    { "lt",         "\u003c" },     // XML 1.0
    { "gt",         "\u003e" },     // XML 1.0
    { "nbsp",       "\u00a0" },     // HTML 3.2
    { "iexcl",      "\u00a1" },     // HTML 3.2
    { "cent",       "\u00a2" },     // HTML 3.2
    { "pound",      "\u00a3" },     // HTML 3.2
    { "curren",     "\u00a4" },     // HTML 3.2
    { "yen",        "\u00a5" },     // HTML 3.2
    { "brvbar",     "\u00a6" },     // HTML 3.2
    { "sect",       "\u00a7" },     // HTML 3.2
    { "uml",        "\u00a8" },     // HTML 3.2
    { "copy",       "\u00a9" },     // HTML 3.2
    { "laquo",      "\u00ab" },     // HTML 3.2
    { "not",        "\u00ac" },     // HTML 3.2
    { "shy",        "\u00ad" },     // HTML 3.2
    { "reg",        "\u00ae" },     // HTML 3.2
    { "macr",       "\u00af" },     // HTML 3.2
    { "deg",        "\u00b0" },     // HTML 3.2
    { "plusmn",     "\u00b1" },     // HTML 3.2
    { "sup2",       "\u00b2" },     // HTML 3.2
    { "sup3",       "\u00b3" },     // HTML 3.2
    { "acute",      "\u00b4" },     // HTML 3.2
    { "micro",      "\u00b5" },     // HTML 3.2
    { "para",       "\u00b6" },     // HTML 3.2
    { "middot",     "\u00b7" },     // HTML 3.2
    { "cedil",      "\u00b8" },     // HTML 3.2
    { "sup1",       "\u00b9" },     // HTML 3.2
    { "ordm",       "\u00ba" },     // HTML 3.2
    { "raquo",      "\u00bb" },     // HTML 3.2
    { "frac14",     "\u00bc" },     // HTML 3.2
    { "frac12",     "\u00bd" },     // HTML 3.2
    { "frac34",     "\u00be" },     // HTML 3.2
    { "iquest",     "\u00bf" },     // HTML 3.2
    { "Agrave",     "\u00c0" },     // HTML 2.0
    { "Aacute",     "\u00c1" },     // HTML 2.0
    { "Acirc",      "\u00c2" },     // HTML 2.0
    { "Atilde",     "\u00c3" },     // HTML 2.0
    { "Auml",       "\u00c4" },     // HTML 2.0
    { "Aring",      "\u00c5" },     // HTML 2.0
    { "AElig",      "\u00c6" },     // HTML 2.0
    { "Ccedil",     "\u00c7" },     // HTML 2.0
    { "Egrave",     "\u00c8" },     // HTML 2.0
    { "Eacute",     "\u00c9" },     // HTML 2.0
    { "Ecirc",      "\u00ca" },     // HTML 2.0
    { "Euml",       "\u00cb" },     // HTML 2.0
    { "Igrave",     "\u00cc" },     // HTML 2.0
    { "Iacute",     "\u00cd" },     // HTML 2.0
    { "Icirc",      "\u00ce" },     // HTML 2.0
    { "Iuml",       "\u00cf" },     // HTML 2.0
    { "ETH",        "\u00d0" },     // HTML 2.0
    { "Ntilde",     "\u00d1" },     // HTML 2.0
    { "Ograve",     "\u00d2" },     // HTML 2.0
    { "Oacute",     "\u00d3" },     // HTML 2.0
    { "Ocirc",      "\u00d4" },     // HTML 2.0
    { "Otilde",     "\u00d5" },     // HTML 2.0
    { "Ouml",       "\u00d6" },     // HTML 2.0
    { "times",      "\u00d7" },     // HTML 3.2
    { "Oslash",     "\u00d8" },     // HTML 2.0
    { "Ugrave",     "\u00d9" },     // HTML 2.0
    { "Uacute",     "\u00da" },     // HTML 2.0
    { "Ucirc",      "\u00db" },     // HTML 2.0
    { "Uuml",       "\u00dc" },     // HTML 2.0
    { "Yacute",     "\u00dd" },     // HTML 2.0
    { "THORN",      "\u00de" },     // HTML 2.0
    { "szlig",      "\u00df" },     // HTML 2.0
    { "agrave",     "\u00e0" },     // HTML 2.0
    { "aacute",     "\u00e1" },     // HTML 2.0
    { "acirc",      "\u00e2" },     // HTML 2.0
    { "atilde",     "\u00e3" },     // HTML 2.0
    { "auml",       "\u00e4" },     // HTML 2.0
    { "aring",      "\u00e5" },     // HTML 2.0
    { "aelig",      "\u00e6" },     // HTML 2.0
    { "ccedil",     "\u00e7" },     // HTML 2.0
    { "egrave",     "\u00e8" },     // HTML 2.0
    { "eacute",     "\u00e9" },     // HTML 2.0
    { "ecirc",      "\u00ea" },     // HTML 2.0
    { "euml",       "\u00eb" },     // HTML 2.0
    { "igrave",     "\u00ec" },     // HTML 2.0
    { "iacute",     "\u00ed" },     // HTML 2.0
    { "icirc",      "\u00ee" },     // HTML 2.0
    { "iuml",       "\u00ef" },     // HTML 2.0
    { "eth",        "\u00f0" },     // HTML 2.0
    { "ntilde",     "\u00f1" },     // HTML 2.0
    { "ograve",     "\u00f2" },     // HTML 2.0
    { "oacute",     "\u00f3" },     // HTML 2.0
    { "ocirc",      "\u00f4" },     // HTML 2.0
    { "otilde",     "\u00f5" },     // HTML 2.0
    { "ouml",       "\u00f6" },     // HTML 2.0
    { "divide",     "\u00f7" },     // HTML 3.2
    { "oslash",     "\u00f8" },     // HTML 2.0
    { "ugrave",     "\u00f9" },     // HTML 2.0
    { "uacute",     "\u00fa" },     // HTML 2.0
    { "ucirc",      "\u00fb" },     // HTML 2.0
    { "uuml",       "\u00fc" },     // HTML 2.0
    { "yacute",     "\u00fd" },     // HTML 2.0
    { "thorn",      "\u00fe" },     // HTML 2.0
    { "yuml",       "\u00ff" },     // HTML 2.0
    { "OElig",      "\u0152" },     // HTML 4.0
    { "oelig",      "\u0153" },     // HTML 4.0
    { "Scaron",     "\u0160" },     // HTML 4.0
    { "scaron",     "\u0161" },     // HTML 4.0
    { "Yuml",       "\u0178" },     // HTML 4.0
    { "fnof",       "\u0192" },     // HTML 4.0
    { "circ",       "\u02c6" },     // HTML 4.0
    { "Alpha",      "\u0391" },     // HTML 4.0
    { "Beta",       "\u0392" },     // HTML 4.0
    { "Delta",      "\u0394" },     // HTML 4.0
    { "Epsilon",    "\u0395" },     // HTML 4.0
    { "Zeta",       "\u0396" },     // HTML 4.0
    { "Eta",        "\u0397" },     // HTML 4.0
    { "Theta",      "\u0398" },     // HTML 4.0
    { "Iota",       "\u0399" },     // HTML 4.0
    { "Lambda",     "\u039b" },     // HTML 4.0
    { "Mu",         "\u039c" },     // HTML 4.0
    { "Nu",         "\u039d" },     // HTML 4.0
    { "Xi",         "\u039e" },     // HTML 4.0
    { "Omicron",    "\u039f" },     // HTML 4.0
    { "Pi",         "\u03a0" },     // HTML 4.0
    { "Rho",        "\u03a1" },     // HTML 4.0
    { "Sigma",      "\u03a3" },     // HTML 4.0
    { "Tau",        "\u03a4" },     // HTML 4.0
    { "Upsilon",    "\u03a5" },     // HTML 4.0
    { "Phi",        "\u03a6" },     // HTML 4.0
    { "Chi",        "\u03a7" },     // HTML 4.0
    { "Psi",        "\u03a8" },     // HTML 4.0
    { "Omega",      "\u03a9" },     // HTML 4.0
    { "alpha",      "\u03b1" },     // HTML 4.0
    { "beta",       "\u03b2" },     // HTML 4.0
    { "delta",      "\u03b4" },     // HTML 4.0
    { "epsilon",    "\u03b5" },     // HTML 4.0
    { "zeta",       "\u03b6" },     // HTML 4.0
    { "eta",        "\u03b7" },     // HTML 4.0
    { "theta",      "\u03b8" },     // HTML 4.0
    { "iota",       "\u03b9" },     // HTML 4.0
    { "lambda",     "\u03bb" },     // HTML 4.0
    { "mu",         "\u03bc" },     // HTML 4.0
    { "nu",         "\u03bd" },     // HTML 4.0
    { "xi",         "\u03be" },     // HTML 4.0
    { "omicron",    "\u03bf" },     // HTML 4.0
    { "pi",         "\u03b0" },     // HTML 4.0
    { "rho",        "\u03b1" },     // HTML 4.0
    { "sigmaf",     "\u03b2" },     // HTML 4.0
    { "sigma",      "\u03b3" },     // HTML 4.0
    { "tau",        "\u03b4" },     // HTML 4.0
    { "upsilon",    "\u03b5" },     // HTML 4.0
    { "phi",        "\u03b6" },     // HTML 4.0
    { "chi",        "\u03b7" },     // HTML 4.0
    { "psi",        "\u03b8" },     // HTML 4.0
    { "omega",      "\u03b9" },     // HTML 4.0
    { "thetasym",   "\u03d1" },     // HTML 4.0
    { "upsih",      "\u03d2" },     // HTML 4.0
    { "piv",        "\u03d6" },     // HTML 4.0
    { "ensp",       "\u2002" },     // HTML 4.0
    { "emsp",       "\u2003" },     // HTML 4.0
    { "thinsp",     "\u2009" },     // HTML 4.0
    { "zwnj",       "\u200c" },     // HTML 4.0
    { "zwj",        "\u200d" },     // HTML 4.0
    { "lrm",        "\u200e" },     // HTML 4.0
    { "rlm",        "\u200f" },     // HTML 4.0
    { "ndash",      "\u2013" },     // HTML 4.0
    { "mdash",      "\u2014" },     // HTML 4.0
    { "horbar",     "\u2015" },     // HTML 4.0
    { "lsquo",      "\u2018" },     // HTML 4.0
    { "rsquo",      "\u2019" },     // HTML 4.0
    { "sbquo",      "\u201a" },     // HTML 4.0
    { "ldquo",      "\u201c" },     // HTML 4.0
    { "rdquo",      "\u201d" },     // HTML 4.0
    { "bdquo",      "\u201e" },     // HTML 4.0
    { "dagger",     "\u2020" },     // HTML 4.0
    { "Dagger",     "\u2021" },     // HTML 4.0
    { "bull",       "\u2022" },     // HTML 4.0
    { "hellip",     "\u2026" },     // HTML 4.0
    { "permil",     "\u2030" },     // HTML 4.0
    { "prime",      "\u2032" },     // HTML 4.0
    { "Prime",      "\u2033" },     // HTML 4.0
    { "lsaquo",     "\u2039" },     // HTML 4.0
    { "rsaquo",     "\u203a" },     // HTML 4.0
    { "oline",      "\u203e" },     // HTML 4.0
    { "frasl",      "\u2044" },     // HTML 4.0
    { "euro",       "\u20ac" },     // HTML 4.0
    { "image",      "\u2111" },     // HTML 4.0
    { "weierp",     "\u2118" },     // HTML 4.0
    { "real",       "\u211c" },     // HTML 4.0
    { "trade",      "\u2122" },     // HTML 4.0
    { "alefsym",    "\u2135" },     // HTML 4.0
    { "larr",       "\u2190" },     // HTML 4.0
    { "uarr",       "\u2191" },     // HTML 4.0
    { "rarr",       "\u2192" },     // HTML 4.0
    { "darr",       "\u2193" },     // HTML 4.0
    { "crarr",      "\u21b5" },     // HTML 4.0
    { "lArr",       "\u21d0" },     // HTML 4.0
    { "uArr",       "\u21d1" },     // HTML 4.0
    { "rArr",       "\u21d2" },     // HTML 4.0
    { "dArr",       "\u21d3" },     // HTML 4.0
    { "hArr",       "\u21d4" },     // HTML 4.0
    { "forall",     "\u2200" },     // HTML 4.0
    { "part",       "\u2202" },     // HTML 4.0
    { "exist",      "\u2203" },     // HTML 4.0
    { "empty",      "\u2205" },     // HTML 4.0
    { "nabla",      "\u2207" },     // HTML 4.0
    { "isin",       "\u2208" },     // HTML 4.0
    { "notin",      "\u2209" },     // HTML 4.0
    { "ni",         "\u220b" },     // HTML 4.0
    { "prod",       "\u220f" },     // HTML 4.0
    { "sum",        "\u2211" },     // HTML 4.0
    { "minus",      "\u2212" },     // HTML 4.0
    { "lowast",     "\u2217" },     // HTML 4.0
    { "radic",      "\u221a" },     // HTML 4.0
    { "prop",       "\u221d" },     // HTML 4.0
    { "infin",      "\u221e" },     // HTML 4.0
    { "ang",        "\u2220" },     // HTML 4.0
    { "and",        "\u2227" },     // HTML 4.0
    { "or",         "\u2228" },     // HTML 4.0
    { "cap",        "\u2229" },     // HTML 4.0
    { "cup",        "\u222a" },     // HTML 4.0
    { "int",        "\u222b" },     // HTML 4.0
    { "there4",     "\u2234" },     // HTML 4.0
    { "sim",        "\u223c" },     // HTML 4.0
    { "cong",       "\u2245" },     // HTML 4.0
    { "asymp",      "\u2248" },     // HTML 4.0
    { "ne",         "\u2260" },     // HTML 4.0
    { "equiv",      "\u2261" },     // HTML 4.0
    { "le",         "\u2264" },     // HTML 4.0
    { "ge",         "\u2265" },     // HTML 4.0
    { "sub",        "\u2282" },     // HTML 4.0
    { "sup",        "\u2283" },     // HTML 4.0
    { "nsub",       "\u2284" },     // HTML 4.0
    { "sube",       "\u2286" },     // HTML 4.0
    { "supe",       "\u2287" },     // HTML 4.0
    { "oplus",      "\u2295" },     // HTML 4.0
    { "otimes",     "\u2297" },     // HTML 4.0
    { "perp",       "\u22a5" },     // HTML 4.0
    { "sdot",       "\u22c5" },     // HTML 4.0
    { "lceil",      "\u2308" },     // HTML 4.0
    { "rceil",      "\u2309" },     // HTML 4.0
    { "lfloor",     "\u230a" },     // HTML 4.0
    { "rfloor",     "\u230b" },     // HTML 4.0
    { "lang",       "\u2329" },     // HTML 4.0
    { "rang",       "\u232a" },     // HTML 4.0
    { "loz",        "\u25ca" },     // HTML 4.0
    { "spades",     "\u2660" },     // HTML 4.0
    { "clubs",      "\u2663" },     // HTML 4.0
    { "hearts",     "\u2665" },     // HTML 4.0
    { "diams",      "\u2666" }      // HTML 4.0
};

std::size_t count_lines(const tau::ustring & str) {
    std::size_t n = 0, len = str.size();

    if (0 != len) {
        std::size_t pos = 0;
        ++n;

        while (pos < len) {
            if (tau::char32_is_newline(str[pos])) {
                ++n;
                if (pos+1 < len && ((U'\x000d' == str[pos] && U'\x000a' == str[pos+1]) || (U'\x000a' == str[pos] && U'\x000d' == str[pos+1]))) { ++pos; }
            }

            ++pos;
        }
    }

    return n;
}

} // anonymous namespace

namespace tau {

void Doctype_impl::set_public(const ustring & name, const ustring & loc) {
    public_ = true;
    location_ = loc;
    ustring::size_type p = name.find("//"), q;

    if (p < name.size()) {
        ustring prefix = name.substr(0, p);
        p += 2;
        q = name.find("//", p);

        if (q < name.size()) {
            owner_ = name.substr(p, q-p);
            q += 2;
            p = name.find("//", q);

            if (p < name.size()) {
                description_ = name.substr(q, p-q);
                lang_ = name.substr(p+2);
            }
        }
    }
}

Doc_impl::Doc_impl():
    Doc()
{
}

// Overrides pure Doc.
Decl_element_ptr Doc_impl::decl_v() {
    return decl_;
}

// Overrides pure Doc.
Doctype_ptr Doc_impl::doctype_v() {
    return doctype_;
}

// Overrides pure Doc.
Node_element_ptr Doc_impl::root_v() {
    return root_;
}

// Overrides pure Doc.
std::list<Inst_element_ptr> Doc_impl::instrs_v() {
    return instrs_;
}

// static
Doc_ptr Doc::create() {
    return std::make_shared<Doc_impl>();
}

// static
Doc_ptr Doc::create_xml(bool standalone, const ustring & encoding, unsigned version_major, unsigned version_minor) {
    auto doc = std::make_shared<Doc_impl>();
    auto decl = Decl_element_ptr(new Decl_element);
    auto decl_data = static_cast<Decl_element_data *>(decl->pdata);
    decl_data->standalone = standalone;
    decl_data->encoding = encoding;
    decl_data->version_major = version_major;
    decl_data->version_minor = version_minor;
    doc->assign_decl(decl);
    return doc;
}

// static
bool Doc_impl::xml_element_name_valid(const ustring & name) {
    if (name.empty()) {
        return false;
    }

//     if (name.size() > 2 && "xml" == str_tolower(name.substr(0, 3))) {
//         return false;
//     }

    return true;
}

// static
bool Doc_impl::xml_attr_name_valid(const ustring & name) {
    if (name.empty()) {
        return false;
    }

    return true;
}

// Overrides pure Doc.
void Doc_impl::set_entity_v(const ustring & name, const ustring & value) {
    entities_[name] = value;
}

// Overrides pure Doc.
void Doc_impl::remove_entity_v(const ustring & name) {
    auto i = entities_.find(name);
    if (i != entities_.end()) { entities_.erase(i); }
}

// Overrides pure Doc.
ustring Doc_impl::entity_v(const ustring & name) const {
    auto i = entities_.find(name);
    if (i != entities_.end()) { return i->second; }
    auto j = ent_.find(name);
    if (j != ent_.end()) { return j->second; }
    return ustring();
}

// Overrides pure Doc.
bool Doc_impl::has_entity_v(const ustring & name) const {
    auto i = entities_.find(name);
    if (i != entities_.end()) { return true; }
    auto j = ent_.find(name);
    return j != ent_.end();
}

void Doc_impl::expand_entities(ustring & s) {
    ustring::size_type pos = 0, len = s.size(), begin, end;

    while (pos < len) {
        begin = s.find(U'&', pos);
        if (ustring::npos == begin) { break; }
        end = s.find(U';', begin+1);
        if (ustring::npos == end) { ++pos; continue; }

        if (U'#' == s[begin+1]) {
            char32_t a = 0;

            if (U'x' == s[begin+2] || U'X' == s[begin+2]) {
                for (pos = begin+3; pos < end; ++pos) {
                    char32_t wc = s[pos];

                    if ((wc >= U'0' && wc <= U'9') || (wc >= U'F' && wc <= U'F') || (wc >= U'a' && wc <= U'f')) {
                        a <<= 4;
                        a += wc <= U'9' ? wc-U'0' : (wc <= U'F' ? wc-U'A'+0x10 : wc-U'a'+0x10);
                    }

                    else {
                        break;
                    }
                }
            }

            else {
                for (pos = begin+2; pos < end; ++pos) {
                    char32_t wc = s[pos];

                    if (wc >= U'0' && wc <= U'9') {
                        a *= 10;
                        a += wc-U'0';
                    }

                    else {
                        break;
                    }
                }
            }

            s.replace(begin, end-begin+1, 1, a);
            pos = begin+1;
            len = s.size();
        }

        // Named entity.
        else {
            ustring evalue = entity_v(s.substr(begin+1, end-begin-1));

            if (!evalue.empty()) {
                s.replace(begin, end-begin+1, evalue);
                pos = begin+1;
                len = s.size();
            }

            else {
                pos = end+1;
            }
        }
    }
}

// Overrides pure Doc.
void Doc_impl::load_v(Buffer buffer) {
    std::list<Node_element_ptr> nodes;
    Buffer_iter c;

    for (c = buffer.begin(); c != buffer.end(); ) {
        if (nodes.empty()) { c.skip_whitespace(); }

        if (!c.eof()) {
            if ('<' != *c) {
                if (nodes.empty()) { throw bad_doc(str_format(1+c.row(), ':', 1+c.col(), ": XML syntax error")); }
                Buffer_iter b(c), d(c);
                c.find('<');
                d.skip_whitespace();

                if (d < c) {
                    ustring s = b.peek(c);
                    expand_entities(s);
                    nodes.back()->append_text(s);
                }
            }

            else {
                ++c;

                if ('?' == *c) {
                    ++c;
                    Buffer_iter e(c), b(c), d(c);
                    if (!c.find("?>")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated XML processing instruction")); }
                    while (!char32_isblank(*d) && d < c) { ++d; }
                    if (b == d) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": XML processing instruction syntax error")); }
                    ustring iname = b.peek(d);
                    std::list<std::pair<ustring, ustring>> attrs;

                    while (d < c) {
                        d.skip_blanks();
                        b = d;
                        if (!d.find('=', c)) { break; }

                        if (b < d) {
                            ustring attr_name = b.peek(d++);

                            if ('"' == *d) {
                                ++d;
                                b = d;

                                if (d.find('"', c)) {
                                    ustring attr_value = b.peek(d++);
                                    attrs.push_back(std::pair<ustring, ustring>(attr_name, attr_value));
                                }
                            }
                        }
                    }

                    if ("xml" == iname) {
                        if (root_) { throw bad_doc(str_format(1+e.row(), ':', 1+e.col(), ": XML declaration must preceed root element")); }
                        decl_ = Decl_element_ptr(new Decl_element);
                        auto decl_data = static_cast<Decl_element_data *>(decl_->pdata);

                        for (auto & p: attrs) {
                            if ("encoding" == p.first) {
                                decl_data->encoding = p.second;
                            }

                            else if ("version" == p.first) {
                                auto v = str_explode(p.second, '.');
                                if (!v.empty()) { decl_data->version_major = std::stoi(v[0]); }
                                if (v.size() > 1) { decl_data->version_minor = std::stoi(v[1]); }
                            }

                            else if ("standalone" == p.first) {
                                decl_data->standalone = ("yes" == p.second);
                            }
                        }
                    }

                    else {
                        auto instr = Inst_element_ptr(new Inst_element);
                        auto instr_data = static_cast<Inst_element_data *>(instr->pdata);
                        instr_data->name = iname;
                        for (auto & p: attrs) { instr->set_attribute(p.first, p.second); }

                        if (nodes.empty()) {
                            instrs_.push_back(instr);
                        }

                        else {
                            Node_element_data * ndata = static_cast<Node_element_data *>(nodes.back()->pdata);
                            ndata->elems.push_back(instr);
                        }
                    }

                    c += 2;
                }

                else if (c.text_cmp_adv("!--")) {
                    Buffer_iter b(c);
                    if (!c.find("-->")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated comment")); }
                    c += 3;
                }

                else if (c.text_cmp_adv("![CDATA[")) {
                    if (nodes.empty()) { throw bad_doc(str_format(1+c.row(), ':', 1+c.col(), ": CDATA outside of node")); }
                    const Buffer_iter b(c);
                    if (!c.find("]]>")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated CDATA section")); }
                    nodes.back()->append_text(b.peek(c));
                    c += 3;
                }

                else if (c.text_cmp_adv("!DOCTYPE")) {
                    c.skip_whitespace();
                    Buffer_iter b(c);

                    // Root element must be here.
                    const ustring delimiters = "['\">";
                    while (!char32_isblank(*c) && !char32_is_newline(*c) && ustring::npos == delimiters.find(*c)) { ++c; }
                    if (c == b) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": missing DOCTYPE name")); }
                    auto obj = std::shared_ptr<Doctype_impl>(new Doctype_impl(b.peek(c)));
                    doctype_ = obj;

                    // After root element.
                    c.skip_whitespace();
                    b = c;

                    // Private DTD.
                    if (c.text_cmp_adv("SYSTEM")) {
                        // DTD location required (quoted string).
                        c.skip_whitespace();
                        char32_t wc = *c++;
                        b = c;
                        if ((U'\'' != wc && U'"' != wc) || !c.find_first_of(ustring(">[")+wc) || wc != *c) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": missing private DTD location")); }
                        obj->set_private(b.peek(c++));
                        c.skip_whitespace();
                    }

                    // Public DTD.
                    else if (c.text_cmp_adv("PUBLIC")) {
                        // DTD name required (quoted string).
                        c.skip_whitespace();
                        char32_t wc = *c++;
                        b = c;
                        if ((U'\'' != wc && U'"' != wc) || !c.find_first_of(ustring(">[")+wc) || wc != *c) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": missing public DTD name")); }
                        ustring name = b.peek(c++);

                        // DTD location required (quoted string).
                        c.skip_whitespace();
                        wc = *c++;
                        b = c;
                        if ((U'\'' != wc && U'"' != wc) || !c.find_first_of(ustring(">[")+wc) || wc != *c) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": missing public DTD location")); }
                        obj->set_public(name, b.peek(c++));
                        c.skip_whitespace();
                    }

                    if ('[' == *c) {
                        b = c;
                        bool done = false;

                        while (!done) {
                            if (!c.find_first_of("<]\"'")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated DOCTYPE section")); }

                            if (c.text_cmp_adv("<!--")) {
                                if (!c.find("-->")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated comment")); }
                                c.skip_whitespace();
                            }

                            else if ('<' == *c) {
                                ++c;
                            }

                            if ('\'' == *c || '"' == *c) {
                                Buffer_iter d(c);
                                if (!c.find(*c++)) { throw bad_doc(str_format(1+d.row(), ':', 1+d.col(), ": missing closing quote in DOCTYPE section")); }
                                ++c;
                                if (!c.find_first_of("<]\"'")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated DOCTYPE section")); }
                            }

                            if (']' == *c) {
                                ++c;
                                done = true;
                            }
                        }
                    }

                    c.skip_whitespace();
                    if ('>' != *c) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated DOCTYPE section")); }
                    ++c;
                }

                // Element definition, get here with c set after '<' character.
                else {
                    Buffer_iter b(c);
                    c.skip_whitespace();

                    // Check if it is leading slash present and skip it.
                    bool close = false;
                    if ('/' == *c) { close = true; ++c; }

                    // Extract element name.
                    ustring name;
                    while ('/' != *c && '>' != *c && !char32_isblank(*c) && !char32_is_newline(*c)) { name += *c++; }
                    if (!xml_element_name_valid(name)) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": invalid XML ELEMENT name")); }

                    if (!close) {
                        Node_element_ptr node;

                        if (nodes.empty()) {
                            node = Node_element_ptr(new Node_element(name));
                            if (!root_) { root_ = node; }
                        }

                        else {
                            node = nodes.back()->append_node(name);
                        }

                        bool attrs_done = false;

                        while (!attrs_done) {
                            c.skip_whitespace();
                            Buffer_iter d(c);
                            if (!c.find_first_of("=/>")) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated XML ELEMENT ", name, " definition")); }
                            if ('=' != *c) { attrs_done = true; continue; }
                            ustring attr_name = d.peek(c++);
                            if (!xml_attr_name_valid(attr_name))  { throw bad_doc(str_format(1+d.row(), ':', 1+d.col(), ": invalid XML ATTRIBUTE name '", attr_name, "'")); }

                            // Move to the opening quote.
                            c.skip_whitespace();
                            d = c;
                            if ('"' != *c && '\'' != *c) { throw bad_doc(str_format(1+d.row(), ':', 1+d.col(), ": need opening quote for attribute ", attr_name)); }
                            char32_t qchar = *c++;
                            d = c;

                            // Move to the closing quote.
                            if (!c.find(qchar)) { throw bad_doc(str_format(1+d.row(), ':', 1+d.col(), ": need closing quote for attribute ", attr_name)); }
                            ustring attr_value = d.peek(c++);
                            node->set_attribute(attr_name, attr_value);
                        }

                        // Check if trailing slash present.
                        if ('/' != *c) { nodes.push_back(node); }
                    }

                    else {
                        if (nodes.empty()) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": misplaced XML ELEMENT '", name, "' closure")); }
                        if (name != nodes.back()->name()) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": misplaced XML ELEMENT '", name, "' closure")); }
                        nodes.pop_back();
                    }

                    // Ensure we are at '>' character now.
                    if ('>' != *c && !c.find('>')) { throw bad_doc(str_format(1+b.row(), ':', 1+b.col(), ": unterminated XML ELEMENT ", name, " definition")); }
                    ++c;
                }
            }
        }
    }

    if (!nodes.empty()) {
        throw bad_doc(str_format(1+c.row(), ':', 1+c.col(), ": unterminated ELEMENT"));
    }
}

void Doc_impl::save_element(Element_ptr elem, Buffer buf, unsigned indent, unsigned indent_size) const {
    if (auto node = std::dynamic_pointer_cast<Node_element>(elem)) {
        buf.insert(buf.end(), U' ', indent);
        buf.insert(buf.end(), ustring("<")+node->name());

        if (node->has_attributes()) {
            for (const ustring & attr_name: node->list_attributes()) {
                buf.insert(buf.end(), ustring(" ")+attr_name+"=\""+node->attribute(attr_name)+"\"");
            }
        }

        auto v = node->elements();

        if (v.empty()) {
            buf.insert(buf.end(), "/>\n");
        }

        else {
            if (1 == v.size()) {
                if (auto txt = std::dynamic_pointer_cast<Text_element>(v.front())) {
                    ustring text = txt->text();

                    if (1 == count_lines(text)) {
                        buf.insert(buf.end(), ustring(">")+text+"</"+node->name()+">\n");
                        return;
                    }
                }
            }

            buf.insert(buf.end(), ">\n");
            for (auto e: v) { save_element(e, buf, indent+indent_size, indent_size); }
            buf.insert(buf.end(), U' ', indent);
            buf.insert(buf.end(), ustring("</")+node->name()+">\n");
        }
    }

    else if (auto txt = std::dynamic_pointer_cast<Text_element>(elem)) {
        auto v = str_explode(txt->text());

        if (!v.empty()) {
            buf.insert(buf.end(), U' ', indent);
            buf.insert(buf.end(), v[0]);
            if (v.size() > 1) { buf.insert(buf.end(), U'\n'); }

            for (std::size_t i = 1; i < v.size(); ++i) {
                buf.insert(buf.end(), U' ', indent);
                buf.insert(buf.end(), v[i]+"\n");
            }
        }
    }
}

// Overrides pure Doc.
void Doc_impl::save_v(Buffer buf, unsigned indent_size) const {
    if (decl_) {
        buf.insert(buf.end(), str_format("<?xml version=\"", decl_->version_major(), '.', decl_->version_minor(), "\" "));
        if (!decl_->encoding().empty()) { buf.insert(buf.end(), ustring("encoding=\"")+decl_->encoding()+"\" "); }
        buf.insert(buf.end(), ustring("standalone=\"")+(decl_->standalone() ? "yes" : "no")+"\"?>\n\n");
    }

    save_element(root_, buf, 0, indent_size);
}

// Overrides pure Doc.
Node_element_ptr Doc_impl::create_root_v(const ustring & root_name) {
    if (root_) { throw bad_doc("Document already has root element"); }
    root_ = Node_element_ptr(new Node_element(root_name));
    return root_;
}

} // namespace tau

//END
