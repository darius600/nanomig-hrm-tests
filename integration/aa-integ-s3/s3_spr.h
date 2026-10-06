// s3_spr.h -- sprite stimulus builders and output analysis shared by the
// sprite and collision propositions.

#pragma once

#include "s3_vid.h"

namespace s3 {

constexpr int kXs[8] = {0x90, 0xA8, 0xC0, 0xD8, 0xF0, 0x108, 0x120, 0x138};   // X_x = 0x90 + 0x18 * x

// the output lines of the tag lines of one frame
struct FrameView {
    const RunData* r = nullptr;
    Frame f;
    std::array<int, 0x100> L;
    bool ok = false;
    FrameView() { L.fill(-1); }
    FrameView(const RunData& run, int i) : r(&run), f(get_frame(run, i)) {
        L.fill(-1);
        ok = f.ok && f.tag.size() >= 200;
        if (!ok) return;
        for (int n = run.spec.tag_lo; n <= run.spec.tag_hi; ++n) L[n] = out_line(run, f, n);
    }
    int line(int n) const { return n >= 0 && n < 0x100 ? L[n] : -1; }
    std::vector<uint8_t> cls(int n, uint16_t colour) const { return is_col(*r, line(n), colour); }
};

// positions of the 1-runs of v: (start, length)
inline std::vector<std::pair<int, int>> runs_of(const std::vector<uint8_t>& v) {
    std::vector<std::pair<int, int>> out;
    size_t i = 0;
    while (i < v.size()) {
        if (!v[i]) { ++i; continue; }
        size_t j = i;
        while (j < v.size() && v[j]) ++j;
        out.push_back({int(i), int(j - i)});
        i = j;
    }
    return out;
}

inline int first_one(const std::vector<uint8_t>& v) {
    for (size_t i = 0; i < v.size(); ++i) if (v[i]) return int(i);
    return -1;
}
inline int count_in_range(const std::vector<uint8_t>& v, int a, int n) {   // ones in [a, a+n)
    int c = 0;
    for (int i = a; i < a + n; ++i) if (i >= 0 && size_t(i) < v.size() && v[i]) ++c;
    return c;
}

// the 16-bit word as a 32-pixel pattern at `edge` matches the 1s of v within [edge, edge+32)
inline bool pattern_at(const std::vector<uint8_t>& v, int edge, uint16_t word) {
    if (edge < 0 || size_t(edge + 32) > v.size()) return false;
    for (int b = 0; b < 16; ++b)
        for (int k = 0; k < 2; ++k)
            if (v[edge + 2 * b + k] != ((word >> (15 - b)) & 1)) return false;
    return true;
}

// a manual sprite's setup writes (2.8): POS, CTL, DATB, DATA in this order
inline void manual_load(std::vector<Wr>& v, int x, int X, uint16_t ctl, uint16_t datb, uint16_t data, int vstart = 0x40) {
    v.push_back({R_SPRxPOS(x), uint16_t((vstart << 8) | ((X >> 1) & 0xFF))});
    v.push_back({R_SPRxCTL(x), ctl});
    v.push_back({R_SPRxDATB(x), datb});
    v.push_back({R_SPRxDATA(x), data});
}

// the list region of sprite x: first access in a frame to [base, base + 0x20)
inline const Acc* first_list_read(const RunData& r, const Frame& f, uint32_t base) {
    return first_in(r, f.t0, f.t1, base, base + 0x20);
}

// sprite list reads of F: the byte addresses accessed in [base, base + len) in time order
inline std::vector<std::pair<uint64_t, uint32_t>> list_reads(const RunData& r, const Frame& f, uint32_t base, uint32_t len) {
    std::vector<std::pair<uint64_t, uint32_t>> v;
    for (size_t i = acc_from(r, f.t0); i < r.acc.size() && r.acc[i].cyc < f.t1; ++i) {
        const Acc& a = r.acc[i];
        uint32_t b = a.waddr * 2;
        if (!a.wr && b >= base && b < base + len) v.push_back({a.cyc, b});
    }
    return v;
}

}  // namespace s3
