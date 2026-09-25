# Specification: ch7-bplcon2-live-bits-and-ecs

Fixture: `denise`. Subject: BPLCON2's live bits measured one at a time, the
Appendix-A `(E)` mark, KILLEHB and ENBPLCN3.

Written from the quotations in `ch7-bplcon2-live-bits-and-ecs-spec-stage.jsonl`
alone. No RTL has been opened to produce this document; neither has
`ch7-bplcon2-live-bits-and-ecs.jsonl` nor
`STEP3-NOTES-ch7-bplcon2-live-bits-and-ecs.md` -- both are step-3 reading.
Every claim is cited by its `uid` (`<reader>:<id>`), never by the bare `id`,
per BUILD-BRIEF.

## Chipset scope

Chapter 7's body names no chipset of its own; printed p.224 hands the ECS
system-control registers to Appendix C, so Table 7-1 (p.210) and Appendix A's
bit matrix (p.271) are read as the OCS account, and Appendix C (p.307) as the
ECS account. The 18 claims fall into three groups:

- **Undisputed on both chipsets** -- BPLCON2 bits 15, 8 and 7: Table 7-1 and
  Appendix A agree they carry no field, and Appendix C's Genlock Extensions
  table (p.307) never assigns any of the three a name either.
- **Disputed between the OCS pages and Appendix C, resolved by chipset, not by
  one page being wrong** -- BPLCON2 bits 14, 13, 12, 11, 10 and 9. On OCS all
  six are inert-looking (undisputed, measured -- not graded, per repair;
  see P1's repair note). On ECS, Appendix C p.307 assigns each
  a name; per this package's own conflict records, five of those (bits
  14-10: ZDBPSEL2, ZDBPSEL1, ZDBPSEL0, ZDBPEN, ZDCTEN) are stated by the
  classification to be unimplemented in this design and are recorded, not
  asserted. Bit 9 (KILLEHB) is the one exception the classification marks as
  implemented and testable, and it is this package's centerpiece.
- **Outside the bit-matrix conflict entirely** -- the Appendix A/B `(E)` mark
  on the BPLCON2 register-list row, and BPLCON0 bit 0 (ENBPLCN3), which rides
  along in Appendix C p.307's genlock table next to the BPLCON2 columns this
  package owns.

A count check: the file carries 10 claims with a recorded `conflict_resolution`,
not twelve as the operational notes said. The assert-undisputed/record-disputed
split below has been applied to all 10, and every other claim in the file is
accounted for at the bottom of this document.

## Testable propositions

### P1 -- BPLCON2 bit 15 carries no function, on OCS or ECS

> "15-7 Not used (keep at 0)" (printed p.210, Table 7-1)
> "15 HIRES X X" (printed p.271, Appendix A bit matrix, BPLCON2 column)

Testable form (as first written, before repair): writing BPLCON2 bit 15
alone, against a known baseline, produces no change in Denise's display
output, under `ecs=0` and under `ecs=1` alike.

**Repair note** (independent validation,
`work/2026-09-13_ch7_sol_validation_B2.md`, finding 1): this "no change"
reading was withdrawn. "Not used (keep at 0)" (p.210) and "X" (p.271) tell
software what to write; the Appendix-A preamble (p.264) warns that setting
an unused bit "may cause serious future software incompatibility if
[it is] implemented in the future" -- a forward-compatibility warning to
programmers, not a specification that today's hardware must ignore a bit
already written 1. The built package measures this (still reports the
diff count) but no longer grades it as pass/fail. The same reasoning
applies, and the same note is not repeated, at every other "OCS half
asserted" / "undisputed on both chipsets" proposition below (P2-P9's
"no change" halves).

Undisputed on both chipsets: Appendix C's Genlock Extensions table (p.307)
runs from bit 14 down to bit 0 and never reaches bit 15, so nothing in the
manual gives it an ECS function either.

Cited by: `opus:7-appA-bplcon2.c5` (per-bit, undisputed, p.271); `sol:7-video.c18`,
`opus:7-vp.c17` (Table 7-1's "15-7" range, twins, p.210, undisputed sub-claim);
`sol:7-appA-bplcon2.c6` (Appendix A's combined "15-7" row, p.271, undisputed
sub-claim).

### P2 -- BPLCON2 bit 14 carries no function on OCS; its Appendix-C ECS name (ZDBPSEL2) is recorded, not asserted

> "14 BPU2 X X" (printed p.271, BPLCON2 column)

Testable form (OCS half, measured -- not graded; see P1's repair note):
writing BPLCON2 bit 14 alone, against a known baseline, produces no change
in Denise's display output under `ecs=0`.

Recorded, not asserted (ECS half): this claim's own `conflict_resolution`
names Appendix C p.307's assignment of bit 14 as ZDBPSEL2 on ECS, and states
that this design does not implement it. There is nothing to measure, so no
assertion is made about `ecs=1` for this bit.

Cited by: `opus:7-appA-bplcon2.c6`.

### P3 -- BPLCON2 bit 13 carries no function on OCS; its Appendix-C ECS name (ZDBPSEL1) is recorded, not asserted

> "13 BPU1 X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half measured, not graded (see P1's repair note); ECS
half recorded per this claim's own `conflict_resolution` (Appendix C p.307
names bit 13 as ZDBPSEL1 on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c7`.

### P4 -- BPLCON2 bit 12 carries no function on OCS; its Appendix-C ECS name (ZDBPSEL0) is recorded, not asserted

> "12 BPU0 X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half measured, not graded (see P1's repair note); ECS
half recorded per this claim's own `conflict_resolution` (Appendix C p.307
names bit 12 as ZDBPSEL0 on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c8`.

### P5 -- BPLCON2 bit 11 carries no function on OCS; its Appendix-C ECS name (ZDBPEN) is recorded, not asserted

> "11 HOMOD X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half measured, not graded (see P1's repair note); ECS
half recorded per this claim's own `conflict_resolution` (Appendix C p.307
names bit 11 as ZDBPEN on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c9`.

### P6 -- BPLCON2 bit 10 carries no function on OCS; its Appendix-C ECS name (ZDCTEN) is recorded, not asserted

> "10 DBLPF X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half measured, not graded (see P1's repair note); ECS
half recorded per this claim's own `conflict_resolution` (Appendix C p.307
names bit 10 as ZDCTEN on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c10`.

### P7 -- BPLCON2 bit 9 carries no function on OCS; on ECS it is KILLEHB and kills Extra Half-Brite

> "09 COLOR X X" (printed p.271, Appendix A bit matrix, BPLCON2 column)
> "09 KILLEHB Kill halfbrite" (printed p.307, Appendix C Genlock Extensions)

Testable form:

- OCS half (measured, not graded -- see P1's repair note): writing BPLCON2
  bit 9 alone, against a known baseline, produces no change in Denise's
  display output under `ecs=0`.
- ECS half (asserted -- the one Appendix-C BPLCON2 bit this design
  implements, per the classification): with Extra Half-Brite active in the
  baseline stimulus (see Precondition, below) and `ecs=1`, setting BPLCON2
  bit 9 (KILLEHB) removes the Extra-Half-Brite effect from Denise's display
  output; clearing it restores that effect.

**Repair note** (finding 5): the ECS half was originally graded only
`diff > 0` between a baseline and a bit-set sweep, which passes for
blanking, inversion or any other unrelated perturbation of bit 9 -- none of
which is "kill halfbrite." The built package now grades an exact colour
pair instead: killehb=0 reads Extra-Half-Brite's own folded-and-halved
COLOR00; killehb=1 reads a distinct, categorically different value (on this
package's rig, a colour-table entry the automatic half-brite fold no longer
redirects through COLOR00), demonstrating the fold itself stopped rather
than merely that "something" changed.

Both halves resolve the page-210/271-versus-307 conflict by chipset, not by
one page being wrong: OCS and ECS are each right about their own chipset.

Cited by: `opus:7-appA-bplcon2.c11` (OCS half, p.271); `sol:7-appC-bplcon2.c5`,
`opus:7-appC-bplcon2.c7` (ECS half, p.307, twins).

Co-cited, denotation only: `sol:7-appA-bplcon2.c4` (p.271, Appendix A) and
`sol:7-appB-bplcon2.c5` (p.291, Appendix B) each carry the `(E)` mark on the
BPLCON2 register-list row -- stating only that BPLCON2's function set
differs under ECS, and pointing to Appendix C p.307 for the detail. KILLEHB
is the one concrete, testable instance of that difference within this
package's claims, so both are folded in here rather than given a separate
proposition.

### P8 -- BPLCON2 bit 8 carries no function, on OCS or ECS

> "08 GAUD X X" (printed p.271, Appendix A bit matrix, BPLCON2 column)

Same shape as P1: undisputed on both chipsets (Appendix C's table never
reaches bit 8 either); measured, not graded (see P1's repair note). Also
corroborated by the Table 7-1 / Appendix A range claims cited under P1
(`sol:7-video.c18`, `opus:7-vp.c17`, `sol:7-appA-bplcon2.c6`), whose
undisputed sub-claim spans bits 15, 8 and 7 together.

Cited by: `opus:7-appA-bplcon2.c12`.

### P9 -- BPLCON2 bit 7 carries no function, on OCS or ECS

> "07 X PF2H3 X" (printed p.271, Appendix A bit matrix -- the BPLCON2 cell is
> the third column)

Same shape as P1: undisputed on both chipsets; measured, not graded (see
P1's repair note). Also corroborated by the Table 7-1 / Appendix A range
claims cited under P1, per the same undisputed-sub-claim span noted under
P8.

Cited by: `opus:7-appA-bplcon2.c13`.

### P10 -- BPLCON2 governs video priority: PF2PRI (bit 6) is live

> "You control the video priorities by using the bits in BPLCON2 (for
> \"bitplane control register number 2\") as shown in Table 7-1." (printed
> p.210)

Added by repair (independent validation,
`work/2026-09-13_ch7_sol_validation_B2.md`, finding 6). Originally read as
framing only -- the manual's own licence for using Denise's pixel output as
the "is this bit live" observable throughout this package -- and recorded
rather than asserted, with the licence exercised only by
`CONTROL-FIXTURE-ALIVE`, a check that never writes BPLCON2. The
classification's own evidence for this claim already names a concrete,
testable experiment (writing word address $104/BPLCON2 with bit 6 clear
paints a dual-playfield line in COLOR01, and with bit 6 set paints it in
COLOR09), so the claim is testable, not merely framing.

Testable form: in a dual-playfield configuration with both playfields fully
covered, PF2PRI=0 selects playfield 1's colour and PF2PRI=1 selects
playfield 2's colour, at the same pixel.

Cited by: `opus:7-vp.c16`.

### P11 -- ENBPLCN3 (BPLCON0 bit 0) gates BRDRBLNK

> "00 ENBPLCN3 ... Enable new BLPCON3 register." (printed p.307, Appendix C
> Genlock Extensions)
> "05 BRDRBLNK Border blank" (same table, printed p.307)

Added by repair (independent validation,
`work/2026-09-13_ch7_sol_validation_B2.md`, finding 7). Originally declined
for lack of a quoted BPLCON3 field to observe ENBPLCN3's enabling effect
through -- but p.307, the same page this package already cites for
KILLEHB, defines BRDRBLNK on the same table, so a BPLCON3 field is in fact
available without leaving this package's own cited page.

Testable form: with BPLCON3's BRDRBLNK bit fixed at 1 and a border-region
pixel sampled (outside the display window), ENBPLCN3=0 shows the border's
own colour and ENBPLCN3=1 blanks it to black.

Cited by: `opus:7-appC-bplcon2.c9`.

### P12 -- Appendix C's ECS register map places BPLCON2 at word offset $104

> "104 BPLCON2 chg   WA" (printed p.298, Appendix C ECS register map)

Testable form: PF2PRI (bit 6, P10) switches the painted playfield when
written to word address $104; the identical bit pattern written to a
neighbouring Denise address does not. This package's own Non-goals leave
BPLCON2's address, as a subject in its own right, to
`hrm/ch3-register-map-denise` -- this proposition only shows that $104,
and not a neighbour, is where this package's own PF2PRI effect lives.

Cited by: `sol:7-appC-map-bplcon2.c1`.

### P13 -- Appendix C's ECS register map marks BPLCON2 changed from OCS

> "104 BPLCON2 chg   WA" (printed p.298, Appendix C ECS register map)

The claim is a comparison against OCS. BPLCON2-BIT9-KILLEHB-LIVE-ECS (P7's
ECS half) only runs at `ecs=1` (`Harness h(true)`); it shows KILLEHB live
under ECS but never shows the OCS side, so a design where KILLEHB already
lived under OCS too would still pass that assertion. P7's OCS half is
separately declined at BPLCON2-BIT9-OCS-NOT-GRADED, for the unrelated
"keep at 0" reasoning (p.210/264/271), not for this comparison. The id is
withdrawn from BPLCON2-BIT9-KILLEHB-LIVE-ECS and recorded instead, on
BPLCON2-BIT9-CHG-NOT-COMPARED: whether killehb's ECS behaviour is new
relative to OCS is not established by anything in this file.

Cited by: `sol:7-appC-map-bplcon2.c2` (recorded, not asserted).

### P14 -- ECS BPLCON2's function retains video-priority control and adds the p.307 genlock fields

> "104 BPLCON2 chg   WA" [restored function text, same page:] "Bitplane
> control (video priority control)" (printed p.298, Appendix C ECS register
> map)

A bundled claim: its own text states both a video-priority half and a
genlock-fields half, and only one is reachable at this boundary.

- **Video-priority half**: under `ecs=1`, BPLCON2 still selects between
  playfields by PF2PRI (bit 6) -- independently established by P10's own
  testable form (VIDEO-PRIORITY-PF2PRI-LIVE), under P10's own id, not this
  one.
- **Genlock-fields half**: of the six p.307 fields this half points to,
  KILLEHB (bit 9) is P7's own centerpiece and is asserted there, under its
  own id; the other five (ZDBPSEL2, ZDBPSEL1, ZDBPSEL0, ZDBPEN, ZDCTEN,
  bits 14-10) have no consumer and no output port in this design (absent)
  and are already recorded per bit in this document (P2-P6). absent_signal:
  "a genlock/border output driven from bplcon2[14:9]". reason_kind: absent.

Per the rule that an id whose claim bundles an untested clause is withdrawn
whole: `sol:7-appC-map-bplcon2.c5` stays off every graded line and is
recorded instead, co-cited on each of the five BPLCON2-BITn-ECS-NOT-
ASSERTED lines (bits 14-10). It does not go on VIDEO-PRIORITY-PF2PRI-LIVE:
that assertion establishes the video-priority half in full, but not the
genlock-fields half this claim also states, so the whole id is declined,
not kept with a caveat. This is what `check_uid_placement.py` checks for a
claim it finds on both a graded and a declined line, and which side of
that pair loses the id.

Cited by: `sol:7-appC-map-bplcon2.c5` (recorded, not asserted).

## Not turned into assertions

- The ECS-side reading of BPLCON2 bits 14, 13, 12, 11 and 10 (Appendix C
  p.307's ZDBPSEL2, ZDBPSEL1, ZDBPSEL0, ZDBPEN and ZDCTEN) -- see P2-P6
  above. Each of these claims is otherwise turned into an assertion (its OCS
  half); only this remainder is recorded, per each claim's own
  `conflict_resolution`. P14's genlock-fields half is co-cited on the same
  five recorded lines.
- P13's OCS-vs-ECS comparison (BPLCON2-BIT9-KILLEHB-LIVE-ECS runs at
  `ecs=1` only and never shows the OCS side) -- recorded on
  BPLCON2-BIT9-CHG-NOT-COMPARED, not co-cited on the KILLEHB assertion.

## Precondition to assert

Two preconditions, neither sourced to a manual quotation -- both are the
fixture-delivers-a-known-word rule (BUILD-BRIEF) applied to this fixture and
these claims:

1. Before any of P1-P9 is trusted, the fixture must be shown to deliver
   distinct, known pixel/priority values, in order, at the cycle they were
   driven -- the same discipline that caught chapter 6's dead data path
   (every `data_out` reading zero because the DMA grant was sampled in the
   same `eval()` as the request). A `denise` fixture whose output is
   silently stuck would report every one of P1-P9's "no change" halves as a
   false pass rather than a true one.
2. Before P7's ECS half is trusted, the baseline stimulus must be shown to
   actually put Denise into an Extra-Half-Brite-active state under `ecs=1`
   with KILLEHB=0 -- otherwise a green on "KILLEHB removes the EHB effect"
   is indistinguishable from "there was no EHB effect to remove in the first
   place." This package's claims do not themselves say how EHB is enabled
   (that combination of other control bits is out of this package's
   boundary); it must be established once the RTL is open, and asserted as
   its own precondition before P7's ECS half is graded.

## Control case

P1, P8 and P9 (bits 15, 8, 7) are undisputed inert-looking on both chipsets
and are measured on the identical basis as P7's ECS half: same fixture,
same "does the display output change" observable, same per-bit stimulus.
They remain this package's units-error control against a broken
measurement (stuck output, wrong cycle sampled) reading a dead bit as
live -- but after the repair (finding 1, P1's repair note above) this is
demonstrated by `CONTROL-FIXTURE-ALIVE` directly, not by grading P1/P8/P9's
own "no change" result: `CONTROL-FIXTURE-ALIVE` shows two known, distinct
values arrive in order, on the same rig, without relying on a "no change"
reading to prove the fixture is not stuck.

## Claims accounted for

All 21 claims in `ch7-bplcon2-live-bits-and-ecs-spec-stage.jsonl`:

- P1: `opus:7-appA-bplcon2.c5`, `sol:7-video.c18`, `opus:7-vp.c17`,
  `sol:7-appA-bplcon2.c6` (4)
- P2: `opus:7-appA-bplcon2.c6` (1)
- P3: `opus:7-appA-bplcon2.c7` (1)
- P4: `opus:7-appA-bplcon2.c8` (1)
- P5: `opus:7-appA-bplcon2.c9` (1)
- P6: `opus:7-appA-bplcon2.c10` (1)
- P7: `opus:7-appA-bplcon2.c11`, `sol:7-appC-bplcon2.c5`,
  `opus:7-appC-bplcon2.c7`, plus co-cited `sol:7-appA-bplcon2.c4`,
  `sol:7-appB-bplcon2.c5` (5)
- P8: `opus:7-appA-bplcon2.c12` (1)
- P9: `opus:7-appA-bplcon2.c13` (1)
- P10 (added by repair): `opus:7-vp.c16` (1)
- P11 (added by repair): `opus:7-appC-bplcon2.c9` (1)
- P12: `sol:7-appC-map-bplcon2.c1` (1)
- P13 (recorded, not asserted): `sol:7-appC-map-bplcon2.c2` (1)
- P14 (asserted and recorded, one half each): `sol:7-appC-map-bplcon2.c5` (1)

4 + 1 + 1 + 1 + 1 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 1 + 1 = 21.
