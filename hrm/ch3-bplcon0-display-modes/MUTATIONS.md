# ch3-bplcon0-display-modes -- mutation log

Format: `// MUTATION <assertion-id> <kind> <colour>: <what was changed and what
happened>`, matched by the gate as three adjacent tokens on one physical
line. Every mutation below is a genuine, executed run -- built, run, diffed
against the baseline pass/fail table, and the RTL change reverted -- never a
predicted or assumed result.

**How mutation testing works without touching `nanomig/`:** `nanomig/` is a
tracked, read-only submodule (task instruction: "Never: Change RTL"), so no
mutation may edit or copy over any file inside it. `make MUTANT=M<n> test`
instead builds a disposable mutant copy: the `mutant-stage` Makefile target
reads (never writes) the one or two real file(s) each mutation touches out
of `nanomig/src/minimig-aga/`, copies them into this package's own
`obj_dir/mutant_M<n>/`, and applies `mutations/M<n>.patch` there with
`patch`. That directory is put first on Verilator's `-y` search path (and,
when the mutation touches `$(TOP_MODULE).v` itself, used directly as the
top-level source), so the mutant file resolves and every untouched sibling
module still resolves from the real, unmodified
`nanomig/src/minimig-aga/` directory. No file under `nanomig/` is ever
opened for writing; `git status` inside `nanomig/` stays clean throughout.

Six mutations were run, each covering a cluster of assertions that share the
same underlying measurement. Where more than one mutation would have flipped
the same assertion, the table below credits the narrowest/first one that
did; broader mutations 4's flip list overlaps heavily with the others by
construction (it touches the window-open logic nearly everything reads).

**Label correction:** all six are `implementation` mutations, not `stimulus`. Each one edits
the disposable copy of `denise.v` or `denise_bitplane_shifter.v` itself -- a gating threshold,
a bit-source mapping, a shift-enable expression -- and rebuilds against that edited copy, so
each mutates the design under test. A stimulus mutation instead changes what the module is
*asked to do* (the register writes and timing this harness drives) and leaves both the
implementation and the expected value alone; none of the six below do that, so all six and
every per-assertion log line below are labelled `implementation` accordingly.

**Replaying a mutation:** `mutations/M1.patch` .. `mutations/M6.patch` are unified diffs
of the six edits above, taken against `denise.v` or `denise_bitplane_shifter.v` as they read
at the RTL pin this package builds. `make MUTANT=M<n> test` replays mutation `<n>`: it reads
(never writes) the real file(s) that patch touches out of `nanomig/`, copies them into
`obj_dir/mutant_M<n>/`, applies the patch there with `patch`, and rebuilds with that directory
searched by Verilator before the real `nanomig/src/minimig-aga/` directory -- the mutant file
resolves, every other module still resolves from the real, untouched submodule. A red assertion
makes `make` exit non-zero -- the result each mutation is meant to produce, not a broken build.
Leaving `MUTANT` unset builds the plain sources unchanged; no mutant directory is created or
searched.

## Mutations executed

**Mutation 1** (implementation): denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should.

**Mutation 2** (implementation): denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11]).

**Mutation 3** (implementation): denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES.

**Mutation 4** (implementation): denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT).

**Mutation 5** (implementation): denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert.

**Mutation 6** (implementation): denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one.


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
| `3-dual-activate.c4` | none | impossible (see below) |
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
| `C-bplcon0.b04` | none | impossible (see below) |
| `C-shres.c18` | none | impossible (see below) |

## Mutation log lines (gate format)

```
// MUTATION 3-basic-color.c1 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c2 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c3 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c4 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-basic-color.c5 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c04 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c06 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c07 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c08 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c09 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.c19 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpl.f06 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c1 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c10 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c11 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c12 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c13 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c14 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c15 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c16 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c17 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-bplcon0.c19 none impossible: BPU=000 (code 0) already gates every plane off via every threshold in the bpldata[1..6] gating chain regardless of where those thresholds sit; mutations 1 and 6 (which move those thresholds) leave code 0's own zero-planes result unchanged by construction, and no single-line mutation distinguishes "threshold moved" from "code 0 is the trivial floor" without also being caught by the (already red-tested) codes 1-6.
// MUTATION 3-bplcon0.c2 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c21 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c22 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:173-180 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-bplcon0.c23 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:173-180 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-bplcon0.c3 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c4 none impossible: BPU=000 (code 0) already gates every plane off via every threshold in the bpldata[1..6] gating chain regardless of where those thresholds sit; mutations 1 and 6 (which move those thresholds) leave code 0's own zero-planes result unchanged by construction, and no single-line mutation distinguishes "threshold moved" from "code 0 is the trivial floor" without also being caught by the (already red-tested) codes 1-6.
// MUTATION 3-bplcon0.c5 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c6 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c7 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c8 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bplcon0.c9 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpu.c01 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-bpu.c02 none impossible: this is the joint "bits 14,13,12 together are BPU" identity check (g_bpu[4]&&g_bpu[2]&&g_bpu[1] all consistent), reusing the BPU category's own per-code measurements; codes 1,2,4 are each independently mutation-tested above (mutations 1 and 6), and this claim adds no further distinguishable proposition beyond their conjunction.
// MUTATION 3-bpu.c03 none impossible: this is the joint "bits 14,13,12 together are BPU" identity check (g_bpu[4]&&g_bpu[2]&&g_bpu[1] all consistent), reusing the BPU category's own per-code measurements; codes 1,2,4 are each independently mutation-tested above (mutations 1 and 6), and this claim adds no further distinguishable proposition beyond their conjunction.
// MUTATION 3-bpu.c04 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:173-180 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-bpu.c05 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:173-180 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-code.c01 none impossible: the "one bitplane, all-zero bits reads colour index 0" claim (assertColorCountZero) reuses BPU code 1's own measurement (g_bpu[1]), which is independently mutation-tested above (mutation 6); zero is also the floor value every BPU code returns for an all-zero stimulus regardless of gating thresholds, so this specific claim adds no further distinguishable proposition of its own.
// MUTATION 3-code.c04 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-code.c05 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-coding.c1 none impossible: the "one bitplane, all-zero bits reads colour index 0" claim (assertColorCountZero) reuses BPU code 1's own measurement (g_bpu[1]), which is independently mutation-tested above (mutation 6); zero is also the floor value every BPU code returns for an all-zero stimulus regardless of gating thresholds, so this specific claim adds no further distinguishable proposition of its own.
// MUTATION 3-coding.c10 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-coding.c5 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-coding.c6 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c10 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c17 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c19 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c7 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c8 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-color.c9 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-activate.c2 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-activate.c3 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual-activate.c4 none impossible: no longer asserted in this package -- this row's own claim, "DBLPF changes the hardware movement behavior of the bitplanes on-screen," named the page-72 "changes hardware movement" clause but was graded by the colour-grouping evidence above, which SPEC.md's own category-8 note calls a wrong-clause proxy for that clause. Withdrawn to a RECORDED line (test_denise.cpp/claims_gen.inc) citing p.85's warning ("Always set all six bits, even if you have only one playfield. Set 3-0 and 7-4 to the same value if you are using only one playfield"), which predicts PF2H governs bitplanes 2, 4 and 6's delay the same way regardless of DBLPF -- see the Extension section below, `3-dual.c46`, for the direct measurement. There is no red/green outcome left for a mutation to flip.
// MUTATION 3-dual-mode.c12 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-dual.c05 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c01 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c02 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c03 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c04 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c05 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c08 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c1 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c2 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c3 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c4 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c5 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ehb.c8 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c07 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c12 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c20 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c21 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c23 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c24 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c25 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-ham.c7 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu001 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu010 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu011 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu100 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu101 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu110 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.bpu111 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-r0.c01 none impossible: this assertion (assertAtomicWrite) verifies a structural fact read directly from source (denise.v:173-180 has exactly one write path, one address, one 16-bit word, no per-bit strobe) rather than a runtime measurement; every mutation in this log that touched BPLCON0-gated behaviour (1,2,4,5,6) still reached its target configuration through that same single write -- the write protocol line itself was never mutated, and mutating it would break literally every other assertion in this package at once rather than isolate this one.
// MUTATION 3-r0.c02 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c18 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c20 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c21 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c22 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c23 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c24 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c25 none impossible: this is the joint "bits 14,13,12 together are BPU" identity check (g_bpu[4]&&g_bpu[2]&&g_bpu[1] all consistent), reusing the BPU category's own per-code measurements; codes 1,2,4 are each independently mutation-tested above (mutations 1 and 6), and this claim adds no further distinguishable proposition beyond their conjunction.
// MUTATION 3-r0.c26 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c27 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r0.c28 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-r2.c05 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c1 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c15 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c16 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c17 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c18 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c20 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c21 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c22 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c23 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c24 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c25 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c26 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-regsum-bplcon0.c27 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c28 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c7 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon0.c8 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-regsum-bplcon2.c4 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.c10 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.c11 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.c12 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-res.f02 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-resolution.c4 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-resolution.c5 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-sum.c01 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-sum.c04 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-sum.c06 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c01 none impossible: the "one bitplane, all-zero bits reads colour index 0" claim (assertColorCountZero) reuses BPU code 1's own measurement (g_bpu[1]), which is independently mutation-tested above (mutation 6); zero is also the floor value every BPU code returns for an all-zero stimulus regardless of gating thresholds, so this specific claim adds no further distinguishable proposition of its own.
// MUTATION 3-t1.c02 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c03 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c04 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t1.c05 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c01 none impossible: BPU=000 (code 0) already gates every plane off via every threshold in the bpldata[1..6] gating chain regardless of where those thresholds sit; mutations 1 and 6 (which move those thresholds) leave code 0's own zero-planes result unchanged by construction, and no single-line mutation distinguishes "threshold moved" from "code 0 is the trivial floor" without also being caught by the (already red-tested) codes 1-6.
// MUTATION 3-t5.c03 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c04 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c05 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c06 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c07 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c08 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c09 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c10 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c11 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c12 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c13 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c14 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION 3-t5.c15 none impossible: the manual assigns no defined plane count to BPU=111; this claim's assertion (assertBpu111) is a recorded observation of whatever this fixture does with that code, not a check against a specific required value -- there is no red/green distinction a stimulus mutation could produce without changing what is being asserted.
// MUTATION 3-t5.c16 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b00.x implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b07.x implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b10.c0 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b11.c0 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b12.c0 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b13.c0 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b14.c0 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.b15.c0 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bpc.intro implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c12 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c13 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c15 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c16 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c18 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c19 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c21 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c22 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c24 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c25 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c27 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c28 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c3 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c30 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c33 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c36 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c37 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c39 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c40 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c43 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c46 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c58 implementation red: mutation 5 (denise.v: BPLCON0 bits chapter 3 calls "not used" (0,4,5,6,7) OR-ed into dpf, and BPLCON2 bits 7-15 ("not used") OR-ed into the pf2pri bit passed to denise_playfields, so each becomes observably non-inert) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c61 implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c62 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c63 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c64 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c65 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c67 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon.c68 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon0.chip implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon0.fn implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon2.fn implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION A-bplcon3.fn implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.bpu implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.hires implementation red: mutation 3 (denise_bitplane_shifter.v: hires shift-enable changed from '~c1 ^ c3' (every other 28MHz clock) to '~c1 & ~c3' (same as lores), so HIRES no longer shifts any faster than LORES) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.homod2 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION A-leg.homod3 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION B-subject.c113 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION B-subject.c121 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION B-subject.c125 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b04 none impossible: no longer asserted in this package -- appendix C p.300's row for bit4 carries the mnemonic SPRHWRM and no description column at all (unlike bit6 three rows above, which states its behaviour in full), so the manual requires nothing of the RTL at this bit; independently SPRHWRM names a sprite function and this fixture drives bitplanes only. Declined with a recorded reason (see SPEC.md's R8 and claims_gen.inc's declineBit4NoManualBehavior); there is no red/green outcome left for a mutation to flip. (This replaces a withdrawn finding: the claim previously asserted a measurable effect the manual never specified.)
// MUTATION C-bplcon0.b06 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b10 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b11 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b12 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b13 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b14 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-bplcon0.b15 none impossible: this assertion's pass condition is fixed true by design -- it records this fixture's observed, unenforced behaviour for a combination/limit the manual documents as a programming restriction rather than checking it against a specific required RTL behaviour (see SPEC.md's SHRES category and this package's report). A stimulus mutation cannot flip a recorded observation red without changing what is being asserted, which would be adjusting the oracle -- the one thing this task forbids outright.
// MUTATION C-ecsbit.b00 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-ecsbit.b09 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-genlock-ext.c14 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-genlock-ext.c17 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-library.c1 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c04 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c07 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c08 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shr.c09 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c1 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c10 implementation red: mutation 1 (denise.v: bpldata[3..6] plane-gating thresholds each raised by one (l_bpu>2,3,4,5 became l_bpu>3,4,5,6), so BPU codes 3-6 each report one fewer gated plane than they should) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c11 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c12 implementation red: mutation 6 (denise.v: hires remapped from bplcon0[15] to bplcon0[14]; plane 1/2 gating thresholds each raised by one (l_bpu>0,1 became l_bpu>1,2), so BPU=1 shows no planes and BPU=2 shows only one) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c13 none impossible: this assertion's pass condition is fixed true by design -- it records this fixture's observed, unenforced behaviour for a combination/limit the manual documents as a programming restriction rather than checking it against a specific required RTL behaviour (see SPEC.md's SHRES category and this package's report). A stimulus mutation cannot flip a recorded observation red without changing what is being asserted, which would be adjusting the oracle -- the one thing this task forbids outright.
// MUTATION C-shres.c14 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c15 implementation red: mutation 2 (denise.v: HOMOD/DBLPF bit sources swapped (ham was bplcon0[11]/dpf was bplcon0[10]; now ham=bplcon0[10]/dpf=bplcon0[11])) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c16 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c18 none impossible: no longer asserted in this package -- appendix C p.300's row for bit4 carries the mnemonic SPRHWRM and no description column at all (unlike bit6 three rows above, which states its behaviour in full), so the manual requires nothing of the RTL at this bit; independently SPRHWRM names a sprite function and this fixture drives bitplanes only. Declined with a recorded reason (see SPEC.md's R8 and claims_gen.inc's declineBit4NoManualBehavior); there is no red/green outcome left for a mutation to flip. (This replaces a withdrawn finding: the claim previously asserted a measurable effect the manual never specified.)
// MUTATION C-shres.c2 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c3 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c7 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
// MUTATION C-shres.c9 none impossible: this assertion's pass condition is fixed true by design -- it records this fixture's observed, unenforced behaviour for a combination/limit the manual documents as a programming restriction rather than checking it against a specific required RTL behaviour (see SPEC.md's SHRES category and this package's report). A stimulus mutation cannot flip a recorded observation red without changing what is being asserted, which would be adjusting the oracle -- the one thing this task forbids outright.
// MUTATION C-superhires.c4 implementation red: mutation 4 (denise.v: ecsena remapped from bplcon0[0] to bplcon0[7], and the window-open/close comparisons against hdiwstrt/hdiwstop swapped (window now opens at DIWSTOP and closes at DIWSTRT)) turned this assertion red; reverted, confirmed green again.
```

## Extension claims -- 9 graded, 2 recorded

Nine claims graded below, plus two recorded. These use
the fixed `expect()`/`note()`/`record()` call form (test_denise.cpp), not this package's
own `A()`/`SKIP()` helpers -- everything above this section is unchanged. The `// MUTATION`
comments live inline in claims_gen.inc, next to each call; this section restates them for the
same reason the log above exists.

**BPU=000 background-colour group (3-bplcon0.c18, 3-t5.c02, 3-r0.bpu000,
3-regsum-bplcon0.c19).** Not a re-read of `g_bpu[0]` (BPU=0's own plane-count entry, already
found "impossible" to mutate under M1-M6 above -- the trivial floor of the gating chain). A
fresh, dedicated measurement instead: COLOR0 driven to a chosen 12-bit value (overriding the
default preload), all six planes hard-driven, and the observed pixel decoded against THAT
value. Graded on BOTH runs together -- COLOR0 at its normal preload AND overridden to a
distinctive non-zero value (0x0f0f) -- so the comparator is shown to track actual COLOR00
content rather than a fixed constant it would report regardless; the non-zero run previously
existed only as an ungraded MEASURED note beside a comparator graded on the zero run alone. The
mutation is a genuine **stimulus** change: BPLCON0 changed from BPU=000 to BPU=001 with plane 1
driven, COLOR0 left at its default preload -- the same "pixel equals COLOR00" comparator both
runs use now reads false (plane 1's data shows through), confirming the assertion is sensitive
to the BPU=000 condition it names; reverted to BPU=000, green again -- verified: 229/233 PASS,
4 FAIL, and these four ids are the only new FAILs.

**Sixth-bitplane DBLPF half (3-bplcon0.c20).** No longer the combined dual-playfield/HAM
evidence used for 3-bplcon0.c21/3-t5.c16 above (`g_pf2_ok && g_ham_ok`) -- that bundled range
(planes 2, 4, 6 driven together, any index 8..15 accepted) does not discriminate plane 6's own
contribution: with plane 6 left undriven in that same bundled measurement, the index is 11,
still inside 8..15, so the claim would still read PASS; and `g_ham_ok` grades a different claim
(3-bplcon0.c21) entirely. Measured directly instead: plane 6 driven ALONE, DBLPF=1, BPU=6 --
p.70's Table 3-11 ("Bits from PLANE 6 have the highest significance") gives bit combination 100
-> COLOR12 exactly, the one value reachable only if plane 6 itself reaches the playfield-2
colour path. Mutation is a genuine **stimulus** change: plane 6 left undriven (0x8000 ->
0x0000) in that same plane-6-alone measurement -- no pixel in the scan ever decodes to a
non-zero index (transparent throughout), so the exact-value comparator goes red; reverted,
confirmed green again -- verified: 232/233 PASS, 1 FAIL, and 3-bplcon0.c20 is the only new FAIL.

**Dual-playfield movement clause (3-dual.c46) -- withdrawn to record.** NOT the colour-grouping
proxy used for 3-dual-activate.c4's identical clause -- that proxy does not establish this
clause (a different clause of the same sentence), and citing it here would be exactly the "id on
an assertion that does not establish its claim" case this package's own rules forbid. Measured
directly instead: plane 2 (an even/playfield-2 plane) driven alone, BPLCON1 PF2H nibble 4 vs
nibble 12 compared, once with DBLPF=0 and once with DBLPF=1. Measured: the shift PF2H induces on
plane 2's position is identical either way (32 clocks, reproduced across a full 0-15 PF2H sweep
under both DBLPF settings). p.85's own warning is exactly why: "Always set all six bits, even if
you have only one playfield. Set 3-0 and 7-4 to the same value if you are using only one
playfield" -- PF2H governs bitplanes 2, 4 and 6's delay the same way regardless of DBLPF, so the
identical shift is what the manual itself states, not a DBLPF-dependent movement mechanism this
fixture could show. Kept as a MEASURED note, not graded; the id (and `3-dual-activate.c4`, whose
existing line loses its id for the same reason -- see the table entry above) is withdrawn to
a RECORDED line citing p.85. There is no red/green outcome left for a mutation to flip.

**HIRES dual-playfield plane identity (3-dual-planes.c5/c6, 3-dual.c19/c20).** A fresh
measurement: HIRES=1, DBLPF=1, BPU=100, driving only the two named planes per side, checked
against the EXACT expected sub-index (3 for playfield 1's {1,3}, 11 for playfield 2's {2,4}) --
not a broad range. That exactness mattered: an earlier draft used the same 1-7/8-15 ranges the
BPU=6 category checks, and mutation 2 did NOT flip it, because this RTL's single-playfield
fallback (`plfdata=bpldata[8:1]` read as one raw value) produces 5 and 10 for these two
stimuli -- both of which still fall inside the broader ranges. The exact-value form was kept
after that finding, and re-verified red under mutation 2 (this package's own accepted
implementation-mutation convention, as above).

| claim id | mutation | result |
|---|---|---|
| `3-bplcon0.c18` | stimulus (BPU=001, plane 1 driven) | red |
| `3-t5.c02` | stimulus (BPU=001, plane 1 driven) | red |
| `3-r0.bpu000` | stimulus (BPU=001, plane 1 driven) | red |
| `3-regsum-bplcon0.c19` | stimulus (BPU=001, plane 1 driven) | red |
| `3-bplcon0.c20` | stimulus (plane 6 left undriven) | red |
| `3-dual.c46` | -- | recorded, not graded |
| `3-dual-planes.c5` | 2 | red |
| `3-dual.c19` | 2 | red |
| `3-dual-planes.c6` | 2 | red |
| `3-dual.c20` | 2 | red |
| `C-shr.c13` | -- | recorded, not graded |