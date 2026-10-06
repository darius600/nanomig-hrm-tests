// s3_run.h -- run machinery of package aa-integ-s3: building a run (setup
// table, Copper list, sprite lists, event table), executing it on the harness
// and extracting what the ports showed. No proposition is graded here.

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "nanomig_harness.h"

namespace s3 {

// ---- fixed addresses (SPECIFICATION.md 2.3) and the program's tables ----------
constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint32_t kSetupTab = 0x088000;   // setup table read by prog_main
constexpr uint32_t kEventTab = 0x08A000;   // event table read by prog_main
constexpr uint32_t kEvRes    = 0x0C0010;   // result record of event k: + 0x10*k
constexpr uint32_t kCL       = 0x100000;
constexpr uint32_t kZ        = 0x104000;
constexpr uint32_t kU        = 0x0D0000;
constexpr uint32_t kU2       = 0x0E0000;
constexpr uint16_t kSetupMarker = 0x5E7F;
constexpr uint32_t kVbData0 = kCL + 6;      // data word of the first vertical-blank MOVE of the Copper list
inline uint32_t vb_data(int i) { return kVbData0 + 4u * i; }

inline uint32_t P_k(int k) { return 0x090000u + 0x4000u * (k - 1); }
inline uint32_t L_x(int x) { return 0x0B0000u + 0x1000u * x; }
inline uint32_t L_alt(int x) { return 0x0B8000u + 0x1000u * x; }

// T_i regions (2.3): PTH h_i, PTL l_i
constexpr uint16_t kTh[9] = {0, 0x11, 0x12, 0x14, 0x18, 0x0C, 0x13, 0x1C, 0x1F};
constexpr uint16_t kTl[9] = {0, 0x0000, 0x5554, 0x2AAA, 0x8000, 0xC002, 0x4444, 0x1110, 0xA000};
inline uint32_t T_addr(int i) { return (uint32_t(kTh[i]) << 16) | kTl[i]; }

// custom registers (offsets)
enum : uint16_t {
    R_VPOSR = 0x004, R_VHPOSR = 0x006, R_CLXDAT = 0x00E, R_COP1LCH = 0x080, R_COP1LCL = 0x082, R_COPJMP1 = 0x088,
    R_DIWSTRT = 0x08E, R_DIWSTOP = 0x090, R_DDFSTRT = 0x092, R_DDFSTOP = 0x094, R_DMACON = 0x096,
    R_CLXCON = 0x098, R_INTREQ = 0x09C, R_BPL1PTH = 0x0E0, R_SPR0PTH = 0x120, R_SPR0POS = 0x140,
    R_BPLCON0 = 0x100, R_BPLCON1 = 0x102, R_BPLCON2 = 0x104, R_BPLCON3 = 0x106, R_BPL1MOD = 0x108,
    R_BPL2MOD = 0x10A, R_BPLCON4 = 0x10C, R_CLXCON2_P8 = 0x10E, R_CLXCON2_P18 = 0x10C,
    R_BPL1DAT = 0x110, R_HTOTAL = 0x1C0, R_VTOTAL = 0x1C8, R_SPRHSTRT = 0x1D0, R_SPRHSTOP = 0x1D2,
    R_BPLHSTRT = 0x1D4, R_BPLHSTOP = 0x1D6, R_BEAMCON0 = 0x1DC, R_BPLHMOD = 0x1E6, R_SPRHPTH = 0x1E8,
    R_SPRHPTL = 0x1EA, R_BPLHPTH = 0x1EC, R_BPLHPTL = 0x1EE, R_FMODE = 0x1FC, R_COLOR00 = 0x180
};
inline uint16_t R_BPLxPTH(int x) { return uint16_t(0x0E0 + 4 * (x - 1)); }
inline uint16_t R_BPLxPTL(int x) { return uint16_t(0x0E2 + 4 * (x - 1)); }
inline uint16_t R_BPLxDAT(int x) { return uint16_t(0x110 + 2 * (x - 1)); }
inline uint16_t R_SPRxPTH(int x) { return uint16_t(0x120 + 4 * x); }
inline uint16_t R_SPRxPTL(int x) { return uint16_t(0x122 + 4 * x); }
inline uint16_t R_SPRxPOS(int x) { return uint16_t(0x140 + 8 * x); }
inline uint16_t R_SPRxCTL(int x) { return uint16_t(0x142 + 8 * x); }
inline uint16_t R_SPRxDATA(int x) { return uint16_t(0x144 + 8 * x); }
inline uint16_t R_SPRxDATB(int x) { return uint16_t(0x146 + 8 * x); }

struct Wr { uint16_t reg; uint16_t val; };

inline void push_ptr(std::vector<Wr>& v, uint16_t pth, uint16_t ptl, uint32_t addr) {
    v.push_back({pth, uint16_t((addr >> 16) & 0x1F)});
    v.push_back({ptl, uint16_t(addr & 0xFFFE)});
}

// ---- run description ----------------------------------------------------------
struct EventSpec {          // one event of the program's event table
    int nvb = 0;            // vertical-blank flags to pass first
    int line = -1;          // V10-V0, -1: none
    int h = 0;
    std::vector<Wr> wr;
    std::vector<uint16_t> rd;
    std::vector<std::pair<uint32_t, uint16_t>> mw;      // Chip-memory word writes (byte address, value)
};

struct CopEvent {           // Copper events: WAIT(line, h) followed by moves
    int line, h;
    std::vector<Wr> mv;
};

struct RunSpec {
    std::vector<Wr> setup;                      // CPU writes, in order (ends with DMACON)
    std::vector<std::pair<uint32_t, std::vector<uint16_t>>> mem;   // byte address, words
    std::vector<Wr> vb;                         // Copper vertical-blank block (after COLOR00 = 0)
    std::vector<Wr> vb_intended;                // the values the stimulus intends (when they differ from vb)
    std::vector<CopEvent> cev;
    std::vector<EventSpec> ev;
    bool no_tag_moves = false;                  // tag block without its MOVE COLOR00
    bool no_marker = false;                     // the program variant without the setup marker
    uint64_t budget = 30'000'000;
    uint8_t chipset = 0x10;                     // 0x10 AA, 0x08 ECS
    int tag_lo = 0x2C, tag_hi = 0xFF;
    std::string label;
};

// where the list builder put things (word addresses)
struct CopLayout {
    std::map<int, uint32_t> tag;                // line -> byte address of the first word of its tag MOVE
    std::vector<uint32_t> vb_data;              // byte address of each vb MOVE's data word
    std::vector<std::vector<uint32_t>> cev_data;   // per Copper event, per move
    std::vector<uint32_t> cev_wait;             // per Copper event, byte address of the WAIT
    uint32_t end_addr = 0;
};

inline std::vector<uint16_t> build_copper(const RunSpec& s, CopLayout& lay) {
    std::vector<uint16_t> w;
    auto addr = [&]() { return kCL + uint32_t(w.size()) * 2; };
    auto mv = [&](uint16_t reg, uint16_t val) { w.push_back(reg); w.push_back(val); };
    mv(R_COLOR00, 0x000);
    for (const auto& m : s.vb) {
        w.push_back(m.reg);
        lay.vb_data.push_back(addr());
        w.push_back(m.val);
    }
    // events sorted by line, stable
    std::vector<size_t> order(s.cev.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        if (s.cev[a].line != s.cev[b].line) return s.cev[a].line < s.cev[b].line;
        return s.cev[a].h < s.cev[b].h;
    });
    lay.cev_data.assign(s.cev.size(), {});
    lay.cev_wait.assign(s.cev.size(), 0);
    size_t oi = 0;
    // events placed before the first tag line (none in use) are not supported
    for (int n = s.tag_lo; n <= s.tag_hi; ++n) {
        w.push_back(uint16_t((n << 8) | 0x01));
        w.push_back(0xFFFE);
        lay.tag[n] = addr();
        if (!s.no_tag_moves) {
            w.push_back(R_COLOR00);
            w.push_back(uint16_t(0x100 | n));
        }
        while (oi < order.size() && s.cev[order[oi]].line == n) {
            const CopEvent& e = s.cev[order[oi]];
            lay.cev_wait[order[oi]] = addr();
            w.push_back(uint16_t((n << 8) | (e.h & 0xFE) | 0x01));
            w.push_back(0xFFFE);
            for (const auto& m : e.mv) {
                w.push_back(m.reg);
                lay.cev_data[order[oi]].push_back(addr());
                w.push_back(m.val);
            }
            ++oi;
        }
    }
    lay.end_addr = addr();
    w.push_back(0xFFFF);
    w.push_back(0xFFFE);
    return w;
}

// ---- what a run returns --------------------------------------------------------
struct Acc { uint64_t cyc; uint32_t waddr; bool wr; uint16_t data; };

struct RunData {
    bool loaded = false, released = false, done = false;
    uint64_t cycles = 0, period = 0;
    std::vector<Acc> acc;                        // held bus cycles collapsed
    std::vector<nmh::VideoSample> vid;
    std::vector<nmh::Span> lines;
    std::map<uint32_t, uint16_t> res;            // words of RES
    bool marker_seen = false;
    uint64_t t_marker = 0;
    std::vector<uint64_t> tF;                    // fetches at the COP1LC address after the marker
    CopLayout lay;
    RunSpec spec;
    // Copper-list reads by byte address
    std::map<uint32_t, std::vector<std::pair<uint64_t, uint16_t>>> clread;
    uint16_t word(uint32_t a) const { auto it = res.find(a); return it == res.end() ? 0 : it->second; }
    uint16_t ev_word(int k, int off) const { return word(kEvRes + 0x10 * k + off); }
};

inline void put_words(nmh::NanomigHarness& h, uint32_t byte_addr, const std::vector<uint16_t>& w) {
    for (size_t i = 0; i < w.size(); ++i) h.poke_word(byte_addr / 2 + uint32_t(i), w[i]);
}

inline std::string g_dir = ".";

inline std::shared_ptr<RunData> run(const RunSpec& s) {
    auto r = std::make_shared<RunData>();
    r->spec = s;
    nmh::Config cfg;
    cfg.chipset_config = s.chipset;
    cfg.memory_config = 0x03;
    nmh::NanomigHarness h(cfg);
    std::string prog = s.no_marker ? "prog_nomarker" : "prog_main";
    r->loaded = h.load_program(g_dir + "/" + prog + ".bin") > 0;
    if (!r->loaded) return r;

    std::vector<uint16_t> st;
    for (const auto& w : s.setup) { st.push_back(w.reg); st.push_back(w.val); }
    st.push_back(0xFFFF);
    put_words(h, kSetupTab, st);

    std::vector<uint16_t> et;
    for (const auto& e : s.ev) {
        et.push_back(1);
        et.push_back(uint16_t(e.nvb));
        et.push_back(e.line < 0 ? 0xFFFF : uint16_t(e.line));
        et.push_back(uint16_t(e.h));
        // the up to four values written most often are held in D0-D3 before the beam wait
        std::vector<std::pair<int, uint16_t>> freq;      // (count, value)
        for (const auto& w : e.wr) {
            bool f = false;
            for (auto& q : freq) if (q.second == w.val) { ++q.first; f = true; }
            if (!f) freq.push_back({1, w.val});
        }
        std::stable_sort(freq.begin(), freq.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
        std::vector<uint16_t> pre, code;
        std::map<uint16_t, int> dreg;
        for (size_t i = 0; i < freq.size() && i < 4; ++i)
            if (freq[i].first >= 2) {
                dreg[freq[i].second] = int(i);
                pre.push_back(uint16_t(0x303C | (i << 9)));     // MOVE.W #value,Di
                pre.push_back(freq[i].second);
            }
        for (const auto& w : e.wr) {
            auto it = dreg.find(w.val);
            if (it != dreg.end()) { code.push_back(uint16_t(0x3340 | it->second)); code.push_back(w.reg); }   // MOVE.W Di,reg(A1)
            else { code.push_back(0x337C); code.push_back(w.val); code.push_back(w.reg); }                    // MOVE.W #val,reg(A1)
        }
        for (size_t j = 0; j < e.rd.size(); ++j) { code.push_back(0x3569); code.push_back(e.rd[j]); code.push_back(uint16_t(6 + 2 * j)); }   // MOVE.W reg(A1),n(A2)
        for (const auto& m : e.mw) { code.push_back(0x33FC); code.push_back(m.second); code.push_back(uint16_t(m.first >> 16)); code.push_back(uint16_t(m.first & 0xFFFF)); }   // MOVE.W #val,abs.L
        if (pre.empty()) et.push_back(0);
        else { et.push_back(uint16_t(pre.size() + 1)); for (auto w : pre) et.push_back(w); et.push_back(0x4E75); }
        if (code.empty()) et.push_back(0);
        else { et.push_back(uint16_t(code.size() + 1)); for (auto w : code) et.push_back(w); et.push_back(0x4E75); }
    }
    et.push_back(0);
    put_words(h, kEventTab, et);

    auto cl = build_copper(s, r->lay);
    put_words(h, kCL, cl);
    for (const auto& m : s.mem) put_words(h, m.first, m.second);

    r->released = h.boot(8'000'000).cpu_released;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, s.budget);
    r->done = d.has_value();
    r->cycles = h.cycles();

    const auto& log = h.ram_log();
    uint64_t period = 0;
    for (size_t i = 1; i < log.size(); ++i) {
        uint64_t dd = log[i].cycle - log[i - 1].cycle;
        if (dd > 0 && (period == 0 || dd < period)) period = dd;
    }
    r->period = period;
    std::vector<nmh::RamAccess> col = period ? nmh::coalesce_adjacent(log, period) : log;
    r->acc.reserve(col.size());
    for (const auto& a : col) r->acc.push_back({a.cycle, a.word_addr, a.is_write, a.data});

    for (const auto& a : r->acc) {
        uint32_t b = a.waddr * 2;
        if (!r->marker_seen && a.wr && b == kResults && a.data == kSetupMarker) {
            r->marker_seen = true;
            r->t_marker = a.cyc;
        }
        if (!a.wr && b >= kCL && b < kCL + 0x4000) r->clread[b].push_back({a.cyc, a.data});
    }
    if (r->marker_seen) {
        auto it = r->clread.find(kCL);
        if (it != r->clread.end())
            for (const auto& p : it->second)
                if (p.first > r->t_marker) r->tF.push_back(p.first);
    }
    for (uint32_t a = kResults; a < kResults + 0x1000; a += 2) {
        auto w = h.peek_logical(a);
        if (w) r->res[a] = *w;
    }
    r->vid = h.video();
    r->lines = nmh::find_lines(r->vid);
    return r;
}

// ---- frames, tags, output lines --------------------------------------------------
inline uint16_t col_of(const nmh::VideoSample& s) { return uint16_t(s.r | (s.g << 4) | (s.b << 8)); }
inline uint16_t tag_col(int n) { return uint16_t(0x1 | ((n >> 4) << 4) | ((n & 15) << 8)); }
constexpr uint16_t kColON = 0xFFF, kColS = 0x0F0;

// time of the marker write of event k (the event's last store), 0 when absent
inline uint64_t marker_time(const RunData& r, int k) {
    for (const auto& a : r.acc)
        if (a.wr && a.waddr * 2 == kEvRes + 0x10 * uint32_t(k) && a.data == uint16_t(0xE000 + k)) return a.cyc;
    return 0;
}

inline uint64_t frame_end(const RunData& r, int i) {   // i is 1-based
    if (size_t(i) < r.tF.size()) return r.tF[i];
    return r.cycles;
}

// time of the first read of byte address `a` within [t0,t1)
inline std::optional<uint64_t> first_clread(const RunData& r, uint32_t a, uint64_t t0, uint64_t t1) {
    auto it = r.clread.find(a);
    if (it == r.clread.end()) return std::nullopt;
    for (const auto& p : it->second)
        if (p.first >= t0 && p.first < t1) return p.first;
    return std::nullopt;
}

struct Frame {
    int idx = 0;
    uint64_t t0 = 0, t1 = 0;
    bool ok = false;
    std::map<int, uint64_t> tag;      // n -> time
};

inline Frame get_frame(const RunData& r, int i) {   // i is 1-based
    Frame f;
    f.idx = i;
    if (size_t(i) > r.tF.size()) return f;
    f.t0 = r.tF[i - 1];
    f.t1 = size_t(i) < r.tF.size() ? r.tF[i] : r.cycles;
    f.ok = true;
    for (const auto& kv : r.lay.tag) {
        auto t = first_clread(r, kv.second, f.t0, f.t1);
        if (t) f.tag[kv.first] = *t;
    }
    return f;
}

// line period P: the median spacing of consecutive tags; 0 when the tags are missing
inline uint64_t line_period(const Frame& f) {
    std::vector<uint64_t> d;
    int prev = -1;
    for (const auto& kv : f.tag) {
        if (prev >= 0 && kv.first == prev + 1) d.push_back(kv.second - f.tag.at(prev));
        prev = kv.first;
    }
    if (d.empty()) return 0;
    std::sort(d.begin(), d.end());
    return d[d.size() / 2];
}

inline int line_at(const RunData& r, uint64_t t) {   // index of the output line containing time t
    int lo = 0, hi = int(r.lines.size()) - 1, ans = -1;
    while (lo <= hi) {
        int m = (lo + hi) / 2;
        if (r.lines[m].begin <= t) { ans = m; lo = m + 1; } else hi = m - 1;
    }
    return ans;
}

inline std::optional<size_t> first_nonblack(const RunData& r, int L) {
    if (L < 0 || size_t(L) >= r.lines.size()) return std::nullopt;
    for (size_t i = r.lines[L].begin; i < r.lines[L].end && i < r.vid.size(); ++i)
        if (r.vid[i].r || r.vid[i].g || r.vid[i].b) return i - r.lines[L].begin;
    return std::nullopt;
}

inline uint16_t pix(const RunData& r, int L, size_t x) {
    size_t i = r.lines[L].begin + x;
    if (i >= r.lines[L].end || i >= r.vid.size()) return 0xFFFF;
    return col_of(r.vid[i]);
}

// the background colour of an output line: the colour of the sample 8 samples
// after the first non-blank sample (the left border)
inline uint16_t bg_of(const RunData& r, int L) {
    auto fb = first_nonblack(r, L);
    if (!fb) return 0xFFFF;
    return pix(r, L, *fb + 8);
}

// output line of chip line n of the frame: the first of the first pair of
// consecutive output lines that shows background tag(n) after t_tag(n)
inline int out_line(const RunData& r, const Frame& f, int n) {
    auto it = f.tag.find(n);
    if (it == f.tag.end()) return -1;
    int L0 = line_at(r, it->second);
    if (L0 < 0) return -1;
    uint16_t want = tag_col(n);
    for (int L = L0; L < L0 + 8 && size_t(L) + 1 < r.lines.size(); ++L)
        if (bg_of(r, L) == want && bg_of(r, L + 1) == want) return L;
    return -1;
}

}  // namespace s3
