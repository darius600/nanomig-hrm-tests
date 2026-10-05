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
  access. The counting loop of `prog_sharing.s` and `prog_bitplane.s` sits at
  the same address ($080500) in every build of both, so that the same
  instruction boundary has the same pair of addresses in every capture; a
  setup shorter than $100 bytes precedes it and jumps to it.
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
  - `INTENAR` ($DFF01C), the read of the interrupt enables (BBUSY-LOCKOUT's
    mutation reads it instead of `DMACONR`; its bit 14 is `INTEN`, which no
    program here sets).
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
  comparator reading GREEN on a different, proven-live input (PARITY,
  SYSDMA-C29 on TG68K). A mutation of the design under test (`implementation`)
  is a private copy of an RTL file outside the repository, built into a
  separate binary; the repository's own RTL is never changed.
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

### A1 [p170] pointer registers: adjacent words, one 32-bit write

Claims: `6-dma-channels.c16`, `6-dma-channels.c18`, `ch6-dma.c14`,
`ch6-dma.c16`.

Proposition: p.170: "The address pointer registers are each composed of two
words, named BLTxPTH and BLTxPTL ... The two words of each register are
adjacent in the 68000 address space, with the high address word first, so
they can both be written with one 32-bit write from the processor." x is
A, B, C or D. For each of the four pointer registers a single 32-bit
`MOVE.L` to `BLTxPTH` sets both words, correctly pointing the channel at the
intended word.

Program (`prog_pointer.s`): four one-word blits, one per channel, each in
turn, with its own source word, its own decoy word and its own destination
word (every word a distinct pattern). The pointer of the channel under test
is set by one `MOVE.L #addr,BLTxPTH`; every other pointer the blit uses is
set by two `MOVE.W` writes, so a failed 32-bit write shows in that channel
alone:

| channel | pointer set by `MOVE.L` | blit | minterm | result word |
|---|---|---|---|---|
| A | `BLTAPTH` | D = A | $F0 | $CAFE (A source) |
| B | `BLTBPTH` | D = B | $CC | $B00B (B source) |
| C | `BLTCPTH` | D = C | $AA | $C0DE (C source) |
| D | `BLTDPTH` | D = A | $F0 | $D00D (A source, to the intended destination) |

The four result words are stored to `RESULTS`, `RESULTS`+2, +4 and +6 and read
with `peek_logical()`.

Components (`A1-A`, `A1-B`, `A1-C`, `A1-D`, no claim id): each result word
equals its source pattern. Aggregate (`A1`, carrying the four claim ids): all
four component propositions hold.

Mutations (stimulus): `prog_pointer_mutant_a.s` .. `_d.s` (`PTR_MUT_A` ..
`PTR_MUT_D`, `NMH_MUTATE=A1-A` .. `A1-D`) point one channel's `MOVE.L` at that
channel's decoy word (destination decoy for D); the unchanged component check
of that channel must go red and the aggregate with it.
`prog_pointer_mutant.s` (all four set, `NMH_MUTATE=A1`) must turn the
aggregate red.

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

### BBUSY-PREDICTS [p186] BBUSY set predicts more writes still to come

No claim id (the ids of the busy-bit test pair are on BBUSY-LOCKOUT, below).

Proposition: p.186: "A blitter done flag, also called the blitter busy flag,
is provided as DMAF_BLTDONE (bit 14) in DMACONR. This flag is set when a blit
is in progress." After the manual's prescribed intervening chip-memory read,
a `BBUSY` read that comes back *set* is not a false alarm: the blit genuinely
still has destination writes left to make.

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

A8, SYSDMA-C29, A9, A10, A11, A6, OPEN50 and PARITY below depend on this.
Figure 6-9 (p.195) draws a memory-refresh block at the start of each
horizontal line's own DMA-slot allocation, ahead of the display fetch
region; p.194 gives a line as "227.5 'color clocks', or memory access
cycles" -- 227 whole slots in the run (SLOT-UNIT prints it), an odd count, so
any fixed-cycle reference's parity flips from one line to the next. The slot
unit is the one SLOT-UNIT derives and asserts.

At the RAM port, refresh shows up as reads below word address $1000 --
far below every program-specific range this package uses (`PROGRAM_ORG`,
every scratch word, every blit source/destination, every bitplane
pointer) -- 4 per line, 2 slots apart, matching Figure 6-9's own refresh
block. Every low read is grouped (a gap over 12.5 slots starts a new
group), but not every such group is a line's own refresh: in the
bitplane builds, BPL2-4's own pointers are left unset and so also read
low memory, in groups of a different size and spacing. Only a group that is
exactly four reads, each two slots after the one before, is kept as a line's
own start; any other group is some other master's low read, not refresh, and
does not anchor a line. PARITY-REF's own precondition below, not an unproven
assumption, is what confirms the reads this restricted grouping keeps still
land on the real refresh schedule. A slot is numbered from its line's first
refresh slot, which is number 1 (so the refresh slots carry the odd numbers,
Figure 6-9); a CPU chip-bus access is graded even/odd by its own slot offset
from *its own line's* last refresh read, floor-divided by one slot -- not by
its raw remainder, since a CPU sample and a DMA (refresh) sample are each
taken at a fixed sub-slot phase half a slot apart, so their difference is
never an exact multiple of a slot, only its floor is meaningful. This is a
fact about the RAM-port sampling of the harness, not a page citation.

## Missed cycles of the 68000

p.196: "Some 68000 instructions do not match perfectly with the allocation of
even cycles and cause cycles to be missed. If cycles are missed, the 68000
must wait until its next available memory slot before continuing." A8 and
SYSDMA-C29 below identify a missed cycle from the RAM-port log alone. The
log shows the cycles in which an access happened, never the cycle in which
the 68000 asked for one, so the cycle the 68000 was ready for is found from
two accesses of its own and the address pair they form (a program fetch and
the scratch write each have their own address, so a pair names the
instruction boundary it brackets):

- with no DMA access in any slot between them, the second access follows the
  first after `g` slots, that address pair's *unimpeded gap*; the largest
  unimpeded gap the run shows for the pair is `g`, so the 68000 is ready for
  the second access `g` slots after the first at the latest;
- when the pair is further apart than `g`, the slot `t` = first + `g` is the
  slot the 68000 was ready for; if a DMA access took `t`, the cycle was
  *missed* (a delayed pair whose `t` no DMA access took is counted and set
  aside);
- a pair that spans a line start is set aside, because the slot numbering
  restarts there.

A missed cycle is *even* or *odd* by the number of its slot `t` (Figure 6-9).

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

### A8 [p194,p196] a missed cycle waits for the next available slot

Claims: `6-system-dma.c54`, `6-system-dma.c55`, `ch6-sysdma.c30`.

Proposition: p.196: "If cycles are missed, the 68000 must wait until its next
available memory slot before continuing." A missed even-numbered cycle (the
slot the 68000 was ready for, which Figure 6-10 assigns to it, was taken by a
DMA access) is followed by the access at the next available slot. The page
does not say whether a free odd-numbered slot is also available to the 68000
(the odd-slot accesses are PARITY's finding), so the graded line leaves that
open: the access must come no later than the first free even-numbered slot,
that is, no free even-numbered slot may lie unused between the missed slot
and the access. A wait that skips an available slot would not be a wait
"until its next available memory slot".

Program: `prog_sharing_share.s` (`SHARE` build): the counting loop with a
share-mode blit running, so the blitter takes slots from the 68000
throughout the blit's active window. The loop sits at the same address in
every build of `prog_sharing.s` and `prog_bitplane.s`.

Observed (precondition, "A8PRE", no claim id): the number of missed
even-numbered cycles found by the procedure above (see "Missed cycles of the
68000"), and the address pair of the first (printed).

Expected value (precondition): at least one.

Observed (claim): among those missed even cycles, the number with a free
even-numbered slot between the missed slot and the access.

Expected value: zero.

Mutation (precondition, `NMH_MUTATE=A8PRE`): the no-DMA run (`prog_sharing.s`)
replaces the share-mode run; no DMA access takes a slot the 68000 was ready
for, so no even cycle is missed, the precondition goes red and A8 prints
`NOT GRADED`.

Mutation (`implementation`, run with `NMH_MUTATE=A8`): a private copy of
`minimig_m68k_bridge.v`, outside the repository, in which the 68000 is not
acknowledged in the first two free colour clocks after a colour clock the
chipset took from it; the package is built against that copy and the
unchanged comparator finds a free even-numbered slot left unused after every
missed even cycle (two consecutive slots are left unused, so that one of
them is even-numbered whatever the parity of the first). No stimulus can
make this comparator red: the 68000 is always either pending or not, and the
stimulus cannot make the design leave a slot free that a pending 68000 could
have used. A stimulus that idles the 68000 (a long instruction) is calibrated
away by the unimpeded gap, and the TG68K core has no instruction timing to
idle at all.

### SYSDMA-C29 [p196] some 68000 instructions miss the even-cycle allocation

Claim: `ch6-sysdma.c29`.

Proposition: "Some 68000 instructions do not match perfectly with the
allocation of even cycles and cause cycles to be missed." An instruction
does not match the allocation when the slot the 68000 is ready for after it
is an odd-numbered slot, which Figure 6-10 assigns to other devices; in the
four-low-resolution-bitplane display the display takes those slots
(Figure 6-9, Figure 6-11), so the cycle is missed. Existence only.

Program: `prog_bitplane.s`, four low-resolution planes (`ADDQ.W #1,D0` / `MOVE.W D0,(A1)` / `BRA.S .loop`).

Observed: among the pairs of consecutive CPU accesses, the number of missed
odd-numbered cycles found by the procedure of "Missed cycles of the 68000"
(the address pair of the first is printed).

Expected value: at least one, on both cores, by the same comparator. A core
whose 68000 is never ready for an odd-numbered slot in this run reads red.

Mutation (`prog_bitplane_aligned.s`, `ALIGNED` set,
`NMH_MUTATE=SYSDMA-C29`, `stimulus red`): the loop's `BRA.S` is replaced
with `JMP (An)` (a whole multiple of four clock periods, MC68000 timing
tables): every gap is two slots and the 68000 is never ready for an odd
slot; the unchanged "at least one" check must go red.

Mutation (`NMH_MUTATE=SYSDMA-C29-SHARE`, `stimulus green`): where the
unmutated line is red, the same comparator reads the share-mode blit's own run
(`prog_sharing_share.s`), a different, live input on which the 68000 is ready
for odd slots the blitter takes; the check must read green.

### A9 [p196] four or fewer low-resolution bitplanes grant the CPU alternate cycles

Claims: `6-system-dma.c61`, `ch6-sysdma.c34`.

(`ch6-sysdma.c35` -- "only if it is ready to ask for the cycle and is the
highest priority item at the time" -- is withdrawn; see the table below.)

Proposition: p.196: "If the display contains four or fewer low resolution
bitplanes, the 68000 can be granted alternate memory cycles (if it is ready
to ask for the cycle and is the highest priority item at the time)."
Alternate memory cycles are every second slot, the even-numbered ones
(Figure 6-10). The stimulus is a 68000 that is continuously ready -- the
whole-4-clock-aligned counting loop -- with nothing enabled beside the
display, so that nothing ranks above it but the display's own slots; the
claim's condition is met. For one, two, three and four low-resolution
bitplanes, every pair of consecutive CPU accesses inside a line's fetch
window must be exactly two slots apart and both on even-numbered slots.

Programs: `prog_bitplane_aligned_p1.s`, `_p2.s`, `_p3.s` (`BP_NPLANES`=1, 2, 3,
`ALIGNED`) and `prog_bitplane_aligned.s` (four planes): the aligned counting
loop (`ADDQ.W #1,D0` / `MOVE.W D0,(A1)` / `JMP (A2)`, 4, 8 and 8 clock
periods) with the standard low-resolution `DIWSTRT`/`DIWSTOP`/`DDFSTRT`/
`DDFSTOP`.

**Fetch windows** (shared with A10, A11, A6, OPEN50, PARITY): BPL1-range
reads ($0C6000..) are grouped into per-line bursts (a gap over 25 slots
starts a new burst). p.197: "either 20 or 40 words will be fetched during the
horizontal line display time"; a burst holds as many reads as the run's most
frequent burst (one burst the capture started or ended inside holds fewer
and is not a window). Four high-resolution bitplanes need "all of the
available memory time slots during the display time just to fetch the 40 data
words for each line of the four bitplanes (40 * 4 = 160 time slots)"
(p.197), and Figure 6-12 fills every one of those 160 slots with planes in the
repeating order 4-2-3-1 -- BPL1 is fetched *last* in its own 4-slot group.
Figure 6-11 (six-bitplane, same page) shows the same structure at low
resolution, an 8-slot group ending in plane 1. So each burst's own window is:
from `(burst's own modal read-to-read spacing) - 1 slot` before its first
read, to its last read -- derived from the capture's own measured spacing
rather than a hard-coded, resolution-specific constant.

Observed (precondition, "A9PRE", no claim id): for each of the four plane
counts, that fetch windows are found and that the loop is continuously ready --
for every pair of its accesses of one line with no DMA access in any slot
between them, the gap is exactly two slots.

Observed (claim): for each plane count, the pairs of consecutive CPU accesses
that lie in one fetch window, and among them those that are not two slots
apart or not on even-numbered slots.

Expected value: precondition holds, and for every plane count there is at
least one pair and none is off the pattern. If the precondition fails the line
prints `NOT GRADED`.

Mutation (`NMH_MUTATE=A9`): the four-plane run is replaced with the six-plane
aligned run (`prog_bitplane_planes6_aligned.s`): six planes take even slots
from the 68000 and its pairs are four slots apart; the unchanged check must
go red.

Precondition mutation (`NMH_MUTATE=A9PRE`): the four-plane run is replaced
with the no-DMA aligned run (`prog_sharing_aligned.s`), which has no display
windows; the precondition goes red and A9 prints `NOT GRADED`.

### A10 [p196,p197] four high-resolution bitplanes lock the CPU out entirely

Claims: `6-system-dma.c71`, `ch6-sysdma.c42`.

Proposition: at four high-resolution bitplanes (double the low-res fetch
bandwidth), the CPU gets no chip-bus access at all during the active
display window.

Program (`prog_bitplane_hires.s`, `PLANES=4 HIRES=1` build): as A9,
`BPLCON0`'s `HIRES` bit set, the high-resolution `DDFSTRT`/`DDFSTOP`
window.

Fetch windows: as defined in A9.

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

### A11 [p196] more than four low-resolution bitplanes steal cycles during the display

Claims: `6-system-dma.c62`, `ch6-sysdma.c36`.

Proposition: p.196: "if there are more than four bitplanes, bitplane DMA will
begin to steal cycles from the 68000 during the display." Raising the
low-resolution bitplane count above four takes cycles from the CPU that it
had at four. Graded inside the display windows, with five and with six
planes, by the page's direction only: no percentage is asked for.

Programs: `prog_bitplane.s` with `PLANES=4` (default), `prog_bitplane_planes5.s`
(`BP_NPLANES`=5) and `prog_bitplane_planes6.s` (`BP_PLANES6`), the same
counting loop (`BRA.S`) and display window in each. The beam is the same in
every run (the boot is identical until the display starts), so a fetch window
found at the same cycle in the four-, five- and six-plane runs is the same
line's window.

Observed (precondition, "A11PRE", no claim id): the fetch windows of the
three runs coincide (every window of the four-plane run is found at the same
cycle in the other two, and no other), and the CPU keeps running in all three
runs (it makes chip-bus accesses outside the windows) -- so a dead CPU cannot
satisfy the comparison.

Observed (claim): for every window, the number of the CPU's chip-bus accesses
(program fetches and scratch writes) inside it, in each of the three runs.

Expected value: precondition holds, and in every window the five-plane count
and the six-plane count are each strictly lower than the four-plane count.

Mutation (`NMH_MUTATE=A11`): the five- and six-plane runs are replaced with
the four-plane run; the counts are equal and the unchanged strict check must
go red.

Precondition mutation (`NMH_MUTATE=A11PRE`): the five-plane run is replaced
with the no-DMA run; there are no windows to coincide, the precondition goes
red and A11 prints `NOT GRADED`.

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
for the in-window intervals; `prog_sharing_aligned.s` (`BASELINE`, aligned
loop) for the loop's period with the bus to itself.

Observed: the loop's full-speed period, the interval between two scratch
writes with no DMA access in any slot between them in the no-DMA run (its most
frequent value is taken; the intervals are printed with their count); and
every interval between two scratch writes that lie in one fetch window of the
four-low-res-plane run (the windows of A9).

Expected value: there is at least one in-window interval, and every one of
them equals the full-speed period -- every interval, not the most frequent
one.

Mutation (`prog_bitplane_planes6_aligned.s`, `BP_PLANES6` and `ALIGNED`
both set, `NMH_MUTATE=A6`): the in-window intervals are taken from six
aligned-loop planes instead of four; the unchanged "every interval equals
the full-speed period" check must go red (six planes slow the CPU, per
A11/OPEN50).

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
(p.197, of the six-bitplane case, Figure 6-11). p.196: "160 time slots"
per line during the display time; a slot not taken by a DMA access is open.
Graded in every fetch window, not only in the median one.

Program: `prog_bitplane.s`, `PLANES=4` and `PLANES=6` builds, using the
same p.197 fetch windows as A9/A10.

Observed (precondition, "OPEN50PRE", no claim id): every fetch window of the
two runs spans the page's 160 slots.

Observed (claim): for every window, the number of open slots (160 minus the
non-CPU accesses inside it), for both plane counts.

Expected value: precondition holds, and all four-plane windows leave the same
positive number of slots open, all six-plane windows leave the same number,
and the six-plane number is exactly half the four-plane number.

Mutation (`NMH_MUTATE=OPEN50`): the six-plane side is the four-plane
capture instead; the unchanged "exactly half" check must go red.

Precondition mutation (`NMH_MUTATE=OPEN50PRE`): the six-plane run is replaced
with the no-DMA run; there are no windows, the precondition goes red and
OPEN50 prints `NOT GRADED`.

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

Measured: fx68k -- 20006 of 41007 no-DMA accesses on odd slots; 125 of
243 in the share-mode window. TG68K -- 0 of 45209 no-DMA; 124 of 254 in
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

### TAS-SPAN [p196] the read-modify-write cycle of TAS does not fit into one slot

Claims: `6-system-dma.c57`, `ch6-sysdma.c33`.

Proposition: p.196: "the indivisible read-modify-write cycle that is used
only in this instruction will not fit into a DMA memory access slot." An
ordinary 68000 bus cycle occupies one memory slot (Figure 6-10). On the RAM
port a cycle is the run of samples of one address and direction; its span is
the cycle of its last sample minus the cycle of its first. The TAS cycle is
the read at the TAS address and the write that follows; it does not fit a slot
if its span, from the read's first sample to the write, exceeds one slot,
while every ordinary cycle at the same address, measured in the same run,
spans at most one slot.

Program (`prog_tas_span.s`): three ordinary accesses to one byte (a write, a
read, a write), then one `TAS.B` on it, with no DMA channel enabled.

Observed (precondition, "TAS-SPAN-PRE", no claim id): the cycles at the TAS
address are the program's five: write, read, write, then the TAS read and
write.

Observed (claim): the longest span among the three ordinary cycles; the
interval from the first sample of the TAS read to the TAS write.

Expected value: precondition holds, the ordinary spans are at most one slot
and the TAS interval exceeds one slot.

Mutation (precondition, `prog_tas_span_mut.s`, `NMH_MUTATE=TAS-SPAN-PRE`): the
`TAS.B` is replaced with `NOP`; the address shows three cycles, the
precondition goes red and TAS-SPAN prints `NOT GRADED`.

Mutation of TAS-SPAN itself: `none impossible`, unfalsifiable at this
boundary. The read and the write of a read-modify-write are two RAM-port
samples in two different slots, so no stimulus makes their span at most one
slot, and an ordinary cycle is one sample; only a design that did the whole
cycle inside one slot could be red, and the stimulus cannot make the design
do that. The line is a comparison against the slot unit that SLOT-UNIT
asserts, and the three ordinary cycles measured in the same run are its
control on the same RAM-log basis.

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

### BBUSY-LOCKOUT [p186] the second busy-bit test is correct in a locked-out blit

Claims: `6-blitter-done-flag.c10`, `ch6-done.c12`, `ch6-done.c10`.

Proposition: p.186: "If a blit has just been started but has been locked out
of memory access because of, for instance, display fetches, this bit may not
yet be set. The processor, on the other hand, may be running completely
uninhibited out of Fast memory or its internal cache, so it will continue to
have memory cycles. The solution is to read a chip memory or hardware register
address with the processor before testing the bit ... The first "test" of the
blitter done bit may not return the correct result, but the second will."

Program (`prog_bbusy_lockout.s`, `prog_fastrun_lockout.s` with `BB_TEST`): the
display, the Copper list and the blit of FASTRUN-LOCKOUT (four
high-resolution bitplanes lock the blitter and the 68000 out of the fetch
window, p.197; the Copper starts the blit just before a window), with
`BLTPRI` clear, and a 68000 loop in Fast memory that reads `DMACONR` twice in
a row and stores both values, in order, to Fast memory, 600 times.

The 68000 needs a Chip bus cycle for nothing in that loop but the two register
reads, so it is the reads that are held across the window. An iteration whose
previous pair was stored before the window began and whose own first store comes
after it was pending on a register read throughout the window (nothing else
in the loop waits), so its second read completed after the window -- after the
Copper's write of the blit size -- and, since the pair was stored before the
blit's last write, before the blit ended. The blitter's first access also comes
after the window, so the blit had been started and locked out.

Observed (precondition, "BBUSY-LOCKOUT-PRE", no claim id): one fetch window lies
between the Copper's fetch of the BLTSIZE value and the blitter's first access;
a pair pending across it exists; the program reached `DONE`; and that pair was
stored before the blit's last write.

Observed (claim): `BBUSY` (bit 14) of the second value of that pair. The first
value is printed as a measurement.

Expected value: precondition holds, and `BBUSY` reads set. If the precondition
fails the line prints `NOT GRADED`.

Mutation (`NMH_MUTATE=BBUSY-LOCKOUT`, `prog_bbusy_lockout_intena.s`,
`BB_INTENA`): both tests read `INTENAR`, whose bit 14 is clear; the unchanged
check must go red.

Precondition mutation (`NMH_MUTATE=BBUSY-LOCKOUT-PRE`,
`prog_bbusy_lockout_2plane.s`, `FR_2PLANE`): two high-resolution bitplanes leave
half the window's slots free, the blitter's first access lands inside the
window, the precondition goes red and BBUSY-LOCKOUT prints `NOT GRADED`.

## Claims not asserted

| claim id(s) | page | reason |
|---|---|---|
| `6-intro.c5`, `ch6-intro.c5` | 169 | Factor not toleranced ("approximately twice as fast" has no numeric tolerance on the page); ordering asserted by ORDER above (a blitter A-to-D copy of 500 words, uncontended, finishes before a 68000 `MOVE.W` copy loop of the same 500 words). ORDER carries no claim id of its own -- only the ordering is graded, not the factor, so these ids stay withdrawn rather than moving onto ORDER's own label. |
| `6-blitter-done-flag.c9`, `ch6-done.c11`, `ch6-done.c8` | 186 | These claims are permissive ("may not return the correct result", "may not yet be set"), so no simulation outcome contradicts them; a deterministic Verilator run cannot falsify a "may". The mechanism the page actually gives (p.186's NOTE) is deterministic, not metastable: a blit just started can be locked out of its first DMA cycle by display fetches, so `DMAF_BLTDONE` has not yet gone high, while the CPU keeps running and can observe the not-yet-set flag. BBUSY-LOCKOUT above builds that locked-out case and grades the second test (`6-blitter-done-flag.c10`, `ch6-done.c12`, `ch6-done.c10`); the first test's value there is printed as a measurement. |
| `ch6-multi.c4` | 187 | See its own section above: `cpu_config`=3 was tried on TG68K and measured, producing no observable difference from `cpu_config`=0 in a no-DMA fixture; fx68k does not read `cpu_config` at all. The page's own consequence needs the CPU cache path too, which was not exercised. |
| `6-system-dma.c12`, `ch6-sysdma.c10` | 194 | "The two lowest priority levels" ranks the blitter and CPU against every other DMA channel (audio, disk, sprites, copper, bitplane), not just against each other. Confirming it needs all of those channels driven simultaneously against blitter and CPU, a materially larger fixture than this package's scope. The narrower fact -- the blitter outranks the CPU specifically -- is A4-SHARE (and A4 with `BLTPRI` set). Holds as a scope decision with a named fixture. |
| `ch6-sysdma.c35` | 196 | "The 68000 is granted the cycle only if it is ready to ask for it and is the highest priority item at the time": the "only if" direction -- never granted when it has not asked or is outranked -- needs the 68000's request visible, which the RAM port does not show (see A5-WITHDRAWN). A9 grades the "if" direction: a continuously ready, otherwise unopposed 68000 is granted alternate cycles. |
| `6-system-dma.c51`, `6-system-dma.c52`, `ch6-sysdma.c26`, `ch6-sysdma.c27` | 196 | "About half" of instruction time is an average over the 68000's own full instruction mix, with no numeric tolerance given on the page. An instruction-timing histogram compiled per core would settle it. |
| `ch6-sysdma.c32` | 196 | "The 68000 test-and-set instruction (TAS) should never be used in the Amiga" is prescriptive advice to a programmer, not an RTL-observable claim; holds as written. |
| `ch6-sysdma.c31` | 196 | "Most instructions do not cause cycles to be missed" is a statistic over the instruction set with no numeric tolerance; holds as written (SYSDMA-C29 above establishes the *existence* of misses, per `c29`, not their rate). |

Total: 17 graded assertions carry claim ids (A1, A2, A3, DESC-OFFBY1,
A4-SHARE, A8, SYSDMA-C29, A9, A10, A11, A6, OPEN50, PARITY, TAS-SPAN,
BBUSY-LOCKOUT, FASTRUN, FASTRUN-LOCKOUT), 42 claim ids in full. Graded lines
without a claim id: A1-A, A1-B, A1-C, A1-D, A4, ORDER, BBUSY-PREDICTS.
Apparatus and precondition checks without a claim id: FIXTURE, SLOT-UNIT,
DESC-BASE, A3PRE, A4PRE, A4-SHARE-PRE, A8PRE, A9PRE, A10PRE, A11PRE,
OPEN50PRE, PARITY-REF, TAS-SPAN-PRE, FASTRUN-LOCKOUT-PRE, BBUSY-LOCKOUT-PRE.
5 claim ids are withdrawn by `checks.record()`: 2 as A5-WITHDRAWN, 1 as
SLOTSPAN-WITHDRAWN, 1 as TAS-WITHDRAWN, 1 as `ch6-multi.c4`. 14 claim ids
are withdrawn in the table above. 42 + 5 + 14 = 61 claim ids checked in
total.

## RECORDED

The final run of `make -B test` on each core. `PASS` and `FAIL` are graded lines; `NOT GRADED` is a line whose precondition was not met; `RECORDED` is a decline (a withdrawn or scope-limited claim). Counts: fx68k 38 passed, 1 failed; tg68k 37 passed, 2 failed.

| line | fx68k | tg68k |
|---|---|---|
| `FIXTURE` | PASS | PASS |
| `A1-A` | PASS | PASS |
| `A1-B` | PASS | PASS |
| `A1-C` | PASS | PASS |
| `A1-D` | PASS | PASS |
| `A1` | PASS | PASS |
| `DESC-BASE` | PASS | PASS |
| `DESC-OFFBY1` | PASS | PASS |
| `A3` | PASS | PASS |
| `A3PRE` | PASS | PASS |
| `BBUSY-PREDICTS` | PASS | PASS |
| `SLOT-UNIT` | PASS | PASS |
| `A2` | PASS | PASS |
| `A4PRE` | PASS | PASS |
| `A4` | PASS | PASS |
| `A4-SHARE-PRE` | PASS | PASS |
| `A4-SHARE` | PASS | PASS |
| `A5-WITHDRAWN` | RECORDED | RECORDED |
| `A8PRE` | PASS | PASS |
| `A8` | PASS | PASS |
| `SYSDMA-C29` | PASS | FAIL |
| `A9PRE` | PASS | PASS |
| `A9` | PASS | PASS |
| `A10PRE` | PASS | PASS |
| `A10` | PASS | PASS |
| `A11PRE` | PASS | PASS |
| `A11` | PASS | PASS |
| `A6` | PASS | PASS |
| `SLOTSPAN-WITHDRAWN` | RECORDED | RECORDED |
| `OPEN50PRE` | PASS | PASS |
| `OPEN50` | PASS | PASS |
| `PARITY-REF` | PASS | PASS |
| `PARITY` | FAIL | FAIL |
| `ORDER` | PASS | PASS |
| `TAS-WITHDRAWN` | RECORDED | RECORDED |
| `TAS-SPAN-PRE` | PASS | PASS |
| `TAS-SPAN` | PASS | PASS |
| `FASTRUN` | PASS | PASS |
| `FASTRUN-LOCKOUT-PRE` | PASS | PASS |
| `FASTRUN-LOCKOUT` | PASS | PASS |
| `BBUSY-LOCKOUT-PRE` | PASS | PASS |
| `BBUSY-LOCKOUT` | PASS | PASS |
| `MULTI-C4-WITHDRAWN` | - | RECORDED |

Measurements printed by the run (fx68k):

```text
DESC-OFFBY1: A (pointer at last word) writes first=yes extra=no; B (pointer one word past the last) writes $0C2100=no $0C2102=yes $0C2104=yes $0C2106=yes $0C2108=yes $0C210A=no
BBUSY-PREDICTS: 200-word build -- last D-write at cycle 2274873, RESULTS store at cycle 2274645, 26 D-writes total
capture health: base.log.size=47619 share.log.size=48282 nasty.log.size=48154 tiny.log.size=47612 base_aligned.log.size=51819
capture health: lores4.log.size=163218 hires4.log.size=173463 lores6.log.size=173456 lores4_aligned.log.size=177674 lores6_aligned.log.size=177674
SLOT-UNIT: smallest gap between two refresh accesses = 16 clk_sys, slot unit = 8 clk_sys; modal line period = 1816 clk_sys = 227 slots (1651 of 1651 line intervals); 1652 refresh group(s) graded
A2: active window 2274377..2284489, 48 scratch write(s) strictly inside it (excluding the window's own endpoints)
A4-SHARE: CPU accesses 243 in a share-mode window of 10112 clk_sys; reference 41007 in 726488 clk_sys (no-DMA run)
A5: inside the share-mode blit's active window consecutive CPU accesses are most often 40 clk_sys apart (190 of 242 intervals)
A8: 40669 access pairs, 124 missed even cycle(s), 305 missed odd cycle(s), 0 with an even free slot left unused, 0 delayed with a free ready slot, 400 spanning a line start, 6 without an unimpeded gap; first missed pair: fetch $80500 -> fetch $80502, 5 slots
SYSDMA-C29: 92965 access pairs, 0 missed even cycle(s), 11097 missed odd cycle(s), 0 with an even free slot left unused, 0 delayed with a free ready slot, 178 spanning a line start, 0 without an unimpeded gap; first missed pair: fetch $80506 -> fetch $80500, 4 slots
A10: 768 fetch window(s); maximum CPU accesses in any one window = 0
A11: 768 four-plane window(s), 768 found at the same cycle with five and six planes; CPU accesses outside the windows: 42278 / 42276 / 42276
A6: no-DMA period of the aligned loop = 80 clk_sys (8398 unimpeded intervals); 11520 scratch-write interval(s) inside the display windows, 0 not equal to it
SLOTSPAN: modal interval between consecutive CPU bus accesses of the BASELINE loop = 16 clk_sys (2 slot(s)), 41006 intervals
OPEN50: open slots per 160-slot window -- four planes 80..80, six planes 40..40
PARITY-REF: 15360 of 15360 BPL1 reads classify odd by the per-line reference (four-lores-plane build)
PARITY: no-DMA (baseline) odd-slot accesses 20006 of 41007; share-mode window (blit's own active window) odd-slot accesses 125 of 243
ORDER: CPU copy loop of 500 word(s) spans 44200 clk_sys (500 writes seen); the nasty-mode blit's own 500-word copy spans 8112 clk_sys, same yardstick (first to last destination write)
TAS: read-to-write gap = 24 clk_sys, 5 other RAM-port access(es) between them (TAS.B, share-mode blit concurrent)
TAS-SPAN: slot unit 8 clk_sys; the three ordinary cycles span at most 0 clk_sys; the TAS read spans 16 clk_sys and its write follows the read's first sample after 24 clk_sys
ch6-multi.c4 [p187]: not applicable on fx68k (fixed 68000 implementation; cpu_config selects TG68K-only behaviour, nanomig.v:198)
FASTRUN: 50-iteration -- done=y RESULTS=50 blit window=2276281..2284425 FR_CNT fast-RAM writes strictly inside=49; 2000-iteration -- done=y RESULTS=2000 blit window=2276281..2284425 FR_CNT fast-RAM writes strictly inside=75; in-window period(mode)=108 clk_sys (74 gaps), after-window period=108 clk_sys (1923 gaps), every in-window gap equals after-window period=y
FASTRUN-LOCKOUT: done=y RESULTS=8000; Copper's own MOVE-data-word fetch at cycle=2401033; fetch window=2401049..2402321; blitter's own first DMA access at cycle=2402361; FR_CNT fast-RAM writes strictly inside the fetch window=12
BBUSY-LOCKOUT: Copper data-word fetch at cycle 2401033; fetch window 2401049..2402321; blitter's first access at 2402369, last at 2433225; pair pending across the window: iteration 42, first store at cycle 2402372
BBUSY-LOCKOUT: first test read 03C0 (BBUSY clear), second test read 43C0 (BBUSY set)
```

Measurements printed by the run (tg68k):

```text
DESC-OFFBY1: A (pointer at last word) writes first=yes extra=no; B (pointer one word past the last) writes $0C2100=no $0C2102=yes $0C2104=yes $0C2106=yes $0C2108=yes $0C210A=no
BBUSY-PREDICTS: 200-word build -- last D-write at cycle 2274833, RESULTS store at cycle 2274645, 26 D-writes total
capture health: base.log.size=51822 share.log.size=52429 nasty.log.size=52305 tiny.log.size=51811 base_aligned.log.size=51822
capture health: lores4.log.size=177677 hires4.log.size=177677 lores6.log.size=177677 lores4_aligned.log.size=177677 lores6_aligned.log.size=177677
SLOT-UNIT: smallest gap between two refresh accesses = 16 clk_sys, slot unit = 8 clk_sys; modal line period = 1816 clk_sys = 227 slots (1651 of 1651 line intervals); 1652 refresh group(s) graded
A2: active window 2274337..2284537, 50 scratch write(s) strictly inside it (excluding the window's own endpoints)
A4-SHARE: CPU accesses 254 in a share-mode window of 10200 clk_sys; reference 45209 in 726528 clk_sys (no-DMA run)
A5: inside the share-mode blit's active window consecutive CPU accesses are most often 40 clk_sys apart (243 of 253 intervals)
A8: 44815 access pairs, 125 missed even cycle(s), 116 missed odd cycle(s), 0 with an even free slot left unused, 0 delayed with a free ready slot, 399 spanning a line start, 7 without an unimpeded gap; first missed pair: fetch $80500 -> fetch $80502, 5 slots
SYSDMA-C29: 107423 access pairs, 0 missed even cycle(s), 0 missed odd cycle(s), 0 with an even free slot left unused, 0 delayed with a free ready slot, 950 spanning a line start, 0 without an unimpeded gap; first missed pair: none
A10: 768 fetch window(s); maximum CPU accesses in any one window = 0
A11: 768 four-plane window(s), 768 found at the same cycle with five and six planes; CPU accesses outside the windows: 45984 / 45984 / 45984
A6: no-DMA period of the aligned loop = 80 clk_sys (8399 unimpeded intervals); 11520 scratch-write interval(s) inside the display windows, 0 not equal to it
SLOTSPAN: modal interval between consecutive CPU bus accesses of the BASELINE loop = 16 clk_sys (2 slot(s)), 45208 intervals
OPEN50: open slots per 160-slot window -- four planes 80..80, six planes 40..40
PARITY-REF: 15360 of 15360 BPL1 reads classify odd by the per-line reference (four-lores-plane build)
PARITY: no-DMA (baseline) odd-slot accesses 0 of 45209; share-mode window (blit's own active window) odd-slot accesses 124 of 254
ORDER: CPU copy loop of 500 word(s) spans 48112 clk_sys (500 writes seen); the nasty-mode blit's own 500-word copy spans 8112 clk_sys, same yardstick (first to last destination write)
TAS: read-to-write gap = 40 clk_sys, 4 other RAM-port access(es) between them (TAS.B, share-mode blit concurrent)
TAS-SPAN: slot unit 8 clk_sys; the three ordinary cycles span at most 0 clk_sys; the TAS read spans 0 clk_sys and its write follows the read's first sample after 16 clk_sys
ch6-multi.c4 [p187]: loop iterations over 3000000 cycles, no DMA -- cpu_config=3 (68020): 9040, cpu_config=0 (68000): 9040 -- contradicts nothing on the page (no DMA is competing for the bus here)
FASTRUN: 50-iteration -- done=y RESULTS=50 blit window=2276057..2284169 FR_CNT fast-RAM writes strictly inside=48; 2000-iteration -- done=y RESULTS=2000 blit window=2276057..2284169 FR_CNT fast-RAM writes strictly inside=127; in-window period(mode)=64 clk_sys (126 gaps), after-window period=64 clk_sys (1870 gaps), every in-window gap equals after-window period=y
FASTRUN-LOCKOUT: done=y RESULTS=8000; Copper's own MOVE-data-word fetch at cycle=2401033; fetch window=2401049..2402321; blitter's own first DMA access at cycle=2402361; FR_CNT fast-RAM writes strictly inside the fetch window=20
BBUSY-LOCKOUT: Copper data-word fetch at cycle 2401033; fetch window 2401049..2402321; blitter's first access at 2402369, last at 2433545; pair pending across the window: iteration 69, first store at cycle 2402408
BBUSY-LOCKOUT: first test read 43C0 (BBUSY set), second test read 63C0 (BBUSY set)
```
