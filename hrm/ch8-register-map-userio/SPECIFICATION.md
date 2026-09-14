# Specification — `ch8-register-map-userio`

Package: `hrm/ch8-register-map-userio`
Fixture: `userio.v` — one module, compiled alone (no sibling package exists for
this fixture; nothing in this suite has been measured against it before).
Claims file: `hrm-testpipeline/work/ch8-s1-build/ch8-register-map-userio.jsonl`
(117 claims, all `classify.status: "testable"`)
Subject: chapter 8's register map for the seven controller-port registers
decoded at this boundary — JOY0DAT, JOY1DAT, POT0DAT, POT1DAT, POTGOR, POTGO,
JOYTEST — their addresses and their read/write access direction, plus the
appendix-B row and legend claims that state those two facts.

This file is written from the `extract.quote` / `extract.restatement` fields
of the 117 claims alone. No RTL file for this package has been opened. It
fixes the printed propositions to be tested and the composition/decline
reasoning; port names, concrete stimulus values, and the `expect()` calls
themselves are added only after this file is committed and `userio.v` is
opened, per the build brief.

## The seven registers and their printed addresses

| Register | Address (p.229 prose) | Appendix A offset (p.281/282) | Appendix B offset (p.289) | Direction |
|---|---|---|---|---|
| JOY0DAT | $DFF00A | 00A | *00A | R |
| JOY1DAT | $DFF00C | 00C | *00C | R |
| POT0DAT | $DFF012 | 012 | *012 | R (chip mark `P(E)`) |
| POT1DAT | $DFF014 | 014 | *014 | R (chip mark `P(E)`) |
| POTGOR | $DFF016 | 016 | *016 | R |
| POTGO | $DFF034 | 034 | *034 | W |
| JOYTEST | (no p.229 entry) | 036 | *036 | W |

JOYTEST is named only in the two appendix tables; the p.229 prose register
list (which gives the other six their full `$DFFxxx` CPU address directly,
uncomposed) does not mention it. No claim in this file states JOYTEST's full
`$DFFxxx` address — only its custom-chip offset, `036`. The proposition
tested for JOYTEST is therefore worded as an offset-decode fact, not a
`$DFFxxx`-address fact, matching what the claims actually state.

## Claim accounting (all 117)

| Category | Count | Disposition |
|---|---|---|
| ADDRESS (p.229 + p.281/282 + p.289 rows, one register's address stated from several manual locations) | 41 | Graded, 7 composed propositions (PROP-ADDR-\*) |
| READ/WRITE DIRECTION (p.281/282 + p.289 rows, composed with the p.287 legend; p.240 prose for POTGO/POTGOR) | 32 | Graded, 7 composed propositions (PROP-RW-\*) |
| FUNCTION / semantic content (what a register's bits mean, what it counts, port-numbering prose) | 44 | Declined — out of this package's own scope (see below) |

41 + 32 + 44 = 117. Every claim is accounted for below by uid.

## PROP-ADDR-\* — one register's address, composed from every page that states it

**Proposition to test, per register:** `userio` answers on `data_out` at the
register's own address and at no address belonging to a different register in
this table — the same selectivity shape the accepted `VPOSR-ADDRESS-004` and
`DENISEID-ADDR-RO` propositions use elsewhere in this suite.

The p.229 prose already states each register's full `$DFFxxx` address
directly and without composition (`"JOY0DAT ($DFF00A) Counter for digital
(mouse) input (port 1)"` — nothing else on that page is needed to read this
as an address). The two appendix tables restate the same fact as a bare
offset from the custom-chip base; they are corroborating citations on the
same assertion, not a composition dependency the way the R/W column is.

| Prop | Register | p.229 uids | p.281/282 uids | p.289 uids |
|---|---|---|---|---|
| PROP-ADDR-JOY0DAT | JOY0DAT | `opus:8-regs.c1`, `sol:8-reg.c1` | `opus:8-appa-joy.c1`, `sol:8-appa-joy.c1` | `opus:8-appB.c8`, `sol:8-appB.c7` |
| PROP-ADDR-JOY1DAT | JOY1DAT | `opus:8-regs.c3`, `sol:8-reg.c3` | `opus:8-appa-joy.c5`, `sol:8-appa-joy.c6` | `opus:8-appB.c13`, `sol:8-appB.c12` |
| PROP-ADDR-POT0DAT | POT0DAT | `opus:8-regs.c7`, `sol:8-reg.c8` | `opus:8-appa-pot.c1`, `sol:8-appa-pot.c1` | `opus:8-appB.c18`, `sol:8-appB.c17` |
| PROP-ADDR-POT1DAT | POT1DAT | `opus:8-regs.c9`, `sol:8-reg.c10` | `opus:8-appa-pot.c5`, `sol:8-appa-pot.c6` | `opus:8-appB.c23`, `sol:8-appB.c22` |
| PROP-ADDR-POTGOR | POTGOR | `opus:8-regs.c13`, `sol:8-reg.c15` | `opus:8-appa-potgo.c5`, `sol:8-appa-pot.c33` | `opus:8-appB.c28`, `sol:8-appB.c27` |
| PROP-ADDR-POTGO | POTGO | `opus:8-regs.c11`, `sol:8-reg.c12`, plus `opus:8-dio.c2` (p.240 prose, "The POTGO register is at address $DFF034.") | `opus:8-appa-potgo.c1`, `sol:8-appa-pot.c28` | `opus:8-appB.c48`, `sol:8-appB.c47` |
| PROP-ADDR-JOYTEST | JOYTEST | (none — see note above) | `opus:8-appa-joytest.c1`, `sol:8-appa-joytest.c1` | `opus:8-appB.c53`, `sol:8-appB.c52` |

Status: testable, all seven. 41 uids total (6 per register × 6 registers + 7
for POTGO + 4 for JOYTEST — the table above sums to exactly that).

## PROP-RW-\* — one register's read/write direction, composed with the p.287 legend

**A row letter and the legend state one fact between them.** The p.289 (and
p.281/282) row for JOY0DAT prints `R` in its R/W column; that letter is not
itself the English word "read-only" — page 287's legend defines what the
letter means:

> `W=write-only; R=read-only,`
— quoted at `opus:8-appB.c3` / `opus:8-appB.c4` (opus reader, both halves)
and `sol:8-appB.c5` / `sol:8-appB.c6` (sol reader, both halves), page 287.

These four legend uids are **not** among this package's own 117 claims —
`classify.status` for all four is `"not-asserted"`, `reason_kind`
`"not-hardware"` ("a gloss on appendix B's own notation... no port can carry
a typographic convention"), and they do not appear anywhere in
`ch8-s1-build/ch8-register-map-userio.jsonl`. They were located in
`hrm-testpipeline/work/gate-ch8-s1/userio.jsonl`, the wider (923-record,
pre-selection) claim pool this package's 117-claim file was drawn from; each
row claim's own `classify.composes_with` field in *this* package's 117 names
exactly which of the four it depends on (e.g. `opus:8-appB.c9`'s
`composes_with: ["opus:8-appB.c4"]`). They are cited here, on the composed
assertion, because the brief requires the composed fact to be graded and
names both contributing claims — not because they are being added to this
package's own 117-claim inventory. This mirrors an existing accepted
pattern in this suite (`hrm/agnus_blitter_regdecode` cites `ch6-appb-legend.wr`,
a legend claim that lives in a different chapter-6 file than its own).

Similarly, `POT0DAT` and `POT1DAT` carry the appendix-B chip mark `P (E)`
rather than a bare `P`. **`(E)` is not read as "ECS" here.** Page 288 states
what it means:

> "Registers denoted with an "(E)" in the chip column means that those
> registers have been changed in the Enhanced Chip Set (ECS)."
— page 288 (quoted in the chapter-6 inventory corpus at the same physical
manual page; no claim in this package's own 117 turns its *grading* on this
mark — see the FUNCTION section below, where `opus:8-appB.c21` /
`opus:8-appB.c26` are declined rather than graded). Where those declines
reason about the mark at all, they cite page 288 for it rather than assuming
"ECS" silently, per the build brief's instruction.

**userio.v has no rd/hwr/lwr strobe in its port list** (disclosed by this
package's own claims' `classify.evidence` field, which already inspected the
module header while establishing testability — this is upstream classifier
work, not this specification reading ahead into the RTL for step 2). A read
cycle and a write cycle are therefore the same stimulus at this boundary, and
"read-only" / "write-only" cannot be graded by asserting the manual's word
directly. They are graded as the conjunction actually observable:

- **read-only** = the address answers on `data_out` (a distinct, correct
  value comes back), **and** no value presented on `data_in` at that address
  ever reaches any state (a later read at that address, or at any other
  address whose value could only have come from that write, is unaffected).
- **write-only** = a value presented on `data_in` at that address **does**
  reach some state (a later read — of that address or of wherever the value
  surfaces — reflects it), **and** the address itself does not answer its own
  content on `data_out` (reading it does not present the state it just
  changed).

Note: this package's own claims carry a `classify.boundary_caveat` field
that states the write-only conjunction with the *read-only* wording verbatim
("the address answers on the data output, and no value presented on the
data input at that address reaches any state") on `opus:8-appB.c49`,
`sol:8-appB.c48`, `opus:8-appB.c54` and `sol:8-appB.c53` — this looks like a
copy of the read-only template that was not flipped for the write-only
claims. The `evidence` field on the sibling Appendix-A claims for the same
two registers (`opus:8-appa-potgo.c2`, `opus:8-appa-joytest.c2`, and their
twins) states the actually-measured shape correctly ("a read of $034
returned $0000 while a write of $8001 changed which pot counter ran"), and
that is the conjunction this package tests for POTGO/JOYTEST — the
mismatched `boundary_caveat` sentence is reported here rather than silently
followed.

| Prop | Register | Direction | p.281/282 row uids | p.289 row uids | p.287 legend uids | p.240 prose uids |
|---|---|---|---|---|---|---|
| PROP-RW-JOY0DAT | JOY0DAT | R | `opus:8-appa-joy.c2`, `sol:8-appa-joy.c2` | `opus:8-appB.c9`, `sol:8-appB.c8` | `opus:8-appB.c4`, `sol:8-appB.c6` | — |
| PROP-RW-JOY1DAT | JOY1DAT | R | `opus:8-appa-joy.c6`, `sol:8-appa-joy.c7` | `opus:8-appB.c14`, `sol:8-appB.c13` | `opus:8-appB.c4`, `sol:8-appB.c6` | — |
| PROP-RW-POT0DAT | POT0DAT | R | `opus:8-appa-pot.c2`, `sol:8-appa-pot.c2` | `opus:8-appB.c19`, `sol:8-appB.c18` | `opus:8-appB.c4`, `sol:8-appB.c6` | — |
| PROP-RW-POT1DAT | POT1DAT | R | `opus:8-appa-pot.c6`, `sol:8-appa-pot.c7` | `opus:8-appB.c24`, `sol:8-appB.c23` | `opus:8-appB.c4`, `sol:8-appB.c6` | — |
| PROP-RW-POTGOR | POTGOR | R | `opus:8-appa-potgo.c6`, `sol:8-appa-pot.c34` | `opus:8-appB.c29`, `sol:8-appB.c28` | `opus:8-appB.c4`, `sol:8-appB.c6` | `opus:8-dio.c5`, `sol:8-dio.c3` |
| PROP-RW-POTGO | POTGO | W | `opus:8-appa-potgo.c2`, `sol:8-appa-pot.c29` | `opus:8-appB.c49`, `sol:8-appB.c48` | `opus:8-appB.c3`, `sol:8-appB.c5` | `opus:8-dio.c4`, `sol:8-dio.c2` |
| PROP-RW-JOYTEST | JOYTEST | W | `opus:8-appa-joytest.c2`, `sol:8-appa-joytest.c2` | `opus:8-appB.c54`, `sol:8-appB.c53` | `opus:8-appB.c3`, `sol:8-appB.c5` | — |

Status: testable, all seven. 32 uids from this package's own 117 (4 per
register × 5 read registers + 6 for POTGO + 6 for JOYTEST — table sums to
that), plus the four external legend uids named above, cited jointly and
repeatedly wherever their mark appears.

## Claims declined — 44 FUNCTION / semantic-content claims

This package's own subject line scopes it to **address, access direction,
and the appendix-B row/legend claims that state those two facts** — not to
what a register's *value* means (which bits are vertical vs. horizontal,
what a counter counts, port-numbering prose, or what "formerly called
POTINP" records historically). That semantic content is the explicit subject
of sibling packages already staged in this same build lot:
`ch8-joydat-counter-layout`, `ch8-mouse-quadrature-decode`,
`ch8-pot-counter-start-and-charge`, `ch8-joytest-counter-write`,
`ch8-potgo-register-bit-layout`, `ch8-appb-chip-column-denise`,
`ch8-appb-chip-column-paula`. Declining a function claim here is not a
finding that the claim is untestable — several of this package's own
`classify.evidence` fields disclose that a sibling *has already measured*
the underlying behaviour (e.g. the quadrature-count evidence quoted on
`opus:8-regs.c2`) — it is a scope decision, matching the precedent
`hrm/ch3-register-map-denise` set for BPLCON0/2/3's bit semantics.

| Register | Declined uids (page) |
|---|---|
| JOY0DAT | `opus:8-regs.c2`, `sol:8-reg.c2` (229); `opus:8-appa-joy.c4`, `sol:8-appa-joy.c4`, `sol:8-appa-joy.c5` (281); `opus:8-appB.c11`, `sol:8-appB.c10` (289) |
| JOY1DAT | `opus:8-regs.c4`, `sol:8-reg.c4` (229); `opus:8-appa-joy.c8`, `sol:8-appa-joy.c9`, `sol:8-appa-joy.c10` (281); `opus:8-appB.c16`, `sol:8-appB.c15` (289) |
| POT0DAT | `opus:8-regs.c8`, `sol:8-reg.c9` (229); `opus:8-appa-pot.c4`, `sol:8-appa-pot.c4`, `sol:8-appa-pot.c5` (282); `opus:8-appB.c21`, `sol:8-appB.c20` (289, carries the `(E)` boundary note — p.288 applies, not silently read as "ECS") |
| POT1DAT | `opus:8-regs.c10`, `sol:8-reg.c11` (229); `opus:8-appa-pot.c8`, `sol:8-appa-pot.c9`, `sol:8-appa-pot.c10` (282); `opus:8-appB.c26`, `sol:8-appB.c25` (289, same `(E)`/p.288 note) |
| POTGOR | `opus:8-regs.c14`, `sol:8-reg.c16` (229); `opus:8-appa-potgo.c8`, `sol:8-appa-pot.c36` (282); `opus:8-appB.c31`, `sol:8-appB.c30` (289) |
| POTGO | `opus:8-regs.c12`, `sol:8-reg.c13`, `sol:8-reg.c14` (229); `opus:8-appa-potgo.c4`, `sol:8-appa-pot.c31`, `sol:8-appa-pot.c32` (282); `opus:8-appB.c51`, `sol:8-appB.c50` (289) |
| JOYTEST | `opus:8-appB.c56`, `sol:8-appB.c55` (289) |

7+7+7+7+6+8+2 = 44.

## Fixture-liveness proof (built carefully — no sibling package to copy from)

`userio.v` has never had a test package in this suite; there is no harness
to inherit and no prior baseline to trust. Per the build brief, distinct
known words must arrive before anything data-dependent is graded. Two
independent halves are required, because this boundary's read and write
paths are proven by different mechanisms:

1. **Read-liveness, two distinct non-zero values at two addresses**,
   mirroring the accepted `CONTROL-FIXTURE-ALIVE` (DENISEID+CLXDAT) and
   `VPOSR-ADDRESS-004` (force_vpos) patterns: JOY0DAT and JOY1DAT are
   counters driven by the module's own joystick/mouse input pins (not by the
   register bus — driving them is not the same stimulus this package's R/W
   claims are grading). Stepping port 1's quadrature inputs by a known
   number of transitions and port 2's by a *different* known number, then
   reading both registers back, must show two distinct, correct,
   mutually-independent counts before any ADDR or RW proposition above is
   graded on either register.
2. **Write-liveness, a value written through the bus reaching state
   somewhere**: this package's own claims already disclose (evidence field
   on `opus:8-appa-potgo.c2` / `opus:8-appa-joytest.c2`) that writing POTGO
   or JOYTEST changes state observable elsewhere. Before PROP-RW-POTGO /
   PROP-RW-JOYTEST are graded on the "does not answer its own content" half,
   the "a value written does reach state" half must independently be shown
   true with a *distinct* stimulus value — otherwise a $0000 reading at the
   write address would be indistinguishable from a dead bus.

Concrete stimulus values, exact port names, and the pass/fail wiring are
step-3 material (added once `userio.v` is open) — this section fixes the
*shape* the liveness proof must take, so step 3 does not have to re-derive
it.

## On a control case and on mutation

At least one control case must PASS on the same measurement basis as
anything under suspicion. The seven ADDR propositions and seven RW
propositions are naturally each other's controls: they exercise the same
`data_out`/state-observation mechanism at different addresses, so a passing
JOY0DAT proposition (say) stands as a control for a suspect POT0DAT
proposition graded the same way, and vice versa. The write-liveness proof
in the previous section is also, independently, a control for both
write-only propositions.

A stimulus mutation per assertion (never an oracle/expected-value change) is
step-3 material once concrete stimulus values exist; the shape available
without any RTL fact: for an ADDR proposition, redirect the read to a
neighbouring register's address (the fact "answers here, not there" must go
red when "there" is substituted for "here"); for an RW proposition,
redirect the *other* half's address or value so the specific half under
test is defeated while the rest of the harness is untouched. Exact
targets are fixed in step 3.

## What this specification deliberately does not contain

Port names, signal names, parameter names, and concrete stimulus values are
absent by design — added only after this file is committed and `userio.v`
is read. No claim in this package's own 117 is left unassigned: 41 ADDR + 32
RW + 44 FUNCTION = 117. No chip-assignment claim (which physical chip —
Agnus/Denise/Paula — decodes a given address) exists as its own testable
proposition anywhere among the 117; the two sibling packages
`ch8-appb-chip-column-denise` and `ch8-appb-chip-column-paula` own that
material, and this package's own subject line does not require re-deriving
it.
