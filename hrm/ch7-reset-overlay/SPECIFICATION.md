# Specification — `ch7-reset-overlay`

Package: `hrm/ch7-reset-overlay`
Fixture: `gary`
Claims file: `hrm-testpipeline/work/ch7-build/ch7-reset-overlay-spec-stage.jsonl` (9 claims)
Subject: printed p.223 — what the overlay does to the address map after
reset.

This file is written from the `quote` / `restatement` fields of the 9 claims
alone. No RTL file for this package has been opened yet. It fixes the
printed propositions to be tested; concrete port names, stimulus values, and
the `expect()` assertions themselves are added only after this file is
committed and step 3 (opening the RTL) begins, per `BUILD-BRIEF.md`.

## Claim inventory (all 9, grouped by proposition)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:7-reset.c1`   | 223 | PROP-A, primary |
| 2 | `opus:7-dma.c23`   | 223 | PROP-A, twin of #1 |
| 3 | `sol:7-reset.c4`   | 223 | PROP-B, primary |
| 4 | `opus:7-reset.c1`  | 223 | PROP-B, twin of #3 (ROM-half only) |
| 5 | `sol:7-reset.c5`   | 223 | PROP-C, primary |
| 6 | `opus:7-reset.c2`  | 223 | PROP-C, twin of #5 |
| 7 | `sol:7-reset.c13`  | 223 | PROP-D, primary |
| 8 | `sol:7-reset.c14`  | 223 | PROP-E, primary |
| 9 | `opus:7-reset.c10` | 223 | PROP-E, twin of #8 (ROM-half only) |

All 9 claims are accounted for; none is left unassigned at this stage.

## PROP-A — Chip-memory access is DMA-controlled (positive half only)

Page 223.

> The Amiga chips access Chip memory directly via DMA, rather than utilizing
> traditional bus arbitration mechanisms.
— `sol:7-reset.c1`

> The Amiga chips access Chip memory directly via DMA, rather than utilizing
> traditional bus arbitration mechanisms.
— `opus:7-dma.c23` (identical quote; twin of `sol:7-reset.c1`. Both
crossreads passed. Grouped as one proposition, not double-counted.)

**Split**, carried from both claims' own `split` field: the negative half —
"rather than traditional bus arbitration" — is **not asserted**. Both claims
record that a grep over `nanomig/src` for `_BGACK|bgack|bus_grant|_BR\b`
returns nothing: `gary`, `agnus`, `denise`, `paula` and `minimig` all lack
any bus-request/grant/acknowledge port. That half is deferred to a
`record`, not an `expect`, with reason `absent` — there is no `_BG`/`_BGACK`
-class port anywhere in this RTL tree to observe, so nothing can be wired
to test its absence either.

**Proposition to test (positive half only):** access to Chip memory in this
fixture's domain is DMA-controlled, not mediated by a bus-arbitration
handshake.

Status: testable (positive half only).

## PROP-B — ROM answers at $00000000 during the post-reset overlay

Page 223.

> An additional copy of the system ROM responds starting at
> memory location $00000000.
— `sol:7-reset.c4`

> When the Amiga is turned on or externally reset, the memory map is in a
> special state.  An additional copy of the system ROM responds starting at
> memory location $00000000.
— `opus:7-reset.c1` (bundled quote; only its "$00000000 answers from ROM"
half is classified here, at `gary`. Its other half — "the memory map is in
a special state," attributed to "turned on or externally reset" — is
classified under `sol:7-reset.c3`, at fixture `minimig`. That claim is not
in this package's claims file and is out of scope for `ch7-reset-overlay`.)

**Proposition to test:** with the reset overlay active, an access to address
$00000000 is answered by an image of the system ROM.

Status: testable.

## PROP-C — RAM at $00000000 is unavailable during the post-reset overlay

Page 223.

> The system RAM that would normally be located
> at this address is not available.
— `sol:7-reset.c5`

> The system RAM that would normally be located
> at this address is not available.
— `opus:7-reset.c2` (identical quote; twin of `sol:7-reset.c5`. Both
crossreads passed.)

**Proposition to test:** with the reset overlay active, the normal system
RAM at address $00000000 does not answer.

Status: testable.

## PROP-D — Executing RESET removes memory from the map (AUTOCONFIG half excluded)

Page 223.

> All memory and AUTOCONFIG(TM) cards disappear
— `sol:7-reset.c13` (partial sentence; the same sentence continues at
PROP-E's quote, `sol:7-reset.c14`.)

**Split**, carried from the claim's own `split` field: the AUTOCONFIG half
is **not asserted**. A grep over `nanomig/src` finds `AUTOCONFIG` only in
`minimig-aga/cpu_wrapper.v`, which the `minimig` fixture does not
instantiate — the `minimig` fixture Verilates and links cleanly without it,
a measurement, not an assumption. That half is deferred to a `record`, not
an `expect`, with reason `absent` — no AUTOCONFIG configuration/shut-up
port exists to observe.

**Note:** `opus:7-reset.c10`'s bundled quote (see PROP-E below) restates
this same "all memory and AUTOCONFIG cards disappear" half, but that
record's own `split` field attributes this half to `sol:7-reset.c13` rather
than classifying it independently. It is therefore not cited a second time
on this proposition, to avoid crediting one record on two assertions.

**Proposition to test (memory half only):** executing the 68000 RESET
instruction removes memory from the active address map (the
AUTOCONFIG-card portion of that removal excluded, per above).

Status: testable (memory half only).

## PROP-E — Executing RESET makes the ROM image reappear at $00000000

Page 223.

> the ROM image appears
> at location $00000000.
— `sol:7-reset.c14` (continues the sentence begun at PROP-D's quote,
`sol:7-reset.c13`.)

> All memory and AUTOCONFIG(TM) cards disappear, and the ROM image appears
> at location $00000000.
— `opus:7-reset.c10` (bundled quote spanning both PROP-D's and PROP-E's
halves; only its "$00000000 shows the ROM image" half is classified here,
twin of `sol:7-reset.c14`. Its "all memory and AUTOCONFIG cards disappear"
half is classified under `sol:7-reset.c13` (PROP-D) instead, per this
record's own `split` field, and is not separately cited here.)

**Proposition to test:** executing the 68000 RESET instruction makes the
ROM image reappear at address $00000000.

Status: testable.

## Scope note: two triggers, four propositions

The chapter states the reset overlay under two different triggering
conditions on the same printed page: power-on or external reset (PROP-B,
PROP-C) and execution of the 68000 RESET instruction (PROP-D, PROP-E).
PROP-B and PROP-E both assert "ROM answers at $00000000"; PROP-C and PROP-D
both assert "the memory that would otherwise be there does not." Each pair
nonetheless rests on its own quotation and its own printed sentence, so they
are kept as four separate assertions rather than collapsed into two, even
where the same fixture measurement may end up settling more than one of
them. Collapsing them would under-cite the manual for whichever trigger's
sentence got dropped.

## Halves excluded from assertion, summarized

Two sub-clauses — not separate claims, but clauses within claims already
listed above — are carried through this specification without becoming
`expect()` assertions:

1. The bus-arbitration negative half of PROP-A (`sol:7-reset.c1`,
   `opus:7-dma.c23`): no bus-request/grant/acknowledge port exists anywhere
   in `nanomig/src` to observe. Reason: `absent`.
2. The AUTOCONFIG half of PROP-D (`sol:7-reset.c13`, and the corresponding
   half of `opus:7-reset.c10`'s bundled quote): no AUTOCONFIG
   configuration/shut-up port exists in a module the `gary`/`minimig`
   fixtures need to instantiate. Reason: `absent`.

Both will be handled with a `record`, carrying this reason, at build time —
never asserted, and never silently dropped.

## Claims not assigned an assertion

None. All 9 claims are classified `testable` at `gary` in the claims file,
and each is assigned to exactly one of the five propositions above (see the
inventory table for the exact uid-to-proposition mapping). Two sub-clauses
within those same claims are excluded from assertion for the `absent`-signal
reasons given above; that exclusion is not an unassigned claim, since the
claim itself is still asserted through its other half.

## What this specification deliberately does not contain

Port names, signal names, and concrete stimulus values are absent by
design. Per `BUILD-BRIEF.md`'s order of work, they are added only after
this file is committed and the RTL is read for step 3. PROP-A, PROP-B,
PROP-C, PROP-D and PROP-E above are the five propositions that become this
package's `expect()` assertions, each with its own control case and
stimulus mutation, once that step opens.
