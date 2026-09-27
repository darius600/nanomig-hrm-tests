# NM-HRM-082 — the left and right joystick lines never reach a button register, where p. 329's NOTE makes them a proportional controller's buttons

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. p. 329's NOTE changes what the left and right lines are used for, not where they are read: p. 329's own table puts pins 3 and 4 at JOY[0/1]DAT bits 9 and 1, p. 332 sends a proportional controller's left and right buttons to the digital joystick's register, and NanoMig reads them there. The red assertion requires POTINP bit 10 to move, a bit pp. 240 and 282 give to pin 9, and an implementation that follows those pages fails it as NanoMig does. |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L229-L254) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L250-L275) |
| **Test** | [`hrm/appDE-joystick-decode`](../../hrm/appDE-joystick-decode) — `sol:E-joystick.c8+opus:E-joystick.c9` (p. 329, the NOTE) still fails, because it requires POTINP bit 10 to change when pin 3 or pin 4 closes (`test_userio.cpp:392-393`); in the same run JOY0DAT carries both presses at the bits p. 329's table gives them |
| **Chapter** | Appendix E, printed pp. 327, 329, 331 and 332; 8 — Interface Hardware, printed pp. 228, 232–234 and 240; Appendix A, printed pp. 281–282 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 8 (`7 passed, 1 failed`): `sol:E-joystick.c8+opus:E-joystick.c9` reads POTINP bit 10 as 1 idle, 1 with Left* closed and 1 with Right* closed. The table's pin-to-register assertions for pins 1 to 4, `sol:E-joystick.c10+opus:E-joystick.c12` to `sol:E-joystick.c13+opus:E-joystick.c15`, are green |
| **Error class** | **a silence read as a requirement** |
| **Verdict of the four** | **unanimous with NanoMig and with p. 332**: WinUAE and Copperline model a proportional controller and put its two buttons on the left and right lines, read at JOYxDAT bits 9 and 1; vAmiga and Denise model none. In all four a closed left or right line reaches JOYxDAT alone, and POTGOR reads pins 5 and 9 only |
| **See also** | [NM-HRM-037](NM-HRM-037.md) (the same module's POTINP bits: the right port's pin-5 bit has no input; the pin-9 bit this red reads has one, and is not part of that finding), [NM-HRM-038](NM-HRM-038.md) (POTGOR after POTGO START; the package here never writes POTGO) |

---

## Why it was withdrawn

The red assertion closes the left line (pin 3) and then the right line (pin 4)
of the port JOY0DAT reads, and requires bit 10 of POTINP to change with each.
It stays at 1 throughout, while JOY0DAT shows each press at the bit p. 329's
own table gives it.

**No page connects pins 3 or 4 to POTINP.** The NOTE names no register, and
neither claim the assertion carries names one; both restate the NOTE. The
register was chosen by the assertion.

**The pages that do name a register name the joystick's.** p. 329's table
binds pin 3 to `JOY[0/1]DAT<9>` and pin 4 to `JOY[0/1]DAT<1>`. p. 332's table
for proportional inputs gives the two buttons the same pins and, for their
register, *See Digital Joystick*. p. 234, under *Reading Proportional
Controller Buttons*, says the direction lines *serve as* the paddles' fire
buttons. The NOTE's *button inputs* are the same two lines, read through the
same register; what changes is that software reads bits 9 and 1 as buttons and
leaves forward and back unused.

**POTINP bit 10 belongs to another pin.** p. 240's Table 8-4 gives it to port 1,
pin 9, and p. 282 gives POTGOR's data bits to Paula's four pot pins, which its
connector table ties to pins 5 and 9. Pins 1 to 4 go to Denise instead: p. 281
samples LEFT* and RIGH* on Denise's M0V and M0H and uses them to clock the mouse
counters. An implementation that follows these pages leaves POTINP bit 10 where
it is when pin 3 or pin 4 closes and fails the assertion exactly as NanoMig
does, so the assertion cannot tell the two apart.

**The NOTE's case is a controller on the port, not a mode of the chips.** No
page names a register bit, a pin or a signal that selects it. POTGO, which
p. 240 says can redefine the functions of some of the controller port pins,
controls the four pot pins only. Figure 8-3 draws pins 1 to 4 through an
inverter and an exclusive-OR per counter into JOY0DAT's two counters and to
nothing else. A paddle's button closing pin 3 is therefore, at the chips, the
event a joystick's left switch closing pin 3 is, which is the stimulus the
package drives.

**p. 331's "The buttons are also available in POTGO and POTINP registers" is
about the pot pins.** It follows the sentence that names POTY and POTX as the
right and unused mouse buttons, and p. 332's table gives POTGO and POTINP to
pins 5 and 9 alone.

---

## What the HRM says

**Printed p. 329**, *Game Port Interface to Digital Joysticks*, the NOTE whole:

> NOTE: The right and left direction inputs are also designed to be right and
> left buttons, respectively, for use with proportional inputs. In this case,
> the forward and back inputs are not used, while right and left become button
> inputs rather than joystick inputs.

and the page's own register column for the two pins, read from the render (the
text layer displaces this column):

```
PIN  MNEMONIC  DESCRIPTION             HARDWARE REGISTER/NOTES
3    LEFT*     Left joystick switch    JOY[0/1]DAT<9>
4    RIGHT*    Right joystick switch   JOY[0/1]DAT<1>
```

**Printed p. 332**, *Connector Pin Usage for Proportional Inputs*, read from
the render:

```
PIN  MNEMONIC  DESCRIPTION     HARDWARE REGISTER/NOTES
1    XBUT      Extra Button
2    Unused
3    LBUT*     Left button     See Digital Joystick
4    RBUT*     Right button    See Digital Joystick
5    POTX      X analog in     POT[0/1]DAT<7:0>, POTGO, POTINP
9    POTY      Y analog in     POT[0,1]DAT<15:8>, POTGO, POTINP
```

**Printed p. 331**, *Game Port Interface to Proportional Controllers*:

> NOTE: The POTY and POTX inputs are designated as "right mouse button" and
> "unused mouse button" respectively. An opened switch corresponds to high
> resistance, a closed switch to a low resistance. The buttons are also
> available in POTGO and POTINP registers.

**Printed p. 327** lists the four device types the connectors take, the third
of them:

> 3. Proportional (pot or proportional joystick), 2 buttons max.

**Printed p. 228**, Table 8-1, the two rows, read from the render:

```
Pin  Joystick  Mouse ...  Proportional controller (pair)  X-Y proportional joystick
3    left      VQ-pulse   left button                     button 1
4    right     HQ-pulse   right button                    button 2
```

**Printed p. 234**, *Reading Proportional Controllers*:

> A paddle controller pair consists of two individual enclosures, each
> containing a single resistor and fire-button and each connected to a common
> controller port input connector.

and under *Reading Proportional Controller Buttons*, the section whole:

> For the paddle controllers, the left and right joystick direction lines serve
> as the fire buttons for the left and right paddles.

Figure 8-4 on the same page draws each paddle's fire button between pin 8 and
pin 3 and each wiper on pin 9, for both paddles alike; Table 8-1 and p. 332 put
the right paddle's button on pin 4 and its wiper on pin 5. In both, the button
closes a direction line (read from the drawing, not quoted).

**Printed p. 233**, Figure 8-3, *Joystick to Counter Connections*: pins 1 to 4
of port 1, labelled FORWARD, BACK, LEFT and RIGHT with their mouse names, run
through one inverter and one exclusive-OR gate per counter into the MOUSE 0 Y
and X counters of JOY0DAT, and to nothing else; JOY1DAT *"is wired similarly"*
(read from the drawing, not quoted).

**Printed p. 232**, *Mouse Buttons*:

> Button 2 (right button on Amiga mouse) is connected to pin 9 of the
> controller ports, one of the proportional pins.

**Printed p. 240**, *Digital I/O on the Controller Port*:

> The control lines built into the POTGO register (address $DFF034) can
> redefine the functions of some of the controller port pins.

> The pot-control register controls a four-bit bidirectional I/O port that
> shares the same four pins as the four pot inputs.

and the row of Table 8-4 the assertion reads:

> 10 DATLY data for port 1, pin 9 (right mouse button)

**Printed p. 281**, Appendix A, JOY0DAT and JOY1DAT:

> The following table shows the mouse/joystick connector pin usage. The pins
> (and their functions) are sampled (multiplexed) into the DENISE chip during
> the clock times shown in the table.

with the two rows for the left port, read from the render:

```
Conn  Joystick  Mouse      Sampled by DENISE
Pin   Function  Function   Pin  Name  Clock
L3    LEFT*     YQ         38   M0V at CCK*
L4    RIGH*     XQ          9   M0H at CCK*
```

> After being sampled, these connector pin signals are used in quadrature to
> clock the mouse counters. The LEFT and RIGHT joystick functions (active high)
> are directly available on the Y1 and X1 bits of each counter.

**Printed p. 282**, Appendix A, POTGOR:

> This register controls a 4-bit bi-directional I/O port that shares the same
> four pins as the four pot counters above.

and the pot counters' connector table, read from the render:

```
Loc.   Dir.  Sym  Pin   Paula Pin#  Pin Name
RIGHT  Y     RY   9     36          (POT1Y)
RIGHT  X     RX   5     35          (POT1X)
LEFT   Y     LY   9     33          (POT0Y)
LEFT   X     LX   5     32          (POT0X)
```

with bit 10 of POTGOR, DATLY, the *"I/O data Paula pin 33"*.

---

## What NanoMig does

**A closed pin 3 or pin 4 reaches JOY0DAT and nothing else.** With `joy_swap`
tied to 0, the `_joy2` input feeds the port JOY0DAT reads —
[`src/minimig-aga/userio.v#L92`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L92), `wire joy_swap = 1'b0;`, and [`#L215`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L215), `_sjoy1 <= joy_swap ? joy1 : joy2;`.
Between counter steps the left line sets bit 9 and the right line bit 1 —
[`#L239`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L239), `dmouse0dat[1:0] <= {!_djoy1[0], _djoy1[0] ^ _djoy1[2]};`, and
[`#L252`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L252), `dmouse0dat[9:8] <= {!_djoy1[1], _djoy1[1] ^ _djoy1[3]};`.
POTINP bit 10 is the port's second-button input (bit 5 of the host word,
`fire2` in `minimig.v`'s port list), the right mouse button and POTGO's drive,
with no direction line among its terms —
[`#L174`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L174), `potcap[1] <= _mright & _djoy1[5] & ~(potreg[11] & ~potreg[10]);`,
read out at [`#L300-L305`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L300-L305), `1'b0, potcap[1],`.

**The module carries p. 332's pin assignment for a host controller, switched
off.** With `joy_ana_en` set, the host word is re-laid onto the connector lines
so that fire lands on the left line, fire2 on the right line, bit 6 on the
forward line and the back line stays open —
[`#L210`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L210), `wire [15:0] joy1 = joy_ana_en ? {{12{1'b1}}, _joy1[6], 1'b1, _joy1[4], _joy1[5]} : _joy1;`
— which is p. 332's LBUT*, RBUT*, XBUT and unused pin 2. It is tied off at
[`#L93`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L93), `wire joy_ana_en = 1'b0;`.
It decides how a host's controller is presented on the lines, not what the
chips do with a closed line; the decode it would feed is the one measured.

### Measured

[`hrm/appDE-joystick-decode`](../../hrm/appDE-joystick-decode) builds
`userio` alone and drives `_joy2`, every bit explicitly, with POTGO never
written and the mouse buttons released; `7 passed, 1 failed`.

| case | JOY0DAT | POTINP bit 10 | graded by |
|---|---|---|---|
| idle, joystick mode latched | — | 1 | — |
| pin 3 (Left*) closed | `0xFF00`: bit 9 = 1, bit 9 xor bit 8 = 0 | 1 | `sol:E-joystick.c8+opus:E-joystick.c9`, **red** |
| pin 4 (Right*) closed | `0x00FF`: bit 1 = 1, bit 1 xor bit 0 = 0 | 1 | the same assertion |
| pin 3, then pin 4, each closed from idle in a block of its own | bit 9 = 1; bit 1 = 1 | not read | `sol:E-joystick.c12+opus:E-joystick.c14`, `sol:E-joystick.c13+opus:E-joystick.c15`, green |

Each press reads as p. 329's table and the NOTE put it: the pressed line's bit
set, and forward or back, which the NOTE leaves unused, clear.

---

## What the others say

The question put to each: when pin 3 or pin 4 closes, which register shows it,
and does POTGOR change?

| | code | |
|---|---|---|
| **WinUAE** | [`od-win32/dinput.cpp:4787-4791`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/dinput.cpp#L4787-L4791), [`inputdevice.cpp:6978-6980`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L6978-L6980), [`inputdevice.cpp:5646-5661`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L5646-L5661), [`inputdevice.cpp:3631-3651`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3631-L3651), [`inputdevice.cpp:3877`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3877) | a port in analogue-joystick mode takes `INPUTEVENT_JOY1_HORIZ_POT, INPUTEVENT_JOY1_VERT_POT, INPUTEVENT_JOY1_LEFT, INPUTEVENT_JOY1_RIGHT`, and the default mapping gives the host's first button `port ? INPUTEVENT_JOY2_LEFT : INPUTEVENT_JOY1_LEFT` and its second `port ? INPUTEVENT_JOY2_RIGHT : INPUTEVENT_JOY1_RIGHT`. Those events take the joystick's own path, `/* button to axis mapping */`, into the counter bits, `b1 = (right ^ 1) ? 2 : 0;` and `b9 = (left ^ 1) ? 2 : 0;`. POTGOR's pin bits follow the pot capacitors, which a press pulls down only for the pin-5 and pin-9 buttons, `int isbutton = getbuttonstate (joy, i == 0 ? JOYBUTTON_3 : JOYBUTTON_2);`. **A proportional controller's buttons read at JOYxDAT bits 9 and 1; POTGOR does not see them** |
| **vAmiga** | [`Core/Ports/ControlPortTypes.h:20-25`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPortTypes.h#L20-L25), [`Core/Peripherals/Joystick/Joystick.cpp:175-224`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Joystick/Joystick.cpp#L175-L224), [`Core/Components/Paula/PaulaRegs.cpp:144-161`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L144-L161) | a port holds `NONE`, `MOUSE` or `JOYSTICK`, nothing proportional. A joystick's left and right go into its JOYxDAT bits alone, `if (axisX == -1) result \|= 0x0300;` and `else if (axisX == 1) result \|= 0x0003;`; POTGOR is built from the pot charges, `REPLACE_BIT(result, 10, chargeY0 >= 1.0);`, which the joystick overrides only from its second and third buttons, `if (button2) {`. **No proportional controller; a closed left or right line reaches JOYxDAT alone** |
| **Denise** | [`emulation/libami/input/controlPort/controlPort.cpp:18-36`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/controlPort.cpp#L18-L36), [`emulation/libami/input/controlPort/joypad.cpp:17-46`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/joypad.cpp#L17-L46), [`emulation/libami/paula/pot.cpp:64-78`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L64-L78) | a port device is a `CD32Pad`, a `Joypad`, a `FourPlayerAdapter` or a `Mouse`, nothing proportional. A joypad's left and right land in the direction word, `if (interface->inputPoll( device->id, 2 ) & 1) out \|= 0x300; // Left` and `if (interface->inputPoll( device->id, 3 ) & 1) out \|= 0x3; // Right`; its pot pins are its second and third buttons, `// PotY = Pin9 = Button 2`, and POTGOR reads the pot capacitors, `if (pot.capY0 == 255) out \|= POT_DAT_Y0;`. **No proportional controller; a closed left or right line reaches JOYxDAT alone** |
| **Copperline** | [`src/bus.rs:2827-2828`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L2827-L2828), [`src/bus.rs:2988-3027`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L2988-L3027), [`src/chipset/paula.rs:1217-1243`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1217-L1243), [`src/bus/tests.rs:12253-12273`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L12253-L12273) | its `Analogue` device is the proportional controller, `Analogue paddles / proportional stick: positions present resistances` … `on POTxX/POTxY; the two buttons ground the LEFT/RIGHT lines.` Its JOYxDAT sets the two bits over the idle counters, `v \|= 0x0200;` and `v \|= 0x0002;`, without the companion bit a joystick's decode would set, so its forward and back read differently from a joystick's there, which the NOTE leaves unused. POTGOR's pin bits follow the pot-pin buttons alone, `!self.button3` and `!self.button2`. Its own test says it in words, `the left/right switch lines ARE the paddle buttons.`, and reads `assert_eq!(bus.custom_read(0x00A, 2), 0x0200);`. **A proportional controller's buttons read at JOYxDAT bits 9 and 1; POTGOR does not see them** |
| **MiSTer** | [`rtl/userio.v:273`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L273), [`rtl/userio.v:193`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L193), [`rtl/userio.v:231`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L231), [`rtl/userio.v:466`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L466) | descendant — NanoMig's decode, `dmouse0dat[9:8] <= {!_djoy1[1], _djoy1[1] ^ _djoy1[3]};`, and NanoMig's POTINP bit, `potcap[1] <= _mright & _djoy1[5] & ~(potreg[11] & ~potreg[10]);`. The analogue-joystick mapping NanoMig ties off is live here, `wire [15:0] joy1 = joy_ana_en ? {{12{1'b1}}, _joy1[6], 1'b1, _joy1[4], _joy1[5]} : _joy1;`, switched from the host, `{joy_swap, cd32pad, joy_ana_en} <= {IO_DIN[3], ~IO_DIN[1] & IO_DIN[2], IO_DIN[1]};`. A closed left or right line reaches JOY0DAT alone |
| **TC64** | [`rtl/minimig/userio.v:418`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L418), [`rtl/minimig/userio.v:229`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L229), [`rtl/minimig/userio.v:330-333`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L330-L333) | descendant — the same decode, `dmouse0dat[9:8] <= #1 {!_djoy1[1], _djoy1[1] ^ _djoy1[3]};`, and a POTINP bit with no direction term, `potcap[1] <= #1 _mright0 & _rmb & _djoy1[5] & ~(potreg[11] & ~potreg[10]);`. On the joystick port an OSD-selected analogue mode puts the host controller's buttons on pins 4 and 3, `wire _tpin4 = anajoy ? _tjoy2[5] : _tjoy2[0];` and `wire _tpin3 = anajoy ? _tjoy2[4] : _tjoy2[1];`, the same assignment as MiSTer's. A closed left or right line reaches JOYxDAT alone |

**All four read a closed left or right line in JOYxDAT and nowhere else, as
NanoMig does.** The two that model a proportional controller, WinUAE and
Copperline, wire its buttons exactly as p. 332 does, to the left and right
lines, and read them at bits 9 and 1; Copperline's comment and test say so in
words. None lets a direction line reach POTGOR. Both Minimig lines carry
NanoMig's decode and POTINP bit, and both switch a host controller's buttons
onto pins 3 and 4 in an analogue mode that NanoMig's copy has tied off.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 9 — agrees with the manual:

  > "The pins (and their functions) are sampled (multiplexed) into the DENISE chip during the clock times shown in the Table."
  <!-- corpus: denise-spec-252126/p009.txt -->

  Commodore's ECS Denise specification carries Appendix A's connector table: pins 1 to 4 of each port, L3 LEFT* and L4 RIGH* among them, sampled on Denise's M0V, M0H, M1V and M1H, and no other pin. It does not mention proportional controllers, paddles, buttons or the pot pins.

---

## Why this entry is kept

p. 329's NOTE says the two lines *"become button inputs rather than joystick
inputs"* and names no register, and a reader looking for a button register in
the port logic finds POTINP beside JOY0DAT. This page is where that reader
finds p. 332's *See Digital Joystick* for the same two pins, and pp. 240, 281
and 282, which keep pins 3 and 4 on Denise's counters and POTINP on pins 5
and 9. A test that grades the NOTE at POTINP is red on NanoMig, on all four
implementations read here and on any implementation of those pages.
