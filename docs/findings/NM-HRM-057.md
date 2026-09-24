# NM-HRM-057 — SERDATR bit 11 is a registered copy of RXD, not a direct read [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red, contested — the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_uart.v#L36-L43) |
| **Report to** | **the Minimig line** |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) `RXD-BIT11-IS-NOT-A-DIRECT-COMBINATIONAL-READ`, and independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) `C4` |
| **Chapter** | 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, in two packages built by different agents |
| **Verdict of the four** | **3–1 for the page, and the one dissent does not match NanoMig either** |

---

## What the HRM says

**p.257**, the whole of bit 11's entry: *"Direct read of RXD pin on Paula chip."*

## What NanoMig does

Two synchroniser flops and one more register:

    reg [1:0] rxd_sync = 2'b11;             // paula_uart.v:36
    rxd_sync <= {rxd_sync[0], rxd};         // :40
    assign rxds = rxd_sync[1];              // :43

Measured: the bit lags a pin change by **three `clk7_en` edges**. The
`ch8-serial-interface` package measured the same path as two beats from its own
starting point and its control shows the bus read mux itself costs none — the
two agree on there being a fixed chain and differ only in where they start
counting.

### Measured

[`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word) measures a
delay of 3 `clk7_en` enable(s) between a pin change and bit 11 —
`RXD-BIT11-IS-NOT-A-DIRECT-COMBINATIONAL-READ`, red, one of the package's `11
passed, 3 failed`. [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface)
measures the same path from its own starting point as 2 `clk7_en` beats,
against a control showing the bus read path itself is purely combinational —
`C4`, red, one of the package's `48 passed, 3 failed`.

## What the others say

| | |
|---|---|
| **vAmiga** | **immediate.** `REPLACE_BIT(result, 11, serialPort.getRXD())` — `Core/Components/Paula/UART/UART.cpp:110`, and `getRXD()` is `getPin(3)` (`Core/Ports/SerialPort.h:151`). No intervening state |
| **Denise** | **immediate.** `if (serial.getRXD()) out \|= 0x800;` — `emulation/libami/paula/uart.cpp:32`, over `getPin(RXD)` (`emulation/libami/system/serial.h:37`). No RX synchroniser exists in the tree |
| **Copperline** | **delayed by two stages.** `serial_rx_pin_high` → `serial_rx_sync_0_high` → `serial_rx_sync_1_high` (`src/chipset/paula.rs:505-507`, advanced at `:1612-1616`), read at `:1101`. Deliberate and tested: `fn serdatr_rxd_uses_two_stage_synchronized_pin()` at `:3896` |
| **WinUAE** | **cannot answer.** There is no RXD pin to read. Bit 11 is a break-state flag forced high unless a break is active (`od-win32/serial_win32.cpp:1050-1051`); only the loopback path reconstructs a level, and it does so from the transmit shift register by elapsed bit position (`:1032-1048`) |
| **MiSTer** | descendant — the same synchroniser on the same lines: `rtl/paula_uart.v:36-43` is byte-identical to NanoMig's, the same three-`clk7_en`-edge lag |
| **TC64** | descendant — `rtl/minimig/paula_uart.v:36-43` — the identical two-flop synchroniser and `assign rxds = rxd_sync[1];`, same file, same line numbers, byte-identical. The three-`clk7_en`-edge lag this entry measures is inherited unchanged by both lines |

**Two of the four independent implementations read the pin exactly as the page
says, one adds two synchroniser stages, one has no pin.** So a synchroniser is
not unheard of — but nobody has three.

That gap is the useful part of this entry. **A test that distinguishes a
two-cycle lag from a three-cycle one separates NanoMig from every emulator here,
Copperline included**, and that is the measurement to take before anyone decides
whether "direct" forbids a synchroniser at all. The adjudication is an Amiga
hardware question; the lag is not in doubt.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

