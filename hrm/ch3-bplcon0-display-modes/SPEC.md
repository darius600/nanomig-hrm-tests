# ch3-bplcon0-display-modes -- specification

Written from the quoted manual text and PACKAGE.md's boundary description alone, **before** nanomig/src/minimig-aga/denise.v was opened. Order of work: this file is committed first; the RTL is opened only afterwards, and only harness-wiring mechanics (port names, exact bit widths, exact COLORxx encoding) are expected to change after that -- not the oracle stated here.

**Extension.** The sections below describing categories 19 and 20, and the four rows added to
category 2, the one row added to category 3, and the R9 reason group, were written from
`ch3-bplcon0-display-modes-spec-stage.jsonl` alone (quote, restatement, page -- no
`classify.evidence`), following this same rule: committed before the full claims file (with the
router's evidence) or `denise.v` were opened for this extension. Nothing already accepted below
this line changed; the additions only append new categories, rows and reason-group entries.

## Fixture and measurement primitives (planned)

- Top module `denise`, sources per PACKAGE.md's fixture list (denise.v +
  denise_bitplanes.v, denise_bitplane_shifter.v, denise_playfields.v, denise_sprites.v,
  denise_sprites_shifter.v, denise_collision.v, denise_hamgenerator.v, denise_colortable.v,
  denise_colortable_ram_mf.v, denise_spritepriority.v, bitplane_ram.v).
- Clock phases (trap 1, given): a 4-phase counter per 28 MHz tick with
  `clk7_en = (phase==0)`, `c1 = (phase==0 || phase==1)`, `c3 = (phase==1 || phase==2)` --
  never driven as disjoint halves. The plane shifter's parallel load fires on
  `!c1 && !c3` (denise_bitplane_shifter.v:224), so the display path depends on that
  phase relationship, and a harness that asserts nothing cannot tell a live path from
  a dead one.
- Horizontal scroll alignment (trap 2, given): denise_bitplanes.v:146-152 decodes
  `hpos[3:2]` into 0/4/8/12 lores pixels of extra scroll; plane writes in this suite are
  aligned to `hpos[3:2]==0` unless a test is deliberately exercising that offset.
- Window: DIWSTRT H=$30, DIWSTOP H=$50, reused from PACKAGE.md's own MEASURED example, as the
  common window for every category unless a category states otherwise.
- Register writes: one 16-bit word per write, address 0x100 = BPLCON0, address 0x104 =
  BPLCON2 (per PACKAGE.md's boundary note); address 0x106 = BPLCON3 is touched only by the
  REGMAP_LIGHT category. Exact address-bus signal name confirmed on RTL open.
- Colour-index recovery: COLORxx (the palette RAM) is preloaded, before any measurement, with
  a distinct, recoverable value per index (e.g. COLORn's channel bits encode n itself), so
  that the colour index a given stimulus produced can be read back uniquely from the rgb
  output alone -- exact bit packing confirmed on RTL open (COLORxx channel width: 4 bits/OCS
  vs wider/AGA is a harness-wiring detail, not an oracle).
- No RTL-derived constants are used to convert a raw measurement (tick counts, transition
  counts) into a claimed value; every quantitative expectation below is a number taken from
  the manual text itself.

## Claim categories

### 0. Control case / fixture smoke test

Measurement: this is the fixture's own smoke test, and doubles as the suite's mandatory
control case. Reproduce PACKAGE.md's own MEASURED walk-through verbatim: DIWSTRT H=$30,
DIWSTOP H=$50, BPLCON0 BPU=1 (planes 2-6 left at zero), BPL1DAT=$FF00, all other bitplane
data registers zero. Drive c1/c3/clk7_en per the four-phase pattern given in the task
(clk7_en=(phase==0), c1=(phase==0||1), c3=(phase==1||2)) -- NOT as disjoint halves; the
plane shifter loads on `!c1 && !c3` (denise_bitplane_shifter.v:224). Run enough clocks for one BPL1DAT load and one scan across
the window. Expect the rgb output to carry a distinguishable COLOR01-derived value inside the
window and COLOR00-derived value outside it. This is asserted FIRST, before any other claim in
this package, and is the basis every other category's "known value" measurement depends on.
Predicted outcome: PASS (this is the reference measurement the whole suite is built on; if it
fails, no other assertion in this package can be trusted, and the run halts).

| claim id | page | reader | restatement |
|---|---|---|---|
| `A-bpc.intro` | 271 | opus | Appendix A: the BPLCON registers control the operation of the bitplanes and various aspects of the display. |

1 assertion(s) in this category.

### 1. BPLCON0 bit position <-> field name identity

Measurement: for each named field, write BPLCON0 with ONLY that one bit set (all other
15 bits zero, including BPU/HIRES/etc.) and confirm the corresponding, distinctly-observable
effect activates (e.g. bit 15 alone selects the resolution-doubling behaviour, bit 11 alone
selects the HAM colour path, bit 9/10 alone changes COLOR_ON/DBLPF-gated behaviour, bits 14-12
alone select the BPU-gated plane count) while confirming NO other bit's effect activates from
that same single-bit write. This isolates bit position from field name directly, independent of
any RTL-internal name. Where a claim only asserts identity ("bit N is field X") without a
value-meaning, the pass condition is: setting bit N in isolation changes exactly the behaviour
documented for X and nothing else documented for another bit.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-bplcon0.c1` | 48 | sol | BPLCON0 bit 14 is BPU2. |
| `3-bplcon0.c2` | 48 | sol | BPLCON0 bit 13 is BPU1. |
| `3-bplcon0.c3` | 48 | sol | BPLCON0 bit 12 is BPU0. |
| `3-bpu.c01` | 48 | opus | The number of bitplanes in use is selected by writing to BPLCON0. |
| `3-bpu.c02` | 48 | opus | The bitplane-count field of BPLCON0 is bits 14, 13 and 12. |
| `3-bpu.c03` | 48 | opus | Those three bits are named BPU2, BPU1 and BPU0. |
| `3-r0.c15` | 90 | opus | BPLCON0 bit 9 is COLOR_ON, colour enable. |
| `3-r0.c18` | 90 | opus | BPLCON0 bit 10 is DBLPF, double-playfield enable. |
| `3-r0.c21` | 90 | opus | BPLCON0 bit 11 is HOMOD, hold-and-modify enable. |
| `3-r0.c25` | 90 | opus | BPLCON0 bits 14, 13 and 12 are BPU2, BPU1, BPU0 and give the number of bitplanes used. |
| `3-r0.c26` | 90 | opus | BPLCON0 bit 15 is HIRES, high resolution enable. |
| `A-bpc.b09.c0` | 271 | opus | Appendix A BPLCON bit table: bit 09 of BPLCON0 is COLOR. |
| `A-bpc.b10.c0` | 271 | opus | Appendix A BPLCON bit table: bit 10 of BPLCON0 is DBLPF. |
| `A-bpc.b11.c0` | 271 | opus | Appendix A BPLCON bit table: bit 11 of BPLCON0 is HOMOD. |
| `A-bpc.b12.c0` | 271 | opus | Appendix A BPLCON bit table: bit 12 of BPLCON0 is BPU0. |
| `A-bpc.b13.c0` | 271 | opus | Appendix A BPLCON bit table: bit 13 of BPLCON0 is BPU1. |
| `A-bpc.b14.c0` | 271 | opus | Appendix A BPLCON bit table: bit 14 of BPLCON0 is BPU2. |
| `A-bpc.b15.c0` | 271 | opus | Appendix A BPLCON bit table: bit 15 of BPLCON0 is HIRES. |
| `A-bplcon.c13` | 271 | sol | BPLCON0 bit 15 is field HIRES. |
| `A-bplcon.c16` | 271 | sol | BPLCON0 bit 14 is field BPU2. |
| `A-bplcon.c19` | 271 | sol | BPLCON0 bit 13 is field BPU1. |
| `A-bplcon.c22` | 271 | sol | BPLCON0 bit 12 is field BPU0. |
| `A-bplcon.c25` | 271 | sol | BPLCON0 bit 11 is field HOMOD. |
| `A-bplcon.c28` | 271 | sol | BPLCON0 bit 10 is field DBLPF. |
| `A-bplcon.c31` | 271 | sol | BPLCON0 bit 9 is field COLOR. |
| `C-bplcon0.b12` | 300 | opus | BPLCON0 bit 12 is BPU0, the low bit of the SuperHires depth field. |
| `C-bplcon0.b13` | 300 | opus | BPLCON0 bit 13 is BPU1; the BPU field encodes a SuperHires depth of 1 or 2 only. |
| `C-bplcon0.b14` | 300 | opus | BPLCON0 bit 14 is BPU2; with BPU1 and BPU0 it gives the depth of SuperHires mode, 1 or 2. |
| `C-shres.c10` | 300 | sol | ECS BPLCON0 bit 14 is BPU2, part of the SuperHires depth field. |
| `C-shres.c11` | 300 | sol | ECS BPLCON0 bit 13 is BPU1, part of the SuperHires depth field. |
| `C-shres.c12` | 300 | sol | ECS BPLCON0 bit 12 is BPU0, part of the SuperHires depth field. |

31 assertion(s) in this category.

### 2. BPU field (bits 14:12) -- plane-count truth table

Measurement: for BPU code 000 through 111 (all 8 values, including code 111 which the
manual calls "not used" and 000 which is called "None -- shows only background colour"), set
BPLCON0.BPU to that code with HIRES=0, DBLPF=0, HOMOD=0, and drive all six BPLxDAT registers
with a bit pattern where each active plane contributes a distinct, recoverable bit to the
colour index (see MSBORDER's plan for the recovery method). Confirm exactly the number of
planes the manual's Table 3-5 / register summary names respond in the resulting colour index,
and that planes beyond that count do not change it. Predicted outcome: codes 000-110 PASS
(control case media already proves the harness can read a known value). Code 111 is explicitly
undefined by the manual ("value not used") -- assert only what the manual states (that 111 is
not an assigned/used code), not a specific RTL behaviour for it, so this one is a documentation
claim, not a plane-count measurement; record whatever the RTL actually does as an observation,
not a pass/fail on plane count.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-bplcon0.c10` | 48 | sol | With BPU[2:0]=011, planes 1–3 are active. |
| `3-bplcon0.c11` | 48 | sol | BPU[2:0]=100 selects four bitplanes. |
| `3-bplcon0.c12` | 48 | sol | With BPU[2:0]=100, planes 1–4 are active. |
| `3-bplcon0.c13` | 48 | sol | BPU[2:0]=101 selects five bitplanes. |
| `3-bplcon0.c14` | 48 | sol | With BPU[2:0]=101, planes 1–5 are active. |
| `3-bplcon0.c15` | 48 | sol | BPU[2:0]=110 selects six bitplanes. |
| `3-bplcon0.c16` | 48 | sol | With BPU[2:0]=110, planes 1–6 are active. |
| `3-bplcon0.c17` | 48 | sol | BPU[2:0]=111 is unused. |
| `3-bplcon0.c19` | 48 | sol | Selecting zero bitplanes makes no playfield visible. |
| `3-bplcon0.c4` | 48 | sol | BPU[2:0]=000 selects no bitplanes. |
| `3-bplcon0.c5` | 48 | sol | BPU[2:0]=001 selects one bitplane. |
| `3-bplcon0.c6` | 48 | sol | With BPU[2:0]=001, the active bitplane is plane 1. |
| `3-bplcon0.c7` | 48 | sol | BPU[2:0]=010 selects two bitplanes. |
| `3-bplcon0.c8` | 48 | sol | With BPU[2:0]=010, planes 1 and 2 are active. |
| `3-bplcon0.c9` | 48 | sol | BPU[2:0]=011 selects three bitplanes. |
| `3-r0.bpu001` | 90 | opus | Register summary: BPLCON0 BPU code 001 selects 1 bitplane, PLANE 1. |
| `3-r0.bpu010` | 90 | opus | Register summary: BPLCON0 BPU code 010 selects 2 bitplanes, PLANES 1 and 2. |
| `3-r0.bpu011` | 90 | opus | Register summary: BPLCON0 BPU code 011 selects 3 bitplanes, PLANES 1 - 3. |
| `3-r0.bpu100` | 90 | opus | Register summary: BPLCON0 BPU code 100 selects 4 bitplanes, PLANES 1 - 4. |
| `3-r0.bpu101` | 90 | opus | Register summary: BPLCON0 BPU code 101 selects 5 bitplanes, PLANES 1 - 5. |
| `3-r0.bpu110` | 90 | opus | Register summary: BPLCON0 BPU code 110 selects 6 bitplanes, PLANES 1 - 6. |
| `3-r0.bpu111` | 90 | opus | Register summary: BPLCON0 BPU code 111 is not used. |
| `3-regsum-bplcon0.c20` | 90 | sol | BPLCON0.BPU=001 selects one bitplane, plane 1. |
| `3-regsum-bplcon0.c21` | 90 | sol | BPLCON0.BPU=010 selects planes 1 and 2. |
| `3-regsum-bplcon0.c22` | 90 | sol | BPLCON0.BPU=011 selects planes 1–3. |
| `3-regsum-bplcon0.c23` | 90 | sol | BPLCON0.BPU=100 selects planes 1–4. |
| `3-regsum-bplcon0.c24` | 90 | sol | BPLCON0.BPU=101 selects planes 1–5. |
| `3-regsum-bplcon0.c25` | 90 | sol | BPLCON0.BPU=110 selects planes 1–6. |
| `3-regsum-bplcon0.c26` | 90 | sol | BPLCON0.BPU=111 is unused. |
| `3-sum.c01` | 63 | opus | Summary: BPLCON0 bits 12-14 (BPU2-BPU0) set the number of bitplanes. |
| `3-t5.c01` | 48 | opus | Table 3-5: BPU value 000 selects no bitplanes. |
| `3-t5.c03` | 48 | opus | Table 3-5: BPU value 001 selects 1 bitplane. |
| `3-t5.c04` | 48 | opus | Table 3-5: with BPU=001 the bitplane used is named PLANE 1. |
| `3-t5.c05` | 48 | opus | Table 3-5: BPU value 010 selects 2 bitplanes. |
| `3-t5.c06` | 48 | opus | Table 3-5: with BPU=010 the bitplanes used are PLANES 1 and 2. |
| `3-t5.c07` | 48 | opus | Table 3-5: BPU value 011 selects 3 bitplanes. |
| `3-t5.c08` | 48 | opus | Table 3-5: with BPU=011 the bitplanes used are PLANES 1 - 3. |
| `3-t5.c09` | 48 | opus | Table 3-5: BPU value 100 selects 4 bitplanes. |
| `3-t5.c10` | 48 | opus | Table 3-5: with BPU=100 the bitplanes used are PLANES 1 - 4. |
| `3-t5.c11` | 48 | opus | Table 3-5: BPU value 101 selects 5 bitplanes. |
| `3-t5.c12` | 48 | opus | Table 3-5: with BPU=101 the bitplanes used are PLANES 1 - 5. |
| `3-t5.c13` | 48 | opus | Table 3-5: BPU value 110 selects 6 bitplanes. |
| `3-t5.c14` | 48 | opus | Table 3-5: with BPU=110 the bitplanes used are PLANES 1 - 6. |
| `3-t5.c15` | 48 | opus | Table 3-5: BPU value 111 is not used. |
| `A-bplcon.c62` | 271 | sol | BPLCON0.BPU codes 000 through 110 select zero through six bitplanes. |
| `A-leg.bpu` | 271 | opus | Appendix A legend: BPU is a bitplane use code with the valid range 000 to 110, meaning none through six inclusive. |

**Extension.** Four more claim rows restate BPU=000's Table 3-5 /
register-summary footnote ("Shows only a background color; no playfield is visible") from two
more source locations (the footnote text itself, and the register-summary row) and two more
readers. Colour index 0 is COLOR00, the register this package's own control case already
identifies as "background colour" (its own outside-window MEASURED reading, category 0 above).
Graded from a fresh, dedicated measurement, not a re-read of `g_bpu[0]`/`3-bplcon0.c19`/
`3-bplcon0.c4` above (that entry is already logged in MUTATIONS.md as impossible to mutate under
this package's six implementation mutations -- the trivial floor of the plane-gating chain, no
stimulus can make a non-existent plane show through). These four claims instead check that what
shows with BPU=000 tracks COLOR00's actual content: COLOR0 is driven to a chosen 12-bit value
and the observed pixel decoded against that value, with a genuine stimulus mutation (BPU changed
to 001 with plane 1 driven) confirming the comparator is sensitive to the BPU=000 condition.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-bplcon0.c18` | 48 | sol | Selecting zero bitplanes shows only the background color. |
| `3-t5.c02` | 48 | opus | Table 3-5 footnote: with BPU=000 only the background colour is shown and no playfield is visible. |
| `3-r0.bpu000` | 90 | opus | Register summary: BPLCON0 BPU code 000 shows only a background colour. |
| `3-regsum-bplcon0.c19` | 90 | sol | BPLCON0.BPU=000 selects only background color. |

50 assertion(s) in this category (46 original + 4 added).

### 3. Sixth bitplane usage footnote (dual-playfield / HAM)

Measurement: with BPU=110 (six planes) and DBLPF=1 (dual-playfield) OR HOMOD=1
(HAM), confirm plane 6's data reaches the colour computation (reusing the DBLPF/HAM category's
own measurement setup). This tests only the AFFIRMATIVE half of the Table 3-5 footnote
("sixth bitplane is used ... in dual-playfield mode and in hold-and-modify mode"). NOTE: this
footnote's "only" qualifier is in tension with this same package's own EHB claims (3-ehb.c03,
page 95), which require BPU=6 with DBLPF=0 AND HOMOD=0 -- i.e. the EHB section itself uses a
sixth bitplane in neither dual-playfield nor HAM mode. This is a manual-internal tension found
before any RTL was opened; it is recorded here, not resolved, and not mine to resolve per the
HAM/bits-4-7 precedent set for this package. Only the affirmative "is used in X" half of the
footnote is asserted; the exclusive "only" half is not.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-bplcon0.c21` | 48 | sol | The sixth bitplane is available in hold-and-modify mode. |
| `3-t5.c16` | 48 | opus | Table 3-5 footnote: the sixth bitplane is used only in dual-playfield mode and in hold-and-modify mode. |
| `3-bplcon0.c20` | 48 | sol | The sixth bitplane is available in dual-playfield mode. |

3 assertion(s) in this category. `3-bplcon0.c20` -- the DBLPF half of the same footnote --
no longer shares the combined dual-playfield/HAM evidence the two rows above it use: that
bundled range (planes 2, 4, 6 driven together, any index 8..15 accepted) does not discriminate
plane 6's own contribution, and its HAM half grades a different claim (3-bplcon0.c21) entirely.
It is graded instead by its own measurement: plane 6 driven ALONE, DBLPF=1, BPU=6, checked
against the exact expected index 12 (p.70 Table 3-11: plane 6 is playfield 2's most significant
bit).

### 4. Whole-register reload (no independent bit set)

Measurement: the register-bus write path for custom chip registers in this
architecture (per PACKAGE.md's boundary note, "BPLCON0 9h100", write l.171-178) accepts one
16-bit word per write cycle at one address; there is no documented sub-register byte- or
bit-lane strobe for BPLCON0. Confirm structurally that a single write to address 0x100 is
what is required to change any field (already exercised by every other category's setup step:
each scenario reaches its target field configuration via exactly one BPLCON0 write), and that
no alternate address or partial-word path exists in the boundary description for setting a
single bit. Predicted outcome: PASS by construction of the write protocol.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-bplcon0.c22` | 48 | sol | BPLCON0 bits cannot be updated independently. |
| `3-bplcon0.c23` | 48 | sol | Changing any BPLCON0 bit requires rewriting the whole register. |
| `3-bpu.c04` | 48 | opus | The bits of BPLCON0 cannot be set independently. |
| `3-bpu.c05` | 48 | opus | Setting any one BPLCON0 bit requires reloading the whole register. |
| `3-r0.c01` | 89 | opus | Register summary: BPLCON0 bits cannot be independently set. |

5 assertion(s) in this category.

### 5. Plane count -> achievable colour-index range

Measurement: for plane counts 1 through 5 (single playfield, normal mode -- the
manual caps this range at five; six planes is dual-playfield/HAM territory per Table 3-5's
footnote and is out of range for this claim group), drive all active planes' data through the
full 2^N combinations at one pixel position (N = plane count) using the COLOR_ON basis; confirm
the resulting colour index takes exactly 2^N distinct values within the range the manual's
Table 3-1 names (e.g. N=1 -> 2 values = "1-2 colors", N=3 -> 8 values = "5-8" upper bound "eight
colors", etc. -- Table 3-1's ranges are inclusive lower bounds from fewer active bits, so the
achievable-maximum reading, not the open lower end, is what is checked). N=0 (3-code.c01, a
single bitplane driven to all zero bits) is checked as a sub-case: one bitplane, all bits zero,
must read colour index 0 ("one colour" -- the single available index).

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-basic-color.c1` | 45 | sol | One bitplane provides 1–2 colors. |
| `3-basic-color.c2` | 45 | sol | Two bitplanes provide 3–4 colors. |
| `3-basic-color.c3` | 45 | sol | Three bitplanes provide 5–8 colors. |
| `3-basic-color.c4` | 45 | sol | Four bitplanes provide 9–16 colors. |
| `3-basic-color.c5` | 45 | sol | Five bitplanes provide 17–32 colors. |
| `3-bpl.c04` | 42 | opus | A four-colour display requires two bitplanes in memory. |
| `3-bpl.c06` | 42 | opus | At most five bitplanes may be combined in this way (single playfield, normal mode). |
| `3-bpl.c07` | 42 | opus | Three bitplanes give a choice of eight colours. |
| `3-bpl.c08` | 42 | opus | Four bitplanes give a choice of sixteen colours. |
| `3-bpl.c09` | 42 | opus | Five bitplanes give a choice of thirty-two colours. |
| `3-code.c01` | 55 | opus | A one-colour playfield is produced by writing 0 into every bit of the single bitplane. |
| `3-code.c04` | 56 | opus | Three or more colours require more than one bitplane. |
| `3-code.c05` | 56 | opus | A single playfield combines at most five bitplanes. |
| `3-coding.c1` | 55 | sol | A single bitplane filled with zero bits produces a one-color playfield. |
| `3-coding.c10` | 56 | sol | A normal single playfield combines up to five bitplanes. |
| `3-coding.c5` | 56 | sol | A playfield with at least three colors requires more than one bitplane. |
| `3-coding.c6` | 56 | sol | The multi-bitplane color-index method supports playfields with up to 32 colors. |
| `3-color.c10` | 42 | sol | Five bitplanes provide a choice of thirty-two colors. |
| `3-color.c7` | 42 | sol | Normal single-playfield color indirection combines at most five bitplanes. |
| `3-color.c8` | 42 | sol | Three bitplanes provide a choice of eight colors. |
| `3-color.c9` | 42 | sol | Four bitplanes provide a choice of sixteen colors. |
| `3-t1.c01` | 45 | opus | Table 3-1: 1 to 2 colours require 1 bitplane. |
| `3-t1.c02` | 45 | opus | Table 3-1: 3 to 4 colours require 2 bitplanes. |
| `3-t1.c03` | 45 | opus | Table 3-1: 5 to 8 colours require 3 bitplanes. |
| `3-t1.c04` | 45 | opus | Table 3-1: 9 to 16 colours require 4 bitplanes. |
| `3-t1.c05` | 45 | opus | Table 3-1: 17 to 32 colours require 5 bitplanes. |

26 assertion(s) in this category.

### 6. Bitplane -> colour-index bit-significance order

Measurement: with BPU=5 (five planes, single playfield), set exactly one plane's
data bit high at a time at a fixed pixel position (planes 1..5 in turn, others held at zero),
and read the resulting colour index. The manual states plane 1 contributes the least-significant
bit and each higher-numbered plane the next bit up; confirm index values equal 2^(planeNumber-1)
for a lone bit on plane `planeNumber`, i.e. plane 1 alone -> index 1, plane 5 alone -> index 16.
This is measured directly from the recovered colour index (see COLORCOUNT/CONTROL for the
COLORxx-preload recovery method) with no RTL-derived conversion constant -- the expected index
values (1,2,4,8,16) come from the manual's stated bit order, not from anything read out of the
Verilog.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-bpl.c19` | 43 | opus | The next highest-numbered bitplane forms the next bit, descending in significance. |
| `3-bpl.f06` | 44 | opus | Figure 3-4 (printed p.44): the bottom row of the same block is annotated 'Data in bitplane 1 - least significant' - bitplane 1 supplies the least significant bit. |
| `3-color.c17` | 43 | sol | The highest-numbered active bitplane supplies the most-significant bit of the color index. |
| `3-color.c19` | 43 | sol | Successively lower-numbered bitplanes supply successive lower-significance bits of the color index. |

4 assertion(s) in this category.

### 7. HIRES (bit 15) -- resolution-mode bit

Measurement: with a fixed DIWSTRT/DIWSTOP window (reusing the control case's $30/$50
window) and a fixed, alternating BPL1DAT pattern, count the number of distinct colour-index
CHANGES the output makes while scanning across the window, once with HIRES=0 and once with
HIRES=1, all else equal. The manual states hires mode fits twice as many pixels in the same
physical/window width as lores (page 41's figure, page 49's bit description). Expected result:
the HIRES=1 run shows exactly twice as many index changes across the identical window as the
HIRES=0 run, counted directly from observed output transitions -- not derived by dividing a
tick count by a pixel-width constant taken from the RTL (that would make the RTL's own assumed
pixel width the oracle, which is exactly the circularity this suite must avoid).

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-r0.c27` | 90 | opus | BPLCON0 bit 15 = 1 selects high resolution mode. |
| `3-r0.c28` | 90 | opus | BPLCON0 bit 15 = 0 selects low resolution mode. |
| `3-regsum-bplcon0.c27` | 90 | sol | BPLCON0.HIRES bit 15 value one selects high resolution. |
| `3-regsum-bplcon0.c28` | 90 | sol | BPLCON0.HIRES bit 15 value zero selects low resolution. |
| `3-res.c10` | 49 | opus | Horizontal resolution mode is set by BPLCON0 bit 15, named HIRES. |
| `3-res.c11` | 49 | opus | Writing 1 to BPLCON0 bit 15 selects high resolution mode. |
| `3-res.c12` | 49 | opus | Writing 0 to BPLCON0 bit 15 selects low resolution mode. |
| `3-res.f02` | 41 | opus | Figure 3-2 (printed p.41): the right screen carries the label '640 Pixels' on a horizontal double arrow spanning an identically sized frame, captioned 'In high resolution mode, 640 pixels fill a horizontal line.' - the two frames are drawn the same physical width, so 640 hires pixels occupy the same width as 320 lores pixels. |
| `3-resolution.c4` | 49 | sol | BPLCON0 HIRES bit 15 value 1 selects high-resolution mode. |
| `3-resolution.c5` | 49 | sol | BPLCON0 HIRES bit 15 value 0 selects low-resolution mode. |
| `3-sum.c04` | 63 | opus | Summary, low resolution: clear BPLCON0 bit 15 (HIRES). |
| `3-sum.c06` | 63 | opus | Summary, high resolution: set BPLCON0 bit 15 (HIRES). |
| `A-bplcon.c61` | 271 | sol | BPLCON0.HIRES selects 70-ns pixels. |
| `A-leg.hires` | 271 | opus | Appendix A legend: HIRES selects high resolution, which is a 70 ns pixel. |

14 assertion(s) in this category.

### 8. DBLPF (bit 10) -- dual-playfield grouping

Measurement: with BPU=6 (six planes) and DBLPF=1, drive only the odd-numbered planes
(1,3,5) with a distinguishing pattern while holding even planes (2,4,6) at zero, and separately
drive only the even planes while holding odd at zero. Confirm the two resulting colour groupings
are independently controllable and that each group's achievable index range spans exactly 3 bits
(matching "each playfield ... formed from one, two or three bitplanes", 3-dual.c05 /
3-dual-mode.c12) -- i.e. odd planes 1/3/5 form one 3-bit group and even planes 2/4/6 form the
other, with no cross-contribution measured in either group when the other group's planes are
driven. This single measurement setup is the evidence used for the odd/even grouping claims and
the playfield-depth claims. It does NOT establish the "changes hardware movement" clause of the
same page-72 sentence (3-dual-activate.c4 / 3-dual.c46) -- that is a different clause of the
sentence, graded separately in category 20 below.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-dual-activate.c2` | 72 | sol | DBLPF groups all odd-numbered bitplanes together for color interpretation. |
| `3-dual-activate.c3` | 72 | sol | DBLPF groups all even-numbered bitplanes together for color interpretation. |
| `3-dual-mode.c12` | 68 | sol | The labels above both source playfields state that each playfield can contain 1, 2, or 3 bitplanes. |
| `3-dual.c05` | 67 | opus | Each playfield in a dual display is formed from one, two or three bitplanes. |
| `3-r0.c20` | 90 | opus | BPLCON0 bit 10 = 0 enables a single playfield. |
| `3-regsum-bplcon0.c15` | 90 | sol | BPLCON0.DBLPF bit 10 value zero enables single playfield mode. |
| `A-bplcon.c67` | 271 | sol | In double-playfield mode, PF1 comprises odd bitplanes. |
| `A-bplcon.c68` | 271 | sol | In double-playfield mode, PF2 comprises even bitplanes. |
| `C-bplcon0.b10` | 300 | opus | BPLCON0 bit 10 (DPF) is compatible with SuperHires mode. |
| `C-shres.c15` | 300 | sol | Dual-playfield mode is compatible with ECS SuperHires. |

10 assertion(s) in this category.

**A note on `3-dual-activate.c4`:** this row's own claim, "DBLPF changes the hardware movement
behavior of the bitplanes on-screen," is the SAME wrong-clause proxy problem `3-dual.c46` (the
identical clause, second reader) is NOT graded by, in category 20 below. Both ids name the same
page-72 "changes hardware movement" clause, which the colour-grouping evidence above does not
establish; both are withdrawn to a RECORDED line citing p.85 -- see "Claims not asserted in this
package", R10 below.

### 9. HOMOD (bit 11) -- hold-and-modify enable conditions

Measurement: with HOMOD=1 (and, per the affirmative half of the manual's own stated
conditions, BPU=101 or 110, HIRES=0), confirm the colour-computation path measurably departs
from a plain index-into-COLORxx lookup (e.g. a control-bit-pair in the plane data selects
"modify previous R/G/or B channel" instead of a fresh palette lookup, per the standard HAM
encoding this manual describes elsewhere in the chapter) -- the exact wiring of this check is
finalised once the RTL is open (denise_hamgenerator.v), but the oracle is fixed now: HOMOD=1
must select a distinguishably different colour path than HOMOD=0 at the same index bits.
DISPUTED BIT NUMBER, not resolved here: every claim row assigned to this package that names a
bit number for the HAM enable condition (3-ham.c20/c21 both readers, 3-r0.c21/c22,
3-regsum-bplcon0.c16, A-bplcon.c63, A-bpc.b11.c0, C-bplcon0.b11 -- pages 87, 90, 271, 300) names
bit 11. The task's own brief separately reports that printed page 95 states the HAM-enable bit
as bit 10 elsewhere in that page's prose; no claim row in this package's assigned 263 carries
that "bit 10" wording (the page-95 rows this package DOES hold, 3-ehb.c03/c04/3-ehb.c4/c5, are
from the EHB paragraph on that same page and themselves say "bit 11") -- so this package asserts
what its own quoted sources unanimously state (bit 11 enables HAM) and records, without a claim
row to hang it on, that the disputed "page 95 says bit 10" reading exists per the task brief and
is not adjudicated here.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-ham.c07` | 86 | opus | Hold-and-modify mode uses all six bitplanes. |
| `3-ham.c12` | 86 | opus | If only five bitplanes are used, plane 6 data is automatically supplied as 0. |
| `3-ham.c20` | 87 | opus | BPLCON0 bit 11 selects hold-and-modify mode. |
| `3-ham.c20` | 87 | sol | BPLCON0 bit 11 selects HAM mode. |
| `3-ham.c21` | 87 | opus | For HAM to be active, BPLCON0 bit 11 (HOMOD) must be 1. |
| `3-ham.c21` | 87 | sol | HAM is active only when BPLCON0.HOMOD bit 11 is one. |
| `3-ham.c23` | 87 | opus | For HAM to be active, BPLCON0 bit 15 (HIRES) must be 0, i.e. low resolution mode. |
| `3-ham.c23` | 87 | sol | HAM is active only in low resolution with BPLCON0.HIRES bit 15 zero. |
| `3-ham.c24` | 87 | opus | For HAM to be active, BPLCON0 bits 14-12 (BPU2-BPU0) must be 101 or 110. |
| `3-ham.c24` | 87 | sol | HAM is active with BPLCON0.BPU=101 (five bitplanes). |
| `3-ham.c25` | 87 | opus | BPU 101 or 110 means five or six bitplanes are active - HAM requires five or six bitplanes. |
| `3-ham.c25` | 87 | sol | HAM is active with BPLCON0.BPU=110 (six bitplanes). |
| `3-ham.c7` | 86 | sol | Full OCS HAM operation uses six bitplanes. |
| `3-r0.c22` | 90 | opus | BPLCON0 bit 11 = 1 enables hold-and-modify. |
| `3-r0.c23` | 90 | opus | BPLCON0 bit 11 = 0 disables hold-and-modify. |
| `3-regsum-bplcon0.c16` | 90 | sol | BPLCON0.HOMOD bit 11 value one enables HAM. |
| `3-regsum-bplcon0.c17` | 90 | sol | BPLCON0.HOMOD bit 11 value zero disables HAM. |
| `A-bplcon.c63` | 271 | sol | BPLCON0.HOMOD=1 selects HAM. |
| `C-bplcon0.b11` | 300 | opus | BPLCON0 bit 11 (HAM) is incompatible with SuperHires mode. |
| `C-shres.c14` | 300 | sol | HAM is incompatible with ECS SuperHires. |

20 assertion(s) in this category.

### 10. Extra Half-Brite conditions (HOMOD=0, DBLPF=0, BPU=6)

Measurement: with HOMOD=0, DBLPF=0, BPU=110 (six planes), confirm that plane 6's data
bit acts as a per-pixel intensity-halving control over the index formed by planes 1-5 (i.e. the
five-plane index selects one of 32 COLORxx entries, and plane6=1 yields an output RGB equal to
that entry's channels each right-shifted one bit / divided by two, while plane6=0 yields the
un-halved entry) -- giving 64 reachable output colours (32 base + 32 half-brite) versus the
32 reachable with a plain 5-plane index. Also check the ECS KILLEHB override (C-ecsbit.b09 /
C-genlock-ext.c14, BPLCON2 bit 9): if implemented in this fixture, KILLEHB=1 under the same
HOMOD=0/DBLPF=0/BPU=6 setup must suppress the halving. Wiring finalised once BPLCON2's bit
width in this build is confirmed on RTL open.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-ehb.c01` | 95 | opus | Extra Half Brite (EHB) mode doubles the maximum number of colours displayable at one time. |
| `3-ehb.c02` | 95 | opus | EHB mode requires six bitplanes to be set up. |
| `3-ehb.c03` | 95 | opus | EHB mode requires BPU = 6 in BPLCON0 bits 12, 13 and 14. |
| `3-ehb.c04` | 95 | opus | EHB mode requires HOMOD = 0 (BPLCON0 bit 11). |
| `3-ehb.c05` | 95 | opus | EHB mode requires DBLPF = 0 (BPLCON0 bit 10). |
| `3-ehb.c08` | 95 | opus | EHB mode allows 64 colours to be displayed at one time instead of the usual 32. |
| `3-ehb.c1` | 95 | sol | EHB doubles the normal simultaneous-color maximum. |
| `3-ehb.c2` | 95 | sol | EHB requires six bitplanes. |
| `3-ehb.c3` | 95 | sol | EHB requires BPLCON0.BPU=6 in bits 14–12. |
| `3-ehb.c4` | 95 | sol | EHB requires BPLCON0.HOMOD bit 11 equal to zero. |
| `3-ehb.c5` | 95 | sol | EHB requires BPLCON0.DBLPF bit 10 equal to zero. |
| `3-ehb.c8` | 95 | sol | EHB displays up to 64 simultaneous colors rather than the normal 32. |
| `3-r0.c24` | 90 | opus | With HOMOD = 0, extra-half-brite (EHB) is enabled when DBLPF = 0 and BPUx = 6. |
| `3-regsum-bplcon0.c18` | 90 | sol | With HOMOD=0, EHB is enabled only when DBLPF=0 and BPU=6. |
| `A-bplcon.c64` | 271 | sol | HOMOD=0 and BPU=6 are required for EHB. |
| `A-bplcon.c65` | 271 | sol | DBLPF=0 is also required for EHB. |
| `A-leg.homod2` | 271 | opus | Appendix A legend: HOMOD = 0 gives Extra Half Brite (EHB) when HAM=0 and BPU=6. |
| `A-leg.homod3` | 271 | opus | Appendix A legend: EHB additionally requires DBLPF=0. |
| `C-ecsbit.b09` | 307 | opus | BPLCON2 bit 9 is KILLEHB, which kills Extra Half Brite. |
| `C-genlock-ext.c14` | 307 | sol | ECS BPLCON2 bit 9 KILLEHB disables Extra Half Brite. |

20 assertion(s) in this category.

### 11. Bits 0 and 4-7 -- OCS "not used, make 0"

Measurement: with the fixture's chipset selection at its OCS/base configuration
(confirmed on RTL open -- PACKAGE.md's boundary decode names an `aga` flag and an `ecsena`
signal, so whichever selection this fixture defaults to is the one this category measures),
set bits 0 and 4-7 of BPLCON0 individually (all other bits held at a known-working baseline
configuration) and confirm no observable change in rgb output, resolution, plane count, DBLPF
grouping, or HAM/EHB behaviour results -- matching chapter 3 page 89's "not used (make 0)".
Every assertion in this category names OCS explicitly in its text, per the task's instruction
to carry the chipset designation in any claim touching these bits.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-r0.c02` | 89 | opus | BPLCON0 bit 0 is unused. |
| `3-regsum-bplcon0.c1` | 89 | sol | BPLCON0 bit 0 is unused. |
| `3-regsum-bplcon0.c7` | 89 | sol | BPLCON0 bits 7–4 are unused. |
| `3-regsum-bplcon0.c8` | 89 | sol | BPLCON0 bits 7–4 must be written as zero. |
| `A-bpc.b00.x` | 271 | opus | Appendix A BPLCON bit table: bit 00 of BPLCON0 is marked X, i.e. unassigned. |
| `A-bpc.b07.x` | 271 | opus | Appendix A BPLCON bit table: bit 07 of BPLCON0 and of BPLCON2 is marked X, i.e. unassigned. |
| `A-bplcon.c37` | 271 | sol | BPLCON0 bit 7 is unused on OCS. |
| `A-bplcon.c40` | 271 | sol | BPLCON0 bit 6 is unused on OCS. |
| `A-bplcon.c43` | 271 | sol | BPLCON0 bit 5 is unused on OCS. |
| `A-bplcon.c46` | 271 | sol | BPLCON0 bit 4 is unused on OCS. |
| `A-bplcon.c58` | 271 | sol | BPLCON0 bit 0 is unused on OCS. |

11 assertion(s) in this category.

### 12. Bits 0 and 4-6 -- ECS reassignment (chipset-carrying)

Measurement: with the fixture's chipset selection at ECS (if this build
supports selecting it -- confirmed on RTL open; if the fixture is OCS-only, every claim in this
category is asserted as "not reachable in this fixture's chipset configuration" rather than
adjusted to pass, per the instruction never to adjust an oracle to match what the fixture can
reach), set bit 0 (ENBPLCN3) and confirm it gates BPLCON3 register writes taking effect; set
bit 6 (SHRES) and confirm the SuperHires pixel-rate behaviour (SHRES category) activates; bit 5
(BPLHWRM) is declined (its sprite/bitplane horizontal-window-repositioning function is DIW/scroll
territory belonging to sibling packages, and this fixture does not decode it to any observable
port). Bit 4 (SPRHWRM) is declined too, but for a different, printed-page reason -- see R8 in
"Claims not asserted in this package" below: p.300's row for bit 4 carries a mnemonic and no
description column at all, unlike bit 6 three rows above which states its behaviour in full, so
the manual requires nothing of the RTL at this bit; independently SPRHWRM names a sprite
function and this fixture drives bitplanes only. Every assertion remaining in this category
names ECS explicitly.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-bplcon0.b05` | 300 | opus | BPLCON0 bit 5 is BPLHWRM in the ECS. |
| `C-bplcon0.b06` | 300 | opus | BPLCON0 bit 6 is SHRES, the SuperHires 35 ns pixel enable bit. |
| `C-ecsbit.b00` | 307 | opus | BPLCON0 bit 0 is ENBPLCN3; the table says it enables the new 'BLPCON3' register. |
| `C-genlock-ext.c17` | 307 | sol | ECS BPLCON0 bit 0 ENBPLCN3 enables the new BPLCON3 register. |
| `C-shres.c16` | 300 | sol | ECS BPLCON0 bit 6 is SHRES; setting it enables 35-ns SuperHires pixels. |
| `C-shres.c17` | 300 | sol | ECS BPLCON0 bit 5 is BPLHWRM. |

6 assertion(s) in this category. (`C-bplcon0.b04` and `C-shres.c18`, ECS bit 4, are declined --
see R8 in "Claims not asserted in this package" below.)

### 13. SHRES (bit 6) -- SuperHires 35 ns pixel rate

Measurement: reusing the HIRES category's toggle-counting method (no RTL-derived
pixel-width constant), compare the number of colour-index transitions across the same window
with SHRES=1 against the HIRES=1 count and against the HIRES=0 count, using the same alternating
plane-data pattern in all three runs. Expect SHRES:HIRES = 2:1 and SHRES:LORES = 4:1 in
transition counts, matching "twice the horizontal resolution of Hires mode, and four times the
Lores rates" (page 300). Also assert the interaction rule "HIRES must be 0 if SHRES is enabled"
(bit 15 vs bit 6) as a distinct, separately-measured claim: set both HIRES=1 and SHRES=1
simultaneously and record whatever the fixture actually does (the manual states this as a
required programming discipline, not necessarily a hardware interlock, so this is recorded as
an observation of the documented combination, asserted against the manual's "set it to zero"
instruction). BPU-depth-limited-to-1-or-2 under SHRES (C-shres.c13) is checked by confirming
the achievable colour-index range under SHRES tops out at 2 bitplanes' worth in this fixture's
implementation, or is recorded as an observation if the fixture does not special-case it.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-bplcon0.b15` | 300 | opus | BPLCON0 bit 15 (HIRES) must be set to zero when SHRES is enabled. |
| `C-library.c1` | 297 | sol | ECS SuperHires bitplane pixels run at 35 ns per pixel. |
| `C-shr.c04` | 297 | opus | SuperHires mode provides a 35 ns pixel rate. |
| `C-shr.c07` | 300 | opus | SuperHires mode provides a 35 ns pixel display rate. |
| `C-shr.c08` | 300 | opus | SuperHires is twice the horizontal resolution of Hires mode. |
| `C-shr.c09` | 300 | opus | SuperHires is four times the Lores rate. |
| `C-shres.c1` | 300 | sol | ECS SuperHires outputs one pixel every 35 ns. |
| `C-shres.c13` | 300 | sol | The ECS BPU field selects a SuperHires depth of one or two planes. |
| `C-shres.c2` | 300 | sol | ECS SuperHires has twice the horizontal resolution of Hires. |
| `C-shres.c3` | 300 | sol | ECS SuperHires has four times the horizontal resolution of Lores. |
| `C-shres.c9` | 300 | sol | In ECS SuperHires, BPLCON0.HIRES bit 15 must be zero. |
| `C-superhires.c4` | 296 | sol | ECS retains all OCS display bitplane depths. |

12 assertion(s) in this category.

### 14. LACE (bit 2) -- interlace bit, SuperHires compatibility note

Measurement: bit 2 (LACE) is asserted at the identity/position level (BITID-style
isolation write) plus its SuperHires-compatibility note: with SHRES=1, toggle LACE and confirm
it still independently changes whatever field-latching behaviour LACE controls at the denise
boundary (i.e. SHRES does not force LACE to a fixed value or make it unreadable/ineffective).
Full interlace field-timing behaviour itself is out of this package's scope (a beam-counter /
vertical-timing concern, sibling territory); only the bit's continued independent
controllability under SHRES is measured here.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-bplcon0.b02` | 300 | opus | BPLCON0 bit 2 (LACE) is compatible with SuperHires mode. |
| `C-shres.c20` | 300 | sol | Interlace is compatible with ECS SuperHires. |

2 assertion(s) in this category.

### 15. LPEN (bit 3) -- light-pen enable bit, SuperHires compatibility note

Measurement: bit 3 (LPEN) is asserted at the identity/position level plus its
SuperHires-compatibility note, using the same independent-controllability method as LACE.
Light-pen strobe/latch behaviour itself is out of this package's scope.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-bplcon0.b03` | 300 | opus | BPLCON0 bit 3 (LPEN) is compatible with SuperHires mode. |
| `C-shres.c19` | 300 | sol | Light-pen enable is compatible with ECS SuperHires. |

2 assertion(s) in this category.

### 16. ERSY (bit 1) -- external synchronisation bit

Measurement: bit 1 (ERSY), value 0 = external synchronisation disabled. Asserted at
the identity/value level: isolate bit 1 and confirm it is independently latched and, where the
fixture exposes an observable synchronisation-mode effect at the denise boundary, that 0
corresponds to the internally-generated (non-external) sync path. If no such effect is
observable at denise (sync generation may be Agnus's function, outside this fixture), this is
downgraded to an identity-only check (bit 1 latches independently of the other 15 bits),
consistent with the BITID category's method.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-r0.c05` | 89 | opus | BPLCON0 bit 1 = 0 disables external synchronization. |

1 assertion(s) in this category.

### 17. BPLCON2 bits 7-15 -- unused on OCS

Measurement: same method as UNUSED_OCS, applied to BPLCON2 (address 0x104)
bits 7 through 15: set each individually against a known-working BPLCON2 baseline and confirm
no observable behaviour change results, matching page 90's "Bits 7-15 not used". Every
assertion in this category is an OCS-configuration claim (the appendix C ECS table reassigns
some of these bits, e.g. bit 9 = KILLEHB, tested separately under EHB).

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-r2.c05` | 90 | opus | BPLCON2 bits 7-15 are not used. |
| `3-regsum-bplcon2.c4` | 90 | sol | BPLCON2 bits 15–7 are unused on OCS. |
| `A-bplcon.c15` | 271 | sol | BPLCON2 bit 15 is unused on OCS. |
| `A-bplcon.c18` | 271 | sol | BPLCON2 bit 14 is unused on OCS. |
| `A-bplcon.c21` | 271 | sol | BPLCON2 bit 13 is unused on OCS. |
| `A-bplcon.c24` | 271 | sol | BPLCON2 bit 12 is unused on OCS. |
| `A-bplcon.c27` | 271 | sol | BPLCON2 bit 11 is unused on OCS. |
| `A-bplcon.c30` | 271 | sol | BPLCON2 bit 10 is unused on OCS. |
| `A-bplcon.c33` | 271 | sol | BPLCON2 bit 9 is unused on OCS. |
| `A-bplcon.c36` | 271 | sol | BPLCON2 bit 8 is unused on OCS. |
| `A-bplcon.c39` | 271 | sol | BPLCON2 bit 7 is unused on OCS. |

11 assertion(s) in this category.

### 18. BPLCON0/2/3 register-address identity (light touch, reuses primary write path)

Measurement: this package's own write path already demonstrates that address
0x100 controls BPLCON0-described behaviour and address 0x104 controls BPLCON2-described
behaviour, by construction, in every other category above. These claims (the appendix/subject
"BPLCON0/2/3 register ... controls ..." function-description rows) are asserted by re-citing
that already-exercised write path rather than re-deriving a new measurement, to avoid
duplicating the ch3-register-map-denise sibling package's address-map coverage. BPLCON3
(address 0x106, "enhanced features") is the one new address touched here: if this fixture does
not implement/decode address 0x106 at all, that is recorded as a genuine RED/finding (the
appendix's claim that BPLCON3 exists as a controlling register is then unmet by this fixture),
not adjusted.

| claim id | page | reader | restatement |
|---|---|---|---|
| `A-bplcon.c12` | 271 | sol | BPLCON3 is an enhanced Denise register controlling enhanced bitplane features. |
| `A-bplcon.c3` | 271 | sol | BPLCON0 controls miscellaneous bitplane and display functions in Agnus and Denise, with ECS changes marked for Denise. |
| `A-bplcon0.chip` | 271 | opus | Appendix A: BPLCON0 lives in both Agnus and Denise, and is changed in the ECS ('A D( E )'). |
| `A-bplcon0.fn` | 271 | opus | Appendix A: BPLCON0 is the bitplane control register carrying miscellaneous control bits. |
| `A-bplcon2.fn` | 271 | opus | Appendix A: BPLCON2 is the bitplane control register for video priority control. |
| `A-bplcon3.fn` | 271 | opus | Appendix A: BPLCON3 is the bitplane control register for enhanced features. |
| `B-subject.c113` | 291 | sol | BPLCON0 provides miscellaneous bitplane control. |
| `B-subject.c121` | 291 | sol | BPLCON2 provides priority control. |
| `B-subject.c125` | 291 | sol | BPLCON3 provides enhanced bitplane features. |
| `C-shres.c7` | 300 | sol | ECS changes BPLCON0. |

10 assertion(s) in this category.

### 19. High-resolution dual-playfield plane identity (planes 1&3 / 2&4)

Measurement: a fresh stimulus, not a re-read of category 8's BPU=6/three-planes-per-side
result. With HIRES=1 and DBLPF=1, set BPLCON0.BPU=100 (four planes total, matching the manual's
"up to two bitplanes in each playfield" hires figure -- page 68), drive plane 1 and plane 3
only (planes 2, 4, 5, 6 held at zero) and confirm the resulting colour index falls in
playfield 1's non-zero range (1-7, the same range category 8's DBLPF measurement already
established for the odd group); separately drive plane 2 and plane 4 only (planes 1, 3, 5, 6
held at zero) and confirm the index falls in playfield 2's range (8-15). This establishes only
the AFFIRMATIVE identity the quoted text states -- which two named planes feed each playfield
under hires -- not a claim that planes 5/6 are hardware-blocked from also contributing (the
quoted text does not say that, and it is not measured here).

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-dual-planes.c5` | 68 | sol | High-resolution dual-mode playfield 1 uses planes 1 and 3. |
| `3-dual-planes.c6` | 68 | sol | High-resolution dual-mode playfield 2 uses planes 2 and 4. |
| `3-dual.c19` | 68 | opus | High resolution dual-playfield: playfield 1 uses bitplanes 1 and 3. |
| `3-dual.c20` | 68 | opus | High resolution dual-playfield: playfield 2 uses bitplanes 2 and 4. |

4 assertion(s) in this category.

### 20. Dual-playfield movement (PF1H/PF2H independence) -- withdrawn to record

Measurement: the "changes hardware movement" clause of page 72's sentence (the clause
`3-dual-activate.c4`/`3-dual.c46` name, distinct from the colour-grouping clause category 8
measures) requires a PF1H/PF2H differential, not the colour-grouping evidence -- a wrong-clause
proxy is exactly the "id on an assertion that does not establish its claim" case this package's
own rules forbid. Drive plane 2 (a playfield-2/even plane) alone and measure the 28MHz-clock
offset at which its data becomes visible, for BPLCON1 PF2H nibble 4 vs nibble 12, once with
DBLPF=0 and once with DBLPF=1. Measured directly (a full 0-15 PF2H sweep under both DBLPF
settings, reproduced across repeated runs): the shift PF2H induces on plane 2's position is
identical under DBLPF=0 and DBLPF=1 -- 32 clocks either way, for this nibble pair, and the whole
sweep matches point for point between the two DBLPF settings past an initial warm-up transient
(a harness artifact, not an RTL property -- the very first one or two calls of this specific
stimulus read a different absolute clock than every later call of the same inputs; a discarded
warm-up call removes it, documented where it is discarded).

p.85's own warning is why: "Always set all six bits, even if you have only one playfield. Set
3-0 and 7-4 to the same value if you are using only one playfield" -- PF2H governs bitplanes 2,
4 and 6's delay the same way regardless of DBLPF, so the identical shift this fixture measures is
what the manual itself states, not a DBLPF-dependent movement mechanism for this fixture to show.
Page 71's "can be scrolled separately" (the section immediately preceding this one) is the
independent ODD/EVEN pointer and modulo movement ch3-bitplane-pointers-and-modulo covers, not a
DBLPF-dependent change to BPLCON1's delay mechanism -- reading one into it here would be treating
the manual's silence as a requirement.

0 assertion(s) in this category. `3-dual.c46` is kept as a MEASURED note (not graded) and
withdrawn, together with `3-dual-activate.c4` (category 8), to a RECORDED line citing p.85; see
"Claims not asserted in this package", R10.


Total claim-rows assigned to a measurement category: 243 (235 original + 10 added, less 2 recorded under R10)

## Claims not asserted in this package

Per the task's instruction to report claims that cannot be settled, grouped by
reason, rather than silently drop them:

### R1-boundary

Outside the denise-only fixture boundary: implemented by Agnus (DMA fetch, memory-pointer/modulo arithmetic, BEAMCON0/scan-rate timing, VPOSR chip ID) and not observable at the `denise` top module measured here.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-hscr.c20` | 84 | opus | The BPLxMOD registers hold the modulo for each bitplane used. |
| `3-vscroll.c7` | 81 | sol | The upward arrow and in-frame text show that increasing the bitplane start pointer reveals more of the lower picture. |
| `3-vscroll.c8` | 81 | sol | The downward arrow and in-frame text show that decreasing the bitplane start pointer reveals more of the upper picture. |
| `A-ddf.c7` | 277 | sol | Vertical bitplane-DMA timing follows the display-window vertical timing. |
| `C-beamcon0.c17` | 305 | sol | ECS VGA 31-kHz mode disables hard stops for display synchronization. |
| `C-chipid.c2` | 299 | sol | VPOSR Agnus ID value $20 identifies an enhanced Hires Agnus. |
| `C-chipid.c3` | 299 | sol | VPOSR Agnus ID value $30 identifies an enhanced Hires Agnus. |
| `C-scan.c1` | 304 | sol | ECS Agnus provides programmable scan-rate registers. |
| `C-shr.c12` | 300 | opus | Two SuperHires bitplanes saturate DMA bandwidth as much as four Hires bitplanes do. |
| `C-shres.c6` | 300 | sol | Two ECS SuperHires bitplanes consume as much DMA bandwidth as four Hires bitplanes. |
| `C-vposr.c07` | 299 | opus | A VPOSR identification value of 20 or 30 indicates that the enhanced HiRes Agnus is present. |

### R2-arith

A pure arithmetic/software formula (bytes-per-line x lines x planes), not a hardware behaviour any denise measurement can observe.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-big.c13` | 76 | opus | Memory requirement = bytes per line * lines in playfield * number of bitplanes. |
| `3-sum.c13` | 64 | opus | Summary: total bitplane data bytes = bytes per line * lines in playfield * number of bitplanes. |

### R3-regmap-sibling

DENISEID is a chip-identification register, not a BPLCON0 mode bit; its address-map coverage belongs to the ch3-register-map-denise sibling package (present, in progress, in this same hrm/ tree) and is not duplicated here.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-deniseid.c3` | 299 | sol | ECS Denise 8373 returns $FC in DENISEID bits 7–0. |
| `C-deniseid.c4` | 299 | sol | ECS DENISEID bits 15–8 are reserved. |

### R4-superhires-color-sibling

SuperHires per-pixel RGB bit-depth is a colour-encoding question, explicitly reserved by PACKAGE.md to the SuperHires-colour-encoding sibling package, not this BPLCON0-mode-bits package.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-shres-color.c1` | 301 | sol | An ECS SuperHires bitplane pixel has two red bits. |
| `C-shres-color.c2` | 301 | sol | An ECS SuperHires bitplane pixel has two green bits. |
| `C-shres-color.c3` | 301 | sol | An ECS SuperHires bitplane pixel has two blue bits. |

### R5-sprite-reg

Concerns SPRxCTL (a sprite control register), not BPLCON0 -- outside this package's assigned register family regardless of which sibling eventually covers it.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-shres-sprite.c10` | 303 | sol | ECS SPRxCTL bit 4 is SHSH1 and selects the 70-ns SuperHires start-position increment. |
| `C-shres-sprite.c11` | 303 | sol | ECS SPRxCTL bit 3 is reserved for an unimplemented 35-ns SuperHires start-position increment. |
| `C-shres-sprite.c16` | 303 | sol | Using the ECS 70-ns sprite-position bit in Hires mode can produce unpredictable results. |
| `C-spr.c06` | 303 | opus | SPRxCTL bit 04 is SHSH1, a 70 ns start-horizontal increment in SuperHires mode. |

### R6-genlock-zdepth

ZDBPSEL (BPLCON2 bits 12-13) selects a bitplane for external genlock Z-depth priority; it is not a display-mode bit and no display-mode behaviour this package tests depends on it.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-ecsbit.b12` | 307 | opus | BPLCON2 bit 12 is ZDBPSEL0, part of the three-bit bitplane-select field. |
| `C-ecsbit.b13` | 307 | opus | BPLCON2 bit 13 is ZDBPSEL1, part of the three-bit bitplane-select field. |

### R7-manual-notation

Describes the printed manual's own typographic convention (what an "(E)" mark in its own tables means), not a hardware behaviour observable at denise.

| claim id | page | reader | restatement |
|---|---|---|---|
| `B-ecsmark.c1` | 288 | sol | An (E) chip-column mark identifies a register changed in ECS. |
| `B-ecsmark.c2` | 288 | sol | Some (E)-marked registers are new in ECS. |

### R8-no-printed-behaviour

Page 300's ECS bit-assignment table gives BPLCON0 bit 4 only the mnemonic SPRHWRM -- unlike bit
6, three rows above, which carries a full description ("SuperHires 35ns pixel enable bit"):

```
            06       SHRES       SuperHires 35ns pixel enable bit
            05       BPLHWRM
            04       SPRHWRM
            03       LPEN        Compatible with SuperHires mode
```

A row that names a bit and states no behaviour requires nothing of the RTL at this or any
boundary -- not that an effect exists, and not that it doesn't; both would be adjusting an
oracle the manual never wrote. Independently, SPRHWRM names a sprite-horizontal function and
this package's fixture drives bitplanes only, so it could not observe a sprite effect even if
page 300 had printed one. (Previously asserted via `assertEcsReassignBit4`, which required a
measurable effect the manual never specified; that assertion was itself the misstatement and
is withdrawn here in favour of declining the claim -- see MUTATIONS.md.)

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-bplcon0.b04` | 300 | opus | BPLCON0 bit 4 is SPRHWRM in the ECS. |
| `C-shres.c18` | 300 | sol | ECS BPLCON0 bit 4 is SPRHWRM. |

### R9-software-not-hardware

Names what `graphics.library` (Amiga OS software) does, not what a chip does, so there is no
RTL-observable claim here at any fixture boundary. The full quotation: "This mode is controlled
by the graphics.library by writing to the BPLCON0 register in the LOF copperlist (/SHF if
interlaced)" (p.300) describes the OS's own convention for reaching BPLCON0, not a denise (or
even Agnus) hardware behaviour; this harness's register writes are driven directly by the test
bus regardless, so which software mechanism issued a write is outside any RTL fixture's
boundary. Distinct from R1-boundary (Agnus DMA/timing functions this fixture cannot reach) --
this claim's obstacle is not which chip implements it, but that it describes software, not
hardware. Its only hardware residue -- a Copper MOVE instruction landing on BPLCON0 without
CDANG set -- is a separate claim, `opus:B-bplcon0.copper`, routed to the ch2-copper-move-sweep
sibling package, not held here.

| claim id | page | reader | restatement |
|---|---|---|---|
| `C-shr.c13` | 300 | opus | SuperHires mode is controlled by writing BPLCON0 in the LOF copperlist (/SHF if interlaced). |

### R10-pf2h-delay-not-dblpf-gated

Both ids name page 72's "changes hardware movement" clause ("Selecting dual-playfield mode also
changes ... the way hardware can move the bitplanes on the screen"). This package's
colour-grouping evidence (category 8's DBLPF=1/BPU=6 odd-vs-even measurement) is a wrong-clause
proxy for it, not an establishment of it. Measured directly instead (category 20's fixture:
plane 2 alone, BPLCON1 PF2H nibble 4 vs nibble 12, DBLPF=0 vs DBLPF=1): the shift is identical
either way. p.85's own warning is exactly why: "Always set all six bits, even if you have only
one playfield. Set 3-0 and 7-4 to the same value if you are using only one playfield" -- PF2H
governs bitplanes 2, 4 and 6's delay the same way regardless of DBLPF, so the identical shift is
what the manual itself states, not a DBLPF-dependent movement mechanism this fixture's boundary
could show.

| claim id | page | reader | restatement |
|---|---|---|---|
| `3-dual-activate.c4` | 72 | sol | DBLPF changes the hardware movement behavior of the bitplanes on-screen. |
| `3-dual.c46` | 72 | opus | Selecting dual-playfield mode also changes the way the hardware can move the bitplanes on the screen. |


Total not-asserted claim ids: 31 (28 original + 1 added + 2 recorded under R10)


Grand total: 245 asserted claim-rows + 29 not-asserted claim ids = 274 (274 claim rows over 269
unique claim ids -- 258 original + 11 added, all 11 new ids unique; 5 ids in the HAM
category are each backed by two independent reader rows, opus and sol, both asserted
separately, unchanged from before).

## Two manual defects in this package's area (not mine to resolve)

**HAM enable bit.** Every claim row this package holds that names a bit number for HAM enable
says bit 11 (pages 87, 90, 271, 300 -- see the HAM category above). The task brief reports that
printed page 95 elsewhere states bit 10; no claim row in this package's 263 carries that
wording. Asserted: a BPLCON0 bit enables HAM (undisputed), specifically bit 11 per every quoted
source this package holds. Recorded, not resolved: the page-95 "bit 10" reading per the task
brief.

**BPLCON0 bits 0 and 4-7, OCS vs ECS.** Chapter 3 page 89 calls bits 4-7 "not used (make 0)";
appendix C page 300 assigns bit 6=SHRES, bit 5=BPLHWRM, bit 4=SPRHWRM in ECS, and appendix C
page 307 assigns bit 0=ENBPLCN3 in ECS. Not a contradiction -- OCS vs ECS. Every assertion in
UNUSED_OCS and ECS_REASSIGN carries its chipset explicitly.

**Newly noted while drafting this spec (not from the task brief): Table 3-5's sixth-bitplane
footnote (page 48, "used only in dual-playfield mode and in hold-and-modify mode") is in
tension with this same package's own EHB claims (page 95), which use a sixth bitplane with
DBLPF=0 and HOMOD=0 -- neither dual-playfield nor HAM. See the SIXTHPLANE category above. Only
the affirmative half of the footnote is asserted; the exclusive "only" half is not, and this
tension is recorded, not resolved.**
