// hrm/appF-cia-address-decode -- gary conformance test
//
// Test purpose: how Appendix F's printed pages map CPU addresses onto the
//   two 8520 CIA chips, as `gary`'s combinational chip-select logic
//   (sel_cia, sel_cia_a, sel_cia_b, gary.v:190-192) actually decodes it --
//   the CIAA/CIAB address-map figures (p.339, p.340) and the "Hardware
//   Connection Details" decode rule (p.353), per SPECIFICATION.md
//   (committed 52d8fd1, before this file was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   339, 340, 353.
//
// ORDER OF WORK: SPECIFICATION.md was written from the 117 claims' own
//   quote/restatement fields (plus what several claims' own
//   classify.evidence fields already disclosed -- several hand over gary.v
//   line numbers and exact stimulus, see that file's own "order-of-work"
//   disclosure) and committed before gary.v was opened for this file. Two
//   corrections were found once gary.v was actually read; both are
//   disclosed in SPECIFICATION.md's header and neither changes any
//   disposition, since the claims' own evidence fields already said the
//   same thing.
//
// BOUNDARY: gary.v, reached at $(NANOMIG)/src/minimig-aga/gary.v. A
//   single, self-contained file -- it instantiates no submodules, so no
//   other file is compiled alongside it (matching the accepted
//   ch1-memory-map-and-two-buses / ch7-custom-address-space /
//   ch7-reset-overlay precedent, the three existing packages that already
//   build gary standalone in this suite).
//
// STRUCTURAL NOTE, load-bearing for every assertion below: gary.v:190-192
//   are plain `assign`s of cpu_address_in alone --
//     sel_cia   = cpu_address_in[23:16] == 8'hBF;
//     sel_cia_a = sel_cia & ~cpu_address_in[12];
//     sel_cia_b = sel_cia & ~cpu_address_in[13];
//   -- with no `dbr`-gated branch at all (unlike sel_chip/sel_slow/
//   sel_kick), so DMA can never select either CIA and dbr's value does not
//   affect any measurement below. cpu_address_in is declared [23:1]
//   (gary.v:51) -- A0 is not present on this port; the byte lane a real
//   68000 cycle would pick via A0 is decided outside gary by cpu_hwr/
//   cpu_lwr (per sol:F-signals.c63's own evidence, minimig.v:614,:639).
//
// CLAIM COVERAGE: all 117 claims in this package's slice of
//   2026-09-17_appF_packages.jsonl are cited exactly where
//   SPECIFICATION.md's claim-accounting table assigns them -- 14 on
//   record() lines citing an accepted sibling package (not re-graded), 28
//   on two uid-bearing expect() sweeps (CIAA-MAP-SWEEP / CIAB-MAP-SWEEP,
//   both PASS), 30+2+30 on record()-only lines (Groups B/C/D, declined --
//   register identity and register absence are not observable at a
//   gary-alone fixture), 4+3 on two further uid-bearing expect()s
//   (CIA-DECODE-DONTCARE / CIA-AB-TWOBIT-RULE, both RED -- honest
//   RTL-vs-manual findings, not adjudicated), 2 on a record() line
//   (Group G, CS/phi2 timing, outside gary's boundary -- no phi2 port), and
//   4 on note()-only lines plus a record() (Group H, NM-HRM-062 -- both
//   readings measured, neither graded).
//
// RESULTS (WSL, Verilator 5.052, built and run 2026-09-17): see out.txt.
//   FIXTURE-LIVENESS, CIAA-MAP-SWEEP and CIAB-MAP-SWEEP are expected GREEN.
//   CIA-DECODE-DONTCARE and CIA-AB-TWOBIT-RULE are expected RED at
//   baseline -- both are honest measurements of gary.v:190 and
//   gary.v:191-192 against an unambiguous half of p.353 (the printed
//   binary-pattern transposition, the OTHER half of p.353, is the
//   separate NM-HRM-062 question and is not graded by either assertion).
//
// MUTATION LOG (5 entries, one per expect() -- every mutation below was
//   actually built and run: `make clean; make MUT=<NAME> test`, one at a
//   time, output read from out.txt, then `make clean; make test` confirmed
//   a clean return to baseline. No oracle/expected value was ever touched
//   by any of them -- every one is a STIMULUS change.
//
// MUTATION FIXTURE-LIVENESS stimulus red: the second probe address was
//   withheld at $000000 instead of moving to $BFE001, so sel_cia never
//   asserts and the liveness check (which requires two DIFFERENT readings)
//   goes red.
// MUTATION CIAA-MAP-SWEEP stimulus red: the sweep's first address was
//   redirected from $BFE101 to $BFD101, CIAB's own space, so sel_cia_a is
//   not asserted at that point and the sweep goes red.
// MUTATION CIAB-MAP-SWEEP stimulus red: the sweep's first address was
//   redirected from $BFD000 to $BFE000, CIAA's own space, so sel_cia_b is
//   not asserted at that point and the sweep goes red.
// MUTATION CIA-DECODE-DONTCARE stimulus green: baseline is RED (a real
//   measurement, not a stuck assertion) -- both "top three bits are 101,
//   the rest don't care" probes ($A00000, $BE0000) were redirected onto
//   the exact-match baseline address ($BFE001), collapsing the suspect
//   half onto a trivially-true case so the whole assertion goes green,
//   proving the baseline RED is a live reading of those two specific
//   addresses and not a coding error.
// MUTATION CIA-AB-TWOBIT-RULE stimulus green: baseline is RED (a real
//   measurement) -- the overlap probe was redirected from $BFC100 to
//   $000000, entirely outside CIA space, where sel_cia_a and sel_cia_b are
//   both trivially 0, so the whole assertion goes green, proving the
//   baseline RED is a live reading of $BFC100 specifically.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vgary.h"

#ifndef MUT_LIVENESS
#define MUT_LIVENESS 0
#endif
#ifndef MUT_CIAAMAP
#define MUT_CIAAMAP 0
#endif
#ifndef MUT_CIABMAP
#define MUT_CIABMAP 0
#endif
#ifndef MUT_DONTCARE
#define MUT_DONTCARE 0
#endif
#ifndef MUT_TWOBIT
#define MUT_TWOBIT 0
#endif

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

static std::string hexaddr(uint32_t byte_addr) {
    char buf[10];
    std::snprintf(buf, sizeof buf, "$%06X", byte_addr & 0xFFFFFFu);
    return std::string(buf);
}
static std::string b(bool v) { return v ? "1" : "0"; }

// cpu_address_in is declared [23:1] -- word-aligned, A0 dropped (see the
// STRUCTURAL NOTE above).
static inline uint32_t waddr(uint32_t byte_addr) { return (byte_addr >> 1) & 0x7FFFFFu; }

struct Harness {
    Vgary *top;
    Harness() {
        top = new Vgary;
        top->clk = 0;
        top->reset = 1;
        top->cpu_address_in = 0;
        top->dma_address_in = 0;
        top->cpu_data_out = 0;
        top->custom_data_out = 0;
        top->ram_data_out = 0;
        top->a1k = 0;
        top->bootrom = 0;
        top->cpu_rd = 1;
        top->cpu_hwr = 0;
        top->cpu_lwr = 0;
        top->cpu_hlt = 0;
        top->ovl = 0;
        top->dbr = 0;
        top->dbwe = 0;
        top->memory_config = 0xF;
        top->ecs = 1;
        top->hdc_ena = 1;
        tick();
        tick();
        top->reset = 0;
        tick();
    }
    ~Harness() { top->final(); delete top; }

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
    void settle() { top->eval(); top->eval(); }

    // A CPU-bus probe: Agnus does not have the bus (dbr=0), and
    // cpu_address_in carries the 680x0 address under test. sel_cia /
    // sel_cia_a / sel_cia_b do not depend on dbr at all (STRUCTURAL NOTE
    // above) -- dbr=0 is set here purely to represent a genuine CPU cycle.
    void cpu_probe(uint32_t byte_addr) {
        top->dbr = 0;
        top->cpu_address_in = waddr(byte_addr);
        settle();
    }
};

int main() {
    Checks checks;

    // =========================================================================
    // FIXTURE-LIVENESS (no uid -- harness precondition, per the build
    // brief's "the fixture proves it can deliver a known value before
    // anything reads one" and the ch1-userio-ports precedent).
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0x000000);  // chip RAM: a known non-CIA point.
        bool ram_point = ((h.top->sel_chip & 0x1) == 1) && (h.top->sel_cia == 0);
        std::string ram_detail = "sel_chip=" + std::to_string((int)h.top->sel_chip) +
                                  " sel_cia=" + b(h.top->sel_cia);

        uint32_t cia_probe = 0xBFE001;
#if MUT_LIVENESS
        cia_probe = 0x000000;  // MUTATION: address change withheld entirely.
#endif
        h.cpu_probe(cia_probe);
        bool cia_point = (h.top->sel_cia == 1) && (h.top->sel_chip == 0);
        std::string cia_detail = "sel_chip=" + std::to_string((int)h.top->sel_chip) +
                                  " sel_cia=" + b(h.top->sel_cia);

        checks.expect(ram_point && cia_point,
            "FIXTURE-LIVENESS (no uid -- harness precondition): the fixture reads two "
            "different, known values for two different stimuli rather than one stuck "
            "value -- $000000 reads as chip RAM only (measured " + ram_detail +
            ") and " + hexaddr(cia_probe) + " reads as the CIA window only (measured " +
            cia_detail + ") -- gary.v:190-192 respond live to cpu_address_in.");
    }

    // =========================================================================
    // CIAA-MAP-SWEEP -- Group A (CIAA), 14 claims graded PASS.
    //
    // Proposition: every one of the appendix's 14 unnoted CIAA address-map
    // rows (p.339, $BFEr01 for r = 1,3,4,5,6,7,8,9,A,B,C,D,E,F -- r=0 and
    // r=2 are declined above, already graded by ch8-ciaapra-address-window)
    // decodes at gary's boundary into sel_cia_a, and never sel_cia_b. This
    // is the full content of each sol:F-ciaa-map.c* "places a register
    // slot at $X" claim -- an address-membership claim, not a claim about
    // which register lives there (gary has no register-select output at
    // all; see SPECIFICATION.md Group B/D). The gap row ($BFEB01, r=B) is
    // included: its address-membership fact is true regardless of whether
    // a register lives there (see Group C in SPECIFICATION.md).
    // =========================================================================
    {
        Harness h;
        std::vector<uint32_t> addrs = {
            0xBFE101, 0xBFE301, 0xBFE401, 0xBFE501, 0xBFE601, 0xBFE701,
            0xBFE801, 0xBFE901, 0xBFEA01, 0xBFEB01, 0xBFEC01, 0xBFED01,
            0xBFEE01, 0xBFEF01
        };
#if MUT_CIAAMAP
        addrs[0] = 0xBFD101;  // MUTATION: first address redirected into CIAB's own space.
#endif
        bool all_a = true, none_b = true;
        std::string detail;
        for (uint32_t a : addrs) {
            h.cpu_probe(a);
            bool sa = h.top->sel_cia_a == 1;
            bool nb = h.top->sel_cia_b == 0;
            all_a = all_a && sa;
            none_b = none_b && nb;
            detail += " " + hexaddr(a) + ":a=" + b(h.top->sel_cia_a) + ",b=" + b(h.top->sel_cia_b);
        }
        checks.expect(all_a && none_b,
            "CIAA-MAP-SWEEP: every one of the appendix's 14 unnoted CIAA address-map "
            "rows ($BFEr01, p.339) selects sel_cia_a and never sel_cia_b (gary.v:190-192) "
            "-- the printed table's CIAA rows each fall inside gary's CIA-A window. "
            "measured:" + detail + ". [claims: sol:F-ciaa-map.c11, sol:F-ciaa-map.c17, "
            "sol:F-ciaa-map.c20, sol:F-ciaa-map.c23, sol:F-ciaa-map.c26, sol:F-ciaa-map.c29, "
            "sol:F-ciaa-map.c32, sol:F-ciaa-map.c35, sol:F-ciaa-map.c38, sol:F-ciaa-map.c41, "
            "sol:F-ciaa-map.c43, sol:F-ciaa-map.c46, sol:F-ciaa-map.c49, sol:F-ciaa-map.c52]");
    }

    // =========================================================================
    // CIAB-MAP-SWEEP -- Group A (CIAB), 14 claims graded PASS. Mirror of
    // CIAA-MAP-SWEEP: r=1 ($BFD100) and r=D ($BFDD00) are declined above,
    // already graded by ch8-cia-disk-port-windows.
    // =========================================================================
    {
        Harness h;
        std::vector<uint32_t> addrs = {
            0xBFD000, 0xBFD200, 0xBFD300, 0xBFD400, 0xBFD500, 0xBFD600,
            0xBFD700, 0xBFD800, 0xBFD900, 0xBFDA00, 0xBFDB00, 0xBFDC00,
            0xBFDE00, 0xBFDF00
        };
#if MUT_CIABMAP
        addrs[0] = 0xBFE000;  // MUTATION: first address redirected into CIAA's own space.
#endif
        bool all_b = true, none_a = true;
        std::string detail;
        for (uint32_t a : addrs) {
            h.cpu_probe(a);
            bool sb = h.top->sel_cia_b == 1;
            bool na = h.top->sel_cia_a == 0;
            all_b = all_b && sb;
            none_a = none_a && na;
            detail += " " + hexaddr(a) + ":a=" + b(h.top->sel_cia_a) + ",b=" + b(h.top->sel_cia_b);
        }
        checks.expect(all_b && none_a,
            "CIAB-MAP-SWEEP: every one of the appendix's 14 unnoted CIAB address-map "
            "rows ($BFDr00, p.340) selects sel_cia_b and never sel_cia_a (gary.v:190-192) "
            "-- the printed table's CIAB rows each fall inside gary's CIA-B window. "
            "measured:" + detail + ". [claims: sol:F-ciab-map.c1, sol:F-ciab-map.c21, "
            "sol:F-ciab-map.c24, sol:F-ciab-map.c27, sol:F-ciab-map.c30, sol:F-ciab-map.c33, "
            "sol:F-ciab-map.c36, sol:F-ciab-map.c39, sol:F-ciab-map.c42, sol:F-ciab-map.c45, "
            "sol:F-ciab-map.c48, sol:F-ciab-map.c50, sol:F-ciab-map.c56, sol:F-ciab-map.c59]");
    }

    // =========================================================================
    // Group B -- CIA register identity, 30 claims declined. No expect():
    // these claims carry only a register NAME, no address-membership
    // content -- gary has no register-select or register-name output at
    // all (sel_cia_a/sel_cia_b say which CHIP, never which of the 16
    // registers). Register identity is ciaa.v's own register-bank decode,
    // a different top module (appF-cia-register-map's own territory).
    // =========================================================================
    checks.record(
        "Group B (CIAA register names, p.339, 15 claims) -- NOT gradeable at gary: "
        "gary.v has no register-select decode and no register-name output; only which "
        "CHIP an address belongs to (sel_cia_a/sel_cia_b) is observable here, never "
        "which of the 16 internal registers. Register identity is ciaa.v's own "
        "register-bank decode, a different top module, appF-cia-register-map's own "
        "territory. sol:F-ciaa-map.c2, sol:F-ciaa-map.c12, sol:F-ciaa-map.c15, "
        "sol:F-ciaa-map.c18, sol:F-ciaa-map.c21, sol:F-ciaa-map.c24, sol:F-ciaa-map.c27, "
        "sol:F-ciaa-map.c30, sol:F-ciaa-map.c33, sol:F-ciaa-map.c36, sol:F-ciaa-map.c39, "
        "sol:F-ciaa-map.c44, sol:F-ciaa-map.c47, sol:F-ciaa-map.c50, sol:F-ciaa-map.c53.");
    checks.record(
        "Group B (CIAB register names, p.340, 15 claims) -- same reason as the CIAA "
        "group immediately above: not gradeable at a gary-alone fixture. "
        "sol:F-ciab-map.c2, sol:F-ciab-map.c12, sol:F-ciab-map.c22, sol:F-ciab-map.c25, "
        "sol:F-ciab-map.c28, sol:F-ciab-map.c31, sol:F-ciab-map.c34, sol:F-ciab-map.c37, "
        "sol:F-ciab-map.c40, sol:F-ciab-map.c43, sol:F-ciab-map.c46, sol:F-ciab-map.c51, "
        "sol:F-ciab-map.c54, sol:F-ciab-map.c57, sol:F-ciab-map.c60.");

    // =========================================================================
    // Group C -- map gap rows, 2 claims declined. Measured directly: the
    // sweeps above already probe both gap addresses ($BFEB01 is in
    // CIAA-MAP-SWEEP, $BFDB00 is in CIAB-MAP-SWEEP) and gary asserts the
    // matching chip-select there exactly as it does at every other row --
    // it has no signal that distinguishes a defined register slot from an
    // undefined one, so the absence of a register cannot be observed here.
    // =========================================================================
    checks.record(
        "Group C (map gap rows, 2 claims) -- NOT gradeable at gary: CIAA-MAP-SWEEP "
        "above already shows sel_cia_a=1 at $BFEB01 exactly as at every other CIAA row, "
        "and CIAB-MAP-SWEEP shows sel_cia_b=1 at $BFDB00 exactly as at every other CIAB "
        "row -- gary has no signal that would show either address is the appendix's "
        "'not used' slot rather than an ordinary one; that fact is internal to ciaa's "
        "own register decode, outside this package's boundary. "
        "opus:F-ciaa-map.c31, opus:F-ciab-map.c31.");

    // =========================================================================
    // Group D -- combined address+name rows, 30 claims declined in part
    // (Rule 1: a uid on a graded line counts ASSERTED, so a partially-
    // gradeable claim's uid sits on record() alone). The address-
    // membership half of each is the identical fact CIAA-MAP-SWEEP /
    // CIAB-MAP-SWEEP measure at the same address (13 of 15 rows per page)
    // or the identical fact ch8-ciaapra-address-window / ch8-cia-disk-
    // port-windows measure (the other 2 rows per page: $BFE001/$BFE201 for
    // CIAA, $BFD100/$BFDD00 for CIAB -- the same four addresses declined
    // in the "already graded elsewhere" group below). No separate
    // bare-label expect() re-measures the same signals at the same
    // addresses a second time; the existing measurement is named directly
    // below. The register-name half is not gradeable, same reason as
    // Group B.
    // =========================================================================
    checks.record(
        "Group D (CIAA combined rows, p.339, 15 claims, PARTIAL per Rule 1) -- each "
        "opus:F-ciaa-map.c* claim states BOTH 'a slot exists at $X' AND 'it is named Y'. "
        "The address-membership half is graded: for $BFE101/$BFE301/$BFE401/$BFE501/"
        "$BFE601/$BFE701/$BFE801/$BFE901/$BFEA01/$BFEC01/$BFED01/$BFEE01/$BFEF01, by "
        "CIAA-MAP-SWEEP above (same fixture, same addresses, same signal); for $BFE001 "
        "and $BFE201, by ch8-ciaapra-address-window (see the 'already graded elsewhere' "
        "record below). The register-name half is not gradeable at gary (Group B "
        "reason) for any of the 15. opus:F-ciaa-map.c1, opus:F-ciaa-map.c3, "
        "opus:F-ciaa-map.c5, opus:F-ciaa-map.c7, opus:F-ciaa-map.c9, opus:F-ciaa-map.c11, "
        "opus:F-ciaa-map.c13, opus:F-ciaa-map.c15, opus:F-ciaa-map.c17, opus:F-ciaa-map.c19, "
        "opus:F-ciaa-map.c21, opus:F-ciaa-map.c23, opus:F-ciaa-map.c25, opus:F-ciaa-map.c27, "
        "opus:F-ciaa-map.c29.");
    checks.record(
        "Group D (CIAB combined rows, p.340, 15 claims, PARTIAL per Rule 1) -- mirror of "
        "the CIAA group immediately above. Address-membership half graded: for "
        "$BFD000/$BFD200/$BFD300/$BFD400/$BFD500/$BFD600/$BFD700/$BFD800/$BFD900/$BFDA00/"
        "$BFDC00/$BFDE00/$BFDF00, by CIAB-MAP-SWEEP above; for $BFD100 and $BFDD00, by "
        "ch8-cia-disk-port-windows (see the 'already graded elsewhere' record below). "
        "Register-name half not gradeable at gary for any of the 15. opus:F-ciab-map.c1, "
        "opus:F-ciab-map.c3, opus:F-ciab-map.c5, opus:F-ciab-map.c7, opus:F-ciab-map.c9, "
        "opus:F-ciab-map.c11, opus:F-ciab-map.c13, opus:F-ciab-map.c15, opus:F-ciab-map.c17, "
        "opus:F-ciab-map.c19, opus:F-ciab-map.c21, opus:F-ciab-map.c23, opus:F-ciab-map.c25, "
        "opus:F-ciab-map.c27, opus:F-ciab-map.c29.");

    // =========================================================================
    // CIA-DECODE-DONTCARE -- Group E, 4 claims graded. This measures the
    // outer build brief's own second named item: "gary.v:190 compares all
    // eight bits of A23-A16 against $BF where p.353 decodes three and
    // calls the rest don't cares."
    //
    // Proposition: p.353 states the CIA select rests on three fixed bits
    // (A23-A21=101) plus a which-chip field (A13/A12, graded separately
    // under CIA-AB-TWOBIT-RULE) plus a register-select field (A11-A8);
    // every other bit -- including A20 and A19-A16, the five bits
    // gary.v:190's 8-bit compare adds beyond the top three -- is a
    // don't-care. Measured: it is not. Control, same measurement basis:
    // the manual's don't-care claim DOES hold, in gary, for every bit
    // below A12 (register-select plus the genuinely free low byte) --
    // gary.v:190 never examines those bits at all for sel_cia.
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xBFE001);  // exact match: $BF, the only byte gary.v:190 accepts.
        bool baseline = h.top->sel_cia == 1;

        uint32_t dontcare1 = 0xA00000;  // top three bits 101 (A=1010), A20/A19-16 differ from $BF.
        uint32_t dontcare2 = 0xBE0000;  // top three bits 101, differs from $BF by one bit (A16).
#if MUT_DONTCARE
        dontcare1 = 0xBFE001;  // MUTATION: both don't-care probes collapsed onto the exact-match baseline.
        dontcare2 = 0xBFE001;
#endif
        h.cpu_probe(dontcare1);
        bool topbits_a0 = h.top->sel_cia == 1;
        std::string d1 = hexaddr(dontcare1) + ":sel_cia=" + b(h.top->sel_cia);
        h.cpu_probe(dontcare2);
        bool topbits_be = h.top->sel_cia == 1;
        std::string d2 = hexaddr(dontcare2) + ":sel_cia=" + b(h.top->sel_cia);

        // Control: bits 11-0 (register-select plus the genuinely free low
        // byte) really are don't-cares in gary -- flip all twelve of them
        // and sel_cia_a is unaffected.
        h.cpu_probe(0xBFE001);
        bool ctrl_low = h.top->sel_cia_a == 1;
        h.cpu_probe(0xBFEFFF);
        bool ctrl_high = h.top->sel_cia_a == 1;

        checks.expect(baseline && topbits_a0 && topbits_be && ctrl_low && ctrl_high,
            "CIA-DECODE-DONTCARE: gary.v:190 is `sel_cia = cpu_address_in[23:16] == "
            "8'hBF` -- an 8-bit compare, not a 3-bit one. Baseline $BFE001 (exact match) "
            "selects (measured sel_cia=" + b(baseline) + "). Two addresses whose top three bits " +
            "are 101 but whose other five (A20,A19-A16) differ from $BF's own do NOT "
            "select, where p.353's 'all other bits are don't cares' (following 'the "
            "upper three address bits are 101') says they should: measured " + d1 + ", " +
            d2 + ". CONTROL, same measurement basis: bits 11-0 (register-select plus the "
            "genuinely free low byte) really are don't-cares in gary -- $BFE001 and "
            "$BFEFFF (all twelve low bits flipped) both select CIA-A (measured ctrl_low=" +
            b(ctrl_low) + " ctrl_high=" + b(ctrl_high) + "). Reported per the outer build "
            "brief's 'measure it, report what you find, adjudicate nothing' instruction "
            "for gary.v:190 -- this is a different question from NM-HRM-062's printed-"
            "pattern transposition; p.353's 'upper three bits are 101' / 'all other bits "
            "are don't cares' sentences have only one reading each. "
            "[claims: sol:F-signals.c56, sol:F-signals.c63, opus:F-conn.c1, opus:F-conn.c6]");
    }

    // =========================================================================
    // CIA-AB-TWOBIT-RULE -- Group F, 3 claims graded. This measures the
    // outer build brief's own first named item: "gary.v:191-192 decodes
    // each CIA on ONE address bit where p.353 requires two, so $BFC100
    // raises sel_cia_a and sel_cia_b together."
    //
    // Proposition: p.353 states a two-bit rule per chip (both A12 AND A13
    // must hold for that chip, and -- read as the selection rule it is --
    // implicitly neither chip otherwise). gary.v:191 reads A12 alone for
    // sel_cia_a; gary.v:192 reads A13 alone for sel_cia_b. The positive
    // halves hold at the prose's own worked points ($BFE001, $BFD000).
    // The implied negative half does not: at $BFC100 (A12=0, A13=0,
    // satisfying NEITHER chip's full two-bit condition) both selects
    // assert together. ch8-cia-disk-port-windows' own
    // OVERLAP-BOTH-SELECTS-ACTIVE assertion already measures this same
    // $BFC100 fact but its own label/comment read "mechanism fact, not a
    // manual citation" / "no page claims or denies this" -- this is the
    // first assertion in this suite to grade it against p.353 itself (see
    // SPECIFICATION.md "Grep check").
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xBFE001);  // prose's and the chosen-address's own CIAA point (A12=0,A13=1).
        bool ciaa_point = h.top->sel_cia_a == 1;
        h.cpu_probe(0xBFD000);  // prose's and the chosen-address's own CIAB point (A12=1,A13=0).
        bool ciab_point = h.top->sel_cia_b == 1;

        uint32_t overlap_probe = 0xBFC100;  // A12=0,A13=0: satisfies neither chip's full rule.
#if MUT_TWOBIT
        overlap_probe = 0x000000;  // MUTATION: probe moved outside CIA space entirely.
#endif
        h.cpu_probe(overlap_probe);
        bool neither = (h.top->sel_cia_a == 0) && (h.top->sel_cia_b == 0);
        checks.note("CIA-AB-TWOBIT-RULE-OVERLAP: at " + hexaddr(overlap_probe) +
            " sel_cia_a=" + b(h.top->sel_cia_a) + " sel_cia_b=" + b(h.top->sel_cia_b));

        checks.expect(ciaa_point && ciab_point && neither,
            "CIA-AB-TWOBIT-RULE: p.353's rule is two-bit per chip -- gary.v:191 reads A12 "
            "alone (sel_cia_a), gary.v:192 reads A13 alone (sel_cia_b). Positive halves "
            "hold: $BFE001 (A12=0,A13=1) selects CIA-A (measured " + b(ciaa_point) +
            ") and $BFD000 (A12=1,A13=0) selects CIA-B (measured " + b(ciab_point) +
            "). The implied negative half does not: " + hexaddr(overlap_probe) +
            " (A12=0,A13=0) should select NEITHER chip per the prose's own two-bit rule, "
            "but both sel_cia_a and sel_cia_b are asserted together (measured neither-"
            "selected=" + b(neither) + ", i.e. both ARE selected) -- gary.v:191-192 each "
            "test only one of the two bits p.353 requires. Reported per the outer build "
            "brief's 'measure it, report what you find, adjudicate nothing' instruction; "
            "ch8-cia-disk-port-windows' OVERLAP-BOTH-SELECTS-ACTIVE already measures this "
            "same address but explicitly declines to cite it as a manual fact -- this is "
            "the first grade of that fact against p.353 itself, not a re-grade. "
            "[claims: sol:F-signals.c57, sol:F-signals.c58, opus:F-conn.c2]");
    }

    // =========================================================================
    // Group G -- CS/phi2 timing, 2 claims declined. gary's port list
    // carries no clock-phase/E-clock/phi2 input at all, and gary.v:190-192
    // derive their outputs combinationally from cpu_address_in alone -- no
    // clock term appears in any of the three lines.
    // =========================================================================
    checks.record(
        "Group G (CS/phi2 timing, p.353, 2 claims) -- NOT gradeable at gary: gary's port "
        "list (gary.v:49-108) carries no clock-phase, E-clock or phi2 input of any kind, "
        "and gary.v:190-192 (sel_cia, sel_cia_a, sel_cia_b) are combinational functions "
        "of cpu_address_in alone -- no clock term appears in any of the three lines. Per "
        "both claims' own classify.evidence, the phasing that does exist sits one module "
        "away, at minimig_m68k_bridge.v:131 (_dtack withheld until eclk under vpa), a "
        "different top module outside this package's boundary. "
        "sol:F-signals.c76, opus:F-signals.c8.");

    // =========================================================================
    // NM-HRM-062 -- Group H, 4 claims. "Measure both readings, record
    // both, grade neither." No expect() for these four uids -- note()
    // only, graded by nothing, plus one record() per pair.
    // =========================================================================
    {
        Harness h;
        // The prose reading (matches the page's own chosen addresses, and
        // per NM-HRM-062 all four surveyed emulators).
        h.cpu_probe(0xBFE001);
        checks.note("NM-HRM-062-PROSE-CIAA: $BFE001 (A13=1,A12=0, the prose's and the "
            "page's own chosen CIAA address) -> sel_cia_a=" + b(h.top->sel_cia_a) +
            " sel_cia_b=" + b(h.top->sel_cia_b));
        h.cpu_probe(0xBFD000);
        checks.note("NM-HRM-062-PROSE-CIAB: $BFD000 (A13=0,A12=1, the prose's and the "
            "page's own chosen CIAB address) -> sel_cia_a=" + b(h.top->sel_cia_a) +
            " sel_cia_b=" + b(h.top->sel_cia_b));

        // The printed-pattern reading: an address carrying the printed
        // CIAA pattern's own A13/A12 bits (A13=0,A12=1 -- the SAME bits as
        // the prose's CIAB point).
        h.cpu_probe(0xBFD001);
        checks.note("NM-HRM-062-PRINTED-CIAA-PATTERN: an address carrying the printed "
            "CIAA binary pattern's own A13=0,A12=1 (measured at $BFD001) -> sel_cia_a=" +
            b(h.top->sel_cia_a) + " sel_cia_b=" + b(h.top->sel_cia_b) +
            " -- decodes as CIA-B, not CIA-A");
        // The printed CIAB pattern's own bits (A13=1,A12=0 -- the SAME
        // bits as the prose's CIAA point, e.g. $BFE001 itself).
        h.cpu_probe(0xBFE001);
        checks.note("NM-HRM-062-PRINTED-CIAB-PATTERN: an address carrying the printed "
            "CIAB binary pattern's own A13=1,A12=0 (measured at $BFE001) -> sel_cia_a=" +
            b(h.top->sel_cia_a) + " sel_cia_b=" + b(h.top->sel_cia_b) +
            " -- decodes as CIA-A, not CIA-B");
        checks.note("NM-HRM-062-A0: cpu_address_in is declared [23:1] (gary.v:51) -- A0 "
            "is not present on this port at all, so the printed patterns' A0 half cannot "
            "be driven or observed as an address bit at this boundary; reported, not "
            "measured.");

        checks.record(
            "NM-HRM-062 (p.353): the printed CIAA/CIAB binary selection patterns are "
            "transposed relative to the page's own prose and the page's own chosen "
            "addresses (docs/findings/NM-HRM-062.md, this tests repo, full four-emulator "
            "survey). Both readings measured above via note() -- gary follows the prose "
            "reading; the printed-pattern reading decodes each chip's own address as the "
            "OTHER chip. Per the outer build brief: 'a test built from the printed "
            "patterns would red a correct decoder.' Neither reading is graded here. "
            "sol:F-signals.c64, sol:F-signals.c65, opus:F-conn.c7, opus:F-conn.c8.");
    }

    // =========================================================================
    // Already graded elsewhere -- 14 claims, record() only, not re-graded
    // (per the build brief, unconditional for this group).
    // =========================================================================
    checks.record(
        "Already graded elsewhere (p.339, 2 claims): ch8-ciaapra-address-window already "
        "grades $BFE001 and $BFE201 as printed-page address-map facts (labels A1-A4, "
        "test_ch8-ciaapra-address-window.cpp:390,414,432,450, uids opus:8-regs.c5, "
        "sol:8-reg.c5, opus:8-mbtn.c4, sol:8-button.c3). Not re-graded. "
        "sol:F-ciaa-map.c1, sol:F-ciaa-map.c14.");
    checks.record(
        "Already graded elsewhere (p.340, 3 claims): ch8-cia-disk-port-windows already "
        "grades $BFD100 and $BFDD00 as address-map/window facts (labels "
        "CIABPRB-ALIAS-OPUS-CIA-C5 / CIABPRB-CHIPLETTER-SOL-CIA-C4, "
        "test_ch8-cia-disk-port-windows.cpp:553,574, uids opus:8-cia.c5, sol:8-cia.c4). "
        "Not re-graded. sol:F-ciab-map.c11, sol:F-ciab-map.c53, sol:F-chip-map.c2.");
    checks.record(
        "Already graded elsewhere (p.351, 6 claims): related to the CIA-B window "
        "ch8-cia-disk-port-windows grades (CIABPRB-ALIAS-OPUS-CIA-C5 / "
        "CIABPRB-CHIPLETTER-SOL-CIA-C4, opus:8-cia.c5 / sol:8-cia.c4) and its "
        "OVERLAP-BOTH-SELECTS-ACTIVE mechanism measurement; ch8-ciaapra-address-window "
        "covers the $BFE001 side (opus:8-regs.c5, sol:8-reg.c5, opus:8-mbtn.c4, "
        "sol:8-button.c3). Not re-graded. sol:F-signals.c8, sol:F-signals.c10, "
        "sol:F-signals.c32, sol:F-signals.c34, opus:F-portsig.c7, opus:F-portsig.c28.");
    checks.record(
        "Already graded elsewhere (p.353, 3 claims, the 'chosen addresses' sentence): "
        "ch8-ciaapra-address-window covers the CIAA r-nibble family ($BFEr01) and "
        "ch8-cia-disk-port-windows covers the CIAB r-nibble family ($BFDr00), both per "
        "their own classify.note. Not re-graded. sol:F-signals.c66, sol:F-signals.c67, "
        "opus:F-conn.c9.");

    // stdout, not stderr: `> out.txt` captures only stdout, so a summary on
    // cerr reaches the runner (which reads make's output) and is MISSING from
    // this package's own out.txt and from its stored capture.
    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
