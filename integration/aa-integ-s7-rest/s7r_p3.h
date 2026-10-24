// s7r_p3.h -- P3 to P7 (the UHRES bit-plane and sprite pointers), SPECIFICATION.md section 5.
#pragma once

#include "s7r_u.h"

namespace s7r {

inline std::string first_line(const URun& r, const std::vector<Acc>& v) {
    if (v.empty()) return "none";
    const int l = line_of(r.m, v.front().t);
    return l < 0 ? "outside the markers" : "$" + hex(unsigned(l), 3);
}

inline std::string u_summary(const URun& r) {
    int ok1 = 0, ok2 = 0;
    for (int v = r.d.bstrt + 1; v < r.d.bstop; ++v) if (on_line(r.bl, v).size() == 1) ++ok1;
    for (int v = r.d.sstrt + 1; v < r.d.sstop; ++v) if (on_line(r.sl, v).size() == 2) ++ok2;
    return r.d.nm + ": BPLH " + dec((long long)r.bplh.size()) + " SPRH " + dec((long long)r.sprh.size()) + " stray " + dec((long long)r.stray.size()) +
           "; lines with one BPLH access " + dec(ok1) + " of " + dec(r.d.bstop - r.d.bstrt - 1) + ", with two SPRH accesses " + dec(ok2) + " of " + dec(r.d.sstop - r.d.sstrt - 1) +
           "; first BPLH access on line " + first_line(r, r.bplh) + " (BPLHSTRT $" + hex(unsigned(r.d.bstrt), 3) + "), first SPRH access on line " + first_line(r, r.sprh) + " (SPRHSTRT $" + hex(unsigned(r.d.sstrt), 3) + ")";
}

inline bool e5_bplh_bounds(const URun& r) {
    const int st = r.d.bstrt, sp = r.d.bstop;
    uint64_t tb = tmark(r, st - 1), ts = tmark(r, sp);
    if (!tb || !ts) return false;
    if (count_before(r.bplh, tb) != 0) return false;                                 // E5.1
    if (r.bplh.empty()) return false;
    const int l0 = line_of(r.m, r.bplh.front().t);
    if (l0 != st && l0 != st + 1) return false;                                      // E5.2: the first access is on line start or start+1
    if (!one_per_line(r, st + 1, sp - 1)) return false;                              // E5.3
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

// The accesses E6.4 orders exist: two SPRH accesses and one BPLH access on every line $59 to $67.
inline bool e6_4_population(const URun& r) {
    for (int v = r.d.sstrt + 1; v < r.d.sstop; ++v)
        if (on_line(r.sl, v).size() != 2 || on_line(r.bl, v).size() != 1) return false;
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
    if (mut("PR-U5")) { b1 = "prog_u1_gateB0.bin"; glo1 = 0xB0; }
    if (mut("R2-XCHK-U")) { /* the shared run UR is the mutated one */ }
    if (mut("P3-E1") || mut("P3-AGG") || mut("P6-E1") || mut("P6-AGG")) b1 = "prog_u0.bin";
    if (mut("P3-E2")) b1 = "prog_u1_bpl40.bin";    // the first BPLH address is 0x40 bytes beyond the expected one, inside the run's BPLH range
    if (mut("P6-E2")) b1 = "prog_u1_spl40.bin";    // the first SPRH address is 0x40 bytes beyond the expected one
    if (mut("P3-E4")) b3 = "prog_u3_bpl40.bin";    // U3's BPLH addresses differ from U1's
    if (mut("P6-E5")) b3 = "prog_u3_spl40.bin";    // U3's SPRH addresses differ from U1's
    if (mut("P3-E5")) b1 = "prog_u1_sph16.bin";
    if (mut("P3-U0") || mut("P6-U0")) b0 = "prog_u1.bin";
    if (mut("P3-U0d")) b0d = "prog_u1.bin";
    if (mut("P4")) b2 = "prog_u2_m0.bin";
    if (mut("P5")) b1 = "prog_u1_s58.bin";
    if (mut("P6-E3")) b1 = "prog_u1_rw.bin";
    if (mut("P6-CTRL")) b1 = "prog_u1_sstrt50.bin";
    if (mut("P7")) b1 = "prog_u1_ss60.bin";

    std::string bw = "prog_uw.bin";
    if (mut("P3-E6")) bw = "prog_uw_mb.bin";    // case 7 of the bit-plane pointer walks bit 8 instead of bit 7
    if (mut("P6-E6")) bw = "prog_uw_ms.bin";    // case 7 of the sprite pointer walks bit 8 instead of bit 7

    std::string bd1 = "prog_d1.bin", bd0 = "prog_d0.bin", bdh = "prog_dh.bin";
    if (mut("PR-9c")) bdh = "prog_dh_slow.bin";
    if (mut("PR-9b")) bd1 = "prog_d1_nohead.bin";
    if (mut("FIXTURE-D")) bd1 = "prog_d1_nofix.bin";
    if (mut("PR-9-U1")) bd1 = "prog_d1_noend.bin";
    if (mut("PR-9-U2")) bd1 = "prog_d1_skip.bin";
    if (mut("PR-9-U3")) bd1 = "prog_d1_late.bin";
    if (mut("PR-9a")) bd1 = "prog_d1_var.bin";
    if (mut("P9-E2")) bd1 = "prog_d1_m16.bin";
    if (mut("PR-9-U5")) bd1 = "prog_d1_nobit.bin";
    if (mut("P9-D0")) bd0 = "prog_d1.bin";
    URun U1 = urun("U1", b1), U2 = urun("U2", b2), U3 = urun("U3", b3), U4 = urun("U4", b4), U0 = urun("U0", b0), U0d = urun("U0d", b0d);
    URun UW = urun("UW", bw);
    URun D1 = urun("D1", bd1), D0 = urun("D0", bd0), DH = urun("DH", bdh);
    measure_u(D1); measure_u(D0); measure_u(DH);
    U1.d.glo = glo1;
    std::vector<URun*> all = {&U1, &U2, &U3, &U4, &U0, &U0d, &UW};
    for (URun* r : all) measure_u(*r);
    const std::vector<URun*> ui = {&U1, &U2, &U3, &U4};          // the runs with UHRES enabled and expected ranges
    const std::vector<URun*> uline = {&U1, &U2, &U3, &U4, &UW};  // the runs whose UHRES accesses are assigned to lines (PR-U5)
    const std::vector<URun*> uall = {&U1, &U2, &U3, &U4, &U0, &U0d, &UW, &sh.ur};

    bool fx = true;
    for (URun* r : all) fx = fx && r->cap.loaded && r->cap.released && r->cap.fixture == 0xBEEF;
    ck.expect(fx, "FIXTURE-U: runs U1, U2, U3, U4, U0, U0d and UW loaded, booted and stored the constant $BEEF at their fixture address before any access is classified");

    bool xc = sh.ur_ok && sh.anchor;
    for (URun* r : all) xc = xc && xcheck(*r, sh.ur);
    ck.expect(xc, "R2-XCHK-U: in U1, U2, U3, U4, U0, U0d and UW, from the end of prologue step 1 to the end marker, the accesses the refresh output marks are exactly the accesses at the port times of UR's non-CPU accesses");

    bool pu1 = true;
    for (URun* r : uall) pu1 = pu1 && r->cap.done && r->tend != 0;
    ck.expect(pu1, "PR-U1: the end marker is present in U1, U2, U3, U4, U0, U0d, UW and UR");

    bool pu2 = true;
    for (URun* r : uall) pu2 = pu2 && r->an.all && r->an.lines && r->an.window && r->an.mono;
    ck.expect(pu2, "PR-U2: in U1, U2, U3, U4, U0, U0d, UW and UR a marker exists for every V of the run's window, its line number equals V, its H field lies in the loop's gate and the marker times strictly increase (R1)");

    bool pu3 = true;
    for (URun* r : uall) pu3 = pu3 && r->setup_line >= 0 && r->setup_line < r->d.vlo;
    ck.expect(pu3, "PR-U3: the line in the setup-done record is below Vlo in U1, U2, U3, U4, U0, U0d, UW and UR (the setup finished before the marker window)");

    const bool pu4 = U0.tend != 0 && U0.uh.empty();
    ck.expect(pu4, "PR-U4: from the setup-done record to the end marker every access of U0, whose UHRES bit is clear, is a refresh, CPU or zero-block access (AA p.16 BPLCON0 bit 7 'UHRES= ultrahi res enables the UHRES pointers')");

    // PR-U5: every UHRES access lies on the line R1 assigns it to, by the H counter: before the sample of that line's marker,
    // at a non-negative beam position
    bool pu5d = true;
    size_t nuh = 0;
    double pmin = 1e18, pmax = -1e18;
    for (URun* r : uline) {
        const uint64_t tlast = (r->m.have.size() && r->m.have.back()) ? r->m.t.back() : r->tend;
        for (auto& x : r->uh) {
            if (x.t > tlast) continue;
            ++nuh;
            const int v = line_of(r->m, x.t);
            const uint64_t ts = v >= 0 ? sample_time(*r, v) : 0;
            if (!kok || v < 0 || !ts) { pu5d = false; continue; }
            const double p = beam_pos(*r, v, x.t, K);
            pmin = std::min(pmin, p); pmax = std::max(pmax, p);
            if (!(x.t < ts) || p < 0.0) pu5d = false;
        }
    }
    const bool pu5 = kok && pu5d;       // what the lines below need: the line of every access holds wherever there is an access
    if (nuh > 0)
        ck.expect(pu5, "PR-U5: every UHRES access of U1, U2, U3, U4 and UW lies on the line R1 assigns it to: before the sample of that line's marker, and at a beam position, read from the H field of the marker, that is not negative (AA p.29 H8-H1 'RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)')");
    else
        ck.record("NOT GRADED PR-U5: U1, U2, U3, U4 and UW have no UHRES access, so there is no line to grade");
    ck.note("PR-U5 population: " + dec((long long)nuh) + " UHRES accesses in U1, U2, U3, U4 and UW" +
            (nuh > 0 && pmax >= pmin ? "; beam positions from " + num3(pmin) + " to " + num3(pmax) + " memory cycles after the line start" : std::string()) + " (" + cap_core() + ")");
    for (URun* r : all) if (r != &UW) ck.note(u_summary(*r) + " (" + cap_core() + ")");

    bool pu6 = true, pu7 = true;
    for (URun* r : {&U1, &U2, &U3}) {
        pu6 = pu6 && !r->bplh.empty() && !r->sprh.empty();
        pu7 = pu7 && e6_4_population(*r);
    }
    ck.expect(pu6, "PR-U6: U1, U2 and U3 each have at least one BPLH access and at least one SPRH access from the setup-done record to t(Vhi)");
    ck.expect(pu7, "PR-U7: on every line $59 to $67 of U1, U2 and U3 there are two SPRH accesses and one BPLH access, so that the order of the accesses of a line is observed");

    const bool pre = fx && xc && pu1 && pu2 && pu3 && pu4 && pu5;
    const std::string unmet = "FIXTURE-U / R2-XCHK-U / PR-U1 to PR-U5";

    // ---- P3 ----
    const std::string ids3 =
        "opus:AA-bplhpth.c1 opus:AA-bplhpth.c2 opus:AA-bplhpth.c4 opus:AA-bplhpth.c6 opus:AA-bplhptl.c1 opus:AA-bplhptl.c2 opus:AA-bplhptl.c4 opus:AA-bplhptl.c6 "
        "sol:AA-bplhpth.c1 sol:AA-bplhpth.c2 sol:AA-bplhpth.c4 sol:AA-bplhpth.c6 sol:AA-bplhptl.c1 sol:AA-bplhptl.c2 sol:AA-bplhptl.c4 sol:AA-bplhptl.c6";
    bool e31 = true;
    for (URun* r : {&U1, &U2, &U3}) e31 = e31 && one_per_line(*r, 0x51, 0x6F);
    graded(ck, pre, e31,
           "P3-E1: in U1, U2 and U3 every line V with $51 <= V <= $6F, the lines after BPLHSTRT up to BPLHSTOP-1, has exactly one BPLH access (AA p.15 BPLHPTH,L: 'this pointer comes out on the 2nd free cycle after the start of each horiz. line')", unmet);
    bool e32 = true;
    for (URun* r : {&U1, &U2, &U3}) {
        if (r->bplh.empty()) { e32 = false; continue; }
        const int l0 = line_of(r->m, r->bplh.front().t);
        e32 = e32 && r->bplh.front().a == r->P && (l0 == 0x50 || l0 == 0x51);
    }
    graded(ck, pre && pu6, e32,
           "P3-E2: the first BPLH access of the run, which lies on line $50 or $51, is at (BPLHPTH bits 4-0) << 16 | (BPLHPTL & $FFFE): $15AAAA in U1 and U3, $0A5554 in U2 (AA p.6 legend '+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4.')", unmet + " / PR-U6");
    bool e34 = !U1.bplh.empty() && tl(U3.bplh) == tl(U1.bplh);
    graded(ck, pre && pu6, e34,
           "P3-E4: U3, whose BPLHPTH has bits 15-5 set, has the same BPLH access times and addresses as U1 (the high word uses DB0-DB4 only)", unmet + " / PR-U6");
    bool e35 = true;
    for (URun* r : ui) e35 = e35 && r->stray.empty();
    graded(ck, pre && nuh > 0, e35, "P3-E5: no UHRES access of U1, U2, U3 or U4 lies outside the BPLH and SPRH ranges of its run (no stray access)", unmet + " / U1 to U4 have UHRES accesses");
    std::string wdet_b, wdet_s;
    const bool e36 = walk_ok(UW, false, wdet_b);
    const bool e66 = walk_ok(UW, true, wdet_s);
    ck.note("E3.6 (BPLH walking cases in UW):" + wdet_b + " (" + cap_core() + ")");
    ck.note("E6.6 (SPRH walking cases in UW):" + wdet_s + " (" + cap_core() + ")");
    graded(ck, pre, e36,
           "P3-E6: in UW each of the 40 walking-one and walking-zero cases of the bit-plane pointer, which set or clear each of DB1-DB15 of BPLHPTL and DB0-DB4 of BPLHPTH alone, has a UHRES access at its address (BPLHPTH bits 4-0) << 16 | (BPLHPTL & $FFFE) on its line $51 + n (AA p.6 legend '+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4.')", unmet);
    graded(ck, pre, e31 && e32 && e34 && e35 && e36,
           "P3-AGG: BPLHPTH and BPLHPTL hold the UHRES bit-plane pointer: with UHRES enabled its address is accessed exactly once on every line of the range, starting at the written address, and each used pointer bit drives its address bit (AA p.10 rows 1EC and 1EE, p.15) " + ids3, unmet);
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
        // E3.1 over $51..$6F in U1 and U2 is a clause of the line: an empty BPLH population has no modulo to show
        const bool pop4 = one_per_line(U1, 0x51, 0x6F) && one_per_line(U2, 0x51, 0x6F);
        bool ok = pop4;
        std::string m;
        for (URun* r : {&U1, &U2}) {
            if (!pop4) break;
            for (int v = 0x51; v < 0x6F; ++v)
                if (int64_t(on_line(r->bl, v + 1)[0].a) - int64_t(on_line(r->bl, v)[0].a) != r->bstep) ok = false;
            m += " " + r->d.nm + ": " + dec(r->bstep) + " expected, first steps";
            for (int v = 0x51; v < 0x54; ++v) m += " " + dec(int64_t(on_line(r->bl, v + 1)[0].a) - int64_t(on_line(r->bl, v)[0].a));
        }
        ck.note(pop4 ? "P4 steps between the BPLH addresses of consecutive lines:" + m + " (" + cap_core() + ")"
                     : "P4: U1 and U2 do not have exactly one BPLH access on every line $51..$6F; BPLH accesses U1 " + dec((long long)U1.bplh.size()) + ", U2 " + dec((long long)U2.bplh.size()) + " (" + cap_core() + ")");
        graded(ck, pre, ok,
               "P4: every line $51..$6F of U1 and U2 has exactly one BPLH access, and from line to line the BPLH address advances by the sign-extended BPLHMOD plus 2: +18 in U1 (BPLHMOD $0010) and -14 in U2 (BPLHMOD $FFF0), for every pair of consecutive lines (AA p.15 BPLHMOD 'every line, and then another 2 is added') opus:AA-bplhmod.c1 opus:AA-bplhmod.c2 opus:AA-bplhmod.c5 sol:AA-bplhmod.c1 sol:AA-bplhmod.c2",
               unmet);
    }

    // ---- P5 ----
    {
        const bool ok = e5_bplh_bounds(U1) && e5_bplh_bounds(U4);
        graded(ck, pre, ok,
               "P5: BPLHSTRT and BPLHSTOP bound the lines on which the UHRES bit-plane pointer is fetched: in U1 (start $50, stop $70) and U4 (start $F8, stop $108) no BPLH access comes before t(start-1), the first one lies on line start or start+1, every line from start+1 to stop-1 has one, and none comes after t(stop) (AA p.15 BPLHSTRT 'the line when the data fetch starts', BPLHSTOP 'the line when the data fetch stops') opus:AA-bplhstrt.c1 opus:AA-bplhstrt.c2 opus:AA-bplhstrt.c5 opus:AA-bplhstop.c1 opus:AA-bplhstop.c2 opus:AA-bplhstop.c5 sol:AA-bplhstrt.c1 sol:AA-bplhstrt.c2 sol:AA-bplhstop.c1 sol:AA-bplhstop.c2",
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
        graded(ck, pre && pu6, e62, "P6-E2: the first SPRH access of each of U1, U2 and U3 is at (SPRHPTH bits 4-0) << 16 | (SPRHPTL & $FFFE): $132468 in U1 and U3, $0CDB96 in U2 (AA p.6 legend '+')", unmet + " / PR-U6");
        graded(ck, pre && pu6, e63, "P6-E3: in U1, U2 and U3, for every pair of consecutive lines that both have SPRH accesses, every SPRH address on the later line is greater than every SPRH address on the earlier one (AA p.27 'It increments for the next line')", unmet + " / PR-U6");
        graded(ck, pre && pu7, e64, "P6-E4: on every line $59 to $67 of U1, U2 and U3 the time order of the accesses is SPRH, BPLH, SPRH (AA p.27 '1st & 3rd free cycles', AA p.15 BPLHPTH,L 'on the 2nd free cycle')", unmet + " / PR-U7");
        graded(ck, pre && pu6, e65, "P6-E5: U3, whose SPRHPTH has bits 15-5 set, has the same SPRH access times and addresses as U1", unmet + " / PR-U6");
        bool ctrl = true;
        for (int v = 0x51; v <= 0x56; ++v) ctrl = ctrl && on_line(U1.bl, v).size() == 1 && on_line(U1.sl, v).size() == 0;
        graded(ck, pre, ctrl, "P6-CTRL: lines $51 to $56 of U1, inside the BPLH range but at or below SPRHSTRT-2, carry one BPLH access and no SPRH access", unmet);
        const bool c0 = fx && xc && pu1 && pu2 && pu3 && U0.tend && U0.sprh.empty() && U0.bplh.empty() && U0.stray.empty();
        graded(ck, fx && xc && pu1 && pu2 && pu3, c0, "P6-U0: U0, run as U1 with the UHRES bit of BPLCON0 clear, has no SPRH access", "FIXTURE-U / R2-XCHK-U / PR-U1 to PR-U3");
        graded(ck, pre, e66, "P6-E6: in UW each of the 40 walking-one and walking-zero cases of the sprite pointer, which set or clear each of DB1-DB15 of SPRHPTL and DB0-DB4 of SPRHPTH alone, has a UHRES access at its address (SPRHPTH bits 4-0) << 16 | (SPRHPTL & $FFFE) on its line $51 + n (AA p.6 legend '+')", unmet);
        graded(ck, pre, e61 && e62 && e63 && e64 && e65 && e66,
               "P6-AGG: SPRHPTH and SPRHPTL hold the UHRES sprite pointer: it comes out twice on each line of its range, before and after the bit-plane pointer, starting at the written address and moving to higher addresses on each following line, and each used pointer bit drives its address bit (AA p.10 rows 1E8 and 1EA, p.27) opus:AA-sprhpth.c1 opus:AA-sprhpth.c2 opus:AA-sprhpth.c4 opus:AA-sprhpth.c6 opus:AA-sprhptl.c1 opus:AA-sprhptl.c2 opus:AA-sprhptl.c4 opus:AA-sprhptl.c6 sol:AA-sprhpth.c1 sol:AA-sprhpth.c2 sol:AA-sprhpth.c4 sol:AA-sprhpth.c6 sol:AA-sprhptl.c1 sol:AA-sprhptl.c2 sol:AA-sprhptl.c4 sol:AA-sprhptl.c6",
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
        for (URun* r : {&D1, &D0, &DH}) fxd = fxd && r->cap.loaded && r->cap.released && r->cap.fixture == 0xBEEF;
        ck.expect(fxd, "FIXTURE-D: runs D1, D0 and DH loaded, booted and stored the constant $BEEF at their fixture address before any access is classified");
        bool xcd = sh.ur_ok && sh.anchor;
        for (URun* r : {&D1, &D0, &DH}) xcd = xcd && xcheck(*r, sh.ur);
        ck.expect(xcd, "R2-XCHK-D: in D1, D0 and DH, from the end of prologue step 1 to the end marker, the accesses the refresh output marks are exactly the accesses at the port times of UR's non-CPU accesses");
        bool q1 = DH.cap.done && DH.tend != 0, q2 = true, q3 = true;
        for (URun* r : {&D1, &D0}) {
            q1 = q1 && r->cap.done && r->tend != 0;
            q2 = q2 && r->an.all && r->an.lines && r->an.window && r->an.mono;
            q3 = q3 && r->setup_line >= 0 && r->setup_line < r->d.vlo;
        }
        ck.expect(q1, "PR-9-U1: the end marker is present in D1, D0 and DH");
        ck.expect(q2, "PR-9-U2: in D1 and D0 a marker exists for every V of the window, its line number equals V, its H field lies in the loop's gate and the marker times strictly increase (R1)");
        ck.expect(q3, "PR-9-U3: the line in the setup-done record is below Vlo in D1 and D0");
        ck.expect(pu4, "PR-9-U4: run U0, with the UHRES bit clear, has no UHRES and no stray access from the setup-done record to the end marker");
        // PR-9b: the sample times t_s of R1 exist for the markers $40 and $80 of D1 and D0
        const bool p9b = kok && sample_time(D1, 0x40) && sample_time(D1, 0x80) && sample_time(D0, 0x40) && sample_time(D0, 0x80);
        ck.expect(p9b, "PR-9b: the marker loop's first instruction is found in the programs of D1 and D0 and is fetched before the stores of markers $40 and $80, so that the sample times of R1 exist");
        // PR-9c: DH has 64 sample stores whose largest interval is below (N+1)/2 memory cycles
        const Samples sdh = samples(DH.cap);
        double dh_max = 0;
        if (sdh.full && kok)
            for (size_t i = 0; i + 1 < sdh.t.size(); ++i) dh_max = std::max(dh_max, double(sdh.t[i + 1] - sdh.t[i]) / K);
        const bool p9c = kok && sdh.full && dh_max < double(0x71 + 1) / 2.0;
        ck.note("PR-9c: DH has " + std::string(sdh.full ? "64" : "fewer than 64") + " sample stores, largest sample interval " + num3(dh_max) + " memory cycles (" + cap_core() + ")");
        ck.expect(p9c, "PR-9c: DH has 64 sample stores of HHPOSR and the largest interval between two of them is below (N+1)/2 = 57 memory cycles, so each phase step of the alternate counter is unambiguous");
        // PR-9a: the ordinary line length is unchanged by DUAL, from the H counter and the markers $40 and $80
        auto linelen = [&](const URun& r) {
            const double span = (double(sample_time(r, 0x80)) - double(sample_time(r, 0x40))) / K;
            return (span - double(marker_h(r, 0x80) - marker_h(r, 0x40))) / 64.0;
        };
        const bool pre9a = p9b && D1.an.all && D0.an.all;
        bool c9a = false;
        if (pre9a) {
            const double l1 = linelen(D1), l0 = linelen(D0);
            const bool uok = U1.an.all && sample_time(U1, 0x40) && sample_time(U1, 0x80);
            ck.note("PR-9a: main-counter line length in memory cycles, from the H fields of markers $40 and $80 over 64 lines: D1 " + num3(l1) + ", D0 " + num3(l0) +
                    (uok ? ", U1 " + num3(linelen(U1)) : std::string(", U1 not measured")) + "; rounded to whole steps D1 " + dec(std::llround(l1)) + ", D0 " + dec(std::llround(l0)) + " (" + cap_core() + ")");
            c9a = std::llround(l1) == std::llround(l0);
        }
        graded(ck, pre9a, c9a,
               "PR-9a: the ordinary line length is unchanged by DUAL: the main counter's line length, measured from the H fields of markers $40 and $80, is the same whole number of 280 ns steps in D1 and in D0 (AA p.12 DUAL: 'Run the horizontal comparators with the alternate horizontal beam counter'; p.24 'this many + 1 280ns increments')",
               "the R1 anchors of D1 and D0 / PR-9b");
        const bool p9a = pre9a && c9a;

        const bool pre9 = fxd && xcd && q1 && q2 && q3 && pu4 && p9a && p9c && kok && sh.ur_ok;
        const std::string unmet9 = "FIXTURE-D / R2-XCHK-D / PR-9-U1 to PR-9-U4 / PR-9b / PR-9c / PR-9a";
        // the observation: BPLH accesses strictly between t($51) and t($6F)
        auto obs = [&](const URun& r, size_t& kcount, bool& adj2) {
            kcount = 0; adj2 = true;
            uint64_t t0 = tmark(r, 0x51), t1 = tmark(r, 0x6F);
            if (!t0 || !t1) return false;
            std::vector<Acc> in;
            for (auto& x : r.bplh) if (x.t > t0 && x.t < t1) in.push_back(x);
            kcount = in.size();
            for (size_t i = 1; i < in.size(); ++i) if (in[i].a != in[i - 1].a + 2) adj2 = false;
            if (in.size() < 2) adj2 = false;
            return true;
        };
        size_t k1 = 0, k0 = 0; bool a1 = false, a0 = false;
        const bool o1 = obs(D1, k1, a1), o0 = obs(D0, k0, a0);
        if (o1) ck.note("P9: D1 has " + dec((long long)k1) + " BPLH accesses between t($51) and t($6F); D0 has " + dec((long long)k0) + " (" + cap_core() + ")");
        const bool q5 = o1 && k1 >= 2;
        ck.expect(q5, "PR-9-U5: D1 has at least two BPLH accesses between t($51) and t($6F), so that a step between two of them exists");
        // E9.1a (not graded): the lines of $52..$6F, by the H counter, with two or more BPLH accesses are counted
        std::map<int, int> perline;
        if (o1) {
            const uint64_t t0 = tmark(D1, 0x51), t1 = tmark(D1, 0x6F);
            for (auto& x : D1.bplh) if (x.t > t0 && x.t < t1) ++perline[beam_line(D1, x, K)];
        }
        int nl2 = 0, maxl = 0;
        for (int v = 0x52; v <= 0x6F; ++v) { const int c = perline.count(v) ? perline[v] : 0; maxl = std::max(maxl, c); if (c >= 2) ++nl2; }
        ck.note("P9-E1a (not graded): D1 lines of $52..$6F with two or more BPLH accesses: " + dec(nl2) + " of 30, most on one line " + dec(maxl) + " (" + cap_core() + ")");
        ck.record("P9-E1a: not graded. AA p.12 DUAL: \"This allows the UHRES pointers to come out more than once in a horizontal line, assuming there is some memory bandwidth left\"; no precondition of this package establishes that bandwidth. The absence of the UHRES chain is graded by P3-E6, P6-E6, P5 and P7.");
        // E9.1b (not graded): the alternate-counter resets between consecutive BPLH accesses of DH are counted
        std::vector<Acc> ah;
        if (sdh.full) for (auto& x : DH.bplh) if (x.t > sdh.t.front() && x.t < sdh.t.back()) ah.push_back(x);
        auto phase = [&](uint64_t a) {
            size_t i = size_t(std::upper_bound(sdh.t.begin(), sdh.t.end(), a - 1) - sdh.t.begin());    // the first sample with T >= a
            i = i ? i - 1 : 0;                                                                         // the last sample with T < a
            return std::fmod(double(sdh.r[i] & 0xFF) + double(a - sdh.t[i]) / K, double(0x71 + 1));
        };
        std::string nk;
        for (size_t k = 0; ah.size() >= 3 && k + 1 < ah.size(); ++k) {
            const double v = phase(ah[k].t) + double(ah[k + 1].t - ah[k].t) / K - phase(ah[k + 1].t);
            const long n = std::lround(v / double(0x71 + 1));
            nk += " " + dec(n);
        }
        ck.note("P9-E1b (not graded): DH has " + dec((long long)ah.size()) + " BPLH accesses between its first and last sample; alternate-counter resets between consecutive accesses:" + (nk.empty() ? " none" : nk) + " (" + cap_core() + ")");
        ck.record("P9-E1b: not graded. AA p.12 DUAL: the alternate counter reset starts the UHRES pointer chain; the reset count is reconstructed from HHPOSR by interpolation with the measured conversion, no graded bound shows the rounded count is exact, and nothing in the run establishes the memory bandwidth that p.12 makes the condition of the chain giving accesses (DUAL = 0 defines no HHPOSR on p.24, so no contrast exists). The absence of the UHRES chain is graded by P3-E6, P6-E6, P5 and P7.");
        graded(ck, pre9 && o1 && q5, a1,
               "P9-E2: in D1, whose BPLHMOD is 0, each BPLH address is the previous one + 2, for every consecutive pair between t($51) and t($6F) (AA p.15 BPLHMOD 'Its modulo is added every time it comes out'; the pointer advances by 2 after each access)", unmet9 + " / PR-9-U5");
        graded(ck, pre9 && o0, k0 == 30,
               "P9-D0: D0, which repeats D1 with DUAL = 0, has exactly one BPLH access on each of the 30 lines $52 to $6F between t($51) and t($6F) (AA p.15 BPLHPTH,L 'on the 2nd free cycle after the start of each horiz. line')", unmet9);
    }
}

} // namespace s7r
