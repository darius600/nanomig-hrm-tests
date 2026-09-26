# NM-AA-002 — SPRHDAT is the one register in the Coprocessor-restricted range the AA list leaves unmarked

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | Nothing in the RTL. NanoMig restricts $078 exactly as it restricts every neighbouring address. What is asked is whether the missing `~` on SPRHDAT's row is a printing omission — which the other 58 rows in the range suggest — or a deliberate statement that this one register is reachable by the Coprocessor without CDANG. |
| **Module** | `src/minimig-aga/agnus_copper.v:225` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L225) at the pinned revision |
| **Report to** | **nowhere in code.** The defect is in a 1991 Commodore document, and the company is gone. It is filed so that no AA claim rests on SPRHDAT's row being complete |
| **Test** | [`aa/aa-copper-register-map`](../../aa/aa-copper-register-map) · seven graded assertions — all green. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF pp. 6 and 7, the address-ordered register list |
| **Assertions** | 7 green, each with its own mutation; `make mutate` reports 0 passed, 7 failed |
| **Verdict of the four** | **unanimous, and all four implement a RANGE where the document prints a per-register mark.** Every one of them blocks a Coprocessor MOVE to `$078` with CDANG clear by comparing the address against `$080`, none consults a table of marks, and so the mark SPRHDAT's row is missing is one no implementation would read. See [What the others say](#what-the-others-say) |

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

### Measured

[`aa/aa-copper-register-map`](../../aa/aa-copper-register-map) drives a
Coprocessor MOVE to `$078` and reads whether it reaches `reg_address_out`:
blocked with CDANG clear, allowed with CDANG set — the same behaviour as
every address the list marks `~` — `SPRHDAT-078-IS-RESTRICTED`, green, one of
the package's `7 passed, 0 failed`. See *What holds this up* for the
assertion in full.

## The measurement

`tools/aa/tilde_vs_copper_range.py` in the pipeline repository reads all 237
address-list rows and compares the two:

```
marked `~` by the document      : 58
below $080, the RTL's ECS rule  : 59

marked `~` but at or above $080 (RTL would allow the Coprocessor): 0
below $080 but NOT marked `~` (RTL blocks, document is silent)   : 1
    SPRHDAT    $078  p7
```

**Zero over-marks and one under-mark.** The document never claims a restriction
the RTL does not enforce; it omits one the RTL does.

These counts read 231 rows, 57 and 58 until 21.09.2026, and the conclusion is
the one thing that did not move. The row pattern held the mark column as the
character range `a-h`, which excludes `p` — a mark both legends define — and
could not read a name the document prints in brackets. Six rows were dropped in
silence: BPLCON4 and CLXCON2 (`p`), BPL7DAT and BPL8DAT (`&p`), FMODE (`P`), and
`(BPLHDAT)`. BPLHDAT is marked `~` and sits at $07A, so it joins both columns
and cancels out; SPRHDAT is still the single register the RTL blocks and the
document does not mark. Found by two blind readers of different slices
reporting the same shape, and confirmed by counting the rows both ways.

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

## What the others say

The document marks registers one at a time; every implementation read here
decides by address range instead. That makes the four unanimous about `$078`
without any of them having an opinion about SPRHDAT specifically.

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:2809`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2809) | `if (addr < ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {` — one comparison for every address there is. `$078` is below `$080`, so a Coprocessor MOVE to it is refused with CDANG clear and allowed with CDANG set |
| **vAmiga** | [`Copper.cpp:394`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L394-L402) | `isIllegalAddress`: `if (cdang) { return agnus.isOCS() ? addr < 0x40 : false; } else { return addr < 0x80; }` — the same boundary, with the OCS and ECS floors written out separately |
| **Denise** | [`copper.cpp:203`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L203) | `if (reg < cdang)`, where `cdang` is not a flag but the floor itself: `cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80` ([`:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L341-L342)). One comparison again |
| **Copperline** | [`dma_slots.rs:697`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L708) | `copper_can_write_custom` splits the range in two — `off <= 0x03E` needs ECS *and* CDANG, and `if (0x040..=0x07E).contains(&off) { return self.agnus.copper_danger_enabled(); }`. The upper range's top is `$07E`, so `$078` is inside it |
| **MiSTer** | [`rtl/agnus_copper.v:225`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225) | descendant — the identical `ir2[8:7]==2'b00 && !cdang` line |
| **TC64** | [`rtl/minimig/agnus_copper.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L225) | descendant — the identical line |

**All four agree with this RTL about `$078` and none of them can testify about
the mark.** The finding is that the printed list is silent where its own rule
would speak; four implementations blocking the address tells a reader that the
silence describes nothing anyone built, which is the useful half. Whether the
1991 silicon exempted SPRHDAT is a question none of these four is evidence
about, because not one of them models the document's marks at all.

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
