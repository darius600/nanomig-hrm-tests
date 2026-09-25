# Adding a test

For a developer who wants to add or change a package in this suite. It is short
on purpose; the long form is the pipeline's
[`chapter-playbook.md`](https://github.com/darius600/hrm-testpipeline/blob/main/docs/chapter-playbook.md),
which is written for an automated run and carries a great deal you do not need.

**The rules below are the ones a reviewer will actually check.** Every one of
them exists because leaving it out produced a wrong result at least once.

## The one rule everything else serves

**The expectation comes from the manual, never from the RTL.** Write down what
the printed page requires *before* you open the Verilog. If you read the code
first you will write an assertion that describes it, and a test that agrees with
the implementation by construction proves nothing.

A red result is a **result** here, not a broken test. Adjusting an assertion so
that it passes deletes a finding, and that is the one failure mode this suite
exists to prevent.

## Two examples, and read them in this order

**The shape, in one screen:** [`docs/example-package/`](example-package/) is a
real package — it builds, it grades one bit of CIAA's `DDRA` against printed
page 292, and `make mutate` turns it red. About a hundred lines including the
header, of which about thirty are the test. Start here if you have not written
one before; it shows the header fields, the three verbs, the mutation guard and
its `// MUTATION` record, and nothing else.

It lives in `docs/` rather than `hrm/` on purpose: `run_all.sh` and
`check_published_counts.py` both count the directories under `hrm/`, and an
example is not a package.

**The whole thing:**
[`hrm/ch3-beam-counters-and-blanking`](../hrm/ch3-beam-counters-and-blanking)
grades `BPLCON0`'s `ERSY` bit and shows every rule below in place: the header
block, per-assertion page citations, a control on the same measurement basis,
and the mutation lines. It is 1,749 lines, which is what a package looks like
when it carries a chapter's worth of claims — go to it once the shape is
familiar, and read it for the rules rather than end to end.

## What a finished package has

**A header that says what it is.** Test purpose, test class, the printed pages
it answers for, and — this one gets forgotten — **non-goals**: what a reader
might expect to be covered here and is not, with the reason. Usually the reason
is that the signal is not a port of the module under test.

**One assertion per proposition**, each naming its printed page. Never a
collective *"all cases passed"*: when it goes red, nobody can tell which
sentence of the manual is in question.

**The claim id on the graded line.** Coverage is counted by reading the assertion
labels, so an id that appears only in a comment counts as uncovered. And an id
whose claim bundles a clause this package does not test is withdrawn whole rather
than kept with a caveat in the prose — a uid on a graded line counts as
*asserted*, whatever the surrounding sentence says.

**At least one control that passes on the same measurement basis** as anything
under suspicion. Without it a red does not survive the obvious objection — that
the fixture counts in the wrong units, or is not driving the module at all.
Eight blitter assertions once came out at exactly half the manual's figure and
looked like a strong finding; the cause was one clock-enable defect in the shared
fixture.

**A stimulus mutation per assertion**, recorded in the header as

```
// MUTATION <assertion-label> <oracle|stimulus|none> <red|green|impossible> <what changed>
```

where `<assertion-label>` is **exactly the label passed to `expect()`** — not the
`-D` define name. Change what the module is *asked to do*, leave the expected
value alone, confirm the assertion goes red, revert. Flipping an expected value
proves only that a boolean can be false.

**If the assertion is already red**, the usual mutation is unavailable: a failing
assertion cannot be made to fail harder. Put a *different* mechanism, shown live
elsewhere in the same run, through the same comparator and show it goes green.
That is what such a mutation has to establish — that the comparator is not
vacuous.

**If a mutation is impossible, say which of two things that means.** Either
*unfalsifiable at this boundary* — no stimulus in the module's whole input space
can break it, so the green is a property of the operand widths rather than of the
design — or *not exercised, by a scope decision*, naming the path you chose not
to drive. They are not the same and one word hides the difference.

**A truthful exit code.** A package with a failing assertion must exit non-zero,
and its summary must still be printed. If the `Makefile` redirects the binary to
`out.txt`, preserve the exit status across the `cat`:

```make
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc
```

Without that, `make` stops before printing and the package **disappears from the
suite's own count exactly when it goes red**. That was true of 59 packages until
18.09.2026 and cost the published figure 81 failing assertions.

**A clean rebuild.** `make clean && make test` from the package directory, and
the whole suite still green-for-green via `./run_all.sh` — a new package must not
change any other package's result.

## What not to do

- **Do not modify anything under `nanomig/`.** It is a pinned submodule and the
  implementation under test.
- **Do not add debug ports** to reach a signal. If the module's own boundary
  cannot observe the claim, that is a finding about the boundary; record it
  rather than widening the interface.
- **Do not adjust an expectation to make a test pass.** The single exception is
  an assertion that misstates the printed page — then the test is the thing being
  fixed, and the commit message has to say which sentence it got wrong.
- **An emulator is not an oracle.** WinUAE, vAmiga, Denise and Copperline are
  read for context in [`findings/`](findings/), and none of them decides
  anything. The manual decides.

## Where the claims come from

All 216 packages in the suite came through the pipeline, and each cites a claim
file in the sibling repository,
[`hrm-testpipeline/work/`](https://github.com/darius600/hrm-testpipeline/tree/main/work).

**You do not need one to add a test.** A package that cites its printed page and
fixes the expectation before opening the RTL is welcome; what the pipeline adds
on top is a second independent reading of the same pages and a per-claim ledger,
and a package without those is not thereby worth less — it just does not carry a
completeness claim about its chapter. What a green means either way is defined in
[`README.md`](../README.md#what-a-green-means).

An earlier suite of 23 packages had no claim file because it had no manual
citation either: its oracle was a frozen module specification, written by reading
the Verilog. Twenty of those are gone and
[`legacy-names.md`](legacy-names.md) maps their names. That is the one shape of
package that is not welcome — not because it was hand-written, but because a
description of the implementation cannot decide whether the implementation is
right.
