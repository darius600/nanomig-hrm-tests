# NM-HRM-016 — SuperHires sprite colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should NanoMig's Denise, in SuperHires, show sprite colours with two bits per component as p. 302's table encodes them in COLOR16–COLOR31, instead of reading those registers as ordinary twelve-bit colours? Closes when `denise_colortable.v` gains a SuperHires mode input and the sprite path shows each pixel's colour from the register half that holds it, turning `REG-0x11`, `REG-0x12`, `REG-0x13`, `REG-0x14`, `REG-0x15`, `REG-0x16`, `REG-0x17`, `REG-0x18`, `REG-0x19`, `REG-0x1A`, `REG-0x1B`, `REG-0x1C`, `REG-0x1D`, `REG-0x1E` and `REG-0x1F` green, or when an Amiga specialist rejects the change on a capture from ECS Denise hardware (the observation under *Risks and unknowns*), in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's Denise colour table and sprite path in SuperHires, COLOR16–COLOR31; isolated through sprite 0 (COLOR17–COLOR19), sprites 2, 4 and 6, and an attached pair (COLOR20, COLOR24, COLOR28); the Denise revision the page describes is `unknown` |
| **Software impact** | `unknown; no named affected software` |
| **Chapter** | Appendix C, printed pp. 301–302 (*SuperHires Mode and Denise Color Registers*); chapter 4, printed pp. 103 and 108 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (22)</summary> `C1` carries no claim id; `C2` carries no claim id; `C3` carries no claim id; `P1`: `sol:C-shres-sprite-symbols.c4`, `sol:C-shres-sprite-symbols.c5`, `sol:C-shres-sprite-symbols.c6`, `sol:C-shres-sprite-symbols.c7`, `sol:C-shres-sprite-symbols.c8`, `sol:C-shres-sprite-symbols.c9`, `sol:C-shres-sprite-symbols.c10`, `sol:C-shres-sprite-symbols.c11`, `sol:C-shres-sprite-symbols.c12`, p. 302; `P2` carries no claim id; `DOT-BITS`: `sol:C-shres-sprite-bits.c17`, `sol:C-shres-sprite-bits.c18`, `sol:C-shres-sprite-bits.c19`, `sol:C-shres-sprite-bits.c20`, `sol:C-shres-sprite-bits.c33`, `sol:C-shres-sprite-bits.c34`, `sol:C-shres-sprite-bits.c35`, `sol:C-shres-sprite-bits.c36`, `sol:C-shres-sprite-bits.c49`, `sol:C-shres-sprite-bits.c50`, `sol:C-shres-sprite-bits.c51`, `sol:C-shres-sprite-bits.c52`, `sol:C-shres-sprite-bits.c65`, `sol:C-shres-sprite-bits.c66`, `sol:C-shres-sprite-bits.c67`, `sol:C-shres-sprite-bits.c68`, `sol:C-shres-sprite-bits.c81`, `sol:C-shres-sprite-bits.c82`, `sol:C-shres-sprite-bits.c83`, `sol:C-shres-sprite-bits.c84`, `sol:C-shres-sprite-bits.c97`, `sol:C-shres-sprite-bits.c98`, `sol:C-shres-sprite-bits.c99`, `sol:C-shres-sprite-bits.c100`, `sol:C-shres-sprite-bits.c113`, `sol:C-shres-sprite-bits.c114`, `sol:C-shres-sprite-bits.c115`, `sol:C-shres-sprite-bits.c116`, `sol:C-shres-sprite-bits.c129`, `sol:C-shres-sprite-bits.c130`, `sol:C-shres-sprite-bits.c131`, `sol:C-shres-sprite-bits.c132`, `sol:C-shres-sprite-bits.c161`, `sol:C-shres-sprite-bits.c162`, `sol:C-shres-sprite-bits.c163`, `sol:C-shres-sprite-bits.c164`, `sol:C-shres-sprite-bits.c177`, `sol:C-shres-sprite-bits.c178`, `sol:C-shres-sprite-bits.c179`, `sol:C-shres-sprite-bits.c180`, `sol:C-shres-sprite-bits.c193`, `sol:C-shres-sprite-bits.c194`, `sol:C-shres-sprite-bits.c195`, `sol:C-shres-sprite-bits.c196`, `sol:C-shres-sprite-bits.c209`, `sol:C-shres-sprite-bits.c210`, `sol:C-shres-sprite-bits.c211`, `sol:C-shres-sprite-bits.c212`, `sol:C-shres-sprite-bits.c225`, `sol:C-shres-sprite-bits.c226`, `sol:C-shres-sprite-bits.c227`, `sol:C-shres-sprite-bits.c228`, `sol:C-shres-sprite-bits.c241`, `sol:C-shres-sprite-bits.c242`, `sol:C-shres-sprite-bits.c243`, `sol:C-shres-sprite-bits.c244`, p. 302; `REG-0x10`: `opus:C-shrs.r10`, `sol:C-shres-sprite-bits.c1`, `sol:C-shres-sprite-bits.c2`, `sol:C-shres-sprite-bits.c3`, `sol:C-shres-sprite-bits.c4`, `sol:C-shres-sprite-bits.c5`, `sol:C-shres-sprite-bits.c6`, `sol:C-shres-sprite-bits.c7`, `sol:C-shres-sprite-bits.c8`, `sol:C-shres-sprite-bits.c9`, `sol:C-shres-sprite-bits.c10`, `sol:C-shres-sprite-bits.c11`, `sol:C-shres-sprite-bits.c12`, `sol:C-shres-sprite-bits.c13`, `sol:C-shres-sprite-bits.c14`, `sol:C-shres-sprite-bits.c15`, `sol:C-shres-sprite-bits.c16`, p. 302; `REG-0x11`: `opus:C-shrs.r11`, `sol:C-shres-sprite-bits.c21`, `sol:C-shres-sprite-bits.c22`, `sol:C-shres-sprite-bits.c23`, `sol:C-shres-sprite-bits.c24`, `sol:C-shres-sprite-bits.c25`, `sol:C-shres-sprite-bits.c26`, `sol:C-shres-sprite-bits.c27`, `sol:C-shres-sprite-bits.c28`, `sol:C-shres-sprite-bits.c29`, `sol:C-shres-sprite-bits.c30`, `sol:C-shres-sprite-bits.c31`, `sol:C-shres-sprite-bits.c32`, `sol:C-shres-color.c8`, `opus:C-shrs.c01`, `opus:C-shrc.srcs16`, `sol:C-shres-sprite-symbols.c1`, `sol:C-shres-sprite-symbols.c2`, `sol:C-shres-sprite-symbols.c3`, p. 302; `opus:C-shrc.c03`, p. 301; `REG-0x12`: `opus:C-shrs.r12`, `sol:C-shres-sprite-bits.c37`, `sol:C-shres-sprite-bits.c38`, `sol:C-shres-sprite-bits.c39`, `sol:C-shres-sprite-bits.c40`, `sol:C-shres-sprite-bits.c41`, `sol:C-shres-sprite-bits.c42`, `sol:C-shres-sprite-bits.c43`, `sol:C-shres-sprite-bits.c44`, `sol:C-shres-sprite-bits.c45`, `sol:C-shres-sprite-bits.c46`, `sol:C-shres-sprite-bits.c47`, `sol:C-shres-sprite-bits.c48`, p. 302; `REG-0x13`: `opus:C-shrs.r13`, `sol:C-shres-sprite-bits.c53`, `sol:C-shres-sprite-bits.c54`, `sol:C-shres-sprite-bits.c55`, `sol:C-shres-sprite-bits.c56`, `sol:C-shres-sprite-bits.c57`, `sol:C-shres-sprite-bits.c58`, `sol:C-shres-sprite-bits.c59`, `sol:C-shres-sprite-bits.c60`, `sol:C-shres-sprite-bits.c61`, `sol:C-shres-sprite-bits.c62`, `sol:C-shres-sprite-bits.c63`, `sol:C-shres-sprite-bits.c64`, p. 302; `REG-0x14`: `opus:C-shrs.r14`, `sol:C-shres-sprite-bits.c69`, `sol:C-shres-sprite-bits.c70`, `sol:C-shres-sprite-bits.c71`, `sol:C-shres-sprite-bits.c72`, `sol:C-shres-sprite-bits.c73`, `sol:C-shres-sprite-bits.c74`, `sol:C-shres-sprite-bits.c75`, `sol:C-shres-sprite-bits.c76`, `sol:C-shres-sprite-bits.c77`, `sol:C-shres-sprite-bits.c78`, `sol:C-shres-sprite-bits.c79`, `sol:C-shres-sprite-bits.c80`, p. 302; `REG-0x15`: `opus:C-shrs.r15`, `sol:C-shres-sprite-bits.c85`, `sol:C-shres-sprite-bits.c86`, `sol:C-shres-sprite-bits.c87`, `sol:C-shres-sprite-bits.c88`, `sol:C-shres-sprite-bits.c89`, `sol:C-shres-sprite-bits.c90`, `sol:C-shres-sprite-bits.c91`, `sol:C-shres-sprite-bits.c92`, `sol:C-shres-sprite-bits.c93`, `sol:C-shres-sprite-bits.c94`, `sol:C-shres-sprite-bits.c95`, `sol:C-shres-sprite-bits.c96`, `opus:C-shrc.srcs17`, p. 302; `REG-0x16`: `opus:C-shrs.r16`, `sol:C-shres-sprite-bits.c101`, `sol:C-shres-sprite-bits.c102`, `sol:C-shres-sprite-bits.c103`, `sol:C-shres-sprite-bits.c104`, `sol:C-shres-sprite-bits.c105`, `sol:C-shres-sprite-bits.c106`, `sol:C-shres-sprite-bits.c107`, `sol:C-shres-sprite-bits.c108`, `sol:C-shres-sprite-bits.c109`, `sol:C-shres-sprite-bits.c110`, `sol:C-shres-sprite-bits.c111`, `sol:C-shres-sprite-bits.c112`, p. 302; `REG-0x17`: `opus:C-shrs.r17`, `sol:C-shres-sprite-bits.c117`, `sol:C-shres-sprite-bits.c118`, `sol:C-shres-sprite-bits.c119`, `sol:C-shres-sprite-bits.c120`, `sol:C-shres-sprite-bits.c121`, `sol:C-shres-sprite-bits.c122`, `sol:C-shres-sprite-bits.c123`, `sol:C-shres-sprite-bits.c124`, `sol:C-shres-sprite-bits.c125`, `sol:C-shres-sprite-bits.c126`, `sol:C-shres-sprite-bits.c127`, `sol:C-shres-sprite-bits.c128`, p. 302; `REG-0x18`: `opus:C-shrs.r18`, `sol:C-shres-sprite-bits.c133`, `sol:C-shres-sprite-bits.c134`, `sol:C-shres-sprite-bits.c135`, `sol:C-shres-sprite-bits.c136`, `sol:C-shres-sprite-bits.c137`, `sol:C-shres-sprite-bits.c138`, `sol:C-shres-sprite-bits.c139`, `sol:C-shres-sprite-bits.c140`, `sol:C-shres-sprite-bits.c141`, `sol:C-shres-sprite-bits.c142`, `sol:C-shres-sprite-bits.c143`, `sol:C-shres-sprite-bits.c144`, p. 302; `REG-0x19`: `opus:C-shrs.r19`, `sol:C-shres-sprite-bits.c149`, `sol:C-shres-sprite-bits.c150`, `sol:C-shres-sprite-bits.c151`, `sol:C-shres-sprite-bits.c152`, `sol:C-shres-sprite-bits.c153`, `sol:C-shres-sprite-bits.c154`, `sol:C-shres-sprite-bits.c155`, `sol:C-shres-sprite-bits.c156`, `sol:C-shres-sprite-bits.c157`, `sol:C-shres-sprite-bits.c158`, `sol:C-shres-sprite-bits.c159`, `sol:C-shres-sprite-bits.c160`, p. 302; `REG-0x1A`: `opus:C-shrs.r1A`, `sol:C-shres-sprite-bits.c165`, `sol:C-shres-sprite-bits.c166`, `sol:C-shres-sprite-bits.c167`, `sol:C-shres-sprite-bits.c168`, `sol:C-shres-sprite-bits.c169`, `sol:C-shres-sprite-bits.c170`, `sol:C-shres-sprite-bits.c171`, `sol:C-shres-sprite-bits.c172`, `sol:C-shres-sprite-bits.c173`, `sol:C-shres-sprite-bits.c174`, `sol:C-shres-sprite-bits.c175`, `sol:C-shres-sprite-bits.c176`, `opus:C-shrc.srcs18`, p. 302; `REG-0x1B`: `opus:C-shrs.r1B`, `sol:C-shres-sprite-bits.c181`, `sol:C-shres-sprite-bits.c182`, `sol:C-shres-sprite-bits.c183`, `sol:C-shres-sprite-bits.c184`, `sol:C-shres-sprite-bits.c185`, `sol:C-shres-sprite-bits.c186`, `sol:C-shres-sprite-bits.c187`, `sol:C-shres-sprite-bits.c188`, `sol:C-shres-sprite-bits.c189`, `sol:C-shres-sprite-bits.c190`, `sol:C-shres-sprite-bits.c191`, `sol:C-shres-sprite-bits.c192`, p. 302; `REG-0x1C`: `opus:C-shrs.r1C`, `sol:C-shres-sprite-bits.c197`, `sol:C-shres-sprite-bits.c198`, `sol:C-shres-sprite-bits.c199`, `sol:C-shres-sprite-bits.c200`, `sol:C-shres-sprite-bits.c201`, `sol:C-shres-sprite-bits.c202`, `sol:C-shres-sprite-bits.c203`, `sol:C-shres-sprite-bits.c204`, `sol:C-shres-sprite-bits.c205`, `sol:C-shres-sprite-bits.c206`, `sol:C-shres-sprite-bits.c207`, `sol:C-shres-sprite-bits.c208`, p. 302; `REG-0x1D`: `opus:C-shrs.r1D`, `sol:C-shres-sprite-bits.c213`, `sol:C-shres-sprite-bits.c214`, `sol:C-shres-sprite-bits.c215`, `sol:C-shres-sprite-bits.c216`, `sol:C-shres-sprite-bits.c217`, `sol:C-shres-sprite-bits.c218`, `sol:C-shres-sprite-bits.c219`, `sol:C-shres-sprite-bits.c220`, `sol:C-shres-sprite-bits.c221`, `sol:C-shres-sprite-bits.c222`, `sol:C-shres-sprite-bits.c223`, `sol:C-shres-sprite-bits.c224`, p. 302; `REG-0x1E`: `opus:C-shrs.r1E`, `sol:C-shres-sprite-bits.c229`, `sol:C-shres-sprite-bits.c230`, `sol:C-shres-sprite-bits.c231`, `sol:C-shres-sprite-bits.c232`, `sol:C-shres-sprite-bits.c233`, `sol:C-shres-sprite-bits.c234`, `sol:C-shres-sprite-bits.c235`, `sol:C-shres-sprite-bits.c236`, `sol:C-shres-sprite-bits.c237`, `sol:C-shres-sprite-bits.c238`, `sol:C-shres-sprite-bits.c239`, `sol:C-shres-sprite-bits.c240`, p. 302; `REG-0x1F`: `opus:C-shrs.r1F`, `sol:C-shres-sprite-bits.c245`, `sol:C-shres-sprite-bits.c246`, `sol:C-shres-sprite-bits.c247`, `sol:C-shres-sprite-bits.c248`, `sol:C-shres-sprite-bits.c249`, `sol:C-shres-sprite-bits.c250`, `sol:C-shres-sprite-bits.c251`, `sol:C-shres-sprite-bits.c252`, `sol:C-shres-sprite-bits.c253`, `sol:C-shres-sprite-bits.c254`, `sol:C-shres-sprite-bits.c255`, `sol:C-shres-sprite-bits.c256`, `opus:C-shrc.srcs19`, p. 302 </details> |
| **Module** | `denise_colortable.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v); inherited (MiSTer and TC64 carry the same mode-blind `denise_colortable` port list and the same `case (spres)` in `denise_sprites.v`) |
| **Test** | [`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding): `make -C hrm/ch3-shres-sprite-colour-encoding test`; the fifteen reds are expected |
| **Assertions** | 15 red, one per row of p. 302's table: `REG-0x11`, `REG-0x12`, `REG-0x13`, `REG-0x14`, `REG-0x15`, `REG-0x16`, `REG-0x17`, `REG-0x18`, `REG-0x19`, `REG-0x1A`, `REG-0x1B`, `REG-0x1C`, `REG-0x1D`, `REG-0x1E`, `REG-0x1F` (`4 passed, 15 failed, 2 declined`). Expected: a flipped register bit moves the colour and channel its row prints; observed, for `REG-0x11` bit 0: colour 17 moves, page-predicted colour 16, blue. `REG-0x10` and `DOT-BITS` are declined, `P1` and three Hires controls are green, and `P2` is printed, not graded |
| **Verdict of the four** | **one of four** reads `$10`–`$1F` as the page describes (WinUAE, ECS Denise only); vAmiga pair-encodes only the lower sixteen registers; Denise and Copperline do not pair-encode; outside the emulators, the 8373R4 dissection indexes the upper sixteen registers by two sprite pixels and shows two bits per component, as WinUAE does — see *What other Commodore documents say*. Hardware: not measured |
| **Effort** | none; if a capture from ECS Denise hardware confirms p. 302's encoding, M: `denise_colortable.v` needs a mode input it does not have, and the sprite path must show a pixel's colour from the register half that holds it |
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

In NanoMig bit 0 of COLOR17, Colour 16's blue letter `F`, moves Colour 17's blue; bit 0 of COLOR21, Colour 17's `L`, moves Colour 17's blue, but not in the top two bits where the key puts `L`:

```
MEASURED: REG-0x11: bit 0, run S1, tick 9: observed colour 17, page-predicted colour 16, component(s) moved: B.
MEASURED: REG-0x15: bit 0, run S1-SPR2, tick 9: observed colour 17, page-predicted colour 17, component(s) moved: B.
```

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
| **MiSTer** | [`denise_colortable.v:7-20`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L7-L20) | descendant — no `shres`/`spres` port, same as NanoMig; its [`rtl/denise_sprites.v:110-115`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v#L110-L115) has the identical `spres` case, still only driving `shift` |
| **TC64** | [`denise_colortable.v:7-20`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L7-L20) | descendant — the same lines |

**One of the four independent implementations reads the upper sixteen
registers as the page describes: WinUAE, on ECS Denise only.** vAmiga
pair-encodes SuperHires colour from the lower sixteen registers alone and looks
a sprite pixel up there; Denise stores one colour per register in every mode;
Copperline takes a sprite's colour from one register of the classic sprite
block in every mode. MiSTer and TC64 are mode-blind in the identical way
NanoMig is, which is not another reading — a descendant with no `shres` port
inherited that absence, it did not arrive at it independently.

### What other Commodore documents say

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 8c, 9a, 6j1, 6k and 9h — agrees with the manual:

  *Read from the drawing, not quoted:* in SuperHires the colour-index PLA on sheet 8c sets CNA1 and CNA0 from the B and A bits of channels 0 to 3, which carry the first of the two 70 ns pixels a sprite shows per 140 ns cycle, S0_AF to S3_AF (sheet 6j1), and CNA3 and CNA2 from channels 4 to 7, which in SuperHires carry the second, S0_AG to S3_AG (sheet 6k). The rows that put the sprite pair's number into CNA3 and CNA2 act only outside SuperHires, and no SuperHires row has an attach term. CNA0 to CNA3 select one of rCOLOR16 to rCOLOR31, the sixteen upper registers (sheet 9a). In SuperHires the output stage of sheet 9h joins red outputs R0 with R2 and R1 with R3 and drives them from register bits 11 and 10 in one 70 ns half and from bits 9 and 8 in the other.
  <!-- drawing: denise-8373r4-dissection/sheet_059.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_071.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_036.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_079.txt -->

  A SuperHires sprite reads COLOR(16 + 4 × second + first) for each pair of its pixels and shows each pixel's two bits per component from one half of that register: the arrangement of p. 302's table, and the pair lookup of WinUAE that *What the test measures* sets against NanoMig. A solid run of value 01 reads COLOR21, whose row the table fills with Colour 17 alone. As drawn, sprites 4 to 7 have no channel of their own in SuperHires.

---

## Decision

**Suggested change:** the feature is absent rather than wrong, and
adding it is not a one-line change: `denise_colortable` would need a mode input
it does not have, and the sprite path would need to show a pixel's colour from
the half of a register that holds it, two bits per component.
