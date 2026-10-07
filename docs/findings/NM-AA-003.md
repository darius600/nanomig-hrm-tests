# NM-AA-003 — the AA bit table spells BLTCON0 bit 12 ASA0 in one column and ASH0 in the other

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | No RTL change follows from this; NanoMig already takes all four bits as one shift field, which is what the page's own legend asks for. Question: is `ASA0` in the area-mode cell for BLTCON0 bit 12 on p. 14 a typographic slip for `ASH0`, which the same row's second column, the legend below the table and the three cells above it all print? Closes when an Amiga specialist records `ASA0` as a misprint for `ASH0`, or confirms that `ASA0` names a field of its own, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — AA documentation: whoever keeps errata for Commodore's AA specification |
| **Scope** | blitter BLTCON0 bits 15-12 in area mode; bit 12 isolated by shift value 1, which sets it alone; line mode not exercised; chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 14, the BLTCON0/BLTCON1 bit table (area mode left, line mode right, labelled at the foot of p. 13). <details><summary>claim ids by assertion (26)</summary> `ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A`: `sol:AA-bltcon0.c5`, `sol:AA-bltcon0.c7`, `sol:AA-bltcon0.c9`, `sol:AA-bltcon0.c11`, `sol:AA-bltcon0.c37`, `opus:AA-bltcon0.area15`, `opus:AA-bltcon0.area14`, `opus:AA-bltcon0.area13`, `opus:AA-bltcon0.area12`, `opus:AA-bltfield.c1`; `BSH-BITS-ARE-THE-SHIFT-VALUE-OF-B`: `sol:AA-bltcon1.c6`, `sol:AA-bltcon1.c8`, `sol:AA-bltcon1.c10`, `sol:AA-bltcon1.c12`, `sol:AA-bltcon1.c38`, `opus:AA-bltcon1.area15`, `opus:AA-bltcon1.area14`, `opus:AA-bltcon1.area13`, `opus:AA-bltcon1.area12`, `opus:AA-bltfield.c2`; `LF-NONE-IS-ZERO-ALL-IS-ONES`: `sol:AA-bltcon0.c42`; `LF-LINES-ARE-DISJOINT-AND-COMPLETE`: `opus:AA-bltfield.c8`; `USED-DECIDES-WHETHER-D-IS-WRITTEN`: `sol:AA-bltcon0.c41`, `sol:AA-bltcon0.c19`, `opus:AA-bltfield.c7`, `opus:AA-bltcon0.area08`. All five assertions carry claim ids </details> |
| **Module** | `src/minimig-aga/agnus_blitter.v:232` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v#L232) at the pinned revision; inherited (MiSTer and TC64 carry the identical line) |
| **Test** | [`aa/aa-blitter-function-and-shift`](../../aa/aa-blitter-function-and-shift) · `make -C aa/aa-blitter-function-and-shift test`, or `hrm/run_all.sh --in ../aa` (that one runs the whole aa/ tree, which has red packages); this package is all green, so its own make exits zero |
| **Assertions** | none red. 5 graded assertions, all green (`5 passed, 0 failed`). The one that grades this page, `ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A`: expected the same word from writing k into bits 15-12 as from shifting the A source by k places beforehand, k = 1, 2, 4, 8; observed that match on all 24 line/shift pairs where the source moves the output (8 pairs where it does not prove nothing). The other four, named under *Chapter*, do not concern BLTCON0 bit 12 |
| **Verdict of the four** | **unanimous, and not one of them reads bit 12 apart from bits 15-13.** Every implementation takes `bltcon0[15:12]` in a single expression as the A shift value, and two of them name that field ASH. The spelling `ASA0` appears in the source of none of the six. Hardware: not measured |
| **Effort** | none — no RTL change (a documentation note on p. 14 only) |
| **See also** | [NM-AA-001](NM-AA-001.md), the other place where two statements of the same register disagree inside this document |

---

## Source requirement

BLTCON0 and BLTCON1 are printed as one table. Its rows are on p. 14, and the top four read:

```
                    15   ASH3        BSH3        15   ASH3        BSH3
                    14   ASH2        BSH2        14   ASH2        BSH2
                    13   ASH1        BSH1        13   ASH1        BSH1
                    12   ASA0        BSH0        12   ASH0        BSH0
```

Below the table's last row, the same page's field legend reads:

```
                    ASH3-0  Shift value of A source
                    BSH3-0  Shift value of B source and line texture
```

`ASA0` appears once, in the area-mode cell for bit 12; the line-mode cell of the same
row reads `ASH0`. The legend names the field `ASH3-0`, which requires the four cells to
be ASH3, ASH2, ASH1 and ASH0.

This is the page's print, not a scanner's: the corpus's three independent readings
(Tesseract at 300 dpi, the 1993 `AGA.guide` retyping, a model reading the page image
blind to the other two) all produced `ASA0`, tagged `[3/3]  ocr+guide+model` in
`p14.provenance.tsv` like every neighbouring row. Both claim records read the cell as a
misprint: `sol:AA-bltcon0.c11` as `MISPRINT: in area mode
BLTCON0 bit 12 is printed ASA0`, and `opus:AA-bltcon0.area12` as `In area mode bit 12 of
BLTCON0 is printed ASA0`, naming the line-mode column against it.

## RTL and measurement

`agnus_blitter.v:232` takes the four bits as one field and nothing reads bit 12 apart,
so the RTL implements the legend rather than the cell:

```verilog
assign ash[3:0] = bltcon0[15:12];
```

[`aa/aa-blitter-function-and-shift`](../../aa/aa-blitter-function-and-shift) writes
k = 1, 2, 4 and 8 into bits 15-12 (k = 1 is bit 12 alone) and compares the destination
word with the one from shifting the A source by k places before the blit. It runs all
eight minterm select lines and skips, as proving nothing, the pairs where moving the
source does not move the output:

```
A shift: 24 line/shift pairs where the source moves the output,
         8 where it does not and which prove nothing;
         every one of the first group matched a pre-shift
```

Bit 12 behaves as the low bit of the shift value the legend calls ASH3-0:
`ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A`, green, one of the package's `5 passed, 0 failed`.

## Risks and unknowns

- **The test shows what NanoMig does, not what silicon does.** Bit 12 of a real blitter
  is not measured, and the page cannot exclude that `ASA0` is a name this document means.
- **The shift direction is not asserted**, because the legend prints none; the assertion
  accepts either.
- **Area mode only.** The line-mode column, where `ASH0` is printed, is declined by the
  package (line mode is its first non-goal), so only the area-mode column is exercised.
- **One preliminary document.** The specification is marked PRELIMINARY, 06/07/91. The
  page claims only that one cell disagrees with the three cells above it, with the other
  half of its own row and with the legend, not that the specification is wrong about the
  blitter.
- **`LF7-0` is not graded.** The same table names the eight minterm select lines and
  prints no truth table (measured across all seven slice unions: the register rows and
  the legend only). The package grades the set and declines the individual names as
  `source-silent`.

## Independent evidence

A name is not a behaviour, so the four cannot be asked which spelling the page should
have carried; they can be asked whether bit 12 is the low bit of the shift field.

| | where | what it does |
|---|---|---|
| **WinUAE** | [`blitfunc.cpp:12`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitfunc.cpp#L12) | `uae_u16 ashift = blt_info.bltcon0 >> 12;` — the whole field in one shift, repeated in every generated blit function, with `16 - (blt_info.bltcon0 >> 12)` for the descending direction ([`:32`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitfunc.cpp#L32)) |
| **vAmiga** | [`Blitter.h:404`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/Blitter.h#L404) | `u16 bltconASH() const { return bltcon0 >> 12; }` — the accessor is **named ASH**, and the debugger prints the field under that name ([`BlitterInfo.cpp:29`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/BlitterInfo.cpp#L29)) |
| **Denise** | [`blitterCycle.cpp:10`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitterCycle.cpp#L10) | `#define SHIFTA (bltcon0 >> 12)` — one macro for all four bits |
| **Copperline** | [`blitter.rs:1014`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1014) | `let ash = ((con0 >> 12) & 0x0F) as u32;` — masked to four bits and then carried through line mode as one value, `ash_now` ([`:1211`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1211)) |
| **MiSTer** | [`rtl/agnus_blitter.v:233`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L233) | descendant — `assign ash[3:0] = bltcon0[15:12];` |
| **TC64** | [`rtl/minimig/agnus_blitter.v:232`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L232) | descendant — the identical line |

The area-mode cell is the only place in these sources or in the legend where bit 12 gets
a name of its own, and `ASA0` occurs in the source of none of the six.

## Decision

No RTL change: nothing downstream was misled, because no implementation gave bit 12 a
separate meaning, which is worth stating because a reader who trusted the cell over the
legend would have had no way to tell.
