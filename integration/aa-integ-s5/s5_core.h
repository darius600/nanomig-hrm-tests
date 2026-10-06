// s5_core.h -- shared machinery of aa-integ-s5: the script of prog_run.s, a run
// that captures the ports, and the reference frames of SPECIFICATION.md
// section 2 (colour clock, markers and log, fetches, output runs).
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
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

namespace s5 {

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

std::string g_dir = ".";
const char* g_mutate = nullptr; // NMH_MUTATE: unset -> the normal run runs no mutant
bool mut(const char* id) { return g_mutate && std::strcmp(g_mutate, id) == 0; }

std::string dec(long long v) { char b[40]; std::snprintf(b, sizeof b, "%lld", v); return b; }
std::string num(double v) { char b[40]; std::snprintf(b, sizeof b, "%g", v); return b; }
std::string num2(double v) { char b[40]; std::snprintf(b, sizeof b, "%.2f", v); return b; }
std::string hex4(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%04X", v & 0xFFFF); return b; }
std::string hex5(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%05X", v & 0xFFFFF); return b; }

// ---- memory map (SPECIFICATION.md 2.1) -------------------------------------
constexpr uint32_t kScript  = 0x0B0000;
constexpr uint32_t kMarks   = 0x0B8000;
constexpr uint32_t kResults = 0x0C0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint32_t kLog     = 0x100000;
constexpr uint32_t kBuf[4]  = {0x0D0000, 0x0D2000, 0x0D4000, 0x0D6000};
constexpr uint64_t kBootBudget = 4'000'000;

// custom register offsets (AA pp. 6-12, 27; HRM Appendix B)
enum : uint16_t {
    DMACONR = 0x002, VHPOSR = 0x006, ADKCONR = 0x010, SERDATR = 0x018, INTENAR = 0x01C, INTREQR = 0x01E,
    SERDAT = 0x030, SERPER = 0x032, DMACON = 0x096, INTENA = 0x09A, INTREQ = 0x09C, ADKCON = 0x09E,
    AUD0LC = 0x0A0, AUD0LEN = 0x0A4, AUD0PER = 0x0A6, AUD0VOL = 0x0A8, AUD0DAT = 0x0AA,
};
inline uint16_t audreg(int ch, uint16_t base0) { return uint16_t(base0 + 0x10 * ch); }
inline uint16_t audlc(int ch)  { return audreg(ch, 0x0A0); }
inline uint16_t audlen(int ch) { return audreg(ch, 0x0A4); }
inline uint16_t audper(int ch) { return audreg(ch, 0x0A6); }
inline uint16_t audvol(int ch) { return audreg(ch, 0x0A8); }
inline uint16_t auddat(int ch) { return audreg(ch, 0x0AA); }

struct Poke { uint32_t byte_addr; uint16_t value; };

// ---- script builder --------------------------------------------------------
struct Script {
    std::vector<uint16_t> w;
    int marks = 0;               // markers stored so far
    int w16(uint16_t reg, uint16_t v) {
        w.insert(w.end(), {1, reg, v}); int m = marks + 1; marks += 2; return m;
    }
    int w32(uint16_t reg, uint32_t v) {
        w.insert(w.end(), {2, reg, uint16_t(v >> 16), uint16_t(v & 0xFFFF)}); int m = marks + 1; marks += 2; return m;
    }
    void dly(unsigned n) { w.insert(w.end(), {3, uint16_t(n)}); }
    void rd(uint16_t reg) { w.insert(w.end(), {4, reg}); }
    void waitb(uint16_t reg, uint16_t mask, uint16_t tmo) { w.insert(w.end(), {5, reg, mask, tmo}); }
    void poll(uint16_t reg, unsigned n) { w.insert(w.end(), {6, reg, uint16_t(n)}); }
    int mark() { w.push_back(8); return ++marks; }
    int w8(uint32_t addr, uint8_t v) {
        w.insert(w.end(), {9, uint16_t(addr >> 16), uint16_t(addr & 0xFFFF), v}); int m = marks + 1; marks += 2; return m;
    }
    void rd3(uint16_t a, uint16_t b) { w.insert(w.end(), {10, a, b}); }
    int wm(uint32_t addr, uint16_t v) {
        w.insert(w.end(), {11, uint16_t(addr >> 16), uint16_t(addr & 0xFFFF), v}); int m = marks + 1; marks += 2; return m;
    }
    // n+1 reads of reg; after each read with (value & mask) != 0 the loop writes cval to creg
    void pact(uint16_t reg, uint16_t mask, unsigned n, uint16_t creg, uint16_t cval) {
        w.insert(w.end(), {7, reg, mask, uint16_t(n), creg, cval});
    }
    // reads of reg until nhits have had (value & mask) != 0; after each the loop writes cval to INTREQ and the
    // next of `words` to SERDAT; at most tmo+1 reads without a hit
    void feed(uint16_t reg, uint16_t mask, uint16_t tmo, uint16_t cval, const std::vector<uint16_t>& words, bool storm = false) {
        uint32_t resume = kScript + uint32_t(2 * (w.size() + 8 + words.size()));
        w.insert(w.end(), {uint16_t(storm ? 13 : 12), reg, mask, uint16_t(words.size()), tmo, cval, uint16_t(resume >> 16), uint16_t(resume & 0xFFFF)});
        w.insert(w.end(), words.begin(), words.end());
    }
    void end() { w.push_back(0); }
};

// ---- a run -------------------------------------------------------------------
struct RunSpec {
    std::string bin = "prog_run.bin";
    Script sc;
    std::vector<Poke> pokes;
    uint64_t cycles = 3'000'000;     // clk_sys cycles run after boot() at most
    bool sample_txd = false;
    uint8_t chipset = 0x10;          // chipset_config: AA
    uint8_t cpu_config = 0x00;
    std::function<void(nmh::NanomigHarness&, uint64_t)> drive;   // before each cycle (absolute cycle)
    uint64_t tail = 4000;            // cycles run after the DONE write
};

struct Cap {
    bool loaded = false, released = false, fixture_ok = false, done = false;
    uint64_t base = 0;                          // first cycle of refresh/txd/audio
    std::vector<nmh::RamAccess> ram;            // whole log
    std::vector<nmh::AudioSample> audio;        // from `base`
    std::vector<uint8_t> refresh, txd;
    uint16_t timeouts = 0;
    std::vector<uint64_t> mark_t;               // mark_t[i-1] = cycle of marker i (0 if absent)
    std::vector<std::pair<uint64_t, uint16_t>> log;   // (cycle, value) of every log store
    uint64_t done_cycle = 0;
};

inline void poke_words(std::vector<Poke>& v, uint32_t addr, const std::vector<uint16_t>& ws) {
    for (size_t i = 0; i < ws.size(); ++i) v.push_back({addr + uint32_t(2 * i), ws[i]});
}

// Scans the RAM log (first entry of each stretched bus cycle) for markers and log stores.
inline void scan_ram(Cap& c) {
    c.mark_t.assign(8192, 0);
    uint64_t pc = ~0ull; uint32_t pa = 0; uint16_t pd = 0;
    for (auto& a : c.ram) {
        if (!a.is_write) continue;
        bool same = a.cycle - pc <= 8 && a.word_addr == pa && a.data == pd;
        pc = a.cycle; pa = a.word_addr; pd = a.data;
        if (same) continue;
        uint32_t b = a.word_addr * 2;
        if (b >= kMarks && b < kMarks + 2 * 8192) {
            size_t i = (b - kMarks) / 2;
            if (a.data == i + 1 && c.mark_t[i] == 0) c.mark_t[i] = a.cycle;
        } else if (b >= kLog && b < 0x1F0000) {
            c.log.push_back({a.cycle, a.data});
        }
    }
    while (!c.mark_t.empty() && c.mark_t.back() == 0) c.mark_t.pop_back();
}

inline Cap run(const RunSpec& s) {
    Cap c;
    nmh::Config cfg;
    cfg.chipset_config = s.chipset;
    cfg.cpu_config = s.cpu_config;
    nmh::NanomigHarness h(cfg);
    for (size_t i = 0; i < s.sc.w.size(); ++i) h.poke_word(kScript / 2 + i, s.sc.w[i]);
    for (auto& p : s.pokes) h.poke_word(p.byte_addr / 2, p.value);
    c.loaded = h.load_program(g_dir + "/" + s.bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.base = h.cycles();
    c.refresh.reserve(s.cycles);
    if (s.sample_txd) c.txd.reserve(s.cycles);
    size_t seen = h.ram_log().size();
    uint64_t done_at = 0;
    for (uint64_t i = 0; i < s.cycles; ++i) {
        if (s.drive) s.drive(h, h.cycles());
        h.run_cycles(1);
        c.refresh.push_back(h.ports()->refresh ? 1 : 0);
        if (s.sample_txd) c.txd.push_back(h.ports()->uart_tx ? 1 : 0);
        if (!done_at && (i & 7) == 0) {
            const auto& lg = h.ram_log();
            for (; seen < lg.size(); ++seen)
                if (lg[seen].is_write && lg[seen].word_addr == kDoneAddr / 2 && lg[seen].data == 0x600D) { done_at = i; c.done_cycle = lg[seen].cycle; }
        }
        if (done_at && i >= done_at + s.tail) break;
    }
    auto fx = h.peek_logical(kResults);
    c.fixture_ok = fx.has_value() && *fx == 0xBEEF;
    c.timeouts = h.peek_logical(kResults + 2).value_or(0);
    c.done = done_at != 0;
    c.ram = h.ram_log();
    c.audio.assign(h.audio().begin() + c.base, h.audio().end());
    scan_ram(c);
    return c;
}

// ---- colour clock (SPECIFICATION.md 2.2) -------------------------------------
// K = clk_sys cycles per colour clock, from the refresh pattern of a run with no DMA on:
// four refresh reads per line, each two colour clocks after the one before.
struct Frame {
    bool ok = false;
    uint64_t K = 0;
    std::vector<uint64_t> start;     // first refresh cycle of each line
    double gap_min = 0, gap_max = 0;
};

inline Frame analyse_refresh(const Cap& c) {
    Frame r;
    std::vector<uint64_t> edges;
    for (size_t i = 1; i < c.refresh.size(); ++i)
        if (c.refresh[i] && !c.refresh[i - 1]) edges.push_back(c.base + i);
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < edges.size(); ++i) {
        if (i == 0 || edges[i] - edges[i - 1] > 64) groups.push_back({});
        groups.back().push_back(edges[i]);
    }
    if (groups.size() > 2) { groups.erase(groups.begin()); groups.pop_back(); }
    if (groups.size() < 10) return r;
    uint64_t sp = groups[0].size() == 4 ? groups[0][1] - groups[0][0] : 0;
    bool ok = sp > 0 && sp % 2 == 0;
    r.K = sp / 2;
    for (auto& g : groups) {
        if (g.size() != 4) { ok = false; break; }
        for (size_t k = 1; k < 4; ++k) if (g[k] - g[k - 1] != sp) ok = false;
        r.start.push_back(g[0]);
    }
    r.gap_min = 1e9; r.gap_max = 0;
    for (size_t i = 1; ok && i < r.start.size(); ++i) {
        double gap = double(r.start[i] - r.start[i - 1]) / double(r.K);
        r.gap_min = std::min(r.gap_min, gap); r.gap_max = std::max(r.gap_max, gap);
        if (gap < 226.0 || gap > 229.0) ok = false;
    }
    r.ok = ok;
    return r;
}

inline bool refresh_at(const Cap& c, uint64_t cycle) {
    if (cycle < c.base + 3) return false;
    uint64_t i = cycle - c.base;
    if (i >= c.refresh.size()) return false;
    for (int d = 0; d <= 3; ++d) if (c.refresh[i - d]) return true;
    return false;
}

// ---- fetches (SPECIFICATION.md 2.4) ----------------------------------------
struct Fetch { uint64_t t; uint32_t addr; uint16_t data; };

// Reads of [lo, hi) that are not refresh cycles, between cycles t0 and t1.
inline std::vector<Fetch> fetches(const Cap& c, uint32_t lo, uint32_t hi, uint64_t t0 = 0, uint64_t t1 = ~0ull) {
    std::vector<Fetch> out;
    uint64_t pc = ~0ull; uint32_t pa = ~0u;
    for (auto& a : c.ram) {
        if (a.is_write || a.cycle < t0 || a.cycle > t1) continue;
        uint32_t b = a.word_addr * 2;
        if (b < lo || b >= hi) continue;
        bool same = a.cycle - pc <= 8 && a.word_addr == pa;
        pc = a.cycle; pa = a.word_addr;
        if (same) continue;
        if (refresh_at(c, a.cycle)) continue;
        out.push_back({a.cycle, b, a.data});
    }
    return out;
}

// ---- output (SPECIFICATION.md 2.5) -----------------------------------------
inline int32_t out_at(const Cap& c, uint64_t cycle) {
    uint64_t i = cycle - c.base;
    if (cycle < c.base || i >= c.audio.size()) return 0;
    // the outputs are 15-bit two's complement values
    auto sx = [](uint16_t v) { return int32_t(v & 0x7FFF) - ((v & 0x4000) ? 0x8000 : 0); };
    return sx(c.audio[i].left) + sx(c.audio[i].right);
}

// Maximal intervals [t0, t1) (cycles) over which O is constant.
struct Seg { uint64_t t0, t1; int32_t v; };
inline std::vector<Seg> segments(const Cap& c, uint64_t t0, uint64_t t1) {
    std::vector<Seg> out;
    if (t0 < c.base) t0 = c.base;
    if (t1 > c.base + c.audio.size()) t1 = c.base + c.audio.size();
    for (uint64_t t = t0; t < t1; ++t) {
        int32_t v = out_at(c, t);
        if (out.empty() || out.back().v != v) out.push_back({t, t + 1, v});
        else out.back().t1 = t + 1;
    }
    return out;
}

inline uint64_t marker(const Cap& c, int idx) {
    return (idx >= 1 && size_t(idx) <= c.mark_t.size()) ? c.mark_t[idx - 1] : 0;
}

} // namespace s5
