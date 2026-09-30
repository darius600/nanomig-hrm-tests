# NM-HRM-016 — SuperHires sprite colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Each of the package's fifteen reds grades one row of p. 302's table at its bits' own places, and a private copy of the RTL that shows COLOR16–COLOR31 as the table prints them turns all fifteen green (*What the test measures*). |
| **Module** | `denise_colortable.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 15 red, one per row of p. 302's table: `REG-0x11`, `REG-0x12`, `REG-0x13`, `REG-0x14`, `REG-0x15`, `REG-0x16`, `REG-0x17`, `REG-0x18`, `REG-0x19`, `REG-0x1A`, `REG-0x1B`, `REG-0x1C`, `REG-0x1D`, `REG-0x1E`, `REG-0x1F` (`4 passed, 15 failed, 2 declined`). `REG-0x10` and `DOT-BITS` are declined, `P1` and three Hires controls are green, and `P2` is printed, not graded (*What the test measures*) |
| **Verdict of the four** | **one of four** reads `$10`–`$1F` as the page describes (WinUAE, ECS Denise only); vAmiga pair-encodes only the lower sixteen registers; Denise and Copperline do not pair-encode; outside the emulators, the 8373R4 dissection indexes the upper sixteen registers by two sprite pixels and shows two bits per component, as WinUAE does — see *What other Commodore documents say* |
| **See also** | [NM-HRM-017](NM-HRM-017.md), the bitplane half of the same mechanism |

---

## What the HRM says

**Printed p. 301** limits a SuperHires pixel to two bits per component:

> SuperHires mode has a coarser granularity of color control than either Hires
> or Lores modes. ... There are only two bits of red, green and blue color
> resolution per hires pixel.

> In order to decode sprite and bitplane color information in SuperHires mode,
> certain multiplexing occurs in the use of the registers.

**Printed p. 302** gives the sprite half of the scheme:

> SuperHires sprites are encoded in the upper sixteen color registers using a
> similar scheme:

The key names each sprite colour's two bits per component, and the table places
them in the registers (the key and three of the sixteen rows):

```
                            R    G    B
      Sprite (Color 16) :   AB-- CD-- EF--
      Sprite (Color 17) :   GH-- IJ-- KL--
      Sprite (Color 18) :   MN-- OP-- QR--
      Sprite (Color 19) :   ST-- UV-- WX--

      BIT  15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
   O  11   .   .   .   .   G   H   A   B   I   J   C   D   K   L   E   F
   R  14   .   .   .   .   A   B   G   H   C   D   I   J   E   F   K   L
      15   .   .   .   .   G   H   G   H   I   J   I   J   K   L   K   L
```

Row `$1n` carries Colour 16 + (*n* mod 4) in bits 11–10, 7–6 and 3–2 and
Colour 16 + (*n* div 4) in bits 9–8, 5–4 and 1–0. Row `$11` holds Colour 17 in
its upper bit pairs and Colour 16 in its lower ones; row `$15` holds Colour 17
alone. The page does not say which register, or which half of it, a sprite
pixel reads. It closes:

> Keep in mind that because of the loss of lower bits of precision in specifying
> SuperHires colors, pastel colors in a closely graduated color scheme may be
> visually difficult to distinguish from each other.

Colour 16 is what sprite value 00 selects, and chapter 4 makes that
transparent:

> The color value of the first register in each group of four registers is
> ignored by sprites. When the sprite bits select this register, the
> "transparent" value is used.

(p. 103; p. 108: "Recall that binary 00 always means transparent and never
refers to a color except background"). No page describes attached sprites, or
sprites 2 to 7, in SuperHires.

---

## What NanoMig does

It reads them as ordinary colour registers in every mode.

**Root cause, verified by reading the RTL:** `denise_colortable.v` has **no
`shres` or `spres` port at all**. Its whole interface is mode-blind (the port
list, condensed from its declaration):

```verilog
module denise_colortable (
  clk, clk7_en, reg_address_in, data_in, rdram,
  select, bplxor, bank, loct, ehb_en, rgb );   // no shres, no spres
```

`spres` reaches the sprite path in exactly one place —
[`denise_sprites.v:158-166`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L158-L166) — where it sets `shift`, the
shift-register drain rate. That changes how **wide** a SuperHires sprite is,
never what colour it is.

So with COLOR16–COLOR31 loaded as the table prescribes, a sprite-0 pixel of
value 01 shows all twelve bits of COLOR17
([`denise_colortable.v:56-58`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v#L56-L58)):
Colour 17's letters in the upper two bits of each nibble and Colour 16's in the
lower two (red `G H A B`), and likewise for values 10 and 11 through COLOR18
and COLOR19.

### Measured

At the system boundary,
[`integration/ch4-shres-sprite-a`](../../integration/ch4-shres-sprite-a) loads
COLOR16–COLOR31 as the table prescribes, in five bands in which Colour 16 takes
four different values, and records bits 1–0 of every colour-17, -18 and -19
pixel without grading them. In all fifteen groups, on both CPU cores, they are
Colour 16's letters (`A B`, `C D`, `E F`); band 0's Colour 16 is (01, 10, 11),
band 1's (10, 11, 00):

```
MEASURED: group 0 bits1-0 (section 3.7, ungraded): R=[1,1] G=[2,2] B=[3,3]
MEASURED: group 3 bits1-0 (section 3.7, ungraded): R=[2,2] G=[3,3] B=[0,0]
```

Bits 3–2 carry each colour's own two letters, which that package grades green
(P2–P4), as
[`integration/ch4-shres-sprite-b`](../../integration/ch4-shres-sprite-b) does
for the same letters and for the absence of Colour 16's letters from bits 3–2
(P1–P4). Neither package grades bits 1–0: both specifications read the key's
`--` as leaving them without an expected value. Four register bits, two of them
Colour 16's, decide each component of NanoMig's SuperHires sprite pixel, where
p. 301 gives it two.

The unit package flips one register bit at a time and reads which colour and
channel moves, and where. In NanoMig bit 0 of COLOR17, Colour 16's blue letter
`F`, moves Colour 17's blue; bit 0 of COLOR21, Colour 17's `L`, moves Colour
17's blue, but not in the top two bits where the key puts `L`:

```
MEASURED: REG-0x11: bit 0, run S1, tick 9: observed colour 17, page-predicted colour 16, component(s) moved: B.
MEASURED: REG-0x15: bit 0, run S1-SPR2, tick 9: observed colour 17, page-predicted colour 17, component(s) moved: B.
```

---

## What the test measures

[`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding)
loads COLOR16–COLOR31 as p. 302's table packs four fixed palettes, sets
SuperHires, and flips each register's twelve colour bits in turn, sampling the
output on every 28 MHz clock. A flip must move the colour and channel its row
prints, in that channel's top two bits (p. 301: "There are only two bits of
red, green and blue color resolution per hires pixel"). No page says which
register a sprite pixel reads, so the registers are reached as read paths only:
sprite 0 for COLOR17–COLOR19, sprites 2, 4 and 6 for the rest of each group
(p. 103, Figure 4-6), and an attached pair for COLOR20, COLOR24 and COLOR28.

| assertion (`test_denise.cpp`) | NanoMig | p. 302 copy | what it grades |
|---|---|---|---|
| `REG-0x11` to `-0x13` (`:1176-1207`), `REG-0x14` to `-0x1F` (`:1215-1252`) | red | green | one row of the table each, at its bits' own places |
| `P1` (`:967-1025`) | green | green | colours 17 to 19 each show their own two-bit triple in the top two bits, read through COLOR21, COLOR26 and COLOR31, which hold that colour in both halves |
| `C1`, `C2`, `C3` (`:863-965`) | green | `C3` red | Hires controls: the sprite path is live, value 00 is transparent, a register reads as 0RGB outside SuperHires |
| `REG-0x10` (`:1158-1169`) | declined | declined | row `$10` holds only Colour 16, which no sprite pixel shows (p. 103, p. 108) |
| `DOT-BITS` (`:1109-1156`) | declined | declined | bits 15–12, the table's `.`, are dropped by every colour-register write, not by the table |
| `P2` (`:1027-1107`) | printed | printed | 18 of 54 traces change, each on a Colour 16 letter; not graded, since the copy reads the same |

**The implementation control.** A private copy of `denise_colortable.v`,
outside the repository, shows a COLOR16–COLOR31 read as p. 302 prints it:
alternate 28 MHz samples carry the register's upper and lower bit pairs, each
in the channel's top two bits. On it the package reads `18 passed, 1 failed,
2 declined`: the fifteen rows turn green, and `C3` goes red because the copy
shows those registers that way outside SuperHires too.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:4622-4638`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4622-L4638) | <pre>uae_u8 off = ((p1 & 3) * 4) + (p0 & 3) + ((p0 \| p1) & 16);</pre> reinterprets, ECS Denise only ([`:6470`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L6470)): a pair of sprite pixels selects one of the upper sixteen registers |
| **vAmiga** | [`Core/Components/Denise/PixelEngine.h:301-305`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.h#L301-L305) | <pre>return ((mbuf[base + 2] & 3) * 4) + (mbuf[base] & 3);</pre> pair-encodes, ECS Denise only, but from the lower sixteen registers alone — *"Only the first 16 registers are reachable in that mode"* ([`PixelEngine.h:114-120`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.h#L114-L120)); a sprite pixel's index is cut to its low two bits and looked up there, never in `$10`–`$1F` |
| **Denise** | [`emulation/libami/video/denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | <pre>colors[pos] = value;</pre> one table, no mode |
| **Copperline** | [`src/video/bitplane/sprite.rs:121-146`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L121-L146) | *"pre-AGA uses the classic 16..31 block"* — a sprite's colour is one register in every mode; the test that rules out pair encoding ([`src/video/bitplane/tests.rs:7563`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L7563)) is about the playfield, see [NM-HRM-017](NM-HRM-017.md) |
| **MiSTer** | [`rtl/denise_colortable.v:32-35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L32-L35) | descendant — no `shres`/`spres` port, same as NanoMig; the 11-line diff there is a RAM primitive's word width growing from 12/24 bits to 32 with byte-enables, not a colour-mode change. `rtl/denise_sprites.v:110-115` has the identical `case (spres)` block, still only driving `shift` |
| **TC64** | [`rtl/minimig/denise_colortable.v:32-35`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L32-L35) | descendant — the identical 11 lines; MiSTer's and TC64's diffs from NanoMig are themselves byte-identical to each other, same primitive, same edit. `denise_sprites.v` likewise: `rtl/minimig/denise_sprites.v:110-115` — the same three-line case. Neither line gives `denise_colortable` a mode input or makes `denise_sprites` pick a colour slot from `spres`; a fix is new work on both lines, in both places |

**One of the four independent implementations reads the upper sixteen
registers as the page describes: WinUAE, on ECS Denise only.** vAmiga
pair-encodes SuperHires colour from the lower sixteen registers alone and looks
a sprite pixel up there; Denise stores one colour per register in every mode;
Copperline takes a sprite's colour from one register of the classic sprite
block in every mode. MiSTer and TC64 are mode-blind in the identical way
NanoMig is, which is not another reading — a descendant with no `shres` port
inherited that absence, it did not arrive at it independently.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 8c, 9a, 6j1, 6k and 9h — agrees with the manual:

  *Read from the drawing, not quoted:* in SuperHires the colour-index PLA on sheet 8c sets CNA1 and CNA0 from the B and A bits of channels 0 to 3, which carry the first of the two 70 ns pixels a sprite shows per 140 ns cycle, S0_AF to S3_AF (sheet 6j1), and CNA3 and CNA2 from channels 4 to 7, which in SuperHires carry the second, S0_AG to S3_AG (sheet 6k). The rows that put the sprite pair's number into CNA3 and CNA2 act only outside SuperHires, and no SuperHires row has an attach term. CNA0 to CNA3 select one of rCOLOR16 to rCOLOR31, the sixteen upper registers (sheet 9a). In SuperHires the output stage of sheet 9h joins red outputs R0 with R2 and R1 with R3 and drives them from register bits 11 and 10 in one 70 ns half and from bits 9 and 8 in the other.
  <!-- drawing: denise-8373r4-dissection/sheet_059.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_071.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_036.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_079.txt -->

  A SuperHires sprite reads COLOR(16 + 4 × second + first) for each pair of its pixels and shows each pixel's two bits per component from one half of that register: the arrangement of p. 302's table, and WinUAE's pair lookup (*What the others say*). A solid run of value 01 reads COLOR21, whose row the table fills with Colour 17 alone. As drawn, sprites 4 to 7 have no channel of their own in SuperHires. The PLA and the read decoder are read from their drawn crosspoints, not from a netlist; the output stage is traced in the netlist for red, the one component drawn in full, and green and blue draw only their bit 0. The dissection is a work in progress, and of one revision.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; US patent 5,103,499, Commodore-Amiga, filed 1989 as a division of the 1986 application, granted 1992.

---

## Suggested change

The feature is absent rather than wrong, and adding it is not a one-line change:
`denise_colortable` would need a mode input it does not have, and the sprite path
would need to show a pixel's colour from the half of a register that holds it,
two bits per component.

**Three of four reference implementations do not do this**; vAmiga pair-encodes
only the lower sixteen registers. So the first question is not how to implement
it but whether ECS Denise really does — and that is what WinUAE, which
reinterprets for ECS Denise only, is evidence for.

The 8373R4 dissection answers that question for one revision: in SuperHires it
indexes the upper sixteen registers by two sprite pixels and shows each pixel's
two bits per component (*What other Commodore documents say*). It is a
third-party reading of that chip's silicon, and a work in progress, not a
Commodore document.
