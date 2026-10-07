# NM-HRM-037 — the right controller port's X potentiometer input does not exist [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `potcap[2]` in `userio.v`, the right port's X pot counter (POT1X, connector pin 5), take an external input in its update as the other three counters do, as pp.228, 234, 240 and 282 print four sensed pot inputs? Closes when an Amiga specialist names the input that carries RX on this board (the page suggests `_djoy2[5]`, unverified), or records why the right X counter is drive-only, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — Minimig-AGA (MiSTer-devel/Minimig-AGA_MiSTer), whose `userio.v` line is byte-identical to NanoMig's; NanoMig takes the fix |
| **Scope** | unknown chip revision; NanoMig's `userio.v` at the pinned revision, the right port's pin 5 (`potcap[2]`), measured with every port-2 joystick input asserted. Port 1's X and Y and port 2's Y respond to their pins in the same runs |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, pp.228 (Table 8-1), 234, 240 and Appendix A p.282 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · p.234: `opus:8-pot.c1`, `sol:8-pot.c1`; p.228: `opus:8-tbl8-1.c15` / `sol:8-pin.c17`, `opus:8-tbl8-1.c16` / `sol:8-pin.c18`; p.240: `opus:8-dio.c32`, `sol:8-dio.c30`, `opus:8-dio.c33`, `sol:8-dio.c31`; p.282: `opus:8-appa-pot.c12`, `opus:8-appa-pot.c13`, `sol:8-appa-pot.c15`; `PROP-4` carries no claim id |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L153-L176) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L174-L195); fixed: MiSTer is byte-identical, TC64 differs (it ANDs `_mthird1`, see *Independent evidence*) |
| **Test** | [`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge): `make -C hrm/ch8-pot-counter-start-and-charge test`; [`hrm/ch8-pot-pins-as-digital-io`](../../hrm/ch8-pot-pins-as-digital-io): `make -C hrm/ch8-pot-pins-as-digital-io test`; [`hrm/ch8-controller-pin-function-map`](../../hrm/ch8-controller-pin-function-map): `make -C hrm/ch8-controller-pin-function-map test`; [`hrm/ch8-potgo-register-bit-layout`](../../hrm/ch8-potgo-register-bit-layout): `make -C hrm/ch8-potgo-register-bit-layout test`; an expected red exits nonzero |
| **Assertions** | 14 red across four packages. In `hrm/ch8-pot-counter-start-and-charge` (76 passed, 13 failed; the other eight reds are [NM-HRM-038](NM-HRM-038.md)'s): `opus:8-pot.c1`, `sol:8-pot.c1`, `sol:8-appa-pot.c15`, `opus:8-appa-pot.c12`, `opus:8-appa-pot.c13`; right connector, every joystick input asserted, 6 lines counted: expected `$0606`, observed `$0600`. In `hrm/ch8-pot-pins-as-digital-io` (33 passed, 4 failed): `opus:8-dio.c32`, `opus:8-dio.c33`, `sol:8-dio.c30`, `sol:8-dio.c31`. In `hrm/ch8-controller-pin-function-map` (25 passed, 4 failed): `opus:8-tbl8-1.c15`, `sol:8-pin.c17`, `opus:8-tbl8-1.c16`, `sol:8-pin.c18`. In `hrm/ch8-potgo-register-bit-layout` (13 passed, 1 failed): `PROP-4`. The FAIL lines of the last three packages carry the id only. Controls pass on the same measurement basis in each |
| **Verdict of the four** | **none reproduces it** — all four give the right-hand X counter the same external treatment as the other three. Hardware: not measured here; the documents under *Independent evidence* agree with the manual |
| **Effort** | S — one term on the right-hand side of `potcap[2]` in `userio.v`; which input is open |

---

## Source requirement

Four printed pages state it, from three directions.

**p.234**: *"There are two common types of proportional controllers: the
'paddle' controller pair and the X-Y proportional joystick"*, each port handling
**two** proportional inputs.

**p.228, Table 8-1**: connector pin 5 is `Right POT` for a proportional
controller pair and `POT X` for an X-Y joystick.

**p.282, Appendix A**: the connector table lists two pot pins per connector,
`LEFT X LX 5 32 (POT0X)` and `RIGHT X RX 5 35 (POT1X)`.

**p.240**: POTGO's four OUT/DAT pairs govern *"the entire register"* and read
back *"the current state of the pot pins"*. Those are universal propositions,
and one dead member falsifies each.

## RTL and measurement

```verilog
wire joy2_pin5 = ~(potreg[13] & ~potreg[12]);   // POTGO bits only
potcap[2] <= joy2_pin5;                          // right X: POTGO only
potcap[0] <= _mthird & joy1_pin5;                // left X: POTGO AND an input
```

`joy2_pin5` is a pure function of the POTGO write register, the pin's *driven*
value. The left counter ANDs it with `_mthird` (`~mouse_btn[2]`, a module input);
the right counter ANDs it with nothing. None of `_joy1`, `_joy2` and `mouse_btn`
(the controller inputs `userio.v` uses; `joy_ana1` and `joy_ana2` sit behind
`joy_ana_en = 1'b0`) reaches `potcap[2]`.

[`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge)
releases a counter's inputs after a warm-up longer than either printed reset
interval and counts the lines that follow:

| lines counted after release | reads | the pages |
|---|---|---|
| port 1, X alone, then Y alone, 6 and 9 | `$0006`, `$0900` | the same |
| port 2, every input the module reads asserted, 11 | `$0b00`: Y counts, X stays 0 | both count |
| right connector, every joystick input asserted, 6 | `$0600` | `$0606`, as the left connector reads |

`opus:8-pot.c1` and `sol:8-pot.c1` (`test_userio.cpp:417-488`) and
`sol:8-appa-pot.c15`, `opus:8-appa-pot.c12` and `opus:8-appa-pot.c13`
(`:1396-1446`) are red; port 1 and the left connector pass in the same runs.

## Risks and unknowns

- **Which input carries RX is a hardware question.** `_djoy2[5]` is what the
  neighbouring `potcap[3]` uses for the same connector; the page does not verify
  it against the wiring.
- **No hardware measurement.** The documents under *Independent evidence* agree
  with the manual. The Paula dissection is the silicon of one revision (8364R7),
  and its author hedges ("pretty much"). The A500 schematic is read from the
  drawing, not quoted.
- **TC64 is build-dependent.** Its Chameleon (TC64) builds have the same missing
  gate as NanoMig; its MiST and SiDi builds have a live term that is a second
  host mouse's middle button, not a joystick pin.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:3867`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3867) | `cap_check` is a double loop over `joy 0..1` and `i 0..1`; every external term is indexed by both. `joy=1, i=0` is POT1X and takes exactly what the other three take |
| **vAmiga** | [`Agnus.cpp:877-880`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L877-L880) | four symmetric increments; POT1X's term is `controlPort2.mouse.MMB()` |
| **Denise** | [`libami/paula/pot.cpp:132-139`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L132-L139) | four structurally identical blocks; POT1X's is identical to its siblings |
| **Copperline** | [`chipset/paula.rs:1305`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1305) | `for i in 0..self.pot_counters.len()` with no index singled out; index 2 is filled from `p2.pot_x_released()` |
| **MiSTer** | [`rtl/userio.v:188`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L188) | descendant — byte-identical to NanoMig: `potcap[2] <= joy2_pin5;`, no external term |
| **TC64** | [`rtl/minimig/userio.v:224`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L224) | descendant — `potcap[2] <= _mthird1 & joy2_pin5;`. Builds whose top level is `minimig_virtual_top.v`, the Chameleon (TC64) builds among them, define `` `MINIMIG_PS2_MOUSE` `` (`rtl/soc/minimig_defines.vh:13`, under `MINIMIG_VIRTUAL`), which hardwires `_mthird1` to `1'b1` (`userio.v:517`): the AND term is a no-op there, as NanoMig's missing gate. The MiST and SiDi builds (`minimig_mist_top.v`) do not define it, and the term is live: a second host mouse's middle button (`userio.v:574`) |

The asymmetry is NanoMig's and Minimig-AGA's alone, textually: no independent
implementation reads the manual that way.

### What other Commodore documents say

- **US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989**, PDF page 10 — agrees with the manual:

  > "Individual control switches 158a, 158, 158c, and 158a monitor respective ones of the lines 85a, 87a, 89a, and 91a. When a preset level is sensed, each control switch outputs a control signal 150a, 150b, 150c, and 150d to latch registers 153a, b, c, d."
  <!-- corpus: patent-us4829473-paula/p010.txt -->

  The pot circuit (FIG. 5) has four identical port circuits, 83a-d; each pot line has its own sensing switch. POTGO-style bit pairs 15/14, 13/12, 11/10 and 9/8 drive the four tri-state drivers alike (p.9). No port is drive-only.

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — agrees with the manual:

  > "All of the channels are built pretty much in the same way, so we now focus on POT0X."
  <!-- corpus: paula-8364r7-dissection/8_8_txt.txt -->

  The note names four channels, 'POT0X, POT0Y, POT1X, POT1Y', and describes POT0X as representative: a Schmitt trigger senses the pad, and the counter value is loaded into the channel register while the pad is low. It does not single POT1X out.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 298 — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 4 of the A500 main board carries four pot lines side by side, POT0X, POT0Y, POT1X and POT1Y, each with a .047uF capacitor, to sheet 5, where (PDF page 300) the right joystick port's pin 5 is POT1X and pin 9 POT1Y, wired as the left port's pin 5 and pin 9 are to POT0X and POT0Y.
  <!-- drawing: a500-a2000-trm-1987/p298.txt -->

## Decision

Give `potcap[2]` the external term its three siblings have; the shape is in the
file one line above:

```verilog
potcap[2] <= _djoy2[5] & joy2_pin5;   // or whichever input carries RX here
```

Which input is correct belongs to someone who knows the wiring. The decision
belongs to an Amiga specialist, not to this repository.
