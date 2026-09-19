// ch7-reset-overlay -- gary conformance test
//
// Test purpose: what the reset overlay does to gary's address decode -- does
//   Chip memory reach the CPU via DMA-style direct substitution rather than
//   a bus-arbitration handshake; what answers at $00000000 with the overlay
//   active versus released; and whether "all memory ... disappear[s]" while
//   the overlay is active, taken literally, across all four 512KB chip
//   banks.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 7,
//   printed p.223.
// Non-goals: gary carries what the overlay DOES to the address map and holds
//   no overlay state of its own -- `ovl` is a plain input here. Where the
//   overlay bit comes FROM (power-on/external reset vs. the 68000 RESET
//   instruction) is the sibling package ch7-overlay-source, fixture
//   `minimig`; not reached here (see the OVERLAY-SOURCE-NOT-IN-SCOPE record
//   near the end). The `cpu_hlt` exception folded into gary.v's sel_chip[0]/
//   sel_kick terms (read at gary.v:159,166 below) is not named by any of
//   this package's 9 claims and is not asserted (see CPU-HLT-NOT-IN-SCOPE).
// Claim ids: 10 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM SOURCE: hrm-testpipeline/work/ch7-build/
//   ch7-reset-overlay.jsonl -- 9 claim records, all classified "testable" at
//   gary (the pre-RTL spec stage used ch7-reset-overlay-spec-stage.jsonl,
//   the same 9 records with ports/signals/evidence withheld).
//
// BOUNDARY: gary.v, reached at $(NANOMIG)/src/minimig-aga/gary.v. A
//   self-contained file -- it instantiates no submodules, so no other file
//   is compiled alongside it.
//
// ORDER OF WORK: SPECIFICATION.md (this package's own) was written from the
//   9 claims' quote/restatement fields alone and committed (5e61135) before
//   gary.v was opened. This file was written after that commit, with gary.v
//   open. PROP-A..PROP-E below are exactly the five propositions the
//   specification named; nothing here was widened or narrowed once the RTL
//   was visible.
//
// ON THE STEP-3 NOTE'S OWN MEASUREMENT: STEP3-NOTES-ch7-reset-overlay.md
//   flagged that "all memory ... disappear[s]" does not hold in this RTL,
//   because sel_chip[1..3] carry no `ovl` term (only sel_chip[0] does) --
//   reading $080000 while the overlay is active still selects chip RAM. Per
//   the lead's instruction that note is reproduced independently below
//   (RESET-ALL-MEMORY-DISAPPEARS-SWEEP), not adopted as this file's expected
//   value; this build's own measurement is what the RESULTS section reports.
//
// CONFIGURATION CHOSEN, AND WHY: memory_config=0, ecs=0, hdc_ena=0, a1k=0,
//   bootrom=0, cpu_hlt=0 for every probe in this file. None of gary's
//   slow-RAM ($C0/$C8/$D0), IDE/GAYLE, or 256K-mirror decode overlaps any
//   proposition this package tests -- every probe address here is either a
//   chip-memory bank ($000000-$1FFFFE) or the kickstart range ($F80000-
//   $FFFFFF) -- so those inputs are held at their most inert values rather
//   than tuned per-probe. bootrom=0 additionally keeps sel_kick256kmirror
//   structurally 0 regardless of rom_readonly's clocked state (gary.v:92,
//   167), so it never confounds a sel_kick reading. cpu_hlt=0 throughout is
//   a deliberate exclusion, not an oversight -- see CPU-HLT-NOT-IN-SCOPE.
//
// WHY NO DATA-DELIVERY PROOF: every assertion in this file reads a SELECT
//   line (sel_chip, sel_kick, sel_kick256kmirror) or the multiplexed
//   ram_address_out register, each a continuous function of the address,
//   dbr and ovl inputs alone in gary.v's `assign`/`always @(*)` blocks (read
//   directly at gary.v:143-169,131 while writing this file) -- no clocked
//   state and no data bus sits in any of their cones. There is no data word
//   to fail to deliver. The MUTATION on every assertion below is what proves
//   each one reads a live signal rather than a stuck value, the same
//   guarantee BUILD-BRIEF.md's data-delivery rule exists to give a
//   data-path assertion.
//
// CLAIM COVERAGE -- all 9 ids, grouped by the proposition each serves
//   (SPECIFICATION.md's own PROP-A..PROP-E):
//
//   PROP-A / GARY-DMA-DIRECT-CHIP-ACCESS (2): sol:7-reset.c1,
//     opus:7-dma.c23 (twin). Negative half recorded, not asserted --
//     BUS-ARBITRATION-ABSENT.
//   PROP-B / OVERLAY-ROM-AT-000000 (2): sol:7-reset.c4, opus:7-reset.c1
//     (twin, ROM-half only -- its other half belongs to sol:7-reset.c3 at
//     fixture minimig, out of scope, not in this package's claims file).
//   PROP-C / OVERLAY-RAM-UNAVAILABLE-WRITE-000000 (2): sol:7-reset.c5,
//     opus:7-reset.c2 (twin).
//   PROP-D / RESET-ALL-MEMORY-DISAPPEARS-SWEEP (1): sol:7-reset.c13.
//     AUTOCONFIG half recorded, not asserted -- AUTOCONFIG-HALF-NOT-ASSERTED.
//   PROP-E / RESET-ROM-REAPPEARS-000000 (2): sol:7-reset.c14,
//     opus:7-reset.c10 (twin, ROM-half only -- its memory-disappear half is
//     credited to PROP-D via sol:7-reset.c13 instead, per this record's own
//     `split` field and SPECIFICATION.md's own note).
//
//   2+2+2+1+2 = 9. All 9 ids named above; every one is cited inside a
//   checks.expect() string.
//
// CONTROL / PRECONDITION: OVL-CONTROL-BASELINE-000000 carries no claim id.
//   It measures the same address ($000000) the same way (sel_chip[0],
//   sel_kick) with the overlay OFF, and is required reading before PROP-B
//   and PROP-C: without it, a fixture that answered nothing at all at
//   $000000 under any ovl value would pass both "ROM answers" and "RAM is
//   unavailable" alike. It doubles as the "before" state PROP-D and PROP-E's
//   transition presupposes (memory present, ROM absent, prior to the
//   overlay) -- see BUILD-BRIEF.md's precondition rule.
//
// RESULTS (WSL/Verilator 5.052 build, default flags, 2026-09-13): 6 passed,
//   1 failed. The one FAIL is RESET-ALL-MEMORY-DISAPPEARS-SWEEP (PROP-D) --
//   sel_chip($080000)=2, sel_chip($100000)=4, sel_chip($180000)=8 with
//   ovl=1, reproducing STEP3-NOTES-ch7-reset-overlay.md's flagged
//   measurement independently on this build: banks 1-3 stay mapped, so "All
//   memory ... disappear" does not hold in this RTL. All other 6 assertions
//   PASS. `make`'s own exit status is nonzero because the test binary
//   returns checks.failed!=0 -- a valid, finished outcome per
//   BUILD-BRIEF.md's oracle rule, not a build failure.
//
// MUTATION LOG. Each rebuilt with `verilator ... -CFLAGS -D<FLAG>=1 ...
//   gary.v test_gary.cpp`, run, then reverted -- see RESULTS above for the
//   default-build baseline every line below is measured against.
//   Machine-readable records for tools/check_gates.py --tests, one per
//   assertion, id first (exactly the label passed to expect()):
//
// MUTATION GARY-DMA-DIRECT-CHIP-ACCESS stimulus red: MUT_A1 forces dbr=0
//   (Agnus does not have the bus); sel_chip/ram_address_out then track
//   cpu_address_in instead of dma_address_in and sel_kick raises ->
//   GARY-DMA-DIRECT-CHIP-ACCESS FAIL (sel_chip=0, sel_kick=1,
//   ram_address_out=$7C0000); every other assertion unchanged.
// MUTATION OVL-CONTROL-BASELINE-000000 stimulus red: MUT_CTRL0 forces ovl=1
//   (the overlay state this control exists to rule out) ->
//   OVL-CONTROL-BASELINE-000000 FAIL (sel_chip=0, sel_kick=1); every other
//   assertion unchanged.
// MUTATION OVERLAY-ROM-AT-000000 stimulus red: MUT_B1 forces ovl=0 (overlay
//   released) -> OVERLAY-ROM-AT-000000 FAIL (sel_kick=0); every other
//   assertion unchanged.
// MUTATION OVERLAY-RAM-UNAVAILABLE-WRITE-000000 stimulus red: MUT_C1 forces
//   ovl=0 -> OVERLAY-RAM-UNAVAILABLE-WRITE-000000 FAIL (the write now lands
//   in RAM, sel_chip=1); every other assertion unchanged.
// MUTATION RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY stimulus red:
//   MUT_D2 forces ovl=0 -> RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY
//   FAIL (all four bank-0 points read sel_chip=1 instead of 0); every other
//   assertion unchanged.
// MUTATION RESET-ALL-MEMORY-DISAPPEARS-SWEEP stimulus green: this
//   assertion's baseline is already RED (a genuine finding -- banks 1-3
//   stay mapped under ovl=1, see RESULTS above and its own MEASURED note),
//   so the shape that proves it is live runs the other way from every
//   other record in this file: MUT_D1 confines all four sample points to
//   bank 0 (the one bank the manual's claim actually holds for in this
//   RTL) instead of one per bank -> RESET-ALL-MEMORY-DISAPPEARS-SWEEP PASS
//   (sel_chip=0 at all four); every other assertion unchanged. FAIL->PASS
//   here is correct, not backwards: it proves the baseline red comes from
//   banks 1-3's own address decode (gary.v:160-162 carry no `ovl` term),
//   not from a stuck harness -- the always-green RESET-ALL-MEMORY-
//   DISAPPEARS-CONTROL-BANK0-ONLY above makes the same point by
//   construction, on the same measurement basis.
// MUTATION RESET-ROM-REAPPEARS-000000 stimulus red: MUT_E1 forces ovl=0 ->
//   RESET-ROM-REAPPEARS-000000 FAIL (sel_kick=0, sel_chip=1); every other
//   assertion unchanged.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vgary.h"

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
static std::string hexreg(uint32_t v) {
    char buf[10];
    std::snprintf(buf, sizeof buf, "$%06X", v & 0xFFFFFFu);
    return std::string(buf);
}
static std::string hexbits(uint32_t v) {
    char buf[6];
    std::snprintf(buf, sizeof buf, "%X", v & 0xFu);
    return std::string(buf);
}

// cpu_address_in is declared [23:1]; dma_address_in is declared [20:1].
// Both buses drop bit 0 (registers are word-aligned), so the value placed
// on the port is the byte address shifted right by one.
static inline uint32_t waddr(uint32_t byte_addr)  { return (byte_addr >> 1) & 0x7FFFFFu; }
static inline uint32_t dwaddr(uint32_t byte_addr) { return (byte_addr >> 1) & 0xFFFFFu; }

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
        top->memory_config = 0x0;  // see header, "CONFIGURATION CHOSEN".
        top->ecs = 0;
        top->hdc_ena = 0;
        tick();
        tick();
        top->reset = 0;
        tick();
    }
    ~Harness() { top->final(); delete top; }

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
    void settle() { top->eval(); top->eval(); }
};

int main() {
    Checks checks;

    // ================= PROP-A =================
    // ---------------- GARY-DMA-DIRECT-CHIP-ACCESS ----------------
    {
        Harness h;
        h.top->dma_address_in = dwaddr(0x080000);   // bank 1 ($080000-$0FFFFF)
        h.top->cpu_address_in = waddr(0xF80000);    // deliberately conflicting: kickstart ROM range
        h.top->cpu_rd = 1;
        h.top->dbwe = 0;                            // a DMA read cycle
        h.top->dbr =
#if MUT_A1
            0;  // MUTATION: Agnus does NOT have the bus -- selection must now track cpu_address_in instead of dma_address_in.
#else
            1;
#endif
        h.top->eval();
        bool sel_follows_dma = (h.top->sel_chip == 0x2) &&
                               (h.top->ram_address_out == h.top->dma_address_in);
        bool no_kick = (h.top->sel_kick == 0) && (h.top->sel_kick256kmirror == 0);
        checks.expect(sel_follows_dma && no_kick,
            "GARY-DMA-DIRECT-CHIP-ACCESS: with dbr=1 (Agnus owns the bus), "
            "driving dma_address_in at $080000 while cpu_address_in "
            "simultaneously carries a conflicting kickstart-ROM address "
            "($F80000, as if waiting there for a CPU cycle) still selects "
            "sel_chip=bank-1 and multiplexes ram_address_out from "
            "dma_address_in alone, and never raises sel_kick or "
            "sel_kick256kmirror -- gary.v's dbr-branch (gary.v:145-155) "
            "decodes sel_chip from dma_address_in alone and forces "
            "sel_kick/sel_kick256kmirror to 0 (gary.v:154-155), with no "
            "separate request/grant/acknowledge signal gating any of it "
            "(gary.v:131 multiplexes ram_address_out the same way). 'The "
            "Amiga chips access Chip memory directly via DMA, rather than "
            "utilizing traditional bus arbitration mechanisms.' (p.223, "
            "sol:7-reset.c1, opus:7-dma.c23 -- positive/DMA half only; see "
            "the BUS-ARBITRATION-ABSENT record below for the negative "
            "half). measured sel_chip=" + hexbits(h.top->sel_chip) +
            " ram_address_out=" + hexreg(h.top->ram_address_out) +
            " sel_kick=" + std::to_string((int)h.top->sel_kick) +
            " dbr=" + std::to_string((int)h.top->dbr) + ".");
    }
    checks.record(
        "BUS-ARBITRATION-ABSENT: the negative half of 'rather than "
        "utilizing traditional bus arbitration mechanisms' (p.223, "
        "half graded: sol:7-reset.c1, opus:7-dma.c23) is not asserted. gary.v's full "
        "port list (read directly for this package, gary.v:49-108) has no "
        "bus-request/grant/acknowledge port of any kind -- dbr ('Agnus "
        "takes the bus', gary.v:70) is a plain level input, not one half of "
        "a request/acknowledge handshake pair. Reason: absent.");

    // ================= PROP-B / PROP-C =================
    // ---------------- OVL-CONTROL-BASELINE-000000 ----------------
    {
        Harness h;
        h.top->cpu_address_in = waddr(0x000000);
        h.top->cpu_rd = 1;
        h.top->ovl =
#if MUT_CTRL0
            1;  // MUTATION: overlay engaged -- this becomes PROP-B/E's own state instead of the control's.
#else
            0;
#endif
        h.top->eval();
        bool ram_here = (h.top->sel_chip & 0x1) == 1;
        bool rom_absent = h.top->sel_kick == 0;
        checks.expect(ram_here && rom_absent,
            "OVL-CONTROL-BASELINE-000000: [control/precondition, no claim "
            "id] with the overlay off (ovl=0), at $000000, measured the "
            "same way as OVERLAY-ROM-AT-000000 and OVERLAY-RAM-UNAVAILABLE-"
            "WRITE-000000 below (sel_chip[0], sel_kick): the normal system "
            "RAM answers (sel_chip[0]=1) and the ROM image does not "
            "(sel_kick=0). Without this pair, both of PROP-B's and PROP-C's "
            "assertions could also pass on a fixture that answered nothing "
            "at all at $000000 regardless of ovl. It also stands as the "
            "'before' state PROP-D's and PROP-E's RESET transition "
            "presupposes: memory present, ROM absent, prior to the "
            "overlay. measured sel_chip=" + hexbits(h.top->sel_chip) +
            " sel_kick=" + std::to_string((int)h.top->sel_kick) + ".");
    }

    // ---------------- OVERLAY-ROM-AT-000000 ----------------
    {
        Harness h;
        h.top->cpu_address_in = waddr(0x000000);
        h.top->cpu_rd = 1;
        h.top->ovl =
#if MUT_B1
            0;  // MUTATION: overlay released.
#else
            1;
#endif
        h.top->eval();
        bool ok = h.top->sel_kick == 1;
        checks.expect(ok,
            "OVERLAY-ROM-AT-000000: with the overlay active (ovl=1), a read "
            "of $000000 raises sel_kick (gary.v:166, the `cpu_rd && ovl && "
            "cpu_address_in[23:19]==5'b0000_0` term). 'An additional copy "
            "of the system ROM responds starting at memory location "
            "$00000000.' (p.223, sol:7-reset.c4, opus:7-reset.c1 -- ROM-half "
            "only; that record's other half, 'the memory map is in a "
            "special state' from 'turned on or externally reset,' is "
            "classified at fixture minimig and is out of scope here -- see "
            "RESET-SPECIAL-STATE-HALF-NOT-ASSERTED below, which is where "
            "that claim's uid is cited). Its 'before' precondition is asserted "
            "separately at OVL-CONTROL-BASELINE-000000. measured sel_kick(" +
            hexaddr(0x000000) + ", ovl=" + std::to_string((int)h.top->ovl) +
            ")=" + std::to_string((int)h.top->sel_kick) + ".");
    }

    checks.record(
        "RESET-SPECIAL-STATE-HALF-NOT-ASSERTED (sol:7-reset.c3, p.223): the "
        "'turned on or externally reset' half of p.223's sentence -- that the "
        "memory map enters a special state -- is classified at fixture "
        "minimig, not gary, and is out of this package's scope. Declined here "
        "with a reason rather than tested.\n"
        "        Cited on this line and NOT on any graded line, deliberately: "
        "until stage 8 the uid appeared inside OVERLAY-ROM-AT-000000's label, "
        "where it said this very thing -- that the claim is out of scope -- "
        "and the coverage ledger, which searches graded lines for uids, read "
        "that mention as coverage. A negative citation on a graded line is "
        "read by the ledger as the opposite of what it says.");

    // ---------------- OVERLAY-RAM-UNAVAILABLE-WRITE-000000 ----------------
    {
        Harness h;
        h.top->cpu_address_in = waddr(0x000000);
        h.top->cpu_rd = 0;
        h.top->cpu_hwr = 1;
        h.top->cpu_lwr = 1;
        h.top->ovl =
#if MUT_C1
            0;  // MUTATION: overlay released.
#else
            1;
#endif
        h.top->eval();
        bool nothing_answers = (h.top->sel_kick == 0) &&
                               ((h.top->sel_chip & 0x1) == 0);
        checks.expect(nothing_answers,
            "OVERLAY-RAM-UNAVAILABLE-WRITE-000000: with the overlay active "
            "(ovl=1), a WRITE to $000000 selects neither the ROM "
            "(sel_kick=0, since gary.v:166's overlay term requires cpu_rd, "
            "which a write cycle does not assert) nor the RAM "
            "(sel_chip[0]=0, since gary.v:159 requires !ovl) -- the normal "
            "RAM does not silently answer writes just because the ROM "
            "overlay is read-only. 'The system RAM that would normally be "
            "located at this address is not available.' (p.223, "
            "sol:7-reset.c5, opus:7-reset.c2). measured sel_chip=" +
            hexbits(h.top->sel_chip) + " sel_kick=" +
            std::to_string((int)h.top->sel_kick) + " (ovl=" +
            std::to_string((int)h.top->ovl) + ").");
    }

    // ================= PROP-D =================
    // ---------------- RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY ----------------
    {
        Harness h;
        h.top->cpu_rd = 1;
        h.top->ovl =
#if MUT_D2
            0;  // MUTATION: overlay released -- bank 0 must then reappear too.
#else
            1;
#endif
        bool all_ok = true;
        std::string detail;
        const uint32_t addrs[4] = {0x000000, 0x010000, 0x020000, 0x030000};
        for (int i = 0; i < 4; ++i) {
            h.top->cpu_address_in = waddr(addrs[i]);
            h.top->eval();
            bool gone = (h.top->sel_chip & 0x1) == 0;
            all_ok = all_ok && gone;
            detail += " " + hexaddr(addrs[i]) + ":sel_chip=" +
                hexbits(h.top->sel_chip);
        }
        checks.expect(all_ok,
            "RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY: [control, no "
            "claim id] four points sampled only inside bank 0 (the one bank "
            "the manual's claim actually holds for in this RTL), overlay "
            "active, measured the same way (sel_chip) as the sweep below --"
            " all four read gone. Same measurement basis as the suspect "
            "sweep, expected to pass: rules out a harness that could never "
            "show sel_chip disappearing at all. measured" + detail + ".");
    }

    // ---------------- RESET-ALL-MEMORY-DISAPPEARS-SWEEP ----------------
    {
        Harness h;
        h.top->cpu_rd = 1;
        h.top->ovl = 1;  // the state gary presents once RESET has set the overlay -- gary itself holds no reset/overlay state (see OVERLAY-SOURCE-NOT-IN-SCOPE), so ovl=1 is that state's entire footprint at this fixture.
        bool all_ok = true;
        std::string detail;
#if MUT_D1
        // MUTATION: all four sample points moved inside bank 0 (the one
        // bank the manual's claim actually holds for), instead of one
        // address per 512KB chip-memory bank. The same comparison then
        // reads green, showing the FAIL below (without this mutation) is a
        // property of banks 1-3's own address decode, not of this check.
        const uint32_t addrs[4] = {0x000000, 0x010000, 0x020000, 0x030000};
#else
        const uint32_t addrs[4] = {0x000000, 0x080000, 0x100000, 0x180000};
#endif
        uint32_t measured_bits[4];
        for (int i = 0; i < 4; ++i) {
            h.top->cpu_address_in = waddr(addrs[i]);
            h.top->eval();
            measured_bits[i] = h.top->sel_chip;
            bool gone = h.top->sel_chip == 0;
            all_ok = all_ok && gone;
            detail += " " + hexaddr(addrs[i]) + ":sel_chip=" +
                hexbits(h.top->sel_chip);
        }
        checks.expect(all_ok,
            "RESET-ALL-MEMORY-DISAPPEARS-SWEEP: one address per 512KB "
            "chip-memory bank (banks 0-3), overlay active (ovl=1), must show "
            "sel_chip==0 at every bank for 'All memory ... disappear' to "
            "hold, measured the same way (sel_chip) as the control above. "
            "'All memory and AUTOCONFIG(TM) cards disappear' (p.223, "
            "sol:7-reset.c13; AUTOCONFIG half excluded, see record below). "
            "measured" + detail + ".");
#if !MUT_D1
        checks.note(
            "RESET-ALL-MEMORY-DISAPPEARS-SWEEP breakdown, measured directly "
            "on this build (reproducing STEP3-NOTES-ch7-reset-overlay.md's "
            "own flagged measurement independently, not adopting it): "
            "sel_chip($080000)=" + hexbits(measured_bits[1]) +
            ", sel_chip($100000)=" + hexbits(measured_bits[2]) +
            ", sel_chip($180000)=" + hexbits(measured_bits[3]) +
            " with ovl=1 -- banks 1-3 remain mapped because gary.v:160-162's "
            "sel_chip[1]/[2]/[3] terms carry no `ovl` at all; only "
            "gary.v:159's sel_chip[0] is gated by !ovl. Only bank 0 "
            "($000000-$07FFFF) actually disappears under the overlay in "
            "this RTL. The sweep above is graded on the manual's literal "
            "word 'all' and reads red for exactly this reason -- a finding "
            "about gary.v, not an error in the check (BUILD-BRIEF.md's "
            "oracle rule; see this assertion's own MUTATION, which reads "
            "green when confined to bank 0 alone, and the control above, "
            "which does the same by construction).");
#endif
    }
    checks.record(
        "AUTOCONFIG-HALF-NOT-ASSERTED: the AUTOCONFIG half of 'All memory "
        "and AUTOCONFIG(TM) cards disappear' (p.223, half graded: sol:7-reset.c13) is not "
        "asserted. gary.v's full port list (read directly for this package, "
        "gary.v:49-108) has no AUTOCONFIG configuration or shut-up port of "
        "any kind. Reason: absent.");

    // ================= PROP-E =================
    // ---------------- RESET-ROM-REAPPEARS-000000 ----------------
    {
        Harness h;
        h.top->cpu_address_in = waddr(0x000000);
        h.top->cpu_rd = 1;
        h.top->ovl =
#if MUT_E1
            0;  // MUTATION: the state the RESET instruction produces (ovl=1) withheld.
#else
            1;
#endif
        h.top->eval();
        bool ok = (h.top->sel_kick == 1) && ((h.top->sel_chip & 0x1) == 0);
        checks.expect(ok,
            "RESET-ROM-REAPPEARS-000000: measured the same way as OVERLAY-"
            "ROM-AT-000000 (PROP-B) -- sel_kick=1 and sel_chip[0]=0 at "
            "$000000 with ovl=1. Distinct printed sentence, same underlying "
            "gary measurement (SPECIFICATION.md's own scope note: gary "
            "holds no state distinguishing 'power-on/external reset' from "
            "'the RESET instruction' -- both are simply ovl=1 at this "
            "fixture): '...the ROM image appears at location $00000000.' "
            "(p.223, sol:7-reset.c14, opus:7-reset.c10 -- ROM-half only; "
            "the memory-disappear half of opus:7-reset.c10's bundled quote "
            "is credited to RESET-ALL-MEMORY-DISAPPEARS-SWEEP (PROP-D) "
            "instead, per that record's own split field). Its 'before' "
            "precondition -- ROM does NOT answer here without the overlay "
            "-- is asserted separately at OVL-CONTROL-BASELINE-000000. "
            "measured sel_kick=" + std::to_string((int)h.top->sel_kick) +
            " sel_chip=" + hexbits(h.top->sel_chip) + ".");
    }

    checks.record(
        "OVERLAY-SOURCE-NOT-IN-SCOPE: gary carries what the overlay DOES to "
        "the address map and holds no overlay state of its own -- `ovl` "
        "(gary.v:69) is a plain input, and every sel_chip/sel_kick term that "
        "reads it (gary.v:159,166) is combinational, with no register in "
        "its cone. Where the overlay bit comes FROM -- power-on/external "
        "reset versus the 68000 RESET instruction -- is the sibling package "
        "ch7-overlay-source, fixture minimig. Not reached here.");
    checks.record(
        "CPU-HLT-NOT-IN-SCOPE: gary.v:159 and gary.v:166 both fold a "
        "`cpu_hlt` exception into the overlay decode (sel_chip[0] can read 1 "
        "and the address-$000000 term of sel_kick is unaffected by cpu_hlt "
        "specifically, but sel_kick's OWN first disjunct also accepts "
        "cpu_hlt over the kickstart range) -- read directly at gary.v:159, "
        "166 while finding the port list for this package. None of this "
        "package's 9 claims names CPU-halt behaviour. Held at cpu_hlt=0 "
        "throughout; not asserted.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
