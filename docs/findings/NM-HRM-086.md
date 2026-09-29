# NM-HRM-086 — CIA-B has no serial port: in output mode a byte written to its SDR never sets the ICR's SP bit, where pp. 339, 345 and 348 give both 8520s one [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `ciab.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L186) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L238) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's reading is accepted. NanoMig, MiSTer and TC64 all tie CIA-B's serial interrupt to 0, so there is no fix to port; the transmit logic CIA-A already has in all three is what a fix would reuse — see *Suggested change* |
| **Test** | [`integration/appF-cia-system`](../../integration/appF-cia-system) — one assertion, run on both CPU cores: `P7.6-CIAB [p345,p348]: at least one ICR sample has SP=1 (CRA<-$41, SPMODE output, SDR<-$A5)`, the SPMODE = 1 field of claim `opus:F-cra.c13` (p. 349), red on fx68k and on TG68K. Its green control is `P7.6-CIAB-GREENCTL`, run with `NMH_MUTATE=P76CIAB-GREENCTL`: the same program on CIA-A, through the same comparator |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 339, 340, 345–349, 351; Appendix E — I/O Connectors and Interfaces, printed pp. 324, 337 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red on each core, `P7.6-CIAB`: 0 of 300 ICR samples show SP on fx68k and on TG68K. In the same run `P7.6-CIAB-PRE`, green on both cores, finds timer A underflowing in 136 of the 300 samples. Run with `NMH_MUTATE=P76CIAB-GREENCTL`, `P7.6-CIAB-GREENCTL`, the same program on CIA-A through the same comparator, is green on both cores |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline run one CIA model for both chips, and on CIA-B, with CRA bit 6 set, shift a byte written to SDR out on timer A's underflows and set ICR bit 3 when the eighth bit is out. WinUAE, vAmiga and Denise take two underflows a bit, as p. 345's "1/2 the underflow rate" does; Copperline takes one. vAmiga, Copperline and WinUAE also tie CIA-B's SP and CNT to its PA0 and PA1, the lines pp. 337 and 351 pair them with |

---

## What the HRM says

**Printed p. 339** gives every CIA the same parts:

> Each Amiga system contains two 8520 Complex Interface Adapter (CIA) chips. Each chip has 16 general purpose input/output pins, plus a serial shift register, three timers, an output pulse pin and an edge detection input.

**Printed p. 345** describes the shift register's output mode:

> In the output mode, Timer A is used as the baud rate generator. Data is shifted out on the SP pin at 1/2 the underflow rate of Timer A.

> To begin transmission, you must first set up Timer A in continuous mode, and start the timer. Transmission will start following a write to the serial data register. The clock signal derived from Timer A appears as an output on the CNT pin.

> After eight CNT pulses, an interrupt is generated to indicate that more data can be sent.

**Printed p. 348**, CRA bit 6:

> 6 SPMODE 1 = Serial port=output (CNT is the source of the shift clock)

**Printed p. 346** lists the interrupt among every 8520's five:

> There are five sources of interrupts on the 8520: ... Serial port full/empty

and p. 347's READ INTERRUPT CONTROL REGISTER row, read from the page image
because the text layer moves its tail up a line, puts SP at bit 3. None of
these sentences distinguishes the two chips: each speaks of *the 8520*, which
p. 339 fits twice.

### What the pages say about CIA-B's serial port in particular

**It has no system task.** Printed p. 345, before the modes:

> In the Amiga system one shift register is used for the keyboard, and the other is unassigned.

and the CIA-B address map on **printed p. 340** calls its register

> CIAB serial data register (unused)

**Its pins are wired all the same.** The port signal assignments on **printed
p. 351**, read from the page image because the text layer displaces the CIA-B
column, list `SP...BUSY` and `CNT..POUT` under CIA-B and join them by brackets,
labelled "commodore", to `PA0..BUSY` and `PA1..POUT`. Appendix E's copy of the
same table on **printed p. 337** labels the two brackets "commodore serial bus".
The parallel connector's own description on **printed p. 324** gives those two
lines their serial roles; the text layer displaces both notes from their rows,
and the page image puts the first on BUSY and the second on POUT:

> This is a general purpose I/O pin also connected to a serial data I/O pin (serial clock on pin 12).

> This is a general purpose I/O pin to a serial clock I/O pin (serial data on pin 11).

**So the pages leave nothing open on the point measured.** *Unassigned* and
*unused* say that the system gives CIA-B's shift register no job. They do not
say that the second 8520 lacks one: p. 339 gives *each chip* a serial shift
register, pp. 345–348 describe the output mode, the SPMODE bit and the SP
interrupt for the 8520 without naming either chip, and pp. 324, 337 and 351 bring
this chip's SP and CNT out to pins 11 and 12 of the parallel connector as a data
and a clock line. What the package measures is the chip's own register behaviour
— a write to SDR in output mode, and the SP bit in the ICR — and in output mode
the shift clock is timer A's (p. 345; p. 348, "CNT is the source of the shift
clock"), so no page makes that interrupt wait for anything on the connector.
Nothing in the manual predicts an 8520 whose ICR bit 3 never sets.

---

## What NanoMig does

**CIA-B's SDR is a plain latch, and its ICR's serial input is tied to 0.** The
register stores a written byte and returns it on a read —
[`src/minimig-aga/ciab.v#L71-L81`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L71-L81):

```verilog
// fake serial port data register
always @(posedge clk)
  if (clk7_en) begin
    if (reset)
      sdr_latch[7:0] <= 8'h00;
    else if (wr & sdr)
      sdr_latch[7:0] <= data_in[7:0];
  end

// sdr register read
assign sdr_out = (!wr && sdr) ? sdr_latch[7:0] : 8'h00;
```

— and the interrupt controller's SP input is a constant,
[`#L186`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L186),
`.ser(1'b0),`, while timer A's SPMODE output is left open,
[`#L208`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L208),
`.spmode(),`. `cia_int.v` sets ICR bit 3 from that input and from nothing
else,
[`src/minimig-aga/cia_int.v#L56`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_int.v#L56),
`icr[3] <= icr[3] | ser;    // external ser input`. The module has no SP or CNT
port at all
([`#L4-L22`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L4-L22)),
and the instance in `minimig.v` connects none
([`src/minimig-aga/minimig.v#L633-L651`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L633-L651)).

**CIA-A has the transmit side, and it counts p. 345's sixteen underflows.** A
write to SDR while CRA bit 6 is set starts a run, and a four-bit counter of
timer A's underflows ends it —
[`src/minimig-aga/ciaa.v#L197-L217`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L197-L217):

```verilog
    if (reset || !spmode) // reset or not in output mode
      ser_tx_run <= 0;
    else if (sdr && wr) // write to serial port data register when serial port is in output mode
      ser_tx_run <= 1;
    else if (ser_tx_irq) // last bit has been transmitted
      ser_tx_run <= 0;
...
    else if (tmra_ovf) // bits are transmitted when tmra overflows
      ser_tx_cnt <= ser_tx_cnt + 4'd1;
...
assign ser_tx_irq = &ser_tx_cnt & tmra_ovf; // signal irq when ser_tx_cnt overflows
```

and that reaches CIA-A's ICR,
[`#L322`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L322),
`.ser(keystrobe | ser_tx_irq),`. The file's own header, which describes both
CIAs, lists the gap —
[`src/minimig-aga/ciaa.v#L23-L24`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L23-L24):

```verilog
// NOT implemented is:
// serial data register for CIA B(but keyboard input for CIA A is supported)
```

### Measured

[`integration/appF-cia-system`](../../integration/appF-cia-system), proposition
P7.6, loads CIA-B's timer A with 10, writes CRA ← $41 (START, SPMODE output,
continuous) and then SDR ← $A5, and reads CIA-B's ICR 300 times, storing every
read (`p76_ciab.s`). It grades, on fx68k and on TG68K:

`P7.6-CIAB [p345,p348]: at least one ICR sample has SP=1 (CRA<-$41, SPMODE output, SDR<-$A5)`

| run | fx68k | TG68K | graded by |
|---|---:|---:|---|
| CIA-B, `p76_ciab.s`: samples showing TA | 136 of 300 | 136 of 300 | `P7.6-CIAB-PRE`, green |
| CIA-B, `p76_ciab.s`: samples showing SP | **0 of 300** | **0 of 300** | `P7.6-CIAB`, **red** |
| CIA-A, `p76_ciaa.s`, the same program: a sample showing SP | yes | yes | `P7.6-CIAB-GREENCTL`, run with `NMH_MUTATE=P76CIAB-GREENCTL`, green |

The package prints 61 passed, 26 failed on each core, and 62 passed, 26 failed
with the green control added. `P7.6-CIAB` is the one red line of this finding;
`P10 opus:F-cra.c13`, the conjunction over the CRA bitmap's 1-settings, is red
through it among other members.

**Where SP appears.** Read with the package's own two programs and harness — a
measurement for this page, not a graded assertion — on both cores alike: on
CIA-A the SP bit shows first in sample 34, the sample that shows timer A's
sixteenth underflow, which is p. 345's eight bits at "1/2 the underflow rate of
Timer A". On CIA-B the underflows go on through all 300 samples and SP never
shows. The samples are 192 to 208 clk_sys apart and no two neighbours both show
TA, so each flagged sample is one underflow.

---

## What the others say

The question put to each: on CIA-B, with CRA bit 6 set, does a byte written to
SDR shift out on timer A's underflows and set ICR bit 3 when it is done?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:510`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L510), [`:575-585`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L575-L585), [`:654-656`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L654-L656), [`:91`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L91), [`:1846-1854`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1846-L1854), [`:2046-2052`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2046-L2052) | <pre>if ((c-&gt;t[0].cr &amp; (CR_SPMODE \| CR_RUNMODE)) == CR_SPMODE &amp;&amp; c-&gt;sdr_cnt &gt; 0) {<br>&nbsp;&nbsp;&nbsp;&nbsp;c-&gt;sdr_cnt--;<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;if (c-&gt;sdr_cnt == 0) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sp = 1;</pre> **one model for both chips, and the port runs on CIA-B.** `CIA_update_check()` steps both CIAs through the same code, `for (int num = 0; num < 2; num++) {`. A write to SDR while CRA has SPMODE and START set loads the shift register and sets `c->sdr_cnt = 8 * 2;`; every timer-A underflow in continuous mode counts it down, shifting on every second one, and at zero `sp = 1;` becomes `c->icr2 \|= ICR_SP;`, with `#define ICR_SP 8`. With no printer or parallel-port adapter configured, a read of CIA-B's PRA in output mode shows the port's clock on PA1 and its data on PA0, `tmp \|= (c->sdr_cnt & 1) ? 2 : 0;` and `tmp \|= (c->sdr_buf & 0x80) ? 1 : 0;` |
| **vAmiga** | [`Core/Components/CIA/CIA.h:707`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.h#L707), [`Core/Components/CIA/CIARegs.cpp:358-362`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L358-L362), [`Core/Components/CIA/CIA.cpp:431`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L431), [`:494-529`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L494-L529), [`:661-662`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L661-L662), [`:1019-1035`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1019-L1035) | <pre>if (timerAOutput &amp;&amp; (cra &amp; 0x40)) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;feed ^= CIASerClk0;</pre> **one model for both chips, and the port runs on CIA-B.** `class CIAB final : public CIA {` overrides only the port and interrupt-line functions, so CIA-B runs the shared cycle function's serial code. A write to SDR, `sdr = value;`, queues `delay \|= CIASdrToSsr0;`; the next underflow with CRA bit 6 set, `bool timerAOutput = (counterA == 0 && (delay & CIACountA2));`, loads the shift register, `serCounter = 8;`, and every underflow toggles the serial clock, so a bit takes two; at the last bit `if (serCounter == 1) delay \|= CIASerInt0;` leads to `icr \|= 0x08;`. `CIAB::updatePA()` states the board wiring, *"Inside the Amiga, PA0 and PA1 of CIAB are wired to the SP pin and the CNT pin, respectively"*, and feeds PA1's edges to the CNT input |
| **Denise** | [`emulation/libami/system/system.h:48-49`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.h#L48-L49), [`emulation/cia/new/register.cpp:343-346`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L343-L346), [`emulation/cia/new/cia.cpp:112-117`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L112-L117), [`:267-269`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L267-L269), [`:318-322`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L318-L322), [`:439-441`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L439-L441), [`:121`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L121), [`emulation/libami/system/system.cpp:38-41`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L38-L41) | <pre>auto Cia&lt;model&gt;::timerAUnderflow() -&gt; void {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (timerA.control &amp; 0x40)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;shiftOut();</pre> **one model for both chips, and the port runs on CIA-B.** The Amiga's two CIAs are two instances of one template, `Cia<MOS_8520> cia1;` and `Cia<MOS_8520> cia2;`. A write to SDR, `sdr = value;`, schedules the load into the shift register; with CRA bit 6 set each timer-A underflow calls `shiftOut()`, which starts a sequence with `sdrShiftCount = 16;` and schedules one CNT flip per underflow, and `if (--sdrShiftCount == 1) {` ends it; the scheduled finish sets the SP bit, `if (_delay & CIA_FINISH_SDR2) intIncomming \|= 8;`. Only `cia1`'s pins are connected, `cia1.serialOut = [this](bool spLine, bool cntLine) {`, to the keyboard handshake; `cia2`'s SP and CNT go nowhere, and its ICR bit is set all the same |
| **Copperline** | [`src/chipset/cia.rs:10-18`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L10-L18), [`:576-581`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L576-L581), [`:749`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L749), [`:775`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L775), [`:878-890`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L878-L890), [`:665-669`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L665-L669) | <pre>let pulses = ta_underflows.min(8 - self.sdr_shift_count as u32) as u8;<br>self.sdr_shift_count += pulses;<br>if self.sdr_shift_count &lt; 8 {<br>&nbsp;&nbsp;&nbsp;&nbsp;return 0;<br>}</pre> **one model for both chips, and the port runs on CIA-B, at one underflow a bit.** The module has one CIA model for `Which::A` and `Which::B`, and its header lists *"serial shift input/output"*. A write to SDR with CRA bit 6 set arms the output, `self.sdr_out = Some(val);`; each timer step passes timer A's underflows to `tick_sdr_output`, which counts eight of them and returns `ICR_SP`, latched by `let _ = self.latch_interrupts(sp_mask);`. That is half the sixteen p. 345's "1/2 the underflow rate" gives; P7.6-CIAB grades only that SP is set, so Copperline would pass it. Its CIA-B ties PA0 and PA1 to its own SP and CNT, under *"A500 board tie: CIA-B PA0 is wired to this CIA's own SP pin"* |
| **MiSTer** | [`rtl/ciab.v:118-129`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L118-L129), [`:238`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L238), [`rtl/minimig.v:722`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L722) | descendant — the same gap, stated in the file: *"CIA B's serial port is not implemented in this simplified version"* over the same SDR latch, and `.ser(1'b0),               // Serial port not implemented`. MiSTer's CIA-B has a CNT input, `cnt_in`, for its timers only, and ties it high, `.cnt_in(1'b1),` |
| **TC64** | [`rtl/minimig/ciab.v:71-81`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L71-L81), [`:186`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L186), [`:209`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L209) | descendant — identical to NanoMig on this point: `// fake serial port data register`, `.ser(1'b0),` and `.spmode()`. Neither line has a CIA-B serial port, and both keep CIA-A's transmit counter |

**All four implement the port on CIA-B, and none ties its SP bit to 0.** Each
runs the same CIA model for both chips, shifts a byte written to SDR out on
timer A's underflows while CRA bit 6 is set, and sets ICR bit 3 when the byte
is out. WinUAE, vAmiga and Denise take two underflows a bit, p. 345's rate;
Copperline takes one, so its SP bit comes after eight underflows where p. 345
gives sixteen, a difference `P7.6-CIAB` does not grade. Three of them also
model the board wiring pp. 337 and 351 draw, CIA-B's SP and CNT on PA0 and PA1.

---

## What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 220 and 106 — agrees with the manual:

  > "Transmission wilt start following a write to the Serial Data Register (provided TIMER A is running and in continuous mode)."
  <!-- corpus: a500-a2000-trm-1987/p220.txt -->

  > "After 8 CNT pulses, an interrupt is generated to indicate more data can be sent."
  <!-- corpus: a500-a2000-trm-1987/p220.txt -->

  > "signals that together can also function as a synchronous serial data port driven by an 8520 CIA device"
  <!-- corpus: a500-a2000-trm-1987/p106.txt -->

  > "For serial port usage, BUSY is the serial clock, POUT is the serial data line."
  <!-- corpus: a500-a2000-trm-1987/p106.txt -->

  The TRM's 8520 section gives the output mode in p. 345's words, for the part the A500 and A2000 carry ("wilt" is the OCR's reading of "will", and the interrupt's "generated" is hyphenated across a line). Its description of a video connector that also carries parallel-port lines says BUSY and POUT together can serve as a synchronous serial port driven by an 8520, the port pp. 324, 337 and 351 wire to CIA-B. It names the two lines the other way round from p. 324, BUSY the clock and POUT the data; which is right does not bear on this finding, which reads the port's interrupt, not its pins.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, the overview on sheet 1 and schematic sheets 5d and 5e, 10a, 11a and 11b — agrees with the manual as far as it is read here:

  *Read from the drawing, not quoted:* the overview on sheet 1, a gate-level drawing of the whole chip, drives the SP pad through an open-collector NAND of CRA6 and SP_TXD#, which is taken through pass switches and latches from the shift register's bit 7, SR_out7, and the CNT pad through an open-collector NAND of CRA6 and SP_TCK#, which the serial port control derives from TA_INT, timer A's underflow; while CRA6 is 0 the SP pad is read in instead, SP_RXD# being a NOR of SP and CRA6. W_SDR, the write strobe of SDR, reaches both the shift register and the control. The control's output SP_INT passes two latches into IFR3#, bit 3 of the interrupt flag register, which R_ICR puts on D3io and which, with IER3#, enters INT_ACT. Sheet 11b, "serial port control", carries the notes "TX start condition", "in TX mode, transfer contents of data latch into shift register at the start of sequence", "Bit counter clock generator", "4 Bit ring counter //counting the Bits of a shift sequence" and "internal TX shift clock"; sheets 5d and 5e draw the SP and CNT pads with CRA6 on each driver, and sheet 10a the interrupt register's SP_INT input.
  <!-- drawing: cia-8520r4-dissection/sheet_001.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_007.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_015.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_016.txt -->

  The silicon has the output mode p. 345 describes: SDR loaded into a shift register, its bit 7 on SP, a clock derived from timer A's underflow on CNT, the pads driven only while SPMODE is set and open collector, as p. 346 says, and an interrupt into ICR bit 3. How many underflows make a bit, and at which step SP_INT fires, are not traced here: sheet 11b draws the control transistor by transistor, and the overview's path from TA_INT to SP_INT runs through latches and pass switches that this reading followed by connection, not by simulation. Nothing in the drawing depends on which of the two sockets the part sits in. The dissection is of one revision.

---

## Suggested change

**One option: give `ciab.v` the transmit side `ciaa.v` already has.** Connect
timer A's `spmode` output, copy the `ser_tx_run` and `ser_tx_cnt` logic of
`ciaa.v` lines 197–217 — a run started by a write to SDR in output mode and
ended at the sixteenth underflow of timer A — and feed its end to the interrupt
controller in place of `1'b0`. `P7.6-CIAB-GREENCTL` shows that logic, in CIA-A,
setting SP with P7.6-CIAB's own program. This has not been run on CIA-B here.
What it would change:

- `P7.6-CIAB` would see SP at the sixteenth underflow, as CIA-A does;
- the byte would still reach no pin. Bringing SP and CNT out to PA0 and PA1, the
  parallel port's BUSY and POUT as pp. 337 and 351 wire them, and the input
  mode, which needs a CNT edge from outside, are a larger change;
- NanoMig would move toward all four implementations and away from both Minimig
  lines, neither of which has the port.

---

## What holds the finding up

- **The comparator reads green on a live input.** The same program on CIA-A,
  through the same comparator, sets SP on both cores (`P7.6-CIAB-GREENCTL`),
  and in this page's diagnostic SP shows with the sixteenth underflow, the count
  p. 345's rate gives.
- **CIA-B's timer A runs in the same window.** 136 of 300 samples show TA on
  each core (`P7.6-CIAB-PRE`), so the missing SP is not a stopped timer.
- **The RTL says why.** `ciab.v` ties the ICR's serial input to `1'b0`, and the
  header of `ciaa.v` lists the CIA-B serial data register as not implemented.
- **p. 339 and pp. 345–348 describe one part, fitted twice**, and pp. 324, 337
  and 351 bring this chip's SP and CNT out to the parallel connector.
- **What weakens it is that the system gives the port no task.** p. 345 calls
  CIA-B's shift register unassigned and p. 340 calls its register unused, so
  the finding is about the chip's own function, not about a function the
  Amiga's system assigns. Whether a Minimig-derived core has to carry an
  unassigned port is the specialist's decision; all four implementations carry
  it, and neither Minimig line does.
