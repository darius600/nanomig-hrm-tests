# NM-HRM-072 — a stop bit with a word waiting behind it lasts N+1½ colour clocks, where p. 255 gives every transmitted bit N+1 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_uart.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L111-L117) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L111-L117) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**. NanoMig has `paula_uart.v` from there unchanged and TC64 carries the same file, so there is no fix to port; vAmiga, Denise and Copperline each show where the waiting word's start bit goes — see *What the others say* |
| **Test** | [`integration/ch8-disk-serial`](../../integration/ch8-disk-serial) — three assertions over the same transmit programs, run on both CPU cores, each grading p. 255's sentence "If the SERPER divisor is set to the number N, then N+1 color clocks occur ... between transmissions of output bits" at the N it samples: `P1`, NTSC; `P2`, PAL, red through its clause (ii); `P3`, NTSC at N=371. Each is red on fx68k and on TG68K. The divisor-formula claims `sol:8-serper.c8`, `c9` and `c10` are on `RECORDED` lines beside them: three sampled N cannot establish a formula for every N, and the red at N=371 is their counterexample |
| **Chapter** | 8 — Interface Hardware, printed pp. 255, 257, 258, 259; Appendix A, printed pp. 283, 284 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 3 red on fx68k and 3 on TG68K (`P1`, `P2`, `P3`; 74 passed, 3 failed on each core, and these three are the package's only reds). In the same run the single-word captures at N=0 and N=32396 land on `P1`'s grid 10 of 10 each, and `1d`/`2d`, green, show that every word after the first was written before the previous word's stop bit began |
| **Verdict of the four** | **unanimous against NanoMig**: vAmiga, Denise and Copperline load a waiting word in the step that ends the stop bit, so its start bit follows the stop bit after N+1 colour clocks, as p. 255 says. WinUAE has no TxD pin; in its cycle-exact mode with no serial device it starts the waiting word's frame 10(N+1) colour clocks after the previous frame began, and in its loopback mode 3 colour clocks later. None adds half a colour clock |

---

## What the HRM says

**Printed p. 255**, *Setting the Baud Rate*, gives every transmitted bit the
same length:

> All timing is done on the basis of a "color clock," which is 279.36ns long on NTSC machines and 281.94ns on PAL machines. If the SERPER divisor is set to the number N, then N+1 color clocks occur between samples of the state of the input pin (for receive) or between transmissions of output bits (for transmit).

**Printed p. 284**, SERPER in Appendix A, says the same without an exception:

> If this number is N, then the baud rate is 1 bit every (N+1)*.2794 microseconds.

**Printed pp. 258 and 259** say what is shifted out, and that stop bits are
bits like the others:

> Data will be moved out of the shift register, one bit during each time interval, starting with the least significant bit. The shifting continues until all 1 bits have been shifted out. Any number or combination of data and stop bits may be specified this way.

> The data to be transmitted is placed in the output register ( SERDAT ). Above the data bits, 1 bits must be added as stop bits. Normally, either one or two stop bits are sent.

> The transmission of the start bit is independent of the contents of this register. One start bit is automatically generated before the first data bit (bit 0) is sent.

**The reload of the shift register is described three times, and none of the
three gives it a time of its own.** p. 259:

> The register stops shifting and signals "shift register empty" (TSRE) when there is a 1 bit present in the bit-shifted-out position and the rest of the contents of the shift register are 0s. When new nonzero contents are loaded into this register, shifting begins again.

Appendix A's SERDAT entry, **printed p. 283**:

> Data from this buffer is moved into a serial shift register for output transmission whenever it is empty.

and p. 257's TSRE row names the case measured here, a word placed into
SERDAT while another is still being shifted:

> If you stop writing data into the output register ( SERDAT ), then this bit will become a 1 after both the word currently in the shift register and the word placed into SERDAT have been transmitted.

Figure 8-11 on p. 258, *read from the drawing, not quoted*, draws the
*"Starting Appearance of SERDAT and Shift Register"*: fifteen 0s under the
bit numbers 15 to 0, labelled *"All zeros from last shift"*, and a separate cell to its
right labelled *"one bit"* holding a 1. The register is empty while the last 1
is still in the bit-shifted-out position; the figure draws no step between that
state and the next load.

**The one phrase that allows a longer stop is about the receiver.** p. 255,
*Setting the Receive Mode*:

> In either case, the receive circuitry expects to see one start bit, eight or nine data bits, and at least one stop bit.

It says what the receiver accepts. For the transmitter, p. 259 makes the
number of stop bits the number of 1 bits written above the data, and p. 255
gives each of them N+1 colour clocks. **No page gives a stop bit N+1½ colour
clocks, with or without a word waiting behind it.**

---

## What NanoMig does

**The transmitter spends one `clk7_en` period in `TX_IDLE` between the end of
a stop bit and the start bit of a word that is already waiting.** The count of
the stop bit ends like every other bit's; when the shift register is empty the
state machine goes to `TX_IDLE` instead of loading the waiting word —
[`src/minimig-aga/paula_uart.v#L111-L117`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L111-L117):

```verilog
if (tx_cnt == 16'd0) begin
  // check if any bit left to send out
  if (tx_shift == 16'd0) begin
    // set TSRE flag when serdat register is empty
    if (tx_tbe) tx_tsre <= #1 1'b1;
    // data sent, go to idle state
    tx_state <= #1 TX_IDLE;
```

and only `TX_IDLE`, on the next `clk7_en` edge, loads the word and drives its
start bit —
[`#L87-L104`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L87-L104):

```verilog
TX_IDLE : begin
  // txd pin inactive in idle state
  tx_txd <= #1 1'b1;
  // check if new data loaded to serdat register
  if (!tx_tbe) begin
  ...
    // generate start bit
    tx_txd <= #1 1'b0;
  ...
    tx_shift <= #1 serdat;
    // reload period register
    tx_cnt <= #1 {serper[14:0], 1'b1};
```

Every other bit is counted as
[`#L118-L124`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v#L118-L124)
does it: reload `tx_cnt <= #1 {serper[14:0], 1'b1};`, count down to 0, and
drive the next bit, `tx_txd <= #1 tx_shift[0];`, on the edge that finds 0. That
is 2N+1 edges counting down and one more that changes the pin, 2N+2 `clk7_en`
edges. The module runs on `clk7_en`
([`src/minimig-aga/paula.v#L203-L206`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L203-L206),
`.clk7_en (clk7_en),`), which comes once in every four `clk_28` cycles, while
the colour clock `cck` toggles once in every four —
[`src/minimig-aga/amiga_clk.v#L31-L35`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/amiga_clk.v#L31-L35),
`clk7_en_reg  <= (clk7_cnt == 2'b00);` and `cck <= ~cck;` — so two `clk7_en`
edges are one colour clock and a bit lasts N+1 colour clocks, as p. 255 says.
The stop bit of a word with another waiting behind it lasts those 2N+2 edges
and the `TX_IDLE` edge after them: 2N+3, which is **N+1½ colour clocks**.

### Measured

[`integration/ch8-disk-serial`](../../integration/ch8-disk-serial) writes
SERPER = N, then SERDAT = $0155 — a start bit, the data bits 1,0,1,0,1,0,1,0
and one stop bit, so every bit boundary is a change of TxD — and, for W words,
polls TBE and writes the next word as soon as the previous one has moved into
the shift register. It logs every change of `uart_tx` at `nanomig.v`'s ports in
`clk_sys` cycles and measures the colour clock C from the four refresh
accesses of each line: C = 8 cycles on every capture (`1a`, `2a`, green, each
with a mutation that goes red). The committed output, identical on fx68k and
TG68K:

| assertion | capture | what is compared | measured |
|---|---|---|---|
| `P1` | NTSC, N=0, 371 and 32396 | every change within ±C/2 of t0 + k·(N+1)·C | N=0 10 of 10, **N=371 20 of 40**, N=32396 10 of 10 |
| `P2`, clause (ii) | PAL, the same three N | U_N = T_N/(N+1), each run's mean bit interval T_N divided by N+1, equal for every N | U_0 = U_32396 = 8.000000, **U_371 = 8.000827**; the difference exceeds its tolerance of 0.000579 |
| `P3` | NTSC, N=371 | the same grid as `P1`, at 372·C | **20 of 40** |

On PAL the N=371 run spans 39 intervals in 116076 cycles, 12 more than
39 × 2976. On `P1`'s grid the changes of word 2 lie exactly at +C/2, which the
comparison still counts as on the grid, and those of words 3 and 4 at +C and
+3C/2.

**Where the 12 cycles are.** Read with the package's own capture loop and its
own transmit programs, assembled to the same bytes as the package's
`prog_tx_n0.bin`, `prog_tx_n371.bin` and `prog_tx_n32396.bin` — a measurement
for this page, not a graded assertion — every interval between two changes of
TxD, on both cores, identical on fx68k and TG68K:

| run | intervals of N+1 colour clocks | stop bits with a word waiting behind them |
|---|---|---|
| N=371, 4 words, NTSC and PAL | 36 of 39, 2976 cycles = 372·C | **3 of 39, 2980 cycles = 372½·C** |
| N=31, 4 words, NTSC | 36 of 39, 256 cycles = 32·C | **3 of 39, 260 cycles = 32½·C** |
| N=32396, 2 words, NTSC | 18 of 19, 259176 cycles = 32397·C | **1 of 19, 259180 cycles = 32397½·C** |
| N=0 and N=32396, 1 word, NTSC | 9 of 9 | none |

The N=31 and two-word N=32396 runs are variants of the package's program with
those values, the second with a TBE poll bound that covers one word time. The
extra 4 cycles are one `clk7_en` period, C/2, at every N. In each run every
word after the first was written before the previous word's stop bit began:
the word waited in SERDAT, the case p. 257's TSRE row describes.

### How large, and whether it accumulates

**Half a colour clock per word boundary, whatever N is**: 139.7 ns on NTSC and
141.0 ns on PAL, by p. 255's colour-clock lengths. It is 1/(2(N+1)) of one bit,
0.13 % at N=371 and 1.6 % at N=31.

**It does not accumulate within a word and does accumulate across words.** Each
word's bits lie on N+1-colour-clock steps from that word's own start bit. Each
waiting word's start bit comes half a colour clock after the step p. 255 gives
it, so word k of a back-to-back run starts (k−1)/2 colour clocks after its place on
the grid counted from word 1's start bit: +½, +1 and +1½ colour clocks for
words 2, 3 and 4 above. A continuous run sends one word every 10(N+1)+½
colour clocks instead of 10(N+1).

---

## What the others say

The question put to each: when a word is waiting in SERDAT, how long after the
stop bit begins does its start bit go out?

| | code | |
|---|---|---|
| **WinUAE** | [`od-win32/serial_win32.cpp:773-795`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L773-L795), [`:702-708`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L702-L708), [`:797-815`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L797-L815), [`:1374-1388`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/serial_win32.cpp#L1374-L1388), [`custom.cpp:649`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L649) | <pre>int per = sper * (serdatshift_bits + 1);<br>...<br>if (serempty_enabled &amp;&amp; !serxdevice_enabled) {<br>&nbsp;&nbsp;&nbsp;&nbsp;event2_newevent_xx(-1, per * CYCLE_UNIT, 0, sersend_end);</pre> **no TxD pin; the frame is timed, and the next one starts where it ends.** In its cycle-exact mode, `ser_accurate = currprefs.cpu_memory_cycle_exact \|\| (currprefs.cpu_model <= 68020 && currprefs.cpu_compatible && currprefs.m68k_speed == 0);`, `serdatcopy` counts the bits up to the highest 1, the stop bit, adds one for the start bit and schedules the frame's end (N+1)·(bits+1) `CYCLE_UNIT`s later, under the comment *"if someone uses serial port as some kind of timer.."*; a `CYCLE_UNIT` is a colour clock, `#define HSYNCTIME (maxhpos * CYCLE_UNIT)`. At that event `sersend_end` runs `data_in_sershift = 0;` and `serdatcopy();`, which moves the waiting word into the shift register at once, so its frame begins 10(N+1) colour clocks after the previous frame began. That is the path with no serial device and no MIDI, `serempty_enabled = true;`. In loopback mode the end is carried behind the modelled receiver, `recper += 3;` under *"TX to RX delay (1488/1489 chip delays)"*, and with SERPER's LONG bit clear and one stop bit the waiting word starts 3 colour clocks later than that |
| **vAmiga** | [`Core/Components/Paula/UART/UARTEvents.cpp:27-65`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/UART/UARTEvents.cpp#L27-L65), [`Core/Components/Paula/UART/UART.cpp:176-181`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/UART/UART.cpp#L176-L181), [`Core/Components/Paula/UART/UART.h:164`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/UART/UART.h#L164) | <pre>transmitShiftReg &gt;&gt;= 1;<br><br>if (!transmitShiftReg &amp;&amp; transmitBuffer) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;copyToTransmitShiftRegister();</pre> **the start bit goes out in the event that shifts the stop bit out.** Each `TXD_BIT` event shifts one bit and schedules the next one `pulseWidth()` later, `agnus.scheduleRel<SLOT_TXD>(pulseWidth(), TXD_BIT);`, where `pulseWidth()` is `DMA_CYCLES((serper & 0x7FFF) + 1)`, N+1 DMA cycles. When the shift leaves the register empty and a word is waiting, the same event copies it in with its start bit prefixed, `transmitShiftReg <<= 1;`, and drives it, `outBit = transmitShiftReg & 1;`. The stop bit lasts one `pulseWidth()` |
| **Denise** | [`emulation/libami/paula/uart.cpp:55-70`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/uart.cpp#L55-L70), [`:107-117`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/uart.cpp#L107-L117), [`:2`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/uart.cpp#L2), [`emulation/libami/agnus/agnus.cpp:495-500`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L495-L500), [`:626`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L626) | <pre>serShifter &gt;&gt;= 1;<br><br>if (!serShifter) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (serDat) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;prepareTransfer();</pre> **the same: the stop bit's event loads the waiting word.** `prepareTransfer` moves `serDat` into the shifter with a start bit, `serShifter <<= 1; // one start bit is mandatory`, `updateTxd()` drives it, and the next event is set `serialTransferEvent = agnus.clock + PULSE_WIDTH;` with `#define PULSE_WIDTH ((serPer & 0x7fff) + 1)`. `agnus.clock` counts colour clocks: `dmaCycle()` advances it once, `if (++clock == nextClock)`, as it advances the beam by one, `switch(++hPos) {`. The stop bit lasts N+1 colour clocks |
| **Copperline** | [`src/chipset/paula.rs:1372-1390`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1372-L1390), [`:1520-1541`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1520-L1541), [`:1335-1338`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1335-L1338), [`:1567-1569`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1567-L1569) | <pre>shift.bit_index += 1;<br>if shift.bit_index &gt;= shift.total_bits {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;self.serial_tx_pin_high = true;<br>&nbsp;&nbsp;&nbsp;&nbsp;irq \|= self.load_serial_shift_if_idle();</pre> **the same, within one colour-clock step.** `tick_serial` advances the port *"by `cck` color clocks"*; each bit holds `remaining_cck: bit_cck`, with `u32::from(self.serper & 0x7FFF).saturating_add(1).max(1)`. When the stop bit's count ends, `load_serial_shift_if_idle` takes the waiting word from `serial_tx_buffer`, starts it at `bit_index: 0`, the start bit, and drives the pin from it, `self.serial_tx_pin_high = Self::serial_tx_bit(&shift);`; the remainder of the same span then counts the start bit. The stop bit lasts N+1 colour clocks |
| **MiSTer** | [`rtl/paula_uart.v:111-117`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L111-L117), [`:87-104`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_uart.v#L87-L104) | descendant — identical to NanoMig, same lines: the stop bit's count ends in `tx_state <= #1 TX_IDLE;` and the waiting word is loaded from `TX_IDLE` one `clk7_en` edge later, `if (!tx_tbe) begin` … `tx_shift <= #1 serdat;` |
| **TC64** | [`rtl/minimig/paula_uart.v:111-117`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_uart.v#L111-L117), [`:87-104`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_uart.v#L87-L104) | descendant — the same file, line for line: `tx_state <= #1 TX_IDLE;` after the stop bit, `tx_txd <= #1 1'b0;` from `TX_IDLE` on the next edge. `paula_uart.v` is identical in all three trees, so the extra edge is inherited by both lines and fixed in neither |

**None of the four spends time between a stop bit and a waiting word's start
bit.** vAmiga, Denise and Copperline load the waiting word in the same step
that ends the stop bit's N+1 colour clocks and drive its start bit there;
WinUAE, which has no pin, begins the next frame at the event that ends the
previous one in its cycle-exact mode, and is 3 colour clocks late only in its
loopback mode, for a receive delay it models there. **Nobody adds the half
colour clock NanoMig adds**, and nobody's reload has a duration of its own.

---

## Suggested change

**One option: load a waiting word in the edge that ends the stop bit, as
`TX_IDLE` does.** In the `tx_shift == 16'd0` branch of lines 111–117:

```verilog
if (tx_shift == 16'd0) begin
  if (!tx_tbe) begin
    // a word is waiting: its start bit follows the stop bit
    tx_irq   <= #1 1'b1;
    tx_txd   <= #1 1'b0;
    tx_shift <= #1 serdat;
    tx_cnt   <= #1 {serper[14:0], 1'b1};
  end else begin
    tx_tsre  <= #1 1'b1;
    tx_state <= #1 TX_IDLE;
  end
end
```

and line 139 has to set `tx_tbe` on that load as it does in `TX_IDLE`, since the
state no longer passes through it. This has not been run here. What it would
change:

- a stop bit with a word waiting behind it would last 2N+2 `clk7_en` edges,
  N+1 colour clocks, and `P1`, `P2` and `P3` would read the grid p. 255
  gives;
- the waiting word's transfer into the shift register and its TBE interrupt
  would come one `clk7_en` edge, half a colour clock, earlier;
- nothing would change for a word written with the transmitter idle, the
  window [NM-HRM-056](withdrawn/NM-HRM-056.md) reads, or for TSRE after the last word.

---

## What holds the finding up

- **The comparator is exact where the page predicts exactly.** The single-word
  captures at N=0 and N=32396 land on `P1`'s grid 10 of 10 each, and on PAL
  their mean bit intervals divided by N+1 are 8.000000 cycles, one colour
  clock, in `P2`'s clause (ii); the within-word intervals of the four-word run are all N+1 colour
  clocks.
- **The words are waiting.** `1d` and `2d`, green on both cores, show every
  word after the first written before the previous word's stop bit began.
- **The unit is the page's.** C is measured on each capture from the refresh
  slots (`1a`, `2a`, green, with mutations that go red), and each capture's
  own field shows the standard it was run in (`1b`, `2b`).
- **It is Paula's, not the CPU's.** Every change of TxD lies at the same
  offset from the first start bit on fx68k and on TG68K, although the two
  cores write the words at different times, and the extra time is C/2 at
  N=31, 371 and 32396.
- **What weakens it is its size.** Half a colour clock per word is below the
  page's unit for one interval: a single stop bit measured to the nearest
  colour clock reads either N+1 or N+2. `P1` and `P3` see it only because it
  adds up over words 3 and 4, and word 2 lies exactly on their ±C/2
  tolerance. No page prints a tolerance for a bit interval, and the receiver
  p. 255 describes accepts a stop of one bit or more.
- **The unit package does not grade it.**
  [`hrm/ch8-serial-interface`](../../hrm/ch8-serial-interface)'s `D4` times two
  back-to-back words in whole bit cells, 19 to 23 for the 21 it expects, and is
  green; its own comment names the extra idle-to-shift step, which that
  tolerance absorbs.

---

## For the specialists

1. **Does Paula spend time between a stop bit and a waiting word's start
   bit?** p. 255 gives every transmitted bit N+1 colour clocks, and pp. 258,
   259 and 283 give the reload none; Figure 8-11 draws the register empty
   while the last 1 is still in the bit-shifted-out position. Two words written
   back to back on real hardware, the time between their start edges timed on
   TxD — 10(N+1) colour clocks, or half a colour clock more — would settle it.
2. **What the half colour clock changes.** A receiver that resynchronises on
   each start bit sees only a stop bit 1/(2(N+1)) of a bit longer, which the
   *"at least one stop bit"* of p. 255 accepts. What it changes is the rate of
   a continuous run and anything that times the line, the use WinUAE's comment
   above its frame timing names.
