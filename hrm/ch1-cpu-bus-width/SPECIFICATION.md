# Specification — `ch1-cpu-bus-width`

Package: `hrm/ch1-cpu-bus-width`
Fixture: `minimig_m68k_bridge`, per the task sheet (`work/2026-09-16_ch1_build_brief.md`
and the dispatching session's own package assignment).
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered on
`classify.package == "ch1-cpu-bus-width"` (6 claims).
Subject: printed pages 5 and 15 of the *Amiga Hardware Reference Manual, 3rd
edition* — the CPU side of the bus, cycle-stealing by the custom chips, and the
TAS-instruction warning.

**Disclosure on order of work.** The build brief for this chapter says to write
this file from the quotations alone, commit it, and only then open the RTL — and
says explicitly: "If your task sheet or the claim data has already done that to
you, say so in your report — do not pretend the order held." It did. The
dispatching task handed this package two RTL facts with file:line citations
before any of this was written (`minimig_m68k_bridge.v:56`'s `data[15:0]`
declaration and byte strobes at `:47-48`; `minimig_m68k_bridge.v:131`'s DTACK
gate on `dbr && dbs`, plus the negative grep for `rmw|read.modify|indivisible`),
and the six claims' own `classify.evidence` fields — which had to be read in
full to extract `extract.quote`/`extract.page`/`restatement`, since they live in
the same JSON records — carry further RTL citations (gary.v's address-range
decode, `minimig_m68k_bridge.v:39-41`'s port list, `cpu_wrapper.v:113`). The
order this brief asks for did not hold. This file is still written to state the
three printed propositions in their own terms, from the quotations, before the
RTL is opened *for the purpose of writing test code* — but the claim that no RTL
was seen before this file existed would be false, and this paragraph says so
instead of hiding it.

## Claim inventory (all 6)

Three distinct propositions, each carried by both readers under different bare
ids — consistent with the brief's measured fact that 96% of this chapter's bare
ids are shared between readers, and its warning that `(reader, id)` is the uid,
never the bare id alone.

| # | uid | page | proposition |
|---|---|---|---|
| 1 | `sol:1-components.c90` | 5 | **PROP-A** |
| 2 | `opus:1-components.c89` | 5 | **PROP-A** (twin of #1) |
| 3 | `sol:1-components.c91` | 5 | **PROP-B** |
| 4 | `opus:1-components.c90` | 5 | **PROP-B** (twin of #3) |
| 5 | `sol:1-guidelines.c99` | 15 | **PROP-C** |
| 6 | `opus:1-guidelines.c99` | 15 | **PROP-C** (twin of #5) |

All 6 claims are testable and are graded here; none is declined to a sibling
package. A grep of `../nanomig-hrm-tests/hrm/` for existing assertions on cycle
stealing, `dbr`/`dbs`, TAS, "indivisible", or "read-modify-write" found no
package already grading either proposition (the one substring hit outside this
package's own topic — `agnus_blitter_fill`'s "16 bits form one indivisible fill
domain" — is a different claim, about the blitter's fill logic, not the CPU bus).

## PROP-A — cycle stealing blocks only the CPU's access to internal, shared memory

Page 5 (`sol:1-components.c90`, twin `opus:1-components.c89`):

> Even when such cycle stealing occurs, it only blocks the 680x0's access to the
> internal, shared memory.

Restatement (sol): "Cycle stealing blocks only 680x0 access to internal shared
memory."
Restatement (opus): "Cycle stealing blocks only the CPU access to internal
shared memory."

**Testable proposition:** when the custom chips are actively using (stealing)
the shared bus, the CPU's access to that *same, shared* memory is held off. This
is the positive half of a two-sentence pair (PROP-B is the other): cycle
stealing *does* block CPU access to internal/shared memory. It is a structural
question about the arbitration boundary — does an active custom-chip bus grab
withhold the CPU's bus acknowledge — not a claim about timing accuracy, cycle
counts, or which specific address ranges Gary treats as shared (that decode is
Gary's, not this module's, and is out of this boundary; see "Scope" below).

## PROP-B — custom chips cannot steal cycles when the CPU is using ROM or Fast memory

Page 5 (`sol:1-components.c91`, twin `opus:1-components.c90`):

> The custom chips cannot steal cycles when the 680x0 is using ROM or external
> memory, also known as Fast memory.

Restatement (sol): "Custom chips cannot steal CPU cycles while the 680x0
accesses ROM or external Fast memory."
Restatement (opus): "The custom chips cannot steal cycles while the CPU is
using ROM or external memory, which is also called Fast memory."

**Testable proposition:** the negative half of the same pair — when the CPU's
access is to ROM or Fast (external, non-shared) memory, an active custom-chip
bus request does *not* block it. Structurally: the same arbitration boundary
that PROP-A shows withholding acknowledge for shared-memory accesses must *not*
withhold it for ROM/Fast-memory accesses, even while the custom chips are
requesting the bus. This is also this package's control case: it is graded on
the identical measurement basis as PROP-A (the same "is the CPU's bus
acknowledge withheld while a custom-chip request is active" question), just
with the other half of the address space, and it is expected to PASS while
PROP-A's condition is the one under suspicion of being wrong.

## PROP-C — system DMA can conflict with TAS's indivisible read-modify-write cycle

Page 15 (`sol:1-guidelines.c99`, twin `opus:1-guidelines.c99`), printed page 15
— **not 13**, correcting an earlier, wrong page number for this chapter that a
prior brief carried; verified directly against
`docs/references/amiga-hrm-3rd/ch1/p15.txt`, section "ADDITIONAL ASSEMBLER
DEVELOPMENT GUIDELINES":

> Never use the TAS instruction on the Amiga. System DMA can conflict with this
> instruction's special indivisible read-modify-write cycle.

Restatement (sol): "System DMA can conflict with TAS's indivisible
read-modify-write cycle."
Restatement (opus): "System DMA can collide with the indivisible
read-modify-write cycle TAS performs."

**Testable proposition, opcode stripped out:** the sentence's causal claim is
that *system DMA* (a custom-chip bus grab, the same mechanism PROP-A/PROP-B
already put to the question) *can conflict with* a CPU bus cycle that is meant
to be indivisible — i.e., held open across what would be TAS's read and its
matching write, with no window for another bus master in between. The TAS
opcode itself is not observable at this boundary (no port here carries an
instruction word or an opcode of any kind), and is not needed: the sentence's
claim is about what DMA can do to a *bus cycle shaped like TAS's* — held open,
uninterruptible in principle — not about the 68000's instruction decoder, which
is outside every UUT root in this suite. The claim is falsifiable in the
direction the manual asserts (DMA *can* conflict) and also in the direction the
manual implies by contrast (an indivisible cycle is supposed to resist that);
demonstrating the former is what conformance to this specific warning sentence
requires.

## Scope: this module does not decode addresses; `dbr`/`dbs` are given, not derived

All three propositions turn on the same arbitration signals: `dbr` ("data bus
request, Gary keeps CPU off the bus") and `dbs` ("data bus slowdown, access to
chip ram or custom registers"), both plain inputs to `minimig_m68k_bridge`. This
module does not itself decode `address` into `dbs` — that decode is Gary's,
external to this package's top and graded (or not) in `ch1-memory-map-and-two-
buses`. At this boundary, "the CPU is accessing shared/internal memory" is
observed as `dbs == 1` and "the CPU is accessing ROM/Fast memory" is observed as
`dbs == 0`, exactly as they would arrive at this module's pins if Gary's decoder
were attached upstream — not as a claim this package can independently verify
about which physical addresses produce which `dbs` value. `address` is driven
to a plausible value for each case for narrative clarity but is not itself
load-bearing at this boundary; this is stated here, before the RTL is read for
test-writing purposes, as a scope boundary, not discovered as a surprise later.

## On a control case and mutation

- **Fixture liveness.** Before any of PROP-A/B/C is graded, the harness must
  show it can deliver at least two distinct, known values through this module
  end to end: once through the unconditional write-data register path
  (independent of any bus-cycle timing at all), and once through the gated
  read-data latch path that PROP-A/B/C's own bus-cycle helper exercises — so
  that a red result at any of the three propositions cannot be blamed on a
  dead or miswired harness.
- **Control case.** PROP-B is graded on the identical measurement basis as
  PROP-A (same "is DTACK withheld while a bus request is active" question) and
  is expected to PASS. PROP-C's bus-cycle shape (a continuously held address
  strobe spanning two phases) is different enough from PROP-A/B's ordinary
  single-phase cycle that it gets its own dedicated, ungraded control showing
  that shape completes normally with no DMA contention present.
- **Mutations.** Each of the graded assertions below is expected to hold at
  baseline (green); each mutation is therefore a stimulus change that must turn
  the same assertion red, per the rule that a stimulus-green record at an
  already-passing assertion proves nothing. Concrete stimulus values, the exact
  tick budgets, and the `// MUTATION` lines themselves are written after the RTL
  is opened for step 2, and are recorded in the C++ source, not here.

## HALT check

No claim in this package needs the TAS opcode, an instruction decoder, or any
port this module does not have. No claim needs RTL changes, a new port, or an
invented requirement. No HALT is raised for this package's six claims.
