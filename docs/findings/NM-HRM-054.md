# NM-HRM-054 — the parallel port has a direction register and no data register

| | |
|---|---|
| **Status** | open · red |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/ciaa.v#L267-L279) |
| **Upstream** | **inherited, and acknowledged there in comments.** `MiSTer-devel/Minimig-AGA_MiSTer`'s `rtl/ciaa.v` has no `regportb` at all — not even commented out — and names it: `:312` *"Port B - Parallel port (simplified, mostly unused in Amiga)"*, `:141` *"Port B output (unused)"* |
| **Report to** | **the Minimig line, not NanoMig alone.** NanoMig's copy is the upstream behaviour with the register's corpse left in place |
| **Test** | [`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction) — `PROP-1` red, `CONTROL-PA0-OUTPUT` green on the same comparator |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
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

## What the four do

| | |
|---|---|
| **WinUAE** | `c->prb = val;` on the CIA-A write (`cia.cpp:2143`), and the value goes on to a printer (`doprinter(val)`) and to the Arcadia parallel port. A real data register with real consumers |
| **vAmiga** | stores `prb` and reads it back through the direction mask: `result = (result & ~ddrb) \| (prb & ddrb)` (`Core/Components/CIA/CIA.cpp:931`, `:1114`) — the exact behaviour p.259's "bi-directional" names |
| **Denise** | keeps `prb` beside `ddrb` in its CIA line state (`emulation/cia/new/cia.h:20-31`) and serialises both (`cia.cpp:455`, `:463`) |
| **Copperline** | keeps both in its register file and documents the same board wiring NanoMig has — *"the parallel-port joystick adapter's switches on CIA-A PB0-7"* (`src/chipset/cia.rs:460-465`) — while still storing the data register |

Copperline is the interesting one: it models **the same joystick-adapter use of
the connector** that NanoMig wires, and still implements the register, because
the adapter overlays the input pins rather than replacing the port.
