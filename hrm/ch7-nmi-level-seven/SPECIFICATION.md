# Specification — `ch7-nmi-level-seven`

Package: `hrm/ch7-nmi-level-seven`
Fixture: `minimig` (the whole top-level module — see "Scope and limits" below)
Claims file: `hrm-testpipeline/work/ch7-build/ch7-nmi-level-seven-spec-stage.jsonl` (3 claims)
Subject: Figure 7-4's NMI row: hardware priority 7, no Exec software priority.

This file is written from the `extract.quote` / `restatement` fields of the 3
claims alone. No RTL file for this package has been opened, and none of
`ch7-nmi-level-seven.jsonl`, `STEP3-NOTES-ch7-nmi-level-seven.md` has been
read — both are step-3 material per the task sheet and are deliberately left
unopened until this file is committed. Port names, signal names, compile-time
defines, concrete stimulus values, and the `expect()` assertions themselves
are added only after this file is committed and step 3 (opening the RTL under
`minimig-aga/`) begins.

## Scope and limits — read before anything else in this file

This package compiles RTL that no shipped configuration in this tree
compiles: the NMI path exists only behind a compile-time define that is not
set by any build in `nanomig-hrm-tests`. This is known already, from the
task sheet, not from having opened the RTL. Two consequences follow, and
both are stated here so they travel with the specification rather than
being visible only in a step-3 file header:

- **A green result from this package is not evidence about the shipped
  design.** It can only be evidence about the behaviour of the RTL under
  the non-default define this package must enable to compile the path at
  all. Any report drawn from this package must say so explicitly rather
  than presenting a pass as though it exercised what actually ships.
- **The precondition itself must be checked, not assumed.** A build that
  silently fails to enable the define would still produce *some* interrupt
  behaviour on the fixture (whatever the shipped default does), and a check
  written carelessly against "level 7" could pass or fail for reasons that
  have nothing to do with NMI at all. Per `BUILD-BRIEF.md`'s instruction —
  "where a green depends on a precondition that might never occur, assert
  the precondition too" — step 3 must include an assertion (or an
  equally explicit check reported through `expect`) that the build actually
  compiled with the NMI path enabled, before any proposition below is
  graded on that build. This specification cannot name the define yet; that
  naming is step-3 work.

The task sheet also warns that the fixture is the whole top-level module,
that reset alone takes over two million clock ticks, and that if the build
or run turns out impractical that is a HALT to be reported, not a smaller
module to substitute quietly. This specification does not change scope to
work around that; if step 3 finds the full fixture impractical, that is
reported as required, not solved by narrowing the fixture unannounced.

## Claim inventory (all 3)

| # | uid | page | role |
|---|---|---|---|
| 1 | `opus:7-int.c78` | 221 | PROP-1 (hardware-priority portion), primary; also carries a non-hardware portion, declined below |
| 2 | `sol:7-fig7-4.c48` | 221 | PROP-1, twin of #1's hardware-priority portion |
| 3 | `opus:7-fig7-4.c48` | 221 | naming/identification, cited jointly with PROP-1, not separately scored |

All 3 claims are accounted for; none is left unassigned at this stage.

## PROP-1 — the NMI condition is asserted at hardware priority 7

Page 221.

> &nbsp;&nbsp;&nbsp;7 -----|\_\_\_\_ --        non-maskable interrupt       NMI
— `opus:7-int.c78`, restated as: "Figure 7-4: the non-maskable interrupt,
NMI, has hardware priority 7 and no Exec software priority (the cell reads
'--')."

> [FIGURE 7-4 -- figures/p221.png -- not text; open the image and read it]
— `sol:7-fig7-4.c48`, restated as: "In Figure 7-4, the hardware-priority
cell numbered 7 spans the NMI/non-maskable-interrupt row." The claim's own
crossread (`opus-crossread-fig-A`) reads past the layout wording to the
proposition it carries: "its content is the interrupt level the chip
drives for this source, i.e. NMI raises hardware level 7. That is testable
against the RTL (INTREQ/INTENA bit to IPL encoding)." That crossread note
is taken here as the reading of record for this claim.

Both claims converge on one proposition, which this package tests:

**Proposition to test:** when the condition Figure 7-4 calls "non-maskable
interrupt" (row NMI) is asserted, the interrupt priority level the module
presents toward the CPU is 7 — the same level the manual prints as this
row's hardware priority, and (per the row's own name) one that cannot be
masked off.

**Correlation, not two independent observations (repair, ch7 slice B1
independent validation, finding 6).** "The interrupt priority level the
module presents toward the CPU is 7" is a statement about one cycle, not
about two separate facts ("the NMI condition happened at some point" and
"the module's output was 7 at some point") that could be true for
unrelated reasons. Step 3 must grade this by requiring the NMI condition
and the level-7 presentation to be observed in the same cycle, not by
latching each independently over a monitoring window and requiring only
that both eventually became true.

This is the only proposition PROP-1 tests. It is **not** the proposition
that no other source in this module can ever produce level 7 — the claims
file's own `classify.note` on `sol:7-fig7-4.c48` says that complement "is
carried by `sol:7-interrupts.c2` and `opus:7-int.c3` and is asserted there,"
i.e. in a different package. This package stays inside the boundary its
own claims set, not the neighbouring one.

**Chipset scope:** OCS. Taken from `opus:7-int.c78`'s own
`extract.chipset_basis`: "Chapter 7 names no chipset. Printed p.224 closes
the chapter with 'ECS system control. For information on the system
control registers in the Enhanced Chip Set (ECS), see Appendix C.' — so the
chapter body is the original (OCS) account and the ECS account is appendix
C's." Figure 7-4 is chapter-7 body material (p.221), so this proposition is
scoped to OCS, not derived from any RTL reading.

**Identification of "NMI"**, supporting PROP-1 without its own separate
score:

> [FIGURE 7-4 -- figures/p221.png -- not text; open the image and read it]
— `opus:7-fig7-4.c48`, restated as: "In Figure 7-4 the 'Description' cell
of the row whose fourth column reads NMI holds the text 'non-maskable
interrupt'. The name NMI denotes the interrupt raised by the condition
'non-maskable interrupt'."

This claim ties the row's name column ("NMI") to its description column
("non-maskable interrupt") — it identifies which condition PROP-1 is about,
but states no priority-level value of its own that could be independently
right or wrong in hardware terms. It is folded into PROP-1's wording (the
phrase "the condition Figure 7-4 calls 'non-maskable interrupt' (row NMI)"
above is exactly this identification) rather than made into a fourth
proposition. See "Claims not assigned an assertion" below.

Status: testable.

## Declined — "no Exec software priority" is not a hardware proposition

`opus:7-int.c78`'s own `restatement` bundles two things: the hardware
priority (taken into PROP-1 above) and "no Exec software priority (the
cell reads '--')." The claim's own `classify.split_note` separates these
explicitly: "the claim also states the Exec cell reads '--', which is exec
ordering and not-hardware."

Exec software priority is AmigaOS's own interrupt-server ordering — a
property of a linked list Exec's `AddIntServer()` maintains in software,
not a signal the custom chipset drives, holds, or could be made to drive
by any compile-time define. A Verilog fixture of this hardware has nothing
to sample for it: there is no port, register, or timing behaviour that
"no Exec software priority" could be checked against, with the NMI path
enabled or without it. Per `BUILD-BRIEF.md`'s distinction between the three
verbs — "a row the manual does not rule on gets a `note` or a `record`, not
a pass/fail" — this is exactly the `record()` case: not silence from the
manual (the manual is explicit that the cell reads "--"), but a claim whose
subject sits outside what hardware simulation can observe at all. Step 3
must `record()` this claim's second half with this reason; it must not be
asserted, and it must not be quietly dropped either.

## On a control case and on mutation

`BUILD-BRIEF.md` requires at least one control case expected to PASS on the
same measurement basis as the case under suspicion, and a stimulus mutation
per assertion. Concrete stimulus values and port/define names are step-3
material and are deliberately not fixed here, but the shape of both is
already visible from the claims alone:

- **Control case:** the same measurement path that reads "the interrupt
  priority level the module presents toward the CPU" must first be shown,
  on a stimulus where the NMI condition is *not* asserted, to present
  something other than level 7 (idle, or a lower level from whatever
  ordinary stimulus the fixture needs to reach an observable state at all).
  Without this, a PASS on PROP-1 does not distinguish "the module correctly
  encodes NMI as level 7" from "the measurement always reads back 7
  regardless of what happened," which is precisely the silently-dead-path
  failure `BUILD-BRIEF.md` warns about under "the fixture proves it can
  deliver a known word before anything reads one." This control is on the
  same basis as PROP-1 (the same output, read the same way) and is expected
  to PASS.
- **Mutation for PROP-1:** change the stimulus so the NMI-triggering
  condition is *not* asserted (or is asserted on a different, non-NMI
  path if the fixture offers one), leaving the expected value ("level ==
  7") untouched. The check must go red, because a level-7 presentation
  with no NMI condition asserted is not what the proposition claims. This
  changes what the module is asked to do, not the expected value, per
  `BUILD-BRIEF.md`'s definition of a mutation.
- **Precondition check:** as noted under "Scope and limits," step 3 must
  also establish, and report through `expect` or an equally explicit
  check, that the build actually has the NMI path's compile-time define
  set — this is not a mutation of PROP-1, it is the gate PROP-1's grading
  depends on.
- **Mutation for the correlation repair, added after step 3:** because
  `_cpu_ipl` is a continuous assignment purely of `int7` at this
  boundary, and Paula's own priority encoder never legitimately outputs
  level 7 on any input pattern, no *real* stimulus on this correct RTL can
  ever present the NMI condition and the level-7 output apart in time —
  correlation always holds here by construction. A stimulus mutation
  therefore cannot exercise the correlation-checking logic itself; step 3
  instead fabricates the false-positive sequence directly (forcing the
  `_cpu_ipl` port from the harness) to prove the repaired check rejects
  data an independent-flags check would have accepted. See the test
  file's own header and `MUT_DECORRELATE` for the exact mechanism and its
  run record.

This section states a strategy, not the assertions themselves; the actual
`expect()` calls, their exact stimulus values, the define's name, and the
`// MUTATION` header lines are written in step 3.

## Claims not assigned an assertion

None outright unassigned. `opus:7-fig7-4.c48` is not made into its own
scored proposition because it states no priority-level value of its own —
it identifies which condition ("non-maskable interrupt", row NMI) PROP-1's
priority-level claim is about, and is cited jointly with PROP-1 for that
reason, in the same manner a legend claim with no address of its own was
folded into the propositions it modified in a sibling ch7 package. The
non-hardware half of `opus:7-int.c78` ("no Exec software priority") is
explicitly declined (recorded, not asserted) in its own section above,
per its own `classify.split_note`. `sol:7-fig7-4.c48` is folded into
PROP-1 as a twin of `opus:7-int.c78`'s hardware-priority half. All 3
claims are therefore accounted for: 1 tested proposition (PROP-1, cited by
2 claims plus 1 identification claim), 1 declined non-hardware half.

## What this specification deliberately does not contain

Port names, signal names, the compile-time define's name, and concrete
stimulus values are absent by design, per `BUILD-BRIEF.md`'s order of
work — they are added only after this file is committed and the RTL under
`minimig-aga/` is read for step 3. This file also does not contain the
proposition that no other interrupt source in this module can produce
level 7; per `sol:7-fig7-4.c48`'s own `classify.note`, that complement
belongs to a different package (`sol:7-interrupts.c2`, `opus:7-int.c3`) and
is out of scope here.
