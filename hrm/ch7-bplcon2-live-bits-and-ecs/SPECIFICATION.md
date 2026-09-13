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
  six are inert (undisputed, asserted). On ECS, Appendix C p.307 assigns each
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

Testable form: writing BPLCON2 bit 15 alone, against a known baseline,
produces no change in Denise's display output, under `ecs=0` and under
`ecs=1` alike.

Undisputed on both chipsets: Appendix C's Genlock Extensions table (p.307)
runs from bit 14 down to bit 0 and never reaches bit 15, so nothing in the
manual gives it an ECS function either.

Cited by: `opus:7-appA-bplcon2.c5` (per-bit, undisputed, p.271); `sol:7-video.c18`,
`opus:7-vp.c17` (Table 7-1's "15-7" range, twins, p.210, undisputed sub-claim);
`sol:7-appA-bplcon2.c6` (Appendix A's combined "15-7" row, p.271, undisputed
sub-claim).

### P2 -- BPLCON2 bit 14 carries no function on OCS; its Appendix-C ECS name (ZDBPSEL2) is recorded, not asserted

> "14 BPU2 X X" (printed p.271, BPLCON2 column)

Testable form (OCS half, asserted): writing BPLCON2 bit 14 alone, against a
known baseline, produces no change in Denise's display output under `ecs=0`.

Recorded, not asserted (ECS half): this claim's own `conflict_resolution`
names Appendix C p.307's assignment of bit 14 as ZDBPSEL2 on ECS, and states
that this design does not implement it. There is nothing to measure, so no
assertion is made about `ecs=1` for this bit.

Cited by: `opus:7-appA-bplcon2.c6`.

### P3 -- BPLCON2 bit 13 carries no function on OCS; its Appendix-C ECS name (ZDBPSEL1) is recorded, not asserted

> "13 BPU1 X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half asserted (no change under `ecs=0`); ECS half
recorded per this claim's own `conflict_resolution` (Appendix C p.307 names
bit 13 as ZDBPSEL1 on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c7`.

### P4 -- BPLCON2 bit 12 carries no function on OCS; its Appendix-C ECS name (ZDBPSEL0) is recorded, not asserted

> "12 BPU0 X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half asserted (no change under `ecs=0`); ECS half
recorded per this claim's own `conflict_resolution` (Appendix C p.307 names
bit 12 as ZDBPSEL0 on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c8`.

### P5 -- BPLCON2 bit 11 carries no function on OCS; its Appendix-C ECS name (ZDBPEN) is recorded, not asserted

> "11 HOMOD X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half asserted (no change under `ecs=0`); ECS half
recorded per this claim's own `conflict_resolution` (Appendix C p.307 names
bit 11 as ZDBPEN on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c9`.

### P6 -- BPLCON2 bit 10 carries no function on OCS; its Appendix-C ECS name (ZDCTEN) is recorded, not asserted

> "10 DBLPF X X" (printed p.271, BPLCON2 column)

Same shape as P2: OCS half asserted (no change under `ecs=0`); ECS half
recorded per this claim's own `conflict_resolution` (Appendix C p.307 names
bit 10 as ZDCTEN on ECS, unimplemented in this design).

Cited by: `opus:7-appA-bplcon2.c10`.

### P7 -- BPLCON2 bit 9 carries no function on OCS; on ECS it is KILLEHB and kills Extra Half-Brite

> "09 COLOR X X" (printed p.271, Appendix A bit matrix, BPLCON2 column)
> "09 KILLEHB Kill halfbrite" (printed p.307, Appendix C Genlock Extensions)

Testable form:

- OCS half (asserted): writing BPLCON2 bit 9 alone, against a known
  baseline, produces no change in Denise's display output under `ecs=0`.
- ECS half (asserted -- the one Appendix-C BPLCON2 bit this design
  implements, per the classification): with Extra Half-Brite active in the
  baseline stimulus (see Precondition, below) and `ecs=1`, setting BPLCON2
  bit 9 (KILLEHB) removes the Extra-Half-Brite effect from Denise's display
  output; clearing it restores that effect.

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
reaches bit 8 either). Also corroborated by the Table 7-1 / Appendix A range
claims cited under P1 (`sol:7-video.c18`, `opus:7-vp.c17`,
`sol:7-appA-bplcon2.c6`), whose undisputed sub-claim spans bits 15, 8 and 7
together.

Cited by: `opus:7-appA-bplcon2.c12`.

### P9 -- BPLCON2 bit 7 carries no function, on OCS or ECS

> "07 X PF2H3 X" (printed p.271, Appendix A bit matrix -- the BPLCON2 cell is
> the third column)

Same shape as P1: undisputed on both chipsets. Also corroborated by the
Table 7-1 / Appendix A range claims cited under P1, per the same
undisputed-sub-claim span noted under P8.

Cited by: `opus:7-appA-bplcon2.c13`.

## Not turned into assertions

- `opus:7-vp.c16` (p.210): "You control the video priorities by using the
  bits in BPLCON2 (for \"bitplane control register number 2\") as shown in
  Table 7-1." This is framing, not a proposition of its own: it is the
  manual's own statement that BPLCON2 governs a directly observable display
  characteristic, which is what licenses using Denise's pixel/priority
  output as the "is this bit live" observable for P1-P9 and the KILLEHB
  test. Recorded as the methodological basis for the package rather than
  asserted as a separate claim.
- `opus:7-appC-bplcon2.c9` (p.307): "00 ENBPLCN3 ... Enable new BLPCON3
  register" (a BPLCON0 bit). This claim's own `scope_note` says it stands in
  this package only because it shares Appendix C p.307's genlock table with
  the BPLCON2 columns this package owns. Its stated behaviour --  enabling
  another register -- can only be observed by exercising that other
  register's own fields, and no BPLCON3 field is quoted anywhere in this
  package's claims file; that belongs to a package built against BPLCON3's
  own claims. Recorded, not asserted, here.
- The ECS-side reading of BPLCON2 bits 14, 13, 12, 11 and 10 (Appendix C
  p.307's ZDBPSEL2, ZDBPSEL1, ZDBPSEL0, ZDBPEN and ZDCTEN) -- see P2-P6
  above. Each of these claims is otherwise turned into an assertion (its OCS
  half); only this remainder is recorded, per each claim's own
  `conflict_resolution`.

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

P1, P8 and P9 (bits 15, 8, 7) are undisputed dead on both chipsets and are
measured on the identical basis as P7's ECS half: same fixture, same
"does the display output change" observable, same per-bit mutation. They are
this package's control cases against the units-error objection -- if the
measurement itself were broken (stuck output, wrong cycle sampled), a dead
bit would falsely read as live exactly the way a real live bit would, and
P1/P8/P9 passing establishes that the measurement correctly reports "no
change" when no change is the true condition.

## Claims accounted for

All 18 claims in `ch7-bplcon2-live-bits-and-ecs-spec-stage.jsonl`:

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
- Not turned into assertions: `opus:7-vp.c16`, `opus:7-appC-bplcon2.c9` (2)

4 + 1 + 1 + 1 + 1 + 1 + 5 + 1 + 1 + 2 = 18.
