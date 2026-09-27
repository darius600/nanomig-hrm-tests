# NM-HRM-039 — Table 8-2's wrapped examples compute against 255 on a counter with 256 states

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L219-L280) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v) |
| **Report to** | **probably nobody.** See *Suggested change* |
| **Test** | [`hrm/ch8-mouse-counter-wraparound`](../../hrm/ch8-mouse-counter-wraparound) |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 3 red, all on the two wrapping rows and their footnotes: `opus:8-limits.c15`, `opus:8-limits.c20` (with `sol:8-mouse-limit.c17`) and `opus:8-limits.c21` (with `sol:8-mouse-limit.c18`). The two non-wrapping rows are green in the same package |
| **Verdict of the four** | **unanimous on the half they can answer** — 256 states per axis in all four, and not one uses 255 as a modulus |

---

## What the HRM says

**p.231**, Table 8-2, gives worked examples of mouse-counter movement. Every row
is described as measuring **100 pulses**. Two of the rows wrap, and their
footnotes compute the count by subtracting from **255**:

- row 3: 200 → 45 is *Down*, and the footnote computes 100
- row 4: 45 → 200 is *Up*, likewise 100

## What NanoMig does

The counters are plain 8-bit registers. They wrap **mod 256**. Reaching the
printed target from the printed start therefore takes **101** quadrature state
changes, not 100 — the extra one being the state the wrap passes through that a
subtraction from 255 does not count.

### Measured

| Table 8-2 row | printed count | measured state changes |
|---|---:|---:|
| 1 — 200 → 100, no wrap | 100 | **100** |
| 2 — 100 → 200, no wrap | 100 | **100** |
| 3 — 200 → 45, wraps | 100 | **101** |
| 4 — 45 → 200, wraps | 100 | **101** |

**The non-wrapping rows are the control**, and they are green. One counter
transition is one measured state change on this fixture; the two wrapping rows
are the only ones that move.

## What the others say

| | | modulus at JOYxDAT |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:3717`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3717) | `v = (uae_u8)mouse_x[joy] \| (mouse_y[joy] << 8);` — eight bits per axis reach the CPU |
| **vAmiga** | [`ControlPort.cpp:109-121`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPort.cpp#L109-L121) | composes `& 0xFC` with `& 0x03` — eight bits, nothing wider |
| **Denise** | [`input/controlPort/mouse.cpp:67`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/mouse.cpp#L67) | `(posY & 0xff) << 8 \| (posX & 0xff)` |
| **Copperline** | [`bus.rs:3342`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L3342) | `counter_x: u8` advanced with `wrapping_add` — mod 256 by construction |
| **MiSTer** | [`rtl/userio.v:256`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L256) | descendant — identical: `dmouse0dat[7:0] <= dmouse0dat[7:0] + 1'd1;`, a plain 8-bit field, mod 256 |
| **TC64** | [`rtl/minimig/userio.v:401`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L401) | descendant — the same statement, same width: `dmouse0dat[7:0] <= #1 dmouse0dat[7:0] + 1'd1;`. Both lines give the counter 256 states; neither wraps at 255 |

**Not one of the four independent implementations uses 255 as a modulus or a
wrap target in the mouse-counter path.** Denise's many `255` constants in its
input and Paula code are in `paula/pot.cpp` and are the pot capacitor's
saturation sentinel — a different counter. Copperline's computations against
255 are the analogue paddle scale and its inverse, a calibration choice. Both maintained Minimig lines give the counter
256 states as well; neither wraps at 255.

**The honest limit.** None of the four models the quadrature *pulse* the way
this finding frames it: all four add a host delta straight to the counter, one
unit to one count. Only the digital-joystick encode touches real Gray-code
phases. So the four answer the **modulus** half of the question and are silent
on the **pulse-counting** half.

## Suggested change

**Most likely none to any RTL.** An 8-bit counter has 256 states; that is not a
choice any implementation made and not one any of them could make differently.
The printed worked example subtracts from 255 where the hardware wraps at 256,
and the discrepancy is exactly one.

Whether Table 8-2's footnotes are wrong, or whether "100 pulses" was always
meant as the approximate figure the surrounding prose treats it as, is a
question about the manual. It belongs to whoever maintains the errata, not to
anyone who maintains a core.

## What holds the finding up

- Four rows of the same table, measured the same way, and only the two that wrap
  disagree.
- The arithmetic is not in dispute: 255 and 256 differ by one, and the
  measurement differs by one.
- Four independent implementations give the counter 256 states.
- Nothing here depends on a reading of ambiguous prose — the footnote prints its
  own subtraction.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

