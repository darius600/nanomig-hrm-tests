# NM-HRM-039 — Table 8-2's wrapped examples compute against 255 on a counter with 256 states

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect. Question: should the footnotes to Table 8-2 rows 3 and 4 on p.231 (200 → 45 and 45 → 200, each printed as a true count of 100) read 101 on a counter that has 256 states? Closes when an Amiga specialist records that the footnotes' 100 is an approximation or a printed error, or confirms that the counter wraps at 255, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Chapter 8 of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | unknown: the page names no chip revision; measured on NanoMig's `userio.v` mouse counter read through JOY0DAT, the two wrapping rows of Table 8-2 against the two non-wrapping rows; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.231, Table 8-2 and its footnotes `*` and `**` · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims, each with the assertion that carries it: `opus:8-limits.c15` (all four rows are "100 pulses"), `opus:8-limits.c20` / `sol:8-mouse-limit.c17` (footnote `*`), `opus:8-limits.c21` / `sol:8-mouse-limit.c18` (footnote `**`); the green controls are `opus:8-limits.c16` / `sol:8-mouse-limit.c13` (row 1) and `opus:8-limits.c17` / `sol:8-mouse-limit.c14` (row 2). Every assertion named here carries a claim id |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L219-L280) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v); inherited: MiSTer and TC64 have the same increment statement on a plain 8-bit field |
| **Test** | [`hrm/ch8-mouse-counter-wraparound`](../../hrm/ch8-mouse-counter-wraparound): `make -C hrm/ch8-mouse-counter-wraparound test`; an expected red exits nonzero |
| **Assertions** | 3 red, the package reports `15 passed, 3 failed`: `opus:8-limits.c15` — expected 100 state changes on every row of Table 8-2, observed 100, 100, 101, 101; `opus:8-limits.c20` (with `sol:8-mouse-limit.c17`) — expected 100 for 200 → 45, observed 101; `opus:8-limits.c21` (with `sol:8-mouse-limit.c18`) — expected 100 for 45 → 200, observed 101. The two non-wrapping rows are green in the same package |
| **Verdict of the four** | **unanimous on the half they can answer** — 256 states per axis in all four, and not one uses 255 as a modulus. Hardware: not measured |
| **Effort** | none: no RTL change; any correction is to the two footnotes on p.231 |

---

## Source requirement

**Printed p.231**, Table 8-2, gives four worked examples of mouse-counter movement and says of them:

> A count of 100 pulses is measured in each case.

Two rows wrap, and their footnotes compute the count by subtracting from **255**:

> the true count must be 255 - ( 200-45) = 100; the direction is down.

(row 3, footnote `*`: 200 → 45, *Down*)

> the true count must be  255 + (-155) = 100; the direction is up.

(row 4, footnote `**`: 45 → 200, *Up*)

## RTL and measurement

The counters are plain 8-bit registers ([`userio.v#L219-L280`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L219-L280)) and wrap mod 256. Reaching the printed target from the printed start therefore takes **101** quadrature state changes, not 100: the extra one is the state the wrap passes through that a subtraction from 255 does not count. The two wrapping rows are the only ones that move; 255 and 256 differ by one, and the measurement differs by one.

| Table 8-2 row | printed count | measured state changes |
|---|---:|---:|
| 1 — 200 → 100, no wrap | 100 | **100** |
| 2 — 100 → 200, no wrap | 100 | **100** |
| 3 — 200 → 45, wraps | 100 | **101** |
| 4 — 45 → 200, wraps | 100 | **101** |

The non-wrapping rows are the control, and they are green. One counter transition is one measured state change on this fixture.

## Risks and unknowns

- **The pulse half is unanswered.** None of the four models the quadrature pulse the way this finding frames it: all four add a host delta straight to the counter, one unit to one count. Only the digital-joystick encode touches real Gray-code phases. So they answer the modulus half and are silent on pulse counting.
- **Whether the footnotes are wrong, or "100 pulses" was always meant as an approximate figure, is open.** It is a question about the manual, not about a core.
- **No hardware read.** The page names no chip revision and no counter was measured on silicon.

## Independent evidence

| | | modulus at JOYxDAT |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:3717`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3717) | `v = (uae_u8)mouse_x[joy] \| (mouse_y[joy] << 8);` — eight bits per axis reach the CPU |
| **vAmiga** | [`ControlPort.cpp:109-121`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/ControlPort.cpp#L109-L121) | composes `& 0xFC` with `& 0x03` — eight bits, nothing wider |
| **Denise** | [`input/controlPort/mouse.cpp:67`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/mouse.cpp#L67) | `(posY & 0xff) << 8 \| (posX & 0xff)` |
| **Copperline** | [`bus.rs:3342`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L3342) | `counter_x: u8` advanced with `wrapping_add` — mod 256 by construction |
| **MiSTer** | [`rtl/userio.v:256`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L256) | descendant — identical: `dmouse0dat[7:0] <= dmouse0dat[7:0] + 1'd1;`, a plain 8-bit field, mod 256 |
| **TC64** | [`rtl/minimig/userio.v:401`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L401) | descendant — the same statement, same width: `dmouse0dat[7:0] <= #1 dmouse0dat[7:0] + 1'd1;` |

Denise's `255` constants in its input and Paula code are in `paula/pot.cpp` and are the pot capacitor's saturation sentinel, a different counter. Copperline's computations against 255 are the analogue paddle scale and its inverse, a calibration choice.

## Decision

**Most likely no change to any RTL.** An 8-bit counter has 256 states; that is not a choice any implementation made and not one any of them could make differently. The printed worked example subtracts from 255 where the hardware wraps at 256, and the discrepancy is exactly one. The finding closes as stated in *What is asked of a reader*.
