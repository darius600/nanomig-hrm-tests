# NM-HRM-001 — PAL frame has one line too many

| | |
|---|---|
| **Status** | **WITHDRAWN** — the manual sanctions the measured value |
| **Module** | `agnus_beamcounter.v` |
| **Test** | [`hrm/agnus_beamcounter`](../../hrm/agnus_beamcounter) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four** run a 313-line PAL field |

---

## Why it was withdrawn

The test asserted that a normal PAL non-interlace field is 312 positions, `vpos`
0..311. NanoMig measures `max_vpos = 312` and 313 end-of-line pulses.

**Table 7-5 sanctions 313.** The finding stood as documented for weeks before
that was read.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | follows the long-frame bit |
| **vAmiga** | — | follows the long-frame bit |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:178`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L178) | <pre>lines = ntsc ? 261 : 311;</pre> frame restarts at `vPos == lines + lof` |
| **Copperline** | [`src/chipset/agnus.rs:7`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L7) | <pre>pub const PAL_LINES: u32 = 313;</pre> non-interlaced fields are all long frames |

**All four run a 313-line PAL field.** Nobody agrees with the assertion.

---

## Why this entry is kept

This is the entry the whole "keep the withdrawals" argument rests on. It was a
documented finding for weeks. Without the record, the next reader of that page
files it again — and the four implementations, the table and the reasoning all
have to be found a second time.
