# Specification — `appK-autoconfig-register-map-fx68k`

Package: `hrm/appK-autoconfig-register-map-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as `hrm/appK-autoconfig-register-map`,
with the same test source (`test_cpu_wrapper.cpp` here is byte-identical to
that package's). For the claim inventory, the boundary investigation and
the expected values, see `hrm/appK-autoconfig-register-map/SPECIFICATION.md`.

## Build difference

The CPU core `cpu_wrapper` instantiates differs from
`hrm/appK-autoconfig-register-map`, and so does everything that follows from
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
also passes `-DCORE_FX68K=1` (unused by this test source's `#ifdef`s, but
part of the real build-time difference), and its `test` target copies
fx68k's microcode ROMs (`microrom.mem`, `nanorom.mem`) into `obj_dir/` and
runs the built binary from there, since fx68k loads them relative to its
own working directory. `ENABLE_RAM32=1` is kept, matching the sibling
package -- see the disclosure below.

Disclosure on `ENABLE_RAM32`: no NanoMig board top builds both
`ENABLE_RAM32` and fx68k -- all four RAM32 tops (`lattice/icepi-zero`,
`lattice/icesugar-pro`, `mistle/gw3a_20`, `mistle/gw5a_25`) also define
`ENABLE_TG68K`, and the six fx68k tops define no `ENABLE_RAM32`. This
package therefore measures the sibling's `ENABLE_RAM32` configuration on
the other core -- a pairing no shipped bitstream contains.

## Measured result

Built and run with `make -B test` (no local edits to this package's own
files beyond the Makefile and this SPECIFICATION.md):

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
