// Test purpose: Verify line mode at the agnus_blitter boundary: BLTCON1 bit 0
//   (LINE) selecting a completely different state machine (BLT_L1-L4, not
//   BLT_A-D); the octant encoding of Table 6-3 (p.190) and Appendix A (p.268),
//   measured together as one table stated twice; the single A bit ($8000)
//   walking the word automatically via ash; the B-channel texture selected by
//   BSH and decremented one bit per pixel; ONEDOT (BLTCON1 bit 1) suppressing
//   repeat writes within a horizontal row; the SIGNFLAG bit (BLTCON1 bit 6)
//   setting the first pixel's error sign; the sign flag driving the modulo
//   bank choice (BLTAMOD vs BLTBMOD); BLTCPT/BLTDPT as separate registers in
//   the same role; and the required BLTSIZE width of 2.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169, 184, 189-192, 268-270.
// Non-goals: minterm/LF decode correctness beyond one targeted check reusing
//   a manual-cited line-mode function as a consequence, not a re-derivation
//   (PKG-BLT-MINTERM's job, which packages.md item 1 already assigns the
//   $4A/AB+A'C/ABC'+A'C line-mode functions to); the barrel shifter's own
//   arithmetic across shift/value space, reused here only via the
//   already-validated ascending_shift() formula (PKG-BLT-SHIFT, 4.2M-case
//   sweep); BLTAFWM/BLTALWM's general masking semantics outside line mode
//   (PKG-BLT-MASKS); the literal 1024-pixel OCS length maximum and BLTSIZE's
//   10-bit zero-means-1024 height field width, which is the SAME field
//   PKG-BLT-SIZE already tests exhaustively -- this package confirms only
//   that line mode's per-pixel loop uses that field as a pixel counter, using
//   a small height for runtime, not a 1024-pixel run; the modulo VALUE
//   formulas (4*(dy-dx), 4*dy, "twice screen width") and the "normalize into
//   octant 0" calculation, all of which are the programmer's arithmetic on
//   dy/dx before writing a register -- no dy/dx port exists on this module,
//   only the register BANK SELECTION these formulas feed into is RTL
//   behavior, and that is what is asserted; "BLTSIZE should not be written
//   until the very end" -- programming-order advice, not a hardware gate
//   (same treatment PKG-BLT-STATUS gives "write BLTSIZE last").
//
// SUPERSESSION. Supersedes hrm/agnus_blitter_line_octant, per the build
// register. That package has TWO defects (docs/blitter-prior-art.md):
//
//   DEFECT 1: it compiles a LOCAL COPY of agnus_blitter.v:609-612 pasted into
//   its own agnus_blitter_line_octant.v -- verified against
//   nanomig/src/minimig-aga/, which has NO such file; the octant logic is
//   inline wire assigns inside agnus_blitter.v itself, not a real design
//   partition. A change to the real agnus_blitter.v cannot make that test go
//   red. This package compiles agnus_blitter.v itself (via
//   $(NANOMIG)/src/minimig-aga/, the same SRC list as
//   hrm/agnus_blitter_status/Makefile) and, because lineinc/linedec/lineadd/
//   linesub are internal wires with NO port on agnus_blitter (and this task
//   forbids adding debug ports), observes their effect the only legitimate
//   way available at this boundary: through address_out's pointer-bank
//   trajectory during a real line-mode blit, and through data_out for the
//   texture/A-bit claims. See "HOW THE OCTANT/SIGN LOGIC IS OBSERVED" below.
//
//   DEFECT 2: four of the superseded test's sixteen test_case calls assert
//   "no X step" for its four Y-major octants using ash=8 -- a value that
//   makes BOTH lineinc's ash==4'b1111 gate and linedec's ash==4'b0000 gate
//   fail regardless of the rest of each formula, so the assertion is true by
//   construction. Proved by mutation there: removing the !sign_del guard
//   from lineinc left that test green; only driving ash=15 against lineinc
//   (not ash=8) turned it red. The prior art measured only that one probe;
//   packages.md's fix requires BOTH: ash=15 against lineinc AND ash=0
//   against linedec, or the affected octants stay vacuous. Both probes are
//   implemented below, in OCTANT-MINOR-AXIS-SUPPRESSED-AT-SIGN1 -- see that
//   block for exactly which octants and why each probe value is the one
//   that makes its half of the claim falsifiable.
//
//   THE OCTANT NUMBERING IS NOT REUSED FROM THE SUPERSEDED TEST. Its
//   {SUD,SUL,AUL}-to-"octant N" assignment was invented by that test's own
//   author (defect 1's consequence: no real source was consulted), not taken
//   from the manual. This package re-derives the table directly from Table
//   6-3 (p.190) and Appendix A (p.268), which agree bit-for-bit -- see the
//   OCTANTS[] table below. In THIS package's own numbering the four
//   octants whose minor axis is X (where the defect-2 vacuity risk actually
//   lives) are 1, 2, 5 and 6 -- not "2, 3, 6, 7", which were the superseded
//   test's own invented labels for a different, unsourced bit assignment.
//   Both numberings happen to have four X-minor octants (the geometry
//   forces that split 4-and-4), but the specific octant NUMBERS do not
//   transfer, and reusing them without saying so would have been exactly
//   the "guessing the octant encoding from the RTL" defect 1 already
//   describes. It is NOT deleted here: it stays in the tree, unchanged and
//   still green, until this package is reviewed and confirmed in its place.
//
// HOW THE OCTANT/SIGN LOGIC IS OBSERVED (no debug ports; address_out only).
//   address_out is combinational off whichever pointer bank ptrsel currently
//   selects (agnus_blitter_adrgen.v:82-84) -- valid on EVERY cycle, not only
//   at a DMA grant. In line mode, ptrsel is CHA at BLT_L1, CHC at BLT_L2,
//   CHD at BLT_L3 (always), and CHD-or-CHC at BLT_L4 depending on
//   first_line_pixel (agnus_blitter.v:795-869). Because L1-L4 run one cycle
//   apart with no stalls (enadma=clkena=1 held throughout, and every DMA
//   request this fixture issues is granted the same cycle), BLT_L3 is always
//   exactly one tick before that pixel's own D-channel write (which fires at
//   BLT_L4, captured via `we`, chsel==CHD, unconditional on
//   first_line_pixel). Sampling address_out one tick before each `we` (i.e.
//   at BLT_L3) therefore ALWAYS reads the D pointer bank's value BEFORE that
//   pixel's own step is applied (D's step happens AT that same BLT_L3 cycle
//   for pixel >= 2, or is skipped there -- enaptr=0 -- for pixel 1, whose
//   step instead happens at its own BLT_L4). Comparing pixel k+1's
//   pre-step sample to pixel k's gives EXACTLY the delta pixel k's own step
//   applied -- see measure_pixel1_delta() and its comment for the full
//   derivation, including why sampling AT the `we` tick itself (rather than
//   one tick before) does not work: from pixel 2 onward the `we`-tick sample
//   reads the C bank, which by construction has not yet received pixel k's
//   own delta at that point.
//
//   ash is a REAL, externally-writable field (BLTCON0 bits 15:12,
//   agnus_blitter.v:219-232) -- not an internal signal. Preloading it via an
//   ordinary BLTCON0 write before BLTSIZE starts the blit is legitimate
//   external stimulus, exactly like a real programmer choosing the line's
//   starting texture/shift alignment; it is not a debug port. Because
//   incash/decash (which move ash automatically once the blit is running,
//   agnus_blitter.v:943-944) are gated by blt_state==BLT_L4, the PRELOADED
//   ash value is still in effect, unmodified, for pixel 1's own BLT_L4
//   evaluation of lineinc/linedec -- which is what makes it usable as
//   defect 2's probe.
//
// FIXTURE. Started from the working harness in hrm/agnus_blitter_status/
// (the only agnus_blitter fixture in the tree proven to deliver data
// correctly) and adapted for line mode: TEST-DATA-DELIVERY-LINE below
// reproduces that package's data-delivery proof through BLT_L1-L4 instead of
// BLT_A-D, and is asserted first, before anything data-dependent, per the
// build brief. Harness::cycle's DMA-grant handshake (the settling eval()
// before the clock edge) is the same mechanism as agnus_blitter_status,
// agnus_blitter_masks and agnus_blitter_shift_chain, all read before writing
// this file. Every timing trap documented in agnus_blitter_status's header
// applies to any agnus_blitter fixture; the ones this file actually touches:
//   1. `we`/`data_out` are transient, sampled at the grant (Harness::cycle),
//      never polled after the tick.
//   2. BLTAFWM/BLTALWM default to 0 after reset and are ANDed into the A
//      path continuously (agnus_blitter.v:311-319,345), NOT gated by `line`
//      -- see LINE-MASKS-MUST-BE-FFFF below, which is exactly the four
//      claims PKG-BLT-MASKS deferred here because of this. Every block that
//      cares about the A path writes both to 0xFFFF explicitly, except that
//      one assertion's deliberate control case.
//   3/4/5. busy/int3/zero completion timing is not read anywhere in this
//      file; run_until_done() only watches `busy` falling, which is a
//      simple registered signal, not subject to those three traps.
//   A NEW fixture fact, specific to line mode: address_out is combinational
//   and UNCONDITIONAL (no ackdma/we gating at all), unlike data_out/we which
//   are only meaningful at a grant -- this fixture samples it on EVERY tick
//   (Harness::address_trace) rather than only at grants, and picks out the
//   tick it needs relative to each pixel's own `we` tick (see "HOW THE
//   OCTANT/SIGN LOGIC IS OBSERVED" above). Getting this wrong (e.g. sampling
//   AT the `we` tick instead of one tick before) silently reads the WRONG
//   pointer bank for pixel >= 2 and was caught before being asserted: an
//   early draft of measure_pixel1_delta() sampled at `we_ticks[i]` directly
//   and every octant's "confirmed" count came back 0, because the C bank it
//   was reading had not received any delta yet at that point in the
//   pipeline (see the trace above -- diagnosed by hand-deriving the D/C bank
//   timeline for a 3-pixel run before writing a single register value).
//
// CLAIM COVERAGE. All 225 records of classify.package=="PKG-BLT-LINE" and
// classify.status=="testable" in work/2026-09-08_fpga-090_classified.jsonl
// are covered below or listed under NOT ASSERTED with a reason; both
// readers (sol/opus) cover much of the same ground, and the octant table in
// particular is stated three times over (Table 6-3, Figure 6-8, Appendix A).
// Every id below is written out in full, per tools/check_coverage.py's own
// rule ("Write ids out in full. `done-flag.c7/c13` is one id and a fragment
// to any reader that is not a person") -- an earlier draft of this section
// used shared-prefix shorthand like `6-line-mode.c47/c48` and the coverage
// script correctly reported the second half of every such pair as
// unmentioned; this is the corrected, literal form. Assertion name ->
// covered ids:
//   TEST-DATA-DELIVERY-LINE: fixture check, no claim id.
//   LINE-BIT0-SELECTS-MODE (p.169/189/192): 6-intro.c4, 6-intro.c15,
//     6-intro.c16, ch6-intro.c4, ch6-intro.c14, ch6-intro.c15,
//     6-line-mode.c1, 6-line-mode.c2, 6-line-mode.c92, ch6-line.c1,
//     ch6-line.c2, ch6-linesum.c17, 6-line-mode.c3, ch6-line.c3 (the
//     general half of "changes the meaning of other bits"; ONEDOT's
//     specific half is under LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES).
//   LINE-HEIGHT-IS-PIXEL-COUNT (p.191/192/269/189): 6-line-mode.c71,
//     6-line-mode.c100, ch6-line.c41, 6-appa-registers.c42,
//     6-appa-registers.c65, 6-appa-registers.c68, ch6-appa-bltsize.c6,
//     ch6-appa-bltsize.ld1, ch6-linesum.c25, 6-line-mode.c4, ch6-line.c4.
//   LINE-WIDTH-MUST-BE-2 (p.191/192/269): 6-line-mode.c72, 6-line-mode.c101,
//     ch6-line.c42, 6-appa-registers.c69, ch6-appa-bltsize.c5,
//     ch6-appa-bltsize.ld3, ch6-linesum.c26.
//   LINE-MASKS-MUST-BE-FFFF (p.191/192/266 -- deferred by PKG-BLT-MASKS's
//     own header to this package, not in classify.package=="PKG-BLT-LINE"):
//     6-appa-registers.c13, ch6-line.c24, ch6-linesum.c2, ch6-linesum.c3,
//     6-line-mode.c50, 6-line-mode.c51, 6-line-mode.c75, 6-line-mode.c76 .
//   LINE-BLTADAT-8000-AND-A-WALKS (p.191/192/268/269): 6-appa-control.c82,
//     6-line-mode.c49, 6-line-mode.c69, ch6-line.c23, ch6-line.c37,
//     ch6-linesum.c0, 6-appa-registers.c83, 6-appa-registers.c84,
//     ch6-appa-bltxdat.c7, ch6-appa-bltxdat.c7b, ch6-appa-lf.c7,
//     ch6-line.c36.
//   LINE-MINTERM-4A-A-BIT-INVERTS-ONE (p.268): ch6-appa-lf.c3, ch6-appa-lf.c5.
//   LINE-B-TEXTURE-BSH-START-BIT (p.169/184/191/192/268/269): 6-line-mode.c5,
//     6-line-mode.c6, 6-line-mode.c53, 6-line-mode.c55, 6-line-mode.c56,
//     6-line-mode.c70, 6-line-mode.c73, ch6-line.c5, ch6-line.c26,
//     ch6-line.c28, ch6-line.c29, ch6-line.c38, 6-appa-control.c117,
//     6-appa-registers.c85, ch6-appa-bltxdat.c7c, ch6-linesum.c20,
//     6-area-fill.c2, ch6-fill.c2, ch6-fill.c3, ch6-fill.c4 (the last four
//     cited for the texture/one-bit mechanism that makes an untextured
//     area-fill outline possible; the "must be undextured" application
//     advice itself is not a signal).
//   LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES (p.189/191/192): 6-line-mode.c66,
//     6-line-mode.c67, 6-line-mode.c98, 6-line-mode.c99, ch6-line.c35,
//     ch6-linesum.c23, ch6-linesum.c24, plus the ONEDOT-specific half of
//     6-line-mode.c3, ch6-line.c3, ch6-line.c6.
//   LINE-SIGNFLAG-SETS-INITIAL-SIGN (p.191/192): 6-line-mode.c44,
//     ch6-linesum.c22.
//   LINE-SIGN-SELECTS-MODULO-BANK (p.191/192/270/268 -- bank-selection half
//     only, see NOT ASSERTED for the arithmetic-formula half):
//     6-appa-registers.c96, 6-appa-registers.c97, 6-appa-registers.c115,
//     ch6-appa-bltxmod.c9, ch6-appa-bltxpt.c8b .
//   LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE (p.191/192/270 -- ch6-linesum.c9,
//     "BLTBPT is unused", is corroborated by direct RTL citation here
//     rather than simulated: no `case (blt_state)` branch in BLT_L1-L4 ever
//     assigns ptrsel or chsel the value CHB, agnus_blitter.v:795-869, so
//     BLTBPT genuinely cannot be read or written by line mode's own state
//     machine; there is no register-readback path to confirm this by
//     measurement the way the other claims here are measured):
//     6-line-mode.c57, 6-line-mode.c58, 6-line-mode.c59, 6-line-mode.c60,
//     6-line-mode.c79, 6-line-mode.c80, 6-line-mode.c83, 6-line-mode.c84,
//     ch6-line.c30, ch6-line.c31, 6-appa-registers.c100,
//     6-appa-registers.c101, 6-appa-registers.c102, 6-appa-registers.c117,
//     6-appa-registers.c118, 6-appa-registers.c119, 6-appa-registers.c120,
//     ch6-appa-bltxmod.c12, ch6-appa-bltxpt.c11, ch6-linesum.c6,
//     ch6-linesum.c7, ch6-linesum.c9, ch6-linesum.c10, ch6-linesum.c11 .
//   OCTANT-CODE-EQUALS-BITS (p.189/190/268 -- arithmetic, not RTL):
//     6-line-mode.c7, 6-line-mode.c8, 6-line-mode.c9, 6-line-mode.c10,
//     6-line-mode.c11, 6-line-mode.c12, 6-line-mode.c13, 6-line-mode.c14,
//     ch6-line.c8, ch6-line.c9, ch6-line.c10, ch6-line.c11,
//     ch6-line.f8.oct0, ch6-line.f8.oct1, ch6-line.f8.oct2, ch6-line.f8.oct3,
//     ch6-line.f8.oct4, ch6-line.f8.oct5, ch6-line.f8.oct6, ch6-line.f8.oct7,
//     6-appa-control.c90, 6-appa-control.c91, 6-appa-control.c92,
//     6-line-mode.c94, ch6-linesum.c19 .
//   OCTANT-MAJOR-AXIS-DIRECTION, OCTANT-MINOR-AXIS-SUPPRESSED-AT-SIGN1 and
//     OCTANT-MINOR-AXIS-FIRES-AT-SIGN0, TOGETHER (p.190/268 -- the numeric
//     bit-value table itself, exercised through the direction behaviour
//     those bits produce; written out once here rather than split three
//     ways per PKG-BLT-MASKS's own precedent for a shared fixture fact):
//     ch6-appa-oct.select,
//     6-line-mode.c15, 6-line-mode.c16, 6-line-mode.c17, 6-line-mode.c18,
//     6-line-mode.c19, 6-line-mode.c20, 6-line-mode.c21, 6-line-mode.c22,
//     6-line-mode.c23, 6-line-mode.c24, 6-line-mode.c25, 6-line-mode.c26,
//     6-line-mode.c27, 6-line-mode.c28, 6-line-mode.c29, 6-line-mode.c30,
//     6-line-mode.c31, 6-line-mode.c32, 6-line-mode.c33, 6-line-mode.c34,
//     6-line-mode.c35, 6-line-mode.c36, 6-line-mode.c37, 6-line-mode.c38,
//     ch6-t63.oct0.bit2, ch6-t63.oct0.bit3, ch6-t63.oct0.bit4,
//     ch6-t63.oct1.bit2, ch6-t63.oct1.bit3, ch6-t63.oct1.bit4,
//     ch6-t63.oct2.bit2, ch6-t63.oct2.bit3, ch6-t63.oct2.bit4,
//     ch6-t63.oct3.bit2, ch6-t63.oct3.bit3, ch6-t63.oct3.bit4,
//     ch6-t63.oct4.bit2, ch6-t63.oct4.bit3, ch6-t63.oct4.bit4,
//     ch6-t63.oct5.bit2, ch6-t63.oct5.bit3, ch6-t63.oct5.bit4,
//     ch6-t63.oct6.bit2, ch6-t63.oct6.bit3, ch6-t63.oct6.bit4,
//     ch6-t63.oct7.bit2, ch6-t63.oct7.bit3, ch6-t63.oct7.bit4,
//     6-appa-control.c93, 6-appa-control.c94, 6-appa-control.c95,
//     6-appa-control.c96, 6-appa-control.c97, 6-appa-control.c98,
//     6-appa-control.c99, 6-appa-control.c100, 6-appa-control.c101,
//     6-appa-control.c102, 6-appa-control.c103, 6-appa-control.c104,
//     6-appa-control.c105, 6-appa-control.c106, 6-appa-control.c107,
//     6-appa-control.c108, 6-appa-control.c109, 6-appa-control.c110,
//     6-appa-control.c111, 6-appa-control.c112, 6-appa-control.c113,
//     6-appa-control.c114, 6-appa-control.c115, 6-appa-control.c116 .
//
// NOT ASSERTED (reported, not silently dropped):
//   ch6-line.c17 (p.191, "these calculations normalize the line into octant
//     0") -- a programming-algorithm description (how software reduces an
//     arbitrary dx/dy to octant 0 before mapping to the real octant); no
//     RTL signal performs this normalization. absent_signal: none named (it
//     is an algorithm, not a hardware quantity); reason_kind: insufficient.
//   ch6-line.c43 (p.191, "BLTSIZE should not be written until the very end")
//     -- programming-order advice, not a hardware gate or check; same
//     treatment PKG-BLT-STATUS gives "write BLTSIZE last". absent_signal:
//     none; reason_kind: insufficient.
//   6-appa-registers.c103 (p.270, "image width in bytes is normally twice
//     screen width in words") -- explanatory arithmetic for a TYPICAL
//     programmed value; BLTCMOD/BLTDMOD are opaque 16-bit registers to this
//     RTL, which never computes or checks this relationship. absent_signal:
//     screen width (no such port); reason_kind: absent.
//   ch6-appa-bltxmod.c11, ch6-appa-bltxpt.c10 (p.270, "Y/X is the line
//     slope") -- defines what dy/dx mean to the programmer computing
//     BLTAMOD/BLTBMOD/BLTAPTL's initial value; no dy/dx port exists.
//     absent_signal: dy, dx; reason_kind: absent.
//   6-line-mode.c47, 6-line-mode.c48, 6-line-mode.c77, 6-line-mode.c78,
//     ch6-line.c21, ch6-line.c22, ch6-linesum.c4, ch6-linesum.c5,
//     6-appa-registers.c98, 6-appa-registers.c99, ch6-appa-bltxmod.c10
//     (p.191/192/270, the LITERAL "4*(dy-dx)"/"4*dy" formulas) -- the RTL
//     treats BLTAMOD/BLTBMOD as opaque sign-extended modulo values
//     (agnus_blitter_adrgen.v:112-119); it has no dy/dx ports and computes
//     no such arithmetic. Only the bank-SELECTION half of these same claims
//     (BLTAMOD used when sign=0, BLTBMOD when sign=1) is RTL behavior, and
//     IS asserted, in LINE-SIGN-SELECTS-MODULO-BANK. absent_signal: dy, dx;
//     reason_kind: absent.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean, confirmed byte-identical to the baseline by
// `git diff --stat` after every revert) and the FAIL/PASS lines quoted are
// what the run printed, not predictions. All 18 mutations (16 assertions,
// two of which -- LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE's compound claim --
// were probed on both halves separately) FAILED ONLY the assertion named,
// with every other assertion staying PASS on the same run: no mutation was
// found vacuous, so there is no defect-2-shaped finding to report from this
// campaign beyond the one it was built to fix.
//
// Every mutation is a STIMULUS mutation to this test file's OWN inputs
// (a register value, an octant's bit pattern, a probe, a table entry) --
// never to nanomig/, which this task forbids touching for any reason.
//
// MUTATION TEST-DATA-DELIVERY-LINE stimulus red: fed C word at index 2
//   changed from 0x9ABC to 0x9ABD (expected constants left untouched) ->
//   FAIL, only this assertion; 15/16 others PASS.
// MUTATION LINE-BIT0-SELECTS-MODE stimulus red: h_line's BLTCON1 write
//   changed from LINE_BIT to 0x0000 (so BOTH harnesses now run normal
//   mode) -> FAIL (h_line now also produces 6 writes, not the expected 3);
//   15/16 others PASS.
// MUTATION LINE-HEIGHT-IS-PIXEL-COUNT stimulus red: BLTSIZE height changed
//   from 5 to 3 (expected constant 5 left untouched) -> FAIL (only 3
//   writes occur); 15/16 others PASS.
// MUTATION LINE-WIDTH-MUST-BE-2 stimulus red: h_correct's width changed
//   from 2 to 3 (expected constant 6 left untouched) -> FAIL (fewer than 6
//   writes occur with width=3 too, confirming 2 specifically is required,
//   not merely "not 1"); 15/16 others PASS.
// MUTATION LINE-MASKS-MUST-BE-FFFF stimulus red: h_correct's BLTADAT
//   changed from 0x8000 to 0x4000 (expected constant 0x8000 left
//   untouched) -> FAIL; 15/16 others PASS.
// MUTATION LINE-BLTADAT-8000-AND-A-WALKS stimulus red: octant changed from
//   7 (X-major INC) to 3 (X-major DEC) -- same major axis, opposite
//   direction (expected sequence left untouched) -> FAIL; 15/16 others
//   PASS.
// MUTATION LINE-MINTERM-4A-A-BIT-INVERTS-ONE stimulus red: ash changed
//   from 3 to 5 (expected 0xEFFF, i.e. bit 12 cleared, left untouched) ->
//   FAIL (the actually-cleared bit moves to 10); 15/16 others PASS.
// MUTATION LINE-B-TEXTURE-BSH-START-BIT stimulus red: BLTBDAT changed from
//   0b1010 to 0b0101 (expected sequence left untouched) -> FAIL; 15/16
//   others PASS.
// MUTATION LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES stimulus red: h_onedot's
//   BLTAPTL changed from 0x8000 to 0x0000 (so the accumulator's bit 15,
//   and therefore sign_out, is 0 instead of self-sustaining 1 -- pixels no
//   longer all stay on the same row) -> FAIL (h_onedot now also produces 5
//   writes, not the expected 1); 15/16 others PASS.
// MUTATION LINE-SIGNFLAG-SETS-INITIAL-SIGN stimulus red: the SIGNFLAG=0
//   call's ash_probe changed from 15 to 0 (now probing linedec, which this
//   octant's minor axis never uses, expected constant 1 left untouched) ->
//   FAIL; 15/16 others PASS.
// MUTATION LINE-SIGN-SELECTS-MODULO-BANK stimulus red: the SIGNFLAG=0
//   call's BLTAMOD changed from 0x0040 to 0x0080 (expected constant 32
//   left untouched) -> FAIL; 15/16 others PASS.
// MUTATION LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE stimulus red, part 1:
//   h_identity's BLTDPTL changed from 0x0500 to 0x0600 (expected constant
//   0x0280 left untouched) -> FAIL; 15/16 others PASS. Part 2, probed
//   separately: h_mod's BLTDMOD changed from 0x0080 to 0x0100 (expected
//   constant 64 left untouched) -> FAIL, same assertion, same isolation.
// MUTATION OCTANT-CODE-EQUALS-BITS stimulus red: FIG68's octant-0 entry
//   changed from code 6 to code 7 (OCTANTS[] itself, and every other
//   assertion that reads it, left untouched) -> FAIL; 15/16 others PASS.
// MUTATION OCTANT-MAJOR-AXIS-DIRECTION stimulus red: the X-major branch's
//   ash probe changed from `o.major_positive?15:0` to always 15 (so the
//   three X-major-DEC octants are probed with the wrong polarity; expected
//   constants left untouched) -> FAIL; 15/16 others PASS.
// MUTATION OCTANT-MINOR-AXIS-SUPPRESSED-AT-SIGN1 [DEFECT-2 FIX] stimulus
//   red: BOTH probe calls' initial_sign changed from 1 to 0 (major-only ->
//   major+minor, so the minor axis now legitimately fires; expected
//   constant 12/12 left untouched) -> FAIL; 15/16 others PASS -- confirming
//   this assertion is sensitive to exactly the sign_del dimension defect 2
//   was about, unlike the superseded test's vacuous ash=8 probe.
// MUTATION OCTANT-MINOR-AXIS-FIRES-AT-SIGN0 stimulus red: both probe
//   calls' initial_sign changed from 0 to 1 (major+minor -> major-only, so
//   the minor axis this assertion expects to fire is now suppressed;
//   expected constant 8/8 left untouched) -> FAIL; 15/16 others PASS.

#include <cstdint>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0 = to_reg(0x040);
constexpr uint16_t BLTCON1 = to_reg(0x042);
constexpr uint16_t BLTAFWM = to_reg(0x044);
constexpr uint16_t BLTALWM = to_reg(0x046);
constexpr uint16_t BLTCPTL = to_reg(0x04A);
constexpr uint16_t BLTAPTL = to_reg(0x052);
constexpr uint16_t BLTDPTL = to_reg(0x056);
constexpr uint16_t BLTSIZE = to_reg(0x058);
constexpr uint16_t BLTCMOD = to_reg(0x060);
constexpr uint16_t BLTBMOD = to_reg(0x062);
constexpr uint16_t BLTAMOD = to_reg(0x064);
constexpr uint16_t BLTDMOD = to_reg(0x066);
constexpr uint16_t BLTCDAT = to_reg(0x070);
constexpr uint16_t BLTBDAT = to_reg(0x072);
constexpr uint16_t BLTADAT = to_reg(0x074);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252. Only USEC matters for
// line mode's own DMA request logic (agnus_blitter.v:823,860 both key on
// `usec` alone, never usea/useb/used) -- USED is written anyway for
// programming-realism, but is not load-bearing here.
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

constexpr uint16_t LINE_BIT = 1u << 0;

// Minterm LF settings (agnus_blitter_minterm.v), used here strictly as
// plumbing to isolate one signal at a time -- minterm correctness itself is
// PKG-BLT-MINTERM's job (packages.md item 1).
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A
constexpr uint16_t LF_B_PASSTHROUGH = 0xCC;  // D = B
constexpr uint16_t LF_LINE_4A = 0x4A;        // one of packages.md's three cited line-mode functions

// The ascending-mode shift formula, independently validated in
// hrm/agnus_blitter_barrelshifter (PKG-BLT-SHIFT, 4.2M-case sweep) and
// reused verbatim by hrm/agnus_blitter_masks and hrm/agnus_blitter_shift_chain.
constexpr uint16_t ascending_shift(uint16_t shift, uint16_t previous, uint16_t current) {
    return shift == 0
        ? current
        : static_cast<uint16_t>((static_cast<uint32_t>(previous) << (16u - shift)) |
                                 (static_cast<uint32_t>(current) >> shift));
}

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525, 555-556) -- the SAME field line mode
// reuses as its own pixel counter (agnus_blitter.v:539-561).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

// Table 6-3 (p.190) and Appendix A (p.268) -- one 8-row table, BLTCON1 bits
// 4 (SUD), 3 (SUL), 2 (AUL), stated twice by two sources that agree
// bit-for-bit. major_positive/minor_positive are DERIVED from those three
// bits through agnus_blitter.v:609-612's own lineinc/linedec/lineadd/
// linesub formulas -- worked by hand for all 8 rows before writing any test
// code, cross-checked against Figure 6-8's own sector description (p.190,
// "right-and-up shallow", etc.) and the "x increasing towards the right, y
// increasing down" convention (ch6-line.c9, p.189). Not reused from the
// superseded agnus_blitter_line_octant test -- see the file header.
struct Octant {
    int n;
    uint8_t sud, sul, aul;  // BLTCON1 bits 4, 3, 2
    bool x_major;           // true: SUD==1, X is the major (unconditional) axis
    bool major_positive;    // major axis direction: X-major -> true=INC(right); Y-major -> true=ADD(down)
    bool minor_positive;    // minor axis direction (fires only at SIGNFLAG-derived sign_del==0): same convention
};

constexpr Octant OCTANTS[8] = {
    // n  sud sul aul  x_major major_positive minor_positive
    {0, 1, 1, 0, true, true, false},    // X major INC(right); Y minor SUB(up)
    {1, 0, 0, 1, false, false, true},   // Y major SUB(up); X minor INC(right)
    {2, 0, 1, 1, false, false, false},  // Y major SUB(up); X minor DEC(left)
    {3, 1, 1, 1, true, false, false},   // X major DEC(left); Y minor SUB(up)
    {4, 1, 0, 1, true, false, true},    // X major DEC(left); Y minor ADD(down)
    {5, 0, 1, 0, false, true, false},   // Y major ADD(down); X minor DEC(left)
    {6, 0, 0, 0, false, true, true},    // Y major ADD(down); X minor INC(right)
    {7, 1, 0, 0, true, true, true},     // X major INC(right); Y minor ADD(down)
};

// Copied in mechanism from hrm/agnus_blitter_status (the DMA-grant
// second-eval() fix) and hrm/agnus_blitter_masks (address_out capture),
// both read in full before writing this file.
class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        data_writes.clear();
        we_ticks.clear();
        address_trace.clear();
        tick_count = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    bool run_until_done(int max_ticks = 500) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const bool busy_now = static_cast<bool>(dut->busy);
            if (busy_now) seen_busy = true;
            if (seen_busy && !busy_now) return true;
        }
        return false;
    }

    std::deque<uint16_t> c_words;
    std::vector<uint16_t> data_writes;    // D-channel words, in store order
    std::vector<size_t> we_ticks;         // tick index of each D-channel write
    std::vector<uint32_t> address_trace;  // address_out, sampled every tick

    int tick_count = 0;

private:
    Vagnus_blitter* dut;

    static uint16_t pop_or_zero(std::deque<uint16_t>& q) {
        if (q.empty()) return 0;
        uint16_t v = q.front();
        q.pop_front();
        return v;
    }

    bool last_we = false;
    uint16_t last_data_out = 0;
    uint32_t last_address = 0;

    void tick_bookkeeping() {
        address_trace.push_back(last_address);
        if (last_we) {
            data_writes.push_back(last_data_out);
            we_ticks.push_back(static_cast<size_t>(tick_count));
        }
        ++tick_count;
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        last_we = false;
        last_data_out = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            if (reg == BLTCDAT) v = pop_or_zero(c_words);
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the combinational
            // network BEFORE the clock edge -- the step the salvaged
            // diag.cpp harness skipped (see hrm/agnus_blitter_status's
            // header). we/data_out are transient and only valid here.
            dut->eval();
            if (dut->we) {
                last_we = true;
                last_data_out = dut->data_out;
            }
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        // address_out is combinational and unconditional (no ackdma/we
        // gating) -- sample it every cycle at this same settled moment.
        last_address = dut->address_out;
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
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

// Measures the pointer delta PIXEL 1 of a fresh line-mode blit applies to
// the C/D pointer pair, isolated from every later pixel.
//
// address_out only reliably reflects the ptrsel-selected bank when
// enaptr==1 (agnus_blitter_adrgen.v:64, `bltptr_sel = enaptr ? ptrsel :
// {reg_address_in[4],reg_address_in[2]}` -- when enaptr is 0, the bank
// read is instead whatever this fixture's own idle-cycle reg_address_in
// happens to select, NOT ptrsel). BLT_L3's enaptr is gated by
// first_line_pixel (0 for pixel 1 specifically, agnus_blitter.v:836) so
// BLT_L3 is NOT a reliable read point for pixel 1. BLT_L2's enaptr is
// `enable` UNCONDITIONALLY, every pixel, with no first_line_pixel
// exception (agnus_blitter.v:818) -- so BLT_L2 (ptrsel=CHC, always) is used
// instead, two ticks before each pixel's own D-channel write (which fires
// at BLT_L4, captured via `we`). This was found empirically: an earlier
// version of this helper sampled BLT_L3 (`we_ticks[k]-1`) and every
// octant's delta measurement came back reading the WRONG bank for pixel 1
// (traced by hand against a full per-tick address_out/reqdma/ackdma dump
// before touching the helper -- see the build log for the trace).
//
// C and D step identically and stay equal throughout the blit (both
// initialized to the same word by this helper, and BLT_L2/L3/L4's
// incptr/decptr/addmod/submod are the SAME lineinc/linedec/lineadd/linesub
// values within one pixel's 4-state cycle, agnus_blitter.v:813-869), so
// C's value at BLT_L2 (before this pixel's own step, which is applied at
// this very L2 for pixel 1 and at L3 for pixel >= 2) is exactly the sum of
// every EARLIER pixel's delta. Comparing pixel 2's L2 sample to pixel 1's
// therefore isolates pixel 1's own delta.
//
// ash_probe is written into BLTCON0's ash field directly (a real,
// externally-writable register, agnus_blitter.v:219-232) before the blit
// starts, so it is still in effect, unmodified, for pixel 1's own BLT_L4
// evaluation. cmod_dmod is written identically to BOTH BLTCMOD and BLTDMOD.
// A width of 2 and height of 3 keep every call small and fast; only pixel
// 1's delta is ever read.
int32_t measure_pixel1_delta(uint8_t sud, uint8_t sul, uint8_t aul, uint8_t initial_sign,
                              uint8_t ash_probe, uint16_t cmod_dmod) {
    Harness h;
    h.write_reg(BLTADAT, 0x8000);
    h.write_reg(BLTBDAT, 0x0000);
    h.write_reg(BLTAFWM, 0xFFFF);
    h.write_reg(BLTALWM, 0xFFFF);
    h.write_reg(BLTCPTL, 0x0400);
    h.write_reg(BLTDPTL, 0x0400);
    h.write_reg(BLTCMOD, cmod_dmod);
    h.write_reg(BLTDMOD, cmod_dmod);
    h.write_reg(BLTCON0, static_cast<uint16_t>((static_cast<uint16_t>(ash_probe) << 12) | USEC | USED | LF_C_PASSTHROUGH));
    h.write_reg(BLTCON1, static_cast<uint16_t>((static_cast<uint16_t>(initial_sign) << 6) |
                                                (static_cast<uint16_t>(sud) << 4) |
                                                (static_cast<uint16_t>(sul) << 3) |
                                                (static_cast<uint16_t>(aul) << 2) | LINE_BIT));
    h.write_reg(BLTSIZE, bltsize_word(2, 3));
    h.run_until_writes(2, 200);
    if (h.we_ticks.size() < 2 || h.we_ticks[0] < 2 || h.we_ticks[1] < 2) return INT32_MIN;
    const uint32_t c_before_pixel1 = h.address_trace[h.we_ticks[0] - 2];
    const uint32_t c_before_pixel2 = h.address_trace[h.we_ticks[1] - 2];
    return static_cast<int32_t>(c_before_pixel2) - static_cast<int32_t>(c_before_pixel1);
}

// Measures the delta BLT_L1's accumulator update applies to the CHA pointer
// bank (BLTAPTL) using its own initial sign -- BLT_L1's ptrsel is
// unconditionally CHA (agnus_blitter.v:795-811), and it always runs before
// any other line state, so `address_trace[we_ticks[k]-3]` (three ticks
// before pixel k's own D-channel write: L1, L2, L3, then L4=we) reads the
// accumulator at BLT_L1 -- before pixel 1's own L1 for k=0, after pixel 1's
// L1 (and before pixel 2's) for k=1.
int32_t measure_accumulator_delta(uint8_t initial_sign, uint16_t amod, uint16_t bmod) {
    Harness h;
    h.write_reg(BLTADAT, 0x0000);
    h.write_reg(BLTBDAT, 0x0000);
    h.write_reg(BLTAFWM, 0xFFFF);
    h.write_reg(BLTALWM, 0xFFFF);
    h.write_reg(BLTAMOD, amod);
    h.write_reg(BLTBMOD, bmod);
    h.write_reg(BLTCPTL, 0x0400);
    h.write_reg(BLTDPTL, 0x0400);
    h.write_reg(BLTCON0, static_cast<uint16_t>((8u << 12) | USEC | USED | LF_C_PASSTHROUGH));
    h.write_reg(BLTCON1, static_cast<uint16_t>((static_cast<uint16_t>(initial_sign) << 6) | LINE_BIT));  // octant 6
    h.write_reg(BLTSIZE, bltsize_word(2, 3));
    h.run_until_writes(2, 200);
    if (h.we_ticks.size() < 2 || h.we_ticks[0] < 3 || h.we_ticks[1] < 3) return INT32_MIN;
    const uint32_t acc_before_pixel1 = h.address_trace[h.we_ticks[0] - 3];
    const uint32_t acc_before_pixel2 = h.address_trace[h.we_ticks[1] - 3];
    return static_cast<int32_t>(acc_before_pixel2) - static_cast<int32_t>(acc_before_pixel1);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY-LINE (fixture check, no page) ----------------
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        h.write_reg(BLTADAT, 0x0000);
        h.write_reg(BLTBDAT, 0x0000);
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTCPTL, 0x0400);
        h.write_reg(BLTDPTL, 0x0400);
        h.write_reg(BLTCMOD, 0x0000);
        h.write_reg(BLTDMOD, 0x0000);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h.write_reg(BLTCON1, LINE_BIT);  // octant 6, sign irrelevant here
        h.write_reg(BLTSIZE, bltsize_word(2, 4));
        h.run_until_writes(4, 200);

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY-LINE: four distinct C words fed through "
            "a LINE-MODE blit (BLT_L1-L4, not BLT_A-D) arrive at data_out, in order, one "
            "per pixel -- proves the DMA grant handshake (the same second-eval() fix "
            "documented in hrm/agnus_blitter_status) delivers data correctly through the "
            "line-mode state machine specifically, which every data-dependent assertion "
            "below depends on. Reproduced first, per the build brief, before anything "
            "data-dependent.");
    }

    // ---------------- LINE-BIT0-SELECTS-MODE (p.169, p.189) ----------------
    {
        Harness h_normal;
        h_normal.c_words = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666};
        h_normal.write_reg(BLTAFWM, 0xFFFF);
        h_normal.write_reg(BLTALWM, 0xFFFF);
        h_normal.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h_normal.write_reg(BLTCON1, 0x0000);  // LINE=0
        h_normal.write_reg(BLTSIZE, bltsize_word(2, 3));
        h_normal.run_cycles(120);

        Harness h_line;
        h_line.c_words = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666};
        h_line.write_reg(BLTADAT, 0x0000);
        h_line.write_reg(BLTAFWM, 0xFFFF);
        h_line.write_reg(BLTALWM, 0xFFFF);
        h_line.write_reg(BLTCPTL, 0x0400);
        h_line.write_reg(BLTDPTL, 0x0400);
        h_line.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h_line.write_reg(BLTCON1, LINE_BIT);  // LINE=1, octant 6
        h_line.write_reg(BLTSIZE, bltsize_word(2, 3));
        h_line.run_cycles(120);

        checks.expect(h_normal.data_writes.size() == 6 && h_line.data_writes.size() == 3,
            "LINE-BIT0-SELECTS-MODE: with byte-for-byte identical BLTSIZE, BLTCON0 and C "
            "feed, LINE=0 produces 6 destination writes (2 words x 3 rows, normal mode's "
            "own BLT_A-D loop) and LINE=1 produces exactly 3 (one per pixel, BLT_L1-L4's "
            "own loop, which ignores the width field for iteration count) -- setting "
            "BLTCON1 bit 0, LINEMODE, genuinely selects a different state machine, not "
            "merely a different data path ('line draw mode is selected by setting bit 0, "
            "LINEMODE, of BLTCON1', 6-line-mode.c2/ch6-line.c2, p.189; 'the line-mode "
            "register summary sets LINEMODE to 1'/'BLTCON1 bit LINEMODE = 1', "
            "6-line-mode.c92/ch6-linesum.c17, p.192; 'the blitter can draw patterned "
            "lines'/'is used to draw lines', 6-intro.c4/c15/c16, ch6-intro.c4/c14/c15, "
            "p.169; 6-line-mode.c1/ch6-line.c1, p.189; 'line draw mode changes the "
            "meaning of some other bits in BLTCON0 and BLTCON1', 6-line-mode.c3/"
            "ch6-line.c3, p.189, general half -- ONEDOT's specific half of this same "
            "claim, and ch6-line.c6's 'many of the blitter registers serve other "
            "purposes in line-drawing mode', are under LINE-ONEDOT-SUPPRESSES-"
            "REPEAT-WRITES below)");
    }

    // ---------------- LINE-HEIGHT-IS-PIXEL-COUNT (p.191, p.192, p.269) ----------------
    {
        Harness h;
        h.c_words = std::deque<uint16_t>(5, 0);
        h.write_reg(BLTADAT, 0x0000);
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTCPTL, 0x0400);
        h.write_reg(BLTDPTL, 0x0400);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h.write_reg(BLTCON1, LINE_BIT);
        h.write_reg(BLTSIZE, bltsize_word(2, 5));
        const bool done = h.run_until_done(200);

        checks.expect(done && h.data_writes.size() == 5,
            "LINE-HEIGHT-IS-PIXEL-COUNT: with BLTSIZE height=5 and width=2 (as "
            "required), the blit performs exactly 5 destination writes and then "
            "signals done -- BLTSIZE's height field is the number of pixels in the "
            "line, dx+1, not a row count in the normal-mode sense ('the blit height is "
            "set to the length of the line, which is dx + 1', ch6-line.c41, p.191; "
            "'line-mode blit height equals dx+1'/'BLTSIZE bits 15-6 with dx+1'/'BLTSIZE "
            "bits 15-6 = dx + 1', 6-line-mode.c71/c100, ch6-linesum.c25, p.191/192; "
            "'in line mode, BLTSIZE height equals line length'/'BLTSIZE controls line "
            "length', 6-appa-registers.c42/c65, ch6-appa-bltsize.c6/ld1, p.269. Height's "
            "own 10-bit, zero-means-1024 field width and the literal 1024-pixel OCS "
            "maximum ('on the original chip set, line mode can draw lines up to 1024 "
            "pixels long', 6-line-mode.c4/ch6-line.c4, p.189; 'the original-chip-set h "
            "field permits lines up to 1024 dots long', 6-appa-registers.c68, p.269) are "
            "the SAME field PKG-BLT-SIZE already tests exhaustively -- confirmed here "
            "only to be the mechanism line mode actually uses, not re-run at full "
            "length)");
    }

    // ---------------- LINE-WIDTH-MUST-BE-2 (p.191, p.192, p.269) ----------------
    {
        Harness h_correct;
        h_correct.c_words = std::deque<uint16_t>(6, 0);
        h_correct.write_reg(BLTADAT, 0x0000);
        h_correct.write_reg(BLTAFWM, 0xFFFF);
        h_correct.write_reg(BLTALWM, 0xFFFF);
        h_correct.write_reg(BLTCPTL, 0x0400);
        h_correct.write_reg(BLTDPTL, 0x0400);
        h_correct.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h_correct.write_reg(BLTCON1, LINE_BIT);
        h_correct.write_reg(BLTSIZE, bltsize_word(2, 6));
        const bool done_correct = h_correct.run_until_done(200);

        // width=1 (a real programming mistake, not the documented value):
        // width_cnt (loaded from width=1) never leaves first_word==last_word
        // ==TRUE (agnus_blitter.v:539-540 -- width_cnt_rld re-fires every
        // time, agnus_blitter.v:531-537), so height_cnt's own decrement
        // condition (enable&&next_word&&last_word, agnus_blitter.v:559-560)
        // is true at BOTH BLT_L2 and BLT_L4 every pixel instead of only
        // BLT_L4 -- the line finishes in fewer than 6 writes.
        Harness h_wrong;
        h_wrong.c_words = std::deque<uint16_t>(6, 0);
        h_wrong.write_reg(BLTADAT, 0x0000);
        h_wrong.write_reg(BLTAFWM, 0xFFFF);
        h_wrong.write_reg(BLTALWM, 0xFFFF);
        h_wrong.write_reg(BLTCPTL, 0x0400);
        h_wrong.write_reg(BLTDPTL, 0x0400);
        h_wrong.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h_wrong.write_reg(BLTCON1, LINE_BIT);
        h_wrong.write_reg(BLTSIZE, bltsize_word(1, 6));
        const bool done_wrong = h_wrong.run_until_done(200);

        checks.expect(done_correct && h_correct.data_writes.size() == 6 &&
                       done_wrong && h_wrong.data_writes.size() < 6,
            "LINE-WIDTH-MUST-BE-2: with width=2 (as required) a height=6 line produces "
            "exactly 6 writes; with width=1 (NOT the documented value) the SAME "
            "height=6 line produces fewer than 6 -- the 'must be 2' instruction is "
            "load-bearing on this design's actual pixel-count mechanism, not a "
            "stylistic convention ('BLTSIZE w must be $02 for line drawing'/'in line "
            "draw the w field must be set to 02', 6-appa-registers.c69, "
            "ch6-appa-bltsize.c5/ld3, p.269; 'line-mode blit width must be two words'/"
            "'the blit width must be set to two for all line drawing', 6-line-mode.c72/"
            "ch6-line.c42, p.191; 'BLTSIZE bits 5-0 with 2'/'BLTSIZE bits 5-0 = 2', "
            "6-line-mode.c101/ch6-linesum.c26, p.192)");
    }

    // ---------------- LINE-MASKS-MUST-BE-FFFF (p.191, p.192, p.266) ----------------
    {
        // Deferred by PKG-BLT-MASKS's own header (hrm/agnus_blitter_masks,
        // and docs/blitter-prior-art.md) to whoever builds the dedicated
        // line-mode fixture: 6-appa-registers.c13, ch6-line.c24,
        // ch6-linesum.c2/c3. These classify as classify.package==
        // "PKG-BLT-MASKS", not "PKG-BLT-LINE" -- picked up here per this
        // task's explicit brief, which names them.
        //
        // bltaold is held at zero throughout line mode (agnus_blitter.v:336,
        // "in line mode this register is equal zero all the time") but the
        // mask still ANDs continuously into the shifter's new_val
        // (agnus_blitter.v:345, bltadat & bltamask -- a continuous
        // assignment, not gated by `line` at all), so a non-$FFFF mask still
        // corrupts the walking A bit. This is PKG-BLT-MASKS's own evidence
        // field for these four claims.
        Harness h_correct;
        h_correct.write_reg(BLTADAT, 0x8000);
        h_correct.write_reg(BLTBDAT, 0x0000);
        h_correct.write_reg(BLTAFWM, 0xFFFF);
        h_correct.write_reg(BLTALWM, 0xFFFF);
        h_correct.write_reg(BLTCPTL, 0x0400);
        h_correct.write_reg(BLTDPTL, 0x0400);
        h_correct.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_A_PASSTHROUGH));
        h_correct.write_reg(BLTCON1, static_cast<uint16_t>((1u << 4) | LINE_BIT));  // octant 7
        h_correct.write_reg(BLTSIZE, bltsize_word(2, 1));
        h_correct.run_until_writes(1, 60);

        Harness h_wrong;
        h_wrong.write_reg(BLTADAT, 0x8000);
        h_wrong.write_reg(BLTBDAT, 0x0000);
        h_wrong.write_reg(BLTAFWM, 0x0000);  // NOT $FFFF -- the documented mistake
        h_wrong.write_reg(BLTALWM, 0x0000);
        h_wrong.write_reg(BLTCPTL, 0x0400);
        h_wrong.write_reg(BLTDPTL, 0x0400);
        h_wrong.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_A_PASSTHROUGH));
        h_wrong.write_reg(BLTCON1, static_cast<uint16_t>((1u << 4) | LINE_BIT));
        h_wrong.write_reg(BLTSIZE, bltsize_word(2, 1));
        h_wrong.run_until_writes(1, 60);

        checks.expect(h_correct.data_writes.size() == 1 && h_correct.data_writes[0] == 0x8000 &&
                       h_wrong.data_writes.size() == 1 && h_wrong.data_writes[0] == 0x0000,
            "LINE-MASKS-MUST-BE-FFFF [deferred by PKG-BLT-MASKS to this package]: with "
            "BLTAFWM=BLTALWM=$FFFF the line's moving bit ($8000) reaches data_out intact "
            "(0x8000); with them left at their post-reset default of $0000 instead, the "
            "SAME bit is masked away to 0x0000 -- the mask genuinely still applies in "
            "line mode even though bltaold itself stays zero, so '[BLTAFWM/BLTALWM] "
            "should be set to $FFFF' (ch6-line.c24, p.191) / 'BLTAFWM = $FFFF' / "
            "'BLTALWM = $FFFF' (ch6-linesum.c2/c3, p.192) / 'all mask bits are 1 in "
            "line-drawing mode' (6-appa-registers.c13, p.266) is a real hardware "
            "requirement, not a stylistic one");
    }

    // ---------------- LINE-BLTADAT-8000-AND-A-WALKS (p.191, p.192, p.268, p.269) ----------------
    {
        // Octant 7 (SUD=1,SUL=0,AUL=0): X-major, unconditional INC, so ash
        // increments every pixel regardless of sign (incash's own formula,
        // agnus_blitter.v:943, is the same SUD/AUL/sign_del condition as
        // lineinc minus its ash-value gate).
        Harness h;
        h.write_reg(BLTADAT, 0x8000);
        h.write_reg(BLTBDAT, 0x0000);
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTCPTL, 0x0400);
        h.write_reg(BLTDPTL, 0x0400);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_A_PASSTHROUGH));  // ash starts at 0
        h.write_reg(BLTCON1, static_cast<uint16_t>((1u << 4) | LINE_BIT));
        h.write_reg(BLTSIZE, bltsize_word(2, 4));
        h.run_until_writes(4, 200);

        const std::vector<uint16_t> expected = {0x8000, 0x4000, 0x2000, 0x1000};
        checks.expect(h.data_writes == expected,
            "LINE-BLTADAT-8000-AND-A-WALKS: preloading BLTADAT with a single true bit "
            "($8000) and running 4 pixels of an octant whose X step is unconditional "
            "produces data_out = 0x8000, 0x4000, 0x2000, 0x1000 in order -- the one bit "
            "walks one position per pixel, automatically, with no software re-write of "
            "BLTADAT between pixels ('line-mode source A contains one true bit "
            "represented by $8000', 6-appa-control.c82, p.268; 'the A data register is "
            "preloaded with $8000 for line mode'/'in line mode, BLTADAT is preloaded "
            "with $8000', 6-line-mode.c49/ch6-line.c23, p.191; 'BLTADAT = $8000', "
            "ch6-linesum.c0, p.192; 'line mode uses BLTADAT as an index register'/"
            "'requires BLTADAT to be preloaded with $8000'/'in line draw BLTADAT is "
            "used as an index register'/'must be preloaded with 8000', "
            "6-appa-registers.c83/c84, ch6-appa-bltxdat.c7/c7b, p.269; 'the A bit is "
            "automatically moved across the word by the hardware', ch6-appa-lf.c7, "
            "p.268; 'source A supplies the moving line bit'/'in line mode the A "
            "channel is the bit to set in the line', 6-line-mode.c69/ch6-line.c37, "
            "p.191; 'in line mode the C DMA channel represents the original source', "
            "ch6-line.c36, p.191, corroborated by LF_A_PASSTHROUGH deliberately "
            "isolating A from C here)");
    }

    // ---------------- LINE-MINTERM-4A-A-BIT-INVERTS-ONE (p.268) ----------------
    {
        // ch6-appa-lf.c3/c5 describe the REAL line-mode minterm function's
        // effect on the moving A bit -- primarily PKG-BLT-MINTERM's
        // territory (packages.md item 1 cites the same $4A/AB+A'C/ABC'+A'C
        // functions), but classify.package places these two ids in THIS
        // package, so they are addressed here against the observable
        // consequence at this boundary, using one of the three cited
        // functions ($4A) rather than the isolating A-passthrough above.
        // agnus_blitter_minterm.v computes per-bit, MSB-first as (A,B,C):
        // with B held at 0, LF=$4A (01001010) gives, per bit, out=C when
        // A=0 (bit index 0 or 1: LF bits 0,1 = 0,1) and out=0 when A=1
        // (bit index 4 or 5: LF bits 4,5 = 0,0) -- every C bit passes
        // through except the ONE position the A bit occupies, forced to 0.
        Harness h;
        h.c_words = {0xFFFF};
        h.write_reg(BLTADAT, 0x8000);
        h.write_reg(BLTBDAT, 0x0000);
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTCPTL, 0x0400);
        h.write_reg(BLTDPTL, 0x0400);
        h.write_reg(BLTCON0, static_cast<uint16_t>((3u << 12) | USEC | USED | LF_LINE_4A));  // ash=3
        h.write_reg(BLTCON1, static_cast<uint16_t>((1u << 4) | LINE_BIT));                    // octant 7
        h.write_reg(BLTSIZE, bltsize_word(2, 1));
        h.run_until_writes(1, 60);

        const uint16_t expected = static_cast<uint16_t>(0xFFFF & ~(1u << 12));  // 0xEFFF
        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == expected,
            "LINE-MINTERM-4A-A-BIT-INVERTS-ONE: with LF=$4A (one of the three "
            "line-mode functions packages.md item 1 cites, p.191-192/268), C=0xFFFF "
            "and the moving A bit at position 12 (ash=3, $8000>>3), data_out is 0xEFFF "
            "-- every C bit passes through unchanged except the single position the A "
            "bit occupies, which is forced to 0 regardless of C ('because A contains a "
            "single bit true (8000), most bits pass the C field unchanged', "
            "ch6-appa-lf.c3, p.268; 'one bit will invert the C field and combine it "
            "with texture', ch6-appa-lf.c5, p.268 -- the B=0 case measured here shows "
            "the 'invert' half; the texture-combining half is B's own contribution, "
            "PKG-BLT-MINTERM's territory, not re-derived here)");
    }

    // ---------------- LINE-B-TEXTURE-BSH-START-BIT (p.169,184,191,192,268,269) ----------------
    {
        // BLTBDAT bits 3,2,1,0 = 1,0,1,0. Initial BSH=3 (BLTCON1[15:12]),
        // decrementing one per pixel (decbsh, agnus_blitter.v:945,
        // unconditional on octant/sign): pixel k reads bit (3-k) of
        // BLTBDAT. Expected sequence: bit3=1, bit2=0, bit1=1, bit0=0.
        Harness h;
        h.c_words = std::deque<uint16_t>(4, 0);
        h.write_reg(BLTADAT, 0x0000);
        h.write_reg(BLTBDAT, 0x000A);  // 0b1010
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTCPTL, 0x0400);
        h.write_reg(BLTDPTL, 0x0400);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_B_PASSTHROUGH));
        h.write_reg(BLTCON1, static_cast<uint16_t>((3u << 12) | LINE_BIT));  // BSH=3, octant 6
        h.write_reg(BLTSIZE, bltsize_word(2, 4));
        h.run_until_writes(4, 200);

        const std::vector<uint16_t> expected = {0xFFFF, 0x0000, 0xFFFF, 0x0000};
        checks.expect(h.data_writes == expected,
            "LINE-B-TEXTURE-BSH-START-BIT: with BLTBDAT=0b1010 and BSH initially 3, "
            "four pixels read data_out = 0xFFFF, 0x0000, 0xFFFF, 0x0000 -- exactly bits "
            "3,2,1,0 of BLTBDAT, one per pixel, most significant first, replicated "
            "across the whole word ('BLTBDAT holds the line texture pattern'/'the B "
            "data register is initialized with the line texture pattern', "
            "6-line-mode.c53/ch6-line.c26, p.191; 'the register summary says BLTBDAT "
            "holds the texture pattern', 6-line-mode.c73, p.192; 'in the line-mode "
            "logic function, source B supplies the texture'/'in line mode the B "
            "channel is the pattern to draw', 6-line-mode.c70/ch6-line.c38, p.191; "
            "'line mode uses BLTBDAT for texture'/'used for texture', "
            "6-appa-registers.c85, ch6-appa-bltxdat.c7c, p.269; 'line mode uses source "
            "B for texture', 6-appa-control.c117, p.268) BEGINNING at bit 3, exactly "
            "the value written to BSH ('the B shift specifies the texture start "
            "bit'/'the B shift value is set to the bit number at which to start the "
            "line texture', 6-line-mode.c55/ch6-line.c28, p.191; BSH's own field "
            "identity, 'BLTCON1 bits 15-12 select the line-texture start bit'/'bits "
            "15-12 = the start bit for the line texture', 6-line-mode.c95/"
            "ch6-linesum.c20, p.192), decrementing automatically one bit per pixel with "
            "no software re-write of BSH or BLTBDAT -- and, had BSH started at 0, the "
            "FIRST pixel would have read bit 0 directly ('a B shift of zero starts the "
            "texture at its least-significant bit'/'a B shift value of zero means the "
            "last significant bit', 6-line-mode.c56/ch6-line.c29, p.191), which is what "
            "the field's own zero point means. This is also the mechanism 'a special "
            "line draw mode... for simple area fill' (6-line-mode.c6/ch6-line.c5, "
            "p.189) and 'an area-fill outline must be drawn with untextured lines'/"
            "'the area to fill must be defined first by drawing untextured lines'/"
            "'those defining lines must have only one bit set per horizontal row'/'a "
            "special line draw mode is available for drawing those defining lines' "
            "(6-area-fill.c2, ch6-fill.c2/c3/c4, p.184) actually rest on -- an "
            "untextured line is BLTBDAT=$FFFF (texture bit always 1, no gaps), and "
            "'one bit set per horizontal row' is ONEDOT (see "
            "LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES below); the application-level "
            "instruction to use them THAT way for area-fill is not itself a hardware "
            "signal");
    }

    // ---------------- LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES (p.189,191,192) ----------------
    {
        // Octant 7 (SUD=1,SUL=0,AUL=0): X-major, unconditional INC. Self-
        // sustaining sign_del=1 for EVERY pixel, not only the first:
        // BLTAPTL preloaded with bit 15 set (0x8000) and BLTAMOD/BLTBMOD
        // left at their reset default (0), so BLT_L1's accumulator update
        // (agnus_blitter.v:795-811, always adds a modulo of 0) never
        // changes the accumulator -- sign_out=newptr[15] stays 1 forever,
        // so every pixel's own sign_del is 1. With sign_del always 1 the
        // minor (Y) axis never steps: every pixel stays on the same row,
        // exactly the scenario ONEDOT exists for.
        Harness h_onedot;
        h_onedot.c_words = std::deque<uint16_t>(5, 0xFFFF);
        h_onedot.write_reg(BLTADAT, 0x0000);
        h_onedot.write_reg(BLTBDAT, 0x0000);
        h_onedot.write_reg(BLTAFWM, 0xFFFF);
        h_onedot.write_reg(BLTALWM, 0xFFFF);
        h_onedot.write_reg(BLTAPTL, 0x8000);
        h_onedot.write_reg(BLTCPTL, 0x0400);
        h_onedot.write_reg(BLTDPTL, 0x0400);
        h_onedot.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        // SIGNFLAG=1, ONEDOT=1 (bit1), SUD=1/SUL=0/AUL=0 (octant7), LINE=1
        h_onedot.write_reg(BLTCON1, static_cast<uint16_t>((1u << 6) | (1u << 1) | (1u << 4) | LINE_BIT));
        h_onedot.write_reg(BLTSIZE, bltsize_word(2, 5));
        h_onedot.run_until_done(200);

        Harness h_normal;
        h_normal.c_words = std::deque<uint16_t>(5, 0xFFFF);
        h_normal.write_reg(BLTADAT, 0x0000);
        h_normal.write_reg(BLTBDAT, 0x0000);
        h_normal.write_reg(BLTAFWM, 0xFFFF);
        h_normal.write_reg(BLTALWM, 0xFFFF);
        h_normal.write_reg(BLTAPTL, 0x8000);
        h_normal.write_reg(BLTCPTL, 0x0400);
        h_normal.write_reg(BLTDPTL, 0x0400);
        h_normal.write_reg(BLTCON0, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH));
        h_normal.write_reg(BLTCON1, static_cast<uint16_t>((1u << 6) | (0u << 1) | (1u << 4) | LINE_BIT));  // ONEDOT=0
        h_normal.write_reg(BLTSIZE, bltsize_word(2, 5));
        h_normal.run_until_done(200);

        checks.expect(h_onedot.data_writes.size() == 1 && h_normal.data_writes.size() == 5,
            "LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES: across 5 pixels that all stay on "
            "the same row (sign_del held at 1 throughout, so the minor/Y axis never "
            "steps), ONEDOT=1 (BLTCON1 bit 1) produces exactly 1 destination write "
            "(only first_pixel) while ONEDOT=0 produces all 5 -- 'setting ONEDOT "
            "restricts line output to one bit per horizontal row'/'clearing ONEDOT "
            "permits more than one bit per horizontal row', 6-line-mode.c66/c67, "
            "p.191; 'if only a single bit per horizontal row is desired the ONEDOT "
            "bit of BLTCON1 is set, otherwise it is cleared', ch6-line.c35, p.191; "
            "'for one pixel per row BLTCON1 bit ONEDOT = 1'/'otherwise BLTCON1 bit "
            "ONEDOT = 0', ch6-linesum.c23/c24, p.192; 'line mode has a one-dot form "
            "suitable for preparing simple area fills'/'lines can be drawn in a "
            "special way for simple area fill', 6-line-mode.c6/ch6-line.c5, p.189. "
            "This is also the direct demonstration that bit 1 no longer means "
            "DESCENDING once LINE=1 is set ('line draw mode changes the meaning of "
            "some other bits in BLTCON0 and BLTCON1', 6-line-mode.c3/ch6-line.c3, "
            "p.189; 'many of the blitter registers serve other purposes in "
            "line-drawing mode', ch6-line.c6, p.189): with LINE=0 the same bit "
            "reverses the pointer's step direction (PKG-BLT-MASKS's DESC-* "
            "assertions), but here it silently drops writes instead -- a completely "
            "different effect, not a variant of descending mode");
    }

    // ---------------- LINE-SIGNFLAG-SETS-INITIAL-SIGN (p.191, p.192) ----------------
    {
        // Octant 1 (SUD=0,SUL=0,AUL=1): Y-major, X minor-INC. ash=15
        // matches the minor-INC direction; delta should be 0 when
        // SIGNFLAG=1 (major-only) and +1 when SIGNFLAG=0 (major+minor).
        const int32_t delta_sign1 = measure_pixel1_delta(0, 0, 1, /*initial_sign=*/1, /*ash_probe=*/15, /*cmod_dmod=*/0);
        const int32_t delta_sign0 = measure_pixel1_delta(0, 0, 1, /*initial_sign=*/0, /*ash_probe=*/15, /*cmod_dmod=*/0);
        checks.expect(delta_sign1 == 0 && delta_sign0 == 1,
            "LINE-SIGNFLAG-SETS-INITIAL-SIGN: on octant 1 (Y-major, X minor-INC), "
            "writing BLTCON1 bit 6 (SIGNFLAG) to 1 before BLTSIZE starts the blit "
            "gives pixel 1 a pointer delta of 0 (X suppressed, major-only step) while "
            "writing it to 0 gives pixel 1 a delta of +1 (X fires too) -- BLTCON1 bit "
            "6 IS the line-error accumulator's initial sign, sampled by pixel 1's own "
            "step ('SIGNFLAG is BLTCON1's line-error sign bit', 6-line-mode.c44, "
            "p.191; 'otherwise BLTCON1 bit SIGNFLAG = 0', ch6-linesum.c22, p.192), "
            "measured directly against agnus_blitter.v's `sign<=data_in[6]` register "
            "write (agnus_blitter.v:929-935) and its effect on pixel 1's sign_del "
            "(agnus_blitter.v:937-941)");
    }

    // ---------------- LINE-SIGN-SELECTS-MODULO-BANK (p.191, p.192, p.270) ----------------
    {
        const int32_t delta_a = measure_accumulator_delta(/*initial_sign=*/0, /*amod=*/0x0040, /*bmod=*/0x0000);
        const int32_t delta_b = measure_accumulator_delta(/*initial_sign=*/1, /*amod=*/0x0000, /*bmod=*/0x0040);
        checks.expect(delta_a == 32 && delta_b == 32,
            "LINE-SIGN-SELECTS-MODULO-BANK: with SIGNFLAG=0, only a nonzero BLTAMOD "
            "(0x0040, against a $0000 BLTBMOD) moves the accumulator by +32; with "
            "SIGNFLAG=1 and the modulo values swapped, only BLTBMOD moves it by the "
            "same +32 -- BLT_L1 selects BLTAMOD's bank when sign=0 and BLTBMOD's bank "
            "when sign=1 (agnus_blitter.v:799, `modsel = sign ? CHB : CHA`), which is "
            "'the sign flag driving the modulo choice' the build brief names, and the "
            "RTL fact underneath the register-summary's role assignment ('the A "
            "modulo register is set to 4 * (dy - dx)'/'the B modulo register is set to "
            "4 * dy', ch6-line.c21/c22, p.191; 'in line mode, BLTAMOD is 4*(dy-dx)'/"
            "'BLTBMOD is 4*dy', 6-line-mode.c47/c48, p.191; register-summary echoes, "
            "'BLTAMOD to 4*(dy-dx)'/'BLTBMOD to 4*dy'/'BLTAMOD = 4 * (dy - dx)'/"
            "'BLTBMOD = 4 * dy', 6-line-mode.c77/c78, ch6-linesum.c4/c5, p.192; 'line "
            "mode uses BLTAMOD as slope storage'/'BLTBMOD as slope storage'/'used as "
            "slope storage registers', 6-appa-registers.c96/c97, ch6-appa-bltxmod.c9, "
            "p.270) -- the literal dy/dx arithmetic is the programmer's, not this "
            "design's (no dy/dx port exists), so only the bank-selection half is "
            "asserted here; see the file header's NOT ASSERTED section for the "
            "formula half");
    }

    // ---------------- LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE (p.191, p.192, p.270) ----------------
    {
        // (a) BLTCPT and BLTDPT are separately-addressed registers
        // ($048/$04A vs $054/$056) and BLTDPT specifically -- not BLTCPT --
        // governs the pointer pixel 1's own store reads. Pixel 1's own
        // BLT_L4 has enaptr=1 UNCONDITIONALLY (agnus_blitter.v:854) and
        // ptrsel=CHD (first_line_pixel is still 1 at that point,
        // agnus_blitter.v:852) -- both guaranteed, so sampling address_out
        // AT the `we` tick itself (not offset) reliably reads BLTDPT's own
        // bank.
        Harness h_identity;
        h_identity.write_reg(BLTADAT, 0x0000);
        h_identity.write_reg(BLTAFWM, 0xFFFF);
        h_identity.write_reg(BLTALWM, 0xFFFF);
        h_identity.write_reg(BLTCPTL, 0x0300);  // deliberately different from D
        h_identity.write_reg(BLTDPTL, 0x0500);
        h_identity.write_reg(BLTCON0, static_cast<uint16_t>((8u << 12) | USEC | USED | LF_C_PASSTHROUGH));
        h_identity.write_reg(BLTCON1, LINE_BIT);  // octant 6
        h_identity.write_reg(BLTSIZE, bltsize_word(2, 1));
        h_identity.run_until_writes(1, 60);
        const bool d_used_for_pixel1 =
            !h_identity.we_ticks.empty() &&
            h_identity.address_trace[h_identity.we_ticks[0]] == 0x0280;  // 0x0500 stored as data_in[15:1]

        // (b) BLTDPT's own step magnitude follows BLTDMOD specifically, not
        // BLTCMOD -- octant 6 (Y-major, unconditional ADD), ash=8 disables
        // the X component so only the modulo (Y) contribution is visible.
        // Measured across PIXELS 2 AND 3, not pixel 1: BLT_L3 (where D's
        // own step is applied, modsel=CHD) has enaptr gated by
        // first_line_pixel and is only a reliable read point once
        // first_line_pixel has cleared, i.e. from pixel 2 onward (same
        // reliability rule as measure_pixel1_delta's own comment).
        Harness h_mod;
        h_mod.write_reg(BLTADAT, 0x0000);
        h_mod.write_reg(BLTAFWM, 0xFFFF);
        h_mod.write_reg(BLTALWM, 0xFFFF);
        h_mod.write_reg(BLTCPTL, 0x0400);
        h_mod.write_reg(BLTDPTL, 0x0400);
        h_mod.write_reg(BLTCMOD, 0x0040);  // would give a delta of 32 if wrongly used for D
        h_mod.write_reg(BLTDMOD, 0x0080);  // gives a delta of 64 when correctly used for D
        h_mod.write_reg(BLTCON0, static_cast<uint16_t>((8u << 12) | USEC | USED | LF_C_PASSTHROUGH));
        h_mod.write_reg(BLTCON1, static_cast<uint16_t>((1u << 6) | LINE_BIT));  // octant 6
        h_mod.write_reg(BLTSIZE, bltsize_word(2, 3));
        h_mod.run_until_writes(3, 200);
        const int32_t d_delta = (h_mod.we_ticks.size() >= 3 && h_mod.we_ticks[1] >= 1 && h_mod.we_ticks[2] >= 1)
            ? static_cast<int32_t>(h_mod.address_trace[h_mod.we_ticks[2] - 1]) -
              static_cast<int32_t>(h_mod.address_trace[h_mod.we_ticks[1] - 1])
            : INT32_MIN;

        checks.expect(d_used_for_pixel1 && d_delta == 64,
            "LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE: (1) preloading BLTCPTL=0x0300 and "
            "BLTDPTL=0x0500 to DIFFERENT values shows pixel 1's own store reads "
            "BLTDPTL's value specifically, not BLTCPTL's -- BLTCPT and BLTDPT are "
            "genuinely separate, independently-addressed registers ($048/$04A vs "
            "$054/$056), each preloaded to the SAME word in real use ('the C and D "
            "pointer registers are initialized to the word containing the first pixel "
            "of the line', ch6-line.c30, p.191; 'BLTCPT is preloaded with the line's "
            "starting address'/'BLTDPT is preloaded with the line's starting "
            "address', 6-appa-registers.c119/c120, ch6-appa-bltxpt.c11, p.270; "
            "'BLTCPT starts at the word containing the line's first pixel'/'BLTDPT "
            "starts at...', 6-line-mode.c57/c58, p.191; 'the line-mode register "
            "summary points BLTCPT/BLTDPT at the word containing the line's first "
            "pixel', 6-line-mode.c83/c84, ch6-linesum.c10/c11, p.192; 'line mode uses "
            "BLTCPT as a line pointer'/'uses both halves of BLTDPT as a line "
            "pointer', 6-appa-registers.c117/c118, p.270); (2) with BLTCMOD=0x0040 "
            "and BLTDMOD=0x0080 programmed to DIFFERENT values, D's own step follows "
            "BLTDMOD (+64 word units) not BLTCMOD -- proving BLTCMOD and BLTDMOD are "
            "each their OWN modulo register, exactly why the manual requires "
            "programming BOTH to the SAME value or C and D drift apart mid-line "
            "('BLTCMOD equals the bitplane width in bytes'/'BLTDMOD equals the "
            "bitplane width in bytes', 6-line-mode.c59/c60, p.191; 'the C and D "
            "modulo registers are set to the width of the bitplane in bytes', "
            "ch6-line.c31, p.191; 'line mode loads the same width into BLTCMOD and "
            "BLTDMOD'/'BLTCMOD uses image width in bytes'/'BLTDMOD uses image width "
            "in bytes', 6-appa-registers.c100/c101/c102, p.270; 'BLTCMOD/BLTDMOD "
            "must both be preloaded with the width, in bytes, of the image', "
            "ch6-appa-bltxmod.c12, p.270; register-summary echoes, 'BLTCMOD to the "
            "bitplane width in bytes'/'BLTDMOD to the bitplane width in bytes'/"
            "'BLTCMOD = width of the bitplane in bytes'/'BLTDMOD = width of the "
            "bitplane in bytes', 6-line-mode.c79/c80, ch6-linesum.c6/c7, p.192). "
            "'BLTBPT is unused in line mode'/'BLTBPT is unused' (6-line-mode.c82/"
            "ch6-linesum.c9, p.192) is corroborated by direct RTL citation rather "
            "than simulated: no BLT_L1-L4 case branch ever assigns ptrsel or chsel "
            "the value CHB (agnus_blitter.v:795-869), and there is no register "
            "readback path on this design to confirm 'unused' by measurement the way "
            "every other claim in this file is measured");
    }

    // ---------------- OCTANT-CODE-EQUALS-BITS (p.189, p.190, p.268) ----------------
    {
        // Not an RTL measurement -- confirms Figure 6-8's own drawn code
        // number for each octant equals the 3-bit {SUD,SUL,AUL} value Table
        // 6-3/Appendix A assign that SAME octant, tying the figure claims
        // to the bit-table claims formally (all three describe the
        // identical 8-row table).
        struct FigCode {
            int octant;
            int code;
        };
        constexpr FigCode FIG68[8] = {
            {0, 6}, {1, 1}, {2, 3}, {3, 7}, {4, 5}, {5, 2}, {6, 0}, {7, 4},
        };
        int matches = 0;
        for (const auto& f : FIG68) {
            const Octant& o = OCTANTS[f.octant];
            const int bits = (o.sud << 2) | (o.sul << 1) | o.aul;
            if (bits == f.code) ++matches;
        }
        checks.expect(matches == 8,
            "OCTANT-CODE-EQUALS-BITS [arithmetic, not an RTL measurement]: for all 8 "
            "octants, Figure 6-8's own bare code number equals {SUD,SUL,AUL} read as "
            "a 3-bit value -- e.g. octant 0's code 6 = 0b110 = SUD=1,SUL=1,AUL=0, "
            "octant 6's code 0 = 0b000 = SUD=0,SUL=0,AUL=0 -- tying the figure's "
            "sector/code claims ('the direction of the drawn line vector can lie in "
            "any of eight octants'/'the standard Amiga convention is used, with x "
            "increasing towards the right and y increasing down', ch6-line.c8/c9, "
            "p.189; the eight sector+code descriptions, 6-line-mode.c7-c14, "
            "ch6-line.c10/c11, ch6-line.f8.oct0-oct7, p.190) to the bit-value claims "
            "(ch6-t63.oct0-oct7.bit2/bit3/bit4, p.190; SUD/SUL/AUL's own names and "
            "per-octant values, 'SUD selects sometimes-up-or-down octant "
            "behavior'/'SUL selects sometimes-up-or-left'/'AUL selects "
            "always-up-or-left', 6-appa-control.c90/c91/c92, and "
            "6-appa-control.c93-c116, 'the three bits SUD, SUL and AUL select the "
            "octant for line drawing', ch6-appa-oct.select, p.268) as ONE table "
            "stated three times, not three separate facts");
    }

    // ---------------- OCTANT-MAJOR-AXIS-DIRECTION (p.190, p.268) ----------------
    {
        int confirmed = 0;
        for (const auto& o : OCTANTS) {
            for (int s = 0; s <= 1; ++s) {
                const uint8_t initial_sign = static_cast<uint8_t>(s);
                int32_t delta;
                int32_t expected;
                if (o.x_major) {
                    const uint8_t probe = o.major_positive ? 15 : 0;
                    delta = measure_pixel1_delta(o.sud, o.sul, o.aul, initial_sign, probe, 0x0000);
                    expected = o.major_positive ? 1 : -1;
                } else {
                    delta = measure_pixel1_delta(o.sud, o.sul, o.aul, initial_sign, 8, 0x0040);
                    expected = o.major_positive ? 32 : -32;
                }
                if (delta == expected) ++confirmed;
            }
        }
        checks.expect(confirmed == 16,
            "OCTANT-MAJOR-AXIS-DIRECTION: for all 8 octants of Table 6-3/Appendix A "
            "(6-line-mode.c15-c38, 6-appa-control.c93-c116, p.190/268), at BOTH "
            "SIGNFLAG=0 and SIGNFLAG=1 (16 checks total), the major axis steps "
            "exactly one unit in the documented direction regardless of sign -- 'the "
            "direction of the drawn line vector can lie in any of eight octants' "
            "(ch6-line.c8, p.189), using 'the standard Amiga convention... with x "
            "increasing towards the right and y increasing down' (ch6-line.c9, "
            "p.189) as the direction convention, measured against "
            "agnus_blitter.v:609-612's own lineinc/linedec/lineadd/linesub formulas "
            "for every {SUD,SUL,AUL} combination the table assigns");
    }

    // ---------------- OCTANT-MINOR-AXIS-SUPPRESSED-AT-SIGN1 [DEFECT-2 FIX] (p.190, p.268) ----------------
    {
        // docs/blitter-prior-art.md's defect 2: the superseded
        // agnus_blitter_line_octant test asserted "no X step" for its four
        // Y-major octants using ash=8, which makes lineinc's own
        // ash==4'b1111 gate AND linedec's ash==4'b0000 gate BOTH fail
        // regardless of what the rest of each formula computes -- true by
        // construction. Proved by mutation there: removing the !sign_del
        // guard from lineinc left that test green; only driving ash=15
        // against lineinc (not ash=8) turned it red. packages.md's fix:
        // carry BOTH probes, ash=15 against lineinc and ash=0 against
        // linedec, or the affected octants stay vacuous.
        //
        // WHY ash=15 falsifies lineinc: at ash=15, lineinc's ash-gate is
        // SATISFIED, so the assertion depends entirely on the rest of the
        // formula (SUD/AUL/SUL/sign_del) being correct -- if a defect made
        // that formula ignore !sign_del (exactly the mutation prior-art
        // applied), the pointer would incorrectly step +1 and this check
        // would catch it, where ash=8 could not.
        // WHY ash=0 falsifies linedec: symmetric reasoning for the DEC
        // formula and its own ash==0000 gate, which the prior art's probe
        // never exercised at all -- a bug living ONLY in linedec's formula
        // is invisible to an ash=15 probe.
        //
        // In THIS package's own table (re-derived from Table 6-3/Appendix
        // A directly, not the superseded test's invented numbering -- see
        // file header) the octants whose minor axis is X are 1, 2, 5 and 6.
        // For the other four (0, 3, 4, 7, X-major, Y-minor) the analogous
        // check is on lineadd/linesub, which are NOT ash-gated at all, so
        // there is no ash-vacuity risk there -- checked once each with a
        // nonzero modulo (never zero) so a wrongly-firing add/sub would
        // still be caught.
        int total_checks = 0, confirmed = 0;
        for (const auto& o : OCTANTS) {
            if (o.x_major) {
                ++total_checks;
                const int32_t delta = measure_pixel1_delta(o.sud, o.sul, o.aul, /*initial_sign=*/1, 8, 0x0040);
                if (delta == 0) ++confirmed;
            } else {
                total_checks += 2;
                const int32_t delta_inc_probe = measure_pixel1_delta(o.sud, o.sul, o.aul, /*initial_sign=*/1, 15, 0x0000);
                const int32_t delta_dec_probe = measure_pixel1_delta(o.sud, o.sul, o.aul, /*initial_sign=*/1, 0, 0x0000);
                if (delta_inc_probe == 0) ++confirmed;
                if (delta_dec_probe == 0) ++confirmed;
            }
        }
        checks.expect(confirmed == 12 && total_checks == 12,
            "OCTANT-MINOR-AXIS-SUPPRESSED-AT-SIGN1 [DEFECT-2 FIX]: at SIGNFLAG=1 "
            "(major-only stepping), all 8 octants' minor axis genuinely does not "
            "step -- for octants 1, 2, 5, 6 (X minor), BOTH the ash=15/lineinc probe "
            "AND the ash=0/linedec probe read a zero delta (8 checks); for octants 0, "
            "3, 4, 7 (Y minor, not ash-gated), a single nonzero-modulo probe reads a "
            "zero delta (4 checks) -- 12 checks total, all confirmed. This directly "
            "replaces the superseded agnus_blitter_line_octant test's vacuous "
            "ash=8-only assertion (docs/blitter-prior-art.md, defect 2) with the "
            "falsifiable pair packages.md requires");
    }

    // ---------------- OCTANT-MINOR-AXIS-FIRES-AT-SIGN0 (p.190, p.268) ----------------
    {
        int confirmed = 0;
        for (const auto& o : OCTANTS) {
            int32_t delta;
            int32_t expected;
            if (o.x_major) {
                delta = measure_pixel1_delta(o.sud, o.sul, o.aul, /*initial_sign=*/0, 8, 0x0040);
                expected = o.minor_positive ? 32 : -32;
            } else {
                const uint8_t probe = o.minor_positive ? 15 : 0;
                delta = measure_pixel1_delta(o.sud, o.sul, o.aul, /*initial_sign=*/0, probe, 0x0000);
                expected = o.minor_positive ? 1 : -1;
            }
            if (delta == expected) ++confirmed;
        }
        checks.expect(confirmed == 8,
            "OCTANT-MINOR-AXIS-FIRES-AT-SIGN0: at SIGNFLAG=0 (major+minor stepping), "
            "all 8 octants' minor axis steps exactly one unit in the documented "
            "direction, using the SAME probe methodology as the defect-2 fix above "
            "(the matching ash value for X-minor octants, a nonzero modulo for "
            "Y-minor octants) -- positive confirmation that both lineinc/linedec's "
            "and lineadd/linesub's !sign_del-gated terms fire correctly, completing "
            "the octant table's direction behaviour across both sign states "
            "(6-line-mode.c15-c38, 6-appa-control.c93-c116, p.190/268)");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
