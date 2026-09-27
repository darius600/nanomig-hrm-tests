# NM-HRM-056 — SERDATR's TBE bit cannot be observed clear from outside

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. p. 258 moves a word written from idle into the shift register at the start of the operation, so TBE returns to 1 at once, as NanoMig's does; where p. 257 needs the bit clear, while a second word waits behind a busy shift register, NanoMig holds it at 0 until the shift register takes the word. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L139) |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) — its `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY` assertion writes a second word while the first is shifting, the case p. 257 describes, and requires TBE to read 0 on at least 20 consecutive reads before it reads 1 with TSRE 0 (`test_ch8-serdatr-read-word.cpp:533-592`) |
| **Chapter** | 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY` — green: TBE read 0 on 219 consecutive reads while the second word waited, then 1 with TSRE 0; one of the package's `12 passed, 2 failed`. `DOUBLE-BUFFER-AND-TBE` in [`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) grades p. 258 on a write from idle and is green: TBE back at 1 within 3 ticks |
| **Error class** | **a silence read as a requirement** |
| **Verdict of the four** | **unanimous with NanoMig where p. 257 needs the bit clear**: all four keep TBE at 0 while a second word waits behind a busy shift register. After a write from idle, vAmiga, Denise and Copperline move the word into the shift register at once, as NanoMig does; WinUAE holds TBE at 0 for one `CYCLE_UNIT` in its `ser_accurate` mode |

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

---

## What NanoMig does

[`src/minimig-aga/paula_uart.v#L139`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L139)

```verilog
      tx_tbe <= #1 (rga_i == REG_SERDAT[8:1]) ? 1'b0 : ((tx_state == TX_IDLE) ? 1'b1 : tx_tbe);
```

A SERDAT write clears `tx_tbe`; it is set again only on an edge at which
`tx_state` is `TX_IDLE`, and the waiting word is moved into the shift register
only from `TX_IDLE`:

[`src/minimig-aga/paula_uart.v#L91-L104`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L91-L104)

```verilog
          if (!tx_tbe) begin
            ...
            tx_shift <= #1 serdat;
            ...
            tx_state <= #1 TX_SHIFT;
```

- **From idle**, the next `clk7_en` edge loads the shift register and sets
  `tx_tbe` again: TBE is 0 for one `clk7_en` period, the transfer p. 258 puts at
  the start of the operation.
- **While a frame is shifting**, `tx_state` is `TX_SHIFT` and `tx_tbe` keeps its
  0 until the frame ends (`:111-117`); the edge after that loads the waiting
  word and sets it again. SERDATR bit 13 is `tx_tbe` itself
  ([`:248`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L248)).

### Measured

[`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) reads SERDATR idle
as `0x3800`, writes a second word while the first is shifting and polls: TBE
reads 0 on 219 consecutive reads, one per `clk7_en` period, until the first
word's frame has gone, and then 1 with TSRE clear (`0x2800`), the second word
now shifting — `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY`, green, one of the
package's `12 passed, 2 failed`. On the read after a single write from idle the
same package finds `0x2800`, TBE already set
(`TSRE-BIT12-TRACKS-COMPLETE-TRANSMISSION`, which grades bit 12 only).
[`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) measures the window
after a write from idle per tick, `tbe_duration=1 ticks, tsre_duration=374
ticks`, and grades it green as `DOUBLE-BUFFER-AND-TBE`.

---

## What the others say

| | |
|---|---|
| **Denise** | **as NanoMig.** `setSerdat` assigns `serDat = value` (`emulation/libami/paula/uart.cpp:39`) and, when the shift register is empty (`if (value && !serShifter) {`, `:42`), calls `prepareTransfer()`, which zeroes `serDat` at `:112`; the read derives TBE from `if (!serDat)` at `:16`. From idle, set and cleared in one event; with the shift register busy, `serDat` keeps the word and TBE reads 0 |
| **Copperline** | **as NanoMig.** `write_serdat` stores `Some(val)` and immediately calls `load_serial_shift_if_idle`, whose `.take()` empties it in the same call when the shift register is idle (`src/chipset/paula.rs:1080`, `:1520-1524`); while it is busy the function returns before the `take`, and TBE reads 0. Its own test asserts the idle case: `assert_ne!(paula.read_serdatr() & (1 << 13), 0)` right after a write (`:3681`) |
| **vAmiga** | **as NanoMig.** TBE is derived live from `transmitBuffer == 0` (`Core/Components/Paula/UART/UART.cpp:108`), and the buffer is only drained by `copyToTransmitShiftRegister` (`:178`). `setSERDAT` fills the buffer and, with the shifter empty, schedules `SLOT_TXD` with no delay (`:134-139`); that lowers the secondary-slot trigger to the current cycle (`Core/Components/Agnus/Agnus.h:739-741`), and `executeUntil` services `SLOT_REG` and then the secondary slots, `SLOT_TXD` among them, in the same pass (`Core/Components/Agnus/Agnus.cpp:559-560`, `:581`, `:611-612`), so the buffer is empty again before any later read. While the shifter is busy the guard at `:137` fails and TBE reads 0 for the rest of the frame, as NanoMig's `paula_uart.v:139` holds `tx_tbe` while `tx_state` is not `TX_IDLE` |
| **WinUAE** | **a window of its own after a write from idle, in its `ser_accurate` mode** (`ser_accurate = currprefs.cpu_memory_cycle_exact \|\| (currprefs.cpu_model <= 68020 && currprefs.cpu_compatible && currprefs.m68k_speed == 0)`, `od-win32/serial_win32.cpp:1388`). There `:780` sets `data_in_serdat_delay = get_cycles() + CYCLE_UNIT` **after** `:741` has already cleared the buffer flag, and the read at `:1020` keeps bit 13 clear until that cycle passes; outside `ser_accurate` the read does not consult the delay. With the shift register busy, `serdatcopy` returns before clearing the flag (`if (data_in_sershift > 0 \|\| !data_in_serdat)`, `:732`) and TBE reads 0, as in the other three |
| **MiSTer** | descendant — the same statement on the same line: `rtl/paula_uart.v:139` is identical to NanoMig's |
| **TC64** | descendant — `rtl/minimig/paula_uart.v:139` — the identical `tx_tbe <= (rga_i == REG_SERDAT[8:1]) ? 1'b0 : ((tx_state == TX_IDLE) ? 1'b1 : tx_tbe);`, same file, same line number. Both lines inherit NanoMig's TBE unchanged |

**All four keep TBE at 0 while a second word waits, the case p. 257 describes.**
After a write from idle, three move the word into the shift register at once, as
NanoMig does, and WinUAE models a one-`CYCLE_UNIT` window on purpose in its
cycle-accurate mode; no page decides between the two.

---

## What the test measures

`TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY`
(`test_ch8-serdatr-read-word.cpp:533-592`) reads SERDATR idle (`:536`), writes a
first word to SERDAT with the transmitter idle (`:537`), lets one `clk7_en`
period pass, the edge on which NanoMig moves that word into the shift register
(`:538`), and writes a second word while the first is shifting (`:547`). It then
reads SERDATR, one `read_reg(A_SERDATR)` bus cycle per `clk7_en` period
(`:170-177`), until bit 13 reads 1, for at most 300 reads (`:552`, `:561-565`).
It passes when TBE reads 1 idle, at least 20 reads find it at 0 before it
returns (`:570`), and the read that finds it at 1 has TSRE, bit 12, at 0
(`:567`, `:571-572`): the second word has left SERDAT and is shifting. Measured:
`idle=0x3800 (TBE=1) reads_while_clear=219 final=0x2800 (TBE=1 TSRE=0)`. The 219
reads are the rest of the first word's frame; TBE returns on the edge after it,
when `tx_state` is `TX_IDLE` again. Without the second write, the package's
mutation (`:542-546`), the first read already finds TBE at 1 and the 20-read
floor fails.

No assertion in the package grades bit 13 on the bus read after a single write
from idle, and the first word's own transfer is not read. The window p. 258 puts
at the start of the operation is graded per tick by
[`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit)'s
`DOUBLE-BUFFER-AND-TBE`.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

---

## Why this entry is kept

p. 257 defines TBE, and a test that writes SERDAT once and reads SERDATR next
finds it already set on this core and on three of the four implementations. The
bit is clear where p. 257's TSRE row puts a waiting word, and p. 258 has the word
from idle leave SERDAT at the start of the operation. A reader who grades the TBE
row with one write from idle will find a red; this page is where
they find pp. 257 and 258 read together.
