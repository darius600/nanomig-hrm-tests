# NM-HRM-063 — FLAG is negative edge-sensitive; the RTL ORs a level [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `src/minimig-aga/cia_int.v:57` |
| **Report to** | Minimig/MiSTer maintainers, once Amiga specialists confirm it matters in practice |
| **Test** | `hrm/appF-ciab-port-assignments` · `PROP-FLAG-EDGE-POLARITY` — **red** |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters · *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p.341 |
| **Assertions** | 1 red, with a paired control that passes on the same measurement channel |
| **Verdict of the four** | **unanimous, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all set the bit on a transition and none of them from a held level |

---

## What the HRM says

Printed page 341, on the 8520's handshaking and interrupt inputs:

> FLAG is a negative edge-sensitive input that can be used for receiving the PC
> output from another 8520 or as a general-purpose interrupt input. **Any
> negative transition on FLAG will set the FLAG interrupt bit.**

The requirement is a *transition*, and specifically a negative one. p.341 is the
only page in the whole corpus that states the polarity at all.

## What the RTL does

`src/minimig-aga/cia_int.v`, the latching branch:

```verilog
icr[4] <= icr[4] | flag;   // external flag input
```

A level-sensitive OR on an active-high line. **There is no edge detector
anywhere in the module and no previous-state register for `flag`**, so no edge
can be derived: the module cannot distinguish a transition from a level.

The clearing branch, eight lines up, is `icr[4] <= flag;` — so reading ICR while
`flag` is still high re-sets the bit in the same cycle. A held level is not only
sufficient to set the bit, it also survives the read that is supposed to clear
it.

### Measured

[`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments)
holds `flag` high from a single rising edge with zero negative transitions
anywhere in the stimulus: ICR bit 4 sets anyway —
`PROP-FLAG-EDGE-POLARITY`, red, one of the package's `7 passed, 3 failed`,
paired with a control that drives a real `flag` 0→1→0 transition and passes
on the same ICR arm/read channel. See *What the test measures* below for the
assertion in full.

## What the test measures

`PROP-FLAG-EDGE-POLARITY` drives `flag` through a single rising edge, holds it
high for five ticks, and issues **zero negative transitions anywhere in the
stimulus**. Under p.341 the FLAG interrupt bit must stay clear. It does not:
ICR reads `0x10`.

The package pairs it with a control on the same ICR arm-and-read channel — a
real negative transition, `flag` 0→1→0 — which sets the bit and passes, so the
red is not a dead measurement path. Its mutation inverts the oracle and turns
the assertion green, so the comparator is not tautologically false.

**Measured independently at a second fixture.** `hrm/appF-cia-interrupt-control`
reached the same result at the `cia_int` boundary while building a different
package, without having seen this one's number.

## What the others say

| | | |
|---|---|---|
| **WinUAE** | an event; no level input exists | `cia.cpp:823-825`: `cia_diskindex()` calls `CIA_sync_interrupt(1, ICR_FLAG)`, and `disk.cpp:3973` calls it once per index pulse. Nothing holds a FLAG level that could be re-sampled |
| **vAmiga** | explicit, in two separate methods | `CIA.cpp:250-266`: `emulateRisingEdgeOnFlagPin()` only calls `wakeUp()`; `emulateFallingEdgeOnFlagPin()` calls `wakeUp()` and then `icr \|= 0x10` |
| **Denise** | an event; no level input exists | `emulation/cia/new/cia.cpp:302-304`'s `setFlag()` sets `intIncomming \|= 0x10`, called from `libami/drive/diskDrive.cpp:79` exactly when the head wraps past the end of a track — once per revolution |
| **Copperline** | an explicit edge detector over retained pin state | `src/chipset/cia.rs:373-380`: `set_flag_pin` computes `let falling_edge = self.flag_pin_high && !high;` and latches `ICR_FLG` only when that is true |
| **MiSTer** | descendant — same statement, in an otherwise-rewritten file | descendant — `rtl/cia_int.v:81` — the same statement, `icr[4] <= icr[4] \| flag;`; 113 of the file's lines differ from NanoMig, nearly all of them added commentary, and this one is unchanged by it |
| **TC64** | descendant — byte-identical to NanoMig | descendant — `rtl/minimig/cia_int.v:57` — byte-identical to NanoMig line for line, down to the "intterupts" typo in the comment on the ICR-clear branch. TC64 is the closer sibling: its whole file is untouched from NanoMig's, so a fix here is one clean edit against an otherwise-identical file. MiSTer's copy has been rewritten around the same statement — the fix lands in a file that has drifted everywhere else. Either way the statement is identical in both, and a fix belongs to both lines equally: `icr[4] <= icr[4] \| flag; |

**Not one of the four independent implementations sets the bit from a level.**
Two of them cannot even represent a held level; the one that does model the pin
as a level detects the edge explicitly. Copperline is the closest structural
comparison to the RTL, and it is the one that keeps the previous-state bit
`cia_int.v` does not have.

## Why this was not found until appendix F was read

`hrm/ch8-disk-index-interrupt-path`'s `PROP-FLAG-PIN` grades *"a pulse on `flag`
sets ICR bit 4 and raises irq"*, and it passes. A pulse satisfies both the level
reading and the edge reading, so that assertion cannot separate them — and
chapter 8 was built without the appendix that specifies the part. p.341 is
narrower than anything chapter 8 had to work from.

This package first **declined** the claim against `PROP-FLAG-PIN`, following a
build-brief instruction to decline rather than re-grade an adjacent proposition
at the same register bit. The builder recorded in the same breath that
`PROP-FLAG-PIN` grades a pulse and not this page's polarity. The instruction was
wrong for a claim that is strictly narrower than the assertion it is measured
against; see `OPEN-ISSUES.md` O78.

## What is left for specialists

Whether a real Amiga ever holds FLAG at a level long enough for this to matter.
On CIA-B the pin is the floppy index pulse, which is a pulse by construction, so
the divergence may never be observable in practice — and the page's own sentence
offers the other use, *"receiving the PC output from another 8520 or as a
general-purpose interrupt input"*, where a held level is entirely plausible.

That judgement, and whether the Minimig line should carry an edge detector,
belongs to Amiga and RTL specialists. **This suite's part is the measurement:
the page requires a transition, the RTL responds to a level, and all four
independent implementations follow the page.**

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, 6526 CIA specification, 1981**, PDF page 6 — agrees with the manual:

  > "Any negative transition on FLAG will set the FLAG interrupt bit."
  <!-- corpus: cia-6526-spec/p006.txt -->

  The datasheet states the manual's p.341 rule almost word for word, 6526 where the manual has 8520: FLAG is 'a negative edge sensitive' input and a transition, not a level, sets the FLAG bit. It also has a read of the ICR data register clear it (p007), and on its edge rule a held level would not set the bit again; it is a 6526 document and does not itself speak for the 8520.

Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

