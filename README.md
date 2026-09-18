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
| test packages | **228** |
| assertions | **2932** — 2753 pass, **179 fail** |
| packages | 136 green, 64 carrying at least one red, **0 build errors** |
| registered findings | **65** — 54 open, 6 withdrawn, 5 source observations |
| accepted units, recorded in [`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance) | **12** — chapters 1 to 8, appendix F, appendices D and E as one record, appendices H and J as one, and appendix K |

The assertion figures cover the **200 packages that print an assertion summary**.
The other 28 are older module-level tests that report only pass or fail for the
package as a whole; they are counted in the package row and not in the assertion
row. That difference is stated rather than smoothed over, because a single
"assertions" number that quietly spans two reporting conventions is how the last
one drifted.

**These five figures come from a run and nothing could check them, so they
drifted.** The row above previously read 149 green and 66 red — 215, under a
line saying 228 — and the failing count was 98 against a true 179. Most of that
gap was one defect in the runner, not in the tests: 59 packages wrote their
summary to `out.txt` and printed it on the next `make` line, so a package that
failed stopped before the print and **vanished from the count exactly when it
went red**. Both are fixed, and `check_published_counts.py --runlog` now
measures this half of the table against a `run_all.sh` log instead of trusting
it. See `hrm-testpipeline/OPEN-ISSUES.md` O104.

**A red test is a result, not a bug in the test.** Each one marks a place where
the RTL does something the manual does not describe. Adjusting an assertion to
make it pass deletes the finding, and that is the one failure mode this suite
exists to prevent.

Every assertion names the manual page it comes from. Where a check and the RTL
disagree, the manual decides.

## The findings

**[`docs/findings.md`](docs/findings.md) is the register** — one table row per
finding with its key, subject, status, package and third witness. It is what
`tools/list_reds.py` reads, and it is the list to go to. **One page per finding
lives in [`docs/findings/`](docs/findings/)**, each written to be readable by
someone who has never seen this project: what the HRM says, what NanoMig does,
what the other four implementations say, a suggested change, what holds the
finding up, and what is left for specialists.

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

**Six findings were withdrawn** and their entries kept, because the reasoning
that withdrew them is the result. Four withdrew for one reason: the assertion had
read a permission or a silence in the manual as a requirement. The sixth,
NM-HRM-029, is a different shape and worth the minute it takes to read — its
assertion tested *"dmasen is true only when LENFIN=1"* by searching for a case
where both were true and failing when none existed. Absence satisfies "only
when". A one-way implication had been tested as a biconditional, which is the
same error as the other four in the direction nobody was watching for.

## Where to go next

| If you want to | Read |
|---|---|
| know what the findings are | [`docs/findings.md`](docs/findings.md) — the register table, then [`docs/findings/`](docs/findings/) for the page on any one of them |
| decide what to do about a finding | [`docs/rtl-decisions.md`](docs/rtl-decisions.md) — each open finding as the manual's words, what NanoMig measurably does, the assertion that measures it, and what WinUAE and vAmiga do about the same question, with verified file-and-line citations into both emulators |
| know whether an emulator was allowed to decide | [`docs/third-witness.md`](docs/third-witness.md) and [`docs/third-witness-ch3-ch6.md`](docs/third-witness-ch3-ch6.md) |
| see what four independent implementations do about the same question | [`docs/fourth-witness-denise-copperline.md`](docs/fourth-witness-denise-copperline.md) — Denise and Copperline read against the findings, file and line each |
| know what a green result means here | the coverage section below, and [`docs/test-inventory.md`](docs/test-inventory.md) |

[`docs/README.md`](docs/README.md) is the map. If you have just watched the suite
go red, that is the shortest way in.

## What is covered, and by which method

A **green** result means different things in the two layers, so the split is
stated here rather than left to be inferred: **204 of the 228 packages came
through the pipeline, 24 did not.**

| Unit | Packages | How they were built |
|---|---:|---|
| 1 — Introduction | 11 | **pipeline** |
| 2 — Coprocessor (Copper) | 13 | 12 through the **pipeline**; `agnus_copper_move` is a regression test against a frozen spec, not against the manual |
| 3 — Playfield | 22 | **pipeline** |
| 4 — Sprites | 14 | **pipeline** |
| 5 — Audio | 11 | **pipeline**; the hand-written audio tests that preceded them still stand beside them |
| 6 — Blitter | 14 | **pipeline**; the hand-written blitter tests that preceded them were superseded |
| 7 — System Control | 19 | **pipeline** |
| 8 — Interface Hardware | 55 | **pipeline**, read in four slices |
| D and E — memory maps, connector pinouts, port signals | 17 | **pipeline** |
| F — 8520 Complex Interface Adapters | 12 | **pipeline** |
| H and J — external disk connector, custom chip pin allocation | 5 | **pipeline** |
| K — Zorro Expansion Bus | 12 | **pipeline** |
| module-level, by hand | 23 | Agnus DMA and slot allocation, Denise's shifters and colour path, Paula's audio, floppy, UART and interrupt controller |

Chapters 2 and 6 are the two whose packages carry no `chN-` prefix: they are the
`agnus_copper_*` and `agnus_blitter_*` directories.

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

**The hand-written tests are not sloppy.** They cite their manual pages, fix the
expected behaviour before looking at the RTL, and some check exhaustively — the
area-fill test walks all 262 144 input combinations. What they do **not** carry
is the pipeline's completeness claim. A green there means *"this assertion
holds"*, not *"the manual's statements about this module are covered"*.

Where a hand-written test has been replaced by a pipeline package, the old one is
deleted only after its replacement is green from a clean build, and only after
every case it covered was checked against the replacement case by case. That
check is not a formality: one Copper case turned out not to be covered at all,
and is now asserted as `B2s` in `hrm/agnus_copper_waitskip_range`.

## Layout

```
hrm/            228 packages, each against a single module boundary
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
  `hrm/agnus_copper_waitskip_range/`.
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
