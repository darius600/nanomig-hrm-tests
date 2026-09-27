# NM-HRM-074 — PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `cia_timera.v`, `cia_timerb.v`, `ciab.v`, `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L39-L40) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L54-L55) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** and **minimig-dev/MinimigAGA-MiST-TC64**. NanoMig has the four modules from the Minimig line; MiSTer's own comments mark both bits not implemented and TC64 carries the same statements, so there is no fix to port — see *What the others say* |
| **Test** | [`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments) — `PROP-CRA-PBON`, claims `sol:F-cra.c4`, `opus:F-cra.c4` (p. 348), and `PROP-CRA-OUTMODE`, claims `sol:F-cra.c5`, `opus:F-cra.c5` (p. 348), both red on the `ciab` fixture; [`integration/appF-cia-system`](../../integration/appF-cia-system) — `P1-MAIN`, claims `opus:F-timers.c10`, `opus:F-timers.c11`, `sol:F-timers.c12`, `opus:F-ports.c7`, `sol:F-ports.c7`, `sol:F-cra.c17`, `sol:F-crb.c3`, `opus:F-crb.c2`, `sol:F-crb.c14` (pp. 341, 342, 349, 350), and `P2-LEGR`, `P2-MAIN`, claim `opus:F-timers.c14` (p. 342), each red on fx68k and on TG68K |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 339, 341, 342, 348, 349, 350, 351, 352, 354; Appendix E, printed p. 337 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | `hrm/appF-ciab-port-assignments`: 2 red (`PROP-CRA-PBON`, `PROP-CRA-OUTMODE`; 7 passed, 3 failed, the third red being [NM-HRM-063](NM-HRM-063.md)'s). `integration/appF-cia-system`: 3 red on fx68k and 3 on TG68K (`P1-MAIN`, `P2-LEGR`, `P2-MAIN`; 13 passed, 4 failed on each core, the fourth red, `P1-LP`, being the drive LED and not this finding). In the same run `P1-OTHERBITS` and `P1-PRE1.1`–`P1-PRE1.3` are green, and `P1-CONTRAST` and `P2-CONTRAST` are recorded rather than graded because the PBON = 0 and not-started runs read the same as the runs they contrast with |
| **Verdict of the four** | **unanimous against NanoMig on PBON, split on p. 342's toggle clauses.** WinUAE, vAmiga, Denise and Copperline all put a timer output on PB6 or PB7 when PBON is set, chosen by OUTMODE; none leaves the two bits without effect. How much of p. 342 they model differs: vAmiga and Denise invert the toggle at each underflow and pulse high for one cycle; WinUAE shows the START bit in toggle mode and 0 in pulse mode; Copperline inverts the toggle but starts it high after reset, does not set it on a start, and pulses low. Denise's toggle is high after reset as well, and vAmiga's CIA-B lets DDRB keep an input line at its pull-up |
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
`assign tmra_ovf = underflow;` and `assign irq = underflow;`. There is no toggle
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

- `PROP-CRA-PBON` (`test_appF-ciab-port-assignments.cpp:647-659`) writes
  DDRB = $FF, PRB = $00, a timer A latch of 1 and CRA = $03 (START, PBON,
  pulse, continuous), runs two underflows and requires bit 6 of `portb_out` to
  have moved. It has not: red.
- `PROP-CRA-OUTMODE` (`test_appF-ciab-port-assignments.cpp:673-694`) samples
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
through the CPU, the channel p. 341 describes. It reads `13 passed, 4 failed` on
each core, the two output files differing only in their `CORE:` line. `P1-MAIN`
grades p. 342's override on all four timers (CIA-A and CIA-B, timer A on PB6,
timer B on PB7); `P2-LEGR` and `P2-MAIN` grade the toggle output's level after
RES and after each start.

What `P1-MAIN`'s legs read, per instance — read for this page from the package's
own programs on its TG68K build, not a graded result (bit n is 6 for timer A and
7 for timer B):

| leg | CR | DDRB bit n, data bit n | the page gives bit n | CIA-B timer A | CIA-B timer B | CIA-A, both timers |
|---|---|---|---:|---:|---:|---:|
| r0, reference | $00 | input | 1 | $FF | $FF | $FF |
| a2, pulse output, stopped | $02 | input | **0** | $FF | $FF | $FF |
| a1, toggle after one underflow | $0E | input | **0** | $FF | $FF | $FF |
| b1, over the data | $0E | output, 1 | **0** | $FD | $FD | $FF |
| c1, toggle started, no underflow | $07 | output, 0 | **1** | $BB | $7B | $FF |
| d1, PBON off | $00 | output, 0 | 0 | $BB | $48 | $FF |
| d2, PBON off | $00 | input | 1 | $FB | $C8 | $FF |

On CIA-B every leg with PBON set reads what DDRB and the data register give —
the pull-up in a2 and a1, the data in b1 and c1 — and the two legs with PBON
off read as the page predicts. The ICR flags show the timers running: one
underflow before a1 and none during c1 on every instance (`P1-PRE1.2`,
`P1-PRE1.3`, green). On CIA-A every read is $FF, the joystick-adapter inputs,
so d1 misses too; CIA-A cannot show PB6 or PB7 at this boundary for the reason
[NM-HRM-054](NM-HRM-054.md) gives, and CIA-B's legs are the ones that measure
PBON. The same program with PBON = 0 throughout reads a2, a1, b1 and c1 bit for
bit as the PBON = 1 run does, which is why `P1-CONTRAST` is recorded and not
graded.

`P2`'s programs set PBON with the toggle output (CR = $06) as the first write
to the timer's control register after reset, read PRB (r1), then start a
one-shot timer four times, sampling PRB 64 times after each start. On all four
instances r1 reads $FF, bit n high where p. 342 gives low after RES
(`P2-LEGR`, red), and all 4 × 64 samples read $FF: no start raises the line,
because it never leaves the pull-up. The ICR reads after each one-shot carry
the timer's flag on every instance, so all sixteen started and underflowed;
`P2-PRE2.2`, green, shows that flag clear when the one-shot cannot end.
`P2-MAIN` takes the level before the first start to be leg R's 0 rather than
the r1 it read, so its red at the first start is not independent of
`P2-LEGR`'s; the 256 unchanging samples are.

---

## What the others say

The question put to each: with PBON set, does a PRB read show the timer's
output in bit 6 or 7, and what does that output do at RES, at a start and at an
underflow?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1868-1887`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1868-L1887), [`:1977`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1977), [`:2079`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2079), [`:2296-2299`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2296-L2299) | `CIA_PBON` replaces bit 6 when CRA's PBON is set and bit 7 when CRB's is, in the value the port access has already formed: `v &= ~0x40;` / `v \|= pb6 ? 0x40 : 00;`. In toggle mode the level is the START bit, `pb6 = c->t[0].cr & CR_START;`, and in pulse mode it stays 0. Both PRB reads pass through it, `tmp = CIA_PBON(c, tmp);` on CIA-A and on CIA-B, and so does the value a CIA-B PRB write hands the drives, `val = CIA_PBON(c, val);` then `DISK_select(val);`. **PBON overrides DDRB and the data**; the toggle is high while the timer runs and low after RES and after a one-shot ends, but a continuous timer's underflows do not invert it and pulse mode has no pulse |
| **vAmiga** | [`Core/Components/CIA/CIARegs.cpp:385-416`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L385-L416), [`Core/Components/CIA/CIA.cpp:556-608`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L556-L608), [`:914-934`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L914-L934), [`:1066-1079`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1066-L1079), [`:1107-1119`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1107-L1119), [`:55-57`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L55-L57), [`Core/Components/CIA/CIA.h:408-410`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.h#L408-L410) | a start sets the toggle, `pb67Toggle \|= 0x40;`, and PBON routes it, `pb67TimerMode \|= 0x40;`. At an underflow the toggle inverts, `pb67TimerOut ^= 0x40;`, or in pulse mode goes high, `pb67TimerOut \|= 0x40;`, and is cleared again a cycle later, `if (delay & CIAPB6Low1) { pb67TimerOut &= ~0x40; }`; a reset clears `pb67Toggle` with the rest of the serialised state. CIA-A applies the timer bit after DDRB, `REPLACE_BIT(result, 6, pb67TimerOut & (1 << 6));` following `u8 result = (internal & ddrb) \| (external & ~ddrb);`, so **PBON overrides DDRB and the data**. CIA-B puts it into the data before DDRB, in `portBinternal`, and then applies `u8 result = (internal & ddrb) \| (external & ~ddrb);`: on CIA-B the timer replaces the data bit but **a line DDRB leaves an input reads its pull-up** |
| **Denise** | [`emulation/cia/new/register.cpp:84-89`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L84-L89), [`:445-470`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L445-L470), [`:368-369`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L368-L369), [`emulation/cia/new/cia.cpp:266-284`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L266-L284), [`:48-58`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L48-L58) | a PRB read, `uint8_t out = readPort(PORTB, &lines);`, then `adjustBit6And7(out);`, which clears the bit and sets it from the toggle or from the one-cycle underflow stage, `if ( ( (timerA.control & 4) ? timerA.toggle : (delay & CIA_UF_TA1) ) )`; the port lines the drives see take the same adjustment in `updatePortB`. A start sets the toggle, `timerA.toggle = true;`, and an underflow inverts it, `timerA.toggle ^= 1;`. **PBON overrides DDRB and the data, the toggle inverts at each underflow and is set on a start, and the pulse is high for one cycle**; but the reset sets the toggle high too, `timerA.toggle = true;` beside `timerA.control = 0;`, where p. 342 sets it low |
| **Copperline** | [`src/chipset/cia.rs:483-500`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L483-L500), [`:1001-1004`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1001-L1004), [`:1014-1032`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1014-L1032), [`:721-732`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L721-L732), [`:201-202`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L201-L202), test [`:1565-1577`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1565-L1577) | `port_b_pins` sets or clears bit 6 when CRA's PBON is set, `if self.regs[REG_CRA] & CR_PBON != 0 {`, after DDRB and the data, and a guest PRB read returns it, `self.port_b_pins()`. At an underflow toggle mode inverts the level, `self.ta_pb_output_high = !self.ta_pb_output_high;`, and pulse mode drops it, `self.ta_pb_output_high = false;`, for one E-clock, under the comment *"PBON pulse mode holds PB6/PB7 low for one E-clock."* The level starts as `ta_pb_output_high: true,` and no control-register write changes it. **PBON overrides DDRB and the data and the toggle inverts at each underflow**; but the toggle is high after reset and is not set by a start, and the pulse is low where p. 342 gives a positive one. Its own test, `timer_a_toggle_output_overrides_pb6_pin`, grades the override with DDRB = $00 |
| **MiSTer** | [`rtl/cia_timera.v:54-55`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L54-L55), [`rtl/cia_timerb.v:54-55`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerb.v#L54-L55), [`rtl/ciab.v:214`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L214), [`rtl/ciaa.v:27`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L27) | descendant — the same gap, now named in the timer modules' comments: `// Bit 1: PBON - PB6 output mode (not implemented in simplified version)` and `// Bit 2: OUTMODE - PB6 output mode (not implemented)`, the same for PB7; port B is `assign portb_out[7:0] = (~ddrportb[7:0]) \| regportb[7:0];`, and CIA-A's header keeps `// toggling of PB6/PB7 by timer A/B` under "NOT implemented". The files are rewritten around it — 169 lines of `cia_timera.v` differ from NanoMig's — and none of the rewrite reaches the two bits |
| **TC64** | [`rtl/minimig/cia_timera.v:40`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timera.v#L40), [`:115`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timera.v#L115), [`rtl/minimig/ciab.v:164`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L164), [`rtl/minimig/ciaa.v:27`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L27) | descendant — the same statements as NanoMig: `tmcr[6:0] <= {data_in[6:5],1'b0,data_in[3:0]};` stores the register and `\| ({8{~wr&tcr}} & {1'b0,tmcr[6:0]});` reads it back, with nothing else on bits 1 and 2; port B is `assign portb_out[7:0] = (~ddrportb[7:0]) \| regportb[7:0];`, and CIA-A's header says `// toggling of PB6/PB7 by timer A/B` is not implemented. TC64 is the closer sibling here, 6 lines apart from NanoMig on `ciab.v` and 11 on `cia_timera.v`, none of them on these bits |

**All four put a timer output on the line; none of them does what NanoMig
does.** Each routes PBON to bit 6 or 7 of a PRB read and lets OUTMODE choose
the output, and each also hands CIA-B's level to its drives when port B is
written. Where they part is how far they follow p. 342: WinUAE models the
toggle as the running state and has no pulse; vAmiga and Denise model the
toggle, the start and the one-cycle pulse, Denise with the toggle high after
reset; Copperline inverts at underflows but starts high, ignores starts and
pulses low; and vAmiga's CIA-B lets DDRB keep an input line at its pull-up,
where p. 342 says the function overrides DDRB. Read against `P1`'s legs by
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

---

## Suggested change

**One option: give each timer a port-B output and let PBON select it.** In
`cia_timera.v` and `cia_timerb.v`, a toggle flip-flop cleared by `reset`, set
when a control-register write turns `tmcr[0]` on (and by the one-shot start a
timer-high write makes, if a specialist reads p. 342's "started" to include
it), and inverted on `underflow`; a pulse output that is `underflow` held for
one `eclk` count; `tmcr[2]` choosing between them; and the result and
`tmcr[1]` brought out as two new ports. In `ciab.v`, bit 6 and bit 7 of
`portb_out` then take the timer output whenever the matching PBON is set,
whatever `ddrportb` and `regportb` hold; the PRB read, which returns
`portb_out`, follows by itself. This has not been run here. What it would
change:

- `PROP-CRA-PBON`, `PROP-CRA-OUTMODE`, and `P1-MAIN`'s and `P2-LEGR`'s CIA-B
  instances would have a level to measure; `P1-CONTRAST` and `P2-CONTRAST`
  would become gradeable;
- the drives would see the timer on SEL3* or MTR* when a program sets PBON on
  CIA-B, as the wiring of p. 351 implies;
- CIA-A would still read its joystick-adapter inputs on PRB: its PB6 and PB7
  show nothing until the port-B read path of [NM-HRM-054](NM-HRM-054.md)
  carries the pins, and `P1-MAIN`'s CIA-A instances stay red until both change.

---

## What holds the finding up

- **The page is plain and says it five times.** p. 341, p. 342, p. 348,
  p. 349 and p. 350's bitmap give the function to both timers of both CIAs,
  and no page of Appendix F takes it back for the Amiga's wiring. Commodore's
  own A500/A2000 manual and the 6526 specification repeat it.
- **The RTL says so itself.** `ciaa.v`'s header lists "toggling of PB6/PB7 by
  timer A/B" as not implemented, MiSTer's timer modules call both bits "not
  implemented", and the only uses of `tmcr` in either timer module are bits 0,
  3 and 6 and the read-back.
- **The channels are live.** At the module, `FIXTURE-LIVE (portb)` carries two
  bytes through bit 6, and both reds turn green under their oracle mutations.
  At the system, the same CIA-B reads show the data in d1 and the pull-up in
  d2 as the page predicts, `P1-OTHERBITS` is green, and the ICR flags show
  every timer underflowing where the program expects.
- **What weakens it.** CIA-A's instances cannot show the difference at all
  ([NM-HRM-054](NM-HRM-054.md)), so `P1-MAIN` and `P2`'s CIA-A reds are
  overdetermined and CIA-B's carry the measurement. `P2-MAIN`'s first start is
  graded from leg R's expected level, not from r1. `PROP-CRA-PBON` samples a
  pulse-mode output at one instant, which a correct one-cycle pulse passes only
  if the sample falls inside it; NanoMig's red does not depend on that, since
  no path exists. And none of the four implements every clause of p. 342.

---

## For the specialists

1. **Does anything on an Amiga use PBON?** On CIA-B it would put a timer on
   SEL3* or MTR*, on CIA-A on Centronics data bits 6 and 7. The page gives the
   function without exception, and every independent implementation read here
   carries it; whether software depends on it decides how much the gap matters.
2. **The toggle's level after RES and at a start.** p. 342, the A500/A2000
   manual and the 6526 specification all say low after RES and high on every
   start. WinUAE and vAmiga do that, Denise sets it high at reset, and
   Copperline starts it high and does not set it on a start. A fix in the
   Minimig line would do well to follow the page rather than any one of them.
3. **Which starts count.** p. 342 does not say whether the one-shot start that
   a timer-high write makes, or a START = 1 write to a running timer, sets the
   toggle. `integration/appF-cia-system` leaves both ungraded.
