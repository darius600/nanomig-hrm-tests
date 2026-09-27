# NM-AA-005 — the long-line toggle runs, but no line is a colour clock longer [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. The AA specification's HTOTAL entry says the long-line/short-line toggle makes every other line one colour clock longer; this core toggles the bit that reports which line it is on and ends every line at the same count. Whether NanoMig wants the half-clock NTSC line at all is a project question, not one this repository can settle. |
| **Module** | `src/minimig-aga/agnus_beamcounter.v:105`, `:250` and `:293-298` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L105) at the pinned revision |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** if the lines are upstream's; the module's own comment says the feature is unimplemented rather than broken |
| **Test** | [`aa/aa-programmable-sync-windows`](../../aa/aa-programmable-sync-windows) · ten graded assertions, nine green and this one red. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 24, the HTOTAL entry |
| **Assertions** | 10 graded; `make mutate` turns the nine green ones red and this one green, which is the established shape for a red baseline |
| **Verdict of the four** | **two do it, one cannot, one refuses — and none of them does what this RTL does.** WinUAE and Denise lengthen the alternate line under a programmed HTOTAL; vAmiga lengthens it but does not implement HTOTAL; Copperline stops the toggle when HTOTAL is programmed. A toggle that runs and reports while every line stays the same length is produced by none of the four. See [What the others say](#what-the-others-say) |
| **See also** | the same package's nine green assertions, which establish that this module does read HTOTAL, HSSTRT, HSSTOP, HBSTRT and HBSTOP where the document puts them |

## What the AA specification says

Below its bit table, page 24's HTOTAL entry says in two sentences what a line
lasts, and then closes with `Active if VARBEAMEN=1 or DUAL=1.`:

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

The condition is implemented as printed whenever VARBEAMEN is set, which is
when the entry says HTOTAL is active; without VARBEAMEN the RTL ignores LOLDIS,
which is [NM-HRM-033](NM-HRM-033.md):

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

### Measured

[`aa/aa-programmable-sync-windows`](../../aa/aa-programmable-sync-windows)
measures two consecutive lines at 81 and 81 colour clocks with the toggle
running, where the entry's HTOTAL+2 would be 82 —
`LONG-LINES-ARE-ONE-CLOCK-LONGER`, red, one of the package's `9 passed, 1
failed`. See *The measurement* below for the assertion in full.

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
wraps where HTOTAL says, it lasts HTOTAL+1 clocks, HTOTAL decides it only
while VARBEAMEN is set, the eight bits above the value are ignored exactly as
the table's `x` cells mark them, sync starts and stops where HSSTRT and HSSTOP
say and only once VARHSYEN is set as well, blanking starts and stops where
HBSTRT and HBSTOP say, and the interlace enable arrives from BPLCON0 bit 2 and
powers up clear.

## What the others say

The question put to each: with VARBEAMEN set and HTOTAL written, does the
alternate line last one colour clock longer?

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:1899`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1899), [`:2330`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2330), [`:10969-10975`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10969-L10975) | **both halves.** With VARBEAMEN, `maxhpos = htotal + 1`; the line length in use is then `maxhpos = maxhpos_short + lol`, so every other line is one colour clock longer than the programmed value. The condition is the document's sentence: `if (!(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS)) { lol = lol ? false : true; linetoggle = true; } else { lol = false; linetoggle = false; }` |
| **vAmiga** | [`Beam.h:80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L80), [`AgnusRegs.cpp:567`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L567), [`:570`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L570) | **lengthens the line, and cannot answer for a programmed HTOTAL.** `hCnt() const { return lol ? 228 : 227; }` is the whole model of the long line, and the toggle is keyed on NTSC and LOLDIS. HTOTAL is stored under a comment saying these registers *"are recognized and stored, but have no effect yet"* |
| **Denise** | [`events.cpp:479-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L479-L480), [`register.cpp:565`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L565) | **both halves.** The next line is scheduled as `updateEvent<EVENT_HTOTAL>((beamCon & VARBEAMEN) ? (hTotal + lol) : (0xe2 + lol))` — the programmed total plus the toggle — and the condition is `lolToggle = !(value & LOLDIS) && !(value & BEAM_PAL)`, the document's two terms |
| **Copperline** | [`agnus.rs:855-858`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L855-L858), [`:899-904`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L899-L904) | **deliberately neither.** `long_line_toggles()` requires `programmable_line_cck().is_none()`, under a comment that says *"only when the beam is not programmable (VARBEAMEN fixes the line length)"*. With HTOTAL programmed nothing toggles, nothing reports, and every line is HTOTAL+1 |
| **MiSTer** | [`rtl/agnus_beamcounter.v:256`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L256), [`:282-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L282-L285) | descendant — the identical toggle and the identical `hpos[8:0]=={htotal[8:1],1'b0}` comparison with no `long_line` term |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:318`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L318), [`:343-346`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L343-L346) | descendant — the same two places, the same absence |

**What the four agree on is that the two halves travel together.** Where the
toggle runs the line grows (WinUAE, Denise, and vAmiga on its fixed NTSC line);
where the line cannot grow the toggle is stopped (Copperline, explicitly, for
exactly this mode). No implementation reports an alternating long-line bit over
lines that are all the same length, which is what this core does and what the
red assertion measures.

**That makes Copperline the interesting one for a maintainer**, because it is
the cheap half of the fix: suppressing the toggle under VARBEAMEN is a condition,
while lengthening the line touches the comparison every other timing in the
module hangs off. It is also the half the document does *not* license — the
entry makes the toggle conditional on the PAL bit and LOLDIS, not on VARBEAMEN.
Copperline is answering a question about real silicon that this document does
not ask.

## Suggested change

None is proposed. Lengthening the alternate line touches the one comparison
every other timing in the module hangs off, and the feature exists to model an
NTSC line of 227.5 colour clocks on average — 228 and 227 alternately — which
this core does not claim to support anywhere. The honest report is that the
document describes a behaviour, the bit that selects it is implemented and
visible to software, and the behaviour itself is not.

## What holds the finding up

The document's sentence is one line and the reading of it is not in doubt: the
same quotation supports both halves, and the half about the condition is green
against this RTL for its PAL-bit term (the package never sets LOLDIS), so the
sentence is being read correctly. The RTL's own
comments name the gap twice.

## For the specialists

1. **Does any software notice?** A program that reads VPOSR bit 7 to decide
   where to put a sprite on an alternate line will be told the line is long
   and find it is not.
2. **Is the long line wanted here at all?** The document says HTOTAL+2 of its
   280 ns increments, one colour clock more than the short line, and its
   STRLONG entry on p. 29 says the same for NTSC and for a programmed line:
   *"lines with long counts(228- NTSC, HTOTAL+2- VARBEAMEN=1"*. No line lasts half a colour
   clock; the NTSC 227.5 is the average of 228 and 227. Whether NanoMig
   wants the alternation at all is the question before any edit.
