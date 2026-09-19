# NM-HRM-005 — WAIT and SKIP take longer than the manual states

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_copper.v` **lines 384-548**, the `copper_state` machine that spends the cycles — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L384-L548) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v) |
| **Report to** | **nowhere yet** — this one needs hardware, not a patch. See *Suggested change* |
| **Test** | [`hrm/ch2-copper-cycle-times`](../../hrm/ch2-copper-cycle-times), [`hrm/ch2-copper-cycles`](../../hrm/ch2-copper-cycles) |
| **Chapter** | 2 — Coprocessor · *Amiga Hardware Reference Manual*, 3rd ed. |
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

Every figure is exactly double, which was the first thing to rule out.

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
different units. That check mattered: a suite that reports everything at double
the printed value usually has a units error, and this one does not.

---

## What the others say

| | |
|---|---|
| **WinUAE** | not compared |
| **vAmiga** | **4 / 8 / 8** — NanoMig's three numbers |
| **Denise** | — |
| **Copperline** | — |
| **MiSTer** | descendant — `rtl/agnus_copper.v:473-522` — identical to NanoMig's `WAITSKIP1`/`WAITSKIP2` states and their cycle counts |
| **TC64** | descendant — `rtl/minimig/agnus_copper.v:473-522` — the same states, byte for byte. Neither fork's own edit to this file (the blocking-assignment change at lines 234-274) touches the WAIT/SKIP timing; the three numbers this finding measures come from code both lines share unchanged with NanoMig |

**vAmiga measures what NanoMig measures.** That is the single most useful fact on
this page, and it points away from the RTL.

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
