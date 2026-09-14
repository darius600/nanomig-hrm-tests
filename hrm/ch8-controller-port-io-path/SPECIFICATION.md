# SPECIFICATION — ch8-controller-port-io-path

Written from the claims file's `extract.quote` / `extract.restatement` fields
and the manual-level parts of `classify` (`conflict_resolution`,
`observable`) alone. **No RTL file and no sibling package's Makefile or test
source has been opened for this document.** Step 2 (naming the actual port
list, reading `minimig.v` for the wiring, and reading
`hrm/ch8-ciaa-port-direction/Makefile` and
`hrm/ch8-register-map-userio/Makefile` for the build pattern) happens only
after this file is committed. The one exception, already disclosed once here
and not repeated below: I read `hrm/ch8-appb-chip-column-denise/test_denise.cpp`
and its `Makefile` before writing this document, for harness *style* only —
the `expect`/`note`/`record` call shapes, the `MUTATE`-environment-variable
mutation technique, the summary-line format, and the generic Makefile
skeleton (`TOP_MODULE`, `VERILOG_SOURCES`, `VERILATOR_FLAGS`). `denise` is not
a module this package touches and nothing about `userio`, `ciaa`, or their
port lists was learned from it.

## Unit

- Fixture: a **rig**, not a single module — a small top-level Verilog wrapper
  (authored in this unit directory, name TBD in Step 2) that instantiates
  both `userio` and `ciaa` and wires them the way `minimig.v` does, per this
  task's explicit instruction. `minimig.v` is read for the wiring only after
  this specification is committed.
- Manual pages under test: 227, 229, 232.
- Claims file: `ch8-controller-port-io-path.jsonl` — 10 claims, all `status:
  testable`, all flagged by the classifier as `observable: yes, at userio and
  ciaa` or `observable: needs a wider boundary`. None are flagged
  `unobservable` or `boundary decline` in the claims file itself.

### Why a rig, and why exactly these two modules

Three sibling packages sit either side of the same physical signal:
`hrm/ch8-ciaapra-fire-buttons` (fixture `ciaa` alone) grades that CIAAPRA
bits 6 and 7 each track their own pin **once the pin's value is already
present at `ciaa`'s boundary**. `hrm/ch8-joystick-switch-decode` (fixture
`userio` alone) grades the switch decode **up to the point where `userio`
hands the signal onward**. Neither can observe the wire itself: `ciaa` alone
has no controller-port pin to drive, and `userio` alone has no CIAAPRA bit to
read. All 10 claims in this package's file describe a fact that spans that
wire — a controller-port pin (page 227's connector, page 232's mouse
button, the "pin 6" of page 229 and 232) arriving at a CIAAPRA bit (page 229,
232) — so a fixture that stops at either module's own boundary cannot grade
them as the manual states them; it can at best grade half the sentence and
assume the other half. That is the scope error this task warns against in
the other direction, and I check for it below per claim rather than
asserting it once for the whole file.

**Per-claim check for whether the wide boundary is actually earned**, not
assumed:

- Page 232 and the page-229/233 fire-button claims (uids 2–5, 7–10 below) are
  unambiguous: the stimulus (a button at the physical connector) is a
  `userio`-side concept and the observation (a CIAAPRA bit) is a `ciaa`-side
  concept. No single-module fixture can put both a real stimulus and a real
  observation on the same run. The wide boundary is necessary for these 8.
- Page 227's two claims (uids 1 and 6) are the weaker case and I say so
  plainly: uid 6 (`sol:8-overview.c3`, "each connector can carry output") is,
  read narrowly, gradable from `userio` alone if the pot-pin output path
  (pins other than pin 6) is used as the vehicle — that path never leaves
  `userio`. I still ground it inside this rig (Test approach, below) because
  the claim was handed to this package rather than to
  `hrm/ch8-register-map-userio`, and grounding it through the pin-6/CIAAPRA
  path this package uniquely owns keeps the package's 10 assertions on a
  consistent measurement basis instead of silently falling back to a
  narrower single-module proof for just one claim. Uid 1
  (`opus:8-intro.c10`, "input or output") is graded the same way, via pin 6,
  which is explicitly one of the bidirectional pins and therefore does need
  `ciaa`. Both are disclosed here rather than left implicit; if Step 2 shows
  the pin-6 grounding does not work for uid 6, I fall back to declining it
  toward `hrm/ch8-register-map-userio` rather than forcing a wide-boundary
  proof the claim does not need.

## Source quotations (verbatim, from the claims file)

### Page 227 — connector capability (2 claims)

> Each Amiga has two nine-pin connectors that can be used for input or
> output with a variety of controllers. Usually, the nine-pin connectors

- `opus:8-intro.c10` — restatement: "The nine-pin connectors can be used for
  input or for output."
- `sol:8-overview.c3` — restatement: "Each controller connector can carry
  output."

### Page 229 — register map, CIAAPRA row (3 claims)

> CIAAPRA ($BFE001) Input and output for pin 6 (port 1 & 2 fire buttons)

- `opus:8-regs.c6` — restatement: "CIAAPRA is input and output for pin 6,
  the port 1 and port 2 fire buttons."
- `sol:8-reg.c6` — restatement: "CIAAPRA provides input and output for
  controller pin 6 on both ports."
- `sol:8-reg.c7` — restatement: "The fire buttons of controller ports 1 and
  2 use pin 6."

### Page 232 — mouse-button footnote (5 claims)

> \* The left button on the Amiga mouse is connected to CIAAPRA ($BFE001).
> Port 1 uses bit 6 and port 2 uses bit 7. A logic [low presumably indicates
> a press; the quotation is cut off in every claim's extract]

- `opus:8-mbtn.c3` — restatement: "The left button of the Amiga mouse is
  connected to CIAAPRA."
- `opus:8-mbtn.c5` — restatement: "The left button of port 1 appears in bit
  6 of CIAAPRA."
- `opus:8-mbtn.c6` — restatement: "The left button of port 2 appears in bit
  7 of CIAAPRA."
- `sol:8-button.c4` — restatement: "Controller port 1's left mouse button
  uses CIAAPRA bit 6."
- `sol:8-button.c5` — restatement: "Controller port 2's left mouse button
  uses CIAAPRA bit 7."

## KNOWN CONFLICT 3 — the disputed numeral

Per this task's carried-forward finding: printed p.233 numbers the
controller ports 0 and 1, where p.240 and the rest of the chapter number
them 1 and 2. The claims file's own `conflict_resolution` field (present on
every page-229 and page-232 claim above) states the same split in its own
words: *"Undisputed ... there are two controller ports, their fire buttons
occupy CIAAPRA bits 6 and 7, and the lower-numbered port under either scheme
takes bit 6. Disputed: the numeral."*

None of my three printed pages (227, 229, 232) is page 233 or 240, so this
package never quotes the disputed numbering directly — but four of my
claims (`opus:8-mbtn.c5`, `opus:8-mbtn.c6`, `sol:8-button.c4`,
`sol:8-button.c5`) assert a specific numeral ("port 1" / "port 2") paired
with a specific bit. Per this task's instruction, **I grade the undisputed
sub-claim**: that the two ports' buttons occupy two distinct, non-overlapping
CIAAPRA bits (6 and 7), and that whichever port is the *lower-numbered* one
under either printed scheme is the one on bit 6. I do **not** assert which
physical connector on the board carries the manual's numeral "1" — that is
not settled by anything in RTL, only by a page the classifier itself marks
disputed. This is disclosed on each of those four assertions individually in
the test file, not folded silently into a passing result.

A second reason to hold the numeral itself out of scope, not yet verified
but flagged here before RTL is opened so it cannot look like a
post-hoc excuse later: this task's own carried-forward measurement on
`userio` states that `JOY0DAT`'s decode is driven by the port-2-suffixed
signal and `JOY1DAT`'s by the port-1-suffixed signal — "the reverse of what
the port names suggest." If register-number-to-physical-port suffix is
already reversed once in this RTL's own naming, I am not willing to assume
CIAAPRA's bit-to-port labelling free of the same risk before checking, and I
will check it in Step 2 rather than guess.

## Composed propositions and test approach

Ten claims, ten `expect()` calls, each labelled with its `classify.uid`
exactly. Where two uids assert the identical fact in different words (e.g.
`opus:8-mbtn.c5` and `sol:8-button.c4` both assert "port with the
lower numeral → bit 6"), both still get their own `expect()` line — the
gate pairs mutation records against the label, not against content, and
collapsing two uids onto one graded line would mean one of them was never
actually asserted.

| # | uid | Page | What is graded |
|---|---|---|---|
| 1 | `opus:8-intro.c10` | 227 | pin 6 is usable as an input (a press is readable) **and** as an output (a driven value reaches the physical pin) — both, on the one pin this rig can prove either way |
| 2 | `sol:8-overview.c3` | 227 | **both** connectors, not just one, can be driven to output something — grounded on whichever output-capable mechanism Step 2 shows survives the rig on each port (see boundary note above: may be the pot-pin path, may be pin 6; named in the built file) |
| 3 | `opus:8-regs.c6` | 229 | CIAAPRA supports both input and output for the bit the row names "pin 6" |
| 4 | `sol:8-reg.c6` | 229 | same fact as #3, independent uid, independent `expect()` line |
| 5 | `sol:8-reg.c7` | 229 | the position "pin 6" on **each** port's own connector (not one shared pin) is the one that reaches CIAAPRA as that port's fire signal |
| 6 | `opus:8-mbtn.c3` | 232 | a button stimulus at a controller port reaches CIAAPRA at all (existence of the path, either bit) |
| 7 | `opus:8-mbtn.c5` | 232 | the lower-numbered port's button reaches bit 6, and only bit 6 (isolation from bit 7) — numeral itself out of scope, see KNOWN CONFLICT 3 |
| 8 | `opus:8-mbtn.c6` | 232 | the other port's button reaches bit 7, and only bit 7 (isolation from bit 6) — numeral itself out of scope |
| 9 | `sol:8-button.c4` | 232 | same fact as #7, independent uid, independent `expect()` line |
| 10 | `sol:8-button.c5` | 232 | same fact as #8, independent uid, independent `expect()` line |

For every row above, "reaches" means: a distinct stimulus value applied at
the wrapper's controller-port-facing input is observed to change the named
CIAAPRA bit, read through whatever register-read mechanism Step 2 shows
`ciaa` exposes (address/chip-select details unknown until then). For rows
1, 3 and 4's *output* half, "reaches" means the reverse: a value written to
CIAAPRA (with the CIA's own data-direction register — `DDRA` in the
standard 6526 register set the manual's appendix documents for this chip —
set so the bit is an output) is observed at the wrapper's controller-port-
facing pin. I do not yet know the exact signal names for either direction;
that is a Step 2 fact, not a Step 1 one.

**Isolation** (rows 7–10) is graded as a conjunction: pressing port A's
button changes the bit it should and leaves the other bit unpressed;
pressing port B's button does the reverse. A row that only checked "my bit
goes low" without checking "the other bit does not" would pass even if the
two ports were shorted together, which is exactly the kind of comparator
that cannot fail — see Mutation plan.

## Control / liveness

Per this task's requirement, the fixture must prove it can deliver a known,
distinct word across the join before anything built on it is trusted. This
rig has two joins to prove alive, not one — `userio`'s stimulus-facing
input must reach *something* observable inside `userio` itself, and
separately the value that reaches `ciaa`'s side of the wire must be
distinguishable from a stuck value. I will drive at least two distinct,
self-chosen stimulus patterns into the wrapper's controller-port inputs,
using signal positions Step 2 confirms are **not** any of the 10 claims
above, and assert the two patterns arrive as two distinguishable
observations at `ciaa`'s CIAAPRA read. This doubles as this run's control
case, exercised on the same stimulus-in/register-read measurement basis as
every graded claim, and as the unconditional "known word delivered" proof.
It carries no uid and no page citation. Exact signal positions are named in
Step 2.

## Mutation plan

Every `expect()`-graded row above (10 claims plus the liveness/control
checks) gets one stimulus mutation recorded in the file header, using the
`MUTATE` environment-variable technique disclosed above (read from
`test_denise.cpp`): change what the fixture is *asked to do* — a stimulus
value, not the comparison in the `expect()` call — confirm the assertion
goes red under `MUTATE=<id>`, then confirm it reverts to green with
`MUTATE` unset.

Two of the ten (rows 3/4, whose narrow reading is that only the CIAAPRA bit
explicitly named "pin 6" on p.229 is under test — see the rig-justification
section on page 227's weaker claim for the same caution about not
overclaiming beyond a row's literal wording) may turn out, once Step 2 shows
the actual mapping, to be non-independent of an already-graded row (e.g. if
the built assertion for #3 and #1's output half end up driving the exact
same comparator). Where that happens I will use the substitution form
instead of a second stimulus mutation — a different, already-proven-live
mechanism through the same comparator, shown to go green — and say so
explicitly in that assertion's `MUTATION` line and in the build report,
rather than mutate an assertion a second time in a way that cannot add
information.

Where a mutation is `impossible`, the build report states which of the two
reasons applies (unfalsifiable at this boundary, or not exercised by a scope
decision — naming the path and the reason), per this task's requirement. I
do not expect any of the 10 claims to land there, since all 10 are
`status: testable` with a stated observation path in the claims file, but I
am not committing to zero until Step 2 is done.

## What I do not yet know (Step 2 questions, deliberately unanswered here)

- The actual port/signal names on `userio` and `ciaa`, and therefore the
  exact wrapper wiring — read from `minimig.v` only after this file is
  committed.
- Whether `userio` alone (without `ciaa`) would have sufficed for
  `sol:8-overview.c3` (uid 6) — flagged above as the one claim where the
  wide boundary is not obviously earned; resolved honestly in the built
  file rather than papered over.
- Which physical port RTL naming treats as "first" — used only to organise
  the wrapper's two port instances as A/B, never to assert the disputed
  manual numeral (see KNOWN CONFLICT 3).
- The register-read mechanism `ciaa` exposes for CIAAPRA (address lines,
  chip-select convention), needed to write `read_reg`-style helpers.
- The CIA data-direction register's exact port/bit name, needed to drive
  pin 6 as an output for rows 1, 3 and 4.
