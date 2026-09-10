// ch3-sprite-registers -- denise_sprites conformance test
//
// Test purpose: the appendix C sprite-register fields Denise's ECS
//   SuperHires mode adds -- the extra low-order horizontal-start bit
//   (SH0) that SPRxCTL carries, the SH8-SH1 field SPRxPOS carries, the
//   mode-gating that ties the extra bit's significance to SuperHires
//   resolution specifically, the two ECS-only bits SPRxCTL's table notes
//   without naming their function, the joint role of SPRxPOS/SPRxCTL as
//   position-and-feature registers, and that these registers accept a
//   direct (non-DMA) write -- all at the denise_sprites module boundary.
//   Package ch3-sprite-registers, fixture denise_sprites (one Verilator
//   build, this module only, per PACKAGE.md).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 297, 303 (claims.jsonl for ch3-sprite-registers, 20 records). A
//   red result below means denise_sprites.v disagrees with the manual,
//   not that the assertion is wrong -- no assertion here was, or should
//   be, adjusted to make the RTL pass.
// Non-goals: "size" (vertical extent, VSTART/VSTOP) is part of what
//   C-shres-sprite.c4/c5/C-spr.c03 say SPRxPOS/SPRxCTL jointly control,
//   but this module's own port list (below) carries no vertical-position
//   input at all -- only `hpos`. SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING
//   therefore asserts only the two components this boundary can observe,
//   position and feature (attach) control, and says so in its own label
//   rather than silently dropping the "size" half of the claim. Bits 3
//   and 4 of SPRxCTL are named "ECS chips only" by the quoted NOTE but no
//   quotation in this package's 20 records gives either bit a field
//   mnemonic or states what it does -- SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS
//   therefore tests only that each bit is live (changes something
//   observable), not the OCS/ECS chip-scoping itself, because this
//   module exposes a single `aga` input, not a finer OCS-vs-ECS select --
//   documented in that assertion's own label, not silently assumed.
//
// BOUNDARY: nanomig/src/minimig-aga/denise_sprites.v:55,67 (SPRPOSCTLBASE
//   9'h140), per PACKAGE.md, whose own evidence field additionally cites
//   the sprite resolution select at denise.v:252 (`spres` =
//   `bplcon3[7:6] & aga`) and denise.v:390-391/393-416 (instantiation
//   with `reg_address_in`, `hpos`, `sprena`). That `& aga` term means
//   `spres` can only be non-zero one level up, in denise.v, when `aga` is
//   also set -- a fact about denise.v's wiring, outside this package's
//   boundary and not tested here (denise_sprites itself takes `spres` as
//   a bare input and is driven directly by this harness, independent of
//   its own `aga` input). Worth flagging for whoever owns the wider
//   boundary: the manual frames SuperHires sprite positioning as an ECS
//   Denise feature, and this wiring appears to make it reachable only
//   when AGA is also selected.
//
// ORDER OF WORK: this file (assertions, expected values, claim-id
// coverage, register addresses, bit positions) was written and committed
// from claims.jsonl's quotations and PACKAGE.md's own evidence field
// ONLY -- denise_sprites.v itself was not opened until after that commit,
// per the build brief's explicit gate. The register word-address
// convention ($140->0xA0 for SPR0POS, +4 words per sprite for SPR1's own
// registers), the clk7_en/c1/c3 four-phase clocking, the reg_address_in/
// data_in write sequencing, the SPR0DATA=0x8000 arm-on-write convention,
// and the SPR1CTL bit 7 (ATTACH) -> combined-colour-index convention are
// all reused from hrm/denise_sprites/test_denise_sprites.cpp -- an
// already-committed, already-built sibling harness for the SAME module --
// not learned by opening this package's own forbidden-until-committed
// file. The two numeric predictions this file stakes out before opening
// the RTL -- that a +1 step of SPRxPOS's byte moves the measured start by
// exactly 2 raw units, and that SPRxCTL's bit 0 (SH0) moves it by exactly
// 1 -- follow directly from the two quoted bit-table rows themselves
// ("07-00 SH8-SH1 Start horizontal value. Low bit (SH0) in SPRxCTL.":
// SPRxPOS[7:0] holds the *upper* 8 bits of a 9-bit start value, so its
// LSB is worth 2 raw units; SPRxCTL bit 0 is that missing raw LSB, worth
// 1) -- not from the RTL, and not from the sibling harness's own
// observation that SPR0POS=0x40 puts the sprite near hpos 0x80 (that
// observation is consistent with, but not needed to derive, the doubling
// relationship asserted here).
//
// MEASUREMENT BASIS: every check below arms sprite 0 (and, for the
// ATTACH check, sprite 1) with the same DATB=0x0000/DATA=0x8000 image
// (colour index 1, or 13 when attached to an armed sprite 1 -- both
// values already validated as reachable by the sibling harness) and
// reads `sprdata` while sweeping `hpos` 0..511 (the start value SH8-SH0
// is 9 bits, so 512 covers its whole range) from a freshly reset DUT
// each time, to keep every measurement isolated from any earlier one.
// "Start" means the first hpos at which sprdata is non-zero. No
// assertion below depends on the *absolute* value of that first hpos
// (which carries whatever fixed pipeline latency the comparator has,
// unmeasured here) -- every position-based assertion instead compares
// two starts measured the same way, so a constant latency cancels out.
//
// CLAIM COVERAGE: all 20 records of claims.jsonl are cited below, in
// full, inside an expect() label. Ids restating the identical
// proposition under a different manual wording, page, or reader are
// grouped and cited together on the single assertion that tests that
// proposition, as hrm/ch3-collision-registers/ and
// hrm/ch3-sprite-playfield-priority/ both do.
//
//   PROCESSOR-WRITE-REACHES-SPRXPOS (1 id)         -- C-shres-sprite.c7
//   SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 (2 ids)      -- C-shres-sprite.c8,
//                                                      C-spr.c05
//   SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES (9 ids)    -- C-library.c2,
//     C-shr.c05, C-shres-sprite.c1, C-shres-sprite.c2, C-spr.c01,
//     C-shres-sprite.c15, C-spr.c10, C-shres-sprite.c12,
//     C-shres-sprite.c9
//   SH0-IS-HALF-OF-POS-BYTE-STEP (2 ids)           -- C-shres-sprite.c3,
//                                                      C-spr.c02
//   SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING (3 ids)   -- C-shres-sprite.c4,
//     C-shres-sprite.c5, C-spr.c03
//   SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS (3 ids)       -- C-shres-sprite.c13,
//     C-shres-sprite.c14, C-spr.c09
//   1+2+9+2+3+3 = 20. No claim id is declined.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
// "[control]" so the mutation gate does not require a mutation record
// for it, matching hrm/ch3-collision-registers/'s convention) proves the
// write -> arm -> hpos-sweep -> read-sprdata path this whole file shares
// is alive -- the sprite becomes visible somewhere and invisible
// elsewhere -- before any claim-based assertion depends on it.
//
// RESULTS (after opening the RTL and building): 5 passed, 2 failed. Two
// genuine findings account for both failures; neither assertion was
// adjusted to hide them.
//
//   FINDING 1 -- SH0 is never gated by resolution. denise_sprites_shifter.v
//     (non-EBR path) computes the sprite's position match purely as
//     `hpos[7:0] == hstart[7:0]`, where hstart[8:1] comes from SPRxPOS and
//     hstart[0] (SH0) comes from SPRxCTL bit 0 -- this comparison does not
//     read `spres`/`shift` at all. `spres` only changes how fast the
//     already-loaded shift register drains afterwards
//     (denise_sprites.v:158-166, the `shift` case on `spres`), not whether
//     SH0 participates in finding the start. Measured: toggling SH0 moves
//     the start by 1 raw unit at EVERY one of the 4 spres encodings,
//     including spres=0 -- not only at one SuperHires-like value, as
//     C-library.c2/C-shr.c05/C-shres-sprite.c1/c2/c15/C-spr.c01/c10 read.
//     SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES fails on its "inert at spres=0"
//     clause for this reason; its own MEASURED note above records the
//     four per-spres deltas (1,1,1,1) that show it.
//   FINDING 2 -- SPRxCTL bits 3 and 4 are not implemented at all, on any
//     chip mode. The ONLY bits denise_sprites_shifter.v's CTL-register
//     write ever captures are bit 7 (attach) and bit 0 (SH0):
//     `{attach,hstart[0]} <= {data_in[7],data_in[0]};` -- nothing in
//     denise_sprites.v or denise_sprites_shifter.v (non-EBR path) ever
//     reads data_in[3] or data_in[4] from a CTL write. This is a stronger
//     deviation than "ECS chips only" (p.303's NOTE) describes: the bits
//     are not chip-scoped, they are simply absent from this
//     implementation, at aga=0 and (by the same dead-code reading) at
//     aga=1 too.
//   Both findings are read directly off the RTL cited above (file, line,
//   the exact expression), not inferred from the test's failure alone.
//
// MUTATION LOG (all 4 stimulus entries actually run by this agent under
// WSL/Verilator on 11.09.2026 -- edit -> rebuild -> run -> confirm the
// named assertion, and only that assertion, changed colour -> revert ->
// rebuild -> confirm the 5-passed/2-failed baseline restored -- before
// moving to the next one. nanomig/ was never touched; no expected value,
// oracle text or comparison logic was ever edited, only stimulus. The
// first attempt at the SPRXPOS-BYTE mutation also moved
// SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING, which reused that assertion's
// pos_delta variable -- a coupling bug in the harness, not a finding; it
// was fixed by giving JOINTLY-LOAD-BEARING its own independent
// measurement before the mutation below was run for record):
//
// MUTATION PROCESSOR-WRITE-REACHES-SPRXPOS stimulus red: the second SPR0POS write's value changed from 0x50 to 0x10 (the same as the first), so the two writes stopped differing -> FAIL ("4 passed, 3 failed"), only PROCESSOR-WRITE-REACHES-SPRXPOS newly red (lo.start no longer < hi.start; they are equal). Reverted to 0x50: 5/2, same two findings.
// MUTATION SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 stimulus red: the second SPR0POS write's value changed from 0x41 to 0x42, a byte step of 2 instead of 1 -> FAIL ("4 passed, 3 failed"), only SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 newly red (measured delta became 4, not the asserted 2). Reverted to 0x41: 5/2, same two findings.
// MUTATION SH0-IS-HALF-OF-POS-BYTE-STEP stimulus red: the "hi" measurement's SPR0CTL value changed from 0x01 (SH0) to 0x02 (bit 1, a bit this RTL never stores anywhere) -> FAIL ("4 passed, 3 failed"), only SH0-IS-HALF-OF-POS-BYTE-STEP newly red (measured delta became 0, not the asserted 1). Reverted to 0x01: 5/2, same two findings.
// MUTATION SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING stimulus red: the attached-case SPR1CTL write changed from 0x80 (ATTACH set) to 0x00, so sprite 1 is armed but never actually attached -> FAIL ("4 passed, 3 failed"), only SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING newly red (the attached measurement returned colour 1, the same as the unattached one, instead of 13). Reverted to 0x80: 5/2, same two findings.
// MUTATION SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES none impossible: already red at baseline (FINDING 1). SH0 is live at every one of the 4 reachable spres values (deltas 1,1,1,1, printed by this assertion's own MEASURED note) -- there is no spres encoding this harness can present at which SH0 is inert, so no stimulus mutation can turn this assertion from green to red; there is nothing green here to break.
// MUTATION SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS none impossible: already red at baseline (FINDING 2). Bits 3 and 4 of SPRxCTL are not captured by any register in this RTL at all (only bits 7 and 0 are), so no stimulus this harness can present makes either bit observable; there is nothing green here to break.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, reused from hrm/denise_sprites/
// test_denise_sprites.cpp for this same module: SPRPOSCTLBASE is byte
// address 0x140 (PACKAGE.md's own evidence field), so word address
// (reg_address_in) is 0x140>>1 = 0xA0 for SPR0POS; each sprite occupies
// 4 consecutive word addresses (POS, CTL, DATA, DATB), 8 sprites total.
constexpr int SPR0POS = 0xA0;
constexpr int SPR0CTL = 0xA1;
constexpr int SPR0DATA = 0xA2;
constexpr int SPR0DATB = 0xA3;
constexpr int SPR1POS = 0xA4;
constexpr int SPR1CTL = 0xA5;
constexpr int SPR1DATA = 0xA6;
constexpr int SPR1DATB = 0xA7;
constexpr int IDLE_ADDR = 0x00;

constexpr int HPOS_MAX = 512;  // SH8..SH0 is a 9-bit value: 0..511 covers it.

struct Meas {
    int start = -1;                 // first hpos at which sprdata != 0, or -1.
    std::vector<uint8_t> trace;      // sprdata at every swept hpos.
};

// One isolated measurement: fresh DUT, reset, arm sprite 0 (and,
// optionally, sprite 1 for the ATTACH case) with a known image, sweep
// hpos, record when/what becomes visible. Clocking (clk7_en/c1/c3
// four-phase tick, reg_address_in/data_in write sequencing) and the
// DATB=0/DATA=0x8000 arm-on-write convention are reused from the sibling
// harness for this same module (see ORDER OF WORK above), not learned
// from this package's own denise_sprites.v.
class Harness {
public:
    Harness(uint8_t aga, uint8_t spres) : dut(new Vdenise_sprites) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->aga = aga;
        dut->spres = spres;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->hpos = 0;
        dut->sprena = 1;
        dut->esprm = 0;
        dut->osprm = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // c1=0,c3=0 -> shift/load; c1=1,c3=0; c1=1,c3=1 (clk7n_en); c1=0,c3=1.
    void tick_phase() {
        phase = (phase + 1) % 4;
        switch (phase) {
            case 0: dut->c1 = 0; dut->c3 = 0; dut->clk7_en = 1; break;
            case 1: dut->c1 = 1; dut->c3 = 0; dut->clk7_en = 0; break;
            case 2: dut->c1 = 1; dut->c3 = 1; dut->clk7_en = 0; break;
            case 3: dut->c1 = 0; dut->c3 = 1; dut->clk7_en = 0; break;
        }
        tick();
    }

    void tick_7mhz() {
        tick_phase();
        tick_phase();
        tick_phase();
        tick_phase();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick_7mhz();
    }

    Vdenise_sprites* dut;
    int phase = 0;
};

// Arms sprite 0 at the given SPRxPOS byte / SPRxCTL byte and sweeps hpos,
// recording the resulting sprdata trace and first-active hpos.
Meas measure(uint8_t pos_byte, uint8_t ctl_byte, uint8_t spres, uint8_t aga) {
    Harness h(aga, spres);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, ctl_byte);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, 0x8000);  // arms sprite 0.

    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        uint8_t v = static_cast<uint8_t>(h.dut->sprdata);
        m.trace.push_back(v);
        if (v != 0 && m.start == -1) m.start = hp;
    }
    return m;
}

// Arms sprite 0 alone (attach=false), or sprite 0 together with sprite 1
// at the SAME start position with SPR1CTL bit 7 (ATTACH) set (attach=
// true), and returns the first non-zero colour index seen. Reuses the
// sibling harness's own already-validated recipe (SPR1 DATB=DATA=0x8000
// under attach), not derived from this package's own RTL.
uint8_t measure_attach_color(uint8_t pos_byte, bool attach) {
    Harness h(/*aga=*/0, /*spres=*/0);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, 0x00);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, 0x8000);
    if (attach) {
        h.write_reg(SPR1POS, pos_byte);
        h.write_reg(SPR1CTL, 0x80);
        h.write_reg(SPR1DATB, 0x8000);
        h.write_reg(SPR1DATA, 0x8000);
    }
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        uint8_t v = static_cast<uint8_t>(h.dut->sprdata);
        if (v != 0) return v;
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-LIVE-DATA-PATH [control] ----------------
    {
        Meas m = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool took_both_values = m.start != -1;
        bool went_quiet_again = false;
        if (m.start != -1) {
            for (size_t i = static_cast<size_t>(m.start) + 1; i < m.trace.size(); ++i) {
                if (m.trace[i] == 0) { went_quiet_again = true; break; }
            }
        }
        checks.expect(took_both_values && went_quiet_again,
            "[control] FIXTURE-LIVE-DATA-PATH: arming sprite 0 (SPR0POS=0x40, "
            "SPR0CTL=0x00, SPR0DATB=0x0000, SPR0DATA=0x8000) and sweeping hpos "
            "0..511 makes sprdata take both a non-zero value and, afterwards, "
            "zero again -- not a single stuck value -- proving the write -> "
            "arm -> hpos-sweep -> read-sprdata measurement basis every "
            "assertion below shares is actually live before any claim-based "
            "assertion depends on it. Fixture check, no claim id, no page.");
    }

    // ---------------- PROCESSOR-WRITE-REACHES-SPRXPOS ----------------
    {
        Meas lo = measure(/*pos=*/0x10, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas hi = measure(/*pos=*/0x50, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool both_armed = lo.start != -1 && hi.start != -1;
        bool monotonic = both_armed && hi.start > lo.start;
        checks.expect(monotonic,
            "PROCESSOR-WRITE-REACHES-SPRXPOS: writing two different values "
            "(0x10, then separately 0x50) to SPR0POS through reg_address_in/"
            "data_in -- this module's only register-write interface, the "
            "same one every write in this file uses, with no separate "
            "DMA-tagged path to distinguish a processor write from a "
            "DMA-sourced one -- moves the sprite's measured start position "
            "forward as the written value increases, showing a value placed "
            "on this interface reaches SPRxPOS and takes effect: 'They are "
            "usually loaded by the sprite DMA channel, during horizontal "
            "blank, however they may be loaded by the processor.' (p.303). "
            "C-shres-sprite.c7 .");
    }

    // ---------------- SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 ----------------
    int pos_delta = 0;
    {
        Meas a = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas b = measure(/*pos=*/0x41, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool both_armed = a.start != -1 && b.start != -1;
        pos_delta = both_armed ? (b.start - a.start) : -1000;
        checks.expect(both_armed && pos_delta == 2,
            "SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2: incrementing SPR0POS's byte "
            "by 1 (0x40 -> 0x41, SPR0CTL held at 0) moves the measured start "
            "position by exactly 2 raw hpos units, not 1 -- because SPRxPOS "
            "holds the UPPER 8 bits of the 9-bit start value (SH8-SH1), so "
            "its own least-significant bit is worth 2 raw units, with the "
            "true bit-0 (SH0) living elsewhere: "
            "'07-00  SH8-SH1  Start horizontal value. Low bit (SH0) in "
            "SPRxCTL.' (p.303). C-shres-sprite.c8, C-spr.c05 .");
    }

    // ---------------- SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES ----------------
    int superhires_spres = -1;
    {
        // sh0_delta(spres) = start(SH0=1) - start(SH0=0), SPR0POS fixed at
        // 0x40, for each of the 4 values spres (a 2-bit input, per
        // PACKAGE.md's own evidence "spres = bplcon3[7:6] & aga") can take.
        int delta[4];
        bool armed[4];
        for (int sp = 0; sp < 4; ++sp) {
            Meas lo = measure(/*pos=*/0x40, /*ctl=*/0x00, static_cast<uint8_t>(sp), /*aga=*/0);
            Meas hi = measure(/*pos=*/0x40, /*ctl=*/0x01, static_cast<uint8_t>(sp), /*aga=*/0);
            armed[sp] = lo.start != -1 && hi.start != -1;
            delta[sp] = armed[sp] ? (hi.start - lo.start) : -1000;
        }
        // spres=0 is this module's own already-established baseline
        // ("dut->spres = 0; // OCS sprites" in the sibling harness for this
        // same module) -- the claims say the extra granularity is
        // SuperHires-only, so bit 0 must be inert there.
        bool inert_at_baseline = armed[0] && delta[0] == 0;
        int found = -1;
        for (int sp = 1; sp < 4 && found == -1; ++sp) {
            if (armed[sp] && delta[sp] != 0) found = sp;
        }
        superhires_spres = found;
        checks.expect(inert_at_baseline && found != -1,
            "SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES: with SPR0POS fixed at "
            "0x40, toggling SPR0CTL bit 0 changes the measured start "
            "position by 0 raw units at spres=0 (this module's own OCS/"
            "default baseline) but by a non-zero amount at at least one "
            "other spres encoding -- showing bit 0 is exactly the 'SH0' "
            "field the appendix names, and that its significance is tied "
            "to a specific (SuperHires) resolution setting, not present "
            "generally: 'The new SuperHires mode provides 35ns pixel rates "
            "and sprite positioning at 70ns rates.' (p.297); 'SuperHires "
            "mode has a finer granularity of sprite positioning than "
            "either Hires or Lores modes.' (p.303); '70ns sprite positions "
            "are only available in SuperHires mode.' (p.303); "
            "'00      SH0       Start horiz. value Low bit' (p.303). "
            "C-library.c2, C-shr.c05, C-shres-sprite.c1, C-shres-sprite.c2, "
            "C-spr.c01, C-shres-sprite.c15, C-spr.c10, C-shres-sprite.c12, "
            "C-shres-sprite.c9 .");
        if (found == -1) {
            checks.note("SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES: no spres value "
                "in {1,2,3} made SH0 significant; SH0-IS-HALF-OF-POS-BYTE-"
                "STEP below has no SuperHires spres value to measure at and "
                "is therefore decided false for that reason.");
        } else {
            checks.note("SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES: spres=" +
                std::to_string(found) + " is the value at which SH0 was "
                "measured significant (deltas: spres0=" + std::to_string(delta[0]) +
                " spres1=" + std::to_string(delta[1]) + " spres2=" + std::to_string(delta[2]) +
                " spres3=" + std::to_string(delta[3]) + ").");
        }
    }

    // ---------------- SH0-IS-HALF-OF-POS-BYTE-STEP ----------------
    {
        bool ok = false;
        int sh0_delta_at_shres = -1000;
        if (superhires_spres != -1) {
            Meas lo = measure(/*pos=*/0x40, /*ctl=*/0x00,
                               static_cast<uint8_t>(superhires_spres), /*aga=*/0);
            Meas hi = measure(/*pos=*/0x40, /*ctl=*/0x01,
                               static_cast<uint8_t>(superhires_spres), /*aga=*/0);
            if (lo.start != -1 && hi.start != -1) {
                sh0_delta_at_shres = hi.start - lo.start;
                // Fixed, quote-derived target (2 / 2 == 1), independent of
                // whatever SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 measured above,
                // so this assertion cannot pass merely by chaining a
                // possibly-wrong runtime value into itself.
                ok = sh0_delta_at_shres == 1;
            }
        }
        checks.expect(ok,
            "SH0-IS-HALF-OF-POS-BYTE-STEP: at the SuperHires spres value "
            "found above, toggling SPR0CTL bit 0 (SH0) alone moves the "
            "measured start by exactly 1 raw unit -- half of the 2-unit "
            "step a +1 change to SPRxPOS's own byte produces -- i.e. SH0 "
            "lands the sprite on the position exactly between two "
            "SPRxPOS-addressable points, matching 'This allows for "
            "positioning the sprite every other SuperHires pixel on 70ns "
            "boundaries.' (p.303). C-shres-sprite.c3, C-spr.c02 .");
    }

    // ---------------- SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING ----------------
    {
        // Measured independently here (not reusing the outer pos_delta) so
        // this assertion's own result cannot be moved by a mutation aimed at
        // SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2's stimulus above.
        Meas pa = measure(/*pos=*/0x20, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas pb = measure(/*pos=*/0x30, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool position_side = pa.start != -1 && pb.start != -1 && pb.start != pa.start;
        uint8_t plain = measure_attach_color(/*pos=*/0x40, /*attach=*/false);
        uint8_t attached = measure_attach_color(/*pos=*/0x40, /*attach=*/true);
        bool feature_side = plain == 1 && attached == 13 && plain != attached;
        checks.expect(position_side && feature_side,
            "SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING: SPRxPOS alone changes "
            "the measured output (its own byte moves the start position, "
            "shown above), and, independently, SPRxCTL alone -- its bit 7 "
            "(ATTACH), with SPR0POS/SPR1POS held identical -- also changes "
            "the measured output (colour index 1 when sprite 1 is not "
            "attached, 13 when it is) -- neither register alone accounts "
            "for the whole result, as: 'The ECS registers SPRxPOS and "
            "SPRxCTL work together as position, size and sprite feature "
            "control registers.' (p.303). This boundary exposes no "
            "vertical-position input (only hpos), so only the position and "
            "feature-control components are measured here; the size "
            "(vertical extent) component of this claim is outside what can "
            "be observed at this boundary -- see Non-goals above. "
            "C-shres-sprite.c4, C-shres-sprite.c5, C-spr.c03 .");
    }

    // ---------------- SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS ----------------
    {
        bool all_live = true;
        for (int bit : {3, 4}) {
            Meas base = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
            Meas mut = measure(/*pos=*/0x40, static_cast<uint8_t>(1u << bit), /*spres=*/0, /*aga=*/0);
            bool differs = base.trace != mut.trace;
            if (!differs) all_live = false;
        }
        checks.expect(all_live,
            "SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS: setting SPR0CTL bit 3 (with "
            "all other bits, including SH0, held at 0) changes the full "
            "sprdata trace measured across hpos 0..511, and, separately, "
            "setting bit 4 alone also changes it -- each bit is a real, "
            "non-ignored field at this boundary, at this module's own "
            "`aga`=0 input, the nearest chip-mode signal this boundary "
            "exposes (it has no separate OCS-vs-ECS select to test the "
            "chip-scoping itself against): 'Note: bits 3 and  4 are in the "
            "ECS chips only.' (p.303, cited once for each bit's row). "
            "C-shres-sprite.c13, C-shres-sprite.c14, C-spr.c09 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
