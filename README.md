# NanoMig HRM Tests

Conformance tests for the Amiga chipset RTL in
[NanoMig](https://github.com/harbaum/NanoMig), derived from the *Amiga Hardware
Reference Manual*, 3rd edition. **The manual is the oracle, never the RTL.**

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

## Where the suite stands

| | |
|---|---:|
| test packages | **87** |
| failing assertions | **68**, in 33 families |
| registered findings | **23** — 16 open, 5 withdrawn, 2 source observations |
| chapters through the full pipeline | **4** — 2 Copper, 3 Playfield, 4 Sprites, 6 Blitter |

**A red test is a result, not a bug in the test.** Each one marks a place where
the RTL does something the manual does not describe. Adjusting an assertion to
make it pass deletes the finding, and that is the one failure mode this suite
exists to prevent.

Every assertion names the manual page it comes from. Where a check and the RTL
disagree, the manual decides.

## The sixteen open findings

Each is keyed `NM-HRM-nnn` — `NM` for the implementation under test, `HRM` for
the manual that judges it. The key names the **pair that disagrees**, deliberately:
on several of these, independent emulators do what NanoMig does and the printed
sentence is the outlier — see the fourth-witness document below.

| Finding | What the RTL does that the manual does not describe | Chapter |
|---|---|---|
| [NM-HRM-002](docs/findings.md#nm-hrm-002--audio-channels-on-the-wrong-stereo-side) | the four audio channels sit on the wrong stereo side | 5 |
| [NM-HRM-003](docs/findings.md#nm-hrm-003--ocs-copper-address-uses-five-high-bits-instead-of-three) | OCS Copper pointer uses five high address bits; the manual documents three | 2 |
| [NM-HRM-004](docs/findings.md#nm-hrm-004--ecs-danger-bit-admits-too-small-a-range) | ECS with CDANG clear rejects 32 addresses the appendix allows | 2 |
| [NM-HRM-005](docs/findings.md#nm-hrm-005--wait-and-skip-take-longer-than-the-manual-states) | SKIP spans 8 memory cycle times instead of 4, a satisfied WAIT 8 instead of 6 | 2 |
| [NM-HRM-006](docs/findings.md#nm-hrm-006--ddfstrtddfstop-bit-h3-is-gated-by-the-chipset-not-by-resolution) | DDFSTRT bit H3 is gated by the chipset, not by resolution | 3 |
| [NM-HRM-007](docs/findings.md#nm-hrm-007--the-hires-fetch-width-cannot-reach-the-manuals-maximum-of-49-words) | the hires fetch width cannot reach the manual's maximum of 49 words | 3 |
| [NM-HRM-008](docs/findings.md#nm-hrm-008--playfield-1-collision-depends-on-playfield-2-by-design-undocumented) | playfield-1 collision depends on playfield 2 — deliberate, and undocumented | 3 |
| [NM-HRM-011](docs/findings.md#nm-hrm-011--pal-vertical-blanking-is-26-lines-the-manual-says-29) | PAL vertical blanking is 26 lines; the manual says 29 | 3 |
| [NM-HRM-015](docs/findings.md#nm-hrm-015--brdntran-is-declared-and-never-connected) | BRDNTRAN is declared and never connected | 3 |
| [NM-HRM-016](docs/findings.md#nm-hrm-016--superhires-sprite-colour-encoding-is-not-implemented) | SuperHires sprite colour encoding is not implemented | 3 |
| [NM-HRM-017](docs/findings.md#nm-hrm-017--superhires-bitplane-colour-encoding-is-not-implemented) | SuperHires bitplane colour encoding is not implemented | 3 |
| [NM-HRM-019](docs/findings.md#nm-hrm-019--sprxctl-bit-4-shsh1-is-not-implemented) | SPRxCTL bit 4 (SHSH1) is not implemented | 3 |
| [NM-HRM-020](docs/findings.md#nm-hrm-020--bltsizv-is-not-gated-on-ecs-while-its-two-siblings-are) | BLTSIZV is not gated on ECS, though appendix C marks it ECS-new like its two siblings | 6 |
| [NM-HRM-021](docs/findings.md#nm-hrm-021--doff-bltcon1-bit-7-has-no-observable-effect) | DOFF (BLTCON1 bit 7) has no observable effect; no expression reads that bit | 6 |
| [NM-HRM-022](docs/findings.md#nm-hrm-022--bltxpth-carries-five-high-bits-in-both-chipsets) | BLTxPTH takes five high pointer bits in **both** chipsets | 6 |
| [NM-HRM-023](docs/findings.md#nm-hrm-023--the-attach-bit-is-honoured-in-the-even-sprite-not-only-the-odd) | the sprite ATTACH bit is honoured in the even sprite, not only the odd | 4 |

Two of these are worth reading together. **NM-HRM-003 and NM-HRM-022 are the same
deviation in two different engines** — Copper and blitter build their pointers
identically, `data_in[4:0]` as five high bits and `data_in[15:1]` as fifteen low,
unconditionally, and the manual documents three high bits for OCS in both cases.
Separately they read as two omissions. Together they look like one decision not to
model the OCS pointer narrowing anywhere in Agnus. Neither chapter could have
raised that question alone.

**Five findings were withdrawn** and their entries kept, because the reasoning
that withdrew them is the result. Four of the five withdrew for one reason: the
assertion had read a permission or a silence in the manual as a requirement.

## Where to go next

| If you want to | Read |
|---|---|
| decide what to do about a finding | [`docs/rtl-decisions.md`](docs/rtl-decisions.md) — each open finding as the manual's words, what NanoMig measurably does, the assertion that measures it, and what WinUAE and vAmiga do about the same question, with verified file-and-line citations into both emulators |
| know what a specific finding is | [`docs/findings.md`](docs/findings.md) — the register, then one entry per finding with its measurement and its manual page |
| know whether an emulator was allowed to decide | [`docs/third-witness.md`](docs/third-witness.md) and [`docs/third-witness-ch3-ch6.md`](docs/third-witness-ch3-ch6.md) |
| see what four independent implementations do about the same question | [`docs/fourth-witness-denise-copperline.md`](docs/fourth-witness-denise-copperline.md) — Denise and Copperline read against the findings, file and line each |
| know what a green result means here | the coverage section below, and [`docs/test-inventory.md`](docs/test-inventory.md) |

[`docs/README.md`](docs/README.md) is the map. If you have just watched the suite
go red, that is the shortest way in.

## What is covered, and by which method

The suite spans more of the manual than the four chapters that have been through
the full pipeline. That difference matters when reading a **green** result, so it
is stated here rather than left to be inferred: **63 of the 87 packages came
through the pipeline, 24 did not.**

| Chapter | Packages | How they were built |
|---|---:|---|
| 2 — Coprocessor (Copper) | 13 | 12 through the **pipeline**; `agnus_copper_move` is a regression test against a frozen spec, not against the manual |
| 3 — Playfield | 22 | all through the **pipeline** |
| 4 — Sprites | 14 | all through the **pipeline** |
| 6 — Blitter | 14 | all through the **pipeline**; the hand-written blitter tests that preceded them were superseded |
| 5 / 7 / 8 and module-level tests | 24 | by hand |

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
hrm/            87 packages, each against a single module boundary
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

Four chapters have been through the pipeline: chapter 2 (Copper, printed pages
19–36), chapter 3 (Playfield, 39–95), chapter 4 (Sprites, 97–131) and chapter 6
(Blitter, 169–205), each with its appendix A, B and C entries. Chapters 1, 5, 7
and 8 have not been extracted; the packages listed against them above are
hand-written coverage of individual modules.
