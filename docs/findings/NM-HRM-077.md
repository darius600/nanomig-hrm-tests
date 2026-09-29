# NM-HRM-077 — CIA-A has no PC output and ties FLAG to 0, where p. 351 wires them to the parallel port's DRDY\* and ACK\*

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | seen in the RTL source; no test yet |
| **What is asked of a reader** | Nothing yet. p. 351 gives CIA-A's output pulse pin, PC, to the parallel port's data-ready line and its edge detection input, FLAG, to the port's acknowledge; NanoMig's `ciaa` has no PC output and ties FLAG to 0, so the handshake pp. 259 and 324 describe cannot take place. No test grades it: `ciaa` has no pin at which a test could look, and only a device on the parallel port, which NanoMig does not have, could see the difference. |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L321) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L357) |
| **Report to** | **nobody, as things stand.** NanoMig has `ciaa.v` from the Minimig line, and both lines carry the same port list and the same tie-off, so there is no fix to port; MiSTer's comment at its tie-off calls the pin unused, which p. 351 does not. A change belongs in **MiSTer-devel/Minimig-AGA_MiSTer** only if a core is to drive a parallel device — see *What the others say* |
| **Test** | none grades it. [`hrm/appF-cia-register-map`](../../hrm/appF-cia-register-map) records `CHIP-CAPABILITIES-FIVE`, claim `opus:F-intro.c3` (p. 339), as a RECORDED line, not an assertion: of p. 339's five capabilities, PC and FLAG, the two p. 341 names, have no pin at `ciaa`'s boundary to drive or read (`test_appF-cia-register-map.cpp:1292-1308`). The same two capabilities on CIA-A's own lines of p. 351, `sol:F-signals.c30`, `sol:F-signals.c31`, `opus:F-portsig.c26`, `opus:F-portsig.c27`, and p. 339's output pulse pin read on its own, `sol:F-overview.c6`, are classified absent and not asserted: `ciaa` has no port at which a test could look. [`integration/appF-cia-system`](../../integration/appF-cia-system) grades neither pin at the system's ports: its specification finds no PC pin on either CIA among the outputs a test can see, and none of its assertions reads CIA-A's FLG bit |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 339, 341, 345, 351; Appendix E, printed pp. 319, 324, 337; 8 — Interface Hardware, printed p. 259 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | none: the package reads `22 passed, 0 failed`, with `CHIP-CAPABILITIES-FIVE` recorded, not graded. Its control, `CONTROL-CAPABILITIES-CORE`, green, grades the three capabilities `ciaa` has, 16 pins, the serial register and three timers, from constants the test writes; neither line reads the fixture |
| **Verdict of the four** | **split, two and two, along whether they attach a parallel device — and with nothing attached all four read as NanoMig does.** Copperline pulses CIA-A's PC on every port-B read or write, hands the pulse to the attached device as its strobe, and feeds the device's acknowledge into CIA-A's FLAG as a falling edge; WinUAE sets CIA-A's FLAG after each port-B write when a printer or a host parallel port is configured, strobing the host port, and again when the host port's ACK goes active; its parallel-port SCSI adapter raises the same acknowledge. vAmiga and Denise have no PC output and never set CIA-A's FLAG: in both, the FLAG input their CIA model has is driven only by the floppy index, on CIA-B |
| **See also** | [NM-HRM-054](NM-HRM-054.md) (CIA-A's port B, the parallel port's data lines, has no data register), [NM-HRM-074](NM-HRM-074.md) (PB6 and PB7 never carry the timer output, a function of two port pins that p. 341 keeps apart from PC), [NM-HRM-063](NM-HRM-063.md) (the FLG bit is set from a level, which a CIA-A FLAG input would inherit) |

---

## What the HRM says

**Printed p. 339**, the sentence the package records, with the one before it and the one after it:

> Each Amiga system contains two 8520 Complex Interface Adapter (CIA) chips.
> Each chip has 16 general purpose input/output pins, plus a serial shift
> register, three timers, an output pulse pin and an edge detection input. In
> the Amiga system various tasks are assigned to the chip's capabilities as
> follows:

**Printed p. 341** names the two pins the list ends with, and keeps them apart
from the port pins:

> In addition to their normal I/O operations, ports PB6 and PB7 also provide
> timer output functions.

> Handshaking occurs on data transfers using the PC output pin and the FLAG
> input pin. PC will go low on the third cycle after a port B access. This
> signal can be used to indicate "data ready" at port B or "data accepted" from
> port B.

> FLAG is a negative edge-sensitive input that can be used for receiving the PC
> output from another 8520 or as a general-purpose interrupt input. Any
> negative transition on FLAG will set the FLAG interrupt bit.

So the output pulse pin is PC, and the edge detection input is FLAG. PB6 and
PB7 are two of the sixteen port pins, whose timer outputs pp. 342 and 348–350
specify separately.

**Printed p. 351**, *Port Signal Assignments*, read from the render
(`appF/figures/p351.png`) because the text layer moves this column by a row,
assigns both to the parallel port on CIA-A and only one of them on CIA-B:

| | PC | FLAG |
|---|---|---|
| CIA-A (`BFEr01`, INT2) | `PC...drdy*` — "centronics control" | `F....ack*` |
| CIA-B (`BFDr00`, INT6) | `PC...not used` | `F....INDEX*` — "disk index*" |

Appendix E's **p. 337** prints the same two blocks, CIA-A's PC and FLAG again
beside "Centronics control". Its **p. 319** puts DRDY\* (the A1000's name;
STROBE\* on the A500, A2000 and A3000) on pin 1 of the parallel connector and
ACK\* on pin 10, and **p. 324** says what the two do:

> Output-data-ready signal to parallel device in output mode, used in
> conjunction with ACK\* (pin 10) for a two-line asynchronous handshake.

> Output-data-acknowledge from parallel device in output mode, used in
> conjunction with DRDY\* (pin 1) for a two-line asynchronous handshake.

> The 8520 can be programmed to conditionally generate a level 2 interrupt to
> the 680x0 whenever the ACK\* input goes active.

Level 2 is CIA-A's: p. 339's map ends *"Note: CIAA can generate interrupt
INT2."* Chapter 8, **printed p. 259**, says the same of the machine as a whole:

> For each data byte written to the parallel port register, the hardware
> automatically generates a pulse on the data ready pin. The acknowledge pulse
> from the parallel device is hooked up to an interrupt. For pin connections
> and timing, see Appendix E and F.

### The part, or the CIA as the Amiga wires it

**p. 339's list describes the part.** Its subject is *"each chip"*, and the next
sentence turns from the part to the machine: *"In the Amiga system various
tasks are assigned to the chip's capabilities"*. The appendix keeps the two
apart elsewhere too. **Printed p. 345** gives every 8520 a shift register and
then says

> In the Amiga system one shift register is used for the keyboard, and the
> other is unassigned.

and p. 351 marks CIA-B's PC *"not used"*. So a capability can be the part's and
still be connected to nothing on an Amiga, and a red built from p. 339 alone
at such a pin would test the part where the machine asks nothing. **CIA-B's PC
is such a pin. CIA-A's two are not:** p. 351, p. 337, p. 324 and p. 259 all
give CIA-A's PC and FLAG a job, the parallel port's handshake. Read as the part
or as CIA-A in an Amiga, the page asks the same of `ciaa`, and the package's
fixture is the right object. No page read here puts CIA-A's PC or FLAG out of
use.

The object decides the colour, which is why it matters. The suite grades the
edge detection input on its own as well, `sol:F-overview.c7`, and there it is
green: `PROP-FLAG-EXISTS` in
[`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments) drives
`ciab`'s FLAG and sees the ICR bit set. Read as the part, the clause is
satisfied by one of NanoMig's two CIAs; read for CIA-A, it is not.

---

## What NanoMig does

**`ciaa` has no pin on which a pulse could leave after a port-B access, and no
pin on which an acknowledge could arrive.** Its outputs are the data bus, the
interrupt, four port-A lines and the freeze key —
[`src/minimig-aga/ciaa.v#L91-L102`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L91-L102):

```verilog
	output [7:0] data_out,      // bus data out
	input        tick,          // tick (counter input for TOD timer)
	input        eclk,          // eclk (counter input for timer A/B)
	output       irq,           // interrupt request out
	input  [7:2] porta_in,      // porta in
	output [3:0] porta_out,     // porta out
	input  [7:0] portb_in,      // portb in
```

and its interrupt controller's FLAG input is a constant —
[`src/minimig-aga/ciaa.v#L321`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L321):

```verilog
  .flag(1'b0),
```

The interrupt controller sets the FLG bit from nothing else, on a read and
between reads:

- [`src/minimig-aga/cia_int.v#L49`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_int.v#L49), `icr[4] <= flag;`
- [`src/minimig-aga/cia_int.v#L57`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_int.v#L57), `icr[4] <= icr[4] | flag;`

so bit 4 of CIA-A's ICR is 0 for good, whatever the mask says.

The module's own list of what it leaves out names port B and the PB6/PB7 timer
outputs, and neither PC nor FLAG:

- [`src/minimig-aga/ciaa.v#L23-L27`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L23-L27), `// port B for CIA A` … `// toggling of PB6/PB7 by timer A/B`

CIA-B is built differently: it declares a FLAG input, which the top level
drives from the disk index — p. 351's CIA-B assignment:

- [`src/minimig-aga/ciab.v#L17`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L17), `input   flag,         // flag (set FLG bit in ICR register)`
- [`src/minimig-aga/minimig.v#L647`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L647), `.flag(index),`

**Nothing in the tree is a parallel device.** CIA-A's port B reads the two
extra joystick inputs, the four-player adapter that plugs into the parallel
connector and uses neither handshake line:

- [`src/minimig-aga/minimig.v#L624`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L624), `.portb_in({_joy4[0],_joy4[1],_joy4[2],_joy4[3],_joy3[0],_joy3[1],_joy3[2],_joy3[3]}),`

The top level's port list carries no printer or Centronics signal, and no HDL
or pin-constraint file in NanoMig's tree, the board top levels included,
names one.

### Measured

A run of the package, `make -B test` in `hrm/appF-cia-register-map` under
WSL, reads `22 passed, 0 failed`, and `wsl.exe` exits 0. p. 339's sentence is a
RECORDED line, not an assertion:

```
RECORDED: CHIP-CAPABILITIES-FIVE: "Each chip has 16 general purpose input/output pins, plus a serial shift register, three timers, an output pulse pin and an edge detection input" [p.339] opus:F-intro.c3 -- p.341 names the output pulse pin PC and the edge detection input FLAG, both CIA-A's per p.351, p.337, p.324 and p.259 (docs/findings/NM-HRM-077.md). [...] The other two, PC and FLAG, have no pin at ciaa's own boundary to drive or read; a comparator built from constants the test writes itself is not a measurement of them. 16-pins=yes serial-reg=yes three-timers=yes output-pulse-pin(PC)=no (no PC pin at ciaa's boundary) edge-detect-input(FLAG)=no (no FLAG pin at ciaa's boundary). Not graded here.
PASS: CONTROL-CAPABILITIES-CORE: [...]
```

No assertion at `ciaa`'s ports can grade the finding. There is no port on
which to see a PC pulse or to drive a FLAG edge; what the ports do show — FLG
never set in the ICR — is also what an Amiga shows while no device drives
ACK\*. The finding rests on the port list above.

---

## What the others say

The question put to each: after a CIA-A port-B access, does anything leave the
CIA as p. 341's PC pulse, and does an acknowledge from outside ever set CIA-A's
FLG bit?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:823-830`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L823-L830), [`:2148-2157`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2148-L2157), [`:1946-1951`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1946-L1951), [`od-win32/ioport.cpp:331-338`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/ioport.cpp#L331-L338), [`:359-365`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/ioport.cpp#L359-L365), [`:303-307`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/ioport.cpp#L303-L307), [`od-win32/parser.cpp:382-383`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/parser.cpp#L382-L383), [`scsi.cpp:4144-4146`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/scsi.cpp#L4144-L4146) | CIA-A's FLAG is the parallel acknowledge and CIA-B's the disk index: `cia_parallelack` is `CIA_sync_interrupt(0, ICR_FLAG);` and `cia_diskindex` is `CIA_sync_interrupt(1, ICR_FLAG);`. A CIA-A port-B write with a printer configured runs `doprinter(val);` and `cia_parallelack();` — the printer's acknowledge, at once; with a host parallel port it runs `parallel_direct_write_data(val, c->drb);`, which pulses the host port's strobe, `c[1].Control = PARAPORT_MASK_CONTROL_STROBE;`, and `cia_parallelack();` again; a port-B read with a host port runs `parallel_direct_read_data(&v);`, which strobes it too. Reading the host port's status, an ACK going active sets CIA-A's FLAG once more, `if (!oldack)` / `cia_parallelack ();`. Its parallel-port SCSI adapter raises the same acknowledge, `if ((t & SCSI_IO_REQ) && (scsi->chip_state & 4))` / `cia_parallelack();`. There is no PC pin as such: the port-B access stands for it. With no printer named, `if (!currprefs.prtname[0])` / `return 0;`, and no such adapter, nothing sets CIA-A's FLAG. **Carries the handshake to a configured device** |
| **vAmiga** | [`Core/Components/CIA/CIA.cpp:257-266`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L257-L266), [`Core/Peripherals/Drive/FloppyDrive.cpp:859-862`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L859-L862), [`Core/Components/CIA/CIA.cpp:943-946`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L943-L946) | the CIA model has a FLAG input, `CIA::emulateFallingEdgeOnFlagPin()`, which sets `icr \|= 0x10;`; its one caller in the tree is the floppy drive, `if (isSelected()) ciab.emulateFallingEdgeOnFlagPin();`, so only CIA-B's FLAG ever falls. CIA-A's port B has no outside driver, `CIAA::portBexternal() const` / `return 0xFF;`, and nothing in the CIA pulses on a port-B access. **No PC output, and CIA-A's FLAG is never set** |
| **Denise** | [`emulation/cia/new/cia.cpp:302-305`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L302-L305), [`emulation/libami/system/system.cpp:22`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L22), [`:59`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L59), [`:68-88`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L68-L88) | the shared CIA template has a FLAG input, `auto Cia<model>::setFlag() -> void {` / `intIncomming \|= 0x10;`, called only from the disk drives, and every drive is built on the second CIA, `diskDrives { {0, this, agnus, cia2}, {1, this, agnus, cia2},`. CIA-A's port-B write handler is an empty branch, `} else {` / `// parallel port`, and the CIA has no PC line. CIA-A's port B reads the multi-player adapter instead, `input.readParallelportCIA1B(out);` — the use NanoMig makes of it. **No PC output, and CIA-A's FLAG is never set** |
| **Copperline** | [`src/chipset/cia.rs:573-575`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L573-L575), [`:1001-1004`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1001-L1004), [`src/bus.rs:9208-9224`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L9208-L9224), [`src/parallel.rs:3-14`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/parallel.rs#L3-L14), [`:65-70`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/parallel.rs#L65-L70) | a port-B write and a port-B read both latch a PC pulse, `self.pc_pulse_pending = true;`. The bus hands CIA-A's to the attached device as its strobe and turns an accepted byte into a falling edge on CIA-A's FLAG: `if !self.cia_a.take_pc_pulse() {` … `if self.parallel_port.strobe(data, self.emulated_cck) {` / `let _ = self.cia_a.assert_flag();`, under the comment *"Consume CIA-A's one-shot `PC` pulse as the external Centronics `/STROBE`"*. The module comment says the same of the wiring, *"CIA-A's `PC` output is the active-low printer strobe"*, and of the default device, *"The default null peripheral is an unplugged cable: it neither acknowledges nor drives any pin"* — its `strobe` returns `false`. **PC and FLAG as p. 341 and p. 351 give them, for an attached device** |
| **MiSTer** | [`rtl/ciaa.v:357`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L357), [`:122-123`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L122-L123) | descendant — the same tie-off, under a comment p. 351 contradicts, `.flag(1'b0),                      // FLAG pin not used on CIA A`, and no PC output in a port list that labels port B `// Port B - Parallel port data (Centronics printer interface)` and its input `// Port B inputs (not used in Amiga)` |
| **TC64** | [`rtl/minimig/ciaa.v:297`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L297), [`:94-97`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L94-L97) | descendant — the same tie-off, `.flag(1'b0),`, and the same outputs, `output   irq,` and `output   [3:0] porta_out,`, with no PC. Neither line has a fix to port |

**The four split along one line: whether they attach a parallel device.**
Copperline and WinUAE do, and both give CIA-A's FLAG to the device's
acknowledge, as p. 351 does; Copperline also models PC as a pulse on every
port-B access, WinUAE only as the host port's strobe. vAmiga and Denise attach
none, and like NanoMig never set CIA-A's FLAG and have no PC — Denise, like
NanoMig, reads a multi-player joystick adapter on CIA-A's port B instead.
None of the four gives CIA-A's FLAG to anything but the parallel acknowledge,
and none drives it with nothing attached: Copperline's default device never
acknowledges and WinUAE's default configuration names no printer. **With the
parallel port empty, all four behave as NanoMig does**, which is why the
difference needs a device to show.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 106 — agrees with the manual:

  > "This is the acknowledge (/ACK) input, the same as the acknowledge (/ACK) input to the parallel port. Driving this with an output from a Video Card can cause a level 2 interrupt to occur through the 8520 CIA device this is connected to, based on the programming of an 8520 register."
  <!-- corpus: a500-a2000-trm-1987/p106.txt -->

  The B2000's extended video slot carries the parallel port's acknowledge, and the interrupt it raises is level 2, CIA-A's: the board wires the acknowledge to CIA-A, as p. 351 and p. 324 do.

- **Commodore, 6526 CIA specification, 1981**, PDF page 6 — agrees with the manual on the part:

  > "PC will go low for one cycle following a read or"
  <!-- corpus: cia-6526-spec/p006.txt -->

  The 6526's handshake paragraph gives PC, pulsing after a read or write of port B, and FLAG, *"a negative edge sensitive"* input, as p. 341 gives them for the 8520; the OCR runs the sentence into the next column, so the quotation stops at the column break. It describes the part and says nothing of the Amiga's wiring.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, the overview sheet and schematic sheets 4a to 4c, 5a, 5c to 5e, 8a to 9e, 10, 11 and 14 — agrees with the manual:

  *Read from the drawing, not quoted:* the overview sheet, "0) cheat sheet", draws the whole chip: port lines PA0 to PA7 and PB0 to PB7, each with a data and a direction bit; timer A and timer B, 16-bit down counters; TOD, a 24-bit binary up counter with an alarm; an 8-bit shift register with its latch on the SP and CNT pads; a falling-edge detector on the FLAG# pad; the PC# pad; and an interrupt register with the sources TA_INT, TB_INT, ALARM_INT, SP_INT and FLAG_INT. Sheets 4a to 4c draw the port cells, 5a FLAG#, 5c PC#, 5d SP, 5e CNT, 8 and 9 the timers, 10 the interrupt register, 11 the serial port and 14 TOD, most of them transistor by transistor. Sheet 5c's concept drawing pulses PC# low after a PRB read or write.
  <!-- drawing: cia-8520r4-dissection/sheet_001.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_005.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_006.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_007.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_015.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_016.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_020.txt -->

  The part has all of p. 339's capabilities: sixteen port lines, a serial shift register, three timers if TOD is counted among them, an output pulse pin, PC#, and an edge-detecting input, FLAG#, whose falling edge sets the interrupt register's FLAG bit; that register has the five sources p. 346 lists. Which board lines the pins reach is outside the chip. PC#'s pulse is read from the concept drawing, not traced. The dissection is of one revision.

---

## Suggested change

Not an RTL change as NanoMig stands: without a device on the parallel port, a
CIA-A with PC and FLAG would read exactly as the present one does. If a core is
to drive a printer or another parallel device, the change is in `ciaa.v`:

- a PC output that goes low after each CIA-A port-B read or write, p. 341's
  *"third cycle after a port B access"*, brought out through `minimig` to the
  device as its strobe;
- a FLAG input in place of `1'b0` at line 321, brought in from the device's
  acknowledge. `cia_int` would then set the FLG bit from the level of that
  input, which is [NM-HRM-063](NM-HRM-063.md)'s question, so the input wants
  the negative edge p. 341 asks for.

This has not been run here. It depends on [NM-HRM-054](NM-HRM-054.md) as well:
without a port-B data register, a strobe would announce a byte that the port
never drives.

---

## What holds the finding up

- **The port list and the tie-off.** `ciaa.v` has no output that could carry a
  pulse after a port-B access and no FLAG input, and line 321 ties the
  interrupt controller's FLAG to 0; `cia_int.v` sets the FLG bit from nothing
  else. Both Minimig lines are the same.
- **The pages agree with each other.** p. 339 lists the two pins as the part's,
  p. 341 says what they do, and p. 351, p. 337, p. 324, p. 319 and p. 259 give
  both of CIA-A's to the parallel port's handshake. The one pin the manual puts
  out of use is CIA-B's PC, and `ciab`'s FLAG is wired as p. 351 says.
- **No test.** `CHIP-CAPABILITIES-FIVE` is recorded, not graded: `ciaa` has
  no PC or FLAG pin to drive or read. The claims that name the pins on p. 351
  are classified absent and not asserted, for the reason this page gives.
- **What weakens it is that nothing can see it here.** No signal of a parallel
  port leaves NanoMig's tree, and nothing in it is a parallel device; the one thing wired to
  CIA-A's port B, the joystick adapter, uses neither line. No program can read
  PC, and FLAG stays clear on an Amiga too while no device drives ACK\*.

---

## For the specialists

1. **Should a core with no parallel connector carry CIA-A's handshake at all?**
   The manual gives CIA-A's PC and FLAG a job on every Amiga it describes; the
   Minimig lines and two of the four implementations leave both out, and MiSTer
   calls CIA-A's FLAG unused. Nothing observable changes until a device is
   attached, so the question is one of scope, not of behaviour.
2. **If it does, which pulse?** p. 341 has PC go low *"on the third cycle after
   a port B access"*; the 6526 specification has it low for one cycle after a
   read or write of port B. Copperline pulses on every access; WinUAE strobes
   only a host parallel port, on a port-B write and on a port-B read.
