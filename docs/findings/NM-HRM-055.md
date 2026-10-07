# NM-HRM-055 — SERDAT written with all zeros still transmits [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `paula_uart.v` start no transmission when SERDAT is written with `0x0000`, as p.259 prints, where it now drives one low cell (a start bit) on `txd`? The decision belongs to an Amiga specialist, not to this repository. It closes when the specialist accepts or declines a guard on the written value and, if accepted, `ALLZERO-NO-TX` and `E6` are green, or when the specialist rules that p.259 does not describe the silicon and the reason is recorded here |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer. Its `rtl/paula_uart.v` is identical to NanoMig's, and TC64's is too, so there is no fix to port; the gap sits in the Minimig line, not in NanoMig alone |
| **Scope** | Paula's SERDAT (`$030`) written with the all-zero word `0x0000`; the transmit path only. Chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.259 (SERDAT, lines 26-27) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `ALLZERO-NO-TX`: `sol:8-serdat.c37`, `opus:8-serdat.c9`, `sol:8-serdat.c46`, p.259 (its message also cites p.283); `E6`: `opus:8-serial.c140`, p.259; control `WRITE-STARTS-TX`: `sol:8-serdat.c36`, p.259; control `STARTBIT-AUTO`: `sol:8-serdat.c34`, `sol:8-serdat.c35`, `sol:8-serdat.c33`, p.259 |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L77-L140) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L77-L140); inherited |
| **Test** | [`hrm/ch8-serdat-transmit`](../../hrm/ch8-serdat-transmit) `ALLZERO-NO-TX`: `make -C hrm/ch8-serdat-transmit test`; independently [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface) `E6`: `make -C hrm/ch8-serial-interface test`; an expected red exits nonzero |
| **Assertions** | 2 red. `ALLZERO-NO-TX`: expected no transmission after SERDAT=`0x0000`, observed transmission started; the package reports `17 passed, 1 failed`. `E6`: expected `txd` to stay high over 4 bit-cells' worth of `clk7_en` beats, observed it low at least once; one of the package's `48 passed, 3 failed` |
| **Verdict of the four** | **3–1, with a split inside one of them.** vAmiga and Denise implement the sentence in one token; WinUAE implements it in its ordinary path and not in its `ser_accurate` one; Copperline writes an explicit branch to transmit anyway. Hardware status not measured |
| **Effort** | S: `paula_uart.v` only — a test of the written value ahead of the transmit load, the single added token vAmiga and Denise carry |

---

## Source requirement

**p.259:26-27**, the whole of it:

> *"Writing this register starts the data transmission. If this register is
> written with all zeros, no data transmission is initiated."*

The second sentence is the exception to the first: an all-zero SERDAT is how
software is told to *not* start a frame.

## RTL and measurement

`paula_uart.v` loads the transmit shift register on any write to `$030`; there
is no test of the written value anywhere in the transmit path.

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

## Independent evidence

| | |
|---|---|
| **WinUAE** | **implements it in one of its two paths.** `od-win32/serial_win32.cpp:999-1004` returns early on `if (!w)`, with a log line that calls the case out by name: *"zero serial word written?!"*. In `ser_accurate` mode (`:983-992`), which cycle-exact memory or a 68020 or lower in compatible mode at `m68k_speed == 0` selects (`:1388`), there is no such guard and the word is loaded unconditionally |
| **vAmiga** | **implements it.** `if (transmitShiftReg == 0 && transmitBuffer != 0)` — `Core/Components/Paula/UART/UART.cpp:137`. The `!= 0` is the page's sentence |
| **Denise** | **implements it.** `if (value && !serShifter)` — `emulation/libami/paula/uart.cpp:42`. The same guard, one token |
| **Copperline** | **deliberately does not.** `write_serdat` loads any word (`src/chipset/paula.rs:1079-1082`), and `serial_tx_total_bits` carries an explicit branch for it: `if word == 0 { return if long { 11 } else { 10 }; }` (`:1575-1578`) — a full frame, written on purpose |
| **MiSTer** | descendant — `rtl/paula_uart.v:77-140` is identical to NanoMig's, with no test of the written value anywhere in the transmit path |
| **TC64** | descendant — identical too, line numbers included; the whole 262-line file matches NanoMig line for line, confirmed by a full-file comparison |

## Decision

Should `paula_uart.v` start no transmission on a SERDAT write of `0x0000`, with
`ALLZERO-NO-TX` and `E6` as the green ids that close it? The decision belongs to
an Amiga specialist. Re-run: `make -C hrm/ch8-serdat-transmit test` and
`make -C hrm/ch8-serial-interface test`.
