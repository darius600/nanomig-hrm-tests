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
  address/data/cycle only, not by which master issued it. A CPU
  instruction fetch is identified the same way: every program in this
  package is well under 512 bytes and `PROGRAM_ORG` ($080400, `nanomig.i`)
  is fixed, so a read in $080400-$0805FF is a program fetch, never a DMA
  access.
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
- One colour clock (one memory slot) is 8 `clk_sys` cycles
  (`amiga_clk.v`: `cck` toggles once every 4 `clk_sys`).
- Every assertion's `expect()` label starts with its own short id (`A1`,
  `A2`, ...), then `[p<page>...]` where it carries claim ids (an id-less
  label states its page inline), then every claim id it establishes in
  full, comma-separated, then a colon and the proposition text.
- **Mutations are separate runs, selected by `NMH_MUTATE=<id>`** (an
  environment variable the test reads, `std::getenv`), one value per
  assertion (see the test file's header comment for the full list and
  what each one substitutes). A plain `make test` sets no such variable,
  so the normal run prints only the fixture, the preconditions and the
  assertions -- no separate "-mutation" line; under a mutation, the same
  printed label's PASS becomes FAIL (or, for the two assertions that are
  already red at baseline, A8 and PARITY, the mutation instead shows the
  same comparator read GREEN on a different, proven-live capture, per the
  brief's own affordance for an assertion that is already red).
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

Mutation (`prog_fixture_mut.s`, `NMH_MUTATE=FIXTURE`): stores $DEAD
instead of $BEEF; the unchanged check must go red.

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

Observed: `peek_logical()` of the blit's destination word.

Expected value: the destination word equals the pattern stored at `SRC`.

Mutation (`prog_pointer_mutant.s`, `NMH_MUTATE=A1`): `SRC` points at a
different chip-RAM address holding a different known pattern; the
unchanged check (still comparing against the first pattern) must go red.

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
active window, not just before or after it.

Mutation (`prog_sharing_tiny.s`, `SHARE` and `TINY` both set,
`NMH_MUTATE=A2`): the blit shrinks to a near-instant one-word transfer;
the unchanged check (still expecting several writes inside the now much
shorter window) must go red.

### A3 [p187] a small blit can finish before the CPU's first check

Claim: `ch6-multi.c3`.

Proposition: a 68000 may be slow enough, relative to a small blit, that
the blit has already finished the first time the CPU checks.

Program (`prog_earlycheck.s`, `SMALL` build): starts a one-word blit,
performs the manual's prescribed one dummy chip-memory read, immediately
reads `DMACONR` and stores it, `DONE`.

Observed (precondition, its own id, "A3PRE"): the blit's destination
write's cycle in `ram_log()`, against the dummy read's cycle.

Expected value (precondition): the destination write precedes the dummy
read -- the blit really did finish before the CPU's own procedural step,
not just before the later `DMACONR` read.

Observed (claim): the stored `DMACONR`'s `BBUSY` bit.

Expected value: `BBUSY` reads clear.

Mutation (`prog_earlycheck_large.s`, `EARLY_LARGE` set,
`NMH_MUTATE=A3`): the blit is sized to take many more cycles; the
unchanged check (still expecting `BBUSY` clear on the first read) must go
red.

### BBUSY-PREDICTS [p186] BBUSY set correctly predicts more writes still to come

Claims: `6-blitter-done-flag.c10`, `ch6-done.c10`, `ch6-done.c12`.

Proposition: after the manual's prescribed intervening chip-memory read
(p.186's NOTE), a `BBUSY` read that comes back *set* is not a false
alarm: the blit genuinely still has destination writes left to make.

Program: reuses A3's own `EARLY_LARGE` capture (`prog_earlycheck_large.s`,
200-word blit, no `NMH_MUTATE` of its own -- this assertion always uses
the true large capture, so BBUSY-PREDICTS is well-defined even while A3
itself is mutated).

Observed: the stored `DMACONR`'s `BBUSY` bit, and, in `ram_log()`, the
cycle of the last of the blit's 200 destination writes against the cycle
of the prescribed dummy read.

Expected value: `BBUSY` reads set, and the last destination write's cycle
is after the dummy read's cycle -- more writes really were still to come.

Measured: on both cores, only 26 of the blit's 200 destination writes had
landed by the time of the read; the last lands over 400 cycles later.

No mutation: this assertion has no id of its own to swap a capture under
(it always reads the true `EARLY_LARGE` capture); its sensitivity is the
same as A3's own busy-bit check, already demonstrated there.

### A4 [p194,p198] the blitter has higher DMA priority than the 68000

Claims: `6-system-dma.c13`, `ch6-sysdma.c11`, `6-blitter-priority.c1`,
`ch6-sysdma.c52`.

Proposition: with `BLTPRI` set ("nasty"), the blitter never yields a
contested cycle -- its fixed higher priority holds for the whole blit.

Program (`prog_sharing.s`, `NASTY` build): the same CPU scratch-counter
loop as A2, `BLTPRI`=1, a large continuously-demanding blit.

Observed (precondition, its own id, "A4PRE"): the blit's active window
spans enough cycles to have contained several CPU-loop iterations had the
CPU been let through -- guards against a vacuously short window.

Observed (claim): the count of scratch-word writes inside the active
window.

Expected value: precondition holds, and the count is exactly zero.

Mutation (`prog_sharing_share.s`, `NMH_MUTATE=A4`): swaps in the
share-mode capture for this same check; the unchanged "count is zero"
check must go red (share mode lets some through, per A8/SLOTSPAN below).
The precondition itself is not separately exercised by a mutation: every
capture in this package that shrinks the window also changes `BLTPRI` or
the display configuration, which flips the main assertion rather than
isolating the window-length precondition alone -- not exercised, by a
scope decision.

### A5-WITHDRAWN [p198] share mode: the blitter yields after N consecutive denied CPU cycles

Claims: `6-blitter-priority.c13`, `ch6-sysdma.c53`, `ch6-sysdma.c62`.
**Withdrawn** (`checks.record()`, not `expect()` -- these ids are not on
a graded line).

p.198: "If the 68000 is unsatisfied for three consecutive memory cycles,
the blitter will release the bus for one cycle." The RAM-port log can
directly count how many consecutive memory slots the blitter wins between
two consecutive granted CPU accesses in the share-mode window
(`prog_sharing_share.s`): on both cores the dominant pattern is exactly 4
blitter accesses (4 memory slots) between consecutive CPU accesses (189
of 241 fx68k instances; 244 of 253 TG68K instances) -- not 3.

The page does not say from which slot after a granted CPU access the
"unsatisfied" count is meant to start. Two readings are both consistent
with the same data and disagree with each other:

- Count every intervening slot: 4 denied opportunities, contradicting
  "three".
- Count only the slots after the CPU's own next request, excluding the
  one slot immediately following a granted access that p.196's own
  "internal operation / memory access" cadence (Figure 6-10, asserted
  below as SLOTSPAN) says belongs to the CPU's own internal decode, not a
  fresh request: 4 - 1 = 3, matching the page.

Nothing at `nanomig.v`'s ports distinguishes these two readings -- both
are read off the same four-slot gap. Writing a new assertion here would
mean picking the counting convention myself, which the page does not
license (an unstated band would fail the same way the previous `2x-40x`
band did). The ids stay withdrawn until the manual, or a further reading
of it, fixes the convention.

### A6 [p196] alternate cycles are enough for full speed in the normal case

Claims: `6-system-dma.c53`, `ch6-sysdma.c28`.

Proposition: giving the CPU only every other available cycle still lets
it run at full speed once nothing abnormal is competing for the bus.
p.194: "the allocation of alternate memory cycles to the 68000 makes it
appear to the 68000 that it has the memory all of the time." p.196
attributes this specifically to the four-or-fewer-low-res-bitplane case
(bitplane fetches take the odd slots, Figure 6-9/6-11, p.194).

Program: `prog_bitplane.s` (four low-res planes, `A9`'s own capture) for
the in-window period; `prog_sharing.s` `BASELINE` build for the no-DMA
period.

Observed: the CPU's own scratch-write-to-scratch-write period, taken as
the *modal* (most frequent) gap -- inside the four-lores-plane fetch
windows (p.197's per-line window, see A9/A10 below), and separately in
the no-DMA baseline run.

Expected value: the two modal periods are equal.

Measured: fx68k -- 96 `clk_sys` in-window (10241 gaps, effectively the
only value) vs. 88 `clk_sys` no-DMA (8199 of 8207 gaps). TG68K -- 80
`clk_sys` both ways.

**fx68k: red. TG68K: green.** A real, measured disagreement between the
two cores: on fx68k the CPU's own loop runs measurably slower inside the
four-plane fetch window (96 vs. 88 `clk_sys`, +9%) than with no DMA at
all; on TG68K the two are identical. Not reconciled, per BRIEF.md.

Mutation (`NMH_MUTATE=A6`): the in-window period is taken from
`prog_bitplane_planes6.s` (six planes) instead of four; the unchanged
"equals the no-DMA period" check must go red (six planes measurably slow
the CPU further, per A11/OPEN50 below).

### A8 [p196] a denied cycle means the CPU simply waits for its next available slot

Claims: `6-system-dma.c54`, `6-system-dma.c55`, `ch6-sysdma.c30`.

Proposition: a cycle the CPU is denied is not lost or retried some other
way -- the CPU's next access lands on its very next available (i.e.
same-parity) slot, never the other parity.

Program: `prog_sharing.s`, `BASELINE` build (no blit at all -- every
consecutive-CPU-access gap in the whole run).

Observed: for every pair of consecutive CPU bus accesses (a program fetch
or a scratch write) whose gap is a whole number of slots, whether that
gap is an even or an odd number of slots.

Expected value: no odd-slot gap ever occurs.

Measured: fx68k -- 8000 odd-slot instances of 41006 slot-aligned pairs.
TG68K -- 400 of 45208.

**Red on both cores.** This is a genuine RTL finding, not a defect of the
check: SLOTSPAN (below) independently confirms the *typical* CPU cycle is
the page's clean 2-slot pattern (both cores' modal gap is exactly 2
slots), so the odd-slot instances are real, structurally-recurring
exceptions on top of that pattern, not measurement noise -- roughly one
per loop iteration on fx68k (8000 over ~400 lines of ~20 iterations
each), far rarer on TG68K. See PARITY below for the same fact from a
different reference point.

Mutation (`NMH_MUTATE=A8`): the same "no odd-slot gap" check is applied
instead to the nasty-mode blit's own active window
(`prog_sharing_nasty.s`), which (per A4) has at most one CPU access in
its whole window -- no pair to measure a gap from, so the check reads
vacuously GREEN there. Per the brief's guidance for an assertion that is
already red: this shows the comparator itself is not hard-wired to always
fail, on a different, proven-live capture.

### A9 [p196] four or fewer low-resolution bitplanes still let the CPU through

Claims: `6-system-dma.c61`, `ch6-sysdma.c34`.

(`ch6-sysdma.c35` -- "only if it is ready to ask for the cycle and is the
highest priority item at the time" -- is withdrawn; see the table below.)

Proposition: display DMA at four (or fewer) low-resolution bitplanes
leaves the CPU able to get alternate memory cycles.

Program (`prog_bitplane.s`, `PLANES=4 HIRES=0` build): sets `BPLCON0` for
four low-resolution bitplanes with the standard low-resolution
`DIWSTRT`/`DIWSTOP`/`DDFSTRT`/`DDFSTOP`, lets display DMA reach its active
window, runs the CPU scratch-counter loop throughout.

Observed (precondition, its own id, "A9PRE"): p.197's per-line
fetch windows are found at all (see "Fetch windows" below).

Observed (claim): the minimum, over every one of the run's per-line fetch
windows, of the count of CPU chip-bus accesses (a program fetch or a
scratch write) inside that window.

Expected value: precondition holds, and the minimum is at least one --
not just "some window has a CPU access", every one does.

Measured: minimum 66 per window on fx68k, 80 on TG68K (both cores: 768
windows).

Mutation (`NMH_MUTATE=A9`): the capture is `prog_bitplane_hires.bin`
(A10's own build) instead; the unchanged "minimum > 0" check must go red
(A10 measures exactly 0 there). `PLANES=4 HIRES=1` was the obvious
mutation partner, but the page does not predict it reaches zero *outside*
its own dedicated A10 window definition -- see A10 below, where it does,
on the page's own terms.

Precondition mutation (`NMH_MUTATE=A9PRE`): the capture is
`prog_sharing.bin` (no bitplane DMA enabled at all); the unchanged
"windows are found" check must go red (zero windows).

### A10 [p196,p197] four high-resolution bitplanes lock the CPU out entirely

Claims: `6-system-dma.c71`, `ch6-sysdma.c42`.

Proposition: at four high-resolution bitplanes (double the low-res fetch
bandwidth), the CPU gets no chip-bus access at all during the active
display window.

Program (`prog_bitplane.s`, `PLANES=4 HIRES=1` build, `prog_bitplane_hires.s`):
as A9, `BPLCON0`'s `HIRES` bit set, the high-resolution
`DDFSTRT`/`DDFSTOP` window.

**Fetch windows** (shared with A9, A6, OPEN50, PARITY): BPL1-range reads
($0C6000..) are grouped into per-line bursts (a gap over 200 `clk_sys`
starts a new burst). p.197: four high-resolution bitplanes need "all of
the available memory time slots during the display time just to fetch the
40 data words for each line of the four bitplanes (40 * 4 = 160 time
slots)", and Figure 6-12 fills every one of those 160 slots with planes
in the repeating order 4-2-3-1 -- BPL1 is fetched *last* in its own
4-slot group. Figure 6-11 (six-bitplane, same page) shows the same
structure at low resolution, an 8-slot group ending in plane 1. So each
burst's own window is: from `(burst's own modal read-to-read spacing) -
1 slot` before its first read, to its last read -- "3 slots before" at
high resolution, "7 slots before" at low resolution, both derived from
the capture's own measured spacing rather than a hard-coded,
resolution-specific constant.

Observed (precondition, its own id, "A10PRE"): every window's
non-CPU (DMA) access count.

Expected value (precondition): every window is fully DMA -- 160 of 160
slots, none left over, matching p.197's "40 * 4 = 160 time slots"
exactly.

Observed (claim): the maximum, over every window, of the count of CPU
chip-bus accesses inside it.

Expected value: precondition holds, and the maximum is exactly zero --
not just "most windows are empty", none has any CPU access at all.

Measured: 768 of 768 windows fully DMA (160 of 160 slots) on both cores;
maximum CPU accesses per window = 0 on both cores. **This is the page's
own prediction, exactly**, once the window is the one p.197 actually
describes (see "A10 against pp.195-197" in the validator's report: the
previous test counted scratch writes over the *whole run*, including
horizontal/vertical blanking, which p.197 explicitly excludes from the
lockout -- "except during horizontal and vertical blanking" -- so it was
measuring outside the claim's own scope).

Mutation (`NMH_MUTATE=A10`): the capture is `prog_bitplane.bin` (A9's own
low-resolution build) instead; the unchanged "maximum == 0" check must go
red (66-67 per window on fx68k, 80 on TG68K, per A9).

Precondition mutation (`NMH_MUTATE=A10PRE`): the capture is
`prog_sharing.bin` (no bitplane DMA at all); the unchanged "every window
fully DMA" check must go red (zero windows found at all).

### A11 [p196] more than four low-resolution bitplanes measurably steal cycles

Claims: `6-system-dma.c62`, `ch6-sysdma.c36`.

Proposition: raising low-resolution bitplane count above four steals
cycles from the CPU that it had at four or fewer.

Program: `prog_bitplane.s`, `PLANES=6 HIRES=0` build, vs. the `PLANES=4
HIRES=0` build (A9), both run for the same fixed cycle budget.

Observed: total scratch-counter-loop writes over the whole run,
`PLANES=6` vs. `PLANES=4`.

Expected value: `PLANES=6`'s count is at most 95% of `PLANES=4`'s.

Measured: fx68k -- 14489 vs. 18586 (78%). TG68K -- 15334 vs. 21478 (71%).

Mutation (`NMH_MUTATE=A11`): the `PLANES=4` capture is compared against
itself; the unchanged "at most 95%" check must go red (100% of itself,
no difference between identical configurations).

### SLOTSPAN [p196] a 68000 cycle is one memory slot plus one free slot

Claim: `6-system-dma.c58`.

Proposition: Figure 6-10 divides an average 68000 cycle into an
internal-operation half and a memory-access half of equal length -- one
memory slot occupied, the neighbouring slot free.

Program: `prog_sharing.s`, `BASELINE` build (no DMA at all).

Observed: the modal (most frequent) gap between consecutive CPU bus
accesses.

Expected value: exactly two slots (16 `clk_sys`).

Measured: 16 `clk_sys` on both cores (32606 of 41006 fx68k gaps; 44808 of
45208 TG68K gaps -- clearly dominant on both).

Mutation (`NMH_MUTATE=SLOTSPAN`): the gap is taken from the four-lores-
plane fetch window instead (contended, from A9's own capture); the
unchanged "exactly two slots" check must go red (96 `clk_sys` on fx68k,
80 on TG68K -- TG68K's own in-window figure happens to equal 2 slots too,
i.e. this specific mutation is uninformative on TG68K where A6 already
shows the in-window and no-DMA periods coincide; it is genuinely red on
fx68k).

### OPEN50 [p196,p197] six bitplanes steal 50% of the open slots four would have left

Claims: `6-system-dma.c65`, `ch6-sysdma.c38`.

Proposition: "bitplane DMA steals 50 percent of the open slots the
processor might have used if there were only four bitplanes displayed"
(p.197, of the six-bitplane case, Figure 6-11).

Program: `prog_bitplane.s`, `PLANES=4` and `PLANES=6` builds, using the
same p.197 fetch windows as A9/A10.

Observed: the median, over every window, of the non-CPU (DMA) access
count, for both plane counts; "open slots" = 160 minus that median.

Expected value: the six-plane open-slot count is exactly half the
four-plane open-slot count.

Measured: four planes -- 80 DMA, 80 open (both cores). Six planes -- 120
DMA, 40 open (both cores). 40 is exactly half of 80.

Mutation (`NMH_MUTATE=OPEN50`): the six-plane side is the four-plane
capture instead; the unchanged "exactly half" check must go red (80 is
not half of 80).

### PARITY [p196] the 68000 uses only the even-numbered memory-access cycles

Claims: `6-system-dma.c50`, `ch6-sysdma.c25`, `6-system-dma.c60`,
`ch6-sysdma.c29`.

Proposition: p.196: "The 68000 uses only the even-numbered memory access
cycles" (`c50`/`c25`); Figure 6-10 labels the memory-access half as "an
even cycle" (`c60`, the same claim as `c50` in different words); some
instructions "do not match perfectly with the allocation of even cycles"
(`c29`, existential -- becomes observable together with `c50`, since a
slot on the wrong phase is exactly such a miss).

Program: `prog_bitplane.s`, `PLANES=4 HIRES=0` build -- display DMA gives
a stable, recurring reference phase (the BPL1 fetches) to measure the
CPU's own accesses against, which the no-DMA baseline cannot.

Observed: every CPU chip-bus access's cycle, taken modulo 16 `clk_sys`
(two slots) relative to the run's first BPL1 fetch, split into the two
possible 8-`clk_sys`-wide phases.

Expected value: every CPU access falls in the same phase as the display's
own reference; the other phase is never used.

Measured: fx68k -- 46458 in the reference phase, 46508 in the other (of
92966). TG68K -- 53692 vs. 53732 (of 107424). **Both cores: essentially a
50/50 split, not "only even".**

**Red on both cores.** Consistent with A8 above (the same fact from a
different reference point): the CPU does use the "wrong" phase, often
enough that at this boundary "only even-numbered" does not hold as
written. `c29`'s existential claim ("some instructions... miss") is
exactly what this demonstrates -- it is the same access pattern, not a
separate measurement.

Mutation (`NMH_MUTATE=PARITY`): the reference capture is
`prog_bitplane_hires.bin` (A10's build) instead, and the phase count is
restricted to that capture's own fetch windows (A10's own windows, where
A10 already proved the CPU has zero chip-bus accesses at all). Measured
first over the *whole* hi-res run (not window-restricted): still roughly
50/50 (20904 of 41768 fx68k, 23014 of 45984 TG68K) -- CPU accesses
outside the display windows (before/after/blanking) show the same split,
so that alone would not demonstrate anything about the comparator.
Restricted to the windows themselves, both counts are 0 of 0 (vacuously
`phase_b == 0`, matching A10's own zero), which is what the header
comment's mutation record uses.

### TAS [p196] TAS's read-modify-write cycle is indivisible

Claim: `6-system-dma.c56`.

Proposition: "the indivisible read-modify-write cycle that is used only
in this instruction will not fit into a DMA memory access slot" -- TAS's
own read and write of the same byte form one special bus cycle, not two
ordinary, separately-arbitrated ones.

Program (`prog_tas.s`): stores a known byte, then executes one `TAS.B` on
it, `DONE`.

Observed: in `ram_log()`, filtered to the one address TAS touches and
coalesced (`nmh::coalesce()` -- nothing else in this tiny program ever
revisits that address, so two adjacent identical entries there can only
be the same logical, possibly wait-stated cycle sampled twice, never two
genuinely separate accesses), the cycle gap between TAS's own read and
its own write.

Expected value: that gap does not equal the ordinary 2-slot (16
`clk_sys`) spacing SLOTSPAN establishes for two separately-arbitrated CPU
bus cycles -- TAS's cycle does not fit the normal pattern.

Measured: fx68k -- 24 `clk_sys` (3 slots: W@2273653 init, R@2273717,
W@2273741 -- the read-to-write gap is stretched to three slots, with
nothing in between). TG68K -- 16 `clk_sys` (2 slots: indistinguishable
from an ordinary access pair).

**fx68k: green. TG68K: red.** A real, measured disagreement: fx68k models
TAS's own elongated, indivisible bus cycle exactly as the NOTE on p.196
describes it (longer than a normal cycle, matching "will not fit into a
DMA memory access slot"); TG68K's core does not reproduce that specific
timing quirk and executes it as an ordinary read-then-write pair. Not
reconciled.

No mutation with its own id: TAS has only one program and one capture:
there is no "unmutated, still-live" version of this check to compare
against. Its sensitivity is established by SLOTSPAN's own baseline, which
TAS's own expected value directly depends on and which SLOTSPAN
demonstrates is not vacuous.

### ch6-multi.c4 [p187] a 68020 running from cache may race ahead of the blitter (TG68K only)

Claim: `ch6-multi.c4`.

Proposition: "a 68020, perhaps running out of its cache, may be able to
get at the data before the blitter has finished writing it." `cpu_config`
selects "CPU-Type: 00 = 68000, 01 = 68010, 11 = 68020" (`nanomig.v:198`),
feeding the TG68K core; fx68k is a fixed MC68000 implementation and does
not read this input, so it is reported not applicable there, never
graded.

Program: `prog_sharing.s`, `BASELINE` build (no DMA), run twice on TG68K
over the same fixed cycle budget: once with `cpu_config=0`, once with
`cpu_config=3`.

Observed: the CPU's own uncontested scratch-write count over the budget,
both configurations.

Measured: 9041 loop iterations both ways -- **no measurable difference**.
Repeated; stable both times.

**Withdrawn** (stays out of the graded lines). The premise the report
corrected -- that `cpu_config` cannot select 68020 behaviour at all --
was wrong (`nanomig.v:198` and `Config.cpu_config`, `nanomig_harness.h`,
do select it, into the TG68K core). But selecting it here produces no
observable difference in this fixture's own chip-bus access rate, so
there is nothing to grade it against without going further (a cache- or
fast-RAM-resident stimulus, which this harness has no way to place code
into -- see `6-blitter-done-flag.c8`/`ch6-done.c9` below, the same gap).

### ORDER [p169] a blitter copy finishes before an equal-sized CPU copy loop

No claim id (see the withdrawn-table entry for `6-intro.c5`/`ch6-intro.c5`
below: only the ordering is graded here, not "approximately twice", which
has no numeric tolerance on the page).

Proposition: "the blitter is one of the two coprocessors... When copying
memory, it is approximately twice as fast as the 68000" (p.169). The
factor cannot be graded without an invented tolerance, but the ordering
it implies -- the blitter finishes a same-sized copy first -- can be, on
the same yardstick used elsewhere in this package (first-to-last
destination write).

Programs: `prog_copyloop.s` (a plain 68000 `MOVE.W (a0)+,(a1)+` loop
copying 500 words, no DMA competing); the nasty-mode blit's own capture
(`prog_sharing_nasty.s`, established by A4, copying the same 500 words,
uncontended since nothing yields it a cycle).

Observed: each capture's own first-to-last-destination-write span.

Expected value: the blit's span is shorter.

Measured: blit -- 8112 `clk_sys` (both cores, identical). CPU copy loop --
44200 `clk_sys` (fx68k), 48112 `clk_sys` (TG68K).

No mutation: this check carries no claim id and is not itself part of the
package's per-claim coverage; it exists to settle the ordering half of
`6-intro.c5`/`ch6-intro.c5` (see below), and its own two captures are
already independently established elsewhere (the blit capture by A4, the
copy loop by its own fixed, deterministic program).

## Claims not asserted

| claim id(s) | page | why |
|---|---|---|
| `6-intro.c5`, `ch6-intro.c5` | 169 | Factor not toleranced ("approximately twice as fast" has no numeric tolerance on the page); ordering asserted by ORDER above (a blitter A-to-D copy of 500 words, uncontended, finishes before a 68000 `MOVE.W` copy loop of the same 500 words: 8112 `clk_sys` vs. 44200/48112 `clk_sys`, both cores). ORDER carries no claim id of its own -- only the ordering is graded, not the factor, so these ids stay withdrawn rather than moving onto ORDER's own label. |
| `ch6-desc.c12` | 182 | Compares the blitter's descending-pointer setup convention against a hypothetical 680x0 `-(An)` predecrement outcome -- a blit data-correctness question (whether a descending-mode blit result matches or is displaced by one word from the `-(An)` convention), not a DMA-arbitration one. Out of this package's own boundary (CPU/blitter bus sharing); the lead can hand this id to `ch6-blitter-geometry`, which already exercises blit addressing correctness. |
| `6-blitter-done-flag.c9`, `ch6-done.c11` | 186 | These claims are permissive ("may not return the correct result"), so no simulation outcome contradicts them; a deterministic Verilator run cannot falsify a "may". The mechanism the page actually gives (p.186's NOTE) is deterministic, not metastable: a blit just started can be locked out of its first DMA cycle by display fetches, so `DMAF_BLTDONE` has not yet gone high, while the CPU (from Fast memory or cache, on real hardware) keeps running and can observe the not-yet-set flag. BBUSY-PREDICTS above demonstrates the deterministic half of this directly (`c10`/`c12`). |
| `ch6-done.c8` | 186 | Same claim as `6-blitter-done-flag.c9`/`ch6-done.c11` (the first test may be wrong), same correction. |
| `6-blitter-done-flag.c8`, `ch6-done.c9` | 186 | Describes the CPU continuing to execute from Fast memory or its internal cache while the blitter is denied chip memory. `nanomig_harness.h`'s `Program`/`load_program()` only places and boots code in identity-mapped chip RAM; it has no supported way to place and run a program from the `fastram_*` port or to exercise the CPU cache path. Precisely, the gap is: (1) `nmh::Config` has `fastram_config` and `turbo_config` fields (`nanomig_harness.h:93-104`); (2) the harness serves a `fastram_*` port "for tests that do enable turbo/fastram" (lines 485-526); (3) `cpu_wrapper.v:611` decodes Zorro II autoconfig at $E80000; (4) the TG68K build compiles with `ENABLE_CACHE`/`CHIPRAM_CACHE` (`nanomig.mk`). Only a loader into fast RAM (or a program that configures the board and copies its own loop there) is missing, and no such program was written or run here -- a harness capability gap, not worked around, per BRIEF.md's own instruction not to edit `integration/harness/`. |
| `ch6-multi.c4` | 187 | See its own section above: `cpu_config=3` (68020) was tried on TG68K and measured, and produced no observable difference from `cpu_config=0` in this fixture (9041 loop iterations either way); fx68k does not read `cpu_config` at all. Not "no 68020 exists" (that premise was wrong) but "selecting it here changes nothing this boundary can see, absent a cache- or fast-RAM-resident stimulus this harness cannot place" (the same gap as `6-blitter-done-flag.c8`/`ch6-done.c9`). |
| `6-system-dma.c12`, `ch6-sysdma.c10` | 194 | "The two lowest priority levels" ranks the blitter and CPU against every other DMA channel (audio, disk, sprites, copper, bitplane), not just against each other. Confirming it needs all of those channels driven simultaneously against blitter and CPU, a materially larger fixture than this package's scope. The narrower fact -- the blitter outranks the CPU specifically -- is A4. Holds as a scope decision with a named fixture (a full-DMA-channel-contention fixture would settle the rest). |
| `ch6-sysdma.c35` | 196 | "Only if it is ready to ask for the cycle and is the highest priority item at the time" -- no assertion in this package measures readiness-and-priority as its own condition (A9 measures the outcome, not this qualifier). A fixture with a higher-priority requester (a nasty-mode blit) contending during the lores display, checking the CPU gets no slot in the fetch windows then, would settle it; not built here. |
| `6-system-dma.c51`, `6-system-dma.c52`, `ch6-sysdma.c26`, `ch6-sysdma.c27` | 196 | "About half" of instruction time is an average over the 68000's own full instruction mix, with no numeric tolerance given on the page. An instruction-timing histogram compiled per core would settle it. (Not, as previously stated, because the two cores might disagree -- BRIEF.md treats a cross-core disagreement as a result to report, not a reason to decline; the real reason is the missing tolerance.) |
| `6-system-dma.c57`, `ch6-sysdma.c33` | 196 | The indivisible TAS cycle "will not fit into a DMA memory access slot" -- TAS itself is asserted above (TAS, `c56`); this pair states the *consequence* ("not fit") without the page naming an observable difference between "did not fit" and "took a longer, stretched cycle instead", which is exactly what TAS's own measurement shows on fx68k. (Not, as previously stated, "risks an indefinite bus stall" -- that specific hazard was never run or observed here.) |
| `ch6-sysdma.c32` | 196 | "The 68000 test-and-set instruction (TAS) should never be used in the Amiga" is prescriptive advice to a programmer, not an RTL-observable claim; holds as written. |
| `ch6-sysdma.c31` | 196 | "Most instructions do not cause cycles to be missed" is a statistic over the instruction set with no numeric tolerance; holds as written (PARITY/A8 above establish the *existence* of misses, per `c29`, not their rate). |

Total: 38 claim ids on graded (PASS:/FAIL:) lines across 14 assertions
(A1, A2, A3, A4, A6, A8, A9, A10, A11, SLOTSPAN, OPEN50, PARITY, TAS,
BBUSY-PREDICTS) plus the unlabelled fixture check; `ch6-multi.c4` is
graded only conditionally (TG68K), and in this run measured no
difference, so it produced no graded line and is counted with the
withdrawals below instead. 3 claim ids withdrawn by `checks.record()`
(A5). 20 claim ids withdrawn in the table above, including
`ch6-multi.c4`. 61 claim ids in the spec-stage file (38 + 3 + 20 = 61).
