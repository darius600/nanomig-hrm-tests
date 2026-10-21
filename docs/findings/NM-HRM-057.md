# NM-HRM-057 — SERDATR bit 11 is a registered copy of RXD, not a direct read [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Does "Direct read of RXD pin on Paula chip." (p.257, SERDATR bit 11) require bit 11 to follow the RXD pin with no `clk7_en` edge in between, where NanoMig takes three? It closes when `RXD-BIT11-IS-NOT-A-DIRECT-COMBINATIONAL-READ`, `C4` and `SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT` turn green, or when an Amiga specialist records that "direct" permits a synchroniser in front of bit 11. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | SERDATR bit 11 read at `paula_uart` and at the `paula` boundary, after the RXD pin steps from 1 to 0; chip revision unknown; real Paula not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 · printed p.257, SERDATR bit 11 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: `RXD-BIT11-IS-NOT-A-DIRECT-COMBINATIONAL-READ`: `sol:8-serdatr.c43` (p.257), `opus:8-serdatr.c23`, `opus:8-serdatr.c24`, `sol:8-serdatr.c105`, `sol:8-serdatr.c106` (p.283); `C4`: `opus:8-serial.c81` (p.257); `SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT`: `opus:8-serial.c82` (p.257); all three reds carry a claim id |
| **Module** | `paula_uart.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_uart.v#L46-L53) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L36-L43); inherited |
| **Test** | [`hrm/ch8-serdatr-read-word`](../../hrm/ch8-serdatr-read-word): `make -C hrm/ch8-serdatr-read-word test`; [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface): `make -C hrm/ch8-serial-interface test`; [`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations): `make -C hrm/ch8-adkcon-and-interrupt-relations test`; an expected red exits nonzero |
| **Assertions** | 3 red, one per package, each one of the package's reds: `RXD-BIT11-IS-NOT-A-DIRECT-COMBINATIONAL-READ`, expected 0 `clk7_en` enables, observed 3 (`12 passed, 2 failed`); `C4`, expected 0 beats, observed 2 (`48 passed, 3 failed`); `SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT`, expected 0 edges, observed 3 (`17 passed, 2 failed`) |
| **Verdict of the four** | **2–1 for the page, one cannot answer, and the one dissent does not match NanoMig either**. Hardware: not measured |
| **Effort** | none; if an Amiga specialist records that "direct" requires zero lag, S: the change touches `paula_uart.v:46-53` and `:244` |

---

## Source requirement

**p.257**, the whole of bit 11's entry:

> Direct read of RXD pin on Paula chip.

## RTL and measurement

Two synchroniser flops and one more register:

    reg [1:0] rxd_sync = 2'b11;             // paula_uart.v:46
    rxd_sync <= {rxd_sync[0], rxd};         // :40
    assign rxds = rxd_sync[1];              // :43
    rx_rxd <= rxds;                         // :234
    assign serdatr = {rx_ovrun, rx_rbf, tx_tbe, tx_tsre, rx_rxd};   // :248

`ch8-serdatr-read-word` measures 3 `clk7_en` enables between a pin change and
bit 11. `ch8-adkcon-and-interrupt-relations` measures the same 3 edges at the
`paula` boundary. `ch8-serial-interface` measures 2 beats from its own starting
point, against a control showing the bus read mux itself costs none: the three
agree on a fixed chain and differ only in where they start counting.

## Risks and unknowns

- p.257 does not say whether "direct" forbids a synchroniser; the lag is not in
  doubt, the adjudication is an Amiga hardware question.
- Real Paula's lag is not measured; no named software depends on it.
- The four independent implementations split: vAmiga and Denise read the pin
  immediately, Copperline delays it, WinUAE has no pin to read.

## Independent evidence

| | |
|---|---|
| **WinUAE** | **cannot answer.** No RXD pin to read: bit 11 is a break-state flag forced high unless a break is active ([`od-win32/serial_win32.cpp:1050-1051`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L1050-L1051)); only loopback reconstructs a level, from the transmit shift register by elapsed bit position ([`:1032-1048`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L1032-L1048)) |
| **vAmiga** | **immediate.** `REPLACE_BIT(result, 11, serialPort.getRXD())` — [`Core/Components/Paula/UART/UART.cpp:110`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/UART/UART.cpp#L110), and `getRXD()` is `getPin(3)` ([`Core/Ports/SerialPort.h:151`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/SerialPort.h#L151)). No intervening state |
| **Denise** | **immediate.** `if (serial.getRXD()) out \|= 0x800;` — [`emulation/libami/paula/uart.cpp:32`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/uart.cpp#L32), over `getPin(RXD)` ([`emulation/libami/system/serial.h:37`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/serial.h#L37)). No RX synchroniser exists in the tree |
| **Copperline** | **delayed by two stages.** `serial_rx_pin_high` → `serial_rx_sync_0_high` → `serial_rx_sync_1_high` ([`src/chipset/paula.rs:505-507`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L505-L507), advanced once per colour clock at [`:1612-1616`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1612-L1616)), read at [`:1101`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1101): two colour clocks, four `clk7_en` edges. Deliberate and tested: `fn serdatr_rxd_uses_two_stage_synchronized_pin()` at [`:3896`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L3896) |
| **MiSTer** | descendant — [`rtl/paula_uart.v:36-43`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L36-L43) byte-identical to NanoMig's, the same three-edge lag |
| **TC64** | descendant — [`rtl/minimig/paula_uart.v:36-43`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_uart.v#L36-L43), byte-identical, the same three-edge lag |

## Decision

Does p.257's "direct" forbid the synchroniser in front of bit 11? No change is
proposed now. A test separating NanoMig's three `clk7_en` edges from
Copperline's four would set NanoMig apart from every emulator that has a pin,
and is the measurement to take before deciding. The page closes when the three
reds turn green, or when an Amiga specialist records that "direct" permits a
synchroniser.
