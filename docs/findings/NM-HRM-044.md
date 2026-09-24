# NM-HRM-044 — the manual permits the Copper to initialise DSKPTH on one page and forbids it on another

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a source observation, not test-backed |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. One assertion does carry it as red; *Why the status is not red* says why that is not a conformance failure. |
| **Module** | — · the RTL is described below because it is evidence, not because it is the subject |
| **Report to** | **nobody.** There is no repository that can fix a disagreement between two printed pages |
| **Test** | [`hrm/ch8-disk-pointer-ecs-and-copper`](../../hrm/ch8-disk-pointer-ecs-and-copper) — assertion `P1`, graded red against p.279 |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **unanimous — and it decides nothing.** All four block a Copper write to $020 at OCS and allow it under ECS with CDANG set. That is which reading practitioners took, not which page is right |
| **See also** | [NM-HRM-014](NM-HRM-014.md), the precedent for a finding about the manual rather than about the implementation. **Not [NM-HRM-004](NM-HRM-004.md)**, which is the ECS danger-bit *range* — a different claim, register and page |

---

## What this is

A disagreement **inside the manual**, about whether the Copper may write the disk
pointer at all. Two pages answer the same question and they answer it
differently.

**Appendix A, printed p.279**, the entry for the DSKPTH/DSKPTL pair:

> "This pair of registers contains the 18-bit address of disk DMA data. **These
> address registers must be initialized by the processor or Copper before disk
> DMA is enabled.**"

Not "may" — *must*, and the Copper is named as one of the two agents that may do
it.

**Appendix B, printed p.289**, the same register:

> `DSKPTH     + *020   W     A (E)      Disk pointer (high 3 bits, 5 bits if ECS)`

and the legend that defines the mark, **printed p.287**:

> `*    Address not writable by the Copper.`

Unconditionally. The legend offers no chipset, no danger bit and no exception;
the conditional form exists on the same page and is a different glyph —

> `~     Address not writable by the Copper unless the "copper danger bit",
> COPCON is set true.`

— and DSKPTH does not carry it.

**One page names the Copper as a permitted initialiser of this register. The
other says the Copper cannot write it.** Both are appendices to the same edition,
ten pages apart.

## Two things that look like corroboration and are not

**p.288's `PTL,PTH` legend** — the facing page to the table — reads:

> "Chip memory pointer that addresses DMA data. **Must be reloaded by a
> processor before use** (vertical blank for bitplane and sprite pointers, and
> prior to starting the blitter for blitter pointers)."

It names a processor and not the Copper. That is a **silence, not a prohibition**,
and this register does not treat a silence as a requirement. It is recorded here
because it shows p.289's side of the disagreement is not carried by the `*` glyph
alone, not because it adds a third statement.

**The `*` on DSKPTH is a property of an address range, not a judgement about the
disk pointer.** Every row from `$000` to `$03E` on p.289 carries `*`, and every
row from `$040` to `$07E` carries `~`. The manual marked two blocks of addresses,
and DSKPTH is inside the first. That does not weaken the contradiction — the mark
means what the legend says it means wherever it appears — but it does mean nobody
should read the `*` as a considered statement about *this* register.

## Why no implementation can settle it

The question is not "what does the hardware do". It is "which of two printed
sentences describes the hardware", and both were printed by the same publisher
about the same chip. An assertion has to pick one page as the oracle and measure
against it, which answers a different question.

That is the shape [NM-HRM-014](NM-HRM-014.md) set: a manual conflict is recorded, not graded.

## Why the status is not red, although an assertion is

**This entry differs from [NM-HRM-014](NM-HRM-014.md) in one way and the difference should not
be hidden.** [NM-HRM-014](NM-HRM-014.md) has no assertion behind it at all; it is a `note()` in
its package. This one does have a graded assertion, and it failed:

> **FAIL: P1** the Copper may initialise DSKPTH before disk DMA is enabled
> [`sol:8-appa-dskptr.c3`, p.279]

The assertion is sound and the measurement is good. What the red means is
narrower than it looks: **NanoMig does not do what p.279 says.** It says nothing
about whether p.279 is right, because p.289 says NanoMig is correct to refuse.
A red graded against one half of a contradiction is not a conformance failure; it
is the measurement that makes the contradiction visible.

So the register carries this as a source observation whatever colour the
assertion shows, and the assertion stays as it is. Changing it would delete the
evidence.

### The control that makes the red readable

    PASS: COPLIVE  a Copper MOVE reaches an always-unprotected register
                   (DMACON, outside $008-$026), confirmed by a CPU read of DMACONR

Without it, a red P1 would be indistinguishable from a harness that cannot make
the Copper write anything at all.

### What was measured, and at which chipset

`P1` was graded with `ecs` = 0. That is deliberate: it is the condition under
which appendix B's `*` and appendix C's ECS relaxation of the low address range
do not interact, and the ECS-plus-CDANG case is [NM-HRM-004](NM-HRM-004.md)'s territory rather
than a fresh number. The package's own run records the boundary it did not cross:

> **MEASURED:** this package never drives a Copper MOVE at `ecs`=1/`cdang`=1
> against the $008-$026 range … Nothing in this package's build fell into that
> territory.

**DSKPTL ($022) was not separately exercised.** It sits in the same measured
blocked range, but an illegal MOVE halts the Copper's instruction sequencer
rather than letting it continue, so a second MOVE in the same list could never be
reached once the first was blocked. The run records this rather than asserting it.

## What NanoMig does — as evidence, not as the subject

[`src/minimig-aga/agnus_copper.v#L220-L228`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L220-L228),
identical to
[Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L220-L228)
at the same line numbers:

```verilog
// detect illegal register access
// CDANG = 0 (OCS/ECS) : $080-$1FE allowed
// CDANG = 1 (OCS)     : $040-$1FE allowed
// CDANG = 1 (ECS)     : $000-$1FE allowed
always @(*)
if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs) // illegal access
	illegalreg = 1'b1;
```

$020 is `ir2[8:1] = 8'h10`, so `ir2[8:6] == 3'b000`: blocked whenever `ecs` is
low, whatever `cdang` holds. NanoMig follows p.289 at OCS, and follows p.279 only
under ECS with the danger bit set.

### Measured

[`hrm/ch8-disk-pointer-ecs-and-copper`](../../hrm/ch8-disk-pointer-ecs-and-copper)
grades `P1`, "the Copper may initialise DSKPTH before disk DMA is enabled,"
red against p.279, one of the package's `4 passed, 3 failed`, with `COPLIVE`
green as its control that the harness can make the Copper write at all. See
*Why the status is not red, although an assertion is* above for what the red
does and does not mean, and *What was measured, and at which chipset* for the
boundary it was measured at.

## What the others say

All four independent implementations were read. All four do exactly what
NanoMig does.

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2806-2818`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2806-L2818) | <pre>int addr = reg & 0x01fe;<br>if (addr &lt; ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {</pre> floor `$80` with CDANG clear, `$40` with CDANG set on OCS, `0` on ECS. $020 blocked in both OCS cases. The same branch stops the Copper for the line |
| **vAmiga** | [`Copper.cpp:394-402`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L394-L402) | <pre>if (cdang) {<br>&nbsp;&nbsp;return agnus.isOCS() ? addr &lt; 0x40 : false;<br>} else {<br>&nbsp;&nbsp;return addr &lt; 0x80;<br>}</pre> the identical three cases. [`CopperEvents.cpp:144-145`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L144-L145) cancels the Copper slot on an illegal address |
| **Denise** | [`emulation/libami/agnus/copper.cpp:341-343`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L341-L343) | <pre>cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;</pre> stores the floor rather than the bit, then at [`:203-207`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L203-L207) `if (reg < cdang) { state = Off; ... }` — same three cases, same halt |
| **Copperline** | [`src/bus/dma_slots.rs:697-708`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L708) | <pre>if off &lt;= 0x03E {<br>&nbsp;&nbsp;&nbsp;&nbsp;return !matches!(self.agnus.revision(), AgnusRevision::Ocs)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&& self.agnus.copper_danger_enabled();<br>}</pre> the only one of the four that writes the two address blocks out as p.289 marks them — `<= 0x03E` needing both ECS and CDANG, `0x040..=0x07E` needing CDANG |
| **MiSTer** | [`rtl/agnus_copper.v:220-228`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L220-L228) | descendant — byte-identical to NanoMig, same line numbers, comments included: `if (ir2[8:7]==2'b00 && !cdang \|\| ir2[8:6]==3'b000 && !ecs) illegalreg = 1'b1;`. $020 is blocked exactly as it is in NanoMig; this line is what NanoMig's own "What NanoMig does" section already cites as identical to Minimig-AGA |
| **TC64** | [`rtl/minimig/agnus_copper.v:220-228`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L220-L228) | descendant — byte-identical as well, same line numbers, same comments, confirmed by a direct diff against both NanoMig's and MiSTer's lines. No citation for this row existed before this page; it was opened for this table rather than reused from an `Upstream` row, since this entry is a source observation and carries none |

### How to read that unanimity, and how not to

**It is not evidence that p.289 is right.** It is evidence that four people who
implemented this chip independently all read the manual the same way: appendix
B's marks as the operative statement about Copper access, and p.279's sentence as
describing initialisation in general, on a machine where the Copper reaches this
register once CDANG is set.

Three further points, all of which are about the implementations rather than
about the pages:

- **All four halt the Copper** on the blocked MOVE rather than merely dropping the
  write, so this is modelled as a hard access rule, not as a filter.
- **All four condition the $000-$03E block on the chipset**, which appendix B's
  legend does not. That conditioning comes from appendix C, and whether it is
  read correctly is [NM-HRM-004](NM-HRM-004.md)'s question, not this one.
- **Under ECS with CDANG set, all four let the Copper write DSKPTH** — so on the
  configuration NanoMig and the four agree is the modern one, p.279's sentence
  holds. p.279 simply does not say it is conditional.

## What holds the observation up

- **Both sentences are quoted from the printed pages** in this project's own
  extraction of the scan, not from the Developer CD re-edit.
- **The legend that gives the `*` its meaning is on p.287**, in the same
  appendix, and it defines a conditional alternative (`~`) that this register
  does not carry. The prohibition is the unconditional one by the manual's own
  vocabulary.
- **The measurement behind `P1` has a live control** and a recorded reason for
  each thing it declined to grade.

## What weakens it

- **p.279's sentence may be about initialisation in general rather than about
  Copper access to this specific address**, in which case the appendices are
  loose rather than contradictory — the same weakness [NM-HRM-042](NM-HRM-042.md) carries about
  the width. Nothing on the page supports or refutes that reading.
- **This is one of at least two places where appendix B's Copper marks and the
  rest of the manual do not line up**; [NM-HRM-004](NM-HRM-004.md) is the other, and all four
  implementations read appendix C the opposite way from the way this suite reads
  it. Whoever settles either one should look at both.

## Why this entry is kept

Because the alternative is worse. A manual conflict that is noticed and not
recorded gets re-discovered by the next reader, who has no way to tell whether
anyone looked — the reason [NM-HRM-014](NM-HRM-014.md) is kept, applied again.

And because the red assertion behind it will be met again. Someone reading only
the failing test will file it as a conformance defect against `agnus_copper.v`
and propose opening $000-$03E to the Copper at OCS, which would put the RTL in
conflict with p.289 and with all four reference implementations to satisfy one
sentence on p.279. This page exists so that proposal arrives already answered.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "The pointer regis ters 138 are loaded with data (an address) from data bus 404 by the processor 402 or coprocessor 47 under pro gram control."
  <!-- corpus: patent-us4874164-agnus-denise/p016.txt -->

  A general statement for all 25 DMA pointer registers, disk included: the Copper may load them, which is p.279's side. The patent mentions no protected register range.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 29 — agrees with the manual:

  > "These address registers must be initialized by the processor or Copper before disk DMA is enabled."
  <!-- corpus: agnus-8371-spec/p029.txt -->

  This is p.279's side, nearly word for word, for an OCS part. The same document also has p.289's side: Table 2 marks DSKPTH '+*020' (p031), and its legend reads '* Address not writeable by co-processor.' (p034). It therefore repeats the manual's contradiction and does not settle it.

Silent on this point: US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

