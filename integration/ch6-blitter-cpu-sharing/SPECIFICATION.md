# ch6-blitter-cpu-sharing

Claims from Chapter 6 of the Amiga Hardware Reference Manual, 3rd edition,
pages 169-198: the blitter as an asynchronous coprocessor, blitter pointer
registers, the descending-mode pointer convention, the blitter-done-flag
read quirk, and how the blitter and the 68000 share the chip memory bus
(DMA priority, the three-consecutive-cycle share rule, and bitplane-DMA
bandwidth pressure).

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
  - `BLTCON1` bit 1: descending-mode select (`agnus_blitter.v:286`,
    `desc = ~line & bltcon1[1]`).
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
  assertions -- no separate "-mutation" line. Where an assertion is
  already red at baseline, its mutation instead shows the same, unchanged
  comparator reading GREEN on a different, proven-live input (A8, PARITY).
- Both CPU cores run every test. An assertion's verdict is reported for
  each, and a disagreement between them is reported as such, not
  reconciled.

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

### DESC-BASE / DESC-OFFBY1 [p182] the descending-mode pointer convention

Claim: `ch6-desc.c12`.

Proposition: "This differs from 680x0 predecrement, where an address
register would point to the word after the last rather than the last
word" -- in the blitter's own descending mode (`BLTCON1` bit 1), the
pointer registers must be preloaded with the address of the block's LAST
word, not the address one word past it (the 680x0 `-(An)` convention).

Program (`prog_descblit.s`): stores a known 4-word pattern
($1111,$2222,$3333,$4444, ascending addresses) at `DB_SRC`, plus a fifth,
distinct sentinel word ($5555) one word past the block, and a matching
fifth sentinel slot at `DB_DST`. Runs a straight, non-shifted, descending
A-to-D copy of the 4 words. `BLTAFWM`/`BLTALWM` are set to $FFFF (every
program in this suite that runs a real transfer sets them; leaving them
at their post-reset state masks the whole copy to zero). This file alone
(no symbol set) is the blitter's own convention: `BLTAPTH`/`BLTDPTH`
point at `DB_SRC+6`/`DB_DST+6`, the last word.

Observed (precondition, "DESC-BASE", no claim id): `peek_logical()` of
all 5 destination words.

Expected value (precondition): the 4-word pattern is copied faithfully,
and the sentinel word past the block (harness-zero-initialized chip RAM,
never written) still reads $0000.

Observed (claim): whether a write lands in `ram_log()` at the
destination block's own first word, and separately at the sentinel word
one past it, comparing the blitter's own convention (capture A) against
`prog_descblit_offby1.s` (`OFFBY1` set, capture B: `BLTAPTH`/`BLTDPTH` at
`DB_SRC+8`/`DB_DST+8`, one word past the last -- the 680x0 `-(An)`
convention).

Expected value: capture A writes the block's own first word and leaves
the sentinel alone; capture B does the opposite -- the first word goes
unwritten and the sentinel is written instead. The same 4-word transfer
is displaced by exactly one word.

Mutation (`NMH_MUTATE=DESC`): capture B is replaced with a second copy of
capture A (the blitter's own convention run twice); the displacement
disappears (both captures then write the first word and leave the
sentinel alone), so the unchanged "A and B differ this way" check must go
red.

### A3 [p187] a small blit can finish before the CPU's first check

Claim: `ch6-multi.c3`.

Proposition: a 68000 may be slow enough, relative to a small blit, that
the blit has already finished the first time the CPU checks.

Program (`prog_earlycheck.s`, `SMALL` build): starts a one-word blit,
performs the manual's prescribed one dummy chip-memory read, immediately
reads `DMACONR` and stores it, `DONE`.

Observed: the stored `DMACONR`'s `BBUSY` bit.

Expected value: `BBUSY` reads clear.

Mutation (`prog_earlycheck_large.s`, `EARLY_LARGE` set,
`NMH_MUTATE=A3`): the blit is sized to take many more cycles; the
unchanged check (still expecting `BBUSY` clear on the first read) must go
red.

### A3PRE [p187] the blit's own destination write precedes the dummy read

Claim: `ch6-multi.c3`.

Proposition: the same as A3, from the other side -- the blit really did
finish before the CPU's own procedural first step, not merely before the
later `DMACONR` read.

Program: `prog_earlycheck.s`'s `SMALL` build (its own capture,
independent of A3's own mutation).

Observed: the blit's destination write's cycle in `ram_log()`, against
the dummy read's cycle.

Expected value: the destination write precedes the dummy read.

Mutation (`prog_earlycheck_reordered.s`, `EARLY_REORDER` set,
`NMH_MUTATE=A3PRE`): the dummy read is moved to before the blit is even
started; the unchanged "destination write precedes the dummy read" check
must go red.

### BBUSY-PREDICTS [p186] BBUSY set correctly predicts more writes still to come

Claims: `6-blitter-done-flag.c10`, `ch6-done.c12`.

Proposition: after the manual's prescribed intervening chip-memory read
(p.186's NOTE), a `BBUSY` read that comes back *set* is not a false
alarm: the blit genuinely still has destination writes left to make.

Program: `prog_earlycheck_large.s` (200-word blit): the manual's dummy
read, then the immediate `DMACONR` read and store.

Observed: the stored `DMACONR`'s `BBUSY` bit, and, in `ram_log()`, the
cycle of the last of the blit's 200 destination writes against the cycle
of the RESULTS store that captured the read (the moment the read `BBUSY`
value was actually recorded -- not the earlier dummy read, which only
bounds when the CPU started looking).

Expected value: `BBUSY` reads set, and the last destination write's cycle
is after the RESULTS store's cycle -- more writes really were still to
come.

Measured: on both cores, only 26 of the blit's 200 destination writes had
landed by the time RESULTS was stored; the last lands hundreds of cycles
later.

Mutation (`NMH_MUTATE=BBUSY-PREDICTS`): the true 200-word build is
replaced with the true one-word (`SMALL`) build, already finished (BBUSY
clear) by the time it is read; the unchanged "BBUSY set, more writes
still to come" check must go red.

### A2 [p169,p186] the CPU keeps running while the blitter works

Claims: `6-intro.c20`, `ch6-intro.c21`, `ch6-intro.c22`,
`6-blitter-done-flag.c2`, `6-blitter-done-flag.c3`, `ch6-done.c2`,
`ch6-done.c3`.

Proposition: the blitter is an asynchronous coprocessor; the 68000 is not
halted or blocked while a blit is in progress -- not just briefly, but
for the whole of it.

Program (`prog_sharing.s`, `SHARE` build): a tight CPU loop increments a
counter and stores it to the scratch word on every pass, concurrently
with a large, continuously-demanding blit (`BLTPRI`=0) sized to run for
many loop iterations.

Observed: every scratch-word write in `ram_log()` whose cycle falls
between the blit's first and last destination writes (its active window,
both addresses distinct from the scratch address); the largest gap
between two consecutive such writes.

Expected value: at least two scratch writes fall inside the active
window, and the largest in-window gap does not reach even half the
window's own length. This is a deliberately loose sanity bound, not a
page-derived number: the page states no tolerance for "does not halt",
and any threshold far above the measured value would do the same job --
it only needs to rule out the CPU stopping for a meaningful fraction of
the window, which a genuinely halted CPU would fail by a wide margin.

Measured: the largest in-window gap is 224 of 10104 `clk_sys` on fx68k,
216 of 10200 on TG68K -- about 2% of the window, both cores.

Mutation (`prog_sharing_tiny.s`, `SHARE` and `TINY` both set,
`NMH_MUTATE=A2`): the blit shrinks to a near-instant one-word transfer;
the unchanged check must go red (the active window collapses to zero
length, with fewer than two scratch writes inside it).

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
a graded line; no mutation).

p.198: "If the 68000 is unsatisfied for three consecutive memory cycles,
the blitter will release the bus for one cycle." p.194 defines a memory
cycle as one colour clock (one slot). The page does not fix from which
slot after a granted CPU access the "unsatisfied" count is meant to
start; three readings are each consistent with the RAM-port log and
disagree with each other about the CPU-to-CPU grant spacing they predict
in the share-mode window (`prog_sharing_share.s`):

| reading | the 68000 requests from | spacing between grants | blitter slots between |
|---|---|---|---|
| every slot, from the granted slot + 1 | g+1 | 4 slots | 3 |
| every slot, from g+2 (past Figure 6-10's own internal-operation portion) | g+2 | 5 slots | 4 |
| only the 68000's own even-numbered memory cycles (p.196), from g+2 | g+2 | 8 slots | 7 |

The RAM-port log shows a dominant 5-slot spacing (4 blitter accesses
between consecutive CPU grants: 189 of 241 fx68k instances, 244 of 253
TG68K), matching the second reading -- but that reading puts every other
CPU grant on an odd-numbered slot, which p.196 excludes (see PARITY
below, which grades exactly those odd-slot grants in the share-mode
window and is red on both cores independent of which of these three
readings is meant). No reading agrees with both p.198 and p.196 at once;
the page is ambiguous at the point tested. Writing a new assertion here
would mean picking the counting convention itself, which the page does
not license.

## How slots are numbered at the RAM port

A8, PARITY and SYSDMA-C29 below all depend on this. Figure 6-9 (p.195)
draws a memory-refresh block at the start of each horizontal line's own
DMA-slot allocation, ahead of the display fetch region; p.194 gives a
line as "227.5 'color clocks', or memory access cycles" -- 227 whole
slots, an odd count, so any fixed-cycle reference's parity flips from one
line to the next.

At the RAM port, refresh shows up as reads below word address $1000 --
far below every program-specific range this package uses (`PROGRAM_ORG`,
every scratch word, every blit source/destination, every bitplane
pointer) -- 4 per line, 2 slots apart, matching Figure 6-9's own
refresh block. Grouping them (a gap over 100 `clk_sys` starts a new
group) gives each line's own start, independent of any single global
reference. A CPU chip-bus access is graded even/odd by its own slot
offset from *its own line's* last refresh read, floor-divided by one
slot (8 `clk_sys`) -- not by its raw remainder, since a CPU sample and a
DMA (refresh) sample are each taken at a fixed sub-slot phase 4 `clk_sys`
apart, so their difference is never an exact multiple of a slot, only its
floor is meaningful. This is a fact about this RTL's own two sample
phases, not a page citation.

Measured cross-check: on both cores, in the four-lores-plane build, every
line's first BPL1 fetch and every CPU access classified "even slot" by
this reference land on the display-DMA schedule Figure 6-9 and p.197
describe, and TG68K -- which p.196 predicts uses only even-numbered
slots -- comes out with zero accesses classified "odd" under this
reference, both in the no-DMA baseline (0 of 45209) and inside every
bitplane fetch window (all in-window accesses even, both cores). That
internal consistency is the validation: a reference that mislabelled the
parity would show TG68K using the "wrong" phase as often as the right
one, which it does not.

### A8 [p194,p196] a denied cycle means the CPU simply waits for its next available slot

Claims: `6-system-dma.c54`, `6-system-dma.c55`, `ch6-sysdma.c30`.

Proposition: a cycle the CPU is denied is not lost or retried some other
way -- the CPU's next access lands on its very next available (i.e.
same-parity) slot, never the other parity.

Program: `prog_sharing.s`, `BASELINE` build (no blit at all).

Observed: for every pair of consecutive CPU bus accesses (a program fetch
or a scratch write) within one line (no line boundary -- no refresh
group's own first read -- falling between them) whose gap is a whole
number of slots, whether that gap is an even or an odd number of slots.
Pairs that span a line start are excluded: Figure 6-9's numbering
restarts every line, so a fixed-cycle reference cannot compare parity
across that boundary.

Expected value: no odd-slot gap ever occurs.

Measured: 7801 odd-slot instances of 40606 within-line pairs on fx68k;
0 of 44808 on TG68K.

**fx68k: red. TG68K: green.** A real, measured disagreement between the
two cores, not reconciled (BRIEF.md). See SYSDMA-C29 below for what the
fx68k red is.

Mutation (`NMH_MUTATE=A8`): the baseline capture is replaced with the
four-lores-plane build's own in-fetch-window CPU accesses (a different,
live, nonzero population -- 50109 pairs on fx68k, 60467 on TG68K) through
the same, unchanged comparator (including its `pairs > 0` guard); the
check reads GREEN there on both cores, showing the comparator is not
hard-wired to fail.

### SYSDMA-C29 [p196] some 68000 instructions miss the even-cycle allocation

Claim: `ch6-sysdma.c29`.

Proposition: "Some 68000 instructions do not match perfectly with the
allocation of even cycles and cause cycles to be missed" -- existential.

Program: `prog_sharing.s`, `BASELINE` build (the same capture A8 uses).
Its counting loop is `ADDQ.W #1,D0` / `MOVE.W D0,(A1)` / `BRA.S .loop`;
`BRA.S` is a 10-clock-period internal operation (MC68000 timing tables),
not a whole multiple of 4, unlike every other instruction in the loop.

Observed: the same within-line consecutive-CPU-access-pair parity A8
grades, on the same capture.

Expected value: fx68k -- more than one odd-slot (missed-cycle) instance
(existential, but a single instance could be the loop's own one-off
startup transient before its steady rhythm settles, not a recurring
per-iteration miss; more than one instance is what tells the two apart,
the same way `mode_of()` elsewhere in this file discounts a lone outlier
gap rather than a structurally-recurring value). Not exercised on TG68K,
whose own loop timing does not produce this gap at all -- see A6 below,
where the same distinction (BRA.S vs. a whole-4-clock-aligned loop) is
what the original A6 check conflated with alternate-cycle allocation on
fx68k specifically.

Measured: 7801 of 40606 within-line pairs on fx68k (recurring, once per
loop iteration -- the fetch after `BRA.S`'s own internal operation lands
on an odd slot instead of waiting for the next even one). TG68K: 0 of
44808 (not exercised, per the expected value above).

Mutation (`prog_sharing_aligned.s`, `ALIGNED` set, `NMH_MUTATE=SYSDMA-C29`):
the loop's `BRA.S` is replaced with `JMP (An)` (8 clock periods, a whole
multiple of 4, MC68000 timing tables) -- every instruction's own bus-cycle
timing then aligns to whole slots. The unchanged ">1 instance" check must
go red on fx68k (0 or 1 stray instance, not a recurring pattern).

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

Observed (precondition, "A9PRE", no claim id -- a precondition
establishes that windows exist at all, not the claim itself): p.197's
per-line fetch windows are found.

Expected value (precondition): at least one window is found.

Observed (claim): the minimum, over every one of the run's per-line fetch
windows, of the count of CPU chip-bus accesses (a program fetch or a
scratch write) inside that window.

Expected value: precondition holds, and the minimum is at least one --
not just "some window has a CPU access", every one does.

Measured: minimum 66 per window on fx68k, 80 on TG68K (both cores: 768
windows).

Mutation (`NMH_MUTATE=A9`): the capture is `prog_bitplane_hires.bin`
(A10's own build) instead; the unchanged "minimum > 0" check must go red
(A10 measures exactly 0 there).

Precondition mutation (`NMH_MUTATE=A9PRE`): the capture is
`prog_sharing.bin` (no bitplane DMA enabled at all); the unchanged
"windows are found" check must go red (zero windows).

### A10 [p196,p197] four high-resolution bitplanes lock the CPU out entirely

Claims: `6-system-dma.c71`, `ch6-sysdma.c42`.

Proposition: at four high-resolution bitplanes (double the low-res fetch
bandwidth), the CPU gets no chip-bus access at all during the active
display window.

Program (`prog_bitplane_hires.s`, `PLANES=4 HIRES=1` build): as A9,
`BPLCON0`'s `HIRES` bit set, the high-resolution `DDFSTRT`/`DDFSTOP`
window.

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
1 slot` before its first read, to its last read -- derived from the
capture's own measured spacing rather than a hard-coded,
resolution-specific constant.

Observed (precondition, "A10PRE", no claim id): every window's non-CPU
(DMA) access count.

Expected value (precondition): every window is fully DMA -- 160 of 160
slots, none left over, matching p.197's "40 * 4 = 160 time slots"
exactly.

Observed (claim): the maximum, over every window, of the count of CPU
chip-bus accesses inside it.

Expected value: precondition holds, and the maximum is exactly zero --
not just "most windows are empty", none has any CPU access at all.

Measured: 768 of 768 windows fully DMA (160 of 160 slots) on both cores;
maximum CPU accesses per window = 0 on both cores. This is the page's
own prediction, exactly, once the window is the one p.197 actually
describes -- counted over the 160-slot fetch window itself, not the
whole run (which also includes horizontal and vertical blanking, which
p.197 explicitly excludes from the lockout).

Mutation (`NMH_MUTATE=A10`): the capture is `prog_bitplane.bin` (A9's own
low-resolution build) instead; the unchanged "maximum == 0" check must go
red (66-80 per window, per A9).

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

Measured: fx68k -- 14489 vs. 18586 (78%). TG68K -- 15334 vs. 21478 (72%).

Mutation (`NMH_MUTATE=A11`): the `PLANES=4` capture is compared against
itself; the unchanged "at most 95%" check must go red (100% of itself,
no difference between identical configurations).

### A6 [p196] alternate cycles are enough for full speed in the normal case

Claims: `6-system-dma.c53`, `ch6-sysdma.c28`.

Proposition: giving the CPU only every other available cycle still lets
it run at full speed once nothing abnormal is competing for the bus.
p.194: "the allocation of alternate memory cycles to the 68000 makes it
appear to the 68000 that it has the memory all of the time." p.196
attributes this specifically to the four-or-fewer-low-res-bitplane case
(bitplane fetches take the odd slots, Figure 6-9/6-11, p.194).

The stimulus loop's own bus cycles must all align to whole slots: p.196
itself says an instruction that does not (like `BRA.S`, see SYSDMA-C29
above) "cause[s] cycles to be missed", which would measure that fact
about the instruction, not this claim about alternate-cycle allocation.
`prog_sharing_aligned.s`/`prog_bitplane_aligned.s` (`ALIGNED` set) use
`ADDQ.W #1,D0` (4 clock periods) / `MOVE.W D0,(A1)` (8) / `JMP (An)` (8)
-- MC68000 timing tables, every one a whole multiple of 4, unlike
`BRA.S`'s 10.

Program: `prog_bitplane_aligned.s` (four low-res planes, aligned loop)
for the in-window period; `prog_sharing_aligned.s` (`BASELINE`, aligned
loop) for the no-DMA period.

Observed: the CPU's own scratch-write-to-scratch-write period, taken as
the *modal* (most frequent) gap -- inside the four-lores-plane fetch
windows (the per-line window derived under A10 above), and separately in
the no-DMA baseline run.

Expected value: the two modal periods are equal.

Measured: 80 `clk_sys` both ways, both cores (12287 in-window gaps, 9039
no-DMA gaps).

Mutation (`prog_bitplane_planes6_aligned.s`, `BP_PLANES6` and `ALIGNED`
both set, `NMH_MUTATE=A6`): the in-window period is taken from six
aligned-loop planes instead of four; the unchanged "equals the no-DMA
period" check must go red (160 `clk_sys` vs. 80, both cores -- six
planes measurably slow the CPU, per A11/OPEN50).

### SLOTSPAN [p196] a 68000 cycle is one memory slot plus one free slot

Claim: `6-system-dma.c58`.

Proposition: Figure 6-10 divides an average 68000 cycle into an
internal-operation half and a memory-access half of equal length -- one
memory slot occupied, the neighbouring slot free.

Program: `prog_sharing.s`, `BASELINE` build (no DMA at all).

Observed: the modal (most frequent) gap between consecutive CPU bus
accesses.

Expected value: exactly two slots (16 `clk_sys`).

Measured: 16 `clk_sys` on both cores.

Mutation (`NMH_MUTATE=SLOTSPAN`): the *same* quantity (the modal gap
between consecutive CPU bus accesses) is taken from the share-mode
blit's own active window instead (`prog_sharing_share.s`) -- contended,
not the stimulus's own free-running rhythm. The unchanged "exactly two
slots" check must go red: the modal gap there is 40 `clk_sys` (5 slots),
both cores.

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

Claims: `6-system-dma.c50`, `ch6-sysdma.c25`, `6-system-dma.c60`.

(`ch6-sysdma.c29` -- "some instructions... cause cycles to be missed" --
is graded separately, as SYSDMA-C29 above: it is existential and does
not follow from "no odd-slot access" one way or the other.)

Proposition: p.196: "The 68000 uses only the even-numbered memory access
cycles" (`c50`/`c25`); Figure 6-10 labels the memory-access half as "an
even cycle" (`c60`, the same claim as `c50` in different words).

Program: `prog_sharing.s` `BASELINE` build (no DMA) for the whole-run
side; `prog_sharing_share.s` for the share-mode blit's own active window.

Observed: every CPU chip-bus access's slot parity, by the per-line
reference described above ("How slots are numbered at the RAM port"),
graded separately over the no-DMA run and over the share-mode blit's own
active window.

Expected value: every CPU access, in both windows, falls on an
even-numbered slot; the odd phase is never used.

Measured: fx68k -- 20006 of 41007 no-DMA accesses on odd slots; 104 of
242 in the share-mode window. TG68K -- 0 of 45209 no-DMA; 124 of 254 in
the share-mode window.

**fx68k: red (both windows). TG68K: red (share-mode window only; clean
in the no-DMA run).** A real, measured disagreement between the two
cores in the no-DMA case, not reconciled; both cores use odd slots under
share-mode contention.

Mutation (`NMH_MUTATE=PARITY`): both windows are replaced with the same
live, proven input A8's own mutation uses -- the four-lores-plane
build's own in-fetch-window CPU accesses, all on even slots by the same,
unchanged reference -- and the check reads GREEN on both cores.

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

Mutation (`prog_copyloop_tiny.s`, `CL_TINY` set, `NMH_MUTATE=ORDER`): the
CPU copy loop shrinks to one word, finishing before the blit; the
unchanged "blit finishes first" ordering check must go red.

### TAS [p196] TAS's read-modify-write cycle is indivisible

Claim: `6-system-dma.c56`.

Proposition: "the indivisible read-modify-write cycle that is used only
in this instruction" -- TAS's own read and write of the same byte form
one bus transaction a genuinely contending master cannot interleave
into, not two ordinary, separately-arbitrated ones. A CPU-core
bus-protocol property (fx68k and TG68K each model the 68000's own bus
cycle, not the Amiga chipset's arbitration), so it is reported per core,
not reconciled.

Program (`prog_tas_share.s`): a continuously-demanding share-mode blit
(`BLTPRI` clear, so the blitter keeps requesting cycles throughout) runs
concurrently with one `TAS.B` on a known byte.

Observed: in `ram_log()`, filtered to the one address TAS touches and
coalesced (`nmh::coalesce()` -- nothing else in this tiny program ever
revisits that address, so two adjacent identical entries there can only
be the same logical, possibly wait-stated cycle sampled twice), the read-
to-write gap; then, over the *whole* log, the count of any other RAM-port
access whose cycle falls strictly between that read and that write.

Expected value: zero other accesses land between TAS's own read and
write, even though the blitter is actively contending for the bus
throughout.

Measured: fx68k -- 5 intervening accesses. TG68K -- 4.

**Red on both cores; not a chipset finding.** Under active share-mode
contention, both cores let other bus masters interleave between TAS's
own read and write -- a property of how each CPU-core model implements
the instruction's bus cycle, not of the Amiga chipset's arbitration
logic, which is this package's own boundary. Not reconciled.

Mutation (`prog_tas_share_mutant.s`, `MUTANT` set,
`NMH_MUTATE=TAS`): `TAS.B` is replaced with an ordinary, separately-
arbitrated read then write of the same byte, under the identical
share-mode contention. The unchanged "zero intervening accesses" check
must go red much more strongly (19 intervening accesses, both cores),
confirming the comparator is sensitive to genuinely non-indivisible
access pairs.

### ch6-multi.c4 [p187] a 68020 running from cache may race ahead of the blitter -- withdrawn

Claim: `ch6-multi.c4`. **Withdrawn** (`checks.record()`; measured but not
graded).

Proposition: "a 68020, perhaps running out of its cache, may be able to
get at the data before the blitter has finished writing it." `cpu_config`
selects "CPU-Type: 00 = 68000, 01 = 68010, 11 = 68020" (`nanomig.v:198`),
feeding the TG68K core; fx68k is a fixed MC68000 implementation and does
not read this input, so it is reported not applicable there.

Measurement (TG68K only, no DMA): `prog_sharing.s`'s `BASELINE` build,
run once with `cpu_config`=3 and once with `cpu_config`=0, over the same
fixed cycle budget. Both give 9041 loop iterations -- no measurable
difference. This contradicts nothing on the page (no DMA is competing for
the bus in this fixture), so it is not a graded red.

**Not tested.** The page's own consequence is a read of the blit's
destination, served from a 68020's cache, before the blitter has
finished writing it -- that needs `cpu_config`=3 *and* the cache path
(`cpu_wrapper.v:514`, `cchip = turbochip_d && (turbodata_d || cpustate ==
2'b00)`; `nanomig.v:208` gates it with `~ovl`, so a program would first
have to clear the overlay with a CIA-A write) while a blit is actively
writing the same address the CPU reads. No such stimulus is written
here.

### 6-blitter-done-flag.c8 / ch6-done.c9 [p186] Fast memory keeps running during a nasty blit -- withdrawn

Claims: `6-blitter-done-flag.c8`, `ch6-done.c9`. **Withdrawn**
(`checks.record()`; not measured, not committed).

Proposition: "A processor running from Fast memory or cache can continue
executing while the blitter is denied Chip-memory access" / "may be
running completely uninhibited out of Fast memory or its internal cache
and continue to have memory cycles."

A stimulus was built and run, not committed: it enabled the design's
Zorro II fast-RAM path (`cpu_wrapper.v`'s `sel_z2ram`, turned on by one
register write at $E80048 -- independent of `Config.turbo_config`/the CPU
cache and of the overlay), copied a counting loop there, and ran it
concurrently with a nasty blit. The write decodes and the run completes
(it does not hang), but the elapsed time between the last chip-bus access
before the jump into fast RAM and the RESULTS store after the loop came
out identical, to the `clk_sys`, for a 50-iteration and a 2000-iteration
loop on fx68k -- a result the loop's own execution cannot explain.
Something about this measurement is not understood, so nothing is
reported from it either way, and no such program is in this package.

**Not tested; not a harness gap in the sense the first report used the
phrase.** The Zorro II fast-RAM path is reachable through the public
`Config`/harness surface (`cpu_wrapper.v`'s own decode); the step that
fails is establishing what the fixed span actually measures.

## Claims not asserted

| claim id(s) | page | reason |
|---|---|---|
| `6-intro.c5`, `ch6-intro.c5` | 169 | Factor not toleranced ("approximately twice as fast" has no numeric tolerance on the page); ordering asserted by ORDER above (a blitter A-to-D copy of 500 words, uncontended, finishes before a 68000 `MOVE.W` copy loop of the same 500 words). ORDER carries no claim id of its own -- only the ordering is graded, not the factor, so these ids stay withdrawn rather than moving onto ORDER's own label. |
| `6-blitter-done-flag.c9`, `ch6-done.c11`, `ch6-done.c8` | 186 | These claims are permissive ("may not return the correct result", "may not yet be set"), so no simulation outcome contradicts them; a deterministic Verilator run cannot falsify a "may". The mechanism the page actually gives (p.186's NOTE) is deterministic, not metastable: a blit just started can be locked out of its first DMA cycle by display fetches, so `DMAF_BLTDONE` has not yet gone high, while the CPU keeps running and can observe the not-yet-set flag. BBUSY-PREDICTS above demonstrates the deterministic half directly (`c10`/`c12`). |
| `6-blitter-done-flag.c8`, `ch6-done.c9` | 186 | See the withdrawn section above: attempted, not trustworthy, not committed. |
| `ch6-done.c10` | 186 | "Reading a chip memory or hardware register address with the processor before testing the bit avoids the problem" needs a case where the bit would otherwise have been misleading -- a blit locked out of its own first DMA cycle by display fetches (p.186's NOTE), not merely a large blit still running. BBUSY-PREDICTS's own fixture is the latter; the former is not built here. `6-blitter-done-flag.c10`/`ch6-done.c12` state the narrower "the [test after the read] returns the correct result", which BBUSY-PREDICTS does demonstrate. |
| `ch6-multi.c4` | 187 | See its own section above: `cpu_config`=3 was tried on TG68K and measured, producing no observable difference from `cpu_config`=0 in a no-DMA fixture; fx68k does not read `cpu_config` at all. The page's own consequence needs the CPU cache path too, which was not exercised. |
| `6-system-dma.c12`, `ch6-sysdma.c10` | 194 | "The two lowest priority levels" ranks the blitter and CPU against every other DMA channel (audio, disk, sprites, copper, bitplane), not just against each other. Confirming it needs all of those channels driven simultaneously against blitter and CPU, a materially larger fixture than this package's scope. The narrower fact -- the blitter outranks the CPU specifically -- is A4. Holds as a scope decision with a named fixture. |
| `ch6-sysdma.c35` | 196 | "Only if it is ready to ask for the cycle and is the highest priority item at the time" -- no assertion in this package measures readiness-and-priority as its own condition (A9 measures the outcome, not this qualifier). A fixture with a higher-priority requester (a nasty-mode blit) contending during the lores display, checking the CPU gets no slot in the fetch windows then, would settle it; not built here. |
| `6-system-dma.c51`, `6-system-dma.c52`, `ch6-sysdma.c26`, `ch6-sysdma.c27` | 196 | "About half" of instruction time is an average over the 68000's own full instruction mix, with no numeric tolerance given on the page. An instruction-timing histogram compiled per core would settle it. |
| `6-system-dma.c57`, `ch6-sysdma.c33` | 196 | The indivisible TAS cycle "will not fit into a DMA memory access slot" states a consequence without the page naming an observable difference between "did not fit" and "took a longer cycle instead" (which TAS's own assertion above grades directly, under `c56`). |
| `ch6-sysdma.c32` | 196 | "The 68000 test-and-set instruction (TAS) should never be used in the Amiga" is prescriptive advice to a programmer, not an RTL-observable claim; holds as written. |
| `ch6-sysdma.c31` | 196 | "Most instructions do not cause cycles to be missed" is a statistic over the instruction set with no numeric tolerance; holds as written (SYSDMA-C29 above establishes the *existence* of misses, per `c29`, not their rate). |

Total: 16 graded assertions (A1, DESC-OFFBY1, A2, A4, A8, SYSDMA-C29, A9,
A10, A11, A6, SLOTSPAN, OPEN50, PARITY, ORDER, TAS, BBUSY-PREDICTS), plus
unlabelled apparatus/precondition checks that carry no claim id (FIXTURE,
DESC-BASE, A3PRE, A4PRE, A9PRE, A10PRE). 3 claim ids withdrawn by
`checks.record()` as A5-WITHDRAWN. 1 claim id withdrawn as `ch6-multi.c4`.
2 claim ids withdrawn as the Fast-memory section. 15 claim ids withdrawn
in the table above.
