# NanoMig HRM Tests

Unit tests for the Amiga chipset RTL in [NanoMig](https://github.com/harbaum/NanoMig),
derived from the *Amiga Hardware Reference Manual*, 3rd edition.

This is an **add-on, not a fork**. NanoMig comes in as a submodule, pinned to the
commit the tests were last verified against. Nothing under `nanomig/` is modified.

```sh
git clone --recursive <this repo>
cd nanomig-hrm-tests
./run_all.sh
```

Already cloned without `--recursive`? `git submodule update --init`. Have NanoMig
checked out elsewhere? `make -C hrm/<test> NANOMIG=/path/to/NanoMig`.

Requires Verilator; last verified with **5.052**.

## Eight tests are red, and that is the result

```
tests: 38 passed, 8 failed, 0 build errors
```

Those eight are **findings, not broken tests.** Each one marks a place where the
RTL does something the manual does not describe. Do not adjust an assertion to
make it pass — that deletes the finding, and it is the one failure mode this
suite exists to prevent.

| Finding | What the RTL does that the manual does not describe | Tests |
|---|---|---|
| FPGA-050 | PAL non-interlace runs 313 lines; the manual says 312 | `agnus_beamcounter` |
| FPGA-051 | all four audio channels land on the wrong stereo side | `paula_audio_mixer` |
| FPGA-060 | OCS uses five high address bits; the manual documents three | `agnus_copper_control`, `agnus_copper_locreg_width` |
| FPGA-062 | ECS with CDANG=0 rejects 32 addresses the appendix allows | `agnus_copper_move_range`, `agnus_copper_move_sweep` |
| FPGA-069 | SKIP spans 8 memory cycle times instead of 4, a satisfied WAIT 8 instead of 6 | `agnus_copper_cycle_times`, `agnus_copper_cycles` |

[`docs/findings.md`](docs/findings.md) carries each one with its measurement and
its manual page. The `FPGA-nnn` keys come from the private workspace where this
work is planned; that file explains what each finding is, so a red result is
readable without access to anything outside this repository.

Every assertion names the manual page it comes from. Where a check and the RTL
disagree, the manual decides.

## What is covered, and by which method

The suite spans far more of the manual than one chapter, but only one chapter has
been through the full pipeline. That difference matters when reading a green
result, so it is stated here rather than left to be inferred.

| Chapter | Tests | How they were built |
|---|---:|---|
| 2 — Coprocessor (Copper) | 16 | 12 through the **pipeline**, 4 predate it |
| 6 — Blitter | 8 | by hand from the manual, one module at a time |
| 3 / 4 — Playfield, Sprites (`denise_*`) | 8 | by hand |
| 5 / 7 / 8 — Audio, interrupts, interfaces (`paula_*`) | 6 | by hand |
| 3 / 7 — Agnus DMA, beam counter, register bus | 8 | by hand |

The pipeline means two blind readings of the same pages, every quote anchored
mechanically in the source text, an attack on whatever was discarded as
untestable, and a stimulus mutation per assertion showing it would go red if the
behaviour changed.

The hand-written tests are not sloppy — they cite their manual pages, fix the
expected behaviour before looking at the RTL, and some check exhaustively (the
area-fill test walks all 262,144 input combinations). What they do **not** carry
is the pipeline's completeness claim. A green there means "this assertion holds",
not "the manual's statements about this module are covered".

**34 of the 46 tests are in that state, and four of them are Copper.**
[`docs/test-inventory.md`](docs/test-inventory.md) lists every test by layer and
holds the review backlog — including the eight blitter tests, which are prior art
for chapter 6 rather than coverage of it.

## Layout

```
hrm/            46 fixtures, each against a single module
integration/    tests across module boundaries, against minimig.v
nanomig/        submodule, the RTL under test
run_all.sh      everything; hrm/run_all.sh runs only the HRM suite
```

`run_all.sh` deliberately runs both. On 07.09.2026 an upstream change broke the
integration test at build time — `PINNOTFOUND`, not a single line ran — while the
HRM summary still read 38/8/0. It was caught only because the test happened to be
run by hand.

## How these tests come about

Not from the RTL, but from the manual. Every statement is quoted verbatim,
anchored in the source text, split into testable and untestable, and the
discards are attacked rather than believed. The pipeline that does this lives in
a separate repository (`hrm-testpipeline`).

Four rules carry the whole thing:

- **Rotation** — whoever makes a completeness claim does not check it themselves.
- **Do not bundle** — a sentence mixing prescription with behaviour gets split.
- **The reason belongs in the assertion** — a green that hangs on a precondition
  which never occurs asserts nothing. See `W4a`/`W4b` in
  `hrm/agnus_copper_waitskip_range/`.
- **Oracle mutation is not stimulus mutation** — flipping an assertion only shows
  that a boolean can go false.

## Source

> *Amiga Hardware Reference Manual*, Third Edition. Commodore-Amiga, Inc.
> Addison-Wesley Publishing Company, 1991.

Chapter 2 (Copper, printed pages 19–36 plus appendices A, B and C) has been
through the pipeline in full. Everything else in the table above is hand-written
coverage of individual modules.
