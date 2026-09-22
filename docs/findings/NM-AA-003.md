# NM-AA-003 — the AA bit table spells BLTCON0 bit 12 ASA0 in one column and ASH0 in the other

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a graded green with an inverse control |
| **What is asked of a reader** | Nothing in the RTL. NanoMig takes all four bits as one shift field, which is what the page's own legend asks for. What is asked is whether `ASA0` is a typographic slip for `ASH0` — which the same row's second column, the legend two lines below it, and the three cells above it all say — or a name this document means. |
| **Module** | `src/minimig-aga/agnus_blitter.v:232` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v#L232) at the pinned revision |
| **Report to** | **nowhere in code.** The defect is in a 1991 Commodore document, and the company is gone. It is filed so that no AA claim rests on BLTCON0 bit 12 carrying a field name of its own |
| **Test** | [`aa/aa-blitter-function-and-shift`](../../aa/aa-blitter-function-and-shift) · five graded assertions — all green. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 14, the BLTCON0/BLTCON1 bit table |
| **Assertions** | 5 green, each with its own mutation; `make mutate` reports 0 passed, 5 failed |
| **Verdict of the four** | not consulted — this is a defect in a document, not a behaviour the implementations could disagree about |
| **See also** | [NM-AA-001](NM-AA-001.md), the other place where two statements of the same register disagree inside this document |

## What the AA specification says

Page 14 prints BLTCON0 and BLTCON1 as one table with two columns — area mode on
the left, line mode on the right. The top four rows:

```
                    15   ASH3        BSH3        15   ASH3        BSH3
                    14   ASH2        BSH2        14   ASH2        BSH2
                    13   ASH1        BSH1        13   ASH1        BSH1
                    12   ASA0        BSH0        12   ASH0        BSH0
```

Three cells below, the same page's field legend reads:

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

## The measurement

`aa/aa-blitter-function-and-shift` asserts the shift field as an equivalence —
writing k into bits 15-12 produces the same destination word as shifting the A
source by k places before the blit — and runs k = 1, 2, 4 and 8. k = 1 is bit 12
alone.

The direction is not asserted, because the legend prints none: the assertion
accepts either and reports which it found. The equivalence is swept over all
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

## What this does not claim

It does not claim the AA specification is wrong about the blitter. It claims one
cell of one table disagrees with the three cells above it, with the other half
of its own row, and with the legend three lines below — and that the disagreement
is in the document rather than in a scan.

Nor does it grade what `LF7-0` means. The same table names the eight minterm
select lines and the specification never prints which combination of A, B and C
any one of them selects; measured across all seven slice unions, which return the
register rows and the legend and no truth table. The package grades the set —
none selected is zero, all selected is ones, the eight taken singly partition the
word — and declines the individual names as `source-silent`.
