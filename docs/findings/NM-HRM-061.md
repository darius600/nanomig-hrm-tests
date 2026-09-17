# NM-HRM-061 — two reds that are not defects: a whole-machine sentence graded at a board's config register

| | |
|---|---|
| **Status** | open · red · **filed against the classification, not against the RTL and not against the page** |
| **Module** | `sysctrl.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/misc/sysctrl.v#L228-L246) |
| **Upstream** | none. `src/misc` is NanoMig's own substitute layer, not the Minimig chipset lineage (O62) |
| **Report to** | nobody. **This is a finding about this suite**, and the decision it asks for is O72's |
| **Test** | [`hrm/ch1-cpu-options`](../../hrm/ch1-cpu-options) `CPU-OPTIONS-FOUR-DISTINCT-STATES` and `RAM-BASE-FLOOR-BELOW-256K-ABSENT` |
| **Chapter** | 1 — Introduction · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 2 red. Both measure correctly. Neither red means the implementation is wrong |
| **Verdict of the four** | **not applicable, and that is the point.** No emulator has a `system_cpu` field; the sentences are about which machines Commodore sold |

---

## What the HRM says

Printed page 1, in the feature list that opens the manual:

> The Amiga also supports the 68010,68020,       and 68030 processors as an option.

> From 256K to 2 MB of RAM expandable to a total of 8 MB (over a gigabyte on the
> Amiga       3000).

Both sentences describe the **Amiga family** — what Commodore built and sold,
across models and expansion options. Neither is a requirement on any one machine.
A stock A500 supports no 68030 either, and an A1000 shipped with 256K that no
A3000 could be configured down to.

## What NanoMig does, and it is correct

`sysctrl.v:230-231` — a two-bit field with three meanings and its own remap:

```verilog
// Value "T": CPU 68000(0), 68010(1) or 68020(2)
if(id == "T") system_cpu <= data_in[1:0] == 2'b10 ? 2'b11 : data_in[1:0];
```

Raw 2 is remapped onto 3, so the four raw codes reach **three** distinct output
states. The comment names three processors and never a 68030.

`sysctrl.v:240` — the chip-memory floor:

```verilog
// Value "Y": Chipmem 512k(0), 1M(1), 1.5M(2) or 2M(2)
```

The lowest reachable configuration is 512k. There is no 256K state.

Both measurements are right, both assertions are honest, and both went red
because page 1 names a 68030 and a 256K floor that this board does not offer.

## Why this is not a defect

**The claim was graded at a boundary that cannot answer it.** `sysctrl` is a
configuration register in `src/misc` — the layer of NanoMig's own substitutes
for Amiga peripherals, which O62 admitted into the unit under test with the
explicit note that a finding there does not transfer to the shared chipset
lineage. Asking whether a board's configuration menu offers a 68030 is not
asking whether the Amiga chipset has been reimplemented correctly.

**Nothing on the other side to compare with.** The other three findings filed
this week each set NanoMig beside WinUAE, vAmiga, Denise and Copperline on the
same question. Here there is no same question: none of the four has a
`system_cpu` field, because none of them is a board that must be told which CPU
to instantiate. The absence of a comparison is itself the evidence that this is
not a conformance question.

**A red here reads as a defect and is not one.** That is the cost, and it is why
this is filed rather than quietly left: `tools/list_reds.py` reports two reds in
chapter 1 that a reader will take for two places where NanoMig disagrees with
the manual. It does not disagree. It is a different machine, and page 1 is
describing the family.

## What this actually is

**O72, surfacing as a red instead of as a label.** Forty-three of chapter 1's
discards argue in their own evidence that no `reason_kind` fits a claim about
hardware belonging to a machine this board is not; three classification lots
reported it independently and split 29 `absent` to 14 `not-hardware`. These two
claims are the same shape and were classified **testable** instead, so the gap
came out the other end as a failing assertion.

The fix is a decision, not a repair:

1. **Add the fifth `reason_kind`** O72 asks for, and reclassify these two claims
   (four uids: `sol`/`opus:1-components.c7` and `.c12`) as not-asserted under it.
   The two assertions become `record()` lines, chapter 1 goes from 235 testable
   to 233, and it must be re-accepted.
2. **Or leave them graded** and keep this page as the explanation a reader needs
   to not misread the reds.

Either is defensible. What is not defensible is a red with nothing next to it
saying what it is.

## What is left for specialists

Nothing about the Amiga. This one is entirely ours.
