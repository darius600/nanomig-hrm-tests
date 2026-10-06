// s3_vid.h -- reading output lines: pixel classes, pattern search, column spans.

#pragma once

#include "s3_pre.h"

namespace s3 {

// 1 where the output pixel (one video sample, 2.11) has colour c
inline std::vector<uint8_t> is_col(const RunData& r, int L, uint16_t c) {
    std::vector<uint8_t> v;
    if (L < 0 || size_t(L) >= r.lines.size()) return v;
    size_t b = r.lines[L].begin, e = std::min(r.lines[L].end, r.vid.size());
    v.reserve(e - b);
    for (size_t i = b; i < e; ++i) v.push_back(col_of(r.vid[i]) == c ? 1 : 0);
    return v;
}

// index of the first occurrence of needle in hay, -1 if none
inline int find_seq(const std::vector<uint8_t>& hay, const std::vector<uint8_t>& needle, size_t from = 0) {
    if (needle.empty() || hay.size() < needle.size()) return -1;
    for (size_t i = from; i + needle.size() <= hay.size(); ++i) {
        size_t j = 0;
        while (j < needle.size() && hay[i + j] == needle[j]) ++j;
        if (j == needle.size()) return int(i);
    }
    return -1;
}

// the bits of 16-bit words, MSB first, each bit `w` output pixels wide
inline std::vector<uint8_t> word_pixels(const std::vector<uint16_t>& words, int w = 2) {
    std::vector<uint8_t> v;
    for (uint16_t x : words)
        for (int b = 15; b >= 0; --b)
            for (int k = 0; k < w; ++k) v.push_back((x >> b) & 1);
    return v;
}

inline int count_ones(const std::vector<uint8_t>& v) { int n = 0; for (auto c : v) n += c; return n; }

// end of chip line n of frame f: the next tag, or the line period after the last tag
inline uint64_t line_t1(const Frame& f, int n, uint64_t P) {
    auto it = f.tag.find(n + 1);
    if (it != f.tag.end()) return it->second;
    return f.tag.at(n) + P;
}

}  // namespace s3
