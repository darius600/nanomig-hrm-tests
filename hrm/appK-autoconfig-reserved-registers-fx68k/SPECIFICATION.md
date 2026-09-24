# Specification — `appK-autoconfig-reserved-registers-fx68k`

Package: `hrm/appK-autoconfig-reserved-registers-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as
`hrm/appK-autoconfig-reserved-registers`, with the same test source
(`test_cpu_wrapper.cpp` here is byte-identical to that package's). For the
claim inventory, the boundary investigation, and the expected values, see
`hrm/appK-autoconfig-reserved-registers/SPECIFICATION.md`.

## Build difference

The only build-time difference from `hrm/appK-autoconfig-reserved-registers`
is the CPU core `cpu_wrapper` instantiates. That sibling package selects
TG68K explicitly (`-DENABLE_TG68K=1`); this package selects nothing, so
`cpu_wrapper.v`'s own `` `ifndef ENABLE_TG68K`` guard (`:27-32`) defines
`ENABLE_FX68K` instead, and the module instantiates fx68k
(`nanomig/src/fx68k/fx68k.sv`, `fx68kAlu.sv`, `uaddrPla.sv`) rather than
TG68KdotC_Kernel. Neither package defines `ENABLE_RAM32`.

fx68k fails this project's pinned Verilator (5.052) under its default
checks (measured: `%Error-BLKANDNBLK`, `fx68k.sv:293`); the Makefile adds
`-Wno-BLKANDNBLK` to build it, as NanoMig's own `sim/Makefile` does. Without
`--no-assert`, fx68k's `unique case` checks stop the run at time 0
(`fx68kAlu.sv:313`); the Makefile adds that too.

## Measured result

Built and run with `make -B test`:

- **Baseline**: 35 passed, 0 failed — identical line count (56 lines) to
  `hrm/appK-autoconfig-reserved-registers/out.txt`, and every
  PASS/FAIL/MEASURED line diffs empty against it (this package's test
  source carries no core-specific OUTPUT text).
- **Mutation replay**: none. Every one of this package's 35
  `// MUTATION` records is `none impossible` (each graded value is an
  unconditional case-statement default with no input port that can
  perturb it — see the sibling package's SPECIFICATION.md); there is no
  `MUT_`-guarded build to replay, in either package.
