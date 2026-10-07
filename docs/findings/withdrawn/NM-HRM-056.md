# NM-HRM-056 — SERDATR's TBE bit cannot be observed clear from outside

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 258 moves a word written from idle into the shift register at the start of the operation, and where p. 257 needs TBE clear, while a second word waits, NanoMig reads it 0 on 219 consecutive reads |
| **What is asked of a reader** | Nothing. A red from one SERDAT write with the transmitter idle and the SERDATR read after it is not a finding: p. 258 has that word leave SERDAT at the start of the operation, and TBE stays clear only while a second word waits, as NanoMig keeps it. |
| **Withdrawal reason** | No page requires TBE to read 0 after a write from an idle transmitter; p. 257's clear TBE is the waiting-word case, which NanoMig keeps |
| **Error class** | **a silence read as a requirement** |
| **Replacement** | Green: `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY` in [`hrm/ch8-serdatr-read-word`](../../../hrm/ch8-serdatr-read-word) (TBE read 0 on 219 consecutive reads while the second word waited, then 1 with TSRE 0); `DOUBLE-BUFFER-AND-TBE` in [`hrm/ch8-serdat-transmit`](../../../hrm/ch8-serdat-transmit) (TBE back at 1 within 3 ticks of a write from idle) |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L139); inherited (line 139 identical in MiSTer and TC64) |
| **Test** | `make -C hrm/ch8-serdatr-read-word test` (`12 passed, 2 failed`) and `make -C hrm/ch8-serdat-transmit test` (`17 passed, 1 failed`); the reds in both are other assertions, and an expected red exits nonzero |
| **Chapter** | 8 — Interface Hardware, printed pp. 257–258 (SERDATR TBE and TSRE rows; SERDAT); Appendix A, printed p. 283 (SERDAT, SERDATR) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids by assertion: `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY`: `sol:8-serdatr.c25`, `sol:8-serdatr.c27`, `sol:8-serdatr.c31`, p. 257; `DOUBLE-BUFFER-AND-TBE`: `sol:8-serdat.c6`, `sol:8-serdat.c7`, `sol:8-serdat.c8`, p. 258, and `opus:8-serdat.c7`, `opus:8-serdat.c8`, `sol:8-serdat.c44`, `sol:8-serdat.c45`, p. 283; `TSRE-BIT12-TRACKS-COMPLETE-TRANSMISSION`: `sol:8-serdatr.c33`, `sol:8-serdatr.c35`, `sol:8-serdatr.c36`, `sol:8-serdatr.c37`, `sol:8-serdatr.c38`, `sol:8-serdatr.c39`, p. 257, and `opus:8-serdatr.c20`, `opus:8-serdatr.c21`, `sol:8-serdatr.c99`, `sol:8-serdatr.c101`, `sol:8-serdatr.c102`, p. 283 |
| **Assertions** | `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY` green; `DOUBLE-BUFFER-AND-TBE` green, measured `tbe_duration=1 ticks, tsre_duration=374 ticks`; on the read just after one write from idle SERDATR reads `0x2800`, TBE already 1 (`TSRE-BIT12-TRACKS-COMPLETE-TRANSMISSION`, green, which grades bit 12 only) |
| **Verdict of the four** | **unanimous with NanoMig where p. 257 needs the bit clear**: all four keep TBE at 0 while a second word waits behind a busy shift register. After a write from idle, vAmiga, Denise and Copperline move the word into the shift register at once, as NanoMig does; WinUAE holds TBE at 0 for one `CYCLE_UNIT` in its `ser_accurate` mode. Hardware: not measured |

---

## Why it was withdrawn

The withdrawn claim is that the next bus read of SERDATR after a SERDAT write
with the transmitter idle must find bit 13, TBE, at 0. No page asks for that.

**p. 258 says when the word leaves SERDAT, and it is at once:**

> Immediately after you write the data into this register, the system will begin the transmission at the baud rate you selected.

> At the start of the operation, this data is transferred from SERDAT into an internal serial shift register. When the transfer to the serial shift register has been completed, SERDAT can accept new data; the TBE interrupt signals this fact.

From idle, the transfer is the start of the operation. No page gives it a
duration that a processor read must be able to see.

**p. 257 describes the case in which TBE is clear for long.** Its TSRE row:

> If you stop writing data into the output register ( SERDAT ), then this bit will become a 1 after both the word currently in the shift register and the word placed into SERDAT have been transmitted.

A word placed into SERDAT while another is still in the shift register waits
there, and TBE reads 0 until the shift register takes it. That is the use the
TBE row names:

> This bit is normally used for full-duplex operation.

NanoMig does both: after a write from idle its TBE returns to 1 on the next
`clk7_en` edge, when the word moves into the shift register; after a write while
a frame is shifting its TBE stays 0 until that frame has gone. A flag that is
cleared for as long as a word waits is a flag software can use, and p. 258
predicts the short window from idle. Read together, pp. 257 and 258 predict
what NanoMig does.

**Condition for filing again.** A test that writes SERDAT once and reads SERDATR
next finds TBE already set, on this core and on three of the four
implementations, and grades a red against the TBE row. The claim would be filed
again only if a printed page gave the transfer from idle a duration a processor
read must see, or required TBE to read 0 after a write from idle; neither p. 257
nor p. 258 does.

---

## What NanoMig does

A SERDAT write clears `tx_tbe`
([`paula_uart.v#L139`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L139));
it is set again only on an edge at which `tx_state` is `TX_IDLE`, and SERDATR
bit 13 is `tx_tbe` itself
([`:248`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L248)).

- **From idle**, the next `clk7_en` edge loads the shift register and sets
  `tx_tbe` again: TBE is 0 for one `clk7_en` period, the transfer p. 258 puts at
  the start of the operation.
- **While a frame is shifting**, `tx_state` is `TX_SHIFT` and `tx_tbe` keeps its
  0 until the frame ends (`:111-117`); the edge after that loads the waiting
  word and sets it again.

---

## Independent evidence

| | |
|---|---|
| **WinUAE** | **a window of its own after a write from idle, in `ser_accurate` mode:** [`serial_win32.cpp:780`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L780) sets `data_in_serdat_delay = get_cycles() + CYCLE_UNIT` and the read at `:1020` keeps bit 13 clear until it passes; with the shift register busy, TBE reads 0 as in the other three |
| **vAmiga** | **as NanoMig.** [`UART.cpp:108`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/UART/UART.cpp#L108) derives TBE live from `transmitBuffer == 0`; schedules `SLOT_TXD` with no delay (`:134-139`), and while the shifter is busy the guard at `:137` fails and TBE reads 0 |
| **Denise** | **as NanoMig.** [`uart.cpp:39`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/uart.cpp#L39) `setSerdat` calls `prepareTransfer()` at once when the shift register is empty (`:42`), which zeroes `serDat` (`:112`); with the shift register busy `serDat` keeps the word and TBE reads 0 |
| **Copperline** | **as NanoMig.** [`paula.rs:1520-1524`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1520-L1524) `load_serial_shift_if_idle` empties the buffer in the same call as the write when the shift register is idle, and returns before the `take` while it is busy |
| **MiSTer** | descendant — `rtl/paula_uart.v:139` is identical to NanoMig's |
| **TC64** | descendant — `rtl/minimig/paula_uart.v:139` is identical to NanoMig's |

**All four keep TBE at 0 while a second word waits, the case p. 257 describes.**
After a write from idle, three move the word into the shift register at once, as
NanoMig does, and WinUAE models a one-`CYCLE_UNIT` window on purpose in its
cycle-accurate mode; no page decides between the two.
