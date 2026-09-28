# NM-HRM-040 — CIAADDRA's direction mask corresponds one-to-one for two bit positions of eight [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change, which here first needs an answer to the CIA part-number question in *What the others say*. *Suggested change* states what follows from each answer; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L252-L262) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L298-L309) |
| **Report to** | **Minimig-AGA and NanoMig.** Both Minimig lines carry the same two-of-eight mux. **Read the third-witness section first**: two of four implementations gate this behaviour on a CIA part number |
| **Test** | [`hrm/ch8-ciaa-port-direction`](../../hrm/ch8-ciaa-port-direction) · `PROP-1`, claim `opus:8-dio.c44` (`test_ch8-ciaa-port-direction.cpp:380-402`), and `PROP-2`, claims `opus:8-dio.c46`, `sol:8-dio.c43` (`test_ch8-ciaa-port-direction.cpp:404-426`) — **red**, `3 passed, 2 failed`, with all six external lines released (`test_ch8-ciaa-port-direction.cpp:157`) |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, each sweeping all eight positions: at PA2–PA7 each fails on the written 0 alone, read back as 1 (`test_ch8-ciaa-port-direction.cpp:264-266`, `:302-304`), and passes at PA0–PA1. Two PA0-restricted controls on the identical comparators (`test_ch8-ciaa-port-direction.cpp:428-446`) pass in the same run |
| **Verdict of the four** | **a four-way split** where p.341 predicts the value, an output driven 0 on a line nothing else pulls: Denise honours all eight; Copperline reproduces NanoMig's two-of-eight bit for bit; vAmiga honours all eight until a mouse is attached, whose pull-up then overrides that port's fire bit, and restores the full eight regardless only under a chip-variant switch that is off by default; WinUAE follows the write on four (PA7/PA6 as a wired-AND with the fire button) and never stores PA2–PA5, so its chip-variant switch does not restore those four. Over a line held low, a case no page decides and the package does not present, they split again: Denise and vAmiga return the written 1 on PA2–PA5, WinUAE and Copperline the line |

---

## What the HRM says

**p.241**, the paragraph whole:

> The joystick fire buttons can also be configured as outputs. CIAADDRA ($BFE201) contains a mask that corresponds one-to-one with the data read register, CIAAPRA ($BFE001). Setting a 1 in the direction position makes the corresponding bit an output. See Appendix F for more details.

One-to-one, across the register, and p.241 names no exception. It does not say
what a read of an output bit returns; it sends the reader to Appendix F.

**p.341**, Appendix F's port section:

> If a bit in the DDR is set to a 1, the corresponding bit position in the PR becomes an output. If a DDR bit is set to a 0, the corresponding PR bit is defined as an input.
>
> When you READ a PR register, you read the actual current state of the I/O pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or outputs.

**p.354** gives the state of a pin nothing drives:

> The port pins are set as inputs and port registers to zero (although a read of the ports will return all highs because of passive pull-ups).

So the manual predicts a read of an output bit as the pin. Where nothing outside
pulls the line, the pin is what the CIA drives: a written 0 reads 0 and a
written 1 reads 1, at every position. Where something outside pulls the line
low while the CIA drives 1 — for PA5 and PA4 the drive, of which p.244 says
*"The drive will pull this line low"*, for PA6 and PA7 a closed fire button,
p.329's *"normally opened switches"* — no page says which of the two sets the
pin, so none predicts what the read returns.

**PA6 and PA7** as outputs are named twice more. p.229 lists `CIAAPRA ($BFE001)
Input and output for pin 6 (port 1 & 2 fire buttons)`, and p.329 presupposes
that a set direction bit changes what those two bits read:

> Before reading this register, the corresponding bits of the data direction register must be cleared to define input mode

**PA2–PA5** are called inputs for what the disk subsystem does with them. p.241
lists `CIAAPRA ($BFE001) four input bits for disk sensing`; p.339's CIAA
address map gives `ddra` as `Direction for port A (BFE001);1=output (set to
0x03)`, read from the render; and p.244 says:

> Bits labeled PA are input bits in CIAAPRA ($BFE001).

None of the three says a direction bit cannot make those bits outputs; each says
how the system uses them.

## What NanoMig does

```verilog
pa_out[7:0] = {porta_in2[7:2], porta_out[1:0]};                      // :254
assign porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]),
                         (~ddrporta[1:0] | regporta[1:0])};          // :262
```

Two different reasons, which is why this is one finding rather than two:

- **PA2–PA5** — a CIAAPRA read takes bits 7:2 straight from the synchronised
  input. No direction bit can make them outputs at all.
- **PA6/PA7** — their written value *is* captured and *does* drive
  `porta_out[3:2]`, which leaves the module toward the fire-button wiring. But
  the same read mux never reads it back, so a CPU read of CIAAPRA never reflects
  a value it set.

Nothing above the module brings the written PA6/PA7 value back either.
`minimig.v` takes it out as `_fire1_dat,_fire0_dat`
([`:605`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L605))
and feeds the fire lines in from `userio`
([`:622`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L622)),
where outside the CD32-pad mode they are the joystick and mouse alone
([`userio.v:313-314`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L313-L314)):
`assign _fire0 = cd32pad && !cd32pad1_reg_load ? fire1_d : _sjoy1[4] & _mleft;`.
At PA2–PA7 a read is the line whatever the direction bit, so an output driven 0
on a line nothing pulls reads 1, where p.341 predicts 0.

### Measured

The CIAAPRA bit each position reads, with every external line released, as the
two sweeps print them (`MEASURED:` lines, PROP-1 and PROP-2 alike):

    printed  stimulus               PA0  PA1  PA2  PA3  PA4  PA5  PA6  PA7   expected
    ctrl     own DDR 0, PRA 0         1    1    1    1    1    1    1    1   1
    cross    DDR at n+1, PRA 0        1    1    1    1    1    1    1    1   1   (PROP-1 only)
    drv0     own DDR 1, PRA 0         0    0    1    1    1    1    1    1   0
    drv1     own DDR 1, PRA 1         1    1    1    1    1    1    1    1   1
    PROP-1, PROP-2                 PASS PASS FAIL FAIL FAIL FAIL FAIL FAIL

    3 passed, 2 failed

The two PA0-restricted controls call the identical comparator functions and pass
in the same run, so the comparator is live. At PA2–PA7 each FAIL is the written
0 read back as 1, the released line `ciaa.v:254` returns whatever the direction
bit.

## What the test measures

The fixture releases all six external lines
(`test_ch8-ciaa-port-direction.cpp:157`):

```cpp
dut->porta_in = 0x3F;
```

`0x3F` is every line high in the six-bit packing Verilator gives `porta_in[7:2]`
(`test_ch8-ciaapra-disk-sense-bits.cpp:240-247`): the idle state p.354
describes, both fire buttons open and no drive-status line asserted.

- **stimulus**, for each position n, PA0–PA7: DDRA = 0 with PRA = 0, the
  control; then DDRA = `1<<n` with PRA = 0; then PRA = `1<<n` (PROP-2,
  `observe_bit()`, `test_ch8-ciaa-port-direction.cpp:257-273`). PROP-1
  (`observe_position()`, `:288-311`) reads the same three and, between the
  control and the written 0, a cross-bit control: DDRA set at the next
  position, (n+1) mod 8, with PRA = 0 (`:297-300`);
- **observed:** CIAAPRA (`rs` = 0) bit n after each write, printed per position
  by both sweeps (`:337-341`, `:356-359`);
- **expected**, from p.241, p.341 and p.354: with the position's own direction
  bit clear, 1, the released line; with it set, 0 after
  the written 0 and 1 after the written 1, because the read is the pin and
  nothing but the output drives it.

The written 0 is the half that decides. On a released line the control, the
cross-bit control and the written 1 read 1 for an implementation that returns
the line and for one that returns the written value alike; only the written 0
tells them apart, and at PA2–PA7 `ciaa.v:254` returns the line there, 1. No
line is held low anywhere in the package, so the written 1 over a line held
low, the case no page decides, is not presented.

[`hrm/ch8-ciaapra-disk-sense-bits`](../../hrm/ch8-ciaapra-disk-sense-bits)'s
PROP-INPUT grades that held-low case for PA2–PA5: it drives one position's line
low (`test_ch8-ciaapra-disk-sense-bits.cpp:295`), sets that position's
direction bit and writes 1 (`:308-309`), and expects 0 (`:311`); it is green,
`1 passed, 0 failed`. The two packages grade no common state: an implementation
in which a line held low wins over an output driving 1, and the output wins
otherwise, passes both.

## What the others say

| | | |
|---|---|---|
| **Denise** | [`libami/system/system.cpp:54`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L54) | the CPU read of CIA-A port A: `out = (lines->pra & lines->ddra) \| (out & ~lines->ddra);` — all eight, and every external contribution passes through `~ddra`, so an output returns the written value whatever the line does: 0 on a released line, and 1 over a line held low, which no page states. The only one of the four that returns the written 0 at every position whatever is attached |
| **Copperline** | [`bus.rs:8998-9010`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8998-L9010) | its generic core is full width, and `cia_a_read` then discards it: PA7/PA6 are overwritten from the fire buttons, then `v = (v & !0x3C) \| self.floppy.cia_a_status_bits();`, no DDR term. **NanoMig's split, bit for bit** |
| **WinUAE** | [`cia.cpp:1916-1917`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1916-L1917) | `:1917` implements the rule for PA1/PA0, and `handle_joystick_buttons(c->pra, c->dra)` on the line above reads an output-low PA7/PA6 as 0 ([`inputdevice.cpp:4012-4015`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4012-L4015)), so a written value comes back on those two while the fire button is released; a pressed button reads 0 whatever was written ([`inputdevice.cpp:4005-4006`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4005-L4006)), the pin with the line winning. PA2–PA5 are never stored: the write keeps only those four bits at [`:2128`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2128) — `c->pra = (c->pra & ~0xc3) \| (val & 0xc3);` |
| **vAmiga** | [`CIA.cpp:863-867`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L863-L867) | muxes all eight, then lets an attached control-port device override PA7/PA6 *after* the mux without consulting `ddra`. A port has no device until the host sends it mouse or joystick input ([`ControlPort.h:50`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPort.h#L50)); a mouse with its default pull-ups then forces a released fire bit to 1 even when it is an output driven 0 ([`Mouse.cpp:155-169`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Mouse/Mouse.cpp#L155-L169)), and a pressed joystick button forces it to 0 ([`Joystick.cpp:198`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Joystick/Joystick.cpp#L198)). The drive status enters only through `~ddra`, so over a line held low PA2–PA5 return the written 1 |
| **MiSTer** | [`rtl/ciaa.v:300`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L300) | descendant — the same split, plus one added trailing comment: `pa_out[7:0] = {porta_in2[7:2],porta_out[1:0]};  // Mix inputs and outputs` |
| **TC64** | [`rtl/minimig/ciaa.v:230`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L230), [`:238`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L238) | descendant — `rtl/minimig/ciaa.v:230,238` — byte-identical to NanoMig, no comment added either: the same `pa_out` assignment and the same `porta_out[3:0]` pull-up assign. A fix diverges from both lines equally; neither has the eight-bit mask this page requires |

**Copperline's split is demonstrably deliberate, not an oversight.** Eleven lines
after the one above, in the same function, its **port B** overlay masks with
`!port_b_ddr()` under the comment *"A pin the guest has switched to an output
stays CIA-driven"*, and CIA-B applies `!ddr` to all three of its overlays.
Somebody decided port A's status bits are different.

**Two of the four gate the printed behaviour on a chip part number, one of them
only in part.** vAmiga's `MOS_8520_PLCC`
([`CIA.cpp:869-871`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L869-L871),
default off) restores strict one-to-one on all eight. WinUAE's `391078-01` type
([`cia.cpp:1923-1927`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1923-L1927),
enabled for the A600 alone, default off) returns PRA contents for every output
bit — its own comment on that branch reads *"391078-01 CIA: output mode bits
always return PRA contents"* — but the write at `:2128` never stores PA2–PA5,
so an output-mode read of those four returns their reset value, zero
([`cia.cpp:2389`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2389)),
whatever was written.

**That is what the split may be about.** The question behind p.241 may not be
"does the implementation obey the page" but "which CIA part is fitted", and two
independent emulator authors evidently concluded the answer differs by part.
That is a decision for Amiga specialists.

One corroboration worth passing on: vAmiga logs *"DDRA: Setting unusual value"*
for CIA-A whenever the write is not `0x03`
([`CIARegs.cpp:230-231`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L230-L231))
— its own code treats PA1/PA0-as-outputs as the only expected mask, which is
exactly the pair NanoMig implements.

## Suggested change

**Not without an answer to the part-number question.** If the fitted CIA
returns PRA contents for bits in output mode, `pa_out` should mux on `ddrporta`
across all eight positions, the way Denise does in one line. If it does not, the
page is describing a part this core does not model, and the finding is
documentation rather than a defect.

What should not happen is a change made because a test is red. The package is
red where p.341 predicts the value, an output driven 0 on a released line;
whether the page is right about the silicon is a different question with a
different owner.

## What holds the finding up

- p.241 makes the mask one-to-one, and p.341, to which it refers, makes every PR
  bit whose DDR bit is 1 an output and the read the pin: an output driven 0 on a
  line nothing else pulls reads 0.
- At six of eight positions `ciaa.v:254` returns the line whatever the direction
  bit, for two distinct and independently readable RTL reasons, so that output
  reads 1. PROP-1 and PROP-2 measure it: with every line released, the written
  0 reads 1 at PA2–PA7 and 0 at PA0–PA1.
- Two controls on the identical comparator pass in the same run, so the
  comparator is live.

## What weakens it

- The package presents only the released line, p.354's idle state. Where a
  closed fire button or an asserting drive holds a line low, no page predicts
  the read (*What the HRM says*), and the four split again there.
- p.241, p.244 and p.339 describe PA2–PA5 as the disk subsystem's inputs and
  DDRA as set to 0x03. None says a direction bit cannot make them outputs, but
  `hrm/ch8-ciaapra-disk-sense-bits` reads p.244 that way; it grades that
  reading only over a line held low, a case p.341 does not decide.
- Only one implementation of four does what the page says by default whatever is
  attached; vAmiga does only while no mouse is attached.
- Two of the four treat it as part-dependent, which is a reading the page does
  not offer and the manual may simply predate.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, 6526 CIA specification, 1981**, PDF page 1 — agrees with the manual:

  > "16 Individually programmable I/O lines"
  <!-- corpus: cia-6526-spec/p001.txt -->

  All sixteen port lines are individually programmable, and the functional description (p005) makes each port an 8-bit PR with an 8-bit DDR, a DDR bit setting 'the corresponding bit' of the PR to an output: the manual's one-to-one mask with no position exempted, against NanoMig's two of eight. Limits: this is the 6526, not the 8520 or its Amiga variants, which the finding already records as possibly differing on read-back; and its read rule returns 'actual port pins' for output bits too, so it promises the written value back only where nothing outside drives the pin.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheet 4a and the overview sheet — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 4a, drawn transistor by transistor for PA0, keeps the PRA bit and the DDRA bit in two latches written from D0io. The pad has a passive pull-up, a pull-down that is on while the DDRA bit is set and the PRA bit clear, and an active pull-up that is on while both are set; with the DDRA bit clear both are off and the pad is released. A read of PRA puts the pad's level, sampled on PHI1, on D0io; no path leads from the PRA latch to the bus. A read of DDRA returns the DDRA latch. The overview sheet names one direction bit per line, DDRA0 to DDRA7, under "port A direction register, 1=output".
  <!-- drawing: cia-8520r4-dissection/sheet_005.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_001.txt -->

  Each port A line has a direction bit of its own that alone decides whether the line is driven, and a read of PRA returns the pin, output bits included, as p. 341 prints. Only PA0's cell is drawn; that PA1 to PA7 are alike is read from the overview sheet's labels, not from a drawing of their cells. The dissection is of one revision.

Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

