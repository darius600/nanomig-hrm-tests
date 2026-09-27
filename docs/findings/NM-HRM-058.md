# NM-HRM-058 — the nine-bit received-character window is longer than p.256 prints, and every implementation with a receiver agrees

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. The red is against p.256, not against the RTL. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L156-L230) |
| **Report to** | **the manual.** Three of the four reference implementations land on NanoMig's numbers exactly |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) `RECEIVER-FULL-CHARACTER-TIME-WINDOW-8-TO-10`, and independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) `B11` |
| **Chapter** | 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, in two packages |
| **Verdict of the four** | **three of four land on 10 and 11 bit times, exactly as NanoMig does** |

---

## What the HRM says

**p.256:27** — *"up to one full character-receive time (8 to 10 bit times) to
accept the data and clear"*.

The same page offers the frame that range does not reach:

> Bit 15 is a 1 if you chose nine data bits for the receive-register full signal, and a 0 if you chose eight data bits.

One start bit, eight data bits and one stop bit make ten bit times, the printed
ceiling; with nine data bits the frame is eleven. The finding rests on the
nine-bit frame alone.

## What NanoMig does

**10 bit times for an 8-bit frame, 11 for a 9-bit frame** — at the printed
ceiling in the short case and one past it in the long one. Two packages
measured it independently and differ only in where they start counting: 10.0 and
11.0 from one, 9.68 and 10.68 from the other.

### Measured

[`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) measures the
back-to-back receive window at 9.681818 bit times for an 8-bit frame and
10.681818 for a 9-bit frame —
`RECEIVER-FULL-CHARACTER-TIME-WINDOW-8-TO-10`, red, one of the package's `12
passed, 2 failed`. [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface)
measures the same window from its own starting point at 10 bit-cells for an
8-bit frame and 11 for a 9-bit frame — `B11`, red, one of the package's `48
passed, 3 failed`. Both reds come from the 9-bit frame: the 8-bit values, 9.68
and 10, lie inside 8 to 10.

## Why this is filed against the page

| | |
|---|---|
| **vAmiga** | **10 / 11.** `if (recCnt >= packetLength() + 2)` — `Core/Components/Paula/UART/UARTEvents.cpp:83`, with `packetLength()` returning 9 or 8 (`UART.h:172`); the +2 is start and stop |
| **Denise** | **10 / 11.** `unsigned bits = (serPer & 0x8000) ? 11 : 10;` — `emulation/libami/paula/uart.cpp:78`, counted at `:80` |
| **Copperline** | **10 / 11.** `total_bits: if long { 11 } else { 10 }` — `src/chipset/paula.rs:1557`, set at shift-load rather than counted against a constant |
| **WinUAE** | **neither confirms nor contradicts.** It has no receiver: bytes arrive whole from the host (`od-win32/serial_win32.cpp:464`, `:600`). Its only 10/11 constant is on the *transmit* side (`:261`), and its loopback receive time is 9 or 10 bit times **plus half a bit** (`:799`), measured to a mid-bit sample rather than as a window bound |
| **MiSTer** | descendant — carries the same receiver lines, already noted on this page |
| **TC64** | `rtl/minimig/paula_uart.v:156-230` — descendant, the identical receiver, same file, same line numbers, byte-identical, confirmed by a full-file comparison. The 10-/11-bit-time window this entry measures against p.256 is inherited unchanged by both lines; the red is against the manual in both cases, not against either RTL |

**Three independent implementations arrive at exactly NanoMig's numbers.** A
frame of one start bit, eight or nine data bits and one stop bit is ten or eleven
bit times by construction, and the page's "8 to 10" cannot describe the eleven.

So the red is real and it is not about this design. What p.256 means by "8 to
10" — whether it counts differently, describes a different interval, or is
simply wrong — is a question for someone who can read it against a real Paula.
This entry records that no implementation, including the two that model the
receiver most carefully, produces a 9-bit-frame number inside the printed range.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "the length of said shift register can be switched to 9 or to 10 Bits."
  <!-- corpus: paula-8364r7-dissection/9_9_txt.txt -->

  The RX register holds the data plus the stop bit, and one more bit holds the start bit: "When the Start Bit comes out of the shift register (there is another "hidden shift register Bit" for that), the UART sees the received frame as completed." A received character on 8364R7 is therefore start + 8 or 9 data + stop = 10 or 11 bit cells: NanoMig's numbers, and outside "8 to 10". The note states how the frame is built, not how long software has to read it; the 10/11 is derived here from that and is not in the note.

Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

