# NM-HRM-017 — SuperHires bitplane colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Two of the package's nineteen reds carry the finding; the other seventeen reds and one green do not measure what the page says (*What the test measures*). |
| **Module** | `denise_colortable.v`, `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-shres-bitplane-colour-encoding`](../../hrm/ch3-shres-bitplane-colour-encoding) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **two against two**: WinUAE and vAmiga pair SuperHires pixels into the sixteen lower registers and show two bits per component, ECS Denise only; Denise and Copperline show each pixel's own register in full; outside the emulators, the 8373R4 dissection pairs SuperHires pixels into the lower sixteen registers and shows two bits per component, as WinUAE and vAmiga do — see *What other Commodore documents say* |
| **See also** | [NM-HRM-016](NM-HRM-016.md), the sprite half of the same mechanism |

---

## What the HRM says

**Printed p. 301** limits a SuperHires pixel to two bits per component and
moves the four bitplane colours into sixteen registers:

> SuperHires mode has a coarser granularity of color control than either Hires
> or Lores modes. ... There are only two bits of red, green and blue color
> resolution per hires pixel.

> In order to decode sprite and bitplane color information in SuperHires mode,
> certain multiplexing occurs in the use of the registers. Instead of 4 bits of
> red, green, and blue for bitplane registers 0-3 stored as 0x0RGB in four color
> registers, SuperHires bitplane colors are specially encoded in the sixteen
> lower color registers:

The key names each colour's two bits per component, and the table places them
in the registers (the key's first two lines and three of the sixteen rows):

```
                              R    G    B
      Bitplane (Color 0) :   ab-- cd-- ef--
      Bitplane (Color 1) :   gh-- ij-- kl--

      BIT  15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
   O  01   .   .   .   .   g   h   a   b   i   j   c   d   k   l   e   f
   R  04   .   .   .   .   a   b   g   h   c   d   i   j   e   f   k   l
      05   .   .   .   .   g   h   g   h   i   j   i   j   k   l   k   l
```

Row *n* carries Color (*n* mod 4) in bits 11–10, 7–6 and 3–2 and Color
(*n* div 4) in bits 9–8, 5–4 and 1–0. Row 05 is the one row that holds Color 1
alone; row 01 holds Color 1 in its upper bit pairs and Color 0 in its lower
ones. The page does not say which register, or which half of it, a given pixel
reads, nor what a pixel shows in the two positions the key marks `--`.

**Printed p. 297** calls this a new reading of the same registers:

> There are new color register interpretations as part of the SuperHires mode.

and **printed p. 300** limits the depth it applies to:

> The maximum plane depth for a SuperHires viewport is 2 bitplanes

so the sixteen lower registers hold the colours of one or two bitplanes.

---

## What NanoMig does

Same root cause as [NM-HRM-016](NM-HRM-016.md), reached down a different path:
`denise_colortable.v` has **no `shres` port**
([`denise_colortable.v:7-20`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v#L7-L20)),
so no colour lookup anywhere in the design can depend on the mode. In a single
playfield its address is the plane bits themselves
([`denise_playfields.v:100`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_playfields.v#L100),
`plfdata[7:0] = bpldata[8:1];`), and all twelve bits of the register it reads
reach the output
([`denise_colortable.v:56-58`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v#L56-L58)).

At a SuperHires depth the address never reaches COLOR04–COLOR0F:
[`denise.v:369-370`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L369-L370)
hold planes 3 and 4 at zero whenever BPU is 2 or less,

```verilog
assign bpldata[3] = l_bpu > 2 ? bpldata_out[3] : 1'b0;
assign bpldata[4] = l_bpu > 3 ? bpldata_out[4] : 1'b0;
```

so with one or two bitplanes a pixel reads one of COLOR00–COLOR03, as in Hires.

The package's port shows every SuperHires pixel: under `shres` the plane
shifter moves on every 28 MHz clock
([`denise_bitplane_shifter.v:206-208`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_bitplane_shifter.v#L206-L208)),
and `red`, `green` and `blue` follow the colour table on that clock
([`denise.v:518`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L518)),
so one sample per clock is one 35 ns pixel.

### Measured

Two assertions measure it; the package reports `5 passed, 19 failed`.

`SHRES-REINTERPRETS-SAME-REGISTERS`: COLOR00 = `0x0000`, COLOR01 = `0x0C39`, a
solid colour-1 run (plane 1 all ones, BPU = 1). NanoMig shows COLOR01 in full in
both modes:

| | |
|---|---|
| `ocs` | `0xCC3399` |
| `shres` | `0xCC3399` |

`SHRES-TWO-BITS-RESOLUTION`: one bit of COLOR01's blue nibble set at a time, a
solid colour-1 run at BPU = 2, `shres` = 1. **All four** bits move the blue
output. Row 01 puts Color 0's `e f` in bits 1–0; the key gives a colour-1
pixel Color 1's two blue bits `k l`, so at most two of the four can decide it.

---

## What the test measures

The package's assertions, set against an implementation of p. 301. The *pair
lookup* is WinUAE's and vAmiga's (*What the others say*): a solid run of value
*v* reads COLOR(5*v*), and each pixel shows two bits per component, repeated to
fill the component. Its column is derived from their code applied to the
package's stimuli, not run.

| assertion (`test_denise.cpp`) | NanoMig | pair lookup | what it shows |
|---|---|---|---|
| `SHRES-REINTERPRETS-SAME-REGISTERS` (`:946`) | red | green: the run reads COLOR05, which the test does not write, and each component can only be `0x00`, `0x55`, `0xAA` or `0xFF`, so the reading is never `0xCC3399` | the finding |
| `SHRES-TWO-BITS-RESOLUTION` (`:1126`, `:1180`) | red, 4 of 4 | green, 0 of 4: COLOR01 reaches no pixel of a solid colour-1 run | the finding |
| `SHRES-BITPLANE-TABLE-COLOR-04` to `-0F` (`:1046-1074`) | red | red: `checks.expect(false, label)` | nothing: red on every implementation. The reason they give, that COLOR04–COLOR0F need planes 3 and 4, is NanoMig's lookup; p. 301 puts those registers under one or two bitplanes |
| `SHRES-USES-SIXTEEN-LOWER-REGISTERS` (`:1152`) | red | red: it requires the twelve above | nothing |
| `SHRES-BITPLANE-TABLE-COLOR-00` to `-03` (`:1121`) | red | green, `-01` to `-03` because a run of value 1, 2 or 3 reads COLOR05, COLOR0A or COLOR0F, which the sweep never writes | not their rows. They require every used bit to make the SuperHires reading differ from the plain one, which p. 301 does not state: with the two `--` positions shown as zeros, bit 11 of row 00 gives the plain `0x88` red |
| `SHRES-BPL01-BIT11-ISOLATES-RED` (`:1231-1236`) | green | red: bit 11 of COLOR01 does not reach a solid colour-1 run | NanoMig's lookup: p. 301 says what bit 11 carries, not which pixels read it |

The four controls (`CONTROL-KNOWN-VALUES-ARRIVE`, `FIXTURE-DATA-PATH-LIVE`,
`OCS-BASELINE-0RGB-FORMAT`, `SELECT-REGISTER-ADDRESSES-CORRECTLY`) run with
`shres` = 0 and pass. `OCS-BASELINE-0RGB-FORMAT` checks only the top four bits
of each 8-bit component, the part that "stored as 0x0RGB" fixes.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2991-3000`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2991-L3000) | <pre>uae_u8 off = ((p1 & 3) * 4) + (p0 & 3) + ((p0 \| p1) & 16);</pre> reinterprets, ECS Denise only ([`:6470`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L6470)): two adjacent SuperHires pixels select one of the sixteen lower registers; the first shows its bits 11–10, 7–6, 3–2 (`& 0xccc`), the second its bits 9–8, 5–4, 1–0 (`& 0x333`). A solid colour-1 run reads COLOR05 |
| **vAmiga** | [`Core/Components/Denise/PixelEngine.h:301-305`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.h#L301-L305) | <pre>return ((mbuf[base + 2] & 3) * 4) + (mbuf[base] & 3);</pre> reinterprets, ECS Denise only ([`PixelEngine.cpp:215-222`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L215-L222)): pixels 0 and 2, and pixels 1 and 3, of each group of four select one of the sixteen lower registers; even pixels show the high bit pair of each component, odd pixels the low pair ([`PixelEngine.cpp:620`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L620), [`Colors.cpp:127-148`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Colors.cpp#L127-L148)). A solid colour-1 run reads COLOR05; a mixed pattern can read a different register than under WinUAE's pairing |
| **Denise** | [`emulation/libami/video/denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | <pre>colors[pos] = value;</pre> one table, no mode; a SuperHires pixel shows its own register in full ([`:797`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L797), `color = colors[colIndex];`) |
| **Copperline** | [`src/video/bitplane/tests.rs:7563`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L7563) | *"each 35 ns half resolves palette[1], never a pair-encoded entry"*; the path under test looks up each 35 ns sample on its own ([`src/video/bitplane/output.rs:324-333`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/output.rs#L324-L333)) |
| **MiSTer** | [`rtl/denise_colortable.v:32-35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L32-L35) | descendant — same as [NM-HRM-016](NM-HRM-016.md): no `shres` port on `rtl/denise_colortable.v:32-35`; `rtl/denise.v:339` has the identical `l_bpu > 2` depth-force line, with an added `window_ena &&` term NanoMig's line lacks |
| **TC64** | [`rtl/minimig/denise_colortable.v:32-35`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L32-L35) | descendant — (`rtl/minimig/denise_colortable.v:32-35`) both keep the interface mode-blind, and their 11-line diffs from NanoMig (a RAM primitive's word width) are identical to each other and unrelated to colour mode. `denise.v`: **MiSTer** `rtl/denise.v:339` — `assign bpldata[3] = window_ena && (l_bpu > 2) ? bpldata_out[3] : 1'b0;`, the same `l_bpu > 2` depth force as NanoMig's `:369`, with an added `window_ena &&` term NanoMig's line lacks. **TC64** `rtl/minimig/denise.v:354` — the identical line, `window_ena` term included. The depth truncation this finding rests on is unchanged in both lines; the `window_ena` difference is a separate, narrower divergence this finding does not measure |


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 9b, 8i, 7f to 7h and 9h — agrees with the manual:

  *Read from the drawing, not quoted:* the sixteen lower registers have two read decoders, rmCOLOR00 to rmCOLOR15 addressed by CMA0 to CMA3 and rsCOLOR00 to rsCOLOR15 by CSA0 to CSA3 (sheet 9b). In SuperHires the playfield PLA on sheet 8i (its rows on SH_RES#) sets C_SA1 and C_SA0 from planes 2 and 1 at shift-register position 15 and C_SA3 and C_SA2 from position 13, C_MA1 and C_MA0 from position 14 and C_MA3 and C_MA2 from position 12. Positions 15 to 12 are the four SuperHires pixels of a 140 ns cycle: sheet 7h gives planes 1 and 2 the outputs B1_Q12 to B2_Q15, while planes 3 and 4 have positions 15 and 14 (sheet 7g) and plane 5 position 15 (sheet 7f). In SuperHires the output stage of sheet 9h joins red outputs R0 with R2 and R1 with R3, drives them from register bits 11 and 10 in one 70 ns half and from bits 9 and 8 in the other, and alternates between the two read paths within each half.
  <!-- drawing: denise-8373r4-dissection/sheet_072.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_065.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_047.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_046.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_045.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_079.txt -->

  Each lower register is read for two pixels, the first and third or the second and fourth of a cycle, the grouping vAmiga uses where WinUAE pairs adjacent pixels, and each pixel shows two bits per component, the upper or the lower pair of that register, repeated to fill the component: p. 301's two bits and its table. A solid colour-1 run reads COLOR05, as in WinUAE and vAmiga, where NanoMig shows COLOR01 in full. The PLA and the read decoders are read from their drawn crosspoints, not from a netlist; the output stage is traced in the netlist for red, the one component drawn in full, and green and blue draw only their bit 0. The dissection is a work in progress, and of one revision.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; US patent 5,103,499, Commodore-Amiga, filed 1989 as a division of the 1986 application, granted 1992.

---

## Suggested change

As with [NM-HRM-016](NM-HRM-016.md): `denise_colortable` needs a mode input it
does not have, and at a depth of one or two bitplanes its address must reach
COLOR04–COLOR0F, which `denise.v:369-370` keep out of reach.
The two findings are one mechanism and should be closed together or not at all —
they are kept as separate entries because they rest on separate measurements at
separate boundaries, not because they are separate problems.
