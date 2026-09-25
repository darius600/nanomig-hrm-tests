# NanoMig HRM Tests

Conformance tests for the Amiga chipset RTL in
[NanoMig](https://github.com/harbaum/NanoMig), graded against two printed
oracles:

| Oracle | Documents | Tests |
|---|---|---|
| *Amiga Hardware Reference Manual*, 3rd edition | the original chip set (OCS) and the Enhanced Chip Set (ECS) | `hrm/`, 216 packages |
| Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)* | the AGA chip set | `aa/`, 33 packages |

**The printed page is the oracle, never the RTL.** A red test marks a place
where the RTL does something the page does not describe. It is a result, not a
bug in the test: adjusting an assertion to make it pass deletes the finding.

This is an add-on, not a fork. NanoMig comes in as a submodule, pinned to the
commit the tests were verified against; nothing under `nanomig/` is modified.

## Run it

```sh
git clone --recursive <this repo>
cd nanomig-hrm-tests
./run_all.sh
```

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
| test packages, `hrm/` | **216** |
| assertions, `hrm/` | **3480** — 3287 pass, **193 fail** |
| packages, `hrm/` | 149 green, 67 carrying at least one red, **0 build errors** |
| test packages, `aa/` | **33** |
| assertions, `aa/` | **184** — 179 pass, **5 fail** |
| packages, `aa/` | 29 green, 4 carrying at least one red, **0 build errors** |
| registered findings | **70** — 65 against the Hardware Reference Manual and 5 against the AA specification. **46 ask you to decide whether the RTL should change** — 63 open, 7 withdrawn; of the open ones 54 rest on a graded red, 5 are source observations, 3 on a graded green with an inverse control, and one is filed against the printed page with no test yet |
| AA findings | **5** — two are reds of this RTL, three are filed against the printed page |
| AA specification read | all seven slices classified, 6744 records in all: 6492 across slices 1 to 5 and 7, 252 in slice 6 |
| accepted units, recorded in [`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance) | **20** — twelve against the manual (chapters 1 to 8, appendix F, appendix K, appendices D and E together, appendices H and J together) and eight against the second oracle |

**All 249 packages report both a verdict and an assertion count.**
`./run_all.sh` prints `tests: 149 passed, 67 failed, 0 build errors` for `hrm/`
and `tests: 29 passed, 4 failed, 0 build errors` for `aa/`; the table comes
from such a run. `check_published_counts.py --runlog`, in the separate
`hrm-testpipeline` repository, checks it against the run's log.

## Where to go next

| If you want to | Read |
|---|---|
| see what may be wrong in the RTL, module by module | [`docs/rtl-review.md`](docs/rtl-review.md) |
| look up a finding | [`docs/findings.md`](docs/findings.md), the register, then its page in [`docs/findings/`](docs/findings/) |
| add a test | [`docs/adding-a-test.md`](docs/adding-a-test.md) |
| find anything else | [`docs/README.md`](docs/README.md), the map |
| see how the tests are made | the [`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline) repository |

Each finding page says what the printed page says, what NanoMig does and how
that was measured, what four independent
implementations (WinUAE, vAmiga, Denise, Copperline) and NanoMig's two Minimig
ancestors do, and what is asked of the reader. Keys are `NM-HRM-nnn` against
the manual and `NM-AA-nnn` against the AA specification. Where a bit means
something different under ECS and AGA, the finding says which chip set its
fixture drives.

## What a green means

*The graded assertions in that package held, on its fixture, against the RTL
at the pinned submodule revision, for the printed claims they cite.* It does
not mean the module is correct, that the page's other statements are covered,
or that anything on a `MEASURED` or `RECORDED` line was checked. Coverage is
stated per claim in the acceptance records, never implied by a green.

## Layout

```
hrm/            216 packages against the manual, each at one module's ports;
                chN-… or appX-… names the unit of the manual that judges it
aa/             33 packages against the AA specification
integration/    tests across module boundaries: minimig.v, and nanomig.v's
                own ports on both CPU cores
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

Neither document is distributed here; every assertion names the page it
grades against. Which pages each unit covers, what was measured and
what is disclosed about it are in the acceptance records,
[`hrm-testpipeline/acceptance/`](https://github.com/darius600/hrm-testpipeline/tree/main/acceptance).
