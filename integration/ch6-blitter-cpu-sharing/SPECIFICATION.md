# ch6-blitter-cpu-sharing

Claims from Chapter 6 of the Amiga Hardware Reference Manual, 3rd edition,
pages 169-198: the blitter as an asynchronous coprocessor, blitter pointer
registers, the blitter-done-flag read quirk, and how the blitter and the
68000 share the chip memory bus (DMA priority, the three-consecutive-cycle
share rule, and bitplane-DMA bandwidth pressure).

Every proposition below is tested only through `nanomig.v`'s ports, via
`integration/harness/nanomig_harness.h`: the RAM-port access log
(`ram_log()`), `peek_logical()`, and `run_until_write()`/`boot()`. A 68000
program is the stimulus; the C++ test grades what the ports showed.

## Shared setup

- `RESULTS` ($0C0000) and `DONE` are as `integration/harness/nanomig.i`
  defines them. Programs also store to a dedicated scratch chip-RAM word
  (its own address, distinct from any blit source/destination address used
  in the same program) on every pass of a CPU polling/counting loop; its
  writes are how the CPU's own progress is told apart from the blitter's
  or the display's in `ram_log()`, since `ram_log()` records an access by
  address/data/cycle only, not by which master issued it.
- Custom-register offsets beyond the subset `nanomig.i` already defines
  (`CUSTOM`, `DMACONR`/`DMACON`, the `BLT*` blitter registers) are defined
  locally in this package's own `.s` sources, from the manual's own
  Appendix B (Register Summary, Address Order) and Chapter 3 (bitplane
  display setup) -- not from the RTL:
  - `DMACON`/`DMACONR` bit 14 `BBUSY` (blitter busy, read), bit 10
    `BLTPRI` (blitter priority: 1 = "nasty", never yields; 0 = "share"),
    bit 9 `DMAEN` (master DMA enable), bit 8 `BPLEN` (bitplane DMA
    enable), bit 6 `BLTEN` (blitter DMA enable); bit 15 is the
    `DMACON`/write SET/CLR control bit.
  - `BPLCON0` ($DFF100): bit 15 `HIRES`, bits 14-12 the bitplane count.
  - `DIWSTRT`/`DIWSTOP` ($DFF08E/$DFF090), `DDFSTRT`/`DDFSTOP`
    ($DFF092/$DFF094): the standard low-resolution display window/fetch
    values, and the standard high-resolution `DDFSTRT`/`DDFSTOP` pair
    (narrower, since a high-resolution line fetches twice as often).
  - `BPL1PTH`/`BPL1PTL` ($DFF0E0/$DFF0E2) and the following bitplane
    pointer pairs, each 4 bytes apart.
- Every assertion's `expect()` label starts with its own short id (`A1`,
  `A2`, ...), then `[p<page>...]`, then every claim id it establishes in
  full, comma-separated, then a colon and the proposition text. A
  `MUTATION` comment's second token is that leading id.
- Both CPU cores run every program; a verdict is reported for each, and a
  disagreement between them is reported as such, not resolved.

## Fixture check (no claim id)

`prog_fixture.s`: stores a literal constant to `RESULTS`, then performs
one minimal channel-A-to-D word blit (one word, no shift, straight
pass-through) copying a known word from one chip-RAM location to another,
then `DONE`. The test checks, before trusting any other program's
`RESULTS` or blit output: the constant reads back as written, and the
blit's destination reads back as the known source word. Printed as
`[not graded -- apparatus check, no claim uid]`, consistent with
`apparatus-vasm-program`.

## Assertions

### A1 [p170] pointer register: adjacent words, one 32-bit write

Claims: `6-dma-channels.c16`, `6-dma-channels.c18`, `ch6-dma.c14`,
`ch6-dma.c16`.

Proposition: `BLTAPTH`/`BLTAPTL` (the high/low words of the blitter's A
pointer, $DFF050/$DFF052) are adjacent in the 68000 address space, and a
single 32-bit `MOVE.L` to `BLTAPTH` sets both, correctly pointing the
blitter at the intended source word.

Program (`prog_pointer.s`): stores a known word pattern at a chosen
source address `SRC` in chip RAM, executes one `MOVE.L #SRC,BLTAPTH(a0)`,
configures a minimal A-to-D one-word copy blit, runs it, `DONE`.

Observed: the first blit-attributable read in `ram_log()` after the
`MOVE.L`, and `peek_logical()` of the blit's destination word.

Expected value: the read address equals `SRC` and the destination word
equals the pattern stored there -- a wrong high or low half from a
failed/partial write would read the wrong bank or the wrong offset.

Mutation (`prog_pointer_mutant.s`): `SRC` points at a different chip-RAM
address holding a different known pattern; the original check (still
comparing against the first pattern) must go red.

### A2 [p169,p186] the CPU keeps running while the blitter works

Claims: `6-intro.c20`, `ch6-intro.c21`, `ch6-intro.c22`,
`6-blitter-done-flag.c2`, `6-blitter-done-flag.c3`, `ch6-done.c2`,
`ch6-done.c3`.

Proposition: the blitter is an asynchronous coprocessor; the 68000 is not
halted or blocked while a blit is in progress, and the two operate
concurrently.

Program (`prog_sharing.s`, `SHARE` build): a tight CPU loop increments a
counter and stores it to the scratch word on every pass, concurrently
with a large, continuously-demanding blit (`BLTPRI`=0) sized to run for
many loop iterations.

Observed: the count of scratch-word writes in `ram_log()` whose cycle
falls between the blit's first and last destination writes (its active
window, both addresses distinct from the scratch address).

Expected value: several distinct scratch-word writes fall inside the
active window, not just before or after it -- the CPU executed multiple
instructions while the blit was still running.

Mutation (`prog_sharing_tiny.s`): the blit shrinks to a near-instant
one-word transfer; the same check (still expecting several writes inside
the now much shorter window) must go red.

### A3 [p187] a small blit can finish before the CPU's first check

Claim: `ch6-multi.c3`.

Proposition: a 68000 may be slow enough, relative to a small blit, that
the blit has already finished the first time the CPU checks.

Program (`prog_earlycheck.s`, `SMALL` build): starts a one-word blit,
performs the manual's prescribed one dummy chip-memory read, immediately
reads `DMACONR` and stores it, `DONE`.

Observed: the stored `DMACONR`'s `BBUSY` bit, and whether the blit's
destination write already appears in `ram_log()` before the `DMACONR`
read's cycle.

Expected value: `BBUSY` reads clear, and the blit's destination write
precedes the `DMACONR` read in `ram_log()`.

Mutation (`prog_earlycheck.s`, `LARGE` build): the blit is sized to take
many more cycles; the same check (still expecting `BBUSY` clear on the
first read) must go red.

### A4 [p194,p198] the blitter has higher DMA priority than the 68000

Claims: `6-system-dma.c13`, `ch6-sysdma.c11`, `6-blitter-priority.c1`,
`ch6-sysdma.c52`.

Proposition: with `BLTPRI` set ("nasty"), the blitter never yields a
contested cycle -- its fixed higher priority holds for the whole blit.

Program (`prog_sharing.s`, `NASTY` build): the same CPU scratch-counter
loop as A2, `BLTPRI`=1, a large continuously-demanding blit.

Observed (precondition): the blit's active window spans enough cycles to
have contained several CPU-loop iterations had the CPU been let through
-- guards against a vacuously short window.

Observed (claim): the count of scratch-word writes inside the active
window.

Expected value: precondition holds, and the count is exactly zero -- the
CPU made no chip-bus progress at all while the nasty-mode blit ran.

Mutation: swap to `prog_sharing.s` (`SHARE` build); the same "count is
zero" check must go red (share mode lets some through, per A5).

### A5 [p198] share mode: the blitter yields after three consecutive denied CPU cycles

Claims: `6-blitter-priority.c13`, `ch6-sysdma.c53`, `ch6-sysdma.c62`.

Proposition: with `BLTPRI` clear ("share"), once the CPU has been denied
its requested cycle three times running, the blitter releases the bus for
one cycle and the CPU's fourth attempt succeeds.

Program: `prog_sharing.s` (`SHARE` build, same capture as A2/A4's
counterpart).

Observed (precondition): within the blit's active window, at least one
run of exactly three consecutive denied CPU-slot opportunities occurs
(the one-slot period is calibrated from the blit's own back-to-back
internal fetch cadence measured earlier in the same, otherwise
uncontended, part of the run).

Observed (claim): the CPU's next opportunity after such a run succeeds,
landing at the fourth slot and not later.

Expected value: the precondition holds (three-in-a-row denial actually
happens) and the fourth slot succeeds every time it does.

Mutation: swap to `prog_sharing.s` (`NASTY` build); the same "the fourth
slot succeeds" check must go red -- nasty mode never yields, so no
recovery follows a run of three denials.

### A6 [p196] alternate cycles are enough for full speed in the normal case

Claims: `6-system-dma.c53`, `ch6-sysdma.c28`.

Proposition: giving the CPU only every other available cycle still lets
it run at full speed once nothing abnormal is competing for the bus.

Program: `prog_sharing.s`, `BASELINE` build (CPU scratch-counter loop
alone, no blit) and `SHARE` build.

Observed: scratch-counter loop iterations completed in a fixed cycle
budget, `BASELINE` vs. `SHARE`.

Expected value: `SHARE`'s iteration count equals `BASELINE`'s within a
small tolerance -- the CPU is not measurably slowed by a background,
share-mode blit.

Mutation: compare `BASELINE` against `NASTY` instead; the same "equal
within tolerance" check must go red (nasty mode measurably slows the CPU,
per A4).

### A8 [p196] a denied cycle means the CPU simply waits for its next available slot

Claims: `6-system-dma.c54`, `6-system-dma.c55`, `ch6-sysdma.c30`.

Proposition: a cycle the CPU is denied is not lost or retried some other
way -- the CPU's next access lands on its very next available slot.

Program: reuses A5's `SHARE`-build capture.

Observed: for each denied-then-recovered sequence found for A5, whether
the recovered access's cycle equals exactly the next slot-period boundary
after the third denial.

Expected value: the recovered access aligns to the immediately next slot
boundary, every time.

Mutation: swap to the `NASTY` build; the same "aligns to the next
boundary" check must go red -- there is no recovery event to find at all,
counted as failing.

### A9 [p196] four or fewer low-resolution bitplanes still let the CPU through

Claims: `6-system-dma.c61`, `ch6-sysdma.c34`, `ch6-sysdma.c35`.

Proposition: display DMA at four (or fewer) low-resolution bitplanes
leaves the CPU able to get alternate memory cycles when it is ready and
highest priority.

Program (`prog_bitplane.s`, `PLANES=4 HIRES=0` build): sets `BPLCON0` for
four low-resolution bitplanes with the standard low-resolution
`DIWSTRT`/`DIWSTOP`/`DDFSTRT`/`DDFSTOP`, lets display DMA reach its active
window, runs the CPU scratch-counter loop throughout.

Observed (precondition): bitplane-pointer-attributable reads appear in
`ram_log()` during the window (display DMA is actually active).

Observed (claim): the count of scratch-word writes during the same
window.

Expected value: precondition holds and the count is at least one.

Mutation: swap to the `PLANES=4 HIRES=1` build (A10); the same "at least
one" check must go red (zero, per A10).

### A10 [p196,p197] four high-resolution bitplanes lock the CPU out entirely

Claims: `6-system-dma.c71`, `ch6-sysdma.c42`.

Proposition: at four high-resolution bitplanes (double the low-res fetch
bandwidth), the CPU gets no chip-bus access at all during the active
display window.

Program (`prog_bitplane.s`, `PLANES=4 HIRES=1` build): as A9, `BPLCON0`'s
`HIRES` bit set, the high-resolution `DDFSTRT`/`DDFSTOP` window.

Observed (precondition): bitplane-pointer-attributable reads appear in
`ram_log()` during the window.

Observed (claim): the count of scratch-word writes during the window.

Expected value: precondition holds and the count is exactly zero.

Mutation: swap to the `PLANES=4 HIRES=0` build (A9); the same "count is
zero" check must go red (at least one, per A9).

### A11 [p196] more than four low-resolution bitplanes measurably steal cycles

Claims: `6-system-dma.c62`, `ch6-sysdma.c36`.

Proposition: raising low-resolution bitplane count above four steals
cycles from the CPU that it had at four or fewer.

Program: `prog_bitplane.s`, `PLANES=6 HIRES=0` build, vs. the `PLANES=4
HIRES=0` build (A9).

Observed: scratch-counter loop iterations completed during a
fixed-length display window, `PLANES=6` vs. `PLANES=4`.

Expected value: `PLANES=6` completes measurably fewer iterations than
`PLANES=4`.

Mutation: compare the `PLANES=4` build against itself (two runs of the
same binary); the same "measurably fewer" check must go red (no
difference between identical configurations).

## Claims not turned into assertions

| claim id(s) | page | why |
|---|---|---|
| `6-intro.c5`, `ch6-intro.c5` | 169 | "approximately twice as fast" is a comparative, approximate performance claim with no numeric tolerance given on the page. A fair test needs a calibrated CPU-only copy-loop baseline compared against the blitter's transfer under matching conditions; a dedicated throughput-comparison fixture with an explicitly chosen tolerance would settle it. |
| `ch6-desc.c12` | 182 | Compares the blitter's descending-pointer setup convention against a hypothetical 680x0 `-(An)` predecrement outcome; `nanomig.v`'s ports expose no such reference value, and settling it needs a blit data-correctness check (not a DMA-arbitration one) with the pointer set both ways. A dedicated blitter-descending-addressing correctness fixture would settle it. |
| `6-blitter-done-flag.c9`, `6-blitter-done-flag.c10`, `ch6-done.c8`, `ch6-done.c10`, `ch6-done.c11`, `ch6-done.c12` | 186 | The "first busy-bit test may be wrong, second is correct" quirk, and the display-fetch-lockout timing race that causes it, describe a real-silicon timing/metastability hazard. A Verilator simulation of synthesizable RTL is fully deterministic; there is no signal at `nanomig.v`'s ports that is "sometimes wrong on the first read" in a reproducible, provokable way. |
| `6-blitter-done-flag.c8`, `ch6-done.c9` | 186 | Describes the CPU continuing to execute from Fast memory or its internal cache while the blitter is denied chip memory. `nanomig_harness.h`'s `Program`/`load_program()` only places and boots code in identity-mapped chip RAM; it has no supported way to place and run a program from the `fastram_*` port or to exercise the CPU cache path -- a harness capability gap, not worked around. |
| `ch6-multi.c4` | 187 | Describes 68020 cache-ahead-of-DMA behaviour. `Config.cpu_config` only selects "68000 core behaviour" and both builds instantiate fx68k or TG68K, both 68000-class cores; no 68020 exists anywhere in this design. |
| `6-system-dma.c12`, `ch6-sysdma.c10` | 194 | "The two lowest priority levels" ranks the blitter and CPU against every other DMA channel (audio, disk, sprites, copper, bitplane), not just against each other. Confirming it needs all of those channels driven simultaneously against blitter and CPU, a materially larger fixture than this package's scope. The narrower fact -- the blitter outranks the CPU specifically -- is A4. A fixture exercising every DMA channel concurrently against blitter and CPU would settle the full claim. |
| `6-system-dma.c50`, `ch6-sysdma.c25` | 196 | "Only even-numbered memory access cycles" names a parity in the design's own internal slot numbering that `nanomig.v`'s ports don't expose (only raw `clk_sys` cycle counts are visible, not a labelled slot index). Inferring "evenness" purely from CPU access spacing would conflate the architectural slot grid with the contention-driven throttling A4/A5/A6 already test more directly; A6 covers the practical consequence (full speed in the uncontended case) as far as this boundary lets it be tested. |
| `6-system-dma.c51`, `6-system-dma.c52`, `ch6-sysdma.c26`, `ch6-sysdma.c27` | 196 | "About half" of instruction time is an average over the CPU's full instruction mix, with no numeric tolerance given, and is core-microarchitecture-dependent; fx68k and TG68K are not expected to share identical per-instruction internal/external cycle splits, and this package's assertions must carry one verdict per core. An instruction-timing histogram compiled per core would settle it, and could legitimately disagree between cores. |
| `6-system-dma.c56`, `6-system-dma.c57`, `ch6-sysdma.c32`, `ch6-sysdma.c33` | 196 | The manual's point about TAS is prescriptive ("should never be used"): an indivisible read-modify-write cycle not fitting a DMA slot. Provoking the actual hazard on chip RAM risks an indefinite bus stall with no clean pass/fail short of a timeout, and `nanomig.v`'s ports give no way to observe "did not fit a slot" as a value distinct from "took longer". |
| `6-system-dma.c58`, `6-system-dma.c60` | 196 | Figure 6-10 illustrates the same "about half" statistical split and the same slot-parity fact withdrawn above (halfsplit, evenslot); the figure adds no independently observable value at `nanomig.v`'s ports beyond what those two already do or don't give. |
| `ch6-sysdma.c29`, `ch6-sysdma.c31` | 196 | Which specific instructions misalign with the even-cycle allocation, and that most don't, are both statistical/instruction-set-coverage claims needing a representative instruction sample, and, like the half-split claims, depend on per-core instruction timing the two cores aren't expected to match. |
| `6-system-dma.c65`, `ch6-sysdma.c38` | 196 | The precise "50 percent" figure needs a full-frame, cycle-exact accounting of every CPU-available slot at four vs. six bitplanes. A11 tests the same underlying fact directionally (six planes measurably worse than four) without committing to the exact percentage, which a coarse loop-counting measurement can't responsibly pin down. A cycle-exact per-frame accounting fixture would settle the percentage. |

Total: 31 claim ids across 11 assertions plus the unlabelled fixture check;
30 claim ids withdrawn above. 61 claim ids in the spec-stage file.
