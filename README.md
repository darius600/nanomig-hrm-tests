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
tests: 42 passed, 8 failed, 0 build errors
failing: agnus_blitter_ecs agnus_copper_control agnus_copper_cycle_times
         agnus_copper_cycles agnus_copper_locreg_width agnus_copper_move_range
         agnus_copper_move_sweep paula_audio_mixer
plus both integration tests green
```

Those eight carry **seven findings, not broken tests** — `agnus_blitter_ecs`
holds three of them. Each marks a place where the RTL does something the manual
does not describe. Do not adjust an assertion to make it pass — that deletes the
finding, and it is the one failure mode this suite exists to prevent.

| Finding | What the RTL does that the manual does not describe | Tests |
|---|---|---|
| FPGA-060 | OCS uses five high address bits; the manual documents three | `agnus_copper_control`, `agnus_copper_locreg_width` |
| FPGA-062 | ECS with CDANG=0 rejects 32 addresses the appendix allows | `agnus_copper_move_range`, `agnus_copper_move_sweep` |
| FPGA-069 | SKIP spans 8 memory cycle times instead of 4, a satisfied WAIT 8 instead of 6 | `agnus_copper_cycle_times`, `agnus_copper_cycles` |
| FPGA-051 | the four audio channels sit on the wrong stereo side | `paula_audio_mixer` |
| *chapter 6, unnumbered* | `DOFF` (BLTCON1 bit 7) is not implemented at all — appendix C p309 says it disables the D output in line mode; no expression in `agnus_blitter.v` reads that bit | `agnus_blitter_ecs` |
| *chapter 6, unnumbered* | `BLTSIZV` loads the height whether or not `ecs` is set, though appendix C marks it ECS-new | `agnus_blitter_ecs` |
| *chapter 6, unnumbered* | `BLTxPTH` takes five high pointer bits in **both** chipsets — p270 says three, five if ECS. `ecs` never reaches the pointer decode, even at `agnus_blitter` where the port exists | `agnus_blitter_ecs` |

The first three came out of the pipeline and are witnessed more than once.

**For a developer picking these up:** [`docs/rtl-decisions.md`](docs/rtl-decisions.md)
states each open finding as the manual's words, what NanoMig measurably does, the
assertion that measures it, and what WinUAE and vAmiga do about the same question
— with verified file-and-line citations into both emulators. It is written to be
the starting point, not a summary of one.
The three chapter-6 entries have no FPGA number yet; numbering lives in the
project workspace, not in this repository. They came out of the same pipeline and
each is red in the suite today, with a control that passes on the same
measurement basis so the result survives a units objection.

The last of them is worth reading next to FPGA-060, because it is the same
deviation in a different engine. Copper and blitter build their pointers
identically — `data_in[4:0]` as five high bits, `data_in[15:1]` as fifteen low,
unconditionally — and the manual documents three high bits for OCS in both cases.
Separately they read as two omissions. Together they look like one decision not to
model the OCS pointer narrowing anywhere in Agnus. Neither chapter could have
raised that question alone; see [`docs/findings.md`](docs/findings.md).

FPGA-051 and FPGA-050 were recorded in August, and both turned out to rest on a
manual that contradicts itself. FPGA-050 (PAL line count) is **withdrawn**: the
manual sanctions the measured value. FPGA-051 (stereo routing) is **restored**:
WinUAE and vAmiga, read independently, both agree with the page the test cites.

How that was decided, with permalinks into both emulators, is in
[`docs/third-witness.md`](docs/third-witness.md). An emulator is not promoted to
oracle there — it is admitted only to decide between two sentences of the manual.

[`docs/findings.md`](docs/findings.md) carries each one with its measurement and
its manual page. The `FPGA-nnn` keys come from the private workspace where this
work is planned; that file explains what each finding is, so a red result is
readable without access to anything outside this repository.

Every assertion names the manual page it comes from. Where a check and the RTL
disagree, the manual decides.

## What is covered, and by which method

The suite spans far more of the manual than the two chapters that have been
through the full pipeline. That difference matters when reading a green result,
so it is stated here rather than left to be inferred: **28 of the 50 tests came
through the pipeline, 22 did not.**

| Chapter | Tests | How they were built |
|---|---:|---|
| 2 — Coprocessor (Copper) | 13 | 12 through the **pipeline**; `agnus_copper_move` is a regression test against a frozen spec, not against the manual |
| 6 — Blitter | 16 | all 16 through the **pipeline**, 09.–10.09.2026; the eight hand-written blitter tests that preceded them were superseded and deleted |
| 3 / 4 — Playfield, Sprites (`denise_*`) | 8 | by hand |
| 5 / 7 / 8 — Audio, interrupts, interfaces (`paula_*`) | 6 | by hand |
| 3 / 7 — Agnus DMA, beam counter, register bus | 7 | by hand |

The pipeline means two blind readings of the same pages, quotes anchored
mechanically in the source text, an attack on whatever was discarded as
untestable, and a stimulus mutation per assertion showing it would go red if the
behaviour changed. What that does and does not guarantee is set out in the
pipeline repository's own README, weaknesses included.

The hand-written tests are not sloppy — they cite their manual pages, fix the
expected behaviour before looking at the RTL, and some check exhaustively (the
area-fill test walks all 262,144 input combinations). What they do **not** carry
is the pipeline's completeness claim. A green there means "this assertion holds",
not "the manual's statements about this module are covered".

**22 of the 50 tests are in that state.**
[`docs/test-inventory.md`](docs/test-inventory.md) lists every test by layer and
holds the review backlog. The eight blitter tests that used to be listed there as
prior art are gone: three were extended into pipeline packages and five were superseded and deleted, the last of them on 10.09.2026.

Three pre-pipeline Copper tests were removed on 07.09.2026 after being measured
case by case against the layer-A tests. That measurement is not a formality: five
of their six cases were already covered and covered more strictly, and the sixth
was not covered at all. It is now asserted as `B2s` in
`hrm/agnus_copper_waitskip_range`.

## Layout

```
hrm/            50 fixtures, each against a single module
integration/    tests across module boundaries, against minimig.v
nanomig/        submodule, the RTL under test
docs/           what a red result means, and who decides — see docs/README.md
run_all.sh      everything; hrm/run_all.sh runs only the HRM suite
```

[`docs/README.md`](docs/README.md) says which document answers which question. If
you have just watched the suite go red, that is the shortest way in.

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

Two chapters have been through the pipeline: chapter 2 (Copper, printed pages
19–36 plus appendices A, B and C) and chapter 6 (Blitter, printed pages 169–206
plus its appendix entries). Everything else in the table above is hand-written
coverage of individual modules.
