# NanoMig HRM Tests

**Why this exists.** Minimig is already a mature core, and NanoMig is built on
it. This repository tries to offer means to improve it a bit further: it reads the
chips' original documentation statement by statement, tests the RTL against
each one, and offers what it finds to the maintainers of NanoMig and of the
Minimig lines it descends from, as questions rather than verdicts. The reading,
the tests and the finding pages were produced by AI models (Anthropic's Claude
and OpenAI's Codex) under human direction. As in any review, whoever does it, a
finding can be mistaken, so every finding page names its sources for the
reader to check.

Conformance tests for the Amiga chipset RTL in
[NanoMig](https://github.com/harbaum/NanoMig). **The only oracle is printed
literature:** two documents Commodore wrote about the chips. Every assertion
in `hrm/`, `aa/` and the graded packages of `integration/` grades against a
page its package names, and no expected value
comes from NanoMig, from another core or from an emulator. Emulators appear
on the finding pages as witnesses, never as the judge.

| Oracle | Documents | Tests |
|---|---|---|
| *Amiga Hardware Reference Manual*, 3rd edition | the original chip set (OCS) and the Enhanced Chip Set (ECS) | `hrm/`, 217 packages; `integration/`, 17 graded packages |
| Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)* | the AGA chip set | `aa/`, 33 packages |

**`integration/` runs whole systems** rather than one module: `nanomig.v`
with a 68000 program assembled by vasm, on both CPU cores NanoMig offers
(fx68k and TG68K). Seventeen packages there are graded against the manual, like
`hrm/`, for claims that only show across module boundaries, and seventeen more
are specified and not yet built. A red in them becomes a finding only once a
validation has confirmed it against the page, and
[`docs/findings.md`](docs/findings.md) names the lines that did.
[`integration/README.md`](integration/README.md) lists the packages, what each
grades, and the harness checks and smoke test beside them, which are graded
against no page.

**If you have one evening,** [the shortlist](docs/findings.md#if-you-have-one-evening)
names three findings to start with and what each takes.

**The printed page is the oracle, never the RTL.** A red test in `hrm/`,
`aa/` or a graded package of `integration/` marks a place where the RTL does
something the page does not describe — in `integration/`, once a validation
has confirmed it. It is a result, not a bug in the test:
adjusting an assertion to make it pass deletes the finding.

This is an add-on, not a fork. NanoMig comes in as a submodule, pinned to the
commit the tests were verified against; nothing under `nanomig/` is modified.

## Run it

```sh
git clone --recursive <this repo>
cd nanomig-hrm-tests
./run_all.sh
```

`./run_all.sh --jobs 8` runs up to eight packages of each tree at a time and
prints the same lines in the same order. A full run that way took 30 minutes on
a 16-thread machine, most of it the integration packages.

Requires Verilator (last verified with **5.052**) and `make`; the tests at
`nanomig.v`'s own ports under `integration/` also need the 68000 assembler
vasm (`vasmm68k_mot`, from http://sun.hasenbraten.de/vasm/, last verified
with 2.0f) on the PATH. If *every* package reports a build error, the
toolchain is missing rather than the tree broken; on Windows these usually
live only inside WSL.

Cloned without `--recursive`? `git submodule update --init`. NanoMig checked
out elsewhere? `make -C hrm/<package> NANOMIG=/path/to/NanoMig`. Against
another Minimig core, name the RTL file on the command line, for example
`make -C hrm/ch5-audio-location-pointer SRC=/path/to/Minimig-AGA_MiSTer/rtl/agnus_audiodma.v test`,
after looking up the variable in the package's `Makefile` (most use `SRC`,
`SRC_DIR` or `RTL`). A red against another core is not a finding here.

## Where the suite stands

| | |
|---|---:|
| test packages, `hrm/` | **217** |
| assertions, `hrm/` | **3670** — 3482 pass, **188 fail** |
| packages, `hrm/` | 155 green, 62 carrying at least one red, **0 build errors** |
| test packages, `aa/` | **33** |
| assertions, `aa/` | **184** — 179 pass, **5 fail** |
| packages, `aa/` | 29 green, 4 carrying at least one red, **0 build errors** |
| graded packages, `integration/` | **17**, each run on both CPU cores: 6 green, 11 carrying a red; 17 more specified, not yet built |
| assertions, `integration/` | **1111** on fx68k — 974 pass, **137 fail**; **1172** on TG68K — 1038 pass, **134 fail** |
| registered findings | **74**, all open — 69 against the Hardware Reference Manual and 5 against the AA specification. **49 ask you to decide whether the RTL should change**; 65 rest on a failing test, 2 were seen in the RTL source with no test yet, 4 rest on a passing test whose other reading fails, and 3 are filed against the printed page with no test yet. Withdrawn findings are kept apart, in [`docs/findings/withdrawn/`](docs/findings/withdrawn/) |
| AA findings | **5** — two are reds of this RTL, three are filed against the printed page |
| AA specification read | all seven slices classified, 6744 records in all: 6492 across slices 1 to 5 and 7, 252 in slice 6 |
| accepted units, recorded in [`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance) | **20** — twelve against the manual (chapters 1 to 8, appendix F, appendix K, appendices D and E together, appendices H and J together) and eight against the second oracle |

**Reading a test's output.** A test prints four kinds of line:

| line | what it is |
|---|---|
| `PASS:` / `FAIL:` | a check: the measurement compared with the value the page gives. Only these lines count, and a `FAIL` is a failing test |
| `MEASURED:` | a value observed and printed, compared with nothing |
| `RECORDED:` | a claim of the page this test deliberately does not check, with the reason |
| the summary, `N passed, M failed` | the count of `PASS:` and `FAIL:` lines |

**What each state of a finding asks of you.** A *failing test* is a result:
its label names the claim, and [`docs/findings.md`](docs/findings.md) names the
finding it belongs to. An *open* finding asks for the decision its page names.
For most of them that is whether the RTL should change; those filed against
the printed page ask about the manual instead. A finding *seen in the RTL
source* has no test behind it yet: read it, but nothing has been measured. A
*withdrawn* finding asks nothing; its page, in
[`docs/findings/withdrawn/`](docs/findings/withdrawn/), keeps the reasoning
that withdrew it. A *passing* test asks nothing either; [what a green means](#what-a-green-means)
says how far it reaches.

**All 250 packages report both a verdict and an assertion count** in `hrm/` and
`aa/`, and each graded `integration/` package does on both cores.
`./run_all.sh` prints `tests: 155 passed, 62 failed, 0 build errors` for `hrm/`
and `tests: 29 passed, 4 failed, 0 build errors` for `aa/`, and one
line per core for each of the 17 graded `integration/` packages; the table comes
from such a run. `check_published_counts.py --runlog`, in the separate
`hrm-testpipeline` repository, checks it against the run's log.

## Where to go next

| If you want to | Read |
|---|---|
| pick one finding to settle in an evening | [the shortlist](docs/findings.md#if-you-have-one-evening) in the register |
| see what may be wrong in the RTL, module by module | [`docs/rtl-review.md`](docs/rtl-review.md) |
| look up a finding | [`docs/findings.md`](docs/findings.md), the register, then its page in [`docs/findings/`](docs/findings/) |
| add a test | [`docs/adding-a-test.md`](docs/adding-a-test.md) |
| find anything else | [`docs/README.md`](docs/README.md), the map |
| see how the tests are made | the [`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline) repository |

Each finding page says what the printed page says, what NanoMig does and how
that was measured, what four independent
implementations (WinUAE, vAmiga, Denise, Copperline) and NanoMig's two Minimig
ancestors do, and what is asked of the reader. A withdrawn page keeps only the
reasoning that withdrew it. Where a page did not read one of the four, its row
for that implementation says so. Keys are `NM-HRM-nnn` against
the manual and `NM-AA-nnn` against the AA specification. Where a bit means
something different under ECS and AGA, the finding says which chip set its
fixture drives.

## What a green means

*The graded assertions in that package held, on its fixture, against the RTL
at the pinned submodule revision, for the printed claims they cite.* It does
not mean the module is correct, that the page's other statements are covered,
or that anything on a `MEASURED` or `RECORDED` line was checked. Coverage is
measured per claim, from the suite's own output, by the coverage ledger in
[`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline); it is
never implied by a green.

## Layout

```
hrm/            217 packages against the manual, each at one module's ports;
                chN-… or appX-… names the unit of the manual that judges it
aa/             33 packages against the AA specification
integration/    17 packages against the manual at nanomig.v's ports, with a
                68000 program on both CPU cores, 17 specified and not yet
                built; checks of the test harness, and a smoke test of
                minimig.v
nanomig/        submodule, the RTL under test
docs/           findings, the RTL review list, the rules for adding a test
run_all.sh      runs all three layers; hrm/run_all.sh runs only hrm/
```

## Sources

> *Amiga Hardware Reference Manual*, Third Edition. Commodore-Amiga, Inc.
> Addison-Wesley Publishing Company, 1991.
>
> Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*,
> 06/07/91, PRELIMINARY.

Neither document is distributed here; every graded assertion in `hrm/` and
`aa/` names the page it grades against. Which pages each unit covers, what was measured and
what is disclosed about it are in the acceptance records,
[`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance).
