# NM-HRM-008 — playfield-1 collision depends on playfield 2, by design, undocumented

| | |
|---|---|
| **Status** | open · red — but see the first section |
| **Module** | `denise_collision.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_collision.v#L62-L63) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) |
| **Upstream** | **MiSTer** `rtl/denise_collision.v:62-63` — byte-identical: `assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf \| evenmatch);` gates PF1 on PF2 exactly as NanoMig does. **TC64** `rtl/minimig/denise_collision.v:62-63` — the same two lines, unchanged. The whole file is identical on both lines; there is no divergence anywhere to fix, so changing NanoMig here means diverging from both ancestors at once |
| **Report to** | **nowhere. The RTL is very likely more correct than the manual** |
| **Test** | [`hrm/ch3-collision-registers`](../../hrm/ch3-collision-registers) · [`hrm/ch7-collision-playfield-mode`](../../hrm/ch7-collision-playfield-mode) |
| **Chapter** | 3 — Playfield, and 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four gate playfield 1 on playfield 2** — NanoMig's expression, term for term |

---

## This is a manual defect, not an RTL defect

The test is red because the manual is this project's oracle, and that is the
correct outcome of the process. But **the finding to carry forward is that
appendix A's table of independent collision bits is incomplete** — not that
NanoMig is wrong.

Everything below is evidence for that reading.

---

## Chapter 7 found the same thing from two more pages

This finding was opened from appendix A's table of collision bits. Chapter 7
reached it independently, from the chapter body and from a different appendix
page, and five assertions in `hrm/ch7-collision-playfield-mode` are red on it.

> "collision detection does not change when you select either single- or
> dual-playfield mode" (printed p. 214)

> "Playfield 1 is all odd-numbered enabled bitplanes" (printed p. 273)

Measured there: `CLXDAT & 0x7FFF` for the same PF1-SPR stimulus differs between
`dblpf=0` and `dblpf=1` for all four sprite pairs, and with both an odd and an
even bitplane enabled, a match on the odd plane alone does not set the PF1 bit.

**That changes this finding's argument.** It is no longer one incomplete table in
an appendix: the manual states the independence in body text *and* in appendix A,
and the implementations — all four of them — contradict all of it. A single
incomplete table is an editing slip; the same proposition printed three times in
three places is what the manual means.

The obvious objection was checked and rejected by the independent validation:
pp. 211-212 do not limit collision independence to dual-playfield mode, they
discuss display priority.

---

## What the HRM says

**Appendix A's `CLXDAT` bit table, printed p. 273**, gives each collision pair its
own bit and treats them as independent.

---

## What NanoMig does

[`src/minimig-aga/denise_collision.v#L62-L63`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_collision.v#L62-L63)

```verilog
assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf | evenmatch);
assign evenmatch = bm[7] & bm[5] & bm[3] & bm[1];
```

With `dblpf = 0` — single playfield — `oddmatch` additionally requires
`evenmatch`. Playfield 1's presence is made to depend on playfield 2's.

**And the RTL says why, in the comment directly above it:**

> this is the implementation of an undocumented function in the real Denise chip,
> developed by Yaqube. trigger was the game Rotor. mentioned in WinUAE sources to
> be the only known game needing this feature. it also fixes the Spaceport
> instantly helicopter crash at takeoff and Archon-1 'sticky effect' of player
> sprite at the battlefield.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | PF1 gated by PF2 |
| **vAmiga** | — | PF1 gated by PF2 |
| **Denise** | [`emulation/libami/video/denise.cpp:478-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L478-L480) | <pre>if (!clxNoCol)&nbsp;clxDat \|= (v1 &lt;&lt; 5) \| (v1 &lt;&lt; 1) \| 1;<br>else if ((clxNoCol & 2) == 0)&nbsp;clxDat \|= (v1 &lt;&lt; 5);<br>else if (((clxNoCol & 1) == 0) && _doublePlayfield)&nbsp;clxDat \|= (v1 &lt;&lt; 1);</pre> the PF1 group only when `_doublePlayfield` |
| **Copperline** | [`src/video/bitplane.rs:6797`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L6797) | <pre>let odd_match = odd_match_raw && (dual_playfield \|\| even_match);</pre> |
| **MiSTer** | [`rtl/denise_collision.v:62-63`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) | descendant — byte-identical: `assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf \| evenmatch);`, gates PF1 on PF2 exactly as NanoMig does |
| **TC64** | [`rtl/minimig/denise_collision.v:62-63`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L62-L63) | descendant — the same two lines, unchanged |

**All four independent implementations gate playfield 1 on playfield 2 in
single-playfield mode** — and Copperline's line is NanoMig's expression term for
term, with a test named after the behaviour:
`collision_pixel_single_playfield_odd_match_requires_even_match`
([`src/video/bitplane/tests.rs:6626`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L6626)).
MiSTer and TC64 do too, byte-identical to NanoMig — as descendants that says
nothing about the manual, only that the behaviour has never diverged anywhere in
the ancestry.

---

## Suggested change

**None to the RTL.** Four independent implementations, three named games and a
reverse-engineering note all say real Denise behaves this way. Changing NanoMig
to match the manual would break Rotor, Spaceport and Archon-1.

What the evidence supports is a **footnote to appendix A's table**: the collision
bits are not independent in single-playfield mode, where playfield 1's bit
additionally requires playfield 2's match. That is a documentation change, and
this project does not own the document.

---

## For the specialists

This is the clearest case in the register of the manual being the incomplete
party. The RTL documents the behaviour, names the games, and credits the person
who reverse-engineered it; four implementations agree; and the manual's table
simply predates the knowledge.

The red result is the process working correctly — the manual is the oracle, the
oracle says something else, and the disagreement is recorded rather than
silently resolved in the implementation's favour.
