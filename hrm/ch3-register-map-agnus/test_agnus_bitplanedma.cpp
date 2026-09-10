// Test purpose: The REGISTER MAP agnus_bitplanedma decodes -- addresses,
//   read/write direction, and chip ownership -- for the registers named in
//   this package's claims.jsonl: DDFSTRT, DDFSTOP, BPL1MOD, BPL2MOD, and the
//   six bitplane pointer pairs BPL1PTH/PTL .. BPL6PTH/PTL. Package
//   ch3-register-map-agnus, fixture agnus_bitplanedma (one Verilator build,
//   this module only). This is deliberately NOT about the display-data-fetch
//   WINDOW behaviour these same two DDFSTRT/DDFSTOP registers also control
//   (word counts, hardware limits, the horizontal-scroll extra word, the
//   printed manual's own PAL byte-count arithmetic error) -- that is
//   hrm/ch3-ddf-fetch-window/'s job, already built; no claim id or
//   PROPOSITION tested there is repeated here. Every assertion below tests
//   exactly one proposition: "this register lives at this address" (and,
//   where the SAME printed row states it, "...and is write-only,
//   Agnus-owned"). Where the only way to observe a write-only register's
//   address is through the module's write-only-effect ports (address_out,
//   reg_address_out, dma) -- this module has no data_out at all, so nothing
//   can be "read back" through the register bus the way some sibling
//   modules allow -- those ports are used purely as ADDRESS-DECODE PROBES:
//   did the write reach a register that measurably affects the fetched
//   address stream AT ALL, and does it do so ONLY at the address the manual
//   names, not at a neighbour -- never as a check that the fetched VALUE,
//   WORD COUNT or TIMING matches any manual-stated figure.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 270, 272, 277 (Appendix A register listing) and 290, 291
//   (Appendix B register listing).
// Non-goals: the display-data-fetch WINDOW itself -- word counts, the
//   $18/$D8 hardware limits, DDFSTRT/DDFSTOP bit-field granularity,
//   horizontal-scroll extra-word behaviour, modulo VALUES, dual-playfield
//   sharing -- all belong to hrm/ch3-ddf-fetch-window/ and are not
//   re-tested here, even though several assertions below reuse the SAME
//   Harness mechanics and even the SAME underlying signals (dma,
//   address_out, reg_address_out) that package already reads for a
//   different purpose. Also not tested: the printed "(high 3 bits)" /
//   "(low 15 bits)" bit-WIDTH descriptors that ride along in the same
//   quoted Appendix A/B rows as the twelve pointer-register ADDRESS claims
//   -- see CONFLICT NOTE below.
//
// BOUNDARY (from the build brief, not yet from opening the file):
//   nanomig/src/minimig-aga/agnus_bitplanedma.v:57-58 (BPL1MOD_REG 9h108,
//   BPL2MOD_REG 9h10a), write decode l.262-274, modulo add l.484-493
//   (plane[0] selects BPL2MOD, else BPL1MOD), mod strobe l.454;
//   agnus_bitplanedma.v:54 (BPLPTBASE_REG 9h0E0), pointer banks l.217-242,
//   pointer arithmetic l.484-493, address_out[20:1] port l.46 -- there is no
//   address bit 0; agnus_bitplanedma.v:55-56 (DDFSTRT_REG 9h092, DDFSTOP_REG
//   9h094), write decode l.247-259 (ddfstrt[8:2] <= data_in[7:1]), position
//   compare l.336-354, hard limits $18/$D8 l.356-374, ddfena l.407-414.
//   These line numbers and register names come from claims.jsonl's own
//   evidence field and this package's PACKAGE.md, handed to this agent
//   before agnus_bitplanedma.v was opened for this package -- reused here,
//   not independently re-derived. This module's own top-level Verilator
//   Harness (tick/write_reg/write_ptr/run_to_line/run_single_line/
//   prime_to_line) is reused verbatim from
//   hrm/ch3-ddf-fetch-window/test_agnus_bitplanedma.cpp -- an
//   already-built, already-RTL-verified fixture FOR THIS SAME TOP MODULE --
//   per the precedent in hrm/ch3-collision-registers/'s own spec-only
//   commit and hrm/ch3-register-map-agnus-beamcounter/'s own spec-only
//   commit (reusing an already-built sibling harness for the identical top
//   module is not "opening the RTL"; the claims.jsonl evidence field and
//   the sibling package's own published comments already made every one of
//   these addresses, port names and harness mechanics public before this
//   file was written). No PASS/FAIL threshold or expected value below was
//   chosen or adjusted because of anything seen in agnus_bitplanedma.v
//   beyond what BOUNDARY states above.
//
// CLAIM GROUPS (16 propositions covering all 46 claims; two ids restating
//   the identical proposition from a different reader/page are grouped and
//   cited together on the one assertion that tests it):
//
//  -- BPL1MOD at $108 (p.270, p.291) --
//   A-bpl1mod.addr (opus, p270): "BPL1MOD    108      W       A     Bitplane
//     modulo (odd planes)"
//   A-bplmod.c1 (sol, p270): same row.
//   B-bpl1mod.addr, B-bpl1mod.addr-p291 (opus, p291): "BPL1MOD        108
//     W   A           Bitplane modulo (odd planes)"
//   B-subject.c126 (sol, p291): same row.
//
//  -- BPL2MOD at $10A (p.270, p.291) --
//   A-bpl2mod.addr (opus, p270): "BPL2MOD    10A      W       A     Bitplane
//     modulo (even planes)"
//   A-bplmod.c5 (sol, p270): same row.
//   B-bpl2mod.addr, B-bpl2mod.addr-p291 (opus, p291): "BPL2MOD        10A
//     W   A           Bitplane modulo (even planes)"
//   B-subject.c130 (sol, p291): same row.
//
//  -- BPL1PTH at $0E0 (p.272 generic BPLxPTH row, p.291 named row) --
//   A-bplptr.c1 (sol, p272): "BPLxPTH    0E0      W       A     Bitplane x
//     pointer (high 3 bits)"
//   A-bplxpth.addr (opus, p272): same row.
//   B-bpl1pth.addr (opus, p291): "BPL1PTH    +   0E0    W   A
//     Bitplane 1 pointer    (high 3 bits)"
//   B-subject.c26 (sol, p291): same row.
//
//  -- BPL1PTL at $0E2 (p.272 generic BPLxPTL row, p.291 named row) --
//   A-bplptr.c4 (sol, p272): "BPLxPTL    0E2      W       A     Bitplane x
//     pointer (low 15 bits)"
//   A-bplxptl.addr (opus, p272): same row.
//   B-bpl1ptl.addr (opus, p291): "BPL1PTL    +   0E2    W   A
//     Bitplane 1 pointer    (low 15 bits)"
//   B-subject.c33 (sol, p291): same row.
//
//  -- BPL2PTH at $0E4 / BPL2PTL at $0E6 (p.291) --
//   B-bpl2pth.addr (opus) / B-subject.c40 (sol): "BPL2PTH    +   0E4
//     W   A           Bitplane 2 pointer    (high 3 bits)"
//   B-bpl2ptl.addr (opus) / B-subject.c47 (sol): "BPL2PTL    +   0E6
//     W   A           Bitplane 2 pointer    (low 15 bits)"
//
//  -- BPL3PTH at $0E8 / BPL3PTL at $0EA (p.291) --
//   B-bpl3pth.addr / B-subject.c54: "BPL3PTH    +   0E8    W   A
//     Bitplane 3 pointer    (high 3 bits)"
//   B-bpl3ptl.addr / B-subject.c61: "BPL3PTL    +   0EA    W   A
//     Bitplane 3 pointer    (low 15 bits)"
//
//  -- BPL4PTH at $0EC / BPL4PTL at $0EE (p.291) --
//   B-bpl4pth.addr / B-subject.c68: "BPL4PTH    +   0EC    W   A
//     Bitplane 4 pointer    (high 3 bits)"
//   B-bpl4ptl.addr / B-subject.c75: "BPL4PTL    +   0EE    W   A
//     Bitplane 4 pointer    (low 15 bits)"
//
//  -- BPL5PTH at $0F0 / BPL5PTL at $0F2 (p.291) --
//   B-bpl5pth.addr / B-subject.c82: "BPL5PTH    +   0F0    W   A
//     Bitplane 5 pointer    (high 3 bits)"
//   B-bpl5ptl.addr / B-subject.c89: "BPL5PTL    +   0F2    W   A
//     Bitplane 5 pointer    (low 15 bits)"
//
//  -- BPL6PTH at $0F4 / BPL6PTL at $0F6 (p.291) --
//   B-bpl6pth.addr / B-subject.c96: "BPL6PTH    +   0F4    W   A
//     Bitplane 6 pointer    (high 3 bits)"
//   B-bpl6ptl.addr / B-subject.c103: "BPL6PTL    +   0F6    W   A
//     Bitplane 6 pointer    (low 15 bits)"
//
//  -- DDFSTRT at $092 (p.277 Appendix A, p.290 Appendix B) --
//   A-ddf.c3 (sol, p277): "DDFSTRT    092      W       A    Display data
//     fetch start (horiz. position)"
//   A-ddfstrt.addr (opus, p277): same row.
//   B-ddfstrt.addr (opus, p290): "    DDFSTRT        092    W
//     Display bitplane data fetch start"
//   B-subject.c16 (sol, p290): same row.
//
//  -- DDFSTOP at $094 (p.277 Appendix A, p.290 Appendix B) --
//   A-ddf.c1 (sol, p277): "DDFSTOP    094      W       A    Display data
//     fetch stop (horiz. position)"
//   A-ddfstop.addr (opus, p277): same row.
//   B-ddfstop.addr (opus, p290): "    DDFSTOP        094    W
//     Display bitplane data fetch stop"
//   B-subject.c19 (sol, p290): same row.
//
// CHIP OWNERSHIP ('A' = Agnus): the p.270/272/291 rows above all print an
//   explicit 'A' in the chip column alongside the address, and are asserted
//   as such. The p.290 Appendix B rows for DDFSTRT/DDFSTOP print NO chip
//   letter at all (blank column) -- per this package's own build brief,
//   "a blank cell is missing information, not a statement that no chip owns
//   the register": DDFSTRT/DDFSTOP's chip ownership is NOT asserted from
//   the p.290 quotes, only their address and W direction are. Their chip
//   ownership IS asserted, but from the undisputed p.277 Appendix A rows
//   instead, which do print 'A'. Chip ownership is additionally established
//   STRUCTURALLY, independent of any printed letter: this package's
//   Makefile compiles exactly one Verilog source, agnus_bitplanedma.v --
//   no Denise, Paula, CPU or other custom-chip RTL is anywhere in the
//   Verilator build -- so any address that produces an observable effect
//   inside this DUT was necessarily decoded and held by Agnus circuitry.
//
// CONFLICT NOTE (12 claims -- the twelve BPLnPTH/BPLnPTL address ids above
//   -- PACKAGE.md marks these as "carries source conflicts"): the ADDRESS
//   fact itself (BPLPTBASE=$0E0, BASE+4N per-plane spacing) is corroborated
//   consistently everywhere it is cited -- the RTL's own base-address
//   comment (l.54), the generic Appendix A p.272 row, and the six explicit
//   Appendix B p.291 rows all agree numerically -- and is asserted as
//   undisputed below. What is NOT asserted is the "(high 3 bits)" /
//   "(low 15 bits)" bit-WIDTH descriptor riding along in the same quoted
//   rows: BPLPTBASE_REG's high-word field width is a fact to be confirmed
//   once the RTL is open, not assumed from the printed "(high 3 bits)"
//   phrase, and is recorded as a measurement (see IMPLEMENTATION NOTES,
//   added after the RTL is opened) rather than folded into any address
//   PASS/FAIL. No literal `conflict_resolution` field was found on these
//   12 records beyond their shared `source_defines: "conflict: page 54
//   versus page 272 (and appendix B page 291)"` text; this note states the
//   resolution this package actually applies, and why.
//
// METHODOLOGY (planned, applied identically to all 16 assertions):
//   ADDRESS-DECODE SELECTIVITY: for a write-only register R with manual
//   address A and a chosen "wrong" address W (always a DIFFERENT REAL
//   register's manual address, never an unassigned one -- proving the
//   decoder actually discriminates between two live paths, not merely that
//   garbage addresses are ignored), three fresh harnesses with identical
//   companion setup are built: h_base (R never written), h_wrong (test data
//   written to W), h_correct (the SAME test data written to A). The
//   proposition: measure(h_wrong) is indistinguishable from measure(h_base)
//   (W has no effect on R), and measure(h_correct) is clearly different
//   from measure(h_base) (A does affect R). For the twelve pointer
//   registers the observable is the plane's own fetched address_out field
//   (high 5 bits for a PTH claim, low 16 bits for a PTL claim), read via
//   this module's existing dma/address_out/reg_address_out ports -- no
//   internal signal is peeked. For BPL1MOD/BPL2MOD the observable is the
//   line-to-line step of a plane's own first fetched address (does writing
//   a large modulo value change how far the pointer moves between one line
//   and the next). For DDFSTRT the observable is the hpos of the line's
//   first dma pulse; for DDFSTOP it is the line's total dma pulse count --
//   in both cases only WHETHER the correct address moves the signal is
//   asserted, never the manual's specific word-count or timing figures
//   (that comparison is reserved for hrm/ch3-ddf-fetch-window/).
//
// CLAIM COVERAGE: all 46 records of claims.jsonl will be cited, in full,
//   inside an expect() label. 16 ADDRESS assertions (one per proposition
//   above) cover all 46 claims, plus one no-claim FIXTURE-DELIVERS-
//   KNOWN-VALUE control. No claim id is declined.
//
// CONTROL CASES: every assertion already carries its own internal control
//   (h_wrong is expected to PASS -- match h_base -- on the identical
//   measurement basis as h_correct, which is the case under test).
//   FIXTURE-DELIVERS-KNOWN-VALUE additionally proves this package's own
//   independently built obj_dir actually delivers a known pointer value
//   through the write-then-fetch path before any address-decode assertion
//   below is trusted to mean anything.
//
// MUTATION LOG: recorded after agnus_bitplanedma.v is actually opened and
//   `make test` has actually been run under WSL/Verilator (left empty in
//   this spec-only commit, filled in the follow-up commit, per the build
//   brief's order of work and the precedent in
//   hrm/ch3-register-map-agnus-beamcounter/'s own spec-only commit).
//
// No RTL has been opened yet to write this comment block beyond the
// BOUNDARY text handed to this agent in PACKAGE.md/claims.jsonl. The
// Harness, helper functions and assertion bodies are added in the next
// commit, after agnus_bitplanedma.v is read in full for this package.
