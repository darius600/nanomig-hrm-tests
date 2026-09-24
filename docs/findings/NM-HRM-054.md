# NM-HRM-054 — the parallel port has a direction register and no data register [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L267-L279) |
| **Report to** | **the Minimig line, not NanoMig alone.** NanoMig's copy is the upstream behaviour with the register's corpse left in place |
| **Test** | [`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction) — `PROP-1` red, `CONTROL-PA0-OUTPUT` green on the same comparator |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 3 |
| **Verdict of the four** | **unanimous against.** All four store CIA-A's port B data register and read it back through the direction mask |

---

## What the HRM says

**p.259**: *"The general-purpose bi-directional parallel interface is a 25-pin
connector on the back panel of the computer"* — and bi-directional is the word
the claim rests on. A port is bi-directional when a bit configured as an output
carries the value written to it.

**Appendix F, p.341, states it outright, and this finding no longer rests on one
word.** The appendix was extracted on 17.09.2026, after this finding was written,
and it specifies the part:

> Ports A and B each consist of an 8-bit peripheral data register (PR) and an
> 8-bit data direction register (DDR). If a bit in the DDR is set to a 1, the
> corresponding bit position in the PR becomes an output. If a DDR bit is set to
> a 0, the corresponding PR bit is defined as an input.

So the manual says each port **consists of** a peripheral data register and a
direction register. NanoMig's CIA-A port B has the DDR and no PR — `regportb` is
commented out in full. The finding's own title is now the manual's wording rather
than an inference from p.259.

The same page's read rule looks at first like it licenses the implementation and
does not:

> When you READ a PR register, you read the actual current state of the I/O pins
> (PA0-PA7, PB0-PB7), regardless of whether you have set them to be inputs or
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

[`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction)
holds `portb_in` at `0x3C` and reads CIAAPRB with `CIAADDRB=0x00` and
`CIAADDRB=0xFF`: `0x3C` both times — `PROP-1`, red, one of the package's `2
passed, 1 failed`. The identical comparator pointed at CIA-A port A instead —
`CONTROL-PA0-OUTPUT` — is green on the same fixture.

## What the port is actually used for here

`minimig.v:624` wires `.portb_in({_joy4[0..3],_joy3[0..3]})` — **CIA-A port B is
this tree's four-player joystick adapter.** That is a real Amiga use of the
parallel connector, and it explains the design decision; it does not make the
port bi-directional.

There is no handshake either: `grep -rniwE 'pc|drdy|strobe|ack'` over `ciaa.v`
and `ciab.v` returns nothing, and `ciaa.v:321` ties the interrupt controller's
`.flag` to `1'b0`, so the acknowledge interrupt cannot fire. The only FLAG
driven anywhere in the tree is CIA-B's, carrying the floppy index pulse
(`minimig.v:647`).

## Why the red is not a broken fixture

The package runs the **identical comparator** against CIA-A port **A** —
`CONTROL-PA0-OUTPUT`, and the mutation `MUT_PROP1_TARGET_A` which retargets only
the register-select arguments — and it passes. Port A's direction mechanism
works on the same fixture and the same bus protocol, which is also what the
sibling package [`hrm/ch8-ciaa-port-direction`](../../hrm/ch8-ciaa-port-direction)
already grades. A units error or a broken rig is ruled out by the package's own
control.

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
| **vAmiga** | stores `prb` and reads it back through the direction mask: `result = (result & ~ddrb) \| (prb & ddrb)` (`Core/Components/CIA/CIA.cpp:931`, `:1114`) — the exact behaviour p.259's "bi-directional" names |
| **Denise** | keeps `prb` beside `ddrb` in its CIA line state (`emulation/cia/new/cia.h:20-31`) and serialises both (`cia.cpp:455`, `:463`) |
| **Copperline** | keeps both in its register file and documents the same board wiring NanoMig has — *"the parallel-port joystick adapter's switches on CIA-A PB0-7"* (`src/chipset/cia.rs:460-465`) — while still storing the data register |
| **MiSTer** | descendant — no counterpart; `regportb` does not exist here at all, not even commented out. `rtl/ciaa.v:312` names the section *"Port B - Parallel port (simplified, mostly unused in Amiga)"*, and `:141` calls the unused wire out directly, *"Port B output (unused)"*. |
| **TC64** | descendant — live, not commented out: `regportb` is written and drives `portb_out` (`rtl/minimig/ciaa.v:243,247-253,276`), but CIA-A's own instantiation never connects `.portb_out` (`rtl/minimig/minimig.v:906-927`) and the read mux still returns `portb_in` unconditionally on a PRB read (`ciaa.v:268`). The register exists and reaches nothing. |

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

Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

