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
    for (int x = 0; x < 4; ++x) runs.push_back(p5_run(x));
    PreAgg agg;
    bool off = true, flat = true, order = true, same_z = true, pairs = true;
    for (int x = 0; x < 4; ++x) {
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
        for (int k : {0, 3, 5, 7}) same_z &= within(double(last(k).v), Z0, 1.0);
        order &= H - Z0 >= 1.0 && Z0 - Lo >= 1.0;
        for (int k : {4, 6}) {
            auto& sg = w[size_t(k)].seg;
            bool ok = sg.size() >= 3;
            if (ok) {
                double s1 = sg[1].v, s2 = sg[2].v;
                bool hl = within(s1, H, 1.0) && within(s2, Lo, 1.0), lh = within(s1, Lo, 1.0) && within(s2, H, 1.0);
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

struct VolSweep {
    ARun r;
    std::vector<int> m6, m7;           // markers of the writes v = 0..63, 64 and 0x40|k
    std::vector<int> m64;
    int m_end = 0;
    double A6[65] = {}, A7[64] = {};
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
    s.dly(wait_iters(2 * 227 * 3));
    uint16_t vreg = mut("P6-ADDR") && x == 0 ? 0x0B8 : audvol(x);
    for (int val = 0; val <= 64; ++val) {
        uint16_t w = uint16_t(val == 64 ? 0x0040 : val);
        if (mut("P6") && val == 32) w = 16;
        if (mut("P6-ZERO") && val == 0) w = 1;                    // contrast: volume 1 where the grader expects 0
        v.m6.push_back(s.w16(vreg, w));
        s.dly(mut("P6-SHORT") ? 60 : 230);
    }
    if (p7) {
        for (int k = 0; k < 64; ++k) {
            uint16_t w = mut("P7") ? uint16_t(k) : uint16_t(0x40 | k);
            v.m7.push_back(s.w16(audvol(x), w));
            s.dly(230);
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

inline bool win_level(const VolSweep& v, int m_write, int m_next, double& out, double* dev = nullptr) {
    const Cap& c = v.r.cap;
    uint64_t ta = marker(c, m_write + 1), tn = marker(c, m_next);
    uint64_t line = 227 * v.r.K;
    uint64_t t0 = ta + 2 * line, t1 = ta + 4 * line;
    if (!ta || !tn || t1 > tn) return false;
    out = mean_o(c, t0, t1) - v.r.Z;
    if (dev) { *dev = 0; for (uint64_t t = t0; t < t1; ++t) *dev = std::max(*dev, std::fabs(double(out_at(c, t)) - v.r.Z)); }
    return true;
}

inline void run_p67(Checks& ck) {
    const bool p7 = true;
    const std::string pn = "P6";
    std::vector<VolSweep> sw[2];   // [0]: 0x7F7F, [1]: 0x8080
    for (int x = 0; x < 4; ++x) {
        sw[0].push_back(vol_sweep(x, mut("P6-PRE") ? 0x0101 : 0x7F7F, p7));
        sw[1].push_back(vol_sweep(x, mut("P6-SIGN") ? 0x7F7F : 0x8080, p7));   // contrast: both data words 0x7F7F
    }
    PreAgg agg;
    bool gd = true, gf = true, win = true, scale = true;
    bool lin = true, zero = true, sign = true, bit6 = true;
    for (int x = 0; x < 4; ++x) {
        for (int d = 0; d < 2; ++d) {
            VolSweep& v = sw[d][size_t(x)];
            agg.add(v.r.st);
            uint16_t dm = logv(v.r.cap, v.r.i_dmaconr);
            gd &= (dm & 0x200) && (dm & (1u << x));
            gf &= v.r.f.size() >= 20;
            for (int val = 0; val <= 64; ++val) {
                int mn = val < 64 ? v.m6[size_t(val + 1)] : (p7 ? v.m7[0] : v.m_end);
                double dev = 0;
                win &= win_level(v, v.m6[size_t(val)], mn, v.A6[val], &dev);
                if (val == 0) v.zero_dev = dev;
            }
            if (p7)
                for (int k = 0; k < 64; ++k) {
                    int mn = k < 63 ? v.m7[size_t(k + 1)] : v.m_end;
                    win &= win_level(v, v.m7[size_t(k)], mn, v.A7[k]);
                }
        }
        VolSweep& hi = sw[0][size_t(x)];
        VolSweep& lo = sw[1][size_t(x)];
        scale &= std::max(std::fabs(hi.A6[63]), std::fabs(hi.A6[64])) >= 126.0;
        sign &= hi.A6[64] > 0 && lo.A6[64] < 0;
        zero &= hi.zero_dev <= 1.0 && lo.zero_dev <= 1.0;
        for (int d = 0; d < 2; ++d) {
            VolSweep& v = sw[d][size_t(x)];
            double A64 = v.A6[64];
            for (int val = 0; val <= 63; ++val) lin &= std::fabs(v.A6[val] - val * A64 / 64.0) < 1.0 + val / 64.0;
            if (p7) for (int k = 0; k < 64; ++k) bit6 &= within(v.A7[k], A64, 1.0);
            std::string lv;
            for (int val : {0, 1, 16, 32, 48, 62, 63, 64}) lv += dec(val) + ":" + num(v.A6[val]) + " ";
            if (p7) lv += " | 0x40|k: 0:" + num(v.A7[0]) + " 31:" + num(v.A7[31]) + " 63:" + num(v.A7[63]);
            ck.note(pn + " ch" + dec(x) + (d ? " word 0x8080" : " word 0x7F7F") + " A(v): " + lv);
        }
        note_gc(ck, pn + " ch" + dec(x), hi.r.st);
    }
    for (int d = 0; d < 2; ++d) for (auto& v : sw[d]) { (void)v; }
    for (int d = 0; d < 2; ++d) for (auto& v : sw[d]) agg.add(v.r.st);
    for (const std::string pq : {std::string("P6"), std::string("P7")}) {
        print_std(ck, pq, agg, 8);
        ck.expect(gd, pq + "-G-D: after each start DMACONR bit 9 and the channel's bit read 1 in all eight runs (AA p.22)");
        ck.expect(gf, pq + "-G-F: the channel's fetches occur in all eight runs (AA p.22 '00 AUD0EN')");
        ck.expect(win, pq + "-PRE-WIN: every measuring window starts two lines after its write marker and ends before the next write marker (SPECIFICATION.md P6 step 2)");
        ck.expect(scale, pq + "-PRE-SCALE: for data 0x7F7F the larger of |A(63)| and |A(64)| is at least 126 LSB on every channel, so the sweep spans many times the quantisation bound (AA p.11 AUDxVOL)");
        bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && win && scale;
        const std::string un = "fixture, G-S0, G-C, G-D, G-F, the windows or the resolution";
        if (pq == "P6") {
            graded(ck, pre, lin, "P6-LIN: for both data words, every channel and every v in 0..63 and for 0x0040, |A(v) - v*A(64)/64| < 1 + v/64 LSB (AA p.11 'Bits 6,5,4,3,2,1,0 specify 65 linear volume levels')", un);
            graded(ck, pre, zero, "P6-ZERO: at v = 0 the output equals Z at every sample of the window, for both data words and every channel (AA p.11 '000000=no output')", un);
            graded(ck, pre, sign, "P6-SIGN: A(64) is positive for 0x7F7F and negative for 0x8080 on every channel (AA p.11 AUDxDAT 'twos complement signed integer')", un);
            graded(ck, pre, lin && zero && sign,
                   "P6 sol:AA-audxvol.c1 sol:AA-audxvol.c4 sol:AA-audxvol.c7: AUDxVOL at 0A8 (0B8, 0C8, 0D8) gives 65 linear levels, bits 5-0 one of 64 levels and 000000 no output, for every channel (AA p.11)", un);
        } else {
            graded(ck, pre, bit6, "P7 sol:AA-audxvol.c6: with bit 6 set the level is the maximum level whatever bits 5 to 0 hold: A(0x40|k) equals A(64) within 1 LSB for every k in 0..63, both data words, every channel (AA p.11 '06 Forces volume to max (64 ones, no zeros)')", un);
        }
    }
}

// ---- P8 ---------------------------------------------------------------------

inline void run_p8(Checks& ck) {
    struct R8 { ARun r; int m_cal0 = 0, m_cal1 = 0, m_start8 = 0; int m_clear0 = 0; double L9 = 0; uint64_t R = 0; };
    std::vector<R8> runs(4);
    const unsigned L = 6;
    for (int x = 0; x < 4; ++x) {
        R8& q = runs[size_t(x)];
        ARun& r = q.r;
        RunSpec rs; rs.bin = run_bin();
        Script& s = rs.sc;
        r.pre = prefix(s, true, 119, mut("P8-INTENA") ? std::function<void(Script&)>([x](Script& sc) { sc.w16(INTENA, uint16_t(0xC000 | (1u << (7 + x)))); }) : nullptr);
        std::vector<uint16_t> blk = {0x1010, 0x3030, 0x5050, 0x7070, 0xD0D0, 0x9090};
        if (mut("P8")) blk.insert(blk.end(), {0xB0B0, 0xF0F0, 0x4040});
        if (mut("P8-L9")) blk[0] = 0x9090;                 // contrast: the calibration word occurs twice per block
        poke_words(rs.pokes, kBuf[x], blk);
        poke_words(rs.pokes, kBuf[x] + 0x100, {0x9090});
        // calibration
        s.w32(audlc(x), kBuf[x] + 0x100); s.w16(audlen(x), 1); s.w16(audper(x), 400); s.w16(audvol(x), 64);
        q.m_cal0 = s.w16(DMACON, start_val(1u << x));
        r.i_dmaconr = r.pre.logn++; s.rd(DMACONR);
        s.dly(wait_iters(8 * 227));
        q.m_cal1 = s.w16(DMACON, uint16_t(1u << x));
        s.dly(100);
        // the block run
        s.w32(audlc(x), kBuf[x]); s.w16(audlen(x), uint16_t(blk.size() + (mut("P8-LEN") ? 1 : 0))); s.w16(audper(x), 400); s.w16(audvol(x), 64);
        q.m_clear0 = s.w16(INTREQ, uint16_t(1u << (7 + x)));
        r.m_start = q.m_start8 = s.w16(DMACON, start_val(1u << x));
        if (mut("P8-R")) {   // contrast: a poll loop with a delay in it, so that R exceeds the bound
            for (int i = 0; i < 220; ++i) { s.rd(INTREQR); s.w16(INTREQ, uint16_t(1u << (7 + x))); s.dly(40); }
        } else
        s.pact(INTREQR, uint16_t(1u << (7 + x)), 2600, INTREQ, mut("P8-NOCLEAR") ? uint16_t(0) : uint16_t(1u << (7 + x)));
        r.m_stop = s.w16(DMACON, uint16_t(1u << x));
        s.dly(100);
        s.end();
        rs.cycles = 4'000'000 + 8ull * 90000;
        r.cap = run(rs);
        // fetches of the block run only
        r.fr = analyse_refresh(r.cap);
        r.K = r.fr.K ? r.fr.K : 8; r.tau = 1 + r.K;
        r.Z = zero_ref(r.cap, r.pre, r.K);
        uint64_t t0 = marker(r.cap, q.m_start8), t1 = marker(r.cap, r.m_stop + 1);
        r.f = fetches(r.cap, kBuf[x] - 0x40, kBuf[x] + 0x1000, t0, t1);
        r.runs = runs_of(r.cap, t0, t1);
        // calibration fetches
        std::vector<Fetch> cf = fetches(r.cap, kBuf[x] + 0x100 - 0x40, kBuf[x] + 0x100 + 0x40, marker(r.cap, q.m_cal0), marker(r.cap, q.m_cal1 + 1));
        r.st = std_pre(r.cap, r.pre, r.f);
        // L9: the level over the last four lines of the calibration play
        uint64_t tc1 = marker(r.cap, q.m_cal1), line = 227 * r.K;
        q.L9 = mean_o(r.cap, tc1 - 4 * line, tc1) - r.Z;
        (void)cf;
        // R: the largest interval between consecutive log stores of the poll loop
        size_t i0 = size_t(r.pre.logn);
        uint64_t Rm = 0;
        for (size_t i = i0 + 1; i < r.cap.log.size(); ++i)
            if (r.cap.log[i].first > t0 && r.cap.log[i - 1].first > t0) Rm = std::max(Rm, r.cap.log[i].first - r.cap.log[i - 1].first);
        q.R = Rm;
    }
    PreAgg agg;
    bool gd = true, gf = true, gr = true, once = true, blocks = true, clearok = true, wins = true, events_ok = true;
    for (int x = 0; x < 4; ++x) {
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
            const unsigned Lw = mut("P8") ? 9 : 6;
            if (ret.size() >= 2) {
                Blocks b = block_check(r.f, kBuf[x], Lw, ret[1]);
                blocks &= b.distinct == Lw && b.wrap_bad == 0 && b.outside == 0 && b.step_bad == 0;
            } else blocks = false;
        }
        // events: reads of 1 in bit 7+x whose previous read was 0, in the block run
        uint16_t mask = uint16_t(1u << (7 + x));
        uint64_t tstart = marker(r.cap, q.m_start8);
        std::vector<uint64_t> ev;
        {
            int prev = -1;
            for (size_t i = size_t(r.pre.logn); i < r.cap.log.size(); ++i) {
                if (r.cap.log[i].first <= tstart) continue;
                int bit = (r.cap.log[i].second & mask) ? 1 : 0;
                if (bit == 1 && prev == 0) ev.push_back(r.cap.log[i].first);
                if (bit == 1 && prev == 1) clearok = false;
                prev = bit;
            }
        }
        // windows W_k, k = 2..6, with word L = 6 (offset 10) as the grader keeps it
        std::string wn;
        for (size_t k = 2; k <= 6; ++k) {
            if (ret.size() < k + 1) { wins = false; events_ok = false; continue; }
            size_t blk0 = ret[k - 1], blk1 = ret[k];          // fetch indexes of the k-th and (k+1)-th fetch at BUFx
            uint64_t Fk = 0, Fnext2 = 0;
            for (size_t i = blk0; i < blk1; ++i) if (r.f[i].addr == kBuf[x] + 10) Fk = r.f[i].t;
            if (blk1 + 1 < r.f.size()) Fnext2 = r.f[blk1 + 1].t;
            if (!Fk || !Fnext2) { wins = false; events_ok = false; continue; }
            int nl9 = 0; size_t wr = 0;
            for (size_t i = 0; i < r.runs.size(); ++i)
                if (r.runs[i].t0 > Fk && r.runs[i].t0 < Fnext2 && within(double(r.runs[i].v) - r.Z, q.L9, 1.0)) { if (!nl9) wr = i; ++nl9; }
            if (nl9 != 1) { once = false; events_ok = false; continue; }
            if (wr + 1 >= r.runs.size()) { wins = false; events_ok = false; continue; }
            uint64_t Ek = r.runs[wr + 1].t0;
            uint64_t t_lo = Fk, t_hi = Ek + r.tau + 2 * q.R;
            int n = 0;
            for (uint64_t e : ev) if (e >= t_lo && e <= t_hi) ++n;
            if (n < 1) events_ok = false;
            wn += "W" + dec((long long)k) + ":" + dec(n) + " ";
        }
        ck.note("P8 ch" + dec(x) + ": events=" + dec((long long)ev.size()) + " windows (events inside) " + wn + " R=" + dec((long long)q.R) + " cycles, L9=" + num(q.L9));
    }
    print_std(ck, "P8", agg, 4);
    ck.expect(gd, "P8-G-D: after each start DMACONR bit 9 and the channel's bit read 1 on all four channels (AA p.22)");
    ck.expect(gf, "P8-G-F: each channel makes at least eight fetches at the location, so blocks 1 to 7 start (AA p.22 '00 AUD0EN')");
    ck.expect(gr, "P8-G-R: R, the largest interval between consecutive log stores of the poll loop, is at most P*c/8 = 50 colour clocks on every channel (SPECIFICATION.md 2.3)");
    ck.expect(blocks, "P8-PRE-BLOCKS: from pass 2 on the fetches show blocks of as many words as the program wrote to AUDxLEN (the P1 check)");
    ck.expect(once, "P8-PRE-L9: in each block k = 2..6 exactly one run at level L9 starts between F_k and the fetch of word 2 of block k+1, on every channel");
    ck.expect(clearok, "P8-PRE-CLEAR: no two consecutive polls read 1: each clear took effect before the next poll (AA p.25 'These bits may be polled by the processor')");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && gr && blocks && once && clearok && wins;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F, G-R, the blocks, the L9 runs or the clear";
    graded(ck, pre, events_ok, "P8 sol:AA-audxdat.c11: for each channel and each block k = 2..6 the channel's interrupt request bit (INTREQ bit 7+x) is set, so that it reads 1, in the window from the fetch of word L to the start of the run after word L's run plus tau and 2R (AA p.11 to 12 'When the DMA data is finished (words outputted=Length) and the data in this register has been used, an audio channel interrupt request is set'; AA p.25 '07 AUD0 4 Audio channel 0 block finished')", un);
}

} // namespace s5
