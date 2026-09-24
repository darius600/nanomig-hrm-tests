# Specification — `appK-autoconfig-space-fx68k`

Package: `hrm/appK-autoconfig-space-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as `hrm/appK-autoconfig-space`,
with the same test source (`test_cpu_wrapper.cpp` here is byte-identical to
that package's). For the claim inventory, the boundary investigation, and
the expected values, see `hrm/appK-autoconfig-space/SPECIFICATION.md`.

## Build difference

The only build-time difference from `hrm/appK-autoconfig-space` is the CPU
core `cpu_wrapper` instantiates. That sibling package selects TG68K
explicitly (`-DENABLE_TG68K=1`); this package selects nothing, so
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

- **Baseline**: 26 passed, 0 failed — identical line count (47 lines) to
  `hrm/appK-autoconfig-space/out.txt`, and `diff` reports no differences at
  all (this package's test source carries no core-specific OUTPUT text).
- **Mutation replay**, each via `make clean && make MUT=<NAME> test`:
  - `MUT=INSIDEPROBE`: `sol:K-0742`, `sol:K-0744`, `opus:K-0767` (26/0 → 23/3).
  - `MUT=LIFECYCLE`: `sol:K-0727`, `opus:K-0755`, `sol:K-0734`,
    `sol:K-0739`, `sol:K-0842`, `sol:K-0843`, `opus:K-0879`, `opus:K-0880`
    (26/0 → 17/9).
  - `MUT=NOCOMPLEMENT`: `sol:K-0755`, `opus:K-0776`, `sol:K-0756`,
    `sol:K-0759` (26/0 → 22/4).
  - `MUT=NOHANDOFF`: `sol:K-0733` (26/0 → 25/1).
  - `MUT=NORESET`: `sol:K-0727`, `opus:K-0755` (26/0 → 24/2).
  - `MUT=NYBBLE`: `sol:K-0750` (26/0 → 25/1).
  - `MUT=REG00SILENT`: `sol:K-0755`, `opus:K-0776` (26/0 → 24/2).
  - `MUT=SWAPNYBBLE`: `sol:K-0752`, `sol:K-0753`, `opus:K-0773`,
    `sol:K-0760`, `opus:K-0779` (26/0 → 21/5).
  - `MUT=SWEEPWRONGREG`: `sol:K-0755`, `opus:K-0776` (26/0 → 24/2).
  - Every claim id above is exactly what that `MUT`'s
    `// MUTATION ... stimulus red` comments name in `test_cpu_wrapper.cpp`,
    and every one of these nine flipped-line sets is identical, PASS/FAIL
    line for PASS/FAIL line, to what the same `MUT=` build produces in
    `hrm/appK-autoconfig-space`.
  - Rebuilding without `MUT` afterward restores the baseline exactly.
