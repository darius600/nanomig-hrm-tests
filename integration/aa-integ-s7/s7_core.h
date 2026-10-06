// s7_core.h -- shared machinery of aa-integ-s7: the script of prog_run.s, a run that
// captures the ports, markers, and the attribution of RAM-port reads
// (SPECIFICATION.md sections 1.4, 1.6 and 1.9).
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

namespace s7 {

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
inline std::string num2(double v) { char b[40]; std::snprintf(b, sizeof b, "%.2f", v); return b; }
inline std::string hex4(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%04X", v & 0xFFFF); return b; }
inline std::string hex6(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%06X", v & 0xFFFFFF); return b; }

// ---- memory map (SPECIFICATION.md 1.2 and the build section) ---------------
constexpr uint32_t kScript   = 0x081000;
constexpr uint32_t kProgLo   = 0x080400;   // PROGRAM_ORG
constexpr uint32_t kProgHi   = 0x08CC36;   // the first byte of the S3 regions
constexpr uint32_t kMK       = 0x0B8000;   // the marker word
constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kResHi    = 0x0C1000;   // RES and its log: [kResults, kResHi)
constexpr uint32_t kLogBase  = 0x0C0010;
constexpr uint32_t kStackLo  = 0x0FC200;
constexpr uint32_t kStackHi  = 0x0FFF00;   // PROGRAM_STACK
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint64_t kBootBudget = 4'000'000;
constexpr uint16_t kVbTmo = 15000;

// custom register offsets (AA pp. 6-10; HRM Appendix B)
enum : uint16_t {
    DMACONR = 0x002, VPOSR = 0x004, VHPOSR = 0x006, JOY0DAT = 0x00A, JOY1DAT = 0x00C, ADKCONR = 0x010,
    POTINP = 0x016, DSKBYTR = 0x01A, INTENAR = 0x01C, INTREQR = 0x01E,
    DSKPTH = 0x020, DSKPTL = 0x022, DSKLEN = 0x024, VPOSW = 0x02A, VHPOSW = 0x02C, COPCON = 0x02E,
    POTGO = 0x034, JOYTEST = 0x036,
    BLTCON0 = 0x040, BLTCON1 = 0x042, BLTAFWM = 0x044, BLTALWM = 0x046,
    BLTCPTH = 0x048, BLTCPTL = 0x04A, BLTBPTH = 0x04C, BLTBPTL = 0x04E, BLTAPTH = 0x050, BLTAPTL = 0x052,
    BLTDPTH = 0x054, BLTDPTL = 0x056, BLTSIZE = 0x058, BLTCON0L = 0x05A, BLTSIZV = 0x05C, BLTSIZH = 0x05E,
    BLTCMOD = 0x060, BLTBMOD = 0x062, BLTAMOD = 0x064, BLTDMOD = 0x066,
    BLTCDAT = 0x070, BLTBDAT = 0x072, BLTADAT = 0x074,
    COP1LCH = 0x080, COP1LCL = 0x082, COPJMP1 = 0x088, DIWSTRT = 0x08E, DIWSTOP = 0x090, DDFSTRT = 0x092, DDFSTOP = 0x094,
    DMACON = 0x096, INTENA = 0x09A, INTREQ = 0x09C, ADKCON = 0x09E,
    BPL1PTH = 0x0E0, BPLCON0 = 0x100, BPLCON1 = 0x102, BPLCON2 = 0x104, BPLCON3 = 0x106, BPL1MOD = 0x108, BPL2MOD = 0x10A,
    BPLCON4 = 0x10C, SPR0PTH = 0x120, COLOR00 = 0x180, FMODE = 0x1FC,
};
inline uint16_t audlc(int c)  { return uint16_t(0x0A0 + 0x10 * c); }
inline uint16_t audlen(int c) { return uint16_t(0x0A4 + 0x10 * c); }
inline uint16_t audper(int c) { return uint16_t(0x0A6 + 0x10 * c); }
inline uint16_t audvol(int c) { return uint16_t(0x0A8 + 0x10 * c); }

// ---- script builder ----------------------------------------------------------
struct Script {
    std::vector<uint16_t> w;
    int marks = 0;     // markers placed so far (their values are 1, 2, 3, ...)
    int logn = 0;      // log words stored so far by the program
    void w16(uint16_t reg, uint16_t v) { w.insert(w.end(), {1, reg, v}); }
    void w16r(uint16_t r1, uint16_t v1, uint16_t r2, uint16_t v2) { w.insert(w.end(), {12, r1, v1, r2, v2}); }
    void w32(uint16_t reg, uint32_t v) { w.insert(w.end(), {2, reg, uint16_t(v >> 16), uint16_t(v & 0xFFFF)}); }
    void dly(unsigned n) { w.insert(w.end(), {3, uint16_t(n)}); }
    int rd(uint16_t reg) { w.insert(w.end(), {4, reg}); return logn++; }
    int mark() { w.insert(w.end(), {5, uint16_t(++marks)}); return marks; }
    void vblsync(uint16_t tmo = kVbTmo) { w.insert(w.end(), {6, tmo}); }
    int waitline(uint16_t v) { w.insert(w.end(), {7, v}); return logn++; }
    void lines(unsigned n) { w.insert(w.end(), {8, uint16_t(n)}); }
    void blitdone() { w.push_back(9); }
    int pollb(uint16_t reg, uint16_t mask, uint16_t tmo) { w.insert(w.end(), {10, reg, mask, tmo}); return logn++; }
    void wm(uint32_t addr, uint16_t v) { w.insert(w.end(), {11, uint16_t(addr >> 16), uint16_t(addr & 0xFFFF), v}); }
    void waitb(uint16_t reg, uint16_t mask, uint16_t tmo) { w.insert(w.end(), {13, reg, mask, tmo}); }
    void end() { w.push_back(0); }
};

struct Poke { uint32_t byte_addr; uint16_t value; };

inline void poke_words(std::vector<Poke>& v, uint32_t addr, const std::vector<uint16_t>& ws) {
    for (size_t i = 0; i < ws.size(); ++i) v.push_back({addr + uint32_t(2 * i), ws[i]});
}

// ---- a run -------------------------------------------------------------------
struct RunSpec {
    std::string bin = "prog_run.bin";
    Script sc;
    std::vector<Poke> pokes;
    uint64_t cycles = 30'000'000;   // clk_sys cycles run after boot() at most
    bool want_video = false, want_audio = false;
    uint8_t chipset = 0x10;         // chipset_config: AA
    uint8_t floppy = 0x00;          // floppy_config: one drive, DF0
    // before each cycle: the harness, the absolute cycle, the highest marker seen so far
    std::function<void(nmh::NanomigHarness&, uint64_t, int)> drive;
    uint64_t tail = 4000;           // cycles run after the DONE write
};

struct Cap {
    bool loaded = false, released = false, fixture_ok = false, done = false;
    uint64_t base = 0;                          // first cycle of refresh/audio/video
    std::vector<nmh::RamAccess> ram;
    std::vector<nmh::AudioSample> audio;        // from base, when asked
    std::vector<nmh::VideoSample> video;        // from base, when asked
    std::vector<uint8_t> refresh;               // from base
    uint16_t timeouts = 0;
    std::map<int, uint64_t> mark_t;             // marker value -> cycle of its first write to MK
    std::vector<uint16_t> log;                  // the log words the program stored
    uint64_t done_cycle = 0;
    uint64_t end_cycle = 0;
};

inline void scan_marks(Cap& c, size_t from, size_t to) {
    for (size_t i = from; i < to; ++i) {
        const auto& a = c.ram[i];
        if (a.is_write && a.word_addr * 2 == kMK) c.mark_t.emplace(int(a.data), a.cycle);
    }
}

inline Cap run(const RunSpec& s) {
    Cap c;
    nmh::Config cfg;
    cfg.chipset_config = s.chipset;
    cfg.floppy_config = s.floppy;
    nmh::NanomigHarness h(cfg);
    for (size_t i = 0; i < s.sc.w.size(); ++i) h.poke_word(kScript / 2 + i, s.sc.w[i]);
    for (auto& p : s.pokes) h.poke_word(p.byte_addr / 2, p.value);
    c.loaded = h.load_program(g_dir + "/" + s.bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.base = h.cycles();
    size_t vbase = h.video().size();   // the harness captures from construction on
    c.refresh.reserve(s.cycles / 2);
    size_t seen = h.ram_log().size();
    size_t scanned = seen;
    uint64_t done_at = 0;
    int last_mark = 0;
    for (uint64_t i = 0; i < s.cycles; ++i) {
        if (s.drive) s.drive(h, h.cycles(), last_mark);
        h.run_cycles(1);
        c.refresh.push_back(h.ports()->refresh ? 1 : 0);
        if ((i & 7) == 0) {
            const auto& lg = h.ram_log();
            for (; seen < lg.size(); ++seen) {
                const auto& a = lg[seen];
                if (!a.is_write) continue;
                if (a.word_addr * 2 == kMK && int(a.data) > last_mark) last_mark = int(a.data);
                if (!done_at && a.word_addr == kDoneAddr / 2 && a.data == 0x600D) { done_at = i; c.done_cycle = a.cycle; }
            }
            (void)scanned;
        }
        if (done_at && i >= done_at + s.tail) break;
    }
    c.end_cycle = h.cycles();
    auto fx = h.peek_logical(kResults);
    c.fixture_ok = fx.has_value() && *fx == 0xBEEF;
    c.timeouts = h.peek_logical(kResults + 2).value_or(0);
    c.done = done_at != 0;
    c.ram = h.ram_log();
    for (int i = 0; i < s.sc.logn; ++i) c.log.push_back(h.peek_logical(kLogBase + 2 * uint32_t(i)).value_or(0xDEAD));
    if (s.want_audio) c.audio.assign(h.audio().begin() + vbase, h.audio().end());
    if (s.want_video) c.video.assign(h.video().begin() + vbase, h.video().end());
    scan_marks(c, 0, c.ram.size());
    return c;
}

inline uint64_t marker(const Cap& c, int id) {
    auto it = c.mark_t.find(id);
    return it == c.mark_t.end() ? 0 : it->second;
}
inline uint16_t logv(const Cap& c, int idx) {
    return (idx >= 0 && size_t(idx) < c.log.size()) ? c.log[size_t(idx)] : 0xDEAD;
}

// ---- attribution of the RAM-port reads (SPECIFICATION.md 1.6) -------------------
inline bool refresh_at(const Cap& c, uint64_t cycle) {
    if (cycle < c.base + 3) return false;
    uint64_t i = cycle - c.base;
    if (i >= c.refresh.size()) return false;
    for (int d = 0; d <= 3; ++d) if (c.refresh[i - d]) return true;
    return false;
}

// One logical access: the first entry of each stretched bus cycle.
struct Acc { uint64_t t; uint32_t addr; uint16_t data; bool write; bool refresh; };

inline std::vector<Acc> accesses(const Cap& c) {
    std::vector<Acc> out;
    uint64_t pc = ~0ull; uint32_t pa = ~0u; uint16_t pd = 0; bool pw = false;
    for (auto& a : c.ram) {
        bool same = a.cycle - pc <= 8 && a.word_addr == pa && a.data == pd && a.is_write == pw;
        pc = a.cycle; pa = a.word_addr; pd = a.data; pw = a.is_write;
        if (same) continue;
        out.push_back({a.cycle, a.word_addr * 2, a.data, a.is_write, !a.is_write && refresh_at(c, a.cycle)});
    }
    return out;
}

struct Region { uint32_t lo, hi; };   // byte range [lo, hi)
inline bool in_any(const std::vector<Region>& rs, uint32_t a) {
    for (auto& r : rs) if (a >= r.lo && a < r.hi) return true;
    return false;
}
inline bool is_cpu(uint32_t a) {
    return (a >= kProgLo && a < kProgHi) || (a >= kStackLo && a < kStackHi) || a == kMK || (a >= kResults && a < kResHi);
}

// The reads of the window (t0, t1) by class. `chan` are the regions of the channel(s) the proposition
// names; `copper` the Copper lists. A read that is none of CPU, refresh, Copper or channel is a stray DMA read.
struct Window {
    std::vector<Acc> chan, stray, copper, refresh;
    std::vector<Acc> writes_in;   // every write of the window, any address
};

inline Window classify(const std::vector<Acc>& acc, uint64_t t0, uint64_t t1,
                       const std::vector<Region>& chan, const std::vector<Region>& copper) {
    Window w;
    for (auto& a : acc) {
        if (a.t <= t0 || a.t >= t1) continue;
        if (a.write) { w.writes_in.push_back(a); continue; }
        if (a.refresh) { w.refresh.push_back(a); continue; }
        if (is_cpu(a.addr)) continue;
        if (in_any(copper, a.addr)) { w.copper.push_back(a); continue; }
        if (in_any(chan, a.addr)) { w.chan.push_back(a); continue; }
        w.stray.push_back(a);
    }
    return w;
}

// The refresh precondition of 1.6: the refresh-flagged reads of the window come in groups of four, one
// group per line. A group cut by the window's edge may hold fewer; at least one whole group must lie in it.
struct RefreshCheck { bool ok = false; size_t groups = 0, whole = 0; };
inline RefreshCheck refresh_groups(const std::vector<Acc>& refresh, uint64_t t0, uint64_t t1, uint64_t K) {
    RefreshCheck r;
    if (K == 0) return r;
    std::vector<std::vector<uint64_t>> g;
    for (auto& a : refresh) {
        if (g.empty() || a.t - g.back().back() > 8 * K) g.push_back({});
        g.back().push_back(a.t);
    }
    r.groups = g.size();
    bool ok = !g.empty();
    for (size_t i = 0; i < g.size(); ++i) {
        bool edge = (i == 0 && g[i].front() - t0 <= 16 * K) || (i + 1 == g.size() && t1 - g[i].back() <= 16 * K);
        if (g[i].size() == 4) ++r.whole;
        else if (!(edge && g[i].size() < 4)) ok = false;
    }
    r.ok = ok && r.whole >= 1;
    return r;
}

// K = clk_sys cycles per colour clock, from the spacing of the four refresh reads of a line
// ("There are 4 refresh time slots", p.29): consecutive refresh reads are two colour clocks apart.
inline uint64_t colour_clock(const std::vector<Acc>& refresh) {
    std::vector<uint64_t> d;
    for (size_t i = 1; i < refresh.size() && d.size() < 400; ++i) {
        uint64_t g = refresh[i].t - refresh[i - 1].t;
        if (g < 200) d.push_back(g);
    }
    if (d.empty()) return 0;
    std::sort(d.begin(), d.end());
    return d[d.size() / 2] / 2;
}

} // namespace s7
