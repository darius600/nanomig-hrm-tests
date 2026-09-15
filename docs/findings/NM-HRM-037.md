# NM-HRM-037 — the right controller port's X potentiometer input does not exist

| | |
|---|---|
| **Status** | open · red |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/userio.v#L153-L176) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L174-L195) |
| **Upstream** | **inherited** — both lines are byte-identical to upstream |
| **Report to** | **Minimig-AGA and NanoMig.** No other implementation does this |
| **Test** | [`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge), [`hrm/ch8-pot-pins-as-digital-io`](../../hrm/ch8-pot-pins-as-digital-io), [`hrm/ch8-controller-pin-function-map`](../../hrm/ch8-controller-pin-function-map), [`hrm/ch8-potgo-register-bit-layout`](../../hrm/ch8-potgo-register-bit-layout) |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 14 red across four packages. Controls pass on the same measurement basis in each |
| **Verdict of the four** | **none reproduces it** — all four give the right-hand X counter the same external treatment as the other three |

---

## What the HRM says

Four printed pages state it, from three different directions.

**p.234** — *"There are two common types of proportional controllers: the
'paddle' controller pair and the X-Y proportional joystick"*, each port handling
**two** proportional inputs.

**p.228, Table 8-1** — connector pin 5 is `Right POT` for a proportional
controller pair and `POT X` for an X-Y joystick.

**p.282, Appendix A** — the connector table lists two pot pins per connector:
`LEFT X LX 5 32 (POT0X)` and `RIGHT X RX 5 35 (POT1X)`.

**p.240** — POTGO's four OUT/DAT pairs govern *"the entire register"* and read
back *"the current state of the pot pins"*. Those are universal propositions,
and one dead member falsifies each.

## What NanoMig does

```verilog
wire joy2_pin5 = ~(potreg[13] & ~potreg[12]);   // POTGO bits only
wire joy1_pin5 = ~(potreg[9]  & ~potreg[8]);    // POTGO bits only

potcap[2] <= joy2_pin5;                          // right X: POTGO only
potcap[0] <= _mthird & joy1_pin5;                // left X: POTGO AND an input
```

Both `pinX` wires are pure functions of the POTGO write register — they are the
pin's **driven** value. The difference is the conjunction. The left counter ANDs
it with `_mthird`, which is `~mouse_btn[2]` and a real module input; the right
counter ANDs it with nothing.

`_joy1`, `_joy2` and `mouse_btn` are the only external inputs `userio.v` has,
and none reaches `potcap[2]`. **The signal is named for a connector pin it does
not sense.**

### Measured

| | POT1DAT low byte (right X) |
|---|---|
| every input the fixture has, driven to its most active combination | `0x00 → 0x00` |
| port 1's equivalent channel, same comparator, same run | `0x00 → 0x14 → 0x14` |

The port-1 control is what makes this survive the units-error objection: the
same measurement basis moves on the live channel and not on the dead one.

## What the others say

| | | |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:3867`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3867) | `cap_check` is a double loop over `joy 0..1` and `i 0..1`; every external term is indexed by both. `joy=1, i=0` is POT1X and takes exactly what the other three take |
| **vAmiga** | [`Agnus.cpp:877-880`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L877-L880) | four symmetric increments; POT1X's term is `controlPort2.mouse.MMB()` |
| **Denise** | [`libami/paula/pot.cpp:132-139`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L132-L139) | four structurally identical blocks; POT1X's is identical to its siblings |
| **Copperline** | [`chipset/paula.rs:1305`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1305) | `for i in 0..self.pot_counters.len()` with no index singled out; index 2 is filled from `p2.pot_x_released()` |

**The asymmetry is NanoMig's and Minimig-AGA's alone.** It was not inherited
from a shared reading of the manual, because nobody else reads it that way.

Two unrelated defects were found while reading and belong to those projects
rather than to this finding: vAmiga accumulates all four pot charge deltas into
`chargeX0` ([`PaulaEvents.cpp:110-113`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaEvents.cpp#L110-L113)),
and `ControlPort::chargeDX`/`chargeDY` are never assigned anywhere in that
repository.

## Suggested change

Give `potcap[2]` the external term its three siblings have. The shape is
already in the file one line above:

```verilog
potcap[2] <= _djoy2[5] & joy2_pin5;   // or whichever input carries RX here
```

Which input is correct is a hardware question — `_djoy2[5]` is what the
neighbouring `potcap[3]` uses for the same connector — and belongs to someone
who knows the wiring.

## What holds the finding up

- Four printed pages, none of them ambiguous, and one of them a table whose
  whole purpose is the pin-to-function map.
- A control on the same comparator, in the same run, on the port that works.
- No implementation of four agrees with the RTL.
- The gap is visible in the source without running anything: `potcap[2]`'s
  right-hand side contains no signal that leaves the module.
