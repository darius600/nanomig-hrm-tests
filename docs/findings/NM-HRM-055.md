# NM-HRM-055 — SERDAT written with all zeros still transmits [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L77-L140) |
| **Report to** | **the Minimig line**, not NanoMig alone |
| **Test** | [`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) `ALLZERO-NO-TX`, and independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) `E6` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, in two packages built by different agents from different claim sets |
| **Verdict of the four** | **3–1, with a split inside one of them.** vAmiga and Denise implement the sentence in one token; WinUAE implements it in its ordinary path and not in its `ser_accurate` one; Copperline writes an explicit branch to transmit anyway |

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

The same fixture writing a non-zero word transmits correctly —
`WRITE-STARTS-TX`, green — and `STARTBIT-AUTO` grades the start bit's
appearance on its own printed sentence.
So the red is not "this fixture cannot see a transmission" — it is "this
transmission should not have happened".

## What the others say

| | |
|---|---|
| **vAmiga** | **implements it.** `if (transmitShiftReg == 0 && transmitBuffer != 0)` — `Core/Components/Paula/UART/UART.cpp:137`. The `!= 0` is the page's sentence |
| **Denise** | **implements it.** `if (value && !serShifter)` — `emulation/libami/paula/uart.cpp:42`. The same guard, one token |
| **WinUAE** | **implements it in one of its two paths.** `od-win32/serial_win32.cpp:999-1004` returns early on `if (!w)`, with a log line that calls the case out by name: *"zero serial word written?!"*. In `ser_accurate` mode (`:983-992`), which cycle-exact memory or a 68020 or lower in compatible mode at `m68k_speed == 0` selects (`:1388`), there is no such guard and the word is loaded unconditionally |
| **Copperline** | **deliberately does not.** `write_serdat` loads any word (`src/chipset/paula.rs:1079-1082`), and `serial_tx_total_bits` carries an explicit branch for it: `if word == 0 { return if long { 11 } else { 10 }; }` (`:1575-1578`) — a full frame, written on purpose |
| **MiSTer** | descendant — the same gap in the same file: `rtl/paula_uart.v:77-140` is identical to NanoMig's, with no test of the written value anywhere in the transmit path |
| **TC64** | descendant — identical too, line numbers included; the whole 262-line file matches NanoMig line for line, confirmed by a full-file comparison and not only the cited range. The all-zero-still-transmits gap is inherited unchanged by both lines |

**Three of the four independent implementations implement the sentence
somewhere**, two of them with a single added token. That is what makes the
omission worth reporting rather than excusing.

The two dissents are the interesting part.
Copperline wrote a branch to transmit a zero word. WinUAE implements the rule in
its ordinary path and drops it in its `ser_accurate` one — which is the opposite
of what one would expect if the rule were simply true of the silicon.
**NanoMig's behaviour matches those two.**

Whether p.259 describes the silicon or prescribes an interface is the question
an Amiga hardware person has to settle, and `paula.rs:1575` and
`serial_win32.cpp:983` are the two places to read before concluding the page is
unambiguous. The measurement is not in doubt.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

