# NM-HRM-056 — SERDATR's TBE bit cannot be observed clear from outside

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red, contested — the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_uart.v#L139) |
| **Report to** | **the Minimig line** |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) `TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY` |
| **Chapter** | 8 · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 1 red, with a mutation that flips it green by peeking unsynchronised — the demonstration that the bit is not unobservable in principle |
| **Verdict of the four** | **2–2.** Denise and Copperline do exactly what NanoMig does; WinUAE and vAmiga hold the bit clear for a real interval |

---

## What the HRM says

**p.257** gives bit 13 as TBE, the transmit-buffer-empty flag, and p.259 makes
writing SERDAT the thing that fills the buffer. A flag that is never observed in
its cleared state tells software nothing.

## What NanoMig does

`paula_uart.v:139` reasserts `tx_tbe` whenever `tx_state` reads `TX_IDLE`:

    tx_tbe <= (rga_i == REG_SERDAT[8:1]) ? 1'b0
            : ((tx_state == TX_IDLE) ? 1'b1 : tx_tbe);

and the state is still `TX_IDLE` on the cycle the write lands. So the 0 exists
for one `clk7_en` pulse and is gone before any bus read can sample it.

## Why this is a statement about the design, not the fixture

The builder argued it and checked it rather than asserting it. `clk7_en` is not
the harness's invention: `amiga_clk.v:17-33` generates it as a fixed
28.375160 MHz ÷ 4, the machine's own ~7.09 MHz CPU clock enable. **All five
`always` blocks in `paula_uart.v` are gated on it without exception**
(`:38, :50, :59, :77, :156`), so nothing in the module can act between two
enables. A real 68000 bus cycle spans at least four S-states, more with the wait
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
| **vAmiga** | **observable.** TBE is derived live from `transmitBuffer == 0` (`Core/Components/Paula/UART/UART.cpp:108`), and the buffer is only drained by `copyToTransmitShiftRegister` (`:178`). While the shifter is busy the guard at `:137` fails, nothing is scheduled, and TBE reads 0 for the rest of the frame |
| **WinUAE** | **observable, deliberately.** `od-win32/serial_win32.cpp:780` sets `data_in_serdat_delay = get_cycles() + CYCLE_UNIT` **after** `:741` has already cleared the buffer flag, and the read at `:1020` keeps bit 13 clear until that cycle passes. A modelled TBE-low window, not a side effect |
| **MiSTer** | descendant — the same statement on the same line: `rtl/paula_uart.v:139` is byte-identical to NanoMig's, the same one-cycle TBE-clear window |
| **TC64** | descendant — `rtl/minimig/paula_uart.v:139` — the identical `tx_tbe <= (rga_i == REG_SERDAT[8:1]) ? 1'b0 : ((tx_state == TX_IDLE) ? 1'b1 : tx_tbe);`, same file, same line number, byte-identical. The one-cycle TBE-clear window this entry measures is inherited unchanged by both lines. |

**Two of the four independent implementations reach NanoMig's behaviour and two
model a drain time of their own.** So the four independent implementations do
not settle whether a real Paula shows TBE low; they settle that it is a
modelling choice each project made independently, and that two projects thought
the window worth building on purpose.

One qualification the survey stated rather than smoothed: vAmiga's *idle* case
depends on same-cycle slot ordering between `SLOT_REG` and `SLOT_TXD` and is not
decided by reading the source. Its busy-shifter case needs no such tiebreak and
is what the row above rests on.
