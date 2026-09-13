# Specification: ch7-beam-position-word-layout

Fixture: `agnus_beamcounter`. Subject: Table 7-5 (printed p.216) and Appendix A
p.285 — the word layout and read/write direction of VPOSR, VPOSW, VHPOSR and
VHPOSW, plus the Appendix B/C material (p.285/298/299) that documents the same
four registers.

Written from `ch7-beam-position-word-layout-spec-stage.jsonl` alone (93
claims). No RTL, no port list, no `STEP3-NOTES-ch7-beam-position-word-layout.md`
and no `ch7-beam-position-word-layout.jsonl` have been opened to produce this
document.

## How the 93 claims were organised

Three things make 93 claims collapse into a much smaller assertion list, and
all three are visible in the claims file itself, not inferred:

1. **Twins.** `sol` and `opus` independently read the same table rows and
   produced separate claim records for the same proposition (e.g.
   `sol:7-beam.c7` and `opus:7-bp.c7` both state "VPOSR bits 14-1 are unused
   (OCS)"). Each assertion below cites every uid that supports it.
2. **Splits.** Several claim records bundle more than one printed fact in one
   quotation (an appendix-A table row bundles address + direction + chip +
   function in one line). The classify stage already split these; I follow
   the split it recorded rather than re-deriving one.
3. **Existing coverage.** `hrm/ch3-register-map-agnus-beamcounter` and
   `hrm/ch3-beam-counters-and-blanking` already assert most of the per-bit and
   per-address content of VPOSR/VHPOSR/VPOSW/VHPOSW on this same fixture, cited
   from Appendix B p.289 and Appendix C p.299. I confirmed the exact label
   strings by reading those two packages' existing `test_agnus_beamcounter.cpp`
   (not this package's material, not RTL): `VPOSR-ADDRESS-004`,
   `VHPOSR-ADDRESS-006`, `VPOSW-ADDRESS-02A`, `VHPOSW-ADDRESS-02C`,
   `VPOSR-BIT15-IS-LOF-LONG-FRAME`, `VPOSR-IDENTIFICATION-FIELD-BITS-14-7`,
   `VPOSR-BITS-6-3-UNUSED`, `VPOSR-BITS-2-0-VERTICAL-POSITION`,
   `LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION`. Where a claim here states
   exactly what one of those labels already grades, on the same fixture, I do
   not duplicate it — I cite the existing label and explain why nothing new is
   needed. Where a claim's proposition is not what the existing label grades
   (e.g. "these are the only four addresses", not "$004 answers to VPOSR"), it
   gets a new assertion.

Every one of the 93 uids is accounted for below, either as support for a new
assertion or in the "claims not turned into a new assertion" table with a
reason.

## Chipset scope

Chapter 7's body (p.216, p.285) is the OCS/original account; ECS changes are
in Appendix C (p.298-299), which chapter 7 points to on p.224. Two bit bands
are genuinely disputed across chipsets and are asserted only under the
chipset they were stated for:

- VPOSR bits 14-1 (OCS: "unused", p.216/p.285) vs bits 14-8/7/2-1 (ECS:
  I6-I0/LOL/v10-v9, p.299) — the ECS side is already covered by
  `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` / `VPOSR-BITS-6-3-UNUSED` /
  `VPOSR-BITS-2-0-VERTICAL-POSITION`. Only the OCS side is new here (A4 below),
  and it must be measured with the fixture's chipset-select configured for
  OCS — whatever that control turns out to be is a step-3 question.
- Bit 15 (LOF) and bit 0 (V8) are undisputed on both chipsets and are already
  covered unconditionally by `VPOSR-BIT15-IS-LOF-LONG-FRAME` and
  `VPOSR-BITS-2-0-VERTICAL-POSITION`.

Every assertion in this package that is new (A1-A14, N1 below) is otherwise
unconditional — nothing in the 93 claims disputes VHPOSR/VPOSW/VHPOSW content
or direction across chipsets, and the "four addresses" and "most-significant
word" propositions are stated in the chapter-7 body without an ECS carve-out.

## New assertions

### A1 — BEAM-COUNTER-DELIVERS-KNOWN-VALUE
- Pages: 216. Uids: `sol:7-beam.c1`, `opus:7-bp.c1`.
- Quote (p.216): "The address for accessing the beam counter is provided so
  that you can determine the value of the video beam counter and perform
  certain operations based on the beam position."
- Proposition: reading the beam-counter address returns the current beam
  counter value.
- This doubles as the mandatory fixture health check: before any
  content-specific assertion is graded, feed distinct, known values into the
  counter and confirm each is read back, in order, at the cycle it was
  produced — not that some fixed/zero word came back regardless of stimulus.
  Concrete mechanism (how a value is forced into the counter for the test) is
  a step-3 decision once the port list is visible.
- Expected: PASS (harness precondition; the package cannot report anything
  about content once this fails).

### A2 — FOUR-ADDRESSES-EXACT
- Page: 216. Uids: `sol:7-beam.c4`, `opus:7-bp.c4`.
- Quote: "There are four addresses that access the beam position counter."
- Proposition: exactly four register addresses ($004 VPOSR, $006 VHPOSR, $02A
  VPOSW, $02C VHPOSW) answer as the beam-position counter; no other address in
  the module's decoded range aliases to it. `ch3-register-map-agnus-beamcounter`
  asserts each of the four addresses individually but never the "exactly these
  four, and no more" proposition — that residual is what this assertion is for.
- Expected: not stated by the claims file; to be measured in step 3.

### A3 — VPOSR-IS-READ-ONLY
- Pages: 216, 285, 298. Uids: `sol:7-beam.c5` (direction half),
  `opus:7-bp.c5` (direction half), `sol:7-appA-vpos.c2`,
  `sol:7-appC-map-vposr.c3`.
- Quotes: p.216 "VPOSR Read-only ..."; p.285 "VPOSR 004 R A(E) Read vertical
  most significant bit (and frame flop)"; p.298 "004 VPOSR chg RA Read
  vertical most sig. bits (and frame flop)" (column-scrambled corpus row,
  restored per the crossread against the page image).
- Proposition: VPOSR is read-only, on both OCS and ECS (three independent
  pages agree; none scopes this to one chipset).
- Control case: same measurement basis as A6 (VPOSW-IS-WRITE-ONLY, expected
  RED) — one register genuinely read-only, one nominally write-only register
  that the RTL note below says is not enforced as such.
- Expected: PASS.

### A4 — VPOSR-OCS-BITS-14-1-UNUSED
- Pages: 216, 285. Uids: `sol:7-beam.c7`, `opus:7-bp.c7`,
  `sol:7-appA-vpos.c11`, `opus:7-appA-beam.c10` (bits 14-1 as a band);
  supporting register-level restatements `sol:7-beam.c5` (content half),
  `opus:7-bp.c5` (content half), `sol:7-appA-vpos.c5` (OCS content).
- Quotes: p.216 "Bits 14-1 Unused"; p.285 "USE LOF-- -- -- -- -- -- --,-- --
  -- -- -- -- -- V8" (dashes over bits 14-1).
- Proposition: on OCS, VPOSR bits 14 through 1 carry no assigned value (LOF at
  15 and V8 at 0 are the undisputed bits either side of this band, and are
  already covered by `VPOSR-BIT15-IS-LOF-LONG-FRAME` /
  `VPOSR-BITS-2-0-VERTICAL-POSITION` — not reasserted here).
- Precondition: this must be measured with the fixture's chipset-select input
  driven to its OCS/non-ECS setting. If the fixture exposes no such input (a
  step-3 finding), that precondition is trivially satisfied by construction
  and should be recorded as such rather than asserted.
- Expected: not stated by the claims file; to be measured in step 3.

### A5 — VPOSW-BIT-LAYOUT-MATCHES-VPOSR
- Pages: 216, 285. Uids: `sol:7-beam.c13` (layout half), `opus:7-bp.c15`
  (layout half), `sol:7-appA-vpos.c9`.
- Quotes: p.216 "VPOSW Write only Bits same as VPOSR above."; p.285 "VPOSW
  02A W A Write vertical most significant bit (and frame flop)".
- Proposition: writing VPOSW places bit 15 (long-frame) and bit 0 (vertical
  MSB) at the same positions VPOSR reads them at.
- Expected: PASS (the claims file's own note on `sol:7-beam.c13` says the
  bit-layout half of this row passes against the RTL it already looked at for
  classification; I have not looked at the RTL myself and this is not final
  until measured).

### A6 — VPOSW-IS-WRITE-ONLY
- Pages: 216, 285. Uids: `sol:7-beam.c13` (direction half), `opus:7-bp.c15`
  (direction half), `sol:7-appA-vpos.c7`, `opus:7-appA-beam.c8` (direction
  half only — its blank-cell-inference half is discarded, see below).
- Same quotes as A5.
- Proposition: VPOSW is write-only — a read of $02A should not return the
  beam-position content.
- Expected: **RED**. The claims file records this as a deliberate, already
  anticipated failure against the RTL it examined for classification (agnus_
  beamcounter.v:109, by its account): the write address is also wired into
  the read mux. I have not opened the RTL; this package must still assert the
  manual's proposition and report the true result, not adjust it to pass.

### A7 — VHPOSR-IS-READ-ONLY
- Pages: 216, 285. Uids: `sol:7-beam.c9` (direction half), `opus:7-bp.c10`
  (direction half), `sol:7-appA-vhpos.c2`.
- Quotes: p.216 "VHPOSR Read-only Read vertical and horizontal position of
  the counter that is producing the beam on the screen (also reads the light
  pen)."; p.285 "VHPOSR 006 R A Read vertical and horizontal position of beam
  or lightpen".
- Proposition: VHPOSR is read-only.
- Control case: same measurement basis as A12 (VHPOSW-IS-WRITE-ONLY, expected
  RED).
- Expected: PASS.

### A8 — VHPOSR-RETURNS-V-AND-H-POSITION
- Pages: 216, 285. Uids: `sol:7-beam.c9` (content half), `opus:7-bp.c10`
  (content half), `sol:7-appA-vhpos.c4` (content half).
- Same quotes as A7.
- Proposition: VHPOSR returns the vertical and horizontal position of the
  beam counter. The "(also reads the light pen)" / "or lightpen" half of each
  quote is **not** asserted — see the discard table below.
- Expected: not stated by the claims file; to be measured in step 3.

### A9 — VHPOSR-BITS-15-8-ARE-V7-V0
- Pages: 216, 285. Uids: `sol:7-beam.c10`, `opus:7-bp.c12`,
  `sol:7-appA-vhpos.c9`, `opus:7-appA-beam.c3`.
- Quotes: p.216 "Bits 15-8 Low bits of the vertical position, bits V7-V0";
  p.285 "USE V7 V6 V5 V4 V3 V2 V1 V0,H8 H7 H6 H5 H4 H3 H2 H1" (top half).
- Proposition: VHPOSR bits 15 through 8 carry vertical-position bits V7
  through V0. New relative to `ch3-register-map-agnus-beamcounter`, which
  asserts only that $006 decodes as VHPOSR, never the internal V7-V0/H8-H1
  split.
- Expected: not stated by the claims file; to be measured in step 3.

### A10 — VHPOSR-BITS-7-0-ARE-H8-H1
- Pages: 216, 285. Uids: `sol:7-beam.c11`, `opus:7-bp.c13`,
  `sol:7-appA-vhpos.c10`, `opus:7-appA-beam.c4`.
- Quotes: p.216 "Bits 7-0 The horizontal position, bits H8-H1."; p.285 same
  row as A9, bottom half.
- Proposition: VHPOSR bits 7 through 0 carry horizontal-position bits H8
  through H1. Same "new relative to ch3" reasoning as A9.
- Expected: not stated by the claims file; to be measured in step 3.

### A11 — VHPOSW-BIT-LAYOUT-MATCHES-VHPOSR
- Pages: 216, 285. Uids: `sol:7-beam.c14` (layout half), `opus:7-bp.c16`
  (layout half), `sol:7-appA-vhpos.c8` (content half).
- Quotes: p.216 "VHPOSW Write only Bits same as VHPOSR above."; p.285 "VHPOSW
  02C W A Write vertical and horizontal position of beam or lightpen"
  (light-pen half not asserted, same reason as A8).
- Proposition: writing VHPOSW places V7-V0 and H8-H1 at the same bit
  positions VHPOSR reads them at.
- Expected: PASS (per the same kind of prior-look note as A5, not yet
  confirmed by this package).

### A12 — VHPOSW-IS-WRITE-ONLY
- Pages: 216, 285. Uids: `sol:7-beam.c14` (direction half), `opus:7-bp.c16`
  (direction half), `sol:7-appA-vhpos.c6`.
- Same quotes as A11.
- Proposition: VHPOSW is write-only.
- Expected: **RED**, same reasoning and same RTL line the claims file names
  for A6 (agnus_beamcounter.v:111, by its account) — a deliberate,
  already-anticipated failure, not something to adjust.

### A13 — VHPOSW-WRITE-FORCES-BOTH-COUNTERS
- Page: 216. Uids: `sol:7-beam.c15` (mechanism half), `opus:7-bp.c17`
  (mechanism half).
- Quote: "Used for counter synchronization with chip test patterns."
- Proposition: writing VHPOSW forces both the vertical and horizontal beam
  counters to the written value (the mechanism the sentence names). The
  "with chip test patterns" half — a factory usage of that mechanism, naming
  no port and no separate proposition — is **not** asserted; see the discard
  table.
- Expected: not stated by the claims file; to be measured in step 3.

### A14 — MSW-IS-VPOSR-VPOSW
- Page: 216. Uids: `sol:7-beam.c17`, `opus:7-bp.c18` (ordering half only —
  its long-word-access half is discarded, see below).
- Quote: "the address pairs VPOSR,VHPOSR and VPOSW,VHPOSW can be read from and
  written to as long words ... with the most significant addresses being
  VPOSR and VPOSW."
- Proposition: within each long-word pair, VPOSR/VPOSW hold the
  most-significant half and VHPOSR/VHPOSW the least-significant half.
- Expected: not stated by the claims file; to be measured in step 3.

## Measured, not graded

### N1 — RESOLUTION-RATIO-1-160
- Pages: 216, 285. Uids: `sol:7-beam.c12`, `opus:7-bp.c14`,
  `sol:7-appA-vhpos.c11` (ratio half only), `opus:7-appA-beam.c5`.
- Quotes: p.216 "Horizontal resolution is 1/160th of the screen width."; p.285
  "RESOLUTION = 1/160 of screen width (280 ns)" (ratio half only — the "280
  ns" half is discarded, see below).
- This is chipset-dependent and unscoped in the source: on ECS, BEAMCON0's
  VARBEAMEN together with HTOTAL can make horizontal timing programmable
  (Appendix C p.305 — a different package's claim), so a fixture running an
  ECS configuration with a non-default HTOTAL would make this ratio false
  without the manual being wrong. Recorded via `note()`, not graded via
  `expect()`, and only meaningful measured against a fixed/default beam-timing
  configuration (OCS, or ECS with HTOTAL/BEAMCON0 at reset).

## Claims not turned into a new assertion

| uid(s) | page | proposition | reason |
|---|---|---|---|
| `sol:7-beam.c6`, `opus:7-bp.c6`, `sol:7-appA-vpos.c10`, `opus:7-appA-beam.c9`, `sol:7-appC-vposr.c2`, `opus:7-appC-vposr.c1` | 216, 285, 299 | VPOSR bit 15 = LOF | Already asserted, both chipsets, by `VPOSR-BIT15-IS-LOF-LONG-FRAME` (`hrm/ch3-beam-counters-and-blanking`), same fixture. |
| `sol:7-beam.c8`, `opus:7-bp.c8`, `sol:7-appA-vpos.c12`, `opus:7-appA-beam.c11`, `sol:7-appC-vposr.c6`, `opus:7-appC-vposr.c5` | 216, 285, 299 | VPOSR bit 0 (and, on ECS, bits 2-0) = V8 (v10,v9,V8) | Already asserted by `VPOSR-BITS-2-0-VERTICAL-POSITION`, same fixture, sweeping both chipsets. |
| `opus:7-bp.c9` | 216 | V8 makes a PAL count above 255 (up to 313) representable | **Guardrail, not an assertion.** Do not assert that a PAL field *is* 313 lines — `agnus_beamcounter` withdrew exactly that assertion as NM-HRM-001 because the manual sanctions both 312 and 313. What this claim actually supports (V8 exists and is readable) is covered by the row above; re-asserting the line count would reopen a question this suite already settled. |
| `sol:7-appC-map-vposr.c5`, `sol:7-appC-vposr.c1`, `sol:7-appC-vposr.c3`, `sol:7-appC-vposr.c4`, `sol:7-appC-vposr.c5`, `sol:7-appC-vposr.c7`, `sol:7-appC-vposr.c8`, `sol:7-appC-vposr.c9`, `sol:7-appC-vposr.c10`, `sol:7-appC-vposr.c11`, `sol:7-appC-vposr.c12`, `opus:7-appC-vposr.c1`-`c10` | 298, 299 | ECS VPOSR content: bits 14-8 = I6-I0, bit 7 = LOL, bits 6-3 unused, chip-identification values ($00/$10/$20/$30 across (ecs,ntsc)) | Already asserted by `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` / `VPOSR-BITS-6-3-UNUSED` / `VPOSR-BITS-2-0-VERTICAL-POSITION` (`hrm/ch3-beam-counters-and-blanking`), which the claims file itself notes already sweeps all four (ecs, ntsc) combinations on this same fixture. |
| `sol:7-appA-vpos.c13`, `opus:7-appA-beam.c12` | 285 | LOF is the long-frame auto-toggle bit tied to BPLCON0 | Already asserted by `LACE-BIT-CONTROLS-INTERLACE-FIELD-ALTERNATION`. Note for whoever writes A4/A5 in step 3: on ECS this toggle can be disabled by BEAMCON0 LOLDIS (a different package's claim), so any assertion here that depends on LOF actually toggling needs BEAMCON0 at its reset value — already handled where it's asserted. |
| `sol:7-appA-vpos.c1`, `sol:7-appA-vpos.c3`, `sol:7-appC-map-vposr.c1`, `sol:7-appC-map-vposr.c4` | 285, 298 | VPOSR is at $004 / is implemented in Agnus | Address already asserted by `VPOSR-ADDRESS-004`. "Implemented in Agnus" is a structural fact of which module answers the address, not a separate stimulus/response measurement — it is evidence produced by the same address-decode assertion, not a distinct one. |
| `sol:7-appA-vhpos.c1`, `sol:7-appA-vhpos.c3` | 285 | VHPOSR is at $006 / is implemented in Agnus | Same reasoning, `VHPOSR-ADDRESS-006`. |
| `sol:7-appA-vpos.c6`, `sol:7-appA-vpos.c8` | 285 | VPOSW is at $02A / is implemented in Agnus | Same reasoning, `VPOSW-ADDRESS-02A`. |
| `sol:7-appA-vhpos.c5`, `sol:7-appA-vhpos.c7` | 285 | VHPOSW is at $02C / is implemented in Agnus | Same reasoning, `VHPOSW-ADDRESS-02C`. |
| `sol:7-appA-vpos.c4` | 285 | the "(E)" mark states VPOSR changes under ECS | Legend/pointer notation, not itself a hardware behaviour distinct from the concrete ECS content it points to — which is covered by the ECS-content row above. |
| `sol:7-appC-map-vposr.c2` | 298 | the "chg" mark in the ECS register map | Same reasoning as the "(E)" mark, one level removed — points at the same already-covered ECS content. |
| `opus:7-appA-beam.c1` | 285 | bundled VHPOSR row (address + direction + chip + function) | Subsumed — `sol` split the identical row into `7-appA-vhpos.c1`-`c4`, which is the split this specification follows. |
| `opus:7-appA-beam.c2` | 285 | bundled VHPOSW row | Subsumed by `sol:7-appA-vhpos.c5`-`c8`. |
| `opus:7-appA-beam.c7` | 285 | bundled VPOSR row (incl. the "(E)" mark) | Subsumed by `sol:7-appA-vpos.c1`-`c5`. |
| `opus:7-appC298.c3` | 298 | bundled p.298 register-map row | Subsumed by `sol:7-appC-map-vposr.c1`-`c5`. |
| `opus:7-appA-beam.c8` (blank-cell half) | 285 | VPOSW's appendix-A row "carries no ECS mark" | Discarded — an empty table cell states nothing; this is an inference from silence, not a proposition the page makes (the crossread on this record flags exactly this: `blank-cell-inference`). |
| light-pen half of `sol:7-beam.c9`, `opus:7-bp.c10`, `sol:7-appA-vhpos.c4`, `sol:7-appA-vhpos.c8`, `opus:7-appA-beam.c1` (via its subsuming split) | 216, 285 | VHPOSR/VHPOSW "also read(s)/write(s) the light pen" | Discarded as absent — there is no light-pen input in this design; no port exists for the proposition to be about. |
| "chip test patterns" half of `sol:7-beam.c15`, `opus:7-bp.c17` | 216 | factory use of the VHPOSW counter-forcing mechanism | Discarded as absent — names no port and no proposition about the chip attached to the mechanism, which is itself asserted (A13). |
| `sol:7-beam.c16`, long-word half of `opus:7-bp.c18` | 216 | VPOSR/VHPOSR readable, VPOSW/VHPOSW writable, as one 32-bit long word | Discarded as absent — this is a proposition about the CPU bus's transfer-size handling, not about the beam-counter module itself; no bus transfer-size input exists at this module's boundary for the proposition to be measured against. |
| ratio half already counted in N1; "280 ns" half of `sol:7-appA-vhpos.c11` | 285 | one VHPOS increment is 280 ns | Discarded — no page defines a clock period or cycle-to-ns mapping this module boundary can be measured against without an added assumption; the ratio (1/160) survives unit-stripping and is recorded in N1, the absolute duration does not. |

## Totals

- 93 claims read, all 93 accounted for above.
- 14 new graded assertions (A1-A14): 2 expected RED (A6, A12), the rest not
  pre-judged by the source claims and left to be measured in step 3, with two
  (A3, A7) serving as control-PASS cases on the same measurement basis as the
  expected-RED ones.
- 1 measured-not-graded note (N1).
- The remaining claims are either already asserted elsewhere on this same
  fixture (no duplicate needed), subsumed by a finer split of the same
  quotation, or discarded as absent/not-hardware/blank-cell, each with its
  reason recorded above.

Nothing in this package required opening the RTL, the port list, or
`STEP3-NOTES-ch7-beam-position-word-layout.md` to write. No HALT condition was
encountered.
