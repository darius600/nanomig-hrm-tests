# NM-HRM-062 — Appendix F prints the two CIA address patterns interchanged

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | none |
| **What is asked of a reader** | No RTL change follows from this. Are the two binary patterns on p.353 (CIAA `101x xxxx xx01 rrrr xxxx xxx0`, CIAB `101x xxxx xx10 rrrr xxxx xxx1`) exchanged, so that the page's prose and its chosen addresses `$BFEr01` and `$BFDr00` stand? Closes when an Amiga specialist records the two patterns as exchanged, or confirms them as printed, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendix F of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | unknown; p.353 names no chip revision or mode. The isolated case is the choice between CIAA and CIAB by A13, A12 and A0 |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, *Hardware Connection Details* · *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p.353 · claims `sol:F-signals.c64`, `sol:F-signals.c65`, `opus:F-conn.c7`, `opus:F-conn.c8` (all four marked `conflict` at classification); no assertion grades them |
| **Module** | `gary.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L190-L192) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L193-L195); inherited, TC64 has the same A12 and A13 terms |
| **Test** | No package grades the claim, so there is no command to run |
| **Assertions** | none: no assertion grades the claim |
| **Verdict of the four** | **unanimous, and all four contradict the printed patterns**: WinUAE, vAmiga, Denise and Copperline all decode the way the page's prose says. Hardware not measured |
| **Effort** | none: no RTL change; the correction is one exchange of the two printed patterns |
| **Origin** | raised at classification by the appendix-F lot-E classifier; verified against the render of p.353 |

---

## Source requirement

Printed p.353, *Hardware Connection Details*, in prose:

> The system hardware selects the CIAs when the upper three address bits are 101.
> Furthermore, CIAA is selected when A12 is low, A13 high; CIAB is selected when
> A12 is high, A13 low.

Two paragraphs later, as binary patterns:

> So, CIAA is selected by the following binary address:
> `101x xxxx xx01 rrrr xxxx xxx0`. CIAB address:
> `101x xxxx xx10 rrrr xxxx xxx1`

Read MSB first, `101x` is A23–A20, `xxxx` A19–A16 and `xx01` A15–A12:

| | A13 | A12 | A0 |
|---|---|---|---|
| prose, CIAA | **high** | **low** | — |
| printed CIAA pattern | 0 | 1 | 0 |
| prose, CIAB | low | high | — |
| printed CIAB pattern | 1 | 0 | 1 |

Each printed pattern carries the other chip's bits. The page's next paragraph
also contradicts the patterns: "CIAA = BFEr01; CIAB = BFDr00". `$BFE001` has
A13=1, A12=0 and A0=1, the prose's CIAA; `$BFD000` has A13=0, A12=1 and A0=0,
the prose's CIAB. Prose and chosen addresses agree with each other, including
on A0, and disagree with both patterns.

The defect is in the print, not the scan: `figures/p353.png`, rendered at 200 dpi
from PDF page 368, shows the patterns as quoted, and the text layer reads the
same bits apart from OCR misreadings of `1`, `0` and `rrrr` (`l`, `O`, `irrr`).

## RTL and measurement

Nothing is measured: no package observes the claim, and the defect was found by
reading p.353 against itself and checked against the render.

NanoMig's
[`gary.v#L190-L192`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L190-L192)
has `sel_cia_a = sel_cia & ~cpu_address_in[12]` and
`sel_cia_b = sel_cia & ~cpu_address_in[13]`: one bit per chip, with the prose's
polarity (CIAA on A12 low, CIAB on A13 low) and not the printed patterns'
(CIAA with A12=1).

## Risks and unknowns

- Nothing here was measured on hardware, and only p.353 of the 3rd edition was read.
- The suite treats the page as the oracle, so a test built from the printed
  patterns rather than the prose would grade a correct decoder red; the four
  claims named in *Chapter* are deliberately not adjudicated at classification.
- Which of prose or patterns the authors meant is a decision for an Amiga
  specialist; the page shows only that one exchange makes all three statements agree.
- A0 is contradicted by three of the four: Copperline matches by address range
  and does not look at A0.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:2677-2701`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2677-L2701) | follows the prose and enforces A0: `cia_bget` switches on `cia_chipselect(addr)`, `(addr >> 12) & 3`, A13:A12 (`:2636`); A13 high with A12 low reads CIAA only on an odd address, A13 low with A12 high reads CIAB only on an even one. The debugger's `validate_cia` (`:2586-2592`) logs anything but `0xe000` with an odd address or `0xd000` with an even one as invalid |
| **vAmiga** | [`Memory.cpp:2236-2252`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2236-L2252) | follows the prose: `sel = (addr >> 12) & 0b11`, and `0b01` (A13=0, A12=1) with `a0` false reaches `ciab` |
| **Denise** | [`libami/agnus/memory.cpp:61-66`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L61-L66) | follows the prose: `switch(adr & 0x3000)`, `0x2000` (A13=1, A12=0) with an odd address reaches `cia1`, `0x1000` (A13=0, A12=1) with an even one reaches `cia2` |
| **Copperline** | [`src/cpu.rs:18-21`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L18-L21), [`:4154-4161`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L4154-L4161) | follows the prose: `CIA_A_BASE` is `0x00BF_E000` and `CIA_B_BASE` `0x00BF_D000`, 4K each, matched by address range, so CIA-A where A13 is high and A12 low |
| **MiSTer** | [`rtl/gary.v:193-195`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L193-L195) | descendant — the same `sel_cia_a` and `sel_cia_b` terms as NanoMig |
| **TC64** | [`rtl/minimig/gary.v:189-192`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/gary.v#L189-L192) | descendant — the same A12 and A13 terms |

## Decision

**No RTL change.** NanoMig follows the prose, as all four implementations do.

The change belongs to the manual: exchange the two printed patterns, CIAA
`101x xxxx xx10 rrrr xxxx xxx1` and CIAB `101x xxxx xx01 rrrr xxxx xxx0`. Then
the prose, the chosen addresses and the patterns agree, including on A0.

Closes when an Amiga specialist records the patterns as exchanged, or confirms
them as printed, in which case this page is refiled against the RTL.
