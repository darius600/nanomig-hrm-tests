# NM-HRM-005 — WAIT and SKIP take longer than the manual states [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_copper.v` **lines 384-548**, the `copper_state` machine that spends the cycles — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L384-L548) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v) |
| **Report to** | **nowhere yet** — this one needs hardware, not a patch. See *Suggested change* |
| **Test** | [`hrm/ch2-copper-cycle-times`](../../hrm/ch2-copper-cycle-times), [`hrm/ch2-copper-cycles`](../../hrm/ch2-copper-cycles) |
| **Chapter** | 2 — Coprocessor · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 8 red: `P3` and `P4` (`SKIP`), `P5` (satisfied `WAIT`), `P6` (`WAIT` − `MOVE`), `AC5` and `AC6` (`SKIP`, p. 275) in `hrm/ch2-copper-cycle-times`; `C5` (satisfied `WAIT`) and `C6` (`WAIT` − `MOVE`) in `hrm/ch2-copper-cycles`. `AC6` is expository, not independent (*What holds the finding up*) |
| **Verdict of the four** | **unanimous with NanoMig, against pp. 20 and 275**: WinUAE, vAmiga, Denise and Copperline all give a `MOVE` 4, a `SKIP` 8 and a satisfied `WAIT` 8 memory cycles; none gives the manual's `SKIP` 4 or `WAIT` 6. Copperline names Minimig and vAmiga as its model for these cycles |

---

## What the HRM says

**Printed pp. 20 and 275:**

| instruction | memory cycle times |
|---|---|
| `SKIP` | 4 |
| satisfied `WAIT` | 6 |
| `WAIT` minus `MOVE` | 2 |

---

## What NanoMig does

### Measured

| | manual | NanoMig |
|---|---:|---:|
| `SKIP` | 4 | **8** |
| satisfied `WAIT` | 6 | **8** |
| `WAIT` − `MOVE` | 2 | **4** |

`SKIP` and `WAIT` − `MOVE` are exactly double the printed figure; a satisfied
`WAIT` is 8 against 6.

**Where the cycles are spent.** The counting happens in one combinational state
machine: `agnus_copper.v:384-548` decodes `copper_state` and decides, per DMA
slot, whether the copper fetches, waits, skips or halts. `:373-375` is the
register that advances it. The test does not read that machine directly — it
drives the module's own DMA handshake and counts the allocated slots between two
instructions, which is what the manual's "memory cycle times" counts — so the
line range is where to look for a cause, not where the measurement is taken.

**The unit question is settled.** Appendix A p. 275 defines the bus cycle against
the memory cycle time and gives the factor two, so the two readings are
comparable and the deviation is real rather than an artefact of counting in
different units.

---

## What the others say

| | |
|---|---|
| **WinUAE** | **4 / 8 / 8** for `MOVE` / `SKIP` / satisfied `WAIT` — NanoMig's numbers. `generate_copper()` runs once per colour clock and moves the Copper only on odd ones ([`custom.cpp:9067-9073`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9067-L9073): `bool odd_cycle = (agnus_hpos & 1) == COPPER_CYCLE_POLARITY;`); a step set on one odd cycle is promoted from 1 to 2 on the next and acts then. After a `MOVE`'s second-word fetch the next first-word fetch goes out on the following odd cycle ([`:9118-9126`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9118-L9126): `if (cop_state.validmove && !cop_state.irload1)`), so first word to first word is 4. A `WAIT` or `SKIP` has no such branch. It restarts only once the Copper reads idle, `bool idle = !cop_state.irload1 && !cop_state.irload2 && !cop_state.start;`, sampled before the promotion and so first true on the third odd cycle after the first word ([`:9090-9103`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9090-L9103): `if (cop_state.inst == COP_INS_SKIP && idle)`, and for a `WAIT` `else if (!comp)`), and the restart fetches one odd cycle later ([`:9137-9142`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9137-L9142): `if (cop_state.start == 2)`): first words 8 colour clocks apart. The machine has no chipset branch; `COPPER_CYCLE_POLARITY` is a constant (`:69`) |
| **vAmiga** | **4 / 8 / 8** for `MOVE` / `SKIP` / satisfied `WAIT` — NanoMig's numbers. Each Copper state is scheduled two DMA cycles after the one before ([`Core/Components/Agnus/Copper/Copper.h:331`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.h#L331): `void schedule(EventID next, int delay = 2);`); a `MOVE` passes `COP_FETCH` and `COP_MOVE`, a `SKIP` `COP_FETCH`, `COP_WAIT_OR_SKIP`, `COP_SKIP1` and `COP_SKIP2`, and a satisfied `WAIT` `COP_FETCH`, `COP_WAIT_OR_SKIP`, `COP_WAIT1` and `COP_WAIT2` before the next fetch (`CopperEvents.cpp`) |
| **Denise** | **4 / 8 / 8** — NanoMig's numbers. The Copper is stepped only on even colour clocks ([`emulation/libami/agnus/agnus.cpp:656-659`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L656-L659): `if ((hPos & 1) == 0)` then `copper.process();`), so each state of its machine is two memory cycles. `Read1` fetches the first word and `Read2` the second; a `MOVE` writes there and goes straight back, `agnus.writeCustom(reg, ir2, Agnus::Trigger_Copper);` then `state = Read1;`, two states and 4. A `WAIT` or `SKIP` leaves `Read2` by `state = (ir2 & 1) ? Skip1 : Wait1;` ([`emulation/libami/agnus/copper.cpp:176-231`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L176-L231)). A `SKIP` then passes `Skip1` and `Skip2` into `Read1FromSkip`, which fetches ([`:232-241`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L232-L241)): four states, 8. A satisfied `WAIT` passes `Wait1`, where `if (compare())` sends it to `Wait3`, and `Wait3` returns to `Read1` on a free cycle ([`:242-276`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L242-L276): `// needs a free cycle after wait (not allocated)`): four states, 8. No chipset setting enters these states |
| **Copperline** | **4 / 8 / 8** — NanoMig's numbers. The Copper is stepped on every colour clock and fetches only on even ones ([`src/chipset/copper.rs:614-616`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L614-L616): `hpos & 1 == 0`), so a `MOVE`, two fetches, is 4. A `WAIT` or `SKIP` spends two bus-free cycles after its second-word fetch, and the comments say where the next fetch falls, as the states bear out: `so an immediately-true WAIT's next fetch lands at fetch2+6` ([`:517-531`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L517-L531)) and `so the post-SKIP fetch lands at fetch2+6 -- the same footprint as a WAIT` ([`:571-583`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L571-L583)). The second word is fetched at +2, so first word to first word is 8. Its unit tests walk the same count on the bus: a `WAIT` started at `bus.agnus.hpos = 0x20;` `wakes on the 0x26 access cycle, and the MOVE write lands at hpos 0x2A` — the `MOVE`'s second word at $2A, its first at $28, 8 after the `WAIT`'s ([`src/bus/tests.rs:3706-3725`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L3706-L3725)), and a `SKIP` from $20 reaches the following `WAIT` `(next fetch at fetch2+6)` ([`:3793-3819`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L3793-L3819)). Two caveats. The doc comment on `step_eligible_slot` still reads `MOVE/SKIP spans 4 color clocks and a WAIT 6` ([`src/chipset/copper.rs:690-694`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L690-L694)), which the states it documents do not do. And the comments name Minimig's `WAITSKIP1`/`WAITSKIP2` and vAmiga's `COP_WAIT1`/`COP_WAIT2` as the model for these cycles ([`:256-264`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L256-L264), [`:517-525`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/copper.rs#L517-L525)), so this row is not independent of NanoMig's lineage or of vAmiga. No chipset setting enters it |
| **MiSTer** | descendant — `rtl/agnus_copper.v:473-536` — identical to NanoMig's `WAITSKIP1`/`WAITSKIP2` states and their cycle counts |
| **TC64** | descendant — `rtl/minimig/agnus_copper.v:473-536` — the same states, byte for byte. The only difference either line has from NanoMig in this file (blocking where NanoMig assigns non-blocking, lines 234-274) does not touch the WAIT/SKIP timing; the three numbers this finding measures come from code both lines share unchanged with NanoMig |

**All four emulators count what NanoMig measures, and none counts the manual's
`SKIP` 4 or `WAIT` 6.** That is the single most useful fact on this page, and it
points away from the RTL. Every row counts colour clocks on a free bus — WinUAE's `agnus_hpos`,
Denise's `hPos`, Copperline's `hpos`, vAmiga's DMA cycles — and a colour clock is
one memory cycle of 280 ns, so the numbers stand beside the manual's without
conversion. Every row is read from the emulator's Copper state machine, not run;
Copperline's own unit tests assert its `WAIT` and `SKIP` counts. The four are
fewer than four independent witnesses: Copperline's comments name Minimig,
NanoMig's own ancestor, and vAmiga as the model for these cycles.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 18 — agrees with the manual:

  > "only the odd memory cycles are requested by the Copper 47, so four memory cycles are required for execution of MOVE and SKIP instructions and six memory cycles are re quired for the WAIT instruction."
  <!-- corpus: patent-us5103499-copper/p018.txt -->

  SKIP 4, WAIT 6, and so WAIT minus MOVE 2 — all three of the manual's figures, in the manual's unit (memory cycles, the Copper using every other one), against NanoMig's 8 / 8 / 4. The sentence just before gives the underlying 2 and 3 cycles. It closely paraphrases the manual's own Copper-timing text, so it is not an independent measurement.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 35 — agrees with the manual:

  > "NOTE: Copper Data Move instructions require four (4) slots. Copper Wait instructions require six (6) slots."
  <!-- corpus: agnus-8371-spec/p035.txt -->

  MOVE takes 4 slots and WAIT 6, so WAIT minus MOVE is 2. p027 gives SKIP the non-WAIT figure: 'All instructions (except for WAIT) require 2' bus cycles, so '4 memory cycle times are required per instruction. (memory cycles are 280 ns)'. Those are the manual's 4/6/2 in memory cycles, against NanoMig's 8/8/4.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Suggested change

**None, and a patch would be the wrong instrument.** All four emulators produce
NanoMig's numbers, so either the manual's cycle figures are wrong — which this
project has documented elsewhere — or every implementation shares a misreading.
Copperline's agreement weighs least: by its own comments it follows Minimig and
vAmiga here.

**This is decidable on real hardware and by no other means available here.** A
small 68k program timing a loop of `WAIT`s against a CIA timer, run on a real
machine, settles it outright instead of mediating between a 1991 manual and five
implementations, NanoMig and the four emulators. Both Copperline's `timing-test/` and vAmigaTS arrive
independently at the same technique — probe programs booted from an ADF, results
read from CRT photographs — and Copperline states the principle: *where real
hardware and FS-UAE disagree, real hardware wins.*

Until someone runs that, the honest status is "measured, matched by all four
emulators, unresolved".

---

## What holds the finding up

`AC6` is **expository, not independent** — given `AC4` and `AC5` it cannot fail
on its own, and it is counted as one family rather than as a separate
corroboration. Saying so matters: three red assertions that are really one
measurement would otherwise read as three.
