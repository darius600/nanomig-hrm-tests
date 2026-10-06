// s7r_hv.h -- run HV, the conversion from clk_sys cycles to memory cycles
// (SPECIFICATION.md section 2), and helpers shared by the propositions.
#pragma once

#include "s7r_core.h"

namespace s7r {

// Graded line with its preconditions: NOT GRADED when they do not hold.
inline void graded(Checks& c, bool pre, bool cond, const std::string& label, const std::string& unmet) {
    if (!pre) {
        c.record("NOT GRADED " + label.substr(0, label.find_first_of(" :")) + ": precondition(s) not met: " + unmet);
        return;
    }
    c.expect(cond, label);
}

// The samples a run stored at BUF + 2i and the port times of those stores.
struct Samples {
    std::vector<uint16_t> r;
    std::vector<uint64_t> t;
    bool full = false;
};

inline Samples samples(const Cap& c, int n = 64) {
    Samples s;
    s.r.assign(size_t(n), 0); s.t.assign(size_t(n), 0);
    std::vector<bool> have(size_t(n), false);
    int got = 0;
    for (auto& x : c.acc) {
        if (!x.w || x.a < kBuf || x.a >= kBuf + 2 * uint32_t(n)) continue;
        size_t i = (x.a - kBuf) / 2;
        if (have[i]) continue;
        have[i] = true; s.r[i] = x.d; s.t[i] = x.t; ++got;
    }
    s.full = got == n;
    return s;
}

// Run HV: 64 reads of VHPOSR stored at BUF + 2i. For every pair of consecutive samples
// with the same V field (bits 15-8) the H field (bits 7-0) advances by dH memory cycles
// while the stores advance by dT clk_sys cycles. K = sum dT / sum dH. A pair whose H field
// decreased straddles a line start and is not a pair of one line; it is counted apart.
struct Conv {
    bool ok = false;           // the anchor: at least 16 pairs, each with dH >= 1
    int pairs = 0;
    int wraps = 0;             // same-V pairs whose H field decreased (a line start between the reads)
    bool each_pos = false;
    double K = 0;              // clk_sys cycles per memory cycle
    std::string why;
};

inline Conv conversion(const Cap& c) {
    Conv v;
    Samples s = samples(c);
    if (!s.full) { v.why = "fewer than 64 samples"; return v; }
    double sdt = 0, sdh = 0;
    v.each_pos = true;
    for (size_t i = 0; i + 1 < s.r.size(); ++i) {
        if ((s.r[i] >> 8) != (s.r[i + 1] >> 8)) continue;
        int dh = int(s.r[i + 1] & 0xFF) - int(s.r[i] & 0xFF);
        if (dh < 0) { ++v.wraps; continue; }   // the H field wrapped: the pair straddles a line start
        ++v.pairs;
        if (dh < 1) v.each_pos = false;
        sdt += double(s.t[i + 1] - s.t[i]);
        sdh += dh;
    }
    v.ok = v.pairs >= 16 && v.each_pos && sdh > 0;
    if (v.ok) v.K = sdt / sdh;
    return v;
}

inline Cap runbin(const std::string& bin, uint64_t cycles = 7'000'000) {
    RunSpec rs;
    rs.bin = bin;
    rs.cycles = cycles;
    return run(rs);
}

inline bool fixture_ok(const Cap& c) { return c.loaded && c.released && c.fixture == 0xBEEF; }
inline bool run_ok(const Cap& c) { return fixture_ok(c) && c.done; }

// The accesses of class `k` with t in [t0, t1].
inline std::vector<Acc> select(const Cap& c, Cls k, uint64_t t0 = 0, uint64_t t1 = ~0ull) {
    std::vector<Acc> o;
    for (auto& x : c.acc) if (x.t >= t0 && x.t <= t1 && classify(x) == k) o.push_back(x);
    return o;
}

} // namespace s7r
