# NM-HRM-061 — two reds that are not defects: a whole-machine sentence graded at a board's config register

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the classification |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change. Question: should p.1's "68030" and "From 256K" sentences be reclassified as not-asserted under a fifth `reason_kind`, because they describe which machines Commodore sold and not what NanoMig's `sysctrl.v` offers? Closes when the claim classification in hrm-testpipeline either adds that `reason_kind` and chapter 1 is re-accepted with the two assertions as `record()` lines, or keeps them graded with this page as their explanation. |
| **Report to** | `no RTL action` — hrm-testpipeline: whoever maintains the suite's claim classification |
| **Scope** | NanoMig's `sysctrl.v` config values `id='T'` (`system_cpu`) and `id='Y'` (`system_chipmem`) at `4495f86`; no Amiga chip is involved |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 1 — Introduction, printed p. 1 (feature list) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. `CPU-OPTIONS-FOUR-DISTINCT-STATES`: `sol:1-components.c7`, `opus:1-components.c7`, p. 1. `RAM-BASE-FLOOR-BELOW-256K-ABSENT` carries no claim id; the ids `sol:1-components.c12` and `opus:1-components.c12` (p. 1) sit on the package's `record()` line, which records that sentence as partially graded |
| **Module** | `sysctrl.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/sysctrl.v#L228-L246); in neither MiSTer nor TC64; NanoMig's own |
| **Test** | [`hrm/ch1-cpu-options`](../../hrm/ch1-cpu-options): `make -C hrm/ch1-cpu-options test`; an expected red exits nonzero |
| **Assertions** | 2 red, both measure correctly. `CPU-OPTIONS-FOUR-DISTINCT-STATES`: expected 4 pairwise-distinct `system_cpu` states, observed 3. `RAM-BASE-FLOOR-BELOW-256K-ABSENT`: expected a `system_chipmem` state at 256K, observed floor raw 0 documented `"512k"`. The package reports `3 passed, 2 failed` |
| **Verdict of the four** | **not applicable, and that is the point.** No emulator has a `system_cpu` field; the sentences are about which machines Commodore sold. Hardware: not measured |
| **Effort** | none — no RTL change (option 1 changes the classification: the `reason_kind` vocabulary, four claim ids and two assertions in chapter 1, and its re-acceptance) |

---

## Source requirement

Printed page 1, in the feature list that opens the manual:

> The Amiga also supports the 68010,68020,       and 68030 processors as an option.

> From 256K to 2 MB of RAM expandable to a total of 8 MB (over a gigabyte on the
> Amiga       3000).

Both sentences describe the **Amiga family**: what Commodore built and sold,
across models and expansion options. Neither is a requirement on any one machine.
A stock A500 supports no 68030 either, and an A1000 shipped with 256K that no
A3000 could be configured down to.

## RTL and measurement

`sysctrl.v:230-231` is a two-bit field with three meanings and its own remap:

```verilog
// Value "T": CPU 68000(0), 68010(1) or 68020(2)
if(id == "T") system_cpu <= data_in[1:0] == 2'b10 ? 2'b11 : data_in[1:0];
```

Raw 2 is remapped onto 3, so the four raw codes reach **three** distinct output
states. The comment names three processors and never a 68030.

`sysctrl.v:240` documents the chip-memory floor: `// Value "Y": Chipmem
512k(0), 1M(1), 1.5M(2) or 2M(2)`. The lowest reachable configuration is 512k;
there is no 256K state.

[`hrm/ch1-cpu-options`](../../hrm/ch1-cpu-options) sweeps the four raw values
`id='T'` accepts: `system_cpu` reaches 3 pairwise-distinct states, not the four
processor types p.1 names, `CPU-OPTIONS-FOUR-DISTINCT-STATES`, red. It also reads
`system_chipmem` at raw value 0, the field's numeric floor: documented `"512k"`,
not 256K, `RAM-BASE-FLOOR-BELOW-256K-ABSENT`, red. Both measurements are right;
the reds come from what page 1 names, which this board does not offer.

The claim was graded at a boundary that cannot answer it. `sysctrl` is a
configuration register in `src/misc`, the layer of NanoMig's own substitutes for
Amiga peripherals; a finding there does not transfer to the shared chipset
lineage, as the package's own assertion messages say. Asking
whether a board's configuration menu offers a 68030 is not asking whether the
Amiga chipset has been reimplemented correctly.

## Risks and unknowns

- **A red here reads as a defect and is not one.** `tools/list_reds.py` reports
  two reds in chapter 1 that a reader will take for two places where NanoMig
  disagrees with the manual. It does not; page 1 describes the family.
- No comparison exists to check this against: none of the four has a
  `system_cpu` field, so the finding cannot be confirmed or refuted by a witness.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | no vote | no `system_cpu` field; not a board that must be told which CPU to instantiate |
| **vAmiga** | no vote | same |
| **Denise** | no vote | same |
| **Copperline** | no vote | same |
| **MiSTer** | no `sysctrl.v` (descendant) | absent from `MiSTer-devel/Minimig-AGA_MiSTer`'s `rtl/` |
| **TC64** | no `sysctrl.v` (descendant) | absent from `minimig-dev/MinimigAGA-MiST-TC64`'s `rtl/minimig/` |

[NM-HRM-060](NM-HRM-060.md), [NM-HRM-062](NM-HRM-062.md) and
[NM-HRM-063](NM-HRM-063.md) each set NanoMig beside the four on the same
question; here there is no same question, and that absence is the evidence that
this is not a conformance question. `src/misc` is NanoMig's own substitute layer,
so neither Minimig line has an ancestor to inherit this from and a fix has
nowhere to travel.

## Decision

No question remains for an Amiga specialist; this is entirely the suite's. The
reds are the open question of a fifth `reason_kind` surfacing as a red instead of as a label: forty-three of chapter 1's
discards argue in their own evidence that no `reason_kind` fits a claim about
hardware belonging to a machine this board is not; three classification lots
reported it independently and split 29 `absent` to 14 `not-hardware`. These two
claims are the same shape and were classified **testable** instead.

1. **Add a fifth `reason_kind`**, and reclassify these two claims
   (`sol:1-components.c7`, `opus:1-components.c7`, `sol:1-components.c12`,
   `opus:1-components.c12`) as not-asserted under it.
   The two assertions become `record()` lines, chapter 1 goes from 235 testable
   to 233, and it must be re-accepted.
2. **Or leave them graded** and keep this page as the explanation a reader needs
   to not misread the reds.

Either is defensible; a red with nothing next to it saying what it is, is not.
