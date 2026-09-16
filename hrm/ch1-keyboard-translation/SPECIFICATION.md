# Specification — `ch1-keyboard-translation`

Package: `hrm/ch1-keyboard-translation`
Fixture / boundary: `keymap`

    module keymap (input [6:0] code, output [6:0] amiga);

Two ports, purely combinational. This name/port pair and its file
(`amiga_keymap.v`, not `keymap.v` — the module is declared in a
differently-named file) were given directly by the task sheet, along with
the citation `amiga_keymap.v:10-13`. Neither `amiga_keymap.v` nor any other
RTL file has been opened to write this document; see "Order of work" below
for the caveat this citation forces.

Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered
on `classify.package == "ch1-keyboard-translation"`. Manual page 2.

## The two claims

Both claims are the same bare id (`1-components.c19`), one row per reader,
both `classify.status: testable`, both assigned to this package.

> A professional keyboard with numeric keypad, 10 function keys, and
> cursor keys.
— `sol:1-components.c19`, `opus:1-components.c19`

Restatements: sol — "The keyboard has a numeric keypad, ten function keys,
and cursor keys."; opus — "The keyboard has a numeric keypad, 10 function
keys and cursor keys." Same proposition, same page, no wording difference
that changes what is being claimed.

## Scope: this finding-family is NanoMig-specific

**Every red this package could produce would be a statement about NanoMig,
not about the Minimig family.** `keymap` lives in `src/misc`, which holds
NanoMig's own substitutes for Amiga peripherals, not the shared chipset
lineage the way `src/minimig-aga` is. A finding here does not transfer to
other Minimig boards, which solve keyboard translation differently again.
This sentence is required in this file by the build brief and applies to
anything this package eventually asserts.

## What the sentence actually claims

The printed sentence names three groups and one adjective:

1. a numeric keypad exists,
2. **10** function keys exist,
3. cursor keys exist,
4. the keyboard is "professional" — not a threshold, not graded.

This is a page-2 INTRODUCTION sentence: a structural inventory ("these
three groups of keys are present"), not a per-key code claim. Per the
build brief's chapter-1 rule, the assertion to write is the structural one
the sentence actually asks — presence and, for the function-key group,
count — not a re-derivation of which code belongs to which key.

## Checked against the existing suite: already graded, more finely, in `ch8-keycode-table`

Before writing an assertion, the build brief requires grepping
`nanomig-hrm-tests/hrm/` for one already grading the same printed
proposition. `grep -rn "numeric keypad|cursor keys|10 function|ten
function|F1-F10" nanomig-hrm-tests/hrm/` matches exactly one package:
`ch8-keycode-table` (same top, same boundary — `keymap(code, amiga)` —
built from the chapter-8 keycode-table claims, pages 251/252/254).

That package's `test_keymap.cpp` already grades every component of this
sentence, at a finer grain than page 2 asks for:

- **10 function keys** — its `FUNCTION-RANGE` assertion tests exactly this
  proposition: "the ten function-key codes are exactly the contiguous
  range $50 (F1) to $59 (F10)", citing `opus:8-keycode.c30` (p.252: "50-59
  Function keys F1-F10"). A block/count claim, not a per-key one — the
  same shape as page 2's "10 function keys".
- **Cursor keys** — its `KEY-$4C`/`KEY-$4D`/`KEY-$4E`/`KEY-$4F` rows grade
  cursor-up/down/right/left individually, each a PASS at baseline,
  collectively establishing the group's presence in the table.
- **Numeric keypad** — its `KEY-$0F`, `KEY-$1D`-`$1F`, `KEY-$2D`-`$2F`,
  `KEY-$3C`-`$3F`, `KEY-$43`, `KEY-$4A`, `KEY-$5A`-`$5E` rows grade every
  individual numeric-pad key (digits, decimal point, the four operators,
  Enter), collectively establishing the group's presence.

Re-grading the same structural facts here — even worded at page 2's
coarser "does the group exist" level instead of page 252/254's "is each
code right" level — would be pure duplication of an accepted package's
measurement, with no new information and exactly the risk the build brief
warns about: this chapter reaching a different verdict from an accepted
one about the same underlying proposition. **Both claims are therefore
declined**, on `record()` lines in `test_keymap.cpp`, citing
`ch8-keycode-table` and the specific uids above — not re-asserted.

This package's own test source still instantiates `keymap` and includes
one harness-precondition self-check (no claim uid — see the build brief's
"the fixture proves it can deliver a known value before anything reads
one"), so the build is a real, running conformance package rather than an
empty shell, even though it grades nothing itself.

## A claim this package does *not* carry, despite the task sheet's framing

The task sheet describes this package's oracle as "page 2's sentence that
a variety of international keyboards are supported." That sentence is
`1-components.c20`, immediately following `c19` in the source page, and it
is **not** one of this package's two claims — it is not assigned to
`ch1-keyboard-translation` at all. Its own classification (`sol:1-
components.c20`, `opus:1-components.c20`) is `status: not-asserted`,
reason `absent`: `amiga_keymap.v`'s whole interface is one 7-bit input and
one 7-bit output driving one unconditional comparison chain, with no
layout/locale/language selector anywhere in `src/misc` — one fixed map,
no way to ask for a different one, so "a variety ... are supported" is not
constructible at this boundary and carries no package. This package's two
actual claims are both `c19` (the numeric-keypad/function-keys/cursor-keys
sentence), not `c20`. Reported as a task-sheet inaccuracy in the final
report, per the build brief's request to flag what turned out wrong.

## Order of work

The build brief requires this file to be written from the claims'
`extract.quote`/`extract.restatement` alone, committed, and only then the
RTL opened — and states plainly that if the task sheet or claim data have
already supplied `classify.evidence`'s `file:line` citations, that breaks
the order, and to say so rather than pretend it held.

It did not fully hold here. The task sheet handed over
`amiga_keymap.v:10-13` (the port declaration) and a line-cited description
of where each of the three key groups lives in the RTL directly, quoting
`classify.evidence`. Filtering the claims file for this package's rows
also surfaced that same `evidence` text (it is a field on every claim
record, not something separately looked up). Checking the existing suite
for a prior assertion on the same proposition (required, and done above)
additionally surfaced `ch8-keycode-table/test_keymap.cpp`'s comments,
which cite further `amiga_keymap.v` line ranges and specific key-code
values. None of this came from opening `amiga_keymap.v` directly — that
file is still unopened as this line is written — but the file:line and
value information in it reached this document anyway, through the claim
data and through the existing-package check the brief itself requires.
Reported here rather than presented as a clean read.
