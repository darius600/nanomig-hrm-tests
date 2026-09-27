# NM-HRM-016 — SuperHires sprite colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. The package's sixteen reds are one measurement, of p. 301's multiplexing clause: NanoMig shows a sprite pixel alike in both modes. None measures a row of the table, and one green and one control rest on NanoMig's lookup (*What the test measures*). The two bits per component the page gives are measured at the system boundary and graded nowhere. |
| **Module** | `denise_colortable.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **one of four** reads `$10`–`$1F` as the page describes (WinUAE, ECS Denise only); vAmiga pair-encodes only the lower sixteen registers; Denise and Copperline do not pair-encode |
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

The unit package writes one register at a time and compares the set of
distinct colours a sprite pixel shows at `shres = 0` and at `shres = 1`, a
comparison that is blind to the SuperHires drain rate. Every register it reads
returns the identical set:

```
MEASURED: REG-0x11: shres=0 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)};
                    shres=1 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)}.
```

---

## What the test measures

The package's assertions (`3 passed, 16 failed, 1 declined`), set against an
implementation of p. 302. The *pair lookup* is WinUAE's (*What the others
say*): two sprite samples per low-resolution pixel select
COLOR(16 + 4·(*second* & 3) + (*first* & 3)), so the inside of a solid run of
value *v* reads COLOR(16 + 5*v*), and only a pair with a transparent half reads
COLOR17–COLOR19. For an attached pair WinUAE keeps only the even sprite's two
bits of the colour index
([`drawing.cpp:4568-4575`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4568-L4575)).
Its column is derived from WinUAE's code applied to the package's stimuli, not
run.

| assertion (`test_denise.cpp`) | NanoMig | pair lookup | what it shows |
|---|---|---|---|
| `SHRES-MODE-CHANGES-OUTPUT` (`:839`) | red | green: a solid value-01 run reads COLOR21, which the test leaves at the value it leaves COLOR00, so no sample differs from the sprite-less trace | the clause it cites, p. 301's multiplexing, and only that: NanoMig's SuperHires sprite pixel equals its lores one. Its stimulus writes COLOR17 alone, which is not an encoding the table gives, so the page predicts no colour for it; vAmiga's lower-sixteen reading, which p. 302 excludes, and a display that drew no sprite pass it the same way |
| `REG-0x11` to `-0x13` (`:974`) | red | green, the same way: the runs read COLOR21, COLOR26 and COLOR31 | the same measurement again, not their rows: the register each names is read at most at the run's edge, which the page does not describe |
| `REG-0x14` to `-0x1F` (`:974`) | red | green: the attached pair reads COLOR16, COLOR21, COLOR26 or COLOR31, which is the named register for `-0x15`, `-0x1A` and `-0x1F` only | attached sprites, which no page describes in SuperHires; nine of the twelve do not read their row |
| `REG-0x10` (`:917`) | declined | declined | row `$10` holds only Colour 16's letters, and no sprite-0 pixel shows Colour 16 (p. 103, p. 108) |
| `SHRES-SPR11-BIT11-ISOLATES-RED` (`:1035`) | green | red, or green only where the harness's one sample in four lands on an edge pair that reads COLOR17: bit 11 of COLOR17 reaches no pixel inside a solid value-01 run | NanoMig's lookup |
| `[control] FIXTURE-SPRITE-DATA-PATH-LIVE` (`:790`) | green | as the row above: it runs at `shres = 1` (`:682`) with COLOR17 alone written | a control that depends on the mode under test |
| `[control] FIXTURE-SPR0-TRANSPARENT-AT-VALUE0` (`:804`) | green | green | value 00 is transparent in both modes |

The harness samples once per 7 MHz clock (`:591-600`), one SuperHires pixel in
four. Each `REG` assertion also requires that bits 15–12 have no effect, for
which the table's `.` gives no value.

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

None of them speaks to this point.
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
