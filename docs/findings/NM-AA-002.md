# NM-AA-002 — SPRHDAT is the one register in the Coprocessor-restricted range the AA list leaves unmarked

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a graded green with an inverse control |
| **What is asked of a reader** | Nothing in the RTL. NanoMig restricts $078 exactly as it restricts every neighbouring address. What is asked is whether the missing `~` on SPRHDAT's row is a printing omission — which the other 57 rows in the range suggest — or a deliberate statement that this one register is reachable by the Coprocessor without CDANG. |
| **Module** | `src/minimig-aga/agnus_copper.v:225` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L225) at the pinned revision |
| **Report to** | **nowhere in code.** The defect is in a 1991 Commodore document, and the company is gone. It is filed so that no AA claim rests on SPRHDAT's row being complete |
| **Test** | [`aa/aa-copper-register-map`](../../aa/aa-copper-register-map) · seven graded assertions — all green. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF pp. 6 and 7, the address-ordered register list |
| **Assertions** | 7 green, each with its own mutation; `make mutate` reports 0 passed, 7 failed |
| **Verdict of the four** | not consulted — this is a defect in a document, not a behaviour the implementations could disagree about |

## What the AA specification says

The p. 6 legend defines the mark:

```
~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.
```

and the address list then carries it per register. In the range the mark covers,
the rows run:

```
BLTCDAT   & ~070  W   A      Blitter source C data reg
BLTBDAT   & ~072  W   A      Blitter source B data reg
BLTADAT   & ~074  W   A      Blitter source A data reg
SPRHDAT  &h  078  W   A      ext logic UHRES sprite pointer & data identifier
LISAID    h ~07C  R      D   Chip Revision level for Denise/Lisa
DSKSYNC     ~07E  W         P  Disk sync pattern register for disk read.
```

SPRHDAT's row carries `&h` — two marks, neither of them `~`.

## What settles the range

NanoMig does not implement the mark per register. It implements a range:

```verilog
if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs) // illegal access
  illegalreg = 1'b1;
```

`ir2[8:1]` is the word address shifted right once, so `ir2[8:7]==00` is every
address below $080 — and $078 is inside it.

## The measurement

`tools/aa/tilde_vs_copper_range.py` in the pipeline repository reads all 231
address-list rows and compares the two:

```
marked `~` by the document      : 57
below $080, the RTL's ECS rule  : 58

marked `~` but at or above $080 (RTL would allow the Coprocessor): 0
below $080 but NOT marked `~` (RTL blocks, document is silent)   : 1
    SPRHDAT    $078  p7
```

**Zero over-marks and one under-mark.** The document never claims a restriction
the RTL does not enforce; it omits one the RTL does.

## What holds this up

`SPRHDAT-078-IS-RESTRICTED` in the package: a Coprocessor MOVE to $078 does not
reach `reg_address_out` with CDANG clear and does reach it with CDANG set —
which is what the RTL does for every address the list marks `~`, done for the
one it does not.

Two independent readers of the corpus reported the missing mark from the page
before this comparison was made, each calling SPRHDAT the one row in the
$000-$07E span printed without it, and each declining to claim it as a
Coprocessor-access fact because the page states nothing there. The brief tells a
reader to put such an absence in its report rather than in a claim, and both
did.

## Suggested change

None to the RTL.

For the corpus: a claim about SPRHDAT's Coprocessor accessibility cites this
finding rather than the row, because the row is silent and silence is not a
permission.

## What is left for specialists

Whether SPRHDAT was genuinely exempt in the silicon or the mark was simply
dropped in typesetting. The row already carries two marks where its neighbours
carry one or two, so a third may not have fitted the column — that is a
guess about a 1991 document's production, and it belongs to someone who has
seen more of them than this project has.
