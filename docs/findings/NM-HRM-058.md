# NM-HRM-058 — the nine-bit received-character window is longer than p.256 prints, and every implementation with a receiver agrees

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect; the red is against p.256, not against the RTL. Question: does p.256's "up to one full character-receive time (8 to 10 bit times)" describe a window that a nine-data-bit frame, which takes 11 bit times, does not fit in? Closes when an Amiga specialist records that "8 to 10" counts differently or covers only the eight-data-bit frame, or confirms that the nine-data-bit frame fits in it, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Chapter 8 of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | Paula's serial receiver with nine data bits (SERPER bit 15 set), measured on NanoMig's `paula_uart.v` at the pinned revision; chip revision unknown; hardware not measured. The eight-data-bit frame lies inside the printed range (9.68 and 10 bit times) |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8, printed p.256 (receive-time sentence and SERPER bit-15 sentence) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims `RECEIVER-FULL-CHARACTER-TIME-WINDOW-8-TO-10`: `sol:8-serdatr.c9`, `sol:8-serdatr.c10`, p.256; `B11`: `opus:8-serial.c54`, p.256. Both assertions carry a claim id |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L156-L230); inherited (identical in MiSTer and TC64) |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word): `make -C hrm/ch8-serdatr-read-word test`, and independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface): `make -C hrm/ch8-serial-interface test`; an expected red exits nonzero |
| **Assertions** | 2 red, in two packages. `RECEIVER-FULL-CHARACTER-TIME-WINDOW-8-TO-10`: expected both windows in 8 to 10 bit times, observed 9.681818 (8-bit) and 10.681818 (9-bit); the package reports `12 passed, 2 failed`. `B11`: expected both in 8 to 10 bit-cells, observed 10 (8-bit) and 11 (9-bit); the package reports `48 passed, 3 failed` |
| **Verdict of the four** | **three of four land on 10 and 11 bit times, exactly as NanoMig does**. Hardware: not measured |
| **Effort** | none — no RTL change asked; the open question is the wording of p.256 |

---

## Source requirement

**p.256:27**:

> up to one full character-receive time (8 to 10 bit times) to accept the data and clear

The same page offers the frame that range does not reach:

> Bit 15 is a 1 if you chose nine data bits for the receive-register full signal, and a 0 if you chose eight data bits.

One start bit, eight data bits and one stop bit make ten bit times, the printed
ceiling; with nine data bits the frame is eleven. The finding rests on the
nine-bit frame alone.

## RTL and measurement

**10 bit times for an 8-bit frame, 11 for a 9-bit frame** — at the printed
ceiling in the short case and one past it in the long one.

[`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) measures the
back-to-back receive window at 9.681818 bit times for an 8-bit frame and
10.681818 for a 9-bit frame (`RECEIVER-FULL-CHARACTER-TIME-WINDOW-8-TO-10`, red).
[`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) measures it from its
own starting point at 10 bit-cells and 11 (`B11`, red). Both reds come from the
9-bit frame: the 8-bit values lie inside 8 to 10.

## Risks and unknowns

- What p.256 means by "8 to 10" is open: it may count differently, describe a
  different interval, or be wrong. Real Paula's receive window is not measured.
- WinUAE has no receiver, so the vote is three of four.
- The 6502.org note states how the 8364R7 frame is built, not how long software
  has to read it; the 10/11 is derived from it here. Silicon of one revision.

## Independent evidence

| | |
|---|---|
| **WinUAE** | **neither confirms nor contradicts.** It has no receiver: bytes arrive whole from the host (`od-win32/serial_win32.cpp:464`, `:600`); its only 10/11 constant is on the *transmit* side (`:261`), and its loopback receive time is 9 or 10 bit times **plus half a bit** (`:799`), measured to a mid-bit sample rather than as a window bound |
| **vAmiga** | **10 / 11.** `if (recCnt >= packetLength() + 2)` — `Core/Components/Paula/UART/UARTEvents.cpp:83`, with `packetLength()` returning 9 or 8 (`UART.h:172`); the +2 is start and stop |
| **Denise** | **10 / 11.** `unsigned bits = (serPer & 0x8000) ? 11 : 10;` — `emulation/libami/paula/uart.cpp:78`, counted at `:80` |
| **Copperline** | **10 / 11.** `total_bits: if long { 11 } else { 10 }` — `src/chipset/paula.rs:1557`, set at shift-load rather than counted against a constant |
| **MiSTer** | descendant — the same receiver lines |
| **TC64** | descendant — `rtl/minimig/paula_uart.v:156-230`, byte-identical to NanoMig's, confirmed by a full-file comparison |

### What other Commodore documents say

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "the length of said shift register can be switched to 9 or to 10 Bits."
  <!-- corpus: paula-8364r7-dissection/9_9_txt.txt -->

  The RX register holds the data plus the stop bit, and one more bit holds the start bit: "When the Start Bit comes out of the shift register (there is another "hidden shift register Bit" for that), the UART sees the received frame as completed." A received character on 8364R7 is therefore start + 8 or 9 data + stop = 10 or 11 bit cells: NanoMig's numbers, and outside "8 to 10".

## Decision

No RTL change is asked. The two reds stay against p.256 until the closing event in row 4.
