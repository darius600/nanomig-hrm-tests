# NM-HRM-087 — with BPLCON0 bit 3 set, VPOSR and VHPOSR keep counting: neither a light-pen trigger nor the end of a field freezes them, where pp. 238 and 239 do both [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `agnus_beamcounter.v`, with BPLCON0 bit 3 set, freeze VPOSR and VHPOSR on a light-pen trigger and, with none, at the end of the field, until vertical position 20, as p. 239 prints, where NanoMig returns the running counter? It closes when the latch is in place and the nineteen reds listed under *Assertions*, with the 78 reds of the five further packages listed under *Where else the suite meets it*, are green on both cores, or when an Amiga specialist rejects the reading; if that means pp. 238-239 are wrong, the page is refiled against the printed page |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | OCS Agnus, and ECS Agnus and the AA chip set with BEAMCON0 bit 13 clear; BPLCON0 bit 3 set for more than twenty fields in `integration/ch8-light-pen-beam`, VPOSR and VHPOSR read with a pin-6 pulse on port 1, on port 2, on both and with none; graded in OCS/PAL and OCS/NTSC, recorded not graded in ECS/PAL and ECS/NTSC by `integration/ch8-light-pen-beam` (p. 305 gives LPENDIS no reset value). `integration/appDE-ports-disk`, `integration/ch7-system-control` and `integration/ch3-playfield-system` write BEAMCON0 with bit 13 clear first, so they grade ECS/PAL and ECS/NTSC as they grade OCS; `integration/aa-integ-s1` writes it with bit 13 clear and grades the AA chip set. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed pp. 227, 228 (Table 8-1), 229, 232, 238, 239; 7 — System Control, printed pp. 216, 219; 3 — Playfield Hardware, printed pp. 79, 89; Appendix A, printed p. 271; Appendix C, printed pp. 295, 298, 300, 305; Appendix E, printed p. 333 · *Amiga Hardware Reference Manual*, 3rd ed., 1991; for the AA chip set, Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY, PDF p. 12 · <details><summary>claim ids by assertion (71)</summary> claims by assertion: `P1`: `opus:8-lpen.c1`, `sol:8-pen.c1`, `opus:8-intro.c12`, `sol:8-overview.c6`, pp. 238, 227; `P2`: `opus:8-lpen.c16`, `sol:8-pen.c17`, `opus:8-tbl8-1.c20`, `sol:8-pin.c22`, pp. 238, 228; `P3`: `opus:8-lpen.c17`, `opus:8-lpen.c18`, `sol:8-pen.c18`, `sol:8-pen.c19`, p. 238; `P3(a)`: `sol:8-pen.c20`, p. 238; `P4`: `opus:8-regs.c18`, `opus:8-regs.c20`, `sol:8-reg.c20`, `sol:8-reg.c22`, pp. 229, 238; `P5`: `opus:8-lpen.c36`, `sol:8-pen.c36`, p. 239; `P6`: `opus:8-lpen.c38`, `sol:8-pen.c38`, p. 239; `P7`: `opus:8-lpen.c37`, `sol:8-pen.c37`, p. 239; `P8`: `opus:8-lpen.c39`, p. 239; `P8(release-at-20)`: `sol:8-pen.c39`, p. 239; `P9(release)`: `opus:8-lpen.c12`, `sol:8-pen.c12`, p. 238; `P10(a)`: `opus:8-lpen.c35`, `sol:8-pen.c35`, `opus:8-regs.c16`, `sol:8-reg.c18`, pp. 239, 229; `P11`: `opus:8-lpen.c43`, `sol:8-pen.c41`, `opus:8-lpen.c47`, `sol:8-pen.c44`, `opus:8-lpen.c48`, `sol:8-pen.c45`, `sol:8-pen.c46`, p. 239; `P12`: `opus:8-lpen.c52`, p. 239; `P13`, green: `opus:8-lpen.c10`, `opus:8-lpen.c11`, `sol:8-pen.c10`, `sol:8-pen.c11`, p. 238. `integration/appDE-ports-disk`: `LP-P2-LATCH-AT-PULSE`: `opus:E-lightpen.c12`, `sol:E-lightpen.c10`, `sol:E-lightpen.c21`, pp. 333, 238; `LP-P3A-HOLD`: `opus:E-lightpen.c5`, `sol:E-lightpen.c4`, pp. 333, 239; `LP-P1-LATCH-ENABLE`: `opus:E-lightpen.c21`, `sol:E-lightpen.c18`, pp. 333, 239; `LP-LATCH-ENABLE-AND-POSITION`: `opus:E-lightpen.c3`, `sol:E-lightpen.c2`, p. 333. `integration/ch7-system-control`: `P3-LATCHED-PEN-POSITION`: `opus:7-bp.c3`, `sol:7-beam.c3`, `opus:7-bp.c11`, pp. 216, 238, 239. `integration/ch3-playfield-system`: `P3 aggregate`: `opus:A-bpc.b03.c0`, `sol:A-bplcon.c49`, `opus:A-leg.lpen1`, `sol:A-bplcon.c72`, `opus:3-r0.c09`, `sol:3-regsum-bplcon0.c6`, pp. 271, 89, 229, 239; `P4 aggregate`, recorded not graded: `opus:A-leg.lpen2`, `sol:A-bplcon.c73`, p. 271. `integration/aa-integ-s2`: `P8`: `opus:AA-beamcon0.b13`, `sol:AA-beamcon0.c8`, `opus:AA-beamcon0.c9`, `opus:AA-beamcon0.c10`, `opus:AA-beamcon0.c8`, `sol:AA-beamcon0.c24`, `sol:AA-beamcon0.c25`, `sol:AA-beamcon0.c26`, AA p. 12. `P2(ii)`, `P3(b)`, `P3(c)`, `P9`, `P9(boundary)` and the control `P10(b)` carry no claim id, and neither do the other reds of the five further packages (`P10-LATCH`, `LP-P1A-LATCH-HOLDS`, `LP-P3B-HELD-PULSE-IGNORED`, `LP-P3D-PULSE-BEFORE-ENABLE`, `LP-P4-DETERMINE-TRIGGER`, `P3-HELD`, `P3-PEN-POSITION`, `P3-FIELD-END-LATCH`, `P4-PRE-LATCHF1`, `P4-PRE-CONTROL`, `P3-MAIN`, `P8-0-VH`) or `P4-LPENDIS-READ`, recorded not graded</details> |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L107-L115) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121); MiSTer has no latch either (its read path uses a delayed copy of the counter) and TC64's is NanoMig's line for line; inherited |
| **Test** | [`integration/ch8-light-pen-beam`](../../integration/ch8-light-pen-beam): `make -k -C integration/ch8-light-pen-beam test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. One program, graded in OCS/PAL and OCS/NTSC on both CPU cores. `make -k -C integration/ch8-light-pen-beam latch-test` runs the package against the RTL with the latch of *Decision* applied (`latch.patch`) and exits zero; `late-test` runs it with the release one line late (`late.patch`) and exits nonzero. Five more packages, each with an expected red exiting nonzero: [`integration/appDE-ports-disk`](../../integration/appDE-ports-disk): `make -k -C integration/appDE-ports-disk test`, pulses on one connector at a time, in OCS and ECS, PAL and NTSC; [`integration/ch7-system-control`](../../integration/ch7-system-control): `make -k -C integration/ch7-system-control test`, pulses on both ports at Copper-marked lines, in the same four; [`integration/ch3-playfield-system`](../../integration/ch3-playfield-system): `make -k -C integration/ch3-playfield-system test`, bit 3 set and no pulse, in the same four; [`integration/aa-integ-s1`](../../integration/aa-integ-s1): `make -k -C integration/aa-integ-s1 test`, bit 3 set and no pulse, in the AA chip set; [`integration/aa-integ-s2`](../../integration/aa-integ-s2): `make -k -C integration/aa-integ-s2 test`, a pulse on both ports at line 255, in the AA chip set (each also as `test-fx68k` and `test-tg68k`) |
| **Assertions** | 19 red on fx68k and 19 on TG68K in each of OCS/PAL and OCS/NTSC, in a package run of 156 passed, 38 failed on each core: `P1`, `P2`, `P2(ii)`, `P3`, `P3(a)`, `P3(b)`, `P3(c)`, `P4`, `P5`, `P6`, `P7`, `P8`, `P8(release-at-20)`, `P9`, `P9(boundary)`, `P9(release)`, `P10(a)`, `P11` and `P12` (pp. 227, 228, 229, 238, 239). `P1` expects a pin-6 pulse on one port captured and observes *captured=no* on port 1 and on port 2; `P7` expects one value in lines 0 to 19 after a field with no pulse and observes reads that are not one value. In ECS/PAL and ECS/NTSC the same nineteen and `P2(i)` and `P9(a)` are recorded, not graded. The page-model control is green in all four configurations on both cores (25 passed, 0 failed each), and `P10(b)`, bit 3 clear, is green. Five further packages are red the same way on fx68k and on TG68K, 78 lines on each core, listed under *Where else the suite meets it*. |
| **Verdict of the four** | **split, two and two**: WinUAE and Copperline latch VPOSR and VHPOSR in OCS while BPLCON0 bit 3 is set, on a pulse from their light-pen device and, with none, at the end of the field; WinUAE releases the latch at the end of vertical blanking, Copperline holds it into the next field. vAmiga and Denise have no light-pen latch and freeze the counters only for ERSY. None of the four takes the trigger from a joystick's fire line. With BEAMCON0 bit 13 clear the same split holds in ECS and in the AA chip set; with it set, WinUAE hides the latch from the read and Copperline refuses a pulse but keeps the field-end latch (read from their code, not run). Hardware: not measured |
| **Effort** | M — `agnus_beamcounter.v` (the enable bit, the latch, the read multiplexer), a light-pen input on `agnus.v` and `agnus_beamcounter.v`, and its wiring from the controller ports' pin 6 in `nanomig.v` or `minimig.v` |

---

## Source requirement

**Printed p. 238**, *Reading a Light Pen*, names the port, the pin and what a
trigger does:

> A light pen can be connected to one of the controller ports. On the A1000, the light pen must be connected to port 1. Changing ports requires a minor internal modification. On the A500, A2000 and A3000 the default is port 2. An internal jumper can select port 1. Regardless of the port used, the light pen design is the same.

> 1. Just as the system exits vertical blank, the capture circuitry for the light pen is automatically enabled.

> 3. The sensors in the light pen see a pulse of light as the video beam passes by. The pen converts this light pulse into an electrical pulse on the "Beam Trigger" line (pin 6).

> 4. This trigger signal tells the internal circuitry to capture and save the current contents of the beam register, VPOSR. This allows you to determine where the pen was placed by reading the exact horizontal and vertical value of the counter beam at the instant the beam passed the light pen.

Table 8-1 on p. 228, *read from the table, not quoted*, gives the light pen
pin 6, *"Beam trigger"*, and pin 5, *"Pen pressed to screen"*.

**Printed p. 239** enables the input with bit 3 of BPLCON0 and says what
happens then, with and without a trigger:

> Once the light pen input is enabled and the light pen issues a trigger signal, the value in VPOSR is frozen. If no trigger is seen, the counters latch at the end of the display field. It is impossible to read the current beam location while the VPOSR register is latched. This freeze is released at the end of internal vertical blanking (vertical position 20).

and ends the section with the consequence for software:

> Note that when the light pen latch is enabled, the VPOSR register may be latched at any time, and cannot be relied on as a counter.

Between the two, p. 239 gives two methods of telling a trigger from the
field-end latch, both by comparing the frozen long word with $10500 (V = 261).

**Printed p. 229** lists the registers:

> BPLCON0 ($DFF100) Bit 3 enables the light pen latch
> VPOSR ($DFF004) Read light pen position (high order bits)
> VHPOSR ($DFF006) Read light pen position (low order bits)

**Printed p. 216**, in chapter 7's description of the beam counter:

> In addition, when you are using a light pen, this same address is used to read the light pen position rather than the beam position.

**Appendix A, printed p. 271**, BPLCON0:

> LPEN =Light pen enable (reset on power up)

**Chapter 3, printed p. 89**, the summary of BPLCON0:

> Bit 3 -  LPEN  (light pen enable)

**Appendix E, printed p. 333**, *Game Port Interface to Light Pen*:

> As the electron beam sweeps past the light pen, a trigger pulse is generated which can be enabled to latch the horizontal and vertical beam positions.

> 6 LPENTG* Light pen trigger VPOSR, VHPOSR

### Nothing on these pages predicts a running counter with bit 3 set

- **The end-of-field latch needs no pen.** p. 239's *"If no trigger is seen,
  the counters latch at the end of the display field"* holds with nothing
  plugged in. Which port or pin carries the pen, and with which polarity,
  does not bear on it.
- **p. 333's note** — *"depending on the maker, the light pen input may be
  either"* — does not say either of what. It leaves the trigger's polarity or
  port open, not whether a trigger or a field end latches.
- **p. 239's "may be latched at any time"** describes latching, not its
  absence.
- **LPENDIS** is an ECS register bit; see below.

### What ECS changes, and what it does not

**Appendix C, printed p. 305**, BEAMCON0, a register p. 298 lists as new for
the enhanced Agnus:

> 13 LPENDIS Ignore latched pen value on vertical pos read

What ECS adds is one bit, and by its own words it acts on the read: a vertical
position read ignores the latched pen value. p. 305 gives the bit no reset value, and its
warning on the register says *"Do not write to this register."* What ECS does
not change: p. 300 keeps bit 3 in ECS's BPLCON0,

> 03 LPEN Compatible with SuperHires mode

and p. 295 introduces the chips with

> These chip revisions support advanced features in addition to all of the standard features previously available.

So in OCS the page is unconditional: bit 3 set, a trigger or a field end
freezes VPOSR and VHPOSR until vertical blanking ends. In ECS it is the same
unless LPENDIS is set, and whether it is set after reset the page does not
say. That is why `integration/ch8-light-pen-beam`, which never writes BEAMCON0, grades the latch in OCS and records it in ECS. `integration/appDE-ports-disk`, `integration/ch7-system-control` and `integration/ch3-playfield-system` write BEAMCON0 with bit 13 clear before they read, `$0020` in PAL and `$0000` in NTSC, which leaves ECS under p. 239 alone, and grade ECS as they grade OCS.

**AA specification, PDF p. 12**, BEAMCON0, says what LPENDIS does to the read for the AA chip set:

```
LPENDIS= When this bit is a low and LPE (BPLCON0,BIT 3) is
 enabled, the light-pen latched value (beam hit position)
 will be read by VHPOSR, VPOSR and HHPOSR.  When the bit is
 a high the light-pen latched value is ignored and the actual
 beam counter position is read by VHPOSR, VPOSR and HHPOSR.
```

With the bit low and BPLCON0 bit 3 set, then, the latched value is what the three position registers return, and that is the AA chip set under p. 239's latch. The AA page gives LPENDIS no reset value either: of the bits it describes there it says *"It is cleared upon reset"* only of HARDDIS. `integration/aa-integ-s1` writes BEAMCON0 = `$0020` before it reads, which leaves the AA chip set under p. 239 and p. 12 with the bit low.

## RTL and measurement

**VPOSR and VHPOSR always read the running counter.** The read multiplexer has
no other source —
[`src/minimig-aga/agnus_beamcounter.v#L107-L115`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L107-L115):

```verilog
//beamcounter read registers VPOSR and VHPOSR
always @(*) begin
	if (reg_address_in[8:1]==VPOSR[8:1] || reg_address_in[8:1]==VPOSW[8:1])
		data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
	else if (reg_address_in[8:1]==VHPOSR[8:1] || reg_address_in[8:1]==VHPOSW[8:1])
		data_out[15:0] = {vpos[7:0],hpos[8:1]};
```

`vpos` and `hpos` are the counters themselves; `vpos` changes only by the
increment at each line and by a VPOSW/VHPOSW write
([`#L325-L337`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L325-L337)).

**Agnus does not keep BPLCON0 bit 3.** The beam counter takes bit 1 and bit 2
of a BPLCON0 write and nothing else —
[`#L144-L163`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L144-L163),
`ersy <= data_in[1];` and `lace <= data_in[2];` — and the bitplane sequencer
takes bits 6, 15, 4 and 14–12 —
[`src/minimig-aga/agnus_bitplanedma.v#L282`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L282).
Denise stores the whole register and names bit 3,
`assign lpen     = bplcon0[3];`, and no line of `denise.v` reads `lpen` —
[`src/minimig-aga/denise.v#L168`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L168),
[`#L191`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L191).

**LPENDIS is stored in ECS and read nowhere.** BEAMCON0 is written only with
`ecs` set and resets to `{10'b0, ~ntsc, 5'b0}`, bit 13 clear; the wire that
would name the bit is a comment, `//wire lpendis      = beamcon0_reg[13];` —
[`#L117-L129`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L117-L129).

**No light-pen line reaches Agnus.** Neither `agnus`
([`agnus.v#L54-L100`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L54-L100))
nor `agnus_beamcounter`
([`#L27-L57`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L27-L57))
has an input for one, and no port of `nanomig.v` is one. Pin 6 of each
controller port goes to CIA-A and nowhere else: `nanomig.v` passes its two
joystick bytes, inverted, to Minimig's `_joy1` and `_joy2`
([`src/nanomig.v#L1552-L1553`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/nanomig.v#L1552-L1553)),
`userio` crosses them over, `_sjoy1 <= joy_swap ? joy1 : joy2;` with
`joy_swap` tied to 0
([`src/minimig-aga/userio.v#L215-L218`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L215-L218),
[`#L92`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L92)),
and makes the two fire lines from bit 4, the first also from mouse button 0
([`#L313-L314`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L313-L314));
both end at CIA-A's port A, `_fire1` on PA7 and `_fire0` on PA6
([`src/minimig-aga/minimig.v#L632`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L632),
`.porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change}),`). p. 232 gives
port 1 bit 6 and port 2 bit 7, so `joystick0` bit 4 is port 2's pin 6 and
`joystick1` bit 4, and `mouse_buttons` bit 0, port 1's. The package measures
the same mapping at the ports (its `SPECIFICATION.md`, section 3, *Port
mapping*), and drives the pen's trigger there: bit 4 of `joystick0`,
`joystick1` or both set for 4 colour clocks, which takes pin 6 low from its
open-switch level. That the line is pin 6 is p. 238's step 3 and Table 8-1;
that it is the FIRE input is the A500/A2000 technical reference's, below.

### Measured

[`integration/ch8-light-pen-beam`](../../integration/ch8-light-pen-beam) runs
one program in four phases: it samples VPOSR:VHPOSR as a long word with
BPLCON0 bit 3 clear (phase A), then with bit 3 set for more than twenty fields
(phase B), then clear again (phase C), then reads POTGOR (phase D). Phase A fits
the beam model the rest is graded against. In phase B the harness pulses pin 6
on port 1, on port 2 and on both, at lines 10 to 258 and at H 100 to 108, and
leaves fields with no pulse at all. The model is anchored and the fixture is
live in every configuration on both cores: `CP1` to `CP6` are green, with u = 7
colour clocks, CIA-A's port A reading $FF at idle, and all 54 input changes
landing on their scheduled cycle, each pulse exactly 4 colour clocks wide.
`P13`, graded through the port pins' pot lines, is green.

| configuration | fx68k | TG68K | page-model control, both cores |
|---|---|---|---|
| OCS/PAL | 15 passed, **19 failed** | 15 passed, **19 failed** | 25 passed, 0 failed |
| OCS/NTSC | 15 passed, **19 failed** | 15 passed, **19 failed** | 25 passed, 0 failed |
| ECS/PAL | 13 passed, 0 failed; the 21 recorded | the same | 25 passed, 0 failed |
| ECS/NTSC | 13 passed, 0 failed; the 21 recorded | the same | 25 passed, 0 failed |

The nineteen reds are the same in both OCS configurations and on both cores:

| assertion | page | what it grades |
|---|---|---|
| `P1` | 238, 227 | a pin-6 pulse on at least one port is captured, frozen and not current, at the position `P3(a)` and `P3(b)` require; recorded: port 1 *captured=no*, port 2 *captured=no* |
| `P2` | 238, 228 | pin 6 captures and pins 1–4, 5 and 9 do not |
| `P2(ii)` | 238, 228 | the pin-6 pulse of field B18 is captured; its sibling `P2(i)`, the other pins' pulses are not captured, is green |
| `P3` | 238 | the captured position is the beam's at the pulse, exactly: V, H within a window, and one offset for every pulse |
| `P3(a)` | 238 | the captured V is the model line at the pulse |
| `P3(b)`, `P3(c)` | 238 | the captured H lies within [H_ref(t_on), H_ref(t_on)+4+u]; the offset H − H_ref(t_on) is the same for every pulse |
| `P4` | 229, 238 | V8 in VPOSR bit 0, V7–V0 and H8–H1 in VHPOSR |
| `P5` | 239 | after a trigger VPOSR is frozen until the release |
| `P6` | 239 | no read in a latched window is current |
| `P7` | 239 | a field with no trigger latches at its end |
| `P8` | 239 | every latch holds until the end of internal vertical blanking, and the counter is current after it |
| `P8(release-at-20)` | 239 | the held value is still visible up to V = 20 and the live counter from u colour clocks after it, in each of 20 windows |
| `P9` | 238 | capture re-arms by itself as vertical blank ends: no hold-over, a pulse after blank is captured, in every field |
| `P9(boundary)` | 238 | a pulse at line 20, the stated release line, is captured |
| `P9(release)` | 238 | no pulse is held over the release (`P9(a)`, green) and the pulse at line 20 is captured (`P9(boundary)`) |
| `P10(a)` | 239, 229 | with bit 3 set, pulses are captured and field ends latch |
| `P11` | 239 | the two-read detection method answers correctly |
| `P12` | 239 | the single read during vertical blanking answers correctly |

**Nothing latches, with or without a pulse.** `P1` records that neither port's
pulse was captured. `P7` is red because the reads in lines 0 to 19 after a
field with no pulse are not one value, and `P7(c)` is not evaluated because
`P7(b)` failed for every field with a population (4 of 4, in OCS/PAL and in OCS/NTSC). The ECS runs print it outright — *"every phase-B sample
measured current (no pen capture, no end-of-field latch)"* — and the RTL above
reads the counter the same way in both chipsets. No choice of port, pin,
polarity or pulse width reaches a counter that has no latch in front of it.
The page-model control replaces 38279 and 31600 of the 90000 phase-B samples
on fx68k and 41427 and 34083 on TG68K (PAL and NTSC) with the values p. 238
and p. 239 predict, and the same `P1` to `P13` code grades those green.

### Where else the suite meets it

Five more packages read VPOSR and VHPOSR with BPLCON0 bit 3 set, each by a method of its own, and in all five the reads are not held.
The same lines are red on fx68k and on TG68K. `LP-P1B-NO-LATCH-WHEN-CLEAR`, `P3-LIVE` and `P3-PRE-LIVE`, which read with bit 3 clear or
before a trigger, are green in every configuration on both cores.

**`integration/appDE-ports-disk`, one connector at a time.** [`integration/appDE-ports-disk`](../../integration/appDE-ports-disk) reads VPOSR:VHPOSR as
long words, first with bit 3 clear to fit the beam model, then with BPLCON0 = `$0008` for ten fields, F1 to F10. The harness pulses pin 6
of one connector low for 4 colour clocks at scheduled positions in F2 to F7 and F9, leaves F1, F8 and F10 without a pulse, and runs each
schedule on the left connector (J12) and on the right (J11). In ECS the program first writes BEAMCON0 with bit 13 clear
(`LP-C6-LPENDIS-CLEAR`). Thirteen lines are red in each of OCS/PAL, OCS/NTSC, ECS/PAL and ECS/NTSC, 52 in a run of `230 passed, 56 failed`. In ECS/PAL on J11 the first read
after each pulse of F2 to F7 is on the pulse's line, V = 100, 100, 100, 101, 258 and 60, but at H = 82, 88, 94, 82, 82 and 73 on fx68k
(86, 88, 90, 86, 86 and 76 on TG68K) where the targets are 64, 65, 72, 64, 64 and 64:

```text
MEASURED: ECS/PAL J11 P1-P4 measured: P1=does not hold, P2(a)=holds, P2(c)=does not hold, P3(a)=does not hold, P3(b)=does not hold, P3(c)=holds, P3(d)=does not hold, P4=does not hold
```

Over the eight runs of each core the first read after a pulse lies 7 to 24 colour clocks past the pulse's position on fx68k and 8 to 26 on TG68K.
`P2(a)`, the line alone, holds and `P2(c)`, the horizontal offsets following the pulses, does not
(`test_appDE_ports_disk.cpp:1224-1225`; `LP-P2-LATCH-AT-PULSE`, `:1273-1274`). The reds that follow grade what p. 239 says a latch does:
`LP-P1A-LATCH-HOLDS`, that a pulse freezes the reads and so does the end of a field without one (`test_appDE_ports_disk.cpp:1283`);
`LP-P3A-HOLD`, that every read after a trigger until the end of the following vertical blanking has the latched position (`:1285`);
`LP-P3B-HELD-PULSE-IGNORED`, that F7's second pulse, on line 180, changes nothing (`:1286`);
`LP-P3D-PULSE-BEFORE-ENABLE`, that F9's pulse, on line 10, is not captured because the position of the end of F8 is still held (`:1291`);
`LP-P4-DETERMINE-TRIGGER`, that p. 239's two ways of telling a trigger from a field end give the right answer (`:1292`).
`LP-P1-LATCH-ENABLE` and `LP-LATCH-ENABLE-AND-POSITION` combine `LP-P1A-LATCH-HOLDS` with the bit-clear run (`:1295`, `:1299`).
With bit 3 clear and the same pulses `LP-P1B-NO-LATCH-WHEN-CLEAR` is green in all eight runs:
every read equals the beam model (`test_appDE_ports_disk.cpp:1250`). The beam model is anchored and every pulse is on pin 6 of the
connector under test, 4 colour clocks wide, at its scheduled cycle, in every run (`LP-C3-A1-READS-CURRENT`, `LP-C3-A3-FIELD-LENGTH`,
`LP-C3-A4-FIELD-ANCHOR`, `LP-C5-PULSES`, green). `LP-P3C-RELEASE` is recorded not graded in all eight, because nothing was held. The four reds besides
the thirteen per configuration are `LP-A2-LINE-LENGTHS` in NTSC on both connectors, which grades the NTSC line length,
[NM-HRM-010](NM-HRM-010.md)'s subject.

**`integration/ch7-system-control`, both ports, Copper-marked reads.** [`integration/ch7-system-control`](../../integration/ch7-system-control) sets BPLCON0 = `$0008`
and lets a Copper list raise COPER at lines 40, 60, 110, 130, 190 and 210 (E40 to E210). At each event the CPU reads VPOSR as a long
word and VHPOSR as a word, and about 2 and 10 lines after each VERTB it reads twice more (V1 and V2), timed by counted polls of
INTREQR, never by the register under test. The harness drives pin 6 of both ports for 4 colour clocks on line 100 of F1 and F3, at
horizontal positions 40 colour clocks apart, and on line 180 of F5. In ECS the program first writes BEAMCON0 with bit 13 clear. Four lines are red in each of
OCS-PAL, ECS-PAL, OCS-NTSC and ECS-NTSC, and two more in ECS-PAL: 18 red, the package's only reds (`115 passed, 18 failed`); in OCS-PAL on fx68k:

```text
MEASURED: P3 OCS-PAL item2: V(L1)=110 V(L3)=110 V(L5)=190 H(L3)-H(L1)=6
FAIL: P3-PEN-POSITION OCS-PAL [p.238,p.239]: the latched value is the light-pen position -- V(L1)=100, V(L3)=100, V(L5)=180, H(L3)-H(L1)=40
```

The reads after the pulses return their own lines, 110 and 190, not the pulses' 100 and 180 (`P3-PEN-POSITION`,
`test_ch7_system_control.cpp:1591-1593`), and the reads that follow change (`P3-HELD`, `test_ch7_system_control.cpp:1575-1581`;
`P3-LATCHED-PEN-POSITION`, `:1595-1598`). On TG68K L1, L3 and L5 read the same lines, and H(L3) - H(L1) is -4 in PAL, and 2 (OCS) and
-20 (ECS) in NTSC, where fx68k reads 6 and 16; the page's 40 is in none. After a field with no pulse V1 and V2 read lines 2 and 11
on fx68k (2 and 10 on TG68K), two different values below $10500, `$0029B` and `$00B1F` in F1 on fx68k, where `P3-FIELD-END-LATCH` expects
them to agree, at an M greater than $10500 in PAL and not less than it in NTSC (`P3-FIELD-END-LATCH`, `test_ch7_system_control.cpp:1614-1617`). In ECS-PAL `P4` sets LPENDIS, reads X near line 270 of F1 after a pulse
on line 100, clears LPENDIS and reads Y; in F3 it reads X' and Y' with LPENDIS clear throughout. Its two preconditions that need a
latch are red:

```text
FAIL: P4-PRE-LATCHF1 ECS-PAL [p.239]: the latch is present in F1 -- V(Y)=100 and H(Y)=H(Y')
FAIL: P4-PRE-CONTROL ECS-PAL [p.239]: F3 (LPENDIS clear throughout) shows X'=Y', V(X')=100
```

They measure V(Y) = 270 with H(Y) = 141 and H(Y') = 145 on fx68k (124 and 128 on TG68K), and V(X') = V(Y') = 270 with X' different from Y' (`P4-PRE-LATCHF1`,
`test_ch7_system_control.cpp:1836-1838`; `P4-PRE-CONTROL`, `:1846-1848`). With nothing latched there is nothing for LPENDIS to hide, so
`P4-LPENDIS-READ`, p. 305's bit, is recorded not graded (`P4-LPENDIS-READ`, `test_ch7_system_control.cpp:1859`).

**`integration/ch3-playfield-system`, bit 3 alone, no pen.** [`integration/ch3-playfield-system`](../../integration/ch3-playfield-system) drives no pulse. Two programs,
identical but for BPLCON0, `$0008` (L1) and `$0000` (L0), run with no DMA; after each of five VERTB detections each reads
VPOSR:VHPOSR 600 times as long words. The windows after detections 2 to 5 run from one line after the VERTB marker to about 18 lines
after it, before the vertical position 20 at which p. 239 releases the freeze, and p. 239's field-end latch, which needs no pen, predicts one frozen value in
every L1 window. In ECS the program first writes BEAMCON0 with bit 13 clear. `P3-MAIN` is red in ECS/PAL, ECS/NTSC, OCS/PAL and
OCS/NTSC on both cores (`P3-MAIN`, `test_ch3_playfield_system.cpp:1266-1267`), and so is the `P3 aggregate`, which joins it to `P3-LIVE`: 5 red in a run of `223 passed, 10 failed`, the other five being [NM-HRM-090](NM-HRM-090.md)'s
(`P3 aggregate`, `test_ch3_playfield_system.cpp:1277-1281`):

```text
FAIL: P3-MAIN S1 (ECS/PAL): with LPEN=1, every counted VPOSR/VHPOSR read in the windows after VERTB detections 2-5 shows the same 17-bit value (frozen)
```

`P3-LIVE`, the L0 windows, is green in all four: no two consecutive reads are equal (`P3-LIVE`, `test_ch3_playfield_system.cpp:1207-1210`). `P4-MAIN`, p. 271's reset value, waits for
`P3-MAIN` and prints `RECORDED: P4-MAIN S1: NOT GRADED, P3-MAIN did not pass in this configuration, so a set LPEN is not shown to latch
here and an LPEN of 1 would look like 0` in all four; the `P4 aggregate` is recorded, not graded.

**`integration/aa-integ-s1`, the AA chip set.** [`integration/aa-integ-s1`](../../integration/aa-integ-s1) selects the AA chip set and writes BEAMCON0 = `$0020`
(PAL, LPENDIS low). An LPEN run writes BPLCON0 = `$0008`; three times it waits for VERTB, stores a marker, reads VPOSR:VHPOSR as a long
word (w0), waits at least 40 colour clocks and reads again (w1). Pin 6 of both ports stays at its open-switch level (`P10-FIRE`, green), so no trigger
occurs, and p. 239's field-end latch, with AA p. 12's LPENDIS low, predicts w1 = w0 in every pair. `P10-LATCH` is red on both cores
(`P10-LATCH`, `test_aa_integ_s1.cpp:1696`), 1 red in a run of `156 passed, 10 failed, 11 NOT GRADED`, the other nine grading BPLCON1's reset value, BYPASS and the DUAL counter; in pair 0 of the LPEN run:

```text
MEASURED: P10 main pair 0: w0=$A3000021 (V=0) w1=$A300006E; LPEN run: w0=$A3000029 w1=$A3000076
```

(fx68k; on TG68K the LPEN run reads w0 = `$A3000022` and w1 = `$A3000078` in every pair.) `P10-WIN`, which requires fewer than 19 line boundaries between the marker and the store of w1, is
green. The check that LPEN is off after power-up, which needs a latch to be told from a counter that latches nothing, prints `RECORDED: NOT GRADED P10 main check`.

**`integration/aa-integ-s2`, the AA chip set, a pulse on both ports.** [`integration/aa-integ-s2`](../../integration/aa-integ-s2) selects the AA chip set, waits for line 254, writes BPLCON0 = `$0008`
and then reads, once every 45 colour clocks, CIA-A's port A, VPOSR:VHPOSR as a long word and HHPOSR (`prog_lpen.s`). The harness pulls pin 6 of both ports low
on line 255 for one sample interval and two colour clocks. The program's own reads of PRA bits 7 and 6 are low during the pulse and high before and after it (`P8-TRIGGER`, green), and up to the pulse
the samples match those of a run with no pulse (`P8-ALIGN`, green). With BEAMCON0 = `$0020`, LPENDIS low, AA p. 12 and p. 239 predict one held position on line 255 in every sample after the pulse.
`P8-0-VH` is red on both cores (`P8-0-VH`, `test_aa_integ_s2.cpp:2702`), and so is `P8`, which joins it to the LPENDIS-high run (`P8`, `test_aa_integ_s2.cpp:2736-2738`):
2 red in a run of `131 passed, 13 failed`, the other eleven being [NM-HRM-032](NM-HRM-032.md)'s, [NM-AA-009](NM-AA-009.md)'s, [NM-AA-008](NM-AA-008.md)'s, [NM-AA-010](NM-AA-010.md)'s, [NM-AA-005](NM-AA-005.md)'s and [NM-HRM-010](NM-HRM-010.md)'s. On fx68k:

```text
MEASURED: P8 measured: P8-0 first sample after the pulse: V=255 H=195 HHPOSR=0; last: V=258 H=99 HHPOSR=0
```

(on TG68K the first sample after the pulse is V=255, H=215 and the last V=258, H=119.) With BEAMCON0 = `$2020`, LPENDIS high, the counters are to run, and `P8-1-VH` is green; so is `P8-LPE`,
the same pulse with bit 3 clear. HHPOSR does not run in the run with no pulse (`P8-HHRUN` prints NOT CHECKABLE), so `P8-0-HH` and `P8-1-HH` print NOT GRADED, and `P8-VBREL`, the release at the end of vertical blanking, is not graded because nothing froze.

## Risks and unknowns

- **Half of the implementations agree with NanoMig.** vAmiga and Denise have no
  light-pen latch either. The two that have one, WinUAE and Copperline, both
  latch at the end of a field with bit 3 set, which NanoMig does not, and
  neither triggers from a joystick's fire line. The finding rests on pp. 238
  and 239, not on any implementation.
- **The ECS clause of the verdict is read, not run.** That the same split holds in
  ECS and in the AA chip set with BEAMCON0 bit 13 clear, and that with it set WinUAE hides the latch
  from the read and Copperline refuses a pulse but keeps the field-end latch, is
  read from their code, not run.
- **Hardware is not measured.** Does bit 3 alone freeze the counters? p. 239
  says a field with no trigger latches at its end and stays latched until
  vertical blanking ends. On real hardware, with nothing in either port: set
  BPLCON0 bit 3 and read VPOSR twice anywhere in lines 0 to 19. Equal reads at
  the last line's position settle it.
- **ECS is recorded, not graded, where BEAMCON0 is never written.** p. 305 gives LPENDIS no reset value, and neither does AA
  p. 12. It decides whether an ECS or AA machine that never writes BEAMCON0 shows the latch at
  all; the packages that grade ECS and AA write it clear and do not depend on it. NanoMig's own BEAMCON0 resets bit 13 to 0, which on NanoMig's own terms
  would leave the latched value visible, if there were one.
- **p. 305's LPENDIS read is not graded on NanoMig.** `P4-LPENDIS-READ` in `integration/ch7-system-control` needs a latch to hide;
  with none its two preconditions, `P4-PRE-LATCHF1` and `P4-PRE-CONTROL`, are red and it is recorded not graded.
- **What NanoMig outputs.** [NanoMig's README](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/README.md#L22) lists "HDMI video and audio, PAL and NTSC", and its builds for the Tang Nano 20k are HDMI and [RGB LCD](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/README.md#L85). The pen-less half of the finding, the field-end latch with bit 3 set (`P7`, `P3-MAIN`), needs no pen.
- **Which port.** p. 238 and the technical reference put the pen on port 2's
  pin 6 on the A500, and the B2000's J200 moves it to p. 238's port 1. NanoMig
  would have to route one of its two fire lines to Agnus as well as to CIA-A.
- **When the latch is released.** p. 239 says vertical position 20, p. 219
  ends vertical blanking at line 20 in NTSC and 25 in PAL, and p. 79's table
  stops it at 21 and 29. NanoMig's hardwired end of vertical blanking is
  line 25 in PAL and 20 in NTSC, WinUAE releases at the same lines, and
  Copperline does not release at all until the next latch.
  `integration/ch8-light-pen-beam` grades the release at line 20 in both standards (`P8(release-at-20)`, `P9(boundary)` and `P9(release)`).

## Independent evidence

### What the others say

The question put to each: with BPLCON0 bit 3 set, what freezes VPOSR and
VHPOSR, from which input, and when is it released?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:5438-5460`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L5438-L5460), [`:5352-5358`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L5352-L5358), [`:2273-2278`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2273-L2278), [`:2311-2314`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2311-L2314), [`:2383-2386`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2383-L2386), [`:2503-2506`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2503-L2506), [`:10015-10020`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10015-L10020), [`:10088-10090`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10088-L10090), [`:10112-10114`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10112-L10114), [`:3385-3387`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3385-L3387), [`:3527-3537`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3527-L3537), [`drawing.cpp:1944-1945`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L1944-L1945) | <pre>if (!lightpen_triggered &amp;&amp; (bplcon0 &amp; 8)) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (agnus_vb_start_line) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;vpos_lpen = vpos;<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;} else if (lightpen_enabled &amp;&amp; !agnus_vb_active) {</pre> **latches on a pen's pulse and, with none, at the end of the field, and releases at the end of vertical blanking.** While bit 3 is set and nothing is latched, the start of the field's last line latches the counters, `agnus_vb_start_line` being set on `vpos == maxvpos + lof_store - 1` for every Agnus but the A1000's; outside vertical blank, a pen whose position maps to the current line schedules `lightpen_trigger_func` at its horizontal position, which stores `vpos_lpen = vpos;` and `lightpen_triggered = 1;`. VPOSR and VHPOSR read through `GETVPOS`, `return islightpentriggered() ? vpos_lpen : vpos;`. The latch is cleared at the end of vertical blanking, `if (agnus_vb_active_end_line) {` / `lightpen_triggered = 0;`, which is `if (vpos == 25) {` in PAL and `if (vpos == 20) {` in NTSC. The pen's position is the host pointer's, `lightpen_cx[lpnum] = out ? -1 : cx;`, not the level of a port's pin 6. BEAMCON0 is written only on ECS, `if (ecs_agnus) {`, and there bit 13 makes the reads ignore the latch, `if (beamcon0 & BEAMCON0_LPENDIS)`. Setting bit 3 inside vertical blank latches at once, `if ((va & 8) && !lightpen_triggered && agnus_vb_active) {`, and clearing it releases |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:219-255`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L219-L255), [`:168-198`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L168-L198), [`:353-354`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L353-L354), [`:549-558`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L549-L558), [`Core/Ports/ControlPortTypes.h:20-25`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPortTypes.h#L20-L25) | <pre>if (ersy(bplcon0Initial)) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;result \|= latchedPos.v &gt;&gt; 8;<br><br>} else {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;auto pos = agnus.pos + 5;</pre> **no light-pen latch.** `peekVPOSR` and `peekVHPOSR` return the running counter, five cycles ahead, and a held position only while ERSY, BPLCON0 bit 1, is set, `if ((newValue & 0b10) && !(oldValue & 0b10)) latchedPos = pos;`. Bit 3 is read nowhere. `pokeBEAMCON0` returns at once on OCS, `if (agnus.isOCS()) return;`, and on ECS acts on PAL and LOLDIS; LPENDIS appears only in its comment, `// 13: LPENDIS`. The control-port devices are none, a mouse and a joystick |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:710-747`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L710-L747), [`:2`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L2), [`emulation/libami/agnus/register.cpp:448-453`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L448-L453), [`emulation/libami/interface.cpp:115-161`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L115-L161), [`:306`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L306), [`:367`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L367) | <pre>if (ERSY) {<br>&nbsp;&nbsp;&nbsp;&nbsp;v = vPosLocked;<br>&nbsp;&nbsp;&nbsp;&nbsp;if (hPosLocked) h = 0;<br>}</pre> **no light-pen latch.** `POSR`, which serves both VPOSR and VHPOSR, returns the running counter unless ERSY is set, `#define ERSY (bplCon0 & 2)`, whose rising write stores `vPosLocked = vPos;`. BPLCON0 is kept with bit 3 in it, `bplCon0 = value & (ecsAndHigher() ? ~0xb1 : ~0xf1);`, and nothing reads that bit. The Amiga core offers a joypad, a mouse, a keyboard, a four-player adapter and a CD32 pad as port devices, and no light pen |
| **Copperline** | [`src/chipset/agnus.rs:1253-1262`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1253-L1262), [`:1098-1110`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1098-L1110), [`:1154-1167`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1154-L1167), [`:1221-1244`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1221-L1244), [`src/bus/custom_regs.rs:1081-1083`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1081-L1083), [`src/bus.rs:5037-5061`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5037-L5061), test [`src/chipset/agnus.rs:2482-2486`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L2482-L2486) | <pre>fn trigger_light_pen_at(&amp;mut self, vpos: u32, hpos: u32) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if !self.lpen_enabled \|\| self.lpen_latch_disabled() \|\| self.lpen_triggered_this_field {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return;<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>&nbsp;&nbsp;&nbsp;&nbsp;self.lpen_latch = Some((vpos, hpos));</pre> **latches on a pulse and at the end of a field, and does not release at the end of vertical blanking.** Agnus takes bit 3 from every BPLCON0 write, `self.agnus.set_lpen(val & 0x0008 != 0);`, and while it is set `read_vposr` and `read_vhposr` return the latch, `(Some((v, _)), _) => (v, self.lof, self.lol),`. The first pulse of a field latches the beam position; at the field wrap a field with no pulse latches `Some((previous_vpos, line_cck.saturating_sub(1)))`, and `self.lpen_triggered_this_field = false;` re-arms it. Nothing clears the latch but a change of bit 3, `self.lpen_latch = None;`, so it is held into the next field until that field's first pulse or its end: its own test lifts the pen after a latch at line 60, `agnus.set_light_pen_target(None);`, advances one whole field and still reads it, `assert_eq!(agnus.read_vhposr(), (60 << 8) \| 0x10);`. The pulse comes from a light-pen device on the port whose pin 6 is wired to Agnus, port 2 on the A500 and later and port 1 on the A1000 (`light_pen_wired_port`), fired as the beam passes the pen's screen position; a joystick's fire button does not reach it. In ECS, bit 13 of BEAMCON0 refuses the pulse, `self.revision.is_ecs() && self.beamcon0 & BEAMCON0_LPENDIS != 0` |
| **MiSTer** | [`rtl/agnus_beamcounter.v:113-121`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121), [`:135`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L135), [`rtl/denise.v:154`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L154) | descendant — the same read path, from a copy of the counter two `clk7_en` edges late: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos_rb[10:8]};`, and VHPOSR with `hpos[8:1] - 8'd1`. `//wire lpendis      = beamcon0_reg[13];` and `//assign lpen     = bplcon0[3];` are comments; in NanoMig the first is a comment too, and bit 3 is assigned to a wire that nothing reads, as in TC64. No latch |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:129-136`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L129-L136), [`:147-152`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L147-L152), [`:160`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L160), [`:241`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L241), [`:258`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L258), [`rtl/minimig/denise.v:161`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L161) | descendant — NanoMig's read path, line for line: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};`. BEAMCON0 bit 13 is used, but not for a light pen: a write with it or bit 6 set goes to a shadow register for the RTG display, `if(!rd && (data_in[13] \|\| data_in[6]))`, and `wire lpendis      = beamcon0_sh[13];` then chooses between the normal and the shadow timing registers. Denise assigns `assign lpen     = bplcon0[3];` and nothing reads it. No latch, in either line |

**Two of the four latch, and two do not.** WinUAE and Copperline both freeze
VPOSR and VHPOSR with bit 3 set: on a pulse, and at the end of a field that had
none, which is the clause `P7` grades and the one that needs no pen. WinUAE
releases where p. 239 does, at the end of vertical blanking; Copperline keeps
the value into the next field. vAmiga and Denise have no light pen at all and,
like NanoMig, read the running counter whatever bit 3 holds. **None of the four
fires the latch from a joystick's fire line**: WinUAE and Copperline take the
trigger from a pen positioned on the screen, which is how an emulator models a
pen, not from the level of pin 6. MiSTer and TC64, descendants, have NanoMig's
read path and no latch, so the finding is inherited and fixed in neither line.

**With BEAMCON0 bit 13 clear the same split holds in ECS.** WinUAE's Agnus is ECS when its chipset mask has
the ECS Agnus bit
([`custom.cpp:1187`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1187)); Copperline
counts every revision but OCS as ECS, `!matches!(self, Self::Ocs)`
([`agnus.rs:105-107`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L105-L107)).
They part on what bit 13 does when it is set. WinUAE hides the latch from the read, `if (beamcon0 & BEAMCON0_LPENDIS)` / `return 0;`
in `islightpentriggered`
([`custom.cpp:2273-2278`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2273-L2278)),
and still takes it. Copperline refuses a pulse,
`if !self.lpen_enabled || self.lpen_latch_disabled() || self.lpen_triggered_this_field {`
([`:1256-1262`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1256-L1262)),
but takes the field-end latch and returns it on a read without looking at the bit
([`:1106-1108`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1106-L1108),
[`:1223-1229`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1223-L1229)).
vAmiga and Denise have no latch in any chip set.

**The AA chip set is ECS to both of the two that latch.** WinUAE's full AGA configuration sets the ECS Agnus bit of its chipset mask
([`cfgfile.cpp:3519`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L3519)), and Copperline
counts Alice as ECS, `!matches!(self, Self::Ocs)`
([`agnus.rs:105-107`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L105-L107)),
so with BEAMCON0 bit 13 clear both latch there as in ECS.

### What other Commodore documents say

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF pages 5 and 25 — agrees with the manual:

  > "Active low. This input is used to indicate when the light pen is coincident with the monitor beam."
  <!-- corpus: agnus-8371-spec/p005.txt -->

  > "LPEN=Light pen enable (reset on power up)"
  <!-- corpus: agnus-8371-spec/p025.txt -->

  The pin table gives Agnus an input for the pen, LP*, pin 78, and the register table gives BPLCON0 bit 3 to it as p. 271 does. The 8371 is the OCS Fat Agnus, the PAL part.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 106, 12 and 233 — agrees with the manual:

  > "The Agnus chip latches the raster position that was in effect when the /LPEN signal goes low, so an application can follow the position of a light pen on the screen."
  <!-- corpus: a500-a2000-trm-1987/p106.txt -->

  > "The light pen input on the A500 and B2000 has been moved to the second mouse port to allow use without a pass-thru mouse adapter."
  <!-- corpus: a500-a2000-trm-1987/p012.txt -->

  > "FIRE input of mouse/joystick port 1, as with the A500."
  <!-- corpus: a500-a2000-trm-1987/p233.txt -->

  The first is the B2000 video slot's /LPEN pin, *"an input to the Agnus light pen input"*: Agnus latches the raster position on its falling edge. The other two place the pen as p. 238 does. The technical reference numbers the ports from 0, so its "second mouse port" and "port 1" are p. 238's port 2, and jumper J200's other position selects port 0, *"the scheme used on the A1000 machine"*. The line is the port's FIRE input, pin 6 of Table 8-1.

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 18 — agrees with the manual:

  > "A light pen input signal 33 is fed into the light pen register circuitry 119"
  <!-- corpus: patent-us4874164-agnus-denise/p018.txt -->

  > "can cause the beam count when the beam passed the light pen to be loaded onto the data bus 404 under the control of the register address decoder 27."
  <!-- corpus: patent-us4874164-agnus-denise/p018.txt -->

  The light-pen registers sit beside the sync counters in the address generator, Agnus; the patent's FIG. 3 is their block diagram. It gives the capture, not the field-end latch or the release. 5,103,499 carries the same specification and is not counted again.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 5 — agrees with the manual:

  > "y= register bits contained in AGNUS, not defined here."
  <!-- corpus: denise-spec-252126/p005.txt -->

  BPLCON0 bit 3 is one of the bits marked y: the light-pen enable is Agnus's, the chip that has the 8371's LP* pin and, in the patent, the light-pen registers. In NanoMig, Agnus does not store it.

## Decision

**One option: give Agnus a light-pen line and the latch p. 239 describes.**
The four changes, without LPENDIS:

- **The line.** An input on `agnus` and `agnus_beamcounter`, driven in
  `nanomig.v` or `minimig.v` from port 2's pin 6, the A500's default on
  p. 238 and in the technical reference, which is `_fire1`, CIA-A's PA7
  input. A jumper-like choice of `_fire0` would give the A1000's port 1. The
  change takes the AND of `_fire0` and `_fire1`, so a pulse on either port
  captures.
- **The enable.** Store BPLCON0 bit 3 in `agnus_beamcounter` beside ERSY and
  LACE (lines 144–163), reset to 0 as p. 271 says.
- **The latch.** With bit 3 set and nothing latched, a falling edge of the
  line outside vertical blanking stores `vpos` and `hpos`; with no edge in the
  field, the end of the field stores them (`eof`, line 377). The end of
  vertical blanking releases the latch: the module's `vblend`, line 447, is
  line 25 in PAL and 20 in NTSC, where p. 239 says line 20 for both. The change
  releases when the counter enters line 20, in PAL and in NTSC.
- **The read.** While latched, the multiplexer of lines 108–115 returns the
  stored `vpos[10:8]` and `{vpos[7:0],hpos[8:1]}` instead of the counters; in
  ECS, with BEAMCON0 bit 13 set, VPOSR returns the counter (p. 305: *"on
  vertical pos read"*; the page does not say whether VHPOSR does too; for the AA chip set AA p. 12 says that with the bit high VHPOSR, VPOSR and HHPOSR all return the beam counter). `integration/ch7-system-control`'s `P4-LPENDIS-READ` grades the bit in ECS
  once its preconditions, which need a latch, hold.

```diff
-	data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
+	data_out[15:0] = lp_latched ? lp_vposr : live_vposr;   // live_* = the old right-hand sides
-	data_out[15:0] = {vpos[7:0],hpos[8:1]};
+	data_out[15:0] = lp_latched ? lp_vhposr : live_vhposr;
+	// latch on BPLCON0[3] and a falling edge of _lpen after line 20, else at end_of_frame:
+	lp_latched <= 1; lp_vposr <= live_vposr; lp_vhposr <= live_vhposr;
+	._lpen(_fire0 & _fire1)   // minimig.v, into agnus and agnus_beamcounter
```

This diff is abridged: it leaves out the enable register, the release at line 20 and the new port declarations.

There is no fix to port: MiSTer and TC64 read the running counter the same
way, and neither line's `minimig.v` has a light-pen line into Agnus any more
than NanoMig's `nanomig.v` does.

What it would change:

- the change addresses the nineteen red lines on fx68k and on TG68K: `make -k -C integration/ch8-light-pen-beam latch-test` builds the package against the RTL with `latch.patch`, the four changes above without LPENDIS, and prints `196 passed, 0 failed` on each core, with `P1` to `P12` recorded, not graded, in ECS/PAL and ECS/NTSC; `late-test`, with `late.patch` releasing and arming at line 21 in place of line 20, prints `182 passed, 14 failed` on each core: `P8`, `P8(release-at-20)`, `P9(boundary)`, `P9(release)` and `P11` in OCS/PAL and OCS/NTSC, and the page-model line `P8(release-at-20)` in the four configurations;
- a program that polls VPOSR with bit 3 set would see it frozen, as p. 239
  warns, and a joystick's fire button held on port 2 would latch the counters,
  as the technical reference's *"FIRE input"* implies for the A500.
