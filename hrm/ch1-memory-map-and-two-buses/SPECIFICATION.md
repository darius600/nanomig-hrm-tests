# Specification — `ch1-memory-map-and-two-buses`

Package: `hrm/ch1-memory-map-and-two-buses`
Fixture: `gary` — one module, compiled alone, as in
`ch7-custom-address-space` / `ch7-reset-overlay` (both build `gary`
standalone in this suite).
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered
for `classify.package == "ch1-memory-map-and-two-buses"` — 49 claims, all
`classify.status: "testable"` (measured: zero `not-asserted` /
`insufficient` / `absent` claims in this package's own 49; every claim
needs a graded line or an explicit cross-reference, never a bare
withdrawal).
Subject: chapter 1's memory map as `gary` implements it — the Kickstart ROM
window and its 256K mirror, the CPU's total address reach, the Chip-memory
and slow-memory regions and why each is named what it is, the CIA/Paula
address split, the "custom chips only reach Chip memory" residence rule,
and every block, box and bus Figure 1-1 (p.8) draws inside or immediately
around `gary`'s own boundary — CIA #1/#2, Kickstart ROM, the System
Expansion hard-disk block, BUFFERS AND ARBITRATION LOGIC, the CUSTOM CHIP
SECTION enclosure, CHIP RAM, the "PSEUDO" FAST RAM box and its footnote,
the CPU DATA/ADDRESS buses, and the CHIP ADR BUS/CHIP DATA/MUX group.

**Sources**: the 49 claim records of this package. Every one of their
`classify.evidence` fields already contains `gary.v` file:line citations
and signal names (`sel_chip`, `sel_slow`, `sel_kick`, `dbr`,
`cpu_address_in`, `dma_address_in`, `ram_address_out`, `dbs`, and others)
from the upstream classifier. This file names nothing from `gary.v` beyond
what a claim's own `evidence` string already names; concrete stimulus
recipes, register offsets and the `expect()` assertions are in
`test_gary.cpp`.

## Figure 1-1 checked independently

`docs/references/amiga-hrm-3rd/ch1/figures/p8.png` (1258×1633), read
directly from the image, with the drawing's small text and arrowheads
checked at ~2.5-3x zoom (the CUSTOM CHIP SECTION, the MUX and CHIP RAM, the
CPU buses, and SYSTEM EXPANSION). What the drawing carries, read from the
image rather than taken from any restatement:

- The **MUX** box takes one incoming arrowhead from **CHIP ADR BUS** at its
  top edge; its output leaves the bottom, turns right, and ends in exactly
  **one** arrowhead at **CHIP RAM**'s left edge. The **"PSEUDO" FAST RAM**
  box directly below has **no incoming line of any kind** — no arrowhead,
  no plain line, from the MUX or from anything else.
- The **"PSEUDO" FAST RAM** box is drawn in its own dashed outline, nested
  inside the dashed **CUSTOM CHIP SECTION** enclosure. The drawing carries
  no key and no legend anywhere; the **SYSTEM EXPANSION** enclosure at the
  top left is dashed in the identical style while holding **OTHER EXPANSION
  DEVICES**, **HARD DISK INTERFACE**, **FAST RAM** and **EXPANSION
  CONNECTOR(S)** — none of them optional. The single footnote in the whole
  figure reads `* addressed as CHIP RAM with 1MB Agnus`, matching the
  asterisk on the "PSEUDO" FAST RAM box; it says nothing about optionality.
- The **CHIP DATA** vertical bar's arrowhead enters **CHIP RAM** at its top
  edge; separately, a horizontal line at the same height carries one
  arrowhead **left** into **BUFFERS AND ARBITRATION LOGIC** and one
  arrowhead **right** into **PAULA**. Neither **AGNUS** nor **DENISE** has
  any incoming line from this bar — AGNUS is reached only by **CHIP ADR
  BUS** (address, not data) and by **REG ADR BUS** on the opposite side;
  DENISE is reached only by **REG ADR BUS**.
- **CIA #1 (8520)**, **CIA #2 (8520)** and **KICKSTART ROM** each hang off
  **CPU DATA** and **CPU ADDRESS** by a pair of downward arrows, on the CPU
  side of **BUFFERS AND ARBITRATION LOGIC**, outside the dashed CUSTOM CHIP
  SECTION.
- **BUFFERS AND ARBITRATION LOGIC** sits astride the CUSTOM CHIP SECTION's
  left dashed boundary; **CPU DATA** and **CPU ADDRESS** both end at its
  left edge in double-headed (bidirectional) arrows, and **CHIP ADR BUS**
  and **CHIP DATA** both leave its right edge.
- Inside **SYSTEM EXPANSION**, **HARD DISK INTERFACE** sits between **OTHER
  EXPANSION DEVICES** and **FAST RAM**, joined top-to-bottom by plain
  undirected bars (no arrowheads) — a chain, not a bus tap.

Two readings of this figure over-read it:
**`opus:1-blockdiag.c14`**'s "optional (dashed)" reading of the "PSEUDO"
FAST RAM box, and **`sol:1-blockdiag.c22`**'s "into Chip RAM and
pseudo-fast RAM" reading of the MUX output. Neither of those two uids is
in this package's own 49 — both are classified `not-asserted` /
`source-silent` with no `classify.package` field set, so they belong to no
package's claim inventory — and both are recorded here because they
concern the figure this package grades. See "Two claims outside this
package's own 49" near the end of this file.

## Grep check: is any of this already graded elsewhere?

`hrm/` is searched (directory listing plus `grep -rl` for `gary\.v`,
`sel_chip`, `sel_slow`, `sel_kick`, `sel_cia_a`, `sel_cia_b`, `sel_reg`,
`"16 megabytes"`, `"9 megabytes"`, `"Chip memory"`, `"chip memory"`,
`kickstart`, `"pseudo.fast"`, `"system expansion"`, `"CUSTOM CHIP
SECTION"`) across every package's `SPECIFICATION.md` and `test_*.cpp`,
including the chapter-1 packages `ch1-cpu-and-instruction-cache`,
`ch1-chip-memory-banking`, `ch1-cpu-options` and
`ch1-keyboard-translation`. Two packages build `gary` alone:
`ch7-custom-address-space` and `ch7-reset-overlay`.

**One direct hit**, and this package declines to re-grade it:

- **`ch7-custom-address-space`**'s `DMA-CHIP-MEMORY-ONLY-SWEEP` assertion
  (PASS, confirmed in `ch7-custom-address-space/out.txt`) already grades,
  at this exact fixture, the identical structural proposition this
  package's own `sol:1-guidelines.c27` / `opus:1-guidelines.c27` (p.12,
  "All data for the custom chips must reside in Chip memory (type
  MEMF_CHIP)") states: it drives `dbr=1` and sweeps `dma_address_in` over
  its full representable 2MB range ($000000, $080000, $100000, $1FFFFE),
  showing `sel_chip[3:0]` is always exactly one-hot and `sel_kick` /
  `sel_kick256kmirror` are never asserted — custom-chip (DMA) access reaches
  Chip memory and nothing else. That assertion is itself grounded in
  `opus:7-appA-pre.c8` (p.263, "The custom chips can only access Chip
  memory; using a non-Chip address will fail"), a different printed
  location stating the same fact this package's p.12 claim restates as a
  development guideline. `sol:1-guidelines.c27` / `opus:1-guidelines.c27`
  are put on a `record()` line citing that package and that uid; they are
  not re-graded here. See "PROP-DMA-CHIP-ONLY —
  declined, already graded" below.

**No other hit.** `ch1-chip-memory-banking` (fixture `minimig_bankmapper`)
grades a related but distinct question — how many *physically distinct*
512K banks a given `memory_config` value produces — under its own
different-page uids (`sol:1-components.c93`, `opus:1-components.c92`, p.5);
its own `SPECIFICATION.md` explicitly separates that from "`gary.v:159`...
a different claim" and names both `ch7-custom-address-space` and
`ch7-reset-overlay` as the two existing `gary` packages closest to its own
territory, neither of which it finds overlapping. `ch1-cpu-and-instruction-
cache` (fixture `cpu_wrapper`) owns the page-8 lead-in sentence itself
(`sol:1-blockdiag.c2` / `opus:1-blockdiag.c2`, "Notice that there are two
separate buses...") under its own `PROP-5`, and the Figure 1-1 FAST RAM box
(`sol:1-blockdiag.c8` / `opus:1-blockdiag.c8`) under its own `PROP-6` —
**neither of those two uid pairs is in this package's 49-claim file at
all** (confirmed against this package's own `claims.jsonl` dump); see "The
page-8 lead-in sentence belongs to a sibling package" below.
`ch7-reset-overlay`'s
`PROP-A` (p.223, "chips access Chip memory directly via DMA, rather than
... bus arbitration") is about the DMA-vs-arbitration *mechanism*, not
about which addresses decode where; it shares no uid with this package and
is not the same proposition as any of this package's `PROP-SLOW-*`
assertions. No package in this suite currently cites `sel_slow`, `sel_kick`
(outside the two `ch7-*gary` packages, whose own citations are the p.263/
p.223 material just described), `sel_cia_a`/`sel_cia_b`, or `sel_reg`.

## The page-8 lead-in sentence belongs to a sibling package

The page-8 lead-in sentence — *"Notice that there are two separate buses,
one that only the CPU can access (Fast memory) and another one that the
custom chips share with the CPU (Chip memory)"* — is `sol:1-blockdiag.c2` /
`opus:1-blockdiag.c2`, and both carry `classify.package:
"ch1-cpu-and-instruction-cache"`, **not**
`"ch1-memory-map-and-two-buses"`. It is not among this package's 49 claims,
and this file does not grade it or cite its uid on any graded line — doing
so would grade another package's claim under this package's roof. What
this package *does* carry, fully within its own 49 claims, is the
Chip-memory-is-shared half of the same idea, restated on p.3 and p.4
(`PROP-CHIPMEM-SHARED-P3`, `PROP-CHIPMEM-SHARED-P4` below) and on 27 of
Figure 1-1's 33 classified-testable figure claims, all assigned to this
package and graded below. The specific uid that states the two-bus idea
most directly belongs to a sibling package.

## Claim accounting (all 49)

| Group | Uids | Count | Page(s) | Disposition |
|---|---|---:|---|---|
| PROP-KICKSTART-512K | sol/opus `1-components.c13` | 2 | 1 | Graded |
| PROP-KICKSTART-256K-MIRROR | sol/opus `1-components.c14` | 2 | 1 | Graded |
| PROP-CPU-ADDR-SPACE-16MB | sol/opus `1-components.c28` | 2 | 2 | Graded |
| PROP-CPU-RAM-CEILING-9MB | sol/opus `1-components.c29` | 2 | 2 | Graded (gary's own share only) + `record()` for the composed remainder |
| PROP-CHIPMEM-SHARED-P3 | sol/opus `1-components.c39` | 2 | 3 | Graded |
| PROP-CHIPMEM-512K-FLOOR | sol/opus `1-components.c41` | 2 | 3 | Graded |
| PROP-CHIPMEM-SHARED-P4 | sol `1-components.c82`, opus `1-components.c81` | 2 | 4 | Graded |
| PROP-SLOW-MEM-LOCATION-512K | sol `1-components.c96`, opus `1-components.c95` | 2 | 5 | Graded |
| PROP-SLOW-MEM-NAME-CONTENTION | sol `1-components.c98`, opus `1-components.c97` | 2 | 5 | Graded |
| PROP-SLOW-MEM-SAME-GATE | sol `1-components.c99`, opus `1-components.c98` | 2 | 5 | Graded |
| PROP-CHIPMEM-AMOUNT-SCALES | sol `1-components.c105`, opus `1-components.c104` | 2 | 5 | Graded |
| PROP-CIA-PAULA-ADDRESS-SPLIT | sol/opus `1-components.c124` | 2 | 6 | Graded |
| PROP-DMA-CHIP-ONLY (declined) | sol/opus `1-guidelines.c27` | 2 | 12 | `record()`, cites `ch7-custom-address-space` |
| PROP-FIRST-512K-FLOOR-AND-SCALE | sol/opus `1-guidelines.c31` | 2 | 12 | Graded |
| PROP-CIA1-BLOCK | sol `1-blockdiag.c4` (+opus `c4` jointly) | 1 | 8 (fig.) | Graded |
| PROP-CIA2-BLOCK | sol `1-blockdiag.c5` (+opus `c4` jointly) | 1 | 8 (fig.) | Graded |
| PROP-KICKROM-BLOCK | sol `c6`, opus `c5` | 2 | 8 (fig.) | Graded |
| PROP-SYSEXP-HDD | sol `c9` | 1 | 8 (fig.) | Graded |
| PROP-BUFARB-BOUNDARY | sol `c11`, opus `c7` | 2 | 8 (fig.) | Graded |
| PROP-CHIPRAM-IN-SECTION | sol `c15`, opus `c13` | 2 | 8 (fig.) | Graded |
| PROP-PSEUDOFAST-BOX | sol `c16` | 1 | 8 (fig.) | Graded |
| PROP-FOOTNOTE-1MB-AGNUS | sol `c17`, opus `c15`, opus `c16` | 3 | 8 (fig. + text layer) | Graded |
| PROP-CPUBUS-TAPS | sol `c18`, opus `c6` | 2 | 8 (fig.) | Graded |
| PROP-CHIPADRBUS-MUX | sol `c19`, opus `c11` | 2 | 8 (fig.) | Graded |
| PROP-CHIPDATA-SHARED | sol `c20`, opus `c12` | 2 | 8 (fig.) | Graded, caution noted |
| PROP-CUSTOMCHIPSECTION-ENCLOSURE | opus `c9` | 1 | 8 (fig.) | Graded |
| opus `c4` used jointly | — | (already counted once, on both CIA props) | 8 (fig.) | Graded |

Text/guideline uids: 2×14 = 28. Figure uids: 21 (`opus:c4` cited on two
proposition labels but counted once). 28 + 21 = 49. Every uid in this
package's `claims.jsonl` appears in exactly one row above (the joint
`opus:c4` citation on both CIA propositions is the one deliberate
exception, matching the accepted `hrm/ch8-register-map-userio` precedent of
citing one uid on more than one graded line where the source genuinely
states both facts at once).

---

## PROP-KICKSTART-512K

Page 1.

> 5 12K of system ROM containing a real time, multitasking operating system
> with sound, graphics, and animation support routines.
— `sol:1-components.c13`, `opus:1-components.c13` (identical quote; twin.
Both readers' own scan-damage note: "5 12K" is "512K".)

**Proposition to test:** the system ROM (Kickstart) occupies a 512K address
window. The relative clause — that the ROM's *contents* are a real-time
multitasking OS with sound/graphics/animation routines — describes a
software image, not a hardware boundary; both claims' own `evidence` field
already says so ("not graded at this boundary"), and this file does not
manufacture a fresh test for it.

Status: testable.

## PROP-KICKSTART-256K-MIRROR

Page 1.

> (V I.3 and earlier versions of the OS used 256K of system ROM.)
— `sol:1-components.c14`, `opus:1-components.c14` (identical quote; twin.
"V I.3" is "V1.3".)

**Proposition to test:** a 256K ROM image is addressable within the same
window PROP-KICKSTART-512K occupies (a mirrored/folded arrangement, since
256K is half of 512K). Which OS *version* used the smaller ROM is not a
hardware fact and is not graded.

Status: testable.

## PROP-CPU-ADDR-SPACE-16MB

Page 2.

> The 68000 has an address space of 16 megabytes.
— `sol:1-components.c28`, `opus:1-components.c28` (identical quote; twin.)

**Proposition to test:** the CPU-side address bus this fixture accepts is
exactly wide enough for 16 megabytes (24 address bits / a byte space of
2^24) and no wider — an address bit above that range cannot be presented at
all, and nothing this fixture decodes lies outside it.

Status: testable.

## PROP-CPU-RAM-CEILING-9MB

Page 2.

> In the Amiga, the 68000 can address up to 9 megabytes of random access
> memory (RAM).
— `sol:1-components.c29`, `opus:1-components.c29` (near-identical quote;
twin — opus's restatement: "In an Amiga the 68000 can address up to 9
megabytes of RAM.")

**Proposition to test, and a boundary this package cannot cross:** both
claims' own `evidence` field already discloses that the 9-megabyte figure
composes across two decoders — this fixture's own chip-memory and
slow-memory windows, plus a Zorro-II expansion window the same field places
"one level out" at a different top module, outside `gary`. **This package's
fixture is `gary` alone** (one Verilator build, matching every other
package's boundary convention in this suite); it cannot instantiate or
observe that other module. What is testable here is `gary`'s own share of
the figure: the total distinct address range `gary` itself will answer as
RAM (Chip memory plus slow memory, at whatever `memory_config` allows,
strictly within the 16-megabyte space PROP-CPU-ADDR-SPACE-16MB bounds), and
that it does not exceed what `gary` alone can contribute toward 9MB. The
remainder of the figure — composed from a different module — is not graded
under this claim's uid; see "Claims
graded only in part" below for exactly how the uid is kept on a `record()`
line rather than an `expect()` label: a partially-gradable claim's uid may
not sit on the graded line itself.

Status: testable (`gary`'s own contribution only; the composed total is
out of this package's boundary — recorded, not invented).

## PROP-CHIPMEM-SHARED-P3

Page 3.

> The memory shared between the Amiga's CPU and the custom chips is called
> Chip memory.
— `sol:1-components.c39`, `opus:1-components.c39` (identical quote; twin.)

**Proposition to test:** there exists memory this fixture's CPU-side port
and its custom-chip (DMA) side port can each reach directly — the same
storage answering both sides. That the region is *named* "Chip memory" is
a naming convention, not itself graded; the existence of memory both sides
can reach directly is the sentence's hardware substance.

Status: testable.

## PROP-CHIPMEM-512K-FLOOR

Page 3.

> All Amigas can access at least 512K of Chip memory.
— `sol:1-components.c41`, `opus:1-components.c41` (identical quote, "5 12K"
scan damage in both. Opus's own note: its restatement risks turning
*addressability* into *installed capacity* — "has at least 512K" vs. "can
access at least 512K" — and explicitly says the claim is graded as printed,
the addressability question.)

**Proposition to test:** the first 512K of the CPU-side address space
selects Chip memory regardless of how this fixture's memory-size
configuration is set — a floor that does not depend on configuration. "All
Amigas" is model language and is not gradeable at one board; the
addressability floor is.

Status: testable.

## PROP-CHIPMEM-SHARED-P4

Page 4.

> This shared memory that both the custom chips and the CPU can access
> directly is called Chip memory.
— `sol:1-components.c82`, `opus:1-components.c81` (near-identical quote;
twin restatement of the same structural fact PROP-CHIPMEM-SHARED-P3 states,
from a different printed location one page later.)

**Kept as its own assertion, not folded into PROP-CHIPMEM-SHARED-P3**,
following the precedent the accepted `ch7-custom-address-space` package set
for its own PROP-C/PROP-D ("kept as two separate assertions with two
separate page citations, each grounded in its own quotation... Collapsing
them into one assertion would under-cite the manual, even where the same
fixture measurement may end up answering both").

**Proposition to test:** the same direct-access-from-both-sides fact
PROP-CHIPMEM-SHARED-P3 tests, restated on p.4 and graded here under its own
page citation and its own uids.

Status: testable.

## PROP-SLOW-MEM-LOCATION-512K

Page 5.

> The Fat Agnus chip has DMA access to 512K of Chip memory, just like the
> original Agnus, but also allows an additional 512K of internal slow
> memory or pseudo-fast memory located at ($00C00000).
— `sol:1-components.c96`, `opus:1-components.c95` (near-identical quote,
scan-damaged "S00C0 0000" in both, both readers correct it to
`$00C00000`.)

**Proposition to test:** a second, distinct 512K region — slow memory /
pseudo-fast memory — is addressable starting at $00C00000, separate from
the 512K Chip-memory region PROP-CHIPMEM-512K-FLOOR locates at the bottom
of the map; and DMA (custom-chip) access to the first 512K of Chip memory
exists as its own, separately observable fact.

Status: testable.

## PROP-SLOW-MEM-NAME-CONTENTION

Page 5.

> The name slow memory comes from the fact that bus contention with the
> custom chips can still occur even though only the CPU can access the
> memory.
— `sol:1-components.c98`, `opus:1-components.c97` (identical quote; twin.)

**Proposition to test:** access to the slow-memory region from the CPU side
can be held off by the same busy/contention condition that holds off access
to Chip memory, even though (per PROP-SLOW-MEM-LOCATION-512K) only the CPU,
not DMA, ordinarily reaches slow memory. The *name* "slow memory" is not
itself graded; the contention mechanism the name is explained by is.

Status: testable.

## PROP-SLOW-MEM-SAME-GATE

Page 5.

> Since slow memory is arbitrated by the same gate that controls Chip
> memory, the custom chips can block processor access to slow memory in the
> higher resolution video modes.
— `sol:1-components.c99`, `opus:1-components.c98` (identical quote; twin.)

**Proposition to test:** the same busy/contention signal that can block CPU
access to Chip memory can also block CPU access to slow memory — one gate,
both regions. *Which* video modes or DMA activity actually raises that
signal is a fact about Agnus's own bitplane-DMA timing, a different module
outside this fixture's boundary; both claims' own `evidence` field already
notes the density mechanism lives at `agnus_bitplanedma.v`, not `gary.v`.
This package tests the shared-gate structure directly (drive the busy
condition, observe both regions blocked alike), not the video-mode trigger
for it.

Status: testable (shared-gate structure; the video-mode trigger is a
different module's fact, not graded here).

## PROP-CHIPMEM-AMOUNT-SCALES

Page 5.

> The amount of Chip memory is important since it determines how much
> graphics, audio, and disk data the custom chips can operate on without
> the 680x0 CPU.
— `sol:1-components.c105`, `opus:1-components.c104` (identical quote;
twin.)

**Proposition to test:** the amount of Chip memory is a real,
capacity-bounded quantity at this boundary: every 512K block within the
custom-chip (DMA) side's representable address range selects a distinct
chip bank, and nothing beyond that range can be presented. The claims'
`evidence` proposes varying `memory_config` instead; `test_gary.cpp`'s
header gives the reason this boundary is graded as a ceiling. *What* the
custom chips do with that address range (graphics, audio, disk data
specifically) is chapter 3/5/8 territory and is not re-graded here.

Status: testable.

## PROP-CIA-PAULA-ADDRESS-SPLIT

Page 6.

> The circuitry for some of these peripherals resides on the Paula custom
> chip while the Amiga's two 8520 CIA chips handle other I/O chores not
> specifically assigned to any of the custom chips.
— `sol:1-components.c124`, `opus:1-components.c124` (identical quote;
twin.)

**Proposition to test:** the CPU-side address space this fixture decodes
splits the CIA chips' address window from the custom-register window where
Paula (and the other custom chips) answer — two disjoint, independently
selectable address ranges, matching the sentence's division of I/O between
"on Paula" and "on the two CIAs."

Status: testable.

## PROP-FIRST-512K-FLOOR-AND-SCALE

Page 12 (General Amiga Development Guidelines).

> (On all models of the Amiga, the first 512K of memory is Chip memory and
> in some later models, Chip memory may occupy the first one or two
> megabytes).
— `sol:1-guidelines.c31`, `opus:1-guidelines.c31` (identical quote; twin.)

**Proposition to test:** the first 512K of the CPU-side address space
always selects Chip memory (the same floor PROP-CHIPMEM-512K-FLOOR tests,
restated here on p.12 and graded under its own uids per the
no-under-citing precedent), and `gary`'s own decode permits Chip-memory
selection up through the one- and two-megabyte points the sentence names,
with the floor unconditional beneath it (which logical blocks are backed
by distinct physical memory is `minimig_bankmapper`'s question, not
graded here).

Status: testable.

---

## PROP-DMA-CHIP-ONLY — declined, already graded

Page 12 (General Amiga Development Guidelines).

> All data for the custom chips must reside in Chip memory (type MEMF_CHIP).
— `sol:1-guidelines.c27`, `opus:1-guidelines.c27` (identical quote; twin.)

**Not graded in this package.** See "Grep check" above:
`ch7-custom-address-space`'s `DMA-CHIP-MEMORY-ONLY-SWEEP` assertion already
grades this exact structural proposition at this exact fixture (`gary`),
PASS, citing `opus:7-appA-pre.c8` (p.263, the same fact stated at a
different printed location). Re-grading it here risks this chapter
reaching a different verdict from an existing one about the same
sentence. The MEMF_CHIP software-allocation-flag half of the sentence
has no hardware counterpart at any boundary and is not gradeable regardless
of which package holds the uid.

Status: testable; assigned elsewhere's measurement, not re-tested here.

---

## Figure 1-1 propositions (page 8)

Every quote below is the same marker line,
`[FIGURE 1-1 -- figures/p8.png -- not text; open the image and read it]`
(source_kind `figure`, except `opus:1-blockdiag.c16` which is `text` — the
scanned text layer carrying the same footnote in damaged form, see
PROP-FOOTNOTE-1MB-AGNUS). A figure claim's `quote` is a marker, not
evidence; the evidence is "Figure 1-1 checked independently" above, read
directly from the image at 2.5-3x zoom.

## PROP-CIA1-BLOCK

`sol:1-blockdiag.c4` — restatement: "One box labels CIA #1 as an 8520
device." `opus:1-blockdiag.c4` — restatement bundles both CIAs: "Two
identical chip blocks hang below the CPU buses, labelled CIA #1 (8520) and
CIA #2 (8520): the machine has two 8520 CIA chips on the CPU bus." (cited
jointly on this proposition and on PROP-CIA2-BLOCK, since it states both
facts at once — see the claim-accounting note above.)

**Proposition to test:** an address window on the CPU-side bus, distinct
from every other window this fixture decodes, selects a CIA-class register
interface — the block the figure labels CIA #1. (The "8520" part number is
not observable at this boundary and is not graded.)

Status: testable.

## PROP-CIA2-BLOCK

`sol:1-blockdiag.c5` — restatement: "A separate box labels CIA #2 as an
8520 device." `opus:1-blockdiag.c4` (joint citation, as above).

**Proposition to test:** a second, distinct CIA-class address window,
separate from CIA #1's, selects the block the figure labels CIA #2.

Status: testable.

## PROP-KICKROM-BLOCK

`sol:1-blockdiag.c6` — restatement: "A box identifies Kickstart ROM on the
CPU address and data buses." `opus:1-blockdiag.c5` — restatement: "A block
labelled KICKSTART ROM hangs off the CPU buses next to the two CIAs: the
Kickstart ROM sits on the CPU side of the machine, not behind the
arbitration logic."

**Proposition to test:** the Kickstart ROM window (PROP-KICKSTART-512K) is
reached directly off the CPU-side bus, on the CPU side of the figure's
BUFFERS AND ARBITRATION LOGIC block — not through the custom-chip (DMA)
side.

Status: testable.

## PROP-SYSEXP-HDD

`sol:1-blockdiag.c9` — restatement: "The System Expansion area includes a
hard-disk interface."

**Proposition to test:** a hard-disk-interface address decode exists on the
CPU-only side of the map, grouped in the figure with the other System
Expansion blocks (FAST RAM, OTHER EXPANSION DEVICES, EXPANSION
CONNECTOR(S)) and outside the custom-chip (DMA) side entirely.

Status: testable.

## PROP-BUFARB-BOUNDARY

`sol:1-blockdiag.c11` — restatement: "Buffers and arbitration logic sit
between the CPU buses and the custom-chip section." `opus:1-blockdiag.c7`
— restatement: "A block labelled BUFFERS AND ARBITRATION LOGIC sits
between the CPU data and address buses and the custom chip section: every
CPU access into chip-side memory passes through this arbitration block."

**Proposition to test:** every path from the CPU-side bus into the
custom-chip-shared (Chip-memory) side passes through one common point — the
figure's BUFFERS AND ARBITRATION LOGIC block — rather than reaching Chip
memory by some other, independent route.

Status: testable.

## PROP-CHIPRAM-IN-SECTION

`sol:1-blockdiag.c15` — restatement: "Chip RAM is shown in the custom-chip
section." `opus:1-blockdiag.c13` — restatement: "A block labelled CHIP RAM
sits inside the custom chip section, fed by the chip address bus through
the MUX and by the CHIP DATA bus: Chip RAM is on the custom chip side of
the arbitration logic."

**Proposition to test:** the storage this fixture's custom-chip (DMA) side
reaches directly is the same storage reachable from the CPU side through
BUFFERS AND ARBITRATION LOGIC — Chip RAM sits on the far side of the
arbitration point, not on the CPU-only side with Kickstart ROM and the
CIAs.

Status: testable.

## PROP-PSEUDOFAST-BOX

`sol:1-blockdiag.c16` — restatement: "A dashed box below Chip RAM is
labeled 'PSEUDO' FAST RAM with a footnote marker."

**Proposition to test:** a region distinct from Chip RAM, drawn adjoining
it inside the same custom-chip-side enclosure and carrying the same
footnote marker PROP-FOOTNOTE-1MB-AGNUS explains, exists at this boundary —
the slow-memory / pseudo-fast-memory region PROP-SLOW-MEM-LOCATION-512K
already locates at $00C00000. (Whether the box's *dashed* drawing style
carries any meaning at this boundary is exactly the question
`opus:1-blockdiag.c14` over-read and this package does not — see "Figure
1-1 checked independently" above and the two out-of-package records at the
end of this file. This claim's own restatement says only "a footnote
marker," not "optional," and is graded as printed.)

Status: testable.

## PROP-FOOTNOTE-1MB-AGNUS

`sol:1-blockdiag.c17` — restatement: "The footnote says pseudo-fast RAM is
addressed as Chip RAM with a 1 MB Agnus." `opus:1-blockdiag.c15` — same
footnote, restated identically. `opus:1-blockdiag.c16` — the *text layer*
of the same page, carrying the identical footnote in badly OCR-damaged
form (`"* aJdrtsstdas CHIP RAM with 1MB Agnus"`, i.e. "addressed as CHIP
RAM with 1MB Agnus"); the reader's own restatement discloses the scan
damage and resolves it against the figure. All three are the same printed
fact from the same footnote, reached through two different extraction
channels (the image, and the page's separately-extracted text layer) —
kept as one proposition rather than split, since it is one footnote, not
two independent sentences.

**Proposition to test:** under a configuration this fixture identifies as
"1MB Agnus," the pseudo-fast-memory region PROP-PSEUDOFAST-BOX names
becomes addressable through the same decode path as Chip RAM, rather than
through its own separate path.

Status: testable.

## PROP-CPUBUS-TAPS

`sol:1-blockdiag.c18` — restatement: "CPU DATA and CPU ADDRESS buses
connect the 68000 to the CIAs, Kickstart ROM, expansion connector(s), and
buffers/arbitration logic." `opus:1-blockdiag.c6` — restatement: "Two
labelled horizontal buses run right from the CPU block... the CIAs, the
Kickstart ROM and the expansion connector all tap off these two, and both
end in double-headed arrows at the BUFFERS AND ARBITRATION LOGIC block."

**Proposition to test:** CIA #1, CIA #2, Kickstart ROM and BUFFERS AND
ARBITRATION LOGIC are each reached by decoding the same, single CPU-side
address bus this fixture accepts — four distinct, non-overlapping windows
on one shared bus, matching the figure's drawing of one CPU DATA/CPU
ADDRESS pair tapped by all four. (The expansion connector has no
counterpart at this boundary — it leaves the CPU bus in the opposite
direction, toward System Expansion, and is not decoded by `gary` itself;
not graded.)

Status: testable.

## PROP-CHIPADRBUS-MUX

`sol:1-blockdiag.c19` — restatement: "A CHIP ADDRESS bus runs from the
buffers/arbitration logic toward Agnus and the memory multiplexer."
`opus:1-blockdiag.c11` — restatement: "A vertical bus labelled CHIP ADR BUS
runs from the buffers and arbitration logic down into a block labelled
MUX, whose output arrow goes into CHIP RAM: chip-side addresses reach Chip
RAM through a multiplexer."

**Proposition to test:** the address BUFFERS AND ARBITRATION LOGIC presents
on its custom-chip side is the same address that reaches Chip RAM through a
selecting/multiplexing point — i.e., this fixture's own address output
toward memory is chosen between (at least) two sources rather than being a
single fixed wire, matching the figure's MUX between the arbitration
point and Chip RAM.

Status: testable.

## PROP-CHIPDATA-SHARED

`sol:1-blockdiag.c20` — restatement: "A CHIP DATA path connects the
buffers/arbitration logic and custom-chip section to Chip RAM."
`opus:1-blockdiag.c12` — restatement: "A vertical bus labelled CHIP DATA
runs from the line that feeds AGNUS, PAULA and DENISE down into CHIP RAM,
and the same line carries an arrowhead back into the buffers and
arbitration logic: chip data is shared between the custom chips, Chip RAM
and the CPU side." **Caution, from the image as read in "Figure 1-1
checked independently" above**: the CHIP DATA line's
horizontal junction carries one arrowhead into BUFFERS AND ARBITRATION
LOGIC and one into PAULA — **not** into AGNUS or DENISE. AGNUS is reached
by the separate CHIP ADR BUS (address) and by REG ADR BUS; DENISE has no
incoming line from CHIP DATA at all, only REG ADR BUS. `opus:1-blockdiag.c12`'s
own `evidence` field already discloses this same correction. This
restatement's "AGNUS... and DENISE" half over-reads the drawing; its
substance — a data path shared between the custom-chip side, Chip RAM, and
(back through BUFFERS AND ARBITRATION LOGIC) the CPU side — does not, and
is what this proposition grades.

**Proposition to test:** a single data path carries values in both
directions between the CPU-side bus (through BUFFERS AND ARBITRATION
LOGIC), Chip RAM, and the custom-chip side that directly reaches Chip RAM
(measured here as: Paula, the one custom chip the drawing's CHIP DATA
junction actually shows) — the same data storage answering the CPU side and
the custom-chip side through one shared path, corroborating
PROP-CHIPMEM-SHARED-P3/P4's "directly access" fact from the data-bus half
of the drawing rather than the address-bus half.

Status: testable, caution on the AGNUS/DENISE over-reach noted via `note()`
rather than silently followed.

## PROP-CUSTOMCHIPSECTION-ENCLOSURE

`opus:1-blockdiag.c9` — restatement: "A second dashed enclosure labelled
CUSTOM CHIP SECTION encloses AGNUS, PAULA, DENISE, the MUX, CHIP RAM and
the dashed PSEUDO FAST RAM box: the diagram groups these as one section
reached through the buffers and arbitration logic." (No `sol` counterpart
in this package's 49 — a singleton claim, graded on its own.)

**Proposition to test:** every one of this fixture's custom-chip-side
address decodes (the custom-register window where Paula/Agnus/Denise
answer, the Chip-memory window, and — per PROP-FOOTNOTE-1MB-AGNUS — the
pseudo-fast window under a 1MB-Agnus configuration) is reached only through
the single arbitration point PROP-BUFARB-BOUNDARY names, matching the
figure's drawing of one enclosure entered through one gate.

Status: testable.

---

## Claims graded only in part

**`sol:1-components.c29` / `opus:1-components.c29`** (PROP-CPU-RAM-CEILING-
9MB): under the suite's partial-grading rule ("a uid on a graded line
counts as ASSERTED, whatever the words around it say... if you can only
grade part of a claim, the measurement keeps its `expect()` under a label
carrying no uid, and the uid goes on a `record()` line alone"), the
`gary`-only sub-measurement below is graded under a bare label
with no uid, and the uid pair is kept alive on its own `record()` line
that states exactly what was graded and what was not (the composed
Zorro-II/`cpu_wrapper` half). Both claims keep a home; neither is silently
marked ASSERTED for a total this package's own boundary cannot produce.

---

## Two claims outside this package's own 49

Neither of the following carries `classify.package:
"ch1-memory-map-and-two-buses"` — both are `classify.status: "not-asserted"`
with no package assignment at all, so they are claims in no package's own
inventory. Both over-read Figure 1-1, which this package grades, and are
recorded here. Both are checked against the image; see "Figure 1-1
checked independently" above.

- **`opus:1-blockdiag.c14`** — restatement: "...pseudo-fast RAM is optional
  (dashed) and sits on the chip side rather than with the expansion Fast
  RAM." The image carries: the "PSEUDO" FAST RAM box, drawn in its own
  dashed outline, nested inside the dashed CUSTOM CHIP SECTION enclosure,
  below CHIP RAM, with a footnote asterisk. The image does **not** carry
  any key or legend assigning meaning to a dashed outline — the SYSTEM
  EXPANSION enclosure is dashed in the identical style while holding boxes
  (FAST RAM, OTHER EXPANSION DEVICES, HARD DISK INTERFACE, EXPANSION
  CONNECTOR(S)) nobody would call optional, and the drawing's one footnote
  says only "addressed as CHIP RAM with 1MB Agnus." "Optional" is the
  restatement's own addition, not the drawing's. The claim's own
  `classify.evidence` reaches the identical finding independently
  ("OVER-READ CONFIRMED against the image at 5x").
- **`sol:1-blockdiag.c22`** — restatement: "A multiplexer lies on the
  address path into Chip RAM and pseudo-fast RAM." The image carries: the
  MUX box's output leaves its bottom edge, turns right, and ends in exactly
  one arrowhead at CHIP RAM's left edge — read at ~3x zoom. The image
  does **not**
  carry any line, of any kind, into the "PSEUDO" FAST RAM box — it has zero
  incoming connections in the drawing. "...and pseudo-fast RAM" is the
  restatement's own addition; the multiplexer-into-Chip-RAM half is
  genuinely drawn and is what `PROP-CHIPADRBUS-MUX` above grades under its
  own, different uids. The claim's own `classify.evidence` reaches the
  identical finding independently ("OVER-READ CONFIRMED against the image
  at 5x").

## What this specification deliberately does not contain

Port names, signal names, parameter names, and concrete stimulus values
appear here only where a claim's own `evidence` field names them; the
`expect()` assertions themselves, the mutation guards, and the exact
fixture wiring are in `test_gary.cpp`. Every one of the 49 claims in this
package's `claims.jsonl` is accounted for above:

- **45 uids** sit on their own uid-bearing `expect()` label (24 text/
  guideline uids across 12 propositions, 21 figure uids across 12
  propositions).
- **2 uids** (`sol:1-components.c29` / `opus:1-components.c29`,
  PROP-CPU-RAM-CEILING-9MB) sit on a `record()` line alone, under the
  partial-grading rule,
  because only part of the proposition is gradeable at this boundary; a
  companion bare-label `expect()` (no uid) grades the `gary`-only part so
  the measurement itself is not lost either.
- **2 uids** (`sol:1-guidelines.c27` / `opus:1-guidelines.c27`,
  PROP-DMA-CHIP-ONLY) sit on a `record()` line alone, declined outright
  because `ch7-custom-address-space` already grades the identical
  proposition at this fixture and re-grading it here risks a second,
  possibly divergent verdict on the same sentence.

45 + 2 + 2 = 49. No claim is left without a line that carries its uid.
