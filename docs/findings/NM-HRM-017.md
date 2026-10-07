# NM-HRM-017 — SuperHires bitplane colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should NanoMig's Denise, in SuperHires with one or two bitplanes, show each pixel with two bits per component from the sixteen lower colour registers as p. 301's table encodes them (a solid colour-1 run reading COLOR05, not COLOR01 in full)? *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Closes when an Amiga specialist accepts or rejects the change under *Decision*. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer: the code is upstream's (the MiSTer row below shows the same `denise_colortable.v` interface and the same depth-force line); a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Scope** | NanoMig's Denise colour table and bitplane path in SuperHires at BPU = 2, COLOR00–COLOR0F; isolated through solid runs of value 0 to 3 and 18 one-bit changes to colours 0, 2 and 3; two mixed runs are driven, not graded; the Denise revision the page describes is `unknown` |
| **Software impact** | `unknown; no named affected software` |
| **Chapter** | 3 — Playfield; Appendix C, printed pp. 297, 300 and 301 (the colour table on p. 301) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claim ids as the assertion messages carry them: `SHRES-REINTERPRETS-SAME-REGISTERS`: `C-shr.c06`, `C-superhires-color.c1`, p. 297; `SHRES-BITPLANE-TABLE-COLOR-00` to `-0F`: each its row id `C-shrc.r00` to `C-shrc.r0F` and its sixteen `C-shres-bpl-bits.cNNN` ids, and on `-00`, `-05`, `-0A` and `-0F` the legend ids `C-shrc.srcb0` to `C-shrc.srcb3` with `C-shres-symbols.c1` to `C-shres-symbols.c12`, 288 in all; `SHRES-USES-SIXTEEN-LOWER-REGISTERS`: `C-shrc.c04`, `C-shrc.c05`; `SHRES-TWO-BITS-RESOLUTION`: `C-shrc.c01`, `C-shrc.c02`; `OCS-BASELINE-0RGB-FORMAT`: `C-shres-color.c4` to `c7`. `CONTROL-KNOWN-VALUES-ARRIVE`, `FIXTURE-DATA-PATH-LIVE` and `SELECT-REGISTER-ADDRESSES-CORRECTLY` carry no claim id, and no assertion message cites p. 300 |
| **Module** | `denise_colortable.v`, `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v); inherited (MiSTer and TC64 have the same colour-table code; their `denise.v` adds a `window_ena &&` term to the depth line) |
| **Test** | [`hrm/ch3-shres-bitplane-colour-encoding`](../../hrm/ch3-shres-bitplane-colour-encoding): `make -C hrm/ch3-shres-bitplane-colour-encoding test` — `15 passed, 9 failed`; the nine reds are expected and the run exits nonzero |
| **Assertions** | 9 red: `SHRES-REINTERPRETS-SAME-REGISTERS`; `SHRES-BITPLANE-TABLE-COLOR-01`, `SHRES-BITPLANE-TABLE-COLOR-02`, `SHRES-BITPLANE-TABLE-COLOR-03`, `SHRES-BITPLANE-TABLE-COLOR-05`, `SHRES-BITPLANE-TABLE-COLOR-0A`, `SHRES-BITPLANE-TABLE-COLOR-0F`; `SHRES-USES-SIXTEEN-LOWER-REGISTERS`; `SHRES-TWO-BITS-RESOLUTION`. Expected, for `SHRES-REINTERPRETS-SAME-REGISTERS`: SuperHires reads a solid colour-1 run differently from plain mode (COLOR05, `0x55AAFF` or `0x4488CC`); observed: `0x4499EE`, COLOR01 in full, in both. Each of the nine is green on a lookup that pairs adjacent pixels, and on one that pairs pixels 0/2 and 1/3, at every group alignment; the other eight are in the table under *RTL and measurement* |
| **Verdict of the four** | **two against two**: WinUAE and vAmiga pair SuperHires pixels into the sixteen lower registers and show two bits per component, ECS Denise only; Denise and Copperline show each pixel's own register in full; outside the emulators, the 8373R4 dissection pairs SuperHires pixels into the lower sixteen registers and shows two bits per component, as WinUAE and vAmiga do |
| **Effort** | `M` — `denise_colortable.v` needs a mode input it does not have, and `denise.v:369-370` must let the address reach COLOR04–COLOR0F at one or two bitplanes |
| **See also** | [NM-HRM-016](NM-HRM-016.md), the sprite half of the same mechanism |

---

## Source requirement

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
ones.

**Printed p. 297** calls this a new reading of the same registers:

> There are new color register interpretations as part of the SuperHires mode.

and **printed p. 300** limits the depth it applies to:

> The maximum plane depth for a SuperHires viewport is 2 bitplanes

so the sixteen lower registers hold the colours of one or two bitplanes.

---

## RTL and measurement

### What NanoMig does

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
Under `shres` the plane shifter moves on every 28 MHz clock
([`denise_bitplane_shifter.v:206-208`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_bitplane_shifter.v#L206-L208))
and the colour outputs follow the table on that clock
([`denise.v:518`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L518)),
so one sample per clock is one 35 ns pixel.

### Measured

The package reports `15 passed, 9 failed`. Every stimulus but the four controls' writes the sixteen
lower registers from one palette by p. 301's row formula, two bits per
component for each colour (colour 0: red 0, green 1, blue 2; colour 1: 1, 2, 3;
colour 2: 2, 3, 0; colour 3: 3, 0, 1), sets BPU = 2, and drives *solid runs*:
both planes held at one value *v*. Two lookups show every pixel its own colour
when the registers are written by the table: *adjacent* pixels share one
(WinUAE's), or pixels 0 and 2 and pixels 1 and 3 of each group of four do
(*0/2–1/3*, the 8373R4 reading under *Independent evidence*). Under both, a
solid run of value *v* reads COLOR(5*v*), and each pixel shows two bits per
component. That is all the package grades (`:797-812`): each significant bit of
COLOR(5*v*) must move the value-*v* run and no other; no bit of any other
register, and no bit 15–12 of any register, may move a solid run. Solid runs
are compared tick by tick (`:577-585`). Two mixed runs, planes 1 and 2 loaded
with `0xAAAA`/`0xCCCC` and `0xF0F0`/`0xFF00` (`:571-572`), are driven on every
flip and reported, not graded (`:893-894`). The two lookup columns apply each
lookup, with the two `--` positions filled either by repeating the pair or by
zeros, to the pixel stream the fixture produces; they are derived, not run.

| assertion (`test_denise.cpp`) | NanoMig | adjacent | 0/2–1/3 | what it shows |
|---|---|---|---|---|
| `SHRES-REINTERPRETS-SAME-REGISTERS` (`:785`) | red: a solid run of value 1 shows COLOR01 (`0x49E`) in full, `0x4499EE`, with `shres` = 0 and 1 | green: the run reads COLOR05 (`0x5AF`), shown as `0x55AAFF` or `0x4488CC` depending on the fill, never `0x4499EE` | green, the same | the finding |
| `SHRES-BITPLANE-TABLE-COLOR-01` to `-03`, `-05`, `-0A`, `-0F` (`:897`) | red, 12 of 12 bits each: every significant bit of COLOR01, COLOR02 and COLOR03 moves the run of value 1, 2 or 3; none of COLOR05, COLOR0A or COLOR0F moves any run | green | green | the finding: the value-*v* run moves with COLOR0*v*, which neither lookup gives it, and COLOR(5*v*) does not reach the value-*v* run |
| `SHRES-BITPLANE-TABLE-COLOR-00`, `SHRES-BITPLANE-TABLE-COLOR-04`, `-06` to `-09`, `-0B` to `-0E` (`:897`) | green: COLOR00's bits move the value-0 run alone; no bit of the other nine registers, nor any of the 64 bits 15–12, moves a solid run | green | green | no difference: COLOR00 is COLOR(5·0), the register NanoMig reads too, and no solid run reads the other nine on any of the three |
| `SHRES-USES-SIXTEEN-LOWER-REGISTERS` (`:931`), and its Hires control (`:957`) | red: COLOR0A XORed with `$0FFF` changes none of the four solid runs; the control, the same XOR under plain Hires, changes none either (green) | green: COLOR0A is COLOR(5·2) and moves the value-2 run | green | the finding; the control: outside SuperHires a value-*v* run reads COLOR0*v*, so COLOR0A moves nothing |
| `SHRES-TWO-BITS-RESOLUTION` (`:1026`) | red, 6 of 18: of the one-bit changes to colours 0, 2 and 3, the six to colour 0 move a solid run of value 1 | green, 0 of 18 | green, 0 of 18 | the finding: NanoMig shows COLOR01 in full, and row 01 holds Color 0's `a b`, `c d`, `e f` in bits 9–8, 5–4 and 1–0; the key gives a colour-1 pixel Color 1's own `g h`, `i j`, `k l` |
| `CONTROL-KNOWN-VALUES-ARRIVE` (`:637`), `FIXTURE-DATA-PATH-LIVE` (`:664`), `OCS-BASELINE-0RGB-FORMAT` (`:693`), `SELECT-REGISTER-ADDRESSES-CORRECTLY` (`:731`) | green: run with `shres` = 0; `OCS-BASELINE-0RGB-FORMAT` checks only the top four bits of each 8-bit component, the part "stored as 0x0RGB" fixes | green | green | controls |

Each solid run is reloaded every 32 ticks and sampled on six consecutive ticks,
pixels 5 to 10 of the word's sixteen, with five of the word's pixels on either
side (`solid_run`, `:507-519`). Every adjacent pair and every group of four
that holds a sampled tick lies inside the word at every alignment, so under
both lookups each sampled tick reads COLOR(5*v*), and the six together show
both of its halves.

---

## Risks and unknowns

- **Which register a pixel reads.** p. 301 does not say which register, or
  which half of it, a given pixel reads, nor what a pixel shows in the two
  positions the key marks `--`. The two lookups disagree on mixed pairs, which
  the package therefore drives but does not grade.
- **Depth.** Every SuperHires stimulus runs at BPU = 2, the ceiling p. 300 gives; one
  bitplane is not measured.
- **The lookup columns are derived**, not run.
- **A split witness.** WinUAE and vAmiga pair pixels, and differ from each other
  in how; Denise and Copperline do not pair them.
- **One third-party silicon revision.** The 8373R4 dissection is a work in
  progress, of one revision, not a Commodore document. Its PLA and read decoders
  are read from drawn crosspoints, not from a netlist; the output stage is
  traced in the netlist for red, the one component drawn in full, and green and
  blue draw only their bit 0. None of the evidence here is a hardware capture.
- **A neighbouring divergence.** MiSTer and TC64 add a `window_ena` term to the
  depth line; this finding does not measure it.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2991-3000`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2991-L3000) | <pre>uae_u8 off = ((p1 & 3) * 4) + (p0 & 3) + ((p0 \| p1) & 16);</pre> reinterprets, ECS Denise only ([`:6470`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L6470)): two adjacent SuperHires pixels select one of the sixteen lower registers; the first shows its bits 11–10, 7–6, 3–2 (`& 0xccc`), the second its bits 9–8, 5–4, 1–0 (`& 0x333`). A solid colour-1 run reads COLOR05 |
| **vAmiga** | [`Core/Components/Denise/PixelEngine.h:301-305`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.h#L301-L305) | <pre>return ((mbuf[base + 2] & 3) * 4) + (mbuf[base] & 3);</pre> reinterprets, ECS Denise only ([`PixelEngine.cpp:215-222`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L215-L222)): pixels 0 and 2, and pixels 1 and 3, of each group of four select one of the sixteen lower registers; even pixels show the high bit pair of each component, odd pixels the low pair ([`PixelEngine.cpp:620`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L620), [`Colors.cpp:127-148`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Colors.cpp#L127-L148)). A solid colour-1 run reads COLOR05; a mixed pattern can read a different register than under WinUAE's pairing |
| **Denise** | [`emulation/libami/video/denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | <pre>colors[pos] = value;</pre> one table, no mode; a SuperHires pixel shows its own register in full ([`:797`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L797), `color = colors[colIndex];`) |
| **Copperline** | [`src/video/bitplane/tests.rs:7563`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L7563) | *"each 35 ns half resolves palette[1], never a pair-encoded entry"*; the path under test looks up each 35 ns sample on its own ([`src/video/bitplane/output.rs:324-333`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/output.rs#L324-L333)) |
| **MiSTer** | [`rtl/denise_colortable.v:32-35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L32-L35) | descendant — same as [NM-HRM-016](NM-HRM-016.md): no `shres` port on `rtl/denise_colortable.v:32-35`; `rtl/denise.v:339` — `assign bpldata[3] = window_ena && (l_bpu > 2) ? bpldata_out[3] : 1'b0;`, the same `l_bpu > 2` depth force as NanoMig's `:369`, with an added `window_ena &&` term NanoMig's line lacks |
| **TC64** | [`rtl/minimig/denise_colortable.v:32-35`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L32-L35) | descendant — the same interface, mode-blind; its 11-line diff from NanoMig (a RAM primitive's word width) is identical to MiSTer's and unrelated to colour mode. `rtl/minimig/denise.v:354` is the identical depth line, `window_ena` term included |

**6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 9b, 8i, 7f to 7h and 9h — agrees with the manual. *Read from the drawing, not quoted:* the sixteen lower registers have two read decoders, rmCOLOR00 to rmCOLOR15 addressed by CMA0 to CMA3 and rsCOLOR00 to rsCOLOR15 by CSA0 to CSA3 (sheet 9b). In SuperHires the playfield PLA on sheet 8i (its rows on SH_RES#) sets C_SA1 and C_SA0 from planes 2 and 1 at shift-register position 15 and C_SA3 and C_SA2 from position 13, C_MA1 and C_MA0 from position 14 and C_MA3 and C_MA2 from position 12. Positions 15 to 12 are the four SuperHires pixels of a 140 ns cycle: sheet 7h gives planes 1 and 2 the outputs B1_Q12 to B2_Q15, while planes 3 and 4 have positions 15 and 14 (sheet 7g) and plane 5 position 15 (sheet 7f). In SuperHires the output stage of sheet 9h joins red outputs R0 with R2 and R1 with R3, drives them from register bits 11 and 10 in one 70 ns half and from bits 9 and 8 in the other, and alternates between the two read paths within each half.
<!-- drawing: denise-8373r4-dissection/sheet_072.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_065.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_047.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_046.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_045.txt -->
<!-- drawing: denise-8373r4-dissection/sheet_079.txt -->

Each lower register is read for two pixels, the first and third or the second and fourth of a cycle, the grouping vAmiga uses where WinUAE pairs adjacent pixels, and each pixel shows two bits per component, the upper or the lower pair of that register, repeated to fill the component: p. 301's two bits and its table. A solid colour-1 run reads COLOR05, as in WinUAE and vAmiga, where NanoMig shows COLOR01 in full.

---

## Decision

**Suggested change:** as with [NM-HRM-016](NM-HRM-016.md), `denise_colortable` needs a mode input it does not have, and at a depth of one or two bitplanes its address must reach COLOR04–COLOR0F, which `denise.v:369-370` keep out of reach. The two findings are one mechanism and should be closed together or not at all; they are kept as separate entries because they rest on separate measurements at separate boundaries.

It closes as stated in *What is asked of a reader*. To re-run the measurement: `make -C hrm/ch3-shres-bitplane-colour-encoding test`; the nine reds are expected and the run exits nonzero.
