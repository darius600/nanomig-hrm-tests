# NM-HRM-055 — SERDAT written with all zeros still transmits [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `paula_uart.v` start no transmission when SERDAT is written with `0x0000`, as p.259 prints, where it now drives one low cell (a start bit) on `txd`? It closes when `ALLZERO-NO-TX` and `E6` turn green, or when an Amiga specialist rejects the guard; if the rejection means p.259 does not describe the silicon, the page is refiled against the printed page |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Paula's SERDAT (`$030`) written with the all-zero word `0x0000`; the transmit path only. Chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.259 (SERDAT, lines 26-27), and Appendix A, printed p.283 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `ALLZERO-NO-TX`: `sol:8-serdat.c37`, p.259; `opus:8-serdat.c9`, `sol:8-serdat.c46`, p.283; `E6`: `opus:8-serial.c140`, p.259; control `WRITE-STARTS-TX`: `sol:8-serdat.c36`, p.259; control `STARTBIT-AUTO`: `sol:8-serdat.c34`, `sol:8-serdat.c35`, `sol:8-serdat.c33`, p.259 |
| **Module** | `paula_uart.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_uart.v#L87-L150) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L77-L140); inherited |
| **Test** | [`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) `ALLZERO-NO-TX`: `make -C hrm/ch8-serdat-transmit test`; independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) `E6`: `make -C hrm/ch8-serial-interface test`; an expected red exits nonzero |
| **Assertions** | 2 red. `ALLZERO-NO-TX`: expected no transmission after SERDAT=`0x0000`, observed transmission started; the package reports `17 passed, 1 failed`. `E6`: expected `txd` to stay high over 4 bit-cells' worth of `clk7_en` beats, observed it low at least once; one of the package's `48 passed, 3 failed` |
| **Verdict of the four** | **3–1, with a split inside one of them.** vAmiga and Denise implement the sentence in one token; WinUAE implements it in its ordinary path and not in its `ser_accurate` one; Copperline writes an explicit branch to transmit anyway. Hardware: not measured |
| **Effort** | none; if an Amiga specialist accepts the guard, S: `paula_uart.v` only — a test of the written value ahead of the transmit load, the single added token vAmiga and Denise carry |

---

## Source requirement

**p.259:26-27**, the whole of it:

> *"Writing this register starts the data transmission. If this register is
> written with all zeros, no data transmission is initiated."*

The second sentence is the exception to the first: an all-zero SERDAT is how
software is told to *not* start a frame. The paragraph above it on the same page:

> The transmission of the start bit is independent of the contents of this register.

## RTL and measurement

`paula_uart.v` loads the transmit shift register on any write to `$030`; there
is no test of the written value anywhere in the transmit path. The write clears
`tx_tbe` (`:149`), and `TX_IDLE` loads `serdat` once `tx_tbe` is clear and
`tx_hold` is low (`:101-110`); `tx_hold`, the AT-modem bridge's FIFO-full
input, is the constant 0 of `:40-42` in both packages, which build the module
without `ENABLE_AT_MODEM`.

[`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) writes SERDAT
`0x0000` and watches `txd`: transmission started — `ALLZERO-NO-TX`, red.
[`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) reproduces it:
after `SERDAT=0x0000`, `txd` was observed to go low at least once over 4
bit-cells' worth of `clk7_en` beats — `E6`, red.

The control: the same fixture writing a non-zero word transmits correctly —
`WRITE-STARTS-TX`, green — and `STARTBIT-AUTO` grades the start bit's
appearance on its own printed sentence. So the red is not "this fixture cannot
see a transmission" but "this transmission should not have happened".

## Risks and unknowns

- Whether p.259 describes the silicon or prescribes an interface is the question
  for an Amiga hardware specialist; real Paula's reaction to a zero word is not
  measured, and no named software depends on it. The measurement is not in doubt.
- Two witnesses dissent: Copperline wrote a branch to transmit a zero word, and
  WinUAE implements the rule in its ordinary path and drops it in its
  `ser_accurate` one, the opposite of what one would expect if the rule were simply
  true of the silicon. NanoMig's behaviour matches those two. `paula.rs:1575` and
  `serial_win32.cpp:983` are the places to read first.
- The two p.259 passages give different outcomes for a zero word: `STARTBIT-AUTO` passes on the same all-zero write against "independent of the contents", while `ALLZERO-NO-TX` fails against "no data transmission is initiated"; the package grades each against its own sentence and does not adjudicate between them.

## Independent evidence

| | |
|---|---|
| **WinUAE** | **implements it in one of its two paths.** [`od-win32/serial_win32.cpp:999-1004`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L999-L1004) returns early on `if (!w)`, with a log line that calls the case out by name: *"zero serial word written?!"*. In `ser_accurate` mode ([`:983-992`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L983-L992)), which cycle-exact memory or a 68020 or lower in compatible mode at `m68k_speed == 0` selects ([`:1388`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L1388)), there is no such guard and the word is loaded unconditionally |
| **vAmiga** | **implements it.** `if (transmitShiftReg == 0 && transmitBuffer != 0)` — [`Core/Components/Paula/UART/UART.cpp:137`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/UART/UART.cpp#L137). The `!= 0` is the page's sentence |
| **Denise** | **implements it.** `if (value && !serShifter)` — [`emulation/libami/paula/uart.cpp:42`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/uart.cpp#L42). The same guard, one token |
| **Copperline** | **deliberately does not.** `write_serdat` loads any word ([`src/chipset/paula.rs:1079-1082`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1079-L1082)), and `serial_tx_total_bits` carries an explicit branch for it: `if word == 0 { return if long { 11 } else { 10 }; }` ([`:1575-1578`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1575-L1578)) — a full frame, written on purpose |
| **MiSTer** | descendant — [`rtl/paula_uart.v:77-140`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L77-L140) is NanoMig's transmitter without its two `tx_hold` terms, ten lines earlier, with no test of the written value anywhere in the transmit path |
| **TC64** | descendant — MiSTer's file line for line, all 262 lines; NanoMig's 276-line file differs from both by 18 lines: the AT-modem bridge's `serper_o` output and `tx_hold` input under `ENABLE_AT_MODEM`, the constant `tx_hold` without it, and the two terms that test it |

## Decision

Should `paula_uart.v` start no transmission on a SERDAT write of `0x0000`? No
change is proposed now. The page closes when `ALLZERO-NO-TX` and `E6` turn green,
or when an Amiga specialist rejects the guard; if the rejection means p.259 does
not describe the silicon, the page is refiled against the printed page.
