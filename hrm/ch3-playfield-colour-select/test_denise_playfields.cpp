// ch3-playfield-colour-select -- denise_playfields conformance test
//
// Test purpose: colour selection on the denise_playfields fixture as
// chapter 3 tabulates it: how the bit combination formed from the enabled
// bitplanes selects a colour register. The package's centre of gravity is
// single-playfield selection (Tables 3-17 and 3-18: bit combination N
// selects colour register N, one-for-one, and which bitplane supplies the
// most-significant bit) -- but claims.jsonl also assigns this package a
// large block of dual-playfield colour-code propositions (Tables 3-10,
// 3-11, 3-12, Figure 3-13, the swap-bit/PF2PRI priority mechanism, and
// both Table 3-17's and Table 3-18's dual-playfield columns), extracted by
// different readers from different sentences than the sibling package
// hrm/ch3-dual-playfield's own 98-claim inventory. Those two inventories
// share zero claim ids (measured, not assumed -- see COVERAGE CORRECTION
// below): the sibling's assertions test its own ids, not these ones, so
// every one of this package's own claim ids is tested here, on this same
// fixture, under dblpf=1 where the proposition calls for it.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 40, 42, 43, 44, 56, 69, 70, 71, 93, 94 (claims.jsonl for
//   ch3-playfield-colour-select, 272 records, 258 asserted below). A red
//   result below means denise_playfields.v disagrees with the manual, not
//   that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass.
// Non-goals: `aga` and `pf2of` are held at their reset default (0)
//   throughout -- no claim in this package's inventory names AGA
//   extensions or the OCS priority-offset behaviour, and the RTL's own
//   AGA-only code paths (denise_playfields.v:73-74/87-88, and bpldata bits
//   7/8) are never exercised by any stimulus below (bits 7/8 of bpldata
//   are never set). `nplayfield` is never read: every proposition tested
//   is independently observable through `plfdata` alone (the same
//   convention hrm/ch3-dual-playfield uses for the identical module).
//   `dblpf` and `bplcon2`, unlike the sibling's non-goal list, ARE varied
//   here -- they are this package's own subject matter for every
//   dual-playfield-flavoured assertion below, not an irrelevant default.
//
// COVERAGE CORRECTION (recorded for traceability): an earlier version of
// this file declined 134 dual-playfield-flavoured claim ids as "already
// covered by hrm/ch3-dual-playfield," reasoning from topic overlap rather
// than claim-id coverage. Measured: the two packages' claims.jsonl files
// share zero ids (272 here, 98 there, 0 in common), and none of this
// package's declined ids appeared anywhere in the sibling's assertion
// strings. A claim is covered only by an assertion that names its own id
// -- coverage does not transfer across packages just because both extract
// propositions from the same module and the same general subject. 131 of
// those 134 are asserted below (PF1-CODE-SWEEP, PF2-CODE-SWEEP,
// FIGURE-3-13-ROWS, DUAL-PRIORITY, BIT-ORDER-DUAL, DUAL-MODE-CAPACITY); 3
// (3-dual.c32, 3-dual-control.c1, 3-dual-control.c2) turned out, on
// re-examination, to genuinely name a port this module does not have
// (independently-sized playfield memory / scroll position) and are
// declined below for that reason instead, joined with the memory-geometry
// group they share their exact quotation with.
//
// BOUNDARY (from PACKAGE.md's own evidence field, carried into every claim
// record): nanomig/src/minimig-aga/denise_playfields.v:8-17 module header
//   (aga l.10, `bpldata`[8:1] l.11, dblpf l.12, pf2of l.13, `bplcon2`[6:0]
//   l.14, `nplayfield`[2:1] l.15, `plfdata`[7:0] l.16) | l.95-100 (single
//   playfield: plfdata = bpldata[8:1]); MEASURED (d_pf): bit combination N
//   selects colour register N for every N in 0..31. Opening the RTL (after
//   the spec-only commit) additionally showed l.41-63 (nplayfield valid
//   logic, dblpf-gated) and l.65-94 (dual-playfield priority/code-select
//   logic, dblpf-gated: PF1 code = bpldata[7,5,3,1], PF2 code =
//   8+bpldata[8,6,4,2] non-AGA, PF2PRI = bplcon2[6] selects which of the
//   two is checked first, code 0 on the checked-first side falls through
//   to the other side or to background 0) -- this is what PF1-CODE-SWEEP,
//   PF2-CODE-SWEEP, FIGURE-3-13-ROWS, DUAL-PRIORITY, BIT-ORDER-DUAL and
//   DUAL-MODE-CAPACITY below exercise.
//
// ORDER OF WORK: this file (assertions, expected values, claim-id coverage)
// was written and committed from claims.jsonl's quotations and PACKAGE.md's
// own evidence field ONLY -- denise_playfields.v itself was not opened
// until after that commit, per the build brief's explicit gate. The
// Harness's port names (aga, bpldata, dblpf, pf2of, bplcon2, plfdata) come
// from PACKAGE.md's own evidence field, which quotes the module header
// directly, and are identical to the already-built, already-committed
// sibling harness for the SAME module (hrm/ch3-dual-playfield/
// test_denise_playfields.cpp) -- reused exactly as the build brief instructs
// ("Copy the Makefile pattern from hrm/ch3-dual-playfield/"). The bit-
// packing convention for bpldata (C++ bit (n-1) is Verilog bitplane n's
// live data bit, n=1..8), the general-form sample(bpldata,dblpf,bplcon2)
// signature, and the FIGURE-3-13-ROWS presence-inference technique
// (via_pf1 in 1..7 means playfield 1 present; via_pf2 >= 9 means playfield
// 2 present) are likewise carried from that sibling's already-verified
// harness, not learned by opening this package's own forbidden-until-
// committed file. The single-playfield assertions and their build/mutation
// results are unchanged from the spec-only and first-build commits; the
// dual-playfield assertions below are new, added per coordinator review
// (see COVERAGE CORRECTION) and built/mutated the same way, after the RTL
// was already open.
//
// PRINT DEFECT #1 (Table 3-17, printed page 93 -- not this package's to
// resolve, per the build brief): the table's middle column (headed
// "Bitplanes 4,3,2,1", the 4-bit hold-and-modify/HAM code -- itself out of
// this module's boundary, see NO-HAM-PORT below) prints the same value,
// 0100, on both the 00100 row and the 00110 row -- so 0100 appears to
// select HAM combinations 4 and 6 while 0110 selects nothing. This is a
// transcription error in the print, not asserted either way here (the HAM
// column is declined regardless, as out of this module's boundary); it is
// recorded here for traceability only. It does not affect the single-
// playfield column (leftmost, "Bitplanes 5,4,3,2,1") this package tests --
// rows N=4 (00100) and N=6 (00110) read correctly there (100 -> register 4,
// 110 (sic, printed "0100" only in the HAM column) -> register 6) and are
// swept below on the exact same basis as every other N, serving as their
// own control against the adjacent print defect.
//
// PRINT DEFECT #2 (Table 3-18 mis-paged in the corpus): Table 3-18's rows
// carry printed page 94 in claims.jsonl, not 93 -- this is correct as
// extracted (not a defect to fix here) and every Table 3-18 citation below
// cites page 94 accordingly, per the build brief's explicit instruction.
//
// CLAIM COVERAGE: 258 of the 272 records in claims.jsonl are cited below,
// in full, inside an expect() label; 14 are declined (see DECLINED CLAIMS).
// Verified by script: every one of the 272 ids in claims.jsonl is
// classified exactly once, either asserted or declined, with zero overlap
// and zero omissions.
//
//   COLOUR-INDIRECTION-MECHANISM (7 ids)          -- the general principle.
//   BIT-SIGNIFICANCE-ORDER (7 ids, loop x5)        -- single-playfield MSB..LSB = bitplane 5..1.
//   IDENTITY-MAPPING-SWEEP (113 ids, loop x32)     -- Tables 3-17 & 3-18: N -> register N.
//   DUAL-MODE-CAPACITY (6 ids)                     -- PF1 caps at 7, PF2 at 15, never 16-31.
//   BIT-ORDER-DUAL (6 ids, loop x6)                -- dual-mode bitplane-to-bit-position map.
//   DUAL-PRIORITY (4 ids)                          -- swap-bit / PF2PRI decides front/back.
//   FIGURE-3-13-ROWS (14 ids, loop x7)             -- bitplane count vs playfield occupancy.
//   PF1-CODE-SWEEP (51 ids, loop x8)               -- Tables 3-10/3-12a + dual columns: PF1 code -> register.
//   PF2-CODE-SWEEP (50 ids, loop x8)               -- Tables 3-11/3-12b + dual columns: PF2 code -> register.
//   7+7+113+6+6+4+14+51+50 = 258.
//
// DECLINED CLAIMS (14 ids, grouped by reason -- every one names a port
// this module does not have; none is declined as "covered elsewhere"):
//
//   NO-MEMORY-PORT (p.42/44/53/54/71/76) -- playfield memory geometry: for
//     a single playfield (bytes-per-line organization, memory-vs-displayed
//     -window sizing, even-byte allocation, sprite/audio/animation memory
//     budgets) and, in dual-playfield mode, each playfield having its own
//     independently-sized memory representation with its own independently
//     -selected displayed portion. denise_playfields has no memory-address,
//     memory-size, or scroll/display-window-position port (aga, bpldata,
//     dblpf, pf2of, bplcon2, nplayfield in; plfdata out only) -- it
//     receives one already-fetched, already-positioned bit per bitplane
//     per pixel and has no visibility into how or where that bit was
//     stored or scrolled.
//     (7 ids) 3-video.c28, 3-memory.c20, 3-mem.c12, 3-large.c22, 3-dual.c32, 3-dual-control.c1, 3-dual-control.c2
//
//   NO-WINDOW-PORT (p.80) -- the ECS display-window-limit change. A
//     DDF/display-window boundary question (agnus's fetch-window logic),
//     not this per-pixel colour-composition module's; see
//     hrm/ch3-ddf-fetch-window.
//     (1 id) 3-max-window.c31
//
//   NO-BPLCON1-PORT (p.85) -- horizontal scroll delay, set per playfield
//     via BPLCON1 bits. denise_playfields' only control-register input is
//     bplcon2[6:0] (PF2PRI and AGA priority bits); BPLCON1 is not a port
//     here. See hrm/ch3-bplcon1-scroll-delay.
//     (3 ids) 3-hscr.c22, 3-hscroll.c29, 3-hscroll.c31
//
//   NO-SPRITE-PORT (p.86) -- playfields sharing the display with sprites.
//     No sprite input exists on this module; sprite/playfield compositing
//     is hrm/ch3-sprite-playfield-priority's boundary.
//     (1 id) 3-adv.c01
//
//   NO-HAM-PORT (p.86) -- planes 5 and 6 as HOLD/MODIFY control bits. HAM
//     colour composition is a different module's job (denise_hamgenerator);
//     see hrm/ch3-ham-control-field. (This is also the module that owns
//     Table 3-17/3-18's 4-bit Bitplanes-4,3,2,1 HAM column -- see PRINT
//     DEFECT #1 above on why that column is not exercised here either.)
//     (1 id) 3-ham.c08
//
//   NO-DMA-PORT (p.272) -- the parallel-to-serial shift register's own
//     timing (triggered by bitplane-1 writes, 16 pixels per converted
//     word). denise_playfields receives bpldata already serialized to one
//     bit per plane per pixel; the conversion itself happens upstream, in
//     bitplane DMA/shift-register logic this module has no port into.
//     (1 id) A-bpldat.c8
//
//   7+1+3+1+1+1 = 14.
//
// CONTROL CASE: CONTROL-KNOWN-VALUES-ARRIVE (no claim id, no manual page)
// is the control every page-cited assertion below shares: the fixture is
// shown to deliver two distinct, known plfdata values (5 and 22) through
// the same eval() path used everywhere else, before anything below trusts
// it. It carries no claim id and is prefixed "[control]"; an earlier
// convention (shared with hrm/ch3-dual-playfield's and
// hrm/ch3-collision-registers' equivalents) treated that prefix as
// exempting it from a mutation record too -- a gate blind spot, since a
// passing control only proves the fixture is alive once the control has
// itself been shown able to go red. Corrected here: see MUTATION
// CONTROL-KNOWN-VALUES-ARRIVE below, same as every other assertion.
//
// RESULTS (after opening the RTL and building): 70 passed, 0 failed, plus
// the 1 MEASURED note (printed, not counted as pass or fail). No finding
// in this package -- denise_playfields.v implements the manual's
// single-playfield AND dual-playfield colour-selection description
// exactly as every assertion expected. One RTL observation, checked
// rather than assumed, on opening the RTL: denise_playfields.v:96-99
// hides an undocumented "OCS/ECS...Swiv score display" exception in the
// single-playfield branch (forces plfdata to the constant 16 when
// bplcon2[5:3]>5 and bitplane 5 is active and aga=0). No claim in this
// package's inventory describes it (it is undocumented), and it cannot
// fire under any stimulus above: the single-playfield assertions hold
// bplcon2 at 0, and the dual-playfield assertions only ever reach the
// dblpf=1 branch, which this exception's condition sits outside of either
// way -- confirmed by the identity-mapping sweep's own N=16..31 rows,
// which set bitplane 5 and still read back correctly rather than
// collapsing to that constant.
//
// One build-only fix was needed, and it was in the harness, not the
// oracle: none. The Harness written in the spec-only commit compiled and
// linked against Vdenise_playfields.h without modification -- PACKAGE.md's
// port list (aga, bpldata, dblpf, pf2of, bplcon2, plfdata) matched the
// actual module header exactly, and the eval()-only usage needed no clock
// or reset handling.
//
// MUTATION LOG (each line below was actually built with its own flag
// defined to 1 via `-CFLAGS -DMUT_X=1`, confirmed to turn only its own
// assertion red -- every other assertion unaffected -- then rebuilt with
// no flags and reconfirmed the clean 70-passed/0-failed baseline):
//
// MUTATION CONTROL-KNOWN-VALUES-ARRIVE stimulus red: the first control stimulus was changed from 5 to 4, so the first reading no longer matches the value 5 the check expects; reverted, and the clean 70-passed/0-failed baseline was reconfirmed.
// MUTATION COLOUR-INDIRECTION-MECHANISM stimulus red: the stimulus was changed from 21 (10101) to 20 (10100), so the reading no longer matches the value 21 the label and citations name.
// MUTATION BIT-SIGNIFICANCE-ORDER stimulus red: bitplane 5's isolated case was changed to also present bitplane 1 alongside it, so the reading is no longer the clean, isolated MSB value of 16.
// MUTATION IDENTITY-MAPPING-SWEEP stimulus red: row N=1's own stimulus was changed to present N=2's combination instead, so that row no longer reads back as register 1.
// MUTATION DUAL-MODE-CAPACITY stimulus red: bitplane 5 was dropped from the all-six-planes stimulus, so playfield 1 can no longer reach its 3-bit maximum of register 7.
// MUTATION BIT-ORDER-DUAL stimulus red: bitplane 5's isolated case was changed to also present bitplane 1 alongside it, so the reading is no longer the clean, isolated value of 4.
// MUTATION DUAL-PRIORITY stimulus red: the PF2-priority read's BPLCON2 value was changed to the same all-clear value as the PF1-priority read, so the two reads stopped differing.
// MUTATION FIGURE-3-13-ROWS stimulus red: row n=1's single active plane was presented as bitplane 2 instead of bitplane 1, so that row now shows playfield 2 present and playfield 1 empty instead of the reverse.
// MUTATION PF1-CODE-SWEEP stimulus red: row k=1's own stimulus was changed to present k=2's combination instead, so that row no longer reads back as register 1.
// MUTATION PF2-CODE-SWEEP stimulus red: row k=1's own stimulus was changed to present k=2's combination instead, so that row no longer reads back as register 9.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vdenise_playfields.h"
#include "verilated.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_MECH
#define MUT_MECH 0
#endif
#ifndef MUT_ORDER
#define MUT_ORDER 0
#endif
#ifndef MUT_SWEEP
#define MUT_SWEEP 0
#endif
#ifndef MUT_CAPACITY
#define MUT_CAPACITY 0
#endif
#ifndef MUT_BITORDER
#define MUT_BITORDER 0
#endif
#ifndef MUT_DUALPRI
#define MUT_DUALPRI 0
#endif
#ifndef MUT_FIG313
#define MUT_FIG313 0
#endif
#ifndef MUT_PF1SWEEP
#define MUT_PF1SWEEP 0
#endif
#ifndef MUT_PF2SWEEP
#define MUT_PF2SWEEP 0
#endif

namespace {

// bpldata bit (n-1) is bitplane n's live data bit, n=1..8 -- carried from
// hrm/ch3-dual-playfield/test_denise_playfields.cpp's own stimulus usage,
// not learned by opening this package's own file.
constexpr uint8_t PLANE(int n) { return static_cast<uint8_t>(1u << (n - 1)); }

constexpr uint8_t PF2PRI_BIT = 0x40;  // BPLCON2 bit 6 (p.69/71/90/271)

// 3-bit dual-playfield code k (0..7) placed onto playfield 1's own planes
// (1,3,5 -> bit0,1,2 of k respectively, per Table 3-10's heading and
// p.70's "bitplanes 1, 3, and 5").
uint8_t PF1_CODE(int k) {
    uint8_t v = 0;
    if (k & 1) v |= PLANE(1);
    if (k & 2) v |= PLANE(3);
    if (k & 4) v |= PLANE(5);
    return v;
}
// 3-bit dual-playfield code k (0..7) placed onto playfield 2's own planes
// (2,4,6 -> bit0,1,2 of k respectively, per Table 3-11's heading and
// p.70's "bitplanes 2, 4, and 6").
uint8_t PF2_CODE(int k) {
    uint8_t v = 0;
    if (k & 1) v |= PLANE(2);
    if (k & 2) v |= PLANE(4);
    if (k & 4) v |= PLANE(6);
    return v;
}

class Harness {
public:
    Harness() : dut(new Vdenise_playfields) {
        // aga/pf2of: non-goals (see header, Non-goals).
        dut->aga = 0;
        dut->pf2of = 0;
        dut->bpldata = 0;
        dut->dblpf = 0;
        dut->bplcon2 = 0;
        dut->eval();
    }
    ~Harness() { delete dut; }

    // Presents bpldata/dblpf/bplcon2 for one combinational evaluation
    // (denise_playfields has no clock -- see BOUNDARY) and returns
    // plfdata. dblpf and bplcon2 default to single-playfield mode's own
    // condition (0, 0) so single-playfield callers are unaffected by this
    // general-form signature; dual-playfield callers pass true and a
    // BPLCON2 value explicitly.
    uint8_t sample(uint8_t bpldata_v, bool dblpf_v = false, uint8_t bplcon2_v = 0) {
        dut->bpldata = bpldata_v;
        dut->dblpf = dblpf_v ? 1 : 0;
        dut->bplcon2 = bplcon2_v;
        dut->eval();
        return dut->plfdata;
    }

    Vdenise_playfields* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the SOURCE says, or why a claim is declined.
    // They are not measurements and must not be printed as though they were:
    // a constant string under a MEASURED label is a record no run can
    // contradict, which is how a false "measured" fact survived a day in
    // ch3-sprite-playfield-priority.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// One row per Table 3-17/3-18 single-playfield entry (N=0..31). `ids` cites
// every claim id this row settles (both readers' extractions of Table
// 3-17, both readers' extractions of Table 3-18 where N<16, plus any
// generic-principle or Figure 3-8 claim folded into that specific N -- see
// the CLAIM COVERAGE note above). `p94` is true where Table 3-18
// corroborates the same N at high resolution (printed page 94).
struct SweepRow { int n; const char* ids; const char* note; bool p94; };
const SweepRow SWEEP_ROWS[32] = {
    {0, "3-t17.n00, 3-table3-17.c1, 3-t18.n00, 3-table3-18.c1, 3-t17.fn1, 3-table3-17.c97, 3-t18.fn2, 3-table3-18.c34, 3-color.c3, 3-coding.c9.", "This is also Table 3-17's footnote * and Table 3-18's footnote ** ('Color register 0 always defines the background color'), Figure 3-8's unmarked COLOR 00 region (p.56), and the one-bitplane-off case (p.42).", true},
    {1, "3-t17.n01, 3-table3-17.c4, 3-t18.n01, 3-table3-18.c3, 3-color.c4, 3-bpl.c03, 3-code.f01, 3-coding.c7.", "This is also the one-bitplane-on case (p.42, 'if the bit is 1, the pixel is another color') and Figure 3-8's plane2=0/plane1=1 example labelled 'Color 1' (p.56).", true},
    {2, "3-t17.n02, 3-table3-17.c7, 3-t18.n02, 3-table3-18.c5.", "", true},
    {3, "3-t17.n03, 3-table3-17.c10, 3-t18.n03, 3-table3-18.c7, 3-code.f02, 3-coding.c8, 3-code.f04.", "This is also Figure 3-8's plane2=1/plane1=1 example labelled 'Color 3' (p.56), where the displayed pixel is the combination of both bitplanes, not either alone.", true},
    {4, "3-t17.n04, 3-table3-17.c13, 3-t18.n04, 3-table3-18.c9.", "", true},
    {5, "3-t17.n05, 3-table3-17.c16, 3-t18.n05, 3-table3-18.c11.", "", true},
    {6, "3-t17.n06, 3-table3-17.c19, 3-t18.n06, 3-table3-18.c13.", "", true},
    {7, "3-t17.n07, 3-table3-17.c22, 3-t18.n07, 3-table3-18.c15.", "", true},
    {8, "3-t17.n08, 3-table3-17.c25, 3-t18.n08, 3-table3-18.c17.", "", true},
    {9, "3-t17.n09, 3-table3-17.c28, 3-t18.n09, 3-table3-18.c19.", "", true},
    {10, "3-t17.n10, 3-table3-17.c31, 3-t18.n10, 3-table3-18.c21.", "", true},
    {11, "3-t17.n11, 3-table3-17.c34, 3-t18.n11, 3-table3-18.c23.", "", true},
    {12, "3-t17.n12, 3-table3-17.c37, 3-t18.n12, 3-table3-18.c25.", "", true},
    {13, "3-t17.n13, 3-table3-17.c40, 3-t18.n13, 3-table3-18.c27.", "", true},
    {14, "3-t17.n14, 3-table3-17.c43, 3-t18.n14, 3-table3-18.c29.", "", true},
    {15, "3-t17.n15, 3-table3-17.c46, 3-t18.n15, 3-table3-18.c31.", "", true},
    {16, "3-t17.n16, 3-table3-17.c49.", "", false},
    {17, "3-t17.n17, 3-table3-17.c52.", "", false},
    {18, "3-t17.n18, 3-table3-17.c55.", "", false},
    {19, "3-t17.n19, 3-table3-17.c58.", "", false},
    {20, "3-t17.n20, 3-table3-17.c61.", "", false},
    {21, "3-t17.n21, 3-table3-17.c64.", "", false},
    {22, "3-t17.n22, 3-table3-17.c67.", "", false},
    {23, "3-t17.n23, 3-table3-17.c70.", "", false},
    {24, "3-t17.n24, 3-table3-17.c73.", "", false},
    {25, "3-t17.n25, 3-table3-17.c76.", "", false},
    {26, "3-t17.n26, 3-table3-17.c79.", "", false},
    {27, "3-t17.n27, 3-table3-17.c82.", "", false},
    {28, "3-t17.n28, 3-table3-17.c85.", "", false},
    {29, "3-t17.n29, 3-table3-17.c88.", "", false},
    {30, "3-t17.n30, 3-table3-17.c91.", "", false},
    {31, "3-t17.n31, 3-table3-17.c94, 3-bpl.c01, 3-color.c1, 3-code.c06, 3-coding.c11.", "This is also the '32 different colors...using normal display methods' / 'five bitplanes allows a choice of 32 different colors' capacity statement (p.42, p.56) -- register 31 is the single-playfield maximum.", false},
};

// One row per Table 3-10 (playfield 1, low-resolution dual-playfield code,
// k=0..7) entry. `ids` additionally folds in: Table 3-12a's matching
// high-resolution rows (k=0..3, bitplane 5 simply held at 0 -- Table 3-12a
// tabulates no separate mechanism, just the 2-bit subset of the same 3-bit
// code), a second reader's duplicate extraction of both tables
// (3-dual-color.*), and both readers' extractions of Table 3-17's dual
// column (which restates the identical k -> register mapping on a
// different printed page). `p94` marks rows 0-3, which Table 3-18's dual
// column (p.94) corroborates the same way Table 3-12a does at p.71.
struct DualRow { int k; const char* ids; bool p94; };
const DualRow PF1_ROWS[8] = {
    {0, "3-t10.c00, 3-t17.d00, 3-table3-17.c3, 3-dual-color.c4, 3-dual.c30, 3-t12a.c00, 3-dual-color.c26, 3-t18.d00, 3-table3-18.c2, 3-t18.fn1, 3-table3-18.c33.", true},
    {1, "3-t10.c01, 3-t17.d01, 3-table3-17.c6, 3-dual-color.c5, 3-t12a.c01, 3-dual-color.c27, 3-t18.d01, 3-table3-18.c4.", true},
    {2, "3-t10.c02, 3-t17.d02, 3-table3-17.c9, 3-dual-color.c6, 3-t12a.c10, 3-dual-color.c28, 3-t18.d02, 3-table3-18.c6.", true},
    {3, "3-t10.c03, 3-t17.d03, 3-table3-17.c12, 3-dual-color.c7, 3-t12a.c11, 3-dual-color.c29, 3-t18.d03, 3-table3-18.c8.", true},
    {4, "3-t10.c04, 3-t17.d04, 3-table3-17.c15, 3-dual-color.c8.", false},
    {5, "3-t10.c05, 3-t17.d05, 3-table3-17.c18, 3-dual-color.c9.", false},
    {6, "3-t10.c06, 3-t17.d06, 3-table3-17.c21, 3-dual-color.c10.", false},
    {7, "3-t10.c07, 3-t17.d07, 3-table3-17.c24, 3-dual-color.c11.", false},
};
// Symmetric for playfield 2 (Table 3-11, registers 8-15; Table 3-12b for
// k=0..3). Row 0 additionally carries the two ids that name BOTH
// playfields' code-0 transparency in one sentence (3-dual.c30, Table
// 3-18's footnote * / 3-table3-18.c33) -- already cited once on PF1_ROWS
// row 0 above; cited again here because the same compound sentence
// constrains this row's own measurement too.
const DualRow PF2_ROWS[8] = {
    {0, "3-t11.c00, 3-t17.d08, 3-table3-17.c27, 3-t17.fn2, 3-table3-17.c98, 3-dual-color.c15, 3-t12b.c00, 3-dual-color.c30, 3-t18.d08, 3-table3-18.c18, 3-dual.c30, 3-t18.fn1, 3-table3-18.c33.", true},
    {1, "3-t11.c01, 3-t17.d09, 3-table3-17.c30, 3-dual-color.c16, 3-t12b.c01, 3-dual-color.c31, 3-t18.d09, 3-table3-18.c20.", true},
    {2, "3-t11.c02, 3-t17.d10, 3-table3-17.c33, 3-dual-color.c17, 3-t12b.c10, 3-dual-color.c32, 3-t18.d10, 3-table3-18.c22.", true},
    {3, "3-t11.c03, 3-t17.d11, 3-table3-17.c36, 3-dual-color.c18, 3-t12b.c11, 3-dual-color.c33, 3-t18.d11, 3-table3-18.c24.", true},
    {4, "3-t11.c04, 3-t17.d12, 3-table3-17.c39, 3-dual-color.c19.", false},
    {5, "3-t11.c05, 3-t17.d13, 3-table3-17.c42, 3-dual-color.c20.", false},
    {6, "3-t11.c06, 3-t17.d14, 3-table3-17.c45, 3-dual-color.c21.", false},
    {7, "3-t11.c07, 3-t17.d15, 3-table3-17.c48, 3-dual-color.c22.", false},
};

// One row per Figure 3-13 entry (n = number of lowest consecutive
// bitplanes active, 0..6): pf1_id/pf2_id are one reader's ids for that
// row's playfield-1/playfield-2 column.
struct FigRow { int n; const char* pf1_id; const char* pf2_id; };
const FigRow FIG313_ROWS[7] = {
    {0, "3-f13.c01", "3-f13.c02"},
    {1, "3-f13.c03", "3-f13.c04"},
    {2, "3-f13.c05", "3-f13.c06"},
    {3, "3-f13.c07", "3-f13.c08"},
    {4, "3-f13.c09", "3-f13.c10"},
    {5, "3-f13.c11", "3-f13.c12"},
    {6, "3-f13.c13", "3-f13.c14"},
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL -- the fixture can deliver a known value before anything below
    // is trusted to read one back (package brief, "the fixture proves it can
    // deliver a known value before anything reads one").
    // =========================================================================
    {
        Harness h;
        uint8_t stim1 = 5;
#if MUT_CONTROL
        stim1 = 4;  // the first control stimulus is changed from 5 to 4, so the reading no longer matches the value 5 the check expects
#endif
        uint8_t v1 = h.sample(stim1);   // 00101 -> expect register 5
        uint8_t v2 = h.sample(22);  // 10110 -> expect register 22
        checks.expect(
            v1 == 5 && v2 == 22 && v1 != v2,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct, known "
            "single-playfield stimuli (bit combinations 5 and 22) produce "
            "two distinct, correctly-positioned plfdata readings (5, then "
            "22) through the fixture's own eval() path before any "
            "assertion below trusts it. No manual page: the control for "
            "every assertion in this file, on the same measurement basis.");
    }

    // =========================================================================
    // COLOUR-INDIRECTION-MECHANISM (p.40/42) -- the general principle every
    // table below is the exhaustive statement of: a pixel's displayed
    // colour is the colour-register number formed from the binary
    // combination of the bits that define it (colour indirection), and
    // each pixel is controlled by its own bit or bits.
    // =========================================================================
    {
        Harness h;
        uint8_t stim = 21;  // 10101 -- an arbitrary, non-edge combination
#if MUT_MECH
        stim = 20;  // a different combination is presented than the one the label names
#endif
        uint8_t out = h.sample(stim);
        checks.expect(
            out == 21,
            "COLOUR-INDIRECTION-MECHANISM (p.40/42): an arbitrary bit "
            "combination (21, i.e. 10101) selects colour register 21, "
            "demonstrating the general rule every table below states "
            "exhaustively -- 'the combination of the \"1\"s and \"0\"s "
            "will determine the colors in your display' (p.40), 'the "
            "color of a pixel is always determined by the binary "
            "combination of the bits that define it' (p.42), 'you can "
            "control the color of each individual pixel...by setting the "
            "bit or bits that control each pixel' (p.42), 'this method of "
            "coloring pixels is called color indirection' (p.42), "
            "'combines...bitplanes for display' overlap depth (p.42). "
            "3-video.c4, 3-bpl.c10, 3-bpl.c11, 3-color.c2, 3-color.c11, "
            "3-color.c6, 3-bpl.c05.");
    }

    // =========================================================================
    // BIT-SIGNIFICANCE-ORDER (p.43/44/93/94) -- the highest-numbered active
    // bitplane forms the most significant digit of the colour-register
    // number; each bitplane in isolation (1 through 5) produces exactly
    // its own power-of-two register number, confirming MSB..LSB =
    // bitplane 5..1 in that order.
    // =========================================================================
    {
        struct BitCase { int plane; uint8_t expected; };
        const BitCase cases[5] = {
            {1, 1}, {2, 2}, {3, 4}, {4, 8}, {5, 16},
        };
        for (const auto& c : cases) {
            Harness h;
            uint8_t stim = PLANE(c.plane);
#if MUT_ORDER
            if (c.plane == 5) stim = static_cast<uint8_t>(PLANE(5) | PLANE(1));  // plane 5's isolated case gets a stray plane-1 bit added
#endif
            uint8_t out = h.sample(stim);
            checks.expect(
                out == c.expected,
                "BIT-SIGNIFICANCE-ORDER (p.43/44/93/94): bitplane " +
                std::to_string(c.plane) + ", alone, produces colour "
                "register " + std::to_string(static_cast<int>(c.expected)) +
                " -- 'values in the highest numbered bitplane have the "
                "highest significance...forms the leftmost digit of the "
                "number' (p.43), Figure 3-3's one-pixel-per-bitplane-stack "
                "callout (p.43), Figure 3-4's 'Data in bitplane 5 - most "
                "significant' annotation (p.44), Table 3-17's heading "
                "'(Bitplanes 5,4,3,2,1)' for single-playfield low "
                "resolution (p.93), Table 3-18's heading '(Bitplanes "
                "4,3,2,1)' for single-playfield high resolution (p.94). "
                "3-bpl.c17, 3-bpl.c18, 3-bpl.f04, 3-bpl.f05, 3-color.c18, "
                "3-t17.hdr1, 3-t18.hdr1.");
        }
    }

    // =========================================================================
    // IDENTITY-MAPPING-SWEEP (p.93/94) -- Tables 3-17 and 3-18's
    // single-playfield columns: bit combination N selects colour register
    // N, for every N the manual tabulates (0..31 low resolution, a subset
    // 0..15 corroborated again at high resolution where bitplane 5 is not
    // part of the code). One expect() per table row -- 32 propositions,
    // each naming its own ids and page(s).
    // =========================================================================
    {
        for (int i = 0; i < 32; ++i) {
            const SweepRow& row = SWEEP_ROWS[i];
            Harness h;
            uint8_t stim = static_cast<uint8_t>(row.n);
#if MUT_SWEEP
            if (row.n == 1) stim = 2;  // N=1's own stimulus is presented as N=2's combination instead
#endif
            uint8_t out = h.sample(stim);
            std::string label = "IDENTITY-MAPPING-SWEEP (p.93" +
                std::string(row.p94 ? "/94" : "") + "): bit combination " +
                std::to_string(row.n) + " selects colour register " +
                std::to_string(row.n) + ". " + std::string(row.note) +
                (row.note[0] ? " " : "") + row.ids;
            checks.expect(out == row.n, label);
        }
    }

    // =========================================================================
    // DUAL-MODE-CAPACITY (p.44/93/94) -- with all six dual-mode bitplanes
    // active at once (dblpf=1), playfield 1's own code still reaches its
    // 3-bit maximum (register 7) and playfield 2's reaches its own
    // maximum independently (register 15); neither climbs into the 16-31
    // range Tables 3-17/3-18's dual columns mark "NOT USED IN THIS MODE".
    // =========================================================================
    {
        Harness h;
        uint8_t all6 = static_cast<uint8_t>(PLANE(1) | PLANE(2) | PLANE(3) | PLANE(4) | PLANE(5) | PLANE(6));
#if MUT_CAPACITY
        all6 = static_cast<uint8_t>(PLANE(1) | PLANE(2) | PLANE(3) | PLANE(4) | PLANE(6));  // bitplane 5 dropped: PF1 can no longer reach its 3-bit maximum
#endif
        uint8_t pf1_max = h.sample(all6, true, 0x00);
        uint8_t pf2_max = h.sample(all6, true, PF2PRI_BIT);
        checks.expect(
            pf1_max == 7 && pf2_max == 15,
            "DUAL-MODE-CAPACITY (p.44/93/94): with all six dual-mode "
            "bitplanes active at once, playfield 1's own code still "
            "reaches its 3-bit maximum (register 7, read under PF1 "
            "priority) and playfield 2's reaches its own maximum "
            "independently (register 15, read under PF2 priority) -- "
            "'you also have the choice of defining two separate "
            "playfields' (p.44), dual-playfield mode gives 'each of the "
            "two playfields...a separate set of eight different colors' "
            "(p.44), Table 3-17's dual column stopping at register 15 "
            "with registers 16-31 marked 'NOT USED IN THIS MODE' (p.93), "
            "Table 3-18's dual column likewise capped (p.94). 3-dual.c01, "
            "3-dual.c03, 3-dual.c25, 3-dual.c29, 3-t17.notused2, "
            "3-t18.notused.");
    }

    // =========================================================================
    // BIT-ORDER-DUAL (p.70/93/94) -- each dual-mode bitplane, isolated,
    // produces exactly its own weight within its playfield's code:
    // bitplane 1/3/5 -> playfield 1's weight 1/2/4; bitplane 2/4/6 ->
    // playfield 2's weight 1/2/4 (register 9/10/12, i.e. 8+1/8+2/8+4).
    // =========================================================================
    {
        struct DualBitCase { int plane; uint8_t expected; };
        const DualBitCase cases[6] = {
            {1, 1}, {3, 2}, {5, 4}, {2, 9}, {4, 10}, {6, 12},
        };
        for (const auto& c : cases) {
            Harness h;
            uint8_t stim = PLANE(c.plane);
#if MUT_BITORDER
            if (c.plane == 5) stim = static_cast<uint8_t>(PLANE(5) | PLANE(1));  // plane 5's isolated case gets a stray plane-1 bit added
#endif
            // PF1 priority: when only an even (PF2) plane is active, PF1 is
            // empty and the code falls through to PF2 in the same read.
            uint8_t out = h.sample(stim, true, 0x00);
            checks.expect(
                out == c.expected,
                "BIT-ORDER-DUAL (p.70/93/94): dual-mode bitplane " +
                std::to_string(c.plane) + ", alone, produces colour "
                "register " + std::to_string(static_cast<int>(c.expected)) +
                " -- 'the hardware interprets color numbers for playfield "
                "1 from...bitplanes 1, 3, and 5' (p.70), 'for playfield 2 "
                "from...bitplanes 2, 4, and 6' (p.70), Table 3-17's dual "
                "headings '(Bitplanes 5,3,1)' / '(Bitplanes 6,4,2)' "
                "(p.93), Table 3-18's dual headings '(Bitplanes 3,1)' / "
                "'(Bitplanes 4,2)' (p.94). 3-dual.c22, 3-dual.c26, "
                "3-t17.hdr3, 3-t17.hdr4, 3-t18.hdr2, 3-t18.hdr3.");
        }
    }

    // =========================================================================
    // DUAL-PRIORITY (p.69/71) -- the swap-bit (BPLCON2 bit 6, PF2PRI)
    // decides which playfield is shown in front when both are opaque.
    // =========================================================================
    {
        Harness h;
        uint8_t both = static_cast<uint8_t>(PF1_CODE(7) | PF2_CODE(7));
        uint8_t pf2pri_bit = PF2PRI_BIT;
#if MUT_DUALPRI
        pf2pri_bit = 0x00;  // the PF2-priority request is silently dropped: same bplcon2 as the PF1 read
#endif
        uint8_t out_pri0 = h.sample(both, true, 0x00);
        uint8_t out_pri1 = h.sample(both, true, pf2pri_bit);
        checks.expect(
            out_pri0 == 7 && out_pri1 == 15,
            "DUAL-PRIORITY (p.69/71): with both playfields simultaneously "
            "opaque, the swap-bit (BPLCON2 bit 6, PF2PRI) decides which "
            "one is shown in front -- clear (0) gives playfield 1 "
            "(register 7); set (1) gives playfield 2 (register 15) -- "
            "'either playfield may be placed \"in front of\" or \"behind\" "
            "the other using the \"swap-bit\"' (p.69, Figure 3-13's "
            "footnote, quoted by two different readers), 'either "
            "playfield 1 or 2 may have priority' (p.71). 3-dual.c21, "
            "3-dual-planes.c21, 3-f13.c15, 3-pri.c01.");
    }

    // =========================================================================
    // FIGURE-3-13-ROWS (p.69) -- Figure 3-13's rows: with the lowest N
    // consecutive bitplanes active (N=0..6), playfield 1 is present iff
    // N>=1 and playfield 2 is present iff N>=2. Presence inferred the same
    // way hrm/ch3-dual-playfield's own FIG313-ROW-COUNTS does: a PF1-
    // priority read landing in 1..7 means playfield 1 has data; a
    // PF2-priority read landing at >=9 means playfield 2 has data.
    // =========================================================================
    {
        for (int n = 0; n <= 6; ++n) {
            Harness h;
            uint8_t stim = 0;
            for (int k = 1; k <= n; ++k) stim = static_cast<uint8_t>(stim | PLANE(k));
#if MUT_FIG313
            if (n == 1) stim = PLANE(2);  // row 1's single active plane is presented as plane 2 instead of plane 1
#endif
            uint8_t via_pf1 = h.sample(stim, true, 0x00);
            uint8_t via_pf2 = h.sample(stim, true, PF2PRI_BIT);
            bool pf1_present = (via_pf1 >= 1 && via_pf1 <= 7);
            bool pf2_present = (via_pf2 >= 9);
            bool expect_pf1 = (n >= 1);
            bool expect_pf2 = (n >= 2);
            const FigRow& row = FIG313_ROWS[n];
            checks.expect(
                pf1_present == expect_pf1 && pf2_present == expect_pf2,
                "FIGURE-3-13-ROWS (p.69): with the lowest " +
                std::to_string(n) + " consecutive bitplane(s) active, "
                "playfield 1 is " + std::string(expect_pf1 ? "present" : "empty") +
                " and playfield 2 is " + std::string(expect_pf2 ? "present" : "empty") +
                ". " + std::string(row.pf1_id) + ", " + std::string(row.pf2_id) + ".");
        }
    }

    // =========================================================================
    // PF1-CODE-SWEEP (p.70/71/93/94) -- Table 3-10 (and, for k=0..3, Table
    // 3-12a's matching high-resolution subset): playfield 1's own 3-bit
    // code selects colour register k for k=1..7; code 000 is transparent,
    // shown here by presenting playfield 2 opaque (max code) at the same
    // time and confirming playfield 2's code (register 15), not register
    // 0, is what comes through.
    // =========================================================================
    {
        for (int k = 0; k < 8; ++k) {
            const DualRow& row = PF1_ROWS[k];
            Harness h;
            uint8_t out;
            int expected;
            if (k == 0) {
                uint8_t stim = static_cast<uint8_t>(PF1_CODE(0) | PF2_CODE(7));
                out = h.sample(stim, true, 0x00);  // PF1 priority
                expected = 15;
            } else {
                uint8_t stim = PF1_CODE(k);
#if MUT_PF1SWEEP
                if (k == 1) stim = PF1_CODE(2);  // row k=1's own stimulus is presented as k=2's combination instead
#endif
                out = h.sample(stim, true, 0x00);
                expected = k;
            }
            std::string label = "PF1-CODE-SWEEP (p.70" +
                std::string(row.p94 ? "/71/93/94" : "/93") + "): ";
            if (k == 0) {
                label += "playfield 1's code 000 (all its planes off), "
                    "with playfield 2 opaque at the same time, shows "
                    "playfield 2's code through (register 15) rather than "
                    "selecting register 0 -- code 000 is transparent, not "
                    "an opaque colour.";
            } else {
                label += "playfield 1's 3-bit code " + std::to_string(k) +
                    " selects colour register " + std::to_string(k) + ".";
            }
            label += " " + std::string(row.ids);
            checks.expect(out == expected, label);
        }
    }

    // =========================================================================
    // PF2-CODE-SWEEP (p.70/71/93/94) -- Table 3-11 (and, for k=0..3, Table
    // 3-12b's matching high-resolution subset): playfield 2's own 3-bit
    // code selects colour register 8+k for k=1..7; code 000 is
    // transparent, shown here by presenting playfield 1 opaque (max code)
    // under PF2 priority and confirming playfield 1's code (register 7),
    // not register 8, is what comes through.
    // =========================================================================
    {
        for (int k = 0; k < 8; ++k) {
            const DualRow& row = PF2_ROWS[k];
            Harness h;
            uint8_t out;
            int expected;
            if (k == 0) {
                uint8_t stim = static_cast<uint8_t>(PF2_CODE(0) | PF1_CODE(7));
                out = h.sample(stim, true, PF2PRI_BIT);  // PF2 priority
                expected = 7;
            } else {
                uint8_t stim = PF2_CODE(k);
#if MUT_PF2SWEEP
                if (k == 1) stim = PF2_CODE(2);  // row k=1's own stimulus is presented as k=2's combination instead
#endif
                out = h.sample(stim, true, PF2PRI_BIT);
                expected = 8 + k;
            }
            std::string label = "PF2-CODE-SWEEP (p.70" +
                std::string(row.p94 ? "/71/93/94" : "/93") + "): ";
            if (k == 0) {
                label += "playfield 2's code 000 (all its planes off), "
                    "with playfield 2 given priority and playfield 1 "
                    "opaque at the same time, shows playfield 1's code "
                    "through (register 7) rather than selecting register "
                    "8 -- code 000 is transparent, not an opaque colour.";
            } else {
                label += "playfield 2's 3-bit code " + std::to_string(k) +
                    " selects colour register " + std::to_string(8 + k) + ".";
            }
            label += " " + std::string(row.ids);
            checks.expect(out == expected, label);
        }
    }

    // =========================================================================
    // MEASURED -- print defects, recorded for traceability only (not
    // asserted; see header PRINT DEFECT #1/#2).
    // =========================================================================
    checks.record(
        "Table 3-17 (printed p.93) prints the same hold-and-modify code, "
        "0100, on both its 00100 row and its 00110 row -- a transcription "
        "error in that column, out of this module's boundary regardless "
        "(see NO-HAM-PORT among the declined claims). It does not affect "
        "the single-playfield column tested above: N=4 and N=6 both read "
        "correctly (registers 4 and 6) on the same measurement basis as "
        "every other N in IDENTITY-MAPPING-SWEEP.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
