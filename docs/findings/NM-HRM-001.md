# NM-HRM-001 — PAL frame has one line too many

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The manual sanctions the measured value. |
| **Module** | `agnus_beamcounter.v` |
| **Test** | [`hrm/ch3-field-geometry`](../../hrm/ch3-field-geometry) grades the manual's 312; [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) carries the 313-line control |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
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
| **MiSTer** | [`rtl/agnus_beamcounter.v:87`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L87), [`:330`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L330) | descendant — the same `VTOTAL_PAL_VAL = 11'd312 - 11'd1` and the same `long_frame <= 1'b1` reset default NanoMig has; runs a 313-line PAL field for the same reason |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:103`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L103), [`:386`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L386) | descendant — the same `VTOTAL_PAL_VAL` and the same `long_frame_r <= 1'b1` reset default; runs a 313-line PAL field for the same reason |

**All four independent implementations run a 313-line PAL field.** Nobody agrees with the assertion. (`agnus_beamcounter.v` has no `Upstream` row on this withdrawn finding, so these two citations were derived directly from the `Module` row rather than reused.)


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

---

## Why this entry is kept

This is the entry the whole "keep the withdrawals" argument rests on. It was a
documented finding for weeks. Without the record, the next reader of that page
files it again — and the four implementations, the table and the reasoning all
have to be found a second time.
