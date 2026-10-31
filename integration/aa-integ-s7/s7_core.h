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
    uint64_t period = 0;                        // clk_sys cycles per bus period: the smallest spacing of two log entries
    uint32_t prog_end = 0, script_end = 0;      // the CPU spans of this run: image end + 16, script end + 16
};

// The CPU attribution spans of the run most recently executed (SPECIFICATION.md 1.6): the program image and the script,
// each up to its end + 16.
inline uint32_t g_prog_hi = kProgLo, g_script_hi = kScript;

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
    const size_t psz = h.load_program(g_dir + "/" + s.bin);
    c.loaded = psz > 0;
    if (!c.loaded) return c;
    c.prog_end = kProgLo + uint32_t(psz) + 16;
    c.script_end = kScript + uint32_t(2 * s.sc.w.size()) + 16;
    g_prog_hi = c.prog_end; g_script_hi = c.script_end;
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
    for (size_t i = 1; i < c.ram.size(); ++i) {
        uint64_t d = c.ram[i].cycle - c.ram[i - 1].cycle;
        if (d > 0 && (c.period == 0 || d < c.period)) c.period = d;
    }
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
// A read is a refresh read when the `refresh` output is high at the clk_sys cycle of its log entry (the same cycle).
inline bool refresh_at(const Cap& c, uint64_t cycle) {
    if (cycle < c.base) return false;
    uint64_t i = cycle - c.base;
    return i < c.refresh.size() && c.refresh[i];
}

// One logical access: the first entry of each stretched bus cycle. A bus cycle stretched by wait states appears
// as one log entry per bus period; an entry that repeats the address, direction, data and byte lanes of the entry
// before it exactly one bus period later continues that cycle (the harness's held_continuations), and nothing else
// is merged.
struct Acc { uint64_t t; uint32_t addr; uint16_t data; bool write; bool refresh; };

inline std::vector<Acc> accesses(const Cap& c) {
    std::vector<Acc> out;
    const std::vector<bool> held = nmh::held_continuations(c.ram, c.period);
    for (size_t i = 0; i < c.ram.size(); ++i) {
        if (held[i]) continue;
        const auto& a = c.ram[i];
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
    return (a >= kProgLo && a < g_prog_hi) || (a >= kScript && a < g_script_hi) || (a >= kStackLo && a < kStackHi) || a == kMK || (a >= kResults && a < kResHi);
}

// The reads of the window (t0, t1) by class. `chan` are the regions of the channel(s) the proposition
// names; `copper` the Copper lists. A read that is none of CPU, refresh, Copper or channel is a stray DMA read.
struct Window {
    std::vector<Acc> chan, stray, copper, refresh;
    std::vector<Acc> writes_in;   // every write of the window, any address
};

// The index of the first access later than t0 (the accesses are in time order).
inline size_t acc_first_after(const std::vector<Acc>& acc, uint64_t t0) {
    size_t lo = 0, hi = acc.size();
    while (lo < hi) {
        size_t m = (lo + hi) / 2;
        if (acc[m].t <= t0) lo = m + 1; else hi = m;
    }
    return lo;
}

inline Window classify(const std::vector<Acc>& acc, uint64_t t0, uint64_t t1,
                       const std::vector<Region>& chan, const std::vector<Region>& copper) {
    Window w;
    for (size_t i = acc_first_after(acc, t0); i < acc.size() && acc[i].t < t1; ++i) {
        const Acc& a = acc[i];
        if (a.write) { w.writes_in.push_back(a); continue; }
        if (a.refresh) { w.refresh.push_back(a); continue; }
        if (is_cpu(a.addr)) continue;
        if (in_any(copper, a.addr)) { w.copper.push_back(a); continue; }
        if (in_any(chan, a.addr)) { w.chan.push_back(a); continue; }
        w.stray.push_back(a);
    }
    return w;
}

// The refresh precondition (SPECIFICATION.md 1.6): a line holds four refresh slots (AA p.29 'There are 4 refresh time slots';
// HRM p.194), so a window that contains k line starts (the program counts them with LINES(k)) holds the four refresh reads of at
// least k - 1 whole lines. The clause that the four reads of a line form one group, and that no line holds more than four, is
// unfalsifiable at this boundary: no stimulus changes how many refresh reads a line holds.
constexpr size_t kRefSlots = 4;
inline size_t refresh_reads(const std::vector<Acc>& acc, uint64_t t0, uint64_t t1) {
    size_t n = 0;
    for (size_t i = acc_first_after(acc, t0); i < acc.size() && acc[i].t < t1; ++i) if (acc[i].refresh) ++n;
    return n;
}
inline size_t refresh_min(unsigned lines) { return lines >= 1 ? kRefSlots * (lines - 1) : 0; }

// The final WAIT of a list ($FFFF, $FFFE) and the word after it (SPECIFICATION.md 1.10). AA p.20, WAIT: 'Wait until beam counter
// is equal to, or greater than. (keeps Coprocessor off of bus until beam position has been reached)'. AA p.21: COP1LC and COP2LC are
// transferred to the program counter on a strobe, and COP1LC at the beginning of each vertical blank; HRM p.25: the next instruction is
// fetched from the new location. The first-word fetch of the restarted list is therefore an anchor the RAM port shows. The package
// measures, per list, the Copper's reads of the word after the final WAIT between the read of the WAIT's second word and that fetch,
// with their distance to it; it grades nothing about them, because the pages do not fix the restart's first bus cycle.
struct FwList { std::string name; uint32_t wait_at; };   // byte address of the first word of the list's final WAIT
struct Fw { std::string text; };

// `starts` holds the byte address of the first word of every list the run can restart (IDLE and the case lists); a read of one of them,
// after a read of the WAIT's second word, is the fetch that ends the interval.
inline Fw fw_eval(const Cap& c, const std::vector<Acc>& acc, const std::vector<FwList>& lists, const std::vector<uint32_t>& starts) {
    Fw f;
    std::map<uint32_t, std::vector<uint64_t>> reads;   // address -> cycles of its reads by anything but a write or a refresh cycle, from the release of the CPU on
    std::vector<std::pair<uint64_t, uint32_t>> fetch;  // (cycle, address) of every read of the first word of a list
    for (auto& a : acc) {
        if (a.write || a.refresh || a.t < c.base || a.addr < 0x090000 || a.addr >= 0x090600) continue;
        reads[a.addr].push_back(a.t);
        if (std::find(starts.begin(), starts.end(), a.addr) != starts.end()) fetch.push_back({a.t, a.addr});
    }
    const std::vector<uint64_t> none;
    auto rd = [&](uint32_t a) -> const std::vector<uint64_t>& { auto it = reads.find(a); return it == reads.end() ? none : it->second; };
    auto last_before = [](const std::vector<uint64_t>& v, uint64_t p) -> uint64_t {   // the latest cycle of the sorted v that lies before p, or 0
        auto it = std::lower_bound(v.begin(), v.end(), p);
        return it == v.begin() ? 0 : *(it - 1);
    };
    auto first_fetch_after = [&](uint64_t t) -> size_t {   // the index of the first fetch later than t, or fetch.size()
        size_t i = 0;
        while (i < fetch.size() && fetch[i].first <= t) ++i;
        return i;
    };
    auto dists = [](const std::map<uint64_t, size_t>& d) {   // 'distance x count', ascending
        std::string s;
        for (auto& kv : d) s += (s.empty() ? "" : ", ") + dec((long long)kv.first) + " x " + dec((long long)kv.second);
        return s.empty() ? std::string("none") : s;
    };
    f.text = "Copper reads of the word after a list's final WAIT, each placed between the Copper's read of the WAIT's second word and the next read of the first word of a restarted list (the fetch from the new location), with its distance to that fetch in cycles; not graded, since the pages do not fix the restart's first bus cycle:";
    std::vector<std::string> names;
    for (auto& l : lists) if (std::find(names.begin(), names.end(), l.name) == names.end()) names.push_back(l.name);
    for (auto& nm : names) {
        size_t n = 0, reached = 0, total = 0, with_reads = 0, before = 0, after = 0, no_w2 = 0, no_fetch = 0;
        uint32_t first = 0;
        std::map<uint64_t, size_t> d_before, d_after;
        std::string example, many;
        for (auto& l : lists) {
            if (l.name != nm) continue;
            if (!n++) first = l.wait_at;
            const auto& r0 = rd(l.wait_at); const auto& r1 = rd(l.wait_at + 2); const auto& rp = rd(l.wait_at + 4);
            if (!r0.empty() && !r1.empty()) ++reached;
            if (!rp.empty()) ++with_reads;
            total += rp.size();
            std::map<uint64_t, std::vector<uint64_t>> per_interval;   // cycle of the fetch -> the reads placed before it
            for (uint64_t t : rp) {
                const uint64_t p = last_before(r1, t);
                const size_t qi = first_fetch_after(t);
                const uint64_t lf = qi ? fetch[qi - 1].first : 0;   // the latest fetch before t, or 0
                if (!p) { ++no_w2; continue; }
                if (lf > p) { ++after; ++d_after[t - lf]; continue; }
                if (qi == fetch.size()) { ++no_fetch; continue; }
                ++before; ++d_before[fetch[qi].first - t];
                per_interval[fetch[qi].first].push_back(t);
                if (example.empty())
                    example = " e.g. the WAIT's second word read at cycle " + dec((long long)p) + ", the word after it at cycle " + dec((long long)t) +
                              ", the first word at $" + hex6(fetch[qi].second) + " at cycle " + dec((long long)fetch[qi].first) + " (distance " + dec((long long)(fetch[qi].first - t)) + ");";
            }
            for (auto& kv : per_interval)
                if (kv.second.size() > 1) {
                    many += " $" + hex6(l.wait_at + 4) + " read " + dec((long long)kv.second.size()) + " times before the fetch at cycle " + dec((long long)kv.first) + " (cycles";
                    for (uint64_t p : kv.second) many += " " + dec((long long)p);
                    many += ");";
                }
        }
        f.text += " " + nm + " (" + dec((long long)n) + (n == 1 ? " list" : " lists") + ", the first final WAIT at $" + hex6(first) + "): second word of the final WAIT read in " + dec((long long)reached) +
                  ", word after it read " + dec((long long)total) + " times in " + dec((long long)with_reads) + ": " + dec((long long)before) + " between the read of the WAIT's second word and the next fetch (distance to the fetch: " + dists(d_before) + "), " +
                  dec((long long)after) + " after a fetch and before the next read of the WAIT's second word (distance from the fetch: " + dists(d_after) + "), " +
                  dec((long long)no_w2) + " before every read of the WAIT's second word, " + dec((long long)no_fetch) + " with no fetch after them;" + example +
                  (many.empty() ? "" : " more than one in one interval:" + many);
    }
    return f;
}

// The final-WAIT lines of a proposition `id`: the measurement, and the assertion it does not carry.
inline void fw_print(Checks& ck, const std::string& id, const Fw& f) {
    ck.note(id + " " + f.text);
    ck.record("OPEN " + id + "-FW: whether the Copper reads no word after a final WAIT ($FFFF, $FFFE) before the first fetch from the restarted list (AA p.20 WAIT 'keeps Coprocessor off of bus until beam position has been reached'; AA p.21 the location register is transferred to the program counter on a strobe or at the beginning of each vertical blank; HRM p.25 the next instruction is fetched from the new location) is not graded: the pages do not fix the restart's first bus cycle, since no page says that the Copper's first bus cycle after the strobe or the start of the vertical blank is that fetch, so a read of the word after the WAIT between the WAIT's second word and the fetch is measured and printed above, not judged");
}

} // namespace s7
