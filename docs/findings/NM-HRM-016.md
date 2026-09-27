# NM-HRM-016 — SuperHires sprite colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. This is chapter 3's largest single red result — 16 evidential reds. |
| **Module** | `denise_colortable.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **one of four** reads `$10`–`$1F` as the page describes (WinUAE, ECS Denise only); vAmiga pair-encodes only the lower sixteen registers; Denise and Copperline do not pair-encode |
| **See also** | [NM-HRM-017](NM-HRM-017.md), the bitplane half of the same mechanism |

---

## What the HRM says

**Printed pp. 301–302** give the ECS SuperHires sprite colour scheme. In
SuperHires the sixteen colour registers `$10`–`$1F` are read as **two colour
slots each**:

| slot | bits |
|---|---|
| A | the top two bits of every colour nibble — 11:10 R, 7:6 G, 3:2 B |
| B | the bottom — 9:8, 5:4, 1:0 |

so that a sprite can carry colour at the doubled pixel rate.

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

### Measured

The drain-rate effect is a real confound: at `shres = 1` a sprite is half as
wide, so a raw sample trace differs between modes for a reason that has nothing
to do with colour. The package therefore compares the **set of distinct colours**
a sprite pixel can show, which is width-invariant. Every live register returns
the identical set:

```
MEASURED: REG-0x11: shres=0 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)};
                    shres=1 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)}.
```

Deliberately asymmetric slot-A and slot-B values were used, so a rearrangement
would have been visible.

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
would need to select slot A or slot B per 35 ns half.

**Three of four reference implementations do not do this**; vAmiga pair-encodes
only the lower sixteen registers. So the first question is not how to implement
it but whether ECS Denise really does — and that is what WinUAE, which
reinterprets for ECS Denise only, is evidence for.

---

## An honest correction to the count

`REG-0x10` is declined, not asserted: sprite index 0, the only one that selects
`COLOR16`, is transparent, so no sample can show that register — a
**declaration, not a measurement**. The package's **sixteen** reds
(`3 passed, 16 failed, 1 declined`) are therefore all measurements.

The per-register assertions also test only that *some* mode difference exists,
not the exact symbol-to-bit mapping their claim ids cite. The finding holds;
the precision of the individual assertions is less than the claim ids imply.
