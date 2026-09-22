# NM-AA-005 — the long-line toggle runs, but no line is a colour clock longer

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. The AA specification's HTOTAL entry says the long-line/short-line toggle makes every other line one colour clock longer; this core toggles the bit that reports which line it is on and ends every line at the same count. Whether NanoMig wants the half-clock NTSC line at all is a project question, not one this repository can settle. |
| **Module** | `src/minimig-aga/agnus_beamcounter.v:105`, `:250` and `:293-298` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L105) at the pinned revision |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** if the lines are upstream's; the module's own comment says the feature is unimplemented rather than broken |
| **Test** | [`aa/aa-programmable-sync-windows`](../../aa/aa-programmable-sync-windows) · ten graded assertions, nine green and this one red. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 24, the HTOTAL entry |
| **Assertions** | 10 graded; `make mutate` turns the nine green ones red and this one green, which is the established shape for a red baseline |
| **Verdict of the four** | **not consulted.** WinUAE, vAmiga, Denise and Copperline were not read for this claim. What each does with a programmed HTOTAL while the long-line toggle runs — whether any of them lengthens the alternate line — is exactly the evidence that would settle it |
| **See also** | the same package's nine green assertions, which establish that this module does read HTOTAL, HSSTRT, HSSTOP, HBSTRT and HBSTOP where the document puts them |

## What the AA specification says

Page 24's HTOTAL entry is two sentences:

```
Horiz line has this many + 1 280ns increments. If the PAL bit & LOLDIS are
not high, long line/short line toggle will occur, and there will be this
many + 2 every other line.
```

Two claims: a **condition** — the toggle runs unless the PAL bit or LOLDIS is
high — and an **effect** — every other line lasts HTOTAL+2 rather than
HTOTAL+1. The package grades them separately, because this module answers
them differently.

## What NanoMig does

The condition is implemented exactly as printed:

```verilog
if (end_of_line)
    if (pal || (loldis && varbeamen))
        long_line <= 1'b0;
    else if (!(loldis && varbeamen))
        long_line <= ~long_line;
```

With the PAL bit low and LOLDIS low the bit alternates line by line, and it
reaches software: VPOSR bit 7 is `long_line` (`:110`). That assertion is
green.

The effect is not implemented. The line ends where `end_of_line` fires, and
that comparison never consults `long_line`:

```verilog
if (hpos[8:0]=={htotal[8:1],1'b0})
    end_of_line <= 1'b1;
```

The module says so itself, at the declaration:

```verilog
reg long_line;  // long line signal for NTSC compatibility (actually long lines are not supported yet)
```

and again at `:86`: `line length of 227 CCKs in PAL mode (NTSC line length of
227.5 CCKs is not supported)`.

## The measurement

With the PAL bit low, LOLDIS low, VARBEAMEN set and HTOTAL written to 80:

```
MEASURED: over four lines the long-line bit changes 4 times with the PAL bit
          low and 0 times with it high
PASS: LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS
MEASURED: with the toggle running, two consecutive lines take 81 and 81
          colour clocks; the entry says every other line is HTOTAL+2, which
          is 82
FAIL: LONG-LINES-ARE-ONE-CLOCK-LONGER
```

The two lines are measured the same way, one after the other, by counting cck
ticks between wraps of the horizontal counter — not by reading a register that
might report an intention.

Eight further assertions in the same package are green, which is what makes
this a reading of the line length rather than of a broken fixture: the line
wraps where HTOTAL says, it lasts HTOTAL+1 clocks, the eight bits above the
value are ignored exactly as the table's `x` cells mark them, sync starts and
stops where HSSTRT and HSSTOP say, blanking starts and stops where HBSTRT and
HBSTOP say, and none of it happens until VARBEAMEN is set.

## Suggested change

None is proposed. Lengthening the alternate line touches the one comparison
every other timing in the module hangs off, and the feature exists to model an
NTSC line of 227.5 colour clocks — which is half a clock, not one, and which
this core does not claim to support anywhere. The honest report is that the
document describes a behaviour, the bit that selects it is implemented and
visible to software, and the behaviour itself is not.

## What holds the finding up

The document's sentence is one line and the reading of it is not in doubt: the
same quotation supports both halves, and the half about the condition is green
against this RTL, so the sentence is being read correctly. The RTL's own
comments name the gap twice.

## For the specialists

1. **Does any software notice?** A program that reads VPOSR bit 7 to decide
   where to put a sprite on an alternate line will be told the line is long
   and find it is not.
2. **Is half a colour clock representable here at all?** The document says
   HTOTAL+2 of its 280 ns increments; real NTSC silicon lengthens the line by
   half a colour clock, not a whole one. Which of the two NanoMig would want
   is the question before any edit.
