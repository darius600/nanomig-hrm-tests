# NM-HRM-005 — WAIT and SKIP take longer than the manual states [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Does a real Amiga take 4 memory cycle times for a `SKIP` and 6 for a satisfied `WAIT`, as pp. 20 and 275 print, where NanoMig takes 8 and 8? Closes when a timing from a real machine is reported and, if it matches the print, NanoMig's `WAITSKIP1`/`WAITSKIP2` states take 4 memory cycle times for a `SKIP` and 6 for a satisfied `WAIT`, turning `P3`, `P4`, `P5`, `P6`, `AC5`, `AC6`, `C5` and `C6` green; if it matches NanoMig's 8 and 8, this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | `agnus_copper.v` at the DMA handshake: `SKIP` taken and not taken and a `WAIT` already satisfied on arrival (`P3` to `P6`, `AC5`, `AC6`), counted in memory cycle times; `C5` and `C6` count the bus grants of a `WAIT` that blocks and then wakes. Chip revision unknown; real hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 2 — Coprocessor, printed p. 20, and Appendix A, printed p. 275 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claim ids, which carry no reader prefix: `P3`, `P4`: `COP2-what-is-a-copper-instruction.c17`, `ch2-what-is-a-copper-instruction.c15` (p. 20); `P5`: `COP2-what-is-a-copper-instruction.c19`, `ch2-what-is-a-copper-instruction.c17` (p. 20); `P6`: `COP2-what-is-a-copper-instruction.c20` (p. 20); `C5`: `COP2-what-is-a-copper-instruction.c18`, `ch2-what-is-a-copper-instruction.c16` (p. 20); `C6`: `COP2-what-is-a-copper-instruction.c20`, `ch2-what-is-a-copper-instruction.c18` (p. 20); `AC5` and `AC6` (p. 275) carry no claim id |
| **Module** | `agnus_copper.v` **lines 384-548**, the `copper_state` machine that spends the cycles — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L384-L548) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v); inherited |
| **Test** | [`hrm/ch2-copper-cycle-times`](../../hrm/ch2-copper-cycle-times): `make -C hrm/ch2-copper-cycle-times test`; [`hrm/ch2-copper-cycles`](../../hrm/ch2-copper-cycles): `make -C hrm/ch2-copper-cycles test`; an expected red exits nonzero |
| **Assertions** | 8 red. In `hrm/ch2-copper-cycle-times` (`7 passed, 6 failed`): `P3` and `P4` (`SKIP` taken and not taken, expected 4, observed 8), `P5` (satisfied `WAIT`, 6 → 8), `P6` (`WAIT` − `MOVE`, 2 → 4), `AC5` (`SKIP`, p. 275, 4 → 8) and `AC6` (`SKIP`, 4 → 8; expository, not independent). In `hrm/ch2-copper-cycles` (`10 passed, 2 failed`): `C5` (`WAIT`, three bus grants expected) and `C6` (`WAIT` one grant more than a `MOVE` expected); the test's comment records two grants, the same as a `MOVE` |
| **Verdict of the four** | **unanimous with NanoMig, against pp. 20 and 275**: WinUAE, vAmiga, Denise and Copperline, read from their source code and not run, all give a `MOVE` 4, a `SKIP` 8 and a satisfied `WAIT` 8 memory cycles; none gives the manual's `SKIP` 4 or `WAIT` 6. Copperline names Minimig and vAmiga as its model for these cycles. Hardware: not measured |
| **Effort** | none — no RTL change is asked; a timing program on a real machine would decide it |

---

## Source requirement

**Printed pp. 20 and 275:**

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

`agnus_copper.v:384-548` decodes `copper_state` and decides, per DMA slot, whether the Copper fetches, waits, skips or halts ([NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L384-L548)). The test does not read that machine: it drives the module's DMA handshake and counts the allocated slots between two instructions, so the line range is where to look for a cause, not where the measurement is taken.

| | manual | NanoMig |
|---|---:|---:|
| `SKIP` | 4 | **8** |
| satisfied `WAIT` | 6 | **8** |
| `WAIT` − `MOVE` | 2 | **4** |

`P3` to `P6` grade these rows, and `AC5` and `AC6` restate the `SKIP` row in p. 275's units. `C5` and `C6` count bus grants of a `WAIT` that blocks and then wakes.

## Risks and unknowns

- Real hardware is not measured.
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
| **MiSTer** | descendant — `rtl/agnus_copper.v:473-536` — identical to NanoMig's `WAITSKIP1`/`WAITSKIP2` states and their cycle counts |
| **TC64** | descendant — `rtl/minimig/agnus_copper.v:473-536` — the same states, byte for byte. The only difference either line has from NanoMig in this file (blocking where NanoMig assigns non-blocking, lines 234-274) does not touch the WAIT/SKIP timing |

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

## Decision

**None, and a patch would be the wrong instrument.** All four emulators produce NanoMig's numbers, so either the manual's cycle figures are wrong or every implementation shares a misreading.

**This is decidable on real hardware and by no other means available here.** A small 68k program timing a loop of `WAIT`s against a CIA timer, run on a real machine, settles it outright instead of mediating between a 1991 manual and five implementations, NanoMig and the four emulators. Both Copperline's `timing-test/` and vAmigaTS arrive independently at the same technique — probe programs booted from an ADF, results read from CRT photographs — and Copperline states the principle: *where real hardware and FS-UAE disagree, real hardware wins.*

Until someone runs that, the status is "measured, matched by all four emulators, unresolved". The finding closes as stated in *What is asked of a reader*.
