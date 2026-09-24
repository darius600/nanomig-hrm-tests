// Test purpose: DMACON ($096) and DMACONR ($002) as chapter 4's
//   sprite-facing view of DMA control - SPREN (sprite DMA enable, bit 5),
//   the master DMAEN (bit 9), the DMACONR read-back register, and which bits
//   a write can and cannot reach (DMACON's write-only address; DMACONR's
//   read-only address; BBUSY/BZERO's read-only bits within it).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 278 (Appendix A custom register table). Every claim below cites
//   page 278; none of this package's claims cite any other page.
// Non-goal: reproducing hrm/ch7-dmacon-register-bits. That package already builds
//   this exact fixture (`agnus`, the full chipset top level) and already
//   measures most of the per-bit and register-mechanics propositions this
//   package's claim set also names - see COVERAGE ALREADY MEASURED BY
//   hrm/ch7-dmacon-register-bits below. This package's own assertions are only the
//   propositions that package does not measure.
// Non-goals: anything outside the `agnus` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
// Claim ids: ids such as `APPA-dmacon.c1` and `APPA-dmacon.c3` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch4.json lists under `inventories`.
//
// BOUNDARY: `agnus` (top-level chipset module) - the only fixture this
//   package's claims classify as constructible. Per every claim's own
//   classify.evidence: "the agnus fixture is not constructible from this
//   rtl/ slice -- verilator --lint-only --top-module agnus rtl/*.v reports 7
//   MODMISSING (agnus_refresh, agnus_diskdma, agnus_audiodma,
//   agnus_bitplanedma, agnus_copper, agnus_blitter, agnus_beamcounter); the
//   existing package agnus_top_dmacon builds it in the full tree." This
//   package's Makefile is agnus_top_dmacon's Makefile with only TESTBENCH
//   changed, for exactly that reason.
//
// PORTS (from the claims' own evidence fields, handed to the building agent
//   as part of the task inventory - NOT discovered by opening agnus.v):
//   in  - aen, rd, hwr, lwr, address_in, data_in
//   out - data_out (DMACONR read-back path: agnus.v:281 drives dmaconr,
//         agnus.v:277 puts it on data_out)
//   DMACON write path: aen/hwr/lwr/address_in/data_in reaches the `dmacon`
//   register at agnus.v:289-292. dmacon is 13 bits wide (bits 12:0 only) -
//   DMACONR's bits 14/13 (BBUSY/BZERO) are not stored bits of it at all.
//   The enable bits' downstream *function* (dbr/refresh/dbwe/address_out/
//   reg_address_out via the arbiter, agnus.v:171-262) is this package's
//   NON-goal per the coverage note above - every claim whose proposition is
//   a function (not a register-mechanics fact) is already measured in
//   hrm/ch7-dmacon-register-bits.
//
// ============================================================================
// CLAIMS (verbatim quote + restatement, from
// work/ch4_packages/ch4-dmacon-bits.jsonl, package "ch4-dmacon-bits", all 45
// records, classify.status == "testable", classify.observable == "yes, at
// agnus"). Two readers (opus, sol) recorded most of these sentences
// independently - grouped below by proposition; both ids are cited together
// wherever they name the same sentence.
//
// -- DMACON overview: address $096, write-only, chip partition (A D P),
//    "DMA control write (clear or set)" --
//   APPA-dmacon.c1 (opus): quote "DMACON     096      W     A D P   DMA
//     control write (clear or set)" -> "DMACON is at address $096, is
//     write-only, is implemented in Agnus, Denise and Paula, and is the DMA
//     control write register with clear-or-set semantics."
//   CH4-appA-dmacon.c1 (sol): same quote -> "DMACON is at custom-register
//     offset $096."
//   CH4-appA-dmacon.c2 (sol): same quote -> "DMACON is write-only."
//   CH4-appA-dmacon.c3 (sol): same quote -> "DMACON is implemented across
//     the Agnus, Denise, and Paula custom-chip functions." (classify.split:
//     "Agnus third -> ch4-dmacon-bits; Denise and Paula thirds are not
//     constructible in this rtl/ slice and are a chip-partition statement,
//     not a behaviour" - grep of denise.v finds no $096 decode at all, and
//     paula.v is not in this rtl/ slice, so only the Agnus third is
//     assertable here.)
//   CH4-appA-dmacon.c4 (sol): same quote -> "DMACON writes either set or
//     clear selected DMA-control bits."
//   APPA-dmacon.c3 (opus): quote "This register controls all of the DMA
//     channels and" -> "DMACON controls all of the DMA channels and
//     contains blitter DMA status bits." (classify.evidence explicitly
//     grounds the write-only-ness claim on this same sentence: "nothing
//     sources $096 onto data_out (:277), so both the address and the
//     write-only-ness are assertable at the agnus boundary.")
//
// -- DMACONR overview: address $002, read-only, chip partition (A P),
//    "DMA control (and blitter status) read" --
//   APPA-dmacon.c2 (opus): quote "DMACONR    002      R     A   P   DMA
//     control (and blitter status) read" -> "DMACONR is at address $002, is
//     read-only, is implemented in Agnus and Paula, and reads DMA control
//     and blitter status." (classify.evidence: "Existing assertion
//     DMACONR-READ-DECODED-AT-002 in package agnus_top_dmacon" - the
//     address-decode-for-read half; see coverage note below.)
//   CH4-appA-dmacon.c5 (sol): same quote -> "DMACONR is at custom-register
//     offset $002."
//   CH4-appA-dmacon.c6 (sol): same quote -> "DMACONR is read-only."
//   CH4-appA-dmacon.c7 (sol): same quote -> "DMACONR is implemented by
//     Agnus and Paula." (classify.split: "Agnus half -> ch4-dmacon-bits;
//     Paula half outside this slice.")
//   CH4-appA-dmacon.c8 (sol): same quote -> "Reading DMACONR returns
//     DMA-control state and blitter status."
//   CH4-appA-dmacon.c9 (sol): quote "This register controls all of the DMA
//     channels and\n                 contains blitter DMA status bits." ->
//     "The DMACON/DMACONR register node controls all DMA-channel enables."
//   CH4-appA-dmacon.c10 (sol): same quote -> "The DMACON/DMACONR register
//     node also reports blitter DMA status."
//
// -- SET/CLR write semantics (bit 15; "bits written 0 stay unchanged") --
//   APPA-dmacon.c4 (opus): quote "15    SET/CLR     Set/clear control bit.
//     Determines" -> "DMACON bit 15 is SET/CLR, the set/clear control bit,
//     which determines whether bits written with a 1 get set or cleared."
//   APPA-dmacon.c5 (opus): quote "cleared.  Bits written with a zero" ->
//     "In a DMACON write, bits written with a zero are unchanged."
//   CH4-appA-dmacon.c11 (sol): quote "15    SET/CLR     Set/clear control
//     bit. Determines\n if bits written with a 1 get set or\n cleared." ->
//     "DMACON bit 15 selects whether written one bits set or clear their
//     corresponding controls."
//   CH4-appA-dmacon.c12 (sol): quote "Bits written with a zero\n are
//     unchanged." -> "A zero in a DMACON write leaves the corresponding
//     control bit unchanged."
//   Both readers' classify.evidence for all four: "agnus.v:291-292 -- if
//     (data_in[15]) dmacon |= data_in[12:0]; else dmacon &= ~data_in[12:0],
//     which is exactly set/clear with zero bits unchanged, and it is
//     visible through DMACONR at $002 (:281)." No "Existing assertion" tag
//     in these four - but this agent's own reading of
//     hrm/ch7-dmacon-register-bits's WALKING-BIT-* loop (SET from zero, then CLR
//     from an all-13-bits-set baseline, for every bit 0-12) and its
//     SETCLR-MULTIBIT-DIRECTION assertion (an explicit SET-then-partial-CLR
//     of a 3-bit pattern) shows this exact mechanism is already exercised
//     there, generically, across the whole register. See coverage note.
//
// -- Per-bit position/name, bits 0-10 (the six agnus consumes: 5,6,7,8,9,10;
//    the five it only stores/reads back: 0,1,2,3,4) --
//   APPA-dmacon.c16/CH4-appA-dmacon.c23: "05 SPREN Sprite DMA enable" (bit 5)
//   APPA-dmacon.c15/CH4-appA-dmacon.c22: "06 BLTEN Blitter DMA enable" (bit 6)
//   APPA-dmacon.c14/CH4-appA-dmacon.c21: "07 COPEN Copper DMA enable" (bit 7)
//   APPA-dmacon.c13/CH4-appA-dmacon.c20: "08 BPLEN Bitplane DMA enable" (bit 8)
//   APPA-dmacon.c12/CH4-appA-dmacon.c19: "09 DMAEN Enable all DMA below" (bit 9)
//   APPA-dmacon.c10/CH4-appA-dmacon.c17: "10 BLTPRI Blitter DMA priority" (bit 10)
//   APPA-dmacon.c11/CH4-appA-dmacon.c18: BLTPRI disables /BLS (function)
//   APPA-dmacon.c17/CH4-appA-dmacon.c24: "04 DSKEN Disk DMA enable" (bit 4)
//   APPA-dmacon.c18/CH4-appA-dmacon.c25: "03 AUD3EN Audio channel 3 DMA
//     enable" (bit 3)
//   APPA-dmacon.c19/CH4-appA-dmacon.c26: "02 AUD2EN Audio channel 2 DMA
//     enable" (bit 2)
//   APPA-dmacon.c20/CH4-appA-dmacon.c27: "01 AUD1EN Audio channel 1 DMA
//     enable" (bit 1)
//   APPA-dmacon.c21/CH4-appA-dmacon.c28: "00 AUD0EN Audio channel 0 DMA
//     enable" (bit 0)
//   Bits 4-0's own classify.evidence: "The ENABLE half is not in this tree's
//     Agnus at all ... disk and audio requests arrive already gated from
//     Paula on the disk_dmal / audio_dmal inputs ... Do not write an enable
//     assertion for this bit here." Only the storage-and-read-back half of
//     these five is in scope for `agnus` at all - see coverage note, this
//     half is already measured.
//
// -- BBUSY (bit 14) and BZERO (bit 13), read-only --
//   APPA-dmacon.c6/CH4-appA-dmacon.c13: "14 BBUSY Blitter busy status bit
//     (read only)"
//   APPA-dmacon.c7/CH4-appA-dmacon.c14: "13 BZERO Blitter logic zero status
//     bit (read only)"
//   classify.evidence (both): "DMACONR bit 14 is blit_busy and bit 13 is
//     blit_zero (agnus.v:281), while the writable register dmacon is only
//     13 bits wide (agnus.v:271) and takes only data_in[12:0]
//     (agnus.v:291-292), so neither bit has a write path ... Existing
//     assertions BBUSY-READONLY-WITH-POSITIVE-CONTROL and
//     BZERO-READONLY-WITH-POSITIVE-CONTROL in package agnus_top_dmacon."
//
// That is all 45 records in ch4-dmacon-bits.jsonl. Note what is absent: page
// 278's own bit table skips 12 and 11 (its rows run 15,14,13,10,9,...,0) -
// there is no quote for either position, so no claim for either exists in
// this package's inventory. Per this task's own instruction, that silence is
// classified `insufficient`, not asserted. dmacon IS 13 bits wide
// (agnus.v:271, per the evidence above), so bits 11/12 exist and take a
// write/read-back like any other position - hrm/ch7-dmacon-register-bits's
// WALKING-BIT-11 and WALKING-BIT-12 already measure exactly that (SET from
// zero, CLR from all-ones, same as every other bit), labelled "bit11(X)" /
// "bit12(X)" - no function claimed, matching page 278's silence. This
// package adds nothing about bits 11/12: there is no proposition to assert,
// and the generic SET/CLR mechanics they do have are already measured
// elsewhere. Measured, not asserted - reported in this package's own build
// report, not invented here.
//
// ============================================================================
// COVERAGE ALREADY MEASURED BY hrm/ch7-dmacon-register-bits (read in full before
// writing anything below - its Makefile is copied above with only TESTBENCH
// changed, and its Harness/write_reg/read_reg pattern, forced on this
// fixture by the SAME arbitrated-register-bus fact its own IMPLEMENTATION
// NOTES document, is reused below). Per this task's own instruction: say so,
// do not duplicate. Grouped by the existing assertion that already measures
// each claim's proposition:
//
//   SPREN-GATES-SPRITE-DMA (bit 5 function) + WALKING-BIT-5-SPREN (position):
//     APPA-dmacon.c16, CH4-appA-dmacon.c23.
//   BLTEN-GATES-BLITTER-DMA + WALKING-BIT-6-BLTEN:
//     APPA-dmacon.c15, CH4-appA-dmacon.c22.
//   COPEN-GATES-COPPER-DMA + WALKING-BIT-7-COPEN:
//     APPA-dmacon.c14, CH4-appA-dmacon.c21.
//   BPLEN-GATES-BITPLANE-DMA + WALKING-BIT-8-BPLEN:
//     APPA-dmacon.c13, CH4-appA-dmacon.c20.
//   DMAEN-MASTER-GATES-ALL-DMA-BELOW + WALKING-BIT-9-DMAEN:
//     APPA-dmacon.c12, CH4-appA-dmacon.c19.
//   BLTPRI-BUS-SHARE-UNDER-CONTENTION (function) + WALKING-BIT-10-BLTPRI
//   (position):
//     APPA-dmacon.c10, APPA-dmacon.c11, CH4-appA-dmacon.c17,
//     CH4-appA-dmacon.c18.
//   WALKING-BIT-4-DSKEN (storage/read-back only, per this package's own
//   claims' instruction not to assert an enable function for it here):
//     APPA-dmacon.c17, CH4-appA-dmacon.c24.
//   WALKING-BIT-3-AUD3EN: APPA-dmacon.c18, CH4-appA-dmacon.c25.
//   WALKING-BIT-2-AUD2EN: APPA-dmacon.c19, CH4-appA-dmacon.c26.
//   WALKING-BIT-1-AUD1EN: APPA-dmacon.c20, CH4-appA-dmacon.c27.
//   WALKING-BIT-0-AUD0EN: APPA-dmacon.c21, CH4-appA-dmacon.c28.
//   BBUSY-READONLY-WITH-POSITIVE-CONTROL:
//     APPA-dmacon.c6, CH4-appA-dmacon.c13.
//   BZERO-READONLY-WITH-POSITIVE-CONTROL:
//     APPA-dmacon.c7, CH4-appA-dmacon.c14.
//   WALKING-BIT-* (all 13 positions, SET-from-zero/CLR-from-all-ones) +
//   SETCLR-MULTIBIT-DIRECTION (explicit SET-then-partial-CLR) together
//   already exercise the SET/CLR mechanism (bit 15's role; "bits written 0
//   stay unchanged") generically, across the whole register:
//     APPA-dmacon.c4, APPA-dmacon.c5, CH4-appA-dmacon.c11,
//     CH4-appA-dmacon.c12.
//
// 24 of this package's 45 claim ids are covered this way. The remaining 21
// are this package's own job - see PLANNED ASSERTIONS. None of the 24 above
// get a duplicate assertion in this file; this file's own report will list
// them as "not asserted here - already measured by hrm/ch7-dmacon-register-bits:
// <assertion name>", per this task's instruction to say so rather than
// re-test them.
//
// ============================================================================
// PLANNED ASSERTIONS (names, cited-id groups and intent only - no RTL line
// numbers beyond what the claims' own classify.evidence already handed over;
// no fixture code below this comment block, matching hrm/ch7-dmacon-register-bits's
// own spec-before-RTL gate. Added in the next commit, after agnus.v is
// opened to confirm the write_reg/read_reg pattern copied from
// hrm/ch7-dmacon-register-bits still applies unchanged - that package's own
// IMPLEMENTATION NOTES already document why the retry-on-pre-edge-dbr shape
// is required, not merely convenient, for this arbitrated register bus):
//
//   1. DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER: this package's control
//      case, expected to PASS, and its fixture-delivers-a-known-word proof.
//      Three DIFFERENT DMACON patterns are written in sequence (not just
//      the final state) and DMACONR is read back and checked after EACH
//      one, confirming each arrives distinct and in the order it was
//      written - not a stale or latched value from a previous step. Cites
//      the DMACON overview group (APPA-dmacon.c1, APPA-dmacon.c3,
//      CH4-appA-dmacon.c1, CH4-appA-dmacon.c2, CH4-appA-dmacon.c3,
//      CH4-appA-dmacon.c4), the DMACONR overview group (APPA-dmacon.c2,
//      CH4-appA-dmacon.c5, CH4-appA-dmacon.c6, CH4-appA-dmacon.c7,
//      CH4-appA-dmacon.c8, CH4-appA-dmacon.c9, CH4-appA-dmacon.c10) and the
//      SET/CLR group (APPA-dmacon.c4, APPA-dmacon.c5,
//      CH4-appA-dmacon.c11, CH4-appA-dmacon.c12) - the sequence's own
//      SET-then-partial-CLR step is a direct, freshly-measured
//      demonstration of "bits written 0 stay unchanged" under this
//      package's own ids, alongside (not instead of) the coverage already
//      noted above under hrm/ch7-dmacon-register-bits's WALKING-BIT-*/
//      SETCLR-MULTIBIT-DIRECTION.
//
//   2. DMACON-IS-WRITE-ONLY-AT-096: after a write through $096, a READ
//      attempted at $096 itself (aen/rd/address_in=$096) must NOT reflect
//      the pattern just written - "nothing sources $096 onto data_out"
//      (every claim's own classify.evidence) - while the SAME state read
//      through $002 (DMACONR) in the SAME run DOES reflect it, the paired
//      control on the same measurement basis. Cites APPA-dmacon.c1,
//      APPA-dmacon.c3, CH4-appA-dmacon.c1, CH4-appA-dmacon.c2,
//      CH4-appA-dmacon.c3.
//
//   3. DMACONR-IS-READ-ONLY-AT-002: a write ATTEMPTED at $002 (DMACONR's
//      own address, not $096) must NOT change what a later read at $002
//      returns, while a real write through $096 immediately afterward, in
//      the SAME run, DOES change it - the paired control proving the write
//      path itself is live and the $002 attempt's silence is not a dead
//      harness. Cites APPA-dmacon.c2, CH4-appA-dmacon.c5,
//      CH4-appA-dmacon.c6, CH4-appA-dmacon.c7.
//
// Three planned assertions. Together with the 24 ids covered by
// hrm/ch7-dmacon-register-bits (noted above) and these three assertions' own
// citations, every one of the 45 ids in ch4-dmacon-bits.jsonl is either
// covered elsewhere (named above) or cited on one of these three - none are
// being left silently uncovered.
//
// No RTL has been opened yet to write this comment block: the port names
// (aen, rd, hwr, lwr, address_in, data_in, data_out) and the register-
// mechanics facts (dmacon is 13 bits wide, data_in[15] selects set/clear,
// DMACON=$096 write-only, DMACONR=$002 read-only, bits 14/13 are a live mux
// of blit_busy/blit_zero and not stored bits of dmacon at all) come from
// this package's own claims' classify.evidence fields and from
// hrm/ch7-dmacon-register-bits's already-committed file (both handed to or
// available to the building agent as inventory, not discovered by this
// agent opening agnus.v). Exact confirmation of the write_reg/read_reg retry
// shape against this fixture, and the mutation log, are answered in the next
// commit, after agnus.v is read.
// ============================================================================

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus.v was opened for
// the first time - the specification comment above this point was committed
// on its own first, per the chapter's spec-before-RTL gate; see this
// package's own git history, commit f16df80).
//
// agnus.v confirms every fact the claims' own classify.evidence already
// handed over, at the exact lines cited there: `localparam DMACON = 9'h096`
// (:103), `localparam DMACONR = 9'h002` (:104), `reg [12:0] dmacon` (:271,
// 13 bits), `assign data_out = data_bmc | dmaconr | data_blt` (:277),
// DMACONR's read mux - `if (reg_address[8:1]==DMACONR[8:1]) dmaconr[15:0] =
// {1'b0, blit_busy, blit_zero, dmacon[12:0]}; else dmaconr = 0` (:281-282),
// and DMACON's write - `else if (reg_address[8:1]==DMACON[8:1]) ... if
// (data_in[15]) dmacon[12:0] <= dmacon[12:0] | data_in[12:0]; else
// dmacon[12:0] <= dmacon[12:0] & ~data_in[12:0]` (:289,291-292). Nothing
// here was a surprise against the inventory; opening the file confirmed the
// spec above rather than revising it.
//
// Two things follow directly from that read-mux shape that the three
// assertions below depend on:
//   - DMACON-IS-WRITE-ONLY-AT-096: `dmaconr` is the ONLY driver of
//     `data_out` that ever depends on `reg_address` matching anything
//     related to `dmacon`'s storage, and its own `if` (:281) only fires for
//     DMACONR[8:1] ($002[8:1]), not DMACON[8:1] ($096[8:1]) - the two
//     addresses differ in bits [8:1] (0x096>>1=0x4B vs 0x002>>1=0x01), so a
//     read at $096 takes dmaconr's `else` arm (:282, dmaconr=0) and neither
//     data_bmc nor data_blt (the other two OR terms, sourced from
//     agnus_bmc/agnus_blitter's own register windows, not DMACON's address)
//     has any reason to decode $096 either - data_out is 0 at that address,
//     not merely "not equal to the pattern written".
//   - DMACONR-IS-READ-ONLY-AT-002: the write's `else if` (:289) tests
//     DMACON[8:1], not DMACONR[8:1] - a write with address_in=$002 (i.e.
//     $002>>1=0x001) fails that comparison against DMACON[8:1]=0x4B and
//     never reaches the `dmacon[12:0] <= ...` assignment at all, regardless
//     of data_in or the SET/CLR bit.
//
// The write_reg/read_reg retry-on-pre-edge-dbr shape (copied unchanged
// below from hrm/ch7-dmacon-register-bits's Harness) is still the correct way to
// drive this fixture: the write at :289 is gated by the SAME `reg_address`
// wire the arbiter overwrites on every cycle some DMA engine holds the bus
// (agnus.v's priority mux, documented in hrm/ch7-dmacon-register-bits's own
// IMPLEMENTATION NOTES), and this package never sets any DMA engine's own
// enable bit as its own subject, but agnus_refresh's unconditional periodic
// pulse still contends for the bus regardless of any DMACON bit - the retry
// loop is what makes a register access land on the first available
// non-refresh cycle instead of assuming one is always immediately free.
//
// MUTATION LOG: see the bottom of this file, after main(). Every entry was
// actually applied - edited into this file, rebuilt under WSL/Verilator
// (`make test`), run, the PASS/FAIL line read from real output, then
// reverted and rebuilt clean before the next entry.
// ============================================================================

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vagnus.h"

namespace {

// ---- register addresses (9-bit custom-chip offsets, agnus.v:103-104) ----
constexpr uint16_t DMACON_ADDR  = 0x096;
constexpr uint16_t DMACONR_ADDR = 0x002;

// ---- DMACON/DMACONR bit masks used by this package's own assertions ----
constexpr uint16_t SET_CLR = 0x8000;  // bit 15
constexpr uint16_t SPREN   = 1u << 5;
constexpr uint16_t BPLEN   = 1u << 8;
constexpr uint16_t DMAEN   = 1u << 9;

// Harness: copied from hrm/ch7-dmacon-register-bits's own Harness class unchanged
// (same fixture, same arbitrated register bus, same reason for the
// pre-edge-dbr retry - see IMPLEMENTATION NOTES above). Only the members
// this package's own assertions actually call are kept: write_reg, read_reg
// and the tick machinery they depend on. The DMA-consumption helpers
// (count_nonrefresh_dbr_over_lines, count_dbwe_over_ticks,
// sync_to_line_boundary, start_blit/copper/sprite/bitplane, set_bls) are
// hrm/ch7-dmacon-register-bits's own job, not copied here - this package does not
// duplicate what that package already measures.
class Harness {
public:
    Harness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One CPU register write, held for up to `max_ticks` ticks and applied
    // on the first tick where `dbr==0` (see IMPLEMENTATION NOTES). Returns
    // whether a dbr==0 tick was actually seen via `write_ok`, checked at
    // every call site so a silently-missed write cannot masquerade as a
    // passing assertion.
    void write_reg(uint16_t reg9, uint16_t data, int max_ticks = 8) {
        dut->aen = 1;
        dut->hwr = 1;
        dut->lwr = 1;
        dut->rd = 0;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        dut->data_in = data;
        write_ok = false;
        for (int i = 0; i < max_ticks; ++i) {
            bool pre_dbr = tick_pre_edge_dbr();
            if (!pre_dbr) { write_ok = true; break; }
        }
        dut->aen = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->address_in = 0;
        dut->data_in = 0;
    }

    // One CPU register read, same dbr==0 retry as write_reg. `read_ok`
    // reports whether a clean sample was taken.
    uint16_t read_reg(uint16_t reg9, int max_ticks = 8) {
        dut->aen = 1;
        dut->rd = 1;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        uint16_t val = 0xFFFF;
        read_ok = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (!last_dbr) { val = last_data_out; read_ok = true; break; }
        }
        dut->aen = 0;
        dut->rd = 0;
        dut->address_in = 0;
        return val;
    }

    void tick() { tick_pre_edge_dbr(); }

    // Advances one clk7_en tick (cck toggle, then a clk 0->1->0 pulse) and
    // returns the PRE-edge value of `dbr` - the value the RTL's own
    // `always @(posedge clk)` write logic actually saw for this edge.
    bool tick_pre_edge_dbr() {
        dut->cck = cck_phase ? 1 : 0;
        cck_phase = !cck_phase;
        dut->eval();
        bool pre_dbr = dut->dbr;
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        last_dbr = dut->dbr;
        last_data_out = dut->data_out;
        return pre_dbr;
    }

    bool last_dbr = false;
    uint16_t last_data_out = 0;
    bool write_ok = false;
    bool read_ok = false;

    Vagnus* dut;

private:
    bool cck_phase = false;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER (p.278) ----------------
    // The fixture-integrity proof this chapter's own build brief requires:
    // three DIFFERENT DMACON patterns are written in sequence and DMACONR is
    // sampled after EACH one (not just the final state), confirming each
    // arrives distinct and in the order it was written - the same harness
    // cannot be silently returning a stale or zero value and still pass all
    // three. This is also this package's control case, expected to PASS on
    // the same read_reg/write_reg measurement basis as the two suspicion
    // cases below.
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t after_a = h.read_reg(DMACONR_ADDR);
        bool ok_a = h.read_ok && h.write_ok;

        h.write_reg(DMACON_ADDR, SET_CLR | BPLEN);
        uint16_t after_b = h.read_reg(DMACONR_ADDR);
        bool ok_b = h.read_ok && h.write_ok;

        h.write_reg(DMACON_ADDR, 0x0000 | SPREN);  // CLR only SPREN
        uint16_t after_c = h.read_reg(DMACONR_ADDR);
        bool ok_c = h.read_ok && h.write_ok;

        checks.expect(
            ok_a && ok_b && ok_c &&
                after_a == (SPREN | DMAEN) &&
                after_b == (SPREN | DMAEN | BPLEN) &&
                after_c == (DMAEN | BPLEN),
            "DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER: three distinct "
            "DMACON writes, sampled through DMACONR after each one, arrive "
            "in the order written - SET SPREN|DMAEN (0x0220), then SET BPLEN "
            "additionally (0x0320), then CLR only SPREN, leaving DMAEN and "
            "BPLEN untouched (0x0300) - proving the harness delivers a known, "
            "freshly-fetched word at each step rather than a stale or "
            "zero-flag one, and demonstrating the SET-then-partial-CLR "
            "direction ('bits written with a zero are unchanged') under this "
            "package's own measurement. 'DMACON 096 W ... DMA control write "
            "(clear or set)' / 'DMACONR 002 R ... DMA control (and blitter "
            "status) read' / '15 SET/CLR ... Bits written with a zero ... "
            "unchanged' (p.278). APPA-dmacon.c1, APPA-dmacon.c3, "
            "APPA-dmacon.c4, APPA-dmacon.c5, CH4-appA-dmacon.c1, "
            "CH4-appA-dmacon.c2, CH4-appA-dmacon.c3, CH4-appA-dmacon.c4, "
            "CH4-appA-dmacon.c11, CH4-appA-dmacon.c12, APPA-dmacon.c2, "
            "CH4-appA-dmacon.c5, CH4-appA-dmacon.c6, CH4-appA-dmacon.c7, "
            "CH4-appA-dmacon.c8, CH4-appA-dmacon.c9, CH4-appA-dmacon.c10.");
    }

    // ---------------- DMACON-IS-WRITE-ONLY-AT-096 (p.278) ----------------
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t at_002 = h.read_reg(DMACONR_ADDR);   // positive control: same run, other address
        bool ok_002 = h.read_ok;
        uint16_t at_096 = h.read_reg(DMACON_ADDR);    // the address under test
        bool ok_096 = h.read_ok;

        checks.expect(
            ok_002 && ok_096 && at_002 == (SPREN | DMAEN) && at_096 == 0x0000,
            "DMACON-IS-WRITE-ONLY-AT-096: after a write of SPREN|DMAEN "
            "through $096, a read through the SAME aen/rd/address_in path "
            "AIMED AT $096 itself returns 0x0000, not the pattern just "
            "written, while the SAME state read at $002 (DMACONR) in the "
            "SAME run - the paired control on the same measurement basis - "
            "DOES return it (0x0220) - 'DMACON 096 W ... DMA control write "
            "(clear or set)' (p.278), the R/W column showing DMACON as write "
            "only. APPA-dmacon.c1, APPA-dmacon.c3, CH4-appA-dmacon.c1, "
            "CH4-appA-dmacon.c2, CH4-appA-dmacon.c3.");
    }

    // ---------------- DMACONR-IS-READ-ONLY-AT-002 (p.278) ----------------
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BPLEN);  // establish a known baseline
        uint16_t baseline = h.read_reg(DMACONR_ADDR);
        bool ok_baseline = h.read_ok && h.write_ok;

        h.write_reg(DMACONR_ADDR, SET_CLR | SPREN);  // attempt a write AT $002 itself
        uint16_t after_attempt = h.read_reg(DMACONR_ADDR);
        bool ok_attempt = h.read_ok && h.write_ok;   // write_ok only reports a dbr==0 cycle was seen, not that the write landed in dmacon

        h.write_reg(DMACON_ADDR, SET_CLR | SPREN);   // the paired control: a REAL write, through $096, in the SAME run
        uint16_t after_real_write = h.read_reg(DMACONR_ADDR);
        bool ok_real = h.read_ok && h.write_ok;

        checks.expect(
            ok_baseline && ok_attempt && ok_real &&
                baseline == (DMAEN | BPLEN) &&
                after_attempt == (DMAEN | BPLEN) &&
                after_real_write == (DMAEN | BPLEN | SPREN),
            "DMACONR-IS-READ-ONLY-AT-002: a write attempted AT $002 itself "
            "(DMACONR's own address) leaves DMACONR's later read unchanged "
            "(0x0300 before and after the attempt) even though the attempt's "
            "own data pattern (SET_CLR|SPREN) would have changed it had it "
            "landed, while a REAL write of the SAME pattern through $096, in "
            "the SAME run - the paired control on the same measurement basis "
            "- DOES change it (0x0320) - proving the write path itself is "
            "live and the $002 attempt's silence is not a dead harness. "
            "'DMACONR 002 R ... DMA control (and blitter status) read' "
            "(p.278), the R/W column showing DMACONR as read only. "
            "APPA-dmacon.c2, CH4-appA-dmacon.c5, CH4-appA-dmacon.c6, "
            "CH4-appA-dmacon.c7.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// ============================================================================
// MUTATION LOG (11-12.09.2026). Every entry below was actually applied by
// this agent - edited into this file, rebuilt under WSL/Verilator
// (`make test`), run, the PASS/FAIL line read from real output (quoted
// verbatim below, not predicted), then reverted and rebuilt clean before the
// next entry. Baseline before this log's first mutation and final clean run
// after the last revert were both "3 passed, 0 failed".
//
// 1. DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER: the first write's target
//    address changed from DMACON_ADDR (0x096) to 0x098, a nearby but
//    non-decoded address (agnus.v's write decode is
//    reg_address[8:1]==DMACON[8:1], and 0x098[8:1] != 0x096[8:1]). Result:
//    "2 passed, 1 failed" - DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER went
//    red (after_a read back 0x0000, not SPREN|DMAEN, since the write landed
//    nowhere DMACONR reads from). Reverted to DMACON_ADDR; rebuilt clean (3
//    passed, 0 failed).
//
// 2. DMACON-IS-WRITE-ONLY-AT-096: the second read's target address changed
//    from DMACON_ADDR (the address under test) to DMACONR_ADDR, so both
//    reads in the assertion aimed at $002. Result: "2 passed, 1 failed" -
//    DMACON-IS-WRITE-ONLY-AT-096 went red (at_096 read back 0x0220, equal
//    to at_002, instead of the expected 0x0000 - the assertion's own
//    "returns 0x0000, not the pattern just written" check failed). Reverted
//    to DMACON_ADDR; rebuilt clean (3 passed, 0 failed).
//
// 3. DMACONR-IS-READ-ONLY-AT-002: the write-attempt's target address
//    changed from DMACONR_ADDR (0x002, the address under test) to
//    DMACON_ADDR (0x096), turning the "attempt" into a real write. Result:
//    "2 passed, 1 failed" - DMACONR-IS-READ-ONLY-AT-002 went red
//    (after_attempt read back 0x0320 = DMAEN|BPLEN|SPREN, not the expected
//    0x0300 = DMAEN|BPLEN unchanged - the write landed because it now used
//    DMACON's own address). Reverted to DMACONR_ADDR; rebuilt clean (3
//    passed, 0 failed).
//
// Machine-readable records, one per line:
// MUTATION DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER stimulus red: first write's target address moved from $096 to $098 (not decoded by agnus.v's DMACON path); after_a read back 0x0000, not SPREN|DMAEN.
// MUTATION DMACON-IS-WRITE-ONLY-AT-096 stimulus red: the read aimed at $096 was retargeted to $002, so both reads returned the same value (0x0220) instead of 0x0000 vs 0x0220.
// MUTATION DMACONR-IS-READ-ONLY-AT-002 stimulus red: the write attempt's target address moved from $002 to $096, turning the attempt into a real write; after_attempt came back 0x0320 instead of the unchanged 0x0300.
// ============================================================================
