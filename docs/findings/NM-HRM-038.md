# NM-HRM-038 — the pot counters are not held in reset after a POTGO write

| | |
|---|---|
| **Status** | open · red |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/userio.v#L136-L150) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L157-L171) |
| **Upstream** | **inherited** — there is no reset-hold construct in either tree |
| **Report to** | **Minimig-AGA and NanoMig.** All four implementations model a hold; this one models none |
| **Test** | [`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge) |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 4 red — two from the body text, two from Figure 8-5 |
| **Verdict of the four** | **all four hold, and all four put PAL at eight lines.** They split on NTSC: WinUAE and vAmiga flat 8, Denise and Copperline 7 |

---

## What the HRM says

**p.235**, body text: the counters are held reset for **seven or eight complete
scan lines** after a POTGO write, and the page is explicit that this is the
**hardware** doing it, not advice about what software should wait for.

**Figure 8-5**, p.235: the charging curves begin **eight lines** after POTGO.

The page gives two numbers for one quantity and never resolves them. That
wobble is part of the finding, and the implementations have something to say
about it.

## What NanoMig does

`userio.v` has **no reset-hold construct of any kind** — no counter, no state,
no comparison against a line count. A POTGO write with bit 0 set zeroes the four
counters, and the very next `pot_cnt_en` tick increments them.

### Measured

| | completed reset lines before the first increment |
|---|---|
| p.235 body | 7 or 8 |
| Figure 8-5 | 8 |
| **NanoMig** | **0** |

The first increment arrives on pulse 1, which is zero completed lines. The
package's control measures the same quantity on a channel driven to a known
count through the identical comparator.

## What the others say

| | | hold |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:485`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L485) | `#define POTDAT_DELAY 8`; while `pot_dat < POTDAT_DELAY` the charge is forced to `-2`, and counting starts on the ninth h-sync. No PAL/NTSC branch |
| **vAmiga** | [`PaulaRegs.cpp:188`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L188) | `pokePOTGO` schedules `POT_DISCHARGE` with a payload of **8**, decremented once per line. Hard-wired to `PAL::HPOS_CNT` even in NTSC |
| **Denise** | [`libami/paula/pot.cpp:88`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L88) | `pot.dischargeCounter = agnus.ntsc ? 7 : 8;` — and only when it is exhausted does counting begin |
| **Copperline** | [`bus.rs:7930-7934`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L7930-L7934) | `VideoStandard::Pal => 8, Ntsc => 7`, held in `tick_pot_hsync` while `pot_discharge_lines < discharge_lines` |

**This is the most useful third-witness result in the chapter.** The page's
unresolved "seven **or** eight" is read by two of four independent
implementations as a **video-standard distinction**, with PAL landing on exactly
the 8 that Figure 8-5 draws. The other two use a flat 8. Nobody uses zero.

## Suggested change

Hold the four pot counters for eight scan lines after a POTGO start on PAL. Two
of four implementations use seven on NTSC and two use eight; that difference is
a hardware question this project cannot settle, and whoever implements it should
decide it deliberately rather than inheriting whichever number is convenient.

## What holds the finding up

- The page says **hardware**, not software, so this is not advice being read as
  a requirement.
- The figure independently draws the same interval.
- All four implementations model a hold, and all four agree on PAL.
- The deviation is not an off-by-one against an unclear boundary: it is zero
  against seven or eight.
