// Test purpose: the sprite registers Denise decodes -- SPRxPOS, SPRxCTL,
//   SPRxDATA, SPRxDATB, for all eight sprites (0-7) -- are write-only: a
//   read aimed at the register's own address does not return what was
//   written there. This package is the Denise half of that single
//   proposition, stated once generically on page 284 (Appendix A) and once
//   per sprite on pages 291/292 (Appendix B). A sibling package,
//   hrm/ch4-sprite-registers-write-only-agnus/, is the Agnus half of the
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
