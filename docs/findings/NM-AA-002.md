# NM-AA-002 — SPRHDAT is the one register in the Coprocessor-restricted range the AA list leaves unmarked

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | No RTL change follows. Is the missing `~` on SPRHDAT's row (`&h 078`, PDF p. 7) a printing omission, as the other 58 `~` rows in the range suggest, so that a Coprocessor MOVE to $078 needs COPCON bit 1 (CDANG) set, or a statement that the Coprocessor reaches $078 with CDANG clear? NanoMig blocks it, as it blocks every address below $080. Closes when an Amiga specialist records the mark as dropped in print, or confirms the exemption, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — AA documentation: whoever keeps errata for Commodore's AA specification |
| **Scope** | a Coprocessor MOVE to $078 (SPRHDAT) with CDANG clear and with CDANG set, measured with the Copper's `ecs` input high; chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 6 (the `~` legend) and p. 7 (the SPRHDAT row), the address-ordered register list · claim ids by assertion: `SPRHDAT-078-IS-RESTRICTED`: `opus:AA-sprhdat-tilde.c1`; `COP1LC-AT-080-082`: `opus:AA-cop1lch.c1`, `opus:AA-cop1lcl.c1`, `opus:AA-copjmp1.c1`, `sol:AA-cop1lch.c1`, `sol:AA-cop1lcl.c1`, `sol:AA-copjmp1.c1`; `COP2LC-AT-084-086`: `opus:AA-cop2lch.c1`, `opus:AA-cop2lcl.c1`, `opus:AA-copjmp2.c1`, `sol:AA-cop2lch.c1`, `sol:AA-cop2lcl.c1`, `sol:AA-copjmp2.c1`; `COPINS-AT-08C`: `opus:AA-copins.c1`, `sol:AA-copins.c1`; `COPCON-AT-02E`: `opus:AA-copcon.c1`, `sol:AA-copcon.c1`; `FIXTURE-DELIVERS-KNOWN-VALUE` and `COP1-AND-COP2-ARE-DISTINCT` carry no claim id |
| **Module** | `src/minimig-aga/agnus_copper.v:225` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L225) at the pinned revision; inherited (MiSTer and TC64 carry the same line) |
| **Test** | [`aa/aa-copper-register-map`](../../aa/aa-copper-register-map) · `make -C aa/aa-copper-register-map test` (all green, exits zero) |
| **Assertions** | `none` red. The package reports `7 passed, 0 failed`; the one that grades this page is `SPRHDAT-078-IS-RESTRICTED`: expected blocked with CDANG clear and allowed with CDANG set, observed the same. The package also records, ungraded, that the list's W/R/S access column cannot be graded at this boundary |
| **Verdict of the four** | **unanimous, by range not by mark**: WinUAE, vAmiga, Denise and Copperline each refuse a Coprocessor MOVE to $078 with CDANG clear and allow it with CDANG set, by comparing the address against $080; none consults a table of marks. Hardware: not measured |
| **Effort** | none — no RTL change; the suggested change is a citation rule for the corpus |

## Source requirement

The p. 6 legend defines the mark:

```
~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.
```

and the address list carries it per register. In the range the mark covers the rows run:

```
BLTCDAT   & ~070  W   A      Blitter source C data reg
BLTBDAT   & ~072  W   A      Blitter source B data reg
BLTADAT   & ~074  W   A      Blitter source A data reg
SPRHDAT  &h  078  W   A      ext logic UHRES sprite pointer & data identifier
LISAID    h ~07C  R      D   Chip Revision level for Denise/Lisa
DSKSYNC     ~07E  W         P  Disk sync pattern register for disk read.
```

SPRHDAT's row carries `&h` — two marks, neither of them `~`.

## RTL and measurement

NanoMig implements the mark as a range, not per register:

```verilog
if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs) // illegal access
  illegalreg = 1'b1;
```

`ir2[8:1]` is the word address shifted right once, so `ir2[8:7]==00` is every address below $080, and $078 is inside it.

[`aa/aa-copper-register-map`](../../aa/aa-copper-register-map) drives a Coprocessor MOVE to $078 and reads whether it reaches `reg_address_out`: blocked with CDANG clear, allowed with CDANG set, the same as for every address the list marks `~` (`SPRHDAT-078-IS-RESTRICTED`, green). A MOVE that reached `reg_address_out` with CDANG clear would fail it.

`tools/aa/tilde_vs_copper_range.py` reads all 237 address-list rows: 58 are marked `~`, 59 sit below $080. Zero are marked `~` at or above $080, so the document never claims a restriction the RTL does not enforce; one sits below $080 unmarked, SPRHDAT at $078 on p. 7, so it omits one the RTL does. Both counts include `(BPLHDAT)`, the name the list prints in brackets: it is marked `~` at $07A and cancels out.

## Risks and unknowns

- Whether the 1991 silicon exempted SPRHDAT or the mark was dropped in typesetting is not measured, and none of the implementations read is evidence about it, because none models the document's marks.
- The row already carries two marks where its neighbours carry one or two, so a third may not have fitted the column. That is a guess about a 1991 document's production and belongs to someone who has seen more of them.
- The package ran with the Copper's `ecs` input high only; the first clause of the RTL rule does not read it.

## Independent evidence

Two independent readers of the corpus reported the missing mark from the page, each calling SPRHDAT the one row in the $000-$07E span printed without it, and each declining to claim a Coprocessor-access fact because the page states nothing there.

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:2809`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2809) | `if (addr < ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {` — one comparison for every address. `$078` is below `$080`, so a Coprocessor MOVE to it is refused with CDANG clear and allowed with CDANG set |
| **vAmiga** | [`Copper.cpp:394`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L394-L402) | `isIllegalAddress`: `if (cdang) { return agnus.isOCS() ? addr < 0x40 : false; } else { return addr < 0x80; }` — the same boundary, with the OCS and ECS floors written out separately |
| **Denise** | [`copper.cpp:203`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L203) | `if (reg < cdang)`, where `cdang` is the floor itself: `cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80` ([`:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L341-L342)) |
| **Copperline** | [`dma_slots.rs:697`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L708) | `copper_can_write_custom` splits the range in two: `off <= 0x03E` needs ECS *and* CDANG, and `if (0x040..=0x07E).contains(&off) { return self.agnus.copper_danger_enabled(); }`. The upper range's top is `$07E`, so `$078` is inside it |
| **MiSTer** | [`rtl/agnus_copper.v:225`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225) | descendant — the identical `ir2[8:7]==2'b00 && !cdang` line |
| **TC64** | [`rtl/minimig/agnus_copper.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L225) | descendant — the identical line |

## Decision

No RTL change. If the Amiga specialist records the mark as dropped in print, the page stays against the printed row; if the exemption is confirmed, the page is refiled against the RTL and NanoMig's block at $078 becomes the question.

For the corpus: a claim about SPRHDAT's Coprocessor accessibility cites this finding rather than the row, because the row is silent and silence is not a permission. Re-run: `make -C aa/aa-copper-register-map test`.
