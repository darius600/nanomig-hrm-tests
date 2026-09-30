# ch8-dmaon-enable-composition — SPECIFICATION

Written from the quotations in `ch8-dmaon-enable-composition.jsonl` alone (the
8 claims assigned to this package by
`work/ch8-s2-build/ch8-dmaon-enable-composition.jsonl`). No RTL file
(`paula.v`, `paula_floppy.v`, or any other file under `nanomig/`) has been
opened by me yet. This document is committed before any of that happens.

Fixture assigned by the build task: `paula`. `paula` instantiates
`paula_floppy`, so both DSKBYTR (which the disk-side registers live behind)
and DMACON (Chapter 7's register, decoded one level up from
`paula_floppy`) are reachable from this single boundary — no other fixture
in this suite can separate DMACON's two contributing bits from DSKLEN's own
bit in one build. I have read `hrm/ch5-audio-dmacon-adkcon`'s `Makefile`
(the only existing package in this suite that already compiles `paula.v`)
for its `VERILOG_SOURCES` list and `verilator` flags — the file list needed
to elaborate `paula` at all, not a claim about what any signal does — and
cite nothing from its test source as an oracle for this package's own
assertions.

## What this package is for

Per the build task and the finding candidate this package's claims were
routed from (C11 in
`work/2026-09-15_ch8_s2_finding_candidates.md`): DSKBYTR bit 14, DMAON,
reports whether disk DMA is actually running. Two pages of the manual
describe what has to be true for DMAON to read 1, and they name different
sets of conditions:

- **p.248** (the chapter's own DMAON row, quoted by five of this package's
  eight claims) says DMAON requires **three** separate enables at once: the
  DMAEN bit of DSKLEN, **and** the DSKEN bit of DMACON, **and** the DMAEN
  bit of DMACON.
- **p.278** (Appendix A's DSKBYTR bit-14 row, quoted by the other three
  claims) says DMAON is "Mirror of bit 15 (DMAEN) in DSKLEN, ANDed with
  Bit09 (DMAEN) in DMACON" — **two** terms only. It does not name DMACON's
  DSKEN bit at all.

This package's central job is to grade both readings **separately and
explicitly**, each against its own page, rather than merging them into one
assertion that would hide which page the implementation actually matches.
If NanoMig requires all three terms (matching p.248) while the two-term
reading printed on p.278 turns out insufficient, that is **a
page-versus-page divergence in the manual itself**, not a red finding
against the RTL, and this document says so before any test is run — not
retroactively once a color comes back. A single merged assertion ("DMAON
requires DSKLEN.DMAEN and DMACON.DMAEN, plus maybe DSKEN") would be exactly
the kind of assertion that hides this.

## Assertions — one per proposition, each naming every citing uid and page

### A0 — FIXTURE_LIVENESS (no manual citation)

Not a page claim. Per the build task: "The fixture proves it can deliver a
known word before anything reads one." Before any DMAON-based assertion is
trusted, drive a distinct, known register value through this fixture's
data-out path (a plain, always-decoded register such as DSKBYTR itself, read
back idle) and confirm two different written states produce two different,
correctly-distinguishable readbacks. Required as its own graded,
mutation-recorded item.

### A1 — DMAON_P248_THREE_TERM_COMPOSITION (the p.248 reading; graded on its
own page)

| uid | page | quote |
|---|---|---|
| `opus:8-dskbytr.c9` | 248 | "     14      DMAON      Indicates when DMA is actually enabled.  All the\n                        various DMA bits must be true.  This means the\n                        DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\n                        in  DMACON ." |
| `sol:8-dskbytr.c10` | 248 | "All the\n                        various DMA bits must be true." |

Proposition, read literally off p.248: DSKBYTR bit 14 (DMAON) is true if and
only if **all three** of the following are simultaneously true — DSKLEN's
own DMAEN bit, DMACON's DSKEN bit, and DMACON's (master) DMAEN bit. Grade
this as a three-way composition: each of the eight combinations of the three
enables must produce the truth-table row the AND of all three predicts, on
this page's own terms — not just the one "all three set" row.

### A2 — DMAON_P278_TWO_TERM_COMPOSITION (the p.278 reading; graded on its
own page)

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskbytr.c11` | 278 | "                 14    DMAON      Mirror of bit 15 (DMAEN) in  DSKLEN ,\n                                  ANDed with Bit09 (DMAEN) in  DMACON" |
| `sol:8-appa-dskbytr.c11` | 278 | "                 14    DMAON      Mirror of bit 15 (DMAEN) in  DSKLEN ,\n                                  ANDed with Bit09 (DMAEN) in  DMACON" |

Proposition, read literally off p.278: DSKBYTR bit 14 (DMAON) is the AND of
exactly **two** terms — DSKLEN bit 15 (DMAEN) and DMACON bit 9 (DMAEN). As
printed, this page names no third term: DSKLEN.DMAEN=1 and DMACON.DMAEN=1
together are what this page says is sufficient, with DMACON's DSKEN bit
free to be either value. Graded literally: with DSKLEN.DMAEN=1 and
DMACON.DMAEN=1 held, does DMAON follow DMACON's DSKEN bit (contradicting
"two terms only, DSKEN irrelevant") or ignore it (matching p.278 exactly)?

**This is the package's page-versus-page pair with A1.** Per the build
brief's own advance measurement (and C11), NanoMig requires DSKEN as well —
so A1 (p.248, three terms) is expected to hold and A2 (p.278, two terms,
DSKEN irrelevant) is expected to fail, on the identical measurement basis
(the same DSKBYTR bit 14 readback, the same three drivable enables). A2
failing is not a defect finding against NanoMig; it is p.278 printing an
incomplete restatement of what p.248 states in full. Both assertions are
kept, graded independently, each citing only its own page's own words —
merging them into one assertion would erase exactly the fact this package
exists to record.

### A3 — DMAON_REQUIRES_DSKLEN_DMAEN (sub-term necessity, p.248)

| uid | page | quote |
|---|---|---|
| `sol:8-dskbytr.c11` | 248 | "This means the\n                        DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\n                        in  DMACON ." |

Proposition: DSKLEN's own DMAEN bit is one of DMAON's required terms —
with DMACON's DSKEN and DMAEN both held at the "enabled" value, clearing
DSKLEN's DMAEN alone must make DMAON read 0. (Both p.248 and p.278 agree on
this specific term; it is not part of the page-versus-page divergence A1/A2
grade.)

### A4 — DMAON_REQUIRES_DMACON_DSKEN (sub-term necessity, p.248 only)

| uid | page | quote |
|---|---|---|
| `sol:8-dskbytr.c12` | 248 | "This means the\n                        DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\n                        in  DMACON ." |

Flagged `out_of_slice` by the classifier because DMACON's own bit layout is
Chapter 7's subject; the half graded here is exactly what this package's own
claims file's `classify.evidence` already narrows it to — the disk-side
consequence of DMACON's DSKEN bit, observed at this fixture's `data_out`
(DSKBYTR), not DMACON's own decode.

Proposition: DMACON's DSKEN bit is one of DMAON's required terms — with
DSKLEN's DMAEN and DMACON's (master) DMAEN both held at the "enabled"
value, clearing DMACON's DSKEN alone must make DMAON read 0. **This is the
specific term p.278's two-term reading (A2) omits**, and is the crux of the
page-versus-page divergence: p.248 names it, p.278 does not.

### A5 — DMAON_REQUIRES_DMACON_DMAEN (sub-term necessity, both pages)

| uid | page | quote |
|---|---|---|
| `sol:8-dskbytr.c13` | 248 | "This means the\n                        DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\n                        in  DMACON ." |
| `sol:8-appa-dskbytr.c12` | 278 | "                 14    DMAON      Mirror of bit 15 (DMAEN) in  DSKLEN ,\n                                  ANDed with Bit09 (DMAEN) in  DMACON" |

The p.248 half is flagged `out_of_slice` by the classifier for the same
reason as A4 (DMACON's own bit layout belongs to Chapter 7); the half graded
here is the disk-side consequence at this fixture's `data_out`, per the
claims file's own evidence, same as A4. The p.278 half
(`sol:8-appa-dskbytr.c12`) restates the identical fact from Appendix A's own
words ("DMACON bit 9 is the global DMAEN term used in the DSKBYTR.DMAON
indication") — both pages agree DMACON's master DMAEN bit (bit 9) is a
required term, so this single assertion cites both rather than splitting
into two identical checks.

Proposition: DMACON's (master) DMAEN bit is one of DMAON's required terms —
with DSKLEN's DMAEN and DMACON's DSKEN both held at the "enabled" value,
clearing DMACON's master DMAEN alone must make DMAON read 0.

### A6 — DMACON_BIT4_NAMED_DSKEN_DISK_DMA_ENABLE (Appendix A's own DMACON
bit-4 row, p.278; written from the routed claims' quotations alone, before
the full claims file or any RTL beyond what A1-A5 already disclosed above
was opened for this proposition)

| uid | page | quote |
|---|---|---|
| `APPA-dmacon.c17` | 278 | "04    DSKEN       Disk DMA enable" |
| `CH4-appA-dmacon.c24` | 278 | "                 04    DSKEN       Disk DMA enable" |
| `5-appA-p278.c21` | 278 | "                 04    DSKEN       Disk DMA enable" |
| `A-dmacon.b04` | 278 | "                 04    DSKEN       Disk DMA enable" |

Proposition, read literally off p.278: DMACON's own bit-4 row, in the same
Appendix A register table and on the same printed page as A2's DSKBYTR
bit-14 row above, names bit 4 `DSKEN` and describes it in one line as "Disk
DMA enable" -- the disk channel's own DMA-enable term, as distinct from
DSKLEN's own DMAEN bit (A3) and from DMACON's master DMAEN bit (A5). Four
claims, from four independent readings/cross-reads of this identical row (a
mix of chapter-4 and chapter-5 `classify.package` labels, per this
package's own spec-stage file), all restate the same fact.

**Resolved from the full claims file.** All four ids route to this package
(`routing.route: package:ch8-dmaon-enable-composition`), and each one's own
`routing.how` names the same disposition: `A4-DMAON_REQUIRES_DMACON_DSKEN`
already grades the whole of this row's proposition at this package's
`paula` boundary -- with DSKLEN armed and DMACON's master DMAEN set, disk
DMA runs (DSKBYTR DMAON=1) only when DMACON bit 4 is also set, which is
exactly "DSKEN, Disk DMA enable" read as a functional claim. No new
assertion is added for A6; the four ids are written onto A4's own label
instead. The storage/read-back half of this same printed row (that DMACON's
own register decode returns bit 4 as written) is out of this package's
boundary -- the routed claims' own `classify.evidence` places that half at
a different fixture entirely, one this package does not build.

## Control case

**A1 (`DMAON_P248_THREE_TERM_COMPOSITION`) is this package's control case.**
It is graded on the identical measurement basis as A2 (the same DSKBYTR
bit-14 readback, the same set of drivable enables, the same fixture) and is
the reading this package's own advance measurement (and C11) says NanoMig
actually matches. A1 passing on that shared basis is what makes A2's
expected failure legible as a page-versus-page result rather than a
units-error or a broken harness: the same apparatus that finds A1 green
finds A2 red, on the same register, in the same run.

## What this package declines

Nothing in this claims file is declined outright — all 8 claims resolve
into the five propositions above (A1 through A5), each citing every uid
that quotes it. No claim needs a wider boundary than `paula` provides
(unlike the classifier's `observable_detail` notes for several of these
same claims against the narrower `paula_floppy` boundary — `paula` is
exactly the boundary that removes that obstacle, per the build task's own
fixture assignment) and none is a DSKDATR/DSKDAT "only" row carrying the
attack-stage `note` constraint the build task warns about (checked: no
claim in this package's `.jsonl` carries a `note` field at all).

## Rig hazards named by the build task (restated here, not yet acted on)

- `dsktrack` is reported to have no reset term in `paula_floppy.v`, so a
  reused Verilated model would carry head position between trials. This
  package will allocate a fresh model per trial. Not yet verified from RTL —
  accepted as a build constraint. (This package never drives `_step`, so it
  is not expected to matter, but the fresh-model rule will still be
  followed literally.)
- `paula_floppy_ebr.v` reportedly declares a second `module paula_floppy`;
  since the fixture is `paula` (which instantiates `paula_floppy` and
  `paula_floppy_fifo` directly, per `paula.v`'s own file list as already
  disclosed by `hrm/ch5-audio-dmacon-adkcon`'s Makefile), the EBR file is
  not part of this fixture's source list and this hazard should not arise.
- DSKLEN reportedly needs a double write (`paula_floppy.v:743`) to arm DMA.
  This package's own assertions grade DMACON's terms (DSKEN, DMAEN) against
  DSKBYTR's DMAON bit; wherever DSKLEN's own DMAEN needs to be set to
  produce a true reading, the double-write will be performed, per this
  already-disclosed fact (from the build task text, not yet independently
  read from `paula_floppy.v` by me).
- `agnus_diskdma` reportedly has no `ecs` port. Not relevant to this
  package: `agnus_diskdma` is not part of the `paula` fixture.

## Mutation plan

Each assertion above gets a `// MUTATION <label> <oracle|stimulus|none>
<red|green|impossible> <what changed>` record in the C++ source (not a
separate file), `<label>` being the exact leading token of the string
passed to `expect()`. The default plan is a stimulus mutation: change what
the fixture is asked to do (a different DMACON/DSKLEN write, a different
term held high or low) while leaving the expected value untouched, and
confirm the assertion reddens, then revert.

A2 (`DMAON_P278_TWO_TERM_COMPOSITION`) is expected, from this package's own
claims file and the build task's advance measurement, to be **red at
baseline** against the RTL (DMACON's DSKEN term, which p.278's two-term
reading treats as irrelevant, is in fact required). Per the build task, a
stimulus mutation on an already-red assertion proves nothing (it cannot
fail harder); for A2, the substitute rule applies instead — a different
mechanism proven live elsewhere in the same run (A1's own live three-term
composition, or A4/A5's own live necessity checks) is put through the same
comparator form and shown to go green, demonstrating the comparator itself
is not simply broken or vacuously red.

A0 (fixture liveness) gets its own mutation: drive the second known value
identical to the first, so the two readings become indistinguishable, and
confirm the liveness check reddens.

## What happens next

Only after this file is committed: read `paula.v`, `paula_floppy.v`, and
`paula_floppy_fifo.v` for the actual port names (DMACON's word address and
bit positions, DSKLEN's word address, DSKBYTR's word address and bit 14,
`reg_address_in`/`data_in`/`data_out` conventions, the `enable` wiring
between `paula.v` and `paula_floppy`), and read an existing sibling
package's Makefile/test source (`hrm/ch8-dsklen-dma-arming`, already
committed, and `hrm/ch5-audio-dmacon-adkcon`, which already compiles
`paula.v`) for this suite's established harness/mutation boilerplate. All
of that reading will be disclosed in the next commit.

## A0's disposition

A0 (fixture liveness) is withdrawn to `note()` — it carries no claim uid, so
it is never conformance evidence; grading it through `expect()` costs no
coverage to remove. A1-A5 and A2-SUBSTITUTE are otherwise unchanged; A2 stays the
package's one genuine red (p.278's incomplete two-term reading, finding
candidate C11). Tally: 5 passed, 1 failed.
