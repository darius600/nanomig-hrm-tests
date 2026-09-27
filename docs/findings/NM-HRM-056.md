# NM-HRM-056 — SERDATR's TBE bit cannot be observed clear from outside [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L139) |
| **Report to** | **the Minimig line** |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY` |
| **Chapter** | 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red, with a mutation that flips it green by peeking unsynchronised — the demonstration that the bit is not unobservable in principle |
| **Verdict of the four** | **3–1.** Denise, Copperline and vAmiga do what NanoMig does after a write from idle; WinUAE holds the bit clear for one `CYCLE_UNIT` in its `ser_accurate` mode |

---

## What the HRM says

**p.257** gives bit 13 as TBE, the transmit-buffer-empty flag, and p.259 makes
writing SERDAT the thing that fills the buffer. A flag that is never observed in
its cleared state tells software nothing.

## What NanoMig does

`paula_uart.v:139` reasserts `tx_tbe` whenever `tx_state` reads `TX_IDLE`:

    tx_tbe <= (rga_i == REG_SERDAT[8:1]) ? 1'b0
            : ((tx_state == TX_IDLE) ? 1'b1 : tx_tbe);

and, when no frame is being shifted, the state is still `TX_IDLE` on the cycle
the write lands. So the 0 exists for one `clk7_en` pulse and is gone before any
bus read can sample it.

### Measured

[`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) reads SERDATR
idle (`0x3800`) and immediately after a SERDAT write (`0x2800`) —
`TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY`, red, one of the package's `11
passed, 3 failed`; a mutation that peeks the register unsynchronised instead
of through the ordinary bus read turns the same assertion green, showing the
bit does change and only the reachable read misses it.

## Why this is a statement about the design, not the fixture

`clk7_en` is not the harness's invention: `amiga_clk.v:17-33` generates it as a fixed
28.375160 MHz ÷ 4, the machine's own ~7.09 MHz CPU clock enable. **All five
`always` blocks in `paula_uart.v` are gated on it without exception**
(`:38, :50, :59, :77, :156`), so nothing in the module can act between two
enables. A real 68000 bus cycle spans at least four clock cycles (eight
S-states), more with the wait
states a chip-register access adds — strictly longer than the back-to-back
single edge this harness uses. **A genuine CPU read lands later, so it is more
certain to miss the 0, not less.**

The mutation that flips the assertion green replaces the standard bus read with
an unsynchronised peek, which does catch the 0. That is the control: the bit
changes, and no reachable read sees it change.

## What the others say

| | |
|---|---|
| **Denise** | **as NanoMig.** `setSerdat` assigns `serDat = value` and calls `prepareTransfer()` in the same event, which zeroes `serDat` at `emulation/libami/paula/uart.cpp:112`; the read derives TBE from `if (!serDat)` at `:16`. Set and cleared atomically — no read can interleave |
| **Copperline** | **as NanoMig.** `write_serdat` stores `Some(val)` and immediately calls `load_serial_shift_if_idle`, whose `.take()` empties it in the same call (`src/chipset/paula.rs:1080`, `:1524`). Its own test asserts the consequence: `assert_ne!(paula.read_serdatr() & (1 << 13), 0)` right after a write (`:3681`) |
| **vAmiga** | **as NanoMig after a write from idle.** TBE is derived live from `transmitBuffer == 0` (`Core/Components/Paula/UART/UART.cpp:108`), and the buffer is only drained by `copyToTransmitShiftRegister` (`:178`). `setSERDAT` fills the buffer and, with the shifter empty, schedules `SLOT_TXD` with no delay (`:134-139`); that lowers the secondary-slot trigger to the current cycle (`Core/Components/Agnus/Agnus.h:739-741`), and `executeUntil` services `SLOT_REG` and then the secondary slots, `SLOT_TXD` among them, in the same pass (`Core/Components/Agnus/Agnus.cpp:559-560`, `:581`, `:611-612`), so the buffer is empty again before any later read. While the shifter is busy the guard at `:137` fails and TBE reads 0 for the rest of the frame, as NanoMig's `paula_uart.v:139` holds `tx_tbe` while `tx_state` is not `TX_IDLE` |
| **WinUAE** | **observable, deliberately, in its `ser_accurate` mode** (`ser_accurate = currprefs.cpu_memory_cycle_exact \|\| (currprefs.cpu_model <= 68020 && currprefs.cpu_compatible && currprefs.m68k_speed == 0)`, `od-win32/serial_win32.cpp:1388`). There `:780` sets `data_in_serdat_delay = get_cycles() + CYCLE_UNIT` **after** `:741` has already cleared the buffer flag, and the read at `:1020` keeps bit 13 clear until that cycle passes. A modelled TBE-low window, not a side effect; outside `ser_accurate` the read does not consult the delay |
| **MiSTer** | descendant — the same statement on the same line: `rtl/paula_uart.v:139` is identical to NanoMig's, the same one-cycle TBE-clear window |
| **TC64** | descendant — `rtl/minimig/paula_uart.v:139` — the identical `tx_tbe <= (rga_i == REG_SERDAT[8:1]) ? 1'b0 : ((tx_state == TX_IDLE) ? 1'b1 : tx_tbe);`, same file, same line number. The one-cycle TBE-clear window this entry measures is inherited unchanged by both lines. |

**Three of the four independent implementations reach NanoMig's behaviour after
a write from idle, and one models a window of its own.** So the four
independent implementations do not settle whether a real Paula shows TBE low;
they settle that it is a modelling choice, and that one project, WinUAE,
thought the window worth building on purpose.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

