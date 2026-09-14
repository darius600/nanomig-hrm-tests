# ch7-diwstrt-extends-vertical-blanking

**Test class:** HRM_CONFORMANCE
**Oracle:** Amiga Hardware Reference Manual, 3rd edition, p.219
**Fixture:** `agnus.v` (Agnus top level, with its submodules)

This specification is written from the quoted sentence alone, before the RTL is
opened. What was looked at first, and is disclosed rather than hidden: `agnus.v`'s
**port list**, to establish that a fixture spanning both signals can exist at
all. No behaviour was read. The boundary question -- *does any module see both
DIWSTRT and vertical blanking?* -- is what created this package, so it had to be
answered before there was anything to specify.

## Why this package exists

Chapter 7 routed two claims about p.219's sentence and neither could be settled
anywhere in the suite:

| package | fixture | sees DIWSTRT | sees vertical blanking |
|---|---|---|---|
| `ch3-vertical-display-window` | `agnus_bitplanedma` | yes | **no** |
| `ch7-vertb-vertical-blank-timing` | `agnus_beamcounter` | **no** | yes |
| this package | `agnus` | yes | yes |

`ch3-vertical-display-window` grades the display-start half of the sentence and
`record()`s the blanking half as out of reach at its boundary, correctly. This
package is that declined half.

## The oracle

> After the minimum vertical blanking range, you can control where the display
> actually starts by using the DIWSTRT (display window start) register to
> extend the effective vertical blanking time.
>
> -- p.219

Claims: `sol:7-interrupts.c39`, `opus:7-int.c36`.

Two separate things are asserted in that one sentence, and the word doing the
work is **effective**:

1. there is a **minimum** vertical blanking range, and DIWSTRT operates *after*
   it -- so DIWSTRT does not shorten or lengthen the minimum itself;
2. raising DIWSTRT's VSTART pushes where the display **actually starts** later,
   and the interval before the display starts is what the page calls the
   *effective* blanking time.

Neither half is a claim about a colour, a pixel or a rendered line: both are
claims about *when*, in lines.

## Propositions

**P1 -- DIWSTRT MOVES THE DISPLAY START, LINE FOR LINE.**
With everything else held constant, raising DIWSTRT's VSTART by N lines moves
the first line on which the display actually starts later by exactly N lines.
Graded over at least three distinct VSTART values, so that "moves later" is
measured as a slope and not as one before/after pair.

**P2 -- THE MINIMUM BLANKING RANGE IS NOT WHAT MOVES.**
The number of lines for which hardware vertical blanking holds is unchanged
across those same VSTART values. This is what makes "effective" mean something:
the page says DIWSTRT extends the *effective* blanking time, and says DIWSTRT
acts *after* the minimum range, so the minimum must be indifferent to it.

**P3 -- "AFTER THE MINIMUM" IS A FLOOR.**
A VSTART set inside the minimum blanking range does not make the display start
before hardware blanking has ended. The page grants DIWSTRT control only
*after* the minimum range; if a VSTART below it pulled the display start into
the blanked region, the sentence would be false as printed.

**CONTROL -- the fixture is alive and the two signals are independent.**
Before any of the above is trusted: vertical blanking is observed to hold and
then release within a frame, and a DIWSTRT write is observed to change
something, on the same run and the same clock.

## What this package does NOT claim

- **Nothing about the length of the minimum range.** Whether it is 20 lines or
  21, 25 or 26, is `ch7-vertb-vertical-blank-timing`'s subject, and that package
  carries two reds and a recorded contradiction on exactly that point (p.219
  gives both a count and a range and they disagree by one). P2 here asserts only
  that the number, whatever it is, does not change when DIWSTRT does.
- **Nothing about horizontal blanking or DIWSTOP.** The sentence is about where
  the display starts.
- **Nothing about what is displayed.** No bitplane content, no colour.

## Expected values

Every expected value in this package comes from the page or from arithmetic on
the stimulus this test itself applies -- "raised VSTART by N, expect the start
to move by N". No expected value may be taken from what the RTL produces. If a
measurement disagrees with the page, the assertion is red and the finding is
reported; the expectation is not adjusted to match.

## Observable, to be fixed when the RTL is opened

Required: a per-line indication of vertical blanking, and a per-line indication
that the display has actually started. `agnus.v`'s port list carries `vblank`,
and the display start must be taken from whichever port marks bitplane DMA
activity for a line. The exact port is chosen when the RTL is opened, and the
build commit will say which and why.

If no port distinguishes bitplane DMA from the other DMA channels sharing the
bus, P1 and P3 cannot be graded here either, and this package will `record()`
that with the reason rather than grading a weaker proposition.
