# Specification — `appK-autoconfig-base-address-fx68k`

Package: `hrm/appK-autoconfig-base-address-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as
`hrm/appK-autoconfig-base-address`, with the same test source
(`test_cpu_wrapper.cpp` here is byte-identical to that package's). For the
claim inventory, the boundary investigation, the candidate finding this
package exists to grade, and the expected values (including the eight
assertions expected RED at baseline and the `GREENCHECK_*`/`WRONGREG`
mutation methodology this reuses unchanged), see
`hrm/appK-autoconfig-base-address/SPECIFICATION.md`.

## Build difference

The only build-time difference from `hrm/appK-autoconfig-base-address` is
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

- **Baseline**: 5 passed, 8 failed — identical line count (31 lines) to
  `hrm/appK-autoconfig-base-address/out.txt`, and every PASS/FAIL/MEASURED/
  RECORDED line diffs empty against it (this package's test source carries
  no core-specific OUTPUT text). The eight failures are the same
  already-documented conformance findings as the sibling package's (see its
  SPECIFICATION.md, "The candidate finding" and "A note on the MUTATION
  requirement for already-red assertions"), not a build regression.
- **Mutation replay**, each via `make clean && make MUT=<NAME> test`:
  - `MUT=GREENCHECK_4C`: turns green exactly the six already-red shut-up
    assertions its comments name — `sol:K-0740`, `sol:K-0849`,
    `sol:K-0850`, `opus:K-0889`, `sol:K-0848`, `opus:K-0888` (5/8 → 11/2).
  - `MUT=GREENCHECK_REG48`: turns green exactly the two already-red
    Finding assertions — `sol:K-0844`, `opus:K-0881` (5/8 → 7/6).
  - `MUT=WRONGREG`: turns red exactly the four passing register-identity
    assertions its comments name — `sol:K-0834`, `opus:K-0866`,
    `sol:K-0835`, `opus:K-0867` (5/8 → 1/12).
  - Every one of these three flipped-line sets is identical, PASS/FAIL
    line for PASS/FAIL line, to what the same `MUT=` build produces in
    `hrm/appK-autoconfig-base-address`.
  - Rebuilding without `MUT` afterward restores the baseline exactly.
