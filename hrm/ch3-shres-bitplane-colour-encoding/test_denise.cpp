// ch3-shres-bitplane-colour-encoding -- denise conformance test
//
// Test purpose: the ECS SuperHires bitplane colour encoding printed on
//   p.301 -- the table that maps each of the sixteen lower colour
//   registers' bits onto symbols a-x, and the legend "Bitplane (Color 0) :
//   ab-- cd-- ef--" (and its siblings for colours 1-3) that names which
//   symbol is which colour component. Package ch3-shres-bitplane-colour-
//   encoding, fixture denise (one Verilator build, top module denise, per
//   PACKAGE.md). A sibling package covers the sprite half of the same
//   scheme on page 302.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 297, 301, 302 (claims.jsonl for ch3-shres-bitplane-colour-encoding,
//   301 records). A red result below means denise.v disagrees with the
//   manual, not that the assertion is wrong -- no assertion here was, or
//   should be, adjusted to make the RTL pass.
// Non-goals: AGA's separate SuperHires colour path (aga=1) is out of scope
//   -- every claim in this package's inventory quotes the ECS text (p.297,
//   301, 302), and aga is held at 0 throughout. ham and dpf are non-goal
//   defaults (0): no claim in this inventory combines SuperHires with
//   hold-and-modify or dual-playfield. bpu is held at 2 for every
//   SuperHires stimulus (the two bitplanes SuperHires needs to form a
//   2-bit source colour, p.301's own "certain multiplexing occurs" text)
//   -- more or fewer planes is not this package's subject.
//
// BOUNDARY (from PACKAGE.md's own evidence field, not from opening the
// RTL -- see ORDER OF WORK below): denise.v:31-52 module header
// (reg_address_in[8:1] l.39, data_in l.40, data_out l.42, strhor l.38,
// blank l.43, red/green/blue l.44-46, a1k l.47, ecs l.48, aga l.49,
// hires l.50, shres l.51) | denise.v:59 (BPLCON0 9'h100), write l.171-178,
// field decode l.180-192 (hires=bit15, bpu={bit4&aga,bits14-12}, ham=
// bit11, dpf=bit10, shres=bit6, ecsena=bit0), l_bpu latch l.194-203, plane
// gating l.367-374 (shres = bplcon0[6], l.187) | denise.v:65 (COLORBASE
// 9'h180) with denise_colortable.v:24,30-34 (write decode, wr_dat=
// data_in[11:0]) and the colour packing at denise_colortable.v:56-58.
// Register-bus addressing (reg_address_in = custom byte address >> 1, the
// port's own [8:1] width) and the BPLCON0/BPLxDAT/COLORxx addresses below
// are carried from this repository's own already-committed sibling
// harnesses for the SAME register bus on the SAME denise_* module family --
// hrm/denise/test_denise.cpp (0xC0 = COLOR00, 0xC1 = COLOR01, confirming
// COLORBASE 9'h180 -> reg_address_in 0xC0) and hrm/ch3-bplcon1-scroll-delay
// /test_denise_bitplanes.cpp (BPLCON0 9'h100 -> 0x80, BPL1DAT 9'h110 ->
// 0x88, BPL2DAT 9'h112 -> 0x89, and the proven "pre-load BPL2DAT, then
// BPL1DAT's own write triggers every plane's shifter load" write
// sequencing, denise_bitplanes.v:467) -- reused as infrastructure, not
// read as a claim of this package's own inventory, on the same basis the
// bplcon1-scroll-delay package's own ORDER OF WORK section already
// documents for reusing THOSE same two harnesses.
//
// THE C1/C3 TRAP (build-brief warning, not itself a claim): c3 is the
// registered 7MHz clock and c1 = ~c3 delayed one further 28MHz cycle, so
// across one 7MHz cycle's four 28MHz phases, c1 and c3 are BOTH high in one
// phase and BOTH low in another. denise_bitplanes.v:381 gates its BPLxDAT
// write strobe on c1&c3. Driving c1/c3 as disjoint two-phase signals (the
// pattern hrm/denise/test_denise.cpp itself uses) makes c1&c3 NEVER true,
// so no BPLxDAT write ever reaches a shifter and every plane reads colour 0
// silently -- catastrophic for an encoding test specifically, since it
// would report every colour as 0 and look like a real RTL finding. This
// Harness uses the measured-working four-phase pattern instead:
// clk7_en=(phase==0), c1=(phase==0||phase==1), c3=(phase==1||phase==2).
// FIXTURE-DATA-PATH-LIVE below exists specifically to prove this pattern
// actually delivers a loaded, non-zero bitplane value before any colour
// claim is asserted from it.
//
// THE ADDRESSING MECHANISM (a derivation from the table itself, not a
// claim -- see WHAT p.301'S OWN TABLE PROVES below; the one part of this
// file genuinely unresolved before opening the RTL): p.301's table gives
// the CONTENT of each of the sixteen registers but no claim in this
// package's 301-record inventory states HOW a bitplane pixel stream
// selects which of the sixteen registers a given SuperHires pixel reads --
// that is denise's own internal pixel-index formation, not a proposition
// printed in the manual. Algebra on the table alone (performed by the
// script that generated kRegRow below, before this file was written) shows
// the table is self-consistent with: register index = 4*colorODD +
// colorEVEN, where colorEVEN/colorODD are two source-colour values (0-3)
// and the register's own bits[11:8]/[7:4]/[3:0] (R/G/B) each split into a
// "bits[3:2]" half carrying colorEVEN's own letters and a "bits[1:0]" half
// carrying colorODD's own letters -- consistent with two adjacent
// SuperHires pixels sharing one colour-table fetch, each reading its own
// half. This explains the table's shape without assuming anything about
// the RTL, but WHICH physical tick is "EVEN" (i.e. which of two adjacent
// 28MHz-tick bitplane samples ends up read out through the bits[3:2] half)
// is a pipeline-timing fact this package's evidence does not pin down. The
// four diagonal registers (COLOR00/05/0A/0F, where colorEVEN==colorODD)
// are addressed by a stimulus that holds a single steady colour for many
// ticks -- immune to this ambiguity, since it does not matter which tick
// is "even" when both samples are identical. The twelve off-diagonal
// registers need the real pairing, predicted below as "this stimulus's own
// tick 0 is EVEN" and corrected, with the correction documented, once
// denise_bitplanes.v/denise_colortable.v are read (see RTL OPENED, filled
// in by the second commit).
//
// ORDER OF WORK: this file (assertions, the oracle table, expected values,
// claim-id coverage) was written and committed from claims.jsonl's
// quotations and PACKAGE.md's own evidence field, plus the two already-
// committed sibling harnesses named above, ONLY -- denise.v itself was not
// opened until after that commit, per the build brief's explicit gate. A
// second commit follows once the RTL is opened, the build actually run
// under WSL/Verilator, and the MUTATION LOG below filled in with real
// results.
//
// WHAT p.301'S OWN TABLE PROVES (self-consistency check performed on
// claims.jsonl's 256 C-shres-bpl-bits.cNNN restatements alone, by script,
// before writing kRegRow below): every one of the sixteen rows has bits
// 15-12 unused and bits 11-0 each carrying exactly one of the 24 symbols
// a-x; no two symbols conflict on which source colour/component they name
// across all 256 records; the four "diagonal" rows (00, 05, 0A, 0F) use
// only one source colour's six letters, duplicated in both nibble halves;
// the other twelve rows combine two different source colours' letters, one
// per half. This structure is what the ADDRESSING MECHANISM comment above
// is derived from, and what SHRES-TWO-BITS-RESOLUTION below tests
// directly: since a used bit only ever moves ONE nibble half at ONE tick,
// no single displayed pixel is ever governed by more than two of a
// nibble's four physical bits.
//
// DECLINED CLAIMS (3 ids, not observable at an RTL boundary):
//   C-shrc.c06 (p.302) and C-shres-color.c9 (p.302, same quote) --
//   "SuperHires color encryption is not reflected in the ColorTable."
//   This names a software utility ("the ColorTable", an Amiga OS/IFF
//   colour-table tool), not a hardware behaviour; denise.v has no such
//   concept at its boundary.
//   C-shrc.c08 (p.302) -- "pastel colors in a closely graduated color
//   scheme may be visually difficult to distinguish from each other."
//   A human-perception statement, not an RTL-observable property.
//
// CONTROL CASE: CONTROL-KNOWN-VALUES-ARRIVE (no claim id, no manual page)
// is the control every page-cited assertion below shares: the fixture is
// shown to deliver two distinct, correctly-positioned, known RGB values
// through denise's own register-write-to-pixel-output path before
// anything below trusts it -- the convention hrm/ch3-dual-playfield/'s and
// hrm/ch3-collision-registers/'s CONTROL-KNOWN-VALUES-ARRIVE both use.
// FIXTURE-DATA-PATH-LIVE (no claim id) is a second, harness-specific
// control proving the c1/c3 pattern itself is not silently dead (THE
// C1/C3 TRAP above). OCS-BASELINE-0RGB-FORMAT (C-shres-color.c4/c5/c6/c7)
// is the control the build brief specifically asks for ("a control case
// expected to PASS on the same measurement basis as any case under
// suspicion") -- the plain, non-SuperHires 0x0RGB format, through the
// identical write-register/read-RGB path used for every SuperHires
// measurement below, so a SuperHires red result cannot be blamed on the
// measurement method itself.
//
// CLAIM COVERAGE: all 301 records of claims.jsonl are accounted for below
// -- 298 inside an expect() assertion string (ids written out in full),
// 3 declined (above, with reasons). Ids restating the identical
// proposition, or the same table row from a different angle, are grouped
// and cited together on the single assertion (or loop) that tests that
// proposition -- the convention hrm/ch3-collision-registers/ and
// hrm/ch3-ham-control-field/ both document and use:
//
//   CONTROL-KNOWN-VALUES-ARRIVE (0 ids, no page)
//   FIXTURE-DATA-PATH-LIVE (0 ids, no page)
//   OCS-BASELINE-0RGB-FORMAT (4 ids)              -- C-shres-color.c4-c7
//   SHRES-REINTERPRETS-SAME-REGISTERS (2 ids)      -- C-shr.c06,
//                                                      C-superhires-color.c1
//   SHRES-BITPLANE-TABLE-COLOR-00 .. -0F (16 assertions, 288 ids total):
//     each cites its own row id (C-shrc.rXX) plus its own sixteen
//     C-shres-bpl-bits.cNNN bit ids (272 ids across the sixteen); COLOR-00,
//     -05, -0A and -0F (the four diagonal, single-source-colour rows)
//     additionally cite that source colour's own legend row and its three
//     symbol ids (C-shrc.srcbN + C-shres-symbols.cNN, 16 ids total).
//     COLOR-01's own assertion additionally cites SHRES-TWO-BITS-
//     RESOLUTION's two ids (C-shrc.c01, c02), since COLOR-01's own
//     tightened per-bit isolation check (bits 11-10 move only the EVEN
//     tick, bits 9-8 move only the ODD tick, never both) is exactly what
//     those two ids assert.
//   SHRES-USES-SIXTEEN-LOWER-REGISTERS (2 ids)     -- C-shrc.c04, c05,
//     asserted as an aggregate over the sixteen sweeps' own pass/fail.
//   Tally: 4 (OCS-BASELINE) + 2 (REINTERPRETS) + 288 (the sixteen table
//   sweeps, rows+bits+legend) + 2 (SIXTEEN-LOWER-REGISTERS) + 2 (c01/c02,
//   cited on COLOR-01's own label) = 298 distinct ids. Declined: 3.
//   298 + 3 = 301.
//
// RESULTS: to be filled in once the RTL is opened and the build run.
//
// MUTATION LOG: to be filled in once the RTL is opened, the build actually
// run under WSL/Verilator, and mutation testing actually performed --
// per this repository's own convention, none of that is invented here.

#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

#include <verilated.h>
#include "Vdenise.h"

namespace {

// ---------------------------------------------------------------------
// Oracle: the ECS SuperHires bitplane colour encoding table, p.301, and
// its legend. Derived mechanically from claims.jsonl's own restatement
// text (256 C-shres-bpl-bits.cNNN records), independently cross-checked
// against the sixteen C-shrc.rXX row quotations -- not from the RTL.
// ---------------------------------------------------------------------

// kRegRow[reg], reg 0x0-0xF: sixteen characters, index 0 = bit15 down to
// index 15 = bit0. '.' = unused (per the manual's own printed "."); any
// other character is the symbol (a-x) that bit carries -- not itself used
// numerically below (see WHAT p.301'S OWN TABLE PROVES: the used/unused
// pattern and the tick/component split are both uniform across all
// sixteen rows), kept here for documentation and for cross-checking the
// used/unused mask kRegUsed below was derived correctly.
static const char* const kRegRow[16] = {
    "....ababcdcdefef", // COLOR00 (C-shrc.r00)
    "....ghabijcdklef", // COLOR01 (C-shrc.r01)
    "....mnabopcdqref", // COLOR02 (C-shrc.r02)
    "....stabuvcdwxef", // COLOR03 (C-shrc.r03)
    "....abghcdijefkl", // COLOR04 (C-shrc.r04)
    "....ghghijijklkl", // COLOR05 (C-shrc.r05)
    "....mnghopijqrkl", // COLOR06 (C-shrc.r06)
    "....stghuvijwxkl", // COLOR07 (C-shrc.r07)
    "....abmncdopefqr", // COLOR08 (C-shrc.r08)
    "....ghmnijopklqr", // COLOR09 (C-shrc.r09)
    "....mnmnopopqrqr", // COLOR0A (C-shrc.r0A)
    "....stmnuvopwxqr", // COLOR0B (C-shrc.r0B)
    "....abstcduvefwx", // COLOR0C (C-shrc.r0C)
    "....ghstijuvklwx", // COLOR0D (C-shrc.r0D)
    "....mnstopuvqrwx", // COLOR0E (C-shrc.r0E)
    "....ststuvuvwxwx", // COLOR0F (C-shrc.r0F)
};

static const char* const kRowIds[16] = {
    "C-shrc.r00", "C-shrc.r01", "C-shrc.r02", "C-shrc.r03",
    "C-shrc.r04", "C-shrc.r05", "C-shrc.r06", "C-shrc.r07",
    "C-shrc.r08", "C-shrc.r09", "C-shrc.r0A", "C-shrc.r0B",
    "C-shrc.r0C", "C-shrc.r0D", "C-shrc.r0E", "C-shrc.r0F",
};

// Per-register bit-claim id lists, bit 15 down to bit 0, full ids --
// generated from claims.jsonl's own restatement text (regex-parsed
// "COLORxx bit N (is unused|carries symbol ...)"), not hand-transcribed.
static const char* const kBitIds[16][16] = {
    { "C-shres-bpl-bits.c1", "C-shres-bpl-bits.c2", "C-shres-bpl-bits.c3", "C-shres-bpl-bits.c4", "C-shres-bpl-bits.c5", "C-shres-bpl-bits.c6", "C-shres-bpl-bits.c7", "C-shres-bpl-bits.c8", "C-shres-bpl-bits.c9", "C-shres-bpl-bits.c10", "C-shres-bpl-bits.c11", "C-shres-bpl-bits.c12", "C-shres-bpl-bits.c13", "C-shres-bpl-bits.c14", "C-shres-bpl-bits.c15", "C-shres-bpl-bits.c16" }, // COLOR00
    { "C-shres-bpl-bits.c17", "C-shres-bpl-bits.c18", "C-shres-bpl-bits.c19", "C-shres-bpl-bits.c20", "C-shres-bpl-bits.c21", "C-shres-bpl-bits.c22", "C-shres-bpl-bits.c23", "C-shres-bpl-bits.c24", "C-shres-bpl-bits.c25", "C-shres-bpl-bits.c26", "C-shres-bpl-bits.c27", "C-shres-bpl-bits.c28", "C-shres-bpl-bits.c29", "C-shres-bpl-bits.c30", "C-shres-bpl-bits.c31", "C-shres-bpl-bits.c32" }, // COLOR01
    { "C-shres-bpl-bits.c33", "C-shres-bpl-bits.c34", "C-shres-bpl-bits.c35", "C-shres-bpl-bits.c36", "C-shres-bpl-bits.c37", "C-shres-bpl-bits.c38", "C-shres-bpl-bits.c39", "C-shres-bpl-bits.c40", "C-shres-bpl-bits.c41", "C-shres-bpl-bits.c42", "C-shres-bpl-bits.c43", "C-shres-bpl-bits.c44", "C-shres-bpl-bits.c45", "C-shres-bpl-bits.c46", "C-shres-bpl-bits.c47", "C-shres-bpl-bits.c48" }, // COLOR02
    { "C-shres-bpl-bits.c49", "C-shres-bpl-bits.c50", "C-shres-bpl-bits.c51", "C-shres-bpl-bits.c52", "C-shres-bpl-bits.c53", "C-shres-bpl-bits.c54", "C-shres-bpl-bits.c55", "C-shres-bpl-bits.c56", "C-shres-bpl-bits.c57", "C-shres-bpl-bits.c58", "C-shres-bpl-bits.c59", "C-shres-bpl-bits.c60", "C-shres-bpl-bits.c61", "C-shres-bpl-bits.c62", "C-shres-bpl-bits.c63", "C-shres-bpl-bits.c64" }, // COLOR03
    { "C-shres-bpl-bits.c129", "C-shres-bpl-bits.c130", "C-shres-bpl-bits.c131", "C-shres-bpl-bits.c132", "C-shres-bpl-bits.c133", "C-shres-bpl-bits.c134", "C-shres-bpl-bits.c135", "C-shres-bpl-bits.c136", "C-shres-bpl-bits.c137", "C-shres-bpl-bits.c138", "C-shres-bpl-bits.c139", "C-shres-bpl-bits.c140", "C-shres-bpl-bits.c141", "C-shres-bpl-bits.c142", "C-shres-bpl-bits.c143", "C-shres-bpl-bits.c144" }, // COLOR04
    { "C-shres-bpl-bits.c145", "C-shres-bpl-bits.c146", "C-shres-bpl-bits.c147", "C-shres-bpl-bits.c148", "C-shres-bpl-bits.c149", "C-shres-bpl-bits.c150", "C-shres-bpl-bits.c151", "C-shres-bpl-bits.c152", "C-shres-bpl-bits.c153", "C-shres-bpl-bits.c154", "C-shres-bpl-bits.c155", "C-shres-bpl-bits.c156", "C-shres-bpl-bits.c157", "C-shres-bpl-bits.c158", "C-shres-bpl-bits.c159", "C-shres-bpl-bits.c160" }, // COLOR05
    { "C-shres-bpl-bits.c161", "C-shres-bpl-bits.c162", "C-shres-bpl-bits.c163", "C-shres-bpl-bits.c164", "C-shres-bpl-bits.c165", "C-shres-bpl-bits.c166", "C-shres-bpl-bits.c167", "C-shres-bpl-bits.c168", "C-shres-bpl-bits.c169", "C-shres-bpl-bits.c170", "C-shres-bpl-bits.c171", "C-shres-bpl-bits.c172", "C-shres-bpl-bits.c173", "C-shres-bpl-bits.c174", "C-shres-bpl-bits.c175", "C-shres-bpl-bits.c176" }, // COLOR06
    { "C-shres-bpl-bits.c177", "C-shres-bpl-bits.c178", "C-shres-bpl-bits.c179", "C-shres-bpl-bits.c180", "C-shres-bpl-bits.c181", "C-shres-bpl-bits.c182", "C-shres-bpl-bits.c183", "C-shres-bpl-bits.c184", "C-shres-bpl-bits.c185", "C-shres-bpl-bits.c186", "C-shres-bpl-bits.c187", "C-shres-bpl-bits.c188", "C-shres-bpl-bits.c189", "C-shres-bpl-bits.c190", "C-shres-bpl-bits.c191", "C-shres-bpl-bits.c192" }, // COLOR07
    { "C-shres-bpl-bits.c65", "C-shres-bpl-bits.c66", "C-shres-bpl-bits.c67", "C-shres-bpl-bits.c68", "C-shres-bpl-bits.c69", "C-shres-bpl-bits.c70", "C-shres-bpl-bits.c71", "C-shres-bpl-bits.c72", "C-shres-bpl-bits.c73", "C-shres-bpl-bits.c74", "C-shres-bpl-bits.c75", "C-shres-bpl-bits.c76", "C-shres-bpl-bits.c77", "C-shres-bpl-bits.c78", "C-shres-bpl-bits.c79", "C-shres-bpl-bits.c80" }, // COLOR08
    { "C-shres-bpl-bits.c193", "C-shres-bpl-bits.c194", "C-shres-bpl-bits.c195", "C-shres-bpl-bits.c196", "C-shres-bpl-bits.c197", "C-shres-bpl-bits.c198", "C-shres-bpl-bits.c199", "C-shres-bpl-bits.c200", "C-shres-bpl-bits.c201", "C-shres-bpl-bits.c202", "C-shres-bpl-bits.c203", "C-shres-bpl-bits.c204", "C-shres-bpl-bits.c205", "C-shres-bpl-bits.c206", "C-shres-bpl-bits.c207", "C-shres-bpl-bits.c208" }, // COLOR09
    { "C-shres-bpl-bits.c81", "C-shres-bpl-bits.c82", "C-shres-bpl-bits.c83", "C-shres-bpl-bits.c84", "C-shres-bpl-bits.c85", "C-shres-bpl-bits.c86", "C-shres-bpl-bits.c87", "C-shres-bpl-bits.c88", "C-shres-bpl-bits.c89", "C-shres-bpl-bits.c90", "C-shres-bpl-bits.c91", "C-shres-bpl-bits.c92", "C-shres-bpl-bits.c93", "C-shres-bpl-bits.c94", "C-shres-bpl-bits.c95", "C-shres-bpl-bits.c96" }, // COLOR0A
    { "C-shres-bpl-bits.c97", "C-shres-bpl-bits.c98", "C-shres-bpl-bits.c99", "C-shres-bpl-bits.c100", "C-shres-bpl-bits.c101", "C-shres-bpl-bits.c102", "C-shres-bpl-bits.c103", "C-shres-bpl-bits.c104", "C-shres-bpl-bits.c105", "C-shres-bpl-bits.c106", "C-shres-bpl-bits.c107", "C-shres-bpl-bits.c108", "C-shres-bpl-bits.c109", "C-shres-bpl-bits.c110", "C-shres-bpl-bits.c111", "C-shres-bpl-bits.c112" }, // COLOR0B
    { "C-shres-bpl-bits.c209", "C-shres-bpl-bits.c210", "C-shres-bpl-bits.c211", "C-shres-bpl-bits.c212", "C-shres-bpl-bits.c213", "C-shres-bpl-bits.c214", "C-shres-bpl-bits.c215", "C-shres-bpl-bits.c216", "C-shres-bpl-bits.c217", "C-shres-bpl-bits.c218", "C-shres-bpl-bits.c219", "C-shres-bpl-bits.c220", "C-shres-bpl-bits.c221", "C-shres-bpl-bits.c222", "C-shres-bpl-bits.c223", "C-shres-bpl-bits.c224" }, // COLOR0C
    { "C-shres-bpl-bits.c225", "C-shres-bpl-bits.c226", "C-shres-bpl-bits.c227", "C-shres-bpl-bits.c228", "C-shres-bpl-bits.c229", "C-shres-bpl-bits.c230", "C-shres-bpl-bits.c231", "C-shres-bpl-bits.c232", "C-shres-bpl-bits.c233", "C-shres-bpl-bits.c234", "C-shres-bpl-bits.c235", "C-shres-bpl-bits.c236", "C-shres-bpl-bits.c237", "C-shres-bpl-bits.c238", "C-shres-bpl-bits.c239", "C-shres-bpl-bits.c240" }, // COLOR0D
    { "C-shres-bpl-bits.c113", "C-shres-bpl-bits.c114", "C-shres-bpl-bits.c115", "C-shres-bpl-bits.c116", "C-shres-bpl-bits.c117", "C-shres-bpl-bits.c118", "C-shres-bpl-bits.c119", "C-shres-bpl-bits.c120", "C-shres-bpl-bits.c121", "C-shres-bpl-bits.c122", "C-shres-bpl-bits.c123", "C-shres-bpl-bits.c124", "C-shres-bpl-bits.c125", "C-shres-bpl-bits.c126", "C-shres-bpl-bits.c127", "C-shres-bpl-bits.c128" }, // COLOR0E
    { "C-shres-bpl-bits.c241", "C-shres-bpl-bits.c242", "C-shres-bpl-bits.c243", "C-shres-bpl-bits.c244", "C-shres-bpl-bits.c245", "C-shres-bpl-bits.c246", "C-shres-bpl-bits.c247", "C-shres-bpl-bits.c248", "C-shres-bpl-bits.c249", "C-shres-bpl-bits.c250", "C-shres-bpl-bits.c251", "C-shres-bpl-bits.c252", "C-shres-bpl-bits.c253", "C-shres-bpl-bits.c254", "C-shres-bpl-bits.c255", "C-shres-bpl-bits.c256" }, // COLOR0F
};

// The four diagonal (single-source-colour) registers additionally cite
// their source colour's own legend row (p.301, "Bitplane (Color N) :
// ..") and its three symbol ids.
struct LegendCite { int reg; const char* srcb; const char* sym[3]; };
static const LegendCite kLegend[4] = {
    { 0x0, "C-shrc.srcb0", { "C-shres-symbols.c1", "C-shres-symbols.c2", "C-shres-symbols.c3" } },
    { 0x5, "C-shrc.srcb1", { "C-shres-symbols.c4", "C-shres-symbols.c5", "C-shres-symbols.c6" } },
    { 0xA, "C-shrc.srcb2", { "C-shres-symbols.c7", "C-shres-symbols.c8", "C-shres-symbols.c9" } },
    { 0xF, "C-shrc.srcb3", { "C-shres-symbols.c10", "C-shres-symbols.c11", "C-shres-symbols.c12" } },
};

// ---------------------------------------------------------------------
// Register bus addressing (reg_address_in = custom byte address >> 1) --
// reused from already-committed sibling harnesses, see BOUNDARY above.
// ---------------------------------------------------------------------
constexpr uint8_t kAddrBplcon0 = 0x80; // 9'h100
constexpr uint8_t kAddrColor0 = 0xC0;  // 9'h180, COLORxx = kAddrColor0 + xx
constexpr uint8_t kAddrBpl1Dat = 0x88; // 9'h110
constexpr uint8_t kAddrBpl2Dat = 0x89; // 9'h112

// BPLCON0 field positions (denise.v field decode, l.180-192, per evidence).
constexpr uint16_t kBplcon0Hires = 0x8000;  // bit15
constexpr uint16_t kBplcon0Bpu2 = 0x2000;   // bits14-12 = 2 (two bitplanes)
constexpr uint16_t kBplcon0Shres = 0x0040;  // bit6
constexpr uint16_t kBplcon0Ecsena = 0x0001; // bit0

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

std::string hex3(uint32_t v) {
    std::ostringstream os;
    os << "0x" << std::hex << std::uppercase << (v & 0xFFFu);
    return os.str();
}

class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;
        dut->aga = 0;
        dut->hires = 0; // top-level pin, distinct from BPLCON0's own hires
                        // field -- see BOUNDARY; predicted don't-care,
                        // confirmed/corrected once the RTL is open.
        dut->shres = 0; // likewise
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One 28MHz edge, then advance the four-phase c1/c3/clk7_en
    // relationship for the next edge -- see THE C1/C3 TRAP above.
    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) % 4;
        dut->clk7_en = (phase == 0);
        dut->c1 = (phase == 0 || phase == 1);
        dut->c3 = (phase == 1 || phase == 2);
    }
    void tick(int n) { for (int i = 0; i < n; ++i) tick(); }

    void begin_write_reg(uint16_t word_addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = word_addr;
        dut->data_in = data;
        tick();
    }
    void clear_reg() {
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }
    void write_reg(uint16_t word_addr, uint16_t data) {
        begin_write_reg(word_addr, data);
        clear_reg();
        tick(4); // settle margin, matching the already-committed sibling's
                 // own lesson (a bare write immediately followed by a
                 // timed trigger measured stale state on first build).
    }

    void write_color(int idx, uint16_t val12) {
        write_reg(static_cast<uint16_t>(kAddrColor0 + idx), val12 & 0x0FFF);
    }
    void enable_shres_two_planes() {
        write_reg(kAddrBplcon0,
                   kBplcon0Hires | kBplcon0Bpu2 | kBplcon0Shres | kBplcon0Ecsena);
        tick(32);
    }
    void disable_shres(int bpu) {
        write_reg(kAddrBplcon0, static_cast<uint16_t>((bpu & 7) << 12));
        tick(32);
    }

    // Loads BPL1DAT/BPL2DAT with the given 16-bit shift patterns. Per the
    // already-committed sibling's own evidence, BPL2DAT must be pre-loaded
    // (silently) BEFORE the BPL1DAT write, since a BPL1DAT write's load
    // strobe reaches every plane's shifter at once.
    void load_planes(uint16_t bpl1, uint16_t bpl2) {
        write_reg(kAddrBpl2Dat, bpl2);
        write_reg(kAddrBpl1Dat, bpl1);
    }

    // Steady stimulus: holds a single 2-bit source colour (0-3, bit0 from
    // plane 1, bit1 from plane 2 -- p.271's own bpldata[8:1] plane-N-is-
    // bit-(N-1) packing, reused from the already-committed sibling) for a
    // full 16-tick shift-out window, giving addressing-timing-agnostic
    // settled output -- see ADDRESSING MECHANISM above: it does not matter
    // which tick is "even" or "odd" when both samples are the same colour.
    void hold_colour(int colour) {
        uint16_t p1 = (colour & 1) ? 0xFFFF : 0x0000;
        uint16_t p2 = (colour & 2) ? 0xFFFF : 0x0000;
        load_planes(p1, p2);
        tick(24); // clear any startup transient before sampling.
    }

    // Alternating stimulus: colourEven at bitplane-sample ticks 0,2,4,...
    // and colourOdd at ticks 1,3,5,... within this load's own 16-tick
    // shift-out window (period 2, so MSB/LSB shift order does not matter
    // -- see ADDRESSING MECHANISM above). PREDICTED to align "this load's
    // tick 0" with the RTL's own even/odd colour-table read-out pairing;
    // to be corrected once denise_bitplanes.v/denise_colortable.v are
    // read.
    void hold_alternating(int colourEven, int colourOdd) {
        uint16_t p1 = 0, p2 = 0;
        for (int b = 0; b < 16; ++b) {
            int c = (b % 2 == 0) ? colourEven : colourOdd;
            if (c & 1) p1 |= (1u << (15 - b));
            if (c & 2) p2 |= (1u << (15 - b));
        }
        load_planes(p1, p2);
        tick(24);
    }

    uint32_t rgb() const {
        return (static_cast<uint32_t>(dut->red) << 8) |
               (static_cast<uint32_t>(dut->green) << 4) |
               static_cast<uint32_t>(dut->blue);
    }

    Vdenise* dut;
    int phase = 0;
};

std::string join(std::initializer_list<const char*> ids) {
    std::ostringstream os;
    bool first = true;
    for (auto id : ids) {
        if (!first) os << ", ";
        os << id;
        first = false;
    }
    return os.str();
}
std::string join16(const char* const* ids) {
    std::ostringstream os;
    for (int i = 0; i < 16; ++i) {
        if (i) os << ", ";
        os << ids[i];
    }
    return os.str();
}
int comp_of(uint32_t v, int c) { // c: 0=R,1=G,2=B
    return (v >> (4 * (2 - c))) & 0xF;
}
std::string hexReg(int reg) {
    std::ostringstream os;
    os << std::hex << std::uppercase;
    if (reg < 16) os.width(2), os.fill('0');
    os << reg;
    return os.str();
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // CONTROL -- the fixture can deliver a known value before anything
    // below is trusted to read one back.
    // =====================================================================
    {
        Harness h;
        h.disable_shres(1);
        h.write_color(0, 0x0F00); // COLOR00 = pure red nibble
        h.write_color(1, 0x00F0); // COLOR01 = pure green nibble
        uint32_t v0 = h.rgb(); // colour index 0 (no plane driven high)
        h.load_planes(0xFFFF, 0x0000);
        h.tick(24);
        uint32_t v1 = h.rgb(); // colour index 1 (plane 1 driven high)
        checks.expect(v0 != v1 && v0 == 0xF00 && v1 == 0x0F0,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct, known "
            "colour-register writes (COLOR00=0x0F00, COLOR01=0x00F0) "
            "produce two distinct, correctly-positioned RGB readings "
            "(0xF00 then 0x0F0) at denise's own red/green/blue pins as "
            "the displayed colour index changes, through the fixture's "
            "own register-write-to-pixel-output path, before any "
            "assertion below trusts it. No manual page: the control for "
            "every assertion in this file, on the same measurement basis.");
    }

    // =====================================================================
    // FIXTURE-DATA-PATH-LIVE (no claim id) -- proves the measured-working
    // c1/c3 four-phase pattern actually delivers a loaded, non-zero
    // bitplane sample, not silently-dead colour 0 (THE C1/C3 TRAP above).
    // =====================================================================
    {
        Harness h;
        h.disable_shres(1);
        h.write_color(0, 0x0000);
        h.write_color(1, 0x0FFF); // COLOR01 = white, unmistakable if reached
        uint32_t baseline = h.rgb();
        h.load_planes(0xFFFF, 0x0000); // plane 1 all-1s -> colour index 1
        h.tick(24);
        uint32_t driven = h.rgb();
        checks.expect(baseline == 0 && driven == 0xFFF,
            "[control] FIXTURE-DATA-PATH-LIVE: a BPL1DAT write of an "
            "all-ones plane reaches the bitplane shifter and is displayed "
            "as COLOR01 (0xFFF) within 24 ticks of the four-phase "
            "clk7_en=(phase==0), c1=(phase==0||phase==1), "
            "c3=(phase==1||phase==2) pattern -- the write-strobe and "
            "shifter-load gating this module uses (c1&c3, !c1&&!c3) is "
            "never permanently starved the way a disjoint two-phase "
            "pattern would starve it. No manual page: harness self-check.");
    }

    // =====================================================================
    // OCS-BASELINE-0RGB-FORMAT (p.301) -- the control the build brief asks
    // for: the plain, non-SuperHires 0x0RGB format, on the same write/read
    // basis as every SuperHires measurement below.
    // =====================================================================
    {
        Harness h;
        h.disable_shres(1);
        h.write_color(0, 0x0000);
        h.write_color(1, 0x0A5C); // R=0xA, G=0x5, B=0xC -- distinguishable nibbles
        h.load_planes(0xFFFF, 0x0000);
        h.tick(24);
        uint32_t v = h.rgb();
        checks.expect(v == 0xA5C,
            "OCS-BASELINE-0RGB-FORMAT (p.301, " +
            join({"C-shres-color.c4", "C-shres-color.c5", "C-shres-color.c6",
                  "C-shres-color.c7"}) +
            "): with shres=0, COLOR01 written as 0x0A5C reproduces the "
            "plain 4-bit-per-component 0x0RGB format directly on denise's "
            "red/green/blue pins (measured " + hex3(v) + ", expected 0xA5C) "
            "-- no SuperHires multiplexing. 'Instead of 4 bits of red, "
            "green, and blue for bitplane registers 0-3 stored as 0x0RGB "
            "in four color registers, SuperHires bitplane colors are "
            "specially encoded...' read as describing what happens "
            "WITHOUT SuperHires active. Control: same write-register/"
            "read-RGB measurement basis as every SuperHires assertion "
            "below.");
    }

    // =====================================================================
    // SHRES-REINTERPRETS-SAME-REGISTERS (p.297) -- the same register
    // contents, same colour index, read differently once shres=1.
    // =====================================================================
    {
        Harness h;
        h.disable_shres(1);
        h.write_color(0, 0x0000);
        h.write_color(1, 0x0FFF);
        h.load_planes(0xFFFF, 0x0000);
        h.tick(24);
        uint32_t ocs = h.rgb();
        Harness h2;
        h2.write_color(0, 0x0000);
        h2.write_color(1, 0x0FFF);
        h2.enable_shres_two_planes();
        h2.hold_colour(1);
        uint32_t shres = h2.rgb();
        checks.expect(ocs != shres,
            "SHRES-REINTERPRETS-SAME-REGISTERS (p.297, " +
            join({"C-shr.c06", "C-superhires-color.c1"}) +
            "): identical COLOR00/COLOR01 register contents (0x000, "
            "0xFFF) are displayed differently depending on shres alone -- "
            "OCS/plain reading " + hex3(ocs) + " vs SuperHires reading " +
            hex3(shres) + " -- 'There are new color register "
            "interpretations as part of the SuperHires mode.'");
    }

    // =====================================================================
    // SHRES-BITPLANE-TABLE-COLOR-00 .. -0F -- the main sweep. For each of
    // the sixteen registers: walking-bit stimulus (one bit at a time) into
    // COLORxx, read back through denise's own pixel-output pins at the
    // register's predicted addressing (steady colour for the four
    // diagonal registers; the alternating colourEven/colourOdd pairing,
    // predicted per ADDRESSING MECHANISM above, for the other twelve).
    // Tightened check per bit: a "used" bit (11-0) must move exactly its
    // own component (R=11-8, G=7-4, B=3-0) at exactly its own tick
    // (bits-3-2-of-nibble -> EVEN, bits-1-0-of-nibble -> ODD) and leave
    // every other component/tick combination unchanged; an "unused" bit
    // (15-12) must leave every component/tick combination unchanged. This
    // isolation check is also SHRES-TWO-BITS-RESOLUTION's own evidence
    // (C-shrc.c01, c02): if any nibble's low half leaked into the even
    // tick's own reading or vice versa, more than two of that nibble's
    // four bits would be independently significant per displayed pixel.
    // =====================================================================
    std::array<bool, 16> regOk{};
    for (int reg = 0; reg < 16; ++reg) {
        int colourEven = reg & 3;
        int colourOdd = (reg >> 2) & 3;
        bool diagonal = (colourEven == colourOdd);

        auto sample_reg = [&](Harness& h) -> std::pair<uint32_t, uint32_t> {
            h.enable_shres_two_planes();
            if (diagonal) {
                h.hold_colour(colourEven);
                uint32_t v = h.rgb();
                return {v, v};
            } else {
                h.hold_alternating(colourEven, colourOdd);
                uint32_t even_v = h.rgb();
                h.tick();
                uint32_t odd_v = h.rgb();
                return {even_v, odd_v};
            }
        };

        Harness hb;
        hb.write_color(reg, 0x0000);
        auto base = sample_reg(hb);

        bool all_ok = true;
        for (int bit = 15; bit >= 0; --bit) {
            Harness h;
            h.write_color(reg, static_cast<uint16_t>(1u << bit));
            auto v = sample_reg(h);

            bool used = (bit < 12);
            int comp = used ? (2 - bit / 4) : -1;      // 0=R,1=G,2=B
            bool expectEven = used ? ((bit % 4) >= 2) : false;

            bool ok;
            if (!used) {
                ok = (v.first == base.first) && (v.second == base.second);
            } else {
                uint32_t moving = expectEven ? v.first : v.second;
                uint32_t moving_base = expectEven ? base.first : base.second;
                uint32_t still = expectEven ? v.second : v.first;
                uint32_t still_base = expectEven ? base.second : base.first;
                bool moved_correctly = comp_of(moving, comp) != comp_of(moving_base, comp);
                bool no_other_component_moved =
                    true; // moving's OTHER two components must not change
                for (int c = 0; c < 3; ++c) {
                    if (c == comp) continue;
                    if (comp_of(moving, c) != comp_of(moving_base, c)) no_other_component_moved = false;
                }
                bool other_tick_unchanged = (still == still_base);
                ok = moved_correctly && no_other_component_moved && other_tick_unchanged;
            }
            if (!ok) all_ok = false;
        }
        regOk[reg] = all_ok;

        std::string label = "SHRES-BITPLANE-TABLE-COLOR-" + hexReg(reg) +
            " (p.301, " + std::string(kRowIds[reg]) + ", " + join16(kBitIds[reg]) + ")";
        for (auto& lc : kLegend) {
            if (lc.reg == reg) {
                label += ", legend " + std::string(lc.srcb) + ", " +
                         join({lc.sym[0], lc.sym[1], lc.sym[2]});
            }
        }
        if (reg == 1) {
            label += ", " + join({"C-shrc.c01", "C-shrc.c02"});
        }
        label += ": walking-bit sweep of COLOR" + hexReg(reg) +
                 " -- each of the sixteen bits independently moves exactly "
                 "its own component at exactly its own tick (bits 11-0, "
                 "per the table's own letter placement) or moves nothing "
                 "at all (bits 15-12, printed '.'), matching the table "
                 "exactly and never leaking into the other tick or the "
                 "other two components.";
        if (reg == 1) {
            label += " The same isolation is 'only two bits of red, "
                     "green and blue color resolution per hires pixel' "
                     "made concrete: no more than two of this nibble's "
                     "four physical bits govern any one displayed pixel.";
        }
        checks.expect(all_ok, label);
    }

    // =====================================================================
    // SHRES-USES-SIXTEEN-LOWER-REGISTERS (p.301) -- aggregate over the
    // sixteen sweeps' own pass/fail: all sixteen registers are
    // independently reachable and distinct SuperHires colour storage, not
    // just bitplane-registers-0-3's plain 0x0RGB slots.
    // =====================================================================
    {
        bool all16 = true;
        for (bool ok : regOk) all16 = all16 && ok;
        checks.expect(all16,
            "SHRES-USES-SIXTEEN-LOWER-REGISTERS (p.301, " +
            join({"C-shrc.c04", "C-shrc.c05"}) +
            "): the sixteen per-register sweeps above (SHRES-BITPLANE-"
            "TABLE-COLOR-00 through -0F) each independently confirmed "
            "their own register's bits reach denise's output -- i.e. all "
            "sixteen of COLOR00-COLOR0F are live SuperHires colour "
            "storage. 'SuperHires bitplane colors are specially encoded "
            "in the sixteen lower color registers.'");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
