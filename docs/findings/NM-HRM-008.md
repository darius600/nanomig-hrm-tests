# NM-HRM-008 — playfield-1 collision depends on playfield 2, by design, undocumented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Read *What the others say* before deciding: every implementation read here does what NanoMig does, and WinUAE's changelog calls p. 214's sentence wrong. |
| **Module** | `denise_collision.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L62-L63) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's reading is accepted. `denise_collision.v` is identical in NanoMig, MiSTer and TC64, so there is no fix to port; no implementation read here follows pp. 214 and 273 in single-playfield mode — see *What the others say* |
| **Test** | [`hrm/ch3-collision-registers`](../../hrm/ch3-collision-registers) · [`hrm/ch7-collision-playfield-mode`](../../hrm/ch7-collision-playfield-mode) |
| **Chapter** | 3 — Playfield, and 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **unanimous with NanoMig, against pp. 214 and 273**: WinUAE, vAmiga, Denise and Copperline all register a playfield-1 collision in single-playfield mode only where playfield 2's enabled planes match as well. Copperline's expression is NanoMig's term for term; WinUAE's changelog calls p. 214's sentence wrong and vAmiga's comment calls the gate a hardware oddity; vAmiga also applies it in dual-playfield mode where playfield 1 is opaque and shown |

---

## What the HRM says

**The manual states the rule three times and conditions it on no mode.**

**Chapter 7, printed p. 214**, below Table 7-3, whose bits 1 to 4 read "Odd
bitplanes to sprite 0 (or 1)" to "Odd bitplanes to sprite 6 (or 7)" and bits
5 to 8 the same for the even bitplanes:

> Notice that in this table, collision detection does not change when you
> select either single- or dual-playfield mode. Collision detection depends
> only on the actual bits present in the odd-numbered or even-numbered
> bitplanes.

**Appendix A, printed p. 273**, `CLXDAT`, the note above the bit table:

> NOTE: Playfield 1 is all odd-numbered enabled bitplanes. Playfield 2 is all
> even-numbered enabled bitplanes

The table under it gives each pair its own bit: 01 to 04 "Playfield 1 to
sprite 0 (or 1)" to "Playfield 1 to sprite 6 (or 7)", 05 to 08 the same for
playfield 2, 00 "Playfield 1 to playfield 2".

**Appendix A, printed p. 272**, `CLXCON`, says which planes take part:

> NOTE: Disabled bitplanes cannot prevent collisions. Therefore if all
> bitplanes are disabled, collisions will be continuous, regardless of the
> match values.

The pages around these do not qualify them. p. 213 introduces collisions
between "any sprite group and either of the playfields"; pp. 209-212 set
display priority, and the one sentence there that names single-playfield mode
is about priority, not collision (p. 210):

> PF2P2 - PF2P0, bits 5-3, are the priority bits for normal (non-dual)
> playfields.

No figure in chapter 7 draws the collision logic, and Appendix C does not
mention collision. No printed page says what NanoMig does.

---

## What NanoMig does

[`src/minimig-aga/denise_collision.v#L62-L63`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L62-L63)

```verilog
assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf | evenmatch);
assign evenmatch = bm[7] & bm[5] & bm[3] & bm[1];
```

`dblpf` is BPLCON0 bit 10, the dual-playfield bit
([`src/minimig-aga/denise.v#L183`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L183)).
With `dblpf = 0` — single playfield — `oddmatch` additionally requires
`evenmatch`: playfield 1 registers a collision only where playfield 2's
enabled planes match as well. An even plane that `CLXCON` does not enable
counts as matched, as p. 272's note has it
([`denise_collision.v#L55`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L55)
ORs in the inverted enable bit), so the gate shows only when at least one
even plane is enabled and differs from its match value.

**The RTL says why, in the comment directly above it:**

> this is the implementation of an undocumented function in the real Denise chip,
> developed by Yaqube. trigger was the game Rotor. mentioned in WinUAE sources to
> be the only known game needing this feature. it also fixes the Spaceport
> instandly helicopter crash at takeoff and Archon-1 'sticky effect' of player
> sprite at the battlefield.

The WinUAE source the comment points to names Rotor for a different bit: its
changelog says a game called Rotor "does require working CLXDAT collision bit 0
(="full collision")"
([`od-win32/winuaechangelog.txt:18408-18410`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L18408-L18410)),
the playfield-1-to-playfield-2 bit, which needs both playfields in either mode.
The single-playfield gate is a separate entry, quoted under *What the others
say*.

### Measured

[`hrm/ch3-collision-registers`](../../hrm/ch3-collision-registers) presents
playfield 1 with each sprite pair, `dblpf` at 0, bitplane 1 enabled and matched
for playfield 1 and bitplane 2 enabled and unmatched for playfield 2: p. 273's
table has each pair set its own bit, and NanoMig sets none —
`BITMAP-PAIR PF1-SPR01`, `PF1-SPR23`, `PF1-SPR45` and `PF1-SPR67` are red,
four of the package's `32 passed, 4 failed`. `BITMAP-PAIR PF1-PF2`, where
playfield 2 matches too, and `PLAYFIELD-ODD-EVEN`, where no even plane is
enabled, stay green.

[`hrm/ch7-collision-playfield-mode`](../../hrm/ch7-collision-playfield-mode)
runs the same `CLXCON` setting and stimuli at `dblpf` 0 and 1. `CLXDAT & 0x7FFF` reads `$0000` in
single-playfield mode and `$0002`, `$0004`, `$0008` and `$0010` in
dual-playfield mode for the four playfield-1 pairs, so
`A1-PLAYFIELD-MODE-INVARIANCE PF1-SPR01` to `PF1-SPR67` are red against p. 214,
and `A3-PLAYFIELD-1-IS-ODD-BITPLANES dblpf=0` is red against p. 273 —
`16 passed, 5 failed`. The other eleven pairs of `A1`, `A2`, `A3` at `dblpf=1`
and `A4` in both modes are green.

Both packages can go green: with `(dblpf | evenmatch)` removed from line 62
and nothing else changed, they read `36 passed, 0 failed` and
`21 passed, 0 failed`. No package in the suite asserts the gate: the other
four that build `denise_collision.v` — `hrm/ch7-collision-model`,
`hrm/ch4-sprite-collision`, `aa/aa-collision-data-register` and
`aa/aa-clxcon2-address` — are green with it and without it.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:4518-4531`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4518-L4531) | PF1 gated by PF2: with `bpldualpf` clear — BPLCON0 bit 10 as Denise holds it, `bpldualpf = (bplcon0_denise & 0x400) == 0x400;` ([`drawing.cpp:2855`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2855)) — the odd-plane test `if ((apixel & clxcon_bpl_enable_55) == clxcon_bpl_match_55)` is reached only inside the even-plane one. Its reason is in its changelog: `"collision detection does NOT change when you select either single- or dual-playfield mode" says HRM. BZZZT, WRONG!`, an entry that `Fixes Inferior and Sub Rally` ([`od-win32/winuaechangelog.txt:19144-19146`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L19144-L19146)). Sprite–bitplane collisions need `currprefs.collision_level > 1`; the default is 3 ([`cfgfile.cpp:8642`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L8642)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:1554-1560`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1554-L1560) | PF1 gated by PF2: *"There is a hardware oddity in single-playfield mode. If PF2 doesn't match, PF1 doesn't match either."*, citing a forum post, then `if (!(zBuffer[pos] & Z_DPF)) continue;`. A single-playfield pixel carries no dual-playfield code (`zBuffer[i] = s ? state.zpf2 : 0;`, [`Denise.cpp:829`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L829)), so there the test always skips PF1 when PF2 fails. `Z_DPF` is `0x1` and the dual-playfield codes are numbered 1 to 5 ([`Denise.h:494-499`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L494-L499)), so it also skips PF1 in dual-playfield mode at pixels coded `Z_DPF1` (2) and `Z_DPF12` (4), where PF1 is opaque and shown. The sprite–playfield check runs only under `if (config.clxSprPlf)` ([`Denise.cpp:1226`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1226)), an option that defaults to `false` ([`Core/Infrastructure/Defaults.cpp:174`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L174)) |
| **Denise** | [`emulation/libami/video/denise.cpp:478-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L478-L480) | <pre>if (!clxNoCol)&nbsp;clxDat \|= (v1 &lt;&lt; 5) \| (v1 &lt;&lt; 1) \| 1;<br>else if ((clxNoCol & 2) == 0)&nbsp;clxDat \|= (v1 &lt;&lt; 5);<br>else if (((clxNoCol & 1) == 0) && _doublePlayfield)&nbsp;clxDat \|= (v1 &lt;&lt; 1);</pre> `clxNoCol` bit 0 is the odd planes' mismatch and bit 1 the even planes' ([`denise.cpp:652-653`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L652-L653); shifter A takes ENBP1, ENBP3 and ENBP5, [`denise.cpp:226`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L226)), so the PF1 group without a PF2 match is set only when `_doublePlayfield`. No comment gives a reason |
| **Copperline** | [`src/video/bitplane.rs:6797`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L6797) | <pre>let odd_match = odd_match_raw && (dual_playfield \|\| even_match);</pre> the same expression on the live `CLXDAT` read path ([`src/bus.rs:12281`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L12281)); the comment on the plane match beside it names vAmiga's `checkS2PCollisions` as its reference ([`src/video/bitplane.rs:6812-6817`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L6812-L6817)) |
| **MiSTer** | [`rtl/denise_collision.v:62-63`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) | descendant — byte-identical: `assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf \| evenmatch);` gates PF1 on PF2 exactly as NanoMig does |
| **TC64** | [`rtl/minimig/denise_collision.v:62-63`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L62-L63) | descendant — the same two lines, unchanged. The whole file is identical on both lines; there is no divergence anywhere to fix, so changing NanoMig here means diverging from both ancestors at once |

**All four independent implementations gate playfield 1 on playfield 2 in
single-playfield mode**, and two say why: WinUAE names p. 214's sentence and
calls it wrong, and vAmiga calls the gate a hardware oddity. None cites a
Commodore document for it. Copperline's line is NanoMig's expression term for
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

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 7 — agrees with the manual:

  > "NOTE: Playfield 1 is all odd numbered enabled bit planes."
  <!-- corpus: denise-spec-252126/p007.txt -->

  Commodore's 1988 ECS Denise specification repeats the note the manual prints in appendix A, and its CLXDAT table lists bits 01-04 as 'Playfield 1 to Sprite 0 (or 1)' and so on with no mode condition; nothing in the 14 pages makes the PF1 bit depend on PF2 or on DPF. Table-level only, like appendix A: the chapter-7 sentence about single vs dual playfield is not in this document.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 29 — agrees with the manual:

  > "Fifteen bits in the collision storage register are utilized to indicate whether any of several types of overlaps has occurred: e.g., one sprite to another sprite, even bitplanes to odd bitplanes, even bitplanes to a certain sprite, or odd bitplanes to a certain sprite."
  <!-- corpus: patent-us4874164-agnus-denise/p029.txt -->

  Describes the odd-bitplanes-to-sprite bit as registering an overlap of the odd bitplanes with a sprite, with no playfield-mode condition; p021 likewise defines objects as 'any non-zero bits used to display any of eight sprites or two playfields'. Weak: this is the table-level statement again and says nothing about single vs dual playfield; the FIG. 12 gate diagram (p011) is illegible in the text layer.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988.

---

## Suggested change

If the manual's reading is accepted, drop the playfield-2 term from line 62:

```verilog
assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0];
```

That one line turns all nine reds green and leaves every other assertion in
the six packages that build `denise_collision.v` green (*Measured*, above). It also makes NanoMig the
only implementation read here that registers a playfield-1 collision without
playfield 2 in single-playfield mode, and it removes the behaviour the RTL
comment ties to Rotor, Spaceport and Archon-1 and WinUAE's changelog to
Inferior and Sub Rally.

The other option is to keep the RTL and read the gate as what Denise does; the
manual would then need a single-playfield condition on pp. 214 and 273 that it
does not print, and this project does not own that document.

---

## For the specialists

The question is which side describes the silicon. For the manual: three
printed statements that agree with each other — p. 214's sentence, Table 7-3's
"Odd bitplanes" rows, and p. 273's note and table — and Commodore's 1988 ECS
Denise specification, which repeats the note and the table; no printed page
makes the playfield-1 bit depend on the playfield mode. For NanoMig: all four
independent implementations, WinUAE's changelog naming p. 214's sentence as
wrong and the two games it fixed, vAmiga's comment and the forum post it cites,
and the RTL comment crediting a reverse-engineered Denise function. None of the
implementations cites a Commodore document for the gate.

The red stands either way: the manual is this suite's oracle, and NanoMig does
not do what its three statements say.
