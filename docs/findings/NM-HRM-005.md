# NM-HRM-005 — NanoMig takes 8 memory cycle times for a SKIP and a satisfied WAIT, where p. 20 prints 4 and 6 and p. 275 prints 4 for the SKIP

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this; the red is against pp. 20 and 275, not against the RTL. Question: do pp. 20 and 275 need correcting, so that a `SKIP` reads 8 memory cycle times where both print 4, and a satisfied `WAIT` 8 where p. 20 prints 6 and one extra memory cycle to wake up? Closes when an Amiga specialist records the figures of pp. 20 and 275 as corrected, or confirms that the pages hold as printed, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: an issue on this repository whose title begins with NM-HRM-005 |
| **Scope** | `agnus_copper.v` at the DMA handshake: `SKIP` taken and not taken and a `WAIT` already satisfied on arrival (`P3` to `P6`, `AC5`, `AC6`), counted in memory cycle times; `C5` and `C6` count the bus grants of a `WAIT` that blocks and then wakes. Chip revision unknown; photographs named A500 ECS and A500 OCS reach the masked forms of a `SKIP` not taken and a `WAIT` satisfied on arrival |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 2 — Coprocessor, printed p. 20, and Appendix A, printed p. 275 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claim ids, which carry no reader prefix: `P3`, `P4`: `COP2-what-is-a-copper-instruction.c17`, `ch2-what-is-a-copper-instruction.c15` (p. 20); `P5`: `COP2-what-is-a-copper-instruction.c19`, `ch2-what-is-a-copper-instruction.c17` (p. 20); `P6`: `COP2-what-is-a-copper-instruction.c20` (p. 20); `C5`: `COP2-what-is-a-copper-instruction.c18`, `ch2-what-is-a-copper-instruction.c16` (p. 20); `C6`: `COP2-what-is-a-copper-instruction.c20`, `ch2-what-is-a-copper-instruction.c18` (p. 20); `AC5` and `AC6` (p. 275) carry no claim id |
| **Module** | `agnus_copper.v` **lines 384-548**, the `copper_state` machine that spends the cycles — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L384-L548) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v); inherited |
| **Test** | [`hrm/ch2-copper-cycle-times`](../../hrm/ch2-copper-cycle-times): `make -C hrm/ch2-copper-cycle-times test`; [`hrm/ch2-copper-cycles`](../../hrm/ch2-copper-cycles): `make -C hrm/ch2-copper-cycles test`; an expected red exits nonzero |
| **Assertions** | 8 red. In `hrm/ch2-copper-cycle-times` (`7 passed, 6 failed`): `P3` and `P4` (`SKIP` taken and not taken, expected 4, observed 8), `P5` (satisfied `WAIT`, 6 → 8), `P6` (`WAIT` − `MOVE`, 2 → 4), `AC5` (`SKIP`, p. 275, 4 → 8) and `AC6` (`SKIP`, 4 → 8; expository, not independent). In `hrm/ch2-copper-cycles` (`10 passed, 2 failed`): `C5` (`WAIT`, three bus grants expected) and `C6` (`WAIT` one grant more than a `MOVE` expected); the test's comment records two grants, the same as a `MOVE` |
| **Verdict of the four** | **unanimous with NanoMig, against pp. 20 and 275**: WinUAE, vAmiga, Denise and Copperline, read from their source code and not run, all give a `MOVE` 4, a `SKIP` 8 and a satisfied `WAIT` 8 memory cycles; none gives the manual's `SKIP` 4 or `WAIT` 6. Copperline names Minimig and vAmiga as its model for these cycles. Hardware: not run here; photographs named A500 ECS and A500 OCS show a `SKIP` not taken and a `WAIT` satisfied on arrival, in their masked forms, at 8 memory cycle times each (*What photographs of real machines show*) |
| **Effort** | none; if the pages are confirmed as printed, M: `agnus_copper.v` only: a `SKIP` leaves `FETCH2` for `FETCH1` past `WAITSKIP1` and `WAITSKIP2` (line 463), with the second word and the beam compare (lines 187, 309-319) taken a slot earlier; a satisfied `WAIT` drops one of the two states; `C5` and `C6` also need the wake-up to request the bus, which neither state does |

---

## Source requirement

**Printed p. 20; p. 275 prints the `SKIP` row only:**

| instruction | memory cycle times |
|---|---|
| `SKIP` | 4 |
| satisfied `WAIT` | 6 |
| `WAIT` minus `MOVE` | 2 |

**p. 20:**

> The WAIT instruction requires three memory cycles and six memory cycle times; it takes one extra memory cycle to wake up.

**p. 275:**

> Since only the odd bus cycles are requested, four memory cycle times are required per instruction (memory cycles are 280 ns.)

Appendix A p. 275 defines the bus cycle against the memory cycle time and gives the factor two, so NanoMig's counts and the printed figures are in the same unit.

## RTL and measurement

`agnus_copper.v:384-548` decodes `copper_state` and decides, per DMA slot, whether the Copper fetches, waits, skips or halts ([NanoMig](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L384-L548)). The test does not read that machine: it drives the module's DMA handshake and counts the allocated slots between two instructions, so the line range is where to look for a cause, not where the measurement is taken.

| | manual | NanoMig |
|---|---:|---:|
| `SKIP` | 4 | **8** |
| satisfied `WAIT` | 6 | **8** |
| `WAIT` − `MOVE` | 2 | **4** |

`P3` to `P6` grade these rows, and `AC5` and `AC6` restate the `SKIP` row in p. 275's units. `C5` and `C6` count bus grants of a `WAIT` that blocks and then wakes.

## Risks and unknowns

- Real hardware is not run here, apart from photographs named A500 ECS and A500 OCS, which reach a masked `SKIP` not taken and a masked `WAIT` satisfied on arrival, not `C5` and `C6`, and leave p. 20's wake-up cycle open (*What photographs of real machines show*).
- The four emulators were read from their Copper state machines, not run. They are fewer than four independent witnesses: Copperline's comments name Minimig's `WAITSKIP1`/`WAITSKIP2` and vAmiga's `COP_WAIT1`/`COP_WAIT2` as the model for these cycles ([`:256-264`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L256-L264), [`:517-525`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L517-L525)), so its agreement weighs least. Its doc comment on `step_eligible_slot` still reads `MOVE/SKIP spans 4 color clocks and a WAIT 6` ([`src/chipset/copper.rs:690-694`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L690-L694)), which the states it documents do not do.
- `AC6` is expository, not independent: given `AC4` and `AC5` it cannot fail on its own, so it is no separate corroboration.
- The US patent text below closely paraphrases the manual's own Copper-timing text, so it is not an independent measurement.

## Independent evidence

| | |
|---|---|
| **WinUAE** | **4 / 8 / 8** for `MOVE` / `SKIP` / satisfied `WAIT`. The Copper moves only on odd colour clocks ([`custom.cpp:9067-9073`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9067-L9073)). After a `MOVE` the next first word goes out on the following odd cycle ([`:9118-9126`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9118-L9126)): 4. A `WAIT` or `SKIP` restarts only once the Copper reads idle, first true on the third odd cycle after the first word ([`:9090-9103`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9090-L9103)), and fetches one odd cycle later ([`:9137-9142`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9137-L9142)): 8. No chipset setting enters it |
| **vAmiga** | **4 / 8 / 8**. Each Copper state is scheduled two DMA cycles after the one before ([`Copper.h:331`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.h#L331)); a `MOVE` passes two states, a `SKIP` or a satisfied `WAIT` four (`CopperEvents.cpp`) |
| **Denise** | **4 / 8 / 8**. The Copper is stepped only on even colour clocks ([`agnus.cpp:656-659`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L656-L659)), so each state is two memory cycles. A `MOVE` goes straight back to `Read1`: two states. A `SKIP` passes `Skip1` and `Skip2` ([`copper.cpp:176-241`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L176-L241)) and a satisfied `WAIT` passes `Wait1` and `Wait3`, which needs a free cycle ([`:242-276`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L242-L276)): four states each |
| **Copperline** | **4 / 8 / 8**. It fetches only on even colour clocks ([`copper.rs:614-616`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L614-L616)). A `WAIT` or `SKIP` spends two bus-free cycles after its second word, and its comments give the result: `so an immediately-true WAIT's next fetch lands at fetch2+6` ([`:517-531`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L517-L531)) and `so the post-SKIP fetch lands at fetch2+6 -- the same footprint as a WAIT` ([`:571-583`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L571-L583)); first word to first word is 8. Its unit tests assert the same on the bus ([`tests.rs:3706-3725`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L3706-L3725), [`:3793-3819`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L3793-L3819)) |
| **MiSTer** | descendant — [`rtl/agnus_copper.v:473-536`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L473-L536) — identical to NanoMig's `WAITSKIP1`/`WAITSKIP2` states and their cycle counts |
| **TC64** | descendant — [`rtl/minimig/agnus_copper.v:473-536`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L473-L536) — the same states, byte for byte. The only difference either line has from NanoMig in this file (blocking where NanoMig assigns non-blocking, lines 234-274) does not touch the WAIT/SKIP timing |

Every row counts colour clocks on a free bus, and a colour clock is one memory cycle of 280 ns, so the numbers stand beside the manual's without conversion.

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 18 — agrees with the manual:

  > "only the odd memory cycles are requested by the Copper 47, so four memory cycles are required for execution of MOVE and SKIP instructions and six memory cycles are re quired for the WAIT instruction."
  <!-- corpus: patent-us5103499-copper/p018.txt -->

  SKIP 4, WAIT 6, and so WAIT minus MOVE 2 — all three of the manual's figures, in the manual's unit (memory cycles, the Copper using every other one), against NanoMig's 8 / 8 / 4. The sentence just before gives the underlying 2 and 3 cycles.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 35 — agrees with the manual:

  > "NOTE: Copper Data Move instructions require four (4) slots. Copper Wait instructions require six (6) slots."
  <!-- corpus: agnus-8371-spec/p035.txt -->

  MOVE takes 4 slots and WAIT 6, so WAIT minus MOVE is 2. p027 gives SKIP the non-WAIT figure: 'All instructions (except for WAIT) require 2' bus cycles, so '4 memory cycle times are required per instruction. (memory cycles are 280 ns)'. Those are the manual's 4/6/2 in memory cycles, against NanoMig's 8/8/4.

### What photographs of real machines show

[vAmigaTS](https://github.com/dirkwhoffmann/vAmigaTS/tree/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask), vAmiga's test suite, publishes a screen photograph per program. The photographs of [`waitmask1`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask1/waitmask1.s) and [`skipmask1`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/skipmask1/skipmask1.s) are named A500 ECS and A500 OCS, labels and nothing more ([waitmask1 ECS](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask1/waitmask1_A500_ECS.JPG), [waitmask1 OCS](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask1/waitmask1_A500_OCS.JPG), [skipmask1 ECS](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/skipmask1/skipmask1_A500_ECS.JPG), [skipmask1 OCS](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/skipmask1/skipmask1_A500_OCS.JPG)). No bitplane DMA runs near horizontal positions `$30` to `$40` (DDFSTRT = `$C0`, [`waitmask1.s:2`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask1/waitmask1.s#L2), [`skipmask1.s:2`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/skipmask1/skipmask1.s#L2)). A ruler of MOVEs after a WAIT for ($4E,$38) is the reference ([`waitmask.i:191-192`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask.i#L191-L192)). In four rounds, at horizontal positions `$30`, `$32`, `$34` and `$36` ([`waitmask.i:194`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask.i#L194)), a masked WAIT whose mask `$10` is true on arrival ([`waitmask.i:209-212`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/waitmask.i#L209-L212)) is followed by two colour MOVEs; `skipmask1` runs a masked SKIP in the same place, then a yellow and a blue MOVE ([`skipmask.i:194-198`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/skipmask.i#L194-L198)): where the mask leaves the compare false on arrival the SKIP is not taken and both bars show; where it is true (mask `$10`, [`skipmask.i:212-216`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/Copper/mask/skipmask.i#L212-L216)) the SKIP is taken and only the blue shows. Each round starts 2 colour clocks later by beam position alone, which checks the scale of 5.39 to 5.43 pixels per colour clock without assuming that a MOVE takes 4.

Where the bars start, in colour clocks from the ruler, over the four rounds:

| | A500 ECS | A500 OCS | 8 predicts | 6 (WAIT) or 4 (SKIP) predicts |
|---|---|---|---|---|
| WAIT satisfied on arrival | -0.01, +1.96, +3.97, +5.87 | 0.00, +2.00, +4.06, +6.05 | 0, 2, 4, 6 | -2, 0, 2, 4 |
| SKIP not taken, yellow | -0.11, +1.92, +3.87, +5.89 | -0.05, +1.95, +3.95, +5.91 | 0, 2, 4, 6 | -4, -2, 0, 2 |
| SKIP taken, blue, rounds 1 to 3 | +3.98, +5.99, +7.98 | +4.03, +6.02, +8.03 | | |

Over 33 rows the not-taken SKIP fits 8 - 0.15 colour clocks on A500 ECS and 8 - 0.08 on A500 OCS, where 4 would need the bars 22 pixels to the left. So a SKIP not taken and a WAIT satisfied on arrival take 8 and 8 memory cycle times, as NanoMig and the four implementations have it, and not p. 20's and p. 275's 4 for the SKIP or, as this page reads p. 20, 6 for the WAIT. With the SKIP taken no yellow shows and the blue bar starts where it starts with the SKIP not taken: the SKIP and the MOVE it skips together take 12, and the photograph does not divide that sum.

The limits: the masked forms only; the taken SKIP only as that sum; `C5` and `C6`, a WAIT that blocks and then wakes, are not reached; p. 20's "one extra memory cycle to wake up" leaves open whether a satisfied WAIT's 8 divides as 6 and 2: read that way p. 20's figures add up to 8, and only the SKIP figure is wrong. The bars are read from the photographs, with their start measured against the ruler; the programs were not run on a machine here.

### What practitioners report

Reports from the forum thread [EAB 19676](https://eab.abime.net/showthread.php?t=19676): witnesses, never an oracle ([practitioner reports](../evidence/practitioner-reports.md)).

**EXPLAINS.** [EAB post 1787593](https://eab.abime.net/showpost.php?p=1787593), Toni Wilen, 2026-03-06. Chip set: from the Alice schematics; "all chipset versions" is said only for the two Copper bugs. Known from: the schematics; post 1787687 adds "based on schematics and knowledge of copper's known odd behavior".

> "SKIP also "stops" the copper like WAIT due to LOAD2 "If not MOVE" condition and requires "wakeup" condition."

SKIP and a satisfied WAIT share one longer path through the Copper's states, which fits the 8 and 8 the four implementations give; the post gives no cycle counts; the real-machine timing is under *What photographs of real machines show*.

## Decision

**No RTL change.** NanoMig already takes 8 memory cycle times for a `SKIP` and for a satisfied `WAIT`, as all four implementations read here do. Photographs named A500 ECS and A500 OCS show 8 for a masked `SKIP` not taken and a masked `WAIT` satisfied on arrival, and a taken `SKIP` only as a sum of 12 with the `MOVE` it skips. Making a `SKIP` take 4 and a satisfied `WAIT` 6 would meet the figures of pp. 20 and 275, which `P3` to `P6`, `AC5` and `AC6` grade; it would also make NanoMig the only implementation read here that takes them, and move it away from the photographs.

The change, if one is made, belongs to the manual: pp. 20 and 275 print 4 for a `SKIP` and p. 20 prints 6 for a `WAIT`, and photographs named A500 ECS and A500 OCS show 8 and 8 in the masked forms they reach. What they leave open: the bus grants of a `WAIT` that blocks and then wakes, `C5` and `C6`, which a screen does not show; and whether p. 20's "one extra memory cycle to wake up" is the third of its three cycles or comes on top of them. On top, a satisfied `WAIT`'s 8 is p. 20's 6 and 2, and p. 20 needs correcting for the `SKIP` alone. The page closes as row 4 says.
