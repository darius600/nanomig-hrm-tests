// ch3-shres-bitplane-colour-encoding -- denise conformance test
//
// Test purpose: the ECS SuperHires bitplane colour encoding printed on p.301
//   -- the table that maps each of the sixteen lower colour registers' bits
//   onto symbols a-x, and the legend "Bitplane (Color 0) : ab-- cd-- ef--"
//   (and its siblings for colours 1-3) that names which symbol is which
//   colour component. Package ch3-shres-bitplane-colour- encoding, fixture
//   denise (one Verilator build, top module denise, per PACKAGE.md). A
//   sibling package covers the sprite half of the same scheme on page 302.
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
//   hold-and-modify or dual-playfield. bpu is held at 1 for the OCS/ control
//   measurements and 4 for every register-addressing SuperHires stimulus --
//   see RTL OPENED below for why 4, not 2: this RTL has no
//   bitplane-pair-to-sixteen-registers multiplexing circuit for a plain
//   2-bitplane index to reach through.
// Claim ids: ids such as `C-shres-bpl-bits.c1` and `C-shres-bpl-bits.c2` name
//   one sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch3.json lists under `inventories`.
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
// claim -- see WHAT p.301'S OWN TABLE PROVES below; this was the one part
// of the spec-only commit genuinely unresolved before opening the RTL).
// Algebra on the table alone (performed before this file's spec-only
// commit) showed the table is self-consistent with a shared two-tick
// colour-table fetch: register index = 4*colorODD + colorEVEN, each
// register's R/G/B nibbles split into a "bits[3:2]" half (colorEVEN's own
// letters) and a "bits[1:0]" half (colorODD's own letters). RTL OPENED
// (denise.v, denise_bitplanes.v, denise_colortable.v, all read in full
// after the spec-only commit): this prediction has NO counterpart in the
// RTL to correct, because the mechanism it predicts does not exist here.
// denise_colortable.v (l.7-69) declares no `shres` port at all; its
// address input `select` is `clut_data`, which for a single playfield
// (dpf=0) is `plfdata = bpldata[8:1]` DIRECTLY (denise_playfields.v:100)
// -- plane N's own live serial bit is colour-table address bit (N-1), one
// bit per plane, no combining of adjacent bitplane-clock samples anywhere
// in the call graph. denise_bitplanes.v's only use of `shres`
// (l.298-303) is the per-plane shift-RATE mux (shres -> shift every
// 28MHz tick instead of every 2nd/4th) -- it does not touch which
// colour-table address gets formed or read. So: the sixteen lower
// registers are not reached through any bitplane-driven multiplexing at
// all in this RTL; registers 4-15 are reachable only by driving MORE than
// two bitplanes directly as a plain four-bit index (bpu=4, planes 1-4 =
// the register number bit-for-bit, exactly like an ordinary non-
// SuperHires 4-bitplane display) -- select_register() in the Harness
// below. This is not a harness bug to fix; it is the finding this whole
// package turns on -- see SHRES-BITPLANE-TABLE-COLOR-00 below and RESULTS.
//
// ORDER OF WORK: this file's assertions, the oracle table (kRegRow,
// kBitIds, kRowIds, kLegend) and claim-id coverage were written and
// committed from claims.jsonl's quotations and PACKAGE.md's own evidence
// field, plus the two already-committed sibling harnesses named above,
// ONLY -- denise.v itself was not opened until after that commit, per the
// build brief's explicit gate. This is the second commit: the RTL has now
// been read in full (denise.v, denise_bitplanes.v, denise_colortable.v,
// denise_playfields.v), the ADDRESSING MECHANISM prediction above has been
// corrected (in the Harness's own stimulus construction only -- no oracle
// text, no expected value, no claim id changed), the build has been run
// under WSL/Verilator, and the MUTATION LOG below reflects real,
// individually-confirmed results.
//
// BITPLANE COUNT CORRECTION (third pass, this agent, 11.09.2026): an
// independent validator flagged that the main sweep below selected
// registers using FOUR bitplanes (bpu=4) while page 300 caps SuperHires
// plane depth at TWO -- "The maximum plane depth for a SuperHires
// viewport is 2 bitplanes"; BPLCON0 bits 14-12 are documented as "Depth
// of SuperHires mode (1 or 2)". Checked against the printed page: holds.
// Checked against the RTL (denise.v): `assign bpldata[3] = l_bpu > 2 ?
// bpldata_out[3] : 1'b0;` and the equivalent line for bpldata[4] force
// planes 3 and 4 out of the colour-table address whenever l_bpu<=2 --
// i.e. whenever BPLCON0 encodes any page-300-compliant SuperHires depth
// at all. Consequence: with shres=1 held to a compliant bpu (1 or 2),
// only registers 0-3 (planes 1-2 alone) can ever be selected via
// bitplane-driven addressing in this RTL; registers 4-15 need plane 3
// and/or 4 live and are architecturally unreachable under ANY BPLCON0
// configuration the manual permits for SuperHires -- not merely "not
// reinterpreted" (the pre-existing finding) but never even addressable
// that way. Fix: SHRES-BITPLANE-TABLE-COLOR-00 through -03 now measure
// at bpu=2 (was 4); -04 through -0F are declined (checks.expect(false,
// ...), own ids still cited in full -- the sibling ch3-shres-sprite-
// colour-encoding package's own REG-0x10 convention for a register no
// live measurement can reach), not silently dropped. SELECT-REGISTER-
// ADDRESSES-CORRECTLY (shres=0 throughout, not a SuperHires-mode claim)
// and SHRES-REINTERPRETS-SAME-REGISTERS (bpu=1 throughout, already
// compliant) needed no change and were left untouched, per the build
// brief's own instruction to leave the four existing MUT_* controls
// alone. Re-run: SHRES-BITPLANE-TABLE-COLOR-00..-03 still FAIL at bpu=2
// (shres=1's reading is bit-identical to shres=0's for every register
// this RTL can still reach that way, same underlying reason as always:
// denise_colortable.v has no `shres` input at all) -- the pass/fail
// COUNT is unchanged (still 19 of 23 red), but registers 4-15's own
// FAIL is now honest ("cannot be tested," not "tested and found wanting
// on an out-of-spec configuration") rather than an artefact of the same
// kind SHRES-REINTERPRETS-SAME-REGISTERS's own witness was.
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
// (superseded once the RTL was opened, see below) was derived from, and
// what SHRES-TWO-BITS-RESOLUTION tests directly: the table implies no
// single displayed pixel should ever be governed by more than two of a
// nibble's four physical bits -- measured false, see RESULTS.
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
// measurement method itself. SELECT-REGISTER-ADDRESSES-CORRECTLY (no
// claim id, added after the RTL was opened -- see RESULTS) is a fourth
// control on the SPECIFIC measurement basis (bpu=4, select_register())
// the sixteen-register sweep below actually uses, proving that mechanism
// itself reaches three distinct registers correctly before any of its
// nineteen red results are trusted.
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
//   SELECT-REGISTER-ADDRESSES-CORRECTLY (0 ids, no page)
//   OCS-BASELINE-0RGB-FORMAT (4 ids)              -- C-shres-color.c4-c7
//   SHRES-REINTERPRETS-SAME-REGISTERS (2 ids)      -- C-shr.c06,
//                                                      C-superhires-color.c1
//   SHRES-BITPLANE-TABLE-COLOR-00 .. -0F (16 assertions, 288 ids total):
//     each cites its own row id (C-shrc.rXX) plus its own sixteen
//     C-shres-bpl-bits.cNNN bit ids (272 ids across the sixteen); COLOR-00,
//     -05, -0A and -0F (the four diagonal, single-source-colour rows)
//     additionally cite that source colour's own legend row and its three
//     symbol ids (C-shrc.srcbN + C-shres-symbols.cNN, 16 ids total). Of
//     these sixteen, -00..-03 are live measurements (bpu=2, corrected --
//     see BITPLANE COUNT CORRECTION above); -04..-0F are declined
//     (checks.expect(false, ...), unreachable under any compliant bpu),
//     ids still cited in full either way.
//   SHRES-USES-SIXTEEN-LOWER-REGISTERS (2 ids)     -- C-shrc.c04, c05,
//     asserted as an aggregate over the sixteen sweeps' own pass/fail.
//   SHRES-TWO-BITS-RESOLUTION (2 ids)              -- C-shrc.c01, c02,
//     derived from COLOR-01's own sweep data above, no new stimulus.
//   Tally: 4 (OCS-BASELINE) + 2 (REINTERPRETS) + 288 (the sixteen table
//   sweeps, rows+bits+legend) + 2 (SIXTEEN-LOWER-REGISTERS) + 2 (TWO-BITS-
//   RESOLUTION) = 298 distinct ids. Declined: 3. 298 + 3 = 301.
//
// RESULTS (RTL OPENED, built and run under WSL/Verilator 5.052, 11.09.2026;
// re-confirmed after the WITNESS FIX and BITPLANE COUNT CORRECTION above,
// same date): 4 passed, 19 failed (23 assertions total), unchanged from
// the pre-correction count -- CONTROL-KNOWN-VALUES-ARRIVE, FIXTURE-DATA-
// PATH-LIVE, OCS-BASELINE-0RGB-FORMAT and SELECT-REGISTER-ADDRESSES-
// CORRECTLY (the four controls, none carrying a claim id) PASS. (A fifth
// pass, SHRES-BPL01-BIT11-ISOLATES-RED, was added in a later defect-
// correction pass the same date -- see DEFECT CORRECTION below; current
// totals are 5 passed, 19 failed, 24 assertions.)
// SHRES-REINTERPRETS-SAME-REGISTERS, all sixteen SHRES-BITPLANE-TABLE-
// COLOR-00..-0F entries, SHRES-USES-SIXTEEN-LOWER-REGISTERS and
// SHRES-TWO-BITS-RESOLUTION (19 assertions, all 298 cited ids between
// them) FAIL. SHRES-REINTERPRETS-SAME-REGISTERS now uses a discriminating
// witness (COLOR01=0x0C39, each nibble's own top two bits differing from
// its own bottom two bits) in place of the old all-ones 0x0FFF that could
// not have gone green against any implementation -- re-measured FAIL
// (ocs=shres=0xCC3399), the finding properly established this time, not
// withdrawn. Of the sixteen SHRES-BITPLANE-TABLE-COLOR-XX entries, only
// -00 through -03 are live measurements now (bpu corrected 4->2, the
// manual's own SuperHires plane-depth ceiling, p.300); every "unused bits
// 15-12" half is consistent with the table (the plain 12-bit truncation
// the manual's own table also shows as unused), and every "used bits
// 11-0 must make shres=1's own reading differ from shres=0's own reading
// for that same bit" half fails, for all four still-live registers
// without exception -- denise_colortable.v (l.7-69, read in full) has no
// `shres` input at all, so shres=1 and shres=0 produce bit-identical
// output for every one of these 48 (4 registers x 12 bits) measurements.
// -04 through -0F are declined: their own index bits need plane 3 and/or
// 4 live, which denise.v forces to constant 0 whenever BPLCON0 encodes a
// compliant SuperHires depth (BPU<=2) at all, so no live measurement can
// reach them under any configuration the manual permits -- kept red by
// construction, ids still cited in full, not silently dropped.
// SHRES-TWO-BITS-RESOLUTION measured 4 of COLOR01's own four B-nibble
// bits as independently significant under shres=1 (not <=2), unaffected
// by either correction (COLOR01 was already reachable at bpu<=2). This is
// the finding the build brief anticipated ("If SuperHires is not
// implemented at this boundary at all, that is a finding, not a reason to
// weaken the test"): the ECS SuperHires bitplane colour encoding of p.301
// has no counterpart in this RTL at the denise/denise_bitplanes/
// denise_colortable/denise_playfields boundary -- `shres` only ever
// changes the bitplane shifter's own shift RATE
// (denise_bitplane_shifter.v:206-219), never which colour-table address
// is formed or how a stored 12-bit register value is interpreted. No
// assertion's expected value or oracle text was changed to reach this
// result -- every "FAIL" above is denise.v disagreeing with the manual,
// kept red.
//
// DEFECT CORRECTION (independent validator, this agent, 11.09.2026): the
// validator found that SHRES-BITPLANE-TABLE-COLOR-01's own per-bit check
// (in the sweep above) only asks whether shres=1's reading differs from
// shres=0's reading SOMEWHERE in the RGB word for a given bit -- for
// C-shres-bpl-bits.c21 ("COLOR01 bit 11 carries symbol g from bitplane
// color 1 red"), that check never confirms the difference actually lands
// in the RED channel the symbol names. Added SHRES-BPL01-BIT11-ISOLATES-
// RED (below, no page beyond 301, isolates C-shres-bpl-bits.c21 on its
// own): with every other bit at 0, bit 11 alone measurably moves ONLY the
// red channel (0x880000 vs a 0x000000 baseline) -- PASSES, confirming the
// position-to-channel half of the claim is real and RTL-observable. The
// colour-1-vs-colour-0 half of the claim is not independently observable
// at this boundary (denise_colortable.v reads a register's R nibble as
// one undifferentiated 4-bit value, shres or not) and is not re-tested by
// the new assertion; SHRES-BITPLANE-TABLE-COLOR-01's own sweep above
// already carries that half and stays red for exactly that reason. No
// existing assertion's oracle text or expected value was changed.
//
// ONE HARNESS BUG FOUND AND FIXED HERE (methodology, not the oracle): the
// spec-only commit's Harness never opened denise's own display window
// (DIWSTRT/DIWSTOP/strhor) -- with hdiwstrt=hdiwstop=0 at reset, `window`
// never toggles and denise.v:492-493 forces every pixel to border colour
// (COLOR00) regardless of what is written or driven, making the first
// build's CONTROL-KNOWN-VALUES-ARRIVE itself fail (both readings showed
// COLOR00). Fixed by opening a wide window once per Harness (DIWSTRT=4,
// DIWSTOP=0xF0, one strhor pulse) -- see the Harness constructor. A
// second, deeper timing bug followed: denise_bitplane_shifter.v's real
// output passes through a second 64-bit "scroller" and an 8-bit
// "sh_scroller", each read at a scroll-derived index, and a BPLxDAT
// write only ever supplies sixteen genuine bits (chip48_fmode is forced
// to 48'h0 at fmode=0) -- a single settled load only stays "steady" for
// sixteen ticks, which shres's shift-every-tick rate exhausts before a
// generous but one-shot settle margin could safely sample. Fixed by
// repeating the plane-1 trigger write continuously (every ~5 ticks, well
// under the sixteen-tick budget) instead of loading once and waiting --
// see select_register() and drive_plane1(). A third bug, in the
// assertion logic itself (not the stimulus): the sweep's first working
// build compared shres=1's reading against its OWN baseline (`vs !=
// base_shres`) instead of against shres=0's reading for the identical
// bit (`vs != vp`) -- the former passes trivially for any real,
// connected register bit regardless of whether shres does anything
// special, and produced a false "16 passed" before this was caught and
// fixed by re-reading the assertion's own prose against its own code.
//
// MUTATION LOG (every PASSING assertion below was actually mutated, via
// its own `-CFLAGS -DMUT_*=1` build flag, rebuilt and rerun under WSL/
// Verilator, confirmed exactly that one assertion red -- 3 passed/20
// failed in every case, the other 22 unaffected -- then rebuilt with no
// flags and reconfirmed the clean 4-passed/19-failed baseline. Every
// assertion already red at baseline gets a "none impossible" entry
// instead, per the build brief's own convention):
//
// MUTATION CONTROL-KNOWN-VALUES-ARRIVE stimulus red: COLOR01's write
//   changed from 0x00F0 to 0x0F00 (identical to COLOR00) -> FAIL ("3
//   passed, 20 failed"), only CONTROL-KNOWN-VALUES-ARRIVE red (v0==v1==
//   0xFF0000, no longer distinct). Reverted: 4/19.
// MUTATION FIXTURE-DATA-PATH-LIVE stimulus red: the plane-1 drive changed
//   from drive_plane1(true) to drive_plane1(false) (never reaches colour
//   index 1) -> FAIL ("3 passed, 20 failed"), only FIXTURE-DATA-PATH-LIVE
//   red (driven stayed 0x000000, not 0xFFFFFF). Reverted: 4/19.
// MUTATION OCS-BASELINE-0RGB-FORMAT stimulus red: COLOR01's write changed
//   from 0x0A5C to 0x0000 (no longer a distinguishable pattern) -> FAIL
//   ("3 passed, 20 failed"), only OCS-BASELINE-0RGB-FORMAT red (measured
//   top nibbles 0x000, expected 0xA5C). Reverted: 4/19. Expected-value
//   correction (this agent, 11.09.2026, see the assertion's own comment
//   above): the check now asserts only the top nibble of each 8-bit
//   component (0xA5C), not the full byte (0xAA55CC) -- the old full-byte
//   expectation baked in denise_colortable.v:56-58's own nibble-
//   duplication choice, which no printed page this package cites states;
//   re-ran this same mutation against the corrected check, same result.
// MUTATION SELECT-REGISTER-ADDRESSES-CORRECTLY stimulus red: COLOR0C's
//   write changed from 0x00F0 to 0x0F00 (identical to COLOR03) -> FAIL
//   ("3 passed, 20 failed"), only SELECT-REGISTER-ADDRESSES-CORRECTLY red
//   (v3==vC==0xFF0000, no longer distinct). Reverted: 4/19.
// WITNESS FIX (SHRES-REINTERPRETS-SAME-REGISTERS, this agent, 11.09.2026):
// the previous witness wrote COLOR01=0x0FFF (all ones in every nibble) --
// rearranging which bits feed which colour slot cannot change an all-ones
// value, so that witness could not have gone green even against a perfect
// SuperHires implementation; its FAIL proved nothing. Replaced with
// COLOR01=0x0C39 (R=0xC,G=0x3,B=0x9 -- each nibble's own top two bits
// differ from its own bottom two bits, the same slot-A/slot-B-differ
// reasoning the sibling ch3-shres-sprite-colour-encoding package's own
// witness already uses). Re-measured: ocs=shres=0xCC3399 -- STILL FAILS,
// now for the first time on a witness that could have shown a pass. The
// finding stands, more firmly than before.
// MUTATION SHRES-REINTERPRETS-SAME-REGISTERS stimulus red: -DMUT_REINTERP=1 reverts BOTH harnesses' COLOR01 write from 0x0C39 back to the old non-discriminating 0x0FFF -> still FAIL, "4 passed, 19 failed" unchanged (ocs=shres=0xFFFFFF under the mutation vs ocs=shres=0xCC3399 at baseline) -- confirms the flag correctly reproduces the old witness, and that this assertion's FAIL does not depend on which witness is used: denise_colortable.v has no `shres` input for any stimulus, discriminating or not, to reach. Reverted: baseline 4/19 restored.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-00 none impossible: already red at
//   baseline (same reason; this register's own twelve used-bit checks
//   all fail identically). bpu corrected 4->2 (BITPLANE COUNT CORRECTION
//   above); still reachable with planes 1-2 alone, still red, same reason.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-01 none impossible: already red at
//   baseline, same reason. bpu corrected 4->2, still red, same reason.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-02 none impossible: already red at
//   baseline, same reason. bpu corrected 4->2, still red, same reason.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-03 none impossible: already red at
//   baseline, same reason. bpu corrected 4->2, still red, same reason.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-04 none impossible: already red at
//   baseline, by construction (checks.expect(false, ...) -- unreachable
//   under any compliant bpu, see BITPLANE COUNT CORRECTION above); there
//   is nothing green here to break.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-05 none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-06 none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-07 none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-08 none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-09 none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-0A none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-0B none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-0C none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-0D none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-0E none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-BITPLANE-TABLE-COLOR-0F none impossible: already red at
//   baseline, by construction, same reason as -04.
// MUTATION SHRES-USES-SIXTEEN-LOWER-REGISTERS none impossible: already
//   red at baseline (it aggregates the sixteen sweeps above, all sixteen
//   already red).
// MUTATION SHRES-TWO-BITS-RESOLUTION none impossible: already red at
//   baseline (COLOR01's own sweep already shows all four B-nibble bits
//   independently significant).
// MUTATION SHRES-BPL01-BIT11-ISOLATES-RED stimulus red: -DMUT_BPL01BIT11RED=1 replaces the bit-11 stimulus with bit 7 (a green-channel bit); the measured reading no longer moved the red channel, so the assertion's own red-channel-isolation check went red alone. Reverted: green baseline restored.

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <verilated.h>
#include "Vdenise.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif
#ifndef MUT_OCSBASELINE
#define MUT_OCSBASELINE 0
#endif
#ifndef MUT_SELECTREG
#define MUT_SELECTREG 0
#endif
#ifndef MUT_REINTERP
#define MUT_REINTERP 0
#endif
#ifndef MUT_BPL01BIT11RED
#define MUT_BPL01BIT11RED 0
#endif

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
constexpr uint8_t kAddrBpl3Dat = 0x8A; // 9'h114
constexpr uint8_t kAddrBpl4Dat = 0x8B; // 9'h116
constexpr uint8_t kAddrDiwstrt = 0x47; // 9'h08e
constexpr uint8_t kAddrDiwstop = 0x48; // 9'h090

// BPLCON0 field positions (denise.v field decode, l.180-192, confirmed on
// opening the RTL: hires=bit15, bpu=bits14-12, shres=bit6, ecsena=bit0).
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

std::string hex6(uint32_t v) {
    std::ostringstream os;
    os << "0x" << std::hex << std::uppercase << std::setw(6) << std::setfill('0')
       << (v & 0xFFFFFFu);
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
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();

        // RTL OPENED: denise.v's `hires`/`shres` (l.50-51) are module
        // OUTPUTS (assign hires = bplcon0[15]; assign shres = bplcon0[6];
        // l.180,187), not inputs -- the spec-only commit's prediction
        // that these were don't-care input pins was wrong; there is
        // nothing for this Harness to drive there at all, so nothing is
        // driven. denise.v also gates every non-border pixel behind its
        // own display-window state machine (window/window_ena, l.328-340;
        // clut_data forced to COLOR00 -- "border colour" -- whenever
        // !window_ena, l.492-493) -- a mechanism no claim in this
        // package's inventory is about, but which this Harness must open
        // once, or every measurement below silently reads COLOR00
        // regardless of what is actually written or displayed. DIWSTRT=4,
        // DIWSTOP=0xF0 (-> hdiwstop=0x1F0=496, hdiwstrt=4) opens a window
        // wide enough that hpos (free-running from 2, advancing one per
        // clk7_en tick, wrapping at 512) stays inside it for this
        // Harness's entire, short lifetime.
        write_reg(kAddrDiwstrt, 0x0004);
        write_reg(kAddrDiwstop, 0x00F0);
        while (phase != 0) tick();
        dut->strhor = 1;
        tick();
        dut->strhor = 0;
        tick(32); // hpos climbs from 2 past hdiwstrt(4); window_ena settles to 1.
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

    // RTL OPENED (denise.v, denise_bitplanes.v, denise_colortable.v, all
    // read after the spec-only commit above): denise_colortable.v declares
    // NO `shres` port at all (grep-confirmed, zero occurrences) and its
    // read address is simply `select_xored = select` (l.27), with `select`
    // being `clut_data`, which for a single playfield is
    // `plfdata = bpldata[8:1]` DIRECTLY (denise_playfields.v:100, dpf=0
    // path) -- i.e. plane N's own live bit is colour-table address bit
    // (N-1), with NO combining of adjacent bitplane-clock samples anywhere
    // in this call graph. denise_bitplanes.v's only use of `shres` is the
    // per-plane shift-rate mux (l.298-303: shres -> shift every tick); it
    // does not touch which colour-table address is formed or how many
    // register bits are read. The ADDRESSING MECHANISM prediction above
    // (index = 4*colorODD + colorEVEN via a shared two-tick colour-table
    // fetch) has NO RTL counterpart to correct -- there is no shared-fetch,
    // no even/odd split, and the sixteen lower registers are not reached
    // through any bitplane-driven multiplexing at all. Registers 4-15 are
    // reachable ONLY by driving MORE than two bitplanes directly as a
    // plain 4-bit index (exactly like an ordinary, non-SuperHires 4-
    // bitplane display) -- bpu=4, planes 1-4 forming the register number
    // bit-for-bit (plane N -> index bit (N-1)), matching this repository's
    // OWN ehb_en gate (l_bpu==6 only, so bpu=4 does not trip EHB) and the
    // single-playfield pass-through above. select_register() below drives
    // that four-plane index directly, addressing any of the sixteen
    // registers with no addressing-timing assumption left to correct.
    void set_mode(bool shres_on, int bpu) {
        write_reg(kAddrBplcon0,
                   static_cast<uint16_t>((shres_on ? kBplcon0Shres : 0) |
                                          ((bpu & 7) << 12) | kBplcon0Ecsena));
        tick(32);
    }

    // RTL OPENED (denise_bitplane_shifter.v): a genuinely steady reading
    // needs the shifter kept freshly loaded FASTER than shres's own
    // fastest drain rate (shift every 28MHz tick, sixteen real bits per
    // load -- a sixteen-tick budget), not just settled once and left
    // alone -- see select_register()'s own comment for the full pipeline.
    // Re-triggering a BPL1DAT write reloads ALL FOUR shifters from their
    // OWN, already-buffered content (denise_bitplanes.v:467) at ~5 ticks
    // per write_reg() call, comfortably inside that sixteen-tick budget,
    // so repeating ONLY the plane-1 trigger (no need to re-write planes
    // 2-4's own buffers, which do not change) keeps every plane's shifter
    // continuously fresh. Forty repeats (~200 ticks) first flush the
    // pipeline's own reset-zero history, then keep it saturated; sampling
    // happens immediately after the last trigger, before any drain can
    // start.
    void drive_plane1(bool high) {
        uint16_t p1 = high ? 0xFFFF : 0x0000;
        for (int i = 0; i < 40; ++i) write_reg(kAddrBpl1Dat, p1);
    }

    // RTL OPENED (denise_bitplane_shifter.v, read in full): a plane's
    // live `out` bit is not a direct tap of its 64-bit shift register --
    // it passes through a SECOND 64-bit "scroller" register (fed one bit
    // per `shift` pulse from the main shifter's MSB, l.238) read out at
    // a scroll-derived `select` index (l.209-217, masked to 0-15 by
    // fmode_mask when fmode=0), and THEN through an 8-bit "sh_scroller"
    // that samples that value every 28MHz tick unconditionally
    // (l.260-261) and is itself read at a THIRD scroll-derived index
    // (`sh_select`, l.248-256). Because BPL1DAT..BPL4DAT's own 64-bit
    // shift-register load is only ever 16 real bits wide (the write's own
    // data16) concatenated with 48 FORCED-ZERO bits (chip48_fmode's
    // "default" case at fmode=0, denise_bitplanes.v:374) -- not a genuine
    // 64-bit-wide steady pattern -- a single load only guarantees a
    // steady, addressing-position-independent reading for the sixteen
    // ticks after it, and BPLCON1's own reset value (16'h3300, not zero)
    // makes the exact scroll-derived read index unpredictable enough that
    // waiting it out precisely is not worth doing. Loads BPL1DAT..BPL4DAT
    // with the given 16-bit shift patterns (all-1s or all-0s per bit --
    // steady, addressing-timing-agnostic once refreshed faster than the
    // sixteen-tick window can run out) and REPEATS the load several times
    // with a short gap, so whichever index this three-stage pipeline
    // settles on is reading from an always-fresh, always-identical
    // sixteen-bit window by the time the caller samples. Every OTHER
    // plane is pre-loaded (silently) before each BPL1DAT write, since a
    // BPL1DAT write's load strobe reaches every plane's shifter at once
    // (denise_bitplanes.v:467, confirmed unchanged after reading it).
    void select_register(int reg) {
        uint16_t p1 = (reg & 1) ? 0xFFFF : 0x0000;
        uint16_t p2 = (reg & 2) ? 0xFFFF : 0x0000;
        uint16_t p3 = (reg & 4) ? 0xFFFF : 0x0000;
        uint16_t p4 = (reg & 8) ? 0xFFFF : 0x0000;
        write_reg(kAddrBpl2Dat, p2); // planes 2-4's own buffers do not
        write_reg(kAddrBpl3Dat, p3); // change again below -- only the
        write_reg(kAddrBpl4Dat, p4); // plane-1 trigger needs repeating.
        for (int i = 0; i < 40; ++i) write_reg(kAddrBpl1Dat, p1);
    }

    // denise.v's red/green/blue ports are 8 bits each (module header
    // l.44-46), not 4 -- confirmed on opening the RTL; the spec-only
    // commit's 4-bit assumption is corrected here, in the harness only
    // (no oracle text changed). denise_colortable.v:56-58 packs each
    // 8-bit component as {nibble,nibble} -- the SAME 4-bit COLORxx
    // component value written twice (wr_ws=2'b11 whenever loct=0, i.e.
    // always in this ECS-only, aga=0 package) -- a doubled-nibble 4->8
    // bit DAC scale-up, not two independent colour-table entries.
    uint32_t rgb() const {
        return (static_cast<uint32_t>(dut->red) << 16) |
               (static_cast<uint32_t>(dut->green) << 8) |
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
    // below is trusted to read one back. (bpu=1, single plane -- colour
    // index 0 or 1 directly, unaffected by the register-addressing
    // correction below.)
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0F00); // COLOR00 R-nibble=0xF -> doubled 0xFF0000
#if MUT_CONTROL
        h.write_color(1, 0x0F00); // MUTATION: same as COLOR00, no longer distinct
#else
        h.write_color(1, 0x00F0); // COLOR01 G-nibble=0xF -> doubled 0x00FF00
#endif
        uint32_t v0 = h.rgb(); // colour index 0 (no plane driven high)
        h.drive_plane1(true);
        uint32_t v1 = h.rgb(); // colour index 1 (plane 1 driven high)
        checks.expect(v0 != v1 && v0 == 0xFF0000 && v1 == 0x00FF00,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct, known "
            "colour-register writes (COLOR00=0x0F00, COLOR01=0x00F0) "
            "produce two distinct, correctly-positioned RGB readings (" +
            hex6(v0) + " then " + hex6(v1) + ") at denise's own "
            "red/green/blue pins as the displayed colour index changes, "
            "through the fixture's own register-write-to-pixel-output "
            "path, before any assertion below trusts it. No manual page: "
            "the control for every assertion in this file, on the same "
            "measurement basis.");
    }

    // =====================================================================
    // FIXTURE-DATA-PATH-LIVE (no claim id) -- proves the measured-working
    // c1/c3 four-phase pattern actually delivers a loaded, non-zero
    // bitplane sample, not silently-dead colour 0 (THE C1/C3 TRAP above).
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0000);
        h.write_color(1, 0x0FFF); // COLOR01 = white, unmistakable if reached
        uint32_t baseline = h.rgb();
#if MUT_FIXTURE
        h.drive_plane1(false); // MUTATION: plane 1 left low, never reaches colour index 1
#else
        h.drive_plane1(true); // plane 1 all-1s -> colour index 1
#endif
        uint32_t driven = h.rgb();
        checks.expect(baseline == 0 && driven == 0xFFFFFF,
            "[control] FIXTURE-DATA-PATH-LIVE: a BPL1DAT write of an "
            "all-ones plane reaches the bitplane shifter and is displayed "
            "as COLOR01 (" + hex6(driven) + ") within 24 ticks of the "
            "four-phase clk7_en=(phase==0), c1=(phase==0||phase==1), "
            "c3=(phase==1||phase==2) pattern -- the write-strobe and "
            "shifter-load gating this module uses (c1&c3, !c1&&!c3) is "
            "never permanently starved the way a disjoint two-phase "
            "pattern would starve it. No manual page: harness self-check.");
    }

    // =====================================================================
    // OCS-BASELINE-0RGB-FORMAT (p.301) -- the control the build brief asks
    // for: the plain, non-SuperHires 0x0RGB format, on the same write/read
    // basis as every SuperHires measurement below.
    // EXPECTED VALUE CORRECTED (this agent, 11.09.2026): the previous
    // expected value 0xAA55CC baked in denise_colortable.v:56-58's own
    // choice of HOW a 4-bit nibble becomes this RTL's 8-bit red/green/blue
    // port value (duplicating it into both nibble halves, "{nibble,
    // nibble}") -- an RTL-read implementation detail, not anything the
    // printed pages (295, 296, 299, 300, 301 -- the only pages this
    // package cites) state. Those 8-bit ports do not exist in the 1985
    // chip's own analog video path the manual describes at all; nothing
    // printed says how a stored 4-bit component maps onto them. What the
    // page DOES support is narrower and does not need the RTL to state:
    // COLORxx stores a 0x0RGB value, so the plain (non-SuperHires) reading
    // of a written 4-bit R/G/B nibble must appear, unaltered, as the TOP
    // four bits of whichever output width this boundary happens to use --
    // asserted below without assuming anything about the bottom four bits
    // of each 8-bit component, which the printed page never specifies.
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0000);
#if MUT_OCSBASELINE
        h.write_color(1, 0x0000); // MUTATION: no longer a distinguishable pattern
#else
        h.write_color(1, 0x0A5C); // R=0xA, G=0x5, B=0xC -- distinguishable nibbles
#endif
        h.drive_plane1(true);
        uint32_t v = h.rgb();
        uint32_t rTop = (v >> 20) & 0xF, gTop = (v >> 12) & 0xF, bTop = (v >> 4) & 0xF;
        std::ostringstream topNibbles;
        topNibbles << std::hex << std::uppercase << rTop << gTop << bTop;
        checks.expect(rTop == 0xA && gTop == 0x5 && bTop == 0xC,
            "OCS-BASELINE-0RGB-FORMAT (p.301, " +
            join({"C-shres-color.c4", "C-shres-color.c5", "C-shres-color.c6",
                  "C-shres-color.c7"}) +
            "): with shres=0, COLOR01 written as 0x0A5C (R=0xA, G=0x5, "
            "B=0xC) reproduces the plain 4-bit-per-component 0x0RGB format "
            "-- each written nibble appears, unaltered, as the top four "
            "bits of denise's red/green/blue pins (measured top nibbles 0x" +
            topNibbles.str() + ", expected 0xA5C; full 8-bit-per-component "
            "reading " + hex6(v) + ") -- no SuperHires multiplexing. "
            "Deliberately does NOT assert the bottom four bits of each "
            "8-bit component: this boundary's ports are 8 bits wide (an "
            "RTL/interface fact, not a manual claim), and the printed "
            "pages cited by this package say nothing about how a stored "
            "4-bit value fills the other four -- asserting a specific "
            "fill (e.g. nibble duplication, denise_colortable.v:56-58's "
            "own choice) would be an RTL-derived expectation dressed as a "
            "page one. 'Instead of 4 bits of red, green, and blue for "
            "bitplane registers 0-3 stored as 0x0RGB in four color "
            "registers, SuperHires bitplane colors are specially "
            "encoded...' read as describing what happens WITHOUT "
            "SuperHires active. Control: same write-register/read-RGB "
            "measurement basis as every SuperHires assertion below.");
    }

    // =====================================================================
    // SHRES-REINTERPRETS-SAME-REGISTERS (p.297) -- the same register
    // contents, same colour index, read differently once shres=1.
    // WITNESS CORRECTED (was: COLOR01=0x0FFF, all ones in every nibble).
    // The p.301 table's own mechanism is a REARRANGEMENT of which stored
    // bits feed which colour slot, not a value transform -- rearranging a
    // register made entirely of ones yields ones no matter which bits move
    // where, so the old witness could not have gone green even against a
    // perfect SuperHires implementation; its FAIL proved nothing about
    // NanoMig. Fixed the same way the sibling ch3-shres-sprite-colour-
    // encoding package's own witness does (buildRegValue: slot A = R2/G1/B3,
    // slot B = the bitwise complement, so a leak or rearrangement reads as a
    // different, wrong value, never an accidental match): COLOR01 is now
    // written so each nibble's own top two bits differ from its own bottom
    // two bits (R=0xC=1100, G=0x3=0011, B=0x9=1001). Any mechanism that
    // reads only two of a nibble's four physical bits and duplicates them
    // (p.301's own "only two bits of ... resolution" claim) -- whichever
    // two bits it picks -- necessarily disagrees with the plain, straight-
    // through 4-bit OCS reading, for R, G and B alike: this witness is
    // discriminating by construction, not by accident.
    // =====================================================================
    {
#if MUT_REINTERP
        const uint16_t kWitnessColor1 = 0x0FFF; // MUTATION: back to the non-discriminating all-ones witness (the old bug) for BOTH harnesses
#else
        const uint16_t kWitnessColor1 = 0x0C39;
#endif
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0000);
        h.write_color(1, kWitnessColor1);
        h.drive_plane1(true);
        uint32_t ocs = h.rgb();
        Harness h2;
        h2.write_color(0, 0x0000);
        h2.write_color(1, kWitnessColor1);
        h2.set_mode(true, 1);
        h2.drive_plane1(true);
        uint32_t shres = h2.rgb();
        checks.expect(ocs != shres,
            "SHRES-REINTERPRETS-SAME-REGISTERS (p.297, " +
            join({"C-shr.c06", "C-superhires-color.c1"}) +
            "): COLOR01 written as 0x0C39 (R=0xC, G=0x3, B=0x9 -- each "
            "nibble's own top two bits differ from its own bottom two "
            "bits, a discriminating witness: no rearrangement of WHICH "
            "bits feed a colour slot can accidentally reproduce the plain "
            "reading), same colour index, same read path -- shres alone "
            "changes nothing: OCS/plain reading " + hex6(ocs) +
            " vs SuperHires reading " + hex6(shres) +
            " (RTL OPENED: denise_colortable.v declares no `shres` port "
            "at all, so this is not a timing accident -- there is no "
            "circuit for shres to change this value through). "
            "'There are new color register interpretations as part of "
            "the SuperHires mode.'");
    }

    // =====================================================================
    // SELECT-REGISTER-ADDRESSES-CORRECTLY (no claim id) -- a second,
    // mechanism-specific control: every SHRES-BITPLANE-TABLE-COLOR-XX
    // sweep below (and SHRES-USES-SIXTEEN-LOWER-REGISTERS,
    // SHRES-TWO-BITS-RESOLUTION) rests entirely on select_register()'s
    // bpu=4, four-plane addressing reaching the RIGHT register each
    // time, on the plain (shres=0) basis this control uses -- if that
    // addressing were stuck, off-by-one, or otherwise broken, every red
    // result below would be worthless. Three distinct registers (03, 0C,
    // 0F), three distinct plain 0x0RGB patterns, same bpu=4/
    // select_register path the whole sweep uses.
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 4);
        h.write_color(3, 0x0F00);
#if MUT_SELECTREG
        h.write_color(0xC, 0x0F00); // MUTATION: same as COLOR03, no longer distinct
#else
        h.write_color(0xC, 0x00F0);
#endif
        h.write_color(0xF, 0x000F);
        h.select_register(3);
        uint32_t v3 = h.rgb();
        h.select_register(0xC);
        uint32_t vC = h.rgb();
        h.select_register(0xF);
        uint32_t vF = h.rgb();
        checks.expect(v3 == 0xFF0000 && vC == 0x00FF00 && vF == 0x0000FF &&
                          v3 != vC && vC != vF && v3 != vF,
            "[control] SELECT-REGISTER-ADDRESSES-CORRECTLY: three distinct "
            "registers (COLOR03=0x0F00, COLOR0C=0x00F0, COLOR0F=0x000F), "
            "addressed via the same bpu=4/select_register four-plane "
            "index every sweep below uses, each read back its own "
            "distinct, correctly-positioned value (" + hex6(v3) + ", " +
            hex6(vC) + ", " + hex6(vF) + ") and no other's -- the "
            "addressing mechanism itself is not stuck, off-by-one, or "
            "otherwise broken. No manual page: harness self-check, on the "
            "same measurement basis as every SHRES-BITPLANE-TABLE-COLOR-XX "
            "assertion below.");
    }

    // =====================================================================
    // SHRES-BITPLANE-TABLE-COLOR-00 .. -0F -- the main sweep. RTL OPENED:
    // denise_colortable.v (l.7-69, read in full) has no `shres` input and
    // no logic that depends on it; `select` (its colour-table address) is
    // `clut_data`, which for a single playfield is `plfdata =
    // bpldata[8:1]` directly (denise_playfields.v:100) -- plane N's own
    // live serial bit is address bit (N-1), with no combining of adjacent
    // bitplane-clock samples anywhere in the call graph
    // (denise_bitplanes.v's only use of shres, l.298-303, is the per-plane
    // shift RATE). The ADDRESSING MECHANISM prediction in the spec-only
    // commit (a shared two-tick fetch, index=4*colorODD+colorEVEN) has no
    // RTL counterpart -- there is no such circuit. Registers 4-15 are
    // reached the only way this RTL supports: a plain four-bitplane index
    // (bpu=4, planes 1-4 = the register number bit-for-bit), exactly like
    // an ordinary non-SuperHires 4-bitplane display -- select_register()
    // above. For each of the sixteen registers: walking-bit stimulus into
    // COLORxx, addressed identically, measured under shres=0 and shres=1.
    // Per-bit check: an "unused" bit (15-12, printed '.' in the table)
    // must move nothing under EITHER mode (true under both a plain and a
    // SuperHires reading -- and true here, since data_in[11:0] truncation
    // is unconditional, denise_colortable.v:32); a "used" bit (11-0) must
    // make shres=1's own reading differ from shres=0's own reading for
    // that SAME bit -- the direct, falsifiable meaning of "There are new
    // color register interpretations as part of the SuperHires mode" (p.
    // 297) applied bit-by-bit. Measured: it never does -- shres cannot
    // reach this signal path at all, so every used bit's shres=1 reading
    // is bit-identical to its shres=0 reading.
    // =====================================================================
    std::array<bool, 16> regOk{};
    bool reg1_bit_used_count[4] = {false, false, false, false}; // B-nibble, for SHRES-TWO-BITS-RESOLUTION
    for (int reg = 0; reg < 16; ++reg) {
        std::string label = "SHRES-BITPLANE-TABLE-COLOR-" + hexReg(reg) +
            " (p.301, " + std::string(kRowIds[reg]) + ", " + join16(kBitIds[reg]) + ")";
        for (auto& lc : kLegend) {
            if (lc.reg == reg) {
                label += ", legend " + std::string(lc.srcb) + ", " +
                         join({lc.sym[0], lc.sym[1], lc.sym[2]});
            }
        }

        if (reg >= 4) {
            // UNREACHABLE UNDER A COMPLIANT BPU -- see BITPLANE COUNT
            // CORRECTION in the header above. This register's own combined
            // bitplane-driven index needs plane 3 and/or plane 4 live
            // (index bits 2/3), but denise.v (l.367-370) forces
            // bpldata[3]/bpldata[4] to constant 0 whenever l_bpu<=2 --
            // i.e. whenever BPLCON0 encodes a page-300-compliant SuperHires
            // depth (BPU=1 or 2, "the maximum plane depth for a SuperHires
            // viewport is 2 bitplanes") at all. Registers 0-3 are still
            // measured below (planes 1-2 alone suffice); this one cannot
            // be, on ANY BPLCON0 configuration the manual allows for
            // SuperHires -- declined, not silently dropped, its own ids
            // still cited in full, the same convention the sibling
            // ch3-shres-sprite-colour-encoding package's own REG-0x10 uses
            // for an architecturally-unreachable register.
            regOk[reg] = false;
            label += ": register COLOR" + hexReg(reg) + "'s own combined "
                "bitplane-driven index needs plane 3 and/or plane 4 live "
                "(this register's own index bits 2/3) to select it, but "
                "page 300 caps SuperHires plane depth at BPU=1 or 2 ('The "
                "maximum plane depth for a SuperHires viewport is 2 "
                "bitplanes'; BPLCON0 bits 14-12, 'Depth of SuperHires mode "
                "(1 or 2)'), and denise.v forces bpldata[3]/bpldata[4] to "
                "constant 0 whenever l_bpu<=2 -- so this register can never "
                "be addressed via bitplane-driven selection under any "
                "BPLCON0 configuration the manual permits for SuperHires. "
                "Not measured via live display for that reason -- kept red "
                "rather than silently dropped.";
            checks.expect(false, label);
            continue;
        }

        // Registers 0-3: reachable with planes 1-2 alone -- bpu=2, the
        // manual's own maximum SuperHires plane depth (p.300), corrects
        // the previous bpu=4 (an independent validator's finding, checked
        // against the page and confirmed: SuperHires is capped at 2
        // bitplanes, so shres=1 combined with bpu=4 is a BPLCON0
        // configuration the manual never defines -- testing there proves
        // nothing about SuperHires conformance. See BITPLANE COUNT
        // CORRECTION in the header above.)
        Harness hbp;
        hbp.set_mode(false, 2);
        hbp.write_color(reg, 0x0000);
        hbp.select_register(reg);
        uint32_t base_plain = hbp.rgb();

        Harness hbs;
        hbs.set_mode(true, 2);
        hbs.write_color(reg, 0x0000);
        hbs.select_register(reg);
        uint32_t base_shres = hbs.rgb();

        bool all_ok = true;
        for (int bit = 15; bit >= 0; --bit) {
            Harness hp;
            hp.set_mode(false, 2);
            hp.write_color(reg, static_cast<uint16_t>(1u << bit));
            hp.select_register(reg);
            uint32_t vp = hp.rgb();

            Harness hs;
            hs.set_mode(true, 2);
            hs.write_color(reg, static_cast<uint16_t>(1u << bit));
            hs.select_register(reg);
            uint32_t vs = hs.rgb();

            bool used = (bit < 12);
            // Used bit: the manual claims a genuine SuperHires-specific
            // reinterpretation, so shres=1's OWN reading for this bit
            // must differ from shres=0's OWN reading for the identical
            // written content (vs != vp) -- NOT merely differ from its
            // own baseline (every real, connected 0x0RGB bit does that
            // regardless of any SuperHires-specific meaning, which would
            // make "vs != base_shres" pass trivially and prove nothing).
            // Unused bit: neither mode's reading should move at all.
            bool ok = used ? (vs != vp)
                           : (vp == base_plain && vs == base_shres);
            if (!ok) all_ok = false;

            if (reg == 1 && bit < 4) {
                reg1_bit_used_count[bit] = (vs != base_shres);
            }
        }
        regOk[reg] = all_ok;

        label += ": walking-bit sweep of COLOR" + hexReg(reg) +
                 " -- each of the sixteen bits must either move nothing "
                 "under shres=0 AND shres=1 alike (bits 15-12, printed "
                 "'.') or make the shres=1 reading differ from the "
                 "shres=0 reading for that same bit (bits 11-0, the "
                 "table's own letters) -- 'new color register "
                 "interpretations as part of the SuperHires mode' (p. "
                 "297) applied to this register's own twelve significant "
                 "bits.";
        checks.expect(all_ok, label);
    }

    // =====================================================================
    // SHRES-USES-SIXTEEN-LOWER-REGISTERS (p.301) -- aggregate over the
    // sixteen sweeps' own pass/fail: are all sixteen registers
    // independently live, distinct SuperHires colour storage, not just
    // bitplane-registers-0-3's plain 0x0RGB slots?
    // =====================================================================
    {
        bool all16 = true;
        for (bool ok : regOk) all16 = all16 && ok;
        checks.expect(all16,
            "SHRES-USES-SIXTEEN-LOWER-REGISTERS (p.301, " +
            join({"C-shrc.c04", "C-shrc.c05"}) +
            "): the sixteen per-register sweeps above (SHRES-BITPLANE-"
            "TABLE-COLOR-00 through -0F) would each need to independently "
            "confirm their own register's bits carry a genuine SuperHires-"
            "specific meaning for this to hold -- i.e. all sixteen of "
            "COLOR00-COLOR0F would need to be live, SHRES-reinterpreted "
            "colour storage, not the plain 0x0RGB slots RTL OPENED shows "
            "them to be. 'SuperHires bitplane colors are specially "
            "encoded in the sixteen lower color registers.'");
    }

    // =====================================================================
    // SHRES-TWO-BITS-RESOLUTION (p.301) -- derived from COLOR01's own
    // sweep above (no new stimulus): "only two bits of red, green and
    // blue color resolution per hires pixel" requires that, of a nibble's
    // four physical bits, only two are ever independently significant to
    // what a SuperHires pixel displays. Measured on COLOR01's B-nibble
    // (bits 3-0, all four printed as used letters k,l,e,f in the table):
    // does shres=1 show all four as independently significant (RTL
    // OPENED: it does, being the same plain 4-bit nibble as shres=0 --
    // see SHRES-BITPLANE-TABLE-COLOR-01 above), contradicting a genuine
    // two-bit-per-component ceiling?
    // =====================================================================
    {
        int usedCount = 0;
        for (bool u : reg1_bit_used_count) if (u) ++usedCount;
        checks.expect(usedCount <= 2,
            "SHRES-TWO-BITS-RESOLUTION (p.301, " +
            join({"C-shrc.c01", "C-shrc.c02"}) +
            "): of COLOR01's own four B-nibble bits (3-0), " +
            std::to_string(usedCount) + " independently move denise's "
            "blue output under shres=1 (measured inside SHRES-BITPLANE-"
            "TABLE-COLOR-01's own sweep, reused here, not a separate "
            "stimulus) -- 'only two bits of red, green and blue color "
            "resolution per hires pixel' requires at most two. 'SuperHires "
            "mode has a coarser granularity of color control than either "
            "Hires or Lores modes.'");
    }

    // =====================================================================
    // SHRES-BPL01-BIT11-ISOLATES-RED (p.301) -- an independent validator
    // found that SHRES-BITPLANE-TABLE-COLOR-01's own per-bit check above
    // only asks whether ANY part of the RGB reading differs between
    // shres=0 and shres=1 for a given bit -- it never confirms the
    // reading actually lands in the CHANNEL that bit's own printed symbol
    // names. This isolates C-shres-bpl-bits.c21 ("COLOR01 bit 11 carries
    // symbol g from bitplane color 1 red") directly: with every other bit
    // held at 0, does bit 11 alone move the RED channel specifically, and
    // leave green/blue untouched -- the position-to-channel half of the
    // claim, the part this boundary CAN observe. The other half -- that
    // this is specifically bitplane COLOUR 1's own red, distinct from
    // colour 0's own red at bit 9 (symbol a) -- has no independent
    // circuit to observe at all: this RTL reads a register's R nibble
    // (bits 11-8) as ONE undifferentiated 4-bit value (denise_colortable.v
    // has no shres input, confirmed above), so no measurement at this
    // boundary can tell "colour 1's own bit" apart from "colour 0's own
    // bit" within the same nibble -- that half is NOT re-tested here;
    // SHRES-BITPLANE-TABLE-COLOR-01's own per-bit sweep above already
    // carries it (and stays red, for exactly that reason).
    // =====================================================================
    {
        Harness hbit;
        hbit.set_mode(true, 2);  // shres=1, this package's own claimed mode
#if MUT_BPL01BIT11RED
        hbit.write_color(1, 0x0080);  // MUTATION: bit 7 (green) instead of bit 11 (red)
#else
        hbit.write_color(1, 0x0800);  // COLOR01 bit 11 alone
#endif
        hbit.select_register(1);
        uint32_t v_bit11 = hbit.rgb();

        Harness hbase;
        hbase.set_mode(true, 2);
        hbase.write_color(1, 0x0000);
        hbase.select_register(1);
        uint32_t v_base = hbase.rgb();

        bool red_moves = ((v_bit11 >> 16) & 0xFFu) != ((v_base >> 16) & 0xFFu);
        bool green_still = ((v_bit11 >> 8) & 0xFFu) == ((v_base >> 8) & 0xFFu);
        bool blue_still = (v_bit11 & 0xFFu) == (v_base & 0xFFu);

        checks.expect(
            red_moves && green_still && blue_still,
            "SHRES-BPL01-BIT11-ISOLATES-RED (p.301, C-shres-bpl-bits.c21): "
            "with every other bit of COLOR01 held at 0, bit 11 alone (" +
            hex6(v_bit11) + " vs baseline " + hex6(v_base) + ", shres=1) "
            "moves the RED channel and leaves green/blue untouched -- "
            "'O  01   .   .   .   .   g   h   a   b   i   j   c   d   k   "
            "l   e   f' (p.301) names bit 11 as symbol g, bitplane colour "
            "1's own red. This confirms the position-to-channel half of "
            "the claim (bit 11 is wired to red, not green or blue); it "
            "does NOT confirm the colour-1-distinct-from-colour-0 half "
            "(see this block's own comment above) -- that half stays red "
            "under SHRES-BITPLANE-TABLE-COLOR-01's own sweep.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
