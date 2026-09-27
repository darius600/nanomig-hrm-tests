# appDE-pot-counters — SPECIFICATION

Source: *Amiga Hardware Reference Manual*, 3rd ed., Appendix E, printed pages
331 ("GAME PORT INTERFACE TO PROPORTIONAL CONTROLLERS") and 332 ("CONNECTOR
PIN USAGE FOR PROPORTIONAL INPUTS" table and the "POT COUNTERS" figure).

Claims: `hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl`, filtered to
`classify.package == "appDE-pot-counters"` — 42 claims, two readers (`opus`,
`sol`), 21 twin pairs. Every uid below is `classify.uid` verbatim
(`reader:id`), never the bare `id` — 450 of the union's 1281 bare ids are
shared between readers, so the bare id alone is not a safe key.

**Module boundary**: `userio.v`, compiled alone. Every claim in the file
carries `"observable": "yes, at userio"`.

## p.332's text layer is not used for this package

Per `appE/corpus-defects.md`, p.332's text layer loses every counter-block
label and the POT0X/POT0Y/POT1X/POT1Y names, and its register addresses are
adrift. All figure-sourced claims below (`source_kind: "figure"`) are read
from `figures/p332.png` directly, confirmed here again before grading:

- **CONNECTOR PIN USAGE FOR PROPORTIONAL INPUTS**: 1 XBUT/Extra Button; 2
  Unused; 3 LBUT\*/Left button/*See Digital Joystick*; 4 RBUT\*/Right
  button/*See Digital Joystick*; 5 POTX/X analog in/`POT[0/1]DAT<7:0>, POTGO,
  POTINP`; 6 Unused; 7 +5V/125ma max, 200 ma surge; 8 Ground; 9 POTY/Y analog
  in/`POT[0,1]DAT<15:8>, POTGO, POTINP`.
- **POT COUNTERS**: PORT 0 → POT0X/POT0Y → POT0Y COUNTER, POT0X COUNTER →
  `POT0DAT DFF012`. PORT 1 → POT1X/POT1Y → POT1Y COUNTER LATCH, POT1X COUNTER
  LATCH → `POT1DAT DFF014`. `POTGO DFF034`. `POTINP DFF016`.

Both match every figure-sourced `classify.extract.restatement` in the claims
file exactly. No repair was needed.

## The p.332/p.333 conflict — grade neither side

p.332's own figure prints `POTINP  DFF016`. p.333's *LIGHT PEN* figure prints
`POTINP read only  DFF104` — the same address it gives `BPLCON0 write only`
two boxes above. Per `appE/corpus-defects.md` and
`work/2026-09-17_appDE_finding_candidates.md` item 1, **both of p.333's
labels are wrong**: `appA-playfield/p271.txt:12` and `appB3/p291.txt:29` print
`BPLCON2 104`, BPLCON0 is `100`, and `denise.v:59-60` agrees
(`BPLCON0 = 9'h100; BPLCON2 = 9'h104;`). p.332's own reading is the one the
rest of the manual corroborates.

Two claims in this package rest on p.332's side of that conflict:
`sol:E-pot.c32` and `opus:E-pot.c36` (both restate "POTINP is at DFF016").
Per the build brief, **grade neither side**: both readings are measured
below with `note()` and one `record()` cites both uids, with
`classify.source_defines: "conflict — …"` on both. NM-HRM-062 is the
precedent (see `hrm/appF-cia-address-decode`, Group H). The DFF104 side of
the same conflict belongs to `appDE-lightpen-userio`
(`sol:E-lightpen.c19` / `opus:E-lightpen.c22`), not to this package, and is
not graded here either.

## Decline discipline

Per the build brief: **decline only where a prior assertion grades the same
proposition; grade where this page is narrower or on a different axis.**
Three already-accepted packages cover most of this page's ground:
`hrm/ch8-pot-counter-start-and-charge` (reset interval, per-line compare,
hold, byte layout), `hrm/ch8-potgo-register-bit-layout` (button identity via
POTGOR/POTINP bits), and `hrm/ch8-register-map-userio` (POT register
addresses). `hrm/ch8-controller-pin-function-map` (Table 8-1, p.228) covers
most of p.332's connector table under different device names.

## One coverage gap this lot owns: pins 1, 2 and 6 of the proportional-input connector

`hrm/ch8-controller-pin-function-map`'s own SPECIFICATION.md records that
Table 8-1's pin-1 "Button 3\*\*" cell (XY Prop. Joystick column) — the same
cell p.332 prints as "XBUT: Extra Button" — **"was not handed to this
package as a claim"** and is **"neither asserted nor declined"**. Pins 2 and
6 have no cell in Table 8-1 at all in the Prop. Pair / XY Prop. Joystick
columns; p.332 is the first page in this survey to say anything about them
("Unused"). All three are graded here, new.

## The pins-3/4 question (not itself a classified claim)

The build brief separately flags: p.332 prints pins 3 and 4 as "LBUT\*: Left
button" / "RBUT\*: Right button" — proportional-controller button names —
and no assertion anywhere in the suite tests whether these pins reach the
button-read path (`potcap`/POTINP) the way pins 5 and 9 do. This is measured
in `group_PIN_EXCLUSION()` below (see "Measured, not graded" note) and
reported, not adjudicated. **Measured result: they do not.** `potcap`'s
sequential block (`userio.v:160-178`) reads only `_djoy1[5]`, `_djoy2[5]`,
`_mright`, `_mthird` and `potreg` (POTGO) bits — never bit 0, 1, 2, 3 or 4 of
either connector vector. Pins 1, 2, 3, 4 and 6 (bits 3, 2, 1, 0, 4) are all
structurally excluded from `potcap`/POTINP alike; only pin 9 (bit 5) and the
mouse-button/POTGO terms participate. This is a broader confirmation than
the brief's own bits-\[0\]/\[1\] framing, not a narrower one. p.332's own
NOTES column is consistent with this: pins 3/4 carry no POTGO/POTINP
reference at all (unlike pins 5 and 9, which explicitly list `POTGO,
POTINP`) — it says "See Digital Joystick" instead, which is exactly where
this package declines those two cells to (see Group 7 below).

## Claim groups

| group | uids | topic | verdict |
|---|---|---:|---|
| A1 | `sol:E-pot.c16`, `opus:E-pot.c19` | pin 1 (XBUT) is a readable digital input | ASSERT — NEW |
| A2 | `sol:E-pot.c17`, `opus:E-pot.c20`, `sol:E-pot.c21`, `opus:E-pot.c24` | pins 2 and 6 reach no pot/button-read register | ASSERT — NEW |
| G1 | `sol:E-pot.c2`, `opus:E-pot.c3` | JOY\[0/1\]DAT byte layout (Y high, X low) | DECLINE — re-grade |
| G2 | `sol:E-pot.c5`, `opus:E-pot.c6` | reset interval (7 NTSC / 8 PAL lines) | DECLINE — re-grade |
| G3 | `sol:E-pot.c6`, `opus:E-pot.c8` | continuous compare against threshold | DECLINE — re-grade |
| G4 | `sol:E-pot.c7`, `opus:E-pot.c9` | latch-and-hold on threshold crossing | DECLINE — re-grade |
| G5 | `sol:E-pot.c9`, `opus:E-pot.c11` | POTY/POTX = right/"unused" mouse button | DECLINE — re-grade |
| G6 | `sol:E-pot.c11`, `opus:E-pot.c13` | buttons "also available in POTGO and POTINP" | DECLINE — re-grade + write-only half |
| G7 | `sol:E-pot.c18`, `sol:E-pot.c19`, `opus:E-pot.c21`, `opus:E-pot.c22` | pins 3/4 LBUT\*/RBUT\* device identity | DECLINE — re-grade under cross-reference |
| G8 | `sol:E-pot.c20`, `opus:E-pot.c23` | pin 5 POTX pot-gating | DECLINE — re-grade, already RED |
| G9 | `sol:E-pot.c24`, `opus:E-pot.c27` | pin 9 POTY pot-gating | DECLINE — re-grade, already ASSERT |
| G10 | `sol:E-pot.c25/c26/c28/c29`, `opus:E-pot.c29/c30/c31/c33` | POT COUNTERS figure: port → counter → register | DECLINE — re-grade |
| G11 | `sol:E-pot.c27/c30/c31`, `opus:E-pot.c32/c34/c35` | POT0DAT/POT1DAT/POTGO addresses | DECLINE — re-grade |
| C | `sol:E-pot.c32`, `opus:E-pot.c36` | POTINP address, conflicts with p.333 | CONFLICT — grade neither |

6 ASSERT + 34 DECLINE + 2 CONFLICT = 42. Every uid in the 42-claim file
appears exactly once above.

## Per-group detail

### A1 — pin 1, XBUT (NEW)
Quote (figure): "1  XBUT  Extra Button" (no register note printed). Per
`ch8-controller-pin-function-map`'s own accounting this cell was never
handed to any package before. Measured: pin 1 is `_joy[3]` (port 1's vertical
quadrature line, reaching `_djoy1[3]` via the `joy_swap=1'b0` cross-connect —
see below), and a level change on it alone (the paired line, pin 3/bit 1,
held fixed) produces exactly one JOY0DAT high-byte increment then, on the
reverse transition, exactly one decrement. That is what "readable digital
input" is graded against; nothing about "extra button" as a device identity
is claimed or gradable — this fixture has no notion of what is plugged into
the connector, the same split `ch8-controller-pin-function-map` Group C
already draws for other pins.

### A2 — pins 2 and 6, Unused (NEW, negative)
Quotes (figure): "2  Unused" (no description or register note) and "6
Unused" (same). No accepted package grades the proportional-input column's
silence on these two pins — only what they carry under other columns/pages.
Measured: with every `potcap`-relevant input held at a fixed value, pin 2
(bit 2) and pin 6 (bit 4), on either connector, can be pressed and released
without moving a single bit of POTINP or either pot counter. `potcap`'s own
sequential block (`userio.v:160-178`) never reads these bit positions.

### G1 — JOY\[0/1\]DAT byte layout
`hrm/ch8-joydat-counter-layout`'s "Byte assignment within the register"
proposition (p.230) already grades this exact fact. Identical proposition,
different page; declined as a re-grade.

### G2 — reset interval
`hrm/ch8-pot-counter-start-and-charge` G6 (p.235, "seven or eight scan
lines") already grades this, RED (this RTL holds no separate multi-line
reset state at all — see that package's G6/G11). p.331 is the only page in
the lot that ties the split explicitly to NTSC/PAL, but `ntsc` is not a port
of `userio` (`userio.v:28-49`) — the line rate reaches this module only as
`pot_cnt_en`, so a fixture at this boundary cannot grade the NTSC/PAL
distinction separately from the count G6 already grades. Declined as a
re-grade; the capacitor-discharge half of the same sentence names a
component this design does not have and would not be constructible even if
ungraded elsewhere.

### G3 — continuous compare
`hrm/ch8-pot-counter-start-and-charge` G7 (p.235, "compares once per scan
line") already grades this. Declined as a re-grade. The "continuously
compared to an internal reference level" wording is an analog description of
the same digital per-line gate check G7 grades; `joy_ana_en` is tied `1'b0`
(`userio.v:93`), so no analog comparison branch is reachable in this design
regardless.

### G4 — latch-and-hold
`hrm/ch8-pot-counter-start-and-charge` G9 (p.235, "held until the next
POTGO") already grades this. Declined as a re-grade.

### G5 — POTY/POTX button identity
`hrm/ch8-potgo-register-bit-layout` PROP-11/PROP-12 and
`hrm/ch8-mouse-button-pins` P2/P3 already grade which mouse button reaches
which pot-gate pin, from p.240 and p.232 respectively. Declined as a
re-grade — the wording differs (this page's "unused mouse button" vs.
p.240's "middle button") but the proposition, which button reaches which pot
pin, is the same one.

### G6 — "also available in POTGO and POTINP"
The POTINP half is already graded (`ch8-potgo-register-bit-layout`
PROP-6/PROP-8/PROP-11/PROP-12; `ch8-pot-pins-as-digital-io` Group 6).
Measured here: POTGO (`$DFF034`) is write-only in this design — the read mux
(`userio.v:289-309`) has no case for it, so a read of `$034` returns
`16'h0000` regardless of what was last written, confirmed below. Grading
"available in POTGO" as a readback claim would red a correct write-only
register; not graded for that half either. One `record()`, both halves.

### G7 — pins 3/4 device identity (LBUT\*/RBUT\*)
The page's own NOTES column for both cells reads "See Digital Joystick" —
no register is named, unlike pins 5 and 9. `hrm/ch8-controller-pin-function-map`
Group A already grades pin 3 = joystick left / pin 4 = joystick right (the
electrical fact, at JOY0DAT). p.332's proportional-pair naming
("Left button"/"Right button") is a third name for the identical line and
the page's own cross-reference confirms it is the same proposition, not a
narrower or differently-axised one — unlike Group C's device-identity
declines (pins 5/9), which are declined for lack of observability, these are
declined because they are the same fact under another name. Declined as a
re-grade; the underlying line-exists fact is Group A's, not re-measured
here.

### G8 — pin 5, POTX
`hrm/ch8-controller-pin-function-map` Group B2 and
`hrm/ch8-pot-pins-as-digital-io` Group 5 already grade this pin's pot-gating,
RED (port 2's copy of the gate has no external term at all —
`userio.v:169`, `joy2_pin5 = ~(potreg[13] & ~potreg[12])`, is a pure function
of the POTGO write register). Declined as a re-grade. NEW information on
this page (the three register names POT\[0/1\]DAT\<7:0\>, POTGO, POTINP) is
itself covered by `ch8-pot-counter-start-and-charge` G17 and
`ch8-potgo-register-bit-layout` PROP-12.

### G9 — pin 9, POTY
`hrm/ch8-controller-pin-function-map` Group B already grades this pin's
pot-gating, ASSERT (this gate is symmetric and live on both ports). Declined
as a re-grade, for the same reason as G8.

### G10 — POT COUNTERS figure structure
`hrm/ch8-pot-counter-start-and-charge` G14-G17 already grade the byte
assignment (Y high / X low) and which connector feeds which register.
Declined as a re-grade. **Port numbering is not settled and not resolved
here**: p.330 labels its two connectors PORT 1/PORT 2; p.332's own POT
COUNTERS figure labels the identical pair PORT 0/PORT 1; p.333's LIGHT PEN
figure labels one of them PORT 0 — three consecutive figures, two
conventions, within one appendix. `hrm/ch8-ciaapra-fire-buttons` carries
this as KNOWN CONFLICT 3 and grades only the numeral-free core; this
package does the same — no claim here asserts a specific numeral binding,
and none is resolved by assumption.

### G11 — register addresses
`hrm/ch8-register-map-userio` already grades `$DFF012` (POT0DAT), `$DFF014`
(POT1DAT) and `$DFF034` (POTGO) under PROP-ADDR-POT0DAT/POT1DAT/POTGO.
Declined as a re-grade.

### C — POTINP address conflict
See "The p.332/p.333 conflict" above. Measured, not graded.

## Test plan

- **Fixture liveness**, before any claim is graded: a specific, chosen,
  non-constant word is delivered through POT0DAT/POT1DAT before grading
  starts (same shape as `ch8-pot-counter-start-and-charge`).
- **Mutation policy**: every `expect()` label gets its own
  `// MUTATION <uid> stimulus red ...` record (all six ASSERT uids are
  testable and none is baseline-red, so no oracle-substitute is needed for
  this package). RTL is never touched; a mutation redirects which
  testbench-driven bit is toggled.
- **Declined claims**: one `record()` per group above, naming every uid in
  the group and the reason. No `expect()` call for a declined uid, and no
  declined uid is also asserted (`check_uid_placement.py` requirement).
- **Conflict claims**: `note()` for both readings, one `record()` naming
  both uids, `classify.source_defines: "conflict — …"`, no `expect()`.

## HALT check

No claim in this package needs RTL changed, a port added, or a requirement
invented. Every ASSERT claim is measurable at `userio.v` alone with the
existing port list. No HALT.
