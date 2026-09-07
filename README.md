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

| Test | Findings |
|---|---|
| `agnus_copper_control` | FPGA-055, FPGA-060 |
| `agnus_copper_cycle_times` | FPGA-063, FPGA-064, FPGA-065, FPGA-066, FPGA-084 |
| `agnus_copper_cycles` | FPGA-056, FPGA-058, FPGA-059, FPGA-064, FPGA-087 |
| `agnus_copper_locreg_width` | FPGA-060, FPGA-078, FPGA-085, FPGA-088 |
| `agnus_copper_move_sweep` | FPGA-062, FPGA-074, FPGA-079 |
| `agnus_beamcounter`, `agnus_copper_move_range`, `paula_audio_mixer` | older, no ID in the source |

Every assertion names the manual page it comes from. Where a check and the RTL
disagree, the manual decides.

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

Chapter 2 (Copper, printed pages 19–36 plus appendices A, B and C) is covered.
