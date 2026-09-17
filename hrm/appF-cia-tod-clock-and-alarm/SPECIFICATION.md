# Specification — `appF-cia-tod-clock-and-alarm`

Package: `hrm/appF-cia-tod-clock-and-alarm`
Fixture: **one** top, `ciaa`, compiled with its CIA submodules (`cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v`) — modeled directly on
`hrm/ch1-cia-timers`' single-top-per-chip pattern. Only `ciaa` is built: every
claim in this package's inventory is about the TOD/alarm sub-function of a
CIA, and none of the 61 claims singles out `ciab` — the mechanism is identical
in both physical chips (`cia_timerd` is instantiated the same way in
`ciab.v`), so one fixture suffices, the way `hrm/ch8-ciaa-port-direction`
tests only `ciaa` for a claim that does not name a chip.
Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
to `classify.package == "appF-cia-tod-clock-and-alarm"` (61 claims, read by
both `sol` and `opus`, all `status: testable`, all `source_kind: "text"`).
Subject: Appendix F's "Time of Day Clock" section, printed page 344 in full
(the TOD counter, the programmable alarm, the write-stop/write-start
sequencing rule, the read-latch rule, and the bit-name table for registers
$8/$9/$A), plus the CRB bit-7 ALARM specification on page 349 (prose, in
"CONTROL REGISTER B:") and page 350 (the "BITMAP OF REGISTER CRB" table's
ALARM column) — the two pages that name the control bit p.344's own table
only shows as a bare legend (`CRB7 = 0` / `CRB7 = 1 ALARM`).

## Grading context

`hrm/ch1-cia-timers`' own specification (PROP-TIMER's "Not graded" paragraph)
explicitly names "the TOD/alarm sub-function" as deliberately excluded from
that package's scope, and no other package in this suite exercises
`cia_timerd` at all. This package is the first, and currently only, package
that reaches `cia_timerd` — every one of its 61 claims is new ground, not a
re-grading of a fact another accepted package already covers.

3 of the 61 claims (`sol:F-crb.c8`, `sol:F-crb.c9`, `opus:F-crb.c11`) carry a
`classify.note`. Unlike the "already graded elsewhere, record() it" notes
that dominate this appendix's other packages, this note does not point
anywhere: it states that a grep of every `hrm/**/test_*.cpp` for `PBON`,
`OUTMODE`, `INMODE` and `ALARM` finds nothing, and that
`hrm/ch1-cia-timers` grades only that each CIA owns *a* usable timer
(`CIAA-TIMER-PROVIDED`/`CIAB-TIMER-PROVIDED`) — not any CRA/CRB control bit.
So all three are graded here, the same as the other 58. **No claim in this
package is declined as already covered by an accepted package.**

## A caution about the bare id, confirmed while reading these 61 claims

The two readers numbered claims independently against the same source
paragraphs, and a matching bare id between `sol` and `opus` does **not** by
itself mean the same proposition. The clearest case in this package: `sol`
split the two 8-bit "event byte" rows of p.344's bit-name table into one
claim **per printed bit** — `sol:F-tod.c17`..`sol:F-tod.c40`, 24 claims,
3 registers × 8 bits — while `opus` wrote one claim **per whole row** —
`opus:F-tod.c16`..`opus:F-tod.c18`, 3 claims. `sol:F-tod.c17` ("bit 7 of
register 8 is named E7") and `opus:F-tod.c17` ("register 9 is Event 8-15,
holding bits E15..E8") are unrelated propositions that merely share a bare
id. A second case: `sol:F-tod.c9`'s quote is the full "specific sequence...
must be followed" paragraph while `opus:F-tod.c9`'s quote is the single
clause "TOD is automatically stopped whenever a write to the register
occurs" — `sol:F-tod.c9` pairs with `opus:F-tod.c8`, not `opus:F-tod.c9`.
Every pairing used below was verified against both claims' `extract.quote`
and `classify.restatement` text, not assumed from the id matching.

## Corpus check

Page 344 is not listed in `corpus-defects.md` and the rendered page
(`figures/p344.png`) was read directly: the bit-name table (`8 LSB Event E7
E6 E5 E4 E3 E2 E1 E0`, `9 Event 8-15 E15 E14 E13 E12 E11 E10 E9 E8`, `A MSB
Event E23 E22 E21 E20 E19 E18 E17 E16`) and the `WRITE / CRB7 = 0 / CRB7 = 1
ALARM` legend both match the extracted claims exactly; the text layer's `El`,
`Ell`, `El 9`, `El 7` are `pdftotext`'s own mangling of `E1`, `E11`, `E19`,
`E17` and not a defect in the page, confirmed against the render and already
correctly recognised as such in `opus`'s restatements for `F-tod.c16`-`c18`.

Page 349's "CONTROL REGISTER B:" bit-7 line is one of the pages
`corpus-defects.md` names for an unrelated defect (stray `*`/`»` glyphs on
its CRB6/CRB5 INMODE and START/OUTMODE rows) but the ALARM line itself
(`7 ALARM 1 = writing to TOD registers sets Alarm / 0 = writing to TOD
registers sets TOD clock. Reading TOD registers always reads TOD clock,
regardless of the state of the Alarm bit.`) is clean in both the text layer
and the render (`figures/p349.png`), verified directly.

Page 350 **is** listed in `corpus-defects.md`: "the CRB bitmap is shredded
across lines... The columns cannot be recovered from the text at all."
`sol:F-crb.c9`'s quote is exactly that shredded text. The render
(`figures/p350.png`) was read directly and recovers the table: `NAME |
ALARM | INMODE | LOAD | RUNMODE | OUTMODE | PBON | START`, row `F CRB` reads
`0=TOD` / `1=Alarm` under the ALARM column — matching `sol:F-crb.c9`'s
restatement and p.349's prose. The bit-7 **position** is not independently
established by this table (it lists named fields, not bit numbers); that
comes only from p.349's numbered list (`7 ALARM`). This document uses the
render's recovered field values, not the shredded text-layer quote, as
`corpus-defects.md` instructs.

## Claim inventory and proposition groups (61 claims → 43 assertions, 0 declined)

Every claim below is graded by an `expect()` in `test_ciaa.cpp`; none is
`record()`-declined. Where `sol` and `opus` (or two of `sol`'s own claims)
state the identical testable fact, one assertion carries both/all the
relevant uids, per the build brief's "no claim may lose its home" — every
uid appears in exactly one assertion's label.

| # | Assertion | Page(s) | Claim uids | Proposition |
|---|---|---|---|---|
| 1 | `TOD-COUNTER-24BIT` | 344 | `sol:F-tod.c1`, `opus:F-tod.c1` | TOD is a 24-bit binary counter (wraps at 2^24, not wider/narrower). |
| 2 | `TOD-COUNTER-POSEDGE` | 344 | `sol:F-tod.c2`, `opus:F-tod.c2` | Only a positive edge on the tick input increments the counter (not level-sensitive). |
| 3 | `TOD-ALARM-INTERRUPT` | 344 | `sol:F-tod.c4`, `opus:F-tod.c4` | A programmable alarm generates an interrupt when TOD reaches the alarm value. |
| 4 | `TOD-ALARM-ADDRESSING` | 344 | `sol:F-tod.c5`, `opus:F-tod.c5` | The alarm registers are located at the same addresses ($8/$9/$A) as TOD — no separate alarm address exists. |
| 5 | `TOD-ALARM-WRITE-GATE` | 344 | `sol:F-tod.c6`, `opus:F-tod.c6`, `sol:F-tod.c41`, `opus:F-tod.c19` | A control-register bit (CRB7) governs whether a write to $8/$9/$A reaches TOD (CRB7=0) or the alarm (CRB7=1). |
| 6 | `TOD-READ-ALWAYS-TOD` | 344 | `sol:F-tod.c7`, `opus:F-tod.c7`, `sol:F-tod.c8` | The alarm is write-only: a read of $8/$9/$A always returns TOD, regardless of the ALARM bit. |
| 7 | `TOD-SEQUENCE-MATTERS` | 344 | `sol:F-tod.c9`, `opus:F-tod.c8` | Setting/reading TOD requires a specific event sequence — writing out of the documented order produces a different (incorrect) result than writing in it. |
| 8 | `TOD-WRITE-STOPS-CLOCK` | 344 | `sol:F-tod.c10`, `opus:F-tod.c9` | A write to a TOD register (any of $8/$9/$A) automatically stops the clock. |
| 9 | `TOD-WRITE-STARTS-LSB-ONLY` | 344 | `sol:F-tod.c11`, `opus:F-tod.c10` | The clock does not resume after a stop until a write specifically to the LSB event register ($8). |
| 10 | `TOD-WRITE-STARTS-ATOMIC-VALUE` | 344 | `sol:F-tod.c12`, `opus:F-tod.c11` | When the clock resumes, it starts counting from exactly the value written, not a value that drifted between the byte writes. |
| 11 | `TOD-READ-LATCH-CARRY-CONSISTENCY` | 344 | `sol:F-tod.c13`, `opus:F-tod.c12` | Because a carry can occur mid-read, latching keeps all TOD bytes internally consistent across a byte-boundary carry that happens during a read sequence. |
| 12 | `TOD-READ-LATCH-MSB-FREEZES-ALL` | 344 | `sol:F-tod.c14`, `opus:F-tod.c13` | Reading the MSB event register freezes all three TOD bytes for subsequent reads, until the LSB event register is read. |
| 13 | `TOD-READ-LATCH-COUNTS-UNDERNEATH` | 344 | `sol:F-tod.c15`, `opus:F-tod.c14` | The underlying TOD counter keeps counting while the output latch is frozen. |
| 14 | `TOD-READ-ONTHEFLY-RELEASE` | 344 | `sol:F-tod.c16`, `opus:F-tod.c15` | A single register can be read "on the fly" (tracks the live counter), but once an MSB read has occurred, a subsequent MSB read stays frozen until an LSB read releases the latch. |
| 15 | `TOD-BITMAP-REG8-*` (8 assertions, bits 7..0) | 344 | `sol:F-tod.c17`..`sol:F-tod.c24` | Register $8 ("LSB Event") bit *n* carries TOD counter bit *n* (E7..E0), individually, one claim per bit. |
| 16 | `TOD-BITMAP-REG9-*` (8 assertions, bits 7..0) | 344 | `sol:F-tod.c25`..`sol:F-tod.c32` | Register $9 ("Event 8-15") bit *n* carries TOD counter bit *n+8* (E15..E8), individually. |
| 17 | `TOD-BITMAP-REGA-*` (8 assertions, bits 7..0) | 344 | `sol:F-tod.c33`..`sol:F-tod.c40` | Register $A ("MSB Event") bit *n* carries TOD counter bit *n+16* (E23..E16), individually. |
| 18 | `TOD-BITMAP-REG8-ROW` | 344 | `opus:F-tod.c16` | Register $8 as a whole round-trips all 8 LSB-event bits together. |
| 19 | `TOD-BITMAP-REG9-ROW` | 344 | `opus:F-tod.c17` | Register $9 as a whole round-trips all 8 Event-8-15 bits together. |
| 20 | `TOD-BITMAP-REGA-ROW` | 344 | `opus:F-tod.c18` | Register $A as a whole round-trips all 8 MSB-event bits together. |
| 21 | `CRB7-ALARM-SPEC-P349` | 349 | `sol:F-crb.c8`, `opus:F-crb.c11` | The definitive CRB7 spec: 1 routes a TOD-address write to Alarm, 0 routes it to TOD clock; a read of a TOD address always returns the TOD clock regardless of CRB7. |
| 22 | `CRB7-BITMAP-P350` | 350 | `sol:F-crb.c9` | The CRB bitmap table's ALARM field (0=TOD, 1=Alarm) is governed specifically by bit 7 of CRB, not by any of CRB's other named fields (INMODE/LOAD/RUNMODE/OUTMODE/PBON/START). |

Row 15-17 are 24 separate assertions (one per `sol` per-bit claim); rows
18-20 are 3 more. 14 (rows 1-14) + 24 (rows 15-17) + 3 (rows 18-20) + 2
(rows 21-22) = 43 assertions, covering all 61 claims exactly once. A 44th
graded line, `FIXTURE-LIVE`, is infrastructure and carries no claim uid, the
same shape `hrm/ch1-cia-timers` uses.

## Why the per-bit and whole-row bitmap claims are both graded, separately

`sol`'s per-bit evidence for `F-tod.c17`-`c40` says explicitly: "what a
fixture grades is the bit-POSITION binding this row states, not the printed
spelling" — each of the 24 claims is a distinct, narrow proposition ("this
specific bit carries this specific counter bit"), and a defect that flips
one bit without disturbing its neighbors should fail exactly one of them,
not all 8. `opus`'s 3 whole-row claims are a coarser, independent
proposition ("this whole register round-trips"), satisfied by one
conjunctive check per register. Both are real, distinct claims about the
same table and neither is a duplicate of the other — this package grades
both, the same way it keeps `sol:F-tod.c6`/`opus:F-tod.c6` (the general "a
control bit governs access") distinct from `sol:F-tod.c41`/`opus:F-tod.c19`
(the specific "CRB7=0 → TOD, CRB7=1 → ALARM") even though one assertion
(`TOD-ALARM-WRITE-GATE`) grades all four together, since the general claims
add no separately-testable content beyond what the specific ones state.

## Two items flagged for measurement, not adjudication

The task instructions named two RTL facts already measured during
classification, to be confirmed and reported here, not adjudicated:

1. **`cia_timerd.v:75`** stops the TOD count-enable latch only on a write to
   the HIGH byte (`thi`) — the middle byte's participation is present in the
   source only as a comment, `if (thi/* || tme*/)`. P.344 says TOD "is
   automatically stopped whenever a write to the register occurs" (no byte
   qualifier). `TOD-WRITE-STOPS-CLOCK` (assertion 8) tests all three byte
   writes independently, exactly as `sol:F-tod.c10`'s own evidence proposes,
   and is expected to fail for the LOW and MID bytes while passing for the
   HIGH byte — this is the measurement surfacing as a real, informative red,
   not something this package works around.
2. **`cia_timerd.v:105-110`** resets all three alarm bytes to `8'b1111_1111`
   (`24'hFFFFFF`). No claim in this package's 61 states an expected TOD/alarm
   reset value — that table is on p.354, owned by a different appF package —
   so this is reported via a `note()` (MEASURED, graded by nothing) rather
   than graded here, and repeated in this builder's final report.

## On fixture liveness and mutation

Per the build brief: a fixture-liveness proof precedes any data-dependent
grading; every graded line (fixture-liveness included) carries a recorded
stimulus mutation, in the source, in the fixed `// MUTATION <label>
<oracle|stimulus|none> <red|green|impossible> <what changed>` form, where
`<label>` is the leading token of the string passed to `expect()`.

- **Fixture liveness**, once: two distinct, known bytes written to CIAADDRA
  (`RS_DDRA`, $2) — a register no TOD/alarm assertion depends on — must both
  arrive and read back correctly before any other assertion is graded.
- **Controls on the same measurement basis.** Several assertions below are
  expected to legitimately fail (assertion 8, per item 1 above). Assertion 9
  (`TOD-WRITE-STARTS-LSB-ONLY`) uses the identical write/tick/read-back
  mechanism and page paragraph and is expected to pass, which is this
  package's same-measurement-basis control the build brief asks for, rather
  than a separately labeled control duplicating the mechanism.
- **Mutation.** Each assertion's mutation is a stimulus change (never an RTL
  edit, never a change to what an assertion treats as the expected value) —
  withholding a tick, writing to the wrong register, using the wrong CRB7
  state, or comparing against the wrong bit/byte — built as a compile-time
  `-D` flag per the Makefile's `MUT ?=` convention, one flag at a time,
  each a full `rm -rf $(OBJ_DIR)` rebuild. Concrete flag names and measured
  PASS/FAIL deltas are step-2 material, recorded in `test_ciaa.cpp`'s header
  comment once built and run under WSL Verilator, per this suite's rule
  against writing a record before taking the measurement.

## Disclosure — what was already known before this file was drafted

The order of work this pipeline asks for — write the specification from the
quotations alone, commit it, and only then open the RTL — does not fully
hold here, the same situation every earlier appF package built by this
pipeline has disclosed for itself. All 61 claims' `classify.evidence` fields
carry detailed `file:line` RTL citations (address-decoder lines, the
`cia_timerd` register list, `count`/`count_ena`/`latch_ena` line numbers, the
`irq`/`alrm`/`icr[2]` interrupt path, and, for the two flagged items above,
the exact divergence from the page) — all read before this document was
written, because they arrived attached to the claims file this package was
assigned from. The task instructions that assigned this package also quoted
several of these citations directly.

What that pre-exposure did not do: it did not supply expected values.
Every proposition and page citation in the table above, and every "not
graded"/"declined" decision (there are none to decline, see above), comes
from `extract.quote`/`classify.restatement` and the page text/renders read
directly (p.344, p.349, p.350, plus `corpus-defects.md`) — not from the RTL.
The concrete register offsets, byte patterns, tick counts, and mutation
implementations are step-2 material, written only after this file was
committed and `ciaa.v`/`cia_timerd.v`/`cia_int.v` had been read in full by
this builder directly (not re-typed from what was pre-exposed).
