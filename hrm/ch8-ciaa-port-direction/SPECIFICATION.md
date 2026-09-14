# Specification — `ch8-ciaa-port-direction`

Package: `hrm/ch8-ciaa-port-direction`
Fixture: `ciaa` (compiled alone). The exact RTL filename is not named here —
per the build brief, port names and file identity are step-2 material; the
file actually compiled is recorded once step 2 opens it.
Claims file: `hrm-testpipeline/work/ch8-s1-build/ch8-ciaa-port-direction.jsonl`
(10 claims)
Subject: the general data-direction mechanism described on printed pages 228
and 241 — that some controller-port pins, and the CIAADDRA/CIAAPRA register
pair generally, can be switched from input to output — **excluding** the
fire-button-specific and CIAAPRA-address-specific content that two sibling
packages in this slice own.

This file is written from the `extract.quote` / `classify.extract.restatement`
fields of the 10 claims alone, cross-read against the primary source text for
pages 228 and 241
(`hrm-testpipeline/docs/references/amiga-hrm-3rd/ch8/p228.txt`,
`.../p241.txt`). No RTL file for this package has been opened. It fixes the
printed propositions to be tested; port names, concrete stimulus values, and
the `expect()` assertions themselves are added only after this file is
committed and step 2 (opening the RTL) begins, per the build brief.

## Scope carve-out — two sibling packages own part of this same text

The build task names two sibling packages in this chapter-8 slice that also
touch `ciaa`, and says anything belonging to them is declined here with
`record()`, not graded:

- **`ch8-ciaapra-fire-buttons`** owns the fire lines.
- **`ch8-ciaapra-address-window`** owns where CIAAPRA/CIAADDRA answer on the
  bus — it is described as "a rig over `ciaa` + `gary`", i.e. it needs the
  address decoder, which `ciaa` compiled alone does not have.

Page 228's Table 8-1 identifies DB-9 pin 6 as "Button 1" on a joystick — the
fire button — and page 241 opens with "The joystick fire buttons can also be
configured as outputs," immediately followed by the CIAADDRA/CIAAPRA
paragraph that gives their bus addresses. Four of this package's 10 claims
restate the pin-6/fire-button fact, and three restate a fact that names
CIAADDRA's or CIAAPRA's address ($BFE201 / $BFE001). All seven are out of
scope for this package on that basis (see "Claim inventory" and "Claims not
assigned an assertion" below). What is left — and what this package
specifically tests — is the address-independent, pin-generic statement of
the direction mechanism itself: that CIAADDRA is a one-to-one direction mask
over CIAAPRA's bit positions, and that setting a direction bit to 1 makes
the corresponding data bit an output. Nothing in the two kept propositions'
`extract.quote` text names the fire buttons or a bus address.

## Claim inventory (all 10)

| # | uid | page | assigned |
|---|---|---|---|
| 1 | `opus:8-tbl8-1.c36` | 228 | OUT-OF-SCOPE-FIRE (`ch8-ciaapra-fire-buttons`) |
| 2 | `sol:8-pin.c40` | 228 | OUT-OF-SCOPE-FIRE (twin of #1) |
| 3 | `opus:8-dio.c42` | 241 | OUT-OF-SCOPE-FIRE (`ch8-ciaapra-fire-buttons`) |
| 4 | `opus:8-dio.c43` | 241 | OUT-OF-SCOPE-ADDRESS (`ch8-ciaapra-address-window`) |
| 5 | `opus:8-dio.c44` | 241 | **PROP-1** |
| 6 | `opus:8-dio.c45` | 241 | OUT-OF-SCOPE-ADDRESS (`ch8-ciaapra-address-window`) |
| 7 | `opus:8-dio.c46` | 241 | **PROP-2** |
| 8 | `sol:8-dio.c41` | 241 | OUT-OF-SCOPE-FIRE (twin of #3) |
| 9 | `sol:8-dio.c42` | 241 | OUT-OF-SCOPE-ADDRESS (twin of #4/#6; its correspondence content is already PROP-1) |
| 10 | `sol:8-dio.c43` | 241 | **PROP-2** (twin of #7) |

All 10 claims accounted for: 2 scored propositions (carrying 3 uids between
them), 7 declined to the two named siblings.

## PROP-1 — CIAADDRA is a one-to-one, per-bit-position mask over CIAAPRA

Page 241 (`opus:8-dio.c44`):

> CIAADDRA ($BFE201) contains a mask that corresponds one-to-one with the
> data read register, CIAAPRA ($BFE001).

Restatement: "CIAADDRA holds a mask whose bits correspond one-to-one with
the bits of CIAAPRA."

**Testable proposition, address stripped out:** each bit position of the
CIAADDRA mask governs the *same* bit position of CIAAPRA — not a different
position, not all positions as a single switch. This is a claim about
*positional* correspondence, distinct from PROP-2's claim about *what*
correspondence (input vs. output) means. It is tested by showing that
driving the direction bit at position *i* changes position *i*'s behaviour
and leaves some other position *j* (i ≠ j) unaffected — which two positions,
fixed in step 2 once the port list is known, avoiding whichever positions
turn out to be the fire lines (out of scope here per the carve-out above).

Status: testable, pending step 2 confirming this fixture exposes per-bit
observability at all (if `ciaa` alone turns out to gang all eight bits
through a single undifferentiated path with no way to address one position
independently of another, that is a HALT for this proposition specifically,
to be reported, not assumed here).

## PROP-2 — setting a direction bit to 1 makes the corresponding bit an output

Page 241 (`opus:8-dio.c46`, twin `sol:8-dio.c43`):

> Setting a 1 in the direction position makes the corresponding bit an
> output.

Restatement (opus): "Setting a 1 in a CIAADDRA direction position makes the
corresponding CIAAPRA bit an output."
Restatement (sol): "Setting a CIAADDRA direction bit to 1 makes the
corresponding CIAAPRA line an output."

**Testable proposition:** for a given bit position, when its CIAADDRA bit
reads as 1, that position's behaviour is that of an output — its value, as
observed at this fixture's boundary, is determined by what was written to
CIAAPRA rather than by anything else. The manual states no complementary
sentence about what "0" does in so many words on this page, but "makes an
output" is only a meaningful, falsifiable claim if there is a contrasting
"not an output" state for the same bit when its direction bit is 0 — so the
control side of this proposition (direction = 0 does not show that same
write-follows-read behaviour) is part of what must be shown, not an
independent claim invented here; it is the necessary other half of the same
sentence.

**On the write/read conjunction.** Per the carried-forward note on
data-direction claims: this fixture (`ciaa` alone) has no separate strobe
that lets "the direction bit was set" and "the data bit was written" and
"the data bit was read" be isolated from one another as independent causal
steps — they can only be observed as one ordered sequence: set direction,
write data, read data. What PROP-2 grades is therefore the conjunction that
is actually observable at this boundary (direction bit set to 1, AND a data
value written, AND that same value read back at that position), not a
decomposition into an isolated "write path" fact and an isolated "read path"
fact asserted separately. This is stated here as how PROP-2 will be graded,
in advance of knowing the port names that make it concrete.

Status: testable, contingent on the same per-bit observability question
noted under PROP-1.

## On a control case and mutation

The build brief requires (a) a fixture-liveness proof before any
data-dependent grading, (b) at least one control case expected to PASS on
the same basis as anything under suspicion, and (c) a stimulus mutation per
assertion, recorded in the `// MUTATION <label> <oracle|stimulus|none>
<red|green|impossible> <what changed>` form. Concrete stimulus values and
port names are step-2 material and are not fixed here; the shape of each is
already visible from the claims alone:

- **Fixture liveness.** Before PROP-1 or PROP-2 is graded, the harness must
  drive at least two distinct, known values into `ciaa`'s data-register
  write path and confirm — by some write-dependent effect visible at the
  boundary — that both distinct values actually arrived and did not
  silently collapse to one value or to nothing.
- **Control case.** PROP-2's own direction-bit-1 case, on one representative
  bit position, is itself the control that every other measurement is
  compared against: if writing CIAAPRA with the direction bit held at 1 does
  not reliably read back what was written, no finding about any other bit or
  about PROP-1's positional independence means anything. No separate,
  unscored control harness is needed beyond this.
- **Mutation, PROP-1.** The natural stimulus mutation redirects the
  direction-bit stimulus to a *different* bit position than the one the
  assertion reads back, leaving the expected value and label alone — this
  must go red, because position *i*'s direction bit must not govern position
  *j*.
- **Mutation, PROP-2.** The natural stimulus mutation is to leave the
  direction bit at 0 (input) while writing the same data value and checking
  the same read-back assertion that PROP-2's direction=1 case checks — this
  must go red, because an input bit must not show the write-follows-read
  behaviour that only an output bit is claimed to show.
- Whether either mutation is `impossible` in this implementation, and if so
  which kind ("unfalsifiable at this boundary" vs. "not exercised, by a
  scope decision"), is undecided until step 2 shows the actual port list;
  it is not assumed here.

This section states a strategy, not the assertions themselves; the actual
`expect()` calls, their concrete stimulus values, bit positions, and the
`// MUTATION` header lines are written in step 2, after the RTL is opened.

## Claims not assigned an assertion (7 of 10)

**Out of scope — the fire lines, owned by `ch8-ciaapra-fire-buttons` (4
claims):** `opus:8-tbl8-1.c36`, `sol:8-pin.c40` (page 228, Table 8-1's
pin-6 footnote — pin 6 is listed on that same table as "Button 1" on a
joystick, i.e. the fire button) and `opus:8-dio.c42`, `sol:8-dio.c41` (page
241's opening sentence, "The joystick fire buttons can also be configured as
outputs" — naming the fire buttons directly). These are declined here and
recorded with the sibling package's name, not graded, per the build task's
explicit carve-out.

**Out of scope — where CIAAPRA/CIAADDRA answer on the bus, owned by
`ch8-ciaapra-address-window` (3 claims):** `opus:8-dio.c43` ("CIAADDRA is at
address $BFE201"), `opus:8-dio.c45` ("CIAAPRA ($BFE001) is the data read
register of that pair" — the restatement identifies the register by its bus
address), and `sol:8-dio.c42` (a composite restatement carrying both
addresses plus the one-to-one correspondence fact already scored as PROP-1).
`ciaa` compiled alone has no address decoder — that rig is explicitly the
sibling package's ("a rig over `ciaa` + `gary`") — so no claim naming a bus
address is constructible at this fixture's boundary regardless of ownership,
and ownership independently places it out of scope. `sol:8-dio.c42`'s
non-address content (the correspondence fact) is not lost: it is the same
fact `opus:8-dio.c44` states and PROP-1 already covers, so declining
`sol:8-dio.c42` does not drop any proposition this package could otherwise
score.

## What this specification deliberately does not contain

Port names, signal names, and concrete stimulus values are absent by
design, per the build brief's order of work — they are added only after
this file is committed and the RTL is read for step 2.

Nothing in this file names or infers a Verilog port, wire, register-select
index, or file:line location. The `classify.evidence` field in the 10-claim
source file already carries a prior reader's RTL-derived measurements
(register-select encoding, specific signal names, bit-driver presence) —
that content was read (the claims file could not be parsed for `uid`/page/
quote/restatement without encountering it, since it lives in the same JSON
records), but it is deliberately not reproduced or relied upon anywhere
above: every proposition in this file is stated, and is intended to be
tested, using only the `extract.quote`, `extract.page`, and restatement
content, plus the primary page-228/241 source text. This is disclosed rather
than concealed, following the same practice a sibling package
(`ch8-potgo-register-bit-layout`) recorded for itself.

The choice of *which* bit positions to exercise for PROP-1 and PROP-2 is
deferred to step 2, and will avoid whichever bit positions step 2's own,
independent reading of the RTL shows to be the fire lines — not because this
file already knows which those are (it does not use that content, per
above), but because the carve-out in this document is a scope decision
independent of any RTL fact.
