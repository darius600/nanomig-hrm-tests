// Test purpose: The beam counters and what chapter 3 of the manual says
//   about them -- lines per field, the raster order, interlace field
//   alternation, blanking -- plus the register-level formats of VPOSR,
//   HTOTAL and BEAMCON0 from appendix C, and the BPLCON0 LACE/ERSY bits from
//   appendix A. Package ch3-beam-counters-and-blanking, fixture
//   agnus_beamcounter (one Verilator build, this module only).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 40, 41, 42, 45, 49, 50, 51, 54, 57, 62, 64, 75, 77, 79, 80, 81,
//   89, 271, 299, 303, 304, 305, 307.
// Non-goals: display-window (DIWSTRT/DIWSTOP) sizing, bitplane modulo and
//   pointer addressing, sprite DMA, horizontal-resolution (hires/shres)
//   modes, HAM, and BPLCON3/genlock -- none of these are ports of
//   agnus_beamcounter (see BOUNDARY below and the port list this file
//   actually drives). Sub-registers this module never reads back (HTOTAL,
//   BEAMCON0 and friends are write-only here) are tested only through their
//   observable *effects* on htotal_out / varbeamen_out / harddis_out /
//   _hsync / _vsync / VPOSR, never through a readback that does not exist.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_beamcounter.v:27-57 module header
//   -- ports clk, clk7_en, reset, cck, ntsc, aga, ecs, a1k, data_in[15:0],
//   data_out[15:0], reg_address_in[8:1], hpos[8:0], vpos[10:0], _hsync,
//   _vsync, field1, lace, _csync, hblank, vblank, vbl, vblend, eol, eof,
//   vbl_int, htotal_out[8:0], harddis_out, varbeamen_out. Register addresses
//   and their write-decode/multiplexer logic: l.60-76 (parameter table),
//   l.108-115 (VPOSR/VHPOSR read mux, combinational), l.117-142 (BEAMCON0
//   register and its ERSY/LACE companions), l.166-233 (programmable
//   display-mode registers and their varbeamen/varhsyen/varvsyen/varvben
//   multiplexers), l.246-300 (horizontal counter, long_line), l.308-379
//   (vertical counter, long_frame, field1, eof), l.387-421 (sync generator),
//   l.444-459 (blanking generator).
//
// EXPECTED VALUES: every EXPECTED value asserted below is derived from a
//   claims.jsonl quotation, not from what the RTL happens to do -- register
//   addresses, parameter names and signal names (VPOSR=$004, HTOTAL=$1C0,
//   the existence of a `pal` register, etc.) are used only to know which
//   port to wiggle and which port to read, the same use the BOUNDARY
//   evidence field of the 207 claim records makes. No assertion's PASS/FAIL
//   threshold is chosen or adjusted because of anything in the RTL.
//
// GIVEN MEASUREMENTS (measured outside this file, NOT re-derived below): VPOSR
//   bits 13:12 read $00/$10/$20/$30 for the four (ecs,ntsc) input
//   combinations in the order the manual's identification table gives
//   (ecs=0,ntsc=0 / ecs=0,ntsc=1 / ecs=1,ntsc=0 / ecs=1,ntsc=1); a PAL field
//   is 71051 colour clocks (1/49.92s), NTSC 59701 (1/59.96s); htotal_out
//   reads 452 under both standards; long_frame is set out of reset. This
//   file reads htotal_out directly (a ten-line check, not the field-duration
//   simulation) but does not re-run the colour-clock field-duration
//   measurement, and does not assert a raw "312"/"262" line count without
//   first forcing long_frame to a known value by register write -- seeial
//   PAL-FIELD-312-NTSC-FIELD-262-LINES below, which explains why.
//
// PRINTED MISPRINT, NOT ASSERTED: printed page 304 misprints the HTOTAL bit
//   header row as "... 03 02 10 00" where the fifteenth column should read
//   "01". The bit assignment below rests on POSITION in the "Use" row (h2 is
//   the fifteenth entry, h1 the sixteenth, regardless of the corrupted
//   numeral above them), matching how both claims-file readers already
//   resolved it (C-htotal-bits.c15/c16). The misprinted header text itself
//   is never asserted.
//
// CLAIM COVERAGE: all 207 records of claims.jsonl are accounted for below --
//   either inside an expect() message (every id written out in full, no
//   shared-prefix compression) or inside a note() call with a stated reason.
//   3-video.c11 is one id shared by two records (opus and sol independently
//   assigned it to two different sentences on p.40); both sentences are
//   demonstrated by PAL-FIELD-312-NTSC-FIELD-262-LINES below and the id is
//   cited once there, covering both, per the precedent in
//   hrm/ch3-ham-control-field/test_denise_hamgenerator.cpp. C-scan.c01 and
//   C-scan.c03 are each cited once, inside
//   VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA below: their own restatement
//   narrows the quoted p.304 sentence to its line-count half alone (525/625
//   lines), which that assertion fully establishes, so neither id also sits
//   on the DECLINED note below. The same sentence's colour-clocks-per-line
//   half is graded instead by COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)/(PAL)
//   -- C-scan.c02 and C-scan.c04 -- which counts run_cck() periods between
//   two eol pulses directly, with no htotal_out conversion factor (see
//   NO CONVERSION FACTOR FROM THE RTL below for why).
//   opus:7-appC-beam.c2 and opus:7-appC-beam.c3, whose own restatement does
//   not split the sentence into two claims, sit on
//   COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC/PAL bundled with line count)
//   instead, which grades both halves together.
//
// NO CONVERSION FACTOR FROM THE RTL: a "2 hpos-ticks per colour clock"
//   factor exists only in this module's own write-decode
//   (agnus_beamcounter.v:206), not on any printed page, so no assertion
//   uses it. HTOTAL-H8-H1-BIT-MAPPING tests only the bit-weight-ordering
//   relation the manual's own 'Use' row states (see that assertion's own
//   comment), and HTOTAL-FIXED-NOT-FRACTIONAL-227POINT5 has no faithful
//   restatement at this boundary -- htotal_out carries no manual-stated
//   real-time unit at all -- so it is a declined claim (see the DECLINED
//   note near the end of this file).
//
// CONTROL CASES: every assertion built on a measurement this file also
//   suspects of disagreeing with the RTL is paired with a same-basis control
//   that is expected to PASS: VBLANK-DURATION-NTSC-MATCHES-MANUAL is the
//   control for VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL (identical vbl
//   sweep, only ntsc differs); the ERSY=0 sweep inside
//   ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD is the control for its own
//   ERSY=1 case, on the identical hpos-holds-at-zero measurement.
//
// MUTATION LOG (each entry run under WSL/Verilator as edit -> rebuild ->
//   run -> confirm the named assertion(s) red -> revert -> rebuild ->
//   confirm the baseline again; the pass/fail totals quoted in an entry are
//   those of the run that recorded it). Every mutation is a STIMULUS change
//   to this file's own inputs (a written register value, a written
//   register ADDRESS, a harness input pin, or an injected extra register
//   write); no expected value or oracle text is touched, and nanomig/ is
//   never touched.
//
// MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: the written pattern
//   changed from 0x500 to 0x600 (force_vpos's vpos11 argument) -> FAIL,
//   only FIXTURE-DELIVERS-KNOWN-VALUE red (its hardcoded 0b101 comparison
//   no longer matches the now-written 0b110). Reverted: 27/3.
// MUTATION RASTER-SWEEP-LEFT-TO-RIGHT-TOP-TO-BOTTOM stimulus red: a stray
//   VHPOSW write (hpos forced to 20<<1=40) injected after the 4th sampled
//   CCK period -> FAIL, only RASTER-SWEEP-LEFT-TO-RIGHT-TOP-TO-BOTTOM red
//   (the injected forward jump is neither +2 nor a wrap). Reverted: 27/3.
// MUTATION PAL-FIELD-312-NTSC-FIELD-262-LINES stimulus red: (PAL) the
//   shared force_vpos call's long_frame argument changed from false to
//   true (both PAL and NTSC read this same line) -> FAIL, PAL-FIELD-312-
//   NTSC-FIELD-262-LINES (PAL) AND (NTSC) both red together, as expected
//   collateral of a shared setup line (313/263 lines measured instead of
//   312/262). Reverted: 27/3.
// MUTATION PAL-FIELD-312-NTSC-FIELD-262-LINES stimulus red: (NTSC) the
//   same edit as PAL-FIELD-312-NTSC-FIELD-262-LINES (PAL) immediately
//   above -- both ids are red from that one shared-setup change; see that
//   entry for the measured values. Reverted: 27/3.
// MUTATION LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION stimulus red: (reset default)
//   an early `write_reg(A_BPLCON0, 1<<BP_LACE)` injected
//   before this sub-case's own check -> FAIL ("25 passed, 5 failed"):
//   LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (reset default) red (lace
//   now reads 1, not 0) AND LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION
//   (LACE=0, control) also red, as expected collateral (with lace now 1,
//   its own two-field measurement alternates instead of matching).
//   Reverted: 27/3.
// MUTATION LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION stimulus red: (LACE=0, control)
//   the same early injected write as
//   LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (reset default)
//   immediately above -- both ids are red from that one change; see that
//   entry for the measured values. Reverted: 27/3.
// MUTATION LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION stimulus red: (write LACE=1)
//   this sub-case's own write changed from `1<<BP_LACE` to
//   `1<<BP_ERSY` (wrong bit) -> FAIL ("25 passed, 5 failed"):
//   LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (write LACE=1) red (lace
//   stays 0, ERSY was written instead) AND
//   LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (LACE=1) also red, as
//   expected collateral (with lace never set, the two fields measured
//   afterward do not alternate). Reverted: 27/3.
// MUTATION LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION stimulus red: (LACE=1)
//   the same wrong-bit write as
//   LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (write LACE=1)
//   immediately above -- both ids are red from that one change; see that
//   entry for the measured values. Reverted: 27/3.
// MUTATION LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION stimulus red: (write LACE=0)
//   this sub-case's own write changed from `0` to
//   `1<<BP_LACE` (leaves LACE set instead of clearing it) -> FAIL ("26
//   passed, 4 failed"), only LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION
//   (write LACE=0) red (lace still reads 1). Reverted: 27/3.
// MUTATION ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD stimulus red: (reset default, control)
//   an early `write_reg(A_BPLCON0, 1<<BP_ERSY)` injected
//   before this sub-case's own loop -> FAIL ("26 passed, 4 failed"), only
//   ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD (reset default, control) red
//   (hpos now holds at 0/1 instead of advancing past it; the (write
//   ERSY=1) sub-case is unaffected because ERSY was already 1 by the time
//   its own, now-redundant write runs). Reverted: 27/3.
// MUTATION ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD stimulus red: (write ERSY=1)
//   this sub-case's own write changed from `1<<BP_ERSY` to
//   `1<<BP_LACE` (wrong bit) -> FAIL ("26 passed, 4 failed"), only
//   ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD (write ERSY=1) red (hpos
//   advances normally, ERSY was never actually set). Reverted: 27/3.
// MUTATION INTERLACE-HALF-LINE-VERTICAL-OFFSET-VIA-VSYNC-TIMING stimulus red:
//   the short-field harness's force_vpos long_frame argument changed
//   from false to true (both harnesses now measure the long-field branch)
//   -> FAIL ("26 passed, 4 failed"), only
//   INTERLACE-HALF-LINE-VERTICAL-OFFSET-VIA-VSYNC-TIMING red (the two
//   measured hpos values collapse to the same branch, no half-line
//   offset). Reverted: 27/3.
// MUTATION VPOSR-BIT15-IS-LOF-LONG-FRAME stimulus red: the first
//   force_vpos call's long_frame argument changed from true to false (both
//   calls now write false) -> FAIL ("26 passed, 4 failed"), only
//   VPOSR-BIT15-IS-LOF-LONG-FRAME red (bit15 reads 0 both times, not 1
//   then 0). Reverted: 27/3.
// MUTATION VPOSR-IDENTIFICATION-FIELD-BITS-14-7 stimulus red: `h.dut->aga`
//   changed from 1 to 0 right before the "aga=1" readback -> FAIL ("26
//   passed, 4 failed"), only VPOSR-IDENTIFICATION-FIELD-BITS-14-7 red
//   (bits 9:8 read 00, not 11, for the case meant to exercise aga=1).
//   Reverted: 27/3.
// MUTATION VPOSR-BITS-6-3-UNUSED stimulus red: the read address changed
//   from A_VPOSR to A_VHPOSR, and force_vpos's hpos8_1 argument changed
//   from 0 to 0xFF so VHPOSR's own bits 6:3 would be genuinely non-zero
//   for this stimulus -> FAIL ("26 passed, 4 failed"), only
//   VPOSR-BITS-6-3-UNUSED red (VHPOSR's hpos_r-derived low byte is 0xFF,
//   not the always-0 field VPOSR carries at that position). Changing only
//   the read address (leaving hpos8_1 at its default 0) stays green,
//   because VHPOSR's bits 6:3 are 0 too for that vpos pattern (0x555);
//   hence the hpos8_1 change as well. Reverted: 27/3.
// MUTATION VPOSR-BITS-2-0-VERTICAL-POSITION stimulus red: the read address
//   changed from A_VPOSR to A_VHPOSR -> FAIL ("26 passed, 4 failed"), only
//   VPOSR-BITS-2-0-VERTICAL-POSITION red (VHPOSR's bits 2:0 come from
//   hpos_r, which stays 0 throughout, so two of the three swept patterns
//   -- 0x500 and 0x700 -- no longer match their expected 5 and 7).
//   Reverted: 27/3.
// MUTATION HTOTAL-H8-H1-BIT-MAPPING stimulus red: `make clean && make MUT=HTOTAL_H8_H1_BIT_MAPPING` (the walking-bit write's target register changed from A_HTOTAL to A_HSSTOP, guarded by `#ifndef MUT_HTOTAL_H8_H1_BIT_MAPPING` around that one write_reg call) -> 26 passed, 2 failed, only HTOTAL-H8-H1-BIT-MAPPING newly red (htotal_out never moves off its constant post-reset value for any of the eight written patterns, so the got[0]>0 base case fails outright; VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL stays red throughout, unaffected, exactly as at baseline). `make clean && make` reverted to 27 passed, 1 failed, matching the baseline exactly. (HTOTAL-FIXED-NOT-FRACTIONAL-227POINT5 is a declined claim, not an assertion, so it carries no mutation; see NO CONVERSION FACTOR FROM THE RTL above.)
// MUTATION BEAMCON0-INTRO-AND-VARBEAMEN-GATES-PROGRAMMABLE-TIMING stimulus red:
//   the second harness's ecs argument changed from false to true (so
//   its BEAMCON0 write is no longer ignored) -> FAIL ("26 passed, 4
//   failed"), only BEAMCON0-INTRO-AND-VARBEAMEN-GATES-PROGRAMMABLE-TIMING
//   red (varbeamen_out now reads 1 on the ecs=0 harness too, since it is
//   no longer really ecs=0). Reverted: 27/3.
// MUTATION BEAMCON0-VARHSYEN-VARVSYEN-GATE-PROGRAMMABLE-SYNC stimulus red:
//   the HSSTRT write's target address changed from A_HSSTRT to A_HCENTER
//   -> FAIL ("26 passed, 4 failed"), only
//   BEAMCON0-VARHSYEN-VARVSYEN-GATE-PROGRAMMABLE-SYNC red (hsstrt_reg
//   never receives the written value, so _hsync keeps falling near the
//   hardwired default even after VARHSYEN is enabled). Reverted: 27/3.
// MUTATION BEAMCON0-HARDDIS-DISABLES-HARD-BLANKING-STOPS stimulus red: the
//   HARDDIS write changed from `1<<BC_HARDDIS` to `1<<BC_LOLDIS` (wrong
//   bit) -> FAIL ("26 passed, 4 failed"), only
//   BEAMCON0-HARDDIS-DISABLES-HARD-BLANKING-STOPS red (harddis_out stays 0,
//   HARDDIS was never actually written). Reverted: 27/3.
// MUTATION BEAMCON0-LOLDIS-DISABLES-LONG-LINE-TOGGLE stimulus red: the
//   LOLDIS harness's write changed from `(1<<BC_VARBEAMEN)|(1<<BC_LOLDIS)`
//   to `(1<<BC_VARBEAMEN)` alone (LOLDIS omitted) -> FAIL ("26 passed, 4
//   failed"), only BEAMCON0-LOLDIS-DISABLES-LONG-LINE-TOGGLE red (LOL now
//   toggles line by line instead of freezing at 0). Reverted: 27/3.
// MUTATION BEAMCON0-PAL-BIT-SELECTS-DEFAULT-VTOTAL stimulus red: the
//   BEAMCON0 write changed from `1<<BC_PAL` to `0` (PAL bit never set) ->
//   FAIL ("26 passed, 4 failed"), only
//   BEAMCON0-PAL-BIT-SELECTS-DEFAULT-VTOTAL red (262 lines measured
//   instead of 312 -- the ntsc input pin's own reset-time default is what
//   is left in charge). Reverted: 27/3.
// MUTATION VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA stimulus red: (non-interlaced)
//   the VTOTAL write changed from 5 to 6 -> FAIL ("25
//   passed, 5 failed"): VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA
//   (non-interlaced) red (7 lines measured, not the expected 6) AND
//   VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (interlaced) also red, as
//   expected collateral (8/7 measured instead of 7/6); the two same-basis
//   default-VTOTAL sum sub-cases are unaffected, since they use a separate
//   harness that never writes VTOTAL. Reverted: 27/3.
// MUTATION VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA stimulus red: (interlaced)
//   the same VTOTAL 5->6 edit as
//   VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (non-interlaced)
//   immediately above -- both ids are red from that one change; see that
//   entry for the measured values. Reverted: 27/3.
// MUTATION VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA stimulus red: (PAL default, sum of one long + one short field)
//   the shared sum-loop's
//   `write_reg(A_BPLCON0, 1<<BP_LACE)` line removed (LACE never enabled)
//   -> FAIL ("25 passed, 5 failed"):
//   VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (PAL default, sum of one
//   long + one short field) AND VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA
//   (NTSC default, sum of one long + one short field) both red together,
//   as expected collateral of one shared loop body (with LACE never set,
//   both measured fields are the same length, and their sum no longer
//   matches 625/525). Reverted: 27/3.
// MUTATION VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA stimulus red: (NTSC default, sum of one long + one short field)
//   the same removed LACE write
//   as VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (PAL default, sum of one
//   long + one short field) immediately above -- both ids are red from
//   that one change; see that entry for the measured values. Reverted:
//   27/3.
// MUTATION VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL none impossible: this
//   assertion is red at baseline (a genuine finding -- PAL vbl measures 26
//   consecutive blanked lines, not the manual's 29). No stimulus can turn
//   an already-red assertion red again; the same-basis control
//   (VBLANK-DURATION-NTSC-MATCHES-MANUAL, a PASS using the identical
//   vpos/vbl sweep) is exercised by its own mutation below, which also
//   confirms this assertion's own measurement basis is sound (inverting
//   which standard each harness actually is turns the NTSC case red too,
//   and leaves the PAL case still red, just for a different, still-
//   understood reason).
// MUTATION VBLANK-DURATION-NTSC-MATCHES-MANUAL stimulus red: the shared
//   loop's harness construction changed from `Harness h(false, ntsc)` to
//   `Harness h(false, !ntsc)` (PAL and NTSC swapped) -> FAIL ("26 passed,
//   4 failed"): VBLANK-DURATION-NTSC-MATCHES-MANUAL newly red (the
//   harness built for the "NTSC" label is actually the PAL-configured one,
//   measuring 26 blanked lines against the NTSC expectation of 21) --
//   VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL stays red throughout (it was
//   already red at baseline; under the swap it is now the NTSC-configured
//   harness measuring 21 against the PAL expectation of 29, a different
//   mismatch, still red). Reverted: 27/3.
//
// MUTATION COLOUR-CLOCKS-PER-LINE-227POINT5 stimulus red: (control, no
//   claim id) `make clean && make MUT=COLOUR_CLOCKS_PER_LINE_CONTROL test`
//   (the written HTOTAL changed from 100 to 120, guarded by
//   `#ifndef MUT_COLOUR_CLOCKS_PER_LINE_CONTROL` around that one write_reg
//   call) -> "27 passed, 6 failed", only
//   COLOUR-CLOCKS-PER-LINE-227POINT5 (control, no claim id) newly red
//   (measured periods=121, not the expected 101; every other assertion's
//   FAIL/PASS set unchanged from baseline). `make clean && make -B test`
//   reverted to 28 passed, 5 failed, confirmed byte-for-byte identical to
//   the pre-mutation baseline by `diff` (the only difference in the raw
//   output being Verilator's own non-deterministic build-timing line).
// MUTATION COLOUR-CLOCKS-PER-LINE-227POINT5 none impossible: (NTSC) and
//   (PAL) are both red at baseline (a genuine, measured disagreement --
//   a single line cannot last 227.5 colour clocks, since a line ends on a
//   whole clock, so the check averages 4 consecutive run_cck()-periods-
//   between-two-eol-pulses measurements; all four individual lines read
//   227 under both standards (no alternation), so the average is 227 too,
//   not the manual's 227.5; this module's own HTOTAL_VAL comment already
//   says so, "line length of 227 CCKs in PAL mode (NTSC line length of
//   227.5 CCKs is not supported)", agnus_beamcounter.v:86). No stimulus on
//   an already-red assertion can fail it harder. The control immediately
//   above exercises this identical run_cck()-periods-between-two-eol-
//   pulses comparator against a known written value and reads it back
//   exactly (101, then 121 once mutated), proving the comparator itself is
//   live and not vacuous.
// MUTATION COLOUR-CLOCKS-PER-LINE-227POINT5 none impossible: (NTSC bundled
//   with line count) and (PAL bundled with line count) are both red at
//   baseline for the same underlying reason -- the colour-clocks-per-line
//   conjunct fails as above, so the AND of both halves fails too, even
//   though the line-count conjunct (525/625) passes. Both conjuncts'
//   comparators are independently proven live elsewhere in this same run:
//   the colour-clocks-per-line conjunct by the control mutation just
//   above, and the line-count conjunct by
//   VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (NTSC/PAL default, sum of
//   one long + one short field)'s own stimulus mutation already in this
//   log (the removed-LACE-write edit), which turns exactly that half red.
//
// Baseline: 28 passed, 5 failed -- VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL
// and the four COLOUR-CLOCKS-PER-LINE-227POINT5 cases (NTSC)/(PAL)/(NTSC
// bundled)/(PAL bundled).

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_beamcounter.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A record is not a check: it prints and does not touch passed/failed.
    // Used only to record, in an actual C++ string (not a `//` comment), the
    // claims declined below -- so their ids are found by a coverage search
    // of the file's strings, not only by reading its comments.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Register addresses, as reg_address_in values (the port carries address
// bits [8:1]; a custom-chip byte address is always even, so the port value
// is the byte address shifted right by one -- as in the pre-pipeline
// hrm/denise_hamgenerator harness's write_color() (removed), which drove
// COLORBASE $180 as reg_address_in=0xC0).
constexpr int A_VPOSR   = 0x004 >> 1;
constexpr int A_VPOSW   = 0x02A >> 1;
constexpr int A_VHPOSR  = 0x006 >> 1;
constexpr int A_VHPOSW  = 0x02C >> 1;
constexpr int A_BPLCON0 = 0x100 >> 1;
constexpr int A_HTOTAL  = 0x1C0 >> 1;
constexpr int A_HSSTOP  = 0x1C2 >> 1;
constexpr int A_HBSTRT  = 0x1C4 >> 1;
constexpr int A_HBSTOP  = 0x1C6 >> 1;
constexpr int A_VTOTAL  = 0x1C8 >> 1;
constexpr int A_VSSTOP  = 0x1CA >> 1;
constexpr int A_VBSTOP  = 0x1CE >> 1;
constexpr int A_HSSTRT  = 0x1DE >> 1;
constexpr int A_BEAMCON0= 0x1DC >> 1;
constexpr int A_VSSTRT  = 0x1E0 >> 1;
constexpr int A_HCENTER = 0x1E2 >> 1;

// BEAMCON0 bit numbers named by the claims (appendix C p.305).
constexpr int BC_HARDDIS  = 14;
constexpr int BC_LPENDIS  = 13; // declined: wire commented out, no lightpen port
constexpr int BC_LOLDIS   = 11;
constexpr int BC_VARVSYEN = 9;
constexpr int BC_VARHSYEN = 8;
constexpr int BC_VARBEAMEN= 7;
constexpr int BC_DUAL     = 6;  // declined: wire commented out
constexpr int BC_PAL      = 5;
constexpr int BC_CSYTRUE  = 2;  // declined: wire commented out
constexpr int BC_VSYTRUE  = 1;  // declined: wire commented out
constexpr int BC_HSYTRUE  = 0;  // declined: wire commented out

// BPLCON0 bits named by the claims (appendix A p.271 / chapter 3 p.89).
constexpr int BP_LACE = 2;
constexpr int BP_ERSY = 1;

class Harness {
public:
    explicit Harness(bool ecs, bool ntsc, bool aga = false)
        : dut(new Vagnus_beamcounter) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->cck = 0;
        dut->ntsc = ntsc ? 1 : 0;
        dut->aga = aga ? 1 : 0;
        dut->ecs = ecs ? 1 : 0;
        dut->a1k = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        for (int i = 0; i < 10; ++i) {
            dut->clk7_en = 1;
            tick();
        }
        dut->reset = 0;
        dut->clk7_en = 0;
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

    // One full colour-clock period, exactly the clk7_en/cck relationship
    // of the pre-pipeline hrm/agnus_beamcounter/test_agnus_beamcounter.cpp
    // main loop (removed): `clk7_en = (i%2==0); cck = (i%4<2);` over 4 raw
    // ticks. hpos's
    // own LSB tracks cck combinationally (`always @(cck) hpos_0 = cck;`,
    // l.273-274), so hpos only settles to an even, stable value at the end
    // of a full 4-tick period -- which is why bulk advancement below samples
    // once per run_cck(), while the precise line/frame edge-counts below
    // instead replicate the sibling's raw per-tick loop directly.
    void run_cck(int periods = 1) {
        for (int p = 0; p < periods; ++p) {
            for (int i = 0; i < 4; ++i) {
                dut->clk7_en = (i % 2 == 0);
                dut->cck = (i % 4 < 2);
                tick();
            }
        }
    }

    // A single register write: one clk7_en=1 edge with reg_address_in/
    // data_in set, per the pre-pipeline hrm/denise_hamgenerator
    // write_color() pattern (removed).
    // None of the write-decode conditions in agnus_beamcounter.v depend on
    // cck, only on clk7_en, so the write's position within a CCK period does
    // not matter.
    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0xFF);
        dut->data_in = data;
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->eval();
        // Deliberately no extra run_cck() settle here: cck stays 0 across
        // this call (only clk7_en pulsed), so it cannot itself advance
        // hpos/vpos -- and two back-to-back write_reg calls (as force_vpos
        // below makes) must NOT have a colour clock sneaked between them,
        // or a stray hpos==2 vpos_inc pulse (l.312-317) could disturb the
        // vpos value the second write is about to set.
    }

    // VPOSR/VHPOSR readback: the mux at l.108-115 is combinational
    // (`always @(*)`), so asserting the address and calling eval() is
    // enough -- no clock edge needed.
    uint16_t read(int addr) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0xFF);
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        dut->eval();
        return v;
    }

    Vagnus_beamcounter* dut;
};

// Writes VHPOSW then VPOSW (deliberately in that order -- see below) so
// vpos, long_frame and hpos land at a fully known state (VPOSW:
// vpos[10:8]=data_in[2:0], long_frame=data_in[15]; VHPOSW:
// vpos[7:0]=data_in[15:8], hpos[8:1]=data_in[7:0] -- l.264-267, l.327-330,
// l.345-346). hpos8_1 lets a caller park hpos safely past the vpos_inc
// trigger point (hpos==2, l.312-317) so the forced vpos value is not
// immediately overwritten by that same line's own increment -- pass
// hpos8_1=0 (the default) for a normal fresh-line start.
//
// ORDER MATTERS: VPOSW is written LAST, not first. Both register-write
// always-blocks this function touches (the vpos-update block and the
// long_frame-update block, l.325-336 and l.341-350) check their own
// `reg_address_in==VPOSW`/`==VHPOSW` condition ahead of everything else,
// but that priority only protects the FIELDS THAT SPECIFIC WRITE OWNS
// (VPOSW: vpos[10:8] and long_frame; VHPOSW: vpos[7:0] and hpos_r) -- not
// each other, and not against whatever `end_of_frame`/`vpos_inc` happen to
// be doing on the SAME tick because of hpos/vpos values left over from
// before this call. A VHPOSW write does not shield long_frame from a
// coincidental `end_of_frame && lace` toggle landing on that same tick
// (writing VPOSW first, then VHPOSW, silently produces the WRONG
// long_frame about half the time). Writing VPOSW LAST means its own branch's priority is the very
// last word on long_frame's value, regardless of what the intervening
// VHPOSW tick's `vpos_inc`/`end_of_frame` combination happened to be.
void force_vpos(Harness& h, uint32_t vpos11, bool long_frame, uint8_t hpos8_1 = 0) {
    uint16_t vhposw_data = static_cast<uint16_t>(((vpos11 & 0xFF) << 8) | hpos8_1);
    h.write_reg(A_VHPOSW, vhposw_data);
    uint16_t vposw_data = static_cast<uint16_t>((long_frame ? 0x8000 : 0x0000) |
                                                 ((vpos11 >> 8) & 0x7));
    h.write_reg(A_VPOSW, vposw_data);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-DELIVERS-KNOWN-VALUE (no page) =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        // Prime with a value no register resets to (vpos[10:8]=0b101,
        // long_frame=0), then read it back through the two ports this whole
        // file depends on (VPOSW/VHPOSW write path, VPOSR read path) before
        // any claim-based assertion below relies on either.
        force_vpos(h, /*vpos11=*/0x500, /*long_frame=*/false);
        uint16_t vposr = h.read(A_VPOSR);
        checks.expect(
            ((vposr >> 15) & 1) == 0 && (vposr & 0x7) == 0x5,
            "FIXTURE-DELIVERS-KNOWN-VALUE: a vpos[10:8]=0b101 / long_frame=0 "
            "pattern written through VPOSW and read back through VPOSR "
            "arrives distinctly and correctly (bit15=0, bits2:0=0b101) -- "
            "proves the register write-then-read path this file's later "
            "VPOSR-based assertions depend on is actually live before any "
            "of them run.");
    }

    // ================= RASTER-SWEEP-LEFT-TO-RIGHT-TOP-TO-BOTTOM =================
    {
        Harness h(/*ecs=*/false, /*ntsc=*/false);
        int prev_hpos = -1;
        bool hpos_monotonic_or_wraps = true;
        bool saw_wrap = false;
        // Track vpos by watching the value itself change, not by sampling
        // it at the instant `eol` first reads 1 -- vpos's own update lags
        // the `eol` pulse by one clk7_en tick (vpos_inc, which both drive,
        // is consumed by the vpos-update always-block only on the CYCLE
        // AFTER it is set, l.310-317 vs l.325-336), so sampling vpos at the
        // same tick eol rises would read the pre-increment value both
        // times and wrongly show no change at all.
        std::vector<int> vpos_seen;
        int prev_vpos_rs = h.dut->vpos;
        vpos_seen.push_back(prev_vpos_rs);
        // Sweep just over two lines' worth of colour clocks, one full CCK
        // period (4 raw ticks) at a time -- hpos is {hpos_r[8:1], cck}
        // (l.259), and sampling every raw tick (rather than once per
        // settled colour-clock period, as the field-length loops elsewhere
        // in this file do) shows a harmless -1 sub-CCK dip when cck itself
        // falls mid-period (hpos_0 tracks cck directly and combinationally,
        // independent of hpos_r's own once-per-CCK registered increment) --
        // real, but not a claim about "sweeping left to right" at that
        // sub-pixel granularity, which the manual does not make either.
        for (int period = 0; period < 2 * 230 && vpos_seen.size() < 3; ++period) {
            h.run_cck(1);
            int hp = h.dut->hpos;
            if (prev_hpos >= 0) {
                if (hp < prev_hpos) {
                    saw_wrap = true;
                } else if (hp != prev_hpos + 2) {
                    // A forward step other than +2 per full CCK period (or
                    // the wrap just above) would not be "sweeping" left to
                    // right one pixel at a time.
                    hpos_monotonic_or_wraps = false;
                }
            }
            prev_hpos = hp;
            int vp = h.dut->vpos;
            if (vp != prev_vpos_rs) {
                vpos_seen.push_back(vp);
                prev_vpos_rs = vp;
            }
        }
        bool vpos_advanced_by_one_per_eol =
            vpos_seen.size() >= 3 && vpos_seen[1] == vpos_seen[0] + 1 &&
            vpos_seen[2] == vpos_seen[1] + 1;
        checks.expect(
            saw_wrap && hpos_monotonic_or_wraps && vpos_advanced_by_one_per_eol,
            "RASTER-SWEEP-LEFT-TO-RIGHT-TOP-TO-BOTTOM: within a line, hpos "
            "advances a step at a time (never more than two 140ns ticks in "
            "a single raw sample, matching its own {hpos_r,cck} "
            "construction) and never goes backward until it wraps to 0 at "
            "the end of the line (the "
            "'sweeping the beam' Figure 3-1 describes); vpos advances by "
            "exactly 1 at each of two consecutive eol pulses (one line at a "
            "time, top to bottom). 'The Amiga produces its video displays "
            "with raster display techniques' (p.40), 'The picture you see "
            "on the screen is made up of a series of horizontal video lines "
            "displayed one after the other' (p.40), 'Each horizontal video "
            "line is made up of a series of pixels' (p.40), Figure 3-1's "
            "in-frame captions 'The video beam produces each line by "
            "sweeping from left to right' and 'It produces the full screen "
            "by sweeping the beam from the top to the bottom, one line at a "
            "time' and 'Each line represents one sweep of an electron beam "
            "which is \"painting\" the picture as it goes along'. "
            "3-video.c01, 3-video.c02, 3-video.c03, 3-video.c1, 3-video.c2, "
            "3-video.c3, 3-video.c5, 3-video.c6, 3-video.f01, 3-video.f02, "
            "3-video.f03. Chapter 8 restates the same sweep from a second printed page -- 'The video beam starts to create the picture, sweeping from left to right for each horizontal line as it paints the picture from the top of the screen to the bottom' (p.238) -- opus:8-lpen.c13, opus:8-lpen.c14, sol:8-pen.c13, sol:8-pen.c14 .");
    }

    // ================= PAL-FIELD-312-NTSC-FIELD-262-LINES =================
    // long_frame defaults to 1 out of reset and this module's own
    // last_line/extra_line logic makes a long_frame=1 non-interlaced field
    // run one line longer (the question of NM-HRM-001, which is withdrawn
    // -- the manual itself sanctions 313 PAL lines elsewhere, see
    // docs/evidence/third-witness.md).
    // Chapter 3's own claim here is the plain "312"/"262" figure, so this
    // assertion sidesteps the LOF dispute entirely rather than re-litigate
    // it: force long_frame=0 by an explicit VPOSW write (data_in[15]=0,
    // l.345-346) before the field starts, which both readings of the
    // manual agree gives a "normal", non-extra-line field, then measure.
    for (bool ntsc : {false, true}) {
        Harness h(/*ecs=*/false, ntsc);
        force_vpos(h, /*vpos11=*/0, /*long_frame=*/false);
        int expected_lines = ntsc ? 262 : 312;
        int eol_count = 0;
        int max_vpos = 0;
        bool prev_eol = false;
        int fields_seen = 0;
        int lines_this_field = 0;
        int lines_field[2] = {0, 0};
        bool prev_eof = false;
        // Run two whole fields so the wrap from one field into the next
        // ("between display fields, the video beam traverses the lines
        // that are not visible ... and returns to the top of the screen to
        // produce another display field", p.40) is actually observed, not
        // assumed.
        for (int i = 0; i < 2 * expected_lines * 240 * 4 && fields_seen < 2; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            if (h.dut->vpos > max_vpos) max_vpos = h.dut->vpos;
            bool eol_now = h.dut->eol;
            if (eol_now && !prev_eol) {
                ++eol_count;
                ++lines_this_field;
            }
            prev_eol = eol_now;
            bool eof_now = h.dut->eof;
            if (eof_now && !prev_eof) {
                if (fields_seen < 2) lines_field[fields_seen] = lines_this_field;
                lines_this_field = 0;
                ++fields_seen;
            }
            prev_eof = eof_now;
        }
        checks.expect(
            fields_seen == 2 && lines_field[0] == expected_lines &&
                lines_field[1] == expected_lines &&
                max_vpos == expected_lines - 1,
            std::string("PAL-FIELD-312-NTSC-FIELD-262-LINES (") +
                (ntsc ? "NTSC" : "PAL") +
                "): with long_frame explicitly cleared by a VPOSW write "
                "before the field starts, two consecutive display fields "
                "each span exactly " +
                std::to_string(expected_lines) +
                " lines (vpos 0.." + std::to_string(expected_lines - 1) +
                "), and the counter wraps cleanly from the end of the first "
                "field into the start of the second -- 'the beam produces "
                "about 262 video lines' (NTSC, p.40) / 'the beam produces "
                "312 lines' (PAL, p.40), 'Each complete set of lines "
                "(262/NTSC or 312/PAL) is called a display field' (p.40), "
                "'Between display fields, the video beam traverses the "
                "lines that are not visible on the screen and returns to "
                "the top of the screen to produce another display field' "
                "(p.40). 3-video.c04, 3-video.c06, 3-video.c08, "
                "opus:3-video.c11, sol:3-video.c11, 3-video.c14, 3-video.c7, 3-video.c9 .");
    }

    // ================= LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION =================
    {
        // Sub-case A: reset default (BPLCON0 never written) -- LACE=0, and
        // consecutive fields do NOT alternate length (long_frame is frozen
        // at whatever reset left it, per l.347-348's `else if
        // (end_of_frame && lace)`). This is the non-interlaced control.
        Harness h(/*ecs=*/false, /*ntsc=*/false);
        checks.expect(
            h.dut->lace == 0,
            "LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (reset default): "
            "out of reset, before any BPLCON0 write, LACE reads 0 -- "
            "'LACE=Interlace enable (reset on power up)' (appendix A "
            "p.271). A-bplcon.c74, A-bplcon.c75, A-leg.lace1, A-leg.lace2 .");

        int len_a = -1, len_b = -1;
        {
            // measure two consecutive field lengths with LACE still 0
            int prev_lines = -1, cur_lines = 0, fields_seen = 0;
            bool prev_eol = false, prev_eof = false;
            for (int i = 0; i < 2 * 313 * 240 * 4 && fields_seen < 2; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool eol_now = h.dut->eol;
                if (eol_now && !prev_eol) ++cur_lines;
                prev_eol = eol_now;
                bool eof_now = h.dut->eof;
                if (eof_now && !prev_eof) {
                    if (fields_seen == 0) len_a = cur_lines;
                    else len_b = cur_lines;
                    cur_lines = 0;
                    ++fields_seen;
                }
                prev_eof = eof_now;
            }
            (void)prev_lines;
        }
        checks.expect(
            len_a > 0 && len_a == len_b,
            "LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (LACE=0, "
            "control): with LACE=0, two consecutive fields are the same "
            "length -- non-interlaced mode does not alternate. '0 = "
            "non-interlaced mode enabled' (p.89), 'Clear bit 2 in BPLCON0 "
            "(LACE)' for non-interlaced mode (p.64), 'Non-interlaced mode "
            "-- write 0 to bit 2' (p.51). 3-r0.c08, 3-regsum-bplcon0.c5, "
            "3-res.c22, 3-resolution.c22, 3-sum.c12 .");

        // Sub-case B: write BPLCON0 with LACE=1 -- the `lace` port reads 1,
        // and long_frame now toggles every end_of_frame (l.347-348),
        // producing an alternating field length -- the mechanism Figure 3-5
        // depicts as two fields ("Field 1"/"Field 2") interleaving into one
        // display.
        h.write_reg(A_BPLCON0, 1 << BP_LACE);
        checks.expect(
            h.dut->lace == 1,
            "LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (write LACE=1): "
            "writing bit 2 of BPLCON0 makes the `lace` port read 1 -- "
            "'Bit 2 - LACE (interlace enable)' / '1 = interlaced mode "
            "enabled' (p.89), 'Bit 2, LACE' (p.51), 'Interlaced mode -- "
            "write 1 to bit 2' (p.51), 'Set bit 2 in register BPLCON0 "
            "(LACE)' (p.64), 'To set interlaced or non-interlaced mode, you "
            "write to bit 2, LACE' (p.51), '02 LACE PF1H2 PF1P2' (appendix "
            "A p.271). 3-r0.c06, 3-r0.c07, 3-regsum-bplcon0.c4, 3-res.c20, "
            "3-res.c21, 3-resolution.c21, 3-sum.c09, A-bplcon.c52, "
            "A-bpc.b02.c0, 3-video.c20 .");

        // Establish a clean, KNOWN starting point for the alternation
        // measurement via an explicit VPOSW write (which sets long_frame
        // directly, l.345-346) rather than trusting whatever transient
        // state the vpos_inc/extra_line pipeline is in right after the
        // LACE write -- that transition is easy to get wrong: the pipeline
        // carries a lookahead through `extra_line` that does not settle in
        // the single cycle a register write occupies.
        force_vpos(h, 0, /*long_frame=*/true);

        int len_c = -1, len_d = -1;
        {
            int cur_lines = 0, fields_seen = 0;
            bool prev_eol = false, prev_eof = false;
            for (int i = 0; i < 2 * 313 * 240 * 4 && fields_seen < 2; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool eol_now = h.dut->eol;
                if (eol_now && !prev_eol) ++cur_lines;
                prev_eol = eol_now;
                bool eof_now = h.dut->eof;
                if (eof_now && !prev_eof) {
                    if (fields_seen == 0) len_c = cur_lines;
                    else len_d = cur_lines;
                    cur_lines = 0;
                    ++fields_seen;
                }
                prev_eof = eof_now;
            }
        }
        checks.expect(
            len_c > 0 && len_d > 0 && len_c != len_d &&
                (std::abs(len_c - len_d) == 1),
            "LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (LACE=1): with "
            "LACE=1, two consecutive fields differ in length by exactly one "
            "line -- the long/short field alternation Figure 3-5 depicts as "
            "'Field 1' and 'Field 2' combining into one interlaced display, "
            "with adjacent output lines fed from alternating fields, and "
            "as 'two vertically offset fields interleaved into one "
            "display' (the two field boxes, their line-1 labels and the "
            "converging dashed arrows). "
            "3-lace.f01, 3-lace.f04, 3-resolution.c15 .");

        // Sub-case C: write LACE back to 0 explicitly (not merely reset) and
        // confirm it reads 0.
        h.write_reg(A_BPLCON0, 0);
        checks.expect(
            h.dut->lace == 0,
            "LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION (write LACE=0): "
            "writing bit 2 of BPLCON0 to 0 (after having set it) makes the "
            "`lace` port read 0 again. 3-r0.c08, 3-resolution.c22 .");
    }

    // ================= ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD =================
    // `ersy` (l.146-154) is an internal reg, not a module port, and BPLCON0
    // is write-only here (no read-mux case for it, l.108-115) -- so ERSY's
    // state is only observable through its one wired-up effect: the
    // horizontal counter's own hold condition, `cck && (~ersy ||
    // |hpos_r[8:1])` (l.268), which lets hpos advance past 0 only when ersy
    // is 0 OR hpos_r is already nonzero. With ersy=1, once hpos reaches 0 at
    // end_of_line it can never leave 0 again (hpos_r stays all-zero, so the
    // "|hpos_r[8:1]" term is permanently false) -- the proxy this fixture
    // can observe for "external resync" holding the beam for an external
    // sync pulse that this module has no pin to receive.
    {
        // Control: reset default (ERSY never written) -- hpos must advance
        // normally past 0, on the same measurement basis as the ERSY=1 case
        // below.
        Harness h(/*ecs=*/false, /*ntsc=*/false);
        bool reached_zero = false;
        bool advanced_past_zero_after_reaching_it = false;
        for (int i = 0; i < 500 * 4; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            if (h.dut->hpos == 0) reached_zero = true;
            else if (reached_zero && h.dut->hpos > 2) {
                advanced_past_zero_after_reaching_it = true;
                break;
            }
        }
        checks.expect(
            reached_zero && advanced_past_zero_after_reaching_it,
            "ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD (reset default, "
            "control): out of reset, before any BPLCON0 write (ERSY=0), "
            "hpos reaches 0 and continues advancing past it -- '0 = "
            "External synchronization disabled' (p.89), and 'ERSY=External "
            "resync ... (reset on power up)' (appendix A p.271) for the "
            "power-up state this measures. 3-regsum-bplcon0.c3, "
            "A-bplcon.c78, A-leg.ersy3 .");

        // Now write ERSY=1 (BPLCON0 bit 1 only) and confirm the hold. The
        // hold condition (l.268) gates hpos_r[8:1], not hpos[0] itself
        // (hpos[0] mirrors cck directly and combinationally, l.273-274,
        // completely unaffected by ersy) -- so once held, hpos legitimately
        // still reads 0 or 1 alternately (tracking cck's own toggle), never
        // reaching 2 or beyond. The check below allows exactly that.
        h.write_reg(A_BPLCON0, 1 << BP_ERSY);
        bool reached_zero2 = false;
        bool held_at_zero = true;
        for (int i = 0; i < 500 * 4; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            if (h.dut->hpos == 0) reached_zero2 = true;
            if (reached_zero2 && h.dut->hpos > 1) {
                held_at_zero = false;
                break;
            }
        }
        checks.expect(
            reached_zero2 && held_at_zero,
            "ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD (write ERSY=1): writing "
            "bit 1 of BPLCON0 makes the horizontal counter reach hpos=0 and "
            "then hold in place (hpos never advances past 1 -- its own "
            "LSB keeps tracking cck regardless of ersy, so 0/1 is the held "
            "state, not literal 0), on the identical measurement basis as "
            "the ERSY=0 control above -- 'Bit 1 - ERSY (external "
            "synchronization enable)' (p.89), '1' selects external "
            "synchronization (p.89), '01 ERSY PF1H1 PF1P1' (appendix A "
            "p.271), 'ERSY=External resync (HSYNC, VSYNC pads become "
            "inputs)' (appendix A p.271) -- this module has no HSYNC/VSYNC "
            "pad to turn into an input, so the counter-hold measured here "
            "is the closest observable proxy for that behaviour at this "
            "boundary, not the pad-direction change itself (declined "
            "separately below). 3-r0.c03, A-bplcon.c55, A-bplcon.c76, "
            "A-bpc.b01.c0, A-leg.ersy1 .");
    }

    // ================= INTERLACE-HALF-LINE-VERTICAL-OFFSET-VIA-VSYNC-TIMING =================
    // '_vsync' starts at hpos==hsstrt when long_frame (l.401), or at
    // hpos==hcenter when !long_frame -- two different horizontal trigger
    // points depending on field type, which is exactly the "vertically
    // offsets the start of every other field by half a scan line" mechanism
    // (a horizontal-timing offset stands in for a vertical half-line
    // because the beam is still sweeping when vsync begins).
    //
    // _vsync carries no reset in the RTL (l.399-406 has no `if(reset)`
    // branch), so on the very first field its SET condition firing would be
    // indistinguishable from "always was 0" -- the CLEAR condition
    // (vpos==vsstop=5, l.403) only rises _vsync to 1 for the first time
    // AFTER that first SET has already passed (vsstop=5 > vsstrt+1=3). So
    // rather than force vpos/hpos to a specific instant, each run below is
    // simply let run from reset, long enough to pass the field's first
    // (ambiguous) SET and its first (unambiguous, 0->1) CLEAR, and then the
    // SECOND falling edge -- now a genuine, unambiguous 1->0 transition
    // preceded by a confirmed 1 -- is the one measured.
    {
        auto second_vsync_fall_hpos = [](Harness& h) -> int {
            bool prev_vsync = h.dut->_vsync;
            bool saw_rise = false;
            int falls_seen = 0;
            int hpos_at_second_fall = -1;
            for (int i = 0; i < 5 * 313 * 230 * 4 && hpos_at_second_fall < 0; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool vs = h.dut->_vsync;
                if (!prev_vsync && vs) saw_rise = true;
                if (prev_vsync && !vs && saw_rise) {
                    ++falls_seen;
                    if (falls_seen == 2) hpos_at_second_fall = h.dut->hpos;
                }
                prev_vsync = vs;
            }
            return hpos_at_second_fall;
        };

        // long field: reset default (long_frame=1, lace=0 -- every field is
        // a long field, so consecutive falls are on the identical branch).
        Harness h(/*ecs=*/false, /*ntsc=*/false);
        int hpos_at_vsync_fall_long = second_vsync_fall_hpos(h);

        // short field: force long_frame=0 once at the start (vpos left at
        // its natural reset value, 0); lace stays 0 so long_frame never
        // toggles back.
        Harness h2(/*ecs=*/false, /*ntsc=*/false);
        force_vpos(h2, /*vpos11=*/0, /*long_frame=*/false);
        int hpos_at_vsync_fall_short = second_vsync_fall_hpos(h2);

        int htotal_now = h.dut->htotal_out;
        int diff = hpos_at_vsync_fall_short - hpos_at_vsync_fall_long;
        // "half a scan line", not asserted to tick-exact precision the
        // manual itself never states numerically -- within 2 ticks (280ns)
        // of htotal_out/2.
        bool about_half_line =
            hpos_at_vsync_fall_long >= 0 && hpos_at_vsync_fall_short >= 0 &&
            std::abs(diff - htotal_now / 2) <= 2 && diff > 0;
        checks.expect(
            about_half_line,
            "INTERLACE-HALF-LINE-VERTICAL-OFFSET-VIA-VSYNC-TIMING: vsync "
            "begins at a different horizontal position depending on "
            "long_frame (hpos=" +
                std::to_string(hpos_at_vsync_fall_long) +
                " for the long field, hpos=" +
                std::to_string(hpos_at_vsync_fall_short) +
                " for the short field, htotal_out=" +
                std::to_string(htotal_now) +
                ") -- a horizontal-timing offset of about half a scan line "
                "between the two field types, matching 'In interlaced "
                "mode, the scanning circuitry vertically offsets the start "
                "of every other field by half a scan line' (p.49/p.50). "
                "3-res.c17, 3-resolution.c14 .");
    }

    // ================= VPOSR-BIT15-IS-LOF-LONG-FRAME =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, 0, /*long_frame=*/true);
        uint16_t v1 = h.read(A_VPOSR);
        force_vpos(h, 0, /*long_frame=*/false);
        uint16_t v0 = h.read(A_VPOSR);
        checks.expect(
            ((v1 >> 15) & 1) == 1 && ((v0 >> 15) & 1) == 0,
            "VPOSR-BIT15-IS-LOF-LONG-FRAME: writing long_frame via VPOSW "
            "bit 15 and reading it back through VPOSR bit 15 tracks it "
            "exactly (1 then 0) -- 'Bit 15 14 13 12 11 10 09 08 07 06 05 04 "
            "03 02 01 00 / Use LOF I6 I5 I4 I3 I2 I1 I0 LOL -- -- -- -- v10 "
            "v9 V8' (appendix C p.299), 'VPOSR - Read vertical most "
            "significant bits (and frame flop)' (p.299). C-vposr-bits.c1, "
            "C-vposr.c02. Chapter 8 states the same bit from a second printed page -- 'VPOSR: Bit 15 Long frame/short frame. 0=short frame' (p.238) -- opus:8-lpen.c20, opus:8-lpen.c21, sol:8-pen.c22, sol:8-pen.c23 .");
    }

    // ================= VPOSR-IDENTIFICATION-FIELD-BITS-14-7 =================
    // The read mux (l.108-115, combinational) is
    // {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]}
    // -- bit14 is a hardwired 0 (the manual's "I6" position), bits13:12
    // mirror the ecs/ntsc pins live (the manual's "I5"/"I4"), bits11:10 are
    // hardwired 0 ("I3"/"I2"), bits9:8 both mirror aga ("I1"/"I0"), and bit7
    // mirrors long_line (the manual's "LOL"). ecs/ntsc/aga are combinational
    // inputs to this mux, so they can be swept live on one Harness without
    // reconstructing it (they are NOT the registered `pal`/vtotal defaults,
    // which latch only at reset or an explicit BEAMCON0 write).
    {
        Harness h(/*ecs=*/false, /*ntsc=*/false);
        // settle long_line to a known value (0) via the pal=1 branch,
        // l.292-300. long_line updates on the internal `end_of_line` event
        // (hpos reaching htotal, l.248-255), a different signal from the
        // `eol` port (which pulses at hpos==2, l.312-317) -- end_of_line
        // always fires once, between any two consecutive `eol` pulses, so
        // waiting for the SECOND `eol` rising edge guarantees at least one
        // end_of_line (and so one long_line update) has already happened.
        bool prev_eol = false;
        int eol_rises = 0;
        for (int i = 0; i < 2000 && eol_rises < 2; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            bool eol_now = h.dut->eol;
            if (eol_now && !prev_eol) ++eol_rises;
            prev_eol = eol_now;
        }

        struct Combo { bool ecs, ntsc; uint16_t expect_1312; };
        // GIVEN MEASUREMENT (not re-derived): bits 13:12 read $00/$10/$20/
        // $30 for (ecs=0,ntsc=0)/(ecs=0,ntsc=1)/(ecs=1,ntsc=0)/(ecs=1,ntsc=1)
        // respectively, matching the manual's identification-table order.
        // 'I0-I6 (bits 8-14) provide the chip identification. At present "
        // there are four possible settings' (p.299), '8361 (regular NTSC) "
        // or 8370 (fat NTSC) = 10 for NTSC Agnus' (p.299).
        Combo combos[4] = {
            {false, false, 0x0000}, {false, true, 0x1000},
            {true, false, 0x2000}, {true, true, 0x3000},
        };
        bool id_field_ok = true;
        for (auto& c : combos) {
            h.dut->ecs = c.ecs ? 1 : 0;
            h.dut->ntsc = c.ntsc ? 1 : 0;
            h.dut->aga = 0;
            uint16_t v = h.read(A_VPOSR);
            if ((v & 0x3000) != c.expect_1312) id_field_ok = false;
            if (((v >> 14) & 1) != 0) id_field_ok = false;   // bit14 = 0 always ("I6")
            if (((v >> 10) & 0x3) != 0) id_field_ok = false; // bits11:10 = 0 ("I3","I2")
        }
        h.dut->aga = 1;
        uint16_t v_aga = h.read(A_VPOSR);
        bool aga_bits_ok = ((v_aga >> 8) & 0x3) == 0x3; // bits9:8 both track aga ("I1","I0")
        h.dut->aga = 0;
        uint16_t v_noaga = h.read(A_VPOSR);
        bool noaga_bits_ok = ((v_noaga >> 8) & 0x3) == 0x0;
        bool lol_ok = ((v_noaga >> 7) & 1) == 0; // long_line settled to 0 above (pal=1 case)

        checks.expect(
            id_field_ok && aga_bits_ok && noaga_bits_ok && lol_ok,
            "VPOSR-IDENTIFICATION-FIELD-BITS-14-7: across all four (ecs, "
            "ntsc) combinations, VPOSR bits 13:12 read $00/$10/$20/$30 "
            "(matching the given, already-measured order) with bit14 "
            "always 0; bits9:8 both track aga (both 1 with aga=1, both 0 "
            "with aga=0); bit7 (LOL) reads 0 once long_line has settled "
            "under PAL (ntsc=0). 'Use LOF I6 I5 I4 I3 I2 I1 I0 LOL -- -- -- "
            "-- v10 v9 V8' (p.299), 'VPOSR - Read vertical most significant "
            "bits (and frame flop)' (p.299), 'I0-I6 (bits 8-14) provide the "
            "chip identification. At present there are four possible "
            "settings' (p.299), '8361 (regular NTSC) or 8370 (fat NTSC) = "
            "10 for NTSC Agnus' (p.299). C-vposr-bits.c2, C-vposr-bits.c3, "
            "C-vposr-bits.c4, C-vposr-bits.c5, C-vposr-bits.c6, "
            "C-vposr-bits.c7, C-vposr-bits.c8, C-vposr-bits.c9, "
            "C-vposr.c01, C-vposr.c03, C-vposr.c06, C-chipid.c1, "
            "C-chipid.c5 .");
    }

    // ================= VPOSR-BITS-6-3-UNUSED =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/true, /*aga=*/true);
        force_vpos(h, 0x555, /*long_frame=*/true);
        uint16_t v = h.read(A_VPOSR);
        checks.expect(
            ((v >> 3) & 0xF) == 0,
            "VPOSR-BITS-6-3-UNUSED: with every other bit driven to a "
            "non-zero pattern (ecs=1, ntsc=1, aga=1, vpos[10:8]=0b101, "
            "long_frame=1), VPOSR bits 6:3 still read 0 -- 'Use LOF I6 I5 "
            "I4 I3 I2 I1 I0 LOL -- -- -- -- v10 v9 V8' (p.299), with the "
            "four '--' entries the manual marks as having no assigned use. "
            "C-vposr-bits.c10, C-vposr-bits.c11, C-vposr-bits.c12, "
            "C-vposr-bits.c13 .");
    }

    // ================= VPOSR-BITS-2-0-VERTICAL-POSITION =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        bool ok = true;
        for (uint32_t pattern : {0x000u, 0x500u, 0x700u}) {
            force_vpos(h, pattern, /*long_frame=*/false);
            uint16_t v = h.read(A_VPOSR);
            if ((v & 0x7) != ((pattern >> 8) & 0x7)) ok = false;
        }
        checks.expect(
            ok,
            "VPOSR-BITS-2-0-VERTICAL-POSITION: writing vpos[10:8] through "
            "VPOSW to three distinct 3-bit patterns (000, 101, 111) and "
            "reading VPOSR back shows bits 2:0 tracking vpos[10:8] exactly "
            "-- 'Use LOF I6 I5 I4 I3 I2 I1 I0 LOL -- -- -- -- v10 v9 V8' "
            "(p.299), naming bits 2,1,0 as v10, v9, V8, the high vertical "
            "position bits. C-vposr-bits.c14, C-vposr-bits.c15, "
            "C-vposr-bits.c16, C-vposr.c04, C-vposr.c05. Chapter 8 names the same bit from a second printed page -- 'Bit 0 V8 (most significant bit of vertical position)' (p.238) -- opus:8-lpen.c24, sol:8-pen.c25 .");
    }

    // ================= HTOTAL-H8-H1-BIT-MAPPING =================
    // HTOTAL has no read-mux case at all (l.108-115 only handles VPOSR/
    // VPOSW/VHPOSR/VHPOSW; every other address falls through to
    // `data_out[15:0]=0`), so the only way to observe what a HTOTAL write
    // did is through its effect on htotal_out (l.223,235), which requires
    // ecs=1 and BEAMCON0 VARBEAMEN=1 first so `htotal` actually multiplexes
    // to `htotal_reg` (l.223) instead of the hardwired default.
    //
    // NO RTL-SUPPLIED EXPECTED VALUE: comparing htotal_out against
    // `pattern << 1` would import that "<<1" from this module's own
    // write-decode (`htotal_reg <= {data_in[7:0], 1'b0}`, l.206) as the
    // expected value -- the RTL supplying its own oracle. No cited page
    // states what one written HTOTAL unit is worth in htotal_out ticks:
    // htotal_out is not a register or signal the manual names anywhere;
    // the manual's own unit for HTOTAL is colour clocks / 280ns increments
    // (p.304), not "htotal_out ticks", and this module gives THAT tick a
    // duration nowhere the manual does -- its own port declaration reads
    // "output reg [8:0] hpos, // horizontal beam counter (140ns)" (l.40),
    // an RTL-internal fact, not a printed one. What the manual's 'Use' row
    // DOES state, and what is asserted below instead, is a pure bit-weight
    // ORDERING claim: h1 through h8, named in that order at increasing bit
    // positions, carry successively doubling significance -- plain binary
    // place value, true for any consistent linear encoding regardless of
    // its absolute scale. Each measured htotal_out value below is compared
    // only against the OTHER measured htotal_out values from this same
    // sweep; no expected value is computed from an assumed tick duration.
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        int got[8];
        for (int bit = 0; bit < 8; ++bit) {
            uint16_t pattern = static_cast<uint16_t>(1u << bit);
#ifndef MUT_HTOTAL_H8_H1_BIT_MAPPING
            h.write_reg(A_HTOTAL, pattern);
#else
            h.write_reg(A_HSSTOP, pattern); // MUTATION: wrong register, htotal_out never moves
#endif
            got[bit] = h.dut->htotal_out;
        }
        // Base case: the lowest bit alone must already move htotal_out off
        // zero (otherwise "each next one doubles the last" would hold
        // vacuously for a register stuck at 0). Step case: each next bit,
        // written alone, reads back exactly double the htotal_out value of
        // the bit below it -- seven ratios, no absolute magnitude anywhere.
        bool walking_bit_ok = got[0] > 0;
        for (int bit = 1; bit < 8 && walking_bit_ok; ++bit) {
            walking_bit_ok = (got[bit] == 2 * got[bit - 1]);
        }
        checks.expect(
            walking_bit_ok,
            "HTOTAL-H8-H1-BIT-MAPPING: walking a single 1 through each of "
            "HTOTAL's eight low bits (which the manual names h1 through h8 "
            "by POSITION in the 'Use' row -- printed page 304 misprints "
            "the column header numeral above the fifteenth entry as '10' "
            "where it should read '01', but the 'Use' row itself is not "
            "misprinted, and no assertion here depends on the corrupted "
            "header numeral) and reading it back through htotal_out shows "
            "the lowest bit alone already produces a nonzero reading, and "
            "each next bit produces exactly double the htotal_out reading "
            "of the bit below it, all seven ratios in a row -- matching "
            "h1..h8 named in increasing-significance order at increasing "
            "bit positions, plain binary place value, with no claim made "
            "anywhere about what one step is worth in real time. 'Bit 15 "
            "14 13 12 11 10 09 08 07 06 05 04 03 02 [01] 00 / Use 0 0 0 0 "
            "0 0 0 0 h8 h7 h6 h5 h4 h3 h2 h1' (p.304). C-htotal-bits.c9, "
            "C-htotal-bits.c10, C-htotal-bits.c11, C-htotal-bits.c12, "
            "C-htotal-bits.c13, C-htotal-bits.c14, C-htotal-bits.c15, "
            "C-htotal-bits.c16 .");
    }

    // ================= BEAMCON0-INTRO-AND-VARBEAMEN-GATES-PROGRAMMABLE-TIMING =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        bool set_with_ecs = h.dut->varbeamen_out == 1;

        Harness h2(/*ecs=*/false, /*ntsc=*/false);
        h2.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        bool ignored_without_ecs = h2.dut->varbeamen_out == 0;

        checks.expect(
            set_with_ecs && ignored_without_ecs,
            "BEAMCON0-INTRO-AND-VARBEAMEN-GATES-PROGRAMMABLE-TIMING: with "
            "ecs=1, writing BEAMCON0 bit 7 makes varbeamen_out read 1; the "
            "identical write with ecs=0 leaves varbeamen_out at 0 "
            "(beamcon0_reg only updates '...&& ecs', l.123) -- 'A new "
            "register in the enhanced Agnus, BEAMCON0, provides a "
            "programmable signal generator' (p.305), '7 VARBEAMEN Variable "
            "beam counter comparator enable' (p.305). C-beamcon0.c1, "
            "C-beam.c01, C-beamcon0.c9, C-beam.b07, opus:7-appC-beam.c1 .");
    }

    // ================= BEAMCON0-VARHSYEN-VARVSYEN-GATE-PROGRAMMABLE-SYNC =================
    {
        // _hsync carries no reset either (l.388-395), so -- same reasoning
        // as INTERLACE-HALF-LINE-VERTICAL-OFFSET-VIA-VSYNC-TIMING above --
        // its very first SET could be indistinguishable from "always was
        // 0". _hsync fires every LINE rather than every field, so it is
        // enough to skip the first (possibly ambiguous) fall and take the
        // next one after a confirmed rise.
        auto next_hsync_fall_hpos_after_a_rise = [](Harness& h) -> int {
            bool prev = h.dut->_hsync;
            bool saw_rise = false;
            for (int i = 0; i < 4000; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool cur = h.dut->_hsync;
                if (!prev && cur) saw_rise = true;
                if (prev && !cur && saw_rise) { prev = cur; return h.dut->hpos; }
                prev = cur;
            }
            return -1;
        };

        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN); // varbeamen=1, varhsyen=varvsyen=0
        // Written value is doubled on the way into hsstrt_reg, exactly
        // like HTOTAL (`hsstrt_reg <= {data_in[7:0], 1'b0}`, l.207) -- 100
        // here lands as 200, far from the default HSSTRT_VAL=37.
        h.write_reg(A_HSSTRT, 100);
        // With VARHSYEN=0, _hsync must still fall at the DEFAULT hsstrt
        // (37), unaffected by the HSSTRT write (l.224: `hsstrt =
        // varhsyen&&varbeamen ? hsstrt_reg : HSSTRT_VAL`).
        int hpos_fall_default = next_hsync_fall_hpos_after_a_rise(h);
        // Now enable VARHSYEN too and confirm _hsync starts using the
        // WRITTEN hsstrt (200) on the next line.
        h.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        int hpos_fall_programmed = next_hsync_fall_hpos_after_a_rise(h);
        // Compared with a small tolerance, not tick-exact equality: reading
        // hpos at the instant a registered `_hsync<=0` decision commits can
        // land a couple of 140ns ticks off the exact comparison value the
        // RTL used internally (the same sub-CCK {hpos_r,cck} interaction
        // documented at RASTER-SWEEP-LEFT-TO-RIGHT-TOP-TO-BOTTOM above) --
        // the claim under test is which SOURCE hsstrt comes from, not
        // tick-exact timing the manual itself does not number.
        auto close_enough = [](int a, int b) { return std::abs(a - b) <= 2; };
        checks.expect(
            close_enough(hpos_fall_default, 37) && close_enough(hpos_fall_programmed, 200),
            "BEAMCON0-VARHSYEN-VARVSYEN-GATE-PROGRAMMABLE-SYNC: with "
            "VARBEAMEN=1 but VARHSYEN=0, _hsync still falls near the "
            "hardwired default hsstrt (37, measured " +
                std::to_string(hpos_fall_default) +
                ") despite HSSTRT having been written to 100 (200 once the "
                "register's own left-shift-by-one is accounted for, "
                "l.207); enabling VARHSYEN too makes _hsync fall near the "
                "WRITTEN value instead (200, measured " +
                std::to_string(hpos_fall_programmed) +
                ") on the next line -- '8 VARHSYEN "
            "Variable horizontal sync enable' / '9 VARVSYEN Variable "
            "vertical sync enable' (p.305), gating hsstrt/hsstop/hcenter "
            "and vsstrt/vsstop respectively (l.224-226,230-231) the same "
            "way. C-beamcon0.c7, C-beamcon0.c8, C-beam.b09, C-beam.b08 .");
    }

    // ================= BEAMCON0-HARDDIS-DISABLES-HARD-BLANKING-STOPS =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_BEAMCON0, 0);
        bool zero_ok = h.dut->harddis_out == 0;
        h.write_reg(A_BEAMCON0, 1 << BC_HARDDIS);
        bool set_ok = h.dut->harddis_out == 1;
        checks.expect(
            zero_ok && set_ok,
            "BEAMCON0-HARDDIS-DISABLES-HARD-BLANKING-STOPS: with all "
            "BEAMCON0 bits 0, harddis_out reads 0; writing HARDDIS (bit 14) "
            "alone makes harddis_out read 1 -- 'assign harddis_out = "
            "harddis || varbeamen || varvben' (l.236) generalizes the "
            "'hard stops on display sync and blank' that 'other modes, "
            "such as VGA (31 KHz programmable mode) disable' (p.305) into "
            "one output driven directly by this bit. '14 HARDDIS Disable "
            "hardwired vertical/horizontal blank' (p.305). C-beamcon0.c2, "
            "C-beamcon0.c18, C-beam.b14, C-beam.c03 .");
    }

    // ================= BEAMCON0-LOLDIS-DISABLES-LONG-LINE-TOGGLE =================
    // long_line toggles every end_of_line under NTSC (pal=0) UNLESS
    // loldis&&varbeamen (l.292-300); read it back through VPOSR bit7 (its
    // only externally observable form, per VPOSR-IDENTIFICATION-FIELD-
    // BITS-14-7 above).
    {
        Harness h(/*ecs=*/true, /*ntsc=*/true); // pal=0 at reset
        h.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN)); // loldis=0: control
        std::vector<int> lol_control;
        {
            bool prev_eol = false;
            for (int i = 0; i < 4000 && lol_control.size() < 4; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool eol_now = h.dut->eol;
                if (eol_now && !prev_eol) lol_control.push_back((h.read(A_VPOSR) >> 7) & 1);
                prev_eol = eol_now;
            }
        }
        bool control_toggles = lol_control.size() == 4 &&
            lol_control[0] != lol_control[1] && lol_control[1] != lol_control[2] &&
            lol_control[2] != lol_control[3];

        Harness h2(/*ecs=*/true, /*ntsc=*/true);
        h2.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_LOLDIS));
        std::vector<int> lol_loldis;
        {
            bool prev_eol = false;
            for (int i = 0; i < 4000 && lol_loldis.size() < 4; ++i) {
                h2.dut->clk7_en = (i % 2 == 0);
                h2.dut->cck = (i % 4 < 2);
                h2.tick();
                bool eol_now = h2.dut->eol;
                if (eol_now && !prev_eol) lol_loldis.push_back((h2.read(A_VPOSR) >> 7) & 1);
                prev_eol = eol_now;
            }
        }
        bool loldis_frozen = lol_loldis.size() == 4 &&
            lol_loldis[0] == lol_loldis[1] && lol_loldis[1] == lol_loldis[2] &&
            lol_loldis[2] == lol_loldis[3] && lol_loldis[0] == 0;

        checks.expect(
            control_toggles && loldis_frozen,
            "BEAMCON0-LOLDIS-DISABLES-LONG-LINE-TOGGLE: under NTSC "
            "(pal=0), VPOSR's LOL bit (bit7) toggles line by line with "
            "LOLDIS=0 (the control) but stays frozen at 0 with LOLDIS=1, "
            "on the identical per-line VPOSR-readback measurement -- '11 "
            "LOLDIS Disable long line/short line toggle' (p.305). "
            "C-beamcon0.c5, C-beam.b11 .");
    }

    // ================= BEAMCON0-PAL-BIT-SELECTS-DEFAULT-VTOTAL =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/true); // hardware pin says NTSC
        // varbeamen stays 0: `vtotal` takes the `pal ? PAL : NTSC` default
        // branch (l.229), NOT vtotal_reg -- so this isolates the `pal`
        // REGISTER (written here) from the `ntsc` PIN (left at 1 throughout).
        h.write_reg(A_BEAMCON0, 1 << BC_PAL);
        force_vpos(h, 0, /*long_frame=*/false);
        int lines = 0;
        {
            bool prev_eol = false, prev_eof = false;
            for (int i = 0; i < 320 * 240 * 4; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool eol_now = h.dut->eol;
                if (eol_now && !prev_eol) ++lines;
                prev_eol = eol_now;
                if (h.dut->eof && !prev_eof) break;
                prev_eof = h.dut->eof;
            }
        }
        checks.expect(
            lines == 312,
            "BEAMCON0-PAL-BIT-SELECTS-DEFAULT-VTOTAL: with the ntsc INPUT "
            "PIN held at 1 throughout, writing BEAMCON0 bit 5 (PAL) "
            "switches the hardwired-default field length from 262 to 312 "
            "lines (measured " +
                std::to_string(lines) +
                ") -- the software register, not the pin, decides the "
                "default once written. '5 PAL Programmable pal mode "
                "enable' (p.305). C-beamcon0.c11, C-beam.b05 .");
    }

    // Shared helper for the field-length loops below: counts eol pulses per
    // field and records up to `want_fields` consecutive field lengths.
    auto measure_field_lengths = [](Harness& h, int want_fields, int budget_ticks) {
        std::vector<int> lens;
        int cur = 0;
        bool prev_eol = false, prev_eof = false;
        for (int i = 0; i < budget_ticks && static_cast<int>(lens.size()) < want_fields; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            bool eol_now = h.dut->eol;
            if (eol_now && !prev_eol) ++cur;
            prev_eol = eol_now;
            bool eof_now = h.dut->eof;
            if (eof_now && !prev_eof) {
                lens.push_back(cur);
                cur = 0;
            }
            prev_eof = eof_now;
        }
        return lens;
    };

    // ================= VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        h.write_reg(A_VTOTAL, 5); // vtotal_reg = 5
        // long_frame defaults to 1 out of reset (l.343-344) -- force it to
        // 0 explicitly, same reasoning as PAL-FIELD-312-NTSC-FIELD-262-LINES
        // above, so this (LACE=0) case measures the plain VTOTAL+1 formula
        // rather than the long_frame=1 extra-line path.
        force_vpos(h, 0, /*long_frame=*/false);

        // (a) LACE=0 (default): field length = vtotal+1 = 6.
        auto len_a = measure_field_lengths(h, 1, 30 * 240 * 4);
        checks.expect(
            len_a.size() == 1 && len_a[0] == 6,
            "VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (non-interlaced): "
            "with VTOTAL programmed to 5 and LACE=0, a field is exactly "
            "VTOTAL+1=6 lines -- 'VTOTAL contains the line number at which "
            "to reset the vertical position counter' (p.304), 'This value "
            "represents the number of lines in a field(+1). The exception "
            "is if the INTERLACE bit is set (BPLCON0)' (p.304). C-scan.c9, "
            "C-vtotal.c02, C-vtotal.c03, opus:7-appC-beam.c5, opus:7-appC-beam.c6 .");

        // (b) LACE=1: alternating VTOTAL+2 / VTOTAL+1 = 7/6. As in
        // LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION above, force a
        // clean, known long_frame via an explicit VPOSW write right after
        // enabling LACE, rather than trust the transitional pipeline state.
        h.write_reg(A_BPLCON0, 1 << BP_LACE);
        force_vpos(h, 0, /*long_frame=*/true);
        auto len_b = measure_field_lengths(h, 2, 30 * 240 * 4);
        bool alternates_7_6 = len_b.size() == 2 &&
            ((len_b[0] == 7 && len_b[1] == 6) || (len_b[0] == 6 && len_b[1] == 7));
        checks.expect(
            alternates_7_6,
            "VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (interlaced): with "
            "VTOTAL still 5 and LACE=1, two consecutive fields are 7 and 6 "
            "lines (VTOTAL+2 and VTOTAL+1) -- 'In this case this value "
            "represents the number of lines in the long field (+2) and the "
            "number of lines in the short field (+1)' (p.304), 'exception "
            "is if the INTERLACE bit is set (BPLCON0). In this case ... "
            "long field (+2) ... short field (+1)' (p.304 x2). C-scan.c10, "
            "C-scan.c11, C-scan.c12, C-vtotal.c04, C-vtotal.c05, "
            "C-vtotal.c06, opus:7-appC-beam.c7 .");
    }

    // Unambiguous SUM check using the REAL PAL/NTSC hardwired defaults
    // (varbeamen left at 0) -- sidesteps which single field is "the" field
    // length (the long_frame-out-of-reset question of NM-HRM-001, which is
    // withdrawn) by checking the
    // total of one long plus one short field, which is the same number
    // regardless of which one comes first.
    for (bool ntsc : {false, true}) {
        Harness h(/*ecs=*/false, ntsc);
        h.write_reg(A_BPLCON0, 1 << BP_LACE);
        auto lens = measure_field_lengths(h, 2, 2 * 320 * 240 * 4);
        int sum = (lens.size() == 2) ? (lens[0] + lens[1]) : -1;
        int expected_sum = ntsc ? 525 : 625;
        checks.expect(
            sum == expected_sum,
            std::string("VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (") +
                (ntsc ? "NTSC" : "PAL") +
                " default, sum of one long + one short field): with the "
                "hardwired default VTOTAL and LACE=1, two consecutive "
                "fields sum to " +
                std::to_string(expected_sum) +
                " lines regardless of which comes first (measured sum=" +
                std::to_string(sum) +
                ") -- '" + (ntsc ? "NTSC (525 lines, 227.5 colorclocks per scan line)"
                                 : "PAL  (625 lines, 227.5 colorclocks per scan line)") +
                "' (p.304). " + (ntsc ? "C-scan.c01 ." : "C-scan.c03 ."));
    }

    // ================= COLOUR-CLOCKS-PER-LINE-227POINT5 =================
    // cck is an input this harness drives itself (Harness::run_cck(), "One
    // full colour-clock period" per call, l.427-440 above) and eol is an
    // output (`assign eol = vpos_inc;`, agnus_beamcounter.v:320) -- so,
    // exactly as ch3-field-geometry's measure_field_period_seconds counts
    // raw ticks between two eof pulses and divides by 4 to get CCK cycles
    // for one field, colour clocks per LINE is the number of run_cck()
    // periods between two eol pulses. No htotal_out tick-to-CCK conversion
    // factor is involved anywhere below.
    auto measure_cck_periods_per_line = [](Harness& h, long tick_bound) -> double {
        long i = 0;
        int eol_edges = 0;
        long ticks_second_line = 0;
        bool eol_prev = false;
        for (; i < tick_bound && eol_edges < 2; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            if (eol_edges == 1) ++ticks_second_line;
            bool eol_now = h.dut->eol;
            if (eol_now && !eol_prev) ++eol_edges;
            eol_prev = eol_now;
        }
        if (eol_edges < 2) return -1.0;
        return static_cast<double>(ticks_second_line) / 4.0;
    };

    // A single line cannot last 227.5 colour clocks -- a line ends on a
    // whole clock, so any one line's length is a whole number of run_cck()
    // periods. "227.5 colorclocks per scan line" (p.304) can only be an
    // AVERAGE over lines of alternating length, so the NTSC/PAL and bundled
    // checks below measure several consecutive lines and average them,
    // rather than comparing one line's length against a fractional target.
    // Same edge-counting technique as measure_cck_periods_per_line above,
    // generalized to keep closing off one line's tick count each time a
    // further eol edge is seen, instead of stopping after the first one.
    auto measure_cck_periods_for_lines = [](Harness& h, int n_lines, long tick_bound) -> std::vector<double> {
        std::vector<double> periods;
        long i = 0;
        int eol_edges = 0;
        long ticks_this_line = 0;
        bool eol_prev = false;
        for (; i < tick_bound && static_cast<int>(periods.size()) < n_lines; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            if (eol_edges >= 1) ++ticks_this_line;
            bool eol_now = h.dut->eol;
            if (eol_now && !eol_prev) {
                ++eol_edges;
                if (eol_edges >= 2) {
                    periods.push_back(static_cast<double>(ticks_this_line) / 4.0);
                    ticks_this_line = 0;
                }
            }
            eol_prev = eol_now;
        }
        return periods;
    };

    // Control (no claim id): with VARBEAMEN=1 and HTOTAL written to a known
    // value, the measured period count must equal exactly what was
    // written -- htotal_reg is the written byte doubled (l.206) and
    // end_of_line compares hpos against that same value with its own LSB
    // forced to 0 (l.250), and hpos advances by 2 raw ticks per run_cck()
    // period (established at RASTER-SWEEP-LEFT-TO-RIGHT-TOP-TO-BOTTOM
    // above), so a written HTOTAL of N makes a line exactly N periods long.
    // Same technique as VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA
    // (non-interlaced) already uses against a known written VTOTAL for the
    // vertical axis, applied here to the horizontal one -- proves the
    // comparator this section depends on is live before trusting it against
    // a disputed value below.
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
#ifndef MUT_COLOUR_CLOCKS_PER_LINE_CONTROL
        h.write_reg(A_HTOTAL, 100); // htotal_reg = 200 raw ticks = 100 CCK periods/line
#else
        h.write_reg(A_HTOTAL, 120); // MUTATION: written value changed, periods must track it
#endif
        force_vpos(h, 0, /*long_frame=*/false);
        double periods = measure_cck_periods_per_line(h, 300 * 4);
        // Measured (not assumed): writing HTOTAL=100 reads back 101
        // periods, one more than the written byte -- the identical +1
        // relationship this module's own default exhibits (HTOTAL_VAL=226
        // written, "line length of 227 CCKs" is its own comment, matching
        // the 227 measured on the hardwired default below). The control
        // asserts this fixed, repeatable relationship, not a guessed one.
        checks.expect(
            periods == 101.0,
            "COLOUR-CLOCKS-PER-LINE-227POINT5 (control, no claim id): with "
            "HTOTAL written to 100 (200 raw ticks), counting run_cck() "
            "periods between two eol pulses reads exactly " +
                std::to_string(periods) +
                " -- one more than the written byte, the same relationship "
                "the hardwired default exhibits below (226 written, 227 "
                "measured) -- proving the run_cck()-periods-between-two-"
                "eol-pulses comparator used below tracks a written value in "
                "a fixed, repeatable way, with no htotal_out tick-to-CCK "
                "conversion involved.");
    }

    // NTSC/PAL hardwired default (VARBEAMEN=0): 'NTSC (525 lines, 227.5
    // colorclocks per scan line)' / 'PAL  (625 lines, 227.5 colorclocks per
    // scan line)' (p.304). opus:7-appC-beam.c2 and opus:7-appC-beam.c3
    // restate the identical sentence as one unsplit claim, bundling this
    // colour-clocks-per-line half together with the line-count half that
    // VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (NTSC/PAL default, sum of
    // one long + one short field) above already establishes (525/625,
    // PASS) -- so the bundled ids are graded on both halves together below,
    // not on this half alone.
    for (bool ntsc : {false, true}) {
        Harness h(/*ecs=*/false, ntsc);
        force_vpos(h, 0, /*long_frame=*/false);
        const int kLinesMeasured = 4;
        auto line_periods = measure_cck_periods_for_lines(h, kLinesMeasured, 1500 * 4);
        double sum_periods = 0.0;
        std::string per_line_list;
        for (size_t li = 0; li < line_periods.size(); ++li) {
            sum_periods += line_periods[li];
            if (li) per_line_list += ", ";
            per_line_list += std::to_string(line_periods[li]);
        }
        double avg_periods = (line_periods.size() == static_cast<size_t>(kLinesMeasured))
                                  ? sum_periods / kLinesMeasured
                                  : -1.0;
        bool periods_match_manual = (avg_periods == 227.5);
        checks.expect(
            periods_match_manual,
            std::string("COLOUR-CLOCKS-PER-LINE-227POINT5 (") +
                (ntsc ? "NTSC" : "PAL") +
                "): a single line cannot last 227.5 colour clocks (a line "
                "ends on a whole clock), so '227.5 colorclocks per scan "
                "line' (p.304) can only be an average over lines of "
                "alternating length -- counting run_cck() periods between "
                "consecutive eol pulses over " +
                std::to_string(kLinesMeasured) +
                " consecutive lines on the hardwired default (VARBEAMEN=0) "
                "reads [" + per_line_list + "], average " +
                std::to_string(avg_periods) +
                ", not the manual's 227.5 -- '" +
                (ntsc ? "NTSC (525 lines, 227.5 colorclocks per scan line)"
                      : "PAL  (625 lines, 227.5 colorclocks per scan line)") +
                "' (p.304). This module's own default-value comment agrees "
                "with the measurement, not the manual: 'line length of 227 "
                "CCKs in PAL mode (NTSC line length of 227.5 CCKs is not "
                "supported)' (agnus_beamcounter.v:86) -- every line reads "
                "the same length (no alternation), so the average is that "
                "same length; a genuine, measured disagreement on the same "
                "run_cck()-periods-between-two-eol-pulses comparator the "
                "control above validates, with no RTL-derived tick-to-CCK "
                "conversion involved. " +
                (ntsc ? "C-scan.c02 ." : "C-scan.c04 ."));

        // Re-derive the same-basis line-count half independently (not a
        // second PASS/FAIL line for a fact already graded above -- a local
        // value only, so the bundled claim below is graded on both halves
        // at once, per its own unsplit restatement).
        Harness h2(/*ecs=*/false, ntsc);
        h2.write_reg(A_BPLCON0, 1 << BP_LACE);
        auto lens2 = measure_field_lengths(h2, 2, 2 * 320 * 240 * 4);
        int sum2 = (lens2.size() == 2) ? (lens2[0] + lens2[1]) : -1;
        int expected_sum2 = ntsc ? 525 : 625;
        bool lines_match_manual = (sum2 == expected_sum2);
        checks.expect(
            lines_match_manual && periods_match_manual,
            std::string("COLOUR-CLOCKS-PER-LINE-227POINT5 (") +
                (ntsc ? "NTSC" : "PAL") +
                " bundled with line count): the unsplit sentence states both "
                "halves together -- lines half " +
                (lines_match_manual ? std::string("matches") : std::string("does not match")) +
                " the manual (measured sum=" + std::to_string(sum2) + " vs " +
                std::to_string(expected_sum2) +
                ", the same measurement as VTOTAL-PLUS-ONE-PLUS-TWO-"
                "INTERLACE-FORMULA (" + (ntsc ? "NTSC" : "PAL") +
                " default, sum of one long + one short field) above), "
                "colour-clocks-per-line half " +
                (periods_match_manual ? std::string("matches") : std::string("does not match")) +
                " the manual (measured average " + std::to_string(avg_periods) +
                " over " + std::to_string(kLinesMeasured) +
                " lines [" + per_line_list +
                "] vs 227.5, graded immediately above) -- both halves must "
                "hold for this unsplit claim to hold, and one does not. '" +
                (ntsc ? "NTSC (525 lines, 227.5 colorclocks per scan line)"
                      : "PAL  (625 lines, 227.5 colorclocks per scan line)") +
                "' (p.304). " +
                (ntsc ? "opus:7-appC-beam.c2 ." : "opus:7-appC-beam.c3 ."));
    }

    // ================= VBLANK-DURATION-NTSC-MATCHES-MANUAL / VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL =================
    // vbl = (vpos <= vbstop) (l.444), continuous on vpos -- track vpos
    // changes directly (rather than eol, which pulses at a different point
    // in the line than the actual vpos update, per the timing worked out
    // for VPOSR-IDENTIFICATION-FIELD-BITS-14-7 above) and count how many
    // consecutive lines from vpos=0 read vbl=1.
    for (bool ntsc : {false, true}) {
        Harness h(/*ecs=*/false, ntsc);
        int blanked_lines = 0;
        bool counting = true;
        if (h.dut->vbl) ++blanked_lines;
        int prev_vpos = h.dut->vpos;
        for (int i = 0; i < 60 * 240 * 4 && counting; ++i) {
            h.dut->clk7_en = (i % 2 == 0);
            h.dut->cck = (i % 4 < 2);
            h.tick();
            int cur_vpos = h.dut->vpos;
            if (cur_vpos != prev_vpos) {
                if (h.dut->vbl) {
                    ++blanked_lines;
                } else {
                    counting = false;
                }
                prev_vpos = cur_vpos;
            }
        }
        int manual_value = ntsc ? 21 : 29; // "Vertical Blank Stop $15(21) NTSC / $1D(29) PAL"
        int manual_displayable_normal = ntsc ? 241 : 283;
        int manual_displayable_interlaced = ntsc ? 483 : 567;
        (void)manual_displayable_normal;
        (void)manual_displayable_interlaced;
        if (ntsc) {
            checks.expect(
                blanked_lines == manual_value,
                "VBLANK-DURATION-NTSC-MATCHES-MANUAL: vbl reads high for "
                "exactly 21 consecutive lines from vpos=0 under NTSC "
                "(measured " +
                    std::to_string(blanked_lines) +
                    "), matching the manual's own arithmetic (262 total "
                    "lines - 21 blanked = 241 displayable, matching the "
                    "printed 241; 525 - 21*2 = 483, matching the printed "
                    "interlaced 483) -- 'No data can be displayed in the "
                    "vertical blanking area' (p.79), 'Vertical Blank Start "
                    "0' / 'Vertical Blank Stop $15 (21)' (Table 3-13, "
                    "p.79), 'Displayable lines of screen video 241 483' "
                    "(Table 3-13, p.79), '=525-(21*2)' (Table 3-13, p.79). "
                    "3-max-window.c2, 3-max-window.c4, 3-max-window.c6, "
                    "3-max-window.c7, 3-t13.c01, 3-t13.c03, 3-t13.c05, "
                    "3-t13.c06, 3-t13.c09, 3-max.c02, 3-max-window.c1. Chapter 8 states this same boundary as a POSITION where this assertion measures a COUNT: 'This freeze is released at the end of internal vertical blanking (vertical position 20)' (p.239) and 'the light pen has not triggered since the last top-of-screen (V = 20)' (p.239). Position 20 is the last of the 21 lines measured here -- lines 0 through 20 -- which is the same fact counted the other way. opus:8-lpen.c40, opus:8-lpen.c44 .");
        } else {
            checks.expect(
                blanked_lines == manual_value,
                "VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL: the manual's "
                "Table 3-13 states PAL 'Vertical Blank Stop' as $1D (29) "
                "blanked lines (312 - 29 = 283 displayable, printed "
                "directly; 625 - 29*2 = 567, printed directly as the "
                "interlaced figure) -- vbl instead reads high for " +
                    std::to_string(blanked_lines) +
                    " consecutive lines from vpos=0, not 29, on the "
                    "identical same-basis measurement as "
                    "VBLANK-DURATION-NTSC-MATCHES-MANUAL above (a PASS), "
                    "which is this assertion's control. This module's own "
                    "reset parameter is commented 'PAL 26 lines' "
                    "(agnus_beamcounter.v:89), 3 lines short of Table "
                    "3-13's 29 -- a real, measured disagreement with the "
                    "table, not a measurement artifact. The manual gives "
                    "the PAL blank a second time: printed p.219 ends the "
                    "range at line 25 ('The range starts at line 0 and "
                    "ends at line 20 for NTSC or line 25 for PAL'), which "
                    "the measured 26 lines match read inclusively; "
                    "hrm/ch7-vertb-vertical-blank-timing grades that "
                    "reading as PAL-VBLANK-ENDS-BY-LINE-26, green, and no "
                    "implementation can be green in both. 'Vertical Blank Start 0' / "
                    "'Vertical Blank Stop $1D (29)' (Table 3-13, p.79), "
                    "'Displayable lines of screen video ... 283 567' "
                    "(Table 3-13, p.79), '=625-(29*2)' (Table 3-13, p.79). "
                    "3-max-window.c3, 3-max-window.c5, 3-max-window.c8, "
                    "3-max-window.c9, 3-t13.c02, 3-t13.c04, 3-t13.c07, "
                    "3-t13.c08, 3-t13.c10 .");
        }
    }

    // ================= NOT ASSERTED, GROUPED BY REASON =================

    checks.record(
        "NOT ASSERTED (visible/combined-field resolution numbers -- 20 "
        "claims): 3-video.c25, 3-video.c26, 3-res.c05, 3-res.c06, "
        "3-res.c15, 3-resolution.c8, 3-resolution.c9, 3-resolution.c12, "
        "3-resolution.c13, 3-resolution.c18, 3-lace.f02, 3-lace.f05, "
        "3-basic.c04, 3-basic.c06, 3-basic-size.c4, 3-basic-size.c6, "
        "3-t6.c03, 3-t6.c04, 3-sum.c07, 3-sum.c08. All state the same "
        "family of numbers -- '400 lines NTSC / 512 lines PAL' interlaced, "
        "or the halved diagonal-step figure of Figure 3-6 -- describing "
        "how many VISIBLE lines or how fine an edge two combined fields "
        "produce once bitplane data is actually drawn into them. "
        "agnus_beamcounter has no bitplane, DIW or pixel-rendering port at "
        "all (BOUNDARY above); it produces vpos/vbl/the field1 flip "
        "(demonstrated distinctly by LACE-BIT-CONTROLS-INTERLACE-FIELD-"
        "ALTERNATION and PAL-FIELD-312-NTSC-FIELD-262-LINES above), not "
        "the doubled VISIBLE resolution that results once Denise paints "
        "two vertically-offset fields together. Settling these needs the "
        "bitplane/display-window boundary, not this fixture.");

    checks.record(
        "NOT ASSERTED (horizontal-resolution modes and the blitter -- 4 "
        "claims): 3-video.c27, 3-res.c19, 3-resolution.c19, "
        "3-resolution.c24. c27 is about combining horizontal-resolution "
        "(hires/shres) modes with interlace -- this module has no BPLCON0 "
        "HIRES/SHRES port. c19 (both readings) is about how the BLITTER "
        "DMA channel treats an interlaced playfield when drawing lines -- "
        "agnus_beamcounter has no blitter port. c24 is about bitplane "
        "MEMORY sizing for interlaced playfields -- a bitplane-DMA-channel "
        "concern, not a beam-counter one. None of these are observable at "
        "this boundary.");

    checks.record(
        "NOT ASSERTED (display window, bitplane modulo/pointer addressing "
        "and memory layout -- 19 claims): 3-diw.c05, 3-diw.c12, 3-mem.c01, "
        "3-memory.c21, 3-memory.c22, 3-memory.c23, 3-mod.c10, 3-mod.c11, "
        "3-mod.c12, 3-modulo.c20, 3-modulo.c21, 3-modulo.c24, "
        "3-modulo.c25, 3-modulo.c26, 3-modulo.c27, 3-large.c15, "
        "3-vscr.c05, 3-max.c01, 3-big.f08. DIWSTRT/DIWSTOP resolution and "
        "sweep area (3-diw), bitplane pointer/modulo arithmetic across "
        "interlaced odd/even scans (3-mod/3-modulo), bitplane memory "
        "sizing and per-line memory addresses (3-mem/3-memory), a "
        "bitplane-pointer worked example (3-large.c15), vertical-scroll "
        "pointer manipulation (3-vscr.c05), a playfield-size framing "
        "statement that bundles line count with COLUMN count "
        "(3-max.c01 -- the column half pulls in DDF/horizontal fetch "
        "width, outside this boundary regardless of the line half already "
        "covered by PAL-FIELD-312-NTSC-FIELD-262-LINES above), and a "
        "figure about the sprite/copper addressable vertical range "
        "(3-big.f08, p.77) are all bitplane-DMA-address or DDF-window "
        "matters with no corresponding port on agnus_beamcounter (BOUNDARY "
        "above lists every port this module has). Settling any of these "
        "needs the bitplane-DMA-channel or copper boundary, not this "
        "fixture.");

    checks.record(
        "NOT ASSERTED (BEAMCON0 bits with the corresponding wire commented "
        "out in agnus_beamcounter.v, or with no port to show the wired "
        "effect -- 10 claims): C-beamcon0.c10 (DUAL, bit6), C-beamcon0.c14 "
        "(CSYTRUE, bit2), C-beamcon0.c15 (VSYTRUE, bit1), C-beamcon0.c16 "
        "(HSYTRUE, bit0), C-beamcon0.c3 (LPENDIS, bit13), C-beam.b06 "
        "(DUAL), C-beam.b02 (CSYTRUE), C-beam.b01 (VSYTRUE), C-beam.b00 "
        "(HSYTRUE), C-beam.b13 (LPENDIS). BEAMCON0 is write-only in this "
        "module (no read-mux case, l.108-115), so each of these bits can "
        "only be tested through a wired downstream effect -- and "
        "displaydual (l.136), csynctrue/vsynctrue/hsynctrue (l.140-142) "
        "are each declared as a commented-out `//wire` with nothing "
        "reading beamcon0_reg at that bit position anywhere else in the "
        "file; _hsync/_vsync/_csync are generated with fixed polarity "
        "regardless of what is written there. LPENDIS (l.129, also "
        "commented out) additionally has no lightpen port on this module "
        "at all to show 'ignore the latched pen value' against. Writing "
        "any of these five bits changes nothing this fixture can observe.");

    checks.record(
        "NOT ASSERTED (HTOTAL bits 15:8, 'must be zero' -- 9 claims): "
        "C-htotal-bits.c1, C-htotal-bits.c2, C-htotal-bits.c3, "
        "C-htotal-bits.c4, C-htotal-bits.c5, C-htotal-bits.c6, "
        "C-htotal-bits.c7, C-htotal-bits.c8 (bits 15,14,13,12,11,10,9,8), "
        "C-htotal.c02. HTOTAL "
        "is write-only here (same reasoning as the BEAMCON0 note above): "
        "the write-decode unconditionally takes data_in[7:0] into "
        "htotal_reg[8:1] (l.206) and silently discards data_in[15:8] "
        "regardless of its value -- there is no way to distinguish "
        "'rejected because bits 15:8 must be 0' from 'ignored because only "
        "the low byte was ever read' by observing htotal_out, the only "
        "externally visible effect of a HTOTAL write. "
        "HTOTAL-H8-H1-BIT-MAPPING above asserts the half of "
        "the same quoted row (h8..h1) that IS observable this way. "
        "C-htotal.c02 restates the same row as one unsplit claim ('HTOTAL "
        "bits 7-0 carry h8..h1 and bits 15-8 are 0') bundling both halves "
        "together -- the h8..h1 half is established (HTOTAL-H8-H1-BIT-"
        "MAPPING) but the bits-15:8-zero half is not, for the identical "
        "write-only reason as the eight C-htotal-bits ids above, so the "
        "unsplit id is "
        "withdrawn whole here rather than credited on the established half "
        "alone. This id is not about colour clocks or 280ns at all, and so "
        "does not belong on the DECLINED note below despite sharing its "
        "printed page.");

    checks.record(
        "DECLINED (translating manual colour-clock/280ns quantities into "
        "htotal_out's raw tick count needs an RTL-supplied conversion "
        "factor -- 2 claims): C-htotal.c03, C-scan.c8. "
        "Asserting them "
        "as HTOTAL-FIXED-NOT-FRACTIONAL-227POINT5 -- 'HTOTAL W A Highest "
        "number count in horizontal line' / 'The value in this register "
        "represents the number of 280ns increments on the horizontal "
        "line' and '   NTSC (525 lines, 227.5 colorclocks per scan "
        "line)' / '   PAL  (625 lines, 227.5 colorclocks per scan line)' "
        "(p.304), against htotal_out's hardwired-default reading (452, "
        "both standards, GIVEN MEASUREMENT) via `want = 455` "
        "(227.5 CCK * 2 hpos-ticks/CCK) -- needs a '2 hpos-ticks/CCK' "
        "factor that is not written on p.304 or anywhere else this file "
        "cites: p.304 "
        "states only that HTOTAL's WRITTEN value 'represents the number "
        "of 280ns increments on the horizontal line', and a colour clock "
        "is independently given as approximately 280ns on an uncited page "
        "('there are 227.5 \"color clocks\", or memory access cycles. A "
        "memory cycle is approximately 280 ns in duration', p.194) -- "
        "neither page says what one raw htotal_out tick (this module's "
        "own output, not a manual-named register) is worth in colour "
        "clocks or nanoseconds. That correspondence exists only inside "
        "agnus_beamcounter.v's own write-decode (`htotal_reg <= "
        "{data_in[7:0], 1'b0}`, l.206), its hardwired default "
        "(`HTOTAL_VAL << 1`, l.223) and its own port comment ('output reg "
        "[8:0] hpos, // horizontal beam counter (140ns)', l.40). Any "
        "comparison between the manual's 227.5-CCK figure and a raw "
        "htotal_out count would have to import that doubling from the "
        "RTL to even be stated in the same unit, making the RTL its own "
        "oracle -- exactly what HTOTAL-H8-H1-BIT-MAPPING above avoids "
        "(see NO CONVERSION FACTOR FROM THE RTL near the top of this "
        "file). The underlying observation may still be correct -- "
        "agnus_beamcounter.v's own HTOTAL_VAL comment reads 'NTSC line "
        "length of 227.5 CCKs is not supported' (l.86) -- but this module "
        "gives no manual-anchored unit to state that claim in at this "
        "boundary, so no tick-count assertion is made here. What IS "
        "observable without that conversion -- the h1..h8 bit-weight "
        "ordering, PAL vblank duration disagreeing with the manual on a "
        "plain line-count basis, and colour clocks per line (a different "
        "quantity from htotal_out's raw ticks: cck is an input this "
        "harness drives itself, so counting run_cck() periods between two "
        "eol pulses needs no such conversion) -- is already covered by "
        "HTOTAL-H8-H1-BIT-MAPPING, VBLANK-DURATION-PAL-DISAGREES-WITH-"
        "MANUAL and COLOUR-CLOCKS-PER-LINE-227POINT5 above.");

    checks.record(
        "NOT ASSERTED (ports this module does not have -- 3 claims): "
        "C-genlock-ext.c15 (BPLCON3 bit 5, BRDRBLNK -- no BPLCON3/genlock "
        "port), C-spr.c04 (SPRxPOS/SPRxCTL loaded by the sprite DMA "
        "channel -- no sprite-DMA port), C-sync.c02 ('Programmable "
        "blanking is implemented through four new ECS Agnus registers:' -- "
        "the claims.jsonl quote is truncated at the colon before the list "
        "of four registers actually names them, so which four are meant "
        "cannot be checked against this module's own longer register list "
        "(BEAMCON0, HTOTAL, HSSTOP, HBSTRT, HBSTOP, VTOTAL, VSSTOP, "
        "VBSTOP, HSSTRT, VSSTRT, HCENTER -- eleven, not four) without "
        "guessing which subset the manual meant).");

    checks.record(
        "NOT ASSERTED (prescriptive, not a hardware behaviour -- 1 claim): "
        "C-beamcon0-guidance.c2: 'Do not write to this register [directly] "
        ".. Incorrectly writing directly to BEAMCON0 has the (remote) "
        "possibility of destroying your multisync monitor.' This is "
        "programming advice about what a programmer should avoid doing, "
        "not a statement about what the hardware does -- there is no "
        "'incorrect write' outcome for a Verilator harness to observe or "
        "fail to observe; the harness writes BEAMCON0 directly and "
        "repeatedly, by design, throughout this file.");

    checks.record(
        "NOT ASSERTED (physical pad direction -- 2 claims): A-leg.ersy2 "
        "('ERSY=External resync (HSYNC, VSYNC pads become inputs)'), "
        "A-bplcon.c77 (the same sentence, sol's reading). "
        "agnus_beamcounter's ports have fixed, synthesis-time directions "
        "(_hsync and _vsync are `output reg`, not bidirectional pads); "
        "there is no pad-direction change for a module-level Verilator "
        "harness to observe. "
        "ERSY-BIT-CONTROLS-EXTERNAL-RESYNC-HOLD above asserts the closest "
        "observable proxy within this boundary -- the horizontal counter "
        "holding at hpos=0 with ERSY=1, as if waiting for an external "
        "pulse this module has no pin to receive.");

    checks.record(
        "NOT ASSERTED (no fixed VGA default in this RTL -- 3 claims): "
        "C-scan.c05, C-scan.c06, opus:7-appC-beam.c4 ('VGA (525 lines, "
        "114.0 colorclocks per scan line)'). BEAMCON0 has no 'select VGA' bit -- htotal/vtotal "
        "under ECS are either the hardwired PAL/NTSC constants or whatever "
        "value software explicitly writes into HTOTAL/VTOTAL (l.223,229); "
        "there is no third, VGA-specific default anywhere in this module "
        "to compare 114.0 colorclocks against. A program COULD write "
        "114*2=228 into HTOTAL to get a VGA-like line length, but that "
        "would be demonstrating HTOTAL-H8-H1-BIT-MAPPING's "
        "already-asserted write path again, not a VGA-specific default.");

    checks.record(
        "NOT ASSERTED (word-based DMA-fetch bound, not a raw CRT-timing "
        "duration -- 3 claims): 3-max.c08, 3-max.c09, 3-max-window.c15 "
        "('horizontal blanking actually limits the displayable video to "
        "368 low resolution pixels (23 words)'). A direct hblank-duration "
        "measurement (visible = htotal_out minus the number of hpos-ticks "
        "hblank is asserted) cannot settle it: under "
        "this module's own low-res-pixel unit (1 low-res pixel = 1 CCK = "
        "280ns, matching 'the value in this register represents the "
        "number of 280ns increments' on the very same page), 368 low-res "
        "pixels is 368*280ns = 103040ns -- longer than an entire PAL "
        "scan line (htotal_out=452 ticks * 140ns = 63280ns). The figure "
        "cannot be a duration within one line under that unit, so it must "
        "instead be a fetch-capacity bound expressed in bitplane WORDS "
        "(23 words * 16 bits = 368), constrained by the DMA slots "
        "available during the visible portion of a line -- a bitplane-DMA "
        "concern this module's own hblank/htotal_out signals cannot settle "
        "alone. Asserting a specific tick-count comparison here would test "
        "an assumed unit conversion, not the manual's claim; this needs "
        "the bitplane-DMA-channel boundary.");

    // Chapter 8 routes opus:8-lpen.c22 / sol:8-pen.c24 here. p.238 and
    // p.299 do not contradict each other: p.288 defines "(E)" as "changed in
    // the Enhanced Chip Set", p.289 marks VPOSR "A (E)", and p.299 sits in
    // Appendix C under "ECS Hardware and the Graphics Library". Two chipset
    // scopes, not two descriptions of one.
    //
    // The real tension is p.238 against p.216/p.285, and it is
    // registered: hrm/ch7-beam-position-word-layout grades "the 14-1 band
    // carries no value under OCS" -- green at ntsc=0, RED at ntsc=1, which is
    // NM-HRM-034. p.238 says that band IS the chip identification code, which
    // weighs FOR the implementation and against the manual's own p.216/p.285.
    checks.record(
        "NOT ASSERTED -- this band is contested between three printed pages (2 claims): opus:8-lpen.c22, "
        "sol:8-pen.c24. Printed p.238 says 'Bits 14-1  Chip ID code.  Do not "
        "depend on value!'. That band is graded under OCS by "
        "hrm/ch7-beam-position-word-layout, which grades the OPPOSITE reading (p.216/p.285: the band carries no value) and is RED there at ntsc=1, not "
        "here -- this package grades the band's sub-fields "
        "(VPOSR-IDENTIFICATION-FIELD-BITS-14-7, VPOSR-BITS-6-3-UNUSED, "
        "VPOSR-BITS-2-0-VERTICAL-POSITION) and each is green. MEASURED here: "
        "bits 13/12 follow the ecs and ntsc inputs and bits 9-8 follow aga, so "
        "the upper band is a readable identification field; and stepping "
        "vpos[10:8] from 0b000 to 0b111 changes VPOSR by 0x0007, so bits 2 and "
        "1 carry V10 and V9. p.299 is NOT in conflict with p.238: it is "
        "Appendix C's ECS material and VPOSR is marked '(E)' -- changed in "
        "ECS -- on p.289, defined on p.288. What p.238 does conflict with is "
        "p.216 and p.285, which call the same band unused; that conflict is "
        "NM-HRM-034 and p.238 is printed evidence on the implementation's "
        "side of it.");


    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

