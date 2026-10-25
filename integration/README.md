# Integration packages

Tests of the whole system rather than one module: `nanomig.v` running a 68000
program assembled by vasm, on both CPU cores NanoMig offers (fx68k and TG68K),
graded at `nanomig.v`'s own ports. They cover the claims of the manual and the
AA specification that only show across module boundaries: a Copper list the
CPU starts, an audio channel another channel modulates, a CIA timer the system
clocks.

Run one package from the repository root, with Verilator and vasm
(`vasmm68k_mot`) on the PATH:

```sh
make -k -C integration/ch8-light-pen-beam test   # both cores; -k runs the second after a red
```

One package builds `nanomig.v` once per CPU core and can take a quarter of an hour
or more (measured: 18 minutes for `appK-autoconfig`, 12 minutes for
`aa-integ-s7-rest`).

A package that carries `*.patch` files also has a `<copy>-test` target
(`<copy>-test-fx68k` and `<copy>-test-tg68k` for one core). It builds and runs the
package against a private copy of the chip set with that patch applied; the
submodule is never modified. A finding page whose suggested change has a patch
names the target in its Test row, for example
`make -k -C integration/ch8-light-pen-beam latch-test`, which runs NM-HRM-087's
suggested latch. The package's `Makefile` lists its copies.

Each core writes `out_fx68k.txt` or `out_tg68k.txt` in the package directory,
in the line format the root [`README.md`](../README.md#where-the-suite-stands)
describes. `./run_all.sh --jobs 8` runs every package and prints one line per
package and core.

## Mutation builds

A package whose test source records mutations (`NMH_MUTATE=<selector>` in its comments) runs one with `NMH_MUTATE=<selector> make -C integration/<package> test-fx68k` (or `test-tg68k`): a mutation build, not run by `make test`. For example, `NMH_MUTATE=C11 make -C integration/ch4-shres-bitplane test-fx68k` prints `86 passed, 49 failed` and exits nonzero, as the package's record for `C11` says.

## Graded against the manual

| Package | What it grades |
|---|---|
| `ch1-cpu-system` | the 68000 in the whole system (chapter 1) |
| `ch2-copper-move` | the Copper MOVE, the Copper location registers and the restart of pointers from them |
| `ch2-copper-control` | Copper DMA on and off, the start after reset, the restart from COP1LC, the Copper interrupt, the masked WAIT and the blitter-finished wait |
| `ch2-copper-beam` | the beam positions the Copper sees and the line and field geometry (pp. 22-24, 34) |
| `ch2-dma-cycles` | which memory cycles each DMA channel uses, and that these channels never delay the Copper (p. 20, Figure 6-9) |
| `ch2-interlace-fields` | interlaced fields (pp. 24, 27, 34) |
| `ch3-playfield-system` | the playfield system bits COLOR_ON, LPEN and VBSTRT (pp. 79, 89, 229, 239, 271, 304-305) |
| `ch4-shres-bitplane` | the SuperHires bitplane colours (p. 301) |
| `ch4-shres-sprite-a`, `ch4-shres-sprite-b` | the SuperHires sprite colours (p. 302) |
| `ch4-sprite-registers` | the sprite registers, manual and automatic sprite DMA, and the register-summary strobe (pp. 25-26, 104-110, 287) |
| `ch5-audio-attach-modulation` | one audio channel modulating another through ADKCON's attach bits |
| `ch5-audio-output-scanrate` | audio byte output, ADKCON's disk-rate bits, AUDxPER and the scan rate |
| `ch5-audio-registers-dma` | the audio location registers, DMA control and the Copper danger bit |
| `ch6-blitter-line-fill` | line mode and area fill (pp. 180-192, 200, 268) |
| `ch6-blitter-geometry` | the blitter's addressing and masking: masks, modulos, shifts, pointer order, minterms (pp. 169-183, 200) |
| `ch6-blitter-cpu-sharing` | how the blitter and the 68000 share the chip bus, and the done flag (pp. 169-198) |
| `ch6-dma-priority-load` | DMA priority under load: which requests delay the Copper, and which keep every slot while the blitter, the 68000 or the Copper runs (pp. 19-20, 194-198, 275) |
| `ch7-system-control` | the light-pen read at the beam-position address and LPENDIS (pp. 216, 239) |
| `ch8-controller-ports` | the controller ports (chapter 8 and appendix A) |
| `ch8-disk-serial` | serial-port timing, nine-bit receive and the disk bit-cell rate |
| `ch8-ecs-sync-registers` | the ECS registers HTOTAL, HSSTRT and HSSTOP |
| `ch8-light-pen-beam` | the light-pen latch and the beam position (pp. 227-229, 238-239) |
| `appDE-ports-disk` | the pen-pressed button on the game port (appendix E, p. 327) |
| `appF-cia-system` | the two 8520 CIAs as the system wires them (appendix F) |
| `appHJ-drive-pins` | the external disk interface and the custom chips' pin directions (appendices H and J) |
| `appK-autoconfig` | the RESET instruction and AUTOCONFIG base-address assignment (appendix K) |

## Graded against the AA specification

| Package | What it grades |
|---|---|
| `aa-integ-s1` | BPLCON1 horizontal scroll, BYPASS, the FMODE fetch increment, bit-plane modulos, LPEN after reset, the UART break, the disk DMA pointer and the DUAL-mode beam counter (AA pp. 16-17, 23-26) |
| `aa-integ-s2` | the Blitter pointer and modulo registers and BEAMCON0's light-pen, sync, DUAL and PAL bits (AA pp. 6-7, 12-13, 24) |
| `aa-integ-s3` | the bit-plane and sprite pointers and data, sprite position and control, the UHRES sprite window and collision control (AA pp. 15, 18, 28) |
| `aa-integ-s4` | the bit-plane fetch window, the display window with DIWHIGH, and the DMACON enable bits (AA pp. 21-22) |
| `aa-integ-s5` | the audio data, length, volume and attach bits, and the serial data registers (AA pp. 11, 27) |
| `aa-integ-s6` | sprite resolution modes, sprite widths, attachment, colour banks and their reset values (AA pp. 16-18, 24, 31) |
| `aa-integ-s7` | which registers the Copper may write, and what DMA does with the row-function and address-pair registers (AA pp. 6, 11, 14, 23) |
| `aa-integ-s7-rest` | the UHRES registers, the DUAL-mode beam counter and REFPTR (AA pp. 6, 15, 24, 27) |

Several packages are built in part. Their `SPECIFICATION.md` names what is
not built, by a scope decision, and what building it would take; `run_all.sh`
grades only what is built.

Each package's `SPECIFICATION.md` was written from the manual's claims alone,
by an author who never saw the RTL or a test, and was committed before any
test code. Every expected value comes from a printed page, and a package's
labels or its specification name that page.

**A red here is a finding only once a validation has confirmed it.** An
independent validator checks every expected value against the page; the reds
it confirms as the RTL disagreeing with the page are named, line by line, on a
finding page and in [`docs/findings.md`](../docs/findings.md). A red that no
finding names is a test still under validation or repair, and says only that
the test and the RTL disagree. The three chapter-6 packages were checked until
nothing was left.

## Not graded against a page

A red in these is not a finding.

- `apparatus-cpu-to-video` and `apparatus-vasm-program` test the harness, not
  the chipset. They check values the test program itself writes: a word stored
  and read back, a colour written and seen on the video port. Each has a
  control that must fail, and the package fails if the control passes.
- `minimig_system` is a smoke test of register access and one Copper move
  through the CPU bus. Its expected values rest partly on the manual's register
  addresses and set/clear semantics and partly on NanoMig's own bus interface,
  so it is not independent of the implementation. It catches breakage, for
  example after an upstream change.
- `harness/` holds what every package shares: `nanomig.mk`, the build for both
  cores; `nanomig_harness.h`, the C++ side; `nanomig.i`, the 68000 side.
  The build keeps the serial port on the `uart_rx`/`uart_tx` pins, where the
  manual describes it. The Tang Nano 20K board files at the pinned revision
  set `ENABLE_AT_MODEM`, which routes the port to the FPGA Companion's virtual
  AT modem instead; that interface is not tested here (`nanomig.mk` names the
  defines it leaves out and why).
