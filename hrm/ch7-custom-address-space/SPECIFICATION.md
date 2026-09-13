# Specification — `ch7-custom-address-space`

Package: `hrm/ch7-custom-address-space`
Fixture: `gary`
Claims file: `hrm-testpipeline/work/ch7-build/ch7-custom-address-space.jsonl` (8 claims)
Subject: printed p.223's custom-chip address space — which addresses select
what, with and without the overlay.

This file is written from the `quote` / `restatement` fields of the 8 claims
alone. No RTL file for this package has been opened yet. It fixes the printed
propositions to be tested; concrete port names, stimulus values, and the
`expect()` assertions themselves are added only after this file is committed
and step 3 (opening `gary.v`) begins, per `BUILD-BRIEF.md`.

## Claim inventory (all 8, grouped by proposition)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:7-appA-legend.c1` | 263 | PROP-A, primary |
| 2 | `opus:7-appA-pre.c3`   | 263 | PROP-A, twin of #1 |
| 3 | `opus:7-appA-pre.c4`   | 263 | PROP-A, worked example (grouped) |
| 4 | `sol:7-appA-legend.c2` | 263 | PROP-B, primary |
| 5 | `opus:7-appA-pre.c5`   | 263 | PROP-B, twin of #4 |
| 6 | `opus:7-appA-pre.c8`   | 263 | PROP-C |
| 7 | `sol:7-appB-legend.c3` | 287 | PROP-D, primary |
| 8 | `opus:7-appB.c4`       | 287 | PROP-D, twin of #7 |

All 8 claims are accounted for; none is left unassigned at this stage.

## PROP-A — Chip-address-to-680x0-address formula

Page 263.

> 680x0 address = (chip address) + $DFF000
— `sol:7-appA-legend.c1`

> 680x0 address = (chip address) + $DFF000
— `opus:7-appA-pre.c3` (identical quote; twin of `sol:7-appA-legend.c1`. Both
crossreads passed. Grouped as one proposition, not double-counted.)

> for the 680x0 to write to ADKCON (address = $09E), the address would be
> $DFF09E.
— `opus:7-appA-pre.c4` (a worked instance of the same formula. Crossread
flagged this quote `excluded-non-behaviour` — a worked example rather than an
extracted behaviour — but classify overruled the exclusion: the example
asserts the same mapping the formula does, at a concrete, checkable address,
so it stays grouped on this proposition rather than being discarded. Nothing
in this specification rests on the example alone.)

**Proposition to test:** a chip address `X` in the custom-register bank is
reached by the 680x0 at address `X + $DFF000`; $09E is a concrete instance
of the formula ($09E → $DFF09E).

Status: testable.

## PROP-B — Exclusivity of the $DFF000-based mapping

Page 263.

> No other access address is valid.
— `sol:7-appA-legend.c2`

> No other access address is valid.
— `opus:7-appA-pre.c5` (twin of `sol:7-appA-legend.c2`)

**Wording note**, carried from both claims' own `split` field: the manual's
"valid" is advice to a programmer, not something a test can read off a port.
The proposition must be worded as an observable: **no address outside the
(chip address)+$DFF000 form selects the custom register bank.** This
substitution is specified by the claims themselves, not introduced here.

Status: testable.

## PROP-C — Custom-chip (DMA) access is Chip-memory-only

Page 263.

> The custom chips can only access Chip memory; using a non-Chip address
> will fail (See the AllocMem() documentation or your compiler manual for
> more information on Chip memory).
— `opus:7-appA-pre.c8` (quote as corrected by crossread; the corpus
substring alone was shortened and stopped at "will fail")

**Split**, per the claim's own note: the parenthetical "(See the AllocMem()
documentation...)" is a cross-reference to system software documentation,
not part of the behavioural claim, and is excluded from the assertion.

**Proposition to test:** an address generated for custom-chip (DMA) access
reaches Chip memory only; an address outside Chip memory does not succeed as
a custom-chip access.

Status: testable.

## PROP-D — Address-register-pair pointers must reference Chip memory (Chip-memory half only)

Page 287.

> + Address register pair. Must be an even address pointing to chip memory.
— `sol:7-appB-legend.c3`

> + Address register pair. Must be an even address pointing to chip memory.
— `opus:7-appB.c4` (twin of `sol:7-appB-legend.c3`; same split)

**Split**, carried from both claims: this sentence has two independent
halves, and they belong to two different fixtures. The EVEN-ADDRESS half (a
pointer register's bit 0 always reads/writes zero) is **out of scope for
this package** — `hrm/ch5-audio-location-pointer` (fixture `agnus_audiodma`)
already reached this legend and withdrew it: its `POINTER-LSB-ALWAYS-ZERO`
assertion records that the "...pointing to CHIP memory" half needs `gary.v`,
outside that module, and is "NOT COVERED BY THIS PACKAGE - needs gary.v";
and it credited no claim id for the EVEN-ADDRESS half because its predicate
covered one register (AUD0LCL) while the claim quantifies over every
pointer-type register. That other package's even-address finding does not
travel with this one, and this package must not re-assert it.

**Proposition to test (Chip-memory half only):** an address-register-pair
pointer must reference Chip memory.

Status: testable (Chip-memory half only).

## Scope note: PROP-C vs PROP-D

PROP-C and PROP-D both end up constraining "must be Chip memory," from two
different printed locations (Appendix A preamble, p.263, general
custom-chip/DMA access; Appendix B legend, p.287, specifically for
"+"-marked address-register-pair rows). They are kept as two separate
assertions with two separate page citations, each grounded in its own
quotation. Collapsing them into one assertion would under-cite the manual,
even where the same fixture measurement may end up answering both.

## Claims not assigned an assertion

None. All 8 claims are classified `testable` and `observable ... at gary`
in the claims file, and each is assigned to exactly one of the four
propositions above (PROP-A: claims 1, 3; PROP-A grouped: claim 4 [i.e. the
table row numbered 3]; PROP-B: claims 2, 5; PROP-C: claim 6; PROP-D: claims
7, 8 — see the inventory table for the exact uid-to-proposition mapping).

## What this specification deliberately does not contain

Port names, signal names, and concrete stimulus values are absent by
design. Per `BUILD-BRIEF.md`'s order of work, they are added only after this
file is committed and `gary.v` is read for step 3. PROP-A, PROP-B, PROP-C
and PROP-D above are the four propositions that become this package's
`expect()` assertions, each with its own control case and stimulus
mutation, once that step opens.
