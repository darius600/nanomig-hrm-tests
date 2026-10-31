# NM-HRM-086 — CIA-B has no serial port: in output mode a byte written to its SDR never sets the ICR's SP bit, where pp. 339, 345 and 348 give both 8520s one [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `ciab.v` set ICR bit 3 when a byte written to SDR in output mode has been shifted out on timer A's underflows? The question stops at that interrupt: shifted data on the pins and input mode are outside it. It closes when `ciab.v` generates the request and `P7.6-CIAB` is green on fx68k and TG68K, or when an Amiga specialist rejects the change and the reason is recorded here |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | 8520 CIA-B, output mode: CRA bit 6 set, timer A continuous, a write to SDR; the transmit-completion interrupt (ICR bit 3) only. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 339, 340, 345–349, 351; Appendix E — I/O Connectors and Interfaces, printed pp. 324, 337 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. No assertion the page names carries a claim id in its message: `P7.6-CIAB` [p345,p348], `P7.6-CIAB-PRE` and `P7.6-CIAB-GREENCTL` [SPECIFICATION.md P7.6], `P7.6-CIAA` [p345,p351,p357]; `P7.6-CIAB` grades the SPMODE = 1 field of `opus:F-cra.c13`, p. 349 |
| **Module** | `ciab.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciab.v#L186) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L238); inherited (MiSTer has the same code; TC64's `ciab.v` is identical to NanoMig's on this point) |
| **Test** | [`integration/appF-cia-system`](../../integration/appF-cia-system): `make -k -C integration/appF-cia-system test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero |
| **Assertions** | `P7.6-CIAB [p345,p348]: at least one ICR sample has SP=1 (CRA<-$41, SPMODE output, SDR<-$A5)` — CIA-B's `ICR.SP` request is never generated: 0 of 300 ICR samples show SP, red on fx68k and on TG68K, in a package run of 98 passed, 26 failed on each core. Green in the same run: `P7.6-CIAB-PRE` (timer A underflows in 136 of the 300 samples on fx68k, 111 on TG68K), `P7.6-CIAB-GREENCTL` (the same program on CIA-A through the same comparator) and `P7.6-CIAA` (with CRA bit 6 = 0, a key code reaches CIA-A's serial port: SP reads 1 in the ICR read before the stop write, graded under the timing clauses of the key-code window; a live input path while bit 6 is 0, it does not show that bit 6 selects the input mode) |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline run one CIA model for both chips, and on CIA-B, with CRA bit 6 set, shift a byte written to SDR out on timer A's underflows and set ICR bit 3 when the eighth bit is out. WinUAE, vAmiga and Denise take two underflows a bit, as p. 345's "1/2 the underflow rate" does; Copperline takes one. vAmiga ([`CIA.cpp:1019-1035`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1019-L1035)), Copperline ([`cia.rs:665-669`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L665-L669)) and WinUAE ([`cia.cpp:2046-2052`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2046-L2052)) also tie CIA-B's SP and CNT to its PA0 and PA1, the lines pp. 337 and 351 pair them with. Hardware: not measured |
| **Effort** | S — `ciab.v` only — connect timer A's `spmode` output and copy CIA-A's `ser_tx_run` / `ser_tx_cnt` logic to feed `.ser` |

---

## Source requirement

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

and p. 347's READ INTERRUPT CONTROL REGISTER row, read from the page image, puts SP at bit 3. None of these sentences distinguishes the two chips.

**Printed p. 345**, before the modes, and the CIA-B address map on **printed p. 340**:

> In the Amiga system one shift register is used for the keyboard, and the other is unassigned.

> CIAB serial data register (unused)

Unused in the stock machine is not absent from the specified CIA: pp. 324, 337 and 351 bring this chip's SP and CNT out to pins 11 and 12 of the parallel connector. In output mode the shift clock is timer A's (p. 345; p. 348), so no page makes that interrupt wait for anything on the connector.

## RTL and measurement

**CIA-B's SDR is a plain latch, and its ICR's serial input is tied to 0.** The register stores a written byte and returns it on a read ([`ciab.v#L71-L81`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciab.v#L71-L81)); the interrupt controller's SP input is a constant, [`#L186`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciab.v#L186), `.ser(1'b0),`, and timer A's SPMODE output is left open, [`#L208`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciab.v#L208). `cia_int.v` sets ICR bit 3 from that input and from nothing else ([`cia_int.v#L56`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cia_int.v#L56)), and the module has no SP or CNT port at all ([`ciab.v#L4-L22`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciab.v#L4-L22), [`minimig.v#L643-L661`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L643-L661)).

CIA-A's four-bit counter of timer A's underflows, which ends a run at the sixteenth, is the in-tree pattern a fix reuses: [`ciaa.v#L197-L217`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciaa.v#L197-L217).

### Measured

[`integration/appF-cia-system`](../../integration/appF-cia-system), proposition P7.6, loads CIA-B's timer A with 10, writes CRA ← $41 (START, SPMODE output, continuous) and then SDR ← $A5, and reads CIA-B's ICR 300 times, storing every read (`p76_ciab.s`). It grades, on fx68k and on TG68K:

| run | fx68k | TG68K | graded by |
|---|---:|---:|---|
| CIA-B, `p76_ciab.s`: samples showing TA | 136 of 300 | 111 of 300 | `P7.6-CIAB-PRE`, green |
| CIA-B, `p76_ciab.s`: samples showing SP | **0 of 300** | **0 of 300** | `P7.6-CIAB`, **red** |
| CIA-A, `p76_ciaa.s`, the same program: a sample showing SP | yes | yes | `P7.6-CIAB-GREENCTL`, green |

**Where SP appears.** Read with the package's own two programs and harness in a run whose sample indices `make test` does not print — a reading for this page, not a graded assertion — on both cores alike: on CIA-A the SP bit shows first in sample 34, the sample that shows timer A's sixteenth underflow, which is p. 345's eight bits at "1/2 the underflow rate of Timer A". On CIA-B the underflows go on through all 300 samples and SP never shows. The samples are 192 to 208 clk_sys apart and no two neighbours both show TA, so each flagged sample is one underflow.

## Risks and unknowns

- **Proved:** CIA-B generates no transmit-completion (SP) interrupt after a write to SDR in output mode. **Not measured:** the shifted data, the SP and CNT pins, input mode, and Copperline's rate of one underflow a bit where p. 345 gives two; `P7.6-CIAB` grades only that SP is set, so Copperline would pass it.
- **The system gives the port no task.** p. 345 calls CIA-B's shift register unassigned and p. 340 calls its register unused, so the finding is about the chip's own function, not about a function the Amiga's system assigns. Whether a Minimig-derived core has to carry an unassigned port is open; all four implementations carry it, and neither Minimig line does.
- The suggested change has not been run on CIA-B here.
- The TRM names BUSY the clock and POUT the data, the other way round from p. 324; this finding reads the port's interrupt, not its pins.
- The silicon reading is of one revision, by connection and not by simulation; how many underflows make a bit, and at which step SP_INT fires, are not traced.

## Independent evidence

### What the others say

The question put to each: on CIA-B, with CRA bit 6 set, does a byte written to SDR shift out on timer A's underflows and set ICR bit 3 when it is done?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:575-585`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L575-L585), [`cia.cpp:510`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L510), [`:654-656`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L654-L656), [`:1846-1854`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1846-L1854) | one model for both chips, and the port runs on CIA-B: a write to SDR with SPMODE and START set loads the shift register, timer-A underflows count it down two to a bit, and at zero ICR_SP is set |
| **vAmiga** | [`Core/Components/CIA/CIA.cpp:494-529`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L494-L529), [`Core/Components/CIA/CIA.h:707`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.h#L707), [`:661-662`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L661-L662) | one model for both chips, and the port runs on CIA-B: `CIAB` overrides only the port and interrupt-line functions, every underflow toggles the serial clock so a bit takes two, and the last bit sets ICR bit 3 |
| **Denise** | [`emulation/cia/new/cia.cpp:267-269`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L267-L269), [`emulation/libami/system/system.h:48-49`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.h#L48-L49), [`:121`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L121) | one model for both chips, and the port runs on CIA-B: both CIAs are instances of one template, each underflow with CRA bit 6 set calls `shiftOut()`, and the scheduled finish sets the SP bit; only CIA-A's pins are connected, CIA-B's SP and CNT go nowhere ([`emulation/libami/system/system.cpp:38-41`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L38-L41)), and its ICR bit is set all the same |
| **Copperline** | [`src/chipset/cia.rs:10-18`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L10-L18), [`src/chipset/cia.rs:878-890`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L878-L890) | one model for both chips, and the port runs on CIA-B, at one underflow a bit: eight underflows return `ICR_SP` |
| **MiSTer** | [`rtl/ciab.v:118-129`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L118-L129), [`:238`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L238) | descendant — the same gap, stated in the file: *"CIA B's serial port is not implemented in this simplified version"* over the same SDR latch, and `.ser(1'b0),               // Serial port not implemented` |
| **TC64** | [`rtl/minimig/ciab.v:71-81`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L71-L81), [`:186`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L186), [`:209`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciab.v#L209) | descendant — identical to NanoMig on this point: `// fake serial port data register`, `.ser(1'b0),` and `.spmode()`. Neither line has a CIA-B serial port, and both keep CIA-A's transmit counter |

**All four implement the port on CIA-B, and none ties its SP bit to 0.** Each runs the same CIA model for both chips, shifts a byte written to SDR out on timer A's underflows while CRA bit 6 is set, and sets ICR bit 3 when the byte is out. WinUAE, vAmiga and Denise take two underflows a bit, p. 345's rate; Copperline takes one, so its SP bit comes after eight underflows where p. 345 gives sixteen. Three of them also model the board wiring pp. 337 and 351 draw, CIA-B's SP and CNT on PA0 and PA1.

### What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 220 and 106 — agrees with the manual:

  > "Transmission wilt start following a write to the Serial Data Register (provided TIMER A is running and in continuous mode)."
  <!-- corpus: a500-a2000-trm-1987/p220.txt -->

  > "After 8 CNT pulses, an interrupt is generated to indicate more data can be sent."
  <!-- corpus: a500-a2000-trm-1987/p220.txt -->

  > "signals that together can also function as a synchronous serial data port driven by an 8520 CIA device"
  <!-- corpus: a500-a2000-trm-1987/p106.txt -->

  > "For serial port usage, BUSY is the serial clock, POUT is the serial data line."
  <!-- corpus: a500-a2000-trm-1987/p106.txt -->

  The TRM gives the output mode in p. 345's words, for the part the A500 and A2000 carry ("wilt" is the OCR's reading of "will"), and says BUSY and POUT together can serve as a synchronous serial port driven by an 8520, the port pp. 324, 337 and 351 wire to CIA-B.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, the overview on sheet 1 and schematic sheets 5d and 5e, 10a, 11a and 11b — agrees with the manual as far as it is read here. *Read from the drawing, not quoted:* the silicon has the output mode p. 345 describes: SDR loaded into a shift register, its bit 7 on SP, a clock derived from timer A's underflow on CNT, the pads driven only while SPMODE (CRA6) is set and open collector, as p. 346 says, and an interrupt, SP_INT, into ICR bit 3 (IFR3#). Nothing in the drawing depends on which of the two sockets the part sits in.
  <!-- drawing: cia-8520r4-dissection/sheet_001.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_007.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_015.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_016.txt -->

## Decision

**One option: give `ciab.v` the transmit side `ciaa.v` already has.** Connect timer A's `spmode` output, copy the `ser_tx_run` and `ser_tx_cnt` logic of `ciaa.v` lines 197–217 — a run started by a write to SDR in output mode and ended at the sixteenth underflow of timer A — and feed its end to the interrupt controller in place of `1'b0`. `P7.6-CIAB-GREENCTL` shows that logic, in CIA-A, setting SP with P7.6-CIAB's own program. What it would change:

- `P7.6-CIAB` would see SP at the sixteenth underflow, as CIA-A does;
- the byte would still reach no pin. Bringing SP and CNT out to PA0 and PA1, the parallel port's BUSY and POUT as pp. 337 and 351 wire them, and the input mode, which needs a CNT edge from outside, are a larger change;
- NanoMig would move toward all four implementations and away from both Minimig lines, neither of which has the port.
