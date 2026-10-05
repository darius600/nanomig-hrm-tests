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
  - `BLTCON1` bit 1: descending-mode select (p.182, "Descending mode is
    turned on by setting bit one of BLTCON1").
  - `BPLCON0` ($DFF100): bit 15 `HIRES`, bits 14-12 the bitplane count.
  - `DIWSTRT`/`DIWSTOP` ($DFF08E/$DFF090), `DDFSTRT`/`DDFSTOP`
    ($DFF092/$DFF094): the standard low-resolution display window/fetch
    values, and the standard high-resolution `DDFSTRT`/`DDFSTOP` pair
    (narrower, since a high-resolution line fetches twice as often).
  - `BPL1PTH`/`BPL1PTL` ($DFF0E0/$DFF0E2) and the following bitplane
    pointer pairs, each 4 bytes apart.
  - `COP1LCH`/`COP1LCL` ($DFF080/$DFF082) and `COPJMP1` ($DFF088):
    the Copper's own first-list pointer and its strobe (FASTRUN's and
    FASTRUN-LOCKOUT's own stimuli, below, use these to start a blit
    without any CPU chip-bus write of its own).
  - `COPCON` ($DFF02E) bit 1 `CDANG` ("Copper danger bit"). p.26: the
    registers from $40 to $7E "are protected by a bit in the Copper
    control register ... When CDANG is 0, these registers cannot be
    written by the Copper. When CDANG is 1, these registers can be
    written by the Copper." Appendix B (p.289) marks `BLTSIZE` ($058)
    `~058`; the legend (p.287): "Address not writable by the Copper
    unless the 'copper danger bit', COPCON is set true." FASTRUN's and
    FASTRUN-LOCKOUT's own stimuli set it so the Copper's own list can
    reach `BLTSIZE`.
- One colour clock (one memory slot, p.194) is the unit of every slot count
  below. Its length in `clk_sys` cycles is derived in the run, from Figure
  6-9's refresh pattern, and asserted there (SLOT-UNIT below); it is not an
  input of this specification.
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

## Slot unit (no claim id)

### SLOT-UNIT [Figure 6-9, p194] four refresh slots on every other memory cycle

No claim id (an apparatus check: every slot-dependent line below counts in
this unit).

Proposition: p.194 lists "4 cycles for memory refresh" per horizontal line
and Figure 6-9 draws them on every other memory cycle (slots 0, 2, 4 and 6
of the line's own refresh block). Two refresh accesses of one line are
therefore two slots apart, and the smallest positive gap between two
refresh accesses of a run is two slots.

Program: `prog_sharing.s`, `BASELINE` build (no DMA channel enabled): its
only RAM-port accesses are the 68000's own (program fetches in
$080400-$0805FF, the scratch write, the reset-vector reads from the overlay
window) and refresh, which no DMACON bit switches off; every other read is
refresh, by exclusion.

Observed: the refresh reads grouped into lines (a gap over eight times the
smallest gap starts a new group); for every group its size and the offsets
of its accesses from its first.

Derived: the slot unit, half the smallest positive gap between two refresh
accesses.

Expected value: every group holds exactly four accesses, at offsets of 0, 2,
4 and 6 slot units, and at least one group is found (a group the capture
ended inside is not a line and is left out). The modal line period is
printed as a measurement and graded by nothing: p.194 gives "227.5" colour
clocks per line, an average of lines of different lengths.

Mutation (`NMH_MUTATE=SLOT-UNIT`): the capture the comparator reads is the
four-low-resolution-bitplane build, whose bitplane DMA adds reads to the
refresh reads, so that its groups are not four refresh slots per line; the
unit stays the baseline's. The unchanged "every group is four accesses at
0, 2, 4 and 6 units" check must go red.

If the unit cannot be derived or the pattern fails, every slot-dependent
line (A8, SYSDMA-C29, A9, A10, A11, A6, OPEN50, PARITY-REF, PARITY) prints
`NOT GRADED` naming SLOT-UNIT.

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

Proposition: "Descending mode is turned on by setting bit one of BLTCON1
(defined as BLITREVERSE)" and "This differs from 680x0 predecrement,
where an address register would point to the word after the last rather
than the last word" (p.182) -- in the blitter's own descending mode, the
pointer registers must be preloaded with the address of the block's LAST
word, not the address one word past it (the 680x0 `-(An)` convention).
`BLTCON1` bit 1 itself is cited from p.182, not from the RTL;
`prog_descblit.s`'s own use of it is a stimulus detail, not the oracle.

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

Mutation (`NMH_MUTATE=DESC-BASE`): the capture is
`prog_descblit_offby1.bin` (one word past the last, below) instead; the
unchanged "faithful, sentinel untouched" check must go red (the first
word goes unwritten and the sentinel is written instead).

Observed (claim): which of $0C2100/$0C2102/$0C2104/$0C2106/$0C2108/$0C210A
land a write in `ram_log()`, comparing the blitter's own convention
(capture A, `prog_descblit.bin`) against `prog_descblit_offby1.s`
(`OFFBY1` set, capture B: `BLTAPTH`/`BLTDPTH` at `DB_SRC+8`/`DB_DST+8`,
one word past the last -- the 680x0 `-(An)` convention).

Expected value: capture A writes $0C2100-$0C2106 and leaves $0C2108
alone; capture B writes exactly $0C2102-$0C2108 and leaves both $0C2100
and $0C210A unwritten -- the same 4-word transfer displaced by exactly
one word, not merely "some" displacement (a pointer two words past the
last would also leave $0C2100 unwritten and write beyond $0C2108, and
must be told apart from one word past).

Mutation (`prog_descblit_offby2.s`, `OFFBY2` set,
`NMH_MUTATE=DESC-OFFBY1`): capture B is replaced with a pointer two words
past the last (`DB_SRC+10`/`DB_DST+10`); it writes $0C2104-$0C210A
instead, so the unchanged "exactly $0C2102-$0C2108, neither $0C2100 nor
$0C210A" check must go red.

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

No claim id (an apparatus precondition of A3 above, not a claim of its
own -- `ch6-multi.c3` stays on A3 alone).

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

### A2 [p169,p186,p198] the CPU keeps running while the blitter works

Claims: `6-intro.c20`, `ch6-intro.c21`, `ch6-intro.c22`,
`6-blitter-done-flag.c2`, `6-blitter-done-flag.c3`, `ch6-done.c2`,
`ch6-done.c3`, `ch6-sysdma.c53`.

Proposition: the blitter is an asynchronous coprocessor; the 68000 is not
halted or blocked while a blit is in progress -- existence only, at least
one CPU write strictly inside the blit's active window. p.198: "There are
certain cases, however, when the blitter and the 68000 can share memory
cycles" (`ch6-sysdma.c53`): a share-mode blit's active window in which the
CPU also makes chip-bus writes is such a case.

Program (`prog_sharing.s`, `SHARE` build): a tight CPU loop increments a
counter and stores it to the scratch word on every pass, concurrently
with a large, continuously-demanding blit (`BLTPRI`=0) sized to run for
many loop iterations.

Observed: every scratch-word write in `ram_log()` whose cycle falls
strictly between the blit's first and last destination writes (its
active window, both addresses distinct from the scratch address) --
excluding the window's own endpoints, not just falling inside `[lo,hi]`.

Expected value: existence only -- at least one such write. p.169 ("the
680x0 CPU continues to run as the blit is executing") and p.186 ("they
can both work concurrently") give no bound on how long the CPU may then
wait between two chip-bus writes; p.198's own release rule (three denied
cycles, one released) is what governs that, withdrawn below (A5-WITHDRAWN).
A bound invented here (a fraction of the window, a multiple of the median
gap) would not be from the page.

Measured: 48 scratch writes strictly inside the window on fx68k, 50 on
TG68K.

Mutation (`prog_sharing_tiny.s`, `SHARE` and `TINY` both set,
`NMH_MUTATE=A2`): the blit shrinks to a near-instant one-word transfer;
the unchanged check must go red (the active window collapses to zero
length, so no write can fall strictly inside it).

### A4 [p198] with DMAF_BLITHOG set the blitter keeps the bus

No claim id: the inventory's claims on the blitter's priority over the 68000
say "normally" (A4-SHARE below).

Proposition: p.198: "If DMAF_BLITHOG is a 1, the blitter will keep the bus
for every available Chip memory cycle." The register bit is `BLTPRI` of
`DMACON` (the "blitter-nasty" bit).

Program (`prog_sharing.s`, `NASTY` build): the same CPU scratch-counter
loop as A2, `BLTPRI`=1, a large continuously-demanding blit.

Observed (precondition, its own id, "A4PRE"): the blit's active window
spans enough cycles to have contained several CPU-loop iterations had the
CPU been let through -- guards against a vacuously short window.

Observed (proposition): the count of scratch-word writes inside the active
window.

Expected value: precondition holds, and the count is exactly zero. If the
precondition fails the line prints `NOT GRADED`.

Mutation (`prog_sharing_share.s`, `NMH_MUTATE=A4`): swaps in the
share-mode capture for this same check; the unchanged "count is zero"
check must go red (share mode lets some through).

Precondition mutation (`prog_sharing_nasty_tiny.s`, `NASTY` and `TINY` set,
`NMH_MUTATE=A4PRE`): the nasty-mode blit shrinks to a near-instant one-word
transfer; the unchanged "window long enough" check must go red, and A4
prints `NOT GRADED`.

### A4-SHARE [p194,p198] the blitter normally has a higher priority than the 68000

Claims: `6-system-dma.c13`, `ch6-sysdma.c11`, `6-blitter-priority.c1`,
`ch6-sysdma.c52`.

Proposition: p.194 "The blitter is given the higher priority because it
performs data copying, modifying, and line drawing operations operations
much faster than the 68000"; p.198 "the blitter normally has a higher
priority than the processor for DMA cycles". With `BLTPRI` clear (share
mode, "normally") a CPU that is continuously ready loses cycles to the
blitter: a 68000 with priority over the blitter would be served first in
every contested slot and keep the rate it has with the bus to itself.

Program: `prog_sharing.s`, `SHARE` build (the share-mode blit's active window
with the CPU scratch-counter loop running), against `BASELINE` (the same
loop, no DMA, the bus to itself).

Observed (precondition, "A4-SHARE-PRE", no claim id): the share window is
nonempty and the reference run holds CPU accesses.

Observed (proposition): the count of the CPU's chip-bus accesses (program
fetches and scratch writes) per `clk_sys` inside the share-mode blit's
active window, against the same count over the BASELINE run from its first
to its last CPU access. The two rates are compared by cross multiplication,
so no division and no threshold enters.

Expected value: the in-window rate is strictly lower than the free rate.

Mutation (`NMH_MUTATE=A4-SHARE`): the reference rate is taken from the
nasty-mode blit's own active window (`prog_sharing_nasty.s`, where the CPU
makes two accesses) instead of the BASELINE run; the unchanged "strictly
lower" check must go red.

Precondition mutation (`prog_sharing_tiny.s`, `NMH_MUTATE=A4-SHARE-PRE`): the
share-mode blit shrinks to a near-instant one-word transfer, so its active
window is empty; the precondition goes red and A4-SHARE prints `NOT GRADED`.

### A5-WITHDRAWN [p198] share mode: the blitter releases the bus after three unsatisfied CPU cycles

Claims: `6-blitter-priority.c13`, `ch6-sysdma.c62`. **Withdrawn**
(`checks.record()`, not `expect()` -- these ids are not on a graded line; no
mutation).

p.198: "If the 68000 is unsatisfied for three consecutive memory cycles, the
blitter will release the bus for one cycle." p.194 defines a memory cycle as
one colour clock (one slot). The sentence is explicit; what this boundary
lacks is its antecedent. A 68000 is unsatisfied from the cycle in which it
raises a bus request, and the RAM-port log shows only the cycles in which an
access happened, never the cycle in which a request was raised and denied.
Where the count of consecutive unsatisfied cycles starts, and so when the
release is due, cannot be located from the log, and `nanomig.v` has no CPU
bus-request port to read it from. The modal spacing of consecutive CPU
accesses inside the share-mode window is printed as a measurement and graded
by nothing.

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
pointer) -- 4 per line, 2 slots apart, matching Figure 6-9's own refresh
block. Every low read is grouped (a gap over 100 `clk_sys` starts a new
group), but not every such group is a line's own refresh: in the
bitplane builds, BPL2-4's own pointers are left unset and so also read
low memory, in groups of a different size (60 and 48, not 4) and
spacing. Only a group that is exactly four reads, each two slots (16
`clk_sys`) after the one before, is kept as a line's own start; any other
group is some other master's low read, not refresh, and does not anchor
a line. PARITY-REF's own precondition below, not an unproven assumption,
is what confirms the reads this restricted grouping keeps still land on
the real refresh schedule. A CPU chip-bus access is graded even/odd by
its own slot offset from *its own line's* last refresh read,
floor-divided by one slot (8 `clk_sys`) -- not by its raw remainder,
since a CPU sample and a DMA (refresh) sample are each taken at a fixed
sub-slot phase 4 `clk_sys` apart, so their difference is never an exact
multiple of a slot, only its floor is meaningful. This is a fact about
this RTL's own two sample phases, not a page citation.

### PARITY-REF [Figure 6-9] the per-line reference's own anchor

No claim id (an apparatus precondition for A8, SYSDMA-C29 and PARITY
below, not a claim of its own).

Proposition: Figure 6-9 draws refresh on odd-numbered slots, with the
refresh block's own last read at $05, and the display-fetch region
starting at $38/$40 with plane 1 fetched *last* in its own repeating
4-slot group (Figure 6-12); in the four-lores-plane build, $05 + 58
slots = $3F, exactly where Figure 6-9 puts plane 1 (Figure 6-11's own
8-slot group also ends there). Every BPL1 fetch of that build must
therefore classify odd by the same per-line reference A8/PARITY use --
this is what tells a reference that correctly picks out the "even" phase
apart from one that swapped it (a swapped reference would call BPL1
"even" instead, not merely reshuffle some fraction of the accesses).

Program: `prog_bitplane.s`, `PLANES=4 HIRES=0` build (A9's own capture).

Observed: every BPL1-range read's own slot parity, by the same per-line
reference A8 and PARITY use.

Expected value: at least one such read is found, and every one classifies
odd.

Mutation (`NMH_MUTATE=PARITY-REF`): the capture is `prog_sharing.bin` (no
bitplane DMA enabled at all); the unchanged "found, and all odd" check
must go red (zero BPL1 reads at all).

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
live, nonzero population -- 50178 pairs on fx68k, 60672 on TG68K) through
the same, unchanged comparator (including its `pairs > 0` guard); the
check reads GREEN there on both cores, showing the comparator is not
hard-wired to fail.

Second mutation (`NMH_MUTATE=A8-SHARE`, since fx68k is already red at
baseline and TG68K is not): the baseline capture is replaced with the
share-mode blit's own active-window CPU accesses (the same population
PARITY's own share-mode window uses), through the same, unchanged
comparator. This population's grants sit 5 slots apart (see the
A5-WITHDRAWN halt below), an odd number, so the check reads RED on both
cores -- including TG68K, for which no other mutation here does.

### SYSDMA-C29 [p196] some 68000 instructions miss the even-cycle allocation

Claim: `ch6-sysdma.c29`.

Proposition: "Some 68000 instructions do not match perfectly with the
allocation of even cycles and cause cycles to be missed. If cycles are
missed, the 68000 must wait until its next available memory slot before
continuing." p.196's own consequence of a missed cycle is a *wait*, not
a landing on the wrong slot -- that wait is only observable where the
page's allocation actually holds the odd slots against the CPU (the
four-lores-plane fetch window, Figure 6-11: planes on the odd slots),
not the free-running no-DMA baseline, where the same fractional-slot
instruction simply lands on the odd slot instead (A8's and PARITY's own
red, a different fact).

Program: `prog_bitplane.s`, `PLANES=4 HIRES=0` build (A9's own capture).
Its counting loop is `ADDQ.W #1,D0` / `MOVE.W D0,(A1)` / `BRA.S .loop`;
`BRA.S` is a 10-clock-period internal operation (MC68000 timing tables),
not a whole multiple of 4, unlike every other instruction in the loop.

Observed: among the same in-window CPU accesses A8's own mutation uses
(`lores_inwindow_cpu_cycles`), the count of consecutive-pair gaps that
are exactly 4 slots (32 `clk_sys`) apart -- the specific, recurring step
from the fetch after `BRA.S`'s own internal operation to the next fetch.

Expected value: fx68k -- at least one such 4-slot step (existential, no
threshold set by a run). Not exercised on TG68K, whose own loop has no
fractional-slot internal operation and so produces no such step.

Measured: 9984 of 9984 in-window cases on fx68k are exactly this 4-slot
step (every one of them, since the step recurs once per loop iteration
throughout the run). TG68K: not exercised.

Mutation (`prog_bitplane_aligned.s`, `ALIGNED` set,
`NMH_MUTATE=SYSDMA-C29`): the loop's `BRA.S` is replaced with `JMP (An)`
(8 clock periods, a whole multiple of 4, MC68000 timing tables) -- every
instruction's own bus-cycle timing then aligns to whole slots, inside
the window the same as outside it. The unchanged "at least one 4-slot
step" check must go red on fx68k (zero such steps).

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

### SLOTSPAN-WITHDRAWN [p196] a 68000 cycle is one memory slot plus one free slot -- withdrawn

Claim: `6-system-dma.c58`. **Withdrawn** (`checks.record()`; measured, not
graded; no mutation).

Figure 6-10 divides an average 68000 cycle into an internal-operation
portion and a memory-access portion of equal length. The RAM port records
one sample per CPU bus access, at the moment of the access: an access's own
duration (the memory-access portion) and the internal-operation portion are
not separately observable, only their sum, the interval between two accesses,
and "average" ranges over an instruction mix the page does not give. The
modal interval between consecutive CPU accesses of the `BASELINE` loop is
printed as a measurement and graded by nothing.

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

Mutation (`NMH_MUTATE=PARITY`, `stimulus green`): PARITY is red on both cores
unmutated. Both windows are replaced with a live input on which the same,
unchanged comparator is proven to read GREEN -- the four-lores-plane
build's own in-fetch-window CPU accesses, all on even slots by the same
reference -- and the check reads green on both cores; the comparator is not
hard-wired to fail.

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

Mutation (`prog_copyloop_tiny.s`, `CL_SHORT` set, `NMH_MUTATE=ORDER`): the
CPU copy loop shrinks to 50 words -- a nonzero span shorter than the
blit's own, not a zero-length one that would fail by construction -- so
it finishes before the blit; the unchanged "blit finishes first" ordering
check must go red.

### TAS-WITHDRAWN [p196] TAS's read-modify-write cycle -- withdrawn

Claim: `6-system-dma.c56`. **Withdrawn** (`checks.record()`; measured but
not graded; carries no assertion and no mutation).

p.196's NOTE: "The 68000 test-and-set instruction (TAS) should never be
used in the Amiga; the indivisible read-modify-write cycle that is used
only in this instruction will not fit into a DMA memory access slot."
This states a fact about the 68000's own bus protocol (its two halves
cannot be split across ordinary DMA slot boundaries the way an
ordinary read and an ordinary write can) and a consequence for
programmers ("should never be used"), not a bound on what the chipset
must do between TAS's own read and write: the NOTE names no
chipset-observable requirement that other masters be held off, and the
DMA contention it warns about is the premise of the warning, not
something it forbids.

Program (`prog_tas_share.s`): a continuously-demanding share-mode blit
(`BLTPRI` clear, so the blitter keeps requesting cycles throughout) runs
concurrently with one `TAS.B` on a known byte.

Measured: in `ram_log()`, filtered to the one address TAS touches and
coalesced (`nmh::coalesce()`), the read-to-write gap and the count of any
other RAM-port access whose cycle falls strictly between that read and
that write. fx68k: 5 such accesses, of which 2 are re-samples of TAS's
own read at the same address (only 3 are another master's: a blitter
A-read, D-write, A-read). TG68K: 4, all another master's.

**Withdrawn, not a chipset finding.** Every intervening access is a
chipset-granted blitter slot the CPU's own outstanding read-modify-write
happens to span -- exactly the DMA contention the page's warning
presupposes, not a violation of it. The count is not reachable at zero
on fx68k even with no DMA at all: its own TAS read alone spans 3 slots
there, so the comparator could not go green either way, and the claim
concerns the 68000's own bus protocol rather than the chipset boundary
this suite tests.

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

### FASTRUN [p186] Fast memory keeps running during a nasty blit

Claim: `ch6-done.c9`.

Proposition: "may be running completely uninhibited out of Fast memory or
its internal cache and continue to have memory cycles." This claim sets
no condition on the blitter itself (unlike `6-blitter-done-flag.c8`,
graded separately below by FASTRUN-LOCKOUT): a nasty blit that denies the
CPU all Chip-memory access (A4) is a fair, and simpler, case for it, so
`c9` stays on this nasty-blit stimulus rather than moving to
FASTRUN-LOCKOUT's own display-locked-out one.

Program (`prog_fastrun.s`, 50 iterations; `prog_fastrun_long.s`, `FR_LONG`
set, 2000 iterations): enables the design's Zorro II fast-RAM path
(`Config.fastram_config=0x01`; `cpu_wrapper.v:117 sel_z2ram`, turned on by
one register write at $E80048, `cpu_wrapper.v:640`), relocates a counting
loop there, initializes its own fast-RAM counter, arms a nasty blit (500
words, the same span A4/ORDER establish -- `BLTCON*`, `BLTAPTH`/`BLTDPTH`,
`DMACON` SET `DMAEN,BLTEN,BLTPRI`) and points the Copper's own list at a
single `MOVE` to `BLTSIZE` followed by a WAIT-forever, all from chip RAM,
then jumps to the relocated loop. `COPCON`'s `CDANG` bit is set first
(`$0002`, p.26/Appendix B, see "Shared setup" above). p.30: "you must
initialize one of the Copper location registers (COP1LC or COP2LC) and
write to its strobe address before Copper DMA is turned on. This ensures
a known start address and known state." `COPJMP1` is therefore strobed
from chip RAM, with `COPEN` still clear; `DMACON`'s `COPEN` bit, set from
the relocated loop's own first instruction in fast RAM, is the one event
that starts the Copper running the already-loaded list. Enabling `COPEN`
from fast RAM is the CPU's own deliberate, occasional chip-bus write while
"running from Fast memory or cache" and "continu[ing] to have memory
cycles" (p.186); everything else in the loop (`addq.w #1,(a2)` /
`dbra d0,.loop`) touches only fast RAM. The loop returns through
`jmp (a3)`, the return address loaded into `a3` before the jump into fast
RAM: a direct `jmp` to the instruction immediately following it is what
vasm deletes at assembly time, since the branch target and the
fall-through address coincide; an indirect jump through a register is not
collapsed the same way.

Fast-RAM activity is only visible on `nanomig.v`'s own
`fastram_sel`/`fastram_addr`/`fastram_wr` ports, never on `ram_log()`
(which only samples the physical chip-RAM port): the test steps the
harness one `clk_sys` cycle at a time through its own public
`run_cycles(1)` and reads `ports()` after each step, exactly as the
harness's own comment on `ports()` permits.

Observed: the blit's own active window (first to last destination write,
the same yardstick A2/A4/ORDER use); the fast-RAM counter's own writes
that fall strictly inside it; whether each capture reaches `DONE` with
`RESULTS` holding its own iteration count; the modal (most frequent) gap
between consecutive counter writes, taken separately strictly inside the
window and at or after its own end (the same in-window/no-DMA split A6
makes), from the 2000-iteration capture, which outlasts the window on
both sides.

Expected value: at least one fast-RAM counter write strictly inside the
blit's own active window; both the 50- and the 2000-iteration capture
reach `DONE` with `RESULTS` equal to their own iteration count; and every
counter-write gap strictly inside the window equals the modal
after-window period -- "completely" graded as every gap, not a
mode-to-mode comparison (which would also pass a loop slowed in only some
of its in-window iterations), and not a count or bound the page does not
state.

Measured: on fx68k, the blit's own active window spans 2276281..2284425
(8144 `clk_sys`); the fast-RAM counter's own writes strictly inside it are
49 of 50 (50-iteration build) and 75 (of 2000, 2000-iteration build -- the
loop outlasts the window, as designed); all 74 in-window gaps equal 108
`clk_sys`, the same period measured after the window (1923 gaps). On
TG68K: window 2276057..2284169 (8112 `clk_sys`); 48 of 50 and 127 of 2000
strictly inside; all 126 in-window gaps equal 64 `clk_sys`, the same
period measured after the window (1870 gaps). Both builds reach `DONE`
with `RESULTS` equal to their own iteration count (50 and 2000) on both
cores.

Mutation (`NMH_MUTATE=FASTRUN`): `prog_fastrun_chipcode.s` /
`prog_fastrun_long_chipcode.s` (`FR_CHIPCODE` set) -- only `FR_CODE` moves
to chip RAM ($0C8000); `FR_CNT` stays in fast RAM; `Config.fastram_config`
stays 1 throughout (the mutation substitutes a different binary, not this
setting). A4 (`BLTPRI` set denies the CPU every chip-bus cycle, fetches
included, for the whole of the blit's active window) predicts zero
counter writes there, since the CPU cannot even fetch its own next loop
instruction while the window holds. Measured: on fx68k, both builds, 0
counter writes strictly inside the window (window 2276217..2284329), so
there are no in-window gaps to compare, against an after-window period of
104 `clk_sys` (1998 gaps) -- red through both the existence and the
every-gap clause. On TG68K, both builds, 1 counter write strictly inside
the window (window 2276185..2284297): the existence clause alone stays
green, but one write forms no in-window gap either, against an
after-window period of 80 `clk_sys` (1997 gaps) -- there the every-gap
clause alone is what goes red.

### FASTRUN-LOCKOUT [p186,p197] The blitter locked out of Chip memory by display fetches

Claim: `6-blitter-done-flag.c8`.

Proposition: "If a blit has just been started but has been locked out of
memory access because of, for instance, display fetches, this bit may not
yet be set. The processor, on the other hand, may be running completely
uninhibited out of Fast memory or its internal cache, so it will continue
to have memory cycles." Unlike FASTRUN above (a nasty blit with no
display, where the blitter itself holds the bus and the CPU is the
locked-out party), this reaches the page's own case: the blitter itself
locked out, by the display.

Program (`prog_fastrun_lockout.s`): enables the same Zorro II fast-RAM
path as FASTRUN and relocates a counting loop there (`FR_ITERS`=8000, kept
running well past the interval below); sets up a four-high-resolution-
bitplane display (`prog_bitplane_hires.s`'s own `DIWSTRT`/`DIWSTOP`/
`DDFSTRT`/`DDFSTOP`/`BPLCON0` values -- A10's own build, whose fetch
windows claim every one of their 160 slots, A10PRE); arms a nasty blit
(the same 500-word span as FASTRUN) with `BLTSIZE` left unwritten; sets
`CDANG`; then spin-reads `VHPOSR` from chip RAM until the beam's own line
is confirmed inside the display's active vertical area. The Copper's own
list is then built: a `WAIT` whose own `VP` target is 20 lines past the
line the spin just confirmed (so the `{VP,HP}` comparison -- taken as one
concatenated value, p.23 -- cannot already be true the moment the Copper
first checks it) and whose `HP` target is $28, in the page's own notation
(p.22/p.23; the same color-clock unit as `DDFSTRT`, p.60); four `MOVE`s to
`COLOR00` (a harmless, otherwise-unused write); then the real
`MOVE #$7D01,BLTSIZE`; then `WAIT`-forever. `COPJMP1` is strobed from chip
RAM with `COPEN` still clear (p.30, as FASTRUN); `COPEN`, set from fast
RAM as the loop's own first instruction, is the one start event -- so the
counting loop is already running, and has been for however long the
Copper's own 20-line-ahead `WAIT` takes to release, before the Copper (and
through it, the blit) is allowed to move at all.

`HP`=$28 (40) is 20 color clocks before `DDFSTRT`=$3C (60): measured, the
Copper's first fetch after the `WAIT` lands exactly 20 slots before the
fetch window that follows -- wider than the blitter's own natural
first-access latency (measured ~6 color clocks after the Copper's own
data-word fetch, on an otherwise identical but unobstructed capture), so a
blit armed right there would still get its first access before the window
opens. The four `MOVE`s to `COLOR00` close most of that margin: Figure
6-9's own note gives each Copper Data Move instruction 4 slots, so four of
them consume 16 color clocks, leaving 2 -- inside the blitter's own
natural latency, so its first attempt lands after the window has already
claimed the bus. The number of fill `MOVE`s (four) was chosen on this RTL
to land `BLTSIZE` there; FASTRUN-LOCKOUT-PRE, below, is what checks the
placement.

The Copper's own write to `BLTSIZE` (a custom register) is not itself on
the physical RAM port, the same as `COPJMP1`; the Copper's own fetch of
the `MOVE`'s data word (a plain chip-RAM read, `COP_LIST+22`) is, and
bounds the `BLTSIZE` write from below by at most the Copper's own
single-slot `MOVE`-execution latency. The blitter's own first DMA access
(its first A-read, in `BLIT_SRC`'s range, or its first D-write if that
happens to be logged first) is a physical RAM-port access throughout.

Observed (precondition, "FASTRUN-LOCKOUT-PRE", no claim id): the interval
from the Copper's own data-word fetch to the blitter's own first DMA
access, against the display's own per-line fetch windows (A10's own
construction).

Expected value (precondition): that interval fully contains one such
window (the window's own `lo` at or after the data-word fetch, its own
`hi` at or before the blitter's own first access) -- p.197's own lockout,
not this package's invention: if it goes red, it stays red.

Observed (claim): the fast-RAM counter's own writes strictly inside the
display's own fetch window -- not the wider Copper-to-blitter interval:
p.186's own condition ("locked out ... because of ... display fetches")
holds only inside that window, not in the slots before or after it;
whether the capture reaches `DONE` with `RESULTS` equal to its own
iteration count (8000).

Expected value: precondition holds, and at least one fast-RAM counter
write falls strictly inside the fetch window. No count, ratio or bound
beyond that -- the page states only that the CPU continues to have memory
cycles, not how many.

Measured, identical on both cores: the Copper's own data-word fetch at
cycle 2401033; the display's own fetch window 2401049..2402321 (1272
`clk_sys`, fully inside the Copper-to-blitter interval); the blitter's own
first DMA access at cycle 2402361 (40 `clk_sys` after the window's own end
-- its first attempt was denied throughout the window). Fast-RAM counter
writes strictly inside the fetch window: 12 on fx68k, 20 on TG68K (both
cores' own loop periods differ, as elsewhere in this package). Both cores
reach `DONE` with `RESULTS`=8000.

Mutation (`NMH_MUTATE=FASTRUN-LOCKOUT`): `prog_fastrun_lockout_chipcode.s`
(`FR_CHIPCODE` set) -- only `FR_CODE` moves to chip RAM ($0C0800; not
$0C8000, which falls inside `BP_PTR`'s own $0C6000-$0D6000 heuristic range
and would corrupt this capture's own window detection); `FR_CNT` stays in
fast RAM. The window is entirely DMA (A10PRE: p.197's own "40 * 4 = 160
time slots", none left over), so no CPU access is possible anywhere
strictly inside it: zero counter writes predicted there. Measured, both
cores: the Copper's own data-word fetch, the fetch window and the
blitter's own first DMA access all land at the same cycles as the
unmutated capture (2401033, 2401049..2402321, 2402361) -- FASTRUN-LOCKOUT-
PRE still holds -- and the relocated chip-RAM loop has 0 counter writes
strictly inside the window, on both cores. The claim goes red on both
cores, through its own existence clause.

Mutation (precondition, `NMH_MUTATE=FASTRUN-LOCKOUT-PRE`):
`prog_fastrun_lockout_2plane.s` (`FR_2PLANE` set) -- the same lockout
program with two high-resolution bitplanes (`BPLCON0`=$A000) instead of
four, the same `DDFSTRT`/`DDFSTOP`. Figure 6-9's own double dagger ("These
operations only take slots if the associated operation is being
performed") together with p.197's own full lockout, stated only for four
hi-res planes ("40 * 4 = 160 time slots"), predict that halving the plane
count frees half the window's slots, into one of which the blitter's own
first access can fall. Measured, identical on both cores: the Copper's own
data-word fetch and the fetch window land at the same cycles as the
unmutated capture (2401033, 2401049..2402321 -- `BPL1`'s own fetch cadence
is unaffected by the plane count); the blitter's own first DMA access now
falls at cycle 2401113, inside the window instead of 40 `clk_sys` after
it. The precondition goes red on both cores.

## Claims not asserted

| claim id(s) | page | reason |
|---|---|---|
| `6-intro.c5`, `ch6-intro.c5` | 169 | Factor not toleranced ("approximately twice as fast" has no numeric tolerance on the page); ordering asserted by ORDER above (a blitter A-to-D copy of 500 words, uncontended, finishes before a 68000 `MOVE.W` copy loop of the same 500 words). ORDER carries no claim id of its own -- only the ordering is graded, not the factor, so these ids stay withdrawn rather than moving onto ORDER's own label. |
| `6-blitter-done-flag.c9`, `ch6-done.c11`, `ch6-done.c8` | 186 | These claims are permissive ("may not return the correct result", "may not yet be set"), so no simulation outcome contradicts them; a deterministic Verilator run cannot falsify a "may". The mechanism the page actually gives (p.186's NOTE) is deterministic, not metastable: a blit just started can be locked out of its first DMA cycle by display fetches, so `DMAF_BLTDONE` has not yet gone high, while the CPU keeps running and can observe the not-yet-set flag. BBUSY-PREDICTS above demonstrates the deterministic half directly (`c10`/`c12`). |
| `ch6-done.c10` | 186 | "Reading a chip memory or hardware register address with the processor before testing the bit avoids the problem" needs a case where the bit would otherwise have been misleading -- a blit locked out of its own first DMA cycle by display fetches (p.186's NOTE), not merely a large blit still running. BBUSY-PREDICTS's own fixture is the latter; the former is not built here. `6-blitter-done-flag.c10`/`ch6-done.c12` state the narrower "the [test after the read] returns the correct result", which BBUSY-PREDICTS does demonstrate. |
| `ch6-multi.c4` | 187 | See its own section above: `cpu_config`=3 was tried on TG68K and measured, producing no observable difference from `cpu_config`=0 in a no-DMA fixture; fx68k does not read `cpu_config` at all. The page's own consequence needs the CPU cache path too, which was not exercised. |
| `6-system-dma.c12`, `ch6-sysdma.c10` | 194 | "The two lowest priority levels" ranks the blitter and CPU against every other DMA channel (audio, disk, sprites, copper, bitplane), not just against each other. Confirming it needs all of those channels driven simultaneously against blitter and CPU, a materially larger fixture than this package's scope. The narrower fact -- the blitter outranks the CPU specifically -- is A4. Holds as a scope decision with a named fixture. |
| `ch6-sysdma.c35` | 196 | "Only if it is ready to ask for the cycle and is the highest priority item at the time" -- no assertion in this package measures readiness-and-priority as its own condition (A9 measures the outcome, not this qualifier). A fixture with a higher-priority requester (a nasty-mode blit) contending during the lores display, checking the CPU gets no slot in the fetch windows then, would settle it; not built here. |
| `6-system-dma.c51`, `6-system-dma.c52`, `ch6-sysdma.c26`, `ch6-sysdma.c27` | 196 | "About half" of instruction time is an average over the 68000's own full instruction mix, with no numeric tolerance given on the page. An instruction-timing histogram compiled per core would settle it. |
| `6-system-dma.c57`, `ch6-sysdma.c33` | 196 | The indivisible TAS cycle "will not fit into a DMA memory access slot" states a consequence without the page naming an observable difference between "did not fit" and "took a longer cycle instead" -- and `c56` itself, which would have graded that most directly, is withdrawn (see TAS-WITHDRAWN above). |
| `ch6-sysdma.c32` | 196 | "The 68000 test-and-set instruction (TAS) should never be used in the Amiga" is prescriptive advice to a programmer, not an RTL-observable claim; holds as written. |
| `ch6-sysdma.c31` | 196 | "Most instructions do not cause cycles to be missed" is a statistic over the instruction set with no numeric tolerance; holds as written (SYSDMA-C29 above establishes the *existence* of misses, per `c29`, not their rate). |

Total: 18 graded assertions (A1, A3, DESC-OFFBY1, A2, A4, A8, SYSDMA-C29,
A9, A10, A11, A6, SLOTSPAN, OPEN50, PARITY, ORDER, BBUSY-PREDICTS,
FASTRUN, FASTRUN-LOCKOUT), carrying 39 claim ids in full, plus unlabelled
apparatus/precondition checks that carry no claim id (FIXTURE, DESC-BASE,
A3PRE, A4PRE, A9PRE, A10PRE, PARITY-REF, FASTRUN-LOCKOUT-PRE). 3 claim
ids withdrawn by `checks.record()` as A5-WITHDRAWN. 1 claim id withdrawn
as TAS-WITHDRAWN. 1 claim id withdrawn as `ch6-multi.c4`. 17 claim ids
withdrawn in the table above. 39 + 17 + 3 + 1 + 1 = 61 claim ids checked
in total.
