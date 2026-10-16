# NM-HRM-054 — the parallel port has a direction register and no data register [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should a CIAAPRB read return the bit written, for each bit whose CIAADDRB bit is 1, as p.341 prints ("the corresponding bit position in the PR becomes an output"), where `ciaa.v` returns `portb_in` and a written 0 on a released line reads 1? It closes when `PROP-1` and `PORTB-DIRECTION` turn green, or when an Amiga specialist rejects the change; if the rejection means p.341 is wrong for CIA-A's port B, the page is refiled against the printed page |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | CIA-A port B, CIAAPRB with CIAADDRB, all eight bits, with the port B lines released (`portb_in` = $FF) and nothing driving them; port A's PA0 passes the same comparator (`CONTROL-PA0-OUTPUT`). Chip revision: unknown |
| **Software impact** | Unreal and "some trainer", named by the comment at `minimig.v:136` ("added functional ciaa port b (parallel) register to let Unreal game work and some trainer store data") as needing the register that `ciaa.v:267` now leaves commented out; no test here runs either program, so the result is unknown |
| **Chapter** | 8 — Interface Hardware, printed p.259; Appendix E, printed p.324; Appendix F, printed pp.341 and 354 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · `PROP-1`: `opus:8-par.c1`, `sol:8-par.c1`, p.259; `PORTB-DIRECTION`: `sol:E-parallel.c69`, `sol:E-parallel.c70`, `sol:E-parallel.c71`, `sol:E-parallel.c72`, `sol:E-parallel.c73`, `sol:E-parallel.c74`, `sol:E-parallel.c75`, `sol:E-parallel.c76`, `opus:E-parallel-iface.c10`, p.324 (the same nine on the green `PORTB-WIDTH`); `PORTB-RELEASED-CONTROL` (pp.341, 354) and `P1-PRE1.4/O1` (p.341) carry no claim id; `CONTROL-PA0-OUTPUT` and each package's `FIXTURE-LIVE` carry neither a claim id nor a page |
| **Module** | `ciaa.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L267-L279); MiSTer has no `regportb` and TC64's is live but reaches nothing, the read mux the same in all three; inherited |
| **Test** | [`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction): `make -C hrm/ch8-parallel-port-direction test`; [`hrm/appDE-parallel-port-ciaa`](../../hrm/appDE-parallel-port-ciaa): `make -C hrm/appDE-parallel-port-ciaa test`; [`integration/appF-cia-system`](../../integration/appF-cia-system): `make -k -C integration/appF-cia-system test`. An expected red exits nonzero |
| **Assertions** | 2 red, each red at all eight bit positions, expected: a written 0 reads back 0 at bit n, a written 1 reads back 1; observed: 1 after the 0 and 1 after the 1. `PROP-1`, 1 of 4 in `hrm/ch8-parallel-port-direction` (`3 passed, 1 failed`), and `PORTB-DIRECTION`, 1 of 4 in `hrm/appDE-parallel-port-ciaa` (`3 passed, 1 failed`). Green in the same runs: `PORTB-RELEASED-CONTROL` in both and `CONTROL-PA0-OUTPUT` in ch8. `integration/appF-cia-system` records it, not graded: for CIA-A, `P1-PRE1.4/O1` prints a RECORDED line, not a FAIL, and CIA-A's instances are not graded on P1's PBON legs or on P1-OTHERBITS' pattern-bit clause |
| **Verdict of the four** | **unanimous against.** All four store CIA-A's port B data register and read it back through the direction mask. Hardware: not measured; the 6502.org dissection of one 8520 revision draws a PRB latch beside the DDRB latch |
| **Effort** | none; if an Amiga specialist accepts p.341's data register for CIA-A's port B, S: `ciaa.v` only — restore the commented-out `regportb` block (`:270-279`) and let the PRB read mux (`:293-294`) return it for output bits |

---

## Source requirement

**p.259**:

> The general-purpose bi-directional parallel interface is a 25-pin connector on the back panel of the
> computer.

A port is bi-directional when a bit configured as an output carries the value
written to it. **Appendix F, p.341**, specifies the part:

> Ports A and B each consist of an 8-bit peripheral data register (PR) and an
> 8-bit data direction register (DDR). If a bit in the DDR is set to a 1, the
> corresponding bit position in the PR becomes an output. If a DDR bit is set to
> a 0, the corresponding PR bit is defined as an input.

The same page's read rule does not license the implementation:

> When you READ a PR register, you read the actual current state of the I/O
> pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or
> outputs.

A read returns the pin, but with a PR bit set to output the PR drives that pin, so
the pin carries what was written and the read returns it. Removing the PR removes
what would make the pin carry anything. After RES the port pins are inputs and, p.354:
"a read of the ports will return all highs because of passive pull-ups", the level
of a released line.

## RTL and measurement

CIA-A's port B has the DDR and no PR: the write block is commented out in full
([`ciaa.v#L267-L279`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L267-L279)),

    //reg [7:0] regportb;          // ciaa.v:267 -- commented out
    reg [7:0] ddrportb;            // ciaa.v:268 -- real

so a write to CIAAPRB is stored nowhere, and the PRB read mux at `:293-294`
returns `portb_in[7:0]` without consulting `ddrportb`, which is correctly written
and read back at its own address (`:268`, `:282-288`, `:296`). `ciaa`'s own
`portb_out` wire (`:114`) is declared, never assigned and never read, and the
module has no such output port; the `.portb_out(...)` at `minimig.v:650` belongs
to `ciab`.

Two unit packages build `ciaa.v` alone and use the same comparator
(`test_ch8-parallel-port-direction.cpp:218-227`,
`test_appDE-parallel-port-ciaa.cpp:276-284`): with `portb_in = 0xFF` released
(`test_ch8-parallel-port-direction.cpp:243`,
`test_appDE-parallel-port-ciaa.cpp:288`), for each position n CIAADDRB is set to
`1<<n`, 0 is written to CIAAPRB and read back, then `1<<n`; the position passes
when the reads give 0 and then 1 at bit n.

The half that fails is the written 0: the read at `ciaa.v:293-294` returns the
released line's 1.

- `PROP-1` in [`hrm/ch8-parallel-port-direction`](../../hrm/ch8-parallel-port-direction) grades p.259's "bi-directional" (`test_ch8-parallel-port-direction.cpp:300-315`):

      MEASURED: PROP-1: bit 0: FAIL bit 1: FAIL bit 2: FAIL bit 3: FAIL bit 4: FAIL bit 5: FAIL bit 6: FAIL bit 7: FAIL

- `PORTB-DIRECTION` in [`hrm/appDE-parallel-port-ciaa`](../../hrm/appDE-parallel-port-ciaa) grades p.324's PARALLEL PORT (J8) table, which gives each of D0–D7 the direction I/O and brackets them as "D0-D7 comprise an eight-bit bidirectional bus" (`test_appDE-parallel-port-ciaa.cpp:359-376`):

      MEASURED: PORTB-DIRECTION: D0: writable=no D1: writable=no D2: writable=no D3: writable=no D4: writable=no D5: writable=no D6: writable=no D7: writable=no

  The same nine ids carry `PORTB-WIDTH`, which passes: a single `portb_in`
  line moves only its own PRB bit, at each of the eight.

Green on the same registers and lines:

- `PORTB-RELEASED-CONTROL`, CIAADDRB at input and the lines released, reads CIAAPRB as $FF in ch8 (`test_ch8-parallel-port-direction.cpp:274-284`).
- `PORTB-RELEASED-CONTROL`, the same read, in appDE (`test_appDE-parallel-port-ciaa.cpp:319-331`).
- `CONTROL-PA0-OUTPUT` in ch8, the identical comparator at CIAAPRA bit 0 (`test_ch8-parallel-port-direction.cpp:317-328`).

The `PORTB-RELEASED-CONTROL` lines:

    MEASURED: PORTB-RELEASED-CONTROL: portb_in=0xFF, PRB=0xFF
    MEASURED: PORTB-RELEASED-CONTROL: DDRB=0x00 (input), portb_in=0xFF, PRB=0xFF

The ch8 package also records the direction-independence
(`test_ch8-parallel-port-direction.cpp:345-373`): with `portb_in` held at `0x3C`,
CIAAPRB reads `0x3C` under `CIAADDRB=0x00` and `CIAADDRB=0xFF` alike.

**At the system's ports, on fx68k and on TG68K, CIA-A's PRB reads $FF whatever
is written.** [`integration/appF-cia-system`](../../integration/appF-cia-system)
reads PRB through the CPU. Its precondition `P1-PRE1.4/O1` asks whether a PRB
read shows a driven level at all: with DDRB bit n set and PBON off, PRB is read
with the data bit written 0 (d1) and then 1 (o1c), bit n being 6 in the timer-A
program and 7 in the timer-B one. On CIA-A both reads are $FF, as is every other
CIA-A PRB read the P1 and P2 programs print, where p.341 gives $95 and $D5 in the
timer-A program (DDRB = $7F, data $15 and then $55) and $55 and $D5 in the
timer-B one (DDRB = $BF, data $15 and then $95). On both cores:

    RECORDED: P1-PRE1.4/O1 [p341], CIA-A: not graded as a live read-back -- measured $FF (d1) and $FF (O1 contrast) on CIAA/timer A, $FF and $FF on CIAA/timer B; CIA-A's instances are not graded on P1's PBON legs or on P1-OTHERBITS' pattern-bit clause
    MEASURED: P1 CIAA/TA: r0=$FF a2=$FF a1=$FF b1=$FF c1=$FF d1=$FF o1c=$FF d2=$FF
    MEASURED: P1 CIAA/TB: r0=$FF a2=$FF a1=$FF b1=$FF c1=$FF d1=$FF o1c=$FF d2=$FF

The same line is one PASS over CIA-B's two instances, where d1 and o1c read $BB
and $FB (timer A) and $7B and $FB (timer B): the data bit, 0 and then 1. $FF is
what CIA-A's port B inputs carry here: `nanomig.v` ties joysticks 3 and 4 to
zero, [`wire [15:0] JOY2 = 16'h0000;`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1373-L1374)
and the same for `JOY3`, and passes both to the chipset inverted,
[`._joy3 (~JOY2 ),` and `._joy4 (~JOY3 ),`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1505-L1506),
so the eight joystick-adapter lines `minimig.v:624` routes into CIA-A's port B
are all 1 and the PRB read at `ciaa.v:293-294` returns them. This reproduces the
behaviour at the system level.

## Risks and unknowns

- **Recorded, not graded, at the system's ports:** for CIA-A, `P1-PRE1.4/O1` is a
  RECORDED line, not a FAIL, and CIA-A's instances are not graded on P1's PBON
  legs or on P1-OTHERBITS' pattern-bit clause.
- **Only released lines were graded.** Lines held low from outside are only recorded (`portb_in` = `0x3C`, above), not graded; the chip revision is unknown.
- **CIA-A port B is this tree's four-player joystick adapter** (`minimig.v:624` wires `.portb_in({_joy4[0],_joy4[1],_joy4[2],_joy4[3],_joy3[0],_joy3[1],_joy3[2],_joy3[3]})`),
  a real Amiga use of the connector that explains the design decision and does
  not make the port bi-directional.
- **No handshake was graded.** `grep -rniwE 'pc|drdy|strobe|ack'` over `ciaa.v`
  and `ciab.v` returns nothing, and `ciaa.v:321` ties the interrupt controller's
  `.flag` to `1'b0`, so the acknowledge interrupt cannot fire; the only FLAG
  driven in the tree is CIA-B's, carrying the floppy index pulse (`minimig.v:647`).
- **Second documents:** the 6526 specification is the 6526, while the finding
  concerns CIA-A port B on the Amiga's 8520. The TRM calls the printer port an
  "8 bit bidirectional parallel port" (p106) but never names CIA-A port B as that
  port. The dissection is of one revision of the 8520 (R4), draws only PB0 and PB7,
  says nothing of how a board wires CIA-A's port B, and draws a passive pull-up
  and no active one on port B where p.341 gives both ports active pull-ups.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | `cia.cpp:2143` | `c->prb = val;` on the CIA-A write, and the value goes on to a printer (`doprinter(val)`) and to the Arcadia parallel port: a real data register with real consumers |
| **vAmiga** | `Core/Components/CIA/CIA.cpp:919`, `:937-940` | stores `prb` and reads it back through the direction mask: `CIAA::computePB` returns `(internal & ddrb) \| (external & ~ddrb)`, `internal` being `prb` |
| **Denise** | `emulation/cia/new/cia.h:20-31`, `cia.cpp:455`, `:457`, `emulation/libami/system/system.cpp:60` | keeps `prb` beside `ddrb`, serialises both, and reads CIA-A port B through the direction mask: `out = (lines->prb & lines->ddrb) \| (out & ~lines->ddrb);` |
| **Copperline** | `src/chipset/cia.rs:460-465` | keeps both in its register file and documents the same board wiring, *"the parallel-port joystick adapter's switches on CIA-A PB0-7"*, while still storing the data register |
| **MiSTer** | `rtl/ciaa.v:328-329`, `:312`, `:141` | descendant — the same gap without the commented-out register: `regportb` does not exist, and a PRB read returns `portb_in`; `:312` names the section *"Port B - Parallel port (simplified, mostly unused in Amiga)"*, `:141` calls the unused wire *"Port B output (unused)"* |
| **TC64** | `rtl/minimig/ciaa.v:243,247-253,276`, `:111`, `:268`, `rtl/minimig/minimig.v:906-927` | descendant — live, not commented out: `regportb` is written and drives `portb_out`, but `portb_out` is a wire inside `ciaa`, not a port, so CIA-A's instantiation has nothing to connect, and the read mux still returns `portb_in` on a PRB read. The register exists and reaches nothing |

Copperline models the same joystick-adapter use of the connector that NanoMig
wires and still implements the register, because the adapter overlays the input
pins rather than replacing the port.
### What other Commodore documents say

- **Commodore, 6526 CIA specification, 1981**, PDF page 5 — agrees with the manual:

  > "Ports A and B each consist of an 8-bit Peripheral Data Register (PR) and an 8-bit Data Direction Register"
  <!-- corpus: cia-6526-spec/p005.txt -->

  Port B, like port A, has an 8-bit Peripheral Data Register beside its DDR, the register NanoMig's CIA-A port B lacks; the p006 register table and the p002 block diagram list PRB as a register of its own, and p006 adds a PC handshake output that pulses low after a read or write of PORT B. The same page carries the manual's read rule alongside the PR, not in place of it.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 217 — agrees with the manual:

  > "Ports A and B each consist of an 8-bit Peripheral Data Register (PR) and an 8-bit Data Direction Register (DDR)."
  <!-- corpus: a500-a2000-trm-1987/p217.txt -->

  The 8520 chapter gives each port, B included, a data register as well as a direction register, and a set DDR bit makes the matching PR bit an output.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheets 4b, 4c and 4a — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 4b, drawn transistor by transistor for PB0, keeps a PRB bit and a DDRB bit in two latches, written from D0io by W_PRB and W_DDRB. The pad has a passive pull-up and a pull-down that is on while the DDRB bit is set and the PRB bit clear, and no active pull-up; sheet 4c draws PB7 the same way, with CRB1 ORed into the enable. A read of PRB puts the pad's level, sampled on PHI1, on the bus; no path leads from the PRB latch to the bus, and a read of DDRB returns the DDRB latch. Sheet 4a's PA0 has an active pull-up as well.
  <!-- drawing: cia-8520r4-dissection/sheet_005.txt -->

  Port B consists of a data register and a direction register, as p. 341 prints: a set DDRB bit makes the PRB bit drive its pin, and a read of PRB returns the pin, output bits included. With nothing outside on the line, an output bit reads back what was written, a 0 driven low and a 1 held high by the passive pull-up alone, so a line pulled low from outside reads 0 whatever was written.

## Decision

**Open question for an Amiga specialist: should CIA-A's port B carry the data register p.341 describes?** No change is proposed now. What is missing is the write block `ciaa.v:270-279` leaves commented out (`regportb`, cleared on reset and loaded by a write to PRB) and a PRB read that returns the pin with output bits driven by it, as vAmiga and Denise read it through the direction mask.

The finding closes when `PROP-1` and `PORTB-DIRECTION` turn green, or when an Amiga specialist rejects the change; if the rejection means p.341 is wrong for CIA-A's port B, the page is refiled against the printed page.
