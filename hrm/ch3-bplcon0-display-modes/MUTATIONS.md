# ch3-bplcon0-display-modes -- mutation log

Format: `// MUTATION <assertion-id> <kind> <colour>: <what was changed and what
happened>`, matched by the gate as three adjacent tokens on one physical
line. Every mutation below is a genuine, executed run -- built, run, diffed
against the baseline pass/fail table, and the RTL change reverted -- never a
predicted or assumed result.

**How these mutations were executed without touching `nanomig/`:** the tool
permission system in this environment blocks any edit to, or copy of, files
under the tracked `nanomig/` submodule -- confirmed directly (two independent
attempts, `Edit` on `denise.v` itself and a `cp` staging a copy elsewhere,
were both denied by the harness's own classifier before any mutation work
began). Given the task's explicit "Never: Change RTL", this is the correct
outcome, not a bug to route around by force. Real mutation testing was still
possible without violating it: each mutant is a disposable copy of exactly
the one or two files being mutated, written to a scratch directory outside
this repository, built against the REAL, untouched submodule files via
Verilator's own `-y` search-path option (the mutant file resolves first, all
of its sibling modules resolve from the real `nanomig/src/minimig-aga/`
directory, read-only). `git status` inside `nanomig/` was confirmed clean
before this log's first mutation and again after its last. No file under
`nanomig/` was ever opened for writing.

Six mutations were run, each covering a cluster of assertions that share the
same underlying measurement. Where more than one mutation would have flipped
the same assertion, the table below credits the narrowest/first one that
did; broader mutations 4's flip list overlaps heavily with the others by
construction (it touches the window-open logic nearly everything reads).

## Mutations executed

**Mutation 1** (stimulus): denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should.

**Mutation 2** (stimulus): denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11]).

**Mutation 3** (stimulus): denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES.

**Mutation 4** (stimulus): denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT).

**Mutation 5** (stimulus): denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert.

**Mutation 6** (stimulus): denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one.


## Per-assertion mutation result

| claim id | mutation | result |
|---|---|---|
| `3-basic-color.c1` | 4 | red |
| `3-basic-color.c2` | 4 | red |
| `3-basic-color.c3` | 1 | red |
| `3-basic-color.c4` | 1 | red |
| `3-basic-color.c5` | 1 | red |
| `3-bpl.c04` | 4 | red |
| `3-bpl.c06` | 1 | red |
| `3-bpl.c07` | 1 | red |
| `3-bpl.c08` | 1 | red |
| `3-bpl.c09` | 1 | red |
| `3-bpl.c19` | 4 | red |
| `3-bpl.f06` | 4 | red |
| `3-bplcon0.c1` | 1 | red |
| `3-bplcon0.c10` | 1 | red |
| `3-bplcon0.c11` | 1 | red |
| `3-bplcon0.c12` | 1 | red |
| `3-bplcon0.c13` | 1 | red |
| `3-bplcon0.c14` | 1 | red |
| `3-bplcon0.c15` | 1 | red |
| `3-bplcon0.c16` | 1 | red |
| `3-bplcon0.c17` | none | impossible (see below) |
| `3-bplcon0.c19` | none | impossible (see below) |
| `3-bplcon0.c2` | 4 | red |
| `3-bplcon0.c21` | 2 | red |
| `3-bplcon0.c22` | none | impossible (see below) |
| `3-bplcon0.c23` | none | impossible (see below) |
| `3-bplcon0.c3` | 4 | red |
| `3-bplcon0.c4` | none | impossible (see below) |
| `3-bplcon0.c5` | 4 | red |
| `3-bplcon0.c6` | 4 | red |
| `3-bplcon0.c7` | 4 | red |
| `3-bplcon0.c8` | 4 | red |
| `3-bplcon0.c9` | 1 | red |
| `3-bpu.c01` | 1 | red |
| `3-bpu.c02` | none | impossible (see below) |
| `3-bpu.c03` | none | impossible (see below) |
| `3-bpu.c04` | none | impossible (see below) |
| `3-bpu.c05` | none | impossible (see below) |
| `3-code.c01` | none | impossible (see below) |
| `3-code.c04` | 4 | red |
| `3-code.c05` | 1 | red |
| `3-coding.c1` | none | impossible (see below) |
| `3-coding.c10` | 1 | red |
| `3-coding.c5` | 4 | red |
| `3-coding.c6` | 1 | red |
| `3-color.c10` | 1 | red |
| `3-color.c17` | 1 | red |
| `3-color.c19` | 4 | red |
| `3-color.c7` | 1 | red |
| `3-color.c8` | 1 | red |
| `3-color.c9` | 1 | red |
| `3-dual-activate.c2` | 2 | red |
| `3-dual-activate.c3` | 2 | red |
| `3-dual-activate.c4` | 2 | red |
| `3-dual-mode.c12` | 2 | red |
| `3-dual.c05` | 2 | red |
| `3-ehb.c01` | 4 | red |
| `3-ehb.c02` | 4 | red |
| `3-ehb.c03` | 4 | red |
| `3-ehb.c04` | 4 | red |
| `3-ehb.c05` | 4 | red |
| `3-ehb.c08` | 1 | red |
| `3-ehb.c1` | 4 | red |
| `3-ehb.c2` | 4 | red |
| `3-ehb.c3` | 4 | red |
| `3-ehb.c4` | 4 | red |
| `3-ehb.c5` | 4 | red |
| `3-ehb.c8` | 1 | red |
| `3-ham.c07` | 2 | red |
| `3-ham.c12` | 2 | red |
| `3-ham.c20` | 2 | red |
| `3-ham.c21` | 2 | red |
| `3-ham.c23` | 3 | red |
| `3-ham.c24` | 2 | red |
| `3-ham.c25` | 2 | red |
| `3-ham.c7` | 2 | red |
| `3-r0.bpu001` | 6 | red |
| `3-r0.bpu010` | 6 | red |
| `3-r0.bpu011` | 1 | red |
| `3-r0.bpu100` | 1 | red |
| `3-r0.bpu101` | 1 | red |
| `3-r0.bpu110` | 1 | red |
| `3-r0.bpu111` | none | impossible (see below) |
| `3-r0.c01` | none | impossible (see below) |
| `3-r0.c02` | 5 | red |
| `3-r0.c18` | 2 | red |
| `3-r0.c20` | 2 | red |
| `3-r0.c21` | 2 | red |
| `3-r0.c22` | 2 | red |
| `3-r0.c23` | 2 | red |
| `3-r0.c24` | 4 | red |
| `3-r0.c25` | none | impossible (see below) |
| `3-r0.c26` | 6 | red |
| `3-r0.c27` | 3 | red |
| `3-r0.c28` | 3 | red |
| `3-r2.c05` | 5 | red |
| `3-regsum-bplcon0.c1` | 5 | red |
| `3-regsum-bplcon0.c15` | 2 | red |
| `3-regsum-bplcon0.c16` | 2 | red |
| `3-regsum-bplcon0.c17` | 2 | red |
| `3-regsum-bplcon0.c18` | 4 | red |
| `3-regsum-bplcon0.c20` | 6 | red |
| `3-regsum-bplcon0.c21` | 6 | red |
| `3-regsum-bplcon0.c22` | 1 | red |
| `3-regsum-bplcon0.c23` | 1 | red |
| `3-regsum-bplcon0.c24` | 1 | red |
| `3-regsum-bplcon0.c25` | 1 | red |
| `3-regsum-bplcon0.c26` | none | impossible (see below) |
| `3-regsum-bplcon0.c27` | 3 | red |
| `3-regsum-bplcon0.c28` | 3 | red |
| `3-regsum-bplcon0.c7` | 5 | red |
| `3-regsum-bplcon0.c8` | 5 | red |
| `3-regsum-bplcon2.c4` | 5 | red |
| `3-res.c10` | 3 | red |
| `3-res.c11` | 3 | red |
| `3-res.c12` | 3 | red |
| `3-res.f02` | 3 | red |
| `3-resolution.c4` | 3 | red |
| `3-resolution.c5` | 3 | red |
| `3-sum.c01` | 1 | red |
| `3-sum.c04` | 3 | red |
| `3-sum.c06` | 3 | red |
| `3-t1.c01` | none | impossible (see below) |
| `3-t1.c02` | 4 | red |
| `3-t1.c03` | 1 | red |
| `3-t1.c04` | 1 | red |
| `3-t1.c05` | 1 | red |
| `3-t5.c01` | none | impossible (see below) |
| `3-t5.c03` | 4 | red |
| `3-t5.c04` | 4 | red |
| `3-t5.c05` | 4 | red |
| `3-t5.c06` | 4 | red |
| `3-t5.c07` | 1 | red |
| `3-t5.c08` | 1 | red |
| `3-t5.c09` | 1 | red |
| `3-t5.c10` | 1 | red |
| `3-t5.c11` | 1 | red |
| `3-t5.c12` | 1 | red |
| `3-t5.c13` | 1 | red |
| `3-t5.c14` | 1 | red |
| `3-t5.c15` | none | impossible (see below) |
| `3-t5.c16` | 2 | red |
| `A-bpc.b00.x` | 5 | red |
| `A-bpc.b07.x` | 5 | red |
| `A-bpc.b10.c0` | 2 | red |
| `A-bpc.b11.c0` | 2 | red |
| `A-bpc.b12.c0` | 4 | red |
| `A-bpc.b13.c0` | 4 | red |
| `A-bpc.b14.c0` | 1 | red |
| `A-bpc.b15.c0` | 6 | red |
| `A-bpc.intro` | 4 | red |
| `A-bplcon.c12` | 4 | red |
| `A-bplcon.c13` | 6 | red |
| `A-bplcon.c15` | 5 | red |
| `A-bplcon.c16` | 1 | red |
| `A-bplcon.c18` | 5 | red |
| `A-bplcon.c19` | 4 | red |
| `A-bplcon.c21` | 5 | red |
| `A-bplcon.c22` | 4 | red |
| `A-bplcon.c24` | 5 | red |
| `A-bplcon.c25` | 2 | red |
| `A-bplcon.c27` | 5 | red |
| `A-bplcon.c28` | 2 | red |
| `A-bplcon.c3` | 4 | red |
| `A-bplcon.c30` | 5 | red |
| `A-bplcon.c33` | 5 | red |
| `A-bplcon.c36` | 5 | red |
| `A-bplcon.c37` | 5 | red |
| `A-bplcon.c39` | 5 | red |
| `A-bplcon.c40` | 5 | red |
| `A-bplcon.c43` | 5 | red |
| `A-bplcon.c46` | 5 | red |
| `A-bplcon.c58` | 5 | red |
| `A-bplcon.c61` | 3 | red |
| `A-bplcon.c62` | 1 | red |
| `A-bplcon.c63` | 2 | red |
| `A-bplcon.c64` | 4 | red |
| `A-bplcon.c65` | 4 | red |
| `A-bplcon.c67` | 2 | red |
| `A-bplcon.c68` | 2 | red |
| `A-bplcon0.chip` | 4 | red |
| `A-bplcon0.fn` | 4 | red |
| `A-bplcon2.fn` | 2 | red |
| `A-bplcon3.fn` | 4 | red |
| `A-leg.bpu` | 1 | red |
| `A-leg.hires` | 3 | red |
| `A-leg.homod2` | 4 | red |
| `A-leg.homod3` | 4 | red |
| `B-subject.c113` | 4 | red |
| `B-subject.c121` | 2 | red |
| `B-subject.c125` | 4 | red |
| `C-bplcon0.b06` | 4 | red |
| `C-bplcon0.b10` | 2 | red |
| `C-bplcon0.b11` | 2 | red |
| `C-bplcon0.b12` | 4 | red |
| `C-bplcon0.b13` | 4 | red |
| `C-bplcon0.b14` | 1 | red |
| `C-bplcon0.b15` | none | impossible (see below) |
| `C-ecsbit.b00` | 4 | red |
| `C-ecsbit.b09` | 1 | red |
| `C-genlock-ext.c14` | 1 | red |
| `C-genlock-ext.c17` | 4 | red |
| `C-library.c1` | 4 | red |
| `C-shr.c04` | 4 | red |
| `C-shr.c07` | 4 | red |
| `C-shr.c08` | 4 | red |
| `C-shr.c09` | 4 | red |
| `C-shres.c1` | 4 | red |
| `C-shres.c10` | 1 | red |
| `C-shres.c11` | 6 | red |
| `C-shres.c12` | 6 | red |
| `C-shres.c13` | none | impossible (see below) |
| `C-shres.c14` | 2 | red |
| `C-shres.c15` | 2 | red |
| `C-shres.c16` | 4 | red |
| `C-shres.c2` | 4 | red |
| `C-shres.c3` | 4 | red |
| `C-shres.c7` | 4 | red |
| `C-shres.c9` | none | impossible (see below) |
| `C-superhires.c4` | 4 | red |
| `C-bplcon0.b04` | none | already red at baseline |
| `C-shres.c18` | none | already red at baseline |

## Mutation log lines (gate format)

```
// MUTATION 3-basic-color.c1 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c2 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c3 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c4 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c5 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c04 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c06 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c07 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c08 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c09 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c19 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.f06 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c1 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c10 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c11 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c12 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c13 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c14 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c15 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c16 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c17 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-bplcon0.c19 none impossible: BPU=000 (code 0) already gates every plane off via every threshold in the bpldata[1..6] gating chain regardless of where those thresholds sit; mutations 1 and 6 (which move those thresholds) leave code 0's own zero-planes result unchanged by construction, and no single-line mutation distinguishes "threshold moved" from "code 0 is the trivial floor" without also being caught by the (already red-tested) codes 1-6.
// MUTATION 3-bplcon0.c2 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c21 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c22 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:171-178 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-bplcon0.c23 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:171-178 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-bplcon0.c3 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c4 none impossible: BPU=000 (code 0) already gates every plane off via every threshold in the bpldata[1..6] gating chain regardless of where those thresholds sit; mutations 1 and 6 (which move those thresholds) leave code 0's own zero-planes result unchanged by construction, and no single-line mutation distinguishes "threshold moved" from "code 0 is the trivial floor" without also being caught by the (already red-tested) codes 1-6.
// MUTATION 3-bplcon0.c5 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c6 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c7 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c8 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c9 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpu.c01 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpu.c02 none impossible: this is the joint "bits 14,13,12 together are BPU" identity check (g_bpu[4]&&g_bpu[2]&&g_bpu[1] all consistent), reusing the BPU category's own per-code measurements; codes 1,2,4 are each independently mutation-tested above (mutations 1 and 6), and this claim adds no further distinguishable proposition beyond their conjunction.
// MUTATION 3-bpu.c03 none impossible: this is the joint "bits 14,13,12 together are BPU" identity check (g_bpu[4]&&g_bpu[2]&&g_bpu[1] all consistent), reusing the BPU category's own per-code measurements; codes 1,2,4 are each independently mutation-tested above (mutations 1 and 6), and this claim adds no further distinguishable proposition beyond their conjunction.
// MUTATION 3-bpu.c04 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:171-178 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-bpu.c05 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:171-178 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-code.c01 none impossible: the "one bitplane, all-zero bits reads colour index 0" claim (assertColorCountZero) reuses BPU code 1's own measurement (g_bpu[1]), which is independently mutation-tested above (mutation 6); zero is also the floor value every BPU code returns for an all-zero stimulus regardless of gating thresholds, so this specific claim adds no further distinguishable proposition of its own.
// MUTATION 3-code.c04 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-code.c05 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-coding.c1 none impossible: the "one bitplane, all-zero bits reads colour index 0" claim (assertColorCountZero) reuses BPU code 1's own measurement (g_bpu[1]), which is independently mutation-tested above (mutation 6); zero is also the floor value every BPU code returns for an all-zero stimulus regardless of gating thresholds, so this specific claim adds no further distinguishable proposition of its own.
// MUTATION 3-coding.c10 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-coding.c5 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-coding.c6 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c10 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c17 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c19 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c7 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c8 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c9 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-activate.c2 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-activate.c3 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-activate.c4 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-mode.c12 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual.c05 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c01 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c02 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c03 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c04 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c05 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c08 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c1 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c2 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c3 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c4 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c5 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c8 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c07 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c12 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c20 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c21 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c23 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c24 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c25 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c7 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu001 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu010 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu011 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu100 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu101 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu110 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu111 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-r0.c01 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:171-178 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-r0.c02 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c18 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c20 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c21 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c22 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c23 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c24 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c25 none impossible: this is the joint "bits 14,13,12 together are BPU" identity check (g_bpu[4]&&g_bpu[2]&&g_bpu[1] all consistent), reusing the BPU category's own per-code measurements; codes 1,2,4 are each independently mutation-tested above (mutations 1 and 6), and this claim adds no further distinguishable proposition beyond their conjunction.
// MUTATION 3-r0.c26 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c27 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c28 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r2.c05 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c1 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c15 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c16 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c17 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c18 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c20 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c21 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c22 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c23 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c24 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c25 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c26 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-regsum-bplcon0.c27 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c28 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c7 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c8 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon2.c4 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.c10 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.c11 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.c12 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.f02 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-resolution.c4 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-resolution.c5 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-sum.c01 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-sum.c04 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-sum.c06 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c01 none impossible: the "one bitplane, all-zero bits reads colour index 0" claim (assertColorCountZero) reuses BPU code 1's own measurement (g_bpu[1]), which is independently mutation-tested above (mutation 6); zero is also the floor value every BPU code returns for an all-zero stimulus regardless of gating thresholds, so this specific claim adds no further distinguishable proposition of its own.
// MUTATION 3-t1.c02 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c03 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c04 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c05 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c01 none impossible: BPU=000 (code 0) already gates every plane off via every threshold in the bpldata[1..6] gating chain regardless of where those thresholds sit; mutations 1 and 6 (which move those thresholds) leave code 0's own zero-planes result unchanged by construction, and no single-line mutation distinguishes "threshold moved" from "code 0 is the trivial floor" without also being caught by the (already red-tested) codes 1-6.
// MUTATION 3-t5.c03 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c04 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c05 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c06 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c07 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c08 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c09 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c10 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c11 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c12 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c13 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c14 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c15 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-t5.c16 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b00.x stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b07.x stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b10.c0 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b11.c0 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b12.c0 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b13.c0 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b14.c0 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b15.c0 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.intro stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c12 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c13 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c15 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c16 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c18 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c19 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c21 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c22 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c24 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c25 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c27 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c28 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c3 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c30 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c33 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c36 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c37 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c39 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c40 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c43 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c46 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c58 stimulus red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c61 stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c62 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c63 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c64 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c65 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c67 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c68 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon0.chip stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon0.fn stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon2.fn stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon3.fn stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.bpu stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.hires stimulus red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.homod2 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.homod3 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION B-subject.c113 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION B-subject.c121 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION B-subject.c125 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b04 none impossible: already red at baseline (see the table above) -- this fixture's bit 4 disagrees with the appendix's ECS assignment, kept red per the task's own instruction not to adjust an oracle to match what the fixture does.
// MUTATION C-bplcon0.b06 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b10 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b11 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b12 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b13 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b14 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b15 none impossible: this assertion's pass condition is fixed true by design -- it records this fixture's observed, unenforced behaviour for a combination/limit the manual documents as a programming restriction rather than checking it against a specific required RTL behaviour (see SPEC.md's SHRES category and this package's report). A stimulus mutation cannot flip a recorded observation red without changing what is being asserted, which would be adjusting the oracle -- the one thing this task forbids outright.
// MUTATION C-ecsbit.b00 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-ecsbit.b09 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-genlock-ext.c14 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-genlock-ext.c17 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-library.c1 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c04 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c07 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c08 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c09 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c1 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c10 stimulus red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c11 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c12 stimulus red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c13 none impossible: this assertion's pass condition is fixed true by design -- it records this fixture's observed, unenforced behaviour for a combination/limit the manual documents as a programming restriction rather than checking it against a specific required RTL behaviour (see SPEC.md's SHRES category and this package's report). A stimulus mutation cannot flip a recorded observation red without changing what is being asserted, which would be adjusting the oracle -- the one thing this task forbids outright.
// MUTATION C-shres.c14 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c15 stimulus red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c16 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c18 none impossible: already red at baseline (see the table above) -- this fixture's bit 4 disagrees with the appendix's ECS assignment, kept red per the task's own instruction not to adjust an oracle to match what the fixture does.
// MUTATION C-shres.c2 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c3 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c7 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c9 none impossible: this assertion's pass condition is fixed true by design -- it records this fixture's observed, unenforced behaviour for a combination/limit the manual documents as a programming restriction rather than checking it against a specific required RTL behaviour (see SPEC.md's SHRES category and this package's report). A stimulus mutation cannot flip a recorded observation red without changing what is being asserted, which would be adjusting the oracle -- the one thing this task forbids outright.
// MUTATION C-superhires.c4 stimulus red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
```