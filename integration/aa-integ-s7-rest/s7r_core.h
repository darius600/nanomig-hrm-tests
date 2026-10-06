// s7r_core.h -- shared machinery of aa-integ-s7-rest: a run that captures the
// ports, the access classification of SPECIFICATION.md R2, the line markers of
// R1, and the checking helpers.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace s7r {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

inline std::string g_dir = ".";
inline const char* g_mutate = nullptr;   // NMH_MUTATE: unset -> the normal run runs no mutant
inline bool mut(const char* id) { return g_mutate && std::strcmp(g_mutate, id) == 0; }

inline std::string dec(long long v) { char b[40]; std::snprintf(b, sizeof b, "%lld", v); return b; }
inline std::string num(double v) { char b[40]; std::snprintf(b, sizeof b, "%g", v); return b; }
inline std::string num3(double v) { char b[40]; std::snprintf(b, sizeof b, "%.3f", v); return b; }
inline std::string hex(unsigned v, int w = 4) { char b[24]; std::snprintf(b, sizeof b, "%0*X", w, v); return b; }

// ---- memory layout (SPECIFICATION.md section 2) ----------------------------
constexpr uint32_t kZ = 0x090000, kZEnd = 0x091000;
constexpr uint32_t kL = 0x091000, kLEnd = 0x09100C;
constexpr uint32_t kSdRec = 0x0C0000;
constexpr uint32_t kMk = 0x0C0004;
constexpr uint32_t kBuf = 0x0C0400;
constexpr uint32_t kFixture = 0x0C0800;
constexpr uint32_t kDone = 0x0C0FFE;
constexpr uint64_t kBootBudget = 4'000'000;

inline bool in_prog(uint32_t b) { return b >= 0x080000 && b < 0x090000; }
inline bool in_res(uint32_t b) { return b >= 0x0C0000 && b < 0x0C1000; }
inline bool in_stack(uint32_t b) { return b >= 0x0FF000 && b < 0x0FFF00; }

// ---- one access of the Chip-memory port ----------------------------------------
struct Acc {
    uint64_t t;      // clk_sys cycle of the first sample of the bus cycle
    uint32_t a;      // byte address
    uint16_t d;
    bool w;
    bool ref;        // the refresh output was high at the sample (R2)
};

enum Cls { C_REF, C_CPU, C_COP, C_ZDMA, C_UH };

inline Cls classify(const Acc& x) {
    if (x.ref) return C_REF;
    if (in_prog(x.a) || in_res(x.a) || in_stack(x.a)) return C_CPU;
    if (x.w && ((x.a >= kZ && x.a < kZEnd) || (x.a >= kL && x.a < kLEnd))) return C_CPU;
    if (!x.w && x.a >= kL && x.a < kLEnd) return C_COP;
    if (!x.w && x.a >= kZ && x.a < kZEnd) return C_ZDMA;
    return C_UH;
}

// ---- a run -------------------------------------------------------------------
struct RunSpec {
    std::string bin;
    uint8_t chipset = 0x10;           // chipset_config: AA
    uint8_t cpu_config = 0x00;
    uint64_t cycles = 3'500'000;      // clk_sys cycles run after boot() at most
    uint64_t tail = 3000;             // cycles run after the DONE write
};

struct Cap {
    bool loaded = false, released = false, done = false;
    uint64_t base = 0;                // cycle of the first refresh sample
    uint64_t done_cycle = 0;
    std::vector<uint8_t> refresh;     // refresh output, one per clk_sys cycle from base
    std::vector<Acc> acc;             // every bus cycle, in time order
    std::vector<uint16_t> res;        // words of 0x0C0000..0x0C0FFF after the run
    uint16_t fixture = 0;
};

// The refresh output is a state of the bus-cycle allocation; an access sampled one
// clk_sys cycle after the falling 7 MHz edge belongs to the allocation shown in the
// few cycles before the sample.
inline bool refresh_at(const Cap& c, uint64_t cycle) {
    if (cycle < c.base + 3) return false;
    uint64_t i = cycle - c.base;
    if (i >= c.refresh.size()) return false;
    for (int d = 0; d <= 3; ++d) if (c.refresh[i - d]) return true;
    return false;
}

inline Cap run(const RunSpec& s) {
    Cap c;
    nmh::Config cfg;
    cfg.chipset_config = s.chipset;
    cfg.cpu_config = s.cpu_config;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(g_dir + "/" + s.bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.base = h.cycles();
    c.refresh.reserve(s.cycles);
    size_t seen = h.ram_log().size();
    uint64_t done_at = 0;
    for (uint64_t i = 0; i < s.cycles; ++i) {
        h.run_cycles(1);
        c.refresh.push_back(h.ports()->refresh ? 1 : 0);
        if (!done_at && (i & 7) == 0) {
            const auto& lg = h.ram_log();
            for (; seen < lg.size(); ++seen)
                if (lg[seen].is_write && lg[seen].word_addr == kDone / 2 && lg[seen].data == 0x600D) {
                    done_at = i; c.done_cycle = lg[seen].cycle;
                }
        }
        if (done_at && i >= done_at + s.tail) break;
    }
    c.done = done_at != 0;
    c.fixture = h.peek_logical(kFixture).value_or(0);
    for (uint32_t a = 0x0C0000; a < 0x0C1000; a += 2) c.res.push_back(h.peek_logical(a).value_or(0));
    // the log without the held continuations of a stretched bus cycle
    const auto& lg = h.ram_log();
    std::map<uint64_t, int> hist;
    for (size_t i = 1; i < lg.size() && i < 4000; ++i) ++hist[lg[i].cycle - lg[i - 1].cycle];
    uint64_t period = 4; int best = 0;
    for (auto& kv : hist) if (kv.second > best && kv.first > 0 && kv.first <= 8) { best = kv.second; period = kv.first; }
    std::vector<bool> held = nmh::held_continuations(lg, period);
    for (size_t i = 0; i < lg.size(); ++i) {
        if (held[i]) continue;
        const auto& r = lg[i];
        c.acc.push_back({r.cycle, r.word_addr * 2, r.data, r.is_write, refresh_at(c, r.cycle)});
    }
    return c;
}

inline uint16_t word_at(const Cap& c, uint32_t byte_addr) {
    size_t i = (byte_addr - 0x0C0000) / 2;
    return i < c.res.size() ? c.res[i] : 0;
}

// ---- R1 markers ---------------------------------------------------------------
struct Markers {
    int vlo = 0, vhi = 0;
    std::vector<bool> have;
    std::vector<uint16_t> v, h;
    std::vector<uint64_t> t;          // port time of the store of h
};

inline Markers markers(const Cap& c, int vlo, int vhi, int step = 1) {
    Markers m; m.vlo = vlo; m.vhi = vhi;
    size_t n = size_t(vhi - vlo + 1);
    m.have.assign(n, false); m.v.assign(n, 0); m.h.assign(n, 0); m.t.assign(n, 0);
    (void)step;
    for (auto& x : c.acc) {
        if (!x.w || x.a < kMk || x.a >= kMk + 4 * n || (x.a & 2) == 0) continue;
        size_t i = (x.a - kMk) / 4;
        if (m.have[i]) continue;
        m.have[i] = true; m.t[i] = x.t;
        m.h[i] = x.d; m.v[i] = word_at(c, kMk + 4 * uint32_t(i));
    }
    return m;
}

// The checks at the foot of R1: a marker for every V; each marker's line number is V;
// each H field lies in [lo, lo+width); the times strictly increase.
struct MarkerAnchor { bool all = false, lines = false, window = false, mono = false; int count = 0; };
inline MarkerAnchor anchor_markers(const Markers& m, int glo = 0x60, int gw = 0x30) {
    MarkerAnchor r;
    r.all = true; r.lines = true; r.window = true; r.mono = true;
    for (size_t i = 0; i < m.have.size(); ++i) {
        if (!m.have[i]) { r.all = false; r.lines = r.window = r.mono = false; continue; }
        ++r.count;
        int line = ((m.v[i] & 7) << 8) | (m.h[i] >> 8);
        if (line != m.vlo + int(i)) r.lines = false;
        int hh = m.h[i] & 0xFF;
        if (hh < glo || hh >= glo + gw) r.window = false;
        if (i && m.have[i - 1] && m.t[i] <= m.t[i - 1]) r.mono = false;
    }
    return r;
}

// The line an access belongs to: V with t(V-1) < t < t(V), or -1.
inline int line_of(const Markers& m, uint64_t t) {
    for (size_t i = 0; i < m.t.size(); ++i) {
        if (!m.have[i]) continue;
        if (t < m.t[i]) {
            if (i == 0) return -1;
            if (!m.have[i - 1]) return -1;
            return t > m.t[i - 1] ? m.vlo + int(i) : -1;
        }
    }
    return -1;
}

inline std::string cap_core() { return NMH_XSTR(NANOMIG_CORE_NAME); }

} // namespace s7r
