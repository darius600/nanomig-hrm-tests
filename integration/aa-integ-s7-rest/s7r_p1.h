// s7r_p1.h -- P1, P2 and the joint assertion P1+P2 (REFPTR), SPECIFICATION.md section 5.
#pragma once

#include "s7r_u.h"

namespace s7r {

// The Copper's reads of the list L: word offsets 0, 2, 4, 6, 8, 10 -> their port times and data.
struct CopList {
    int n[6] = {0, 0, 0, 0, 0, 0};
    uint64_t t[6] = {0, 0, 0, 0, 0, 0};
    uint16_t d[6] = {0, 0, 0, 0, 0, 0};
};

inline CopList cop_list(const Cap& c, uint64_t tend) {
    CopList l;
    for (auto& x : c.acc) {
        if (x.w || x.ref || x.a < kL || x.a >= kLEnd || x.t >= tend) continue;
        size_t k = (x.a - kL) / 2;
        if (l.n[k]++ == 0) { l.t[k] = x.t; l.d[k] = x.d; }
    }
    return l;
}

// The Copper fetches list words 0..nw-1 in this order, each exactly once, with the data given.
inline bool fetched(const CopList& l, int nw, const uint16_t* want) {
    for (int k = 0; k < nw; ++k) {
        if (l.n[k] != 1 || l.d[k] != want[k]) return false;
        if (k && l.t[k] <= l.t[k - 1]) return false;
    }
    return true;
}

// Refresh accesses (R2) in [t0, t1]: their word addresses in order, and their times.
struct RSeq { std::vector<uint32_t> a; std::vector<uint64_t> t; };
inline RSeq refresh_seq(const Cap& c, uint64_t t0, uint64_t t1) {
    RSeq s;
    for (auto& x : c.acc) if (x.ref && x.t >= t0 && x.t <= t1) { s.a.push_back(x.a); s.t.push_back(x.t); }
    return s;
}

// The refresh sequences differ in at least one position.
inline bool seq_differs(const RSeq& a, const RSeq& b) {
    size_t n = std::min(a.a.size(), b.a.size());
    for (size_t i = 0; i < n; ++i) if (a.a[i] != b.a[i]) return true;
    return false;
}
inline bool seq_identical(const RSeq& a, const RSeq& b) { return a.a == b.a; }

struct RefRun {
    std::string nm;
    uint16_t da = 0x0028, val = 0;
    Cap cap;
    uint64_t tend = 0;
    CopList cl;
};

inline void measure_ref(RefRun& r) {
    r.tend = end_time(r.cap);
    r.cl = cop_list(r.cap, r.tend ? r.tend : ~0ull);
}

inline void run_refptr(Checks& ck, Shared& sh) {
    const bool kok = sh.hv.conv.ok;
    const double K = kok ? sh.hv.conv.K : 0.0;
    RefRun RA, RB, RE, RNA, RNB, RA0, RB0;
    RA.nm = "RA"; RB.nm = "RB"; RE.nm = "RE"; RNA.nm = "RNA"; RNB.nm = "RNB"; RA0.nm = "RA0"; RB0.nm = "RB0";
    RA.val = 0x5555; RB.val = 0xAAAA; RE.val = 0x5555; RNA.val = 0x5555; RNB.val = 0xAAAA; RA0.val = 0x5555; RB0.val = 0xAAAA;
    RNA.da = RNB.da = 0x01FE;

    // the selectors of the stimulus mutations (SPECIFICATION.md section 5)
    const bool m_p1 = mut("P1") || mut("PR-2a");                 // RB writes the value of RA
    const bool m_ja = mut("P1P2-JOINT-A");                       // RA, RB with COPCON = 0
    const bool m_c2 = mut("P2") || mut("P1P2-JOINT-B");          // RA0, RB0 with COPCON = 2
    const bool m_end = mut("PR-1a");
    const bool m_rn = mut("P1-RN");                              // RNA, RNB move to 028
    if (m_p1) RB.val = 0x5555;
    if (m_rn) RNA.da = RNB.da = 0x0028;
    RA.cap = runbin(mut("FIXTURE-REF") ? "prog_ra_nofix.bin" : m_end ? "prog_ra_end.bin" : m_ja ? "prog_ra_c0.bin" : "prog_ra.bin");
    RB.cap = runbin(m_end ? "prog_rb_end.bin" : m_ja ? "prog_rb_c0.bin" : m_p1 ? "prog_rbm.bin" : mut("PR-1b") ? "prog_rb_nop.bin" : "prog_rb.bin");
    RE.cap = runbin(mut("P1-RE") ? "prog_rb.bin" : "prog_ra.bin");
    RNA.cap = runbin(m_rn ? "prog_ra.bin" : "prog_rna.bin");
    RNB.cap = runbin(m_rn ? "prog_rb.bin" : "prog_rnb.bin");
    RA0.cap = runbin(mut("PR-2b") ? "prog_ra0_end.bin" : m_c2 ? "prog_ra.bin" : "prog_ra0.bin");
    RB0.cap = runbin(mut("PR-2b") ? "prog_rb0_end.bin" : m_c2 ? "prog_rb.bin" : "prog_rb0.bin");
    for (RefRun* r : {&RA, &RB, &RE, &RNA, &RNB, &RA0, &RB0}) measure_ref(*r);

    bool fx = true;
    for (RefRun* r : {&RA, &RB, &RE, &RNA, &RNB, &RA0, &RB0}) fx = fx && fixture_ok(r->cap);
    ck.expect(fx, "FIXTURE-REF: runs RA, RB, RE, RNA, RNB, RA0 and RB0 loaded, booted and stored the constant $BEEF at their fixture address before any trace is read");
    bool xc = sh.ur_ok && sh.anchor;
    for (RefRun* r : {&RA, &RB, &RE, &RNA, &RNB, &RA0, &RB0}) xc = xc && xcheck_cap(r->cap, sh.ur);
    ck.expect(xc, "R2-XCHK-REF: in RA, RB, RE, RNA, RNB, RA0 and RB0, from the end of prologue step 1 to the end marker, the accesses the refresh output marks are exactly the accesses at the port times of UR's non-CPU accesses");

    // PR-1a: the Copper fetches L+0, L+2, L+4, L+6 in this order, once each, with the run's data
    auto want4 = [](const RefRun& r, uint16_t* w) { w[0] = 0x4001; w[1] = 0xFFFE; w[2] = r.da; w[3] = r.val; };
    uint16_t wa[4], wb[4];
    want4(RA, wa); want4(RB, wb);
    bool pr1a = RA.tend && RB.tend && fetched(RA.cl, 4, wa) && fetched(RB.cl, 4, wb);
    ck.expect(pr1a, "PR-1a: in RA and RB the Copper fetches L+0, L+2, L+4 and L+6 in this order, exactly once in the observed frame and before the end marker, with the data $4001, $FFFE, $0028 and the run's value (AA p.20 Copper instruction format)");

    // PR-1b: RA and RB agree up to the fetch of L+6 (data excepted at the value word) and
    // after it in every non-refresh access and in the refresh slots
    std::set<uint32_t> pdiff = bin_diff("prog_ra.bin", "prog_rb.bin");
    bool pr1b = false;
    RSeq sA, sB;
    if (pr1a && RA.cl.t[3] == RB.cl.t[3]) {
        const uint64_t tf = RA.cl.t[3];
        sA = refresh_seq(RA.cap, tf, RA.tend); sB = refresh_seq(RB.cap, tf, RB.tend);
        std::vector<Acc> a0, b0, a1, b1;
        for (auto& x : RA.cap.acc) (x.t < tf ? a0 : a1).push_back(x);
        for (auto& x : RB.cap.acc) (x.t < tf ? b0 : b1).push_back(x);
        bool ok = a0.size() == b0.size();
        for (size_t i = 0; ok && i < a0.size(); ++i) {
            const Acc& x = a0[i]; const Acc& y = b0[i];
            if (x.t != y.t || x.a != y.a || x.w != y.w) ok = false;
            else if (x.d != y.d) {
                bool allowed = (!x.w && pdiff.count(x.a)) || (x.w && x.a == kL + 6);
                if (!allowed) ok = false;
            }
        }
        std::vector<Acc> na, nb;
        for (auto& x : a1) if (!x.ref) na.push_back(x);
        for (auto& x : b1) if (!x.ref) nb.push_back(x);
        ok = ok && na.size() == nb.size();
        for (size_t i = 0; ok && i < na.size(); ++i) if (na[i].t != nb[i].t || na[i].a != nb[i].a) ok = false;
        ok = ok && sA.t == sB.t;
        pr1b = ok;
    }
    ck.expect(pr1b, "PR-1b: up to the Copper's fetch of L+6 the traces of RA and RB agree in time, address and direction for every access and in data except at the value word; after it every non-refresh access and every refresh slot agrees in time and address (R3)");

    // PR-1c: every line of the interval has refresh accesses; the lines are those of run UR, with their starts read from its H fields
    RefLines la, lb;
    const bool urref = sh.ur_ok && sh.ur.an.all && sh.ur.an.lines && sh.ur.an.window && sh.ur.an.mono && !sh.ur.heads.empty();
    if (kok && pr1a && urref) {
        la = refresh_per_line(RA.cap, sh.ur, K, RA.cl.t[3], RA.tend);
        lb = refresh_per_line(RB.cap, sh.ur, K, RB.cl.t[3], RB.tend);
        ck.note("PR-1c: complete lines of UR between the fetch of L+6 and the end marker: RA " + dec(la.lines) + " lines with " + dec(la.minc) + " to " + dec(la.maxc) +
                " refresh accesses per line, RB " + dec(lb.lines) + " lines with " + dec(lb.minc) + " to " + dec(lb.maxc) + " (" + cap_core() + ")");
    }
    bool pr1c = kok && pr1a && urref && RA.cap.done && RB.cap.done && la.ok && lb.ok;
    ck.expect(pr1c, "PR-1c: every complete line from the Copper's fetch of L+6 to the end marker, with its start read from the H field of run UR's markers, has at least one refresh access in RA and in RB (AA p.29: 'There are 4 refresh time slots')");
    if (pr1a) ck.note("P1: refresh accesses between the fetch of L+6 and the end marker: RA " + dec((long long)sA.a.size()) + ", RB " + dec((long long)sB.a.size()) +
                      "; first RA address $" + hex(sA.a.empty() ? 0 : sA.a[0], 6) + " RB $" + hex(sB.a.empty() ? 0 : sB.a[0], 6) + " (" + cap_core() + ")");

    const bool pre1 = xc && pr1a && pr1b && pr1c;
    const bool p1rel = pre1 && seq_differs(sA, sB);
    graded(ck, pre1, p1rel,
           "P1: a Copper MOVE to REFPTR (028) with COPCON bit 1 set changes the refresh addresses that follow: the sequence of refresh-access word addresses of RA (data $5555) differs from that of RB (data $AAAA) in at least one position (AA p.27: 'This register is used as a Dynamic RAM refresh address generator', AA p.6 row 028) opus:AA-refptr.c1 opus:AA-refptr.c2 sol:AA-refptr.c1 sol:AA-refptr.c2",
           "R2-XCHK-REF / PR-1a / PR-1b / PR-1c");

    // contrasts of P1: the same comparator on pairs that the page predicts identical
    {
        RSeq a = refresh_seq(RA.cap, RA.cl.t[3], RA.tend), e = refresh_seq(RE.cap, RE.cl.t[3], RE.tend);
        bool pre = xc && pr1a && RE.cl.n[3] == 1 && RA.cl.n[3] == 1 && a.t.size() == e.t.size();
        graded(ck, pre, seq_identical(a, e),
               "P1-RE: RE, which repeats RA's value $5555, has the same refresh address sequence as RA in every position (a predicted difference of 0)",
               "R2-XCHK-REF / PR-1a for RA / the fetch of L+6 in RE");
        uint16_t wn[4];
        bool pre2 = RNA.tend && RNB.tend && RNA.cl.n[0] == 1 && RNB.cl.n[0] == 1;
        (void)wn;
        uint64_t ta = RNA.cl.n[3] ? RNA.cl.t[3] : 0, tb = RNB.cl.n[3] ? RNB.cl.t[3] : 0;
        RSeq na = refresh_seq(RNA.cap, ta, RNA.tend), nb = refresh_seq(RNB.cap, tb, RNB.tend);
        pre2 = xc && pre2 && ta && tb && na.t.size() == nb.t.size();
        graded(ck, pre2, seq_identical(na, nb),
               "P1-RN: RNA and RNB, whose Copper MOVE goes to 1FE ('NO-OP(NULL)', AA p.10) instead of 028, have the same refresh address sequence in every position (a predicted difference of 0)",
               "R2-XCHK-REF / the Copper's fetch of the MOVE's data word in RNA and RNB");
    }

    // P2
    {
        uint16_t w0[3] = {0x4001, 0xFFFE, 0x0028};
        bool pr2b = RA0.tend && RB0.tend && fetched(RA0.cl, 3, w0) && fetched(RB0.cl, 3, w0);
        ck.expect(pr2b, "PR-2b: in RA0 and RB0 the Copper fetches L+0, L+2 and L+4 in this order, exactly once in the observed frame and before the end marker, with the data $4001, $FFFE and $0028; a fetch of L+6 is neither required nor excluded");
        const bool pr2a = p1rel;
        ck.expect(pr2a, "PR-2a: P1 is green (RA and RB differ), so that a REFPTR without any effect cannot make P2 green without testing the protection");
        RSeq a0 = pr2b ? refresh_seq(RA0.cap, RA0.cl.t[2], RA0.tend) : RSeq();
        RSeq b0 = pr2b ? refresh_seq(RB0.cap, RB0.cl.t[2], RB0.tend) : RSeq();
        const bool comparable = pr2b && a0.t.size() == b0.t.size() && !a0.t.empty();
        graded(ck, xc && pr2a && pr2b && comparable, seq_identical(a0, b0),
               "P2: with COPCON bit 1 clear the Copper cannot write REFPTR: the refresh address sequences of RA0 (data $5555) and RB0 (data $AAAA) are identical in every position from the Copper's fetch of L+4 to the end marker (AA p.6 legend '~=Address not writable by the Coprocessor unless COPCON bit 1 is set true')",
               "R2-XCHK-REF / PR-2a / PR-2b / equal refresh counts");

        // the joint assertion: an effective MOVE with COPCON bit 1 set (PR-2a) is what a MOVE with the bit clear is measured against
        const bool pre = xc && pr1a && pr1b && pr1c && pr2a && pr2b && comparable;
        graded(ck, pre, seq_identical(a0, b0),
               "P1P2-JOINT: where a MOVE to REFPTR with COPCON bit 1 set is effective (PR-2a: the refresh address sequences of RA and RB differ), the refresh address sequences of RA0 (data $5555) and RB0 (data $AAAA), whose MOVE is made with the bit clear, are identical in every position; one MOVE at one beam position, no claim id (AA p.6 row 028 '~' and legend '~=Address not writable by the Coprocessor unless COPCON bit 1 is set true'; AA p.19 '(if 0, access to RGA>7E)')",
               "R2-XCHK-REF / PR-1a / PR-1b / PR-1c / PR-2a / PR-2b / equal refresh counts");
    }
}

} // namespace s7r
