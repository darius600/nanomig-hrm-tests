// s7r_p3.h -- P3 to P7 (the UHRES bit-plane and sprite pointers), SPECIFICATION.md section 5.
#pragma once

#include "s7r_u.h"

namespace s7r {

inline std::string ids_pair(const char* a, const char* b) { return std::string(a) + " " + b; }

inline std::string u_summary(const URun& r) {
    int ok1 = 0, ok2 = 0;
    for (int v = r.d.bstrt; v < r.d.bstop; ++v) if (on_line(r.bl, v).size() == 1) ++ok1;
    for (int v = r.d.sstrt + 1; v < r.d.sstop; ++v) if (on_line(r.sl, v).size() == 2) ++ok2;
    return r.d.nm + ": BPLH " + dec((long long)r.bplh.size()) + " SPRH " + dec((long long)r.sprh.size()) + " stray " + dec((long long)r.stray.size()) +
           "; lines with one BPLH access " + dec(ok1) + " of " + dec(r.d.bstop - r.d.bstrt) + ", with two SPRH accesses " + dec(ok2) + " of " + dec(r.d.sstop - r.d.sstrt - 1);
}

inline bool e5_bplh_bounds(const URun& r) {
    const int st = r.d.bstrt, sp = r.d.bstop;
    uint64_t tb = tmark(r, st - 1), ts = tmark(r, sp);
    if (!tb || !ts) return false;
    if (count_before(r.bplh, tb) != 0) return false;                                 // E5.1
    const auto& ls = on_line(r.bl, st);
    if (ls.empty() || r.bplh.empty() || r.bplh.front().t != ls.front().t) return false;   // E5.2
    if (!one_per_line(r, st, sp - 1)) return false;                                  // E5.3
    return count_after(r.bplh, ts) == 0;                                             // E5.4
}

inline bool e7_sprh_bounds(const URun& r) {
    const int st = r.d.sstrt, sp = r.d.sstop;
    uint64_t t1 = tmark(r, st - 2), t2 = tmark(r, sp);
    if (!t1 || !t2) return false;
    return count_before(r.sprh, t1) == 0 && two_per_line(r, st + 1, sp - 1) && count_after(r.sprh, t2) == 0;
}

inline bool e6_3_increments(const URun& r) {
    for (int v = r.d.vlo; v < r.d.vhi; ++v) {
        const auto& a = on_line(r.sl, v); const auto& b = on_line(r.sl, v + 1);
        if (a.empty() || b.empty()) continue;
        uint32_t amax = 0, bmin = ~0u;
        for (auto& x : a) amax = std::max(amax, x.a);
        for (auto& x : b) bmin = std::min(bmin, x.a);
        if (!(bmin > amax)) return false;
    }
    return !r.sprh.empty();
}

inline bool e6_4_order(const URun& r) {
    for (int v = r.d.sstrt + 1; v < r.d.sstop; ++v) {
        const auto& s = on_line(r.sl, v); const auto& b = on_line(r.bl, v);
        if (s.size() != 2 || b.size() != 1) return false;
        if (!(s[0].t < b[0].t && b[0].t < s[1].t)) return false;
    }
    return true;
}

inline void run_uhres(Checks& ck, Shared& sh) {
    const bool kok = sh.hv.conv.ok;
    const double K = kok ? sh.hv.conv.K : 0.0;
    std::string b1 = "prog_u1.bin", b2 = "prog_u2.bin", b3 = "prog_u3.bin", b4 = "prog_u4.bin", b0 = "prog_u0.bin", b0d = "prog_u0d.bin";
    int glo1 = 0x60;
    if (mut("FIXTURE-U")) b1 = "prog_u1_nofix.bin";
    if (mut("PR-U1")) b1 = "prog_u1_noend.bin";
    if (mut("PR-U2")) b1 = "prog_u1_skip.bin";
    if (mut("PR-U3")) b1 = "prog_u1_late.bin";
    if (mut("PR-U4") || mut("PR-9-U4")) b0 = "prog_u1.bin";
    if (mut("PR-U5")) { b1 = "prog_u1_gate0.bin"; glo1 = 0; }
    if (mut("R2-XCHK-U")) { /* the shared run UR is the mutated one */ }
    if (mut("P3-E1") || mut("P3-AGG") || mut("P6-E1") || mut("P6-AGG")) b1 = "prog_u0.bin";
    if (mut("P3-E2") || mut("P6-E2")) b1 = "prog_u2.bin";
    if (mut("P3-E4") || mut("P6-E5")) b3 = "prog_u2.bin";
    if (mut("P3-E5")) b1 = "prog_u1_sph16.bin";
    if (mut("P3-U0") || mut("P6-U0")) b0 = "prog_u1.bin";
    if (mut("P3-U0d")) b0d = "prog_u1.bin";
    if (mut("P4")) b2 = "prog_u2_m16.bin";
    if (mut("P5")) b1 = "prog_u1_s58.bin";
    if (mut("P6-E3")) b1 = "prog_u1_rw.bin";
    if (mut("P6-CTRL")) b1 = "prog_u1_sstrt50.bin";
    if (mut("P7")) b1 = "prog_u1_ss60.bin";

    std::string bd1 = "prog_d1.bin", bd0 = "prog_d0.bin";
    if (mut("FIXTURE-D")) bd1 = "prog_d1_nofix.bin";
    if (mut("PR-9-U1")) bd1 = "prog_d1_noend.bin";
    if (mut("PR-9-U2")) bd1 = "prog_d1_skip.bin";
    if (mut("PR-9-U3")) bd1 = "prog_d1_late.bin";
    if (mut("PR-9a")) bd1 = "prog_d1_var.bin";
    if (mut("P9-E1")) bd1 = "prog_d0.bin";
    if (mut("P9-E2")) bd1 = "prog_d1_m16.bin";
    if (mut("P9-D0")) bd0 = "prog_d1.bin";
    URun U1 = urun("U1", b1), U2 = urun("U2", b2), U3 = urun("U3", b3), U4 = urun("U4", b4), U0 = urun("U0", b0), U0d = urun("U0d", b0d);
    URun D1 = urun("D1", bd1), D0 = urun("D0", bd0);
    measure_u(D1); measure_u(D0);
    U1.d.glo = glo1;
    std::vector<URun*> all = {&U1, &U2, &U3, &U4, &U0, &U0d};
    for (URun* r : all) measure_u(*r);
    const std::vector<URun*> ui = {&U1, &U2, &U3, &U4};          // the runs with UHRES enabled
    const std::vector<URun*> uall = {&U1, &U2, &U3, &U4, &U0, &U0d, &sh.ur};

    bool fx = true;
    for (URun* r : all) fx = fx && r->cap.loaded && r->cap.released && r->cap.fixture == 0xBEEF;
    ck.expect(fx, "FIXTURE-U: runs U1, U2, U3, U4, U0 and U0d loaded, booted and stored the constant $BEEF at their fixture address before any access is classified");

    bool xc = sh.ur_ok && sh.anchor;
    for (URun* r : all) xc = xc && xcheck(*r, sh.ur);
    ck.expect(xc, "R2-XCHK-U: in U1, U2, U3, U4, U0 and U0d, from the end of prologue step 1 to the end marker, the accesses the refresh output marks are exactly the accesses at the port times of UR's non-CPU accesses");

    bool pu1 = true;
    for (URun* r : uall) pu1 = pu1 && r->cap.done && r->tend != 0;
    ck.expect(pu1, "PR-U1: the end marker is present in U1, U2, U3, U4, U0, U0d and UR");

    bool pu2 = true;
    for (URun* r : uall) pu2 = pu2 && r->an.all && r->an.lines && r->an.window && r->an.mono;
    ck.expect(pu2, "PR-U2: in U1, U2, U3, U4, U0, U0d and UR a marker exists for every V of the run's window, its line number equals V, its H field lies in the loop's gate and the marker times strictly increase (R1)");

    bool pu3 = true;
    for (URun* r : uall) pu3 = pu3 && r->setup_line >= 0 && r->setup_line < r->d.vlo;
    ck.expect(pu3, "PR-U3: the line in the setup-done record is below Vlo in U1, U2, U3, U4, U0, U0d and UR (the setup finished before the marker window)");

    const bool pu4 = U0.tend != 0 && U0.uh.empty();
    ck.expect(pu4, "PR-U4: from the setup-done record to the end marker every access of U0, whose UHRES bit is clear, is a refresh, CPU or zero-block access (AA p.16 BPLCON0 bit 7 'UHRES= ultrahi res enables the UHRES pointers')");

    bool pu5 = kok;
    size_t nuh = 0;
    for (URun* r : ui) {
        for (auto& x : r->uh) {
            ++nuh;
            for (size_t i = 0; i < r->m.t.size(); ++i) {
                if (!r->m.have[i]) continue;
                double dist = double(x.t > r->m.t[i] ? x.t - r->m.t[i] : r->m.t[i] - x.t) / K;
                if (dist < 32.0) pu5 = false;
            }
        }
    }
    ck.expect(pu5, "PR-U5: every UHRES access of U1, U2, U3 and U4 lies at least $20 memory cycles from every marker time, so the line assignment of R1 is unambiguous");
    ck.note("PR-U5 population: " + dec((long long)nuh) + " UHRES accesses in U1, U2, U3 and U4 (" + cap_core() + ")");
    for (URun* r : all) ck.note(u_summary(*r) + " (" + cap_core() + ")");

    const bool pre = fx && xc && pu1 && pu2 && pu3 && pu4 && pu5;
    const std::string unmet = "FIXTURE-U / R2-XCHK-U / PR-U1 to PR-U5";

    // ---- P3 ----
    const std::string ids3 =
        "opus:AA-bplhpth.c1 opus:AA-bplhpth.c2 opus:AA-bplhpth.c4 opus:AA-bplhpth.c6 opus:AA-bplhptl.c1 opus:AA-bplhptl.c2 opus:AA-bplhptl.c4 opus:AA-bplhptl.c6 "
        "sol:AA-bplhpth.c1 sol:AA-bplhpth.c2 sol:AA-bplhpth.c4 sol:AA-bplhpth.c6 sol:AA-bplhptl.c1 sol:AA-bplhptl.c2 sol:AA-bplhptl.c4 sol:AA-bplhptl.c6";
    bool e31 = true;
    for (URun* r : {&U1, &U2, &U3}) e31 = e31 && one_per_line(*r, 0x50, 0x6F);
    graded(ck, pre, e31,
           "P3-E1: in U1, U2 and U3 every line V with $50 <= V <= $6F has exactly one BPLH access (AA p.15 BPLHPTH,L: 'this pointer comes out on the 2nd free cycle after the start of each horiz. line')", unmet);
    bool e32 = true;
    for (URun* r : {&U1, &U2, &U3}) {
        const auto& ls = on_line(r->bl, 0x50);
        e32 = e32 && ls.size() == 1 && ls[0].a == r->P;
    }
    graded(ck, pre, e32,
           "P3-E2: the BPLH access on line $50 is at (BPLHPTH bits 4-0) << 16 | (BPLHPTL & $FFFE): $15AAAA in U1 and U3, $0A5554 in U2 (AA p.6 legend '+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4.')", unmet);
    bool e34 = !U1.bplh.empty() && tl(U3.bplh) == tl(U1.bplh);
    graded(ck, pre, e34,
           "P3-E4: U3, whose BPLHPTH has bits 15-5 set, has the same BPLH access times and addresses as U1 (the high word uses DB0-DB4 only)", unmet);
    bool e35 = true;
    for (URun* r : ui) e35 = e35 && r->stray.empty();
    graded(ck, pre, e35, "P3-E5: no UHRES access of U1, U2, U3 or U4 lies outside the BPLH and SPRH ranges of its run (no stray access)", unmet);
    graded(ck, pre, e31 && e32 && e34 && e35,
           "P3-AGG: BPLHPTH and BPLHPTL hold the UHRES bit-plane pointer: with UHRES enabled its address is accessed exactly once on every line of the range, starting at the written address (AA p.10 rows 1EC and 1EE, p.15) " + ids3, unmet);
    {
        bool c0 = fx && xc && pu1 && pu2 && pu3 && U0.tend && U0.bplh.empty() && U0.sprh.empty() && U0.stray.empty();
        graded(ck, fx && xc && pu1 && pu2 && pu3, c0,
               "P3-U0: U0, run as U1 with the UHRES bit of BPLCON0 clear, has no BPLH and no SPRH access (AA p.16 'UHRES= ultrahi res enables the UHRES pointers')", "FIXTURE-U / R2-XCHK-U / PR-U1 to PR-U3");
        bool cd = U0d.tend && U0d.bplh.empty() && U0d.sprh.empty() && U0d.stray.empty();
        graded(ck, fx && xc && pu1 && pu2 && pu3, cd,
               "P3-U0d: U0d, run as U1 with DMACON lacking DMAEN, has no BPLH and no SPRH access (AA p.22 '09 DMAEN Enable all DMA below (also UHRES dma)')", "FIXTURE-U / R2-XCHK-U / PR-U1 to PR-U3");
    }

    // ---- P4 ----
    {
        const bool pre4 = pre && one_per_line(U1, 0x50, 0x6F) && one_per_line(U2, 0x50, 0x6F);
        bool ok = pre4;
        std::string m;
        for (URun* r : {&U1, &U2}) {
            if (!pre4) break;
            for (int v = 0x50; v < 0x6F; ++v)
                if (int64_t(on_line(r->bl, v + 1)[0].a) - int64_t(on_line(r->bl, v)[0].a) != r->bstep) ok = false;
            m += " " + r->d.nm + ": " + dec(r->bstep) + " expected, first steps";
            for (int v = 0x50; v < 0x53; ++v) m += " " + dec(int64_t(on_line(r->bl, v + 1)[0].a) - int64_t(on_line(r->bl, v)[0].a));
        }
        if (pre4) ck.note("P4 steps between the BPLH addresses of consecutive lines:" + m + " (" + cap_core() + ")");
        graded(ck, pre4, ok,
               "P4: from line to line the BPLH address advances by the sign-extended BPLHMOD plus 2: +18 in U1 (BPLHMOD $0010) and -14 in U2 (BPLHMOD $FFF0), for every pair of consecutive lines $50..$6F (AA p.15 BPLHMOD 'every line, and then another 2 is added') opus:AA-bplhmod.c1 opus:AA-bplhmod.c2 opus:AA-bplhmod.c5 sol:AA-bplhmod.c1 sol:AA-bplhmod.c2",
               unmet + " / E3.1 in U1 and U2");
    }

    // ---- P5 ----
    {
        const bool ok = e5_bplh_bounds(U1) && e5_bplh_bounds(U4);
        graded(ck, pre, ok,
               "P5: BPLHSTRT and BPLHSTOP bound the lines on which the UHRES bit-plane pointer is fetched: in U1 (start $50, stop $70) and U4 (start $F8, stop $108) no BPLH access comes before line start, line start has the first one, every line from start to stop-1 has one, and none comes after t(stop) (AA p.15 BPLHSTRT 'the line when the data fetch starts', BPLHSTOP 'the line when the data fetch stops') opus:AA-bplhstrt.c1 opus:AA-bplhstrt.c2 opus:AA-bplhstrt.c5 opus:AA-bplhstop.c1 opus:AA-bplhstop.c2 opus:AA-bplhstop.c5 sol:AA-bplhstrt.c1 sol:AA-bplhstrt.c2 sol:AA-bplhstop.c1 sol:AA-bplhstop.c2",
               unmet);
    }

    // ---- P6 ----
    {
        bool e61 = true, e62 = true, e63 = true, e64 = true;
        for (URun* r : {&U1, &U2, &U3}) {
            e61 = e61 && two_per_line(*r, 0x59, 0x67);
            e62 = e62 && !r->sprh.empty() && r->sprh.front().a == r->S;
            e63 = e63 && e6_3_increments(*r);
            e64 = e64 && e6_4_order(*r);
        }
        const bool e65 = !U1.sprh.empty() && tl(U3.sprh) == tl(U1.sprh);
        graded(ck, pre, e61, "P6-E1: in U1, U2 and U3 every line strictly inside the SPRH range, $59 to $67, has exactly two SPRH accesses (AA p.27 SPRHPTH,L 'activated in the 1st & 3rd free cycles after horiz line start')", unmet);
        graded(ck, pre, e62, "P6-E2: the first SPRH access of each of U1, U2 and U3 is at (SPRHPTH bits 4-0) << 16 | (SPRHPTL & $FFFE): $132468 in U1 and U3, $0CDB96 in U2 (AA p.6 legend '+')", unmet);
        graded(ck, pre, e63, "P6-E3: in U1, U2 and U3, for every pair of consecutive lines that both have SPRH accesses, every SPRH address on the later line is greater than every SPRH address on the earlier one (AA p.27 'It increments for the next line')", unmet);
        graded(ck, pre, e64, "P6-E4: on every line $59 to $67 of U1, U2 and U3 the time order of the accesses is SPRH, BPLH, SPRH (AA p.27 '1st & 3rd free cycles', AA p.15 BPLHPTH,L 'on the 2nd free cycle')", unmet);
        graded(ck, pre, e65, "P6-E5: U3, whose SPRHPTH has bits 15-5 set, has the same SPRH access times and addresses as U1", unmet);
        bool ctrl = true;
        for (int v = 0x51; v <= 0x56; ++v) ctrl = ctrl && on_line(U1.bl, v).size() == 1 && on_line(U1.sl, v).size() == 0;
        graded(ck, pre, ctrl, "P6-CTRL: lines $51 to $56 of U1, inside the BPLH range but at or below SPRHSTRT-2, carry one BPLH access and no SPRH access", unmet);
        const bool c0 = fx && xc && pu1 && pu2 && pu3 && U0.tend && U0.sprh.empty() && U0.bplh.empty() && U0.stray.empty();
        graded(ck, fx && xc && pu1 && pu2 && pu3, c0, "P6-U0: U0, run as U1 with the UHRES bit of BPLCON0 clear, has no SPRH access", "FIXTURE-U / R2-XCHK-U / PR-U1 to PR-U3");
        graded(ck, pre, e61 && e62 && e63 && e64 && e65,
               "P6-AGG: SPRHPTH and SPRHPTL hold the UHRES sprite pointer: it comes out twice on each line of its range, before and after the bit-plane pointer, starting at the written address and moving to higher addresses on each following line (AA p.10 rows 1E8 and 1EA, p.27) opus:AA-sprhpth.c1 opus:AA-sprhpth.c2 opus:AA-sprhpth.c4 opus:AA-sprhpth.c6 opus:AA-sprhptl.c1 opus:AA-sprhptl.c2 opus:AA-sprhptl.c4 opus:AA-sprhptl.c6 sol:AA-sprhpth.c1 sol:AA-sprhpth.c2 sol:AA-sprhpth.c4 sol:AA-sprhpth.c6 sol:AA-sprhptl.c1 sol:AA-sprhptl.c2 sol:AA-sprhptl.c4 sol:AA-sprhptl.c6",
               unmet);
    }

    // ---- P7 ----
    {
        const bool ok = e7_sprh_bounds(U1) && e7_sprh_bounds(U4);
        graded(ck, pre, ok,
               "P7: SPRHSTRT and SPRHSTOP bound the lines on which the UHRES sprite pointer comes out: in U1 (start $58, stop $68) and U4 (start $100, stop $110) no SPRH access comes before t(start-2), every line from start+1 to stop-1 has exactly two, and none comes after t(stop) (AA p.28 SPRHSTRT/SPRHSTOP 'UHRES sprite vertical display start/stop') opus:AA-sprhstrt.c1 opus:AA-sprhstrt.c2 opus:AA-sprhstrt.c5 opus:AA-sprhstop.c1 opus:AA-sprhstop.c2 opus:AA-sprhstop.c5 sol:AA-sprhstrt.c1 sol:AA-sprhstrt.c2 sol:AA-sprhstop.c1 sol:AA-sprhstop.c2",
               unmet);
    }
    // ---- P9 ----
    {
        bool fxd = true;
        for (URun* r : {&D1, &D0}) fxd = fxd && r->cap.loaded && r->cap.released && r->cap.fixture == 0xBEEF;
        ck.expect(fxd, "FIXTURE-D: runs D1 and D0 loaded, booted and stored the constant $BEEF at their fixture address before any access is classified");
        bool xcd = sh.ur_ok && sh.anchor;
        for (URun* r : {&D1, &D0}) xcd = xcd && xcheck(*r, sh.ur);
        ck.expect(xcd, "R2-XCHK-D: in D1 and D0, from the end of prologue step 1 to the end marker, the accesses the refresh output marks are exactly the accesses at the port times of UR's non-CPU accesses");
        bool q1 = true, q2 = true, q3 = true;
        for (URun* r : {&D1, &D0}) {
            q1 = q1 && r->cap.done && r->tend != 0;
            q2 = q2 && r->an.all && r->an.lines && r->an.window && r->an.mono;
            q3 = q3 && r->setup_line >= 0 && r->setup_line < r->d.vlo;
        }
        ck.expect(q1, "PR-9-U1: the end marker is present in D1 and D0");
        ck.expect(q2, "PR-9-U2: in D1 and D0 a marker exists for every V of the window, its line number equals V, its H field lies in the loop's gate and the marker times strictly increase (R1)");
        ck.expect(q3, "PR-9-U3: the line in the setup-done record is below Vlo in D1 and D0");
        ck.expect(pu4, "PR-9-U4: run U0, with the UHRES bit clear, has no UHRES and no stray access from the setup-done record to the end marker");
        // PR-9a: the ordinary line length is unchanged by DUAL
        bool p9a = kok && U1.an.all && D1.an.all && D0.an.all;
        if (p9a) {
            double lo = 1e18, hi = 0;
            for (int v = 0x41; v <= 0x80; ++v) {
                double d = double(tmark(U1, v) - tmark(U1, v - 1));
                lo = std::min(lo, d); hi = std::max(hi, d);
            }
            const double spread = hi - lo;
            double worst = 0;
            for (URun* r : {&D1, &D0})
                for (int v = 0x41; v <= 0x80; ++v) {
                    double d = double(tmark(*r, v) - tmark(*r, v - 1));
                    double u = double(tmark(U1, v) - tmark(U1, v - 1));
                    worst = std::max(worst, std::fabs(d - u));
                }
            ck.note("PR-9a: U1's own spread of the marker interval " + num3(spread / K) + " memory cycles; largest difference of D1 or D0 from U1 " + num3(worst / K) + " (" + cap_core() + ")");
            if (worst > spread) p9a = false;
        }
        ck.expect(p9a, "PR-9a: the ordinary line length is unchanged by DUAL: in D1 and D0 each interval t(V) - t(V-1) differs from U1's by no more than U1's own spread of that interval (AA p.12 DUAL: 'Run the horizontal comparators with the alternate horizontal beam counter')");

        const bool pre9 = fxd && xcd && q1 && q2 && q3 && pu4 && p9a && kok && sh.ur_ok;
        const std::string unmet9 = "FIXTURE-D / R2-XCHK-D / PR-9-U1 to PR-9-U4 / PR-9a";
        // the observation: BPLH accesses strictly between t($51) and t($6F)
        auto obs = [&](const URun& r, size_t& kcount, double& W, bool& adj2) {
            kcount = 0; W = 0; adj2 = true;
            uint64_t t0 = tmark(r, 0x51), t1 = tmark(r, 0x6F);
            if (!t0 || !t1) return false;
            W = double(t1 - t0) / K;
            std::vector<Acc> in;
            for (auto& x : r.bplh) if (x.t > t0 && x.t < t1) in.push_back(x);
            kcount = in.size();
            for (size_t i = 1; i < in.size(); ++i) if (in[i].a != in[i - 1].a + 2) adj2 = false;
            if (in.size() < 2) adj2 = false;
            return true;
        };
        size_t k1 = 0, k0 = 0; double w1 = 0, w0 = 0; bool a1 = false, a0 = false;
        const bool o1 = obs(D1, k1, w1, a1), o0 = obs(D0, k0, w0, a0);
        if (o1) ck.note("P9: D1 has " + dec((long long)k1) + " BPLH accesses in a window of " + num3(w1) + " memory cycles (HTOTAL+1 = 114); D0 has " + dec((long long)k0) + " in " + num3(w0) + " (" + cap_core() + ")");
        const double n1 = std::floor(w1 / 114.0), n2 = std::ceil(w1 / 114.0);
        graded(ck, pre9 && o1, double(k1) >= n1 - 1 && double(k1) <= n2 + 1,
               "P9-E1: in DUAL mode the UHRES bit-plane pointer comes out once every HTOTAL+1 = 114 memory cycles: between t($51) and t($6F) D1 has floor(W/114) - 1 <= K <= ceil(W/114) + 1 BPLH accesses (AA p.12 DUAL: 'allows the UHRES pointers to come out more than once in a horizontal line', AA p.24 HHPOSR/HHPOSW 'triggering the UHRES pointers')", unmet9);
        graded(ck, pre9 && o1, a1,
               "P9-E2: in D1, whose BPLHMOD is 0, each BPLH address is the previous one + 2, for every consecutive pair between t($51) and t($6F) (AA p.15 BPLHMOD 'Its modulo is added every time it comes out'; the pointer advances by 2 after each access)", unmet9);
        graded(ck, pre9 && o0, k0 == 30,
               "P9-D0: D0, which repeats D1 with DUAL = 0, has exactly one BPLH access on each of the 30 lines $52 to $6F between t($51) and t($6F) (AA p.15 BPLHPTH,L 'on the 2nd free cycle after the start of each horiz. line')", unmet9);
    }
}

} // namespace s7r
