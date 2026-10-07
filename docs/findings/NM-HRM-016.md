# NM-HRM-016 — SuperHires sprite colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should NanoMig's Denise, in SuperHires, show sprite colours with two bits per component as p. 302's table encodes them in COLOR16–COLOR31, instead of reading those registers as ordinary twelve-bit colours? *Suggested change* under *Decision* states one option; the decision belongs to an Amiga specialist, not to this repository. It closes when the specialist decides on the evidence below or the observation named in *Risks and unknowns* is made. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer: the code is upstream's (the MiSTer row below shows the same code); a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Scope** | NanoMig's Denise colour table and sprite path in SuperHires, COLOR16–COLOR31; isolated through sprite 0 (COLOR17–COLOR19), sprites 2, 4 and 6, and an attached pair (COLOR20, COLOR24, COLOR28); the Denise revision the page describes is `unknown` |
| **Software impact** | `unknown; no named affected software` |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991, p. 301–302 |
| **Module** | `denise_colortable.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) (inherited) |
| **Test** | [`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding): `make -C hrm/ch3-shres-sprite-colour-encoding test`; the fifteen reds are expected |
| **Assertions** | 15 red, one per row of p. 302's table: `REG-0x11`, `REG-0x12`, `REG-0x13`, `REG-0x14`, `REG-0x15`, `REG-0x16`, `REG-0x17`, `REG-0x18`, `REG-0x19`, `REG-0x1A`, `REG-0x1B`, `REG-0x1C`, `REG-0x1D`, `REG-0x1E`, `REG-0x1F` (`4 passed, 15 failed, 2 declined`). Expected: a flipped register bit moves the colour and channel its row prints; observed, for `REG-0x11` bit 0: colour 17 moves, page-predicted colour 16, blue. `REG-0x10` and `DOT-BITS` are declined, `P1` and three Hires controls are green, and `P2` is printed, not graded |
| **Verdict of the four** | **one of four** reads `$10`–`$1F` as the page describes (WinUAE, ECS Denise only); vAmiga pair-encodes only the lower sixteen registers; Denise and Copperline do not pair-encode; outside the emulators, the 8373R4 dissection indexes the upper sixteen registers by two sprite pixels and shows two bits per component, as WinUAE does |
| **Effort** | `M` — `denise_colortable.v` needs a mode input it does not have, and `denise_sprites.v` must show a pixel's colour from the register half that holds it |
| **See also** | [NM-HRM-017](NM-HRM-017.md), the bitplane half of the same mechanism |

---

## Source requirement

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
alone. Colour 16 is what sprite value 00 selects, and chapter 4 makes that
transparent:

> The color value of the first register in each group of four registers is
> ignored by sprites. When the sprite bits select this register, the
> "transparent" value is used.

(p. 103; p. 108: "Recall that binary 00 always means transparent and never
refers to a color except background").

---

## RTL and measurement

### What NanoMig does

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

### What the test measures

[`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding)
loads COLOR16–COLOR31 as p. 302's table packs four fixed palettes, sets
SuperHires, and flips each register's twelve colour bits in turn, sampling the
output on every 28 MHz clock. A flip must move the colour and channel its row
prints, in that channel's top two bits (p. 301).

| assertion (`test_denise.cpp`) | NanoMig | what it grades |
|---|---|---|
| `REG-0x11` to `-0x13` (`:1176-1207`), `REG-0x14` to `-0x1F` (`:1215-1252`) | red | one row of the table each, at its bits' own places |
| `P1` (`:967-1025`) | green | colours 17 to 19 each show their own two-bit triple in the top two bits, read through COLOR21, COLOR26 and COLOR31, which hold that colour in both halves |
| `C1`, `C2`, `C3` (`:863-965`) | green | Hires controls: the sprite path is live, value 00 is transparent, a register reads as 0RGB outside SuperHires |
| `REG-0x10` (`:1158-1169`) | declined | row `$10` holds only Colour 16, which no sprite pixel shows (p. 103, p. 108) |
| `DOT-BITS` (`:1109-1156`) | declined | bits 15–12, the table's `.`, are dropped by every colour-register write, not by the table |
| `P2` (`:1027-1107`) | printed | 18 of 54 traces change, each on a Colour 16 letter; not graded |

---

## Risks and unknowns

- **Which register half a pixel reads.** The page does not say which register,
  or which half of it, a sprite pixel reads (p. 302). The test therefore reaches
  the registers as read paths only: sprite 0 for COLOR17–COLOR19, sprites 2, 4
  and 6 for the rest of each group (p. 103, Figure 4-6), and an attached pair for
  COLOR20, COLOR24 and COLOR28.
- **Attached sprites and sprites 2 to 7.** No page describes them in SuperHires.
- **Three of four implementations disagree with the page.** Only WinUAE, on ECS
  Denise only, reads the upper sixteen registers as it describes; vAmiga
  pair-encodes only the lower sixteen.
- **One third-party silicon revision.** The only reading of silicon is the
  6502.org dissection of Denise 8373R4: a work in progress, of one revision, not a
  Commodore document. Its PLA and read decoder are read from drawn crosspoints,
  not from a netlist; the output stage is traced in the netlist for red, the one
  component drawn in full, and green and blue draw only their bit 0. Not measured
  on other revisions or on hardware.
- **Observation needed before implementing.** The first question is not how to
  implement the encoding but whether ECS Denise really does it. No capture from
  ECS Denise hardware showing which register a SuperHires sprite pixel reads is
  in this repository; the effect on any named software is `unknown`.

---

## Independent evidence

| | code | reading |
|---|---|---|
| **WinUAE** | [`drawing.cpp:4622-4638`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4622-L4638) | reinterprets, ECS Denise only: a pair of sprite pixels selects one of the upper sixteen registers |
| **vAmiga** | [`PixelEngine.h:301-305`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.h#L301-L305) | pair-encodes, ECS Denise only, from the lower sixteen registers alone; a sprite pixel is never looked up in `$10`–`$1F` |
| **Denise** | [`denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | one colour table, no mode |
| **Copperline** | [`sprite.rs:121-146`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L121-L146) | a sprite's colour is one register in every mode |
| **MiSTer** | [`denise_colortable.v:7-20`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L7-L20) | descendant — no `shres`/`spres` port, same as NanoMig; its sprite module has the identical `spres` case, still only driving `shift` |
| **TC64** | [`denise_colortable.v:7-20`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L7-L20) | descendant — the same lines |

**One of the four independent implementations reads the upper sixteen
registers as the page describes: WinUAE, on ECS Denise only.** vAmiga
pair-encodes SuperHires colour from the lower sixteen registers alone and looks
a sprite pixel up there; Denise stores one colour per register in every mode;
Copperline takes a sprite's colour from one register of the classic sprite
block in every mode. MiSTer and TC64 are mode-blind in the identical way
NanoMig is, which is not another reading — a descendant with no `shres` port
inherited that absence, it did not arrive at it independently.

**6502.org dissection of Denise 8373R4, undated — third-party, one silicon revision**, schematic sheets 8c, 9a, 6j1, 6k and 9h — agrees with the manual. *Read from the drawing, not quoted:* in SuperHires a sprite reads COLOR(16 + 4 × second + first) from the upper sixteen registers for each pair of its pixels and shows two bits per component from one half of that register: the arrangement of p. 302's table, and WinUAE's pair lookup. As drawn, sprites 4 to 7 have no channel of their own in SuperHires. Its limits are in *Risks and unknowns*.
<!-- drawing: denise-8373r4-dissection/sheet_059.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_071.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_036.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_079.txt -->

---

## Decision

**Suggested change:** the feature is absent rather than wrong, and
adding it is not a one-line change: `denise_colortable` would need a mode input
it does not have, and the sprite path would need to show a pixel's colour from
the half of a register that holds it, two bits per component.

Closes when the specialist decides on this evidence, or when the observation
named in *Risks and unknowns* is made; to re-run the measurement,
`make -C hrm/ch3-shres-sprite-colour-encoding test`.
