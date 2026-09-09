# The eight blitter tests, read before chapter 6

Chapter 6 of the manual is going through the pipeline now. Eight `agnus_blitter_*`
tests already exist, written in the first, unstructured run. They are prior art:
the pipeline has to confirm each one or show what it leaves out, and it cannot do
that against an impression of what they contain.

So they were read, file by file, on 07.09.2026 — before the pipeline's
classification stage produced its package list, so that the list can be compared
against something measured rather than remembered. This file records what each
one actually builds and actually asserts. It makes no judgement about whether the
asserted behaviour is right; that is the pipeline's job.

## What each test builds

The distinction that matters is which Verilog the test compiles.

| Test | Device under test | Source |
|---|---|---|
| `agnus_blitter_barrelshifter` | `agnus_blitter_barrelshifter.v` | NanoMig |
| `agnus_blitter_fill` | `agnus_blitter_fill.v` | NanoMig |
| `agnus_blitter_minterm` | `agnus_blitter_minterm.v` | NanoMig |
| `agnus_blitter_modulo` | `agnus_blitter_adrgen.v` | NanoMig |
| `agnus_blitter_ptr` | `agnus_blitter_adrgen.v` | NanoMig |
| `agnus_blitter_line_octant` | `agnus_blitter_line_octant.v` | **local copy** |
| `agnus_blitter_mask` | `agnus_blitter_mask.v` | **local copy** |
| `agnus_blitter_zero` | `agnus_blitter_zero.v` | **local copy** |

The last three are not NanoMig modules. They are fragments cut out of
`agnus_blitter.v` and pasted into the test directory, each with a comment saying
where it came from. NanoMig has no such modules; the logic lives inline in the
big blitter file.

## Defect 1 — three tests run against a copy that nothing keeps in step

`line_octant`, `mask` and `zero` compile the local `.v` file, not the design. A
change to `agnus_blitter.v` cannot make them go red.

The three copies were compared against the current submodule
(`nanomig` at `d6f7df9`) on 07.09.2026 and all three are still verbatim:

| Copy | Original | Status |
|---|---|---|
| `agnus_blitter_mask.v` | `agnus_blitter.v:311-319` | identical |
| `agnus_blitter_zero.v` | `agnus_blitter.v:481-487` | identical |
| `agnus_blitter_line_octant.v` | `agnus_blitter.v:609-612` | identical |

`agnus_blitter_mask.v` adds one line the original does not have —
`assign data_out = bltadat & bltamask` — but that expression is in the original
twice, at lines 337 and 345, so the copy reproduces it rather than inventing it.

No drift today. Nothing prevents drift tomorrow, and the tests would stay green
through it. That is the finding, not the current state.

## Defect 2 — four assertions in `line_octant` cannot fail

`test_octant` picks the shift value from what it expects on the X axis:

```cpp
uint8_t ash_major = (expected_major_x == 1) ? 15 : ((expected_major_x == 2) ? 0 : 8);
```

For the four octants whose major axis is Y, `expected_major_x` is 0, so `ash` is
8. The RTL gates both X outputs on the shift value:

```verilog
wire lineinc = (...) && ash==4'b1111 ? 1'b1 : 1'b0;
wire linedec = (...) && ash==4'b0000 ? 1'b1 : 1'b0;
```

With `ash == 8` neither can be 1 whatever `BLTCON1` says. The test then asserts
`inc == 0 && dec == 0` — which is true by construction, for a reason that has
nothing to do with the octant encoding the test is about.

The other twelve of the sixteen `test_case` invocations are not affected.

### Measured, not argued

A copy of the test was taken to a scratch directory and the DUT mutated: the
`!sign_del` guard was removed from `lineinc`, so the RTL now steps right on the
X axis in a case where `BLTCON1` says it must not.

```verilog
wire lineinc_w = (bltcon1[4] && !bltcon1[2] || !bltcon1[4] && !bltcon1[3]) && ash==4'b1111 ...
```

**The test stayed green.** That mutation is exactly the kind the pipeline
requires an assertion to catch, and this one does not see it.

The proposed stimulus was then applied to the same scratch copy — `ash = 15`
instead of the neutral 8 when no X step is expected — and re-run twice:

| RTL | Stimulus | Result |
|---|---|---|
| mutated | `ash = 8` (as shipped) | PASS — the defect is invisible |
| mutated | `ash = 15` | **FAIL**, `SUD:0 SUL:0 AUL:1 Sign:1`, got `inc=1`, expected `inc=0` |
| clean | `ash = 15` | PASS |

So the assertion is recoverable without weakening it: the same claim, driven from
a state the RTL could get wrong. The complete fix needs both probes, `ash = 15`
against `lineinc` and `ash = 0` against `linedec`; only the first was measured
here.

### Which octants those are — corrected 09.09.2026

An earlier revision of this section named them "octants 2, 3, 6 and 7". That was
wrong, and the mistake is worth keeping visible because of where it came from:
the numbers were read out of `test_agnus_blitter_line_octant.cpp`'s own comments,
which number the octants 1 to 8 in a scheme the file invented. The manual numbers
them 0 to 7, and Table 6-3 on printed page 190 is the authority:

| BLTCON1 4 3 2 (SUD SUL AUL) | Octant # |
|---|---:|
| 1 1 0 | 0 |
| 0 0 1 | 1 |
| 0 1 1 | 2 |
| 1 1 1 | 3 |
| 1 0 1 | 4 |
| 0 1 0 | 5 |
| 0 0 0 | 6 |
| 1 0 0 | 7 |

`SUD` selects the major axis, so the rows with `SUD = 0` — Y major, X minor — are
octants **1, 2, 5 and 6**. Those are the four the vacuous assertion applies to.

This was found by `PKG-BLT-LINE`'s builder, which re-derived the table from p190
and appendix A p268 rather than from the test it was superseding, and said so.
The correction had already propagated: the wrong numbering was repeated in
`work/2026-09-08_fpga-090_packages.md`'s reconciliation and again in the build
brief. Reading a superseded test's comments as if they were the manual is exactly
the substitution this whole pipeline exists to prevent, and it happened in the
document that records the pipeline's own findings.

## Defect 3 — `fill`'s four "HRM examples" prove nothing the sweep does not

```cpp
{FillMode::inclusive, false, 0x2418U, 0x3C18U, false},
```

Four such cases run first, with the expected words written out. Afterwards the
test sweeps every 16-bit input, both carry values and both modes — 262,144 cases
— against `hrm_oracle()`. The sweep therefore covers the four examples already,
and it covers them against the same oracle, so the hardcoded words add no
independent evidence.

They are still worth keeping, for one reason the file does not state: they are
the manual's own worked example, and a reader comparing the test to the page
wants to find it. What they lack is the page number. `README.md` in that
directory names pages 184, 185 and 267 for the file as a whole, which is not the
same as saying where `0x2418 -> 0x3C18` is printed.

## What is strong here

Three of the eight — `barrelshifter`, `fill`, `minterm` — carry a `hrm_oracle()`
function that computes the expected result from the described behaviour instead
of restating the RTL, and then drive it exhaustively:

| Test | Cases |
|---|---:|
| `agnus_blitter_barrelshifter` | 4,197,376 |
| `agnus_blitter_fill` | 393,244 |
| `agnus_blitter_minterm` | 3,102 |

`barrelshifter` and `fill` also assert their own case count before printing PASS,
so a loop that silently stops early fails the test. That is a better habit than
most of the layer-B suite has.

`minterm` deserves a specific note: its exhaustive stage drives A, B and C as
all-zeros or all-ones words, which turns the 8-bit LF byte into a truth table read
one selector at a time — every LF bit is separately confirmed to reach the output.
Then a second stage uses `0xF0F0 / 0xCCCC / 0xAAAA`, the three words that make the
16 bit positions enumerate all eight selectors twice, and asserts the output is
`lf | (lf << 8)`. Those two stages together pin the minterm encoding exactly.

## Citations

| Test | Oracle named | Where |
|---|---|---|
| `agnus_blitter_barrelshifter` | pages 179, 182, 267 | `README.md` |
| `agnus_blitter_fill` | pages 184, 185, 267 | `README.md` |
| `agnus_blitter_minterm` | pages 174, 175, 177, 178 | `README.md` |
| `agnus_blitter_line_octant` | — | nowhere |
| `agnus_blitter_mask` | — | nowhere |
| `agnus_blitter_modulo` | — | nowhere |
| `agnus_blitter_ptr` | — | nowhere |
| `agnus_blitter_zero` | — | nowhere |

No assertion in any of the eight carries a page number of its own, and none of
the eight declares `Test class:` or `Primary oracle:` in the source. That is what
puts all eight in layer C.

Two of the five uncited tests assert something specific enough that the pipeline
should be able to find its page, and they are the ones to check first:

- `modulo` expects a modulo of `$000A` to advance the word pointer by **5**, and
  `$FFF6` to move it back by 5 — that is, the register is read as a signed *byte*
  offset and applied to a word address. Where the manual says so is not recorded.
- `zero` expects `BZERO` to be set at initialisation, cleared by the first
  non-zero word stored, and **never restored** by a later zero word. The last part
  is the interesting claim and it is uncited.

## What this is for

When the chapter-6 package list arrives, every package meets this file:

- a package covering something already here confirms it, supersedes it, or is
  redundant — and says which;
- a claim here with no package is either out of the chapter's scope or a gap in
  the pipeline's reading, and the difference must be stated;
- nothing here counts as coverage until the pipeline has said which.

The three defects above are fixed as part of the chapter-6 run, not before it.
Fixing `line_octant` now would mean guessing the octant encoding from the RTL
again, which is how it got there.
