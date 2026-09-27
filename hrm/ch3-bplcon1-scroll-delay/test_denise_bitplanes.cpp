// Test purpose: BPLCON1 as the horizontal-scroll delay register -- PF1H for
//   the odd bitplanes (1,3,5), PF2H for the even bitplanes (2,4,6), the
//   delay measured on the bitplane serial output, and the BPLxDAT registers
//   that feed the shifters. Package ch3-bplcon1-scroll-delay, fixture
//   denise_bitplanes (one Verilator build, this module + its own
//   denise_bitplane_shifter and bitplane_ram instances, no other module).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 42, 53, 57, 60, 67, 71, 72, 80, 81, 82, 83, 84, 85, 90, 271, 272, 291.
// Non-goals: vertical scrolling via bitplane pointer/modulo registers (pp.
//   80-81), Copper-driven pointer resets during blanking (p.80), and the
//   -2/-4 pointer and modulo adjustment for the non-scrolled playfield (p.
//   72) are Agnus DMA-address concepts with no pointer, modulo or Copper
//   port anywhere in this module's boundary -- declined below, not resolved.
//   The display-window edge behaviour of rightward scrolling (p. 82) needs
//   DIWSTRT/DIWSTOP, also outside this boundary -- declined. Figures 3-25
//   and 3-27 (pp. 84) describe DMA memory-picture addressing, not a
//   Denise-observable quantity -- declined. AGA's 6-bit-per-field BPLCON1
//   extension and fmode wide-fetch modes are out of scope: every stimulus
//   below holds aga=0, matching the OCS/ECS 4-bit PF1H/PF2H text every claim
//   in this package's inventory actually quotes.
// Claim ids: ids such as `3-fetch.c7` and `A-bplcon.c4` name one sentence of
//   the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch3.json
//   lists under `inventories`.
//
// BOUNDARY (line numbers from the classifier's own evidence field):
// denise_bitplanes.v:34-51 and 96-102 module
// header -- reg_address_in[8:1] (l.42), data_in (l.43), chip48 (l.97),
// hires (l.98), shres (l.99), hpos (l.100), bpldata[8:1] (l.101, this
// module's composed per-pixel bitplane output -- the port every downstream
// Denise submodule in this repository's own sibling harnesses, e.g.
// denise_collision's bpldata input and denise_hamgenerator's select input,
// receives its bitplane sample FROM). denise_bitplanes.v:106 -- BPLCON1 at
// custom-register address 9'h102. denise_bitplanes.v:229-235 -- OCS write
// path keeps only data_in[7:0]. denise_bitplanes.v:205-209 -- pf1h =
// {bplcon1[11:10],bplcon1[3:0],bplcon1[9:8]} (OCS: only the [3:0] half is
// populated). denise_bitplanes.v:217-221 -- pf2h from bplcon1[7:4].
// denise_bitplanes.v:107-114 -- BPL1DAT..BPL8DAT at 9'h110..9'h11e.
// denise_bitplanes.v:382-435 -- the eight buffer registers. denise_bitplanes
// .v:437-441 generates `load` from a BPL1DAT write, and every shifter
// takes it at .load(load), l.452-565.
// denise_bitplane_shifter.v:166-180 module header -- clk (l.168), c1/c3
// (l.170-171), load (l.172), hires (l.173), shres (l.174), fmode (l.175),
// data_in[63:0] (l.177), scroll[7:0] (l.178), out (l.179); rate select at
// l.205-219 (lores shifts once per four 28MHz clocks, hires every other,
// shres always). clk, clk7_en, c1, c3, reset and aga are not individually
// cited in the claim inventory's evidence field (no claim names their line
// numbers) but are taken as this module's own top-level ports on the same
// basis the Harness's whole clocking scheme rests on: they are the
// identical port names, at the identical two-flip-flop-apart c1/c3
// relationship, that this repository's OWN sibling
// harnesses for two OTHER modules in the same denise_* family declare on
// their Vdut objects -- hrm/denise_sprites/test_denise_sprites.cpp (clk,
// clk7_en, c1, c3, reset, aga, reg_address_in, data_in, chip48, hpos, all
// present) and hrm/denise_bitplane_shifter/test_denise_bitplane_shifter.cpp
// (clk, clk7_en, c1, c3 on the shifter itself, the same module instantiated
// inside this fixture). Reused as infrastructure, not read as a claim of
// this package's own inventory.
//
// THE C1/C3 TRAP (not itself a claim): c3 is the
// registered 7MHz clock and c1 = ~c3 delayed one further 28MHz cycle, so
// across one 7MHz cycle's four 28MHz phases, c1 and c3 are BOTH high in one
// phase and BOTH low in another -- they are not two disjoint two-phase
// signals. denise_bitplane_shifter.v:224 gates its shift-register load on
// !c1&&!c3. This Harness uses the measured-working four-phase pattern
// (clk7_en=(phase==0), c1=(phase==0||phase==1), c3=(phase==1||phase==2) --
// phase 1 has both high, phase 3 has both low), the same relationship
// (rotated) already proven live in this repository's own hrm/denise_sprites and
// hrm/denise_bitplane_shifter harnesses. FIXTURE-DATA-PATH-LIVE below
// exists specifically to prove this Harness's own c1/c3 pattern actually
// delivers a loaded, non-zero BPLxDAT value to bpldata BEFORE any scroll
// claim below is asserted from it -- not a formality here.
//
// HPOS[3:2] NOT A CLAIM, HELD CONSTANT: denise_bitplanes.v:146-152 decodes
// hpos[3:2] into an extra 0/4/8/12 lores-pixel write-position delay folded
// into the effective BPLCON1 value at l.198-213. No claim in this package's
// 113-record inventory is about that decode. hpos is held at 0 (hpos[3:2]
// == 0, the zero-offset case) throughout every stimulus below so this
// mechanism contributes nothing to any measurement -- carried as a constant
// non-goal, not measured or asserted.
//
// TIMING MODEL (checked against the RTL, see WHY THE PF1H NIBBLE IS THE
// DELAY below): denise_bitplane_shifter shifts one lores pixel per four 28MHz ticks
// (cited evidence above; independently measured by this repository's own
// hrm/denise_bitplane_shifter harness: "Lowres bit width expected 4
// ticks"). Page 82's "up to 15 pixels of delay" is read as PF1H/PF2H
// counting in that same four-tick lores-pixel unit regardless of hires/
// shres mode -- which is also the mechanism that would produce page 82's
// "In high resolution mode, scrolling is in increments of 2 pixels" (a
// fixed four-tick step, at a shifter rate of one hires pixel per two
// ticks, is exactly two hires pixels per PF1H unit). HIRES-SCROLL-
// GRANULARITY-2-PIXELS measures both quantities directly inside this same
// fixture rather than assuming the ratio.
//
// THE SIX-VS-EIGHT-BITS DISPUTE (not a claims.jsonl record -- no claim id
// exists for it, so nothing below asserts or declines it by id; recorded
// here only, not resolved): printed p.85 is reported to
// tell the reader to set "all six bits" of the BPLCON1 delay value, while
// the adjacent register definition (p.85's own bits-7-0 sentence, p.90's
// PF1H(3-0)/PF2H(3-0) register summary, and appendix A's bit table, all
// quoted below in claims that ARE asserted) allocates eight delay bits,
// 7-0, four per playfield. That is a contradiction inside the manual
// itself, at a page this package already tests from multiple undisputed
// angles; it is not settled here. What IS asserted below (BPLCON1-BITS-3-
// 0-ARE-PF1H, BPLCON1-BITS-7-4-ARE-PF2H, BPLCON1-DELAY-RANGE-0-TO-15) is
// the undisputed eight-bit, two-nibble structure every other citation of
// page 85 and page 90 agrees on.
//
// THE BPLXDAT DMA-ONLY DISPUTE: claims.jsonl flags exactly one record with
// a source conflict, A-bpldat.c9 (p.272): its own quoted sentence ("The MSB
// is output first...") is undisputed and IS asserted below (BPLXDAT-MSB-
// FIRST-16-PIXELS-PER-WORD); the conflict recorded on that same id is
// between appendix A p.272 ("They may also be written by either
// microprocessor") and appendix B p.291, whose BPL1DAT..BPL6DAT table rows
// carry a "&" glyph that six further claims (B-subject.c138/c143/c148/
// c153/c158/c163) independently restate as "used only by a DMA channel" --
// a reading with no legend for "&" quoted anywhere in this package's
// inventory to confirm it. That specific "DMA-only" restatement is
// declined below (not asserted, not resolved) -- see NOT ASSERTED.
//
// CONTROL CASE: FIXTURE-DATA-PATH-LIVE is the control every other
// assertion in this file depends on being true first -- it is measured on
// the identical reg_address_in/data_in write path and the identical
// bpldata readback basis as BPLCON1-ADDRESS-DECODE, BPLXDAT-ADDRESS-MAP-
// SIX-PLANES and every scroll-delay measurement below, in the identical
// Harness session type. BPLCON1-ADDRESS-DECODE additionally carries its
// own internal negative control (a write to an address this module does
// not decode, on the same measurement basis as the positive case).
//
// CLAIM COVERAGE: all 113 records of claims.jsonl are accounted for below
// -- 97 inside an expect() assertion string, 16 inside a note() or
// record() call with a stated reason (6 for the BPLxDAT DMA-only dispute,
// 9 for claims whose subject needs a wider boundary than this module --
// pointers, modulo, Copper, or DIW, and 1 for 3-dual-mode.c2, RECORDED
// near PF1H-PF2H-SCROLL-INDEPENDENTLY -- see CLAIM COVERAGE (routed
// additions) below). Every id is written out in full.
//
// SPECIFICATION ADDITIONS -- seven further propositions from the manual,
// routed to this package at its module boundary:
//   3-memory.c20 (sol, p.53): "Figure 3-7 shows an 8,000-byte memory area
//     organized as 200 lines of 40 bytes each, providing 1 bit for each
//     pixel position in the display plane." -- a bitplane provides one bit
//     per displayed pixel position.
//   3-ddf.c08 (opus, p.60): "the first data fetch before it can actually
//     display the data. As a result," -- the hardware requires some time
//     after the first data fetch before it can display the data.
//   3-dual-mode.c2 (sol, p.67): "You can also move the two playfields
//     independently." -- the two dual-mode playfields can move
//     independently.
//   3-hscr.c22 (opus, p.85): "The amount of delay in horizontal scrolling
//     is controlled by bits 7-0 in BPLCON1. You set the delay separately
//     for each playfield; bits 3-0 for playfield 1 (bitplanes 1, 3, and 5)
//     and bits 7-4 for playfield 2 (bitplanes 2, 4, and 6)." -- the delay
//     is set separately for each playfield.
//   3-hscroll.c29 (sol, p.85): "You set the delay separately for each
//     playfield; bits 3-0 for playfield 1 (bitplanes 1, 3, and 5) and bits
//     7-4 for playfield 2 (bitplanes 2, 4, and 6)." -- the playfield 1
//     delay applies to planes 1, 3, and 5.
//   3-hscroll.c31 (sol, p.85): the same quotation as 3-hscroll.c29 above --
//     the playfield 2 delay applies to planes 2, 4, and 6.
//   A-bpldat.c8 (sol, p.272): "The parallel-to-serial conversion is
//     triggered whenever bitplane #1 is written, indicating the completion
//     of all bitplanes for that word (16 pixels)." -- each converted
//     bitplane word supplies 16 pixels.
//
// CLAIM COVERAGE (routed additions): six of the seven claims above are
// asserted, each attached as a bare id to an existing assertion's label
// whose measurement already establishes its whole proposition -- see the
// source comment beside each attachment: 3-memory.c20 and A-bpldat.c8 on
// BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD; 3-ddf.c08 on FIXTURE-DATA-PATH-LIVE;
// 3-hscr.c22, 3-hscroll.c29 and 3-hscroll.c31 on
// BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN. Those six carry no
// assertion, mutation or PASS/FAIL line of their own.
//
// 3-dual-mode.c2 is RECORDED immediately after PF1H-PF2H-SCROLL-
// INDEPENDENTLY rather than attached to it: the claim is "you can also
// move the two playfields independently" in general, and the assertion
// measures only BPLCON1's horizontal scroll delay -- one axis, one
// mechanism. Coarse and vertical playfield movement is done with the
// odd/even bitplane pointers and modulos (p.72), which this module has no
// port for (see NOT ASSERTED: WIDER BOUNDARY below), so the assertion
// cannot establish the claim's full generality and the id does not sit on
// it with a caveat.
//
// RTL NOTES (WSL/Verilator 5.052). denise_bitplanes.v carries a
// `DENISE_EBR ifdef with a completely different port set on the other
// branch (chip16_r/chip16_idx instead of chip48, a shared external
// bitplane_ram instance); it is undefined anywhere in nanomig/ and this
// package's Makefile does not define it either, so the non-EBR branch
// compiles -- the one the classifier's evidence field's line numbers
// (chip48 l.97, hires l.98, shres l.99, hpos l.100, bpldata l.101) point
// at. Its ports are the ones this file drives (clk, clk7_en, reset, c1,
// c3, aga, reg_address_in[8:1], data_in[15:0], chip48[47:0], hires,
// shres, hpos[8:0], bpldata[8:1] out; denise_bitplane_shifter's
// clk/clk7_en/c1/c3/load/hires/shres/fmode[1:0]/aga/data_in[63:0]
// /scroll[7:0]/out). bitplane_ram.v is a real module but is only
// instantiated on the DENISE_EBR branch; Verilator compiles it as unused
// dead code on this build, harmlessly.
//
// WHY THE PF1H NIBBLE IS THE DELAY (denise_bitplanes.v:198-235): the OCS
// BPLCON1 write path is `bplcon1 <=
// {2'b00,2'b11,2'b00,2'b11,data_in[7:0]}` (l.234) -- so pf1h = {bplcon1
// [11:10],bplcon1[3:0],bplcon1[9:8]} (l.208) actually stores 4*PF1H+3, not
// PF1H itself (the forced "11" pair becomes pf1h's own bits[1:0]).
// denise_bitplane_shifter.v:217 then reads `select[5:0] = scroll[7:2] &
// fmode_mask` in lores mode -- the >>2 exactly cancels the *4 and discards
// the constant +3's low bits, so `select` (the scroller-history read
// index, i.e. the actual delay in lores-pixel steps) still equals the
// PF1H nibble written, unchanged. This is why the expected values below
// (kLoresPixelTicks=4 per unit, binary weighting, the 15-unit range) hold.
//
// WHY settle_bplcon1() EXISTS: denise_bitplanes.v:205-226 latches pf1h/pf2h from
// bplcon1 only `if (load)` -- load pulses for one clk7_en cycle following
// a BPL1DAT write (l.437-441) -- and pf1h_del/pf2h_del (what actually reaches
// each shifter's `scroll` port) lag pf1h/pf2h by one further clk7_en
// cycle. A bare BPLCON1 write immediately followed by the timed BPL1DAT
// trigger write measures a FLAT, PF1H-independent delay for every value
// 0..15 -- the trigger's own shifter load happens before the new BPLCON1
// value has propagated through pf1h_del at all. Harness::settle_bplcon1()
// (a throwaway priming BPL1DAT write plus a 32-tick margin, called after
// every BPLCON1 write and before any timed trigger write) avoids this;
// with it, the measured sweep is exactly 11+4*n ticks for n=0..15, the
// 4-tick-per-unit model.
//
// HOW HIRES-SCROLL-GRANULARITY-2-PIXELS MEASURES: a single bit's output
// pulse is not a proxy for the shift rate. denise_bitplane_shifter.v's
// final output stage (`sh_scroller`, l.260-261) resamples the scroller's
// combinational output every RAW clock tick regardless of the underlying
// shift rate, so a single-bit load reads 1 tick wide, not 2, and a two-bit
// test pattern gives a choppy, non-uniform waveform, not a clean 2-tick
// plateau. Nor does the assertion take the shift rate documented at
// denise_bitplane_shifter.v:212 (`shift = ~c1 ^ c3`, every other clock
// cycle) as a constant: the "2 hires pixels" conclusion would then depend
// on the RTL's own definition of a hires pixel's duration, and a wrong
// shift rate in the RTL would move the constant the same way and stay
// green. The assertion compares two RAW-TICK MEASUREMENTS against each
// other (delta_hires vs. delta_lores, no RTL-derived pixel duration at
// all) and carries the pixel arithmetic entirely in quoted manual text --
// p.82's "up to 15 pixels of delay" (the lores baseline) and page 41's
// Figure 3-2 ("320 Pixels" vs "640 Pixels" over the same physical display
// width) establish that a duration equal to 1 lores pixel is, by that
// same width ratio, exactly 2 hires pixels -- independently of anything
// the RTL calls a pixel. See that assertion's own comment for the full
// chain.
//
// MUTATION LOG (each entry run under WSL/Verilator as edit -> rebuild ->
// run -> confirm the named assertion(s) red -> revert -> rebuild ->
// confirm the baseline again. Every mutation is a STIMULUS change to this
// file's own inputs or Harness call sites; no expected value or oracle
// text is touched, and nanomig/ is never touched. Ten mutations are local
// to one assertion's own block and produce exactly one red result each.
// Two mutations (sweep_pf1h's and sweep_pf2h's own written value) are
// shared measurement infrastructure reused by several assertions by
// design (see "Shared sweeps" above main()); each is reported once,
// against the sweep-owning assertion, with every other assertion it also
// turns red listed as observed collateral.):
//
// MUTATION FIXTURE-DATA-PATH-LIVE stimulus red: the BPL1DAT trigger write
//   changed from 0xACE1 to 0x0000 -> FAIL ("14 passed, 1 failed"), only
//   FIXTURE-DATA-PATH-LIVE red (no bit ever set, first[1] never found).
//   Replayable as MUTATE=FIXTURE-DATA-PATH-LIVE. Reverted: 15/0.
// MUTATION BPLCON1-ADDRESS-DECODE stimulus red: the positive case's PF1H
//   write changed from 9 to 10 (comparison left against baseline[9]) ->
//   FAIL ("14 passed, 1 failed"), only BPLCON1-ADDRESS-DECODE red.
//   Replayable as MUTATE=BPLCON1-ADDRESS-DECODE. Reverted: 15/0.
// MUTATION BPLXDAT-ADDRESS-MAP-SIX-PLANES stimulus red: the per-plane
//   preload marker in the address-map loop changed from 0x8000 to 0x0000
//   for every n != 1 -> FAIL ("14 passed, 1 failed"), only BPLXDAT-
//   ADDRESS-MAP-SIX-PLANES red (planes 2-6 never show their own bit).
//   Replayable as MUTATE=BPLXDAT-ADDRESS-MAP-SIX-PLANES. Reverted: 15/0.
// MUTATION BPL1DAT-WRITE-TRIGGERS-ALL-PLANES stimulus red: plane 4's
//   preload changed from 0x8000 to 0x0000 -> FAIL ("14 passed, 1
//   failed"), only BPL1DAT-WRITE-TRIGGERS-ALL-PLANES red (plane 4 never
//   appears, all_six_appear false). Replayable as MUTATE=BPL1DAT-WRITE-
//   TRIGGERS-ALL-PLANES. Reverted: 15/0.
// MUTATION BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD stimulus red: the trigger
//   write changed from 0x8001 to 0x8000 (LSB marker dropped) -> FAIL
//   ("14 passed, 1 failed"), only BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD
//   red (lsb_last false). Replayable as MUTATE=BPLXDAT-MSB-FIRST-16-
//   PIXELS-PER-WORD. Reverted: 15/0.
// MUTATION PF1H-CONTROLS-PLAYFIELD1-DELAY stimulus red: sweep_pf1h's own
//   written PF1H value changed from plain `n` to `n>8?8:n` (clamped above
//   8) -> FAIL ("11 passed, 4 failed"): PF1H-CONTROLS-PLAYFIELD1-DELAY red
//   (monotonic strictly-increasing broken past n=8) AND, as real observed
//   collateral of sharing this same sweep, BPLCON1-ADDRESS-DECODE (its own
//   separate internal sweep_pf1h() call is the identical function),
//   BPLCON1-BITS-3-0-ARE-PF1H (the n=15 superposition check) and BPLCON1-
//   DELAY-RANGE-0-TO-15 (the 0..15 span check) also red. Replayable as
//   MUTATE=PF1H-CONTROLS-PLAYFIELD1-
//   DELAY. Reverted: 15/0.
// MUTATION BPLCON1-BITS-3-0-ARE-PF1H stimulus red: the same sweep_pf1h
//   clamp mutation as above (the n=15 superposition term is the specific
//   sub-check this id names) -> same run, same "11 passed, 4 failed",
//   BPLCON1-BITS-3-0-ARE-PF1H red along with the three other assertions
//   named above. Replayable as MUTATE=BPLCON1-BITS-3-0-ARE-PF1H (same
//   sweep_pf1h clamp code as PF1H-CONTROLS-PLAYFIELD1-DELAY). Reverted: 15/0.
// MUTATION PF2H-CONTROLS-PLAYFIELD2-DELAY stimulus red: sweep_pf2h's own
//   written PF2H value changed from plain `n` to `n>8?8:n` (clamped above
//   8) -> FAIL ("11 passed, 4 failed"): PF2H-CONTROLS-PLAYFIELD2-DELAY red
//   (monotonic strictly-increasing broken past n=8) AND, as real observed
//   collateral, BPLCON1-BITS-7-4-ARE-PF2H (n=15 superposition), PF1H-PF2H-
//   SCROLL-INDEPENDENTLY (reads sweep2[11], clamped to sweep2[8]'s value)
//   and BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN (reads
//   sweep2[9], likewise clamped) also red. Replayable as MUTATE=PF2H-
//   CONTROLS-PLAYFIELD2-DELAY. Reverted: 15/0.
// MUTATION BPLCON1-BITS-7-4-ARE-PF2H stimulus red: the same sweep_pf2h
//   clamp mutation as above (the n=15 superposition term is the specific
//   sub-check this id names) -> same run, same "11 passed, 4 failed",
//   BPLCON1-BITS-7-4-ARE-PF2H red along with the three other assertions
//   named above. Replayable as MUTATE=BPLCON1-BITS-7-4-ARE-PF2H (same
//   sweep_pf2h clamp code as PF2H-CONTROLS-PLAYFIELD2-DELAY). Reverted: 15/0.
// MUTATION PF1H-PF2H-SCROLL-INDEPENDENTLY stimulus red: its own written
//   PF2H value changed from 11 to 12 (comparison left against sweep2[11])
//   -> FAIL ("14 passed, 1 failed"), only PF1H-PF2H-SCROLL-INDEPENDENTLY
//   red. (The sweep_pf2h clamp mutation above also turns this id red as
//   collateral, via a different stimulus change -- both are genuine.)
//   Replayable as MUTATE=PF1H-PF2H-SCROLL-INDEPENDENTLY (its own call-site
//   switch, separate from the sweep_pf2h clamp). Reverted: 15/0.
// MUTATION BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN stimulus red: plane 5's preload changed from 0x8000 to 0x0000 -> FAIL ("14
//   passed, 1 failed"), only this assertion red (odd_ok false, plane 5
//   never appears). Replayable as MUTATE=BPLCON1-PF1H-GOVERNS-ODD-PLANES-
//   PF2H-GOVERNS-EVEN. Reverted: 15/0.
// MUTATION BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD stimulus red:
//   the written PF2H nibble changed from 7 to 8 (PF1H left at 7) -> FAIL
//   ("14 passed, 1 failed"), only this assertion red (plane 2 now tracks
//   sweep2[8], no longer equal to plane 1's sweep1[7]). Replayable as
//   MUTATE=BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD. Reverted: 15/0.
// MUTATION BPLCON1-DELAY-RANGE-0-TO-15 stimulus red: the same sweep_pf1h
//   clamp mutation used for PF1H-CONTROLS-PLAYFIELD1-DELAY above (the
//   sweep1[15]-sweep1[0] span is the specific sub-check this id names) ->
//   same run, same "11 passed, 4 failed", BPLCON1-DELAY-RANGE-0-TO-15 red
//   along with the three other assertions named there. Replayable as
//   MUTATE=BPLCON1-DELAY-RANGE-0-TO-15 (same sweep_pf1h clamp code as
//   PF1H-CONTROLS-PLAYFIELD1-DELAY). Reverted: 15/0.
// MUTATION BPLCON1-BITS-15-8-UNUSED-ON-OCS stimulus red: the garbage-upper-
//   byte write changed from 0xFF07 to 0xFF08 (PF1H's own nibble corrupted
//   from 7 to 8) -> FAIL ("14 passed, 1 failed"), only this assertion red
//   (measured delay now matches sweep1[8], not sweep1[7]). Replayable as
//   MUTATE=BPLCON1-BITS-15-8-UNUSED-ON-OCS. Reverted: 15/0.
// MUTATION HIRES-SCROLL-GRANULARITY-2-PIXELS stimulus red: the hires PF1H=1
//   comparison write changed to PF1H=2 -> FAIL ("14 passed, 1 failed"),
//   only this assertion red (delta_hires now spans two PF1H units' worth,
//   not one, so delta_hires != delta_lores). Replayable as
//   MUTATE=HIRES-SCROLL-GRANULARITY-2-PIXELS. Reverted: 15/0.

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_bitplanes.h"

namespace {

// Runtime mutation switch: one build serves both the real run (MUTATE
// unset) and every mutation run (MUTATE=<assertion label>), read once at
// startup. mutating(id) is true only while replaying that one id.
const char* g_mutate = nullptr;
bool mutating(const std::string& id) { return g_mutate != nullptr && id == g_mutate; }

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A note is not a check: it prints and does not touch passed/failed.
    // Used to record, in an actual C++ string (not only a `//` comment),
    // every claim declined below -- so coverage is findable by grepping the
    // file's own strings, not only its comments.
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    // A record is not a check either: it prints and does not touch passed/
    // failed. Used to decline a claim id that sits on no graded label,
    // printed as a real C++ string (not only a `//` comment) so a coverage
    // search of the file's own strings finds it.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Custom chip-register addresses, as printed in the manual (appendix A/B,
// pp. 271/272/291). reg_address_in is [8:1] -- an already-halved word
// address -- the same convention this repository's own sibling Denise
// harnesses use (e.g. hrm/ch3-ham-control-field's write_color: COLORBASE
// $180 -> reg_address_in 0xC0 == $180>>1) and the same relationship
// hrm/denise_sprites/test_denise_sprites.cpp assumes for its own
// reg_address_in writes.
constexpr uint16_t kBplcon1Addr = 0x102;
constexpr uint16_t kBpl1DatAddr = 0x110;
constexpr uint16_t kBpl2DatAddr = 0x112;
constexpr uint16_t kBpl3DatAddr = 0x114;
constexpr uint16_t kBpl4DatAddr = 0x116;
constexpr uint16_t kBpl5DatAddr = 0x118;
constexpr uint16_t kBpl6DatAddr = 0x11A;
// A negative-control address this module's boundary does not include at
// all (BPLCON2 on real hardware; no BPLCON2 port anywhere in this
// fixture's cited evidence) -- used only to prove the $102 decode is
// specific, never to claim anything about what $104 itself does.
constexpr uint16_t kWrongAddr = 0x104;
// Index 1..6 by plane number, matching bpldata's own bit numbering.
constexpr uint16_t kBplxDatAddr[7] = {0, kBpl1DatAddr, kBpl2DatAddr, kBpl3DatAddr,
                                       kBpl4DatAddr, kBpl5DatAddr, kBpl6DatAddr};

// One lores pixel, in raw 28MHz ticks, per denise_bitplane_shifter's own
// documented and (in this repository's own sibling harness) measured rate.
// (There is no equivalent kHiresPixelTicks constant: HIRES-SCROLL-
// GRANULARITY-2-PIXELS below deliberately does not take a hires-pixel
// duration from the RTL at all -- see that assertion's own header note.)
constexpr long kLoresPixelTicks = 4;

// Clocking mechanics shared with the pre-pipeline harnesses for other
// denise_* modules with the identical clk/clk7_en/c1/c3/reset/aga/
// reg_address_in/data_in/chip48/hpos port set (hrm/denise_sprites,
// hrm/denise_bitplane_shifter; both removed).
class Harness {
public:
    Harness() : dut(new Vdenise_bitplanes) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->aga = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->hires = 0;
        dut->shres = 0;
        dut->hpos = 0;  // hpos[3:2] == 0: the zero-offset case, held constant throughout.
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One 28MHz clock edge, then advance the four-phase c1/c3/clk7_en
    // relationship for the NEXT edge -- see THE C1/C3 TRAP above. Phase 0
    // is the clk7_en register-capture phase (also when a BPLxDAT write
    // latches its buffer register, denise_bitplanes.v:382-435); phase 1
    // has c1 AND c3 both high; phase 3 has both low (the shifter load
    // phase, denise_bitplane_shifter.v:224).
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

    // Advances to the clk7_en phase, presents (addr>>1, data), captures it
    // on exactly the following clk7_en edge, and returns immediately --
    // phase is left at 1, the phase after the clk7_en capture edge, so a caller measuring
    // timing from a trigger write has a known, minimal-latency starting
    // point. The bus is left driven; call clear_reg() when done with it.
    void begin_write_reg(uint16_t custom_addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = static_cast<uint8_t>(custom_addr >> 1);
        dut->data_in = data;
        tick();
    }
    void clear_reg() {
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }
    // Convenience wrapper for writes whose own timing is not being
    // measured (BPLCON1 setup, preloading planes 2-6 before the BPL1DAT
    // trigger write).
    void write_reg(uint16_t custom_addr, uint16_t data) {
        begin_write_reg(custom_addr, data);
        clear_reg();
    }

    // denise_bitplanes.v:205-209/217-221: pf1h/pf2h only latch BPLCON1's
    // current value on a `load` pulse (a BPL1DAT write, l.437-441), and
    // pf1h_del/pf2h_del (l.211-214/223-226 -- what actually reaches each
    // shifter's `scroll` port) lag pf1h/pf2h by one further clk7_en cycle.
    // A bare BPLCON1 write alone does not move any measured delay; call
    // this once after writing BPLCON1 and before any timed measurement
    // trigger write, so a throwaway BPL1DAT write latches the new value
    // and a generous settling margin lets pf1h_del/pf2h_del catch up
    // before anything is measured. See WHY settle_bplcon1() EXISTS in the
    // header comment above.
    void settle_bplcon1() {
        write_reg(kBpl1DatAddr, 0x0000);
        for (int i = 0; i < 32; ++i) tick();
    }

    // Scans up to `window` ticks from the current instant, sampling BEFORE
    // each tick, and records -- per bitplane 1..6 (bpldata's own bit
    // numbering, bpldata[8:1] packed so plane N is C++ bit (N-1)) -- the
    // tick offset at which that plane's bit is first seen high. -1 if
    // never seen within the window. Index 0 is unused.
    std::array<long, 7> measure_first_high_all(long window) {
        std::array<long, 7> result{};
        result.fill(-1);
        for (long i = 0; i < window; ++i) {
            for (int p = 1; p <= 6; ++p) {
                if (result[p] < 0 && (dut->bpldata & (1u << (p - 1)))) result[p] = i;
            }
            bool all_found = true;
            for (int p = 1; p <= 6; ++p)
                if (result[p] < 0) all_found = false;
            if (all_found) break;
            tick();
        }
        return result;
    }

    Vdenise_bitplanes* dut;
    int phase = 0;
};

// Sweeps BPLCON1's PF1H field 0..15 (PF2H held at 0), one fresh Harness per
// point, and records the tick offset from the BPL1DAT trigger write to
// bpldata's plane-1 bit first going high -- "the amount of delay" (p.82)
// made concrete as a measurement.
std::array<long, 16> sweep_pf1h() {
    std::array<long, 16> t{};
    for (int n = 0; n <= 15; ++n) {
        Harness h;
        // MUTATION PF1H-CONTROLS-PLAYFIELD1-DELAY / BPLCON1-BITS-3-0-ARE-PF1H /
        //   BPLCON1-DELAY-RANGE-0-TO-15 stimulus red: MUTATE=PF1H-CONTROLS-
        //   PLAYFIELD1-DELAY, MUTATE=BPLCON1-BITS-3-0-ARE-PF1H or MUTATE=
        //   BPLCON1-DELAY-RANGE-0-TO-15 clamps the written PF1H value at 8
        //   (n>8?8:n) instead of writing n plainly, so this shared sweep loses
        //   its top range.
        uint16_t pf1h_n = static_cast<uint16_t>(n);
        if (mutating("PF1H-CONTROLS-PLAYFIELD1-DELAY") || mutating("BPLCON1-BITS-3-0-ARE-PF1H") ||
            mutating("BPLCON1-DELAY-RANGE-0-TO-15"))
            pf1h_n = static_cast<uint16_t>(n > 8 ? 8 : n);
        h.write_reg(kBplcon1Addr, pf1h_n);  // PF2H=0, PF1H=n
        h.settle_bplcon1();
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        t[n] = first[1];
    }
    return t;
}

// Same sweep for PF2H 0..15 (PF1H held at 0), tracking plane 2's bit. The
// trigger is still a BPL1DAT write (per A-bplxdat.c05/A-bpldat.c7 -- plane
// 1's write triggers every plane's conversion, plane 2 included), with
// BPL1DAT's own data left at 0 so it does not interfere with reading
// plane 2's bit.
std::array<long, 16> sweep_pf2h() {
    std::array<long, 16> t{};
    for (int n = 0; n <= 15; ++n) {
        Harness h;
        // MUTATION PF2H-CONTROLS-PLAYFIELD2-DELAY / BPLCON1-BITS-7-4-ARE-PF2H
        //   stimulus red: MUTATE=PF2H-CONTROLS-PLAYFIELD2-DELAY or
        //   MUTATE=BPLCON1-BITS-7-4-ARE-PF2H clamps the written PF2H value at
        //   8 (n>8?8:n) instead of writing n plainly.
        uint16_t pf2h_n = static_cast<uint16_t>(n);
        if (mutating("PF2H-CONTROLS-PLAYFIELD2-DELAY") || mutating("BPLCON1-BITS-7-4-ARE-PF2H"))
            pf2h_n = static_cast<uint16_t>(n > 8 ? 8 : n);
        h.write_reg(kBplcon1Addr, static_cast<uint16_t>(pf2h_n << 4));  // PF1H=0, PF2H=n
        h.settle_bplcon1();
        h.write_reg(kBpl2DatAddr, 0x8000);
        h.begin_write_reg(kBpl1DatAddr, 0x0000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        t[n] = first[2];
    }
    return t;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ---------------- FIXTURE-DATA-PATH-LIVE (control case) ----------------
    {
        Harness h;
        // MUTATION FIXTURE-DATA-PATH-LIVE stimulus red: MUTATE=FIXTURE-DATA-
        //   PATH-LIVE drives the trigger word to 0x0000 instead, so no bit
        //   ever loads.
        h.begin_write_reg(kBpl1DatAddr, mutating("FIXTURE-DATA-PATH-LIVE") ? 0x0000 : 0xACE1);  // MSB (bit15) set: a distinct, recognisable nonzero pattern.
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        // 3-ddf.c08 (p.60) restates the same "requires some time after the
        // first data fetch before it can display the data" sentence 3-fetch.c7
        // already reads -- the identical nonzero-latency proposition, graded
        // by this same measurement.
        checks.expect(
            first[1] > 0,
            "FIXTURE-DATA-PATH-LIVE: a distinct nonzero value (0xACE1) written to "
            "BPL1DAT arrives at bpldata's plane-1 bit as a non-zero pixel, and not "
            "in the same instant as the write itself -- proving this Harness's own "
            "four-phase c1/c3 pattern (phase 3: both low, the shifter load) "
            "actually delivers a loaded value through to a shifter. Also the "
            "basis for 'The hardware requires some time after "
            "the first data fetch before it can actually display the data' (p.60) "
            "-- the measured latency here is that same nonzero delay, at this "
            "module's own boundary. 3-fetch.c7, 3-ddf.c08 .");
    }

    // ---------------- BPLCON1-ADDRESS-DECODE ----------------
    {
        Harness h;
        // MUTATION BPLCON1-ADDRESS-DECODE stimulus red: MUTATE=BPLCON1-
        //   ADDRESS-DECODE writes PF1H=10 instead of 9, so the positive case
        //   no longer matches baseline[9].
        h.write_reg(kBplcon1Addr, mutating("BPLCON1-ADDRESS-DECODE") ? 10 : 9);  // PF1H=9
        h.settle_bplcon1();
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto positive = h.measure_first_high_all(400);

        Harness h2;
        h2.write_reg(kWrongAddr, 9);  // identical data, an address this module does not decode
        h2.settle_bplcon1();
        h2.begin_write_reg(kBpl1DatAddr, 0x8000);
        h2.clear_reg();
        auto negative = h2.measure_first_high_all(400);

        auto baseline = sweep_pf1h();
        checks.expect(
            positive[1] == baseline[9] && negative[1] == baseline[0],
            "BPLCON1-ADDRESS-DECODE: a write of PF1H=9 to custom-register address "
            "$102 measurably changes plane 1's scroll delay to the same value a "
            "direct $102 write produces; the identical data written to $104 (a "
            "negative control this module does not decode at all) leaves the "
            "delay at its power-up default -- 'BPLCON1 102 W D Bitplane control "
            "register' (appendix A, p.271), 'BPLCON1 102 W D Bitplane control "
            "reg. (scroll value PF1, PF2)' (appendix B, p.291), 'Bitplane control "
            "register (horizontal scroll control)' (appendix A, p.271). "
            "A-bplcon.c4, A-bplcon1.addr, A-bplcon1.fn, B-bplcon1.addr, "
            "B-subject.c114, B-subject.c117 .");
    }

    // ---------------- BPLXDAT-ADDRESS-MAP-SIX-PLANES ----------------
    {
        bool all_ok = true;
        for (int n = 1; n <= 6; ++n) {
            Harness h;
            // MUTATION BPLXDAT-ADDRESS-MAP-SIX-PLANES stimulus red: MUTATE=
            //   BPLXDAT-ADDRESS-MAP-SIX-PLANES preloads every plane other
            //   than 1 with 0x0000 instead of 0x8000, so no plane but 1 ever
            //   shows a bit.
            if (n != 1) h.write_reg(kBplxDatAddr[n], mutating("BPLXDAT-ADDRESS-MAP-SIX-PLANES") ? 0x0000 : 0x8000);
            h.begin_write_reg(kBpl1DatAddr, (n == 1) ? 0x8000 : 0x0000);
            h.clear_reg();
            auto first = h.measure_first_high_all(400);
            for (int p = 1; p <= 6; ++p) {
                bool want_seen = (p == n);
                bool seen = (first[p] >= 0);
                if (seen != want_seen) all_ok = false;
            }
        }
        checks.expect(
            all_ok,
            "BPLXDAT-ADDRESS-MAP-SIX-PLANES: writing a distinct nonzero pattern to "
            "exactly one of BPL1DAT ($110), BPL2DAT ($112), BPL3DAT ($114), "
            "BPL4DAT ($116), BPL5DAT ($118) or BPL6DAT ($11A) -- with every other "
            "plane's data left at 0 -- produces a pixel on that plane's own "
            "bpldata bit and no other, for all six planes independently; the "
            "write path used throughout (reg_address_in/data_in) is the "
            "processor-style register bus this whole Harness is built on, "
            "confirming it (not DMA-exclusive access) reaches BPLxDAT -- 'They "
            "may also be written by either microprocessor' (p.272). "
            "A-bpldat.c1, A-bpldat.c5, A-bplxdat.addr, A-bplxdat.c02, "
            "B-bpl1dat.addr, B-bpl2dat.addr, B-bpl3dat.addr, B-bpl4dat.addr, "
            "B-bpl5dat.addr, B-bpl6dat.addr, B-subject.c134, B-subject.c139, "
            "B-subject.c144, B-subject.c149, B-subject.c154, B-subject.c159 .");
    }

    // ---------------- BPL1DAT-WRITE-TRIGGERS-ALL-PLANES ----------------
    {
        Harness h;
        h.write_reg(kBpl2DatAddr, 0x8000);
        h.write_reg(kBpl3DatAddr, 0x8000);
        // MUTATION BPL1DAT-WRITE-TRIGGERS-ALL-PLANES stimulus red: MUTATE=
        //   BPL1DAT-WRITE-TRIGGERS-ALL-PLANES preloads plane 4 with 0x0000
        //   instead of 0x8000, so it never appears when BPL1DAT triggers the
        //   others.
        h.write_reg(kBpl4DatAddr, mutating("BPL1DAT-WRITE-TRIGGERS-ALL-PLANES") ? 0x0000 : 0x8000);
        h.write_reg(kBpl5DatAddr, 0x8000);
        h.write_reg(kBpl6DatAddr, 0x8000);
        bool quiescent = true;
        for (int i = 0; i < 40; ++i) {
            if (h.dut->bpldata != 0) quiescent = false;
            h.tick();
        }
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        bool all_six_appear = true;
        for (int p = 1; p <= 6; ++p)
            if (first[p] < 0) all_six_appear = false;
        bool simultaneous = true;
        for (int p = 2; p <= 6; ++p)
            if (first[p] != first[1]) simultaneous = false;

        checks.expect(
            quiescent && all_six_appear && simultaneous,
            "BPL1DAT-WRITE-TRIGGERS-ALL-PLANES: five planes (2-6) preloaded with "
            "data stay entirely quiescent on bpldata until BPL1DAT is written, at "
            "which point all six planes' loaded data appears together, at the "
            "same tick -- 'The parallel-to-serial conversion is triggered "
            "whenever bitplane #1 is written, indicating the completion of all "
            "bitplanes for that word' (p.272), 'They act as a six-word "
            "parallel-to-serial buffer for up to six memory bitplanes (x=1-6)' "
            "(p.272), 'Bitplane x data (parallel-to-serial convert)' (p.272). "
            "A-bpldat.c3, A-bpldat.c6, A-bpldat.c7, A-bplxdat.c03, A-bplxdat.c04, "
            "A-bplxdat.c05, A-bplxdat.fn, B-subject.c137, B-subject.c142, "
            "B-subject.c147, B-subject.c152, B-subject.c157, B-subject.c162 .");
    }

    // ---------------- BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD ----------------
    {
        Harness h;
        // MUTATION BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD stimulus red:
        //   MUTATE=BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD drops the LSB
        //   marker, writing 0x8000 instead of 0x8001.
        h.begin_write_reg(kBpl1DatAddr, mutating("BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD") ? 0x8000 : 0x8001);  // bit15 (MSB) and bit0 (LSB) set, all 14 bits between clear.
        h.clear_reg();
        long first = -1;
        for (long i = 0; i < 400 && first < 0; ++i) {
            if (h.dut->bpldata & 0x1)
                first = i;
            else
                h.tick();
        }
        std::vector<bool> samples;
        for (int px = 0; px < 16 && first >= 0; ++px) {
            samples.push_back((h.dut->bpldata & 0x1) != 0);
            for (long k = 0; k < kLoresPixelTicks; ++k) h.tick();
        }
        bool msb_first = samples.size() == 16 && samples[0];
        bool middle_zero = true;
        for (size_t px = 1; px < 15 && px < samples.size(); ++px)
            if (samples[px]) middle_zero = false;
        bool lsb_last = samples.size() == 16 && samples[15];

        // 3-memory.c20 (p.53, "1 bit for each pixel position") and A-bpldat.c8
        // (p.272, "16 pixels" per converted word) restate, in different words,
        // exactly the 16-bits-in/16-pixels-out, one-bit-per-position mapping
        // this same measurement already establishes (msb_first, middle_zero
        // and lsb_last together cover all 16 bit/pixel positions of the word).
        checks.expect(
            first >= 0 && msb_first && middle_zero && lsb_last,
            "BPLXDAT-MSB-FIRST-16-PIXELS-PER-WORD: loading BPL1DAT with bit15 and "
            "bit0 set and the 14 bits between clear produces a pixel at the very "
            "first lores-pixel position, nothing at the 14 in between, and a "
            "pixel again exactly 15 lores-pixel positions later -- the MSB "
            "output first (leftmost) and one word spanning exactly 16 pixels -- "
            "'The MSB is output first, and is, therefore, always on the left' "
            "(p.272), 'indicating the completion of all bitplanes for that word "
            "(16 pixels)' (p.272). A-bpldat.c9, A-bpldat.c10, A-bplxdat.c07, "
            "A-bplxdat.c08, 3-memory.c20, A-bpldat.c8 .");
    }

    // Shared sweeps -- reused by every PF1H/PF2H-structural assertion below
    // so the underlying measurement is run once, not once per claim.
    auto sweep1 = sweep_pf1h();
    auto sweep2 = sweep_pf2h();

    // ---------------- PF1H-CONTROLS-PLAYFIELD1-DELAY ----------------
    {
        bool monotonic = true, constant_step = true;
        for (int n = 1; n <= 15; ++n) {
            if (!(sweep1[n] > sweep1[n - 1])) monotonic = false;
            if (sweep1[n] - sweep1[n - 1] != kLoresPixelTicks) constant_step = false;
        }
        checks.expect(
            monotonic && constant_step,
            "PF1H-CONTROLS-PLAYFIELD1-DELAY: sweeping BPLCON1's PF1H field 0..15 "
            "moves plane 1's first measured pixel strictly later, one lores-pixel "
            "width per step, for every step -- 'A playfield ... can be scrolled "
            "horizontally' (p.42), 'A larger playfield [can] scroll ... through "
            "the window' (p.57), 'You control the speed of scrolling by "
            "specifying the amount [of delay]' (p.82), 'The greater the delay, "
            "the greater the speed of scrolling' (p.82), figure 3-24's "
            "right-pointing 'As delay is added, onscreen display shifts this "
            "direction' callout (p.83), 'PF1H=Playfield 1 horizontal scroll "
            "code' (p.271). 3-bpl.c14, 3-bpl.c15, 3-video.c31, 3-window.c7, "
            "3-hscr.c01, 3-hscroll.c6, 3-hscr.f04, A-bplcon.c84, A-leg.pf1h .");
    }

    // ---------------- BPLCON1-BITS-3-0-ARE-PF1H ----------------
    {
        bool bit0 = (sweep1[1] - sweep1[0]) == 1 * kLoresPixelTicks;
        bool bit1 = (sweep1[2] - sweep1[0]) == 2 * kLoresPixelTicks;
        bool bit2 = (sweep1[4] - sweep1[0]) == 4 * kLoresPixelTicks;
        bool bit3 = (sweep1[8] - sweep1[0]) == 8 * kLoresPixelTicks;
        bool superposed = (sweep1[15] - sweep1[0]) == 15 * kLoresPixelTicks;
        checks.expect(
            bit0 && bit1 && bit2 && bit3 && superposed,
            "BPLCON1-BITS-3-0-ARE-PF1H: isolating each of PF1H's four bits in "
            "turn (values 1,2,4,8) produces exactly 1,2,4,8 lores-pixel-widths "
            "of extra delay respectively, and all four together (value 15) "
            "produce their sum -- confirming bits 3-0 are a 4-bit binary field, "
            "not four independent switches -- 'Bits 3-0 - PF1H(3-0) Playfield 1 "
            "delay' (p.90), 'bits 3-0 for playfield 1' (p.85). "
            "A-bplcon.c50, A-bplcon.c53, A-bplcon.c56, A-bplcon.c59, "
            "A-bpc.b00.c1, A-bpc.b01.c1, A-bpc.b02.c1, A-bpc.b03.c1, "
            "3-r1.c01, 3-regsum-bplcon1.c1 .");
    }

    // ---------------- PF2H-CONTROLS-PLAYFIELD2-DELAY ----------------
    {
        bool monotonic = true, constant_step = true;
        for (int n = 1; n <= 15; ++n) {
            if (!(sweep2[n] > sweep2[n - 1])) monotonic = false;
            if (sweep2[n] - sweep2[n - 1] != kLoresPixelTicks) constant_step = false;
        }
        checks.expect(
            monotonic && constant_step,
            "PF2H-CONTROLS-PLAYFIELD2-DELAY: sweeping BPLCON1's PF2H field 0..15 "
            "moves plane 2's first measured pixel strictly later, one lores-pixel "
            "width per step, for every step, on the identical measurement basis "
            "as PF1H-CONTROLS-PLAYFIELD1-DELAY above -- 'PF2H=Playfield 2 "
            "horizontal scroll code' (p.271). A-bplcon.c83, A-leg.pf2h .");
    }

    // ---------------- BPLCON1-BITS-7-4-ARE-PF2H ----------------
    {
        bool bit0 = (sweep2[1] - sweep2[0]) == 1 * kLoresPixelTicks;
        bool bit1 = (sweep2[2] - sweep2[0]) == 2 * kLoresPixelTicks;
        bool bit2 = (sweep2[4] - sweep2[0]) == 4 * kLoresPixelTicks;
        bool bit3 = (sweep2[8] - sweep2[0]) == 8 * kLoresPixelTicks;
        bool superposed = (sweep2[15] - sweep2[0]) == 15 * kLoresPixelTicks;
        checks.expect(
            bit0 && bit1 && bit2 && bit3 && superposed,
            "BPLCON1-BITS-7-4-ARE-PF2H: isolating each of PF2H's four bits in "
            "turn (values 1,2,4,8, written into bits 7-4) produces exactly "
            "1,2,4,8 lores-pixel-widths of extra delay on plane 2 respectively, "
            "and all four together (value 15) produce their sum -- 'Bits 7-4 - "
            "PF2H(3-0) Playfield 2 delay' (p.90), 'bits 7-4 for playfield 2' "
            "(p.85). A-bplcon.c38, A-bplcon.c41, A-bplcon.c44, A-bplcon.c47, "
            "A-bpc.b04.c1, A-bpc.b05.c1, A-bpc.b06.c1, A-bpc.b07.c1, "
            "3-r1.c02, 3-regsum-bplcon1.c2 .");
    }

    // ---------------- PF1H-PF2H-SCROLL-INDEPENDENTLY ----------------
    {
        Harness h;
        // MUTATION PF1H-PF2H-SCROLL-INDEPENDENTLY stimulus red: MUTATE=
        //   PF1H-PF2H-SCROLL-INDEPENDENTLY writes PF2H=12 instead of 11, so
        //   the comparison against sweep2[11] no longer matches.
        h.write_reg(kBplcon1Addr, static_cast<uint16_t>(((mutating("PF1H-PF2H-SCROLL-INDEPENDENTLY") ? 12 : 11) << 4) | 5));  // PF1H=5, PF2H=11
        h.settle_bplcon1();
        h.write_reg(kBpl2DatAddr, 0x8000);
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        // 3-dual-mode.c2 (p.67, "You can also move the two playfields
        // independently") is NOT attached here -- see the RECORDED call
        // right below. That claim is about playfield movement in general;
        // this measurement establishes only the horizontal scroll delay.
        checks.expect(
            first[1] == sweep1[5] && first[2] == sweep2[11],
            "PF1H-PF2H-SCROLL-INDEPENDENTLY: with PF1H=5 and PF2H=11 written "
            "together, plane 1's delay measures exactly PF1H=5's own "
            "isolated-sweep value and plane 2's delay measures exactly "
            "PF2H=11's own isolated-sweep value -- neither field perturbed by "
            "the other's simultaneous nonzero value -- 'They can be scrolled "
            "separately' (p.71), 'If you are using dual playfields, you can "
            "scroll them separately' (p.80). 3-dual-control.c3, 3-dual.c33, "
            "3-scroll.c1 .");
        checks.record(
            "WITHDRAWN 3-dual-mode.c2 (sol, p.67, \"You can also move the "
            "two playfields independently\") from PF1H-PF2H-SCROLL-"
            "INDEPENDENTLY above. That claim is about playfield movement in "
            "general; that assertion establishes only "
            "BPLCON1's 0-15-pixel horizontal scroll delay -- one axis, one "
            "mechanism. Coarse and vertical movement of a playfield is done "
            "with the odd/even bitplane pointers and modulos ('adjust the "
            "modulo and the bitplane pointers of the playfield that is not "
            "being scrolled', p.72), an Agnus DMA-address mechanism with no "
            "pointer, modulo or Copper port anywhere in denise_bitplanes' "
            "boundary (see NOT ASSERTED: WIDER BOUNDARY below). Withdrawn "
            "whole rather than kept on the horizontal-only measurement with "
            "a caveat -- a pointers-and-modulo fixture would be needed to "
            "establish the rest.");
    }

    // ---------------- BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN ----------------
    {
        Harness h;
        h.write_reg(kBplcon1Addr, static_cast<uint16_t>((9 << 4) | 6));  // PF1H=6, PF2H=9
        h.settle_bplcon1();
        h.write_reg(kBpl2DatAddr, 0x8000);
        h.write_reg(kBpl3DatAddr, 0x8000);
        h.write_reg(kBpl4DatAddr, 0x8000);
        // MUTATION BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN
        //   stimulus red: MUTATE=BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-
        //   GOVERNS-EVEN preloads plane 5 with 0x0000 instead of 0x8000, so
        //   it never appears.
        h.write_reg(kBpl5DatAddr, mutating("BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN") ? 0x0000 : 0x8000);
        h.write_reg(kBpl6DatAddr, 0x8000);
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        bool odd_ok = (first[1] == sweep1[6]) && (first[3] == sweep1[6]) && (first[5] == sweep1[6]);
        bool even_ok = (first[2] == sweep2[9]) && (first[4] == sweep2[9]) && (first[6] == sweep2[9]);
        // 3-hscr.c22 ("delay is set separately for each playfield"),
        // 3-hscroll.c29 ("playfield 1 delay applies to planes 1, 3, and 5")
        // and 3-hscroll.c31 ("playfield 2 delay applies to planes 2, 4, and
        // 6") restate, each in turn, exactly what odd_ok and even_ok already
        // establish together.
        checks.expect(
            odd_ok && even_ok,
            "BPLCON1-PF1H-GOVERNS-ODD-PLANES-PF2H-GOVERNS-EVEN: with PF1H=6 and "
            "PF2H=9 and all six planes loaded, planes 1, 3 and 5 all measure "
            "exactly PF1H=6's delay and planes 2, 4 and 6 all measure exactly "
            "PF2H=9's delay -- 'bits 3-0 for playfield 1 (bitplanes 1, 3, and 5) "
            "and bits 7-4 for playfield 2 (bitplanes 2, 4, and 6)' (p.85). "
            "3-hscr.c21, 3-hscr.c23, 3-hscr.c24, 3-hscroll.c27, 3-hscroll.c28, "
            "3-hscroll.c30, 3-hscr.c22, 3-hscroll.c29, 3-hscroll.c31 .");
    }

    // ---------------- BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD ----------------
    {
        Harness h;
        // MUTATION BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD stimulus
        //   red: MUTATE=BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD
        //   writes PF2H=8 instead of 7 (PF1H left at 7).
        h.write_reg(kBplcon1Addr, static_cast<uint16_t>(((mutating("BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD") ? 8 : 7) << 4) | 7));  // PF1H=PF2H=7
        h.settle_bplcon1();
        h.write_reg(kBpl2DatAddr, 0x8000);
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        checks.expect(
            first[1] == first[2] && first[1] == sweep1[7] && first[2] == sweep2[7],
            "BPLCON1-SAME-VALUE-BOTH-FIELDS-SINGLE-PLAYFIELD: writing the same "
            "nibble (7) to both PF1H and PF2H produces identical, matching "
            "delay on planes 1 and 2 -- 'Set 3-0 and 7-4 to the same value if "
            "you are using only one playfield' (p.85). 3-hscroll.c33 .");
    }

    // ---------------- BPLCON1-DELAY-RANGE-0-TO-15 ----------------
    {
        checks.expect(
            (sweep1[15] - sweep1[0]) == 15 * kLoresPixelTicks,
            "BPLCON1-DELAY-RANGE-0-TO-15: the full PF1H sweep spans exactly 15 "
            "lores-pixel-widths from value 0 to value 15, its maximum -- 'You "
            "can have up to 15 pixels of delay' (p.82), 'Set bits 7-0 in "
            "BPLCON1 for 0 to 15 bits of delay' (p.85), figure 3-24's two "
            "dashed window outlines identifying the displayed data at scroll "
            "values 0 and 15 (p.83). 3-hscr.c06, 3-hscr.c30, 3-hscroll.c37, "
            "3-hscroll.c21 .");
    }

    // ---------------- BPLCON1-BITS-15-8-UNUSED-ON-OCS ----------------
    {
        Harness h;
        // MUTATION BPLCON1-BITS-15-8-UNUSED-ON-OCS stimulus red: MUTATE=
        //   BPLCON1-BITS-15-8-UNUSED-ON-OCS corrupts PF1H's own nibble from
        //   7 to 8 (0xFF08 instead of 0xFF07).
        h.write_reg(kBplcon1Addr, mutating("BPLCON1-BITS-15-8-UNUSED-ON-OCS") ? 0xFF08 : 0xFF07);  // upper byte all-ones garbage, PF1H=7, PF2H=0
        h.settle_bplcon1();
        h.begin_write_reg(kBpl1DatAddr, 0x8000);
        h.clear_reg();
        auto first = h.measure_first_high_all(400);
        checks.expect(
            first[1] == sweep1[7],
            "BPLCON1-BITS-15-8-UNUSED-ON-OCS: writing $FF07 (an all-ones garbage "
            "upper byte over a clean PF1H=7) measures the identical delay as a "
            "clean $0007 write -- the upper byte has no effect -- 'Bits 15-8 not "
            "used' (p.90), and appendix A's bit table marking bits 8-15 'X' "
            "against BPLCON1 (p.271). A-bpc.b15.x, A-bplcon.c14, A-bplcon.c17, "
            "A-bplcon.c20, A-bplcon.c23, A-bplcon.c26, A-bplcon.c29, "
            "A-bplcon.c32, A-bplcon.c35, 3-r1.c03, 3-regsum-bplcon1.c3 .");
    }

    // ---------------- HIRES-SCROLL-GRANULARITY-2-PIXELS ----------------
    // The oracle for the equality below is the manual alone (pp.41, 82);
    // the RTL is asked only "is one PF1H unit the same duration in both
    // modes" -- a plain tick-count comparison that never asks the RTL what
    // a pixel is. Dividing the measured hires delta by a constant taken
    // from denise_bitplane_shifter.v:212's own shift rate would make that
    // half of the oracle circular: if the RTL's hires shift rate were
    // itself wrong, the constant would be wrong the same way and the
    // assertion would stay green.
    {
        Harness h0;
        h0.dut->hires = 1;
        h0.write_reg(kBplcon1Addr, 0);
        h0.settle_bplcon1();
        h0.begin_write_reg(kBpl1DatAddr, 0x8000);
        h0.clear_reg();
        auto f0 = h0.measure_first_high_all(400);

        Harness h1;
        h1.dut->hires = 1;
        // MUTATION HIRES-SCROLL-GRANULARITY-2-PIXELS stimulus red: MUTATE=
        //   HIRES-SCROLL-GRANULARITY-2-PIXELS writes PF1H=2 instead of 1, so
        //   delta_hires spans two PF1H units instead of one.
        h1.write_reg(kBplcon1Addr, mutating("HIRES-SCROLL-GRANULARITY-2-PIXELS") ? 2 : 1);
        h1.settle_bplcon1();
        h1.begin_write_reg(kBpl1DatAddr, 0x8000);
        h1.clear_reg();
        auto f1 = h1.measure_first_high_all(400);

        // delta_hires: raw ticks per PF1H unit, measured here in hires
        // mode. delta_lores: the identical quantity in lores mode -- reused
        // from sweep1 (sweep1[1]-sweep1[0]) above, not re-derived, since it
        // is the same stimulus under the same measurement already taken.
        // The ONLY thing asked of the RTL in this assertion is whether
        // these two raw-tick counts are equal; everything else below is
        // pixel arithmetic carried by quoted manual text.
        long delta_hires = f1[1] - f0[1];
        long delta_lores = sweep1[1] - sweep1[0];

        checks.expect(
            delta_hires == delta_lores,
            "HIRES-SCROLL-GRANULARITY-2-PIXELS: one PF1H unit takes the "
            "identical raw-tick duration in hires mode as in lores mode -- "
            "measured, not assumed; this is the only fact the RTL "
            "contributes. Combined with two facts the manual itself states: "
            "a hires pixel is half a lores pixel's physical width -- 'In low "
            "resolution mode, the normal playfield has a width of 320 "
            "pixels' against 'High resolution mode gives finer horizontal "
            "resolution -- 640 pixels in the same physical display area' "
            "(p.41; Figure 3-2's own captions read '320 Pixels' / '640 "
            "Pixels', 'In normal resolution mode, 320 pixels fill a "
            "horizontal line' / 'In high resolution mode, 640 pixels fill a "
            "horizontal line') -- and that '[you] can have up to 15 pixels "
            "of delay' (p.82) states the lores baseline of 1 pixel per "
            "PF1H unit with no resolution qualifier, immediately before the "
            "sentence under test. A duration that spans 1 lores pixel "
            "therefore spans, at half the physical width per pixel, exactly "
            "2 hires pixels -- 'In high resolution mode, scrolling is in "
            "increments of 2 pixels' (p.82). Only the equality of the two "
            "measured deltas comes from the Harness; the pixel arithmetic "
            "comes entirely from pp.41 and 82. 3-hscr.c06, 3-hscr.c07, "
            "3-hscroll.c8 .");
    }

    // ---------------- NOT ASSERTED: BPLXDAT DMA-ONLY DISPUTE ----------------
    checks.note(
        "NOT ASSERTED (6 claims) B-subject.c138, B-subject.c143, "
        "B-subject.c148, B-subject.c153, B-subject.c158, B-subject.c163 "
        "(sol, p.291), each restating BPL1DAT/BPL2DAT/BPL3DAT/BPL4DAT/"
        "BPL5DAT/BPL6DAT in turn as \"used only by a DMA channel\": that "
        "reading comes from a \"&\" glyph in appendix B's table row (e.g. "
        "'BPL1DAT & 110 W D Bitplane 1 data (parallel-to-serial convert)'), "
        "with no legend for \"&\" quoted anywhere in this package's "
        "inventory to confirm that meaning. It directly conflicts with "
        "appendix A p.272's explicit 'They may also be written by either "
        "microprocessor' (A-bpldat.c5, A-bplxdat.c02, asserted in "
        "BPLXDAT-ADDRESS-MAP-SIX-PLANES above) -- the same conflict "
        "claims.jsonl's own A-bpldat.c9 record already flags between "
        "appendix A p.272 and appendix B p.291. This fixture's write path "
        "(reg_address_in/data_in, the ordinary processor-style register "
        "bus every assertion in this file is built on) does reach BPLxDAT "
        "and does produce the measured effects above, matching appendix "
        "A's reading -- but whether \"&\" in appendix B actually means "
        "\"DMA-exclusive\" cannot be settled from the quotation available "
        "here. Recorded as a measurement, not resolved either way.");

    // ---------------- NOT ASSERTED: WIDER BOUNDARY THAN denise_bitplanes ----------------
    checks.note(
        "NOT ASSERTED (9 claims), each needing a boundary wider than this "
        "module's own ports (reg_address_in, data_in, chip48, hires, "
        "shres, hpos, bpldata, c1, c3, clk, clk7_en, reset, aga -- no "
        "pointer, modulo, Copper or display-window port anywhere in this "
        "fixture): "
        "3-scroll-copper.c1 (sol, p.80) \"resetting of pointers or "
        "data-fetch registers can be handled by the Copper\" -- no Copper "
        "port here. "
        "3-dual.c39 (opus, p.72) and 3-dual.c41 (opus, p.72), the -2/-4 "
        "pointer and modulo adjustment for the non-scrolled playfield -- "
        "no pointer or modulo port here. "
        "3-scroll.c2 and 3-scroll.c3 (sol, p.80), vertical scrolling by "
        "incrementing/decrementing the bitplane starting pointer -- no "
        "pointer port here. "
        "3-vscr.c02 (opus, p.81), the pointer-increase-scrolls-upward "
        "relationship -- same reason. "
        "3-hscroll.c4 (sol, p.82), bits disappearing off the display "
        "window's right-hand edge as the display shifts -- needs "
        "DIWSTRT/DIWSTOP, no display-window port here. "
        "3-hscr.f09 (opus, p.84, figure 3-25) and 3-hscr.f11 (opus, p.84, "
        "figure 3-27), memory-picture width and per-line DMA start address "
        "-- Agnus DMA address generation, not observable at this Denise "
        "boundary. None of these are resolved by declining them; they "
        "would need a different, wider fixture.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
