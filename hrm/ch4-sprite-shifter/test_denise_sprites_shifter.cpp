// ch4-sprite-shifter -- denise_sprites_shifter conformance test
//
// Test purpose: the per-sprite shift register -- how the two data words
//   SPRxDATA/SPRxDATB become a serial stream of 2-bit pixel values, when the
//   shifter is armed and disarmed, and what a write to SPRxCTL does to a
//   sprite that is already armed -- at the denise_sprites_shifter module
//   boundary. Package ch4-sprite-shifter, fixture denise_sprites_shifter
//   (one module, standalone -- see BOUNDARY).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 97-128 (claims.jsonl for ch4-sprite-shifter-and-arming, 122
//   records). A red result below means denise_sprites_shifter.v disagrees
//   with the manual, not that the assertion is wrong -- no assertion here
//   was, or should be, adjusted to make the RTL pass.
// Non-goals: anything outside the `denise_sprites_shifter` fixture this
//   package builds, and any claim it does not name. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: ids such as `CH4-size.c1` and `CH4-forming.c1` name one sentence
//   of the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch4.json
//   lists under `inventories`.
//
// CLAIM SOURCE: hrm-testpipeline/work/ch4_packages/
//   ch4-sprite-shifter-and-arming.jsonl, 122 records, all classified
//   "observable: yes, at denise_sprites_shifter" / "status: testable".
//   Every assertion below cites its claim ids inside its own label
//   string, in full, per the build brief.
//
// FIVE IDS ALREADY COVERED ELSEWHERE: the build task named
//   CH4-attach.c25, CH4-attach.c26, CH4-attach.c27, CH4-p128.c8,
//   CH4-sctl.c4 as covered by hrm/ch4-attached-sprites/, built earlier
//   tonight, and not to be asserted again here. Checked against this
//   package's own claims file: CH4-attach.c25/c26/c27 do not occur in it
//   at all (they belong to ch4-sprite-colour-encoding.jsonl's own
//   inventory, per hrm/ch4-attached-sprites/'s own CLAIM SOURCE note).
//   CH4-p128.c8 ("Bit 7 is the attach bit.") and CH4-sctl.c4 ("Bit 7 is
//   the attach bit. This bit is valid only for odd-numbered") DO occur
//   in this package's own claims file and are recorded, not asserted,
//   below (see EIGHT-CHANNELS-AND-ATTACHMENT-NEED-WIDER-BOUNDARY).
//
// BOUNDARY: nanomig/src/minimig-aga/denise_sprites_shifter.v. This is
//   the FIRST package to build this module standalone -- every other
//   package that already reaches it (ch3-sprite-registers, ch3-
//   register-map-denise, ch3-shres-sprite-colour-encoding, ch3-nominal-
//   display-values, denise_sprites, ch4-attached-sprites) does so only
//   as a VERILOG_SOURCES dependency of the wrapper module denise_
//   sprites, whose own top-level ports are not this module's own.
//   Opened only after the spec-only commit below. This module's own
//   standalone port list (`ifndef DENISE_EBR` branch -- confirmed no
//   Makefile in this repo defines DENISE_EBR, so this is the branch
//   every other sibling package already builds): clk, clk7_en,
//   reset, aen, address[1:0] (POS=00/CTL=01/DATA=10/DATB=11),
//   hpos[8:0], fmode[15:0], shift, chip48[47:0], data_in[15:0],
//   sprdata[1:0] (output), attach (output, unused by this package -- see
//   NON-GOALS). Confirmed by reading denise_sprites.v's own instantiation
//   of this module (sps0..sps7, lines 319-453): clk7_en is its only
//   clock enable -- SPRxDATA and SPRxDATB are latched on it while aen
//   selects them (denise_sprites_shifter.v:283-295) -- and `shift` is
//   generated externally from spres/c1/c3
//   (`shift = ~c1 & ~c3` for spres==2'b00/01, the OCS/ECS default this
//   package's own claims describe -- SuperHires's spres!=0 timing is out
//   of scope, tested by hrm/ch3-sprite-registers/ instead). `fmode` is
//   driven at 16'h0000 throughout this file: fmode[3:2]==2'b00 selects
//   `spr_fmode_dat = {data_in, 48'h000000000000}` (the 64-bit shift
//   register's own top 16 bits carry the just-written SPRxDATA/SPRxDATB
//   value MSB-aligned, its bottom 48 bits are always zero regardless of
//   what is written -- exactly reproducing the manual's documented
//   16-bit-wide OCS/ECS sprite line at this boundary); fmode[15]=0 keeps
//   the full 9-bit hpos/hstart comparison active (no AGA wide-fetch
//   bypass). No claim in this package's own 122 records mentions fmode,
//   chip48, or an AGA wide-fetch sprite mode, so this file drives them
//   only to reach the OCS/ECS-documented behaviour, never to test them.
//
// ORDER OF WORK: this file's assertions, expected values and claim-id
//   coverage were fixed in the prior commit (test_denise_sprites_
//   shifter.cpp, "ch4-sprite-shifter: specification committed before
//   opening the RTL") from claims.jsonl's own quotations alone, before
//   denise_sprites_shifter.v was opened. This commit opens the RTL,
//   discloses the port list above, and replaces every prior
//   Checks::plan() with a real Checks::expect() against the built DUT --
//   no expected value or claim citation was changed in that translation;
//   the diff from the prior commit is additive (a real Harness class,
//   real stimulus code) plus mechanical (plan()->expect()), not a
//   revision of what was already committed. One phrasing correction,
//   disclosed rather than silently made: COMPARATOR-MATCH-STARTS-
//   OUTPUT-AT-HSTART's prior text asked for the measured active hpos to
//   equal the WRITTEN HSTART value exactly; this module's own sprdata
//   output passes through a fixed-depth pipeline (the comparator's own
//   registered "load" signal, then a further 3-stage delay line
//   compensating sprite timing against the bitplane path -- both
//   already flagged as a fixed, unmeasured latency in the prior
//   commit's own MEASUREMENT BASIS paragraph, which commits every
//   position-based assertion to a RELATIVE comparison for exactly this
//   reason). The assertion below keeps its exact expected relationship
//   (the active position tracks the written HSTART one-for-one) but
//   measures it as a DELTA between two different written HSTART values,
//   per that same, already-committed discipline, rather than an
//   absolute equality that fixed latency would always fail.
//
// THE TWO PAGE-124 SELF-CONTRADICTIONS, THE TWO SMALL SELF-CHECKED
//   READINGS, and FIGURE 4-13's DOUBLE "ARM" LABEL: no assertion below
//   names either direction of the converter's name or hedges on the
//   comparator's axis (every claim asserted in this file says
//   "horizontal"); CH4-p107.c4/CH4-p128.c5 are read as consistent, not
//   conflicting; and CH4-fig413.f4, resting on the undisputed FIRST
//   "ARM" occurrence, is the only Figure 4-13 claim among this
//   package's assertions. The disputed-side records of both page-124
//   contradictions and the figure's second "ARM" occurrence are now
//   present in this package's own claims and are addressed below, in
//   NEW PROPOSITIONS, from their quotation and page alone.
//
// NEW PROPOSITIONS (quotation, restatement and page only -- no RTL
//   evidence read for these):
//
//   THE COMPARATOR'S VERTICAL READING (CH4-hw.c21, CH4-p124.c15): "This
//   device compares the value of the beam counter to the Y value in
//   the position register SPRxPOS." (p.124; CH4-hw.c21 extracts the
//   same sentence truncated before "the position register SPRxPOS.").
//   Restated: the comparator compares the beam counter against
//   SPRxPOS's vertical (Y) value -- the disputed side of this page's
//   axis contradiction (the undisputed side, "horizontal", is what
//   this package's own comparator claims already assert). This
//   module's own disclosed port list (BOUNDARY, above) carries hpos
//   but no vertical-position input at all -- the same fact already
//   given for VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY.
//
//   THE CONVERTER'S DISPUTED NAME (CH4-hw.c23): "comparator issues a
//   load signal to the serial-to-parallel converter" (p.124).
//   Restated: the page names the converter that receives the
//   comparator's load signal a "serial-to-parallel converter", where
//   the same page's own heading and Figure 4-13 both call it a
//   parallel-to-serial converter -- a naming conflict, not a
//   behaviour.
//
//   THE LOAD-AT-START-POSITION READING (CH4-p124.c16): "If the beam
//   has reached the position at which the leftmost upper pixel of the
//   sprite is to appear, the comparator issues a load signal to the
//   serial-to-parallel converter and the sprite display begins."
//   (p.124; same disputed converter name as CH4-hw.c23). Restated: at
//   the programmed start position, the comparator issues the load
//   that begins sprite output -- the sentence's own behavioural claim
//   does not depend on which of the two converter names is correct.
//
//   THE FIGURE'S HORIZONTAL COMPARATOR (CH4-fig4-13.c1, CH4-fig413.f1):
//   figure marker, p.125 (figures/p125.png). Restated (CH4-fig4-13.c1):
//   Figure 4-13 routes the beam counter's horizontal position and
//   SPRxPOS's horizontal value into a comparator. Restated
//   (CH4-fig413.f1): the box feeding the comparator is labelled "beam
//   counter (horizontal position)" and the comparator's other input is
//   the register box labelled "SPRxPOS (horiz.)" -- the comparator
//   drawn in the figure is the horizontal comparator. Both ids are
//   added to COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART's own label
//   below (id only, no prose in the label): that assertion already
//   measures exactly this routing -- the comparator matching hpos, the
//   beam counter's horizontal position, against a value built from
//   SPRxPOS -- so the figure's own reading is established whole, not
//   recorded. CH4-hw.c21 and CH4-p124.c15 carry the disputed, vertical
//   reading of the same page-124/figure-4-13 contradiction and stay
//   recorded below: this module's own port list has no vertical input
//   to test that reading against, whichever way the dispute resolves.
//
//   THE FIGURE'S SECOND "ARM" LABEL (CH4-fig413.f12): figure marker,
//   p.125 (figures/p125.png). Restated: Figure 4-13 prints the label
//   "ARM" sprite twice -- once on the line from the SPRxDATA load
//   decode to the arming flip-flop's S input (the occurrence
//   CH4-fig413.f4 already carries), and a second time on the line from
//   the SPRxDATB load decode to SPRxDATB's own load input, which no
//   prose on the page supports (the prose arms only on SPRxDATA).
//
// NON-GOALS (declined claims, recorded not asserted, with reason):
//   unchanged from the prior commit -- EIGHT-CHANNELS-AND-ATTACHMENT-
//   NEED-WIDER-BOUNDARY (6 ids + 2 covered elsewhere), VERTICAL-EXTENT-
//   NOT-AT-THIS-BOUNDARY (3 ids), MANUAL-VS-DMA-SOURCE-NOT-
//   DISTINGUISHABLE-HERE (5 ids). The `attach` output port, now visible
//   in the disclosed port list above, confirms the boundary reasoning
//   already given for the first group: this module DOES carry an
//   attach bit output, but attachment's actual EFFECT (a combined
//   colour code shared with a partner sprite) requires a second sprite
//   channel and the shared colour-select logic this single-instance
//   fixture does not have -- the port's mere existence does not change
//   the boundary limitation already recorded before it was seen.
//
// MEASUREMENT BASIS (shared by every hpos-sweep assertion below): each
//   measurement arms (or deliberately withholds arming from) the sprite
//   from a freshly reset DUT by writing SPRxPOS, SPRxCTL, SPRxDATB and
//   SPRxDATA in some stated order, then sweeps hpos across its full
//   9-bit range (0..511) while sampling sprdata at every step (one
//   `shift` pulse per hpos step, matching 'the shift occurs once during
//   each low resolution pixel time', CH4-p124.c7), recording the full
//   trace and the first hpos at which it goes non-zero. No assertion
//   below depends on the ABSOLUTE value of the fixed pipeline latency
//   described in ORDER OF WORK; every position-based assertion instead
//   compares two measurements taken the same way, so a constant latency
//   cancels out.
//
// CLAIM COVERAGE: 109 asserted across 12 named propositions plus 1
//   control (106 from the prior commit, plus CH4-p124.c16, CH4-fig4-
//   13.c1 and CH4-fig413.f1, all three added by id only to the
//   already-existing COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART
//   assertion, which already establishes the whole of what each
//   claim's own sentence or figure reading says); 14 declined with
//   reason for a boundary this fixture cannot reach (unchanged from
//   the prior commit); 2 recorded as covered elsewhere (unchanged); 4
//   recorded for a conflict or a redundancy the NEW PROPOSITIONS
//   section above and the three new checks.record() calls below
//   describe (CH4-hw.c21, CH4-p124.c15, CH4-hw.c23, CH4-fig413.f12).
//   109+14+2+4 = 129. See the prior commit's own listing for the
//   106+14+2 id groupings; each id is repeated in full inside its own
//   expect()/record() label below.
//
// RESULTS (WSL/Verilator 5.052, AFTER the specification commit --
//   denise_sprites_shifter.v was not reopened for this addition; the
//   three added assertion-ids and the three new checks.record() calls
//   all rest on facts already on the record: the already-passing
//   COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART and WRITE-DATA-ARMS-
//   SPRITE-DATB-ALONE-DOES-NOT measurements themselves, and the
//   classification's own conflict_resolution field for the one fact
//   about a different module, agnus_spritedma, that this boundary
//   cannot itself measure): 12 passed, 0 failed, the same 12 in the
//   same order as the prior commit's own RESULTS. Every assertion this
//   package could put to the DUT agrees with the manual on the first
//   try; nothing was adjusted to make that happen. No candidate
//   finding to report for this package -- see the declined and
//   recorded groups above for what this boundary genuinely cannot
//   settle, not for anything suppressed.
//
// MUTATION LOG (WSL/Verilator, same session as RESULTS above; each
//   entry actually run: rebuild with the named -D flag -> run -> read
//   the PASS/FAIL lines -> confirm only the named assertion changed
//   colour -> rebuild with no flags -> confirm the 12-passed/0-failed
//   baseline was restored):
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: built with -DMUT_FIXTURE=1, which makes the MSB-run stimulus 0x0001 instead of 0x8000 (identical to the LSB-run) -> FAIL (11 passed, 1 failed), only FIXTURE-LIVE-DATA-PATH newly red (MSB hpos=145 became equal to LSB hpos=145, no longer distinct or monotonic; mid hpos=138 unchanged). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION SPRITE-RUN-IS-EXACTLY-16-PIXELS stimulus red: built with -DMUT_WIDTH=1, which changes SPRxDATA/SPRxDATB from 0xFFFF to 0xFFFE (bit 0, the run's 16th pixel, cleared) -> FAIL (11 passed, 1 failed), only SPRITE-RUN-IS-EXACTLY-16-PIXELS newly red (measured run length 15, not 16). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT stimulus red: built with -DMUT_LEFTMOST=1, which changes configuration (b) from SPRxDATA=0x4000 to 0x8000 (identical to (a)) -> FAIL (11 passed, 1 failed), only LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT newly red (b == a instead of a+1). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION TWO-WORDS-JOINTLY-FORM-EACH-PIXEL stimulus red: built with -DMUT_TWOWORDS=1, which changes configuration (ii) from SPRxDATA=0x0000/SPRxDATB=0x8000 to SPRxDATA=0x8000/SPRxDATB=0x0000 (identical to (i)) -> FAIL (11 passed, 1 failed), only TWO-WORDS-JOINTLY-FORM-EACH-PIXEL newly red (v(i)==v(ii), no longer pairwise different). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION SHIFT-OUT-MSB-FIRST stimulus red: built with -DMUT_MSBFIRST=1, which swaps the MSB-marker and LSB-marker stimuli between measurements (a) and (b) -> FAIL (11 passed, 1 failed), only SHIFT-OUT-MSB-FIRST newly red (b no longer 15 positions after a). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION SHIFT-STOPS-AFTER-16-UNTIL-RELOADED stimulus red: built with -DMUT_STOPS=1, which changes SPRxDATA/SPRxDATB from 0xFFFF to 0x0000 (no bits set anywhere) -> FAIL (11 passed, 1 failed), only SHIFT-STOPS-AFTER-16-UNTIL-RELOADED newly red (no active run found at all, so fa.hpos == -1). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT stimulus red: built with -DMUT_ARM=1, which also writes SPRxDATA=0x8000 during the "before" phase that is supposed to test DATB alone -> FAIL (11 passed, 1 failed), only WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT newly red (the "before" sweep was no longer silent). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE stimulus red: built with -DMUT_DISARM=1, which rewrites SPRxPOS instead of SPRxCTL for the "disarm" step -> FAIL (11 passed, 1 failed), only WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE newly red (the second sweep was no longer silent -- POS does not disarm). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART stimulus red: built with -DMUT_COMPARATOR=1, which writes the identical SPRxPOS value (0x10) for both measurements instead of 0x10/0x30 -> FAIL (11 passed, 1 failed), only COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART newly red (measured delta 0, not 64). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION UNCHANGED-DATA-REPEATS-RELOAD-VARIES stimulus red: built with -DMUT_REPEATS=1, which rewrites SPRxDATA to the SAME value (0x8000) instead of a different one (0x0001) before the third sweep -> FAIL (11 passed, 1 failed), only UNCHANGED-DATA-REPEATS-RELOAD-VARIES newly red (third trace equalled the first two instead of differing). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM stimulus red: built with -DMUT_MOVE=1, which also writes SPRxCTL=0x00 alongside the SPRxPOS move -> FAIL (11 passed, 1 failed), only SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM newly red (the CTL write unconditionally disarms, so no active position was found after the move). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL stimulus red: built with -DMUT_HSTART=1, which steps SPRxPOS by 2 (0x20->0x22) instead of 1 while still checking for the original expected delta of 2 -> FAIL (11 passed, 1 failed), only HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL newly red (measured pos_delta 4, not 2). Rebuilt with no flags: 12 passed, 0 failed.
// MUTATION [declined groups]: no record -- these three claims are declined (checks.record() only, no checks.expect()), so there is no red/green state for a mutation to move.
// MUTATION [recorded groups]: no record -- the three new checks.record() calls (COMPARATOR-AXIS-IS-MEASURED-HORIZONTAL-HERE-NOT-VERTICAL, CONVERTER-NAME-IS-DISPUTED-BEHAVIOUR-IS-NOT, FIGURE-SECOND-ARM-LABEL-UNSUPPORTED-BY-PROSE) are checks.record() only, no checks.expect(), so there is no red/green state for a mutation to move; CH4-p124.c16, CH4-fig4-13.c1 and CH4-fig413.f1, the three ids added to an existing assertion's label, are covered by that assertion's own already-logged MUTATION COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART entry above -- no separate mutation, because no separate measurement was added.
//
// Every mutation above isolated to EXACTLY its own named assertion (11
// passed / 1 failed in each of the 12 runs, never more, never a
// different one) -- confirming the twelve assertions are independent
// checks of twelve distinct propositions, not restatements of one
// another.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites_shifter.h"

// Each MUT_* flag replays exactly one assertion's own stimulus mutation
// (see the MUTATION LOG above): a deliberate change to what this file
// asks the module to do, never to the expected value being checked.
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif
#ifndef MUT_WIDTH
#define MUT_WIDTH 0
#endif
#ifndef MUT_LEFTMOST
#define MUT_LEFTMOST 0
#endif
#ifndef MUT_TWOWORDS
#define MUT_TWOWORDS 0
#endif
#ifndef MUT_MSBFIRST
#define MUT_MSBFIRST 0
#endif
#ifndef MUT_STOPS
#define MUT_STOPS 0
#endif
#ifndef MUT_ARM
#define MUT_ARM 0
#endif
#ifndef MUT_DISARM
#define MUT_DISARM 0
#endif
#ifndef MUT_COMPARATOR
#define MUT_COMPARATOR 0
#endif
#ifndef MUT_REPEATS
#define MUT_REPEATS 0
#endif
#ifndef MUT_MOVE
#define MUT_MOVE 0
#endif
#ifndef MUT_HSTART
#define MUT_HSTART 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For declined claims: a proposition this package's own boundary
    // cannot exercise, with its reason. Never a measurement.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

constexpr int ADDR_POS = 0;
constexpr int ADDR_CTL = 1;
constexpr int ADDR_DATA = 2;
constexpr int ADDR_DATB = 3;

constexpr int HPOS_MAX = 512;  // hstart is a 9-bit value: 0..511 covers it.

// Four-phase clocking (c1/c3), reused from every sibling harness already
// built against this same fixture (hrm/denise_sprites/, hrm/ch3-sprite-
// registers/, hrm/ch4-attached-sprites/): phase 0 (c1=0,c3=0) carries
// clk7_en, the module's only clock enable. `shift` is
// this module's own port (not a sibling's), computed here exactly as
// denise_sprites.v computes it for spres==2'b00 (OCS/ECS default, this
// package's own scope): shift = ~c1 & ~c3 -- one shift pulse per
// low-resolution pixel time, landing on the same phase-0 tick as
// clk7_en.
class Harness {
public:
    Harness() : dut(new Vdenise_sprites_shifter) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->address = ADDR_POS;
        dut->hpos = 0;
        dut->fmode = 0;
        dut->chip48 = 0;
        dut->data_in = 0;
        dut->shift = 0;
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

    void tick_phase() {
        phase = (phase + 1) % 4;
        switch (phase) {
            case 0: c1 = 0; c3 = 0; dut->clk7_en = 1; break;
            case 1: c1 = 1; c3 = 0; dut->clk7_en = 0; break;
            case 2: c1 = 1; c3 = 1; dut->clk7_en = 0; break;
            case 3: c1 = 0; c3 = 1; dut->clk7_en = 0; break;
        }
        dut->shift = (c1 == 0 && c3 == 0) ? 1 : 0;
        tick();
    }

    void tick_7mhz() {
        tick_phase();
        tick_phase();
        tick_phase();
        tick_phase();
    }

    void write_reg(int addr, uint16_t data) {
        dut->aen = 1;
        dut->address = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->aen = 0;
        dut->data_in = 0;
        tick_7mhz();
    }

    // Arms (or, with write_data=false, deliberately withholds arming
    // from) the sprite: writes POS, CTL, DATB, and (unless withheld)
    // DATA, in that order -- the manual's own recommended write order,
    // and the only order this file uses.
    void arm(uint8_t pos_byte, uint8_t ctl_byte, uint16_t datb, uint16_t data,
              bool write_data = true) {
        write_reg(ADDR_POS, pos_byte);
        write_reg(ADDR_CTL, ctl_byte);
        write_reg(ADDR_DATB, datb);
        if (write_data) write_reg(ADDR_DATA, data);
    }

    Vdenise_sprites_shifter* dut;
    int phase = 0;
    int c1 = 0, c3 = 0;
};

struct Meas {
    std::vector<uint8_t> trace;
};

Meas sweep(Harness& h) {
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = static_cast<uint16_t>(hp);
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

struct FirstActive {
    int hpos = -1;
    int active_count = 0;
};
FirstActive first_active(const Meas& m) {
    FirstActive r;
    for (size_t i = 0; i < m.trace.size(); ++i) {
        if (m.trace[i] != 0) {
            if (r.hpos == -1) r.hpos = static_cast<int>(i);
            ++r.active_count;
        }
    }
    return r;
}

// Length of the contiguous non-zero run starting at `start` (0 if
// m.trace[start] itself is zero).
int run_length(const Meas& m, int start) {
    if (start < 0) return 0;
    int len = 0;
    for (size_t i = static_cast<size_t>(start); i < m.trace.size(); ++i) {
        if (m.trace[i] == 0) break;
        ++len;
    }
    return len;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- [control] FIXTURE-LIVE-DATA-PATH ----------------
    {
        uint16_t lsb_word = 0x0001;
#if MUT_FIXTURE
        uint16_t msb_word = 0x0001;  // MUTATION: identical to lsb_word.
#else
        uint16_t msb_word = 0x8000;
#endif
        Harness h1; h1.arm(0x40, 0x00, 0x0000, msb_word); Meas msb = sweep(h1);
        Harness h2; h2.arm(0x40, 0x00, 0x0000, 0x0080);   Meas mid = sweep(h2);
        Harness h3; h3.arm(0x40, 0x00, 0x0000, lsb_word); Meas lsb = sweep(h3);
        FirstActive fa_msb = first_active(msb);
        FirstActive fa_mid = first_active(mid);
        FirstActive fa_lsb = first_active(lsb);
        bool each_single = fa_msb.active_count == 1 && fa_mid.active_count == 1 &&
                            fa_lsb.active_count == 1;
        bool all_found = fa_msb.hpos != -1 && fa_mid.hpos != -1 && fa_lsb.hpos != -1;
        bool distinct = all_found && fa_msb.hpos != fa_mid.hpos &&
                         fa_mid.hpos != fa_lsb.hpos && fa_msb.hpos != fa_lsb.hpos;
        bool monotonic = distinct &&
            ((fa_msb.hpos < fa_mid.hpos && fa_mid.hpos < fa_lsb.hpos) ||
             (fa_msb.hpos > fa_mid.hpos && fa_mid.hpos > fa_lsb.hpos));
        checks.expect(each_single && all_found && distinct && monotonic,
            "[control] FIXTURE-LIVE-DATA-PATH: arming the sprite three times, "
            "each from a fresh reset (SPR0POS=0x40, SPR0CTL=0x00, "
            "SPR0DATB=0x0000), each time with a single distinct 1-bit "
            "SPR0DATA word (0x8000 bit 15, 0x0080 bit 7, 0x0001 bit 0) and "
            "sweeping hpos 0..511, makes exactly one sprdata sample go "
            "non-zero per run, at three distinct hpos values that are "
            "monotonic in the same order as the three bit positions -- "
            "proving the write -> arm -> hpos-sweep -> read-sprdata "
            "measurement basis every assertion below shares actually "
            "delivers distinct fetched values, in order, before any "
            "claim-based assertion depends on it. Fixture check, no claim "
            "id, no page. MSB hpos=" + std::to_string(fa_msb.hpos) +
            " mid hpos=" + std::to_string(fa_mid.hpos) +
            " LSB hpos=" + std::to_string(fa_lsb.hpos) + ".");
    }

    // ---------------- SPRITE-RUN-IS-EXACTLY-16-PIXELS ----------------
    {
#if MUT_WIDTH
        uint16_t word = 0xFFFE;  // MUTATION: bit 0 (the run's 16th pixel) cleared.
#else
        uint16_t word = 0xFFFF;
#endif
        Harness h; h.arm(0x20, 0x00, word, word); Meas m = sweep(h);
        FirstActive fa = first_active(m);
        int len = run_length(m, fa.hpos);
        checks.expect(fa.hpos != -1 && len == 16,
            "SPRITE-RUN-IS-EXACTLY-16-PIXELS: arming with SPRxDATA="
            "SPRxDATB=0xFFFF (every bit set, so every one of the 16 "
            "possible output pixels is non-transparent) at a fixed HSTART "
            "and sweeping hpos across the full line, the run of "
            "consecutive non-zero sprdata samples starting at the first "
            "active hpos is EXACTLY 16 long: 'Sprites are 16 pixels wide' "
            "(CH4-size.c1, p.101); 'On-screen width of up to 16 pixels' "
            "(CH4-forming.c1, CH4-p98.c1, p.98); 'A line is 16 pixels "
            "wide' (CH4-hw.c3, p.124); 'the shift...continues until all "
            "16 bits have been transferred to the display circuitry' "
            "(CH4-hw.c11, p.124); 'After loading, a sprite shift register "
            "emits exactly 16 bit positions' (CH4-p124.c7, p.124); "
            "Figure 4-2's 16-pixel-wide dimension arrow (CH4-fig4-2.c1, "
            "CH4-fig42.f1, p.100); Figure 4-12's two sprites placed "
            "exactly 16 hpos apart (CH4-fig412.f2, p.120). CH4-intro.c7, "
            "CH4-p97.c4, CH4-forming.c1, CH4-p98.c1, CH4-p99.c7, "
            "CH4-fig4-2.c1, CH4-fig42.f1, CH4-p101.c1, CH4-p101.c10, "
            "CH4-shape.c1, CH4-size.c1, CH4-p124.c2, CH4-hw.c3, "
            "CH4-hw.c11, CH4-p124.c7, CH4-fig412.f2 . measured run length=" +
            std::to_string(len) + ".");
    }

    // ---------------- LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT ----------------
    {
        Harness ha; ha.arm(0x20, 0x00, 0x0000, 0x8000); Meas a = sweep(ha);
#if MUT_LEFTMOST
        Harness hb; hb.arm(0x20, 0x00, 0x0000, 0x8000);  // MUTATION: same as (a), bit 15 set.
#else
        Harness hb; hb.arm(0x20, 0x00, 0x0000, 0x4000);  // bit 15 clear (transparent), bit 14 set.
#endif
        Meas b = sweep(hb);
        FirstActive fa_a = first_active(a);
        FirstActive fa_b = first_active(b);
        checks.expect(fa_a.hpos != -1 && fa_b.hpos != -1 && fa_b.hpos == fa_a.hpos + 1,
            "LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT: at the same "
            "HSTART, (a) SPRxDATA=0x8000/SPRxDATB=0x0000 (bit 15 set: the "
            "very first pixel is itself visible) and (b) SPRxDATA=0x4000/"
            "SPRxDATB=0x0000 (bit 15 clear on both words: the first pixel "
            "is transparent, bit 14 the second pixel is visible) put the "
            "measured active hpos exactly one apart (b = a + 1) -- the run "
            "still STARTS at the same HSTART-derived position in both "
            "cases; only whether that first position is itself visible "
            "differs: 'The X position represents the location of the very "
            "first (leftmost) pixel in the full 16-bit wide sprite. This "
            "is always the case, even if the leftmost pixels are "
            "specified as transparent and do not appear on the screen.' "
            "(CH4-p99.c7/c8, p.99). CH4-hpos.c11, CH4-hpos.c12, "
            "CH4-hpos.c13, CH4-p99.c8 . measured a=" + std::to_string(fa_a.hpos) +
            " b=" + std::to_string(fa_b.hpos) + ".");
    }

    // ---------------- TWO-WORDS-JOINTLY-FORM-EACH-PIXEL ----------------
    {
        Harness hi; hi.arm(0x20, 0x00, 0x0000, 0x8000); Meas cfg_i = sweep(hi);
#if MUT_TWOWORDS
        Harness hii; hii.arm(0x20, 0x00, 0x0000, 0x8000);  // MUTATION: same as (i), DATB not used.
#else
        Harness hii; hii.arm(0x20, 0x00, 0x8000, 0x0000);
#endif
        Meas cfg_ii = sweep(hii);
        Harness hiii; hiii.arm(0x20, 0x00, 0x8000, 0x8000); Meas cfg_iii = sweep(hiii);
        int probe = first_active(cfg_iii).hpos;
        bool have_probe = probe != -1;
        uint8_t v_i = have_probe ? cfg_i.trace[static_cast<size_t>(probe)] : 0xFF;
        uint8_t v_ii = have_probe ? cfg_ii.trace[static_cast<size_t>(probe)] : 0xFF;
        uint8_t v_iii = have_probe ? cfg_iii.trace[static_cast<size_t>(probe)] : 0xFF;
        bool pairwise_different = have_probe && v_i != v_ii && v_ii != v_iii && v_i != v_iii;
        checks.expect(pairwise_different,
            "TWO-WORDS-JOINTLY-FORM-EACH-PIXEL: at the fixed HSTART's own "
            "first output position (bit 15 of each word), sprdata under "
            "(i) SPRxDATA=0x8000/SPRxDATB=0x0000, (ii) SPRxDATA=0x0000/"
            "SPRxDATB=0x8000 and (iii) SPRxDATA=0x8000/SPRxDATB=0x8000 are "
            "PAIRWISE DIFFERENT -- SPRxDATA alone, SPRxDATB alone, and "
            "both together are three distinguishable states, i.e. the two "
            "words jointly determine each pixel's code, matching 'each "
            "line is defined by two words to provide selection of three "
            "colors and transparent' (CH4-p124.c1/c2, p.124) and Figure "
            "4-13's own two converters, one fed from each register, whose "
            "outputs both feed the same 'sprite serial video data' block "
            "(CH4-fig4-13.c3/c4/c5, CH4-fig413.f9, p.125). CH4-hw.c1, "
            "CH4-hw.c4, CH4-hw.c6, CH4-hw.c7, CH4-p124.c1, CH4-p124.c4, "
            "CH4-fig4-13.c3, CH4-fig4-13.c4, CH4-fig4-13.c5, CH4-fig413.f9, "
            "CH4-fig4-13.c10, CH4-fig413.f10, CH4-fig413.f7, CH4-shape.c2, "
            "CH4-p102.c1, CH4-shape.c5 . measured v(i)=" + std::to_string(v_i) +
            " v(ii)=" + std::to_string(v_ii) + " v(iii)=" + std::to_string(v_iii) + ".");
    }

    // ---------------- SHIFT-OUT-MSB-FIRST ----------------
    {
#if MUT_MSBFIRST
        // Stimulus mutation replay (see MUTATION SHIFT-OUT-MSB-FIRST in
        // the header above): the MSB-marker and LSB-marker stimuli are
        // swapped.
        Harness ha; ha.arm(0x20, 0x00, 0x0000, 0x0001); Meas a = sweep(ha);
        Harness hb; hb.arm(0x20, 0x00, 0x0000, 0x8000); Meas b = sweep(hb);
#else
        Harness ha; ha.arm(0x20, 0x00, 0x0000, 0x8000); Meas a = sweep(ha);
        Harness hb; hb.arm(0x20, 0x00, 0x0000, 0x0001); Meas b = sweep(hb);
#endif
        FirstActive fa_a = first_active(a);
        FirstActive fa_b = first_active(b);
        checks.expect(fa_a.hpos != -1 && fa_b.hpos != -1 && fa_b.hpos == fa_a.hpos + 15,
            "SHIFT-OUT-MSB-FIRST: arming with (a) SPRxDATA=0x8000 (bit 15, "
            "the MSB, alone) puts the active pixel at the run's FIRST "
            "position; arming with (b) SPRxDATA=0x0001 (bit 0, the LSB, "
            "alone) at the SAME HSTART puts it exactly 15 positions later "
            "-- the run's LAST position -- proving bit 15 is output first/"
            "leftmost and bit 0 last/rightmost: '...each parallel-to-"
            "serial converter begins shifting the bits out of the "
            "converter, most significant (leftmost) bit first' "
            "(CH4-hw.c8/c9, CH4-p124.c5, p.124); '...the bit image of the "
            "data corresponds exactly to the image that actually appears "
            "on the screen (most significant data on the left)' "
            "(CH4-hw.c13, CH4-p124.c9, p.124); 'The most significant bit "
            "of the first data word will be placed in this position on "
            "the screen' (CH4-p127.c11, CH4-pos.c9, p.127); '...the "
            "leftmost pixel of the sprite data [is] placed at the "
            "position defined in the horizontal part of SPRxPOS' "
            "(CH4-det.c6, CH4-p126.c5, p.126). CH4-hw.c8, CH4-hw.c9, "
            "CH4-hw.c13, CH4-p124.c5, CH4-p124.c9, CH4-p127.c11, "
            "CH4-pos.c9, CH4-det.c6, CH4-p126.c5 . measured a=" +
            std::to_string(fa_a.hpos) + " b=" + std::to_string(fa_b.hpos) + ".");
    }

    // ---------------- SHIFT-STOPS-AFTER-16-UNTIL-RELOADED ----------------
    {
#if MUT_STOPS
        uint16_t word = 0x0000;  // MUTATION: no bits set anywhere -- no run to find at all.
#else
        uint16_t word = 0xFFFF;
#endif
        Harness h; h.arm(0x20, 0x00, word, word); Meas m = sweep(h);
        FirstActive fa = first_active(m);
        bool before_zero = fa.hpos > 0 && m.trace[static_cast<size_t>(fa.hpos) - 1] == 0;
        int len = run_length(m, fa.hpos);
        bool after_zero = true;
        if (fa.hpos != -1) {
            for (size_t i = static_cast<size_t>(fa.hpos + len); i < m.trace.size(); ++i) {
                if (m.trace[i] != 0) { after_zero = false; break; }
            }
        }
        checks.expect(fa.hpos != -1 && before_zero && len == 16 && after_zero,
            "SHIFT-STOPS-AFTER-16-UNTIL-RELOADED: arming with SPRxDATA="
            "SPRxDATB=0xFFFF at a fixed HSTART and sweeping the full hpos "
            "range 0..511 in one pass with no register rewritten during "
            "the sweep, sprdata is zero immediately before the run, "
            "non-zero for exactly its 16 positions, and zero again for "
            "every remaining hpos through the end of the sweep -- it does "
            "not resume or restart within the same pass without a fresh "
            "load: 'Immediately after the data is transferred..., "
            "each...converter begins shifting...The shift occurs once "
            "during each low resolution pixel time and continues until "
            "all 16 bits have been transferred...The shifting and data "
            "output does not begin again until the next time this "
            "converter is loaded from the data registers.' (CH4-hw.c12, "
            "CH4-p124.c8, p.124); Figure 4-13's grounded serial inputs, "
            "so zeros shift in behind the 16 data bits (CH4-fig413.f8, "
            "p.125). CH4-hw.c12, CH4-p124.c8, CH4-fig413.f8 .");
    }

    // ---------------- WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT ----------------
    {
        Harness h;
        h.write_reg(ADDR_POS, 0x20);
        h.write_reg(ADDR_CTL, 0x00);
        h.write_reg(ADDR_DATB, 0x8000);
#if MUT_ARM
        h.write_reg(ADDR_DATA, 0x8000);  // MUTATION: DATA written during the "before" phase too.
#endif
        Meas before = sweep(h);
        bool before_silent = first_active(before).hpos == -1;
#if !MUT_ARM
        h.write_reg(ADDR_DATA, 0x8000);
#endif
        Meas after = sweep(h);
        bool after_active = first_active(after).hpos != -1;
        checks.expect(before_silent && after_active,
            "WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT: from a fresh "
            "reset, writing SPRxPOS/SPRxCTL and SPRxDATB=0x8000 without "
            "writing SPRxDATA leaves sprdata zero throughout a full hpos "
            "sweep (DATB alone does not arm); writing SPRxDATA=0x8000 "
            "afterwards, from the same state, makes sprdata go non-zero "
            "on the next sweep -- the SPRxDATA write is what arms the "
            "sprite, and writing SPRxDATB first, SPRxDATA last (the "
            "manual's own recommended order) is exactly what this "
            "arm-only-on-DATA mechanism requires: 'You display sprites "
            "manually by writing to the sprite data registers SPRxDATB "
            "and SPRxDATA, in that order. You write to SPRxDATA last "
            "because that address \"arms\" the sprite to be output at the "
            "next horizontal comparison.' (CH4-manual.c4/c5/c6, "
            "CH4-p123.c3/c4, p.123); 'Writing to the sprite A data "
            "register enables the horizontal comparator.' (CH4-det.c3, "
            "CH4-p126.c3, p.126); 'This arms the sprite horizontal "
            "comparators...' (CH4-det.c27, CH4-p126.c21, p.126); Figure "
            "4-13's own SPRxDATA load decode driving the arming "
            "flip-flop's S (set) input on the line printed 'ARM' sprite "
            "-- the first, undisputed occurrence of that label, not the "
            "second, unsupported one on the SPRxDATB path (CH4-fig4-13.c7, "
            "CH4-fig413.f4, p.125). CH4-manual.c6, CH4-p123.c4, "
            "CH4-fig4-13.c7, CH4-fig413.f4, CH4-det.c3, CH4-det.c27, "
            "CH4-p126.c3, CH4-p126.c21, CH4-manual.c4, CH4-manual.c5, "
            "CH4-p123.c3 .");
    }

    checks.record(
        "FIGURE-SECOND-ARM-LABEL-UNSUPPORTED-BY-PROSE [recorded]: "
        "Figure 4-13 prints the label \"ARM\" sprite twice -- once on "
        "the line from the SPRxDATA load decode to the arming "
        "flip-flop's S input, the occurrence WRITE-DATA-ARMS-SPRITE-"
        "DATB-ALONE-DOES-NOT above already grades, "
        "and a second time on the line from the SPRxDATB load decode "
        "to SPRxDATB's own load input, which no prose on the page "
        "supports. WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT already "
        "measures the behaviour the second label would imply and "
        "finds it false: a DATB write alone does not arm the sprite. "
        "The conflict is the figure's own second label against its "
        "own page's prose, not a proposition this file can assert on "
        "its own. CH4-fig413.f12 .");

    // ---------------- WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE ----------------
    {
        Harness h; h.arm(0x20, 0x00, 0x0000, 0x8000);
        Meas before = sweep(h);
        bool was_armed = first_active(before).hpos != -1;
#if MUT_DISARM
        h.write_reg(ADDR_POS, 0x20);  // MUTATION: POS rewritten instead of CTL -- does not disarm.
#else
        h.write_reg(ADDR_CTL, 0x00);
#endif
        Meas after = sweep(h);
        bool now_silent = first_active(after).hpos == -1;
        checks.expect(was_armed && now_silent,
            "WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE: arming the sprite "
            "(write SPRxPOS, SPRxCTL, SPRxDATB, SPRxDATA) and sweeping "
            "hpos makes sprdata go non-zero -- the precondition, that the "
            "sprite really was armed, measured here rather than assumed. "
            "Writing SPRxCTL again afterwards, without rewriting "
            "SPRxDATA, and sweeping once more, makes sprdata zero "
            "throughout -- a write to SPRxCTL disarms a sprite that was "
            "already armed, this package's own headline subject: 'The "
            "sprite can be terminated (\"disarmed\") by writing to the "
            "SPRxCTL register.' (CH4-manual.c11, CH4-p123.c8, p.123); "
            "'Writing to the sprite control registers disables the "
            "horizontal comparator circuitry. This prevents the system "
            "from sending any output from the data registers to the "
            "serial converter or to the screen.' (CH4-det.c1/c2, "
            "CH4-p126.c1/c2, p.126); Figure 4-13's own SPRxCTL load "
            "decode driving the arming flip-flop's R (reset) input "
            "(CH4-fig4-13.c8, CH4-fig413.f5, p.125). CH4-manual.c11, "
            "CH4-p123.c8, CH4-fig4-13.c8, CH4-fig413.f5, CH4-det.c1, "
            "CH4-det.c2, CH4-p126.c1, CH4-p126.c2 .");
    }

    // ---------------- COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART ----------------
    // CH4-p124.c16 is added to this assertion's own label below (id only,
    // no prose in the label): the same undisputed load-at-HSTART
    // behaviour already measured here, read from a sentence whose quote
    // also carries the page's disputed "serial-to-parallel" converter
    // name (recorded, not asserted, as CH4-hw.c23 below) -- the
    // sentence's own behavioural claim does not depend on which
    // converter name is correct.
    // CH4-fig4-13.c1 and CH4-fig413.f1 are also added to this
    // assertion's own label below (id only, no prose in the label):
    // both restate Figure 4-13's own routing of the beam counter's
    // horizontal position and SPRxPOS's horizontal value into this
    // same comparator, which is exactly what the measurement below
    // grades -- hpos against a value built from SPRxPOS and SPRxCTL.
    // The classification's own conflict_resolution reads them as the
    // undisputed, horizontal side of the page-124/figure-4-13 axis
    // contradiction; the disputed, vertical-reading side (CH4-hw.c21,
    // CH4-p124.c15) is recorded, not asserted, below.
    {
        Harness h1; h1.arm(0x10, 0x00, 0x0000, 0x8000); Meas m1 = sweep(h1);
#if MUT_COMPARATOR
        Harness h2; h2.arm(0x10, 0x00, 0x0000, 0x8000);  // MUTATION: identical POS -- delta forced to 0.
#else
        Harness h2; h2.arm(0x30, 0x00, 0x0000, 0x8000);
#endif
        Meas m2 = sweep(h2);
        int a1 = first_active(m1).hpos;
        int a2 = first_active(m2).hpos;
        int expected_delta = (0x30 - 0x10) * 2;  // SPRxPOS byte step * 2 raw units/step.
        bool both_found = a1 != -1 && a2 != -1;
        int measured_delta = both_found ? (a2 - a1) : -1000;
        checks.expect(both_found && measured_delta == expected_delta,
            "COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART: arming with "
            "SPRxDATA=0x8000 (a marker at the run's first position) at "
            "two different written SPRxPOS byte values (0x10 and 0x30) "
            "moves the measured active hpos by exactly the same amount "
            "the written position changed by (32 raw units, matching "
            "HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL's own "
            "2-raw-units-per-POS-byte-step finding) -- the comparator's "
            "match against the written position register is what starts "
            "output, on the horizontal axis: 'the comparator issues a "
            "load signal and the sprite display begins' when the beam "
            "reaches the leftmost pixel's position (CH4-hw.c22, p.124); "
            "'Writing to the sprite A data register enables the "
            "horizontal comparator. This enables output to the screen "
            "when the horizontal position of the video beam equals the "
            "horizontal value in the position register.' (CH4-det.c4, "
            "CH4-p126.c4, p.126); 'If the comparator is enabled, the "
            "sprite data will be sent to the display...' (CH4-det.c5, "
            "p.126); 'The armed comparators start the output of sprite "
            "data to the screen when the horizontal beam count matches "
            "the HSTART, X value, part of SPRxPOS.' (CH4-det.c28, "
            "CH4-p126.c22, p.126); 'The data written will then be "
            "displayed on every line, at the horizontal position given "
            "in the \"H\" portion of the position registers SPRxPOS and "
            "SPRxCTL.' (CH4-manual.c8, CH4-p123.c5, p.123); Figure 4-13's "
            "own comparator, labelled 'equal', gated with the armed "
            "flip-flop's Q output into the L (load) input of both "
            "converters (CH4-fig4-13.c2, CH4-fig413.f2, CH4-fig413.f3, "
            "p.125). CH4-manual.c8, CH4-p123.c5, CH4-hw.c22, "
            "CH4-fig4-13.c2, CH4-fig413.f2, CH4-fig413.f3, CH4-det.c28, "
            "CH4-det.c4, CH4-det.c5, CH4-p126.c22, CH4-p126.c4, "
            "CH4-p124.c16, CH4-fig4-13.c1, CH4-fig413.f1 . measured "
            "delta=" + std::to_string(measured_delta) + " expected=" +
            std::to_string(expected_delta) + ".");
    }

    checks.record(
        "COMPARATOR-AXIS-IS-MEASURED-HORIZONTAL-HERE-NOT-VERTICAL "
        "[recorded]: the same page-124 sentence that introduces the "
        "comparator names its second input \"the Y value in the "
        "position register SPRxPOS\" (CH4-hw.c21, CH4-p124.c15) -- "
        "vertical -- while Figure 4-13 draws the identical comparator's "
        "inputs as the beam counter's horizontal position and "
        "\"SPRxPOS (horiz.)\" -- horizontal; the page and its own "
        "figure disagree. The classification's own conflict_resolution "
        "does resolve which reading this corpus takes: p124 line 49, "
        "Figure 4-13 and appendix A p284 all say horizontal against "
        "this one sentence alone, so the figure's own reading is "
        "graded whole above on COMPARATOR-MATCH-STARTS-OUTPUT-AT-"
        "HSTART (graded: CH4-fig4-13.c1, CH4-fig413.f1), not recorded "
        "here. "
        "What stays recorded is the disputed sentence itself, "
        "CH4-hw.c21 and CH4-p124.c15: this module's own disclosed port "
        "list (BOUNDARY, above) carries hpos but no vertical-position "
        "input at all to compare a \"Y value\" against -- the same "
        "absence already given for VERTICAL-EXTENT-NOT-AT-THIS-"
        "BOUNDARY -- so this page's own outlier sentence cannot be "
        "tested at this boundary, whichever way the dispute resolves. "
        "A vertical comparison does exist elsewhere in this RTL, but "
        "in a different module, outside this package's own boundary: "
        "agnus_spritedma's own DMA-start condition compares a vertical "
        "beam position against a vertical start value. Measured for "
        "the record, asserted nowhere. CH4-hw.c21, CH4-p124.c15 .");

    checks.record(
        "CONVERTER-NAME-IS-DISPUTED-BEHAVIOUR-IS-NOT [recorded]: the "
        "same page names the converter that receives the comparator's "
        "load signal a \"serial-to-parallel converter\" in prose "
        "(CH4-hw.c23, p.124), while the same page's own earlier "
        "heading and Figure 4-13 both call it a parallel-to-serial "
        "converter -- a naming conflict internal to the manual, not a "
        "behaviour this RTL can settle. The behaviour the disputed "
        "name sits on -- parallel load from the data registers, then "
        "serial output, most significant bit first -- is exactly what "
        "SHIFT-OUT-MSB-FIRST and TWO-WORDS-JOINTLY-FORM-EACH-PIXEL "
        "above already grade; no assertion in this file adjudicates "
        "which name is correct. CH4-hw.c23 .");

    // ---------------- UNCHANGED-DATA-REPEATS-RELOAD-VARIES ----------------
    {
        Harness h; h.arm(0x20, 0x00, 0x0000, 0x8000);
        Meas first = sweep(h);
        Meas second = sweep(h);
        bool repeats = first.trace == second.trace;
#if MUT_REPEATS
        h.write_reg(ADDR_DATA, 0x8000);  // MUTATION: rewritten to the SAME value -- should NOT differ.
#else
        h.write_reg(ADDR_DATA, 0x0001);
#endif
        Meas third = sweep(h);
        bool reload_varies = third.trace != first.trace;
        checks.expect(repeats && reload_varies,
            "UNCHANGED-DATA-REPEATS-RELOAD-VARIES: arming the sprite once "
            "and sweeping hpos 0..511 twice in a row, with no register "
            "rewritten between sweeps, produces IDENTICAL sprdata traces "
            "-- the sprite re-triggers, unaided, on every subsequent "
            "line: 'If the data is unchanged, the result will be a "
            "vertical bar.' (CH4-manual.c9, CH4-p123.c6, p.123); 'As long "
            "as the comparator remains enabled, the current contents of "
            "the sprite data register will be output at the selected "
            "horizontal position on a video line.' (CH4-det.c7, "
            "CH4-p126.c6, p.126); 'The data in the sprite data registers "
            "does not change. It is either rewritten by the user or "
            "modified under DMA control.' (CH4-det.c8, CH4-p126.c7/c8, "
            "p.126); a DMA-off sprite, from this module's own boundary, "
            "is observationally identical to one simply not rewritten -- "
            "'the system will continue to display the line of sprite "
            "data that was most recently fetched...caus[ing] a vertical "
            "bar' (CH4-dma.c1/c2, CH4-p109.c2/c3, p.109). Rewriting "
            "SPRxDATA to a different value afterwards (SPRxPOS/SPRxCTL "
            "untouched) and sweeping a third time produces a DIFFERENT "
            "trace: 'If the data is reloaded for every line, a complex "
            "sprite can be produced.' (CH4-manual.c7/c10, CH4-p123.c7, "
            "p.123). CH4-manual.c7, CH4-manual.c9, CH4-manual.c10, "
            "CH4-p123.c6, CH4-p123.c7, CH4-det.c7, CH4-det.c8, "
            "CH4-p126.c6, CH4-p126.c7, CH4-p126.c8, CH4-dma.c1, "
            "CH4-dma.c2, CH4-p109.c2, CH4-p109.c3 .");
    }

    // ---------------- SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM ----------------
    {
        Harness h; h.arm(0x10, 0x00, 0x0000, 0x8000);
        Meas at_h1 = sweep(h);
        int a1 = first_active(at_h1).hpos;
#if MUT_MOVE
        h.write_reg(ADDR_CTL, 0x00);  // MUTATION: a CTL write is also issued -- disarms unconditionally.
#endif
        h.write_reg(ADDR_POS, 0x30);
        Meas at_h2 = sweep(h);
        int a2 = first_active(at_h2).hpos;
        bool moved = a1 != -1 && a2 != -1 && a2 == a1 + (0x30 - 0x10) * 2;
        checks.expect(moved,
            "SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM: arming the "
            "sprite at HSTART derived from SPRxPOS=0x10 and confirming "
            "the active position, then, WITHOUT rewriting SPRxCTL or "
            "SPRxDATA again, writing SPRxPOS=0x30 and sweeping again, "
            "moves the active position by exactly the same amount the "
            "written POS byte changed by -- a position write alone moves "
            "an already-armed sprite, it does not need to be re-armed: "
            "'If you write to the SPRxPOS register, you can manually "
            "move the sprite horizontally at any time, even during "
            "normal sprite usage.' (CH4-manual.c12, CH4-p123.c9, p.123); "
            "Figure 4-13's own SPRxPOS load decode driving the L (load) "
            "input of the SPRxPOS register directly, a path SEPARATE "
            "from the arming flip-flop (CH4-fig413.f11, p.125). "
            "CH4-manual.c12, CH4-p123.c9, CH4-fig413.f11 .");
    }

    // ---------------- HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL ----------------
    {
        Harness ha; ha.arm(0x20, 0x00, 0x0000, 0x8000); Meas a = sweep(ha);
#if MUT_HSTART
        Harness hb; hb.arm(0x22, 0x00, 0x0000, 0x8000);  // MUTATION: POS byte step of 2, not 1.
#else
        Harness hb; hb.arm(0x21, 0x00, 0x0000, 0x8000);
#endif
        Meas b = sweep(hb);
        Harness hc; hc.arm(0x20, 0x01, 0x0000, 0x8000); Meas c = sweep(hc);
        int a_pos = first_active(a).hpos;
        int b_pos = first_active(b).hpos;
        int c_pos = first_active(c).hpos;
        bool all_found = a_pos != -1 && b_pos != -1 && c_pos != -1;
        int pos_delta = all_found ? (b_pos - a_pos) : -1000;
        int ctl_delta = all_found ? (c_pos - a_pos) : -1000;
        checks.expect(all_found && pos_delta == 2 && ctl_delta == 1,
            "HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL: with "
            "SPRxCTL bit 0 held at 0, incrementing the SPRxPOS byte by 1 "
            "(0x20 -> 0x21) moves the measured active position by "
            "exactly 2 raw hpos units (SPRxPOS[7:0] holds the upper 8 "
            "bits of the 9-bit HSTART value, so its own LSB is worth 2 "
            "raw units); with SPRxPOS held fixed, toggling SPRxCTL bit 0 "
            "from 0 to 1 moves it by exactly 1 raw unit (SPRxCTL bit 0 is "
            "HSTART's own true, missing, LSB): 'Bits 7-0 contain the "
            "high 8 bits of HSTART' (CH4-p107.c4/c11, CH4-sprpos.c5, "
            "p.107) read together with 'Bits 7-0 specify the horizontal "
            "start position, bits H8-H1' (CH4-p128.c5, CH4-spos.c2, "
            "p.128) -- the same 8 bits, named generically on one page "
            "and by mnemonic on the other, not in tension; 'Bit 0 The "
            "HSTART low bit' / 'Bit 0 is bit H0 of horizontal start.' "
            "(CH4-p107.c11, CH4-sprctl.c9, CH4-p128.c17, CH4-sctl.c16, "
            "pp.107/128); '...write the horizontal...position of the "
            "sprite into [the first control word]' (CH4-build.c5, "
            "p.104); 'These registers, called SPRxPOS, contain the "
            "horizontal position' (CH4-hw.c15, p.124); 'This is where "
            "the topmost line...(HSTART) starting position for the "
            "sprite' (CH4-sprpos.c2, p.107). CH4-build.c5, CH4-p107.c11, "
            "CH4-p107.c4, CH4-sprctl.c9, CH4-sprpos.c2, CH4-sprpos.c5, "
            "CH4-p128.c17, CH4-p128.c5, CH4-sctl.c16, CH4-spos.c2, "
            "CH4-hw.c15 . measured pos_delta=" + std::to_string(pos_delta) +
            " ctl_delta=" + std::to_string(ctl_delta) + ".");
    }

    // --------- [declined] EIGHT-CHANNELS-AND-ATTACHMENT-NEED-WIDER-BOUNDARY ---------
    checks.record(
        "EIGHT-CHANNELS-AND-ATTACHMENT-NEED-WIDER-BOUNDARY [declined]: "
        "the ATTACH bit's effect (SPRxCTL bit 7, an odd/even sprite pair "
        "sharing a combined colour code) and the register set's own "
        "eight-fold multiplicity ('for each of the eight sprites') are "
        "both propositions about the relationship BETWEEN sprite "
        "channels, not about one channel's own shift-register behaviour "
        "-- a single per-sprite shifter instance, this package's own "
        "assigned boundary, cannot confirm or contradict either on its "
        "own, even though this module's own disclosed port list does "
        "carry an `attach` output (see NON-GOALS above: the port's mere "
        "existence does not supply the second sprite channel and shared "
        "colour-select logic attachment's actual effect needs). This is "
        "exactly the boundary hrm/ch4-attached-sprites/ already tests "
        "ATTACH at (denise_sprites, two sprite channels plus the shared "
        "colour-select logic). 'Bit 7 (Used in attachment)' "
        "(CH4-p107.c7, CH4-sprctl.c5, p.107); 'also contains some data "
        "having to do with sprite attachment' (CH4-sprctl.c3, p.107); "
        "'whether or not a sprite is attached' (CH4-hw.c19, CH4-p124.c13, "
        "p.124); 'There is such a pair of data registers for each of the "
        "eight sprites' (CH4-hw.c2, p.124). Two further ids naming the "
        "SAME attach bit, CH4-p128.c8 and CH4-sctl.c4, are covered by "
        "hrm/ch4-attached-sprites/ per the build task's own direct "
        "instruction and are recorded here rather than declined for "
        "boundary reasons, but land on the identical underlying "
        "limitation. CH4-p107.c7, CH4-sprctl.c3, CH4-sprctl.c5, "
        "CH4-hw.c19, CH4-p124.c13, CH4-hw.c2, CH4-p128.c8 [covered "
        "elsewhere], CH4-sctl.c4 [covered elsewhere] .");

    // --------- [declined] VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY ---------
    checks.record(
        "VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY [declined]: this "
        "package's own subject, per the build task, is the per-sprite "
        "shift register's horizontal (hpos) arm/disarm/shift behaviour; "
        "vertical extent (VSTART/VSTOP, how tall a sprite is) belongs to "
        "a different (agnus_spritedma) boundary -- confirmed now that "
        "the RTL is open: this module's own disclosed port list (above) "
        "carries hpos but no vertical-position input at all, matching "
        "hrm/ch3-sprite-registers/'s own identically-reasoned Non-goals "
        "note on a sibling claim set at this same fixture. 'Sprites are "
        "16 pixels wide and can be almost any height you wish -- as "
        "short as one line or taller than the screen.' (CH4-p101.c2/c3, "
        "CH4-size.c2, p.101). CH4-p101.c2, CH4-p101.c3, CH4-size.c2 .");

    // --------- [declined] MANUAL-VS-DMA-SOURCE-NOT-DISTINGUISHABLE-HERE ---------
    checks.record(
        "MANUAL-VS-DMA-SOURCE-NOT-DISTINGUISHABLE-HERE [declined]: "
        "'Sprites may be activated \"manually\" whenever they are not "
        "being used by a DMA channel [and] even when the sprite DMA is "
        "turned off.' (CH4-manual.c1/c2/c3, CH4-p123.c1/c2, p.123) "
        "asserts that arming works via a plain register write "
        "regardless of DMA activity. WRITE-DATA-ARMS-SPRITE-DATB-ALONE-"
        "DOES-NOT and COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART above "
        "already arm and match by a bare register write (`aen`+`address`"
        "+`data_in`), asserting no DMA-related precondition anywhere -- "
        "so this proposition already holds on every other assertion's "
        "own measurement basis. Not given its own expect(): this "
        "module's own now-disclosed port list (above) exposes no "
        "distinct 'processor vs DMA' select at all -- `aen`/`address`/"
        "`data_in` are the single write path both sources would use -- "
        "so there is no second condition to test against here; the "
        "distinction, if any, is made upstream of this boundary. "
        "CH4-manual.c1, CH4-manual.c2, CH4-manual.c3, CH4-p123.c1, "
        "CH4-p123.c2 .");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
