# NM-HRM-087 — with BPLCON0 bit 3 set, VPOSR and VHPOSR keep counting: neither a light-pen trigger nor the end of a field freezes them, where pp. 238 and 239 do both [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L107-L115) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's reading is accepted. NanoMig has the VPOSR/VHPOSR read path from there, and TC64 reads the running counter the same way, so there is no fix to port. A fix also needs a light-pen line into Agnus, which neither line's `minimig.v` nor NanoMig's `nanomig.v` has — see *What NanoMig does* |
| **Test** | [`integration/ch8-light-pen-beam`](../../integration/ch8-light-pen-beam) — one program, graded in OCS/PAL and OCS/NTSC on both CPU cores. Nineteen lines are red on fx68k and on TG68K: `P1`, `P2`, `P2(ii)`, `P3`, `P3(a)`, `P3(b)`, `P3(c)`, `P4`, `P5`, `P6`, `P7`, `P8`, `P8(release-at-20)`, `P9`, `P9(boundary)`, `P9(release)`, `P10(a)`, `P11` and `P12` (pp. 227, 228, 229, 238, 239; claims in *Measured*). On a copy of the RTL with the pp. 238-239 latch added, every one of the 240 lines of the run passes on fx68k and on TG68K (240 passed, 0 failed on each core), the nineteen included |
| **Chapter** | 8 — Interface Hardware, printed pp. 227, 228, 229, 232, 238, 239; 7 — System Control, printed pp. 216, 219; 3 — Playfield Hardware, printed p. 79; Appendix A, printed p. 271; Appendix C, printed pp. 295, 298, 300, 305; Appendix E, printed p. 333 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 19 red on fx68k and 19 on TG68K in each of OCS/PAL and OCS/NTSC (156 passed, 38 failed on each core). In ECS/PAL and ECS/NTSC the same nineteen and `P2(i)` and `P9(a)` are recorded, not graded, because p. 305 gives LPENDIS no reset value. In the same run the page-model control, the page's latch applied to the run's own measured samples and graded by the same code, is green in all four configurations on both cores (25 passed, 0 failed each), and `P10(b)`, bit 3 clear, is green |
| **Verdict of the four** | **split, two and two**: WinUAE and Copperline latch VPOSR and VHPOSR in OCS while BPLCON0 bit 3 is set, on a pulse from their light-pen device and, with none, at the end of the field; WinUAE releases the latch at the end of vertical blanking, Copperline holds it into the next field. vAmiga and Denise have no light-pen latch and freeze the counters only for ERSY. None of the four takes the trigger from a joystick's fire line |

---

## What the HRM says

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
say. That is why the package grades the latch in OCS and records it in ECS.

---

## What NanoMig does

**VPOSR and VHPOSR always read the running counter.** The read multiplexer has
no other source —
[`src/minimig-aga/agnus_beamcounter.v#L107-L115`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L107-L115):

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
([`#L325-L337`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L325-L337)).

**Agnus does not keep BPLCON0 bit 3.** The beam counter takes bit 1 and bit 2
of a BPLCON0 write and nothing else —
[`#L144-L163`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L144-L163),
`ersy <= data_in[1];` and `lace <= data_in[2];` — and the bitplane sequencer
takes bits 6, 15, 4 and 14–12 —
[`src/minimig-aga/agnus_bitplanedma.v#L282`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L282).
Denise stores the whole register, and its light-pen wire is commented out —
[`src/minimig-aga/denise.v#L166`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L166),
[`#L189`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L189):

```verilog
//wire          lpen;       // light pen enable
...
//assign lpen     = bplcon0[3];
```

**LPENDIS is stored in ECS and read nowhere.** BEAMCON0 is written only with
`ecs` set and resets to `{10'b0, ~ntsc, 5'b0}`, bit 13 clear; the wire that
would name the bit is a comment —
[`#L117-L129`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L117-L129):

```verilog
wire harddis      = beamcon0_reg[14];
//wire lpendis      = beamcon0_reg[13];
```

**No light-pen line reaches Agnus.** Neither `agnus`
([`agnus.v#L54-L100`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L54-L100))
nor `agnus_beamcounter`
([`#L27-L57`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L27-L57))
has an input for one, and no port of `nanomig.v` is one. Pin 6 of each
controller port goes to CIA-A and nowhere else.

### The pins

`nanomig.v` passes its two joystick bytes, inverted, to Minimig's `_joy1` and
`_joy2` —
[`src/nanomig.v#L1503-L1504`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1503-L1504):

```verilog
	._joy1        (~JOY0            ), // joystick 1 [fire4,fire3,fire2,fire,up,down,left,right] (default mouse port)
	._joy2        (~JOY1            ), // joystick 2 [fire4,fire3,fire2,fire,up,down,left,right] (default joystick port)
```

`userio` crosses them over, `_sjoy1 <= joy_swap ? joy1 : joy2;` with
`joy_swap` tied to 0
([`src/minimig-aga/userio.v#L215-L218`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L215-L218),
[`#L92`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L92)),
and makes the two fire lines from bit 4, the first also from mouse button 0 —
[`#L313-L314`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L313-L314):

```verilog
assign _fire0 = cd32pad && !cd32pad1_reg_load ? fire1_d : _sjoy1[4] & _mleft;
assign _fire1 = cd32pad && !cd32pad2_reg_load ? fire2_d : _sjoy2[4];
```

Both lines end at CIA-A's port A, `_fire1` on PA7 and `_fire0` on PA6 —
[`src/minimig-aga/minimig.v#L622`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L622),
`.porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change}),`. p. 232 gives
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

| configuration | fx68k | TG68K | page-model control, both cores | a copy of the RTL with the pp. 238-239 latch added, both cores |
|---|---|---|---|---|
| OCS/PAL | 15 passed, **19 failed** | 15 passed, **19 failed** | 25 passed, 0 failed | 35 passed, 0 failed; the control 25 passed, 0 failed |
| OCS/NTSC | 15 passed, **19 failed** | 15 passed, **19 failed** | 25 passed, 0 failed | 35 passed, 0 failed; the control 25 passed, 0 failed |
| ECS/PAL | 13 passed, 0 failed; the 21 recorded | the same | 25 passed, 0 failed | 35 passed, 0 failed; the control 25 passed, 0 failed |
| ECS/NTSC | 13 passed, 0 failed; the 21 recorded | the same | 25 passed, 0 failed | 35 passed, 0 failed; the control 25 passed, 0 failed |

The nineteen reds, the same in both OCS configurations and on both cores. A
line without a claim id carries none because it is a component of a combined
assertion, or because no claim is established by it alone:

| assertion | claims | page | what it grades |
|---|---|---|---|
| `P1` | `opus:8-lpen.c1`, `sol:8-pen.c1`, `opus:8-intro.c12`, `sol:8-overview.c6` | 238, 227 | a pin-6 pulse on at least one port is captured, frozen and not current, at the position `P3(a)` and `P3(b)` require; recorded: port 1 *captured=no*, port 2 *captured=no* |
| `P2` | `opus:8-lpen.c16`, `sol:8-pen.c17`, `opus:8-tbl8-1.c20`, `sol:8-pin.c22` | 238, 228 | pin 6 captures and pins 1–4, 5 and 9 do not |
| `P2(ii)` | none | 238, 228 | the pin-6 pulse of field B18 is captured; its sibling `P2(i)`, the other pins' pulses are not captured, is green |
| `P3` | `opus:8-lpen.c17`, `opus:8-lpen.c18`, `sol:8-pen.c18`, `sol:8-pen.c19` | 238 | the captured position is the beam's at the pulse, exactly: V, H within a window, and one offset for every pulse |
| `P3(a)` | `sol:8-pen.c20` | 238 | the captured V is the model line at the pulse |
| `P3(b)`, `P3(c)` | none | 238 | the captured H lies within [H_ref(t_on), H_ref(t_on)+4+u]; the offset H − H_ref(t_on) is the same for every pulse |
| `P4` | `opus:8-regs.c18`, `opus:8-regs.c20`, `sol:8-reg.c20`, `sol:8-reg.c22` | 229, 238 | V8 in VPOSR bit 0, V7–V0 and H8–H1 in VHPOSR |
| `P5` | `opus:8-lpen.c36`, `sol:8-pen.c36` | 239 | after a trigger VPOSR is frozen until the release |
| `P6` | `opus:8-lpen.c38`, `sol:8-pen.c38` | 239 | no read in a latched window is current |
| `P7` | `opus:8-lpen.c37`, `sol:8-pen.c37` | 239 | a field with no trigger latches at its end |
| `P8` | `opus:8-lpen.c39` | 239 | every latch holds until the end of internal vertical blanking, and the counter is current after it |
| `P8(release-at-20)` | `sol:8-pen.c39` | 239 | the held value is still visible up to V = 20 and the live counter from u colour clocks after it, in each of 20 windows |
| `P9` | none | 238 | capture re-arms by itself as vertical blank ends: no hold-over, a pulse after blank is captured, in every field |
| `P9(boundary)` | none | 238 | a pulse at line 20, the stated release line, is captured |
| `P9(release)` | `opus:8-lpen.c12`, `sol:8-pen.c12` | 238 | no pulse is held over the release (`P9(a)`, green) and the pulse at line 20 is captured (`P9(boundary)`) |
| `P10(a)` | `opus:8-lpen.c35`, `sol:8-pen.c35`, `opus:8-regs.c16`, `sol:8-reg.c18` | 239, 229 | with bit 3 set, pulses are captured and field ends latch |
| `P11` | `opus:8-lpen.c43`, `sol:8-pen.c41`, `opus:8-lpen.c47`, `sol:8-pen.c44`, `opus:8-lpen.c48`, `sol:8-pen.c45`, `sol:8-pen.c46` | 239 | the two-read detection method answers correctly |
| `P12` | `opus:8-lpen.c52` | 239 | the single read during vertical blanking answers correctly |

**Nothing latches, with or without a pulse.** `P1` records that neither port's
pulse was captured. `P7` is red because the reads in lines 0 to 19 after a
field with no pulse are not one value, and `P7(c)` is not evaluated: its
population, the no-pulse fields whose next vertical blank reads one value, is
empty. The ECS runs print it outright — *"every phase-B sample
measured current (no pen capture, no end-of-field latch)"* — and the RTL above
reads the counter the same way in both chipsets. The page-model control
replaces 38279 and 31600 of the 90000 phase-B samples on fx68k and 41427 and
34083 on TG68K (PAL and NTSC) with the values p. 238 and p. 239 predict, and
the same `P1` to `P13` code grades those green.

**A copy of the RTL with the pp. 238-239 latch added** passes every line. The
copy stores BPLCON0 bit 3 in `agnus_beamcounter.v`; the counter entering line
20 releases any freeze and arms the capture; a falling edge of the AND of the
two controller ports' pin 6, while armed, stores VPOSR and VHPOSR; the end of
a field with nothing stored stores them; reads return the stored words. LPENDIS
is not modelled. The run prints 240 passed, 0 failed on fx68k and on TG68K,
which is 35 raw and 25 control lines in each of the four configurations. Its
captures have the offset d = H − H_ref(t_on) = 2 colour clocks for each of the
14 graded pulses in every configuration.

---

## What the others say

The question put to each: with BPLCON0 bit 3 set, what freezes VPOSR and
VHPOSR, from which input, and when is it released?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:5438-5460`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L5438-L5460), [`:5352-5358`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L5352-L5358), [`:2273-2278`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2273-L2278), [`:2311-2314`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2311-L2314), [`:2383-2386`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2383-L2386), [`:2503-2506`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2503-L2506), [`:10015-10020`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10015-L10020), [`:10088-10090`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10088-L10090), [`:10112-10114`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10112-L10114), [`:3385-3387`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3385-L3387), [`:3527-3537`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3527-L3537), [`drawing.cpp:1944-1945`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L1944-L1945) | <pre>if (!lightpen_triggered &amp;&amp; (bplcon0 &amp; 8)) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (agnus_vb_start_line) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;vpos_lpen = vpos;<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;} else if (lightpen_enabled &amp;&amp; !agnus_vb_active) {</pre> **latches on a pen's pulse and, with none, at the end of the field, and releases at the end of vertical blanking.** While bit 3 is set and nothing is latched, the start of the field's last line latches the counters, `agnus_vb_start_line` being set on `vpos == maxvpos + lof_store - 1` for every Agnus but the A1000's; outside vertical blank, a pen whose position maps to the current line schedules `lightpen_trigger_func` at its horizontal position, which stores `vpos_lpen = vpos;` and `lightpen_triggered = 1;`. VPOSR and VHPOSR read through `GETVPOS`, `return islightpentriggered() ? vpos_lpen : vpos;`. The latch is cleared at the end of vertical blanking, `if (agnus_vb_active_end_line) {` / `lightpen_triggered = 0;`, which is `if (vpos == 25) {` in PAL and `if (vpos == 20) {` in NTSC. The pen's position is the host pointer's, `lightpen_cx[lpnum] = out ? -1 : cx;`, not the level of a port's pin 6. BEAMCON0 is written only on ECS, `if (ecs_agnus) {`, and there bit 13 makes the reads ignore the latch, `if (beamcon0 & BEAMCON0_LPENDIS)`. Setting bit 3 inside vertical blank latches at once, `if ((va & 8) && !lightpen_triggered && agnus_vb_active) {`, and clearing it releases |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:219-255`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L219-L255), [`:168-198`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L168-L198), [`:353-354`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L353-L354), [`:549-558`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L549-L558), [`Core/Ports/ControlPortTypes.h:20-25`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPortTypes.h#L20-L25) | <pre>if (ersy(bplcon0Initial)) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;result \|= latchedPos.v &gt;&gt; 8;<br><br>} else {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;auto pos = agnus.pos + 5;</pre> **no light-pen latch.** `peekVPOSR` and `peekVHPOSR` return the running counter, five cycles ahead, and a held position only while ERSY, BPLCON0 bit 1, is set, `if ((newValue & 0b10) && !(oldValue & 0b10)) latchedPos = pos;`. Bit 3 is read nowhere. `pokeBEAMCON0` returns at once on OCS, `if (agnus.isOCS()) return;`, and on ECS acts on PAL and LOLDIS; LPENDIS appears only in its comment, `// 13: LPENDIS`. The control-port devices are none, a mouse and a joystick |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:710-747`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L710-L747), [`:2`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L2), [`emulation/libami/agnus/register.cpp:448-453`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L448-L453), [`emulation/libami/interface.cpp:115-161`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L115-L161), [`:306`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L306), [`:367`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L367) | <pre>if (ERSY) {<br>&nbsp;&nbsp;&nbsp;&nbsp;v = vPosLocked;<br>&nbsp;&nbsp;&nbsp;&nbsp;if (hPosLocked) h = 0;<br>}</pre> **no light-pen latch.** `POSR`, which serves both VPOSR and VHPOSR, returns the running counter unless ERSY is set, `#define ERSY (bplCon0 & 2)`, whose rising write stores `vPosLocked = vPos;`. BPLCON0 is kept with bit 3 in it, `bplCon0 = value & (ecsAndHigher() ? ~0xb1 : ~0xf1);`, and nothing reads that bit. The Amiga core offers a joypad, a mouse, a keyboard, a four-player adapter and a CD32 pad as port devices, and no light pen |
| **Copperline** | [`src/chipset/agnus.rs:1253-1262`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1253-L1262), [`:1098-1110`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1098-L1110), [`:1154-1167`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1154-L1167), [`:1221-1244`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1221-L1244), [`src/bus/custom_regs.rs:1081-1083`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1081-L1083), [`src/bus.rs:5037-5061`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5037-L5061), test [`src/chipset/agnus.rs:2482-2486`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L2482-L2486) | <pre>fn trigger_light_pen_at(&amp;mut self, vpos: u32, hpos: u32) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if !self.lpen_enabled \|\| self.lpen_latch_disabled() \|\| self.lpen_triggered_this_field {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return;<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>&nbsp;&nbsp;&nbsp;&nbsp;self.lpen_latch = Some((vpos, hpos));</pre> **latches on a pulse and at the end of a field, and does not release at the end of vertical blanking.** Agnus takes bit 3 from every BPLCON0 write, `self.agnus.set_lpen(val & 0x0008 != 0);`, and while it is set `read_vposr` and `read_vhposr` return the latch, `(Some((v, _)), _) => (v, self.lof, self.lol),`. The first pulse of a field latches the beam position; at the field wrap a field with no pulse latches `Some((previous_vpos, line_cck.saturating_sub(1)))`, and `self.lpen_triggered_this_field = false;` re-arms it. Nothing clears the latch but a change of bit 3, `self.lpen_latch = None;`, so it is held into the next field until that field's first pulse or its end: its own test lifts the pen after a latch at line 60, `agnus.set_light_pen_target(None);`, advances one whole field and still reads it, `assert_eq!(agnus.read_vhposr(), (60 << 8) \| 0x10);`. The pulse comes from a light-pen device on the port whose pin 6 is wired to Agnus, port 2 on the A500 and later and port 1 on the A1000 (`light_pen_wired_port`), fired as the beam passes the pen's screen position; a joystick's fire button does not reach it. In ECS, bit 13 of BEAMCON0 refuses the pulse, `self.revision.is_ecs() && self.beamcon0 & BEAMCON0_LPENDIS != 0` |
| **MiSTer** | [`rtl/agnus_beamcounter.v:113-121`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121), [`:135`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L135), [`rtl/denise.v:154`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L154) | descendant — the same read path, from a copy of the counter two `clk7_en` edges late: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos_rb[10:8]};`, and VHPOSR with `hpos[8:1] - 8'd1`. `//wire lpendis      = beamcon0_reg[13];` and `//assign lpen     = bplcon0[3];` are comments, as in NanoMig. No latch |
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

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

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

Silent on this point: Commodore, A500/A2000 Gary specification, undated; Commodore, 6526 CIA specification, 1981; 6502.org dissection of Denise 8373R4, undated; 6502.org dissection of CIA 8520R4, undated.

---

## Suggested change

**One option: give Agnus a light-pen line and the latch p. 239 describes.**
A copy of the RTL with the four changes below, and without LPENDIS, is the
copy the Test row names; the lists after them say what it does.

- **The line.** An input on `agnus` and `agnus_beamcounter`, driven in
  `nanomig.v` or `minimig.v` from port 2's pin 6, the A500's default on
  p. 238 and in the technical reference, which is `_fire1`, CIA-A's PA7
  input. A jumper-like choice of `_fire0` would give the A1000's port 1. The
  copy takes the AND of `_fire0` and `_fire1`, so a pulse on either port
  captures.
- **The enable.** Store BPLCON0 bit 3 in `agnus_beamcounter` beside ERSY and
  LACE (lines 144–163), reset to 0 as p. 271 says.
- **The latch.** With bit 3 set and nothing latched, a falling edge of the
  line outside vertical blanking stores `vpos` and `hpos`; with no edge in the
  field, the end of the field stores them (`eof`, line 377). The end of
  vertical blanking releases the latch: the module's `vblend`, line 447, is
  line 25 in PAL and 20 in NTSC, where p. 239 says line 20 for both. The copy
  releases when the counter enters line 20, in PAL and in NTSC.
- **The read.** While latched, the multiplexer of lines 108–115 returns the
  stored `vpos[10:8]` and `{vpos[7:0],hpos[8:1]}` instead of the counters; in
  ECS, with BEAMCON0 bit 13 set, VPOSR returns the counter (p. 305: *"on
  vertical pos read"*; the page does not say whether VHPOSR does too).

What it would change, from the copy's runs:

- all nineteen red lines of the Test row turn green on fx68k and on TG68K,
  and so does every other line of the run: 240 passed, 0 failed on each core;
- `K3` and `K4`, which set bit 3 in phase C and in phase A, turn `P10(b)` red
  at the ports: 34 passed, 2 failed, the run's `P10(b)` and the page-model
  contrast's, on both cores. On NanoMig as it is `P10(b)` stays green in both
  (see below);
- a program that polls VPOSR with bit 3 set would see it frozen, as p. 239
  warns, and a joystick's fire button held on port 2 would latch the counters,
  as the technical reference's *"FIRE input"* implies for the A500.

---

## What holds the finding up

- **The comparators read green on the page's latch.** The page-model control
  applies p. 238 step 4 and p. 239 to each run's own measured samples and
  grades them with the same `P1` to `P13` code: 25 passed, 0 failed in OCS/PAL,
  OCS/NTSC, ECS/PAL and ECS/NTSC, on fx68k and on TG68K. A copy of the RTL
  with the latch added passes every line of the run itself, 240 passed,
  0 failed on each core.
- **The fixture is live.** `CP1` to `CP6` hold in every configuration: the beam
  model is anchored on phase A, sampling is dense, pin 6 of both ports reads
  open at CIA-A ($FF), and every scheduled pulse arrives on its cycle, 4 colour
  clocks wide. `P13`, graded through the port pins' pot lines, is green.
- **It does not depend on the pen.** `P7` grades p. 239's field-end latch,
  which needs no trigger at all, and is red: after a field with no pulse the
  reads in lines 0 to 19 are not one value. No choice of port, pin, polarity
  or pulse width reaches a counter that has no latch in front of it.
- **It is Agnus's, not the CPU's.** The nineteen reds and every count above are
  the same on fx68k and TG68K.
- **ECS shows the same and is not graded.** The ECS runs measure every phase-B
  sample current and record 21 lines, the nineteen reds and `P2(i)` and `P9(a)`, because p. 305 does not give
  LPENDIS's reset value. NanoMig's own BEAMCON0 resets bit 13 to 0, which on
  NanoMig's own terms would leave the latched value visible, if there were one.
- **`K3` and `K4` turn `P10(b)` red only where a latch is in front of the
  counter.** They set bit 3 in phase C and in phase A. On NanoMig as it is
  `P10(b)` stays green and the run prints 15 passed, 20 failed: the nineteen
  reds and the page-model contrast's own `P10(b)`, on both cores. `K17` and
  `K18` add pulses to phase A with bit 3 set; each pulse is delivered on its
  cycle and the program finishes, and the only graded `CP1` lines they print
  are the page-model contrast's: `K17` has `CP1` and `CP1(A2)` red with L = 200
  measured (9 passed, 2 failed), `K18` has `CP1`, `CP1(A1)` and `CP1(A2)` red
  (7 passed, 3 failed). `K17` and `K18` print the same on the copy of the RTL
  with the latch.
- **What weakens it is that half of the implementations agree with NanoMig.**
  vAmiga and Denise have no light-pen latch either. The two that have one,
  WinUAE and Copperline, both latch at the end of a field with bit 3 set,
  which NanoMig does not, and neither triggers from a joystick's fire line.
  The finding rests on pp. 238 and 239, not on any implementation; the 8371
  specification, the A500/A2000 technical reference and the Agnus patent say
  the same.

---

## For the specialists

1. **Does bit 3 alone freeze the counters?** p. 239 says a field with no
   trigger latches at its end and stays latched until vertical blanking ends.
   On real hardware, with nothing in either port: set BPLCON0 bit 3 and read
   VPOSR twice anywhere in lines 0 to 19. Equal reads at the last line's
   position settle it.
2. **Which port on the machine NanoMig models.** p. 238 and the technical
   reference put the pen on port 2's pin 6 on the A500, and the B2000's J200
   moves it to p. 238's port 1. NanoMig would have to route one of its two fire lines
   to Agnus as well as to CIA-A, and then a fire button held on that port
   would latch the counters whenever bit 3 is set.
3. **When the latch is released.** p. 239 says vertical position 20, p. 219
   ends vertical blanking at line 20 in NTSC and 25 in PAL, and p. 79's table
   stops it at 21 and 29. NanoMig's hardwired end of vertical blanking is
   line 25 in PAL and 20 in NTSC, WinUAE releases at the same lines, and
   Copperline does not release at all until the next latch. The package
   grades the release only where every page agrees, from line 21 in NTSC and
   29 in PAL, and reports the line.
4. **LPENDIS after reset.** p. 305 does not give it. It decides whether an
   ECS machine that never writes BEAMCON0 shows the latch at all.
