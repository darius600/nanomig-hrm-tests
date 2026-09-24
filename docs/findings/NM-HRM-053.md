# NM-HRM-053 — Appendix B's `*` mark has no ECS qualifier, and nothing honours it under ECS

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a graded red |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. The red is against p.289, not against the RTL. |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L225) |
| **Report to** | **the manual, not the implementation.** All four reference implementations do exactly what NanoMig does |
| **Test** | [`hrm/ch8-appb-copper-access-marks`](../../hrm/ch8-appb-copper-access-marks) — `SERDAT-CDANG1`, `SERPER-CDANG1`, `SERDATR-CDANG1` red; their `-CDANG0` twins and `CTRL-BLTCON0` green |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 3 red of 8, with a control that separates the two printed marks |
| **Verdict of the four** | **unanimous with NanoMig against the page.** All four gate on the same formula, and one of them ships a test table asserting it |

---

## What the HRM says

**`appB3/p287.txt:22`**, the legend for Appendix B's register summary:

       *    Address not writable by the Copper.

and immediately below it, at `:24`, the conditional mark:

       ~     Address not writable by the Copper unless the "copper danger bit",
             COPCON is set true.

**The `*` carries no condition and no chipset.** The `~` carries both. The two
marks are printed two lines apart for exactly the purpose of distinguishing an
absolute prohibition from a conditional one, and p.289 puts `*` on SERDATR
(`*018`), SERDAT (`*030`) and SERPER (`*032`).

## What NanoMig does

`agnus_copper.v:225` blocks the low register range on

    !ecs || !cdang

so the block is lifted when **both** ECS and CDANG are set, and a Copper MOVE
reaches all three registers. Measured at the fixture: blocked at `cdang=0`,
admitted at `cdang=1`, with ECS held high in both cases.

### Measured

[`hrm/ch8-appb-copper-access-marks`](../../hrm/ch8-appb-copper-access-marks)
drives a Copper MOVE at `ecs=1` against the three `*`-marked registers:
blocked at `cdang=0`, admitted at `cdang=1` — `SERDAT-CDANG1`,
`SERPER-CDANG1` and `SERDATR-CDANG1` are all red, three of the package's `5
passed, 3 failed`; their `-CDANG0` twins stay green. See *Why the reds are
not a broken fixture* below for the `~`-marked control on the same rig.

## Why the reds are not a broken fixture

The package grades a **`~`-marked register on the same rig**: BLTCON0 (`$040`)
is blocked at `cdang=0` and admitted at `cdang=1`, which is precisely what its
own conditional legend demands. So the fixture can tell the two marks apart, and
the three reds are not the rig saying "admitted" whenever CDANG is set.

## Why this is filed against the page

All four implementations gate on the same expression, and two of them write it
in almost the same characters:

| | |
|---|---|
| **WinUAE** | `if (addr < ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80))` — `custom.cpp:2809`. With CDANG set on ECS the threshold is **0**: nothing is blocked |
| **vAmiga** | `if (cdang) { return agnus.isOCS() ? addr < 0x40 : false; } else { return addr < 0x80; }` — `Copper.cpp:396-402`. With CDANG set and not OCS, no address is illegal |
| **Denise** | `cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;` — `copper.cpp:342`, tested at `:203` as `if (reg < cdang)`. ECS with CDANG gives a threshold of 0 |
| **Copperline** | ships a test table for it. `copper_forbidden_move_ranges_match_copcon_cdang_on_ocs_and_ecs` (`src/bus/tests.rs:4165`) asserts `$000` and `$03E` **blocked** with CDANG on OCS and **allowed** with CDANG on ECS |
| **MiSTer** | `rtl/agnus_copper.v:225` — descendant, byte-identical: `if (ir2[8:7]==2'b00 && !cdang \|\| ir2[8:6]==3'b000 && !ecs) // illegal access`, the same decode that lifts the block at `ecs & cdang` |
| **TC64** | `rtl/minimig/agnus_copper.v:225` — descendant, the identical line. The file differs from NanoMig by 18 lines on both Minimig lines, but they are the same 18: nine `<=` assignments converted to `=` (`copjmp1`, `copjmp2`, `strobe1`, `strobe2`, `strobe`, at `:237-271`), a blocking-vs-non-blocking edit unrelated to this decode and made identically by both. A fix here diverges from both lines equally; the manual is what disagrees, not either ancestor |

Four independent implementations, one formula, and one of them treats it as a
property worth a regression test. **The page's `*` describes OCS.** Appendix B
prints one mark for a rule that has two chipset cases, and the legend that would
have said so is the `~` two lines below it.

## What this is not

**Not [NM-HRM-044](NM-HRM-044.md).** That finding rests on two printed passages
in genuine conflict — p.279 permits Copper initialisation of DSKPTH and p.289
forbids it. Here there is no second passage: p.265 and p.283, this slice's own
Appendix A pages for these four registers, **do not mention the Copper at
all** — measured, `grep -in copper` over both returns nothing. So this is the
plainer case, a single unconditional statement that no implementation honours,
and it is graded rather than declined as a source conflict.

**Not [NM-HRM-004](NM-HRM-004.md)**, which is a different corner of the same
decode — the `~` range at ECS with CDANG clear — and is untouched by these
assertions.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS); US patent 4,829,473, Commodore-Amiga; US patent 4,874,164, Commodore-Amiga; 6502.org dissection of Paula 8364R7.

