# NM-HRM-076 — a timer-high write reloads a running one-shot timer's counter, where p. 343 loads only the latch

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. p. 343's rule that a high-byte write to a running timer loads the latch and not the counter names no mode, and NanoMig follows it in continuous mode; p. 342 gives one-shot mode its own rule, a write to timer-high that transfers the latch to the counter "regardless of the start bit", and NanoMig follows that in one-shot mode. |
| **Module** | `cia_timera.v`, `cia_timerb.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L76) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L117-L119) |
| **Test** | [`hrm/appF-cia-interval-timers`](../../hrm/appF-cia-interval-timers) — `PROP-RUNNING-WRITE-COUNTER-GATE` (claims `sol:F-timers.c21`, `opus:F-timers.c24`, p. 343) still fails, because it grades p. 343 with the timer in one-shot mode, the one mode p. 342 decides otherwise; the package's mutation record has the same write in continuous mode green. `PROP-ONESHOT-THI-WRITE` (claims `sol:F-timers.c15`, `opus:F-timers.c18`, p. 342) is green, graded on a stopped timer only |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 342–343 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 17 (`16 passed, 1 failed`): `PROP-RUNNING-WRITE-COUNTER-GATE` reads Timer A's counter at 250 before a write of $02 to TAHI and at 512, the new latch value, after it, with START still set. `PROP-LATCH-LOAD-STOPPED` (p. 343's stopped case) and `PROP-FORCE-LOAD` (a running timer in continuous mode) are green on the same write-and-read mechanism |
| **Error class** | **a sentence graded without the paragraph that qualifies it** |
| **Verdict of the four** | **3–1 with NanoMig and with p. 342**: WinUAE, vAmiga and Denise reload the counter on a timer-high write in one-shot mode whether the timer runs or not — vAmiga quotes p. 342 above the code that does it, and Denise does it for its 8520 model and not for its 6526; Copperline loads the counter only while the timer is stopped, whatever the mode, and reads p. 342's sentence as part of the stopped case |

---

## Why it was withdrawn

The red assertion grades the second sentence of p. 343's reload paragraph with
Timer A in one-shot mode: it writes CRA $09 (one-shot, START set), lets the
counter run down from 256, and writes a new latch value of 512 to TALO and
TAHI. The counter reads 512 after the write and the timer is still running.

**p. 342 gives that write in one-shot mode a rule of its own**, in the
paragraph on one-shot and continuous mode:

> In one-shot mode, a write to timer-high (register 5 for timer A, register 7
> for Timer B) will transfer the timer latch to the counter and initiate
> counting regardless of the start bit.

*Regardless of the start bit* leaves neither value of START out, the running
timer included. For a stopped timer the transfer is already in p. 343's first
sentence; what p. 342's sentence adds there is the start, and what it adds for
a running timer is the transfer. The measured 512 is that transfer.

**p. 343's rule names no mode:**

> If the timer is running, a write to the high byte will load the timer latch
> but not the counter.

NanoMig follows it wherever p. 342 does not speak. Its reload gate is the two
sentences term for term — a stopped timer (`~start`, p. 343) or one-shot mode
(`oneshot`, p. 342) — and the package's mutation `MUT_RUNGATE`, the same
running write in continuous mode, is recorded green, `17 passed, 0 failed`.
The only case the assertion reds is the case p. 342 decides, and there NanoMig
does what p. 342 says.

**The narrower reading is the one under which the assertion would hold.** The
package grades p. 342's sentence on a stopped timer only, and one of its two
claims restates *regardless of the start bit* as *even if the start bit is
clear* (`opus:F-timers.c18`); with that narrowing p. 343 would govern a
running one-shot timer. The page says *regardless*, not *even if*.

**No other page speaks to it.** The control-register pages, p. 348 and p. 349,
describe START, RUNMODE and LOAD and say nothing about a write to the timer
registers; p. 341 introduces the counter and the latch; p. 352's example uses
the stopped case only; no page outside Appendix F speaks of a timer latch or
of one-shot mode.

---

## What the HRM says

**Printed p. 341**, the counter and the latch:

> Each interval timer consists of a 16-bit read-only timer counter and a
> 16-bit write-only timer latch. Data written to the timer is latched into the
> timer latch, while data read from the timer is the present contents of the
> timer counter.

**Printed p. 342**, three of the control functions:

> A control bit allows the timer to be started or stopped by the
> microprocessor at any time.

> A control bit selects either timer mode. In one-shot mode, the timer will
> count down from the latched value to zero, generate an interrupt, reload the
> latched value, then stop. In continuous mode, the timer will count down from
> the latched value to zero, generate an interrupt, reload the latched value,
> and repeat the procedure continuously.
>
> In one-shot mode, a write to timer-high (register 5 for timer A, register 7
> for Timer B) will transfer the timer latch to the counter and initiate
> counting regardless of the start bit.

> A strobe bit allows the timer latch to be loaded into the timer counter at
> any time, whether the timer is running or not.

**Printed p. 343**, under *Input Modes*, the reload paragraph whole:

> The timer latch is loaded into the timer on any timer underflow, on a force
> load, or following a write to the high byte of the pre- scalar while the
> timer is stopped. If the timer is running, a write to the high byte will load
> the timer latch but not the counter.

**Printed p. 348**, Control Register A (p. 349 prints the same two rows for
Timer B):

> 0 START 1 = start Timer A, 0 = stop Timer A. This bit is automatically reset
> (= 0) when underflow occurs during one-shot mode.

> 3 RUNMODE 1 = one-shot mode, 0 = continuous mode.

**Printed p. 352**, the appendix's timing example, sets one-shot mode, *"This
sets all bits needed for timer A one-shot mode."*, writes the low and then the
high byte, and waits for the underflow without setting START first: the
stopped case of p. 342's sentence, which both readings share.

---

## What NanoMig does

**In one-shot mode a timer-high write reloads the counter whether the timer
runs or not; in continuous mode only while it is stopped.** The reload request
is registered on the `clk7_en` after the write —
[`src/minimig-aga/cia_timera.v#L73-L80`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L73-L80):

```verilog
// thi is written in one-shot mode so tmr must be reloaded
always @(posedge clk)
  if (clk7_en) begin
    thi_load <= thi & wr & (~start | oneshot);
  end

// timer counter reload signal
assign reload = thi_load | forceload | underflow;
```

and `reload` loads the counter from the two latch bytes ahead of counting —
[`#L87-L88`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L87-L88), `else if (reload)` / `tmr[15:0] <= {tmlh[7:0],tmll[7:0]};`.
In one-shot mode the same request sets START —
[`#L41-L42`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L41-L42), `else if (thi_load && oneshot)` / `tmcr[0] <= 1'd1;`
— which is p. 342's *initiate counting*, and changes nothing for a timer
already running. `start` is CRA bit 0 and `oneshot` CRA bit 3,
[`#L52-L53`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L52-L53).
Timer B carries the same gate,
[`src/minimig-aga/cia_timerb.v#L74`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L74), `thi_load <= thi & wr & (~start | oneshot);`,
and `ciaa.v` sends register 5 to Timer A's high byte,
[`src/minimig-aga/ciaa.v#L141`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L141), `assign  tahi = enable && rs==4'h5;`.

### Measured

[`hrm/appF-cia-interval-timers`](../../hrm/appF-cia-interval-timers) builds
`ciaa` alone and drives its register bus; `16 passed, 1 failed`.

| case | mode | what is read | graded by |
|---|---|---|---|
| a write of $02 to TAHI while the timer runs down from 256 | one-shot | counter **250** before the write, **512** after it, START still set | `PROP-RUNNING-WRITE-COUNTER-GATE`, **red** |
| the same write, as the package's mutation `MUT_RUNGATE` | continuous | recorded `17 passed, 0 failed`: the counter is not reloaded | the same assertion, green |
| a write to TAHI on a stopped timer, START never written | one-shot | counter `0x6400`, START reads 1 | `PROP-ONESHOT-THI-WRITE`, green |
| an underflow, a force load, a high-byte write while stopped | — | 3, `0x1278` and `0x569A`, each the latch | `PROP-LATCH-LOAD-STOPPED`, green |
| a force load while running | continuous | `0x2055`, the latch | `PROP-FORCE-LOAD`, green |

---

## What the others say

The question put to each: does a write to timer-high reload the counter of a
timer that is running in one-shot mode?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1652-1702`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1652-L1702), [`cia.cpp:1800-1803`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1800-L1803), [`cia.cpp:536-537`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L536-L537) | a write to register 5 or 7 reaches `CIA_thi_write(num, tnum, val);`, which states its rule in two comments, `// If ONESHOT: Load and start timer.` and `// If CONTINUOUS: Load timer if not running.` With `acc_mode()` false it loads at once under `if (!(t->cr & CR_START) \|\| (t->cr & CR_RUNMODE)) {`, `t->timer = t->latch;`; in its cycle-accurate mode one-shot mode schedules the load with no START term, `t->loaddelay \|= 0x00000001 << 2;`, and two CIA cycles later `if (t->loaddelay & 0x00000001) {` runs `t->timer = t->latch;`. **Reloads a running one-shot timer's counter**, in both modes |
| **vAmiga** | [`Core/Components/CIA/CIARegs.cpp:253-282`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L253-L282), [`Core/Components/CIA/CIA.cpp:456`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L456), [`Core/Components/CIA/CIA.cpp:316-318`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L316-L318) | the load a stopped timer gets, `if (!(cra & 0x01)) delay \|= CIALoadA0;`, one-shot mode gets again with no START term, `if (cra & 0x08) {` … `delay \|= CIACountA1 \| CIALoadA0 \| CIACountA0;` (skipped only when CRA's INMODE bit selects CNT), under the manual's own words, `/* MOS 8520 only feature:` and `counter and initiate counting regardless of the start bit." [HRM]`. One cycle later `if (delay & CIALoadA1) reloadTimerA(&delay);` sets `counterA = latchA;`. **Reloads a running one-shot timer's counter** |
| **Denise** | [`emulation/cia/new/register.cpp:241-263`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L241-L263), [`emulation/cia/new/cia.cpp:247-253`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L247-L253), [`emulation/libami/agnus/agnus.h:104-105`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L104-L105) | the Amiga's two CIAs are its 8520 model, `Cia<MOS_8520>& cia1;`. A write to TAHI schedules the load for a stopped timer, `else if (!(timerA.control & 1))` / `delay \|= CIA_FL_TA0;`, and then, under `if constexpr (model == MOS_8520) {` and `if (timerA.control & 8) { // oneshot mode`, schedules it again with no START term (skipped when INMODE selects CNT, `if (!(timerA.control & 0x20)) {`). A running timer takes it on the next cycle, `else if ( delay & ( timerId == T_A ? CIA_FL_TA1 : CIA_FL_TB1 ) )` / `rTimer.counter = rTimer.latch;`. **Reloads a running one-shot timer's counter on the 8520; its 6526 model does not** |
| **Copperline** | [`src/chipset/cia.rs:528-558`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L528-L558), [`src/chipset/cia.rs:628-630`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L628-L630), [`src/bus.rs:9045`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L9045) | a CPU write reaches the CIA through `let eff = self.cia_a.write(reg, byte);`, and a write to TAHI loads the count only inside `if !self.ta_running {`, `self.ta_count = self.ta_latch;`, where `ta_running` is CRA's START bit, `self.ta_running = val & 0x01 != 0;`. Its comment files p. 342's sentence under the stopped case: `Per the 8520 datasheet: writing TAHI when the` … `latch. In one-shot mode (CRA bit 3 = 1), it` / `ALSO auto-starts the timer for one underflow.` TBHI is the same, `if !self.tb_running {`. **Leaves a running timer's counter alone in either mode** |
| **MiSTer** | [`rtl/cia_timera.v:109-126`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L109-L126), [`rtl/cia_timerb.v:118`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerb.v#L118) | descendant — NanoMig's gate, `if (thi & wr & (~start \| oneshot))`, under the comment `// In stopped state or one-shot mode, this triggers timer reload`; the request waits for the next E-clock tick, `wire thi_load_eclk= thi_load_latched & eclk;`, `assign reload = thi_load_eclk \| forceload \| underflow;`. A running one-shot timer's counter is reloaded, at that tick |
| **TC64** | [`rtl/minimig/cia_timera.v:73-87`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timera.v#L73-L87), [`rtl/minimig/cia_timerb.v:79`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timerb.v#L79) | descendant — the same as MiSTer, under NanoMig's comment: `// thi is written in one-shot mode so tmr must be reloaded`, `if (thi & wr & (~start \| oneshot))`, `assign reload = thi_load_eclk \| forceload \| underflow;`. Neither line holds a running one-shot timer's counter |

**Three of the four do what NanoMig does, and say why.** WinUAE's comment
puts the rule in two lines, one per mode; vAmiga quotes p. 342 above its code
and calls it an 8520 feature; Denise gives the running reload to its 8520
model and withholds it from its 6526. Copperline alone applies p. 343's
running rule in one-shot mode, and its comment takes p. 342's sentence as
something a stopped timer does. Both Minimig lines carry NanoMig's gate.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 218 and 219 — says what the manual says, both sentences, for the 8520:

  > "In one-shot mode; a write to Timer High (registers 5 for TIMER A, 7 for TIMER B) will transfer the timer latch to the counter and initiate counting regardless of the start bit."
  <!-- corpus: a500-a2000-trm-1987/p218.txt -->

  > "If the timer is running, a write to the high byte will load the timer latch, but not reload the counter."
  <!-- corpus: a500-a2000-trm-1987/p219.txt -->

  The one-shot sentence closes the One-Shot/Continuous paragraph and the running rule follows Input Mode, in the manual's order and without a word that reconciles them.

- **Commodore, 6526 CIA specification, 1981**, PDF page 6 — has p. 343's rule and not p. 342's sentence:

  > "timer is running, a write to the high byte will load the"
  <!-- corpus: cia-6526-spec/p006.txt -->

  > "timer latch, but not reload the counter."
  <!-- corpus: cia-6526-spec/p006.txt -->

  The OCR sets the port register table's row labels between the two lines. The page's One-Short/Continuous paragraph ends with continuous mode, *"reload the latched value and repeat the procedure continuously"* in the render, and Force Load follows; there is no sentence on a timer-high write in one-shot mode. p. 343's rule is the 6526's, printed there with no one-shot exception; the sentence that gives one-shot mode its own rule is in the 8520's description, and vAmiga's comment and Denise's model switch treat it as an 8520 difference. It is a 6526 document and does not itself speak for the 8520.

---

## Why this entry is kept

p. 343's reload paragraph is where a reader looks up what a write to a running
timer does, and it names no mode. The one-shot rule is on the page before,
under *One-shot/continuous*. A test that grades p. 343 with the timer in
one-shot mode is red on NanoMig and on three of the four implementations read
here; this page is where that reader finds p. 342's sentence, and the two
readings of *regardless of the start bit* that decide it.
