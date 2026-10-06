// s5_audio.h -- output runs, levels and passes (SPECIFICATION.md 2.5, 2.6, 2.10)
// and the propositions that grade them: P2, P3, P4.
#pragma once

#include "s5_p1.h"

namespace s5 {

struct LRun { uint64_t t0, t1; int32_t v; uint64_t len() const { return t1 - t0; } };

inline std::vector<LRun> runs_of(const Cap& c, uint64_t t0, uint64_t t1) {
    std::vector<LRun> out;
    for (auto& s : segments(c, t0, t1)) out.push_back({s.t0, s.t1, s.v});
    return out;
}

// Z: the mean of O over two lines while all AUDxVOL are 0 and no audio DMA runs
// (the VHPOSR poll that follows the baseline).
inline double zero_ref(const Cap& c, const Prefix& p, uint64_t K) {
    if (!log_has(c, p.i_gc)) return 0;
    uint64_t t0 = c.log[size_t(p.i_gc)].first, n = 2 * 227 * K;
    double sum = 0;
    for (uint64_t t = t0; t < t0 + n; ++t) sum += out_at(c, t);
    return sum / double(n);
}

inline int run_after(const std::vector<LRun>& r, uint64_t t) {
    for (size_t i = 0; i < r.size(); ++i) if (r[i].t0 > t) return int(i);
    return -1;
}

inline std::vector<size_t> returns_idx(const std::vector<Fetch>& f, uint32_t base) {
    std::vector<size_t> v;
    for (size_t i = 0; i < f.size(); ++i) if (f[i].addr == base) v.push_back(i);
    return v;
}

inline bool within(double a, double b, double tol) { return std::fabs(a - b) <= tol; }

// One audio run of a script: setup, markers, capture.
struct ARun {
    Cap cap; Prefix pre; Std st; Frame fr; double Z = 0; uint64_t tau = 0, K = 0;
    int m_start = 0, m_stop = 0, i_dmaconr = -1;
    std::vector<Fetch> f;
    std::vector<LRun> runs;
};

inline void finish(ARun& r, uint32_t base, int x) {
    r.fr = analyse_refresh(r.cap);
    r.K = r.fr.K ? r.fr.K : 8;
    r.tau = 1 + r.K;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    uint64_t t0 = r.m_start ? marker(r.cap, r.m_start) : 0, t1 = r.m_stop ? marker(r.cap, r.m_stop + 1) : ~0ull;
    r.f = fetches(r.cap, base - 0x40, base + 0x1000, t0, t1);
    r.runs = runs_of(r.cap, t0, t1 == ~0ull ? r.cap.base + r.cap.audio.size() : t1);
    r.st = std_pre(r.cap, r.pre, r.f);
    (void)x;
}

// ---- passes -----------------------------------------------------------------
// Pass p >= 2 begins with the p-th fetch at the location. Its j-th run from the anchor
// belongs to its j-th word. Returns the first run index (or -1) and the anchor fetch index.
struct Pass { int fi = -1; int r0 = -1; uint64_t ta = 0; bool g_w = false; };

inline Pass pass_of(const ARun& r, uint32_t base, int p) {
    Pass ps;
    auto ret = returns_idx(r.f, base);
    if (int(ret.size()) < p) return ps;
    ps.fi = int(ret[size_t(p - 1)]);
    ps.ta = r.f[size_t(ps.fi)].t;
    ps.r0 = run_after(r.runs, ps.ta);
    ps.g_w = true;
    for (auto& u : r.runs) if (u.t0 + r.tau > ps.ta && u.t0 < ps.ta + r.tau) ps.g_w = false;
    return ps;
}

// ---- P2 ---------------------------------------------------------------------

constexpr uint16_t kA = 0x6060, kB = 0xE0E0, kC = 0x2020, kD = 0xA0A0;

inline ARun p2_run(int x, bool swap34, bool wrong_write, bool write_early) {
    ARun r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    std::vector<uint16_t> data = {kA, kB, kA, kC, kB, kD, kC, kD};
    if (swap34) std::swap(data[2], data[3]);
    if (mut("P2-B")) data[7] = 0xA060;                    // contrast: the last word has two different bytes
    poke_words(rs.pokes, kBuf[x], data);
    s.w32(audlc(x), kBuf[x]);
    s.w16(audlen(x), mut("P2-LEN") ? 7 : 8); s.w16(audper(x), mut("P2-PER") ? 600 : 300); s.w16(audvol(x), 0x0040);
    if (write_early) s.wm(kBuf[x] + 10, kA);                  // contrast: RAM written before the DMA starts
    r.m_start = s.w16(DMACON, start_val(1u << x));
    r.i_dmaconr = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(12000));
    if (!write_early) s.wm(kBuf[x] + 10, wrong_write ? kD : kA);
    s.dly(wait_iters(17000));
    r.m_stop = s.w16(DMACON, uint16_t(1u << x));
    s.dly(200);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 60000;
    r.cap = run(rs);
    finish(r, kBuf[x], x);
    return r;
}

inline void run_p2(Checks& ck) {
    std::vector<ARun> runs;
    for (int x = 0; x < nch(); ++x) runs.push_back(p2_run(x, mut("P2-SWAP"), mut("P2-C"), mut("P2-W")));
    PreAgg agg;
    bool gd = true, gf = true, gw = true, gp = true, blocks8 = true, wr_both = true, flat = true;
    bool a_ok = true, b_ok = true, c_ok = true, lev4 = true;
    const uint16_t pat[8] = {kA, kB, kA, kC, kB, kD, kC, kD};
    for (int x = 0; x < nch(); ++x) {
        ARun& r = runs[size_t(x)];
        agg.add(r.st);
        note_gc(ck, "P2 ch" + dec(x), r.st);
        uint16_t d = logv(r.cap, r.i_dmaconr);
        gd &= (d & 0x200) && (d & (1u << x));
        auto ret = returns_idx(r.f, kBuf[x]);
        gf &= ret.size() >= 6;
        Pass ps = pass_of(r, kBuf[x], 2);
        if (ps.fi < 0 || ps.r0 < 0) { gf = false; gw = false; continue; }
        gw &= ps.g_w;
        // the write: the CPU's write to word 6 after the start
        uint64_t tws = 0, twe = 0;
        {
            uint64_t t0 = marker(r.cap, r.m_start);
            for (auto& a : r.cap.ram) if (a.is_write && a.cycle > t0 && a.word_addr * 2 == kBuf[x] + 10 && (a.data == kA || a.data == kD) && !tws) { tws = a.cycle; }
            twe = tws;
        }
        // the fetches of passes >= 2, minus the last two words before the stop
        std::vector<Fetch> f2(r.f.begin() + ps.fi, r.f.end());
        size_t nw = f2.size() >= 2 ? f2.size() - 2 : 0;
        // blocks of eight words from pass 2 on
        for (size_t j = 0; j + 1 < f2.size(); ++j) {
            uint32_t want = f2[j].addr == kBuf[x] + 14 ? kBuf[x] : f2[j].addr + 2;
            if (f2[j + 1].addr != want) blocks8 = false;
        }
        std::vector<int> lev(nw, 0);
        bool paired = ps.r0 >= 0 && size_t(ps.r0) + nw <= r.runs.size();
        if (!paired) { a_ok = b_ok = c_ok = false; gp = false; continue; }
        // expected word of fetch j: by position in the data set (before / after the CPU write)
        auto word_of = [&](size_t j) -> uint16_t {
            uint32_t k = (f2[j].addr - kBuf[x]) / 2;
            if (k == 5 && tws && f2[j].t > twe) return kA;
            return pat[k];
        };
        double LA = 0, LD = 0; bool haveA = false, haveD = false;
        for (size_t j = 0; j < nw; ++j) {
            const LRun& u = r.runs[size_t(ps.r0) + j];
            double lv = double(u.v) - r.Z;
            if (u.len() + r.tau < 600 * r.K || u.len() > 600 * r.K + r.tau) gp = false;   // 600 colour clocks
            if (!haveA && f2[j].addr == kBuf[x]) { LA = lv; haveA = true; }
            if (!haveD && f2[j].addr == kBuf[x] + 14) { LD = lv; haveD = true; }
            b_ok &= u.t0 > f2[j].t;
        }
        (void)flat;
        // (a): equal levels iff equal words; four distinct values give four distinct levels
        for (size_t j = 0; j < nw; ++j)
            for (size_t k = j + 1; k < nw; ++k) {
                bool eq = within(double(r.runs[size_t(ps.r0) + j].v), double(r.runs[size_t(ps.r0) + k].v), 1.0);
                if (eq != (word_of(j) == word_of(k))) a_ok = false;
            }
        {   // four distinct word values -> four distinct levels
            std::map<uint16_t, double> lw;
            for (size_t j = 0; j < nw; ++j) lw[word_of(j)] = double(r.runs[size_t(ps.r0) + j].v);
            if (lw.size() != 4) lev4 = false;
            for (auto& a : lw) for (auto& b : lw) if (a.first != b.first && within(a.second, b.second, 1.0)) lev4 = false;
        }
        // (c): fetches of word 6 after the write give A's level, before it D's level
        int nbefore = 0, nafter = 0;
        for (size_t j = 0; j < nw; ++j) {
            if (f2[j].addr != kBuf[x] + 10) continue;
            double lv = double(r.runs[size_t(ps.r0) + j].v) - r.Z;
            if (tws && f2[j].t > twe) { ++nafter; c_ok &= haveA && within(lv, LA, 1.0); }
            else if (!tws || f2[j].t < tws) { ++nbefore; c_ok &= haveD && within(lv, LD, 1.0); }
        }
        wr_both &= tws != 0 && nbefore >= 1 && nafter >= 1;
        ck.note("P2 ch" + dec(x) + ": pass-2 anchor fetch at cycle " + dec((long long)ps.ta) + ", " + dec((long long)f2.size()) + " fetches from it, " + dec((long long)r.runs.size()) +
                " runs in the interval, word-6 fetches before/after the write " + dec(nbefore) + "/" + dec(nafter) + ", write at cycle " + dec((long long)tws) +
                ", L(A)=" + num(LA) + " L(D)=" + num(LD));
    }
    print_std(ck, "P2", agg, runs.size());
    ck.expect(gd, "P2-G-D: after each start DMACONR bit 9 and the channel's bit read 1 on all four channels (AA p.22)");
    ck.expect(gf, "P2-G-F: each channel makes at least six fetches at the location, so passes 1 to 6 start (AA p.22 '00 AUD0EN')");
    ck.expect(blocks8, "P2-PRE-BLOCKS8: from pass 2 on the fetches of each channel form eight-word blocks BUFx .. BUFx+14 (the P1 check, AA p.11 AUDxLEN)");
    ck.expect(gw, "P2-G-W: on every channel no run starts within tau of the pass-2 anchor fetch (SPECIFICATION.md 2.10)");
    ck.expect(gp, "P2-G-P: from pass 2 on every word gives one run of 600 colour clocks within tau (AA p.11 AUDxPER 'The minimum period is 124 color clocks')");
    ck.expect(wr_both, "P2-PRE-WRITE: the CPU's write to word 6 is seen at the port and a fetch of word 6 from pass 2 on falls on each side of it, on every channel");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && blocks8 && gw && gp && wr_both;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F, eight-word blocks, G-W, G-P or the write precondition";
    graded(ck, pre, a_ok && lev4, "P2-A: from pass 2 on, runs pair one to one with the fetches in order; two runs are equal in level iff the words of their fetches are equal, and the four word values give four distinct levels (AA p.11 'The audio DMA channel controller automatically transfers data to this register from RAM')", un);
    graded(ck, pre, b_ok, "P2-B: each run begins after the time of its fetch (AA p.11 'automatically transfers data to this register from RAM')", un);
    graded(ck, pre, c_ok, "P2-C: every fetch of word 6 from pass 2 on that follows the CPU's write gives a run at A's level and every one before it gives D's level (AA p.11 'This register is the Audio channel x (x=0,1,2,3) DMA data buffer')", un);
    graded(ck, pre, a_ok && lev4 && b_ok && c_ok,
           "P2 sol:AA-audxdat.c5 sol:AA-audxdat.c9: AUDxDAT at 0AA (0BA, 0CA, 0DA) is channel x's DMA data buffer and the audio DMA transfers the words fetched from RAM into it, so the output follows the RAM words in fetch order (AA p.11)", un);
}

} // namespace s5
