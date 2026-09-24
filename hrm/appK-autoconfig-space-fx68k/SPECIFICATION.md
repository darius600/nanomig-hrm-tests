# Specification — `appK-autoconfig-space-fx68k`

Package: `hrm/appK-autoconfig-space-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as `hrm/appK-autoconfig-space`,
with the same test source (`test_cpu_wrapper.cpp` here is byte-identical to
that package's). For the claim inventory, the boundary investigation, and
the expected values, see `hrm/appK-autoconfig-space/SPECIFICATION.md`.

## Build difference

The CPU core `cpu_wrapper` instantiates differs from
`hrm/appK-autoconfig-space`, and so does everything that follows from that
choice. That sibling package selects TG68K explicitly
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
fx68k loads them relative to its own working directory. `ENABLE_RAM32=1` is
kept, matching the sibling package -- see the disclosure below.

Disclosure on `ENABLE_RAM32`: no NanoMig board top builds both
`ENABLE_RAM32` and fx68k -- all four RAM32 tops (`lattice/icepi-zero`,
`lattice/icesugar-pro`, `mistle/gw3a_20`, `mistle/gw5a_25`) also define
`ENABLE_TG68K`, and the six fx68k tops define no `ENABLE_RAM32`. This
package therefore measures the sibling's `ENABLE_RAM32` configuration on
the other core -- a pairing no shipped bitstream contains.

## Measured result

Built and run with `make -B test`:

- **Baseline**: 26 passed, 0 failed — identical line count (47 lines) to
  `hrm/appK-autoconfig-space/out.txt`, and `diff` reports no differences at
  all (this package's test source carries no core-specific OUTPUT text).
- **Mutation replay**, each via `make clean && make MUT=<NAME> test`. This
  test source has thirteen `-DMUT_<NAME>=1` build flags (see its header);
  all thirteen are listed here:
  - `MUT=INSIDEPROBE`: `sol:K-0742`, `sol:K-0744`, `opus:K-0767` (26/0 → 23/3).
  - `MUT=LIFECYCLE`: `sol:K-0727`, `opus:K-0755`, `sol:K-0734`,
    `sol:K-0739`, `sol:K-0842`, `sol:K-0843`, `opus:K-0879`, `opus:K-0880`
    (26/0 → 17/9).
  - `MUT=NOBOARD2WRITE`: `sol:K-0733` (26/0 → 25/1).
  - `MUT=NOCOMPLEMENT`: `sol:K-0755`, `opus:K-0776`, `sol:K-0756`,
    `sol:K-0759` (26/0 → 22/4).
  - `MUT=NOHANDOFF`: `sol:K-0733` (26/0 → 25/1).
  - `MUT=NOOVERRIDE`: `sol:K-0742`, `sol:K-0744`, `opus:K-0767`
    (26/0 → 23/3).
  - `MUT=NORESET`: `sol:K-0727`, `opus:K-0755` (26/0 → 24/2).
  - `MUT=NYBBLE`: `sol:K-0750` (26/0 → 25/1).
  - `MUT=REG00SILENT`: `sol:K-0755`, `opus:K-0776` (26/0 → 24/2).
  - `MUT=REG48NOEFFECT`: `sol:K-0735`, `opus:K-0762` (26/0 → 24/2).
  - `MUT=SWAPNYBBLE`: `sol:K-0752`, `sol:K-0753`, `opus:K-0773`,
    `sol:K-0760`, `opus:K-0779` (26/0 → 21/5).
  - `MUT=SWEEPWRONGREG`: `sol:K-0755`, `opus:K-0776` (26/0 → 24/2).
  - `MUT=WRITETOWRITEREG`: `sol:K-0735`, `opus:K-0762` (26/0 → 24/2).
  - Every claim id above is exactly what that `MUT`'s
    `// MUTATION ... stimulus red` comments name in `test_cpu_wrapper.cpp`,
    and every one of these thirteen flipped-line sets is identical, PASS/FAIL
    line for PASS/FAIL line, to what the same `MUT=` build produces in
    `hrm/appK-autoconfig-space`.
  - Rebuilding without `MUT` afterward restores the baseline exactly.
