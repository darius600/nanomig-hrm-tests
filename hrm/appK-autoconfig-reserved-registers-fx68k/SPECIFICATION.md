# Specification — `appK-autoconfig-reserved-registers-fx68k`

Package: `hrm/appK-autoconfig-reserved-registers-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as
`hrm/appK-autoconfig-reserved-registers`, with the same test source
(`test_cpu_wrapper.cpp` here is byte-identical to that package's). For the
claim inventory, the boundary investigation, and the expected values, see
`hrm/appK-autoconfig-reserved-registers/SPECIFICATION.md`.

## Build difference

The CPU core `cpu_wrapper` instantiates differs from
`hrm/appK-autoconfig-reserved-registers`, and so does everything that
follows from that choice. That sibling package selects TG68K explicitly
(`-DENABLE_TG68K=1`); this package defines no `ENABLE_TG68K`, so
`cpu_wrapper.v`'s own `` `ifndef ENABLE_TG68K`` guard (`:27-32`) defines
`ENABLE_FX68K` instead, and the module instantiates fx68k from its own
sources (`nanomig/src/fx68k/fx68k.sv`, `fx68kAlu.sv`, `uaddrPla.sv`) rather
than linking `TG68KdotC_Kernel.v`. Building fx68k needs `-Wno-BLKANDNBLK`
(it fails this project's pinned Verilator, 5.052, under its default checks
-- measured: `%Error-BLKANDNBLK`, `fx68k.sv:293` -- the same flag NanoMig's
own `sim/Makefile` uses) and `--no-assert` (without it, fx68k's
`unique case` checks stop the run at time 0, `fx68kAlu.sv:313`); the build
also passes `-DCORE_FX68K=1` (unused by this test source's `#ifdef`s), and
its `test` target copies fx68k's microcode ROMs (`microrom.mem`,
`nanorom.mem`) into `obj_dir/` and runs the built binary from there, since
fx68k loads them relative to its own working directory. Neither package
defines `ENABLE_RAM32` -- this no-RAM32/fx68k pairing matches the fx68k
boards' own shipped configuration: the six fx68k tops define no
`ENABLE_RAM32`.

The Makefile also keeps the `MUT=<NAME>` hook every package's Makefile
carries, for uniformity; this package's test source has nothing for it to
guard (see "Measured result" below).

## Measured result

Built and run with `make -B test`:

- **Baseline**: 35 passed, 0 failed — identical line count (56 lines) to
  `hrm/appK-autoconfig-reserved-registers/out.txt`, and every
  PASS/FAIL/MEASURED line diffs empty against it (this package's test
  source carries no core-specific OUTPUT text).
- **Mutation replay**: none. Every one of this package's 35
  `// MUTATION` records is `none impossible` (each graded value is an
  unconditional case-statement default with no input port that can
  perturb it — see the sibling package's SPECIFICATION.md); no `#ifdef
  MUT_` exists in this test source, so the Makefile's `MUT=<NAME>` hook has
  nothing to guard and no `MUT=` value changes this build's output, in
  either package.
