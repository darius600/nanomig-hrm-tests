// s5_p58.h -- P5 (AUDxDAT written by the CPU), P6 and P7 (AUDxVOL), P8 (the block-finished request).
#pragma once

#include "s5_p34.h"

namespace s5 {

inline double mean_o(const Cap& c, uint64_t t0, uint64_t t1) {
    if (t1 <= t0) return 0;
    double sum = 0;
    for (uint64_t t = t0; t < t1; ++t) sum += out_at(c, t);
    return sum / double(t1 - t0);
}

// ---- P5 ---------------------------------------------------------------------

struct P5Run { ARun r; std::vector<int> m; int m_end = 0; int i_dmaconr2 = -1; };

inline P5Run p5_run(int x) {
    P5Run p;
    ARun& r = p.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    uint16_t vals[8] = {0x0000, 0x7F7F, 0x8080, 0x0000, 0x7F80, 0x0000, 0x807F, 0x0000};
    if (mut("P5-SWAP")) { vals[1] = 0x8080; vals[2] = 0x7F7F; }   // contrast: the two settled levels exchanged
    s.w16(audvol(x), 64);
    s.w16(audper(x), 400);
    if (mut("P5-DMA")) {   // contrast: the channel's DMA runs
        poke_words(rs.pokes, kBuf[x], {0x0000, 0x0000, 0x0000, 0x0000});
        s.w32(audlc(x), kBuf[x]); s.w16(audlen(x), 4);
        s.w16(DMACON, 0x8200 | (1u << x));
        p.i_dmaconr2 = r.pre.logn++; s.rd(DMACONR);
    }
    for (int k = 0; k < 8; ++k) {
        uint16_t v = vals[k];
        if (mut("P5-VOL") && k == 4) s.w16(audvol(x), 0);                    // contrast: volume 0 for the 0x7F80 write
        s.w16(INTREQ, uint16_t(1u << (7 + x)));
        int m;
        if (mut("P5-ADDR") && x == 0 && k == 4) { s.w16(audvol(1), 0); m = s.w16(0x0BA, v); }   // contrast: AUD1DAT in place of AUD0DAT
        else m = s.w16(auddat(x), v);
        p.m.push_back(m);
        s.dly(wait_iters(mut("P5-SHORT") ? 300 : 4000));
        if (mut("P5-VOL") && k == 4) s.w16(audvol(x), 64);
    }
    p.m_end = s.mark();
    s.dly(100);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 40000;
    r.cap = run(rs);
    finish(r, kBuf[x], x);
    // fetches over the whole run, whatever the markers: the channel's DMA must be off
    r.f = fetches(r.cap, kBuf[x] - 0x40, kBuf[x] + 0x1000, marker(r.cap, p.m[0]), r.cap.base + r.cap.audio.size());
    r.st = std_pre(r.cap, r.pre, {});
    return p;
}

inline void run_p5(Checks& ck) {
    std::vector<P5Run> runs;
    for (int x = 0; x < nch(); ++x) runs.push_back(p5_run(x));
    PreAgg agg;
    bool off = true, flat = true, order = true, same_z = true, pairs = true;
    for (int x = 0; x < nch(); ++x) {
        P5Run& p = runs[size_t(x)];
        ARun& r = p.r;
        agg.add(r.st);
        note_gc(ck, "P5 ch" + dec(x), r.st);
        uint16_t dm = logv(r.cap, r.pre.i_dmaconr);
        if (p.i_dmaconr2 >= 0) dm = logv(r.cap, p.i_dmaconr2);
        off &= !(dm & (1u << x)) && r.f.empty();
        // windows [marker k, marker k+1]
        struct W { std::vector<LRun> seg; uint64_t t0, t1; };
        std::vector<W> w;
        for (int k = 0; k < 8; ++k) {
            uint64_t t0 = marker(r.cap, p.m[size_t(k)]);
            uint64_t t1 = marker(r.cap, k + 1 < 8 ? p.m[size_t(k + 1)] : p.m_end);
            w.push_back({runs_of(r.cap, t0, t1), t0, t1});
        }
        auto last = [&](int k) { return w[size_t(k)].seg.empty() ? LRun{0, 0, 0} : w[size_t(k)].seg.back(); };
        const uint64_t settle = 2 * 400 * r.K;
        for (int k : {1, 2}) flat &= last(k).len() >= settle;
        double H = last(1).v, Lo = last(2).v, Z0 = last(0).v;
        for (int k : {0, 3, 5, 7}) same_z &= double(last(k).v) == Z0;
        order &= H > Z0 && Z0 > Lo;
        for (int k : {4, 6}) {
            auto& sg = w[size_t(k)].seg;
            bool ok = sg.size() >= 3;
            if (ok) {
                double s1 = sg[1].v, s2 = sg[2].v;
                bool hl = s1 == H && s2 == Lo, lh = s1 == Lo && s2 == H;
                ok = hl || lh;
            }
            pairs &= ok;
            std::string lv;
            for (size_t i = 0; i < sg.size() && i < 6; ++i) lv += dec(sg[i].v) + "(" + dec((long long)sg[i].len()) + ") ";
            ck.note("P5 ch" + dec(x) + " window of write " + dec(k) + " (levels, lengths in cycles): " + lv);
        }
        ck.note("P5 ch" + dec(x) + ": H=" + num(H) + " Lo=" + num(Lo) + " Z0=" + num(Z0) + " DMACONR=" + hex4(dm) + " fetches=" + dec((long long)r.f.size()));
    }
    print_std(ck, "P5", agg, runs.size());
    ck.expect(flat, "P5-G-FLAT: after 0x7F7F and after 0x8080 the output settles at one level that it keeps for at least two periods, on every channel (SPECIFICATION.md 2.5)");
    ck.expect(off, "P5-PRE-OFF: DMACONR bit x reads 0 and channel x makes no fetch on every channel, so the channel's DMA is off (AA p.22 '00 AUD0EN Audio channel 0 DMA enable')");
    bool pre = agg.fixture && agg.s0 && agg.gc && flat && off;
    const std::string un = "fixture, G-S0, G-C, G-FLAT or the DMA-off precondition";
    graded(ck, pre, order && same_z, "P5-LEVELS: after 0x7F7F the output settles at H, after 0x8080 at Lo and after 0x0000 at one level Z0, with H > Z0 > Lo, on every channel (AA p.11 'The Processor can also write directly to this register' and 'twos complement signed integer')", un);
    graded(ck, pre, pairs, "P5-PAIR: after 0x7F80 and after 0x807F the first change of the output goes to H or Lo and the next change goes to the other of the two, on every channel (AA p.11 'outputed sequentially')", un);
    graded(ck, pre, order && same_z && pairs, "P5 sol:AA-audxdat.c1 sol:AA-audxdat.c10: a CPU write at channel x's AUDxDAT (0AA, 0BA, 0CA, 0DA) with the channel's DMA off makes the channel output the two bytes written (AA p.11 'The Processor can also write directly to this register')", un);
}

// ---- P6 and P7 --------------------------------------------------------------

// A measuring window of constant data at one volume: the output repeats a pattern (AA p.11: a volume below 64 is a pattern of
// ones and zeros), and the level of the window is the mean over a whole number of repetitions. The period is the smallest
// distance at which the output repeats over the whole window; a window holds a whole number of repetitions only if that
// distance is at most half the window.
struct PatWin {
    bool ok = false;               // a period of at most half the window exists
    uint64_t pi = 0, whole = 0;    // the period and the number of output samples in the whole repetitions from the window start
    long long sum = 0;             // the sum of O over those samples
    bool constant = false;
};

inline PatWin pat_window(const Cap& c, uint64_t t0, uint64_t t1) {
    PatWin w;
    if (t1 <= t0) return w;
    const uint64_t n = t1 - t0;
    std::vector<int32_t> o(n);
    for (uint64_t i = 0; i < n; ++i) o[i] = out_at(c, t0 + i);
    for (uint64_t pi = 1; pi <= n / 2; ++pi) {
        bool same = true;
        for (uint64_t i = 0; i + pi < n; ++i) if (o[i] != o[i + pi]) { same = false; break; }
        if (same) { w.ok = true; w.pi = pi; break; }
    }
    if (!w.ok) return w;
    w.constant = w.pi == 1;
    w.whole = (n / w.pi) * w.pi;
    for (uint64_t i = 0; i < w.whole; ++i) w.sum += o[i];
    return w;
}

// The level of a window relative to Z, as the exact rational (sum - Z*whole) / whole: numerator and denominator.
struct Lvl { long double num = 0; long double den = 1; };
inline Lvl lvl_of(const PatWin& w, double Z) { return {(long double)w.sum - (long double)Z * (long double)w.whole, (long double)w.whole}; }
// a/b * k == c/d * m, exactly (cross-multiplied)
inline bool prop_eq(const Lvl& a, long double k, const Lvl& b, long double m) { return a.num * b.den * k == b.num * a.den * m; }
inline bool lvl_eq(const Lvl& a, const Lvl& b) { return a.num * b.den == b.num * a.den; }

struct VolSweep {
    ARun r;
    std::vector<int> m6, m7;           // markers of the writes v = 0..63, 64 and 0x40|k
    std::vector<int> m64;
    int m_end = 0;
    double A6[65] = {}, A7[64] = {};
    PatWin P6[65], P7[64];
    bool win_ok = true;
    double zero_dev = 0;               // the largest |O - Z| over the windows of v = 0
};

inline VolSweep vol_sweep(int x, uint16_t word, bool p7) {
    VolSweep v;
    ARun& r = v.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[x], {word});
    s.w32(audlc(x), kBuf[x]); s.w16(audlen(x), 1); s.w16(audper(x), 200); s.w16(audvol(x), 0x0040);
    r.m_start = s.w16(DMACON, start_val(1u << x));
    r.i_dmaconr = r.pre.logn++; s.rd(DMACONR);
    s.bwait(wc(2 * 227 * 3));
    uint16_t vreg = mut("P6-ADDR") && x == 0 ? 0x0B8 : audvol(x);
    for (int val = 0; val <= 64; ++val) {
        uint16_t w = uint16_t(val == 64 ? 0x0040 : val);
        if (mut("P6") && val == 32) w = 16;
        if (mut("P6-ZERO") && val == 0) w = 1;                    // contrast: volume 1 where the grader expects 0
        v.m6.push_back(s.w16(vreg, w));
        s.bwait(mut("P6-SHORT") ? 250 : 1200);
    }
    if (p7) {
        for (int k = 0; k < 64; ++k) {
            uint16_t w = mut("P7") ? uint16_t(k) : uint16_t(0x40 | k);
            v.m7.push_back(s.w16(audvol(x), w));
            s.bwait(1200);
        }
    }
    v.m_end = s.mark();
    r.m_stop = s.w16(DMACON, uint16_t(1u << x));
    s.dly(100);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 227 * 5 * 140 * (p7 ? 2 : 1) * 2;
    r.cap = run(rs);
    finish(r, kBuf[x], x);
    return v;
}

inline bool win_level(const VolSweep& v, int m_write, int m_next, double& out, PatWin* pw = nullptr, double* dev = nullptr) {
    const Cap& c = v.r.cap;
    uint64_t ta = marker(c, m_write + 1), tn = marker(c, m_next);
    uint64_t line = 227 * v.r.K;
    uint64_t t0 = ta + 2 * line, t1 = ta + 4 * line;
    if (!ta || !tn || t1 > tn) return false;
    out = mean_o(c, t0, t1) - v.r.Z;
    if (pw) *pw = pat_window(c, t0, t1);
    if (dev) { *dev = 0; for (uint64_t t = t0; t < t1; ++t) *dev = std::max(*dev, std::fabs(double(out_at(c, t)) - v.r.Z)); }
    return true;
}

inline void run_p67(Checks& ck) {
    const bool p7 = true;
    const std::string pn = "P6";
    std::vector<VolSweep> sw[2];   // [0]: 0x7F7F, [1]: 0x8080
    for (int x = 0; x < nch(); ++x) {
        sw[0].push_back(vol_sweep(x, mut("P6-PAT") ? 0x7F00 : 0x7F7F, p7));   // P6-PAT: two different bytes, so the output has no short period
        sw[1].push_back(vol_sweep(x, mut("P6-SIGN") ? 0x7F7F : 0x8080, p7));   // contrast: both data words 0x7F7F
    }
    PreAgg agg;
    bool gd = true, gf = true, win = true;
    bool per6 = true, per7 = true;      // every window of the sweep (P6), or of the sweeps at 64 and 0x40|k (P7), holds a whole number of repetitions
    bool addr = true, sign = true, zero = true, distinct = true, lin = true, prop = true, bit6 = true;
    for (int x = 0; x < nch(); ++x) {
        for (int d = 0; d < 2; ++d) {
            VolSweep& v = sw[d][size_t(x)];
            agg.add(v.r.st);
            uint16_t dm = logv(v.r.cap, v.r.i_dmaconr);
            gd &= (dm & 0x200) && (dm & (1u << x));
            gf &= v.r.f.size() >= 20;
            for (int val = 0; val <= 64; ++val) {
                int mn = val < 64 ? v.m6[size_t(val + 1)] : (p7 ? v.m7[0] : v.m_end);
                double dev = 0;
                win &= win_level(v, v.m6[size_t(val)], mn, v.A6[val], &v.P6[val], &dev);
                per6 &= v.P6[val].ok;
                if (val == 64) per7 &= v.P6[val].ok;
                if (val == 0) v.zero_dev = dev;
            }
            if (p7)
                for (int k = 0; k < 64; ++k) {
                    int mn = k < 63 ? v.m7[size_t(k + 1)] : v.m_end;
                    win &= win_level(v, v.m7[size_t(k)], mn, v.A7[k], &v.P7[k]);
                    per7 &= v.P7[k].ok;
                }
        }
    }
    // the lines that rest on the levels are formed only when every window holds a whole number of repetitions
    for (int x = 0; x < nch(); ++x) {
        VolSweep& hi = sw[0][size_t(x)];
        VolSweep& lo = sw[1][size_t(x)];
        sign &= hi.A6[64] > 0 && lo.A6[64] < 0;
        zero &= hi.zero_dev == 0 && lo.zero_dev == 0;
        for (int d = 0; d < 2; ++d) {
            VolSweep& v = sw[d][size_t(x)];
            Lvl L[65];
            for (int val = 0; val <= 64; ++val) L[val] = lvl_of(v.P6[val], v.r.Z);
            if (per6) {
                addr &= !lvl_eq(L[0], L[64]);
                // 65 levels: pairwise distinct, and linear in the volume value, 64 ones being the level of 0x0040
                for (int a = 0; a <= 64; ++a) for (int b = a + 1; b <= 64; ++b) distinct &= !lvl_eq(L[a], L[b]);
                for (int val = 0; val <= 63; ++val) lin &= prop_eq(L[val], 64.0L, L[64], (long double)val);
                // 64 slots: among the volumes 1..63 the level is proportional to the number of ones
                for (int a = 1; a <= 63; ++a) for (int b = a + 1; b <= 63; ++b) prop &= prop_eq(L[a], (long double)b, L[b], (long double)a);
            }
            if (per7)
                for (int k = 0; k < 64; ++k) bit6 &= lvl_eq(lvl_of(v.P7[k], v.r.Z), L[64]);
            std::string lv;
            for (int val : {0, 1, 16, 32, 48, 62, 63, 64}) lv += dec(val) + ":" + num(v.A6[val]) + "(" + dec((long long)v.P6[val].pi) + ") ";
            lv += " | 0x40|k: 0:" + num(v.A7[0]) + " 31:" + num(v.A7[31]) + " 63:" + num(v.A7[63]);
            ck.note(pn + " ch" + dec(x) + (d ? " word 0x8080" : " word 0x7F7F") + " A(v)(period in cycles): " + lv);
        }
        note_gc(ck, pn + " ch" + dec(x), hi.r.st);
    }
    if (!per6) { addr = distinct = lin = prop = false; }
    if (!per7) bit6 = false;
    for (int d = 0; d < 2; ++d) for (auto& v : sw[d]) agg.add(v.r.st);
    for (const std::string pq : {std::string("P6"), std::string("P7")}) {
        const size_t nrun = sw[0].size() + sw[1].size();
        print_std(ck, pq, agg, nrun);
        ck.expect(gd, pq + "-G-D: after each start DMACONR bit 9 and the channel's bit read 1 in all " + dec((long long)nrun) + " runs (AA p.22)");
        ck.expect(gf, pq + "-G-F: the channel's fetches occur in all " + dec((long long)nrun) + " runs (AA p.22 '00 AUD0EN')");
        ck.expect(win, pq + "-PRE-WIN: every measuring window starts two lines after its write marker and ends before the next write marker (SPECIFICATION.md P6 step 2)");
        if (pq == "P6") {
            ck.expect(per6, "P6-PRE-PERIOD: in every measuring window of the sweep the output repeats with a period of at most half the window, so the level of the window is the mean over a whole number of repetitions (AA p.11 '(111111=63 0nes, one zero)')");
            bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && win && per6;
            const std::string un = "fixture, G-S0, G-C, G-D, G-F, the windows or the repetition period";
            graded(ck, pre, addr && sign, "P6-ADDR sol:AA-audxvol.c1: AUDxVOL at 0A8 (0B8, 0C8, 0D8) is the channel's volume: a write there takes the level of constant data from the level of 0x0000 to the level of 0x0040, which has the sign of the data word (positive for 0x7F7F, negative for 0x8080) (AA p.11 'AUDxVOL 0A8 W P Audio Channel x Volume')", un);
            graded(ck, pre, distinct, "P6-DISTINCT: the 65 levels written as v = 0..63 and as 0x0040 give 65 pairwise different levels for both data words and every channel (AA p.11 'Bits 6,5,4,3,2,1,0 specify 65 linear volume levels')", un);
            graded(ck, pre, lin, "P6-LIN: for both data words and every channel and every v in 0..63 the level over a whole number of repetitions is v/64 of the level of 0x0040: 64 ones are the level of 0x0040 and v ones in 64 slots are v/64 of it (AA p.11 'Forces volume to max (64 ones, no zeros)', '(111111=63 0nes, one zero)')", un);
            graded(ck, pre, distinct && lin, "P6-LEVELS sol:AA-audxvol.c4: AUDxVOL bits 6 to 0 specify 65 linear volume levels: the levels of v = 0..63 and of 0x0040 are pairwise different and the level of v is v/64 of the level of 0x0040, for both data words and every channel (AA p.11 'Bits 6,5,4,3,2,1,0 specify 65 linear volume levels as shown below')", un);
            graded(ck, pre, zero, "P6-ZERO: at v = 0 the output equals Z at every sample of the window, for both data words and every channel (AA p.11 '000000=no output')", un);
            graded(ck, pre, prop, "P6-PROP: among the volumes v = 1..63 the level over a whole number of repetitions is proportional to v, the number of ones in the 64 slots, for both data words and every channel: level(a) * b == level(b) * a (AA p.11 'Sets one of 64 levels', '(111111=63 0nes, one zero)')", un);
            graded(ck, pre, zero && prop, "P6-LOWER sol:AA-audxvol.c7: AUDxVOL bits 5 to 0 set one of 64 levels: 000000 gives no output and a volume v below 64 gives v ones in a pattern of 64 slots, so the levels of v = 1..63 are in proportion to v and that of v = 0 is the level of no output, for both data words and every channel (AA p.11 '05-00 Sets one of 64 levels (000000=no output (111111=63 0nes, one zero)')", un);
        } else {
            ck.expect(per7, "P7-PRE-PERIOD: in the measuring window of 0x0040 and in every measuring window of 0x40|k the output repeats with a period of at most half the window (AA p.11 '(111111=63 0nes, one zero)')");
            bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && win && per7;
            const std::string un = "fixture, G-S0, G-C, G-D, G-F, the windows or the repetition period";
            graded(ck, pre, bit6, "P7 sol:AA-audxvol.c6: with bit 6 set the level is the maximum level whatever bits 5 to 0 hold: the level over a whole number of repetitions of 0x40|k equals that of 0x0040 for every k in 0..63, both data words, every channel (AA p.11 '06 Forces volume to max (64 ones, no zeros)')", un);
        }
    }
}

// ---- P8 ---------------------------------------------------------------------

struct R8 { ARun r; int m_cal0 = 0, m_cal1 = 0, m_start8 = 0; int m_clear0 = 0; double L9 = 0; uint64_t R = 0; size_t ipact = 0; uint16_t pmask = 0; };

// One play of channel x: a calibration play of one word, then the block play with the poll of INTREQR. other_bit polls the request bit of
// the next channel, which the channel's DMA never sets; ins lists the reads (counted from 0 in the poll) after which the CPU sets the polled
// bit by software, the live input of the contrasts below.
inline R8 p8_build(int x, bool other_bit, const std::vector<unsigned>& ins) {
    R8 q;
    ARun& r = q.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s, true, 119, mut("P8-INTENA") ? std::function<void(Script&)>([x](Script& sc) { sc.w16(INTENA, uint16_t(0xC000 | (1u << (7 + x)))); }) : nullptr);
    std::vector<uint16_t> blk = {0x1010, 0x3030, 0x5050, 0x7070, 0xD0D0, 0x9090};
    if (mut("P8-L9")) blk[5] = 0x9494;                 // contrast: the last word of the block is not the calibration word
    poke_words(rs.pokes, kBuf[x], blk);
    poke_words(rs.pokes, kBuf[x] + 0x100, {0x9090});
    // calibration
    s.w32(audlc(x), kBuf[x] + 0x100); s.w16(audlen(x), 1); s.w16(audper(x), 400); s.w16(audvol(x), 64);
    q.m_cal0 = s.w16(DMACON, start_val(1u << x));
    r.i_dmaconr = r.pre.logn++; s.rd(DMACONR);
    s.bwait(wc(8 * 227));
    q.m_cal1 = s.w16(DMACON, uint16_t(1u << x));
    s.dly(100);
    // the block run
    s.w32(audlc(x), kBuf[x]); s.w16(audlen(x), uint16_t(blk.size() + (mut("P8-LEN") ? 1 : 0))); s.w16(audper(x), 400); s.w16(audvol(x), 64);
    q.pmask = uint16_t(1u << (7 + (other_bit ? (x + 1) % 4 : x)));
    const uint16_t pbit = q.pmask;
    q.m_clear0 = s.w16(INTREQ, uint16_t(1u << (7 + x)));
    r.m_start = q.m_start8 = s.w16(DMACON, start_val(1u << x));
    q.ipact = size_t(r.pre.logn);
    const uint16_t cval = mut("P8-NOCLEAR") ? uint16_t(0) : pbit;
    if (mut("P8-R")) {   // contrast: a poll loop with a delay in it, so that R exceeds the bound
        for (int i = 0; i < 220; ++i) { s.rd(INTREQR); s.w16(INTREQ, pbit); s.dly(40); }
    } else if (ins.empty()) {
        s.pact(INTREQR, pbit, 5200, INTREQ, cval);
    } else {
        unsigned done = 0;
        for (unsigned j : ins) {
            s.pact(INTREQR, pbit, uint16_t(j + 1 - done - 1), INTREQ, cval);
            s.w16(INTREQ, uint16_t(0x8000 | pbit));          // the CPU sets the polled request
            done = j + 1;
        }
        s.pact(INTREQR, pbit, uint16_t(5201 - done - 1), INTREQ, cval);
    }
    r.m_stop = s.w16(DMACON, uint16_t(1u << x));
    s.dly(100);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 90000;
    r.cap = run(rs);
    r.fr = analyse_refresh(r.cap);
    r.K = r.fr.K ? r.fr.K : 8;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    uint64_t t0 = marker(r.cap, q.m_start8), t1 = marker(r.cap, r.m_stop + 1);
    r.f = fetches(r.cap, kBuf[x] - 0x40, kBuf[x] + 0x1000, t0, t1);
    r.runs = runs_of(r.cap, t0, t1);
    r.st = std_pre(r.cap, r.pre, r.f);
    // L9: the level over the last four lines of the calibration play
    uint64_t tc1 = marker(r.cap, q.m_cal1), line = 227 * r.K;
    q.L9 = mean_o(r.cap, tc1 - 4 * line, tc1) - r.Z;
    // R: the largest interval between consecutive log stores of the poll loop (an interval that holds a marker, a write of the CPU, is no poll interval)
    uint64_t Rm = 0;
    for (size_t i = q.ipact + 1; i < r.cap.log.size(); ++i) {
        uint64_t a = r.cap.log[i - 1].first, b = r.cap.log[i].first;
        if (a <= t0) continue;
        bool skip = false;
        for (uint64_t mt : r.cap.mark_t) if (mt > a && mt < b) skip = true;
        if (!skip) Rm = std::max(Rm, b - a);
    }
    q.R = Rm;
    return q;
}

// The blocks k = 2..6 of a play, with word L = 6 (offset 10) as the grader keeps it: F_k is the fetch of word L in block k, F1 that in block
// k+1, S_k the start of word L's run (the first run that starts after F_k at level L9) and E_k the start of the run after it.
struct Blk { size_t k = 0; bool ok = false; uint64_t Fk = 0, F1 = 0, Sk = 0, Ek = 0; };

inline std::vector<Blk> p8_blocks(const R8& q, int x) {
    std::vector<Blk> out;
    const ARun& r = q.r;
    auto ret = returns_idx(r.f, kBuf[x]);
    for (size_t k = 2; k <= 6; ++k) {
        Blk b; b.k = k;
        if (ret.size() >= k + 2) {
            size_t blk0 = ret[k - 1], blk1 = ret[k], blk2 = ret[k + 1];   // the k-th, (k+1)-th and (k+2)-th fetch at BUFx
            uint64_t Fnext2 = 0;
            for (size_t i = blk0; i < blk1; ++i) if (r.f[i].addr == kBuf[x] + 10) b.Fk = r.f[i].t;
            for (size_t i = blk1; i < blk2; ++i) if (r.f[i].addr == kBuf[x] + 10) b.F1 = r.f[i].t;
            if (blk1 + 1 < r.f.size()) Fnext2 = r.f[blk1 + 1].t;
            if (b.Fk && b.F1 && Fnext2) {
                int nl9 = 0; size_t wr = 0;
                for (size_t i = 0; i < r.runs.size(); ++i)
                    if (r.runs[i].t0 > b.Fk && r.runs[i].t0 < Fnext2 && double(r.runs[i].v) - r.Z == q.L9) { if (!nl9) wr = i; ++nl9; }
                if (nl9 == 1 && wr + 1 < r.runs.size()) { b.ok = true; b.Sk = r.runs[wr].t0; b.Ek = r.runs[wr + 1].t0; }
            }
        }
        out.push_back(b);
    }
    return out;
}

// The reads after which the CPU sets the polled request, in the contrasts of a planning play (the same play, nothing set): one in every block, at the
// middle of (F_k, S_k) for P8-EARLY and of (E_k, F_(k+1)) for P8-LIVE.
inline std::vector<unsigned> p8_plan(const R8& base, int x, int mode) {
    // Each set of the CPU delays the reads after it by about 1000 system clocks (the two markers and the write), so the m-th set is
    // placed (m+1) such delays before its target; the value is tuned on this design, and the run prints where every request fell.
    const uint64_t kSetDelay = 1000;
    std::vector<unsigned> ins;
    uint64_t m = 0;
    for (auto& b : p8_blocks(base, x)) {
        if (!b.ok) continue;
        uint64_t tt = mode == 0 ? (b.Fk + b.Sk) / 2 : (mode == 1 ? (b.Sk + b.Ek) / 2 : (b.Ek + b.F1) / 2);
        tt = tt > (m + 1) * kSetDelay ? tt - (m + 1) * kSetDelay : 0;
        for (size_t j = 0; base.ipact + j < base.r.cap.log.size(); ++j)
            if (base.r.cap.log[base.ipact + j].first >= tt) { ins.push_back(unsigned(j)); break; }
        ++m;
    }
    return ins;
}

inline void run_p8(Checks& ck) {
    const int plan_mode = mut("P8-EARLY") ? 0 : (mut("P8-LIVE") ? 2 : -1);
    std::vector<R8> runs;
    for (int x = 0; x < nch(); ++x) {
        if (plan_mode >= 0) {
            R8 base = p8_build(x, true, {});
            runs.push_back(p8_build(x, true, p8_plan(base, x, plan_mode)));
        } else runs.push_back(p8_build(x, mut("P8-BIT"), {}));
    }
    PreAgg agg;
    bool gd = true, gf = true, gr = true, once = true, blocks = true, clearok = true, started_ok = true, set_ok = true;
    for (int x = 0; x < nch(); ++x) {
        R8& q = runs[size_t(x)];
        ARun& r = q.r;
        agg.add(r.st);
        note_gc(ck, "P8 ch" + dec(x), r.st);
        uint16_t dm = logv(r.cap, r.i_dmaconr);
        gd &= (dm & 0x200) && (dm & (1u << x));
        auto ret = returns_idx(r.f, kBuf[x]);
        gf &= ret.size() >= 8;
        if (std::getenv("NMH_DEBUG")) {
            std::string fo;
            for (size_t i = 0; i < r.f.size() && i < 40; ++i) fo += dec((long long)(r.f[i].addr) - (long long)kBuf[x]) + " ";
            ck.note("P8 debug ch" + dec(x) + " fetch byte offsets: " + fo + "; returns=" + dec((long long)ret.size()));
        }
        gr &= q.R > 0 && q.R <= 50 * r.K;       // R <= P*c/8 = 400/8 colour clocks
        // blocks of L words as written (the P1 check), from pass 2 on
        {
            const unsigned Lw = 6;
            if (ret.size() >= 2) {
                Blocks b = block_check(r.f, kBuf[x], Lw, ret[1]);
                blocks &= b.distinct == Lw && b.wrap_bad == 0 && b.outside == 0 && b.step_bad == 0;
            } else blocks = false;
        }
        // events: reads of 1 in the polled bit whose previous read was 0, in the block run; the time of an event is its store time
        uint64_t tstart = marker(r.cap, q.m_start8);
        std::vector<uint64_t> ev;
        {
            int prev = -1;
            for (size_t i = size_t(q.ipact); i < r.cap.log.size(); ++i) {
                if (r.cap.log[i].first <= tstart) continue;
                int bit = (r.cap.log[i].second & q.pmask) ? 1 : 0;
                if (bit == 1 && prev == 0) ev.push_back(r.cap.log[i].first);
                if (bit == 1 && prev == 1) clearok = false;
                prev = bit;
            }
        }
        // The data of word L is in use from S_k to E_k; at E_k the output of the last word is over and its data has been used (AA p.11 to 12),
        // so the request of block k is set at E_k or after. A read of 1 that is stored before E_k was read before E_k.
        std::string wn;
        for (auto& b : p8_blocks(q, x)) {
            if (!b.ok) { once = false; continue; }
            int n_start = 0, n_used = 0, n_set = 0;
            long long first_ev = -1;
            for (uint64_t ee : ev) {
                if (ee >= b.Fk && ee < b.Sk) ++n_start;
                if (ee >= b.Fk && ee < b.Ek) ++n_used;
                if (ee >= b.Sk && ee < b.F1) ++n_set;
                if (ee >= b.Fk && first_ev < 0) first_ev = (long long)(ee - b.Fk);
            }
            if (n_start != 0) started_ok = false;
            if (n_set < 1) set_ok = false;
            wn += "k" + dec((long long)b.k) + ": S-F=" + dec((long long)(b.Sk - b.Fk)) + " E-F=" + dec((long long)(b.Ek - b.Fk)) + " first event-F=" + dec(first_ev) + " events before S/before E (P8-USED, measured and not graded: the page leaves the reading of used open)/from S=" + dec(n_start) + "/" + dec(n_used) + "/" + dec(n_set) + "; ";
        }
        ck.note("P8 ch" + dec(x) + ": events=" + dec((long long)ev.size()) + " per block (cycles from the fetch of word L): " + wn + "R=" + dec((long long)q.R) + " cycles, L9=" + num(q.L9));
    }
    print_std(ck, "P8", agg, runs.size());
    ck.expect(gd, "P8-G-D: after each start DMACONR bit 9 and the channel's bit read 1 on all four channels (AA p.22)");
    ck.expect(gf, "P8-G-F: each channel makes at least eight fetches at the location, so blocks 1 to 7 start (AA p.22 '00 AUD0EN')");
    ck.expect(gr, "P8-G-R: R, the largest interval between consecutive log stores of the poll loop, is at most P*c/8 = 50 colour clocks on every channel (SPECIFICATION.md 2.3)");
    ck.expect(blocks, "P8-PRE-BLOCKS: from pass 2 on the fetches show blocks of as many words as the program wrote to AUDxLEN (the P1 check)");
    ck.expect(once, "P8-PRE-L9: in each block k = 2..6 the fetch of word L in blocks k and k+1 and the fetch of word 2 of block k+1 are found, and exactly one run at level L9 starts between F_k and the fetch of word 2 of block k+1, on every channel");
    ck.expect(clearok, "P8-PRE-CLEAR: no two consecutive polls read 1: each clear took effect before the next poll (AA p.25 'These bits may be polled by the processor')");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && gr && blocks && once && clearok;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F, G-R, the blocks, the L9 runs or the clear";
    graded(ck, pre, started_ok, "P8-STARTED: in each block k = 2..6 no read of 1 in the channel's request bit is stored from F_k, the fetch of word L, to S_k, the start of word L's run: the request is clear until the last data is first in use (AA p.11 to 12 'and the data in this register has been used'; both readings of 'used' require this)", un);
    graded(ck, pre, set_ok, "P8-SET: in each block k = 2..6 a read of 1 in the channel's request bit is stored from S_k to F_(k+1), the fetch of word L in block k+1: the request is set once the last data is in use (AA p.11 to 12 'an audio channel interrupt request is set')", un);
    graded(ck, pre, started_ok && set_ok, "P8 sol:AA-audxdat.c11: for each channel and each block k = 2..6 the channel's interrupt request bit (INTREQ bit 7+x) reads 0 until the data of word L is in use and is set from then on: no read of 1 is stored before S_k and one is stored between S_k and the next block's last fetch; whether it is set at S_k or at E_k, when the output of word L is over, the page leaves open (AA p.11 to 12 'When the DMA data is finished (words outputted=Length) and the data in this register has been used, an audio channel interrupt request is set'; AA p.25 '07 AUD0 4 Audio channel 0 block finished')", un);
    ck.record("P8-USED (measured, not graded): the page leaves open whether 'used' means that the output of word L has begun (S_k) or has ended (E_k); the MEASURED lines of P8 print, per block, the events stored before S_k, before E_k and from S_k");
}

} // namespace s5
