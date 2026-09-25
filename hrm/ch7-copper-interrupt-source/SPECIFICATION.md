# Specification: ch7-copper-interrupt-source

Fixture: `agnus_copper + paula_intcontroller`. Subject: printed p.219, COPER
bit 4, "reserved for specifically identifying the Copper as the interrupt
source."

Written from the quotations in `ch7-copper-interrupt-source-spec-stage.jsonl`
alone. No RTL has been opened to produce this document. Every claim is cited
by its `uid` (`<reader>:<id>`), never by the bare `id`, per BUILD-BRIEF.

## Reading "reserved": convention vs. discriminator

The manual's verb is "reserved," and it supports two readings that are not
the same proposition:

1. **Convention (software/documentation level).** The manual is telling the
   programmer how to *interpret* a bit that is already set: "if you see bit 4
   up, that means the Copper asked for it." This is guidance about meaning,
   not a claim about what the hardware does on any given cycle. No stimulus
   and no observation makes a convention true or false — a testbench can no
   more grade "this bit means X" than it can grade a variable's name. This
   half is **not assertable** in any fixture, this one included.

2. **Discriminator (hardware level).** For the convention to be honest, the
   hardware has to make something *true*: bit 4 must actually respond to the
   Copper's interrupt condition, and not to anything else. That itself
   splits by scope:
   - **2a. Within this fixture's boundary** (`agnus_copper` +
     `paula_intcontroller`, and nothing else): when the Copper produces the
     condition under which it asks for an interrupt, bit 4 of the
     interrupt-request register responds, and no other bit of that register
     responds to the same stimulus. This is falsifiable with the two modules
     assigned and is what **P1** below tests.
   - **2b. Across the whole chip**: that *no other* interrupt source
     anywhere in the design — none of which are present in this fixture —
     can ever also set bit 4. Confirming that needs every other candidate
     source's RTL, which is a wider module boundary than the two modules
     assigned to this package. **Not assertable here**; recorded below.

So only 2a is testable at the boundary this package was given. P1 is written
to test exactly that, and no more.

## Testable proposition

### P1 — the Copper's interrupt condition sets bit 4, and only bit 4, of the interrupt-request register

> "this bit has been reserved for specifically identifying the Copper as the
> interrupt source." (printed p.219)

Testable form: when the Copper module produces the condition under which it
requests an interrupt, bit 4 (COPER) of the interrupt-request register
becomes set, and no other bit of that register changes state as a result of
that same stimulus. Both halves of the conjunction are required — a check
that only looked for "some bit changed" would not test identification, and a
check that only looked for "no other bit changed" would pass vacuously if
bit 4 never got set at all.

This proposition does not presuppose what, mechanically, causes the Copper
to produce that condition (an instruction outcome, a strobe, a stored flag —
whichever the RTL turns out to use). That mechanism is RTL knowledge and is
deliberately not decided here; step 2 picks the concrete stimulus that
realizes "the Copper's interrupt condition." The pass/fail criterion above
does not change once that mechanism is known.

Cited by: `sol:7-interrupts.c41` (printed p.219).

Co-cited, denotation only: `opus:7-fig7-4.c15` (Figure 7-4, printed p.221)
reads the row whose fourth column is "COPER" and whose Description cell
reads "graphics coprocessor" — i.e., the figure names COPER as denoting the
same condition (the Copper, described there as the graphics coprocessor)
that P1 tests. The same figure row also carries an Exec software priority
("5") and a hardware priority ("3") for COPER, visible via the crossread
note rather than this claim's own restatement. Of those two values, the
Exec software priority is a claim about exec.library's server chain, a
distinct proposition from naming and out of this package's boundary (see
"Not turned into assertions"). The hardware priority is different: it is
the value that actually reaches the 680x0's IPL pins, is observable at
this fixture's own boundary (`paula_intcontroller`'s `_ipl` output), and is
asserted as P2 below. Only the naming/denotation half of `opus:7-fig7-4.c15`
is folded into P1.

### P2 — the Copper's interrupt condition, carried through the same real path, reaches the CPU as hardware level 3

> "Bit 4, COPER, is used by the Copper to issue a level 3 interrupt."
> (printed p.219)

This sentence is the paragraph's other half, next to the one P1 already
cites, and it is not this package's own claim: `sol:7-interrupts.c41`'s own
classify note says "the level-3 half of the same printed paragraph is
claimed separately by `7-interrupts.c40` and is testable" — that claim UID
was carved off to `hrm/ch7-interrupt-priority-encoder` (its P9), not handed
to this package, and is not cited here. What this package *does* hold is
`opus:7-fig7-4.c15`'s hardware-priority-3 numeral for the same COPER row
(see P1's citation note above), which names the identical proposition in
Figure 7-4's own vocabulary. That numeral is this proposition's citation.

The sibling package's P9 tests this same bit/level pairing in isolation, on
`paula_intcontroller` alone, with the register bus driven directly by the
testbench — it explicitly excludes "the writer identity ('by the Copper')".
This package's fixture is the complementary case: the Copper module itself,
not the testbench, produces the condition, and the question is whether that
real, Copper-driven bit-4 set — the same one P1 already grades — propagates
through `paula_intcontroller`'s priority encoder to the CPU-visible level.
That is a fact about the integration of the two modules, not reachable from
either module alone, and is exactly this fixture's boundary.

Testable form: after the same stimulus P1 uses (the Copper's own interrupt
condition, COPER enabled), the interrupt controller's `_ipl` output reads 4
— the active-low encoding of hardware level 3 (`paula_intcontroller.v`'s
priority encoder: `intreqena[4]` alone, no other source pending, drives
`_ipl<=4`).

Cited by: `opus:7-fig7-4.c15` (printed p.221), hardware-priority half.

## Control case

BUILD-BRIEF requires at least one control case expected to PASS on the same
measurement basis as the case under suspicion. This package's control for
P1 is the same register read P1 uses, under the state where the Copper's
interrupt condition has not been presented: bit 4 reads clear. Presenting
the condition and observing bit 4 set (P1), against not presenting it and
observing bit 4 clear (control), on the same measurement basis, is also the
fixture-integrity proof BUILD-BRIEF asks for: it shows the path between the
two modules is neither stuck at 0 (the positive case would never go red)
nor stuck at 1 (the control would never pass). Neither half alone would rule
out either failure mode.

P2 gets its own control, on P2's own measurement basis (`_ipl`): sampled
after COPER's own enable bit is set but before the Copper's interrupt
condition is presented, `_ipl` reads 7, the encoder's idle value. Presenting
the condition and observing `_ipl` reach 4 (P2), against not yet presenting
it and observing `_ipl` at 7 (this control), rules out the same two failure
modes for the second half of the path (interrupt-request bit to CPU level)
that the P1/control pair already ruled out for the first half (Copper write
to interrupt-request bit).

## Not turned into assertions

- **The convention half of `sol:7-interrupts.c41`** — that a set bit 4
  *means*, to the programmer, "the Copper did it." Not a runtime behaviour;
  no stimulus/observation pair grades it. Recorded, not asserted.
- **Whole-system exclusivity** — that bit 4 can never be set by any source
  other than the Copper anywhere in the full chip. This package's fixture is
  exactly two modules; confirming exclusivity across the whole design needs
  every other candidate interrupt source's RTL, which is a wider module
  boundary than assigned. Recorded, not asserted.
- **The Exec-software-priority half of `opus:7-fig7-4.c15`** (Exec software
  priority 5 for COPER) — exec.library's server-chain numbering, a distinct
  proposition from both naming (P1) and the hardware-priority value that
  actually reaches the CPU (P2). Recorded, not asserted. The hardware-
  priority half of the same claim is no longer in this list: it is P2,
  above, promoted from a bare measurement because it is directly observable
  at this fixture's own boundary. (`7-interrupts.c40`, the neighbouring
  p.219 prose claim making the same point in different vocabulary, remains
  carved off to `hrm/ch7-interrupt-priority-encoder` and is still not cited
  here — P2 is reached through `opus:7-fig7-4.c15` alone, which was already
  this package's claim.)

## opus:ch2-the-copper-and-the-680x0.c3 (printed p.35)

> "To interrupt the 680x0, use the Copper  MOVE  instruction to store a 1
> into the following bits of  INTREQ :"

Restated: the 680x0 is interrupted by using the Copper MOVE instruction to
store a 1 into bits of INTREQ.

### Reading the claim: a technique, not an enumeration, already run by P1+P2

The quotation is the lead sentence of a list; the list itself (the specific
bits it goes on to name) is not part of this claim's text and is not cited
here. Read on its own terms, the sentence asserts a general technique with
two halves: (a) the Copper's MOVE instruction stores a 1 into a bit of
INTREQ, and (b) doing so interrupts the 680x0. It is not a claim that every
bit the list goes on to name behaves identically, and not a claim that only
one particular bit does — one faithful instance of the mechanism, exercised
end to end at this fixture's real boundary, establishes it; the individual
bit-to-function mapping the list supplies is separate cargo this package
holds no claim for.

Both halves are already run, on the same real Copper-driven stimulus, by
this package's existing P1 and P2:

- **Half (a)**, the MOVE-stores-a-1-into-an-INTREQ-bit half, is exactly what
  P1 measures: the same Copper MOVE (`word2=0x8010`), travelling the
  Copper's real register-write path into `paula_intcontroller`, lands on
  INTREQR bit 4. This half is folded into P1, alongside the existing
  `sol:7-interrupts.c41` / `opus:7-fig7-4.c15` citations there.
- **Half (b)**, the interrupts-the-680x0 half, is exactly what P2 measures
  on the same stimulus: `_ipl` reaches an active (non-idle) encoding at the
  CPU boundary — the 680x0-visible fact of being interrupted. This half is
  folded into P2, alongside its existing `opus:7-fig7-4.c15` citation.

No new assertion, control or mutation is needed: P1's and P2's existing
mutations already exercise exactly this claim's two halves (P1's own
stimulus mutation for half (a); P2's INTENA-mask mutation, which drives
`_ipl` back to idle, for half (b)) and are unchanged by this addition.

Cited by: `opus:ch2-the-copper-and-the-680x0.c3` (printed p.35) — co-cited
into P1 for half (a) and into P2 for half (b), the same pattern this
package already uses for `opus:7-fig7-4.c15`.

## Claims accounted for

Both claims from the prior cut of `ch7-copper-interrupt-source-spec-stage.jsonl`
are accounted for: `sol:7-interrupts.c41` → P1 (discriminator half asserted;
convention half and whole-system-exclusivity half recorded, not asserted).
`opus:7-fig7-4.c15` → co-cited into P1 (naming/denotation half asserted)
*and* into P2 (hardware-priority half asserted); its Exec-software-priority
half is recorded, not asserted.

The one claim in the latest cut of `ch7-copper-interrupt-source-spec-stage.jsonl`,
`opus:ch2-the-copper-and-the-680x0.c3`, is accounted for: co-cited into P1
(MOVE-stores-a-1-into-INTREQ half) and into P2 (interrupts-the-680x0 half),
both already established on the same real Copper-write path and the same
CPU-visible measurement basis P1 and P2 already used for their existing
citations. No clause of this claim is left bundled-but-untested: the only
part of its printed sentence this package does not speak to is the specific
identity of "the following bits," which is not this claim's own text.

## Mutation plan

- **P1**: change what the Copper is asked to do so that its interrupt
  condition does *not* occur (a different, or no, operation), while still
  checking for bit 4 set. Expect the assertion to go red. Revert.
- **Control (P1)**: change the stimulus from "condition withheld" to
  "condition presented," while still checking for bit 4 clear. Expect the
  control to go red. Revert.
- **P2**: change the INTENA stimulus so COPER's own enable bit is never set
  (drop `intena[4]` while leaving the master-enable bit `intena[14]` set),
  while still checking `_ipl` for 4. The interrupt-request bit itself (P1)
  is untouched by this change, so P1 and both controls should stay green;
  P2 alone should go red, because the still-set request bit never reaches
  the priority encoder while masked off. Revert.

All three mutations touch only what the Copper/testbench writes; the
expected value in each assertion is left untouched, per BUILD-BRIEF. All
three were executed (see the test file's header comment for the P1/control
history and the P2 result: 3 passed, 1 failed with the mutation in place,
4 passed, 0 failed after revert).
