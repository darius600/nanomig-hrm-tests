# Specification — `ch1-cia-timers`

Package: `hrm/ch1-cia-timers`
Fixture: **two** independent tops, compiled and run as two separate
binaries — `ciaa` alone and `ciab` alone — each with its CIA submodules
(`cia_int.v`, `cia_timera.v`, `cia_timerb.v`, `cia_timerd.v`), the way
`hrm/ch8-ciaa-port-direction` and `hrm/ch8-ciabprb-disk-control-bits`
already compile them individually. No RTL file has been opened for the
purpose of drafting this document — see the disclosure at the end for what
was already known before drafting began, and why (the task instructions
that assigned this package pre-exposed RTL citations for the timer claim,
the way they did for `hrm/ch8-parallel-port-direction` and
`hrm/ch8-ciabprb-disk-control-bits` before them).
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered
to `classify.package == "ch1-cia-timers"` (4 claims — 2 distinct printed
propositions, each read by both `sol` and `opus`).
Subject: printed pages 6 and 12 — what the CIA (8520) chips are for, in
Chapter 1's introductory, structural sense.

## Claim inventory (all 4)

| # | uid | page | topic | assigned |
|---|---|---|---|---|
| 1 | `sol:1-components.c125` | 6 | "These include modem control, disk status sensing, disk motor and stepping control, ROM enable, parallel input/output interface, and keyboard interface." | DECLINED — boundary |
| 2 | `opus:1-components.c125` | 6 | twin of #1 (identical quote, independent restatement) | DECLINED — boundary |
| 3 | `sol:1-guidelines.c46` | 12 | "The 8520 I/O chips provide timers for developers who are bypassing the operating system..." | **PROP-TIMER** |
| 4 | `opus:1-guidelines.c46` | 12 | twin of #3 (identical quote, independent restatement) | **PROP-TIMER** |

2 claims scored on one proposition (graded twice, once per physical chip);
2 declined, for one reason, stated once below.

## PROP-TIMER — each 8520 (`ciaa`, `ciab`) provides a genuine, programmable timer

Page 12 (`sol:1-guidelines.c46`, `opus:1-guidelines.c46`): "The 8520 I/O
chips provide timers for developers who are bypassing the operating system
(see the Amiga Hardware Reference Manual for more information)."

**Testable proposition:** this is a structural/capability claim about the
chip, not a claim about any particular register bit or timing constant the
page states — the page names no count value, no bit position, no interrupt
vector. What it asserts is that a developer who talks to the CIA's own
registers directly (not through the OS) has a real timer available: a
mechanism that can be loaded with a count, started, and that reaches a
detectable, observable completion after counting down — as opposed to a
register that merely stores a byte and does nothing with time. Grading it
means showing the chip owns such a mechanism and that it responds to time
(clock ticks) rather than to the write itself.

The manual says "the 8520 I/O chips" (plural) — there are two on this
board, `ciaa` and `ciab`, each its own physical chip and its own Verilog
top. A claim naming "the chips" plural is not satisfied by showing it once;
this package accordingly grades the identical proposition once per chip,
against two independent fixtures, and both graded lines carry both uids.

Not graded, because the page does not state it: which of a chip's possibly
several timers is used, one-shot vs. continuous mode, the TOD/alarm
sub-function, an exact bit width, or any absolute time duration (a board
oscillator frequency is outside both UUT roots for this whole chapter, per
the build brief's own warning — irrelevant here too since the page states
no duration to check against).

Status: testable, independently at `ciaa`'s own boundary and at `ciab`'s
own boundary — no `minimig` instantiation or address decoder is needed,
since this is a claim about the chip's own internal capability, not about
which CPU address reaches it.

## Claims not assigned an assertion (2 of 4)

Both `sol:1-components.c125` and `opus:1-components.c125` (p.6, "These
include modem control, disk status sensing, disk motor and stepping
control, ROM enable, parallel input/output interface, and keyboard
interface") are declined, for a boundary reason, not a testability doubt
about the underlying facts.

`ciaa` and `ciab`, compiled alone, expose only generic 8-bit parallel
ports (`porta_in`/`porta_out`/`ddra` and `portb_in`/`portb_out`/`ddrb`) and
a generic serial shift register. Nothing at this boundary carries a name
like "modem control" or "disk motor" — a generic port write reaching a
generic port read proves the port works, not which of the six listed
duties it is wired to serve. That binding — which pin carries which named
signal — is a fact one level above this package's declared tops (`ciaa`,
`ciab`), the same "generic 8520 port, name binding one level up" boundary
`hrm/ch8-ciabprb-disk-control-bits`'s own specification already argued for
16 of its 30 claims, and the same one `hrm/ch8-cia-disk-port-windows`
independently reached for its own two-ports claim ("driving a generic,
hand-picked bit pattern proves two CIA cores are addressable, not that the
DISK SUBSYSTEM is what uses them").

Four of the six named duties already have a specific, named-signal
assertion elsewhere in this suite (not the same page-6 sentence, but the
same underlying fact, measured more precisely, at a top that can see the
wiring): disk status sensing and disk motor/stepping control at
`hrm/ch8-cia-disk-signal-bindings`; keyboard interface at
`hrm/ch8-keyboard-serial-register`; parallel input/output interface at
`hrm/ch8-parallel-port-direction` (which found the interface's output half
does not exist at this boundary — `ciaa`'s Port B write path is commented
out). This package does not re-derive or re-grade any of those; they are
cited only as orientation for the reader of the record() line, not as "the
same proposition already graded," since none of them grades page 6's own
sentence.

The remaining two — ROM enable and modem control — have no assertion
anywhere in this suite as far as a grep of `hrm/` found (checked before
writing this document). Both are out of reach at this package's declared
boundary regardless: ROM enable is realized entirely outside the CIA
module in this design (board-level overlay logic, not a `ciaa`/`ciab`
port at all — `hrm/ch7-overlay-source` grades a related but different
proposition, on a different page, about the OVL bit's effect once it
reaches the overlay logic, and is not re-graded here); modem control's
specific signals (CD/CTS/DSR/RI/DTR/RTS) are a `minimig.v`-level wiring
fact this package's two tops cannot see.

## On a control case, fixture liveness and mutation

Per the build brief: a fixture-liveness proof precedes any data-dependent
grading; at least one control case is expected to PASS on the same
measurement basis; each graded line (fixture-liveness included) carries a
recorded stimulus mutation, in the source, in the fixed `// MUTATION
<label> <oracle|stimulus|none> <red|green|impossible> <what changed>` form.

- **Fixture liveness**, once per fixture: two distinct, known bytes written
  to a register this claim does not itself depend on (so liveness does not
  share a mechanism with what PROP-TIMER grades) must both arrive and read
  back correctly before PROP-TIMER is graded.
- **Control.** PROP-TIMER is expected GREEN at both `ciaa` and `ciab` — a
  real, working feature, not a suspected defect — so the within-assertion
  shape itself carries the control: before the loaded count has had time to
  count down, the completion signal must read inactive; only after the
  right number of clock ticks does it read active. A comparator that cannot
  read false is not proof of anything, so this before/after split is the
  same-measurement-basis control the build brief asks for, rather than a
  separate labeled assertion duplicating it.
- **Mutation.** PROP-TIMER's mutation, at each fixture, withholds the clock
  ticks the count-down needs (stimulus), which must turn the completion
  signal's read from PASS to a clean, unambiguous FAIL — proving the
  assertion is not vacuously true. Concrete register offsets and tick
  counts are step-2 material, fixed once the RTL is open, per the order of
  work.

## Disclosure — what was already known before this file was drafted

This file is written from the 4 claims' `extract.quote` / restatement
content and the task instructions' own prose. The order of work this
pipeline asks for — write the specification from the quotations alone,
commit it, and only then open the RTL — did not fully hold for the timer
claim, and this is disclosed rather than papered over, exactly as the build
brief asks:

The task instructions that assigned this package stated, before this
document existed: "A classifier measured `ciaa.v:331/351`, `ciab.v:195/215`,
the counter at `cia_timera.v:17`, and `eclk` at `:31`." That is RTL detail
— file:line citations naming a specific counter module and its clock-enable
signal — handed to the builder ahead of step 1, the same situation
`hrm/ch8-parallel-port-direction` and `hrm/ch8-ciabprb-disk-control-bits`
both disclosed for themselves. The task instructions also stated, as
background for the package as a whole and not part of this claim: "`ciaa`
declares no port-B output... That belongs to another package's claim" —
this shaped the decision to decline the page-6 parallel-I/O item to
`hrm/ch8-parallel-port-direction` rather than re-deriving it, but named no
port relevant to PROP-TIMER.

What that pre-exposure did not do: it did not supply a register map, a bit
layout, an interrupt-mask convention, or any value used above as an
expected result — PROP-TIMER's wording ("loaded, started, counts down,
reaches a detectable completion") comes from the page-12 quote alone, and
would read identically had the counter's module name and `eclk`'s line
number never been mentioned. No expected value in this document was chosen,
adjusted, or narrowed because of the pre-exposed citations; they confirm
that a timer mechanism exists to be graded, not what grading it should
require. The concrete register offsets, the count value, the tick sequence,
and the mutation implementation are step-2 material, written only after
this file is committed and `ciaa.v`/`ciab.v`/`cia_timera.v`/`cia_int.v`
have been opened directly by this builder.
