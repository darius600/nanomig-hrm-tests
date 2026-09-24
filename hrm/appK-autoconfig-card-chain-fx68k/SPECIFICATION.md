# Specification — `appK-autoconfig-card-chain-fx68k`

Package: `hrm/appK-autoconfig-card-chain-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as
`hrm/appK-autoconfig-card-chain`, with the same test source
(`test_cpu_wrapper.cpp` here is byte-identical to that package's). For the
claim inventory, the boundary investigation, and the expected values, see
`hrm/appK-autoconfig-card-chain/SPECIFICATION.md`.

## Build difference

The CPU core `cpu_wrapper` instantiates differs from
`hrm/appK-autoconfig-card-chain`, and so does everything that follows from
that choice. That sibling package selects TG68K explicitly
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

## Measured result

Built and run with `make -B test`:

- **Baseline**: 27 passed, 0 failed — identical line count (49 lines) to
  `hrm/appK-autoconfig-card-chain/out.txt`, and `diff` reports no
  differences at all (this package's test source carries no core-specific
  OUTPUT text).
- **Mutation replay**, each via `make clean && make MUT=<NAME> test`. This
  test source has thirteen `-DMUT_<NAME>=1` build flags (see its header);
  all thirteen are listed here:
  - `MUT=CHAINONEBOARD`: `sol:K-0744`, `opus:K-0767` (27/0 → 25/2).
  - `MUT=INSIDEPROBE`: `sol:K-0742`, `sol:K-0744`, `opus:K-0767` (27/0 → 24/3).
  - `MUT=LIFECYCLE`: `sol:K-0727`, `opus:K-0755`, `sol:K-0734`,
    `sol:K-0739`, `sol:K-0842`, `sol:K-0843`, `opus:K-0879`, `opus:K-0880`
    (27/0 → 18/9).
  - `MUT=NOCOMPLEMENT`: `sol:K-0755`, `opus:K-0776`, `sol:K-0756`,
    `sol:K-0759` (27/0 → 23/4).
  - `MUT=NOHANDOFF`: `sol:K-0733` (27/0 → 26/1).
  - `MUT=NOOVERRIDE`: `sol:K-0742`, `sol:K-0744`, `opus:K-0767`
    (27/0 → 24/3).
  - `MUT=NORESET`: `sol:K-0727`, `opus:K-0755` (27/0 → 25/2).
  - `MUT=NYBBLE`: `sol:K-0750` (27/0 → 26/1).
  - `MUT=REG00SILENT`: `sol:K-0755`, `opus:K-0776` (27/0 → 25/2).
  - `MUT=REG48NOEFFECT`: `sol:K-0735`, `opus:K-0762` (27/0 → 25/2).
  - `MUT=SWAPNYBBLE`: `sol:K-0752`, `sol:K-0753`, `opus:K-0773`,
    `sol:K-0760`, `opus:K-0779` (27/0 → 22/5).
  - `MUT=SWEEPWRONGREG`: `sol:K-0755`, `opus:K-0776` (27/0 → 25/2).
  - `MUT=WRITETOWRITEREG`: `sol:K-0735`, `opus:K-0762` (27/0 → 25/2).
  - Every claim id above is exactly what that `MUT`'s
    `// MUTATION ... stimulus red` comments name in `test_cpu_wrapper.cpp`,
    and every one of these thirteen flipped-line sets is identical,
    PASS/FAIL line for PASS/FAIL line, to what the same `MUT=` build
    produces in `hrm/appK-autoconfig-card-chain`.
  - Rebuilding without `MUT` afterward restores the baseline exactly.
