# Specification — `ch8-parallel-port-direction`

Package: `hrm/ch8-parallel-port-direction`
Fixture: `ciaa` (compiled alone).
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-parallel-port-direction.jsonl`
(2 claims)
Subject: printed page 259's Interface Hardware paragraph on the parallel
port — the single sentence stating that the general-purpose parallel
interface is bi-directional.

## Claim inventory (both)

| # | uid | page | assigned |
|---|---|---|---|
| 1 | `opus:8-par.c1` | 259 | **PROP-1** |
| 2 | `sol:8-par.c1` | 259 | **PROP-1** (twin of #1) |

Both claims' `extract.quote` fields quote the same clause of the same
sentence on p.259; they differ only in how much of the sentence's tail each
reader carried into the quote (`opus` continues into "This connector is
generally used for a parallel printer.", `sol` stops at the first sentence).
Same proposition, same page — graded together as one testable proposition,
carrying both uids on its assertion, per the pattern the sibling package
`hrm/ch8-ciaa-port-direction` already uses for its own twin uids (e.g. its
PROP-2, carrying `opus:8-dio.c46` and `sol:8-dio.c43` on one line).

## PROP-1 — the general-purpose parallel interface is bi-directional

Page 259 (`opus:8-par.c1`):

> The general-purpose bi-directional parallel interface is a 25-pin
> connector on the back panel of the computer. This connector is generally
> used for a parallel printer.

Page 259 (`sol:8-par.c1`, the same clause):

> The general-purpose bi-directional parallel interface is a 25-pin
> connector on the back panel of the computer.

Restatements: "The parallel interface is bi-directional." (opus) / "The
general-purpose parallel interface is bidirectional." (sol)

**Testable proposition, address and pinout stripped out:** at this
fixture's boundary, "bi-directional" means the interface can carry data in a
direction chosen by software — when configured for output, a value written
by software must be what a subsequent read of the same register returns, not
a fixed echo of something outside software's control regardless of what was
written. This is not tested as a claim about signal levels on the physical
25-pin connector (outside this fixture's boundary) but as the
boundary-level fact the manual's sentence requires to be true of *some*
register this interface exposes: that it has a write side that a read side
can observe, under a direction setting that says it should.

The manual does not, on this page, name a register or a module. The test
grades the proposition at CIA-A port B's data and direction registers
(CIAAPRB/CIAADDRB), at every one of the eight bit positions, with written
values 0 and 1. The port's lines are released first: a line nothing drives
reads high (p.354, "a read of the ports will return all highs because of
passive pull-ups"), while a line the fixture held low from outside would be
a state neither p.259 nor p.354 decides for a driven output.

## Controls and mutations

The sibling package `hrm/ch8-ciaa-port-direction` grades the same shape (a
direction-configured data register that must show write-follows-read
behaviour) for CIA-A's other port. Running the identical measurement (same
fixture, same bus read/write protocol) at that port is this package's
control case: a red on PROP-1, measured on the same basis as a
demonstrated pass, rules out "the fixture cannot deliver a known value" as
the explanation for the red.

- **Fixture liveness.** Before PROP-1 is graded, the harness drives two
  distinct, known values into a direction register on this interface's own
  side (CIAADDRB) and confirms both arrived and read back distinctly —
  proof this exact build of the fixture can deliver a known value at all,
  before anything data-dependent is trusted.
- **Control case, CONTROL-PA0-OUTPUT.** The identical write/read-back
  comparator PROP-1 uses, pointed at CIA-A port A (CIAAPRA/CIAADDRA) bit 0,
  expected to PASS. Not itself a page claim — the measurement-basis control.
- **Released-line control, PORTB-RELEASED-CONTROL.** With CIAADDRB at input
  and port B's lines released the way PROP-1 releases them, a read of
  CIAAPRB must return a 1 at every position (p.341, "you read the actual
  current state of the I/O pins"; p.354). Not itself a page claim.
- **Mutation, PROP-1 (stimulus, green).** A stimulus mutation that turns a
  passing assertion green proves nothing; at PROP-1's red baseline the
  mutation needed is the substitution proof: the identical comparator,
  retargeted by register-select alone (no other code change) at CIA-A port
  A, must go green — proving the comparator itself is not vacuously false.
- **Mutation, control case (stimulus, red).** Omitting the direction-bit
  write the control depends on must turn the control red — proving the
  control is not vacuously true either.
- **Mutation, released-line control (stimulus, red).** Holding the lines
  low instead of releasing them must turn that control red.
- **Mutation, fixture liveness (stimulus, red).** Repeating the same
  stimulus word instead of writing a distinct second word must turn
  liveness red.

Every mutation is a Makefile variant, `MUT ?=` passed on as
`-D$(MUT)=1` to a `MUT_<NAME>` define that defaults to 0 in the test source,
replayable as `make MUT=<NAME> test` — not applied by editing and reverting.
