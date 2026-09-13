# Specification: ch7-beamcon0-ecs-control-bits

Fixture: `agnus_beamcounter`. Subject: BEAMCON0's ECS control bits --
VARVBEN, LOLDIS, and the sync polarity bits (CSYTRUE, VSYTRUE, HSYTRUE) --
plus the other bit-table rows and identity facts the same pages assign to
this package.

Written from the quotations in `ch7-beamcon0-ecs-control-bits-spec-stage.jsonl`
alone. No RTL has been opened to produce this document; neither has
`ch7-beamcon0-ecs-control-bits.jsonl` nor
`STEP3-NOTES-ch7-beamcon0-ecs-control-bits.md` -- both are step-3 reading.
Every claim is cited by its `uid` (`<reader>:<id>`), never by the bare `id`,
per BUILD-BRIEF.

The claims file carries 42 entries: 25 from reader `sol`, 17 from reader
`opus`. Nine of the ten BEAMCON0 bit-table rows this package asserts were
independently extracted by both readers as twins of the same printed row;
this specification treats each such pair as one proposition, cited to both
uids, rather than as two.

## Note on step-1 exposure

This claims file's `classify.note` fields -- though this file has had port
and signal fields removed for step 1, per the task sheet -- still name three
RTL identifiers in passing: `_csync`, `_vsync` and `_hsync` (in the notes on
`sol:7-appC-beamcon0.c15/c16/c17` and their `opus` twins, explaining why an
earlier package's discard of those bits is being overruled), and a file:line
pointer, `agnus_beamcounter.v:123`, contrasted with `:110` for VPOSR (in the
note on `sol:7-appC-map-beamcon0.c2`, explaining the NEW-vs-CHANGED register
distinction). These were read because they sit inside the one file this
order names for step 1, not sought out.

No testable form below rests on them. P11-P13's testable form is stated
from the manual's bit-table quotes alone (bit N controls the polarity of
sync signal S); P1's and P3's identity facts are stated from the register-map
and register-detail quotes alone. Whether `_csync`, `_vsync`, `_hsync` are
in fact this module's port names, and what the NEW/CHANGED distinction
resolves to, is left for step 3 to confirm against the real port list, not
assumed here.

## Chipset scope

Every claim in this file is chipset-scoped to ECS: BEAMCON0 does not exist
before ECS (it is marked "new" on the Appendix C map, p.298, and "(E)" in
Appendix A, p.266), so there is no OCS half to any proposition here, unlike
a register that exists on both chipsets with different bit meanings. The
whole package is read as the ECS account.

## Testable propositions

### P1 -- BEAMCON0 occupies custom-chip offset $1DC

> "BEAMCON0   1DC      W      A( E ) Beam counter control register (SHRES,PAL)"
> (p.266, Appendix A)

> "1DC BEAMCONO new" (p.298, Appendix C ECS map). The corpus's OCR renders
> the digit 0 as a capital O, and this row's own R/W, CHIP and FUNCTION
> cells were carried onto a neighbouring line by a column-scramble defect in
> this scan; both readers' crossreads verified the row placement against the
> printed page image, and the restored row is "1DC BEAMCON0 new   W   A
> Beam counter control register (SHRES,UHRES,PAL)".

Testable form: a write addressed to custom-chip offset $1DC is the one that
reaches BEAMCON0, distinguishable from writes to neighbouring offsets.
Whether the `agnus_beamcounter` fixture exposes a real address bus for this
check, or only a pre-decoded per-register write strobe (address decoding
done by a different module -- this claim's own `covered_by` field points to
`hrm/ch3-register-map-agnus-beamcounter`'s `BEAMCON0-ADDRESS-1DC` on the
same fixture), is not knowable before the port list is open. If the fixture
carries the address bus, this is asserted here in step 3; if it only carries
a pre-decoded strobe, this reduces to a wiring fact recorded here rather
than a fresh assertion, and the address itself stays that other package's
job, per BUILD-BRIEF's "wider module boundary" condition applied to one
claim rather than to the whole package. Decided once RTL is open, not here.

Cited by: `sol:7-appA-beamcon0.c1` (p.266), `sol:7-appC-map-beamcon0.c1`
(p.298), `opus:7-appC298.c5` (p.298, same row, independently classified).

### P2 -- BEAMCON0 is write-only

> "BEAMCON0   1DC      W      A( E ) Beam counter control register (SHRES,PAL)"
> (p.266) -- the "W" field
> "1DC BEAMCONO new" (p.298, restored row carries "W" for R/W, same
> scan-defect note as P1)
> "BEAMCON0  W   A     Beam counter control register" (p.305) -- the "W"
> field of this row; the row's "A" (chip) and its trailing function-name
> text are recorded under P3, not asserted here, per this claim's own note
> that direction, chip and function are three separate propositions bundled
> in one quotation.

Testable form: reading back through BEAMCON0's register interface does not
return the pattern last written to its control bits. The precise observable
(a decoded read that returns a fixed/undefined value, versus no read decode
existing for this address at all) depends on how this fixture models the
register bus, and is pinned down once ports are known.

Cited by: `sol:7-appA-beamcon0.c2` (p.266), `sol:7-appC-map-beamcon0.c3`
(p.298), `sol:7-appC-beamcon0.c2` (p.305, direction sub-fact),
`opus:7-appC-beamcon0.c2` (p.305, twin).

### P3 -- Recorded, not independently graded: chip location, the (E)/new-register marking, and the manual's own SHRES/UHRES inconsistency

None of the following facts admit a stimulus/response experiment at a
single-module fixture's boundary the way a control bit does; they are
recorded as context rather than asserted.

- "BEAMCON0 is implemented in (enhanced) Agnus" -- p.266
  (`sol:7-appA-beamcon0.c3`), p.298 (`sol:7-appC-map-beamcon0.c4`), and
  p.305's framing sentence "A new register in the enhanced Agnus, BEAMCON0,
  provides a programmable signal generator" (`sol:7-appC-beamcon0.c1`,
  `opus:7-appC-beamcon0.c1`). This last pair is pure framing: it introduces
  the register and licenses reading its bits as "a programmable signal
  generator's controls", which is the methodological basis for P4-P13
  rather than a proposition of its own. (The p.305 row's own "A" chip field,
  bundled with direction and function in one quotation, is the same fact
  again -- counted once, under P2 above, per that claim's own split note.)
- "BEAMCON0 carries the Appendix-A `(E)` mark" -- p.266
  (`sol:7-appA-beamcon0.c4`): denotes that the register's function differs
  under ECS, which every bit-level proposition in this file already
  exercises concretely; not an independent behaviour of its own.
- "The ECS map marks BEAMCON0 `new` rather than `changed`" -- p.298
  (`sol:7-appC-map-beamcon0.c2`). Whether NEW vs CHANGED has an observable
  difference at all (for instance, a NEW register having nothing
  corresponding to it outside ECS, where a CHANGED one would still respond
  differently) turns on whether this fixture exposes any chipset-mode
  selector at all -- undecided before the port list is open, so recorded
  rather than asserted here.
- The manual's own inconsistency in BEAMCON0's function-list parenthetical:
  p.266 and p.293 give "(SHRES,PAL)"; p.298 gives "(SHRES,UHRES,PAL)"
  (`opus:7-appA-beam.c13`, which also restates the address/direction/chip/
  mark facts `sol` split into `c1`-`c4` above -- a twin corroboration, not a
  new fact). Per this claim's own `conflict_resolution`, the dispute never
  reaches an assertion because neither SHRES nor UHRES exists as a bit or a
  port anywhere in this design; the PAL half of the same sentence is
  asserted at P10, and the SHRES/UHRES halves are recorded under "Not
  turned into assertions" below.

Cited by: `sol:7-appA-beamcon0.c3`, `sol:7-appA-beamcon0.c4`,
`sol:7-appC-map-beamcon0.c2`, `sol:7-appC-map-beamcon0.c4`,
`sol:7-appC-beamcon0.c1`, `opus:7-appC-beamcon0.c1`, `opus:7-appA-beam.c13`.

### P4 -- BEAMCON0 bit 14, HARDDIS, disables the hardwired vertical/horizontal blanking hard stops

> "14    HARDDIS    Disable hardwired vertical/horizontal blank" (p.305, bit
> table)
> "Other modes, such as VGA (31 KHz programmable mode) disable the hard
> stops on display sync and blank." (p.305, prose)

Testable form: setting bit 14 (HARDDIS) removes the hard-wired blanking
stop on the vertical/horizontal timing, letting the programmable timing
take over instead; against a baseline with the bit clear, where the
hard-wired stop still applies.

Not asserted: "Do not write to this register." -- the sentence immediately
following the prose quote in `opus:7-appC-beamcon0.c20`'s extract. A
caution to the programmer, not a proposition about chip behaviour.

Cited by: `sol:7-appC-beamcon0.c3`, `opus:7-appC-beamcon0.c3` (bit-table
twins, p.305); `sol:7-appC-beamcon0.c19`, `opus:7-appC-beamcon0.c20` (prose
twins, p.305).

### P5 -- BEAMCON0 bit 12, VARVBEN, uses VBSTRT/VBSTOP and disables the hard window stop

> "12    VARVBEN    Use VBSTRT/STOP disable hard window stop" (p.305)

Testable form: setting bit 12 (VARVBEN) switches the vertical-blank
window's stop point from a hard-wired default to the value programmed in
VBSTRT/VBSTOP, disabling the hard window stop; against a baseline with the
bit clear.

This is, per this claim's own note, the new assertion this package adds for
this fixture: `hrm/ch3-beam-counters-and-blanking` asserts six other
BEAMCON0 bits and discards five more without ever reaching bit 12;
`hrm/ch3-register-map-agnus-beamcounter` uses VARVBEN only as a gate to
reach VBSTOP, asserting nothing about the bit itself. Both halves of this
printed row are measurable here and neither is claimed anywhere else in the
suite today.

Cited by: `sol:7-appC-beamcon0.c5`, `opus:7-appC-beamcon0.c5`.

### P6 -- BEAMCON0 bit 11, LOLDIS, disables the long-line/short-line toggle

> "11    LOLDIS     Disable long line/short line toggle" (p.305)

Testable form: setting bit 11 (LOLDIS) stops the line-length toggling
between long and short lines that this design otherwise performs; against
a baseline with the bit clear, where the toggle is observed.

LOLDIS's stated effect on the STRLONG register (p.309) is a separate
sentence about a different register, not asserted here -- see "Not turned
into assertions".

Cited by: `sol:7-appC-beamcon0.c6`, `opus:7-appC-beamcon0.c6`.

### P7 -- BEAMCON0 bit 9, VARVSYEN, enables variable vertical sync

> "9    VARVSYEN   Variable vertical sync enable" (p.305)

Testable form: setting bit 9 (VARVSYEN) switches vertical sync generation
from its hard-wired timing to the variable/programmable timing path;
against a baseline with the bit clear.

Cited by: `sol:7-appC-beamcon0.c8`, `opus:7-appC-beamcon0.c8`.

### P8 -- BEAMCON0 bit 8, VARHSYEN, enables variable horizontal sync

> "8    VARHSYEN   Variable horizontal sync enable" (p.305)

Testable form: setting bit 8 (VARHSYEN) switches horizontal sync generation
from its hard-wired timing to the variable/programmable timing path;
against a baseline with the bit clear.

Cited by: `sol:7-appC-beamcon0.c9`, `opus:7-appC-beamcon0.c9`.

### P9 -- BEAMCON0 bit 7, VARBEAMEN, enables the variable beam-counter comparator

> "7    VARBEAMEN  Variable beam counter comparator enable" (p.305)

Testable form: setting bit 7 (VARBEAMEN) enables the variable beam-position
comparator -- the logic that compares the beam counters against the
programmable start/stop registers -- against a baseline with the bit clear
where that comparator is disabled.

Cited by: `sol:7-appC-beamcon0.c10`, `opus:7-appC-beamcon0.c10`.

### P10 -- BEAMCON0 bit 5, PAL, enables programmable PAL-mode timing; the SHRES/UHRES portions of the function-summary quotes are not asserted

> "5    PAL        Programmable pal mode enable" (p.305, bit table)
> "Programmable changes between PAL and NTSC modes are new for V2.0."
> (p.305, prose)
> "BEAMCON0   1DC      W      A( E ) Beam counter control register
> (SHRES,PAL)" (p.266) -- the PAL half only
> "1DC BEAMCONO new" / restored "...Beam counter control register
> (SHRES,UHRES,PAL)" (p.298) -- the PAL half only

Testable form: setting bit 5 (PAL) selects the programmable-PAL timing path
(for instance a different default vertical-total or sync timing than the
hard-wired default), against a baseline with the bit clear.

Not asserted:

- The SHRES half of the p.266 quote and the SHRES/UHRES portion of the
  p.298 quote. Per those claims' own `split` notes, there is no SHRES or
  UHRES control bit anywhere in this BEAMCON0 bit table and no
  resolution-select port on this module for either to drive -- the signal
  the claim would need is absent, not merely unobserved.
- "new for V2.0" from the prose quote -- an edition/software-release fact
  about the manual itself, not a chip behaviour, per
  `sol:7-appC-beamcon0.c18`'s own note.
- "They rely on hardware sync and blank in the Agnus/Denise chip set to
  guarantee necessary signals for a correctly displayed picture" -- the
  rationale clause in `opus:7-appC-beamcon0.c19`'s fuller quote of the same
  sentence. "A correctly displayed picture" has no observable at any
  boundary in this design, per that claim's own split note.

Cited by: `sol:7-appC-beamcon0.c12`, `opus:7-appC-beamcon0.c12` (bit-table
twins, p.305); `sol:7-appC-beamcon0.c18`, `opus:7-appC-beamcon0.c19` (prose
twins, p.305); `sol:7-appA-beamcon0.c5` (p.266, PAL half);
`sol:7-appC-map-beamcon0.c5` (p.298, PAL half).

### P11 -- BEAMCON0 bit 2, CSYTRUE, controls composite-sync pin polarity

> "2    CSYTRUE    Polarity control for C sync pin" (p.305)

Testable form: toggling bit 2 (CSYTRUE) inverts the polarity of the
composite-sync output, against a known baseline polarity with the bit
clear.

OVERRULES an existing discard, deliberately. `hrm/ch3-beam-counters-and-blanking`
discarded this bit on the reasoning that "writing any of these five bits
changes nothing this fixture can observe" -- the design's answer, not the
manual's. Per this claim's own note, the composite-sync signal is a port of
this module and the manual states this bit transforms it, so the
measurement is constructible and a red result (no observable polarity
change) is a valid, reportable finding, not a discard.

Cited by: `sol:7-appC-beamcon0.c15`, `opus:7-appC-beamcon0.c15`.

### P12 -- BEAMCON0 bit 1, VSYTRUE, controls vertical-sync pin polarity

> "1    VSYTRUE    Polarity control for V sync pin" (p.305)

Testable form: toggling bit 1 (VSYTRUE) inverts the polarity of the
vertical-sync output, against a known baseline polarity with the bit clear.

OVERRULES an existing discard, same reasoning and same fixture as P11.

Cited by: `sol:7-appC-beamcon0.c16`, `opus:7-appC-beamcon0.c16`.

### P13 -- BEAMCON0 bit 0, HSYTRUE, controls horizontal-sync pin polarity

> "0    HSYTRUE    Polarity control for H sync pin" (p.305)

Testable form: toggling bit 0 (HSYTRUE) inverts the polarity of the
horizontal-sync output, against a known baseline polarity with the bit
clear.

OVERRULES an existing discard, same reasoning and same fixture as P11/P12.

Cited by: `sol:7-appC-beamcon0.c17`, `opus:7-appC-beamcon0.c17`.

## Not turned into assertions

- `sol:7-appC-loldis.c1`, `opus:7-appC-beamcon0.c21` (p.309): "The Strobe
  Long Line register (STRLONG) can be disabled if the Disable Long Line
  (LOLDIS) bit is set in the BEAMCON0 register." LOLDIS's own bit-level
  behaviour is asserted at P6, from a different quotation (p.305's
  bit-table row). What this sentence adds is STRLONG, and per both claims'
  own `reason_kind: absent` and note, STRLONG is not part of this fixture's
  boundary -- the discard is about STRLONG, not a second discard of LOLDIS.

Sub-facts already marked not-asserted inside the propositions above are not
repeated here as separate lines and do not carry their own tally count
below -- they are parts of claims counted once, under the proposition that
contains them: the SHRES/UHRES portions and the "new for V2.0" / "correctly
displayed picture" clauses (all under P10), and "Do not write to this
register" (under P4).

## Precondition to assert

Two preconditions, neither sourced to a manual quotation -- both are the
fixture-delivers-a-known-word rule (BUILD-BRIEF) applied to this fixture
and these claims:

1. Before any of P1-P13 is trusted, the fixture must be shown to accept
   distinct, known control-bit patterns into BEAMCON0 and to carry them
   through to whatever it drives, in order, at the cycle they were written
   -- the same discipline that caught chapter 6's dead data path (every
   `data_out` reading zero because the DMA grant was sampled in the same
   `eval()` as the request). A fixture whose write path is silently dead
   would report every one of P4-P13's "bit changes behaviour" propositions
   as a false negative rather than a true one.
2. Before P11, P12 or P13 is trusted, whichever downstream signal serves as
   the sync-polarity observable must itself be shown driven to a known,
   opposite-of-expected state before the corresponding bit is toggled --
   otherwise "polarity inverted" cannot be told apart from "output stuck".
   The same applies to P5's VBSTRT/VBSTOP compare path and P10's PAL-timing
   path. Which signal that is, and how it is driven to a known state, is
   established once the RTL is open in step 3, not here.

## Control case

P4, P6, P7, P8 and P9 (bits 14, 11, 9, 8, 7) and P10's bit-table half (bit
5) are, per their own `covered_by` fields, already independently asserted
by `hrm/ch3-beam-counters-and-blanking` on this same fixture. They are this
package's control cases against the units-error objection: measured here on
the identical fixture and the identical "toggle the bit, observe the
effect" technique as P5 (the never-before-tested VARVBEN) and P11-P13 (the
sync-polarity bits this package deliberately un-discards). If a bit already
known to be live from another package's testing reads live here too, that
confirms this package's own measurement technique is sound on this fixture
-- so that a red result on P5 or P11-P13 can be trusted as a finding about
the design, not blamed on a broken meter.

## Claims accounted for

All 42 claims in `ch7-beamcon0-ecs-control-bits-spec-stage.jsonl`:

- P1: `sol:7-appA-beamcon0.c1`, `sol:7-appC-map-beamcon0.c1`,
  `opus:7-appC298.c5` (3)
- P2: `sol:7-appA-beamcon0.c2`, `sol:7-appC-map-beamcon0.c3`,
  `sol:7-appC-beamcon0.c2`, `opus:7-appC-beamcon0.c2` (4)
- P3: `sol:7-appA-beamcon0.c3`, `sol:7-appA-beamcon0.c4`,
  `sol:7-appC-map-beamcon0.c2`, `sol:7-appC-map-beamcon0.c4`,
  `sol:7-appC-beamcon0.c1`, `opus:7-appC-beamcon0.c1`,
  `opus:7-appA-beam.c13` (7)
- P4: `sol:7-appC-beamcon0.c3`, `opus:7-appC-beamcon0.c3`,
  `sol:7-appC-beamcon0.c19`, `opus:7-appC-beamcon0.c20` (4)
- P5: `sol:7-appC-beamcon0.c5`, `opus:7-appC-beamcon0.c5` (2)
- P6: `sol:7-appC-beamcon0.c6`, `opus:7-appC-beamcon0.c6` (2)
- P7: `sol:7-appC-beamcon0.c8`, `opus:7-appC-beamcon0.c8` (2)
- P8: `sol:7-appC-beamcon0.c9`, `opus:7-appC-beamcon0.c9` (2)
- P9: `sol:7-appC-beamcon0.c10`, `opus:7-appC-beamcon0.c10` (2)
- P10: `sol:7-appC-beamcon0.c12`, `opus:7-appC-beamcon0.c12`,
  `sol:7-appC-beamcon0.c18`, `opus:7-appC-beamcon0.c19`,
  `sol:7-appA-beamcon0.c5`, `sol:7-appC-map-beamcon0.c5` (6)
- P11: `sol:7-appC-beamcon0.c15`, `opus:7-appC-beamcon0.c15` (2)
- P12: `sol:7-appC-beamcon0.c16`, `opus:7-appC-beamcon0.c16` (2)
- P13: `sol:7-appC-beamcon0.c17`, `opus:7-appC-beamcon0.c17` (2)
- Not turned into assertions: `sol:7-appC-loldis.c1`,
  `opus:7-appC-beamcon0.c21` (2)

3 + 4 + 7 + 4 + 2 + 2 + 2 + 2 + 2 + 6 + 2 + 2 + 2 + 2 = 42.
