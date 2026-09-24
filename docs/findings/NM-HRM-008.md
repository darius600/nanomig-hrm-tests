# NM-HRM-008 — playfield-1 collision depends on playfield 2, by design, undocumented

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Read the first section before deciding: it qualifies the red. |
| **Module** | `denise_collision.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_collision.v#L62-L63) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) |
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
| **MiSTer** | [`rtl/denise_collision.v:62-63`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) | descendant — byte-identical: `assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf \| evenmatch);` gates PF1 on PF2 exactly as NanoMig does |
| **TC64** | [`rtl/minimig/denise_collision.v:62-63`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L62-L63) | descendant — the same two lines, unchanged. The whole file is identical on both lines; there is no divergence anywhere to fix, so changing NanoMig here means diverging from both ancestors at once |

**All four independent implementations gate playfield 1 on playfield 2 in
single-playfield mode** — and Copperline's line is NanoMig's expression term for
term, with a test named after the behaviour:
`collision_pixel_single_playfield_odd_match_requires_even_match`
([`src/video/bitplane/tests.rs:6626`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L6626)).
MiSTer and TC64 do too, byte-identical to NanoMig — as descendants that says
nothing about the manual, only that the behaviour has never diverged anywhere in
the ancestry.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS)**, PDF page 7 — agrees with the manual:

  > "NOTE: Playfield 1 is all odd numbered enabled bit planes."
  <!-- corpus: denise-spec-252126/p007.txt -->

  Commodore's 1988 ECS Denise specification repeats the note the manual prints in appendix A, and its CLXDAT table lists bits 01-04 as 'Playfield 1 to Sprite 0 (or 1)' and so on with no mode condition; nothing in the 14 pages makes the PF1 bit depend on PF2 or on DPF. Table-level only, like appendix A: the chapter-7 sentence about single vs dual playfield is not in this document.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga — one shared specification, counted once**, PDF page 29 — agrees with the manual:

  > "Fifteen bits in the collision storage register are utilized to indicate whether any of several types of overlaps has occurred: e.g., one sprite to another sprite, even bitplanes to odd bitplanes, even bitplanes to a certain sprite, or odd bitplanes to a certain sprite."
  <!-- corpus: patent-us4874164-agnus-denise/p029.txt -->

  Describes the odd-bitplanes-to-sprite bit as registering an overlap of the odd bitplanes with a sprite, with no playfield-mode condition; p021 likewise defines objects as 'any non-zero bits used to display any of eight sprites or two playfields'. Weak: this is the table-level statement again and says nothing about single vs dual playfield; the FIG. 12 gate diagram (p011) is illegible in the text layer.

Silent on this point: US patent 4,777,621, Commodore-Amiga.

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
