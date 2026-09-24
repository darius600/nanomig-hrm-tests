// Test purpose: the sprite registers Denise decodes -- SPRxPOS, SPRxCTL,
//   SPRxDATA, SPRxDATB, for all eight sprites (0-7) -- are write-only: a
//   read aimed at the register's own address does not return what was
//   written there. This package is the Denise half of that single
//   proposition, stated once generically on page 284 (Appendix A) and once
//   per sprite on pages 291/292 (Appendix B). A sibling package,
//   hrm/ch4-sprite-write-only-agnus/, is the Agnus half of the
//   same row (SPRxPTH/SPRxPTL, the sprite pointer registers) -- see the
//   claims' own "split" field, quoted below.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition. Page 284
//   (Appendix A, one representative row per register, generically named
//   "sprite x" but printed with sprite 0's own address) and pages 291-292
//   (Appendix B, one row per sprite 0-7). Every claim below cites one or
//   both of those pages; none of this package's claims cite any other page.
// Non-goal: the register's *address* and *function* (what the position,
//   control and data bits mean) -- classify.split on every one of this
//   package's 72 claims says so explicitly: "Only the write-only half
//   belongs here", the address and function halves belonging to the decode
//   packages (ch4-sprite-vertical-position / ch4-sprite-horizontal-and-
//   arming / ch4-sprite-pointer-registers). This package answers one
//   question per register: does a read aimed at its own address come back
//   as what was just written there.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
// Claim ids: ids such as `APPA-sprpos.c1` and `CH4-appA-sprite.c6` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch4.json lists under `inventories`.
//
// BOUNDARY: `denise` (top-level chipset module), the only fixture every one
//   of this package's 72 claims classifies as constructible. Per every
//   claim's own classify.evidence: "port list rtl/denise_sprites.v:32-64 --
//   the module's only outputs are nsprite and sprdata; there is no
//   data_out" -- so the narrower denise_sprites fixture cannot answer this
//   question at all; every claim's own classify.observable says so in as
//   many words: "needs a wider boundary: denise -- denise_sprites has no
//   data_out port ...; the read path is denise.v:97". This package's
//   Makefile is copied from hrm/ch3-register-map-denise/'s Makefile (a
//   package that already builds this exact `denise` top level, with the
//   full 12-file submodule list including denise_bitplanes.v and
//   bitplane_ram.v) with only CPP_TEST changed, for exactly that reason --
//   an older Makefile in this tree (hrm/denise/Makefile) omits three of
//   those twelve files and does not elaborate.
//
// PORTS (from the claims' own evidence fields, and from
//   hrm/ch3-register-map-denise/test_denise.cpp -- an already-committed
//   package building this SAME `denise` fixture, whose own header states
//   its port list was itself "PACKAGE.md's own evidence", i.e. handed to
//   that building agent the same way this package's claims hand ports to
//   this one -- NOT discovered by this agent opening denise.v):
//   in  - clk, clk7_en, c1, c3, cck, reset, strhor, reg_address_in[8:1],
//         data_in[15:0], chip48, blank, a1k, ecs, aga
//   out - data_out[15:0] (denise.v:97 sources it from
//         col_out|deniseid_out|rgb_out only -- every claim's own
//         classify.evidence says this in those words -- so no sprite
//         address is ever a driver of it), red/green/blue, hires, shres
//   reg_address_in carries the manual's byte address already shifted right
//   by one (hrm/ch3-register-map-denise/test_denise.cpp's own comment:
//   "every constant below is the manual's byte address already shifted
//   right by one, matching denise.v's own parameter block"). Applying that
//   shift to this package's own claims' quoted byte addresses (all of
//   $140-$17E) gives the word addresses used below -- SPR_BASE = $140>>1 =
//   $A0, and for sprite n (0-7): POS = $A0+4n, CTL = $A0+4n+1,
//   DATA = $A0+4n+2, DATB = $A0+4n+3 (n's 8-byte spacing in the manual's
//   own addresses, $148-$140=8 etc., is 4 word-addresses). This arithmetic
//   is checked against every one of this package's own 72 claims' quoted
//   addresses, not assumed.
//
// THE FIXTURE-INTEGRITY CONTROL, and why it cannot be "write a known word
//   and read the same address back": every proposition this package tests
//   is exactly that no address it names ever reflects what was written to
//   it -- so the control cannot be built the way hrm/ch4-sprite-dma-control
//   built its (DMACONR mirrors DMACON's write content at a different,
//   readable address; no such mirror exists anywhere for a sprite
//   register, which is the whole point of the proposition under test).
//   hrm/ch3-register-map-denise/, an already-committed package on this same
//   `denise` fixture testing the same *kind* of proposition for a
//   different, disjoint block of write-only registers (BPLCON0/2/3,
//   COLOR00-31), solved this by reading two independently-decoded
//   read-only registers -- DENISEID ($07C) and CLXDAT ($00E), neither of
//   them a sprite register and neither cited by any claim in this
//   package -- and asserting both answer distinct, non-zero values in the
//   same run: "Required by the package brief in exactly these words." This
//   package reuses that same control, unchanged in kind, for the same
//   reason: it is the only available demonstration, on this fixture, that
//   data_out is not simply stuck at zero. DENISEID_ADDR=$07C>>1=$3E,
//   CLXDAT_ADDR=$00E>>1=$07, and DENISEID reading $FFFC under ecs=1/aga=0
//   are all facts read out of that already-committed file, not discovered
//   here.
//
// OCR DAMAGE (documented, changes nothing about what is asserted): all 21
//   sprite-related rows on printed page 291 read a lower-case `w` in the
//   page-file corpus where the print sets upper-case `W`
//   (docs/references/amiga-hrm-3rd/appB3/ocr-repairs.md, confirmed count).
//   16 of those 21 are the Agnus sprite-pointer rows (SPRxPTH/SPRxPTL,
//   outside this package); the other 5 are SPR0POS, SPR0CTL, SPR0DATA,
//   SPR0DATB and SPR1POS -- exactly the five APPB2-SPR0*.acc /
//   CH4-APPB-SPR0*.access / APPB2-SPR1POS.acc / CH4-APPB-SPR1POS.access
//   claims below carrying the "ocr-damage"/"corpus_damage" flag. Every one
//   of those claims' own restatement quotes the damage as the page file has
//   it and corrects the case in the restatement, which is the project's
//   reading rule; nothing here re-derives or disputes that correction, and
//   it does not change which address is asserted write-only or how.
//
// A MISSING COLUMN IS NOT A STATEMENT: this package asserts write-only-ness
//   only, never chip ownership -- several of this package's own claims'
//   quoted rows carry a chip-partition column ("A D" for POS/CTL, "D" alone
//   for DATA/DATB) that is out of scope here (see Non-goal above) and is
//   left unasserted rather than read as "Agnus does not implement this
//   register" or similar. No assertion below claims anything about a chip
//   letter, present or absent, in any row.
//
// ============================================================================
// CLAIMS (verbatim quote + restatement, from
// work/ch4_packages/ch4-sprite-registers-write-only-denise.jsonl, package
// "ch4-sprite-registers-write-only-denise", all 72 records, every one
// classify.status == "testable", every one classify.observable == "needs a
// wider boundary: denise"). Grouped below by the one register each
// concerns -- 8 sprites x 4 registers = 32 groups. Two readers (opus,
// sol) recorded most of these sentences independently; both ids are cited
// together wherever they name the same register. Sprite 0's four registers
// additionally carry a page-284 (Appendix A) pair each, since that page's
// generic "SPRxPOS"/"SPRxCTL"/"SPRxDATA"/"SPRxDATB" row is printed with
// sprite 0's own address ($140/$142/$144/$146) and is therefore a
// sprite-0-specific claim, not merely a generic one.
//
//   SPR0POS ($140):  APPA-sprpos.c1 (opus, p.284), CH4-appA-sprite.c6 (sol,
//     p.284), APPB2-SPR0POS.acc (opus, p.291, ocr-damage),
//     CH4-APPB-SPR0POS.access (sol, p.291, corpus_damage)
//   SPR0CTL ($142):  APPA-sprctl.c1 (opus, p.284), CH4-appA-sprite.c2 (sol,
//     p.284), APPB2-SPR0CTL.acc (opus, p.291, ocr-damage),
//     CH4-APPB-SPR0CTL.access (sol, p.291, corpus_damage)
//   SPR0DATA ($144): APPA-sprdat.c1 (opus, p.284), CH4-appA-sprite.c26
//     (sol, p.284), APPB2-SPR0DATA.acc (opus, p.291, ocr-damage),
//     CH4-APPB-SPR0DATA.access (sol, p.291, corpus_damage)
//   SPR0DATB ($146): APPA-sprdat.c2 (opus, p.284), CH4-appA-sprite.c30
//     (sol, p.284), APPB2-SPR0DATB.acc (opus, p.291, ocr-damage),
//     CH4-APPB-SPR0DATB.access (sol, p.291, corpus_damage)
//   SPR1POS ($148):  APPB2-SPR1POS.acc (opus, p.291, ocr-damage),
//     CH4-APPB-SPR1POS.access (sol, p.291, corpus_damage)
//   SPR1CTL ($14A):  APPB2-SPR1CTL.acc (opus, p.292),
//     CH4-APPB-SPR1CTL.access (sol, p.292)
//   SPR1DATA ($14C): APPB2-SPR1DATA.acc (opus, p.292),
//     CH4-APPB-SPR1DATA.access (sol, p.292)
//   SPR1DATB ($14E): APPB2-SPR1DATB.acc (opus, p.292),
//     CH4-APPB-SPR1DATB.access (sol, p.292)
//   SPR2POS ($150):  APPB2-SPR2POS.acc, CH4-APPB-SPR2POS.access (p.292)
//   SPR2CTL ($152):  APPB2-SPR2CTL.acc, CH4-APPB-SPR2CTL.access (p.292)
//   SPR2DATA ($154): APPB2-SPR2DATA.acc, CH4-APPB-SPR2DATA.access (p.292)
//   SPR2DATB ($156): APPB2-SPR2DATB.acc, CH4-APPB-SPR2DATB.access (p.292)
//   SPR3POS ($158):  APPB2-SPR3POS.acc, CH4-APPB-SPR3POS.access (p.292)
//   SPR3CTL ($15A):  APPB2-SPR3CTL.acc, CH4-APPB-SPR3CTL.access (p.292)
//   SPR3DATA ($15C): APPB2-SPR3DATA.acc, CH4-APPB-SPR3DATA.access (p.292)
//   SPR3DATB ($15E): APPB2-SPR3DATB.acc, CH4-APPB-SPR3DATB.access (p.292)
//   SPR4POS ($160):  APPB2-SPR4POS.acc, CH4-APPB-SPR4POS.access (p.292)
//   SPR4CTL ($162):  APPB2-SPR4CTL.acc, CH4-APPB-SPR4CTL.access (p.292)
//   SPR4DATA ($164): APPB2-SPR4DATA.acc, CH4-APPB-SPR4DATA.access (p.292)
//   SPR4DATB ($166): APPB2-SPR4DATB.acc, CH4-APPB-SPR4DATB.access (p.292)
//   SPR5POS ($168):  APPB2-SPR5POS.acc, CH4-APPB-SPR5POS.access (p.292)
//   SPR5CTL ($16A):  APPB2-SPR5CTL.acc, CH4-APPB-SPR5CTL.access (p.292)
//   SPR5DATA ($16C): APPB2-SPR5DATA.acc, CH4-APPB-SPR5DATA.access (p.292)
//   SPR5DATB ($16E): APPB2-SPR5DATB.acc, CH4-APPB-SPR5DATB.access (p.292)
//   SPR6POS ($170):  APPB2-SPR6POS.acc, CH4-APPB-SPR6POS.access (p.292)
//   SPR6CTL ($172):  APPB2-SPR6CTL.acc, CH4-APPB-SPR6CTL.access (p.292)
//   SPR6DATA ($174): APPB2-SPR6DATA.acc, CH4-APPB-SPR6DATA.access (p.292)
//   SPR6DATB ($176): APPB2-SPR6DATB.acc, CH4-APPB-SPR6DATB.access (p.292)
//   SPR7POS ($178):  APPB2-SPR7POS.acc, CH4-APPB-SPR7POS.access (p.292)
//   SPR7CTL ($17A):  APPB2-SPR7CTL.acc, CH4-APPB-SPR7CTL.access (p.292)
//   SPR7DATA ($17C): APPB2-SPR7DATA.acc, CH4-APPB-SPR7DATA.access (p.292)
//   SPR7DATB ($17E): APPB2-SPR7DATB.acc, CH4-APPB-SPR7DATB.access (p.292)
//
// 8 (page-284 pairs, sprite 0 only) + 10 (page-291 pairs, sprite 0's four
// registers plus SPR1POS) + 27 (page-292, opus) + 27 (page-292, sol) = 72.
// Every one of this package's 72 claim ids is cited on exactly one of the
// 32 groups above; none are left uncited, none are cited twice.
//
// ============================================================================
// PLANNED ASSERTIONS (names and intent only -- no RTL line numbers beyond
// what the claims' own classify.evidence and hrm/ch3-register-map-denise's
// already-committed file already handed over; no fixture code below this
// comment block, matching hrm/ch4-sprite-dma-control's own spec-before-RTL
// gate. Added in the next commit, after denise.v is opened to confirm the
// write_reg/read_reg pattern copied from hrm/ch3-register-map-denise's
// Harness still applies unchanged):
//
//   1. CONTROL-FIXTURE-ALIVE: this package's control case, expected to
//      PASS. DENISEID ($07C) and CLXDAT ($00E) -- two independently
//      decoded read-only registers, neither a sprite register, neither
//      cited by this package's own claims -- both read back distinct,
//      non-zero values in the same run, before any assertion below trusts
//      a $0000 (or otherwise non-written) reading from a sprite register to
//      mean "write-only" rather than "the read path is stuck". No manual
//      page; the control for every assertion in this file, on the same
//      measurement basis, reusing hrm/ch3-register-map-denise's own
//      control unchanged in kind.
//
//   2. SPRITE-REGISTER-WO, one shared loop over the 32 groups listed above
//      (SPR0POS .. SPR7DATB), each its own PASS/FAIL line and its own
//      citation, all sharing the leading id "SPRITE-REGISTER-WO": a
//      distinct, non-zero pattern is written to the register's own
//      address, then a read aimed at that SAME address is checked to NOT
//      return the pattern just written. Cites the claim-id group listed
//      above for that specific register.
//
// Two planned assertion groups (1 control + 1 loop of 32), covering every
// one of the 72 claim ids in this package's own inventory -- none left
// silently uncovered.
//
// No RTL has been opened yet to write this comment block: the port names,
// the reg_address_in word-address convention, and DENISEID/CLXDAT's
// addresses and read behaviour come from this package's own claims'
// classify.evidence fields and from hrm/ch3-register-map-denise's
// already-committed file (both handed to or available to the building
// agent as inventory, not discovered by this agent opening denise.v). The
// register addresses used below ($A0-$BF) are this agent's own arithmetic
// on this package's own claims' quoted byte addresses, checked against all
// 72 of them above. Exact confirmation that the write_reg/read_reg pattern
// copied from hrm/ch3-register-map-denise applies unchanged to this
// package's own build, and the mutation log, are answered in the next
// commit, after denise.v is read.
// ============================================================================

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after denise.v and its
// sub-modules were opened for the first time -- the specification comment
// above this point was committed on its own first, per the chapter's
// spec-before-RTL gate; see this package's own git history, the
// "specification, before RTL is opened" commit.
//
// Every port name and structural fact the PORTS section above already
// handed over is confirmed at the exact lines below; nothing here revised
// the spec, only confirmed it:
//   - denise.v:31-52 -- module header, exact port list as stated above.
//   - denise.v:63 -- `parameter DENISEID = 9'h07c;`.
//   - denise.v:130 -- `assign data_out = col_out | deniseid_out | rgb_out;`
//     (the active, non-`DENISE_EBR` branch -- DENISE_EBR is not defined by
//     this package's Makefile, confirmed by grep of both this package's
//     Makefile and hrm/ch3-register-map-denise's).
//   - denise.v:322 -- `assign deniseid_out = reg_address_in[8:1]==
//     DENISEID[8:1] ? aga ? 16'h00f8 : ecs ? 16'hfffc : 16'hffff :
//     16'h0000;` -- confirms DENISEID answers $FFFC under ecs=1/aga=0,
//     exactly as hrm/ch3-register-map-denise's already-committed file
//     already stated.
//   - denise_collision.v:19-21,117 -- `parameter CLXDAT = 9'h00e;` and
//     `assign data_out = reg_address_in[8:1]==CLXDAT[8:1] ?
//     {1'b1,clxdat[14:0]} : 16'd0;` -- CLXDAT's own data_out forces bit 15
//     to 1'b1 whenever its address is read, so CLXDAT is unconditionally
//     non-zero when addressed, independent of any actual collision state --
//     an even stronger guarantee for the control than merely "measured
//     non-zero today".
//   - denise_sprites.v:104-112 -- `selsprx = SPRPOSCTLBASE[8:6]==
//     reg_address_in[8:6]`, `selsprN = selsprx && reg_address_in[5:3]==N`.
//     With SPRPOSCTLBASE=9'h140 (word address $A0), reg_address_in[5:3]
//     spans weights 16/8/4 (label n has weight 2^(n-1) since the vector is
//     indexed [8:1], not [7:0]) -- incrementing that 3-bit field by 1 is
//     +4 in word-address units, confirming this package's own SPR_BASE+4n
//     arithmetic exactly, not merely assuming it.
//   - denise_sprites.v:176 (and the 7 further sprite instances) --
//     `.address(reg_address_in[2:1])` -- the low 2 bits select the
//     register within a sprite's 4-word block.
//   - denise_sprites_shifter.v:35-38 -- `parameter POS=2'b00; CTL=2'b01;
//     DATA=2'b10; DATB=2'b11;` -- confirms the +0/+1/+2/+3 ordering this
//     package's spec already assumed.
//   - denise_sprites.v/denise_sprites_shifter.v -- grep for `data_out`
//     finds no match in either file, confirming every claim's own
//     classify.evidence ("the module's only outputs are nsprite and
//     sprdata; there is no data_out") independently.
//   - denise_sprites_shifter.v:127-138 and :257-269 -- sprite register
//     writes are gated by a plain `always @(posedge clk) if (clk7_en)`,
//     the SAME generic pattern hrm/ch3-register-map-denise's write_reg
//     already drives successfully for other Denise registers on this same
//     fixture. c1/c3 reach only the shift enables and the bitplane
//     parallel load (denise_bitplane_shifter.v:212, 216, 224;
//     denise_sprites.v:163-164), none of which gates a register write.
//
// Nothing here was a surprise against the inventory; opening the files
// confirmed the spec above rather than revising it. Harness (tick/phase,
// write_reg, read_reg, ecs=1/aga=0 fixture configuration) is copied
// unchanged from hrm/ch3-register-map-denise's own Harness, for the same
// reason that package's own file states: it is the measured-working drive
// pattern for this exact fixture.
//
// MUTATION LOG: see the bottom of this file, after main(). Every entry was
// actually applied -- edited into this file, rebuilt under WSL/Verilator
// (`make test`), run, the PASS/FAIL line read from real output, then
// reverted and rebuilt clean before the next entry.
// ============================================================================

#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vdenise.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_SPRITE_REGISTER_WO
#define MUT_SPRITE_REGISTER_WO 0
#endif

namespace {

// reg_address_in is [8:1] (word address = manual's byte address >> 1);
// every constant below follows hrm/ch3-register-map-denise's own
// convention (see PORTS above).
constexpr uint16_t IDLE_ADDR     = 0x00;
constexpr uint16_t DENISEID_ADDR = 0x3E;  // 9'h07C >> 1 (denise.v:63)
constexpr uint16_t CLXDAT_ADDR   = 0x07;  // 9'h00E >> 1 (denise_collision.v:21)
constexpr uint16_t SPR_BASE      = 0xA0;  // 9'h140 >> 1 (denise_sprites.v:55/67)

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%04X", static_cast<unsigned>(v));
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;   // makes DENISEID answer $FFFC (denise.v:322) -- the
                         // control this package's own measurement depends on.
        dut->aga = 0;
        phase = 0;
        drive_phase();
        dut->eval();
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
    }
    ~Harness() { delete dut; }

    void tick() {
        drive_phase();
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) & 3;
    }

    void drive_phase() {
        dut->clk7_en = (phase == 0) ? 1 : 0;
        dut->c1 = (phase == 0 || phase == 1) ? 1 : 0;
        dut->c3 = (phase == 1 || phase == 2) ? 1 : 0;
    }

    // Writes `data` to word address `addr`, held across a full 4-phase
    // cycle so clk7_en samples it exactly once regardless of phase.
    void write_reg(uint16_t addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }

    // Presents `addr` on reg_address_in, gives it a full 4-phase cycle to
    // settle (data_out is a combinational mux, per denise.v:130), and
    // returns data_out.
    uint16_t read_reg(uint16_t addr) {
        dut->reg_address_in = addr;
        dut->data_in = 0;
        tick(); tick(); tick(); tick();
        uint16_t result = dut->data_out;
        dut->reg_address_in = IDLE_ADDR;
        tick();
        return result;
    }

    Vdenise* dut;
    int phase;
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

struct SpriteRegCase {
    const char* name;
    uint16_t word_addr;
    const char* byte_addr;
    const char* page;
    const char* citations;
};

// 32 groups: 8 sprites x {POS,CTL,DATA,DATB}. Word address = SPR_BASE +
// 4*sprite + {0,1,2,3}, confirmed against denise_sprites.v/
// denise_sprites_shifter.v in IMPLEMENTATION NOTES above.
static const SpriteRegCase kCases[32] = {
    {"SPR0POS",  0xA0, "$140", "284/291", "APPA-sprpos.c1, CH4-appA-sprite.c6, APPB2-SPR0POS.acc, CH4-APPB-SPR0POS.access"},
    {"SPR0CTL",  0xA1, "$142", "284/291", "APPA-sprctl.c1, CH4-appA-sprite.c2, APPB2-SPR0CTL.acc, CH4-APPB-SPR0CTL.access"},
    {"SPR0DATA", 0xA2, "$144", "284/291", "APPA-sprdat.c1, CH4-appA-sprite.c26, APPB2-SPR0DATA.acc, CH4-APPB-SPR0DATA.access"},
    {"SPR0DATB", 0xA3, "$146", "284/291", "APPA-sprdat.c2, CH4-appA-sprite.c30, APPB2-SPR0DATB.acc, CH4-APPB-SPR0DATB.access"},
    {"SPR1POS",  0xA4, "$148", "291",     "APPB2-SPR1POS.acc, CH4-APPB-SPR1POS.access"},
    {"SPR1CTL",  0xA5, "$14A", "292",     "APPB2-SPR1CTL.acc, CH4-APPB-SPR1CTL.access"},
    {"SPR1DATA", 0xA6, "$14C", "292",     "APPB2-SPR1DATA.acc, CH4-APPB-SPR1DATA.access"},
    {"SPR1DATB", 0xA7, "$14E", "292",     "APPB2-SPR1DATB.acc, CH4-APPB-SPR1DATB.access"},
    {"SPR2POS",  0xA8, "$150", "292",     "APPB2-SPR2POS.acc, CH4-APPB-SPR2POS.access"},
    {"SPR2CTL",  0xA9, "$152", "292",     "APPB2-SPR2CTL.acc, CH4-APPB-SPR2CTL.access"},
    {"SPR2DATA", 0xAA, "$154", "292",     "APPB2-SPR2DATA.acc, CH4-APPB-SPR2DATA.access"},
    {"SPR2DATB", 0xAB, "$156", "292",     "APPB2-SPR2DATB.acc, CH4-APPB-SPR2DATB.access"},
    {"SPR3POS",  0xAC, "$158", "292",     "APPB2-SPR3POS.acc, CH4-APPB-SPR3POS.access"},
    {"SPR3CTL",  0xAD, "$15A", "292",     "APPB2-SPR3CTL.acc, CH4-APPB-SPR3CTL.access"},
    {"SPR3DATA", 0xAE, "$15C", "292",     "APPB2-SPR3DATA.acc, CH4-APPB-SPR3DATA.access"},
    {"SPR3DATB", 0xAF, "$15E", "292",     "APPB2-SPR3DATB.acc, CH4-APPB-SPR3DATB.access"},
    {"SPR4POS",  0xB0, "$160", "292",     "APPB2-SPR4POS.acc, CH4-APPB-SPR4POS.access"},
    {"SPR4CTL",  0xB1, "$162", "292",     "APPB2-SPR4CTL.acc, CH4-APPB-SPR4CTL.access"},
    {"SPR4DATA", 0xB2, "$164", "292",     "APPB2-SPR4DATA.acc, CH4-APPB-SPR4DATA.access"},
    {"SPR4DATB", 0xB3, "$166", "292",     "APPB2-SPR4DATB.acc, CH4-APPB-SPR4DATB.access"},
    {"SPR5POS",  0xB4, "$168", "292",     "APPB2-SPR5POS.acc, CH4-APPB-SPR5POS.access"},
    {"SPR5CTL",  0xB5, "$16A", "292",     "APPB2-SPR5CTL.acc, CH4-APPB-SPR5CTL.access"},
    {"SPR5DATA", 0xB6, "$16C", "292",     "APPB2-SPR5DATA.acc, CH4-APPB-SPR5DATA.access"},
    {"SPR5DATB", 0xB7, "$16E", "292",     "APPB2-SPR5DATB.acc, CH4-APPB-SPR5DATB.access"},
    {"SPR6POS",  0xB8, "$170", "292",     "APPB2-SPR6POS.acc, CH4-APPB-SPR6POS.access"},
    {"SPR6CTL",  0xB9, "$172", "292",     "APPB2-SPR6CTL.acc, CH4-APPB-SPR6CTL.access"},
    {"SPR6DATA", 0xBA, "$174", "292",     "APPB2-SPR6DATA.acc, CH4-APPB-SPR6DATA.access"},
    {"SPR6DATB", 0xBB, "$176", "292",     "APPB2-SPR6DATB.acc, CH4-APPB-SPR6DATB.access"},
    {"SPR7POS",  0xBC, "$178", "292",     "APPB2-SPR7POS.acc, CH4-APPB-SPR7POS.access"},
    {"SPR7CTL",  0xBD, "$17A", "292",     "APPB2-SPR7CTL.acc, CH4-APPB-SPR7CTL.access"},
    {"SPR7DATA", 0xBE, "$17C", "292",     "APPB2-SPR7DATA.acc, CH4-APPB-SPR7DATA.access"},
    {"SPR7DATB", 0xBF, "$17E", "292",     "APPB2-SPR7DATB.acc, CH4-APPB-SPR7DATB.access"},
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL-FIXTURE-ALIVE -- the fixture can deliver a known value before
    // anything below is trusted to read one back. DENISEID and CLXDAT are
    // two independently decoded read-only registers (denise.v:322,
    // denise_collision.v:117); both answering distinct, non-zero values
    // before any write-only-register-reads-non-written-value assertion
    // below is trusted rules out "data_out is stuck" as the explanation for
    // every sprite-register reading that follows. No manual page: the
    // control for every assertion in this file, on the same measurement
    // basis. Reused, unchanged in kind, from hrm/ch3-register-map-denise's
    // own CONTROL-FIXTURE-ALIVE group.
    // =========================================================================
    {
        Harness h;
        uint16_t deniseid = h.read_reg(DENISEID_ADDR);
        uint16_t clxdat_addr = CLXDAT_ADDR;
#if MUT_CONTROL
        clxdat_addr = DENISEID_ADDR;
#endif
        uint16_t clxdat = h.read_reg(clxdat_addr);
        checks.expect(
            deniseid != 0 && clxdat != 0 && deniseid != clxdat,
            "CONTROL-FIXTURE-ALIVE: DENISEID ($07C, denise.v:322) and "
            "CLXDAT ($00E, denise_collision.v:117), two independently "
            "decoded read-only registers, both read back distinct non-zero "
            "values (" + hex4(deniseid) + " and " + hex4(clxdat) +
            ") before any assertion below trusts a non-written reading to "
            "mean write-only rather than a stuck read path. No manual page "
            "-- the control for every SPRITE-REGISTER-WO assertion in this "
            "file, on the same measurement basis.");
    }

    // =========================================================================
    // SPRITE-REGISTER-WO (p.284/291/292), one shared loop over all 32
    // registers this package's claims name: a distinct, non-zero pattern
    // written to the register's own address reads back $0000 at that same
    // address (write-only) -- data_out (denise.v:130) is the OR of
    // col_out|deniseid_out|rgb_out only, none of which ever decodes a
    // sprite-register address (IMPLEMENTATION NOTES above), so the correct,
    // strongest expectation is exactly $0000, not merely "not the pattern".
    // =========================================================================
    {
        for (int i = 0; i < 32; ++i) {
            Harness h;
            const SpriteRegCase& c = kCases[i];
            uint16_t pattern = static_cast<uint16_t>(0xC000 + i);
            h.write_reg(c.word_addr, pattern);

            uint16_t read_addr = c.word_addr;
#if MUT_SPRITE_REGISTER_WO
            if (i == 13) read_addr = DENISEID_ADDR;  // SPR3CTL, representative
#endif
            uint16_t r = h.read_reg(read_addr);

            checks.expect(
                r == 0x0000,
                std::string("SPRITE-REGISTER-WO ") + c.name + " (p." + c.page +
                    "): a distinct pattern (" + hex4(pattern) + ") written to " +
                    c.name + " (" + c.byte_addr + ") reads back " + hex4(r) +
                    " at that same address, not the pattern just written "
                    "(write-only). " + c.citations + ".");
        }
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// ============================================================================
// MUTATION LOG (12.09.2026). Every entry below was actually applied by this
// agent -- edited into this file via -DMUT_*=1, rebuilt under
// WSL/Verilator (`make test`), run, the PASS/FAIL line read from real
// output (quoted verbatim below, not predicted), then reverted (flag back
// to 0) and rebuilt clean before the next entry. Baseline before this
// log's first mutation and final clean run after the last revert were both
// "33 passed, 0 failed".
//
// 1. CONTROL-FIXTURE-ALIVE: CLXDAT's read address was redirected from
//    $00E to DENISEID's own address ($07C), so both reads returned the
//    same value ($FFFC) instead of two independently-decoded registers,
//    and the distinctness check failed. Result: "32 passed, 1 failed" --
//    only CONTROL-FIXTURE-ALIVE went red. Reverted (MUT_CONTROL back to
//    0); rebuilt clean (33 passed, 0 failed).
//
// 2. SPRITE-REGISTER-WO (representative index 13, SPR3CTL): the
//    post-write read was redirected from SPR3CTL's own address ($15A) to
//    DENISEID's ($07C), so the reading came back $FFFC instead of $0000.
//    Result: "32 passed, 1 failed" -- only SPRITE-REGISTER-WO SPR3CTL went
//    red ("reads back $FFFC ... not the pattern just written"); all other
//    31 registers in the same loop stayed green. Reverted
//    (MUT_SPRITE_REGISTER_WO back to 0); rebuilt clean (33 passed, 0
//    failed).
//
// Machine-readable records, one per line:
// MUTATION CONTROL-FIXTURE-ALIVE stimulus red: CLXDAT's read address was redirected from $00E to DENISEID's own address ($07C), so both reads returned the same value ($FFFC) instead of two independently-decoded registers and the distinctness check failed (33/0 baseline went to 32 passed/1 failed, only this assertion).
// MUTATION SPRITE-REGISTER-WO stimulus red: index 13's (SPR3CTL) post-write read was redirected from its own address ($15A) to DENISEID's ($07C), so the reading came back $FFFC instead of $0000 (33/0 baseline went to 32 passed/1 failed, only SPRITE-REGISTER-WO SPR3CTL).
// ============================================================================

