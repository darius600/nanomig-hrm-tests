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
| **Verdict of the four** | vAmiga measures exactly NanoMig's three numbers |

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
| **WinUAE** | not read for this finding |
| **vAmiga** | **4 / 8 / 8** for `MOVE` / `SKIP` / satisfied `WAIT` — NanoMig's numbers. Each Copper state is scheduled two DMA cycles after the one before ([`Core/Components/Agnus/Copper/Copper.h:331`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.h#L331): `void schedule(EventID next, int delay = 2);`); a `MOVE` passes `COP_FETCH` and `COP_MOVE`, a `SKIP` `COP_FETCH`, `COP_WAIT_OR_SKIP`, `COP_SKIP1` and `COP_SKIP2`, and a satisfied `WAIT` `COP_FETCH`, `COP_WAIT_OR_SKIP`, `COP_WAIT1` and `COP_WAIT2` before the next fetch (`CopperEvents.cpp`) |
| **Denise** | not read for this finding |
| **Copperline** | not read for this finding |
| **MiSTer** | descendant — `rtl/agnus_copper.v:473-536` — identical to NanoMig's `WAITSKIP1`/`WAITSKIP2` states and their cycle counts |
| **TC64** | descendant — `rtl/minimig/agnus_copper.v:473-536` — the same states, byte for byte. The only difference either line has from NanoMig in this file (blocking where NanoMig assigns non-blocking, lines 234-274) does not touch the WAIT/SKIP timing; the three numbers this finding measures come from code both lines share unchanged with NanoMig |

**vAmiga measures what NanoMig measures.** That is the single most useful fact on
this page, and it points away from the RTL.


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

**None, and a patch would be the wrong instrument.** One independent
implementation already produces NanoMig's numbers, so either the manual's cycle
figures are wrong — which this project has documented elsewhere — or both
implementations share a misreading.

**This is decidable on real hardware and by no other means available here.** A
small 68k program timing a loop of `WAIT`s against a CIA timer, run on a real
machine, settles it outright instead of mediating between a 1991 manual and two
implementations. Both Copperline's `timing-test/` and vAmigaTS arrive
independently at the same technique — probe programs booted from an ADF, results
read from CRT photographs — and Copperline states the principle: *where real
hardware and FS-UAE disagree, real hardware wins.*

Until someone runs that, the honest status is "measured, corroborated once,
unresolved".

---

## What holds the finding up

`AC6` is **expository, not independent** — given `AC4` and `AC5` it cannot fail
on its own, and it is counted as one family rather than as a separate
corroboration. Saying so matters: three red assertions that are really one
measurement would otherwise read as three.
