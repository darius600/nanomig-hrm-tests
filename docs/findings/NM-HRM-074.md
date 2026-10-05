# NM-HRM-074 — PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `cia_timera.v`, `cia_timerb.v`, `ciab.v`, `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L39-L40) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L54-L55) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** and **minimig-dev/MinimigAGA-MiST-TC64**. NanoMig has the four modules from the Minimig line; MiSTer's own comments mark both bits not implemented and TC64 carries the same statements, so there is no fix to port — see *What the others say* |
| **Test** | [`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments) — `PROP-CRA-PBON`, claims `sol:F-cra.c4`, `opus:F-cra.c4` (p. 348), and `PROP-CRA-OUTMODE`, claims `sol:F-cra.c5`, `opus:F-cra.c5` (p. 348), both red on the `ciab` fixture; [`integration/appF-cia-system`](../../integration/appF-cia-system) — `P1-MAIN-CRA`, claim `sol:F-cra.c17` (p. 349), `P1-MAIN-CRB`, claims `sol:F-crb.c3`, `opus:F-crb.c2` (p. 349) and `sol:F-crb.c14` (p. 350), `P1-MAIN`, claims `opus:F-timers.c10`, `opus:F-timers.c11`, `sol:F-timers.c12` (p. 342) and `opus:F-ports.c7`, `sol:F-ports.c7` (p. 341), `P2-TOGGLE`, claim `opus:F-timers.c14` (p. 342), and `P3` (toggle mode, pp. 342, 348–350), which carries no claim id; each red on fx68k and on TG68K and each graded on CIA-B only: CIA-A's PRB read never shows a driven level ([NM-HRM-054](NM-HRM-054.md)), which the package records instead of grading. Three more lines of the package are red on both cores and are not counted among the reds that carry the finding: `P4` (pulse mode, pp. 341, 342), `P9-LEGF-TRANSITIONS` (pp. 341, 342) and `P9-LEGW` (p. 341), none of which has a green control through its own comparator, see *Measured*. The claim ids of `P3`, `P4` and `P9` sit on the package's P10 rows, which join these lines and measure nothing of their own — see *Measured* |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 339, 341, 342, 348, 349, 350, 351, 352, 354; Appendix E, printed p. 337 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | `hrm/appF-ciab-port-assignments`: 2 red (`PROP-CRA-PBON`, `PROP-CRA-OUTMODE`; 7 passed, 3 failed, the third red being [NM-HRM-063](NM-HRM-063.md)'s). `integration/appF-cia-system`: 61 passed, 26 failed on each core, the same lines on both, and 11 of the 26 reds are this finding's lines. Seven are in P1, P2 and the environment checks, the first 26 graded lines (`FIXTURE` to `F1-ANCHOR-PER-RUN`; 19 passed, 7 failed): the four named in *Test* and `P2-TOGGLE`'s three component lines, which carry no claim id — `P2-LEGR` (low after RES), `P2-MAIN` (high after each start from a level of 0) and `P2-LEGH` (high after a start from high). The eighth red line that carries the finding is `P3`. The other three, `P4`, `P9-LEGF-TRANSITIONS` and `P9-LEGW`, are not counted: `P4` has no green control through its pulse comparator, `P9-LEGF-TRANSITIONS` none through its transition-spacing comparator, and `P9-LEGW` does not assert that its own timer underflowed and has no built control. Nine of the package's P10 rows are red through these eleven lines. Six are red through lines that carry the finding, four through them alone (`sol:F-timers.c13`; `opus:F-timers.c12, opus:F-timers.c13`; `sol:F-cra.c16`; `opus:F-crb.c3, sol:F-crb.c4, sol:F-crb.c13`) and two, `opus:F-cra.c13` and `opus:F-crb.c14`, through lines of [NM-HRM-078](NM-HRM-078.md) as well. Three, `opus:F-cra.c12`, `opus:F-crb.c13` and `opus:F-timers.c6, sol:F-timers.c8`, are red only through `P4` and `P9-LEGW` and are not counted. The other six reds are NM-HRM-078's lines and rows and `P7.6-CIAB` ([NM-HRM-086](NM-HRM-086.md)). P1 to P4 are graded on CIA-B's two instances, where `P1-PRE1.4/O1` holds (with DDRB bit n = 1 and PBON off, PRB reads the data bit, 0 and then 1), and not on CIA-A's, where it fails and is recorded; P9 runs on CIA-B's timer B alone. In the same run `P1-CONTROL` — the PBON = 0 run, graded against the page's PBON = 0 values through the same comparator — and `P1-OTHERBITS` are green on CIA-B, as are `P1-PRE1.1`–`P1-PRE1.3`, `P2-PRE2.2`–`P2-PRE2.4`, `P3-PRE`, `P4-PRE` and `P9-LEGF`; `P2-PRE2.1`, `P2-CONTRAST` and `P2-LEGH-CONTRAST` are recorded rather than graded, because this build already reads what each contrast predicts |
| **Verdict of the four** | **unanimous against NanoMig on PBON; split on p. 342's toggle and pulse clauses, which the silicon follows and none of the four follows in full.** WinUAE, vAmiga, Denise and Copperline all put a timer output on PB6 or PB7 when PBON is set, chosen by OUTMODE; none leaves the two bits without effect. vAmiga and Denise pulse high for one cycle, WinUAE shows 0 in pulse mode and Copperline pulses low; vAmiga, Denise and Copperline invert the toggle at each underflow, where WinUAE shows the START bit; Denise's and Copperline's toggle is high after reset, and Copperline's is not set on a start; vAmiga's CIA-B lets DDRB keep an input line at its pull-up. Outside the emulators, the 8520R4 dissection's netlist gives a positive pulse, a toggle low after RES, high after a start and inverted at every underflow, and a line driven while PBON or the DDRB bit is set, as p. 342 does — see *What other Commodore documents say* |
| **See also** | [NM-HRM-054](NM-HRM-054.md) (CIA-A's port B has no data register, so a PRB read of CIA-A returns the joystick-adapter inputs and cannot show PB6 or PB7 either), [NM-HRM-063](NM-HRM-063.md) (FLAG, the other red of the same unit package) |

---

## What the HRM says

**Printed p. 341**, *I/O Ports*, gives PB6 and PB7 a second job:

> In addition to their normal I/O operations, ports PB6 and PB7 also provide timer output functions.

**Printed p. 342** says what the job is, in two of the timer's control
functions:

> PB on/off
>
> A control bit allows the timer output to appear on a port B output line (PB6 for timer A and PB7
> for timer B). This function overrides the DDRB control bit and forces the appropriate PB line to
> become an output.

> Toggle/pulse
>
> A control bit selects the output applied to port B while the PB on/off bit is ON. On every timer
> underflow, the output can either toggle or generate a single positive pulse of one cycle duration.
> The toggle output is set high whenever the timer is started, and set low by RES.

**Printed p. 348**, *Control Register A*, names the two bits:

> 1 PBON    1 = Timer A output on PB6, 0 = PB6 is normal operation.
>
> 2 OUTMODE 1 = toggle, 0 = pulse.

**Printed p. 349**, *Control Register B*, the same for timer B:

> 1 PBON 1 = Timer B output on PB7, 0 = PB7 is normal operation.

and its OUTMODE line repeats p. 348's. The two bitmaps, CRA's on p. 349 and
CRB's on p. 350, read from the page images, not quoted, give the same columns:
PBON `0=PB6OFF` / `1=PB6ON` and `0=PB7OFF` / `1=PB7ON`, OUTMODE `0=pulse` /
`1=toggle`.

**What a program sees of the line.** A PRB read returns the pins, and an
undriven pin reads high — p. 341:

> When you READ a PR register, you read the actual current state of the I/O pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or outputs.

> Ports A and B have passive pull-up devices as well as active pull-ups, providing both CMOS and
> TTL compatibility.

and **printed p. 354**, on RES:

> The port pins are set as inputs and port registers to zero (although a read of the ports will return all highs because of passive pull-ups). The timer control registers are set to zero and the timer latches to all ones.

So after reset PBON is off, and once a program sets it, a PRB read shows the
timer's output in bit 6 or bit 7 whatever DDRB and the data register say.

**Read with the rest of Appendix F, nothing predicts a PBON without effect.**
p. 339 opens the appendix with the chip's capabilities and then "In the Amiga
system various tasks are assigned to the chip's capabilities as follows", and
the assignments, p. 351 and Appendix E's p. 337, give CIA-A's PB6 and PB7 to
Centronics data bits 6 and 7 and CIA-B's to SEL3* and MTR*. Neither page
withdraws the timer function from those lines. p. 352's timing example clears
bits 1 and 2 of CRA and polls ICR for its one-shot timer; it does not use the
port output and says nothing about it.

---

## What NanoMig does

**The two bits are latched with the rest of the control register and read
back, and nothing else reads them.** Timer A's control register —
[`src/minimig-aga/cia_timera.v#L39-L40`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L39-L40):

```verilog
    else if (tcr && wr)  // load control register, bit 4(strobe) is always 0
      tmcr[6:0] <= {data_in[6:5],1'b0,data_in[3:0]};
```

The only bits the module uses are 0, 3 and 6 —
[`#L52-L54`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L52-L54),
`assign oneshot = tmcr[3];`, `assign start = tmcr[0];`,
`assign spmode = tmcr[6];` — and the whole register goes back out on a read,
[`#L106-L108`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L106-L108),
`| ({8{~wr&tcr}} & {1'b0,tmcr[6:0]});`. The underflow leaves the module only as
the interrupt and as timer B's count input,
[`#L99-L103`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L99-L103),
`assign tmra_ovf = underflow;` and `assign irq = underflow;`; in CIA-A,
`tmra_ovf` also clocks the serial port's output,
[`ciaa.v#L213`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L213). There is no toggle
flip-flop, no pulse, and no output port for either.
`cia_timerb.v` is the same: it stores the register at
[`#L38-L39`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L38-L39),
uses bits 0, 3 and 6 at
[`#L31`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L31),
`assign count = tmcr[6] ? tmra_ovf : eclk;`, and
[`#L51-L52`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L51-L52),
`assign oneshot = tmcr[3];` / `assign start = tmcr[0];`, and reads it back at
[`#L101-L103`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L101-L103).

**Port B is the data register through DDRB, with no timer term.** CIA-B's one
driver of the port and its PRB read —
[`src/minimig-aga/ciab.v#L153-L164`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L153-L164):

```verilog
always @(*)
begin
  if (!wr && prb)
    pb_out[7:0] = portb_out[7:0];
  else if (!wr && ddrb)
    pb_out[7:0] = ddrportb[7:0];
  else
    pb_out[7:0] = 8'h00;
end

// assignment of output port while keeping in mind that the original 8520 uses pull-ups
assign portb_out[7:0] = (~ddrportb[7:0]) | regportb[7:0];
```

`portb_out` is what the floppy sees as MTR*, SEL3* to SEL0*, SIDE*, DIR and
STEP*,
[`src/minimig-aga/minimig.v#L650`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L650),
`.portb_out({_motor,_sel3,_sel2,_sel1,_sel0,side,direc,_step})`, so with PBON
set neither the program nor the drive sees a timer on PB6 or PB7.

CIA-A has no port-B data register at all ([NM-HRM-054](NM-HRM-054.md)); a PRB
read returns `portb_in`,
[`src/minimig-aga/ciaa.v#L293-L294`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L293-L294),
`pb_out[7:0] = portb_in[7:0];`, which is joystick adapter ports 3 and 4,
[`src/minimig-aga/minimig.v#L624`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L624),
and NanoMig's top level ties those to their idle level,
[`src/nanomig.v#L1373-L1374`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1373-L1374),
`wire [15:0] JOY2 = 16'h0000;` / `wire [15:0] JOY3 = 16'h0000;`, inverted into
`_joy3` and `_joy4`. The file's own header lists the gap —
[`src/minimig-aga/ciaa.v#L23-L27`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L23-L27):

```verilog
// NOT implemented is:
// serial data register for CIA B(but keyboard input for CIA A is supported)
// port B for CIA A
// counter inputs for timer A and B other then 'E' clock
// toggling of PB6/PB7 by timer A/B
```

### Measured

**At the `ciab` module.**
[`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments)
builds `ciab` alone with its four submodules and reads `portb_out` directly;
it reads `7 passed, 3 failed`.

- `PROP-CRA-PBON` (`test_appF-ciab-port-assignments.cpp:646-658`) writes
  DDRB = $FF, PRB = $00, a timer A latch of 1 and CRA = $03 (START, PBON,
  pulse, continuous), runs two underflows and requires bit 6 of `portb_out` to
  have moved. It has not: red.
- `PROP-CRA-OUTMODE` (`test_appF-ciab-port-assignments.cpp:672-693`) samples
  bit 6 before and after each of three underflows once with OUTMODE = 1 and
  once with OUTMODE = 0, and requires the two waveforms to differ. They are
  identical: red.
- The channel is live: `FIXTURE-LIVE (portb)` writes $A5 and then $5A to PRB
  with DDRB = $FF and reads both back on `portb_out`, bit 6 included. Each
  assertion's oracle mutation (`MUT_A8_CRA_PBON`, `MUT_A9_CRA_OUTMODE`), which
  expects the unchanged bit, turns it green.

**At the system's ports, on both CPU cores.**
[`integration/appF-cia-system`](../../integration/appF-cia-system) runs 68000
programs on the whole of `nanomig.v`, on fx68k and on TG68K, and reads PRB
through the CPU, the channel p. 341 describes. It reads 61 passed, 26 failed on
each core, the same lines on both, and eleven of the red lines are this
finding's. Its first 26 graded lines, P1, P2 and the environment checks
(`FIXTURE` to `F1-ANCHOR-PER-RUN`), read 19 passed, 7 failed. `P1`
grades p. 342's override for each timer, timer A on PB6 and timer B on PB7:
`P1-MAIN-CRA` on the timer-A instances, `P1-MAIN-CRB` on the timer-B instances,
and `P1-MAIN` on both, the other bits included. `P2` grades the toggle output's
level after RES (`P2-LEGR`), after each start from low (`P2-MAIN`) and after a
start from high (`P2-LEGH`), and `P2-TOGGLE` the three together. Each is graded
only where a PRB read shows a driven level at all: `P1-PRE1.4/O1` requires
PRB, with DDRB bit n = 1 and PBON off, to read the data bit, 0 and then 1. It
holds on CIA-B's two instances and fails on CIA-A's.

What `P1`'s legs read, per instance, as the package prints them on both cores
(bit n is 6 for timer A and 7 for timer B):

| leg | CR | DDRB bit n, data bit n | the page gives bit n | CIA-B timer A | CIA-B timer B | CIA-A, both timers |
|---|---|---|---:|---:|---:|---:|
| r0, reference | $00 | input | 1 | $FF | $FF | $FF |
| a2, pulse output, stopped | $02 | input | **0** | $FF | $FF | $FF |
| a1, toggle after one underflow | $0E | input | **0** | $FF | $FF | $FF |
| b1, over the data | $0E | output, 1 | **0** | $FD | $FD | $FF |
| c1, toggle started, no underflow | $07 | output, 0 | **1** | $BB | $7B | $FF |
| d1, PBON off | $00 | output, 0 | 0 | $BB | $7B | $FF |
| o1c, PBON off | $00 | output, 1 | 1 | $FB | $FB | $FF |
| d2, PBON off | $00 | input | 1 | $FB | $FB | $FF |

On CIA-B every leg with PBON set reads what DDRB and the data register give —
the pull-up in a2 and a1, the data in b1 and c1 — and the three legs with PBON
off read as the page predicts. The ICR flags show the timers running: one
underflow before a1 and none during c1 on every instance (`P1-PRE1.2`,
`P1-PRE1.3`, green). On CIA-A every read is $FF, the joystick-adapter inputs,
so d1 misses too; CIA-A cannot show PB6 or PB7 at this boundary for the
reason [NM-HRM-054](NM-HRM-054.md) gives, and CIA-B's legs are the ones that
measure PBON. The same programs with PBON = 0 throughout read, on CIA-B, the
PBON = 0 values the page gives — the pull-up in a2 and a1, the data in b1 and
c1 — and `P1-CONTROL`, which grades that run through the same comparator, is
green.

`P2`'s programs set PBON with the toggle output (CR = $06) as the first write
to the timer's control register after reset and read PRB with the line an
input (r1). With DDRB bit n = 1 and the data bit 0 they then read PRB (s0) and
start a one-shot timer four times, sampling PRB 64 times after each start and
reading it again once the one-shot has ended (z1–z4). Last they start a long
timer, stop it and start it again, reading PRB after each of the three writes
(h1–h3). On CIA-B, r1 reads bit n high where p. 342 gives low after RES
(`P2-LEGR`, red); s0, z1–z4 and all 64 samples after each of the eight starts
read bit n at the driven 0, so no start raises the line (`P2-MAIN`, red); and
h1, h2 and h3 read the same 0, where p. 342 gives high after each start
(`P2-LEGH`, red).
The ICR reads show every one-shot started and ended (`P2-PRE2.2`) and no
underflow during the long timer (`P2-PRE2.4`), both green. On CIA-A every read
is $FF, so no start there has the level of 0 before it that `P2-MAIN` requires,
and r1 and h1–h3 are not graded there.

`P3` (`test_appF_cia_system.cpp:1966-2025`) starts each timer in toggle mode
with PBON (CR = $07) over a latch of 60, with DDRB bit n = 1 and the data bit
0, and reads PRB and ICR 600 times each; p. 342 has the line change level at
every underflow and at no other time, and be high before the first. On CIA-B's
two instances ICR flags 88 underflows, and PRB reads the same byte in all 600
samples, $BF for timer A and $7F for timer B: bit n stays at the driven 0 and
never changes, and the line is red: 88 underflow flags against no transition at
all is the clause "on every timer underflow, the output can either toggle", which
`P3` carries. `P3` also requires the level before the first flag to be high,
a conjunct its control does not run (see *The control*). `P3-PRE`, the underflows at least three
samples apart and at least 16 of them, is green. `P9-LEGF-TRANSITIONS`
(`:2217-2247`) is the same program on timer B with a latch of 40, and requires
consecutive transitions one underflow period apart: by p. 341 the latch is "a
divider of the input clocking frequency", and the toggle's frequency follows
it. It reads the same way, 131 underflows and bit 7 at 0 in all 600 samples,
and is red. It is not counted among the reds that carry the finding: no input
is known on which its transition-spacing comparator reads green, since
`P3-GREENCTL` runs the pairing comparator and not this one. In the same runs `P9-LEGF` finds the underflow period unique and
equal to L + 1 for latches 40, 41, 64 and 100, and is green: the timer runs at
the rate its latch sets, and only its output is missing.

`P4` (`:2070-2167`) starts each timer in pulse mode with PBON (CR = $03) and
DDRB = $00, once for each latch from 20 to 25, and reads PRB and ICR 400 times
each; p. 342 has the line low except for one φ2 cycle at every underflow.
`P4-PRE` is green: in every run the underflow period is unique and equal to
L + 1, and in four of the six runs of each instance the samples cover every
residue of it. On CIA-B PRB reads $FF in all 400 samples of every run, the
pull-up of a line DDRB leaves an input, so every residue reads high and not a
single one, and the line is red. It is not counted among the reds that carry
the finding: the package records that no input is known on which its
one-residue, one-cycle pulse comparator reads green (see *The control*), so
nothing shows that the comparator can be satisfied at this sampling
resolution.

`P9-LEGW` (`:2249-2280`) starts CIA-B's timer B as a one-shot in toggle mode
with PBON (CR = $0F), DDRB bit 7 = 1 and the data bit 0, once with a latch of
40 and once with 100, and reads PRB 200 and 300 times. It grades p. 341's
"variable width pulses" as the difference of the two widths, each from the
start to the first sample at which bit 7 has fallen back to 0, against
60 ± s E periods. Bit 7 reads 0 in every sample of both runs ($7F): it never
rises, neither width exists (the package prints both widths as undefined), and
the line is red. It is not counted among the reds that carry the finding: the
two runs do not assert that their own timer underflowed, so an undefined width
could come from a missing underflow or window as well as from a missing output,
and the control named for it is not built. It is `P2-MAIN`'s
observation, no start raising the line, at two more latches, as
`P9-LEGF-TRANSITIONS` is `P3`'s at another; neither adds a measurement of its
own.

On CIA-A, `P3` and `P4` take `P1-PRE1.4/O1` as their gate, fail it, and are
recorded rather than graded.

**The control.** `P3-GREENCTL` (`:2057-2066`), which the package runs only
when `NMH_MUTATE=P3-GREENCTL` selects it, puts a line the CPU drives through
`P3`'s pairing comparator: on CIA-B's timer B, with the same latch, PBON off
and DDRB bit 7 an output, the program inverts PRB bit 7 itself after every ICR
read that shows the TB flag. It is green on fx68k and on TG68K, and every
other line of those runs reads as in the normal run: the sampler and the
comparator see a bit 7 that moves at each underflow. The control grades the
pairing, not the level before the first flag. `P4` has none: the package
records that a CPU write lands at the sampling loop's own granularity, 9 E
periods, and cannot sit on one residue as a one-cycle pulse would. For
`P9-LEGW` it names one, a width the CPU drives, and does not build it, and
`P9-LEGF-TRANSITIONS` has none.

**The claim rows.** The package's P10 rows carry the claim ids of P2, P3, P4
and P9 (`:2296-2371`). Each joins the lines named for its claim and measures
nothing of its own, so none is evidence beside those lines. Four are red
through lines that carry this finding alone: `sol:F-timers.c13` (`P2-TOGGLE`
and `P3`); `opus:F-timers.c12, opus:F-timers.c13` (`P3`); CRA's OUTMODE,
`sol:F-cra.c16` (`P3`, timer A); and CRB's, `opus:F-crb.c3, sol:F-crb.c4,
sol:F-crb.c13` (`P3`, timer B). Each of these has `P4` as a further member,
and none depends on it. The bitmaps' 1-settings, `opus:F-cra.c13` and
`opus:F-crb.c14`, are red through `P1-MAIN-CRA` or `P1-MAIN-CRB` and `P3`, and
also through the INMODE lines of [NM-HRM-078](NM-HRM-078.md) and, for CRA,
through `P7.6-CIAB` ([NM-HRM-086](NM-HRM-086.md)). Three rows are not counted:
the bitmaps' 0-settings, `opus:F-cra.c12` and `opus:F-crb.c13`, whose only red
member is `P4` (OUTMODE = 0, pulse), and `opus:F-timers.c6, sol:F-timers.c8`,
whose red members are `P4` and `P9-LEGW`. A row is evidence on this page only
through a member that carries the finding.

---

## What the others say

The question put to each: with PBON set, does a PRB read show the timer's
output in bit 6 or 7, and what does that output do at RES, at a start and at an
underflow?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1868-1887`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1868-L1887), [`:1977-1981`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1977-L1981), [`:2079-2083`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2079-L2083), [`:2296-2299`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2296-L2299), [`cfgfile.cpp:9954`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9954) | `CIA_PBON` replaces bit 6 when CRA's PBON is set and bit 7 when CRB's is, in the value the port access has already formed: `v &= ~0x40;` / `v \|= pb6 ? 0x40 : 00;`. In toggle mode the level is the START bit, `pb6 = c->t[0].cr & CR_START;`, and in pulse mode it stays 0. Both PRB reads pass through it, `tmp = CIA_PBON(c, tmp);` on CIA-A and on CIA-B, and so does the value a CIA-B PRB write hands the drives, `val = CIA_PBON(c, val);` then `DISK_select(val);`. **PBON overrides DDRB and the data** in the default CIA type; the other type, `391078-01`, which the A600 preset selects, `p->cs_ciatype[0] = p->cs_ciatype[1] = 1;`, puts the data register back on every line DDRB makes an output after `CIA_PBON`, `tmp &= ~c->drb;` / `tmp \|= c->prb & c->drb;`. The toggle is high while the timer runs and low after RES and after a one-shot ends, but a continuous timer's underflows do not invert it and pulse mode has no pulse |
| **vAmiga** | [`Core/Components/CIA/CIARegs.cpp:385-416`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L385-L416), [`Core/Components/CIA/CIA.cpp:556-608`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L556-L608), [`:914-934`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L914-L934), [`:1066-1079`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1066-L1079), [`:1107-1119`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1107-L1119), [`:55-57`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L55-L57), [`Core/Components/CIA/CIA.h:408-410`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.h#L408-L410), [`CIARegs.cpp:269-281`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L269-L281), [`Core/Infrastructure/Defaults.cpp:179`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L179) | a start sets the toggle, `pb67Toggle \|= 0x40;`, and PBON routes it, `pb67TimerMode \|= 0x40;`. At an underflow the toggle inverts, `pb67TimerOut ^= 0x40;`, or in pulse mode goes high, `pb67TimerOut \|= 0x40;`, and is cleared again a cycle later, `if (delay & CIAPB6Low1) { pb67TimerOut &= ~0x40; }`; a reset clears `pb67Toggle` with the rest of the serialised state. The one-shot start a timer-high write makes sets `pb67Toggle \|= 0x40;` but not `pb67TimerOut`, the bit the port shows, so on that start the line does not rise. CIA-A applies the timer bit after DDRB, `REPLACE_BIT(result, 6, pb67TimerOut & (1 << 6));` following `u8 result = (internal & ddrb) \| (external & ~ddrb);`, so **PBON overrides DDRB and the data**. CIA-B puts it into the data before DDRB, in `portBinternal`, and then applies `u8 result = (internal & ddrb) \| (external & ~ddrb);`: on CIA-B the timer replaces the data bit but **a line DDRB leaves an input reads its pull-up**. This is the default revision, `MOS_8520_DIP`; the PLCC option puts the data register back on every line DDRB makes an output, `result = (result & ~ddrb) \| (prb & ddrb);` |
| **Denise** | [`emulation/cia/new/register.cpp:84-89`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L84-L89), [`:445-470`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L445-L470), [`:368-369`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L368-L369), [`emulation/cia/new/cia.cpp:266-284`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L266-L284), [`:48-58`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L48-L58) | a PRB read, `uint8_t out = readPort(PORTB, &lines);`, then `adjustBit6And7(out);`, which clears the bit and sets it from the toggle or from the one-cycle underflow stage, `if ( ( (timerA.control & 4) ? timerA.toggle : (delay & CIA_UF_TA1) ) )`; the port lines the drives see take the same adjustment in `updatePortB`. A start sets the toggle, `timerA.toggle = true;`, and an underflow inverts it, `timerA.toggle ^= 1;`. **PBON overrides DDRB and the data, the toggle inverts at each underflow and is set on a start, and the pulse is high for one cycle**; but the reset sets the toggle high too, `timerA.toggle = true;` beside `timerA.control = 0;`, where p. 342 sets it low |
| **Copperline** | [`src/chipset/cia.rs:483-500`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L483-L500), [`:1001-1004`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1001-L1004), [`:1014-1032`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1014-L1032), [`:721-732`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L721-L732), [`:201-202`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L201-L202), test [`:1565-1577`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1565-L1577) | `port_b_pins` sets or clears bit 6 when CRA's PBON is set, `if self.regs[REG_CRA] & CR_PBON != 0 {`, after DDRB and the data, and a guest PRB read returns it, `self.port_b_pins()`. At an underflow toggle mode inverts the level, `self.ta_pb_output_high = !self.ta_pb_output_high;`, and pulse mode drops it, `self.ta_pb_output_high = false;`, for one E-clock, under the comment *"PBON pulse mode holds PB6/PB7 low for one E-clock."* The level starts as `ta_pb_output_high: true,` and no control-register write changes it, and an underflow changes it only while PBON is set, `if control & CR_PBON == 0 {`. **PBON overrides DDRB and the data and the toggle inverts at each underflow**; but the toggle is high after reset and is not set by a start, and the pulse is low where p. 342 gives a positive one. Its own test, `timer_a_toggle_output_overrides_pb6_pin`, grades the override with DDRB = $00 |
| **MiSTer** | [`rtl/cia_timera.v:54-55`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L54-L55), [`rtl/cia_timerb.v:54-55`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerb.v#L54-L55), [`rtl/ciab.v:214`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L214), [`rtl/ciaa.v:27`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L27) | descendant — the same gap, now named in the timer modules' comments: `// Bit 1: PBON - PB6 output mode (not implemented in simplified version)` and `// Bit 2: OUTMODE - PB6 output mode (not implemented)`, the same for PB7; port B is `assign portb_out[7:0] = (~ddrportb[7:0]) \| regportb[7:0];`, and CIA-A's header keeps `// toggling of PB6/PB7 by timer A/B` under "NOT implemented". The files are rewritten around it — 169 lines of `cia_timera.v` differ from NanoMig's — and none of the rewrite reaches the two bits |
| **TC64** | [`rtl/minimig/cia_timera.v:40`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timera.v#L40), [`:115`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timera.v#L115), [`rtl/minimig/ciab.v:164`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L164), [`rtl/minimig/ciaa.v:27`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L27) | descendant — the same statements as NanoMig: `tmcr[6:0] <= {data_in[6:5],1'b0,data_in[3:0]};` stores the register and `\| ({8{~wr&tcr}} & {1'b0,tmcr[6:0]});` reads it back, with nothing else on bits 1 and 2; port B is `assign portb_out[7:0] = (~ddrportb[7:0]) \| regportb[7:0];`, and CIA-A's header says `// toggling of PB6/PB7 by timer A/B` is not implemented. TC64 is the closer sibling here, 6 lines apart from NanoMig on `ciab.v` and 11 on `cia_timera.v`, none of them on these bits |

**All four put a timer output on the line; none of them does what NanoMig
does.** Each routes PBON to bit 6 or 7 of a PRB read and lets OUTMODE choose
the output, and each also hands CIA-B's level to its drives when port B is
written. Where they part is how far they follow p. 342, and on each of those
clauses the 8520R4 dissection's netlist reads as p. 342 does (*What other
Commodore documents say*): the line is driven while PBON is set whatever DDRB
holds, the pulse is positive, and the toggle is low after RES, high after a
start and inverted at every underflow. WinUAE models the toggle as the running
state and has no pulse; vAmiga and Denise model the toggle, the start and the
one-cycle pulse, Denise with the toggle high after reset and vAmiga without
raising the line on a one-shot start made by a timer-high write; Copperline
inverts at underflows, and only while PBON is set, but starts high, ignores
starts and pulses low; and vAmiga's CIA-B lets DDRB keep an input line at its
pull-up, where p. 342 says the function overrides DDRB and the dissection
drives the pad while PBON or the DDRB bit is set. Read against `P1`'s legs by
their code, not run: WinUAE and Denise would read every PBON leg as p. 342
predicts, vAmiga on CIA-A but not in CIA-B's input legs a2 and a1, and
Copperline not in a2, where its idle level is high, nor in c1, where a start
does not raise it. Against `P2-LEGR`, Denise and Copperline would read the
line high after reset, and vAmiga's CIA-B instances high through the pull-up.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 218 and 222 — agrees with the manual:

  > "This function overrides the DDRB control bit and forces the appropriate PB tine to an output."
  <!-- corpus: a500-a2000-trm-1987/p218.txt -->

  > "On every timer underflow the output can either toggle or generate a single positive pulse of one cycle duration. The toggle output is set high whenever the timer is started and is set low by RES."
  <!-- corpus: a500-a2000-trm-1987/p218.txt -->

  > "1 PBON 1=TIMER A output appears on PB6, O = PB6 normal operation"
  <!-- corpus: a500-a2000-trm-1987/p222.txt -->

  The manual's 8520 section gives p. 342's PB on/off and toggle/pulse paragraphs in the same words ("tine" is the OCR's reading of "line"), and a CRA bit 1 that puts timer A on PB6. It is Commodore's description of the chip fitted to the A500 and A2000, so it documents the function for those boards.

- **Commodore, 6526 CIA specification, 1981**, PDF pages 6 and 8 — agrees with the manual:

  > "1 PBON 1=TIMER A output appears on PB6, 0=PB6 normal operation."
  <!-- corpus: cia-6526-spec/p008.txt -->

  The 6526 specification gives CRA bit 1 and bit 2 (`1=TOGGLE, O=PULSE`) as the manual does. Its PDF page 6 prints the PB On/Off and Toggle/Pulse paragraphs as p. 342 does, the override of DDRB and the toggle output set high on a start and low by RES among them; the OCR interleaves that page's two columns, so those paragraphs are not quoted here. It is a 6526 document and does not itself speak for the 8520.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheets 8e, 4c and 9e and the overview on sheet 1 — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 8e, drawn transistor by transistor, forms TA_Q, the signal it sends "to PB6 pad driver". A pass switch driven by CRA2 takes it from a latch that RES sets, that a rising edge of the start bit (TA_CEN, CRA0 as sampled) clears, and that flips at each underflow pulse TA_INT; CRA1 does not reach the latch. A pass switch driven by CRA2's complement takes it from an inverter of TA_INT. Through the inverting output stage TA_Q is TA_INT in pulse mode and the latch's complement in toggle mode: low after RES, high after a start, inverted at every underflow. TA_INT is TAH_TC, the underflow, latched on PHI1; TAH_TC enters the counter-load NOR beside RES, so both are active high. The overview on sheet 1 ORs SET_CRA0 into CRA0's latch, and SET_CRA0 is a TAH write in one-shot mode; the sheet's note reads "in one_shot mode: TAH write sets CRA0, what starts timer A". Sheet 4c, drawn transistor by transistor for PB7 and annotated "PB6: CRA1" and "PB6: TA_Q" for PB6, has a pass switch driven by CRB1 put TB_Q where the PRB bit otherwise goes, and enables the pad's pull-down while CRB1 or the DDRB bit is set; the pad then follows the selected signal, its high level from a passive pull-up. Sheet 9e is "layout identical to 8e)".
  <!-- drawing: cia-8520r4-dissection/sheet_012.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_005.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_013.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_001.txt -->

  PBON puts the timer's output on PB6 or PB7 and makes the line an output whatever DDRB holds, and OUTMODE chooses between the toggle and the underflow pulse, as pp. 342 and 348 to 350 print. The polarities are p. 342's as well: a positive pulse, and a toggle set low by RES and high by a start. A start here is a rising edge of CRA0, so the one-shot start a timer-high write makes counts when the timer was stopped, and a START = 1 write to a running timer does not; p. 342 does not say either. The sheet's own notes, 'CRA2=0: low pulse' and 'RES forces toggle to 1', read the other way. The toggle note matches the concept drawing, 'different from original circuitry', which takes TA_Q from the latch node without the inverting output stage of the netlist; the pulse note matches neither the concept drawing nor the netlist. The pulse's length is not traced. PB6 is read from PB7's cell by its annotation, and timer B's control logic from timer A's by the note on sheet 9. The dissection is of one revision.

---

## Suggested change

**One option: give each timer a port-B output and let PBON select it, as the
8520R4 dissection draws it.** In `cia_timera.v` and `cia_timerb.v`, a toggle
flip-flop cleared by `reset`, set on every rising edge of `tmcr[0]`, and
inverted on every `underflow`, whatever `tmcr[1]` holds. A rising edge of
`tmcr[0]` covers a control-register write that starts a stopped timer and the
one-shot start a timer-high write makes,
[`cia_timera.v#L41-L42`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L41-L42),
`else if (thi_load && oneshot)` / `tmcr[0] <= 1'd1;`, and leaves out a
START = 1 write to a running timer, as the dissection's edge detector does.
Beside it, a pulse output that is `underflow` held high for one `eclk` count;
`tmcr[2]` choosing between the two; and the result and `tmcr[1]` brought out
as two new ports. In `ciab.v`, bit 6 and bit 7 of `portb_out` then take the
timer output whenever the matching PBON is set, whatever `ddrportb` and
`regportb` hold, as the dissection's pad drives while PBON or the DDRB bit is
set; the PRB read, which returns `portb_out`, follows by itself. This has not
been run here. What it would change:

- `PROP-CRA-PBON`, `PROP-CRA-OUTMODE`, and every `P1`, `P2`, `P3`, `P4` and
  `P9` line graded on CIA-B would have a level to measure; `P2-PRE2.1`,
  `P2-CONTRAST` and `P2-LEGH-CONTRAST` would read differently from their baselines and could be
  graded;
- the drives would see the timer on SEL3* or MTR* when a program sets PBON on
  CIA-B, as the wiring of p. 351 implies;
- CIA-A would still read its joystick-adapter inputs on PRB: its PB6 and PB7
  show nothing until the port-B read path of [NM-HRM-054](NM-HRM-054.md)
  carries the pins, and until both change `P1-PRE1.4/O1` keeps CIA-A's
  instances recorded rather than graded, in `P3` and `P4` as in `P1` and `P2`.

---

## What holds the finding up

- **The page is plain and says it five times.** p. 341, p. 342, p. 348,
  p. 349 and p. 350's bitmap give the function to both timers of both CIAs,
  and no page of Appendix F takes it back for the Amiga's wiring. Commodore's
  own A500/A2000 manual and the 6526 specification repeat it, and the 8520R4
  dissection's netlist draws it, polarities included.
- **The RTL says so itself.** `ciaa.v`'s header lists "toggling of PB6/PB7 by
  timer A/B" as not implemented, MiSTer's timer modules call both bits "not
  implemented", and the only uses of `tmcr` in either timer module are bits 0,
  3 and 6 and the read-back.
- **The channels are live.** At the module, `FIXTURE-LIVE (portb)` carries two
  bytes through bit 6, and both reds turn green under their oracle mutations.
  At the system, the same CIA-B reads show the data in d1 and o1c and the
  pull-up in d2 as the page predicts, `P1-OTHERBITS` and `P1-CONTROL` are
  green, the ICR flags show every timer underflowing where the program
  expects, at the period its latch sets (`P9-LEGF`), and `P3`'s comparator
  is green on a bit 7 the CPU toggles at each underflow (`P3-GREENCTL`).
- **What weakens it.** CIA-A's instances cannot show the difference at all
  ([NM-HRM-054](NM-HRM-054.md)), so the package grades CIA-B's alone.
  `PROP-CRA-PBON` samples a pulse-mode output at one instant, which a correct
  one-cycle pulse passes only if the sample falls inside it; NanoMig's red does
  not depend on that, since no path exists. `P4` has no green control at the
  system's ports, `P9-LEGF-TRANSITIONS` none through its own comparator and
  `P9-LEGW` an unbuilt one, so the three are red lines that do not carry the
  finding; `P9-LEGW` and `P9-LEGF-TRANSITIONS` repeat `P2-MAIN`'s and
  `P3`'s observation rather than adding one. The finding stands on `P1`, `P2`
  and `P3` at the system's ports and on the two module-level assertions. And
  none of the four implements every clause of p. 342.

---

## For the specialists

1. **Does anything on an Amiga use PBON?** On CIA-B it would put a timer on
   SEL3* or MTR*, on CIA-A on Centronics data bits 6 and 7. The page gives the
   function without exception, and every independent implementation read here
   carries it; whether software depends on it decides how much the gap matters.
2. **The toggle's level after RES and at a start.** p. 342, the A500/A2000
   manual and the 6526 specification all say low after RES and high on every
   start, and the 8520R4 dissection's netlist does the same; its margin notes,
   'RES forces toggle to 1' and 'CRA2=0: low pulse', say otherwise, and the
   netlist does not bear them out. WinUAE does what the page says; vAmiga does
   too, except that a one-shot start made by a timer-high write does not raise
   the line; Denise sets the toggle high at reset, and Copperline starts it
   high and does not set it on a start. A fix in the Minimig line would do
   well to follow the page rather than any one of them.
3. **Which starts count.** p. 342 does not say whether the one-shot start that
   a timer-high write makes, or a START = 1 write to a running timer, sets the
   toggle. The dissection sets it on a rising edge of CRA0, which the first
   makes when the timer was stopped and the second never makes.
   `integration/appF-cia-system` leaves both ungraded.
