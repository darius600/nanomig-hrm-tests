# NM-HRM-083 — an output bit of CIA-A port B reads back a line held low, not the 1 written to it, where p. 324 gives D0–D7 the direction I/O

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The red is measured with every port-B line held low, and what fails is a written 1 reading back as the line's 0; p. 341 makes a read the state of the pin, and no page says what that state is when an output drives 1 and something outside holds the line low. Where the page does decide, an output driven 0 on a released line, NanoMig returns the line where p. 341 predicts 0 — and that is [NM-HRM-054](NM-HRM-054.md), not a new finding. |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L291-L299) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L325-L334) |
| **Test** | [`hrm/appDE-parallel-port-ciaa`](../../hrm/appDE-parallel-port-ciaa) — `PORTB-DIRECTION`, claims `sol:E-parallel.c69`, `sol:E-parallel.c70`, `sol:E-parallel.c71`, `sol:E-parallel.c72`, `sol:E-parallel.c73`, `sol:E-parallel.c74`, `sol:E-parallel.c75`, `sol:E-parallel.c76`, `opus:E-parallel-iface.c10` (p. 324), still fails, because its fixture holds all eight lines at 0 (`test_appDE-parallel-port-ciaa.cpp:211`). `PORTB-WIDTH`, the same nine claims, is green |
| **Chapter** | Appendix E, printed pp. 324, 325, 337; Appendix F — 8520 Complex Interface Adapters, printed pp. 339, 341, 351, 354; 8 — Interface Hardware, printed p. 259 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 3 (`2 passed, 1 failed`): `PORTB-DIRECTION` sets one position's bit in CIAADDRB, writes 0 and then that bit to CIAAPRB, and reads CIAAPRB after each, at D0 to D7 in turn (`test_appDE-parallel-port-ciaa.cpp:321-347`); every position prints `writable=no`. `FIXTURE-LIVE` (CIAADDRB written and read back) and `PORTB-WIDTH` (each `portb_in` bit reaching its own CIAAPRB bit) are green |
| **Error class** | **a silence read as a requirement** |
| **See also** | [NM-HRM-054](NM-HRM-054.md) — the missing port-B data register, measured at bit 0 from p. 259; [NM-HRM-040](NM-HRM-040.md) — CIA-A port A, where the same held-low case was left undecided and the package releases the lines; [NM-HRM-077](NM-HRM-077.md) — the port's PC and FLAG handshake |
| **Verdict of the four** | **unanimous against NanoMig where the page decides, split where the package measures.** On a released line WinUAE, vAmiga, Denise and Copperline all return the written value from an output bit of CIA-A port B, 0 after a written 0, where NanoMig returns the line, 1 — NM-HRM-054's verdict, at all eight positions. Over a line held low, the state the package presents, Denise and Copperline (against its four-player adapter) return the written 1; WinUAE, with its parallel-port joystick adapter holding the line, returns the line's 0 as NanoMig does, except with its 391078-01 CIA type; vAmiga has nothing that can hold a CIA-A port-B line low |

---

## Why it was withdrawn

**The assertion expects a value no page gives.** For each position n,
`PORTB-DIRECTION` writes CIAADDRB = `1<<n`, then CIAAPRB = 0 and CIAAPRB =
`1<<n`, and requires the read after each to be 0 and then `1<<n`
(`test_appDE-parallel-port-ciaa.cpp:321-329`). The harness sets
`dut->portb_in = 0;` (`:211`) and nothing in the sweep changes it, so every
port-B line is at 0 throughout.

**A line at 0 is a line held low.** p. 341 gives both ports passive pull-ups,
and p. 354 says a read of the ports after reset *"will return all highs because
of passive pull-ups"*: a line nothing drives reads 1. So the package presents
all eight lines pulled low from outside, and in that state:

- **the written 0 decides nothing.** The line is 0 and the output drives 0, so
  a read that returns the written value and a read that returns the line both
  give 0; an 8520 with a data register and NanoMig without one read alike;
- **the written 1 is the half that fails, and no page decides it.** p. 341
  makes a read *"the actual current state of the I/O pins"*, output bits
  included. With the output driving 1 and the line held low, the pin is
  whichever of the two wins, and no page of the manual says which. The
  assertion requires 1, the output winning.

The one state in which p. 341 predicts the read of an output bit — a line
nothing else drives, which then carries only what the output drives — is not
presented. [NM-HRM-040](NM-HRM-040.md) reads the same pages the same way for
port A and leaves the held-low case undecided; its package releases every line
for that reason.

**The connector is not the problem.** p. 324's DIR column describes the pins of
J8, not a register. What makes it a statement about CIAAPRB is that p. 337 and
p. 351 give J8's data lines to CIA-A's PB7 to PB0, p. 339 calls the direction
register *"can be in or out"*, and p. 341 says what a port bit set to output
does. That route is sound, and the expected values of the assertion are p. 341's
wherever a line is released. That NanoMig has no J8 at all — its port B is the
four-player joystick adapter,
[`minimig.v:624`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L624),
`.portb_in({_joy4[0],_joy4[1],_joy4[2],_joy4[3],_joy3[0],_joy3[1],_joy3[2],_joy3[3]}),`
— does not matter at the `ciaa` boundary, where the package grades. It is the
fixture's state that decides nothing.

**What remains is NM-HRM-054's.** On a released line NanoMig does depart from
p. 341: a CIAAPRB read is the line whatever was written, so an output driven 0
reads 1. That is the finding [NM-HRM-054](NM-HRM-054.md) registers, from
p. 259 and p. 341, for the same two statements of `ciaa.v`; p. 324's eight rows
add no mechanism of their own. The per-line direction the claims were graded
for is real in the RTL as a register — CIAADDRB is stored and reads back bit
for bit, `FIXTURE-LIVE` — and governs no read, which NM-HRM-054 already says.

---

## What the HRM says

**Printed p. 324**, the parallel connector:

> In this capacity, data flows from the Amiga to the printer. This interface
> may also be used for input or bidirectional data transfers.

Its `PARALLEL PORT (J8)` table, *read from the render*, gives each of D0 to D7
the direction `I/O`, with a bracketed note across the eight rows:

> D0-D7 comprise an eight-bit bidirectional bus ... for communication with
> parallel devices, nominally, a printer.

**Printed p. 325**, *read from the drawing, not quoted*: one timing diagram
puts the Amiga's data on `PB<7:0>` in an output cycle, the other the device's in
an input cycle. Neither has both sides driving the lines.

**Printed p. 337**, Part 4, and **p. 351**, *read from the render, not quoted*
(both text layers are shuffled): CIA-A's PB7 to PB0 carry P7 to P0, *data 7* to
*data 0*, labelled *Centronics parallel interface data*; its PC is *drdy\** and
its F *ack\**. **Printed p. 339**, *read from the render*: `prb` is `Parallel
port`, and `ddrb` is `Direction for port B (BFE101);1=output (can be in or
out)`.

**Printed p. 341**, the rule the assertion's values come from:

> Ports A and B each consist of an 8-bit peripheral data register (PR) and an
> 8-bit data direction register (DDR). If a bit in the DDR is set to a 1, the
> corresponding bit position in the PR becomes an output. If a DDR bit is set
> to a 0, the corresponding PR bit is defined as an input.
>
> When you READ a PR register, you read the actual current state of the I/O
> pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs
> or outputs.
>
> Ports A and B have passive pull-up devices as well as active pull-ups,
> providing both CMOS and TTL compatibility. Both ports have two TTL load drive
> capability.

**Printed p. 354**, the state of a pin nothing drives:

> The port pins are set as inputs and port registers to zero (although a read
> of the ports will return all highs because of passive pull-ups).

**Printed p. 259**, chapter 8's parallel interface:

> The general-purpose bi-directional parallel interface is a 25-pin connector
> on the back panel of the computer.

So the read of an output bit is the pin. On a line nothing else drives, the pin
is what the output drives: a written 0 reads 0 and a written 1 reads 1, at every
position. Where a device holds the line low while the output drives 1, the pages
say that the port has passive and active pull-ups and that the read is the pin,
and none of them says which of the two sets it.

---

## What NanoMig does

**CIA-A port B has a direction register and no data register, and a read is
the line** —
[`src/minimig-aga/ciaa.v#L267-L279`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L267-L279),
[`#L291-L299`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L291-L299):

```verilog
//reg [7:0] regportb;                    // :267, and its write block :270-279 commented out
reg [7:0] ddrportb;                      // :268
...
  if (!wr && prb)
    pb_out[7:0] = portb_in[7:0];         // :293-294
  else if (!wr && ddrb)
    pb_out[7:0] = (ddrportb[7:0]);       // :295-296
```

So in the package's state every CIAAPRB read is `portb_in`, $00: the written 0
reads 0 and the written 1 reads 0, at each position. On released lines every
read is $FF: the written 1 reads 1 and the written 0 reads 1.

### Measured

| `portb_in` | what it is | CIAAPRB after the written 0 | after the written 1 | |
|---|---|---|---|---|
| $00 | every line held low — the package as committed, `PORTB-DIRECTION` | $00 at all eight | $00 at all eight | red on the written 1, which no page decides |
| $FF | every line released, p. 354's idle state — the same comparator and fixture with `portb_in` set before the sweep, not part of the package | $FF at all eight | $FF at all eight | the written 0 reads 1 where p. 341 predicts 0: NM-HRM-054's behaviour |

The package itself prints only `writable=no` per position; the reads in the
table come from a run of its comparator that prints them, and for the first row
they follow from `ciaa.v:294` with the input at $00.

[`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction),
the package NM-HRM-054 cites, has the same two states. Its graded `PROP-1`
presents this package's at bit 0 (`dut->portb_in = 0;`,
`test_ch8-parallel-port-direction.cpp:210`), and fails on the written 1 read
back as the line's 0. Its recorded direction-independence line has the decided
one: with `portb_in` at $3C and CIAAPRB written $00, CIAAPRB reads $3C under
CIAADDRB = $00 and under CIAADDRB = $FF, so at PB2 to PB5, the lines $3C
releases, an output driven 0 reads 1.

---

## What the others say

The question put to each: does an output bit of CIA-A port B read back the
value written to it — on a released line, and over a line that something
attached holds low?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1943-1981`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1943-L1981), [`cia.cpp:2143`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2143), [`inputdevice.cpp:3795-3817`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3795-L3817), [`cfgfile.cpp:8694`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L8694), [`cfgfile.cpp:9954`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9954) | a write stores the byte, `c->prb = val;`, and a read of $BFE101 starts from `tmp = (c->prb & c->drb) \| (c->drb ^ 0xff);`: output bits the written value, input bits the pull-up's 1. **On a released line an output driven 0 reads 0, at all eight.** What can hold a line low is its parallel-port joystick adapter, applied after that mask whatever the direction, `tmp = handle_parport_joystick (0, tmp);`, which clears the bit of each pressed direction, `v &= ~(1 << shift);` — **so over a line held low an output driven 1 reads 0, as in NanoMig**. Its 391078-01 CIA type puts the written bits back afterwards, `tmp \|= c->prb & c->drb;`; that type is off by default, `p->cs_ciatype[0] = 0;`, and on for the A600, `p->cs_ciatype[0] = p->cs_ciatype[1] = 1;`. With a printer configured the whole byte is the written one, `tmp = c->prb;` |
| **vAmiga** | [`Core/Components/CIA/CIA.cpp:913-946`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L913-L946), [`Core/Components/CIA/CIARegs.cpp:35-39`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L35-L39), [`Core/Components/CIA/CIARegs.cpp:223-225`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L223-L225), [`Core/Components/CIA/CIA.h:553`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.h#L553) | a write is `pokePB(value);`, which stores it, `virtual void pokePB(u8 value) { prb = value; updatePB(); }`; a read returns `result = pb;`, computed as `u8 result = (internal & ddrb) \| (external & ~ddrb);` with the stored byte as `internal`, `return prb;`. **On a released line an output driven 0 reads 0, at all eight.** CIA-A's external port-B value is a constant, `return 0xFF;`: nothing attached can hold one of its lines low, so vAmiga has no answer for the state the package presents |
| **Denise** | [`emulation/libami/system/system.cpp:58-61`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L58-L61), [`emulation/cia/new/register.cpp:84-89`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L84-L89), [`emulation/cia/new/register.cpp:205-208`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L205-L208), [`emulation/libami/input/controlPort/fourPlayerAdapter.cpp:11-20`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/fourPlayerAdapter.cpp#L11-L20) | a write stores the byte, `lines.prb = value;`, and a read, `uint8_t out = readPort(PORTB, &lines);`, starts from released lines, `out = 0xff;`, lets the four-player adapter clear each pressed direction, `res &= ~1;`, and then masks: `out = (lines->prb & lines->ddrb) \| (out & ~lines->ddrb);`. **On a released line an output driven 0 reads 0, and over a line the adapter holds low an output driven 1 reads 1**, because every external contribution passes through `~ddrb` |
| **Copperline** | [`src/chipset/cia.rs:992-1004`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L992-L1004), [`src/chipset/cia.rs:483-484`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L483-L484), [`src/chipset/cia.rs:502-506`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L502-L506), [`src/bus.rs:9012-9026`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L9012-L9026) | a write stores every register, `self.regs[reg] = val;`, and a read of PRB is `self.port_b_pins()`, which begins `let mut v = self.read_port(REG_PRB, REG_DDRB);`, that is `(self.regs[port_reg] & ddr) \| !ddr`. **On a released line an output driven 0 reads 0, at all eight.** The bus then lays on what is attached: the four-player adapter's pull-downs through the direction mask, `v &= !(pulls & !self.cia_a.port_b_ddr());`, under the comment *"A pin the guest has switched to an output stays CIA-driven"* — **so over a line the adapter holds low an output driven 1 reads 1** — while an input peripheral such as its sampler replaces the whole byte, outputs included, `v = byte;` |
| **MiSTer** | [`rtl/ciaa.v:312-334`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L312-L334) | descendant — no data register at all, only `reg [7:0] ddrportb;`, and the read is the line, `pb_out[7:0] = portb_in[7:0];`, under the heading *"Port B - Parallel port (simplified, mostly unused in Amiga)"*. NanoMig's values in both states |
| **TC64** | [`rtl/minimig/ciaa.v:243-276`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L243-L276), [`rtl/minimig/ciaa.v:111`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L111) | descendant — the data register is live, `regportb[7:0] <= (data_in[7:0]);`, and drives `assign portb_out[7:0] = ((~ddrportb[7:0]) \| (regportb[7:0]));`, an output 1 modelled as the pull-up; but `wire  [7:0] portb_out;` is not a port of the module, and the read is still the line, `pb_out[7:0] = (portb_in[7:0]);`. NanoMig's values in both states |

**Where p. 341 decides, the four agree, and not with NanoMig.** Each stores what
is written to $BFE101 and returns it from an output bit, so an output driven 0
on a released line reads 0; NanoMig and both Minimig lines return the line, 1.
That is NM-HRM-054's verdict, now read at all eight positions.

**Where the package measures, they split**, as they do for port A on
NM-HRM-040's page. Denise and Copperline let an output win over the four-player
adapter; WinUAE lets the adapter win, as NanoMig's read does, unless its
391078-01 type is chosen; vAmiga has nothing that can hold the line low. Two of
the three that can answer would pass `PORTB-DIRECTION` as built and one would
fail it, on a question the manual does not settle.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 217 — agrees with the manual on the read, and disagrees with it on port B's pull-ups:

  > "PR reflects the information present on the actual port pins (PAO-PA7, PBO-PB7) for both input and output bits."
  <!-- corpus: a500-a2000-trm-1987/p217.txt -->

  > "Port B has only passive pullup"
  <!-- corpus: a500-a2000-trm-1987/p217.txt -->

  The sentence ends *"devices and has a much higher current-sinking capability"*, after port A is given both passive and active pull-ups. On a port B whose output 1 is a passive pull-up, a device holding the line low sets the pin, and the read is 0: NanoMig's value in the package's state, and not the assertion's. p. 341 gives both ports active pull-ups, and the 6526 specification agrees with it; neither document decides which describes the Amiga's 8520.

- **Commodore, 6526 CIA specification, 1981**, PDF page 5 — says what p. 341 says, for the 6526:

  > "Port A and Port B have passive pull-up"
  <!-- corpus: cia-6526-spec/p005.txt -->

  > "devices as well as active pull-ups, providing both"
  <!-- corpus: cia-6526-spec/p005.txt -->

  The OCR sets register-map text between the two lines. Like p. 341, it does not say what a read returns when a device holds a line low against an output driving 1.

---

## Two neighbouring items this entry does not carry

- **CIA-A has no PC output and its FLAG input is tied to 0**,
  [`ciaa.v:321`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L321),
  `.flag(1'b0),`, where p. 337 and p. 351 give PC to the parallel port's
  *drdy\** and F to its *ack\**. That is the port's handshake, not the
  direction of its data lines: other statements of the module and another
  question, on [NM-HRM-077](NM-HRM-077.md)'s page.
- **CIA-B's PRA bits 2 to 0**,
  [`minimig.v:648`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L648),
  `.porta_in({cd,cts,dsr,ri&_joy3[4],1'b1,_joy4[4]}),`: the RS-232 ring
  indicator ANDed with joystick 3's fire button on PA2, a constant 1 on PA1,
  joystick 4's fire button on PA0, where p. 337 and p. 351 print SEL, POUT and
  BUSY (p. 324's SEL note shares that line with the ring indicator on the
  A500/A2000). They are inputs of the other chip's port A, and the question they
  raise is which signal a bit carries, not in which direction; they do not
  belong on this page, and none is filed here.

---

## Why this entry is kept

`PORTB-DIRECTION` stays red until its fixture changes, and a reader of the red
list takes it for p. 324 disagreeing with NanoMig at eight positions — a finding
beside NM-HRM-054, or an eight-bit strengthening of it. As measured it is
neither: its one failing half is the case no page decides.

The test is the thing to correct, and the change is to its state, not its
expected values. Released lines before the sweep — `portb_in` = $FF, p. 354's
idle state, as `hrm/ch8-ciaa-port-direction` releases port A's
(`dut->porta_in = 0x3F;`, `test_ch8-ciaa-port-direction.cpp:157`) — make the
written 0 the deciding half. The assertion is then red at all eight positions
for the reason NM-HRM-054 records, and that red belongs on NM-HRM-054's page as
p. 324's corroboration, not under a key of its own.
