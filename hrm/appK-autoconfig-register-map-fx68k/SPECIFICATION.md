# Specification — `appK-autoconfig-register-map-fx68k`

Package: `hrm/appK-autoconfig-register-map-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as `hrm/appK-autoconfig-register-map`,
with the same test source (`test_cpu_wrapper.cpp` here is byte-identical to
that package's). For the claim inventory, the boundary investigation and
the expected values, see `hrm/appK-autoconfig-register-map/SPECIFICATION.md`.

## Build difference

The only build-time difference from `hrm/appK-autoconfig-register-map` is
the CPU core `cpu_wrapper` instantiates. That sibling package selects
TG68K explicitly (`-DENABLE_TG68K=1`); this package selects nothing, so
`cpu_wrapper.v`'s own `` `ifndef ENABLE_TG68K`` guard (`:27-32`) defines
`ENABLE_FX68K` instead, and the module instantiates fx68k
(`nanomig/src/fx68k/fx68k.sv`, `fx68kAlu.sv`, `uaddrPla.sv`) rather than
TG68KdotC_Kernel. `ENABLE_RAM32=1` is kept, matching the sibling package.

fx68k fails this project's pinned Verilator (5.052) under its default
checks (measured: `%Error-BLKANDNBLK`, `fx68k.sv:293`); the Makefile adds
`-Wno-BLKANDNBLK` to build it, as NanoMig's own `sim/Makefile` does. Without
`--no-assert`, fx68k's `unique case` checks stop the run at time 0
(`fx68kAlu.sv:313`); the Makefile adds that too.

## Measured result

Built and run with `make -B test`, unmodified from the edits carried over
from the sibling package (no local edits to this package's own files
beyond the Makefile and this SPECIFICATION.md):

- **Baseline**: 43 passed, 0 failed — identical line count (63 lines) to
  `hrm/appK-autoconfig-register-map/out.txt`, and every PASS/FAIL/MEASURED
  line diffs empty against it (`diff` reports no differences at all; this
  package's test source carries no core-specific OUTPUT text).
- **Mutation replay** (`make clean && make MUT=WRONGCONFIG test`, the
  package's one guarded mutation): turns exactly the six assertions its
  `// MUTATION ... MUT=WRONGCONFIG` comments name red — `sol:K-0766`,
  `opus:K-0789`, `sol:K-0765`, `opus:K-0788`, `sol:K-0762`, `opus:K-0785`
  (43 passed / 0 failed → 37 passed / 6 failed) — and the flipped-line set
  is identical, PASS/FAIL line for PASS/FAIL line, to what the same
  `MUT=WRONGCONFIG` build produces in `hrm/appK-autoconfig-register-map`.
  Rebuilding without `MUT` afterward restores the baseline exactly.
