# Specification — `appK-autoconfig-space-decode-fx68k`

Package: `hrm/appK-autoconfig-space-decode-fx68k`
Fixture (top module): `cpu_wrapper`

This package grades exactly the same claims as
`hrm/appK-autoconfig-space-decode`, with the same test source
(`test_cpu_wrapper.cpp` here is byte-identical to that package's). For the
claim inventory, the boundary investigation, and the expected values, see
`hrm/appK-autoconfig-space-decode/SPECIFICATION.md`.

## Build difference

The only build-time difference from `hrm/appK-autoconfig-space-decode` is
the CPU core `cpu_wrapper` instantiates. That sibling package selects
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

The Makefile also passes `-CFLAGS -DCORE_FX68K=1`. This test source is the
only one of the seven where that matters: two `record()` calls (declining
`sol:K-0221`/`opus:K-0229` and `sol:K-0456`/`opus:K-0458`) state, as part
of their NOT-GRADED reasoning, a fact that is true of only one core
(which macro TG68K's own address narrowing, or which file carries no FC
port). `CORE_FX68K` selects the text that is true of this build; the
sibling TG68K package's text is unchanged.

## Measured result

Built and run with `make -B test`:

- **Baseline**: 17 passed, 0 failed — identical line count (27 lines) to
  `hrm/appK-autoconfig-space-decode/out.txt`. Every PASS/FAIL/MEASURED line
  diffs empty against it. Exactly two RECORDED lines differ, both by
  design (`CORE_FX68K`-selected text, not a grading change):
  - `sol:K-0221`/`opus:K-0229` (FC0-FC2): the sibling's text says "this
    build defines only ENABLE_TG68K, ... In an fx68k build ... fc_o is
    driven but leaves cpu_wrapper only as the cycle type on cpustate";
    this build's text says "this build defines ENABLE_FX68K (no
    -DENABLE_TG68K, Makefile), so fc_o is driven (:493-495) but leaves
    cpu_wrapper only as the cycle type on cpustate ..., ... nanomig/sim/
    TG68KdotC_Kernel.v, which the sibling TG68K build links instead,
    carries no FC0/FC1/FC2 port under any name". Same verdict
    (`NOT GRADED`, `reason_kind: unreachable-port`), same claim ids, text
    now stated from this build's own side instead of hypothetically.
  - `sol:K-0456`/`opus:K-0458` (Zorro III `$FF000000`): the sibling's text
    cites `` `TG68K_A24, cpu_wrapper.v:34`` as what limits the CPU core to
    24-bit addressing; this build's text cites `cpu_addr_o`'s own [23:1]
    declaration (`cpu_wrapper.v:243`) and `cpu_wrapper.v:298`'s
    `cpu_addr = {8'b0, cpu_addr_o, 1'b0}` instead — the mechanism that
    actually limits fx68k's address here (`` `TG68K_A24`` has no effect
    outside the `` `ifdef ENABLE_TG68K`` branch this build does not
    compile). Same verdict (`NOT GRADED`, `reason_kind:
    unreachable-address`), same claim ids.
- **Mutation replay**, each via `make clean && make MUT=<NAME> test`:
  - `MUT=NOFASTRAMCFG`: turns red exactly the six SWEEP-group assertions
    plus the liveness check its comments name — `sol:K-0041`,
    `sol:K-0172`, `opus:K-0179`, `sol:K-0106`, `opus:K-0122`, `sol:K-0107`.
  - `MUT=NOCONFIGWRITE`: turns red exactly the six CONFIG-group assertions
    its comments name — `sol:K-0108`, `opus:K-0123`, `opus:K-0269`,
    `opus:K-0052`, `sol:K-0057`, `opus:K-0074`.
  - `MUT=NORESET`: turns red exactly the four RESET-group assertions plus
    the liveness check its comments name — `sol:K-0153`, `opus:K-0162`,
    `sol:K-0432`, `opus:K-0438`.
  - Every one of these three flipped-line sets is identical, PASS/FAIL
    line for PASS/FAIL line, to what the same `MUT=` build produces in
    `hrm/appK-autoconfig-space-decode` (the two `CORE_FX68K`-selected
    RECORDED lines are unconditional `record()` calls, unaffected by any
    `MUT_` guard, and stay at their fx68k-side text throughout).
  - Rebuilding without `MUT` afterward restores the baseline exactly.
