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

#include <tau/base64.hh>

namespace {

const char base64_alphabet_[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const uint8_t mime_base64_rank_[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
    52,   53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255,   0, 255, 255,
    255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
    15,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
    41,   42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

} // anonymous namespace

namespace tau {

Base64_encoder::Base64_encoder(bool break_lines):
    break_lines_(break_lines)
{
    nchars_ = 0;
}

void Base64_encoder::encode(char c) {
    acc_.push_back(c);

    if (3 == acc_.size()) {
        out_ += base64_alphabet_[acc_[0] >> 2];
        out_ += base64_alphabet_[acc_[1] >> 4 | ((acc_[0] & 0x03) << 4)];
        out_ += base64_alphabet_[((acc_[1] & 0x0f) << 2) | (acc_[2] >> 6)];
        out_ += base64_alphabet_[acc_[2] & 0x3f];
        acc_.clear();
        nchars_ += 4;
        if (break_lines_ && nchars_ >= 72) { nchars_ = 0; out_ += "\x0d\x0a"; }
    }
}

void Base64_encoder::encode(const char * src, std::size_t nbytes) {
    while (nbytes--) {
        encode(*src++);
    }
}

std::string Base64_encoder::result() const {
    std::size_t abytes = acc_.size();

    if (0 == abytes) {
        return out_;
    }

    std::string result(out_);

    if (1 == abytes) {
        result += base64_alphabet_[acc_[0] >> 2];
        result += base64_alphabet_[(acc_[0] & 0x03) << 4];
        result.append(2, '=');
    }

    else {
        result += base64_alphabet_[acc_[0] >> 2];
        result += base64_alphabet_[acc_[1] >> 4 | ((acc_[0] & 0x03) << 4)];
        result += base64_alphabet_[((acc_[1] & 0x0f) << 2)];
        result += '=';
    }

    return result;
}

void Base64_encoder::reset() {
    out_.clear();
    acc_.clear();
    nchars_ = 0;
}

Base64_decoder::Base64_decoder() {
}

void Base64_decoder::decode(char c) {
    if (c > 0) {
        uint8_t rank = mime_base64_rank_[uint8_t(c)];

        if ('=' == c || 255 != rank) {
            acc_ += c;
            v_ <<= 6;
            v_ |= rank;

            if (4 == acc_.size()) {
                out_.push_back(v_ >> 16);
                if ('=' != acc_[2]) { out_.push_back(v_ >> 8); }
                if ('=' != acc_[3]) { out_.push_back(v_); }
                acc_.clear();
            }
        }
    }
}

std::vector<char> Base64_decoder::result() const {
    std::vector<char> v = out_;
    std::size_t n = acc_.size();

    if (3 == n) {
        if ('=' != acc_[2]) { v.push_back(v_ >> 8); }
        if ('=' != acc_[3]) { v.push_back(v_); }
    }

    else if (2 == n) {
        if ('=' != acc_[3]) { v.push_back(v_); }
    }

    return v;
}

void Base64_decoder::decode(const std::string & src) {
    for (char c: src) {
        decode(c);
    }
}

void Base64_decoder::decode(const char * src) {
    while ('\0' != *src) {
        decode(*src++);
    }
}

void Base64_decoder::reset() {
    v_ = 0;
    acc_.clear();
    out_.clear();
}

} // namespace tau

//END
