# NM-HRM-062 — Appendix F prints the two CIA address patterns interchanged

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | none |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. No implementation is at fault; the defect is in the source, found before a test was built against it. |
| **Module** | none. `gary.v:191-192` follows the page's PROSE and is not at fault here |
| **Report to** | whoever maintains errata for the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Test** | none yet. Raised at classification by the appendix-F lot-E classifier and verified here against the render |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters · *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p.353 |
| **Assertions** | 0. This is a defect in the source, found before a test was built against it |
| **Verdict of the four** | **unanimous, and all four contradict the printed patterns**: WinUAE, vAmiga, Denise and Copperline all decode the way the page's PROSE says |

---

## What the HRM says, twice, and the two do not agree

Printed page 353, *Hardware Connection Details*, first in prose:

> The system hardware selects the CIAs when the upper three address bits are 101.
> Furthermore, CIAA is selected when A12 is low, A13 high; CIAB is selected when
> A12 is high, A13 low.

Then as binary patterns, two paragraphs later:

> So, CIAA is selected by the following binary address:
> `101x xxxx xx01 rrrr xxxx xxx0`. CIAB address:
> `101x xxxx xx10 rrrr xxxx xxx1`

Reading the CIAA pattern off, MSB first — `101x` is A23–A20, `xxxx` A19–A16,
`xx01` A15–A12:

| | A13 | A12 | A0 |
|---|---|---|---|
| prose, CIAA | **high** | **low** | — |
| printed CIAA pattern | 0 | 1 | 0 |
| prose, CIAB | low | high | — |
| printed CIAB pattern | 1 | 0 | 1 |

**Each printed pattern carries the other chip's bits.**

## The page contradicts itself a third time, in its own chosen addresses

The next paragraph:

> With future expansion in mind, we have decided on the following addresses:
> CIAA = BFEr01; CIAB = BFDr00.

`$BFE001`: `E` is `1110`, so A15=1, A14=1, **A13=1, A12=0**, and the address ends
in 1, so **A0=1**. That is the prose's CIAA and the printed pattern's CIAB.

`$BFD000`: `D` is `1101`, so **A13=0, A12=1**, ending in 0, so **A0=0**. The
prose's CIAB.

So the prose and the chosen addresses agree with each other and both disagree
with the two printed binary patterns, including on A0.

## This is the PRINT, not the scan

Read off `figures/p353.png` rendered at 200 dpi from PDF page 368 and cropped.
The text layer agrees with the render bit for bit on both patterns; its `l`, `O`
and `irrr` are OCR misreadings of `1`, `0` and `rrrr`.
`corpus-defects.md` lists p.353 as undamaged, and it is: the defect is the
manual's.

### Measured

Nothing is measured here: Evidence is `none` and Test is `none yet` — this
defect was raised at classification by the appendix-F lot-E classifier, from
reading p.353 against itself, and verified in this entry against the render
rather than by any package.

## What the others say

| | | |
|---|---|---|
| **WinUAE** | follows the prose, and enforces A0 | `cia.cpp:2677-2701`: `cia_bget` switches on `cia_chipselect(addr)`, which is `(addr >> 12) & 3`, A13:A12 (`:2636`); case 2 (A13 high, A12 low) reads CIAA only on an odd address, case 1 (A13 low, A12 high) CIAB only on an even one. The debugger's `validate_cia` (`:2586-2592`) logs as invalid anything but `mask = addr & 0xf000` equal to `0xe000` with `(addr & 1) != 0` or `0xd000` with `(addr & 1) == 0` |
| **vAmiga** | follows the prose | `Memory.cpp:2236-2252`: `sel = (addr >> 12) & 0b11` selects on A13:A12, and `0b01` (A13=0, A12=1) with `a0` false reaches `ciab` |
| **Denise** | follows the prose | `libami/agnus/memory.cpp:61-66`: `switch(adr & 0x3000)`, `0x2000` (A13=1, A12=0) with an odd address reaches `cia1`, `0x1000` (A13=0, A12=1) with an even one reaches `cia2` |
| **Copperline** | follows the prose | its CIA windows are `CIA_A_BASE: u32 = 0x00BF_E000` and `CIA_B_BASE: u32 = 0x00BF_D000`, 4K each (`src/cpu.rs:18-21`), matched by address range at `:4154-4161` — CIA-A where A13 is high and A12 low, CIA-B the reverse |
| **MiSTer** | descendant — no module; this finding is against the printed page, not an implementation | descendant — nor |
| **TC64** | descendant — no module; this finding is against the printed page, not an implementation | descendant — has anything to answer for either |

**Not one implementation decodes the printed patterns**, and NanoMig does not
either: `gary.v:191` is `sel_cia_a = sel_cia & ~cpu_address_in[12]` — CIAA on A12
low, the prose's rule.

## What holds this finding up

The page disagrees with itself three ways — prose against pattern, chosen address
against pattern, and A0 polarity against pattern — and every one of those three
resolves the same way. A single transposition of the two patterns makes the whole
page consistent. Nothing else has to be assumed.

## What is left for specialists

Nothing about NanoMig. This is an erratum in a printed manual, and the reason it
is filed at all is that this suite treats the page as the oracle: **a test built
from the printed patterns rather than from the prose would grade a correct
decoder red.** Four claims in the appendix-F classification carry
`source_defines: "conflict"` for exactly that reason and are deliberately not
adjudicated at classification.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; Commodore, 6526 CIA specification, 1981; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

