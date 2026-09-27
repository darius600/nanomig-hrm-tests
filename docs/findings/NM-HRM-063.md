# NM-HRM-063 — FLAG is negative edge-sensitive; the RTL ORs a level

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | seen in the RTL source; no test yet |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. One assertion does carry it as red; *What the test measures* says why that red grades the wrong transition, and what the assertion must measure instead. |
| **Module** | `src/minimig-aga/cia_int.v:57` |
| **Report to** | **nobody, yet.** If the assertion *What it must measure* describes is red: Minimig/MiSTer maintainers, once Amiga specialists confirm it matters in practice |
| **Test** | `hrm/appF-ciab-port-assignments` · `PROP-FLAG-EDGE-POLARITY` — **red, on the transition p.341 names**: an implementation that follows the page is red on it too |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters · *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p.341 |
| **Assertions** | 1 red that grades the wrong transition; none yet that measures this finding |
| **Verdict of the four** | **unanimous, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all set the bit once per index pulse, on a transition — vAmiga and Copperline name it the pin's falling edge, the transition at which NanoMig's `flag` rises — and none of them from a held level |

---

## What the HRM says

Printed page 341, on the 8520's handshaking and interrupt inputs:

> FLAG is a negative edge-sensitive input that can be used for receiving the PC
> output from another 8520 or as a general-purpose interrupt input. **Any
> negative transition on FLAG will set the FLAG interrupt bit.**

The requirement is a *transition*, and specifically a negative one. p.341 is the
only page in the whole corpus that calls FLAG edge-sensitive. Appendix E says of
the signal wired to CIA-B's FLAG only that the interrupt comes *"whenever the
INDEX\* input goes active"* (p.334), and `*` means an active-low signal (p.379).
Chapter 8 names the same pin, p.245:

> FLAG DSKINDEX\* Disk index pulse ($BFDD00, bit 4). Can be used to create a
> level 6 interrupt.

So the negative transition p.341 names is INDEX\* going active: the start of
each index pulse.

Printed page 346 says what a read of the interrupt data register does:

> When you read the DATA register, its contents are cleared (set to 0), and the
> IRQ line returns to a high state.

Together: a negative transition sets the bit, a read clears it, and while FLAG
stays low after that read nothing sets it again.

## What the RTL does

`src/minimig-aga/cia_int.v`, the latching branch:

```verilog
icr[4] <= icr[4] | flag;   // external flag input
```

A level-sensitive OR on an active-high line. **There is no edge detector
anywhere in the module and no previous-state register for `flag`**, so no edge
can be derived: the module cannot distinguish a transition from a level.

`flag` is 1 while INDEX\* is active. Its one driver is `index` (`minimig.v:647`,
`.flag(index)`; `minimig.v:526`, `.index(index)`), which `paula_floppy.v:576`
computes as `assign index = |(~_sel & motor_on) & ~|rpm_pulse_cnt & sof;` — 1
during the index pulse, 0 otherwise. CIA-A's `flag` is tied to `1'b0`
(`ciaa.v:321`). A 0→1 on `flag` is therefore the pin's negative transition, and
the RTL sets the bit there, as p.341 says. `sof` is one `clk7_en` cycle wide
(`agnus.v:485`, `.eof(sof)`; `agnus_beamcounter.v:374`,
`assign end_of_frame = vpos_inc & last_line;`, with `vpos_inc` registered high
only while `hpos==2`, `agnus_beamcounter.v:309-316`), so in NanoMig `flag` is
never 1 for longer than one cycle. Where `ciab` is given a level, it builds an
edge detector: its TOD input, fed `_hsync` (`minimig.v:644`), counts on
`tick & ~tick_del` (`ciab.v:166-169`, `ciab.v:246`).

The clearing branch, eight lines up, is `icr[4] <= flag;` — so reading ICR while
`flag` is still high re-sets the bit in the same cycle. A held level is not only
sufficient to set the bit, it also survives the read that is supposed to clear
it. That is the one place the RTL and the page part: with `flag` held at 1 —
INDEX\* held active — a read leaves the bit set, where p.346 clears it and p.341
has no transition to set it again. For the one-cycle pulse NanoMig produces, the
bit is set once per pulse and a read clears it, as the page predicts.

### Measured

[`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments)
grades `PROP-FLAG-EDGE-POLARITY` red, one of the package's `7 passed, 3 failed`.
The red is the RTL setting the bit on the transition p.341 names; *What the test
measures* below gives the assertion in full. No graded assertion holds `flag` at
1 across an ICR read, the one stimulus on which the RTL and the page part.

## What the test measures

`PROP-FLAG-EDGE-POLARITY` (`test_appF-ciab-port-assignments.cpp:429-439`) arms
FLG, drives `flag` 0→1, holds it for five ticks, reads ICR once and expects bit
4 **clear**. It takes `flag` for the pin's level, so it counts the 0→1 as a
positive transition and the stimulus as free of negative ones. At this port
`flag` is 1 while INDEX\* is active (*What the RTL does*): the 0→1 is the
negative transition p.341 names, and p.341 predicts the bit **set**. The RTL
sets it, and the assertion is red. An implementation that sets FLG on exactly
that transition is red on it as well; the assertion goes green only for one that
leaves FLG clear for five ticks after `flag` rises — one that sets it when
`flag` falls, as INDEX\* goes inactive, or one that ignores `flag`.

The same package asserts the opposite. `PROP-FLAG-EXISTS`
(`test_appF-ciab-port-assignments.cpp:386-398`) drives the same 0→1, holds it
two ticks, requires bit 4 **set**, and passes. An implementation that keeps a
set bit until it is read cannot pass both.

Nor does the stimulus reach the case where edge and level differ. The package's
`read_reg` (`test_appF-ciab-port-assignments.cpp:281-292`) raises `aen` and
`rd`, evaluates, and lowers them again without a clock edge, so the branch a
read takes (`cia_int.v:43-50`) never runs and no read in the package clears
ICR. The paired control, `flag` 0→1→0
(`test_appF-ciab-port-assignments.cpp:451-465`), passes under either reading and
shows only that the arm-and-read path is live; the oracle mutation shows only
that the comparator can change its answer.

`hrm/appF-cia-interrupt-control` records the same stimulus at the `cia_int`
boundary as a `note()`, not an assertion
(`test_appF-cia-interrupt-control.cpp:879-885`): `flag` 0→1, held five ticks,
DATA `0x10`, described there too as free of negative transitions. The value is
the one p.341 predicts for that transition.

### What it must measure

The same fixture, `ciab` (`TOP := ciab` in the package's `Makefile`), with a
read of ICR made the way a CPU read cycle makes it: `aen=1`, `rd=1`, `wr=0`,
`rs=4'hD` held across one rising `clk` edge with `clk7_en=1`, and `data_out`
sampled before that edge, so that `cia_int.v:43-50` runs.

1. After reset with `flag=0`, write ICR `0x90` (set FLG in the mask).
2. Drive `flag` 0→1 — INDEX\* goes active, the negative transition — and hold it
   three ticks. Read ICR: bit 4 **set** (p.341).
3. Keep `flag` at 1 for five more ticks — INDEX\* held active, no transition.
   Read ICR again: bit 4 **clear**, and bit 7 (IR) with it (p.346, *"When you
   read the DATA register, its contents are cleared"*; p.341 gives nothing that
   sets it again).

Control on the same channel: the same steps with `flag` returned to 0 before the
first read must read bit 4 set, then clear — the clocked read clears ICR on this
fixture, so a set bit at step 3 is the held level and nothing else.

Step 3 separates edge from level: a level-OR sets bit 4 again at the read
(`cia_int.v:49`) and on every cycle after it (`cia_int.v:57`); an edge detector
does not. Step 2 fixes the polarity: an implementation that sets the bit when
`flag` falls reads it clear there. Copperline's own test asserts step 3 at its
CIA: a second `assert_flag()` with no `release_flag()` between leaves ICR
reading 0 (`cia.rs:1496-1499`).

## What the others say

| | | |
|---|---|---|
| **WinUAE** | an event; no level input exists | `cia.cpp:823-825`: `cia_diskindex()` calls `CIA_sync_interrupt(1, ICR_FLAG)`, and `disk.cpp:3973` calls it once per index pulse. Nothing holds a FLAG level that could be re-sampled |
| **vAmiga** | explicit, in two separate methods | `CIA.cpp:250-266`: `emulateRisingEdgeOnFlagPin()` only calls `wakeUp()`; `emulateFallingEdgeOnFlagPin()` calls `wakeUp()` and then `icr \|= 0x10` |
| **Denise** | an event; no level input exists | `emulation/cia/new/cia.cpp:302-304`'s `setFlag()` sets `intIncomming \|= 0x10`, called from `libami/drive/diskDrive.cpp:79` exactly when the head wraps past the end of a track — once per revolution |
| **Copperline** | an explicit edge detector over retained pin state | `src/chipset/cia.rs:373-380`: `set_flag_pin` computes `let falling_edge = self.flag_pin_high && !high;` and latches `ICR_FLG` only when that is true. The index pulse drives the pin low: `assert_flag()` is `self.set_flag_pin(false)` (`cia.rs:365-366`), called at `src/bus.rs:8056` and followed by `release_flag()`. The module's own test holds the pin low across a read: a second `assert_flag()` with no release between leaves ICR reading 0 (`cia.rs:1496-1499`) |
| **MiSTer** | descendant — same statement, in an otherwise-rewritten file | descendant — `rtl/cia_int.v:81` — the same statement, `icr[4] <= icr[4] \| flag;`; 115 of the file's lines differ from NanoMig, nearly all of them added commentary, and this one is unchanged by it |
| **TC64** | descendant — byte-identical to NanoMig | descendant — `rtl/minimig/cia_int.v:57` — byte-identical to NanoMig line for line, down to the "intterupts" typo in the comment on the ICR-clear branch. TC64 is the closer sibling: its whole file is untouched from NanoMig's, so a fix here is one clean edit against an otherwise-identical file. MiSTer's copy has been rewritten around the same statement — the fix lands in a file that has drifted everywhere else. Either way the statement is identical in both, and a fix belongs to both lines equally: `icr[4] <= icr[4] \| flag;` |

**Not one of the four independent implementations sets the bit from a level.**
Three of them cannot even represent a held level — WinUAE and Denise have only
the event, vAmiga only its two edge methods; the one that does model the pin as
a level detects the edge explicitly. Copperline is the closest structural
comparison to the RTL, and it is the one that keeps the previous-state bit
`cia_int.v` does not have.

All four set the bit once per index pulse — vAmiga and Copperline on the pin's
falling edge, the transition at which NanoMig's `flag` rises — and NanoMig's
one-cycle pulse gives the same one bit per revolution. They part from NanoMig only where the pin stays active across an ICR
read, and of the four only Copperline can hold it there.

## Why this was not found until appendix F was read

`hrm/ch8-disk-index-interrupt-path`'s `PROP-FLAG-PIN` grades *"a pulse on `flag`
sets ICR bit 4 and raises irq"*, and it passes. A pulse satisfies both the level
reading and the edge reading, so that assertion cannot separate them — and
chapter 8 was built without the appendix that specifies the part. p.341 is
narrower than anything chapter 8 had to work from.

## What is left for specialists

Whether a real Amiga ever holds FLAG at a level long enough for this to matter.
On CIA-B the pin is the floppy index pulse, which is a pulse by construction, so
the divergence may never be observable in practice — and the page's own sentence
offers the other use, *"receiving the PC output from another 8520 or as a
general-purpose interrupt input"*, where a held level is entirely plausible. In
NanoMig it is not observable: `index` is one `clk7_en` cycle wide and CIA-A's
`flag` is tied to 0 (*What the RTL does*), so the difference sits at `ciab`'s
port, for a driver that holds it.

Whether `flag` is meant as the pin at all is part of the same question:
`cia_int.v:8-12` describes it the way it describes `ta`, `tb` and `alrm`, as an
input that sets its ICR bit, and its one driver delivers a single-cycle event.

That judgement, and whether the Minimig line should carry an edge detector,
belongs to Amiga and RTL specialists. **This suite's part is the measurement,
and no graded assertion makes it yet: the page requires a transition, the RTL
re-sets the bit from a held level at the read, all four independent
implementations follow the page, and the one red assertion grades a transition
on which the RTL already does what the page says.**

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, 6526 CIA specification, 1981**, PDF page 6 — agrees with the manual:

  > "Any negative transition on FLAG will set the FLAG interrupt bit."
  <!-- corpus: cia-6526-spec/p006.txt -->

  The datasheet states the manual's p.341 rule almost word for word, 6526 where the manual has 8520: FLAG is 'a negative edge sensitive' input and a transition, not a level, sets the FLAG bit. It also has a read of the ICR data register clear it (p007), and on its edge rule a held level would not set the bit again; it is a 6526 document and does not itself speak for the 8520.

Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

