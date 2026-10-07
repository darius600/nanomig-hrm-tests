# NM-HRM-044 — the manual permits the Copper to initialise DSKPTH on one page and forbids it on another

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. Is DSKPTH (`$020`) writable by the Copper on OCS, as p.279 prints ("must be initialized by the processor or Copper before disk DMA is enabled"), rather than blocked, as p.289's `*` ("Address not writable by the Copper") prints? NanoMig blocks it on OCS and so do all four implementations; `P1` is red against p.279 and `APPBSTAR $10 DSKPTH ecs=0` is green against p.289, so the red is not an implementation fault. Closes when an Amiga specialist records p.289's `*` as the statement that governs the OCS Copper, or confirms that the OCS Copper reaches `$020`, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendices A and B of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | Agnus Copper MOVE to `$020` with `ecs`=0: CDANG clear in `P1`, CDANG clear and set in the sweep; chip revision unknown. ECS with CDANG set is not driven by `P1`'s package (p.310 allows it); DSKPTL (`$022`) is not separately exercised |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · Appendix A printed p.279 (DSKPTH/DSKPTL), Appendix B printed pp.287–290 (legend, table), Chapter 2 printed p.26, Appendix C printed p.310 · `P1`: `sol:8-appa-dskptr.c3` (p.279); `APPBSTAR $10 DSKPTH ecs=0`: `sol:B-copper-star.c17`, `opus:B-dskpth.copper` (p.289); `COPLIVE` carries no claim id |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L220-L228) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L220-L228); inherited, identical in MiSTer and TC64 |
| **Test** | [`hrm/ch8-disk-pointer-ecs-and-copper`](../../hrm/ch8-disk-pointer-ecs-and-copper): `make -C hrm/ch8-disk-pointer-ecs-and-copper test`; an expected red exits nonzero. [`hrm/ch2-copper-move-sweep`](../../hrm/ch2-copper-move-sweep): `make -C hrm/ch2-copper-move-sweep test` (green line `APPBSTAR $10 DSKPTH ecs=0`) |
| **Assertions** | 1 red of this page: `P1` against p.279 — expected the Copper MOVE to load DSKPTH, observed the pointer not loaded; the package totals `4 passed, 3 failed`, its other two reds belonging to [NM-HRM-043](NM-HRM-043.md). `COPLIVE` is green as the control. `APPBSTAR $10 DSKPTH ecs=0` is green against p.289 in `hrm/ch2-copper-move-sweep` (`97 passed, 1 failed`) |
| **Verdict of the four** | **unanimous with NanoMig**: all four block a Copper write to `$020` on OCS, as p.289 marks it, and allow it under ECS with CDANG set, as p.310 allows; none lets the Copper initialise DSKPTH on OCS, as p.279 would. That is which reading practitioners took, not which page is right. Hardware: not measured |
| **Effort** | none: no RTL change is asked |
| **See also** | [NM-HRM-014](NM-HRM-014.md), the precedent for a finding about the manual rather than about the implementation. **Not [NM-HRM-004](NM-HRM-004.md)**, which is the ECS danger-bit *range* — a different claim, register and page |

---

## Source requirement

**Appendix A, printed p.279**, the entry for the DSKPTH/DSKPTL pair:

> "This pair of registers contains the 18-bit address of disk DMA data. These
> address registers must be initialized by the processor or Copper before disk
> DMA is enabled."

**Appendix B, printed p.289**, the same register:

> `DSKPTH     + *020   W     A (E)      Disk pointer (high 3 bits, 5 bits if ECS)`

and the legend that defines the mark, **printed p.287**:

> `*    Address not writable by the Copper.`

Unconditionally: the conditional form on the same page is a different glyph, and DSKPTH does not carry it:

> `~     Address not writable by the Copper unless the "copper danger bit",
> COPCON is set true.`

**Chapter 2, printed p.26**, says the same of the whole block the `*` marks:

> "Those it cannot affect at all are numbered $00 to $3E inclusive."

It sends ECS to Appendix C, where p.310 lets the ECS Copper reach every register once CDANG is set. **p.288's `PTL,PTH` legend**, the facing page:

> "Chip memory pointer that addresses DMA data. Must be reloaded by a
> processor before use (vertical blank for bitplane and sprite pointers, and
> prior to starting the blitter for blitter pointers)."


One page names the Copper as an initialiser of this register, the other says the Copper cannot write it.

## RTL and measurement

[`agnus_copper.v#L220-L228`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L220-L228):

```verilog
// detect illegal register access
// CDANG = 0 (OCS/ECS) : $080-$1FE allowed
// CDANG = 1 (OCS)     : $040-$1FE allowed
// CDANG = 1 (ECS)     : $000-$1FE allowed
always @(*)
if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs) // illegal access
	illegalreg = 1'b1;
```

`$020` has `ir2[8:6] == 3'b000`, so it is blocked whenever `ecs` is low, whatever CDANG holds; under ECS it is blocked unless CDANG is set.

> **FAIL: P1** the Copper may initialise DSKPTH before disk DMA is enabled
> [`sol:8-appa-dskptr.c3`, p.279]

`P1` is graded with `ecs`=0 and CDANG clear, where p.289's `*` and p.310's ECS relaxation do not interact. Its control is green: `COPLIVE` — a Copper MOVE reaches DMACON (outside `$008`–`$026`), confirmed by a CPU read of DMACONR — so the red is not a harness that cannot make the Copper write.

The other half is graded too, and it passes:

    PASS: APPBSTAR $10 DSKPTH ecs=0: rejected on reg_address_out with CDANG clear
          and with CDANG set (Appendix B '*', p.289, byte $020)

No implementation can make `P1` and this line both green; NanoMig makes p.289's green. Changing either assertion would delete the evidence.

## Risks and unknowns

- **p.279's sentence may be about initialisation in general, not Copper access to this address**, in which case the appendices are loose rather than contradictory — the same weakness [NM-HRM-042](NM-HRM-042.md) carries about the width. Nothing on the page supports or refutes that reading.
- **The `*` is a property of an address range.** Every row from `$000` to `$03E` on p.289 carries `*` and every row from `$040` to `$07E` carries `~`; nobody should read the `*` as a considered statement about this register. The contradiction stands, because the mark means what the legend says wherever it appears. p.288's `PTL,PTH` legend (*Source requirement*) names a processor and not the Copper: a silence, not a prohibition.
- **`P1`'s package does not drive ECS with CDANG set.** It never drives a Copper MOVE at `ecs`=1/`cdang`=1 against `$008`–`$026`, the reach p.310's ECS sentence gives, which [NM-HRM-053](withdrawn/NM-HRM-053.md) (withdrawn) records. DSKPTL (`$022`) is not asserted: a blocked MOVE halts the Copper's sequencer, so a second MOVE in the same list is never reached.
- **Appendix B's Copper marks and the rest of the manual disagree in at least two places**; [NM-HRM-004](NM-HRM-004.md) is the other, and there all four implementations follow Appendix B against the p.310 sentence the suite grades. Whoever settles either should look at both.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2806-2818`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2806-L2818) | floor `$80` with CDANG clear, `$40` with CDANG set on OCS, `0` on ECS; `$020` blocked in both OCS cases; the same test in `generate_copper` (`:9134`) stops the Copper |
| **vAmiga** | [`Copper.cpp:394-402`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L394-L402) | the same three cases; [`CopperEvents.cpp:144-145`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L144-L145) cancels the Copper slot on an illegal address |
| **Denise** | [`emulation/libami/agnus/copper.cpp:341-343`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L341-L343) | the same three cases; [`:203-207`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L203-L207) switches the Copper off |
| **Copperline** | [`src/bus/dma_slots.rs:697-708`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L708) | the only one that writes the two address blocks out as p.289 marks them: `<= 0x03E` needs ECS and CDANG, `0x040..=0x07E` needs CDANG |
| **MiSTer** | [`rtl/agnus_copper.v:220-228`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L220-L228) | descendant — byte-identical to NanoMig, same line numbers, comments included |
| **TC64** | [`rtl/minimig/agnus_copper.v:220-228`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L220-L228) | descendant — byte-identical as well, confirmed by a direct diff against both |

The unanimity is not evidence that p.289 is right; it shows four independent implementers read Appendix B's marks as the operative statement about Copper access. All four halt the Copper on the blocked MOVE rather than only dropping the write, and all four condition the `$000`–`$03E` block on the chipset, which the legend does not; that conditioning is p.310's ECS sentence and does not reach the OCS corner graded here.

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "The pointer regis ters 138 are loaded with data (an address) from data bus 404 by the processor 402 or coprocessor 47 under pro gram control."
  <!-- corpus: patent-us4874164-agnus-denise/p016.txt -->

  A general statement for all 25 DMA pointer registers, disk included: the Copper may load them, which is p.279's side. The patent mentions no protected register range.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 29 — agrees with the manual:

  > "These address registers must be initialized by the processor or Copper before disk DMA is enabled."
  <!-- corpus: agnus-8371-spec/p029.txt -->

  This is p.279's side, nearly word for word, for an OCS part. The same document also has p.289's side: Table 2 marks DSKPTH '+*020' (p031), and its legend reads '* Address not writeable by co-processor.' (p034). It therefore repeats the manual's contradiction and does not settle it.

## Decision

No RTL change. The question is which of two printed sentences describes the hardware, and an assertion has to pick one page as the oracle, so no implementation can settle it; the entry is filed against the printed page, as [NM-HRM-014](NM-HRM-014.md) is. Both assertions stay as they are.

Opening `$000`–`$03E` to the Copper at OCS to turn `P1` green would put the RTL in conflict with p.289 and with all four implementations to satisfy one sentence on p.279. Under ECS with CDANG set, NanoMig and the four let the Copper write DSKPTH, so p.279's sentence holds there; p.279 does not say it is conditional.

Closes when an Amiga specialist records p.289's `*` as the statement that governs the OCS Copper, or confirms that the OCS Copper reaches `$020`, in which case this page is refiled against the RTL.
