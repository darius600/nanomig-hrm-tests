// s5_p915.h -- P9 to P11 (ADKCON bits 0-2, volume attach), P12 to P14 (bits 4-6,
// period attach) and P15 (bit 3), AA p.11.
#pragma once

#include "s5_p58.h"

namespace s5 {

// ---- volume attach ------------------------------------------------------------

struct VolAtt {
    ARun r;
    int m_c[4] = {};                  // markers of the AUDnVOL writes 64, 16, 40, 32 of phase 1
    int m_adk2 = 0, m_adk3 = 0;       // markers of the ADKCON writes of phases 2 and 3
    int i_adk2 = -1, i_adk3 = -1, i_dm_n = -1, i_dm_m = -1;
    int m_p2 = 0;                     // marker of the start of channel m (phase 2)
    int m_p2end = 0;
    double L[4] = {};                 // L64, L16, L40, L32: the level relative to Z over whole repetitions (for the notes)
    Lvl Lc[4];                        // the same levels as exact rationals
    PatWin Pc[4];                     // the calibration windows
    bool cal_per = true;              // every calibration window holds a whole number of repetitions
    uint64_t lam = 1;                 // the least common multiple of the four periods: the length of a measuring window in phase 2
    std::vector<Fetch> fm;            // channel m's fetches in phase 2
};

// The modulator's data set: the volumes 64, 16, 40, the last one twice, so that a first pass that is a word short or two words long
// outputs the same segments (64, 16, 40) as a whole pass.
constexpr uint16_t kVolWords[4] = {0x0040, 0x0010, 0x0028, 0x0028};

inline VolAtt vol_att(int m, int n, const std::string& pid) {
    const bool liver = mut((pid + "-LIVEREV").c_str());
    const bool live = mut((pid + "-LIVE").c_str()) || liver;   // the CPU writes the volumes the modulator words hold, on top of the attach
    VolAtt v;
    ARun& r = v.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[n], {mut((pid + "-PAT").c_str()) ? uint16_t(0x7F00) : uint16_t(0x7F7F)});   // -PAT: two different bytes, so the output is not constant within a window
    std::vector<uint16_t> mod = {kVolWords[0], kVolWords[1], kVolWords[2], kVolWords[3]};
    mod.push_back(mod.back()); mod.push_back(mod.back());   // two words past the set repeat its last word
    poke_words(rs.pokes, kBuf[m], mod);
    s.w32(audlc(n), kBuf[n]); s.w16(audlen(n), 1); s.w16(audper(n), 200); s.w16(audvol(n), 32);
    s.w32(audlc(m), kBuf[m]); s.w16(audlen(m), 4); s.w16(audper(m), 1500); s.w16(audvol(m), 0);
    // phase 1: channel n alone
    r.m_start = s.w16(DMACON, start_val(1u << n));
    v.i_dm_n = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(2 * 227));
    const uint16_t cv[4] = {64, 16, 40, mut((pid + "-CAL").c_str()) ? uint16_t(40) : uint16_t(32)};   // contrast: L32 written as 40
    for (int i = 0; i < 4; ++i) { v.m_c[i] = s.w16(audvol(n), cv[i]); s.dly(wait_iters(3 * 227)); }
    // phase 2
    {
        uint16_t val = mut((pid + "-CLR").c_str()) ? uint16_t(1u << m) : uint16_t(0x8000 | (1u << m));
        v.m_adk2 = s.w16(ADKCON, val);
    }
    v.i_adk2 = r.pre.logn++; s.rd(ADKCONR);
    uint16_t sv = mut((pid + "-NOFETCH").c_str()) ? uint16_t(0x8200) : uint16_t(0x8200 | (1u << m));
    v.m_p2 = s.w16(DMACON, sv);
    v.i_dm_m = r.pre.logn++; s.rd(DMACONR);
    if (live) {
        for (int cyc = 0; cyc < 8; ++cyc)
            for (int i = 0; i < 3; ++i) { s.w16(audvol(n), cv[liver ? (3 - i) % 3 : i]); s.dly(wait_iters(1500)); }
    } else s.dly(wait_iters(36000));
    v.m_p2end = s.mark();
    v.m_adk3 = s.w16(ADKCON, uint16_t(1u << m));
    v.i_adk3 = r.pre.logn++; s.rd(ADKCONR);
    r.m_stop = s.w16(DMACON, uint16_t((1u << m) | (1u << n)));
    s.dly(100);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 50000;
    r.cap = run(rs);
    r.fr = analyse_refresh(r.cap);
    r.K = r.fr.K ? r.fr.K : 8;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    uint64_t t2 = marker(r.cap, v.m_p2), t3 = marker(r.cap, v.m_p2end);
    v.fm = fetches(r.cap, kBuf[m] - 0x40, kBuf[m] + 0x1000, t2, t3);
    r.f = fetches(r.cap, kBuf[n] - 0x40, kBuf[n] + 0x1000, marker(r.cap, r.m_start), marker(r.cap, r.m_stop + 1));
    std::vector<Fetch> all = r.f; all.insert(all.end(), v.fm.begin(), v.fm.end());
    r.st = std_pre(r.cap, r.pre, all);
    uint64_t line = 227 * r.K;
    for (int i = 0; i < 4; ++i) {
        uint64_t ta = marker(r.cap, v.m_c[i] + 1);
        v.Pc[i] = pat_window(r.cap, ta + line, ta + 3 * line);
        v.cal_per &= v.Pc[i].ok;
        v.Lc[i] = lvl_of(v.Pc[i], r.Z);
        v.L[i] = double(v.Lc[i].num / v.Lc[i].den);
        if (v.Pc[i].ok) v.lam = std::lcm<uint64_t>(v.lam, v.Pc[i].pi);
    }
    v.cal_per &= v.lam <= 2 * line;
    return v;
}

// The classes of phase 2: 0 = L64, 1 = L16, 2 = L40, 3 = L32, 4 = other. Phase 2 is read through a window of lam output samples
// (a whole number of repetitions of every calibrated volume), slid one sample at a time: a window inside one volume has that
// volume's level exactly; only windows that straddle a change can have another level, and there are fewer than lam of them
// per change. A stable stretch is a run of at least lam windows of one class.
struct VolObs {
    std::vector<int> cls;                   // the class of each stable stretch, in order (class 4 is never a stable stretch)
    size_t n32 = 0, nother = 0, cnt[3] = {0, 0, 0}, nseg = 0;
    bool cycle = true;
    size_t longest_other = 0;               // the longest run of windows that match no calibrated level
};

inline VolObs vol_obs(const VolAtt& v) {
    VolObs o;
    const Cap& c = v.r.cap;
    uint64_t t2 = marker(c, v.m_p2), t3 = marker(c, v.m_p2end);
    const uint64_t lam = v.lam;
    if (!v.cal_per || t3 <= t2 + lam) return o;
    const uint64_t n = t3 - t2;
    std::vector<long long> S(n + 1, 0);
    for (uint64_t i = 0; i < n; ++i) S[i + 1] = S[i] + out_at(c, t2 + i);
    std::vector<int> k(n - lam + 1, 4);
    for (uint64_t i = 0; i + lam <= n; ++i) {
        Lvl w{(long double)(S[i + lam] - S[i]) - (long double)v.r.Z * (long double)lam, (long double)lam};
        for (int j = 0; j < 4; ++j) if (lvl_eq(w, v.Lc[j])) { k[i] = j; break; }
    }
    // stretches
    std::vector<std::pair<int, size_t>> st;   // (class, length)
    for (size_t i = 0; i < k.size();) {
        size_t j = i;
        while (j < k.size() && k[j] == k[i]) ++j;
        st.push_back({k[i], j - i});
        i = j;
    }
    bool started = false;
    int last = -1;
    for (auto& s : st) {
        if (s.first == 4) { if (started) o.longest_other = std::max(o.longest_other, s.second); continue; }
        if (s.second < lam) continue;                 // a window level that is the level of a calibrated volume by coincidence
        if (!started) { if (s.first <= 2) started = true; else continue; }
        o.cls.push_back(s.first);
        if (last >= 0 && last <= 2 && s.first <= 2 && s.first != (last + 1) % 3) o.cycle = false;
        last = s.first;
    }
    o.nseg = o.cls.size();
    for (int q : o.cls) { if (q == 3) ++o.n32; else ++o.cnt[q]; }
    o.nother = o.longest_other >= lam ? 1 : 0;       // a stretch of windows with no calibrated level that is not a change
    return o;
}

inline void run_vol_att(Checks& ck, const std::string& pid, const char* claim, int m, int n, const char* bitname) {
    VolAtt v = vol_att(m, n, pid);
    PreAgg agg; agg.add(v.r.st);
    note_gc(ck, pid, v.r.st);
    uint16_t dmn = logv(v.r.cap, v.i_dm_n), dmm = logv(v.r.cap, v.i_dm_m);
    bool gd = (dmn & 0x200) && (dmn & (1u << n)) && (dmm & 0x200) && (dmm & (1u << m));
    bool gf = v.fm.size() >= 12 && v.r.f.size() >= 12;
    bool dist = v.cal_per;
    for (int i = 0; i < 4 && v.cal_per; ++i) for (int j = i + 1; j < 4; ++j) dist &= !lvl_eq(v.Lc[i], v.Lc[j]);
    uint16_t a2 = logv(v.r.cap, v.i_adk2), a3 = logv(v.r.cap, v.i_adk3);
    bool adk = (a2 & 0x00FF) == (1u << m) && !(a3 & (1u << m));
    VolObs o = vol_obs(v);
    std::string p2;   // phase 2 as it is, from the start of channel m: every segment's level (relative to Z) and length in cycles
    {
        auto segs = runs_of(v.r.cap, marker(v.r.cap, v.m_p2), marker(v.r.cap, v.m_p2end));
        for (size_t i = 0; i < segs.size() && i < 8; ++i) p2 += num(double(segs[i].v) - v.r.Z) + "(" + dec((long long)segs[i].len()) + ") ";
        p2 = dec((long long)segs.size()) + " segments in all, the first " + dec((long long)std::min<size_t>(segs.size(), 8)) + " (level(length in cycles)): " + p2;
    }
    ck.note(pid + " phase 2 from the start of channel " + dec(m) + ": " + p2);
    ck.note(pid + ": calibration L64=" + num(v.L[0]) + " L16=" + num(v.L[1]) + " L40=" + num(v.L[2]) + " L32=" + num(v.L[3]) + "; phase 2: segments=" + dec((long long)o.nseg) +
            " (L64 " + dec((long long)o.cnt[0]) + ", L16 " + dec((long long)o.cnt[1]) + ", L40 " + dec((long long)o.cnt[2]) + ", L32 " + dec((long long)o.n32) + ", longest run of other " + dec((long long)o.longest_other) +
            "), window " + dec((long long)v.lam) + " cycles, modulator fetches=" + dec((long long)v.fm.size()) + ", ADKCONR phase 2/3 = " + hex4(a2) + "/" + hex4(a3));
    print_std(ck, pid, agg, 1);
    ck.expect(gd, pid + "-G-D: after each start DMACONR bit 9 and the started channel's bit read 1, for channel n and for channel m (AA p.22)");
    ck.expect(gf, pid + "-G-F: channel m makes at least 12 fetches in phase 2 and channel n at least 12 over the run (AA p.22 '00 AUD0EN')");
    ck.expect(v.cal_per, pid + "-PRE-PERIOD: in each of the four calibration windows (volumes 64, 16, 40 and 32 of constant data) the output repeats with a period of at most half the window, and the least common multiple of the four periods is at most the window, so a level over a whole number of repetitions can be formed for each volume (AA p.11 '(111111=63 0nes, one zero)')");
    ck.expect(dist, pid + "-PRE-CAL: the calibration levels L64, L16, L40 and L32, each the mean over whole repetitions, are pairwise different (AA p.11 AUDxVOL)");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && dist && v.cal_per;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F, the calibration or the repetition period";
    auto levels_ok = [](const VolObs& q) { return q.nseg > 0 && q.n32 == 0 && q.nother == 0 && q.cycle && q.cnt[0] >= 3 && q.cnt[1] >= 3 && q.cnt[2] >= 3; };
    graded(ck, pre, adk, pid + "-ADK: in phase 2 ADKCONR bit " + dec(m) + " reads 1 and the other bits 7 to 0 read 0, and in phase 3 bit " + dec(m) + " reads 0 (AA p.10 'SET/CLR Determines if bits written with a 1 get set or cleared')", un);
    graded(ck, pre, levels_ok(o), pid + "-LEVELS: in phase 2 every stable stretch of channel " + dec(n) + "'s output has the level L64, L16 or L40, none has L32, no stretch of windows has any other level, the stretches cycle 64 -> 16 -> 40 and each value occurs at least three times (AA p.11 '" + bitname + "')", un);
    graded(ck, pre, adk && levels_ok(o), pid + " " + claim + ": with ADKCON bit " + dec(m) + " set, which ADKCONR reads back as 1, the data words of channel " + dec(m) + "'s DMA set the volume of channel " + dec(n) + ", which takes the volumes of the words cyclically in place of the volume the CPU wrote (AA p.11 '" + bitname + "')", un);
}

// ---- period attach ------------------------------------------------------------

struct PerAtt {
    ARun r;
    int m_p1 = 0, m_p2 = 0, m_p2end = 0, m_adk2 = 0, m_adk3 = 0;
    int i_adk2 = -1, i_adk3 = -1, i_dm_n = -1, i_dm_m = -1;
    std::vector<Fetch> fm;
};

// The modulator's data set: the periods 200, 600, 350, the last one twice (as kVolWords).
constexpr uint16_t kPerWords[4] = {0x00C8, 0x0258, 0x015E, 0x015E};

inline PerAtt per_att(int m, int n, const std::string& pid) {
    const bool liver = mut((pid + "-LIVEREV").c_str());
    const bool live = mut((pid + "-LIVE").c_str()) || liver;   // the CPU writes the periods the modulator words hold, on top of the attach
    PerAtt v;
    ARun& r = v.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[n], {0x7F80});
    std::vector<uint16_t> mod = {kPerWords[0], kPerWords[1], kPerWords[2], kPerWords[3]};
    mod.push_back(mod.back()); mod.push_back(mod.back());   // two words past the set repeat its last word
    poke_words(rs.pokes, kBuf[m], mod);
    s.w32(audlc(n), kBuf[n]); s.w16(audlen(n), 1); s.w16(audper(n), mut((pid + "-PER").c_str()) ? 800 : 400); s.w16(audvol(n), 64);
    s.w32(audlc(m), kBuf[m]); s.w16(audlen(m), 4); s.w16(audper(m), 3000); s.w16(audvol(m), 0);
    v.m_p1 = r.m_start = s.w16(DMACON, start_val(1u << n));
    v.i_dm_n = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(10 * 227));
    {
        uint16_t val = mut((pid + "-CLR").c_str()) ? uint16_t(1u << (4 + m)) : uint16_t(0x8000 | (1u << (4 + m)));
        v.m_adk2 = s.w16(ADKCON, val);
    }
    v.i_adk2 = r.pre.logn++; s.rd(ADKCONR);
    uint16_t sv = mut((pid + "-NOFETCH").c_str()) ? uint16_t(0x8200) : uint16_t(0x8200 | (1u << m));
    v.m_p2 = s.w16(DMACON, sv);
    v.i_dm_m = r.pre.logn++; s.rd(DMACONR);
    if (live) {
        const uint16_t pv[3] = {200, 600, 350};
        for (int cyc = 0; cyc < 8; ++cyc)
            for (int i = 0; i < 3; ++i) { s.w16(audper(n), pv[liver ? (3 - i) % 3 : i]); s.dly(wait_iters(3000)); }
    } else s.dly(wait_iters(72000));
    v.m_p2end = s.mark();
    v.m_adk3 = s.w16(ADKCON, uint16_t(1u << (4 + m)));
    v.i_adk3 = r.pre.logn++; s.rd(ADKCONR);
    r.m_stop = s.w16(DMACON, uint16_t((1u << m) | (1u << n)));
    s.dly(100);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 90000;
    r.cap = run(rs);
    r.fr = analyse_refresh(r.cap);
    r.K = r.fr.K ? r.fr.K : 8;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    v.fm = fetches(r.cap, kBuf[m] - 0x40, kBuf[m] + 0x1000, marker(r.cap, v.m_p2), marker(r.cap, v.m_p2end));
    r.f = fetches(r.cap, kBuf[n] - 0x40, kBuf[n] + 0x1000, marker(r.cap, r.m_start), marker(r.cap, r.m_stop + 1));
    std::vector<Fetch> all = r.f; all.insert(all.end(), v.fm.begin(), v.fm.end());
    r.st = std_pre(r.cap, r.pre, all);
    return v;
}

struct PerObs {
    size_t nruns = 0, nunclassified = 0, n400 = 0;
    size_t cnt[3] = {0, 0, 0};
    bool cycle = true;
    std::string seq;
};

// Phase 2 of channel n, from the first run whose length is one of the three periods: every run has to last exactly one of
// 200, 600 or 350 colour clocks. The last run is cut by the phase-3 marker and is left out.
inline PerObs per_obs(const PerAtt& v) {
    PerObs o;
    const Cap& c = v.r.cap;
    const uint64_t K = v.r.K;
    uint64_t t2 = marker(c, v.m_p2), t3 = marker(c, v.m_p2end);
    auto runs = runs_of(c, t2, t3);
    const uint64_t want[3] = {200 * K, 600 * K, 350 * K};
    auto cls = [&](const LRun& u) {
        for (int i = 0; i < 3; ++i) if (u.len() == want[i]) return i;
        return 3;
    };
    std::vector<int> k;
    size_t first = runs.size();
    for (size_t i = 0; i < runs.size(); ++i) if (cls(runs[i]) < 3) { first = i; break; }
    for (size_t i = first; i < runs.size(); ++i) k.push_back(cls(runs[i]));
    if (!k.empty()) k.pop_back();
    o.nruns = k.size();
    for (size_t i = 0; i < k.size(); ++i) {
        if (k[i] == 3) {
            ++o.nunclassified;
            const LRun& u = runs[first + i];
            if (u.len() == 400 * K) ++o.n400;
        }
    }
    // segments: maximal sequences of runs of one class
    std::vector<int> segs;
    int last = -1;
    for (size_t i = 0; i < k.size(); ++i) {
        if (k[i] == 3) continue;
        if (k[i] != last) { segs.push_back(k[i]); last = k[i]; ++o.cnt[k[i]]; }
    }
    for (size_t i = 1; i < segs.size(); ++i) if (segs[i] != (segs[i - 1] + 1) % 3) o.cycle = false;
    for (int s : segs) o.seq += char('0' + s);
    return o;
}

inline void run_per_att(Checks& ck, const std::string& pid, const char* claim, int m, int n, const char* bitname) {
    PerAtt v = per_att(m, n, pid);
    PreAgg agg; agg.add(v.r.st);
    note_gc(ck, pid, v.r.st);
    uint16_t dmn = logv(v.r.cap, v.i_dm_n), dmm = logv(v.r.cap, v.i_dm_m);
    bool gd = (dmn & 0x200) && (dmn & (1u << n)) && (dmm & 0x200) && (dmm & (1u << m));
    bool gf = v.fm.size() >= 12 && v.r.f.size() >= 12;
    // G-P in phase 1: runs of 400 colour clocks
    bool gp = true;
    {
        auto runs = runs_of(v.r.cap, marker(v.r.cap, v.m_p1 + 1), marker(v.r.cap, v.m_p2));
        size_t cnt = 0;
        for (size_t i = 2; i + 2 < runs.size(); ++i) { ++cnt; if (runs[i].len() != 400 * v.r.K) gp = false; }
        gp &= cnt >= 3;
        if (std::getenv("NMH_DEBUG")) {
            std::string d;
            for (size_t i = 0; i < runs.size() && i < 12; ++i) d += dec((long long)runs[i].len()) + "/" + dec(runs[i].v) + " ";
            ck.note(pid + " debug phase 1 runs (length/level): " + d + " n=" + dec((long long)runs.size()));
        }
    }
    uint16_t a2 = logv(v.r.cap, v.i_adk2), a3 = logv(v.r.cap, v.i_adk3);
    bool adk = (a2 & 0x00FF) == (1u << (4 + m)) && !(a3 & (1u << (4 + m)));
    PerObs o = per_obs(v);
    std::string hist;   // every run of phase 2, from the start of channel m: colour clocks (rounded) and how many
    {
        std::map<long long, size_t> h;
        for (auto& u : runs_of(v.r.cap, marker(v.r.cap, v.m_p2), marker(v.r.cap, v.m_p2end))) ++h[(long long)((u.len() + v.r.K / 2) / v.r.K)];
        for (auto& e : h) hist += dec(e.first) + ":" + dec((long long)e.second) + " ";
    }
    ck.note(pid + " phase 2 from the start of channel " + dec(m) + ": run lengths in colour clocks (length:count) " + hist);
    auto ok = [](const PerObs& q) {
        return q.nruns > 0 && q.nunclassified == 0 && q.cycle && q.cnt[0] >= 3 && q.cnt[1] >= 3 && q.cnt[2] >= 3;
    };
    ck.note(pid + ": phase 2 runs=" + dec((long long)o.nruns) + " not 200, 600 or 350 colour clocks=" + dec((long long)o.nunclassified) + " (of them 400 colour clocks " + dec((long long)o.n400) +
            ") segments 200/600/350 = " + dec((long long)o.cnt[0]) + "/" + dec((long long)o.cnt[1]) + "/" + dec((long long)o.cnt[2]) + ", ADKCONR phase 2/3 = " + hex4(a2) + "/" + hex4(a3) + ", modulator fetches=" + dec((long long)v.fm.size()));
    print_std(ck, pid, agg, 1);
    ck.expect(gd, pid + "-G-D: after each start DMACONR bit 9 and the started channel's bit read 1, for channel n and for channel m (AA p.22)");
    ck.expect(gf, pid + "-G-F: channel m makes at least 12 fetches in phase 2 and channel n at least 12 over the run (AA p.22 '00 AUD0EN')");
    ck.expect(gp, pid + "-G-P: in phase 1 all runs of channel " + dec(n) + " last exactly 400 colour clocks, each sample being one run (AA p.11 AUDxPER)");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && gp;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F or G-P";
    graded(ck, pre, adk, pid + "-ADK: ADKCONR bit " + dec(4 + m) + " reads 1 in phase 2 and 0 in phase 3, and the other bits 7 to 0 read 0 in phase 2 (AA p.10 'SET/CLR')", un);
    graded(ck, pre, ok(o), pid + "-RUNS: in phase 2 every run of channel " + dec(n) + " lasts exactly 200, 600 or 350 colour clocks, the segments cycle 200 -> 600 -> 350 and each value occurs at least three times (AA p.11 '" + bitname + "')", un);
    graded(ck, pre, adk && ok(o), pid + " " + claim + ": with ADKCON bit " + dec(4 + m) + " set, which ADKCONR reads back as 1, the data words of channel " + dec(m) + "'s DMA set the period of channel " + dec(n) + ", whose samples last the periods of the words cyclically in place of the period the CPU wrote (AA p.11 '" + bitname + "')", un);
}


// ---- P15 -------------------------------------------------------------------------

struct Bit3 { ARun r; int m_start = 0, m_stop = 0, i_adk = -1, i_dm = -1; std::vector<Fetch> f3, fk; };

inline Bit3 bit3_run(int k, bool with_bit3, const std::string& variant) {
    Bit3 b;
    ARun& r = b.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[k], {0x7F80});
    poke_words(rs.pokes, kBuf[3], {0x0010, 0x0020});
    s.w32(audlc(k), kBuf[k]); s.w16(audlen(k), 1); s.w16(audper(k), (!with_bit3 && mut("P15-PERB")) ? 600 : 300); s.w16(audvol(k), 64);   // P15-PERB: run B plays at the period 600
    s.w32(audlc(3), kBuf[3]); s.w16(audlen(3), 2); s.w16(audper(3), 700); s.w16(audvol(3), 0);
    uint16_t adk = 0x8008;
    std::string var = variant;
    // (no attach bit modulates channel 0: for k = 0 the page's contrast is the CPU volume write, selected by P15-VOL)
    if (var == "ATT" && k == 0) var = "";
    if (var == "ATT") {   // the page's contrast: the volume attach bit of the channel before k as well
        int mm = k - 1;
        if (mm >= 0) {
            poke_words(rs.pokes, kBuf[mm], {0x0010, 0x0020});
            s.w32(audlc(mm), kBuf[mm]); s.w16(audlen(mm), 2); s.w16(audper(mm), 700); s.w16(audvol(mm), 0);
            adk = uint16_t(0x8008 | (1u << mm));
        }
    }
    if (with_bit3 && !mut("P15-NOADK")) s.w16(ADKCON, adk);
    b.i_adk = r.pre.logn++; s.rd(ADKCONR);
    unsigned mask = 0x8u | (1u << k);
    if (var == "ATT" && k >= 1) mask |= 1u << (k - 1);
    b.m_start = s.w16(DMACON, start_val(mask));
    b.i_dm = r.pre.logn++; s.rd(DMACONR);
    if (var == "VOL") { s.dly(wait_iters(20 * 227)); s.w16(audvol(k), 16); s.dly(wait_iters(40 * 227)); }
    else s.dly(wait_iters(60 * 227));
    b.m_stop = s.w16(DMACON, uint16_t(mask));
    s.dly(100);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 20000;
    r.cap = run(rs);
    r.fr = analyse_refresh(r.cap);
    r.K = r.fr.K ? r.fr.K : 8;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    uint64_t t0 = marker(r.cap, b.m_start), t1 = marker(r.cap, b.m_stop + 1);
    b.f3 = fetches(r.cap, kBuf[3] - 0x40, kBuf[3] + 0x1000, t0, t1);
    b.fk = fetches(r.cap, kBuf[k] - 0x40, kBuf[k] + 0x1000, t0, t1);
    std::vector<Fetch> all = b.f3; all.insert(all.end(), b.fk.begin(), b.fk.end());
    r.st = std_pre(r.cap, r.pre, all);
    return b;
}

// The runs of channel k from its first fetch to the stop, without the run that holds the first fetch and the run that the stop
// cuts: the interval is anchored on the channel's own first fetch, so run A and run B are compared over the same play.
inline std::vector<LRun> bit3_core(const Bit3& q) {
    std::vector<LRun> out;
    if (q.fk.empty()) return out;
    uint64_t t0 = q.fk.front().t, t1 = marker(q.r.cap, q.m_stop);
    auto all = runs_of(q.r.cap, t0, t1);
    if (all.size() > 2) out.assign(all.begin() + 1, all.end() - 1);
    return out;
}

inline void run_p15(Checks& ck) {
    PreAgg agg;
    bool gd = true, gf = true, adk = true, gp = true, same_ok = true;
    const bool volmut = mut("P15-VOL"), attm = mut("P15-ATT");
    for (int k = 0; k < 3; ++k) {
        Bit3 a = bit3_run(k, true, volmut ? "VOL" : (attm ? "ATT" : ""));
        Bit3 b = bit3_run(k, false, "");
        agg.add(a.r.st); agg.add(b.r.st);
        note_gc(ck, "P15 k=" + dec(k), a.r.st);
        uint16_t dm = logv(a.r.cap, a.i_dm);
        gd &= (dm & 0x200) && (dm & 0x8) && (dm & (1u << k));
        gf &= a.f3.size() >= 6 && a.fk.size() >= 6;
        uint16_t ra = logv(a.r.cap, a.i_adk), rb = logv(b.r.cap, b.i_adk);
        adk &= (ra & 0x8) != 0 && (rb & 0x8) == 0;
        auto ca = bit3_core(a), cb = bit3_core(b);
        // run B: the unmodulated play of channel k is a string of runs of exactly 300 colour clocks, two levels in turn
        std::set<int32_t> lb;
        for (auto& u : cb) lb.insert(u.v);
        bool bp = cb.size() > 10 && lb.size() == 2;
        for (auto& u : cb) bp &= u.len() == 300 * b.r.K;
        gp &= bp;
        // run A against run B, run by run over what both have: level and length
        size_t n = std::min(ca.size(), cb.size()), bad = 0, first_bad = n;
        for (size_t i = 0; i < n; ++i) if (ca[i].v != cb[i].v || ca[i].len() != cb[i].len()) { if (!bad) first_bad = i; ++bad; }
        same_ok &= n > 10 && bad == 0;
        ck.note("P15 k=" + dec(k) + ": run A runs=" + dec((long long)ca.size()) + ", run B runs=" + dec((long long)cb.size()) + ", compared " + dec((long long)n) + ", differing " + dec((long long)bad) +
                " (first at " + dec((long long)first_bad) + "), run B levels=" + dec((long long)lb.size()) +
                " ADKCONR A/B=" + hex4(ra) + "/" + hex4(rb) + " fetches ch3/chk=" + dec((long long)a.f3.size()) + "/" + dec((long long)a.fk.size()));
    }
    print_std(ck, "P15", agg, 6);
    ck.expect(gd, "P15-G-D: after the start in run A DMACONR bit 9, bit 3 and bit k read 1 for k = 0, 1, 2 (AA p.22)");
    ck.expect(gf, "P15-G-F: in run A channel 3 and channel k make fetches (at least six each) for k = 0, 1, 2 (AA p.22 '00 AUD0EN')");
    ck.expect(gp, "P15-G-P: in run B every run of channel k from its first fetch on lasts exactly 300 colour clocks, with two levels in turn, for k = 0, 1, 2 (AA p.11 AUDxPER)");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && gp;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F or G-P";
    graded(ck, pre, adk, "P15-ADK: ADKCONR bit 3 reads 1 in run A and 0 in run B for k = 0, 1, 2 (AA p.10 'SET/CLR')", un);
    graded(ck, pre, same_ok, "P15-RUNS: from channel k's first fetch to the stop, run A and run B play the same runs one by one, level and length, for k = 0, 1, 2 (AA p.11 '03 USE3VN Use audio channel 3 to modulate nothing')", un);
    graded(ck, pre, adk && same_ok, "P15 sol:AA-adkcon.c27: with ADKCON bit 3 set and channel 3's DMA running no channel's volume or period changes: channels 0, 1 and 2 play, from their first fetch to the stop, exactly as with bit 3 clear (AA p.11 '03 USE3VN Use audio channel 3 to modulate nothing')", un);
}

} // namespace s5
