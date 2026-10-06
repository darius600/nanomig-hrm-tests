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
    double L[4] = {};                 // L64, L16, L40, L32
    std::vector<Fetch> fm;            // channel m's fetches in phase 2
};

constexpr uint16_t kVolWords[3] = {0x0040, 0x0010, 0x0028};

inline VolAtt vol_att(int m, int n, const std::string& pid) {
    const bool liver = mut((pid + "-LIVEREV").c_str());
    const bool live = mut((pid + "-LIVE").c_str()) || liver;   // the CPU writes the volumes the modulator words hold, on top of the attach
    VolAtt v;
    ARun& r = v.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[n], {0x7F7F});
    std::vector<uint16_t> mod = {kVolWords[0], kVolWords[1], kVolWords[2]};
    if (mut((pid + "-REV").c_str())) mod = {kVolWords[2], kVolWords[1], kVolWords[0]};
    poke_words(rs.pokes, kBuf[m], mod);
    s.w32(audlc(n), kBuf[n]); s.w16(audlen(n), 1); s.w16(audper(n), 200); s.w16(audvol(n), 32);
    s.w32(audlc(m), kBuf[m]); s.w16(audlen(m), 3); s.w16(audper(m), 1500); s.w16(audvol(m), 0);
    // phase 1: channel n alone
    r.m_start = s.w16(DMACON, start_val(1u << n));
    v.i_dm_n = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(2 * 227));
    const uint16_t cv[4] = {64, 16, 40, mut((pid + "-CAL").c_str()) ? uint16_t(40) : uint16_t(32)};   // contrast: L32 written as 40
    for (int i = 0; i < 4; ++i) { v.m_c[i] = s.w16(audvol(n), cv[i]); s.dly(wait_iters(3 * 227)); }
    // phase 2
    if (!mut((pid + "-NOADK").c_str())) {
        uint16_t val = mut((pid + "-CLR").c_str()) ? uint16_t(1u << m) : uint16_t(0x8000 | (1u << m));
        v.m_adk2 = s.w16(ADKCON, val);
    } else v.m_adk2 = s.mark();
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
    r.K = r.fr.K ? r.fr.K : 8; r.tau = 1 + r.K;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    uint64_t t2 = marker(r.cap, v.m_p2), t3 = marker(r.cap, v.m_p2end);
    v.fm = fetches(r.cap, kBuf[m] - 0x40, kBuf[m] + 0x1000, t2, t3);
    r.f = fetches(r.cap, kBuf[n] - 0x40, kBuf[n] + 0x1000, marker(r.cap, r.m_start), marker(r.cap, r.m_stop + 1));
    std::vector<Fetch> all = r.f; all.insert(all.end(), v.fm.begin(), v.fm.end());
    r.st = std_pre(r.cap, r.pre, all);
    uint64_t line = 227 * r.K;
    for (int i = 0; i < 4; ++i) {
        uint64_t ta = marker(r.cap, v.m_c[i] + 1);
        v.L[i] = mean_o(r.cap, ta + line, ta + 3 * line) - r.Z;
    }
    return v;
}

// The classes of phase 2's segments: 0 = L64, 1 = L16, 2 = L40, 3 = L32, 4 = other.
struct VolObs { std::vector<int> cls; size_t n32 = 0, nother = 0; size_t cnt[3] = {0, 0, 0}; bool cycle = true; size_t nseg = 0; };

inline VolObs vol_obs(const VolAtt& v) {
    VolObs o;
    const Cap& c = v.r.cap;
    uint64_t t2 = marker(c, v.m_p2), t3 = marker(c, v.m_p2end);
    auto segs = runs_of(c, t2, t3);
    bool started = false;
    for (auto& g : segs) {
        double lv = double(g.v) - v.r.Z;
        int k = 4;
        for (int i = 0; i < 4; ++i) if (within(lv, v.L[i], 1.0)) { k = i; break; }
        if (!started) { if (k <= 2) started = true; else continue; }
        o.cls.push_back(k);
    }
    o.nseg = o.cls.size();
    for (size_t i = 0; i < o.cls.size(); ++i) {
        int k = o.cls[i];
        if (k == 3) ++o.n32;
        else if (k == 4) ++o.nother;
        else ++o.cnt[k];
        if (i > 0 && o.cls[i - 1] <= 2 && k <= 2 && k != (o.cls[i - 1] + 1) % 3) o.cycle = false;
    }
    return o;
}

inline void run_vol_att(Checks& ck, const std::string& pid, const char* claim, int m, int n, const char* bitname) {
    VolAtt v = vol_att(m, n, pid);
    PreAgg agg; agg.add(v.r.st);
    note_gc(ck, pid, v.r.st);
    uint16_t dmn = logv(v.r.cap, v.i_dm_n), dmm = logv(v.r.cap, v.i_dm_m);
    bool gd = (dmn & 0x200) && (dmn & (1u << n)) && (dmm & 0x200) && (dmm & (1u << m));
    bool gf = v.fm.size() >= 12;
    bool dist = true;
    for (int i = 0; i < 4; ++i) for (int j = i + 1; j < 4; ++j) dist &= std::fabs(v.L[i] - v.L[j]) > 2.0;
    uint16_t a2 = logv(v.r.cap, v.i_adk2), a3 = logv(v.r.cap, v.i_adk3);
    bool adk = (a2 & 0x00FF) == (1u << m) && !(a3 & (1u << m));
    VolObs o = vol_obs(v);
    ck.note(pid + ": calibration L64=" + num(v.L[0]) + " L16=" + num(v.L[1]) + " L40=" + num(v.L[2]) + " L32=" + num(v.L[3]) + "; phase 2: segments=" + dec((long long)o.nseg) +
            " (L64 " + dec((long long)o.cnt[0]) + ", L16 " + dec((long long)o.cnt[1]) + ", L40 " + dec((long long)o.cnt[2]) + ", L32 " + dec((long long)o.n32) + ", other " + dec((long long)o.nother) +
            "), modulator fetches=" + dec((long long)v.fm.size()) + ", ADKCONR phase 2/3 = " + hex4(a2) + "/" + hex4(a3));
    print_std(ck, pid, agg, 1);
    ck.expect(gd, pid + "-G-D: after each start DMACONR bit 9 and the started channel's bit read 1, for channel n and for channel m (AA p.22)");
    ck.expect(gf, pid + "-G-F: channel m makes at least 12 fetches in phase 2 (AA p.22 '00 AUD0EN')");
    ck.expect(dist, pid + "-PRE-CAL: the calibration levels L64, L16, L40 and L32 differ pairwise by more than 2 LSB (AA p.11 AUDxVOL)");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && dist;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F or the calibration";
    auto levels_ok = [](const VolObs& q) { return q.nseg > 0 && q.n32 == 0 && q.nother == 0 && q.cycle && q.cnt[0] >= 3 && q.cnt[1] >= 3 && q.cnt[2] >= 3; };
    graded(ck, pre, adk, pid + "-ADK: in phase 2 ADKCONR bit " + dec(m) + " reads 1 and the other bits 7 to 0 read 0, and in phase 3 bit " + dec(m) + " reads 0 (AA p.10 'SET/CLR Determines if bits written with a 1 get set or cleared')", un);
    graded(ck, pre, levels_ok(o), pid + "-LEVELS: in phase 2 every segment of channel " + dec(n) + "'s output has the level L64, L16 or L40, none has L32, the segments cycle 64 -> 16 -> 40 and each value occurs at least three times (AA p.11 '" + bitname + "')", un);
    graded(ck, pre, adk && levels_ok(o), pid + " " + claim + ": with ADKCON bit " + dec(m) + " set, which ADKCONR reads back as 1, the data words of channel " + dec(m) + "'s DMA set the volume of channel " + dec(n) + ", which takes the volumes of the words cyclically in place of the volume the CPU wrote (AA p.11 '" + bitname + "')", un);
}

// ---- period attach ------------------------------------------------------------

struct PerAtt {
    ARun r;
    int m_p1 = 0, m_p2 = 0, m_p2end = 0, m_adk2 = 0, m_adk3 = 0;
    int i_adk2 = -1, i_adk3 = -1, i_dm_n = -1, i_dm_m = -1;
    std::vector<Fetch> fm;
};

constexpr uint16_t kPerWords[3] = {0x00C8, 0x0258, 0x015E};

inline PerAtt per_att(int m, int n, const std::string& pid) {
    const bool liver = mut((pid + "-LIVEREV").c_str());
    const bool live = mut((pid + "-LIVE").c_str()) || liver;   // the CPU writes the periods the modulator words hold, on top of the attach
    PerAtt v;
    ARun& r = v.r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[n], {0x7F80});
    std::vector<uint16_t> mod = {kPerWords[0], kPerWords[1], kPerWords[2]};
    if (mut((pid + "-REV").c_str())) mod = {kPerWords[2], kPerWords[1], kPerWords[0]};
    poke_words(rs.pokes, kBuf[m], mod);
    s.w32(audlc(n), kBuf[n]); s.w16(audlen(n), 1); s.w16(audper(n), mut((pid + "-PER").c_str()) ? 800 : 400); s.w16(audvol(n), 64);
    s.w32(audlc(m), kBuf[m]); s.w16(audlen(m), 3); s.w16(audper(m), 3000); s.w16(audvol(m), 0);
    v.m_p1 = r.m_start = s.w16(DMACON, start_val(1u << n));
    v.i_dm_n = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(10 * 227));
    if (!mut((pid + "-NOADK").c_str())) {
        uint16_t val = mut((pid + "-CLR").c_str()) ? uint16_t(1u << (4 + m)) : uint16_t(0x8000 | (1u << (4 + m)));
        v.m_adk2 = s.w16(ADKCON, val);
    } else v.m_adk2 = s.mark();
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
    r.K = r.fr.K ? r.fr.K : 8; r.tau = 1 + r.K;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    v.fm = fetches(r.cap, kBuf[m] - 0x40, kBuf[m] + 0x1000, marker(r.cap, v.m_p2), marker(r.cap, v.m_p2end));
    r.f = fetches(r.cap, kBuf[n] - 0x40, kBuf[n] + 0x1000, marker(r.cap, r.m_start), marker(r.cap, r.m_stop + 1));
    std::vector<Fetch> all = r.f; all.insert(all.end(), v.fm.begin(), v.fm.end());
    r.st = std_pre(r.cap, r.pre, all);
    return v;
}

struct PerObs {
    size_t nruns = 0, ntrans = 0, bad_trans = 0, n400 = 0;
    size_t cnt[3] = {0, 0, 0};
    bool cycle = true;
    std::string seq;
};

inline PerObs per_obs(const PerAtt& v) {
    PerObs o;
    const Cap& c = v.r.cap;
    const uint64_t K = v.r.K, tau = v.r.tau;
    uint64_t t2 = marker(c, v.m_p2), t3 = marker(c, v.m_p2end);
    auto runs = runs_of(c, t2, t3);
    const uint64_t want[3] = {200 * K, 600 * K, 350 * K};
    auto cls = [&](const LRun& u) {
        for (int i = 0; i < 3; ++i) if (u.len() + tau >= want[i] && u.len() <= want[i] + tau) return i;
        return 3;
    };
    std::vector<int> k;
    size_t first = runs.size();
    for (size_t i = 0; i < runs.size(); ++i) if (cls(runs[i]) < 3) { first = i; break; }
    for (size_t i = first; i < runs.size(); ++i) k.push_back(cls(runs[i]));
    // the last run is cut by the phase-3 marker: leave it out
    if (!k.empty()) k.pop_back();
    o.nruns = k.size();
    for (size_t i = 0; i < k.size(); ++i) {
        if (k[i] == 3) {
            ++o.ntrans;
            const LRun& u = runs[first + i];
            if (u.len() + tau >= 400 * K && u.len() <= 400 * K + tau) ++o.n400;
        }
    }
    // segments: transitional runs sit only between two segments of different class, one at a time
    std::vector<int> segs;
    int last = -1;
    for (size_t i = 0; i < k.size(); ++i) {
        if (k[i] == 3) {
            bool single = (i == 0 || k[i - 1] != 3) && (i + 1 >= k.size() || k[i + 1] != 3);
            int before = -1, after = -1;
            for (size_t j = i; j-- > 0;) if (k[j] != 3) { before = k[j]; break; }
            for (size_t j = i + 1; j < k.size(); ++j) if (k[j] != 3) { after = k[j]; break; }
            if (!single || (after >= 0 && before >= 0 && before == after)) ++o.bad_trans;
            continue;
        }
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
    bool gf = v.fm.size() >= 12;
    // G-P in phase 1: runs of 400 colour clocks
    bool gp = true;
    {
        auto runs = runs_of(v.r.cap, marker(v.r.cap, v.m_p1 + 1), marker(v.r.cap, v.m_p2));
        size_t cnt = 0;
        for (size_t i = 2; i + 2 < runs.size(); ++i) { ++cnt; if (runs[i].len() + v.r.tau < 400 * v.r.K || runs[i].len() > 400 * v.r.K + v.r.tau) gp = false; }
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
    auto ok = [](const PerObs& q) {
        return q.nruns > 0 && q.bad_trans == 0 && q.cycle && q.cnt[0] >= 3 && q.cnt[1] >= 3 && q.cnt[2] >= 3;
    };
    ck.note(pid + ": phase 2 runs=" + dec((long long)o.nruns) + " transitional=" + dec((long long)o.ntrans) + " (400-colour-clock runs " + dec((long long)o.n400) + ", misplaced " + dec((long long)o.bad_trans) +
            ") segments 200/600/350 = " + dec((long long)o.cnt[0]) + "/" + dec((long long)o.cnt[1]) + "/" + dec((long long)o.cnt[2]) + ", ADKCONR phase 2/3 = " + hex4(a2) + "/" + hex4(a3) + ", modulator fetches=" + dec((long long)v.fm.size()));
    print_std(ck, pid, agg, 1);
    ck.expect(gd, pid + "-G-D: after each start DMACONR bit 9 and the started channel's bit read 1, for channel n and for channel m (AA p.22)");
    ck.expect(gf, pid + "-G-F: channel m makes at least 12 fetches in phase 2 (AA p.22 '00 AUD0EN')");
    ck.expect(gp, pid + "-G-P: in phase 1 all runs of channel " + dec(n) + " last 400 colour clocks within tau (AA p.11 AUDxPER)");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && gp;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F or G-P";
    graded(ck, pre, adk, pid + "-ADK: ADKCONR bit " + dec(4 + m) + " reads 1 in phase 2 and 0 in phase 3, and the other bits 7 to 0 read 0 in phase 2 (AA p.10 'SET/CLR')", un);
    graded(ck, pre, ok(o), pid + "-RUNS: in phase 2 every run of channel " + dec(n) + " is classified as 200, 600 or 350 colour clocks apart from at most one transitional run at each segment boundary, the segments cycle 200 -> 600 -> 350 and each value occurs at least three times (AA p.11 '" + bitname + "')", un);
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
    s.w32(audlc(k), kBuf[k]); s.w16(audlen(k), 1); s.w16(audper(k), 300); s.w16(audvol(k), 64);
    s.w32(audlc(3), kBuf[3]); s.w16(audlen(3), 2); s.w16(audper(3), 700); s.w16(audvol(3), 0);
    uint16_t adk = 0x8008;
    std::string var = variant;
    if (var == "ATT" && k == 0) var = "VOL";   // no attach bit modulates channel 0: the contrast is a CPU volume write
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
    r.K = r.fr.K ? r.fr.K : 8; r.tau = 1 + r.K;
    r.Z = zero_ref(r.cap, r.pre, r.K);
    uint64_t t0 = marker(r.cap, b.m_start), t1 = marker(r.cap, b.m_stop + 1);
    b.f3 = fetches(r.cap, kBuf[3] - 0x40, kBuf[3] + 0x1000, t0, t1);
    b.fk = fetches(r.cap, kBuf[k] - 0x40, kBuf[k] + 0x1000, t0, t1);
    std::vector<Fetch> all = b.f3; all.insert(all.end(), b.fk.begin(), b.fk.end());
    r.st = std_pre(r.cap, r.pre, all);
    return b;
}

inline void run_p15(Checks& ck) {
    PreAgg agg;
    bool gd = true, gf = true, adk = true, runs_ok = true, lv_ok = true;
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
        // levels of run B
        uint64_t line = 227 * a.r.K;
        auto obs = [&](Bit3& q) {
            uint64_t t0 = marker(q.r.cap, q.m_start) + 2 * line, t1 = marker(q.r.cap, q.m_stop);
            return runs_of(q.r.cap, t0, t1);
        };
        auto rb_runs = obs(b), ra_runs = obs(a);
        std::set<int32_t> lb;
        for (size_t i = 1; i + 1 < rb_runs.size(); ++i) lb.insert(rb_runs[i].v);
        bool two_levels = lb.size() == 2;
        size_t nbad_len = 0, nbad_lv = 0;
        for (size_t i = 1; i + 1 < ra_runs.size(); ++i) {
            const LRun& u = ra_runs[i];
            if (u.len() + a.r.tau < 300 * a.r.K || u.len() > 300 * a.r.K + a.r.tau) ++nbad_len;
            bool m = false;
            for (int32_t l : lb) if (within(double(u.v), double(l), 1.0)) m = true;
            if (!m) ++nbad_lv;
        }
        runs_ok &= nbad_len == 0 && ra_runs.size() > 10;
        lv_ok &= nbad_lv == 0 && two_levels;
        ck.note("P15 k=" + dec(k) + ": run A runs=" + dec((long long)ra_runs.size()) + " wrong length=" + dec((long long)nbad_len) + " wrong level=" + dec((long long)nbad_lv) + " run B levels=" + dec((long long)lb.size()) +
                " ADKCONR A/B=" + hex4(ra) + "/" + hex4(rb) + " fetches ch3/chk=" + dec((long long)a.f3.size()) + "/" + dec((long long)a.fk.size()));
    }
    print_std(ck, "P15", agg, 6);
    ck.expect(gd, "P15-G-D: after the start in run A DMACONR bit 9, bit 3 and bit k read 1 for k = 0, 1, 2 (AA p.22)");
    ck.expect(gf, "P15-G-F: in run A channel 3 and channel k make fetches (at least six each) for k = 0, 1, 2 (AA p.22 '00 AUD0EN')");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf;
    const std::string un = "fixture, G-S0, G-C, G-D or G-F";
    graded(ck, pre, adk, "P15-ADK: ADKCONR bit 3 reads 1 in run A and 0 in run B for k = 0, 1, 2 (AA p.10 'SET/CLR')", un);
    graded(ck, pre, runs_ok && lv_ok, "P15-RUNS: in run A every run of channel k lasts 300 colour clocks within tau and has one of the two levels of channel k in run B, within 1 LSB, for k = 0, 1, 2 (AA p.11 '03 USE3VN Use audio channel 3 to modulate nothing')", un);
    graded(ck, pre, adk && runs_ok && lv_ok, "P15 sol:AA-adkcon.c27: with ADKCON bit 3 set and channel 3's DMA running no channel's volume or period changes: channels 0, 1 and 2 play exactly as with bit 3 clear (AA p.11 '03 USE3VN Use audio channel 3 to modulate nothing')", un);
}

} // namespace s5
