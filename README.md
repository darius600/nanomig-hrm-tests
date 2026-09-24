# NanoMig HRM Tests

Conformance tests for the Amiga chipset RTL in
[NanoMig](https://github.com/harbaum/NanoMig), derived from the *Amiga Hardware
Reference Manual*, 3rd edition. **The manual is the oracle, never the RTL.**

This is an **add-on, not a fork**. NanoMig comes in as a submodule, pinned to the
commit the tests were last verified against. Nothing under `nanomig/` is modified.

**The oracle is OCS and ECS. It is not AGA, and the RTL is.** The third edition
documents the original chip set and the Enhanced Chip Set; across the whole
extracted corpus `ECS` appears 490 times and `AGA` appears on no page at all. The
RTL under test is `minimig-aga` and implements AGA throughout. So every
AGA-specific behaviour in the design is **outside what this suite can adjudicate
by construction** — not untested yet, but untestable here, because there is no
printed page to test it against. Where a finding touches a bit whose meaning
differs between ECS and AGA, it says so and scopes itself to the chipset the
fixture drives; `NM-HRM-023` is the worked example, and `NM-HRM-013` was
withdrawn and split for exactly this reason.

```sh
git clone --recursive <this repo>
cd nanomig-hrm-tests
./run_all.sh
```

Already cloned without `--recursive`? `git submodule update --init`. Have NanoMig
checked out elsewhere? `make -C hrm/<test> NANOMIG=/path/to/NanoMig`.

**Against a different Minimig core** — MiSTer's `Minimig-AGA_MiSTer`, say —
`NANOMIG=` is not enough: it only replaces the root of NanoMig's own layout,
and the package then looks for

```make
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v
```

MiSTer keeps the same files under `rtl/`, so that ends in `No rule to make
target '…/src/minimig-aga/agnus_audiodma.v'`. Name the file instead — a
variable set on the `make` command line overrides the Makefile's own, so
nothing has to be edited:

```sh
make -C hrm/ch5-audio-location-pointer \
     SRC=/path/to/Minimig-AGA_MiSTer/rtl/agnus_audiodma.v test
```

Check the package's `Makefile` for the variable's name first: 136 packages call
it `SRC`, 59 use `SRC_DIR` for a directory, 20 use `RTL`, and a few multi-file
packages split it further. **A red against another core is not a finding here** —
these expectations are pinned to the submodule revision this suite was verified
against.

Requires Verilator and `make`; last verified with Verilator **5.052**.

If `run_all.sh` reports *every* package as a build error, the toolchain is
missing rather than the tree being broken — check `verilator --version` first. On
Windows the two commonly live only inside WSL, and a run started from Git Bash or
PowerShell then fails every package at once while the tree is perfectly healthy.

## Where the suite stands

Measured by one clean `./run_all.sh` over a quiet tree, and by counting the
artefacts rather than by remembering.

| | |
|---|---:|
| test packages, `hrm/` | **208** |
| assertions, `hrm/` | **3283** — 3098 pass, **185 fail** |
| packages, `hrm/` | 142 green, 66 carrying at least one red, **0 build errors** |
| test packages, `aa/` | **33** — sixteen cover one RTL module each from slice 7's register list, four the bit tables of slice 2, five from slice 4, one from slice 3, one from slice 5, five from slice 1 and one the scroll range of slice 6 |
| assertions, `aa/` | **184** — 179 pass, **5 fail**, 0 build errors |
| packages, `aa/` | 29 green, 4 carrying at least one red, **0 build errors**. Two of the four carry findings of their own -- NM-AA-004 and NM-AA-005 -- and two corroborate findings already raised against the Hardware Reference Manual: BEAMCON0's sync-polarity bits reach NM-HRM-032 and DSKBYTR's constant fields reach NM-HRM-041 |
| AA findings | **5** — the second oracle is the 1991 AA chip-set specification. Two of its five findings are reds of this RTL and the other three are filed against the printed page. Two further red packages corroborate findings already raised against the Hardware Reference Manual rather than adding keys of their own. Its suite is no longer small: **33 packages, 184 graded assertions**. **All seven slices are read and classified**, 6744 records in all: 6492 across slices 1 to 5 and 7, and 252 in slice 6, whose pair converged on the fourth attempt at 14.6% divergence against a 25% halt. **All seven now carry an acceptance record**, slice 6's written on 23.09.2026 once its pair converged |
| registered findings | **70** — 65 against the Hardware Reference Manual and 5 against the AA specification, counted in registers of their own. **46 ask you to decide whether the RTL should change** — 63 open, 7 withdrawn; of the open ones 54 rest on a graded red, 5 are source observations, 3 on a graded green with an inverse control, and one is filed against the printed page with no test yet |
| accepted units, recorded in [`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance) | **20** — chapters 1 to 8, appendix F, appendices D and E as one record, appendices H and J as one, appendix K, and seven against the second oracle: `aa-s7` and `aa-s7-rest`, the two halves of the AA specification's address-ordered register list, and `aa-s1` to `aa-s6`, its alphabetical stretches and its display modes — the first, then BEAMCON0 and the blitter's control words, the beam-position words, collision/colour/copper/display-window/DMACON, the controller-port bit tables, and the title page with the new Lisa display modes |

**All 241 packages now report both a verdict and an assertion count**, so the
`hrm/` rows above describe the same population. `./run_all.sh` walks both trees
and prints a line for each: `tests: 142 passed, 66 failed, 0 build errors` for
`hrm/`, `tests: 29 passed, 4 failed, 0 build errors` for `aa/`.

That was not true until 19.09.2026. Nine packages printed only a package-level
pass or fail, in wordings no tool read — `Checked 22 assertions; passed=22
failed=0`, `225/225 PASS, 0 FAIL` — so **333 assertions were missing from every
published total, seven of them failing**. Four of those seven are NM-HRM-002's
stereo-routing reds, which had been counted as a red package and zero red
assertions at the same time. The nine now print the canonical line beside their
own, nothing else about them changed, and the figures above are the first that
cover the whole suite.

**These five figures come from a run and nothing could check them, so they
drifted.** The row above previously read 149 green and 66 red — 215, under a
line saying 228 — and the failing count was 98 against a true 179. Most of that
gap was one defect in the runner, not in the tests: 59 packages wrote their
summary to `out.txt` and printed it on the next `make` line, so a package that
failed stopped before the print and **vanished from the count exactly when it
went red**. Both are fixed, and `check_published_counts.py --runlog` now
measures this half of the table against a `run_all.sh` log instead of trusting
it. That script lives in the **separate `hrm-testpipeline` repository**, at
`hrm-testpipeline/tools/check_published_counts.py` — it is not in this one, and
running it from here fails. See `hrm-testpipeline/OPEN-ISSUES.md` O104.

**A red test is a result, not a bug in the test.** Each one marks a place where
the RTL does something the manual does not describe. Adjusting an assertion to
make it pass deletes the finding, and that is the one failure mode this suite
exists to prevent.

Every assertion names the manual page it comes from. Where a check and the RTL
disagree, the manual decides.

## The findings

**[`docs/findings.md`](docs/findings.md) is the register** — one table row per
finding with its key, subject, status, package and third witness. It is what
`hrm-testpipeline/tools/list_reds.py` reads — that script is in the **separate
`hrm-testpipeline` repository**, not this one — and the table is the list to go
to; you do not need the script to read it. **One page per finding
lives in [`docs/findings/`](docs/findings/)**, each written to be readable by
someone who has never seen this project: what the HRM says, what NanoMig does,
what the other four implementations say, a suggested change, what holds the
finding up, and what is left for specialists.

**Six other implementations are read, and four are counted.** WinUAE, vAmiga,
Denise and Copperline each implemented the manual independently, so they are
the only witnesses to what it means. The two Minimig lines — MiSTer's
Minimig-AGA and MiST/TC64 — are NanoMig's ancestors and share its code, so
their agreement with NanoMig is one implementation counted twice. They answer a
different question: whether a finding is inherited, and where a fix belongs.
Each finding page lists all six and marks the two as `descendant`; the
reasoning is in [`docs/findings/README.md`](docs/findings/README.md#the-two-minimig-lines-are-ancestors-not-witnesses--and-nothing-merges-them-down).

This README deliberately does not reproduce that table. It used to, and it is
the part that went stale first.

Each finding is keyed `NM-HRM-nnn` — `NM` for the implementation under test,
`HRM` for the manual that judges it. The key names the **pair that disagrees**,
deliberately: on several of these, independent emulators do what NanoMig does and
the printed sentence is the outlier — see the fourth-witness document below.

**Three are one finding in three places.** NM-HRM-003, NM-HRM-022 and NM-HRM-024
are the same deviation in three different engines — Copper, blitter and audio
build their pointers identically, `data_in[4:0]` as five high bits and
`data_in[15:1]` as fifteen low, unconditionally, and the manual documents three
high bits for OCS in every case. Separately they read as three omissions.
Together they are one decision not to model the OCS pointer narrowing anywhere in
Agnus — and `agnus_audiodma.v` is the case that settles it, because the module
has no chipset input at all, so there is nothing there to gate on even in
principle. No chapter could have raised that question alone.

**Seven findings were withdrawn** and their entries kept, because the reasoning
that withdrew them is the result. Five withdrew for one reason: the assertion had
read a permission or a silence in the manual as a requirement — the latest,
[NM-HRM-036](docs/findings/NM-HRM-036.md), graded one sentence of p.223 while the
same page, seven lines up, allows part of the RAM to respond. NM-HRM-012 read its
page backwards. The seventh, NM-HRM-029, is a different shape and worth the
minute it takes to read — its
assertion tested *"dmasen is true only when LENFIN=1"* by searching for a case
where both were true and failing when none existed. Absence satisfies "only
when". A one-way implication had been tested as a biconditional, which is the
same error as the other five in the direction nobody was watching for.

## Where to go next

| If you want to | Read |
|---|---|
| know what the findings are | [`docs/findings.md`](docs/findings.md) — the register table, then [`docs/findings/`](docs/findings/) for the page on any one of them |
| decide what to do about a finding | [`docs/rtl-decisions.md`](docs/rtl-decisions.md) — each open finding as the manual's words, what NanoMig measurably does, the assertion that measures it, and what WinUAE and vAmiga do about the same question, with verified file-and-line citations into both emulators |
| know whether an emulator was allowed to decide | [`docs/third-witness.md`](docs/third-witness.md) and [`docs/third-witness-ch3-ch6.md`](docs/third-witness-ch3-ch6.md) |
| see what four independent implementations do about the same question | [`docs/fourth-witness-denise-copperline.md`](docs/fourth-witness-denise-copperline.md) — Denise and Copperline read against the findings, file and line each |
| know what a green result means here | the coverage section below |
| add or change a test | [`docs/adding-a-test.md`](docs/adding-a-test.md) — the rules a reviewer checks, with a package to copy from |

[`docs/README.md`](docs/README.md) is the map. If you have just watched the suite
go red, that is the shortest way in.

## What is covered, and by which method

**What one green means, in a sentence.** *The graded assertions in that package
held, on its fixture, against the RTL at the pinned submodule revision, for the
printed claims they cite.* It does not mean the module is correct, that the
manual's other statements about it are covered, that anything on a `MEASURED` or
`RECORDED` line was checked, or that AGA behaviour was judged at all — the third
edition documents OCS and ECS, so AGA is outside what this suite can adjudicate.
Coverage is a separate statement, made per claim from a committed ledger, and is
never implied by a green.

**All 208 packages came through the pipeline.** Each is assigned to the unit of
the manual whose pages it answers to.

| Unit | Packages | Read in |
|---|---:|---|
| 1 — Introduction | 11 | one pass |
| 2 — Coprocessor (Copper) | 12 | one pass |
| 3 — Playfield | 22 | one pass |
| 4 — Sprites | 14 | one pass |
| 5 — Audio | 12 | one pass |
| 6 — Blitter | 15 | one pass |
| 7 — System Control | 20 | one pass |
| 8 — Interface Hardware | 56 | four slices |
| D and E — memory maps, connector pinouts, port signals | 17 | one pass |
| F — 8520 Complex Interface Adapters | 12 | one pass |
| H and J — external disk connector, custom chip pin allocation | 5 | one pass |
| K — Zorro Expansion Bus | 12 | one pass |

**Three packages declare themselves something other than an HRM-conformance
test, and say why.** `ch2-copper-locreg-width` writes *"Primary oracle: none.
Print page 25 contradicts itself"*; `ch2-copper-rejected-move` says the manual
*"states only that the write does not occur; it does not define what the Copper
does next"*; `ch2-copper-bus-behaviour` is mixed, manual-backed for two of its
five propositions and an interface-contract regression for the other three,
because *"print page 30 never states where the Copper resumes"*. That is the
disposition the pipeline asks for where the page is silent or self-contradicting
— it is not the same thing as the removed packages, whose oracle was a
description of the implementation. Every other package declares
`HRM_CONFORMANCE`.

**One naming scheme, no exceptions.** Every package is `chN-subject` or
`appX-subject`, so a directory name says which unit of the manual judges it.
Chapters 2 and 6 were the last holdouts — they were built as `agnus_copper_*`
and `agnus_blitter_*` before the prefix existed — and were renamed on
19.09.2026. Records in the pipeline repository cite the old names 4821 times and
are not edited; [`docs/legacy-names.md`](docs/legacy-names.md) maps every one.

A first suite of 23 module-level packages preceded the pipeline. Their oracle was
a *frozen specification*: a note describing what the module already does, written
by reading the Verilog. Three were later rewritten as pipeline packages and kept
their directory name; the other twenty are gone, because a description of the
implementation cannot decide whether the implementation is right.
[`docs/legacy-names.md`](docs/legacy-names.md) maps each removed name to the
package that covers its subject now, for the older `//` comments that still cite
one.

**Coverage is measured per claim, from a committed ledger, not from a number that
is remembered.** Chapter 5's figure is 923 of its 970 testable claims: 923 cited
by an assertion that ran and graded PASS or FAIL, 5 recorded by a measurement
deliberately not graded, and 42 not covered — each of the 42 listed with the
fixture it would need. Twenty-four ids were taken off assertions on purpose, on
one rule: **an id whose claim bundles a clause this package does not test is
withdrawn whole, not kept with a caveat in the prose.** Naming an id inside a
passing assertion in order to disclaim it still puts the id on a graded line, and
an independent validator caught six that were being counted as covered by the
very sentence calling them uncovered.

Appendices D and E are the most recent and the most lopsided: 1731 claims,
346 testable, **109 asserted, 237 recorded-only, 0 not covered**. Two thirds sit
on `record()` lines because most of what those pages state is already graded
elsewhere or names a connector this design does not model. A high recorded-only
share is not weak coverage; it is what an appendix of pinouts looks like when
every claim is dispositioned individually.

The reasoning, the ledgers and the four earlier measurements that were wrong are
in the pipeline repository's `work/` directory. A coverage number that counts ids
named anywhere in a source — including in comments — is the thing those files
exist to stop being quoted.

**The pipeline** means two blind readings of the same pages by two different
models, every quotation anchored mechanically in the source text, an attack on
whatever was discarded as untestable, a stimulus mutation per assertion showing
it would go red if the behaviour changed, and an independent validation by a
model that built none of it. What that does and does not guarantee is set out in
the pipeline repository's own README, weaknesses included.

**A package is removed only after its subject is read against the package that
replaces it, proposition by proposition** — not after a count, and not because a
directory looks superseded. The check is not a formality in either direction: one
Copper case turned out not to be covered at all and is now asserted as `B2s` in
`hrm/ch2-copper-waitskip-range`, and of the twenty pre-pipeline packages
removed, two carried a number with no page behind it — a horizontal span of
0..453 and four refresh slots at 9/13/17/21 — which is the reason they went
rather than an obstacle to their going.

## Layout

```
hrm/            208 packages, each against a single module boundary
integration/    tests across module boundaries, against minimig.v
nanomig/        submodule, the RTL under test
docs/           what a red result means, and who decides — see docs/README.md
run_all.sh      everything; hrm/run_all.sh runs only the HRM suite
```

`run_all.sh` deliberately runs both layers. On 07.09.2026 an upstream change
broke the integration test at build time — `PINNOTFOUND`, not a single line ran —
while the HRM summary still read green. It was caught only because the test
happened to be run by hand.

## How these tests come about

Not from the RTL, but from the manual. Every statement is quoted verbatim,
anchored in the source text, split into testable and untestable, and the discards
are attacked rather than believed. The pipeline that does this lives in a
separate repository,
[`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline).

Four rules carry the whole thing:

- **Rotation** — whoever makes a completeness claim does not check it themselves.
- **Do not bundle** — a sentence mixing prescription with behaviour gets split.
- **The reason belongs in the assertion** — a green that hangs on a precondition
  which never occurs asserts nothing. See `W4a`/`W4b` in
  `hrm/ch2-copper-waitskip-range/`.
- **Oracle mutation is not stimulus mutation** — flipping an expected value only
  shows that a boolean can go false.

## Source

> *Amiga Hardware Reference Manual*, Third Edition. Commodore-Amiga, Inc.
> Addison-Wesley Publishing Company, 1991.

**Twelve frozen records cover fourteen printed units**: chapters 1 to 8,
appendix F, appendix K, appendices D and E together, and appendices H and J
together. The two paired records are lots that were read, classified and built
as one and share one record.

**The records live in the sibling repository**, not in this one — they belong to
the method that produced them:
[`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance).
The ranges below are the pages **extracted into the corpus**, counted from the
corpus itself rather than from the book's table of contents:

| Unit | Extracted pages |
|---|---|
| 1 — Introduction | 1–18 |
| 2 — The Copper | 19–36 |
| 3 — Playfield hardware | 39–95 |
| 4 — Sprite hardware | 97–131 |
| 5 — Audio hardware | 133–166 |
| 6 — Blitter hardware | 169–205 |
| 7 — System Control hardware | 207–223 |
| 8 — Interface Hardware | 227–260 |
| D and E — System Memory Maps; Connector Pinouts and Port Signal Assignments | 313–337 |
| F — 8520 Complex Interface Adapters | 339–356, of which 355 and 356 are blank in print |
| H and J — External Disk Connector Interface; Custom Chip Pin Allocation List | 367–371 and 379–381, with 372 and 382 blank in print |
| K — Zorro Expansion Bus | 383–439, with 440 blank in print |

Chapters 2 to 6 also carry their appendix A, B and C entries.

Each record in
[`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance)
says what was measured and what is disclosed about
it, including the parts that went wrong. They are worth reading before trusting
a number in this file: appendix D and E's record, for instance, discloses twelve
coordinator errors in its own briefs, every one of which was caught by a reader
looking the thing up and none by a gate.
