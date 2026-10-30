# apparatus-cpu-to-video

Apparatus check for `integration/harness/nanomig_harness.h`: proves the
harness can drive `nanomig/src/nanomig.v` (the NanoMig system top) out of
reset, place a small 68000 program where the design's own reset-vector
fetch finds it, observe the CPU executing that program through the RAM
port alone, and observe the program's effect on the video output.

No check in this package is an HRM conformance claim. Every PASS/FAIL line
is printed as `[not graded -- apparatus check, no claim uid]`, and the
MEASURED lines are diagnostic measurements, not assertions against a
reference value.

## The two builds

Both builds compile `nanomig/src/nanomig.v` plus the minimig-aga chipset
files listed in `nanomig/sim/Makefile`'s `MINIMIG_FILES`, minus the files
that exist only for that upstream testbench (`nanomig_tb.v`/`.cpp`, and
the `src/misc` sd card/sdram models -- this harness drives `sdc_*` and
`ram_address`/`ram_data` directly instead of through those models).

- **fx68k** -- the configuration `nanomig/src/tang/nano20k/top.sv` gives
  the design: it defines none of the CPU/cache macros, so
  `cpu_wrapper.v`'s own fallback (`ifndef ENABLE_TG68K / ifndef
  ENABLE_FX68K / define ENABLE_FX68K`) selects the fx68k core. Simulating
  the unmodified `fx68k.sv`/`fx68kAlu.sv`/`uaddrPla.sv` needs two
  Verilator options this build adds: `-Wno-BLKANDNBLK` (without it,
  `%Error-BLKANDNBLK` at `fx68k.sv:293`) and `--no-assert` (without it,
  the `unique case` statements in `fx68k.sv`/`fx68kAlu.sv` stop the run at
  time 0 -- upstream's own `nanomig/sim/fx68k_sim.patch` works around the
  same problem by commenting out the `unique` keyword in a patched copy;
  this build uses the Verilator flag against the unpatched source
  instead). fx68k also `$readmemh`s `microrom.mem`/`nanorom.mem` from the
  current working directory, so the Makefile copies them from
  `nanomig/src/fx68k/` into the object directory and runs the binary
  there.
- **tg68k** -- the configuration `nanomig/src/tang/nano20k/top_020.sv`
  gives the design: `ENABLE_TG68K ENABLE_AGA CPU_SLOW14 ENABLE_CACHE
  CHIPRAM_CACHE NO_WS2812 DENISE_EBR`. The TG68K core itself is the kernel
  the boards synthesize, `nanomig/src/tg68k/*.vhd`, translated by GHDL with
  the generics of the board instance (`cpu_wrapper.v:405-417`); the
  Verilator instance (`cpu_wrapper.v:420-441`) names that translation. The
  build compiles a copy of `cpu_wrapper.v` that also connects the kernel's
  skipFetch output (`.skipFetch(skip_fetch_i)`, as the board instance does
  at `:464`), so that `cpu_req` (`:101`) drops the bus request on a skipped
  fetch. Both files are generated at build time
  (`integration/harness/tg68k.mk`). `TG68K_KERNEL=shipped` compiles instead
  `nanomig/sim/TG68KdotC_Kernel.v`, a Verilog netlist of an older kernel
  revision, with the unmodified `cpu_wrapper.v`, whose Verilator instance
  leaves skipFetch unconnected.

Both builds also pass `-DINFER_DPRAM`. It is a **simulation-only**
option: only `minimig-aga/ide.v` reads it, to replace an FPGA-specific RAM
primitive with inferred RAM so the design elaborates under Verilator. It
is not part of either shipped configuration and is not one of the defines
either top file sets.

`memory_config` selects 2.0 MB chip RAM (`chipram_config==2'b11`), which
the harness relies on for its data area (see below); the harness's other
configuration inputs (`fastram_config`, `chipset_config`, `floppy_config`,
`video_config`, `ide_config`) stay at their all-zero defaults -- OCS
chipset, no fast RAM, no floppy or IDE drives, no video filters. See
`integration/harness/nanomig_harness.h`'s `Config` for the exact values
and the reasoning behind each.

`turbo_config` stays 0 (no turbo chip/kick, no data cache). With it 0,
`cpu_wrapper.v`'s `ramsel` (the signal that routes a CPU access to the
direct/cache/fastram path instead of the chip bus) never asserts
(`cpu_wrapper.v:150-168`), so every single CPU memory access in this
apparatus -- fetch or data, on either core -- goes through minimig's chip
bus and is visible on `ram_address`/`ram_data`. None of it uses
`fastram_*`, on either build.

## What the test does

1. Builds a program: it writes an immediate marker to one RAM word,
   copies that word to a second RAM word (`MOVE.W abs.L,abs.L`), writes a
   colour into `COLOR00` ($DFF180), and loops on itself (`BRA.S *`).
2. Places the reset vector (initial SSP, initial PC) and the program in
   the physical RAM location the design's overlay maps Kickstart to (see
   `nanomig_harness.h`'s address-space notes for the derivation, with
   file:line evidence), *not* a real Kickstart image -- the CPU's own
   reset-vector fetch, with the overlay active after reset (the design's
   own reset default), reads the test's SSP/PC directly.
3. Resets the design and polls `cpu_nrst_out` rather than assuming a
   fixed cycle count: the design's internal reset
   (`minimig_syscontrol.v`, gated on whole frames, not clk_sys cycles)
   also gates the CPU reset (`minimig.v:1050-1057`).
4. Polls the RAM-port access log until it shows the expected write (the
   copied marker landing at the second RAM word), proving the program
   ran, then keeps running until at least two more frames have completed.
5. Checks, from the RAM-port log alone:
   - the CPU fetched its initial SSP and PC from the overlay-mapped
     reset vector (the first four words read are exactly the four words
     the vectors were placed at);
   - the CPU then fetched the program's opcode words, in the order they
     were placed (a subsequence check against the full read log, so
     unrelated reads such as refresh cycles interleaved on the same port
     do not break it);
   - the CPU wrote its immediate operand to RAM;
   - the CPU read that value back and copied it to the second RAM word.
6. Checks, from the video output alone: at a deterministic sample point
   (the middle sample of the middle scan-doubler line of the last fully
   captured frame -- not a search for "the first non-black pixel", which
   would not find anything in the `MUTATE=nocolour` control run below),
   `r`,`g`,`b` equal the guns of the colour the program wrote.
7. Prints MEASURED (not graded) hs-to-hs and vs-to-vs periods; see below
   for why the hs period is not the Amiga's native line length.
8. Runs a control: with `MUTATE=nocolour` (read at run time via the
   `MUTATE` environment variable), the program writes `$0000` to COLOR00
   instead. The colour check must then print FAIL -- the Makefile's
   `test` target greps the control run's output for that specific FAIL
   line and stops the build if it is absent, i.e. if the control failed
   to fail.

## Why the video checks say what they say (and not more)

Three facts about `nanomig.v`'s video ports constrain what this apparatus
can honestly claim:

1. `r`/`g`/`b` are forced to 0 during `hbl`/`vbl` (`nanomig.v:1362-1364`)
   and carry the top nibble of Denise's 8-bit colour (`red[7:4]` etc.), so
   a sample is only meaningful outside blanking.
2. The Amber scan doubler is **always enabled**
   (`nanomig.v:1577 .dblscan(1'b1)`), and it writes its line buffer at
   `wr_ptr[10:1]` every `clk28m` (`minimig/Amber.v:156`), so only every
   second 28 MHz sample reaches the buffer, and `hs` is rebuilt from bit
   15 of that buffer (`Amber.v:182`), restarted independently at both the
   next incoming hsync and at the doubler's own end-of-line
   (`Amber.v:118-131`). The hs period this apparatus measures is
   therefore the scan-doubler's own output line rate, not the Amiga
   chipset's native horizontal line length -- the MEASURED line printed
   below is labelled accordingly and is not compared against any
   expected value.
3. `vs` is passed straight through the doubler unchanged
   (`Amber.v:183 _vsync_out <= dblscan ? _vsync_in : ...`), so the
   MEASURED frame length (vs-to-vs) is the real field/frame rate.

Because of (2), this apparatus does not attempt to address a specific
horizontal pixel column by position; it samples the middle of a
scan-doubler line instead, which is colour-independent and robust to the
doubling.

## What this apparatus deliberately does not check

- No HRM register semantics beyond "the bit pattern written to COLOR00
  reaches the video output guns unchanged" -- no other chip register, no
  DMA, no interrupts, no floppy, no IDE, no AGA-specific behaviour
  (chipset stays OCS), no sprites, no bitplanes, no copper list.
- No cache/fastram/turbo path -- `turbo_config` is 0 throughout, so that
  logic (`cpu_wrapper.v`'s `ramsel`, `nanomig.v`'s `cpu_cache_new`
  instance) is never exercised by either build.
- No timing conformance: the RAM-port memory model answers every access
  with a fixed one-clk_sys-cycle-after-`clk7n_en` latency (matching
  `nanomig/sim/nanomig_tb.cpp`'s own model), not real SRAM/SDRAM timing.
- No native (non-doubled) horizontal line length measurement -- see above.
- No claim about `fx68k` vs `tg68k` behavioural equivalence beyond "both
  pass the same checks here"; the two cores are not compared against each
  other bit-for-bit.
