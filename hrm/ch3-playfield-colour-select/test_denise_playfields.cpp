// ch3-playfield-colour-select -- denise_playfields conformance test
//
// Test purpose: single-playfield colour selection as chapter 3 describes
//   it: how the bit combination formed from the enabled bitplanes selects
//   a colour register, one-for-one (Tables 3-17 and 3-18), which bitplane
//   supplies the most-significant bit, and the general colour-indirection
//   principle those tables are the exhaustive statement of. Package
//   ch3-playfield-colour-select, fixture denise_playfields (one Verilator
//   build, this module only, per PACKAGE.md). A sibling package,
//   ch3-dual-playfield, already covers DBLPF mode, odd/even plane
//   grouping, colour-register ranges 0-7/8-15 and playfield transparency
//   on this same fixture; this package does not duplicate it -- every
//   claim in this package's 272-record inventory that names dual-playfield
//   behaviour is declined below, not re-tested.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 40, 42, 43, 44, 56, 93, 94 (claims.jsonl for
//   ch3-playfield-colour-select, 272 records, 127 asserted below). A red
//   result below means denise_playfields.v disagrees with the manual, not
//   that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass.
// Non-goals: `aga` and `pf2of` are held at their reset default (0)
//   throughout, matching the sibling harness's convention -- no claim in
//   this package's inventory names AGA extensions or the OCS
//   priority-offset behaviour. `dblpf` and `bplcon2` are held at 0
//   throughout for a different reason: 0 is not an arbitrary non-goal
//   default here, it is single-playfield mode itself -- the condition this
//   whole package tests under (PACKAGE.md's own evidence: "single
//   playfield: plfdata = bpldata[8:1]" is the dblpf=0 branch of
//   denise_playfields.v:95-100). Whether dblpf actually switches behaviour
//   is hrm/ch3-dual-playfield's DBLPF-ACTIVATION assertion, not this
//   package's. `nplayfield` is never read, for the same reason the sibling
//   gives: no claim here describes its semantics and every proposition
//   tested is independently observable through `plfdata` alone.
//
// BOUNDARY (from PACKAGE.md's own evidence field, carried into every claim
// record): nanomig/src/minimig-aga/denise_playfields.v:8-17 module header
//   (aga l.10, `bpldata`[8:1] l.11, dblpf l.12, pf2of l.13, `bplcon2`[6:0]
//   l.14, `nplayfield`[2:1] l.15, `plfdata`[7:0] l.16) | l.95-100 (single
//   playfield: plfdata = bpldata[8:1]); MEASURED (d_pf): bit combination N
//   selects colour register N for every N in 0..31.
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
// live data bit, n=1..8) and the eval()-only, no-clock usage pattern are
// likewise carried from that sibling, not learned by opening this
// package's own forbidden-until-committed file.
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
// CLAIM COVERAGE: 127 of the 272 records in claims.jsonl are cited below,
// in full, inside an expect() label; 145 are declined (see DECLINED CLAIMS).
// Verified by script: every one of the 272 ids in claims.jsonl is
// classified exactly once, either asserted or declined, with zero overlap
// and zero omissions.
//
//   COLOUR-INDIRECTION-MECHANISM (7 ids)        -- the general principle.
//   BIT-SIGNIFICANCE-ORDER (7 ids, loop x5)      -- MSB..LSB = bitplane 5..1.
//   IDENTITY-MAPPING-SWEEP (113 ids, loop x32)   -- Tables 3-17 & 3-18: N -> register N.
//   7+7+113 = 127.
//
// DECLINED CLAIMS (145 ids, grouped by reason):
//
//   DBLPF-GENERAL (p.44/69/70/71) -- dual-playfield mode as a concept: its existence, the swap-bit/PF2PRI priority mechanism, colour-register-8-15 range framing, and independently-sized playfield memory. Fully covered by hrm/ch3-dual-playfield's TWO-PLAYFIELDS-CAPACITY, COLOUR-REGISTER-RANGES and PF2PRI-PRIORITY assertions.
//     (13 ids) 3-dual.c01, 3-dual.c03, 3-dual.c21, 3-dual.c22, 3-dual.c25, 3-dual.c26, 3-dual.c29, 3-dual.c30, 3-dual.c32, 3-dual-planes.c21, 3-dual-control.c1, 3-dual-control.c2, 3-pri.c01
//
//   TABLE-3-10 (p.70) -- playfield 1, low-resolution dual-playfield 3-bit code table (all 8 rows). Table 3-10 sits inside chapter 3's dual-playfield section (bitplanes 1,3,5 feeding PF1 under DBLPF); its mechanism (odd-plane grouping, code 0 = transparent, codes 1-7 = registers 1-7) is hrm/ch3-dual-playfield's ODD-EVEN-GROUPING, CODE-ZERO-TRANSPARENT and MSB-BIT-IDENTITY territory.
//     (8 ids) 3-t10.c00, 3-t10.c01, 3-t10.c02, 3-t10.c03, 3-t10.c04, 3-t10.c05, 3-t10.c06, 3-t10.c07
//
//   TABLE-3-11 (p.70) -- playfield 2, low-resolution dual-playfield 3-bit code table (all 8 rows). Same reasoning as TABLE-3-10, for the even-plane group (bitplanes 2,4,6) and registers 8-15; hrm/ch3-dual-playfield's ODD-EVEN-GROUPING, COLOUR-REGISTER-RANGES and PF2-LSB-BIT-IDENTITY cover the mechanism.
//     (8 ids) 3-t11.c00, 3-t11.c01, 3-t11.c02, 3-t11.c03, 3-t11.c04, 3-t11.c05, 3-t11.c06, 3-t11.c07
//
//   TABLE-3-12 (p.71) -- playfield 1 and playfield 2, high-resolution dual-playfield 2-bit code tables. High-resolution dual mode (2 bitplanes per playfield instead of 3) is the same DBLPF territory as TABLE-3-10/3-11, one resolution step over; no separate boundary claim in this package's inventory treats high-res dual mode differently from low-res dual mode.
//     (8 ids) 3-t12a.c00, 3-t12a.c01, 3-t12a.c10, 3-t12a.c11, 3-t12b.c00, 3-t12b.c01, 3-t12b.c10, 3-t12b.c11
//
//   DUAL-COLOR-DUPLICATE-READING (p.70/71) -- a second reader's independent extraction of the exact same Table 3-10/3-11/3-12 rows above (same printed lines, same propositions, different claim ids). Declined for the same reason as TABLE-3-10/3-11/3-12.
//     (24 ids) 3-dual-color.c4, 3-dual-color.c5, 3-dual-color.c6, 3-dual-color.c7, 3-dual-color.c8, 3-dual-color.c9, 3-dual-color.c10, 3-dual-color.c11, 3-dual-color.c15, 3-dual-color.c16, 3-dual-color.c17, 3-dual-color.c18, 3-dual-color.c19, 3-dual-color.c20, 3-dual-color.c21, 3-dual-color.c22, 3-dual-color.c26, 3-dual-color.c27, 3-dual-color.c28, 3-dual-color.c29, 3-dual-color.c30, 3-dual-color.c31, 3-dual-color.c32, 3-dual-color.c33
//
//   FIGURE-3-13-ROWS (p.69) -- the bitplane-count-vs-playfield-occupancy figure, rows for 0-6 active bitplanes plus its swap-bit footnote. This is the identical figure hrm/ch3-dual-playfield's FIG313-ROW-COUNTS already sweeps (its own citations, 3-dual-planes.c7-c20, are a different reader's ids for the same 7 rows); this package's own ids for it (3-f13.c01-c15, one reader's 14 rows + footnote) are a straight duplicate.
//     (15 ids) 3-f13.c01, 3-f13.c02, 3-f13.c03, 3-f13.c04, 3-f13.c05, 3-f13.c06, 3-f13.c07, 3-f13.c08, 3-f13.c09, 3-f13.c10, 3-f13.c11, 3-f13.c12, 3-f13.c13, 3-f13.c14, 3-f13.c15
//
//   TABLE-3-17-DUAL-COLUMN (p.93) -- Table 3-17 is one printed table with two code columns: a single-playfield 5-bit column (bitplanes 5,4,3,2,1 -> registers 0-31, ASSERTED below in IDENTITY-MAPPING-SWEEP) and a dual-playfield 3-bit column (bitplanes 5,3,1 for PF1 / 6,4,2 for PF2 -> registers 0-15, footnoted transparent-at-0 and capped at 15). The dual column restates exactly what Table 3-10/3-11 state in prose form and what hrm/ch3-dual-playfield's COLOUR-REGISTER-RANGES and CODE-ZERO-TRANSPARENT already assert -- declined for that reason, not reused here even though it shares a printed table with the single-playfield column this package does test.
//     (37 ids) 3-t17.d00, 3-t17.d01, 3-t17.d02, 3-t17.d03, 3-t17.d04, 3-t17.d05, 3-t17.d06, 3-t17.d07, 3-t17.d08, 3-t17.d09, 3-t17.d10, 3-t17.d11, 3-t17.d12, 3-t17.d13, 3-t17.d14, 3-t17.d15, 3-t17.fn2, 3-t17.hdr3, 3-t17.hdr4, 3-t17.notused2, 3-table3-17.c3, 3-table3-17.c6, 3-table3-17.c9, 3-table3-17.c12, 3-table3-17.c15, 3-table3-17.c18, 3-table3-17.c21, 3-table3-17.c24, 3-table3-17.c27, 3-table3-17.c30, 3-table3-17.c33, 3-table3-17.c36, 3-table3-17.c39, 3-table3-17.c42, 3-table3-17.c45, 3-table3-17.c48, 3-table3-17.c98
//
//   TABLE-3-18-DUAL-COLUMN (printed p.94) -- same reasoning as TABLE-3-17-DUAL-COLUMN, for Table 3-18's dual-playfield 2-bit column (bitplanes 3,1 for PF1 / 4,2 for PF2 -> registers 0-3/8-11, footnoted transparent-at-0, background-at-register-0, and NOT USED IN THIS MODE for the codes the 2-bit column cannot reach).
//     (21 ids) 3-t18.d00, 3-t18.d01, 3-t18.d02, 3-t18.d03, 3-t18.d08, 3-t18.d09, 3-t18.d10, 3-t18.d11, 3-t18.fn1, 3-t18.hdr2, 3-t18.hdr3, 3-t18.notused, 3-table3-18.c2, 3-table3-18.c4, 3-table3-18.c6, 3-table3-18.c8, 3-table3-18.c18, 3-table3-18.c20, 3-table3-18.c22, 3-table3-18.c24, 3-table3-18.c33
//
//   NO-MEMORY-PORT (p.42/53/54/76) -- playfield memory geometry (bytes-per-line organization, memory-vs-displayed-window sizing, even-byte allocation, sprite/audio/animation memory budgets). denise_playfields has no memory-address or memory-size port (aga, bpldata, dblpf, pf2of, bplcon2, nplayfield in; plfdata out only) -- it receives one already-fetched bit per bitplane per pixel and has no visibility into how that bit was stored.
//     (4 ids) 3-video.c28, 3-memory.c20, 3-mem.c12, 3-large.c22
//
//   NO-WINDOW-PORT (p.80) -- the ECS display-window-limit change. A DDF/display-window boundary question (agnus's fetch-window logic), not this per-pixel colour-composition module's; see hrm/ch3-ddf-fetch-window.
//     (1 id) 3-max-window.c31
//
//   NO-BPLCON1-PORT (p.85) -- horizontal scroll delay, set per playfield via BPLCON1 bits. denise_playfields' only control-register input is bplcon2[6:0] (PF2PRI and AGA priority bits); BPLCON1 is not a port here. See hrm/ch3-bplcon1-scroll-delay.
//     (3 ids) 3-hscr.c22, 3-hscroll.c29, 3-hscroll.c31
//
//   NO-SPRITE-PORT (p.86) -- playfields sharing the display with sprites. No sprite input exists on this module; sprite/playfield compositing is hrm/ch3-sprite-playfield-priority's boundary.
//     (1 id) 3-adv.c01
//
//   NO-HAM-PORT (p.86) -- planes 5 and 6 as HOLD/MODIFY control bits. HAM colour composition is a different module's job (denise_hamgenerator); see hrm/ch3-ham-control-field. (This is also the module that owns Table 3-17/3-18's 4-bit Bitplanes-4,3,2,1 HAM column -- see PRINT DEFECT #1 above on why that column is not exercised here either.)
//     (1 id) 3-ham.c08
//
//   NO-DMA-PORT (p.272) -- the parallel-to-serial shift register's own timing (triggered by bitplane-1 writes, 16 pixels per converted word). denise_playfields receives bpldata already serialized to one bit per plane per pixel; the conversion itself happens upstream, in bitplane DMA/shift-register logic this module has no port into.
//     (1 id) A-bpldat.c8
//
//   13+8+8+8+24+15+37+21+4+1+3+1+1+1 = 145.
//
// CONTROL CASE: CONTROL-KNOWN-VALUES-ARRIVE (no claim id, no manual page)
// is the control every page-cited assertion below shares: the fixture is
// shown to deliver two distinct, known plfdata values (5 and 22) through
// the same eval() path used everywhere else, before anything below trusts
// it. It carries no claim id and is prefixed "[control]" so the mutation
// gate does not require a mutation record for it -- the convention
// hrm/ch3-dual-playfield's CONTROL-KNOWN-VALUES-ARRIVE and
// hrm/ch3-collision-registers' equivalent both use.
//
// RESULTS (after opening the RTL and building): 39 passed, 0 failed, plus
// the 1 MEASURED note (printed, not counted as pass or fail).
// denise_playfields.v turned out to implement the manual's single-
// playfield colour-indirection description exactly as every assertion
// above expected -- no finding in this package. One thing worth recording
// precisely because it was checked, not assumed, on opening the RTL:
//   - denise_playfields.v:96-99 hides an undocumented conditional inside
//     the single-playfield branch PACKAGE.md's evidence field cited as a
//     flat "plfdata = bpldata[8:1])" (line 100 only): when
//     `(pf2p>5) && bpldata[5] && !aga` (pf2p = bplcon2[5:3], an
//     "OCS/ECS undocumented feature...Swiv score display" per the RTL's
//     own comment), plfdata is forced to 8'b00010000 (16) regardless of
//     bpldata. No claim in this package's 272-record inventory describes
//     this behaviour -- it is undocumented, so there is nothing in the
//     manual to assert it against either way. It does not interfere with
//     any assertion above: bplcon2 is held at 0 throughout this package
//     (see Non-goals), so pf2p is always 0 and `pf2p>5` is always false,
//     confirmed by the identity-mapping sweep's own N=16..31 rows (which
//     all set bpldata[5]=1 and all read back correctly rather than
//     collapsing to the constant 16 this exception would produce). This
//     is a harness-wiring confirmation, not an oracle change -- no
//     expected value anywhere in this file was altered after the RTL was
//     opened.
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
// assertion red -- 38 passed/1 failed in every case, the other 38
// assertions unaffected -- then rebuilt with no flags and reconfirmed the
// clean 39-passed/0-failed baseline):
//
// MUTATION COLOUR-INDIRECTION-MECHANISM stimulus red: the stimulus was changed from 21 (10101) to 20 (10100), so the reading no longer matches the value 21 the label and citations name.
// MUTATION BIT-SIGNIFICANCE-ORDER stimulus red: bitplane 5's isolated case was changed to also present bitplane 1 alongside it, so the reading is no longer the clean, isolated MSB value of 16.
// MUTATION IDENTITY-MAPPING-SWEEP stimulus red: row N=1's own stimulus was changed to present N=2's combination instead, so that row no longer reads back as register 1.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vdenise_playfields.h"
#include "verilated.h"

#ifndef MUT_MECH
#define MUT_MECH 0
#endif
#ifndef MUT_ORDER
#define MUT_ORDER 0
#endif
#ifndef MUT_SWEEP
#define MUT_SWEEP 0
#endif

namespace {

// bpldata bit (n-1) is bitplane n's live data bit, n=1..8 -- carried from
// hrm/ch3-dual-playfield/test_denise_playfields.cpp's own stimulus usage,
// not learned by opening this package's own file.
constexpr uint8_t PLANE(int n) { return static_cast<uint8_t>(1u << (n - 1)); }

class Harness {
public:
    Harness() : dut(new Vdenise_playfields) {
        // aga/pf2of: non-goals (see header). dblpf/bplcon2: held at 0
        // throughout -- 0 is single-playfield mode itself, the condition
        // this whole package tests under (see header, Non-goals).
        dut->aga = 0;
        dut->pf2of = 0;
        dut->bpldata = 0;
        dut->dblpf = 0;
        dut->bplcon2 = 0;
        dut->eval();
    }
    ~Harness() { delete dut; }

    // Presents bpldata for one combinational evaluation (denise_playfields
    // has no clock -- see BOUNDARY) and returns plfdata. dblpf and bplcon2
    // are never varied in this package (see Non-goals) so sample() takes
    // only the one input this package's claims are about.
    uint8_t sample(uint8_t bpldata_v) {
        dut->bpldata = bpldata_v;
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
        uint8_t v1 = h.sample(5);   // 00101 -> expect register 5
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
            std::string label = "IDENTITY-MAPPING (p.93" +
                std::string(row.p94 ? "/94" : "") + "): bit combination " +
                std::to_string(row.n) + " selects colour register " +
                std::to_string(row.n) + ". " + std::string(row.note) +
                (row.note[0] ? " " : "") + row.ids;
            checks.expect(out == row.n, label);
        }
    }

    // =========================================================================
    // MEASURED -- print defects, recorded for traceability only (not
    // asserted; see header PRINT DEFECT #1/#2).
    // =========================================================================
    checks.note(
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
