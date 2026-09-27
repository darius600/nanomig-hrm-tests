# ch7-diwstrt-extends-vertical-blanking

**Test class:** HRM_CONFORMANCE
**Oracle:** Amiga Hardware Reference Manual, 3rd edition, p.219
**Fixture:** `agnus.v` (Agnus top level, with its submodules)

This specification is written from the quoted sentence alone. The one RTL fact
it rests on is `agnus.v`'s **port list**, which answers the boundary question --
*does any module see both DIWSTRT and vertical blanking?* -- that this package
exists for. No behaviour enters the specification.

## Why this package exists

Two chapter-7 claims about p.219's sentence cannot be settled at any other
fixture in the suite:

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

**P3 -- WITHDRAWN. Measured and reported, not graded.**

As a floor it would read: *a VSTART set inside the minimum blanking range does
not make the display start before hardware blanking has ended, because the page
grants DIWSTRT control only after the minimum range.* It is not graded, for two
reasons.

**The page does not require it.** In context, p.219's passage is advice on how a
programmer gains more housekeeping time during vertical blanking. The sentence
before it gives the minimum as 20 lines NTSC / 25 PAL; the sentence after reads
*"If you find that you still require additional time during vertical blanking,
you can use the Copper to create a level 3 interrupt."* The DIWSTRT sentence
offers another way to get that time. It says where the useful control is. It
says nothing about what the hardware does with a VSTART inside the blanked
region -- and silence is not a requirement.

**And the observable cannot carry it.** This fixture's marker for the
display start is a bitplane DMA bus request. P1 and P2 need only a marker that
*moves*, and it is sound for that. A floor needs the absolute claim *the display
started*, and a fetch inside the blanked region is not a display: the data is
fetched and the video output is blanked regardless.

The measurement is reported in full on a `record()` line, with its numbers. Settling
the printed sentence's floor, if anyone wants it settled, needs a boundary that
sees Denise's video output rather than Agnus's bus requests.

A control assertion stays green beside it, showing the same measurement
mechanism finds no premature activity when VSTART is set outside the blanking
range -- so the recorded number is a real measurement and not an artefact.

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

## Observable

Required: a per-line indication of vertical blanking, and a per-line indication
that the display has actually started. `agnus.v`'s port list carries `vblank`.
It has no port for bitplane DMA activity -- `agnus_bitplanedma`'s own `dma`
output reaches only an internal wire, and `hde` is purely horizontal. The
marker used is `dbr && !refresh`
with every DMA channel but bitplane disabled in DMACON and the disk and audio
request pins held idle, which is the isolation `hrm/ch7-dmacon-register-bits`'s own
committed BPLEN-GATES-BITPLANE-DMA assertion already established. Ports and
ordinary stimulus only; no internal signal is read through `rootp`.

That marker is sound for a claim about movement, which is what P1 and P2 make.
It is not sound for a claim about the display having started, which is one of
the two reasons P3 is withdrawn above.

## Interlace doubles the line count (p.49-50)

Written from `ch7-diwstrt-extends-vertical-blanking-spec-stage.jsonl` alone, as
P1-P3 above were.

### The oracle

> Interlaced mode allows twice as much data to be displayed in the same
> vertical area as in non-interlaced mode. This is accomplished by doubling
> the number of lines appearing on the video screen.
>
> -- p.49

> Figure 3-5 (printed p.50): under the combined frame the caption reads
> "(Same physical space as used by a 200-line, noninterlaced display.)" --
> the 400 interlaced lines occupy the same physical height as 200
> non-interlaced lines.
>
> -- p.50, Figure 3-5

Claims: `opus:3-res.c16` (the sentence above), `opus:3-lace.f03` (the figure
caption above).

`3-lace.f03`'s own extract carries the flag `corpus-print-disagreement`: the
machine-readable text pulled from this figure reads "by a 200-line (256)
PAL)," which is garbled. P5 below rests on the figure image as read and
transcribed by the reader, not on that corpus text.

Two things are asserted, and they are read together rather than singly:

1. an interlaced frame carries twice as many lines as a non-interlaced one;
2. that doubled line count occupies the *same* vertical span the
   non-interlaced count occupied -- it is not simply twice as tall.

### Propositions

**P4 -- INTERLACE DOUBLES THE LINE COUNT PER DISPLAYED FRAME.**
With interlace mode enabled versus disabled, and everything else held
constant, the number of lines belonging to one displayed frame is twice the
number belonging to one non-interlaced frame (p.49: "twice as much data ...
doubling the number of lines appearing on the video screen").

**P5 -- THE DOUBLED LINES OCCUPY THE SAME SPAN, NOT A TALLER ONE.**
The doubled line count of P4 spans the same overall vertical extent as the
non-interlaced frame it is compared against (p.50, Figure 3-5: the combined
interlaced frame is captioned "Same physical space as used by a 200-line,
noninterlaced display").

**CONTROL (interlace) -- the fixture can toggle interlace and observe a
change on the same measurement basis.**
Before P4/P5 are trusted: with interlace mode enabled, whatever signal P4/P5
measure is observed to differ from interlace mode disabled, on the same run
and the same clock -- confirming the toggle reaches the module and the
measurement is not vacuous.

### What P4/P5 do NOT claim

- **Nothing about which specific line totals are correct.** Whether a
  non-interlaced frame is 200, 312 or 262 lines is not this package's
  subject; P4 asserts only that the interlaced count is double whatever the
  non-interlaced count on this fixture measures.
- **Nothing about the half-line vertical timing mechanism** by which real
  interlaced video achieves the visual effect described. Only the line-count
  and physical-space claims in the two quotations above.
- **Nothing about colour, bitplane content, or what is displayed.**

### Expected values

As above: every expected value comes from the page or from arithmetic on
this test's own stimulus -- "the interlaced count is double the
non-interlaced count measured on this same fixture" -- never from what the
RTL produces.

### Observable

Required: a way to enable/disable interlace mode independent of DIWSTRT and
DMACON, and a per-field count of how many lines belong to a displayed frame,
measured on the same basis with the toggle in each state. Interlace is
BPLCON0's LACE bit, and `field1` shows the toggle reaching the module
(CONTROL). The per-field line count uses the same `dbr && !refresh` marker as
P1 and P2. `agnus.v` has no `hpos`/`vpos` port, so where a field's lines fall
is read from the tick offset of the `_vsync` port's falling edge, against the
line period measured on the same run.
