# NM-HRM-054 — the parallel port has a direction register and no data register [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L267-L279) |
| **Report to** | **the Minimig line, not NanoMig alone.** NanoMig's copy is the upstream behaviour with the register's corpse left in place |
| **Test** | [`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction) · `PROP-1`, claims `opus:8-par.c1`, `sol:8-par.c1` (`test_ch8-parallel-port-direction.cpp:397-412`), and [`hrm/appDE-parallel-port-ciaa`](../../hrm/appDE-parallel-port-ciaa) · `PORTB-DIRECTION`, claims `sol:E-parallel.c69` to `sol:E-parallel.c76` and `opus:E-parallel-iface.c10` (`test_appDE-parallel-port-ciaa.cpp:433-454`) — **red** in both, `3 passed, 1 failed` each, with CIA-A's port B lines released before the sweep (`test_ch8-parallel-port-direction.cpp:340`, `test_appDE-parallel-port-ciaa.cpp:362`); each package's `PORTB-RELEASED-CONTROL`, and the ch8 package's `CONTROL-PA0-OUTPUT` on the same comparator, are green in the same runs; [`integration/appF-cia-system`](../../integration/appF-cia-system) reproduces it at the system's ports on fx68k and on TG68K, recorded, not graded: its precondition `P1-PRE1.4/O1` fails on CIA-A's two instances, whose PRB reads $FF whatever DDRB and the data register hold, and holds on CIA-B's |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, each sweeping all eight bit positions of CIAAPRB/CIAADDRB and red at all eight: `PROP-1`, 1 of 4 in `hrm/ch8-parallel-port-direction`, and `PORTB-DIRECTION`, 1 of 4 in `hrm/appDE-parallel-port-ciaa`. At each position the written 0 reads back as the released line's 1. The controls on the same registers and lines pass: `PORTB-RELEASED-CONTROL`, CIAADDRB at input, reads CIAAPRB as $FF in both packages (`test_ch8-parallel-port-direction.cpp:371-381`, `test_appDE-parallel-port-ciaa.cpp:393-405`), and `CONTROL-PA0-OUTPUT` runs the identical comparator at CIAAPRA bit 0 (`test_ch8-parallel-port-direction.cpp:414-425`). `integration/appF-cia-system` records it rather than grading it: for CIA-A, `P1-PRE1.4/O1` is printed as a RECORDED line, not a FAIL, and CIA-A's instances are not graded on P1's PBON legs or on P1-OTHERBITS' pattern-bit clause |
| **Verdict of the four** | **unanimous against.** All four store CIA-A's port B data register and read it back through the direction mask |

---

## What the HRM says

**p.259**: *"The general-purpose bi-directional parallel interface is a 25-pin
connector on the back panel of the computer"* — and bi-directional is the word
the claim rests on. A port is bi-directional when a bit configured as an output
carries the value written to it.

**Appendix F, p.341, states it outright, so this finding does not rest on one
word.** The appendix specifies the part:

> Ports A and B each consist of an 8-bit peripheral data register (PR) and an
> 8-bit data direction register (DDR). If a bit in the DDR is set to a 1, the
> corresponding bit position in the PR becomes an output. If a DDR bit is set to
> a 0, the corresponding PR bit is defined as an input.

So the manual says each port **consists of** a peripheral data register and a
direction register. NanoMig's CIA-A port B has the DDR and no PR — `regportb` is
commented out in full. The finding's title is the manual's wording rather than
an inference from p.259.

The same page's read rule looks at first like it licenses the implementation and
does not:

> When you READ a PR register, you read the actual current state of the I/O pins
> (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or
> outputs.

A read does return the pin. But with a PR present and its DDR bit set, the PR
**drives** that pin, so the pin carries what was written and the read returns it.
Removing the PR does not satisfy the read rule; it removes the thing that would
have made the pin carry anything.

## What NanoMig does

The direction register is real and governs nothing.

```verilog
//reg [7:0] regportb;          // ciaa.v:267 -- commented out
reg [7:0] ddrportb;            // ciaa.v:268 -- real

/*
// writing of output port
always @(posedge clk)
  if (clk7_en) begin
    if (reset)      regportb[7:0] <= 8'd0;
    else if (wr && prb) regportb[7:0] <= (data_in[7:0]);
  end
*/                             // ciaa.v:270-279 -- the whole write block
```

So a write to CIAAPRB is stored nowhere, and the PRB read mux at `:293-294`
returns `portb_in[7:0]` unconditionally without ever consulting `ddrportb`.
Measured directly: with `portb_in` held at `0x3C` and CIAAPRB fixed, toggling
CIAADDRB between all-input and all-output gives an identical read both times.

`ddrportb` itself is correctly written and read back at its own address
(`:268`, `:282-288`, `:296`). It is a direction register with nothing to direct.

### Measured

**Two unit packages are red at all eight bit positions of CIA-A's port B.** Both
build `ciaa.v` alone and use the same comparator
(`test_ch8-parallel-port-direction.cpp:315-324`,
`test_appDE-parallel-port-ciaa.cpp:350-358`): for each position n, CIAADDRB is
set to `1<<n`, 0 is written to CIAAPRB and read back, then `1<<n` is written and
read back; the position passes when the reads give 0 and then 1 at bit n.

Before the sweep each package releases the port B lines, `portb_in = 0xFF`
(`test_ch8-parallel-port-direction.cpp:340`,
`test_appDE-parallel-port-ciaa.cpp:362`), the level p.354 gives a line nothing
drives: after RES the port pins are inputs and "a read of the ports will return
all highs because of passive pull-ups". An output driven 0 on a released line
is the case p.341 decides: the read returns the pin, and a PR bit set to output
drives it to 0.

- [`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction)
  grades p.259's "bi-directional" as `PROP-1` (`opus:8-par.c1`,
  `sol:8-par.c1`), one of the package's `3 passed, 1 failed`:

      MEASURED: PROP-1: bit 0: FAIL bit 1: FAIL bit 2: FAIL bit 3: FAIL bit 4: FAIL bit 5: FAIL bit 6: FAIL bit 7: FAIL

- [`hrm/appDE-parallel-port-ciaa`](../../hrm/appDE-parallel-port-ciaa) grades
  p.324's PARALLEL PORT (J8) table, which gives each of D0–D7 the direction I/O
  and brackets them as "D0-D7 comprise an eight-bit bidirectional bus", as
  `PORTB-DIRECTION` (`sol:E-parallel.c69` to `sol:E-parallel.c76`,
  `opus:E-parallel-iface.c10`), one of the package's `3 passed, 1 failed`:

      MEASURED: PORTB-DIRECTION: D0: writable=no D1: writable=no D2: writable=no D3: writable=no D4: writable=no D5: writable=no D6: writable=no D7: writable=no

  The same nine ids carry `PORTB-WIDTH`, which passes: a single `portb_in`
  line moves only its own PRB bit, at each of the eight.

The packages print one verdict per position; the half that fails is the written
0. With no data register, the read at `ciaa.v:293-294` returns the released
line, 1, after the written 0 and after the written 1 alike.

Each package runs `PORTB-RELEASED-CONTROL` on the same registers and lines:
CIAADDRB left at input, the lines released, a read of CIAAPRB must return $FF
(`test_ch8-parallel-port-direction.cpp:371-381`,
`test_appDE-parallel-port-ciaa.cpp:393-405`). It passes in both:

    MEASURED: PORTB-RELEASED-CONTROL: portb_in=0xFF, PRB=0xFF
    MEASURED: PORTB-RELEASED-CONTROL: DDRB=0x00 (input), portb_in=0xFF, PRB=0xFF

In the ch8 package `CONTROL-PA0-OUTPUT` points the identical comparator at
CIAAPRA/CIAADDRA bit 0 and passes in the same run
(`test_ch8-parallel-port-direction.cpp:414-425`). Beside the graded lines that
package records the direction-independence above
(`test_ch8-parallel-port-direction.cpp:442-465`): with `portb_in` held at
`0x3C`, CIAAPRB reads `0x3C` under `CIAADDRB=0x00` and `CIAADDRB=0xFF` alike.

**At the system's ports, on both CPU cores, CIA-A's PRB reads $FF whatever is
written.** [`integration/appF-cia-system`](../../integration/appF-cia-system)
runs 68000 programs on the whole of `nanomig.v`, on fx68k and on TG68K, and
reads PRB through the CPU. Before it grades a CIA's port B, its precondition
`P1-PRE1.4/O1` asks whether a PRB read shows a driven level at all: with DDRB
bit n set and PBON off, PRB is read with the data bit written 0 (d1) and then
1 (o1c), bit n being 6 in the timer-A program and 7 in the timer-B one. On
CIA-A both reads are $FF, as is every other CIA-A PRB read the P1 and P2
programs print, where p. 341 — an output bit carries its data, an input bit its pull-up — gives
$95 and $D5 in the timer-A program (DDRB = $7F, data $15 and then $55) and $55
and $D5 in the timer-B one (DDRB = $BF, data $15 and then $95). The package
prints, the same on both cores:

    RECORDED: P1-PRE1.4/O1 [p341], CIA-A: a PRB read never shows a driven level -- measured $FF (d1) and $FF (O1 contrast) on CIAA/timer A, $FF and $FF on CIAA/timer B, whatever DDRB and the data register hold -- port B returns the lines, not the data register (NM-HRM-054) -- so CIA-A's instances are not graded on P1's PBON legs or on P1-OTHERBITS' pattern-bit clause
    MEASURED: P1 CIAA/TA: r0=$FF a2=$FF a1=$FF b1=$FF c1=$FF d1=$FF o1c=$FF d2=$FF
    MEASURED: P1 CIAA/TB: r0=$FF a2=$FF a1=$FF b1=$FF c1=$FF d1=$FF o1c=$FF d2=$FF

The same precondition, through the same programs, is a PASS on CIA-B's two
instances, where d1 and o1c read $BB and $FB (timer A) and $7B and $FB
(timer B): the data bit, 0 and then 1.

$FF is what CIA-A's port B inputs carry at this boundary. `nanomig.v` ties
joysticks 3 and 4 to zero,
[`wire [15:0] JOY2 = 16'h0000;`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1373-L1374)
and the same for `JOY3`, and passes both to the chipset inverted,
[`._joy3 (~JOY2 ),` and `._joy4 (~JOY3 ),`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1505-L1506),
so the eight joystick-adapter lines `minimig.v:624` routes into CIA-A's port B
are all 1, and the PRB read at `ciaa.v:293-294` returns them.

**This is the behaviour this page reports, reproduced at the system level; it
is not a graded red of its own.** For CIA-A, `P1-PRE1.4/O1` is a RECORDED line,
not a FAIL, and CIA-A's instances are not graded on P1's PBON legs or on
P1-OTHERBITS' pattern-bit clause.

## What the port is actually used for here

`minimig.v:624` wires `.portb_in({_joy4[0],_joy4[1],_joy4[2],_joy4[3],_joy3[0],_joy3[1],_joy3[2],_joy3[3]}),` — **CIA-A port B is
this tree's four-player joystick adapter.** That is a real Amiga use of the
parallel connector, and it explains the design decision; it does not make the
port bi-directional.

There is no handshake either: `grep -rniwE 'pc|drdy|strobe|ack'` over `ciaa.v`
and `ciab.v` returns nothing, and `ciaa.v:321` ties the interrupt controller's
`.flag` to `1'b0`, so the acknowledge interrupt cannot fire. The only FLAG
driven anywhere in the tree is CIA-B's, carrying the floppy index pulse
(`minimig.v:647`).

## Why the red is not a broken fixture

`PORTB-RELEASED-CONTROL` reads the released lines as $FF through the same
CIAAPRB read, so the fixture does release them and the read path works. Held at
0 instead (`MUT_CONTROL_HELD_LOW`, `test_ch8-parallel-port-direction.cpp:373`,
`test_appDE-parallel-port-ciaa.cpp:395`), the lines read $00 and the control
goes red, `2 passed, 2 failed` in each package, so it is not true by
construction.

The **identical comparator** passes against CIA-A port **A**: in the ch8
package's own run as `CONTROL-PA0-OUTPUT`, and in both packages under the
mutations `MUT_PROP1_TARGET_A` and `MUT_DIRECTION_TARGET_A`
(`test_ch8-parallel-port-direction.cpp:344-349`,
`test_appDE-parallel-port-ciaa.cpp:366-370`), which retarget the whole
eight-position sweep at CIAAPRA/CIAADDRA, bits 0 and 1 in turn, and pass at all
eight positions, `4 passed, 0 failed` in each package. Port A's direction
mechanism works at PA0 and PA1 on the same fixture and the same bus protocol,
the two positions where the sibling package
[`hrm/ch8-ciaa-port-direction`](../../hrm/ch8-ciaa-port-direction) finds it
working as well ([NM-HRM-040](NM-HRM-040.md) is about the other six). A units
error or a broken rig is ruled out by the packages' own controls.

## Two things that point the wrong way

**`ciaa`'s own `portb_out` wire is dead.** `ciaa.v:114` declares
`wire [7:0] portb_out;` and that is its only occurrence in the file — never
assigned, never read — and `ciaa`'s port list has no `portb_out` output at all.
The `.portb_out(...)` at `minimig.v:650` belongs to the separate `ciab` instance
and carries floppy control signals.

**A stale comment claims the opposite.** `minimig.v:136` still reads
*"2011-04-02 - added functional ciaa port b (parallel) register to let Unreal
game work and some trainer store data"*. That register is the one commented out
at `ciaa.v:267`. Anyone grepping the tree's comments for the parallel port will
be told it works.

## What the others say

| | |
|---|---|
| **WinUAE** | `c->prb = val;` on the CIA-A write (`cia.cpp:2143`), and the value goes on to a printer (`doprinter(val)`) and to the Arcadia parallel port. A real data register with real consumers |
| **vAmiga** | stores `prb` and reads it back through the direction mask: `CIAA::computePB` returns `(internal & ddrb) \| (external & ~ddrb)` with `internal` being `prb` (`Core/Components/CIA/CIA.cpp:919`, `:937-940`) — the exact behaviour p.259's "bi-directional" names |
| **Denise** | keeps `prb` beside `ddrb` in its CIA line state (`emulation/cia/new/cia.h:20-31`), serialises both (`cia.cpp:455`, `:457`), and reads CIA-A port B through the direction mask: `out = (lines->prb & lines->ddrb) \| (out & ~lines->ddrb);` (`emulation/libami/system/system.cpp:60`) |
| **Copperline** | keeps both in its register file and documents the same board wiring NanoMig has — *"the parallel-port joystick adapter's switches on CIA-A PB0-7"* (`src/chipset/cia.rs:460-465`) — while still storing the data register |
| **MiSTer** | descendant — the same gap without the commented-out register: `regportb` does not exist here at all, and a PRB read returns `portb_in` (`rtl/ciaa.v:328-329`). `rtl/ciaa.v:312` names the section *"Port B - Parallel port (simplified, mostly unused in Amiga)"*, and `:141` calls the unused wire out directly, *"Port B output (unused)"*. |
| **TC64** | descendant — live, not commented out: `regportb` is written and drives `portb_out` (`rtl/minimig/ciaa.v:243,247-253,276`), but `portb_out` is a wire inside `ciaa` (`:111`), not one of its ports, so CIA-A's instantiation has nothing to connect (`rtl/minimig/minimig.v:906-927`), and the read mux still returns `portb_in` unconditionally on a PRB read (`ciaa.v:268`). The register exists and reaches nothing. |

Copperline is the interesting one: it models **the same joystick-adapter use of
the connector** that NanoMig wires, and still implements the register, because
the adapter overlays the input pins rather than replacing the port.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, 6526 CIA specification, 1981**, PDF page 5 — agrees with the manual:

  > "Ports A and B each consist of an 8-bit Peripheral Data Register (PR) and an 8-bit Data Direction Register"
  <!-- corpus: cia-6526-spec/p005.txt -->

  Port B, like port A, has an 8-bit Peripheral Data Register beside its DDR, the register NanoMig's CIA-A port B lacks; the p006 register table and the p002 block diagram list PRB as a register of its own, and p006 adds a PC handshake output that pulses low after a read or write of PORT B. The same page carries the manual's read rule (a read returns the pins for input and output bits) alongside the PR, not in place of it; this is the 6526, while the finding concerns CIA-A port B on the Amiga's 8520.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 217 — agrees with the manual:

  > "Ports A and B each consist of an 8-bit Peripheral Data Register (PR) and an 8-bit Data Direction Register (DDR)."
  <!-- corpus: a500-a2000-trm-1987/p217.txt -->

  The 8520 chapter gives each port, B included, a data register as well as a direction register, and a set DDR bit makes the matching PR bit an output. NanoMig's CIA-A port B has the DDR and no PR. The source also calls the printer port an '8 bit bidirectional parallel port' (p106) and says its handshake goes through the 8520 it is connected to, but it never names CIA-A port B as that port.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheets 4b, 4c and 4a — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 4b, drawn transistor by transistor for PB0, keeps a PRB bit and a DDRB bit in two latches, written from D0io by W_PRB and W_DDRB. The pad has a passive pull-up and a pull-down that is on while the DDRB bit is set and the PRB bit clear, and no active pull-up; sheet 4c draws PB7 the same way, with CRB1 ORed into the enable. A read of PRB puts the pad's level, sampled on PHI1, on the bus; no path leads from the PRB latch to the bus, and a read of DDRB returns the DDRB latch. Sheet 4a's PA0 has an active pull-up as well.
  <!-- drawing: cia-8520r4-dissection/sheet_005.txt -->

  Port B consists of a data register and a direction register, as p. 341 prints: a set DDRB bit makes the PRB bit drive its pin, and a read of PRB returns the pin, output bits included. With nothing outside on the line, an output bit reads back what was written, a 0 driven low and a 1 held high by the passive pull-up alone, so a line pulled low from outside reads 0 whatever was written; p. 341 gives both ports active pull-ups, and as drawn only port A has one. Only PB0 and PB7 are drawn, and the chip says nothing of how a board wires CIA-A's port B. The dissection is of one revision.

Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

