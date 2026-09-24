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
| **Chapter** | Appendix F — 8520 Complex Interface Adapters · printed p.353 |
| **Assertions** | 0. This is a defect in the source, found before a test was built against it |
| **Verdict of the four** | **unanimous, and all four contradict the printed patterns**: WinUAE, vAmiga, Denise and Copperline's lineage all decode the way the page's PROSE says |

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
The text layer agrees with the render character for character on both patterns.
`corpus-defects.md` lists p.353 as undamaged, and it is: the defect is the
manual's.

## What the others say

| | | |
|---|---|---|
| **WinUAE** | follows the prose, and enforces A0 | `cia.cpp:2586-2592`: `mask = addr & 0xf000` is valid only for `0xe000` or `0xd000`; `0xe000` additionally requires `(addr & 1) != 0` and `0xd000` requires `(addr & 1) == 0`. `$E` is `1110` — A13 high, A12 low — for CIAA, with A0=1 |
| **vAmiga** | follows the prose | `Memory.cpp:2236-2252`: `sel = (addr >> 12) & 0b11` selects on A13:A12, and `0b01` (A13=0, A12=1) with `a0` false reaches `ciab` |
| **Denise** | follows the prose | `libami/agnus/memory.cpp:61-66`: `switch(adr & 0x3000)`, `0x2000` (A13=1, A12=0) with an odd address reaches `cia1`, `0x1000` (A13=0, A12=1) with an even one reaches `cia2` |
| **Copperline** | follows the prose | its CIA windows are the standard `$BFE001` / `$BFD000` pair; no implementation anywhere decodes the printed patterns |
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

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 241 — **agrees with NanoMig**:

  > "BFDOOO-BFDFOO 8520-B (access only at EVEN byte addresses) BFEOO1-BFEF01 8520-A (access only at ODD byte addresses)"
  <!-- corpus: hw-manual-1985/p241.txt -->

  The August 1985 original has no binary patterns; the errata describes the 'Brief address map for 8520's' section as new. Its memory map puts 8520-A at $BFExx1 (A13=1, A12=0, A0=1) and 8520-B at $BFDxx0 (A13=0, A12=1, A0=0). The port-assignment pages (p.245, p.251) mark CIA-A '(Al2*)' on data bits 7-0 and CIA-B '(A13*)' / '(Al3')' on data bits 15-8. That is the page's prose and chosen addresses, and the decode NanoMig's gary.v implements, not the printed patterns. The verdict is agrees-with-rtl because the finding is filed against the patterns.

- **Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985**, PDF page 42 — agrees with the manual:

  > "So, CIM is selected by the following binary address: 101x xxxx xx01 rrrr xxxx xxxO. eIAB address: 101x xxxx xx10 rrrr xxxx xxxl"
  <!-- corpus: hw-manual-errata-1985/p042.txt -->

  The section first appears in the October 1985 errata, and it already carries the defect: the same two patterns (CIAA with A13:A12 = 01, CIAB with 10), next to the same prose ('CIM is selected when Al2 is low, Al3 high') and the same chosen addresses ('CIM = BFErOl; CIAB = BFDrOO.'). It also has a sentence the 3rd edition dropped: 'For byte access (seems to be the usual case), AO must be 0 for CIM, 1 for CIAB.' That sentence agrees with the patterns' A0 and contradicts the chosen addresses (BFErOl is odd). In 1985, then, the A0 half of the conflict was prose-plus-pattern against addresses, and the finding's 'A single transposition of the two patterns makes the whole page consistent' does not hold for this printing.

Silent on this point: Commodore, *A500/A2000 Gary Specification*; US patent 4,829,473, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.
