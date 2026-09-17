# appDE-fire-buttons-ciaa

Source: claims filtered from `work/2026-09-17_appDE_packages.jsonl` on
`classify.package == "appDE-fire-buttons-ciaa"` (16 claims), quoting the
*Amiga Hardware Reference Manual, 3rd edition*, printed pages 329-330
("GAME PORT INTERFACE TO FIRE BUTTONS" / "CONNECTOR PIN USAGE FOR FIRE BUTTON
INPUTS" / "READING FIRE BUTTONS"). Built per
`work/2026-09-17_appDE_build_brief.md`.

Fixture: `ciaa`, compiled alone with its four sibling modules
(`cia_int.v`, `cia_timera.v`, `cia_timerb.v`, `cia_timerd.v` -- `ciaa.v`
alone fails Verilator elaboration with four missing-module errors). No
other module is part of this package's boundary. `ciaa` sees CIAAPRA
($BFE001) and CIAADDRA ($BFE201) and the `porta_in[7:2]`/`porta_out[3:0]`
bus; it has no connector, no pin number, and no notion of "left" or "right"
controller port anywhere in its source (verified: grepping ciaa.v,
cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v for `switch`, `joy` or
`button` returns zero hits in any executable body).

Full derivation, RTL fact citations (with exact line numbers), the complete
disposition of all 16 uids, and the mutation record are in
`test_appDE-fire-buttons-ciaa.cpp`'s own header comment -- this file
summarizes it.

## Candidate 1 -- the fire buttons look like they are on the wrong bits

This is the most consequential claim in the lot
(`work/2026-09-17_appDE_build_brief.md`, `2026-09-17_appDE_finding_candidates.md`
section 8), predicted RED on four uids, ALL of which are in this package:
`sol:E-fire.c2`, `sol:E-fire.c3`, `opus:E-firebutton.c3`,
`opus:E-firebutton.c4`.

**p.329** (this package's own oracle): "PRAO bit 7 = Fire* left controller
port" / "PRAO bit 6 = Fire* right controller port".
**p.327** (read for context only; no claim here quotes it): "J11 is the
right controller port connector (JOY1DAT, P0T1DAT). J12 is the left
controller port connector (JOYODAT, POTODAT)." -- LEFT is anchored to
JOY0DAT.

**RTL** (read directly, not compiled into this fixture):
`minimig.v:622` `.porta_in({_fire1,_fire0,...})` against ciaa.v:95's
`input [7:2] porta_in` puts PA7=`_fire1`, PA6=`_fire0`. `userio.v:92`
`joy_swap` is a hard `1'b0`, so `userio.v:215-218` makes `_sjoy1 = joy2`;
`userio.v:313` derives `_fire0` from `_sjoy1[4]`; `userio.v:54,233-239`
counts JOY0DAT from that same `_sjoy1`/`_djoy1`. So the connector read back
at JOY0DAT drives `_fire0`, which is PA6 -- the manual pairs that same
connector's fire line with PA7. **They disagree.**

**Additional evidence found while checking this package's own pages**,
read from `figures/p330.png` (corpus-defects.md: "the text layer loses the
FIRE0/FIRE1-to-PRA-bit paths and the diagram structure" -- confirmed; this
diagram does not appear at all in `p330.txt`): the wiring figure under
"CONNECTOR PIN USAGE FOR FIRE BUTTON INPUTS" labels PRA's bit-7 box
`FIRE1\` and its bit-6 box `FIRE0\`, and traces PORT 1 pin 6 to the
`FIRE0\` box and PORT 2 pin 6 to the `FIRE1\` box. That numeric labelling
(bit7=FIRE1, bit6=FIRE0) matches `minimig.v:622` exactly. It does **not**
say which of "PORT 1"/"PORT 2" is "left"/"right" -- a third port-numbering
scheme on this same page-pair, alongside p.327's J11/J12 and p.329's own
wording, none of which p.330 ties together explicitly. Not adjudicated; not
used by any graded assertion in this package (no claim here quotes this
diagram for its port-identity content).

**Why this package declines the four uids rather than grading them**: per
the build brief, "the bit-level propositions (a driven `porta_in[7]`
appears at PRA bit 7, bits 6 and 7 are independent) are yours; the
which-port-is-left half is bound at `minimig` and belongs to
`appDE-portsig-cia-bindings`." The bit-level half is already graded, in an
accepted chapter, by `hrm/ch8-ciaapra-fire-buttons`
(PROP-BIT6-PASSTHROUGH, PROP-BIT7-PASSTHROUGH,
PROP-BIT6-INDEPENDENT-OF-BIT7, PROP-BIT7-INDEPENDENT-OF-BIT6 -- read
directly from that package's own test file before this one was written).
The port-identity half needs `userio`+`minimig`, unreachable from `ciaa`
alone. Both halves are declined below, each with its own separately-stated
reason -- not one boundary decline standing in for both -- per the build
brief's warning against a boundary decline swallowing a proposition this
fixture could answer.

**A discrepancy found while checking the brief**: `sol:E-fire.c12`'s own
`classify.evidence` field says "Build should pick it up at `ciaa` (or at
`minimig`...)" for the connector-pin-6 claim. Checked directly against
`ciaa.v`: not correct for `ciaa` -- this fixture has no connector-pin
concept of any kind (see the grep evidence above). Declined here for that
reason; see the disposition table below.

## Disposition of all 16 claims

| # | uid | page | quote (abridged) | disposition |
|---|---|---|---|---|
| 1 | `sol:E-fire.c1` | 329 | "fire buttons...routed to the 8520 [PRAO]" | DECLINED -- duplicate (see GROUP1-EXISTENCE-DECLINED) |
| 2 | `opus:E-firebutton.c2` | 329 | same quote, twin of #1 | DECLINED -- duplicate |
| 3 | `sol:E-fire.c2` | 329 | "PRAO bit 7 = Fire* left controller port" | DECLINED -- compound (candidate 1) |
| 4 | `opus:E-firebutton.c3` | 329 | same quote, twin of #3 | DECLINED -- compound (candidate 1) |
| 5 | `sol:E-fire.c3` | 329 | "PRAO bit 6 = Fire* right controller port" | DECLINED -- compound (candidate 1) |
| 6 | `opus:E-firebutton.c4` | 329 | same quote, twin of #5 | DECLINED -- compound (candidate 1) |
| 7 | `sol:E-fire.c4` | 329 | "DDRA0<7:6> cleared as appropriate" | GRADED -- PROP-DDRA76-INPUT-FOR-FIRE-READ |
| 8 | `opus:E-firebutton.c5` | 329 | "Before reading this register..." | GRADED -- same PROP line |
| 9 | `opus:E-firebutton.c6` | 329 | same quote as #7, twin | GRADED -- same PROP line |
| 10 | `sol:E-fire.c6` | 329 | "Fire buttons are always valid...read at any time" | GRADED -- PROP-FIRE-READ-ANY-TIME |
| 11 | `opus:E-firebutton.c8` | 329 | same quote, twin of #10 | GRADED -- same PROP line |
| 12 | `sol:E-fire.c12` | 330 | figure, connector pin 6 = FIRE* | DECLINED -- boundary |
| 13 | `opus:E-firebutton.c15` | 330 | text, same row, twin of #12 | DECLINED -- boundary |
| 14 | `sol:E-fire.c20` | 330 | figure, DDRA bits 7/6 = 0/IN | GRADED -- folded into PROP-DDRA76-INPUT-FOR-FIRE-READ |
| 15 | `opus:E-firebutton.c24` | 330 | figure, DDRA VALUES row (bits 5:2 contradiction) | RECORDED -- measure both, grade neither |
| 16 | `opus:E-firebutton.c25` | 330 | figure, DDRA LABELS row, twin of #15 | RECORDED -- measure both, grade neither |

6 graded (2 assertions, 10 uids), 8 declined (4 pairs), 2 recorded as an
unresolved printed conflict. 16 of 16 accounted for.

## Assertions

- **A0-LIVENESS** (uid-free): a known word and its complement, driven on
  `porta_in[7:2]`, both arrive unmodified and mutually distinguishable at
  CIAAPRA. Required by the build brief before any read-based claim is
  trusted.
- **PROP-DDRA76-INPUT-FOR-FIRE-READ** (uids 7-9, 14): with CIAADDRA bits
  7:6 cleared to input (as p.329 instructs and p.330's figure shows), PRA
  bits 7:6 independently track their own `porta_in` pins across all four
  combinations. Not a duplicate of any accepted package --
  `hrm/ch8-ciaa-port-direction` declines the fire-button bits by name to
  `hrm/ch8-ciaapra-fire-buttons`, and that package never varies DDRA at
  all. A non-graded context note alongside it records that this RTL's
  guarantee is in fact unconditional on DDRA (`ciaa.v:254` never consults
  `ddrporta` for a PRA read) -- stronger than the page requires, not
  narrower.
- **PROP-FIRE-READ-ANY-TIME** (uids 10-11): once `porta_in` has propagated
  through the one documented synchroniser edge (`ciaa.v:229`), CIAAPRA
  continues to read the identical, correct value across further reads and
  clock edges -- no re-arming, no drift, no read-clears-on-access. New: a
  fresh grep for `always valid|read at any time` over every package in
  `hrm/` returns zero hits outside this file.

## Declines

- **GROUP1-EXISTENCE-DECLINED** (uids 1-2): both halves of "fire buttons
  are switches routed to PRA" are already settled by
  `hrm/ch8-ciaapra-fire-buttons` -- the pin-to-register half by its
  PROP-BIT6/7-PASSTHROUGH, the switch-to-pin half by its own P4-DECLINED
  line, for the identical boundary reason this fixture would give.
- **PROP-FIRE-LEFT-RIGHT-DECLINED** (uids 3-6, candidate 1): see above.
- **PIN6-CONNECTOR-DECLINED** (uids 12-13): `ciaa` has no connector-pin
  concept; the pin-6-to-FIRE*-signal binding is made entirely inside
  `userio.v`.
- **DDRA-FIGURE-CONTRADICTION** (uids 15-16): p.330's own DDRA figure
  prints value `0` and label `OUT` together at bits 5:2 -- appendix F
  p.341 makes a DDR bit of 0 an input, so the two rows cannot both stand.
  Per the build brief ("measure both, record both, grade neither"), this
  package does not adjudicate it. Bits 7:6 (used by
  PROP-DDRA76-INPUT-FOR-FIRE-READ) are internally consistent on the same
  figure and are not part of this contradiction.

## HALTs

None. Every claim in this package's 16 was either constructible at `ciaa`
alone or cleanly declined with its boundary or duplicate reason named; no
claim required changing RTL, adding a port, or inventing an unstated
requirement.
