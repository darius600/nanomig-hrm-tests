# NM-HRM-037 — the right controller port's X potentiometer input does not exist

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/userio.v#L153-L176) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L174-L195) |
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
| **MiSTer** | [`rtl/userio.v:188`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L188) | descendant — byte-identical to NanoMig: `potcap[2] <= joy2_pin5;`, no external term |
| **TC64** | [`rtl/minimig/userio.v:224`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L224) | descendant — `potcap[2] <= _mthird1 & joy2_pin5;` looks different but isn't: this tree's default build (`` `MINIMIG_PS2_MOUSE` ``, set in `rtl/soc/minimig_defines.vh:13`) hardwires `_mthird1` to `1'b1` (`userio.v:517`), so the AND term is a no-op and the built behaviour is NanoMig's missing gate exactly. Only the non-default build path gives it a live term, and that term is a second host mouse's button (`userio.v:574`), not a joystick pin. A fix diverges from both lines' shipped behaviour; neither already has it |

**The asymmetry is NanoMig's and Minimig-AGA's alone, textually.** It was not
inherited from a shared reading of the manual, because nobody else reads it
that way. TC64's line looks like the fix — it does AND `potcap[2]` with
something — but as built, that something is a constant `1`, so its shipped
behaviour is the same missing gate as NanoMig's and MiSTer's. No maintained
line has a live external term on this pin.

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

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 233 — agrees with the manual:

  > "The counters are stopped by signals from 2 control1er connectors(left-right) with 2 pins each."
  <!-- corpus: hw-manual-1985/p233.txt -->

  The August 1985 Appendix A says each of the four pot counters is stopped by a signal from its own connector pin, and its connector table lists 'RIGHT X RX 5 35 (POTlX)' beside the other three, as the 3rd edition p.282 does. p.204 (pin 5 = 'Right POT' / 'POT X') and p.208 (POTGO bits 13/12 = OUTRX/DATRX, right port pin 5) give the same map. Nothing marks POT1X as a pin that is not sensed.

- **US patent 4,829,473, Commodore-Amiga**, PDF page 10 — agrees with the manual:

  > "Individual control switches 158a, 158, 158c, and 158a monitor respective ones of the lines 85a, 87a, 89a, and 91a. When a preset level is sensed, each control switch outputs a control signal 150a, 150b, 150c, and 150d to latch registers 153a, b, c, d."
  <!-- corpus: patent-us4829473-paula/p010.txt -->

  The patent's pot circuit (FIG. 5) has four identical port circuits, 83a-d. Each pot line has its own sensing switch, and that switch latches the free-running counter into the port's register. POTGO-style bit pairs 15/14, 13/12, 11/10 and 9/8 drive the four tri-state drivers alike (p.9). No port is drive-only. All four inputs are sensed, as the manual's four-input reading requires.

- **6502.org dissection of Paula 8364R7 — third-party, the silicon of one revision**, an author's note in the archive — agrees with the manual:

  > "All of the channels are built pretty much in the same way, so we now focus on POT0X."
  <!-- corpus: paula-8364r7-dissection/8_8_txt.txt -->

  8364R7 silicon: the note names four channels, 'POT0X, POT0Y, POT1X, POT1Y', and describes POT0X as representative. A Schmitt trigger senses the pad, and the counter value is loaded into the channel register while the pad is low. POT1X is therefore a sensed input like the other three; the note does not single it out. The 'pretty much' is the author's own hedge.

Silent on this point: Commodore, *A500/A2000 Gary Specification*; Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.
