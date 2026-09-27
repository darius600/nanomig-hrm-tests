# NM-AA-003 — the AA bit table spells BLTCON0 bit 12 ASA0 in one column and ASH0 in the other

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | Nothing in the RTL. NanoMig takes all four bits as one shift field, which is what the page's own legend asks for. What is asked is whether `ASA0` is a typographic slip for `ASH0` — which the same row's second column, the legend below the table, and the three cells above it all say — or a name this document means. |
| **Module** | `src/minimig-aga/agnus_blitter.v:232` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v#L232) at the pinned revision |
| **Report to** | **nowhere in code.** The defect is in a 1991 Commodore document, and the company is gone. It is filed so that no AA claim rests on BLTCON0 bit 12 carrying a field name of its own |
| **Test** | [`aa/aa-blitter-function-and-shift`](../../aa/aa-blitter-function-and-shift) · five graded assertions — all green. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 14, the BLTCON0/BLTCON1 bit table |
| **Assertions** | 5 green, each with its own mutation; `make mutate` runs the five one at a time, and each run reports `4 passed, 1 failed` with exactly its own assertion red |
| **Verdict of the four** | **unanimous, and not one of them reads bit 12 apart from bits 15-13.** Every implementation takes `bltcon0[15:12]` in a single expression as the A shift value, and two of them name that field ASH. The spelling `ASA0` appears in the source of none of the six. See [What the others say](#what-the-others-say) |
| **See also** | [NM-AA-001](NM-AA-001.md), the other place where two statements of the same register disagree inside this document |

## What the AA specification says

BLTCON0 and BLTCON1 are printed as one table with two halves — area mode on
the left, line mode on the right, as the heading at the foot of p. 13 labels
them. Its rows are on p. 14, and the top four read:

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

`ASA0` appears once, in the area-mode cell for bit 12. The line-mode cell of the
same row reads `ASH0`. The legend names the field `ASH3-0`, which requires the
four cells to be ASH3, ASH2, ASH1 and ASH0.

## It is what the page prints, not what a scanner made of it

The corpus is merged from three independent readings — Tesseract at 300 dpi, the
1993 `AGA.guide` retyping, and a model reading the page image band by band,
blind to the other two. `p14.provenance.tsv` tags the row:

```
[3/3]  ocr+guide+model      12   ASA0        BSH0        12   ASH0        BSH0
```

All three produced `ASA0`. Every neighbouring row carries the same `[3/3]` tag,
so the region is not damaged and the readings do not disagree here.

**Both blind readers of slice 2 flagged it independently**, without seeing each
other's work: `sol:AA-bltcon0.c11` restates the cell as `MISPRINT: in area mode
BLTCON0 bit 12 is printed ASA0`, and `opus:AA-bltcon0.area12` as `In area mode
bit 12 of BLTCON0 is printed ASA0`, naming the line-mode column against it.

## What settles it

`agnus_blitter.v:232` takes the four bits as one field and nothing reads bit 12
apart:

```verilog
assign ash[3:0] = bltcon0[15:12];
```

So the RTL implements the legend rather than the cell, and no signal anywhere
carries a name.

### Measured

[`aa/aa-blitter-function-and-shift`](../../aa/aa-blitter-function-and-shift)
sweeps all eight minterm lines and shift values 1, 2, 4 and 8, finding 24
line/shift pairs where the A source moves the output and 8 where it does not;
every one of the 24 matches a pre-shift, so bit 12 behaves as the low bit of
ASH3-0 — `ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A`, green, one of the package's
`5 passed, 0 failed`. See *The measurement* below for the assertion in full.

## The measurement

`aa/aa-blitter-function-and-shift` asserts the shift field as an equivalence —
writing k into bits 15-12 produces the same destination word as shifting the A
source by k places before the blit — and runs k = 1, 2, 4 and 8. k = 1 is bit 12
alone.

The direction is not asserted, because the legend prints none: the assertion
accepts either. The equivalence is swept over all
eight minterm select lines rather than one chosen constant, and the line/shift
pairs where moving the source does not move the output are counted and skipped
as proving nothing:

```
A shift: 24 line/shift pairs where the source moves the output,
         8 where it does not and which prove nothing;
         every one of the first group matched a pre-shift
```

**Bit 12 behaves as the low bit of the shift value the legend calls ASH3-0.**
The inverse control is the mutation: with the field left at zero, the shifted
run equals the unshifted one while the pre-shift it is compared against does
not, and the assertion goes red.

## What the others say

A name is not a behaviour, so the four cannot be asked which spelling the page
should have carried. What they can be asked is whether bit 12 is the low bit of
the shift field — which is what the legend's `ASH3-0` requires and what a
separate `ASA0` would put in doubt.

| | where | what it does |
|---|---|---|
| **WinUAE** | [`blitfunc.cpp:12`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitfunc.cpp#L12) | `uae_u16 ashift = blt_info.bltcon0 >> 12;` — the whole field in one shift, repeated in every generated blit function, with `16 - (blt_info.bltcon0 >> 12)` for the descending direction ([`:32`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitfunc.cpp#L32)). Bit 12 is never separated from the other three |
| **vAmiga** | [`Blitter.h:404`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/Blitter.h#L404) | `u16 bltconASH() const { return bltcon0 >> 12; }` — the accessor is **named ASH**, and the debugger prints the field under that name ([`BlitterInfo.cpp:29`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/BlitterInfo.cpp#L29)) |
| **Denise** | [`blitterCycle.cpp:10`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitterCycle.cpp#L10) | `#define SHIFTA (bltcon0 >> 12)` — one macro for all four bits |
| **Copperline** | [`blitter.rs:1014`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1014) | `let ash = ((con0 >> 12) & 0x0F) as u32;` — masked to four bits and then carried through line mode as one value, `ash_now` ([`:1211`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1211)) |
| **MiSTer** | [`rtl/agnus_blitter.v:233`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L233) | descendant — `assign ash[3:0] = bltcon0[15:12];` |
| **TC64** | [`rtl/minimig/agnus_blitter.v:232`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L232) | descendant — the identical line |

**Six implementations, one field, and the page's own legend agrees with all of
them.** The area-mode cell is the only place in any of these sources or in the
legend where bit 12 gets a name of its own, and `ASA0` occurs in the source of
none of the six.

**It stays a finding about the printed page.** What the four settle is that
nothing downstream was misled — no implementation gave bit 12 a separate
meaning — which is worth stating precisely because a reader who trusted the
cell over the legend would have had no way to tell.

## What this does not claim

It does not claim the AA specification is wrong about the blitter. It claims one
cell of one table disagrees with the three cells above it, with the other half
of its own row, and with the legend below the table — and that the disagreement
is in the document rather than in a scan.

Nor does it grade what `LF7-0` means. The same table names the eight minterm
select lines and the specification never prints which combination of A, B and C
any one of them selects; measured across all seven slice unions, which return the
register rows and the legend and no truth table. The package grades the set —
none selected is zero, all selected is ones, the eight taken singly partition the
word — and declines the individual names as `source-silent`.
