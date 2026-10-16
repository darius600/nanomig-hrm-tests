# NM-HRM-040 — CIAADDRA's direction mask corresponds one-to-one for two bit positions of eight [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `ciaa.v` let CIAAPRA bits 7:2 read back what the CIA drives once their CIAADDRA bit is set, so that an output driven 0 on a released line reads 0 at PA2–PA7 as it already does at PA1 and PA0, where p.341 predicts 0? Closes when `ciaa.v` stores all eight bits and reads PA2–PA7 back as driven while their direction bit is set, in either form *Decision* leaves open (`PROP-1` and `PROP-2` turn green), or when an Amiga specialist rejects the change; if the rejection means p.241 and p.341 are wrong, the page is refiled against the printed page. The CIA part number does not decide the red: on a released line an output driven 0 reads 0 whether the part returns the pin or the written value. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's `ciaa` module, CIA-A port A (CIAAPRA / CIAADDRA), all eight positions, with all six external lines released (`porta_in = 0x3F`); a line held low is not presented by the package; chip revision unknown (the one dissected CIA, an 8520R4, is drawn at PA0 only) |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware; printed p.229, p.241, p.244; Appendix E, printed p.329; Appendix F, printed p.339, p.341 (its port section, to which p.241 refers) and p.354 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · `PROP-1`: `opus:8-dio.c44`, p.241, p.341, p.354; `PROP-2`: `opus:8-dio.c46`, `sol:8-dio.c43`, p.241, p.341, p.354; `FIXTURE-LIVE` and the two PA0-restricted controls, `CONTROL-PA0-POSITIONAL` and `CONTROL-PA0-OUTPUT`, carry no claim id |
| **Module** | `ciaa.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L252-L262) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L298-L309); inherited |
| **Test** | [`hrm/ch8-ciaa-port-direction`](../../hrm/ch8-ciaa-port-direction): `make -C hrm/ch8-ciaa-port-direction test`; an expected red exits nonzero · [`hrm/ch8-ciaapra-disk-sense-bits`](../../hrm/ch8-ciaapra-disk-sense-bits) (green, `PROP-INPUT`): `make -C hrm/ch8-ciaapra-disk-sense-bits test` |
| **Assertions** | 2 red, `PROP-1` (`test_ch8-ciaa-port-direction.cpp:380-402`) and `PROP-2` (`:404-426`), each sweeping all eight positions with all six external lines released (`:157`): expected the written 0 to read 0 with the position's direction bit set, observed 1 at PA2–PA7 (`:264-266`, `:302-304`); both pass at PA0–PA1; the package reports `3 passed, 2 failed`. Two PA0-restricted controls on the identical comparators (`:428-446`) pass in the same run |
| **Verdict of the four** | **a four-way split** where p.341 predicts the value, an output driven 0 on a line nothing else pulls: Denise honours all eight; Copperline reproduces NanoMig's two-of-eight bit for bit; vAmiga honours all eight until a mouse is attached, whose pull-up then overrides that port's fire bit, and restores the full eight whatever is attached only under a chip-variant switch, modelled on WinUAE's and off by default; WinUAE follows the write on four (PA7/PA6 as a wired-AND with the fire button) and never stores PA2–PA5, so its chip-variant switch does not restore those four. Over a line held low, a case no page decides and the package does not present, they split again: Denise and vAmiga return the written 1 on PA2–PA5, WinUAE and Copperline the line. Hardware: the one dissected CIA, an 8520R4, draws PA0 as the pin read |
| **Effort** | S — stores all eight bits in `regporta` (`ciaa.v:238`) and changes the `pa_out` read mux (`:254`); the form over a line held low is open |

---

## Source requirement

**p.241**, the paragraph whole:

> The joystick fire buttons can also be configured as outputs. CIAADDRA ($BFE201) contains a mask that corresponds one-to-one with the data read register, CIAAPRA ($BFE001). Setting a 1 in the direction position makes the corresponding bit an output. See Appendix F for more details.

One-to-one, across the register; p.241 names no exception and does not say what a read of an output bit returns. **p.341**, Appendix F's port section:

> If a bit in the DDR is set to a 1, the corresponding bit position in the PR becomes an output. If a DDR bit is set to a 0, the corresponding PR bit is defined as an input.
>
> When you READ a PR register, you read the actual current state of the I/O pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or outputs.

**p.354** gives the state of a pin nothing drives:

> The port pins are set as inputs and port registers to zero (although a read of the ports will return all highs because of passive pull-ups).

So the manual predicts a read of an output bit as the pin. Where nothing outside pulls the line, a written 0 reads 0 and a written 1 reads 1, at every position. Where something outside pulls the line low while the CIA drives 1 — for PA5 and PA4 the drive, of which p.244 says *"The drive will pull this line low"*, for PA6 and PA7 a closed fire button, p.329's *"normally opened switches"* — no page says which of the two sets the pin, so none predicts the read.

PA6 and PA7 as outputs are named twice more: p.229 lists `CIAAPRA ($BFE001) Input and output for pin 6 (port 1 & 2 fire buttons)`, and p.329 presupposes that a set direction bit changes what those two bits read:

> Before reading this register, the corresponding bits of the data direction register must be cleared to define input mode

PA2–PA5 are called inputs for what the disk subsystem does with them; p.244 says:

> Bits labeled PA are input bits in CIAAPRA ($BFE001).

## RTL and measurement

```verilog
pa_out[7:0] = {porta_in2[7:2], porta_out[1:0]};                      // :254
assign porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]),
                         (~ddrporta[1:0] | regporta[1:0])};          // :262
```

Two different reasons, which is why this is one finding rather than two:

- **PA2–PA5** — a CIAAPRA read takes bits 7:2 straight from the synchronised input. No direction bit can make them outputs at all.
- **PA6/PA7** — their written value is captured and drives `porta_out[3:2]`, toward the fire-button wiring, but the same read mux never reads it back.

Nothing above the module brings the written PA6/PA7 value back: `minimig.v` takes it out as `_fire1_dat,_fire0_dat` ([`:605`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L605)) and feeds the fire lines in from `userio` ([`:622`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L622)), where outside the CD32-pad mode they are the joystick and mouse alone ([`userio.v:313-314`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L313-L314)). At PA2–PA7 a read is the line whatever the direction bit, so an output driven 0 on a line nothing pulls reads 1, where p.341 predicts 0.

### Measured

The fixture releases all six external lines (`test_ch8-ciaa-port-direction.cpp:157`), `dut->porta_in = 0x3F`, every line high in the six-bit packing Verilator gives `porta_in[7:2]` (`test_ch8-ciaapra-disk-sense-bits.cpp:258-265`): p.354's idle state. For each position n, PA0–PA7: DDRA = 0 with PRA = 0 (control); then DDRA = `1<<n` with PRA = 0; then PRA = `1<<n` (PROP-2, `observe_bit()`, `:257-273`). PROP-1 (`observe_position()`, `:288-311`) reads the same three and, between the control and the written 0, a cross-bit control: DDRA set at (n+1) mod 8 with PRA = 0 (`:297-300`). The observed value is CIAAPRA (`rs` = 0) bit n after each write. The expected value, from p.241, p.341 and p.354, is 1 with the position's own direction bit clear, and with it set 0 after the written 0 and 1 after the written 1, because the read is the pin and nothing but the output drives it.

The CIAAPRA bit each position reads, as the two sweeps print them (`MEASURED:` lines, `:337-341`, `:356-359`):

    printed  stimulus               PA0  PA1  PA2  PA3  PA4  PA5  PA6  PA7   expected
    ctrl     own DDR 0, PRA 0         1    1    1    1    1    1    1    1   1
    cross    DDR at n+1, PRA 0        1    1    1    1    1    1    1    1   1   (PROP-1 only)
    drv0     own DDR 1, PRA 0         0    0    1    1    1    1    1    1   0
    drv1     own DDR 1, PRA 1         1    1    1    1    1    1    1    1   1
    PROP-1, PROP-2                 PASS PASS FAIL FAIL FAIL FAIL FAIL FAIL

    3 passed, 2 failed

At PA2–PA7 each FAIL is the written 0 read back as 1, the released line `ciaa.v:254` returns whatever the direction bit. The written 0 is the half that decides: on a released line the control, the cross-bit control and the written 1 read 1 for an implementation that returns the line and for one that returns the written value alike, and only the written 0 tells them apart. The two PA0-restricted controls call the identical comparator functions and pass in the same run, so the comparator is live.

## Risks and unknowns

- The package presents only the released line, p.354's idle state; no line is held low anywhere in it. Where a closed fire button or an asserting drive holds a line low, no page predicts the read (*Source requirement*), and the four split again there: Denise and vAmiga return the written 1 on PA2–PA5, WinUAE and Copperline the line. `hrm/ch8-ciaapra-disk-sense-bits`'s `PROP-INPUT` (`test_ch8-ciaapra-disk-sense-bits.cpp:379-394`) holds one line of PA2–PA5 low (`:315`), writes 1 there with that position's direction bit clear (`:331-332`) and expects 0 (`:335`); it is green, `1 passed, 0 failed`. Neither package presents a written 1 over a line held low with the direction bit set.
- p.241 lists `CIAAPRA ($BFE001) four input bits for disk sensing`, p.339's CIAA address map gives `ddra` as `Direction for port A (BFE001);1=output (set to 0x03)` (read from the render), and p.244 calls them input bits (*Source requirement*). None says a direction bit cannot make PA2–PA5 outputs; each says how the system uses them. `hrm/ch8-ciaapra-disk-sense-bits` grades them as inputs only with their direction bits clear.
- Only Denise does what the page says by default whatever is attached; vAmiga does only while no mouse is attached.
- Two of the four switch the read on a part number, vAmiga's switch modelled on WinUAE's. By their own comments (*Independent evidence*) the switch decides only a line held from outside, but no Commodore document read for this page says what the 391078-01 does, and the silicon witness is PA0 of one 8520R4.
- The 6526 specification is the 6526, not the 8520 or its Amiga variants, which may differ on read-back; its read rule returns "actual port pins" for output bits too, so it promises the written value back only where nothing outside drives the pin.
- The 8520R4 dissection draws PA0 alone, of one revision, and names no part number beyond 8520R4, so it says nothing of the 391078-01 or of PA1 to PA7's cells (that those are alike is read from the overview sheet's labels, not from a drawing of their cells); and it does not say which wins when a line held low from outside meets the active pull-up.
- A change must not be made because a test is red. The package is red where p.341 predicts the value, an output driven 0 on a released line; one revision's PA0 agrees with the page there, and whether every part and position does is a question for the same specialists.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1916-1917`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1916-L1917) | `:1917` implements the rule for PA1/PA0, and `handle_joystick_buttons(c->pra, c->dra)` on the line above reads an output-low PA7/PA6 as 0 ([`inputdevice.cpp:4012-4015`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4012-L4015)), so a written value comes back on those two while the fire button is released; a pressed button reads 0 whatever was written ([`inputdevice.cpp:4005-4006`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4005-L4006)), the pin with the line winning. PA2–PA5 are never stored: the write keeps only those four bits at [`:2128`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2128) — `c->pra = (c->pra & ~0xc3) \| (val & 0xc3);` |
| **vAmiga** | [`CIA.cpp:863-867`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L863-L867) | muxes all eight, then lets an attached control-port device override PA7/PA6 *after* the mux without consulting `ddra`. A port has no device until the host sends it mouse or joystick input ([`ControlPort.h:50`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPort.h#L50)); a mouse with its default pull-ups then forces a released fire bit to 1 even when it is an output driven 0 ([`Mouse.cpp:155-169`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Mouse/Mouse.cpp#L155-L169)), and a pressed joystick button forces it to 0 ([`Joystick.cpp:198`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Joystick/Joystick.cpp#L198)). The drive status enters only through `~ddra`, so over a line held low PA2–PA5 return the written 1 |
| **Denise** | [`libami/system/system.cpp:54`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L54) | the CPU read of CIA-A port A: `out = (lines->pra & lines->ddra) \| (out & ~lines->ddra);` — all eight, and every external contribution passes through `~ddra`, so an output returns the written value whatever the line does: 0 on a released line, and 1 over a line held low, which no page states. The only one of the four that returns the written 0 at every position whatever is attached |
| **Copperline** | [`bus.rs:8998-9010`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8998-L9010) | its generic core is full width, and `cia_a_read` then discards it: PA7/PA6 are overwritten from the fire buttons, then `v = (v & !0x3C) \| self.floppy.cia_a_status_bits();`, no DDR term. **NanoMig's split, bit for bit** |
| **MiSTer** | [`rtl/ciaa.v:300`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L300) | descendant — the same split, plus one added trailing comment: `pa_out[7:0] = {porta_in2[7:2],porta_out[1:0]};  // Mix inputs and outputs` |
| **TC64** | [`rtl/minimig/ciaa.v:230`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L230), [`:238`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L238) | descendant — byte-identical to NanoMig, no comment added either: the same `pa_out` assignment and the same `porta_out[3:0]` pull-up assign. A fix diverges from both lines equally; neither has the eight-bit mask this page requires |

**Two of the four switch the read on a chip part number, and the two switches are one.** vAmiga's `MOS_8520_PLCC` ([`CIA.cpp:869-871`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L869-L871), default off) restores strict one-to-one on all eight; the header that declares it says `CIA_8520_PLCC mimics option "[X] 391078-01" in UAE (A600)` ([`CIATypes.h:17-20`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIATypes.h#L17-L20)). WinUAE's `391078-01` type ([`cia.cpp:1923-1927`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1923-L1927), enabled for the A600 alone, default off) returns PRA contents for every output bit — its own comment reads *"391078-01 CIA: output mode bits always return PRA contents"* — but the write at `:2128` never stores PA2–PA5, so an output-mode read of those four returns their reset value, zero ([`cia.cpp:2389`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2389)), whatever was written.

**What the switch separates, in its authors' own words, is a line held from outside.** WinUAE's changelog introduces the type: *"This CIA revision has a bug, if IO port is in output mode, reading it will always read output mode data state. Other models, including original DIP 8520, 391078-02 and Akiko internal CIAs read IO pin external voltage level as documented."* ([`winuaechangelog.txt:4655`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L4655)), and the option's label reads *"CIA revision that can't read IO pin status in output mode"* ([winuae.rc:859](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/resources/winuae.rc#L859)). vAmiga's default path carries *"A connected device may force the output level to a specific value"* ([`CIA.cpp:865`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L865)). One part returns the pin, the other the written value; on a released line the pin is what the CIA drives, so the two agree and the written 0 reads 0 on either. They part only where something outside holds the line against the output.

**Copperline's split is deliberate.** In the same function, eleven lines after its port A status bits, its port B overlay masks with `!port_b_ddr()` under the comment *"A pin the guest has switched to an output stays CIA-driven"* ([`bus.rs:9019-9026`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L9019-L9026)), and CIA-B applies `!ddr` to all three of its overlays ([`bus.rs:9136-9156`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L9136-L9156)).

Where two of the four read the written 0 back as 1 on a released line, as NanoMig does, Copperline at PA2–PA7 and WinUAE at PA2–PA5, neither ties it to a part. Copperline has no part switch, and WinUAE's default, which its changelog says reads the pin, reads PA2–PA5 from the drive status alone, `DISK_status_ciaa() & 0x3c` ([`cia.cpp:1915`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1915)), never from what the CIA drives there. vAmiga logs *"DDRA: Setting unusual value"* for CIA-A whenever the write is not `0x03` ([`CIARegs.cpp:230-231`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L230-L231)): its own code treats PA1/PA0-as-outputs as the only expected mask, the pair NanoMig implements.

### What other Commodore documents say

- **Commodore, 6526 CIA specification, 1981**, PDF page 1 — agrees with the manual:

  > "16 Individually programmable I/O lines"
  <!-- corpus: cia-6526-spec/p001.txt -->

  All sixteen port lines are individually programmable, and the functional description (p005) makes each port an 8-bit PR with an 8-bit DDR, a DDR bit setting 'the corresponding bit' of the PR to an output: the manual's one-to-one mask with no position exempted, against NanoMig's two of eight.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheet 4a and the overview sheet — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 4a, drawn transistor by transistor for PA0, keeps the PRA bit and the DDRA bit in two latches written from D0io. The pad has a passive pull-up, a pull-down that is on while the DDRA bit is set and the PRA bit clear, and an active pull-up that is on while both are set; with the DDRA bit clear both are off and the pad is released. A read of PRA puts the pad's level, sampled on PHI1, on D0io; no path leads from the PRA latch to the bus. A read of DDRA returns the DDRA latch. The overview sheet names one direction bit per line, DDRA0 to DDRA7, under "port A direction register, 1=output".
  <!-- drawing: cia-8520r4-dissection/sheet_005.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_001.txt -->

  Each port A line has a direction bit of its own that alone decides whether the line is driven, and a read of PRA returns the pin, output bits included, as p. 341 prints.

## Decision

**The part number does not decide this red.** On a released line a part that returns the pin and a part that returns the written value both read an output driven 0 as 0, so p.341 and both parts as WinUAE's and vAmiga's authors describe them give the written value back at every position the package sweeps, and the 8520R4 does at PA0, the one position it draws. Two things in `ciaa.v` stand between NanoMig and that value: `regporta` keeps four of the eight written bits, PA7, PA6, PA1 and PA0 (`ciaa.v:238`), so PA5–PA2 are never stored; and `pa_out` takes PA7–PA2 from the line alone (`ciaa.v:254`). A change stores all eight and lets every position whose `ddrporta` bit is set read back what the CIA drives.

The change leaves one choice, and only a line held low separates its two forms:

- **the pin**: the line ANDed with what the CIA drives, `~ddrporta | regporta`, at every position; at PA1 and PA0, which have no line into `ciaa`, that term alone, as now (`ciaa.v:262`). A line held low then reads 0 whatever was written. This is the read WinUAE's changelog gives the DIP 8520 and WinUAE's default gives PA7/PA6, and the read path the 8520R4 draws for PA0.
- **the written value**: the output register at every output bit, the way Denise does in one line; what WinUAE and vAmiga model for the 391078-01.

This package passes either, and so does `PROP-INPUT`, which writes 1 over a line held low only with the direction bit clear (`test_ch8-ciaapra-disk-sense-bits.cpp:331-335`), where both forms read the line. No page on this finding says which part a core should follow over a line held low.
