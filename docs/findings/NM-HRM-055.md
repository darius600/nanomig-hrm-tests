# NM-HRM-055 — SERDAT written with all zeros still transmits

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_uart.v#L77-L140) |
| **Report to** | **the Minimig line**, not NanoMig alone |
| **Test** | [`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) `ALLZERO-NO-TX`, and independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) `E6` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, in two packages built by different agents from different claim sets |
| **Verdict of the four** | **split, 2–1–1.** vAmiga and Denise implement the sentence in one token; Copperline writes an explicit branch to transmit anyway; WinUAE does not model the line at this level |

---

## What the HRM says

**p.259:26-27**, the whole of it:

> *"Writing this register starts the data transmission. If this register is
> written with all zeros, no data transmission is initiated."*

Two sentences, and the second is the exception to the first. It is not a
side-remark: an all-zero SERDAT is how software is told to *not* start a frame.

## What NanoMig does

`paula_uart.v` loads the transmit shift register on any write to $030. There is
no test of the written value anywhere in the transmit path. Measured at the
fixture: writing `0x0000` still drives one low cell on `txd` — a start bit.

The red was reproduced by a second package, `ch8-serial-interface`, built by a
different agent against different claim uids and its own fixture.

### Measured

[`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) writes SERDAT
`0x0000` and watches `txd`: transmission started — `ALLZERO-NO-TX`, red, one
of the package's `17 passed, 1 failed`.
[`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) reproduces it
independently: after `SERDAT=0x0000`, `txd` was observed to go low at least
once over 4 bit-cells' worth of `clk7_en` beats — `E6`, red, one of the
package's `48 passed, 3 failed`.

## The control that makes it a finding

The same fixture writing a non-zero word transmits correctly, and
`STARTBIT-INDEP` grades the start bit's appearance on its own printed sentence.
So the red is not "this fixture cannot see a transmission" — it is "this
transmission should not have happened".

## What the others say

| | |
|---|---|
| **vAmiga** | **implements it.** `if (transmitShiftReg == 0 && transmitBuffer != 0)` — `Core/Components/Paula/UART/UART.cpp:137`. The `!= 0` is the page's sentence |
| **Denise** | **implements it.** `if (value && !serShifter)` — `emulation/libami/paula/uart.cpp:42`. The same guard, one token |
| **WinUAE** | **implements it in one of its two paths.** `od-win32/serial_win32.cpp:999-1004` returns early on `if (!w)`, with a log line that calls the case out by name: *"zero serial word written?!"*. In `ser_accurate` mode (`:983-992`) there is no such guard and the word is loaded unconditionally |
| **Copperline** | **deliberately does not.** `write_serdat` loads any word (`src/chipset/paula.rs:1079-1082`), and `serial_tx_total_bits` carries an explicit branch for it: `if word == 0 { return if long { 11 } else { 10 }; }` (`:1575-1578`) — a full frame, written on purpose |
| **MiSTer** | descendant — the same gap in the same file: `rtl/paula_uart.v:77-140` is byte-identical to NanoMig's, with no test of the written value anywhere in the transmit path |
| **TC64** | descendant — byte-identical too, line numbers included; the whole 262-line file matches NanoMig line for line, confirmed by a full-file comparison and not only the cited range. The all-zero-still-transmits gap is inherited unchanged by both lines |

**Three of the four independent implementations implement the sentence
somewhere**, two of them with a single added token. That is what makes the
omission worth reporting rather than excusing.

The two dissents are the interesting part and neither is an oversight.
Copperline wrote a branch to transmit a zero word. WinUAE implements the rule in
its ordinary path and drops it in its cycle-accurate one — which is the opposite
of what one would expect if the rule were simply true of the silicon.
**NanoMig's behaviour matches those two.**

Whether p.259 describes the silicon or prescribes an interface is the question
an Amiga hardware person has to settle, and `paula.rs:1575` and
`serial_win32.cpp:983` are the two places to read before concluding the page is
unambiguous. The measurement is not in doubt.

## A citation withdrawn

An earlier version of this entry cited `winuae/serial.cpp:174` and `:199` and
described WinUAE as unable to answer. **That file is not in WinUAE's build** —
the MSVC project's `serial.cpp` entries are `pcem\serial.cpp`, the PC-emulation
subsystem, and the live model is `od-win32/serial_win32.cpp`. The root
`serial.cpp` is 1996-era UAE whose serial body sits behind a `POSIX_SERIAL` its
own line 22 undefines.

Citing it was the defect this project names as *a dead citation is worse than a
dash*: the file exists, the function exists, the line says what it appears to
say, and none of it runs.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

