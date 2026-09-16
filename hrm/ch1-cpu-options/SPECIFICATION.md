# Specification — `ch1-cpu-options`

Package: `hrm/ch1-cpu-options`
Fixture: `sysctrl` (compiled alone). The exact RTL filename and internal port
names are not named here — per the build brief's order of work, that is
step-2 material, opened only after this file is committed.
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered
for `classify.package == "ch1-cpu-options"` (4 claims).
Subject: printed page 1's two "Components of the Amiga" bullet-list items
that name `sysctrl` in their `classify.evidence` — the main-processor/CPU-
options sentence and the base-RAM/expansion-RAM sentence.

This file is written from the `extract.quote` / `extract.restatement` fields
of the 4 claims, cross-read against the primary source text for page 1
(`hrm-testpipeline/docs/references/amiga-hrm-3rd/ch1/p1.txt`). It fixes the
printed propositions to be tested; concrete stimulus values and the
`expect()` assertions themselves are added only after this file is
committed and step 2 (opening the RTL) begins, per the build brief.

## Disclosure — the order of work did not fully hold

The build brief requires writing this file from quotations alone, before any
`file:line` RTL citation is seen, and calls reading `classify.evidence`
first a HALT. That could not be avoided here: `classify.evidence` lives in
the same JSON records as `extract.quote`/`page`/`restatement`, in the same
`work/2026-09-16_ch1_packages.jsonl` file that had to be filtered by
`classify.package` to find this package's 4 claims in the first place. Doing
that filter necessarily surfaced the evidence text alongside the quotations
in the same read. This is the same situation `ch8-ciaa-port-direction` and
`ch8-potgo-register-bit-layout` recorded for themselves, and is disclosed
the same way: the evidence text was seen, but nothing below is drawn from
it. Every proposition and testability judgement in this file is stated using
only `extract.quote`, `extract.page`, `extract.restatement`, and the primary
page-1 source text. No Verilog port, wire, register-select value, or
`file:line` location is named anywhere in this file. The task instructions
that dispatched this build also stated, ahead of any claim data, that a
classifier had measured `sysctrl.v:230` and that it "offers 68000/68010/
68020 and stops there" — that fact, too, was seen before this file was
written, for the same unavoidable reason (it was in the dispatch, not
something this builder went and looked up), and is likewise not used to
shape wording below beyond the scope note already implied by PROP-1's own
printed text. It is treated as unconfirmed until step 2 opens the file
directly.

## Claim inventory (all 4)

| # | uid | page | proposition |
|---|---|---:|---|
| 1 | `sol:1-components.c7` | 1 | **PROP-1** |
| 2 | `opus:1-components.c7` | 1 | **PROP-1** (twin) |
| 3 | `sol:1-components.c12` | 1 | **PROP-2** |
| 4 | `opus:1-components.c12` | 1 | **PROP-2** (twin) |

Both propositions carry two uids — one per reader — over the same bare id
(`c7`, `c12`), consistent with the brief's note that 96% of this chapter's
bare ids are shared by both readers. All 4 claims are accounted for: 2
propositions, no out-of-scope claims to decline in this package (unlike the
`ch8-ciaa-port-direction` model, this package has no sibling package
claiming any part of the same two sentences — see "Duplicate-grading check"
below).

## Duplicate-grading check

Per the build brief: "grep `../nanomig-hrm-tests/hrm/` for one on the same
proposition" before writing an assertion. Done before this file was written:
`grep -rli "68010\|68020\|68030"`, `grep -rli "sysctrl"`, and
`grep -rli "system_chipmem\|system_slowmem\|system_fastmem"` across every
`.cpp`/`.md`/`Makefile` under `nanomig-hrm-tests/hrm/` returned nothing on
this proposition (three incidental numeric hits inside generated
`obj_dir/*.cpp` Verilator output are build artefacts, not test content, and
do not name this proposition). No existing package tests `sysctrl` at all,
and none tests the CPU-options list or the chip/slow/fast RAM ladder. No
`classify.note` on either of these 4 claims names an existing package
either (consistent with the brief's "zero over all 235 testable claims").
Nothing here is re-grading an accepted verdict.

## PROP-1 — the Amiga's optional main processors

Page 1 (`sol:1-components.c7`, twin `opus:1-components.c7`):

> Motorola MC68000 16/32-bit main processor. The Amiga also supports the
> 68010, 68020, and 68030 processors as an option.

Restatement (sol): "The Amiga optionally supports 68010, 68020, and 68030
processors."
Restatement (opus): "68010, 68020 and 68030 are supported as options." (also
notes a scan artefact: the source PDF's "68010,68020," has lost a space
after the first comma — a scanning defect, not a content difference; both
readers' restatements read past it correctly.)

**Testable proposition, structural:** this is a components-list sentence
about which processor *options* the machine offers, not about how any of
those processors internally executes instructions (a different claim,
already carved out by the classifier's own note distinguishing this from
claim c6, "what a processor IS internally"). The Amiga's family of models
share a "common hardware nucleus" (page 1's own introductory paragraph);
that shared nucleus is expected to expose which CPU family member is fitted
as a selectable, observable option, not to hardwire one silently. The sharp,
gradeable question this chapter's own framing calls for ("is this port on
this module") is: does `sysctrl` expose a CPU-selection option, and if so,
which of 68000/68010/68020/68030 does its option set actually cover against
the four the page names?

Status: testable. Whether the option set `sysctrl` implements covers all
four processors the page names, or stops short of one or more of them, is
the measurement — not assumed here, reported once measured.

## PROP-2 — base RAM and its expansion ceiling

Page 1 (`sol:1-components.c12`, twin `opus:1-components.c12`):

> From 256K to 2 MB of RAM expandable to a total of 8 MB (over a gigabyte on
> the Amiga 3000).

Restatement (sol): "Base RAM ranges from 256K to 2 MB and can expand to
8 MB, or to more than 1 GB on the A3000."
Restatement (opus): "RAM runs from 256K to 2MB, expandable to 8MB total and
over a gigabyte on the A3000." This reader's crossread also notes the figure
conflicts with page 2 and Table 1-1, which both give 9 MB as the maximum
total RAM — recorded there (crossread note on `opus:1-components.c12`) as a
source conflict between two pages of the manual. It does not change what is
observable at this fixture's boundary and is not re-litigated here; if page
2's 9 MB figure is graded elsewhere, that is a separate claim (`c29`, per
the crossread note) on a separate proposition.

**Testable proposition, structural, address-independent:** the "over a
gigabyte on the Amiga 3000" clause names a different model's memory
controller, has no `sysctrl` counterpart, and is not graded here (consistent
with the classifier's own scope note on this claim). What remains is a
components-list statement about the Amiga's memory being configurable
across a defined range: a base (Chip) RAM figure whose ceiling is 2 MB, and
a total expanded to 8 MB. The sharp, gradeable question is structural: does
`sysctrl` expose a memory-size configuration option whose base-RAM ceiling
is 2 MB, and (separately) an expansion-RAM option whose ceiling is 8 MB —
matching the two headline figures the sentence gives, not necessarily that
every combination of every memory-size option sums to exactly 8 MB, which
the sentence does not itself claim (it gives one base figure and one total
figure, not an arithmetic identity over every configurable region). Whether
`sysctrl`'s memory-configuration ports show ceilings matching 2 MB and 8 MB
is the measurement.

Status: testable, contingent on step 2 confirming `sysctrl` alone (without
the address-decode fabric that consumes its output) still exposes these
configuration values as observable ports — if the values only ever reach
observability through a downstream address decoder outside `sysctrl`'s own
boundary, that is a HALT for this proposition specifically, to be reported,
not assumed here.

## On a control case and mutation

The build brief requires (a) a fixture-liveness proof before any
data-dependent grading, (b) at least one control case expected to PASS on
the same basis as anything under suspicion, and (c) a stimulus mutation per
assertion, recorded as `// MUTATION <label> <oracle|stimulus|none>
<red|green|impossible> <what changed>` in the C++ source, replayable as a
`-D` / `MUT=` build variant. Concrete stimulus values and port names are
step-2 material and are not fixed here; the shape of each is already
visible from the claims alone:

- **Fixture liveness.** Before PROP-1 or PROP-2 is graded, the harness must
  drive at least two distinct, known configuration values into `sysctrl`'s
  input path and confirm — by some write-dependent effect visible at the
  boundary — that both distinct values actually arrived and did not
  silently collapse to one value or to nothing.
- **Control case.** Both PROP-1 and PROP-2 concern an option *set*, not a
  single value, so each needs its own in-range control: a value inside the
  claimed range that the fixture is expected to (and, once measured, does)
  accept and report back correctly, on the same measurement basis as the
  boundary values under suspicion (68030 for PROP-1; the 2 MB/8 MB ceilings
  for PROP-2). Without such a control, a red on a boundary value cannot be
  told apart from the harness itself being unable to drive or read
  `sysctrl` at all.
- **Mutation, PROP-1.** The natural stimulus mutation substitutes a
  configuration value the fixture does not claim to support at all (outside
  any option this port defines) in place of the value the assertion probes,
  leaving the expected value and label alone — this should go red if the
  probed value was itself a real, distinct option, and is otherwise the
  substitution proof that the comparator is not vacuously true.
- **Mutation, PROP-2.** The natural stimulus mutation substitutes a memory
  configuration value one step past the ceiling being checked (if such a
  step exists in the option encoding) in place of the ceiling value,
  leaving the expected value and label alone.
- Whether either mutation is `impossible` in this implementation, and if so
  which kind, is undecided until step 2 shows the actual port list and
  option encoding; it is not assumed here.

This section states a strategy, not the assertions themselves; the actual
`expect()` calls, concrete stimulus values, and `// MUTATION` header lines
are written in step 2, after the RTL is opened.

## Two things this build is told to say, but that do not touch this package's claims

The dispatch that produced this package carries two chapter-wide cautions
that are recorded here for completeness, though neither claim above needs
either of them:

- `sysctrl` lives in `src/misc` — NanoMig's own substitute for whatever
  configuration mechanism a real Amiga board uses — not in the shared
  chipset lineage (`agnus`/`paula`/`denise`/`gary` and their submodules). A
  finding at this boundary is a finding about NanoMig's own reimplementation
  of "which CPU options exist," not about the custom chips other chapters
  grade, and is scoped that way below wherever the measurement result is
  reported.
- No absolute clock frequency is gradeable in this chapter (a board
  oscillator outside both UUT roots supplies the megahertz; the PAL/NTSC
  switch that does exist never reaches the clock generator). Neither PROP-1
  nor PROP-2 makes a frequency claim, so this does not constrain either
  assertion here — noted only so a later reader does not wonder why no
  clock-rate check appears in a CPU-options package.

## What this specification deliberately does not contain

Port names, signal names, register-select encodings, and concrete stimulus
values are absent by design, per the build brief's order of work — they are
added only after this file is committed and the RTL is read for step 2.
