// Test purpose: pp.391/394/411 bus interrupt lines, `paula_intcontroller`
//   alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 391, 394, 411
// Non-goals: anything outside the `paula_intcontroller` fixture this package
//   builds, and any claim outside the 7 it cites. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: 7 ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-bus-interrupt-lines.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-bus-interrupt-lines -- pp.391/394/411 bus interrupt lines,
// `paula_intcontroller` alone. 7 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-bus-interrupt-lines.jsonl.
//
// All seven claims (sol:K-0103, opus:K-0120 [p.391 "mixed into encoded
// interrupts"]; sol:K-0160, opus:K-0168 [p.394 "only /INT2 and /INT6 ...
// supported"]; sol:K-0439, sol:K-0440, opus:K-0444 [p.411 "two decoded,
// level-sensitive inputs ... labeled /INT2 and /INT6"]) reduce to one
// testable proposition at this boundary: driving `int2` alone (properly
// enabled) decodes to 68000 IPL 2, driving `int6` alone decodes to IPL 6,
// and the two are distinct. paula_intcontroller.v:15 (`int2`), :17
// (`int6`), :112 (`intreq[3]<=tmp[3]|int2`), :132 (`intreq[13]<=tmp[13]
// |int6`), :152-170 (the priority encoder).
//
// BOUNDARY. opus:K-0444's own clause "...available on the expansion bus"
// names a physical-wiring fact this module's ports cannot answer (no port
// says where int2/int6 are routed on the board); that half of the
// sentence is measured as not-exercised (note(), not record() -- the
// uid's OTHER half, the two-lines-two-levels fact, is graded). The "only
// /INT2 and /INT6 ... supported" completeness claims (K-0160/K-0168) are
// graded the same way: the behavioral half (driving int2/int6, reading
// the decode) is what is asserted; the port-list completeness half (no
// int1/int4/int5/int7 port exists on this module) is noted, confirmed by
// the fact that this fixture compiles and links against exactly the ports
// paula_intcontroller.v declares -- no debug port was added to check it.
//
// `int3`, the third `intN`-named port on this module, exists (blitter-
// finished per the RTL's own comment at :118) but is not one of the two
// externally-sourced lines these claims name -- per the task assignment's
// own given fact. `int3` is held low throughout and no uid here asserts
// anything about it.
//
// RESULTS: baseline (no MUT flag) is 1 passed, 7 failed -- ALL SEVEN
// GRADED ASSERTIONS ARE RED. MEASURED: with only int2's own bit (intreq
// bit 3, PORTS) enabled and driven, this RTL's priority encoder
// CORRECTED 18.09.2026: _ipl is the 68000's ACTIVE-LOW pin value and the
// first version of this file read it as the level. The measured _ipl==5 for
// int2 and _ipl==1 for int6 are 7-2 and 7-6 -- levels 2 and 6, correct. The
// original note follows, kept so the mistake is on the record:
// (paula_intcontroller.v:152-170) produces _ipl==5, not 2. With only
// int6's own bit (intreq bit 13, EXTER) enabled and driven, it produces
// _ipl==1, not 6. Both are reproducible, positive-control-checked
// measurements (see the "int2 asserted but only bit6 enabled" control,
// which confirms the encoder IS enable-gated and int2/int6 genuinely
// reach it) -- not an artifact of this fixture. This disagrees with the
// Amiga's well-known INTENA/INTREQ-bit-to-IPL-level table (PORTS at bit 3
// -> IPL 2, EXTER at bit 13 -> IPL 6, the same table the page's own
// /INT2 and /INT6 naming convention encodes) and with what every one of
// this package's seven claims states or implies about these two signals.
// The MUT=matchinglevels mutation (below) confirms the read path and the
// oracle are sound: substituting two OTHER bits that this same casez
// table does map to 2 and 6 (bit 12/syncint, bit 2/software-INTREQ)
// turns every assertion green. This is a finding about
// paula_intcontroller.v, reported as such -- no assertion below was, or
// should be, reworded to make the RTL pass.

#include "Vpaula_intcontroller.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <cstdint>

static Vpaula_intcontroller* top;
static vluint64_t sim_time = 0;
static int passed = 0, failed = 0;

// ---- the fixed harness (character-identical across the suite) ----
void expect(bool cond, const std::string& label) {
    if (cond) { std::cout << "PASS: " << label << "\n"; passed++; }
    else      { std::cout << "FAIL: " << label << "\n"; failed++; }
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// ---- clocking ----
void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

// ---- register bus (byte offsets, matching paula_intcontroller.v:27-30) ----
static const uint16_t INTENAR = 0x01c;
static const uint16_t INTREQR = 0x01e;
static const uint16_t INTENA  = 0x09a;
static const uint16_t INTREQ  = 0x09c;
static const uint8_t  ADDR_NONE = 0xFF;

static uint8_t addr_of(uint16_t regaddr) { return (uint8_t)((regaddr >> 1) & 0xFF); }

void write_reg(uint16_t regaddr, uint16_t data) {
    top->reg_address_in = addr_of(regaddr);
    top->data_in = data;
    tick();
    top->reg_address_in = ADDR_NONE;
    top->data_in = 0;
    tick();
}
uint16_t read_reg(uint16_t regaddr) {
    top->reg_address_in = addr_of(regaddr);
    top->eval();
    uint16_t v = top->data_out;
    top->reg_address_in = ADDR_NONE;
    top->eval();
    return v;
}

// Clear every INTENA and INTREQ bit (data_in[15]=0, mask=0x7FFF clears
// all 15 bits in one write -- the standard clear-all pattern this
// register pair supports).
void clear_all() {
    write_reg(INTENA, 0x7FFF);
    write_reg(INTREQ, 0x7FFF);
    settle(2);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vpaula_intcontroller;

    top->clk = 0;
    top->clk7_en = 1; // held high for the whole run -- every tick() is an active edge
    top->reset = 1;
    top->reg_address_in = ADDR_NONE;
    top->data_in = 0;
    top->rxint = 0;
    top->txint = 0;
    top->vblint = 0;
    top->int2 = 0;
    top->int3 = 0;
    top->int6 = 0;
    top->blckint = 0;
    top->syncint = 0;
    top->audint = 0;
    top->eval();

    settle(5);
    top->reset = 0;
    settle(5);
    clear_all();

    // =====================================================================
    // Fixture liveness -- proven before any claim is graded.
    // =====================================================================
    uint16_t enar_before = read_reg(INTENAR);
    write_reg(INTENA, 0x8000 | (1 << 14) | (1 << 3)); // master + PORTS bit
    uint16_t enar_after = read_reg(INTENAR);
    note("fixture-liveness INTENAR before=0x" + std::to_string(enar_before) +
         " after enabling master+bit3=0x" + std::to_string(enar_after));
    expect(enar_before != enar_after,
        "[not graded -- apparatus check, no claim uid] fixture-liveness: "
        "INTENAR readback responds to a register write");
    clear_all();

    // =====================================================================
    // Isolate int2 (PORTS, intreq[3]) -> expect IPL 2.
    // Isolate int6 (EXTER, intreq[13]) -> expect IPL 6.
    //
    // MUT_matchinglevels replaces BOTH probes at once: int2/bit3 with
    // syncint/bit12 (DSKSYNC), and int6/bit13 with a direct software-set
    // of bit2 (SOFT, intreq[2] has no external OR term -- it is set only
    // by writing INTREQ directly, paula_intcontroller.v:110 `intreq[2] <=
    // tmp[2];`, a legitimate first-class way to request it). Per this
    // RTL's own casez table (paula_intcontroller.v:152-170), bit12 alone
    // decodes to _ipl==2 and bit2 alone decodes to _ipl==6. This is the
    // required "stimulus green" mutation for every assertion below, all
    // of which are RED AT BASELINE (see RESULTS in the file header): a
    // single `make MUT=matchinglevels` build flips `decodes_correctly`
    // (the AND of both halves plus distinctness) fully to green, proving
    // the baseline red is attributable to int2/int6's OWN bits (3 and 13)
    // genuinely decoding to the wrong levels (5 and 1) on this RTL, not
    // to a fixture or oracle that can never read a match.
    // =====================================================================
    write_reg(INTENA, 0x8000 | (1 << 14) | (1 << (
#ifdef MUT_matchinglevels
        12
#else
        3
#endif
    )));
#ifdef MUT_matchinglevels
    top->syncint = 1;
    settle(4);
    top->syncint = 0;
#else
    top->int2 = 1;
    settle(4);
    top->int2 = 0;
#endif
    settle(4);
    uint8_t ipl_int2 = top->_ipl;
    note(std::string("int2 alone (master+bit3 enabled)") +
#ifdef MUT_matchinglevels
         " [MUTATION: bit12/syncint enabled and driven instead of bit3/int2]" +
#endif
         ": _ipl=" + std::to_string((int)ipl_int2) + " (expected 5 = the active-low pin value for level 2)");
    clear_all();

    write_reg(INTENA, 0x8000 | (1 << 14) | (1 << (
#ifdef MUT_matchinglevels
        2
#else
        13
#endif
    )));
#ifdef MUT_matchinglevels
    write_reg(INTREQ, 0x8000 | (1 << 2));
#else
    top->int6 = 1;
    settle(4);
    top->int6 = 0;
#endif
    settle(4);
    uint8_t ipl_int6 = top->_ipl;
    note(std::string("int6 alone (master+bit13 enabled)") +
#ifdef MUT_matchinglevels
         " [MUTATION: bit2/software-INTREQ enabled and set instead of bit13/int6]" +
#endif
         ": _ipl=" + std::to_string((int)ipl_int6) + " (expected 1 = the active-low pin value for level 6)");
    clear_all();

    // =====================================================================
    // Positive control (not graded, no MUT guard of its own) -- int2
    // physically asserted but its own INTENA bit NOT enabled must not
    // reach _ipl, distinguishing "port exists and is wired to the
    // encoder" from "correctly enabled". Always run unconditionally.
    // =====================================================================
    write_reg(INTENA, 0x8000 | (1 << 14) | (1 << 6)); // master + int3/blitter bit, NOT bit3
    top->int2 = 1;
    settle(4);
    top->int2 = 0;
    settle(4);
    uint8_t ipl_int2_wrong_enable = top->_ipl;
    note("int2 asserted but only bit6 (int3/blitter) enabled, not bit3: _ipl=" +
         std::to_string((int)ipl_int2_wrong_enable) +
         " (expected 7/idle -- proves the decode is genuinely enable-gated, "
         "not just level-triggered regardless of INTENA)");
    clear_all();

    // =====================================================================
    // `_ipl` IS THE 68000'S ACTIVE-LOW PIN VALUE, NOT THE INTERRUPT LEVEL.
    // The first version of this package compared it against the level
    // directly and turned all seven claims red on a reading of the port
    // rather than a property of the design.
    //
    // The leading underscore is this codebase's active-low marker, the same
    // one `_fire0`, `_ready` and `_track0` carry. The design's own top level
    // settles it: minimig.v:760 is
    //
    //     assign _cpu_ipl = int7 ? 3'b000 : _iplx;   // m68k interrupt request
    //
    // -- level 7, the highest, drives the pins to 000. So the pins encode
    // 7 - level, and the encoder's own table agrees end to end: no interrupt
    // pending gives `_ipl <= 7` (paula_intcontroller.v:167), which is level 0.
    //
    // int2 is INTREQ bit 3 (PORTS) and int6 is bit 13 (EXTER), and the
    // measured `_ipl` of 5 and 1 are 7-2 and 7-6 exactly. The RTL is right.
    // =====================================================================
    auto ipl_pins_for_level = [](int level) { return 7 - level; };
    bool int2_decodes_ipl2 = (ipl_int2 == ipl_pins_for_level(2));
    bool int6_decodes_ipl6 = (ipl_int6 == ipl_pins_for_level(6));
    bool two_distinct_levels = (ipl_int2 != ipl_int6);
    bool decodes_correctly = int2_decodes_ipl2 && int6_decodes_ipl6 && two_distinct_levels;

    // Structural note (not graded): no int1/int4/int5/int7 port exists on
    // this module -- this fixture links only against the ports
    // paula_intcontroller.v itself declares (clk, clk7_en, reset,
    // reg_address_in, data_in, data_out, rxint, txint, vblint, int2, int3,
    // int6, blckint, syncint, audint, audpen, rbfmirror, _ipl); no debug
    // port was added and none was needed to observe this.
    note("port-list completeness: paula_intcontroller declares int2, int3 "
         "and int6 as its only intN-named ports -- no int1/int4/int5/int7 "
         "port exists anywhere on this module (this fixture compiles "
         "against exactly the ports the source declares)");

    // All seven assertions below are RED AT BASELINE -- see RESULTS in the
    // file header. Per this suite's rule for a red-baseline assertion
    // (ch8-keyboard-events' CAPS-LOCK-RELEASE-NOT-ACCEPTED precedent),
    // each MUTATION below runs in the "stimulus green" direction: it
    // shows a stimulus that WOULD make the assertion pass, proving the
    // baseline red is attributable to int2/int6's own bits genuinely
    // decoding to the wrong level, not to a fixture or oracle that can
    // never read a match.

    // ---- p.391 "mixed into encoded interrupts" (general claim) ----
    // MUTATION sol:K-0103 stimulus green: `make MUT=matchinglevels` --
    // bit12 (syncint) decodes to _ipl==2 and bit2 (software INTREQ)
    // decodes to _ipl==6 on this RTL, so both halves of decodes_correctly
    // read true and the assertion flips fully green.
    expect(decodes_correctly, "sol:K-0103");
    // MUTATION opus:K-0120 stimulus green: same as sol:K-0103 (`make MUT=matchinglevels`).
    expect(decodes_correctly, "opus:K-0120");

    // ---- p.394 "only /INT2 and /INT6 ... supported" ----
    // MUTATION sol:K-0160 stimulus green: same as sol:K-0103 (`make MUT=matchinglevels`).
    expect(decodes_correctly, "sol:K-0160");
    // MUTATION opus:K-0168 stimulus green: same as sol:K-0160.
    expect(decodes_correctly, "opus:K-0168");

    // ---- p.411 "two decoded, level-sensitive inputs ... /INT2 and /INT6" ----
    // MUTATION sol:K-0439 stimulus green: same as sol:K-0103 (`make MUT=matchinglevels`).
    expect(decodes_correctly, "sol:K-0439");
    // MUTATION sol:K-0440 stimulus green: same as sol:K-0103 (`make MUT=matchinglevels`).
    expect(decodes_correctly, "sol:K-0440");
    // opus:K-0444 combines the same two-lines-two-levels fact with
    // "...available on the expansion bus", which this module's ports
    // cannot answer (see BOUNDARY above) -- graded on the behavioral half.
    note("opus:K-0444's \"available on the expansion bus\" clause is a "
         "board-wiring fact outside paula_intcontroller's own ports and is "
         "not exercised by this fixture; the graded half is the two-lines, "
         "two-distinct-levels fact below.");
    // MUTATION opus:K-0444 stimulus green: same as sol:K-0103 (`make MUT=matchinglevels`).
    expect(decodes_correctly, "opus:K-0444");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
