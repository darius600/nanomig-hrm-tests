# NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **all four implement it**, and on ECS all four confine its effect on position to SuperHires, as the page's Warning states |
| **Origin** | the surviving half of the withdrawn [NM-HRM-013](NM-HRM-013.md) |

---

## What the HRM says

**Printed p. 303** gives bit 4 a name *and a behaviour*, which is what makes this
a requirement rather than a mention:

```
04      SHSH1     Start horizontal (SHR mode) 70ns increment
```

scoped by the page's own Warning — *"70ns sprite positions are only available in
SuperHires mode"* — and by the ECS note.

**Compare bit 3 on the same page**, which the manual prints as `(unimplemented)`.
That contrast is the whole reason this half survived and the other did not.

---

## What NanoMig does

Nothing with it. Bit 4 is not read in the sprite path.

### Measured

[`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) sets SPR0CTL bit
4 alone, all other bits including SH0 held at 0, and checks whether the full
`sprdata` trace across `hpos` 0..511 changes, which is what the row promises
— `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE`, red, one of the package's `10
passed, 1 failed`.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2825-2836`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2825-L2836) | <pre>} else if (ecs_denise) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (s-&gt;ctl & 0x10) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sprxp \|= 2;</pre> bit 4 (`0x10`) from ECS Denise on; bit 3 (`0x08`) only on AGA. Outside SuperHires the match drops it ([`:5318`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5318), `sp->xpos & ~3`); in ECS hires it only clips the sprite's leftmost pixel ([`:5325-5329`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5325-L5329)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:171-173`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L171-L173) | <pre>(sprctl[x] & 0x10) &gt;&gt; 4);  // SHSH1 (ECS only)</pre> the lowest bit of the sprite's start; the start is then masked to the sprite pixel's width ([`:968`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L968), [`DeniseRegs.cpp:854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L854)), which on OCS and ECS keeps the bit only in SuperHires |
| **Denise** | [`emulation/libami/video/denise.cpp:299`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L299) | <pre>spr.shsh10 = (value & 0x10) &gt;&gt; 3;</pre> compared only when `_shres` |
| **Copperline** | [`src/video/bitplane/sprite.rs:1857-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1857-L1859) | <pre>pub(super) fn sprite_hsub_70ns(ctl: u16) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;ctl & 0x0010 != 0<br>}</pre> |
| **MiSTer** | — | descendant — `rtl/denise_sprites.v` — the same absence: no `sprctl[4]`, `SHSH1` or `shsh1` anywhere in the file |
| **TC64** | — | descendant — `rtl/minimig/denise_sprites.v` — likewise absent. A fix is new work on both lines; neither reads bit 4 for NanoMig to have dropped |

**All four independent implementations implement it**, and on ECS all four
confine its effect on position to SuperHires, as p. 303's Warning states: Denise reads it
always and compares it only in SuperHires; vAmiga and WinUAE mask it out of the
start position outside SuperHires; Copperline adds it only while SHRES is
set.
MiSTer and TC64 have never read the bit either, which is the same absence
NanoMig has, not a vote against it.

WinUAE's decode is the clearest statement of the ECS-versus-AGA split: `0x10` is
read from ECS Denise onwards, `0x08` only on AGA. That is the split the
withdrawn [NM-HRM-013](NM-HRM-013.md) was broken into.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — **agrees with NanoMig**:

  > "07 ATT Sprite attach control bit (odd Sprites) 06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  In Commodore's ECS Denise specification (it defines SHRES and names the 8373 revisions) the SPRxCTL table gives bits 06-04 as 'Not used' and has no row for bit 3; SHSH1 and 70 ns sprite positioning appear nowhere in its 14 pages. That sides with NanoMig not reading bit 4. Caveat: the table has the shape of an OCS-era SPRxCTL table, without even the EV8 row, so it may have been carried over rather than written for ECS.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; US patent 5,103,499, Commodore-Amiga, filed 1989 as a division of the 1986 application, granted 1992.

---

## Suggested change

Read bit 4 into the sprite's horizontal position as a 70 ns sub-position, and
apply it only in SuperHires. Denise's one-liner plus its `_shres` guard is the
smallest complete model; Copperline's named predicate is the tidiest.

```verilog
wire sprite_hsub_70ns = sprctl[4];   // SHSH1, appendix p.303 -- SuperHires only
```

with the position arithmetic taking it only when the sprite resolution is
SuperHires.

---

## Why this entry exists, and what it replaced

[NM-HRM-013](NM-HRM-013.md) asserted that `SPRxCTL` bits 3 **and** 4 are live fields, resting on
the note *"bits 3 and 4 are in the ECS chips only"* — a statement about **which
chips have them**, not about what they do. That is not a requirement, and the
finding was withdrawn.

The two bits then part company:

| bit | what the manual prints | outcome |
|---|---|---|
| 3 | `(unimplemented)` | the manual settles it in the RTL's favour |
| 4 | `SHSH1  Start horizontal (SHR mode) 70ns increment` | a behaviour — this finding |

The withdrawal was right and the split is what it produced.
