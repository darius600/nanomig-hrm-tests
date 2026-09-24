# Specification — `appK-autoconfig-size-encoding-fx68k`

Package: `hrm/appK-autoconfig-size-encoding-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as
`hrm/appK-autoconfig-size-encoding`, with the same test source
(`test_cpu_wrapper.cpp` here is byte-identical to that package's). For the
claim inventory, the boundary investigation, and the expected values, see
`hrm/appK-autoconfig-size-encoding/SPECIFICATION.md`.

## Build difference

The only build-time difference from `hrm/appK-autoconfig-size-encoding` is
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

Built and run with `make -B test`:

- **Baseline**: 16 passed, 0 failed — identical line count (29 lines) to
  `hrm/appK-autoconfig-size-encoding/out.txt`, and every PASS/FAIL/MEASURED/
  RECORDED line diffs empty against it (this package's test source carries
  no core-specific OUTPUT text).
- **Mutation replay**, each via `make clean && make MUT=<NAME> test`:
  - `MUT=WRONGCONFIG`: turns red exactly the seven assertions its comments
    name — `sol:K-0786`, `opus:K-0795`, `sol:K-0787`, `sol:K-0788`,
    `opus:K-0819`, `sol:K-0771`, `opus:K-0794`.
  - `MUT=WRONGSIZE`: turns red exactly the four assertions its comments
    name — `opus:K-0796`, `opus:K-0808`, `sol:K-0779`, `opus:K-0810`.
  - `MUT=WRONGSTAGE`: turns red exactly the two assertions its comments
    name — `opus:K-0805`, `opus:K-0803`.
  - Every one of these three flipped-line sets is identical, PASS/FAIL
    line for PASS/FAIL line, to what the same `MUT=` build produces in
    `hrm/appK-autoconfig-size-encoding`.
  - Rebuilding without `MUT` afterward restores the baseline exactly.
