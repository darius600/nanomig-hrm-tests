// ch4-sprite-shifter -- denise_sprites_shifter conformance test
//
// Test purpose: the per-sprite shift register -- how the two data words
//   SPRxDATA/SPRxDATB become a serial stream of 2-bit pixel values, when
//   the shifter is armed and disarmed, and what a write to SPRxCTL does
//   to a sprite that is already armed -- at the denise_sprites_shifter
//   module boundary. Package ch4-sprite-shifter, fixture
//   denise_sprites_shifter (one module, standalone -- see BOUNDARY).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition,
//   printed pp. 97-128 (claims.jsonl for ch4-sprite-shifter-and-arming,
//   122 records).  A red result below means denise_sprites_shifter.v
//   disagrees with the manual, not that the assertion is wrong -- no
//   assertion here was, or should be, adjusted to make the RTL pass.
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
// BOUNDARY: nanomig/src/minimig-aga/denise_sprites_shifter.v, reached
//   at $(NANOMIG)/src/minimig-aga/, per the build task. This is the
//   FIRST package to build this module standalone: every other package
//   that already reaches it (ch3-sprite-registers, ch3-register-map-
//   denise, ch3-shres-sprite-colour-encoding, ch3-nominal-display-
//   values, denise_sprites, ch4-attached-sprites -- confirmed by
//   grepping every hrm/*/Makefile in this repo for TOP_MODULE) does so
//   only as a VERILOG_SOURCES dependency of the wrapper module
//   denise_sprites, whose own top-level ports (reg_address_in, data_in,
//   hpos, aga, spres, sprdata, ...) are NOT denise_sprites_shifter's own
//   standalone port list -- that wrapper interface hides this module's
//   own ports behind denise_sprites' internal wiring. No sibling package
//   has ever disclosed denise_sprites_shifter's own standalone
//   interface. This is therefore unlike hrm/ch3-sprite-registers/ and
//   hrm/ch4-attached-sprites/, which could legitimately reuse an
//   ALREADY-BUILT sibling harness's disclosed port list for the SAME
//   top module without opening their own package's RTL -- no such
//   sibling exists for denise_sprites_shifter as a top module.
//
// ORDER OF WORK: this file -- every assertion, its expected value, and
//   the claim ids it cites -- is written from claims.jsonl's own
//   quotations (the "extract" field: page + verbatim quote + restate-
//   ment) ONLY. The "classify" field present on every record of this
//   package's own claims.jsonl already contains RTL line numbers and
//   measured behaviour (e.g. "denise_sprites_shifter.v:284-303: shifta/
//   shiftb are loaded in parallel from datla/datlb...") -- that field
//   was NOT read for the purpose of writing a single assertion or
//   expected value below; only "extract.quote", "extract.page" and,
//   for the two page-124 self-contradictions, "classify.conflict_
//   resolution" (see below) were used, and the latter only for its own
//   English description of which HALF of a contradiction is undisputed,
//   never for its RTL line numbers. denise_sprites_shifter.v has not
//   been opened by this agent.
//
//   Because no port list is legitimately available before that gate,
//   this commit cannot yet contain a working Harness class the way
//   hrm/ch3-sprite-registers/'s and hrm/ch4-attached-sprites/'s own
//   spec-only commits could (they reused an already-disclosed sibling
//   interface for the SAME top module; none exists here). Every
//   assertion below is therefore written as a PLANNED measurement
//   (Checks::plan(), new in this file, printed as "PLANNED:", scored
//   neither pass nor fail) stating its full citation, its expected
//   result, and exactly how it will be measured, in the manual's own
//   register vocabulary (SPRxPOS/SPRxCTL/SPRxDATA/SPRxDATB/HSTART/hpos/
//   sprdata) -- not yet translated into any Verilog signal name. A
//   specification committed after its test does not satisfy the build
//   brief's gate; committing plans that do not yet reference a DUT at
//   all is the more conservative direction, not a workaround of it.
//   RESULTS and the MUTATION LOG are PENDING, filled in by a follow-up
//   commit once denise_sprites_shifter.v is opened, the port list is
//   learned, and every PLANNED measurement below is replaced by a real
//   Checks::expect() against the built DUT -- per the build brief's
//   explicit two-commit gate.
//
// THE TWO PAGE-124 SELF-CONTRADICTIONS (from the build task, indepen-
//   dently checked against this package's own claims file and, for the
//   figure, the source image itself -- not taken on trust):
//
//   (1) Converter direction. Page 124 heads its own section "Parallel-
//   to-serial converters" and, three paragraphs later, writes "the
//   comparator issues a load signal to the serial-to-parallel
//   converter" -- opposite directions on the same page, confirmed in
//   both editions per the build task. CHECKED: this package's own
//   claims.jsonl contains NEITHER "serial-to-parallel" NOR the heading
//   text "Parallel-to-serial converters" in any extract.quote (grepped
//   directly) -- the disputed pair of records (CH4-hw.c23, CH4-p124.c16)
//   belongs to a DIFFERENT, sibling package's own inventory (ch4-
//   regression-comparator-and-arming), not to this one. This package's
//   own converter-direction claims (CH4-hw.c8, CH4-hw.c9, CH4-p124.c5,
//   CH4-p124.c7, CH4-p124.c8, and the Figure 4-13 claims) all fall on
//   the undisputed side: parallel load from the data registers, serial
//   shift-out, most-significant bit first, 16 bits, stopping until
//   reloaded. Per the build task's own instruction, NEITHER direction
//   of the converter's NAME is asserted anywhere below -- every
//   assertion below tests the shift BEHAVIOUR (parallel load; serial,
//   MSB-first output order; 16-bit run length; reload-gated restart),
//   never "this device is correctly/incorrectly called a parallel-to-
//   serial [or serial-to-parallel] converter". A name is not a
//   behaviour, and no assertion over this RTL module claims to
//   adjudicate what the manual calls the circuit.
//
//   (2) Comparator axis. Page 124 line 49 has SPRxPOS holding "the
//   horizontal position"; line 65 has the comparator matching "the Y
//   value in the position register SPRxPOS"; appendix A p284 and Figure
//   4-13 both say horizontal. CHECKED: this package's own claims.jsonl
//   contains no occurrence of "Y value" or "vertical" tied to the
//   comparator in any extract.quote (grepped directly) -- the disputed
//   record (CH4-hw.c21 / CH4-p124.c15, carrying the "Y value" quote)
//   again belongs to the sibling ch4-regression-comparator-and-arming
//   package, not to this one. Every comparator-axis claim actually
//   present in this package's own claims file (CH4-hw.c15, CH4-hw.c22,
//   CH4-det.c3/c4/c5/c6/c7/c28, CH4-p126.c3/c4/c5/c6/c22, CH4-manual.c8,
//   CH4-p123.c5) says "horizontal" throughout, unanimously, on the
//   undisputed side -- so no hedge on axis-naming is needed in this
//   package's own assertions; COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART
//   below asserts exactly, and only, what this package's own claims
//   say: an hpos (horizontal) match starts output.
//
// FIGURE 4-13's DOUBLE "ARM" LABEL (page 125): per the build task, the
//   figure prints the label "ARM" sprite twice -- once on the line from
//   the SPRxDATA load decode to the arming flip-flop's S (set) input,
//   and a second time on the line from the SPRxDATB load decode to
//   SPRxDATB's own L (load) input, which no prose supports. VERIFIED
//   directly by opening hrm-testpipeline/docs/references/amiga-hrm-3rd/
//   ch4/figures/p125.png rather than trusting the build task's or any
//   restatement's description: the label really is printed twice, and
//   the second occurrence sits on the SPRxDATB register's own load
//   line, not on any line reaching the S/R flip-flop. CHECKED against
//   this package's own claims file: only CH4-fig413.f4 among this
//   package's claims describes the "ARM" label at all, and its own
//   restatement names the FIRST occurrence specifically ("the box
//   'SPRxDATA load decode' drives the S, set, input of the arming
//   flip-flop, and the line it drives is labelled 'ARM' sprite") -- the
//   undisputed occurrence, the one prose supports (CH4-manual.c6,
//   CH4-p123.c4: writing SPRxDATA arms). No claim in this package's own
//   file rests on the second (SPRxDATB-path) occurrence; none of this
//   package's other Figure 4-13 claims (CH4-fig4-13.c3/c4/c5/c7/c8/c10,
//   CH4-fig413.f2/f3/f5/f7/f8/f9/f10/f11) mention an "ARM" label on the
//   SPRxDATB path either. WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT
//   below asserts only the first occurrence's own proposition.
//
// TWO SMALL SELF-CHECKED (not build-task-supplied) READINGS: this
//   package's own claims.jsonl flags CH4-p107.c4 ("Bits 7-0 contain the
//   high 8 bits of HSTART", p107) against CH4-p128.c5 ("Bits 7-0 specify
//   the horizontal start position, bits H8-H1", p128) as "flags:
//   conflict" in the extract field. Read plainly, these are NOT in
//   tension: a 9-bit field H8..H0 has H8-H1 as its own high 8 bits, so
//   "the high 8 bits of HSTART" and "bits H8-H1" name the identical 8
//   bits, one generically and one by mnemonic. HSTART-IS-9BIT-FIELD-
//   ACROSS-SPRXPOS-AND-SPRXCTL below treats both quotations as
//   consistent, not as one of the two page-124 conflicts singled out
//   above.
//
// NON-GOALS (declined claims, recorded not asserted, with reason):
//
//   EIGHT-CHANNELS-AND-ATTACHMENT-NEED-WIDER-BOUNDARY (6 ids:
//   CH4-p107.c7, CH4-sprctl.c3, CH4-sprctl.c5, CH4-hw.c19,
//   CH4-p124.c13, CH4-hw.c2 -- plus CH4-p128.c8 and CH4-sctl.c4,
//   covered elsewhere per above, recorded together with these for the
//   same underlying reason): the ATTACH bit (SPRxCTL bit 7) and the "for
//   each of the eight sprites" register multiplicity are both
//   propositions about the relationship BETWEEN sprite channels (an
//   odd/even pair's combined colour code; eight independent instances
//   of this same register set existing chip-wide) -- neither is
//   something a single per-sprite shift-register instance, on its own,
//   can confirm or contradict. This matches hrm/ch4-attached-sprites/'s
//   own boundary, which is exactly where ATTACH is tested (denise_
//   sprites, two sprites plus the shared colour-select logic).
//
//   VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY (3 ids: CH4-p101.c2, "a sprite
//   may be only one line high"; CH4-p101.c3, "may be taller than the
//   screen"; CH4-size.c2, "height may be as short as one line or taller
//   than the screen"): this package's own subject, per the build task,
//   is the per-sprite shift register and its horizontal (hpos) arm/
//   disarm/shift behaviour -- vertical extent is VSTART/VSTOP territory
//   at a different (agnus_spritedma) boundary, matching hrm/ch3-sprite-
//   registers/'s own, identically-reasoned Non-goals note for a sibling
//   claim set on this same fixture.
//
//   MANUAL-VS-DMA-SOURCE-NOT-DISTINGUISHABLE-HERE (5 ids: CH4-manual.c1,
//   CH4-manual.c2, CH4-manual.c3, CH4-p123.c1, CH4-p123.c2 -- "sprites
//   may be activated manually... whenever they are not being used by a
//   DMA channel"): the proposition is that arming works via a plain
//   register write independent of whether sprite DMA is on or off. This
//   package's own other assertions (WRITE-DATA-ARMS-SPRITE-DATB-ALONE-
//   DOES-NOT, COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART) already arm the
//   sprite by a register write with no DMA-enable precondition asserted
//   or required anywhere -- so the proposition already holds on every
//   other assertion's own measurement basis. It is not given its own
//   separate PLANNED/expect() entry because this boundary is not yet
//   known to expose a distinct "this write came from the processor, not
//   DMA" signal to test against as a second condition, and inventing
//   one before the RTL is open would be exactly the port-list guess the
//   build brief's gate forbids.
//
// MEASUREMENT BASIS (shared by every hpos-sweep assertion below): each
//   measurement arms (or deliberately withholds arming from) the sprite
//   from a freshly reset DUT by writing SPRxPOS, SPRxCTL, SPRxDATB and
//   SPRxDATA in some stated order, then sweeps hpos across its full
//   9-bit range (0..511, since HSTART is later shown to be a 9-bit
//   field) while sampling sprdata (the shifter's own serial pixel
//   output) at every step, recording the full trace and the first hpos
//   at which it goes non-zero ("the active position"). No assertion
//   below depends on the ABSOLUTE value of any fixed pipeline latency
//   the comparator may have; every position-based assertion instead
//   compares two measurements taken the same way (either two different
//   stimuli against each other, or a written value against the
//   position it produces), so a constant latency cancels out -- the
//   same discipline hrm/ch3-sprite-registers/ and hrm/ch4-attached-
//   sprites/ both use on this fixture.
//
// CLAIM COVERAGE -- all 122 records of this package's own claims.jsonl
//   accounted for below: 106 asserted (as PLANNED measurements, to
//   become real expect() calls once the RTL is opened) across 12 named
//   propositions plus 1 control; 14 declined with reason (across the
//   two Non-goals groups above); 2 recorded as covered elsewhere
//   (CH4-p128.c8, CH4-sctl.c4, per the build task). 106+14+2 = 122.
//
//   [control] FIXTURE-LIVE-DATA-PATH -- no claim id, no page.
//   SPRITE-RUN-IS-EXACTLY-16-PIXELS (16 ids) -- CH4-intro.c7, CH4-p97.c4,
//     CH4-forming.c1, CH4-p98.c1, CH4-p99.c7, CH4-fig4-2.c1, CH4-fig42.f1,
//     CH4-p101.c1, CH4-p101.c10, CH4-shape.c1, CH4-size.c1, CH4-p124.c2,
//     CH4-hw.c3, CH4-hw.c11, CH4-p124.c7, CH4-fig412.f2
//   LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT (4 ids) --
//     CH4-hpos.c11, CH4-hpos.c12, CH4-hpos.c13, CH4-p99.c8
//   TWO-WORDS-JOINTLY-FORM-EACH-PIXEL (16 ids) -- CH4-hw.c1, CH4-hw.c4,
//     CH4-hw.c6, CH4-hw.c7, CH4-p124.c1, CH4-p124.c4, CH4-fig4-13.c3,
//     CH4-fig4-13.c4, CH4-fig4-13.c5, CH4-fig413.f9, CH4-fig4-13.c10,
//     CH4-fig413.f10, CH4-fig413.f7, CH4-shape.c2, CH4-p102.c1,
//     CH4-shape.c5
//   SHIFT-OUT-MSB-FIRST (9 ids) -- CH4-hw.c8, CH4-hw.c9, CH4-hw.c13,
//     CH4-p124.c5, CH4-p124.c9, CH4-p127.c11, CH4-pos.c9, CH4-det.c6,
//     CH4-p126.c5
//   SHIFT-STOPS-AFTER-16-UNTIL-RELOADED (3 ids) -- CH4-hw.c12,
//     CH4-p124.c8, CH4-fig413.f8
//   WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT (11 ids) -- CH4-manual.c6,
//     CH4-p123.c4, CH4-fig4-13.c7, CH4-fig413.f4, CH4-det.c3,
//     CH4-det.c27, CH4-p126.c3, CH4-p126.c21, CH4-manual.c4,
//     CH4-manual.c5, CH4-p123.c3
//   WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE (8 ids) -- CH4-manual.c11,
//     CH4-p123.c8, CH4-fig4-13.c8, CH4-fig413.f5, CH4-det.c1,
//     CH4-det.c2, CH4-p126.c1, CH4-p126.c2
//   COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART (11 ids) -- CH4-manual.c8,
//     CH4-p123.c5, CH4-hw.c22, CH4-fig4-13.c2, CH4-fig413.f2,
//     CH4-fig413.f3, CH4-det.c28, CH4-det.c4, CH4-det.c5, CH4-p126.c22,
//     CH4-p126.c4
//   UNCHANGED-DATA-REPEATS-RELOAD-VARIES (14 ids) -- CH4-manual.c7,
//     CH4-manual.c9, CH4-manual.c10, CH4-p123.c6, CH4-p123.c7,
//     CH4-det.c7, CH4-det.c8, CH4-p126.c6, CH4-p126.c7, CH4-p126.c8,
//     CH4-dma.c1, CH4-dma.c2, CH4-p109.c2, CH4-p109.c3
//   SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM (3 ids) --
//     CH4-manual.c12, CH4-p123.c9, CH4-fig413.f11
//   HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL (11 ids) --
//     CH4-build.c5, CH4-p107.c11, CH4-p107.c4, CH4-sprctl.c9,
//     CH4-sprpos.c2, CH4-sprpos.c5, CH4-p128.c17, CH4-p128.c5,
//     CH4-sctl.c16, CH4-spos.c2, CH4-hw.c15
//   [declined] EIGHT-CHANNELS-AND-ATTACHMENT-NEED-WIDER-BOUNDARY (6 ids)
//     -- CH4-p107.c7, CH4-sprctl.c3, CH4-sprctl.c5, CH4-hw.c19,
//     CH4-p124.c13, CH4-hw.c2
//   [declined] VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY (3 ids) --
//     CH4-p101.c2, CH4-p101.c3, CH4-size.c2
//   [declined] MANUAL-VS-DMA-SOURCE-NOT-DISTINGUISHABLE-HERE (5 ids) --
//     CH4-manual.c1, CH4-manual.c2, CH4-manual.c3, CH4-p123.c1,
//     CH4-p123.c2
//   [covered elsewhere, hrm/ch4-attached-sprites] (2 ids) --
//     CH4-p128.c8, CH4-sctl.c4
//
//   Note on TWO-WORDS-JOINTLY-FORM-EACH-PIXEL and WRITE-DATA-ARMS-
//   SPRITE-DATB-ALONE-DOES-NOT: CH4-fig413.f7 ("two parallel-to-serial
//   converters, one fed from SPRxDATA and one fed from SPRxDATB, both
//   loaded by the same L signal") and CH4-p123.c5 ("displayed on every
//   line, at the horizontal position...") each support two of the
//   groups above and are cited on both; counted once in the 106 total.
//
// RESULTS: PENDING -- filled in below, after opening
//   denise_sprites_shifter.v and building, in a separate commit per the
//   build brief's order of work.
//
// MUTATION LOG: PENDING -- filled in below, after opening the RTL and
//   building, in a separate commit per the build brief's order of work.

#include <iostream>
#include <string>

namespace {

// Checks: no DUT is referenced anywhere in this file yet (see ORDER OF
// WORK above), so this class has no expect() in this commit -- only
// plan() (a measurement not yet taken) and record() (a declined claim,
// never a measurement). expect() is added in the follow-up commit, once
// denise_sprites_shifter.v is open and a real Harness exists.
class Checks {
public:
    // A measurement this file intends to take once a DUT exists, stated
    // in full now (citations, expected result, how it will be measured)
    // so the expectation is fixed BEFORE the RTL is opened. Does not
    // score pass/fail -- there is nothing to compare against yet.
    void plan(const std::string& msg) { std::cout << "PLANNED: " << msg << "\n"; }
    // For declined claims: a proposition this package's own boundary
    // cannot exercise, with its reason. Never a measurement.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int planned = 0;
    int declined = 0;
};

}  // namespace

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    Checks checks;

    // ---------------- [control] FIXTURE-LIVE-DATA-PATH ----------------
    checks.plan(
        "[control] FIXTURE-LIVE-DATA-PATH: arm the sprite three times, "
        "each time from a fresh reset, with SPRxDATB held at 0x0000 and "
        "SPRxDATA holding a single distinct 1-bit word (0x8000 the MSB, "
        "0x0080 a middle bit, 0x0001 the LSB), sweep hpos across its full "
        "range each time, and require sprdata to go non-zero at exactly "
        "one hpos per run, at three DISTINCT hpos values that are "
        "monotonic in the same order as the three bit positions -- "
        "proving the write -> arm -> hpos-sweep -> read-sprdata "
        "measurement basis every assertion below shares actually "
        "delivers distinct fetched values, in order, before any "
        "claim-based assertion depends on it. Fixture check, no claim "
        "id, no page.");
    checks.planned++;

    // ---------------- SPRITE-RUN-IS-EXACTLY-16-PIXELS ----------------
    checks.plan(
        "SPRITE-RUN-IS-EXACTLY-16-PIXELS: arm the sprite with "
        "SPRxDATA=SPRxDATB=0xFFFF (every bit set, so every one of the 16 "
        "possible output pixels is non-transparent) at a fixed HSTART, "
        "sweep hpos across the full line, and require the run of "
        "consecutive non-zero sprdata samples to be EXACTLY 16 long, "
        "starting at HSTART -- not 15, not 17, and not resuming later in "
        "the same sweep: 'Sprites are 16 pixels wide' (CH4-size.c1, "
        "p.101); 'On-screen width of up to 16 pixels' (CH4-forming.c1, "
        "CH4-p98.c1, p.98); 'A line is 16 pixels wide' (CH4-hw.c3, "
        "p.124); 'the shift...continues until all 16 bits have been "
        "transferred to the display circuitry' (CH4-hw.c11, p.124); "
        "'After loading, a sprite shift register emits exactly 16 bit "
        "positions' (CH4-p124.c7, p.124); Figure 4-2's 16-pixel-wide "
        "dimension arrow (CH4-fig4-2.c1, CH4-fig42.f1, p.100); Figure "
        "4-12's two sprites placed exactly 16 hpos apart (CH4-fig412.f2, "
        "p.120). CH4-intro.c7, CH4-p97.c4, CH4-forming.c1, CH4-p98.c1, "
        "CH4-p99.c7, CH4-fig4-2.c1, CH4-fig42.f1, CH4-p101.c1, "
        "CH4-p101.c10, CH4-shape.c1, CH4-size.c1, CH4-p124.c2, "
        "CH4-hw.c3, CH4-hw.c11, CH4-p124.c7, CH4-fig412.f2 .");
    checks.planned++;

    // ---------------- LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT ----------------
    checks.plan(
        "LEFTMOST-PIXEL-POSITION-TRANSPARENCY-INDEPENDENT: at the SAME "
        "HSTART, compare (a) SPRxDATA=0x8000/SPRxDATB=0x0000 (bit 15 "
        "set: the very first pixel is itself visible) against (b) "
        "SPRxDATA=0x4000/SPRxDATB=0x0000 (bit 15 clear on both words: "
        "the first pixel is transparent, bit 14 the second pixel is "
        "visible). Require active_hpos(b) == active_hpos(a) + 1 -- the "
        "run still STARTS at the same HSTART-derived position in both "
        "cases; only whether that first position is itself visible "
        "differs: 'The X position represents the location of the very "
        "first (leftmost) pixel in the full 16-bit wide sprite. This is "
        "always the case, even if the leftmost pixels are specified as "
        "transparent and do not appear on the screen.' (CH4-p99.c7/c8, "
        "p.99). CH4-hpos.c11, CH4-hpos.c12, CH4-hpos.c13, CH4-p99.c8 .");
    checks.planned++;

    // ---------------- TWO-WORDS-JOINTLY-FORM-EACH-PIXEL ----------------
    checks.plan(
        "TWO-WORDS-JOINTLY-FORM-EACH-PIXEL: at a fixed HSTART, using bit "
        "15 (the run's first output position) as the marker bit, compare "
        "sprdata at that position across three configurations: (i) "
        "SPRxDATA=0x8000/SPRxDATB=0x0000, (ii) SPRxDATA=0x0000/"
        "SPRxDATB=0x8000, (iii) SPRxDATA=0x8000/SPRxDATB=0x8000. Require "
        "all three sprdata values to be PAIRWISE DIFFERENT -- proving "
        "SPRxDATA alone, SPRxDATB alone, and both together are three "
        "distinguishable states, i.e. the two words jointly determine "
        "each pixel's code, matching 'each line is defined by two words "
        "to provide selection of three colors and transparent' "
        "(CH4-p124.c1/c2, p.124) and Figure 4-13's own two converters, "
        "one fed from each register, whose outputs both feed the same "
        "'sprite serial video data' block (CH4-fig4-13.c3/c4/c5, "
        "CH4-fig413.f9, p.125). CH4-hw.c1, CH4-hw.c4, CH4-hw.c6, "
        "CH4-hw.c7, CH4-p124.c1, CH4-p124.c4, CH4-fig4-13.c3, "
        "CH4-fig4-13.c4, CH4-fig4-13.c5, CH4-fig413.f9, CH4-fig4-13.c10, "
        "CH4-fig413.f10, CH4-fig413.f7, CH4-shape.c2, CH4-p102.c1, "
        "CH4-shape.c5 .");
    checks.planned++;

    // ---------------- SHIFT-OUT-MSB-FIRST ----------------
    checks.plan(
        "SHIFT-OUT-MSB-FIRST: arm the sprite at a fixed HSTART with (a) "
        "SPRxDATA=0x8000/SPRxDATB=0x0000 (bit 15, the MSB, alone) and "
        "separately with (b) SPRxDATA=0x0001/SPRxDATB=0x0000 (bit 0, the "
        "LSB, alone). Require active_hpos(a) == HSTART (the FIRST "
        "position of the run) and active_hpos(b) == HSTART + 15 (the "
        "LAST position of the run) -- proving bit 15 is output first/ "
        "leftmost and bit 0 last/rightmost: '...each parallel-to-serial "
        "converter begins shifting the bits out of the converter, most "
        "significant (leftmost) bit first' (CH4-hw.c8/c9, CH4-p124.c5, "
        "p.124); '...the bit image of the data corresponds exactly to "
        "the image that actually appears on the screen (most "
        "significant data on the left)' (CH4-hw.c13, CH4-p124.c9, "
        "p.124); 'The most significant bit of the first data word will "
        "be placed in this position on the screen' (CH4-p127.c11, "
        "CH4-pos.c9, p.127); 'the leftmost pixel of the sprite data "
        "[is] placed at the position defined in the horizontal part of "
        "SPRxPOS' (CH4-det.c6, CH4-p126.c5, p.126). CH4-hw.c8, "
        "CH4-hw.c9, CH4-hw.c13, CH4-p124.c5, CH4-p124.c9, CH4-p127.c11, "
        "CH4-pos.c9, CH4-det.c6, CH4-p126.c5 .");
    checks.planned++;

    // ---------------- SHIFT-STOPS-AFTER-16-UNTIL-RELOADED ----------------
    checks.plan(
        "SHIFT-STOPS-AFTER-16-UNTIL-RELOADED: arm the sprite with "
        "SPRxDATA=SPRxDATB=0xFFFF at a fixed HSTART and sweep the FULL "
        "hpos range 0..511 in ONE pass, with no register rewritten "
        "during the sweep. Require sprdata to be zero at hpos = "
        "HSTART-1 (just before the run), non-zero for all 16 positions "
        "HSTART..HSTART+15, and zero again at every hpos from HSTART+16 "
        "through the end of the 0..511 sweep -- it does not resume or "
        "restart within the same pass without a fresh load: 'Immediately "
        "after the data is transferred..., each...converter begins "
        "shifting...The shift occurs once during each low resolution "
        "pixel time and continues until all 16 bits have been "
        "transferred...The shifting and data output does not begin "
        "again until the next time this converter is loaded from the "
        "data registers.' (CH4-hw.c12, CH4-p124.c8, p.124); Figure "
        "4-13's grounded serial inputs, so zeros shift in behind the 16 "
        "data bits (CH4-fig413.f8, p.125). CH4-hw.c12, CH4-p124.c8, "
        "CH4-fig413.f8 .");
    checks.planned++;

    // ---------------- WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT ----------------
    checks.plan(
        "WRITE-DATA-ARMS-SPRITE-DATB-ALONE-DOES-NOT: from a fresh "
        "reset, write SPRxPOS/SPRxCTL (a fixed HSTART) and SPRxDATB=0x8000 "
        "but do NOT write SPRxDATA; sweep hpos across the full range and "
        "require sprdata to be zero throughout (DATB alone does not "
        "arm). THEN, continuing from the SAME state, write "
        "SPRxDATA=0x8000 and sweep again; require sprdata to now go "
        "non-zero at HSTART -- proving the SPRxDATA write is what arms "
        "the sprite for the next horizontal comparison, and that "
        "writing SPRxDATB first and SPRxDATA last (the manual's own "
        "recommended order) is exactly what this arm-only-on-DATA "
        "mechanism requires: 'You display sprites manually by writing "
        "to the sprite data registers SPRxDATB and SPRxDATA, in that "
        "order. You write to SPRxDATA last because that address \"arms\" "
        "the sprite to be output at the next horizontal comparison.' "
        "(CH4-manual.c4/c5/c6, CH4-p123.c3/c4, p.123); 'Writing to the "
        "sprite A data register enables the horizontal comparator.' "
        "(CH4-det.c3, CH4-p126.c3, p.126); 'This arms the sprite "
        "horizontal comparators...' (CH4-det.c27, CH4-p126.c21, p.126); "
        "Figure 4-13's own SPRxDATA load decode driving the arming "
        "flip-flop's S (set) input on the line printed 'ARM' sprite -- "
        "the FIRST, undisputed occurrence of that label, not the second, "
        "unsupported one on the SPRxDATB path (see FIGURE 4-13's DOUBLE "
        "\"ARM\" LABEL above) (CH4-fig4-13.c7, CH4-fig413.f4, p.125). "
        "CH4-manual.c6, CH4-p123.c4, CH4-fig4-13.c7, CH4-fig413.f4, "
        "CH4-det.c3, CH4-det.c27, CH4-p126.c3, CH4-p126.c21, "
        "CH4-manual.c4, CH4-manual.c5, CH4-p123.c3 .");
    checks.planned++;

    // ---------------- WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE ----------------
    checks.plan(
        "WRITE-CTL-DISARMS-ALREADY-ARMED-SPRITE: arm the sprite (write "
        "SPRxPOS, SPRxCTL, SPRxDATB, SPRxDATA) and sweep hpos; require "
        "sprdata to go non-zero (the PRECONDITION -- the sprite really "
        "was armed -- asserted here, not assumed). THEN, without "
        "rewriting SPRxDATA, write SPRxCTL again and sweep hpos once "
        "more; require sprdata to be zero throughout this second sweep "
        "-- a write to SPRxCTL disarms a sprite that was already armed, "
        "this package's own headline subject: 'The sprite can be "
        "terminated (\"disarmed\") by writing to the SPRxCTL register.' "
        "(CH4-manual.c11, CH4-p123.c8, p.123); 'Writing to the sprite "
        "control registers disables the horizontal comparator circuitry. "
        "This prevents the system from sending any output from the data "
        "registers to the serial converter or to the screen.' "
        "(CH4-det.c1/c2, CH4-p126.c1/c2, p.126); Figure 4-13's own "
        "SPRxCTL load decode driving the arming flip-flop's R (reset) "
        "input (CH4-fig4-13.c8, CH4-fig413.f5, p.125). CH4-manual.c11, "
        "CH4-p123.c8, CH4-fig4-13.c8, CH4-fig413.f5, CH4-det.c1, "
        "CH4-det.c2, CH4-p126.c1, CH4-p126.c2 .");
    checks.planned++;

    // ---------------- COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART ----------------
    checks.plan(
        "COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART: arm the sprite with "
        "SPRxDATA=0x8000 (a marker at the run's first position) at two "
        "DIFFERENT written HSTART values and require the measured "
        "active hpos to equal the WRITTEN HSTART in both cases (not a "
        "fixed or hardcoded position) -- the comparator's match against "
        "the position register is what starts output, on the horizontal "
        "axis: 'the comparator issues a load signal and the sprite "
        "display begins' when the beam reaches the leftmost pixel's "
        "position (CH4-hw.c22, p.124); 'Writing to the sprite A data "
        "register enables the horizontal comparator. This enables "
        "output to the screen when the horizontal position of the video "
        "beam equals the horizontal value in the position register.' "
        "(CH4-det.c4, CH4-p126.c4, p.126); 'If the comparator is "
        "enabled, the sprite data will be sent to the display...' "
        "(CH4-det.c5, p.126); 'The armed comparators start the output "
        "of sprite data to the screen when the horizontal beam count "
        "matches the HSTART, X value, part of SPRxPOS.' (CH4-det.c28, "
        "CH4-p126.c22, p.126); 'The data written will then be displayed "
        "on every line, at the horizontal position given in the \"H\" "
        "portion of the position registers SPRxPOS and SPRxCTL.' "
        "(CH4-manual.c8, CH4-p123.c5, p.123); Figure 4-13's own "
        "comparator, labelled 'equal', gated with the armed flip-flop's "
        "Q output into the L (load) input of both converters "
        "(CH4-fig4-13.c2, CH4-fig413.f2, CH4-fig413.f3, p.125). "
        "CH4-manual.c8, CH4-p123.c5, CH4-hw.c22, CH4-fig4-13.c2, "
        "CH4-fig413.f2, CH4-fig413.f3, CH4-det.c28, CH4-det.c4, "
        "CH4-det.c5, CH4-p126.c22, CH4-p126.c4 .");
    checks.planned++;

    // ---------------- UNCHANGED-DATA-REPEATS-RELOAD-VARIES ----------------
    checks.plan(
        "UNCHANGED-DATA-REPEATS-RELOAD-VARIES: arm the sprite once "
        "(write SPRxPOS/SPRxCTL/SPRxDATB/SPRxDATA) and sweep hpos "
        "0..511 TWICE in a row with no register rewritten between "
        "sweeps; require the two full sprdata traces to be IDENTICAL "
        "(the sprite re-triggers, unaided, on every subsequent line): "
        "'If the data is unchanged, the result will be a vertical bar.' "
        "(CH4-manual.c9, CH4-p123.c6, p.123); 'As long as the "
        "comparator remains enabled, the current contents of the sprite "
        "data register will be output at the selected horizontal "
        "position on a video line.' (CH4-det.c7, CH4-p126.c6, p.126); "
        "'The data in the sprite data registers does not change. It is "
        "either rewritten by the user or modified under DMA control.' "
        "(CH4-det.c8, CH4-p126.c7/c8, p.126); a DMA-off sprite, from "
        "this module's own boundary, is observationally identical to "
        "one simply not rewritten -- 'the system will continue to "
        "display the line of sprite data that was most recently "
        "fetched...caus[ing] a vertical bar' (CH4-dma.c1/c2, "
        "CH4-p109.c2/c3, p.109). THEN, after the first sweep, rewrite "
        "SPRxDATA to a DIFFERENT value (SPRxPOS/SPRxCTL untouched) and "
        "sweep a third time; require this third trace to DIFFER from "
        "the first two: 'If the data is reloaded for every line, a "
        "complex sprite can be produced.' (CH4-manual.c7/c10, "
        "CH4-p123.c7, p.123). CH4-manual.c7, CH4-manual.c9, "
        "CH4-manual.c10, CH4-p123.c6, CH4-p123.c7, CH4-det.c7, "
        "CH4-det.c8, CH4-p126.c6, CH4-p126.c7, CH4-p126.c8, CH4-dma.c1, "
        "CH4-dma.c2, CH4-p109.c2, CH4-p109.c3 .");
    checks.planned++;

    // ---------------- SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM ----------------
    checks.plan(
        "SPRXPOS-WRITE-MOVES-ARMED-SPRITE-WITHOUT-DISARM: arm the "
        "sprite at HSTART=H1 (write SPRxPOS/SPRxCTL/SPRxDATB/SPRxDATA), "
        "sweep hpos, require the active position to be H1. THEN, "
        "WITHOUT rewriting SPRxCTL or SPRxDATA again, write SPRxPOS to "
        "produce a DIFFERENT HSTART=H2 and sweep again; require the "
        "active position to now be H2 -- a position write alone moves an "
        "already-armed sprite, it does not need to be re-armed: 'If you "
        "write to the SPRxPOS register, you can manually move the "
        "sprite horizontally at any time, even during normal sprite "
        "usage.' (CH4-manual.c12, CH4-p123.c9, p.123); Figure 4-13's own "
        "SPRxPOS load decode driving the L (load) input of the SPRxPOS "
        "register directly, a path SEPARATE from the arming flip-flop "
        "(CH4-fig413.f11, p.125). CH4-manual.c12, CH4-p123.c9, "
        "CH4-fig413.f11 .");
    checks.planned++;

    // ---------------- HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL ----------------
    checks.plan(
        "HSTART-IS-9BIT-FIELD-ACROSS-SPRXPOS-AND-SPRXCTL: with SPRxCTL "
        "bit 0 held at 0, increment the SPRxPOS byte by 1 and require "
        "the measured active-position delta to be EXACTLY 2 raw hpos "
        "units (SPRxPOS[7:0] holds the UPPER 8 bits of the 9-bit HSTART "
        "value, so its own LSB is worth 2 raw units). Separately, with "
        "SPRxPOS held fixed, toggle SPRxCTL bit 0 from 0 to 1 and "
        "require the delta to be EXACTLY 1 raw unit (SPRxCTL bit 0 is "
        "HSTART's own true, missing, LSB): 'Bits 7-0 contain the high 8 "
        "bits of HSTART' (CH4-p107.c4/c11, CH4-sprpos.c5, p.107) read "
        "together with 'Bits 7-0 specify the horizontal start position, "
        "bits H8-H1' (CH4-p128.c5, CH4-spos.c2, p.128) -- the same 8 "
        "bits, named generically on one page and by mnemonic on the "
        "other, not in tension (see TWO SMALL SELF-CHECKED READINGS "
        "above); 'Bit 0 The HSTART low bit' / 'Bit 0 is bit H0 of "
        "horizontal start.' (CH4-p107.c11, CH4-sprctl.c9, CH4-p128.c17, "
        "CH4-sctl.c16, pp.107/128); '...write the horizontal...position "
        "of the sprite into [the first control word]' (CH4-build.c5, "
        "p.104); 'These registers, called SPRxPOS, contain the "
        "horizontal position' (CH4-hw.c15, p.124); 'This is where the "
        "topmost line...(HSTART) starting position for the sprite' "
        "(CH4-sprpos.c2, p.107). CH4-build.c5, CH4-p107.c11, "
        "CH4-p107.c4, CH4-sprctl.c9, CH4-sprpos.c2, CH4-sprpos.c5, "
        "CH4-p128.c17, CH4-p128.c5, CH4-sctl.c16, CH4-spos.c2, "
        "CH4-hw.c15 .");
    checks.planned++;

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
        "own. This is exactly the boundary hrm/ch4-attached-sprites/ "
        "already tests ATTACH at (denise_sprites, two sprite channels "
        "plus the shared colour-select logic). 'Bit 7 (Used in "
        "attachment)' (CH4-p107.c7, CH4-sprctl.c5, p.107); 'also "
        "contains some data having to do with sprite attachment' "
        "(CH4-sprctl.c3, p.107); 'whether or not a sprite is attached' "
        "(CH4-hw.c19, CH4-p124.c13, p.124); 'There is such a pair of "
        "data registers for each of the eight sprites' (CH4-hw.c2, "
        "p.124). Two further ids naming the SAME attach bit, "
        "CH4-p128.c8 and CH4-sctl.c4, are covered by hrm/ch4-attached-"
        "sprites/ per the build task's own direct instruction and are "
        "recorded here rather than declined for boundary reasons, but "
        "land on the identical underlying limitation. CH4-p107.c7, "
        "CH4-sprctl.c3, CH4-sprctl.c5, CH4-hw.c19, CH4-p124.c13, "
        "CH4-hw.c2, CH4-p128.c8 [covered elsewhere], CH4-sctl.c4 "
        "[covered elsewhere] .");
    checks.declined++;

    // --------- [declined] VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY ---------
    checks.record(
        "VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY [declined]: this "
        "package's own subject, per the build task, is the per-sprite "
        "shift register's horizontal (hpos) arm/disarm/shift behaviour; "
        "vertical extent (VSTART/VSTOP, how tall a sprite is) belongs to "
        "a different (agnus_spritedma) boundary, matching hrm/ch3-"
        "sprite-registers/'s own identically-reasoned Non-goals note on "
        "a sibling claim set at this same fixture ('this boundary "
        "exposes no vertical-position port, only hpos'). 'Sprites are "
        "16 pixels wide and can be almost any height you wish -- as "
        "short as one line or taller than the screen.' (CH4-p101.c2/c3, "
        "CH4-size.c2, p.101). CH4-p101.c2, CH4-p101.c3, CH4-size.c2 .");
    checks.declined++;

    // --------- [declined] MANUAL-VS-DMA-SOURCE-NOT-DISTINGUISHABLE-HERE ---------
    checks.record(
        "MANUAL-VS-DMA-SOURCE-NOT-DISTINGUISHABLE-HERE [declined]: "
        "'Sprites may be activated \"manually\" whenever they are not "
        "being used by a DMA channel [and] even when the sprite DMA is "
        "turned off.' (CH4-manual.c1/c2/c3, CH4-p123.c1/c2, p.123) "
        "asserts that arming works via a plain register write "
        "regardless of DMA activity. WRITE-DATA-ARMS-SPRITE-DATB-ALONE-"
        "DOES-NOT and COMPARATOR-MATCH-STARTS-OUTPUT-AT-HSTART above "
        "already arm and match by a bare register write, asserting no "
        "DMA-related precondition anywhere -- so this proposition "
        "already holds on every other assertion's own measurement "
        "basis. Not given its own expect(): this module's own port "
        "list, not yet known, may or may not expose a distinct "
        "'processor vs DMA' select to test as a second condition, and "
        "guessing one before the RTL is open is exactly what the build "
        "brief's gate forbids. CH4-manual.c1, CH4-manual.c2, "
        "CH4-manual.c3, CH4-p123.c1, CH4-p123.c2 .");
    checks.declined++;

    std::cout << "\n" << checks.planned << " planned, " << checks.declined
              << " declined (spec-only commit; RTL not yet opened, see "
                 "ORDER OF WORK above)\n";
    return 0;
}
