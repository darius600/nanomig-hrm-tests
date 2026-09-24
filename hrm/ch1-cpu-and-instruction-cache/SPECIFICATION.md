# Specification — `ch1-cpu-and-instruction-cache`

Package: `hrm/ch1-cpu-and-instruction-cache`
Fixture (top module, per task assignment): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered on
`classify.package == "ch1-cpu-and-instruction-cache"` (15 claims)
Subject: printed pages 7, 8 and 13 — AUTOCONFIG expansion, the Fast-memory
side of the two-bus topology, and the instruction cache / CACR guideline.

This file is written from the `quote` / `restatement` fields of the 15 claims
alone, before `cpu_wrapper.v` or `cpu_cache_new.v` is opened for this
package's own purposes. Concrete port names, stimulus values, and the
`expect()` assertions themselves are added only after this file is committed.

**Disclosure on how clean that order actually is:** the classify stage's own
`classify.evidence` field on every one of these 15 claims already carries
`file:line` RTL citations (`cpu_wrapper.v:601`, `:608`, `:619`, `:634-642`,
`:540-599`, `cpu_cache_new.v:31-32`, `:46-48`, `:222`, `:227`, `:292`,
`:497`, and the CACR-export path `cpu_wrapper.v:96/257` /
`nanomig.v:312/422`) — this build's task sheet
repeats several of them directly. That information reached this agent before
this file was written; the "write the spec blind, then open the RTL" order is
therefore not literally intact, and this is reported rather than presented as
if it held. What this file does keep is the substantive discipline the order
protects: no port name, register field name, or concrete stimulus value below
was invented or copied from that evidence text — every proposition is stated
only in the words of the printed page and the claims' own restatements, and
the fixture built afterward is checked against the RTL directly rather than
against the prose of the `classify.evidence` field.

## Duplicate-grading check

Before writing propositions, `hrm/` was searched for existing accepted
assertions on AUTOCONFIG (`AUTOCONFIG|autoconfig|z2ram|z3ram`) and on the
cache/bus-topology vocabulary (`CACR|cpu_cache|instruction cache|Fast
memory|Chip memory`). No chapter-1 package for this subject exists yet in
`hrm/` (this package is the first CPU/cache one built). The only hits:

- `ch7-reset-overlay` (fixture `gary`) explicitly **declines** the AUTOCONFIG
  half of a page-223 claim as `absent`, because neither `gary.v` nor its
  `minimig`-fixture build carries an AUTOCONFIG port — that is a statement
  about the `gary`/`minimig` boundary, not about `cpu_wrapper`, and does not
  collide with anything asserted here.
- `ch7-custom-address-space` (fixture `gary`) explicitly **excludes**
  `cpu_wrapper.v`/`cpu_cache_new.v` from its own build, and its one quote
  naming "Chip memory" (the AllocMem() cross-reference, p.263) is itself
  excluded from that package's own assertion by its `split` field.
- Two Makefiles list `cpu_cache_new.v` only as an excluded/incidental source
  dependency, not as a tested boundary.

No existing accepted assertion grades any of this package's 15 propositions.
None of the 15 claims carries a `classify.note` pointing at an existing
package either. Nothing here is re-graded from a `record()` line citing
another package.

## Figure 1-1 checked independently

Two of the 15 claims (`sol:1-blockdiag.c8`, `opus:1-blockdiag.c8`) cite Figure
1-1 (`docs/references/amiga-hrm-3rd/ch1/figures/p8.png`), not text. The image
was opened and read directly for this package, independent of both readers'
restatements. What it shows: a dashed **SYSTEM EXPANSION** enclosure holding
four stacked boxes — **OTHER EXPANSION DEVICES**, **HARD DISK INTERFACE**,
**FAST RAM**, and **EXPANSION CONNECTOR(S)** at the bottom — joined by plain
lines; the only arrowheads on that stack are two thick arrows leaving
**EXPANSION CONNECTOR(S)** upward into the **CPU DATA** and **CPU ADDRESS**
buses on the CPU side of the diagram. There is no line of any kind from that
stack toward **BUFFERS AND ARBITRATION LOGIC**, the **MUX**, or the
**CUSTOM CHIP SECTION**. Both readers' restatements match this: neither reads
the dashed outline as "optional" and neither routes Fast RAM's connection
anywhere but the CPU buses — unlike the two over-reads named in the build
brief (`opus:1-blockdiag.c14`, `sol:1-blockdiag.c22`), which belong to a
different package (their uids are not in this package's 15) and are not
repeated here. This package's own figure claims are accepted as read.

## Claim inventory (all 15, grouped by proposition)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:1-components.c133`  | 7  | PROP-1, primary |
| 2 | `opus:1-components.c133` | 7  | PROP-1, twin of #1 (shortened quote, stops before the ™ sign) |
| 3 | `sol:1-components.c134`  | 7  | PROP-2, primary |
| 4 | `opus:1-components.c134` | 7  | PROP-2, twin of #3 |
| 5 | `sol:1-components.c135`  | 7  | PROP-3, primary |
| 6 | `opus:1-components.c135` | 7  | PROP-3, twin of #5 |
| 7 | `sol:1-components.c137`  | 7  | PROP-4, primary |
| 8 | `opus:1-components.c137` | 7  | PROP-4, twin of #7 |
| 9 | `sol:1-blockdiag.c2`     | 8  | PROP-5, primary |
| 10 | `opus:1-blockdiag.c2`   | 8  | PROP-5, twin of #9 |
| 11 | `sol:1-blockdiag.c8`    | 8  | PROP-6, primary (figure) |
| 12 | `opus:1-blockdiag.c8`   | 8  | PROP-6, twin of #11 (figure) |
| 13 | `sol:1-guidelines.c70`  | 13 | PROP-7, primary |
| 14 | `opus:1-guidelines.c70` | 13 | PROP-7, twin of #13 |
| 15 | `opus:1-guidelines.c75` | 13 | PROP-8, sole claim (no `sol` twin) |

All 15 claims are accounted for; none is left unassigned at this stage.

## PROP-1 — AUTOCONFIG is the device-recognition mechanism

Page 7.

> These devices are automatically recognized and used by system software
> through a well defined, well documented linking procedure called
> AUTOCONFIG™.
— `sol:1-components.c133`

> These devices are automatically recognized and used by system software
> through a well defined, well documented linking procedure called AUTOCONFIG
— `opus:1-components.c133` (quote stops immediately before the printed ™
sign; crossread flagged `quote_verbatim: no` for exactly that truncation,
`page_correct: yes`, `states_behaviour: yes` — the substance is unaffected
and both claims are classified `testable`. Twin of #1.)

**Proposition to test:** peripheral/expansion devices are automatically
recognized and made usable by system software through a defined linking
procedure (AUTOCONFIG) — as opposed to requiring the system to be told about
them by some other, non-automatic means.

Status: testable.

## PROP-2 — AUTOCONFIG dynamically allocates address space at boot

Page 7.

> AUTOCONFIG is short for automatic configuration and is the process which
> allows memory or I/O space for an expansion board to be dynamically
> allocated by the system at boot time.
— `sol:1-components.c134`, `opus:1-components.c134` (identical quote; twin)

**Proposition to test:** the memory or I/O address space an expansion board
occupies is assigned dynamically (i.e., determined at boot/configuration
time by the system, not fixed in the board's own design).

Status: testable.

## PROP-3 — No DIP switches needed to select expansion address space

Page 7.

> Unlike some other systems, there is no need to set DIP switches to select
> an address space from a fixed range reserved for expansion devices.
— `sol:1-components.c135`, `opus:1-components.c135` (identical quote; twin)

**Wording note:** "no need to set DIP switches" is stated as a user-facing
convenience; the observable substitute is structural — the board's address
comes entirely from the dynamic AUTOCONFIG assignment (PROP-2), not from any
fixed/strapped configuration input standing in for a DIP switch. This
substitution is the natural reading of the sentence in context (it follows
directly from the AUTOCONFIG paragraph, "unlike some other systems"), not an
invented requirement.

Status: testable.

## PROP-4 — Expansion peripheral examples (A500/A1000)

Page 7.

> Peripherals that can be added include hard disk controllers and drives, or
> additional external RAM.
— `sol:1-components.c137`, `opus:1-components.c137` (identical quote; twin)

**Proposition to test:** the named example peripherals — hard disk
controller, and external RAM — are addressable through the same expansion
mechanism as any AUTOCONFIG device. ("Drives" themselves are a mechanical
peripheral with no register-level RTL counterpart to test and are not
claimed here beyond the controller.)

**Scope flag carried into the RTL step:** whichever of these examples turns
out to live outside this package's own top-module boundary (`cpu_wrapper`)
is a candidate for a `record()` rather than an `expect()` — this file does
not yet know which, by design (no RTL opened).

Status: testable.

## PROP-5 — Two separate buses: CPU-only Fast memory, shared Chip memory

Page 8 (body text under Figure 1-1's heading).

> Notice that there are two separate buses, one that only the CPU can access
> (Fast memory) and another one that the custom chips share with the CPU
> (Chip memory).
— `sol:1-blockdiag.c2`, `opus:1-blockdiag.c2` (identical quote; twin)

**Proposition to test:** an access into the Fast-memory bus is reachable by
the CPU and does not reach the custom-chip-shared (Chip-memory) side; the two
are structurally separate at whatever boundary this package's fixture can
observe.

Status: testable.

## PROP-6 — Figure 1-1: Fast RAM hangs off the CPU-only side

Page 8, Figure 1-1 (image evidence — see "Figure 1-1 checked independently"
above).

> [FIGURE 1-1 — figures/p8.png — not text; open the image and read it]
— `sol:1-blockdiag.c8`, `opus:1-blockdiag.c8` (marker line, not a quotation;
twin claims, independently confirmed against the image itself, not against
either restatement — see the section above)

**Proposition to test:** Fast RAM, drawn inside the dashed SYSTEM EXPANSION
enclosure and reached only via the CPU DATA/CPU ADDRESS buses in the figure,
is on the CPU-only (Fast-memory) side of the architecture, not on the
custom-chip (Chip-memory) side — the same structural split PROP-5's text
states, evidenced here by the figure instead of by the paragraph.

Status: testable.

## PROP-7 — Instruction caches store recent instructions, do not monitor writes

Page 13 (General Amiga Development Guidelines; the sentence's own lead-in,
same paragraph: "The 68020, 68030, and 68040 processors all have instruction
caches.").

> These caches store recently used instructions, but do not monitor writes.
— `sol:1-guidelines.c70`, `opus:1-guidelines.c70` (identical quote modulo
the scan's own line break; twin)

**Proposition to test:** an instruction cache in this design retains
recently-fetched instruction values (a repeated fetch of the same
instruction address can be answered from the cache) and does not observe /
react to writes made through it under ordinary operation (i.e., writing new
data does not, by itself, invalidate or update a cached instruction line the
way an explicit cache-clear does) — the two independent halves this sentence
asserts. Whichever half or halves the fixture can actually observe is
determined only once the RTL is open; a half that cannot be observed at this
package's boundary is a candidate for `record()`, not silent omission.

Status: testable.

## PROP-8 — CACR bit $0008 clears the instruction cache

Page 13, the code listing following the guideline paragraph.

> CACRF_ClearI  EQU     $0008 ;Bit for clear instruction cache
— `opus:1-guidelines.c75` (sole claim; no `sol` twin exists for this line —
verified against the claims file, not assumed)

**Proposition to test:** the CACR bit at position 3 (value $0008) is the bit
that, when set, clears/invalidates the instruction cache — the same cache
PROP-7 describes.

Status: testable.

## Claims not assigned an assertion

None. All 15 claims are classified `testable` in the claims file and each is
assigned to exactly one of the eight propositions above (see the inventory
table for the exact uid-to-proposition mapping). Whether every proposition
ultimately reaches an `expect()` line, or instead a `record()` decline, is
decided once `cpu_wrapper.v` (and, for PROP-7/PROP-8, `cpu_cache_new.v`) is
read — not here.

## What this specification deliberately does not contain

Port names, signal names, register field names, and concrete stimulus values
are absent by design (with the one disclosed exception in the "Disclosure"
paragraph above: this agent already possessed several `file:line` citations
before writing this file, but did not transcribe any port/signal name from
them into the propositions above). PROP-1 through PROP-8 are the eight
propositions that become this package's `expect()` assertions (or reasoned
`record()` declines), each with its own control case and stimulus mutation,
once `cpu_wrapper.v` is opened next.
