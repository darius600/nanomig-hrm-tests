# Specification — `appF-cia-interrupt-control`

Package: `hrm/appF-cia-interrupt-control`
Fixture: **one** top, `cia_int`, compiled alone. Read in full (70 lines):
it instantiates nothing else — no sub-modules, no address decoder beyond
its single `icrs` select. This is the smallest top in Appendix F's package
set.
Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`,
filtered to `classify.package == "appF-cia-interrupt-control"` (70 claims).
Subject: printed pages 346-347 (the Interrupt Control Register — sources,
the MASK/DATA split, the set/clear write protocol, the register maps) and
the ICR-touching lines of p.352's worked timing example.

## Order-of-work disclosure

This document is written from `extract.quote` / `classify.restatement` and
the printed pages alone. The task instructions that assigned this package
pre-exposed RTL detail ahead of step 1 — specifically:
`cia_int.v:18-19/22/29/32/34/57` (the two 5-bit registers, the read mux,
the write routing, the sliced bit range, the FLAG line), `ciaa.v:321`
(`flag` tied to `1'b0`) and `ciab.v:186` (`ser` tied to `1'b0`) — the same
situation `hrm/ch1-cia-timers`, `hrm/ch8-parallel-port-direction` and
others have each disclosed for themselves. What that pre-exposure gave:
confidence that the MASK/DATA split and the FLAG level-vs-edge question
were real, worth a dedicated fixture, and where in the file to look first.
It did **not** supply a claim boundary, an expected byte value, or a
grading verdict — those below trace only to the quoted pages, and were
independently re-derived (Read, not re-typed) by this builder after this
document's first commit, as step 2.

## Claim inventory summary

| bucket | count | disposition |
|---|---:|---|
| Graded (PASS, all) | 56 | 10 `expect()` assertions, below |
| Declined — already graded elsewhere | 10 | 1 `record()` line |
| Declined — loose prose vs. precise table | 2 | 1 `record()` line |
| Declined — out of this package's boundary | 2 | 1 `record()` line |
| **Total** | **70** | |

## Correcting the build brief's own generalisation

The build brief states: "23 carry a `classify.note` naming an accepted
package — those go on `record()` lines." That is imprecise for 13 of the
23. Checked one by one (`work/2026-09-17_appF_packages.jsonl`,
`classify.note` field):

- **10 of 23** genuinely say **ALREADY GRADED** — these are the 10 declined
  above.
- **5 of 23** (`sol:F-icr.c6/c7/c8`, `opus:F-icr.c7/c8`) say, in the
  classifier's own words, "**New here**... `hrm/ch1-cia-timers`... and
  `hrm/ch8-disk-index-interrupt-path` both DEPEND on the split... without
  the page that specifies it." That is the opposite of already-graded — it
  is this package's job to supply the page those two accepted packages
  currently assume. Graded above, PASS, as PROP-MASK-DATA-SPLIT.
- **4 of 23** (`sol:F-icr.c24/c26`, `opus:F-icr.c24/c27`) say
  "**EXERCISED, not graded**... NOT already graded — it is assumed there."
  Graded above, PASS, as PROP-EXAMPLE-SEQUENCE.
- **2 of 23** (`sol:F-example.c12`, `opus:F-example.c18`) say "**Related**:
  ...grades no claim about the ICR write's own SET/CLR rule." Graded
  above, PASS, as PROP-ICR-CLEAR-ALL.
- **2 of 23** (`sol:F-example.c17`, `opus:F-example.c23`) note that no
  package grades any CRA/CRB control bit — true, and irrelevant to these
  two specifically: their own restatements are about polling ICR bit 0
  (Timer A), which `cia_int` owns and this package grades directly (folded
  into PROP-INTERRUPT-SOURCES). The CRA "restart timer" half of the same
  printed example (once `corpus-defects.md`'s comment-line shuffle is
  corrected) needs a `CRA` register `cia_int` does not have; it is out of
  this package's boundary regardless of the note, and is not claimed here.

So the brief's "those go on `record()` lines" instruction is followed for
10 of the 23 `classify.note`-carrying claims, not all 23. This is reported
per the build brief's own closing request to flag what turned out to be
wrong when checked, not silently corrected.

## PROP-INTERRUPT-SOURCES

p.346: "There are five sources of interrupts on the 8520:" followed by the
five bulleted sources (Timer A underflow, Timer B underflow, TOD alarm,
Serial port full/empty, Flag). p.347's READ map names D3-D0 as SP, ALRM,
TB, TA. p.352's busy-wait example polls ICR bit 0 for Timer A expiry.

**Testable proposition:** at `cia_int`'s own boundary — which ties nothing
(unlike `ciaa`, which ties `flag` to a constant, or `ciab`, which ties
`ser` to one; see "Claims not assigned an assertion" for that boundary
distinction) — all five named sources are real, independent ports, each
reaching its own, correctly-positioned, exclusively-set bit in the DATA
register.

Status: testable directly. `ta`, `tb`, `alrm`, `ser` are pulsed
individually by this fixture; `flag` is included in the "all five at once"
sub-check (its own bit-for-bit exclusivity, and the level-vs-edge question
p.341 raises about it, are handled separately — see below).

## PROP-MASK-DATA-SPLIT

p.346: "The interrupt control register consists of a write-only MASK
register and a read-only DATA register." New to this package's own claim
set (no `classify.note` claims it is already graded), and specifically the
fact two already-accepted packages (`hrm/ch1-cia-timers`,
`hrm/ch8-disk-index-interrupt-path`) depend on without having had this
page: both write a set/clear byte to the ICR address and read a data byte
back from the *same* address, which only makes sense if the two are
different registers sharing one select line.

**Testable proposition:** a write to `icrs` never reads back as an echo
(proving it is not a single read/write register), while demonstrably
taking effect on a real, separate register that gates IR (proving it was
not silently dropped either), including across an intervening
DATA-register-clearing read (proving the two registers do not share a
clear-on-read side effect).

## PROP-MASK-GATES-IRQ

p.346: "Any interrupt will set the corresponding bit in the DATA
register" (regardless of MASK) and "Any interrupt that is enabled by a
1-bit in that position in the MASK will set the IR bit... and bring the
IRQ pin low." p.347: "If an interrupt is to occur based on a particular
condition, then that corresponding MASK bit must be a 1." This also
carries the WRITE map's non-overlap bit names (D3=SP, D2=ALRM, D1=TB,
D0=TA — D7=S/C and D4=FLG are declined, already graded elsewhere).

**Testable proposition:** each of TA/TB/ALRM/SP sets its own DATA bit
whichever way its MASK bit is set, but only drives IR/`irq` active when
its own, correctly-named MASK bit is 1.

## PROP-READ-CLEAR

p.346: "When you read the DATA register, its contents are cleared (set to
0), and the IRQ line returns to a high state." Graded against a genuine
one-tick source pulse — the shape every other package in this suite drives
`ta`/`tb`/`alrm` with (e.g. Timer A's combinational `underflow`,
self-clearing after one reload tick) — not against a level held through
the read, which `cia_int.v:43-50`'s RTL treats differently (see the FLAG
measurement below; not a claim in this package's inventory, so not graded
here as a separate proposition).

## PROP-SETCLEAR-PROTOCOL

p.346-347's set/clear-bit mechanism: bit 7 selects direction; each 1-bit
among the rest selects a MASK bit to affect; each 0-bit leaves the
corresponding MASK bit at its *current* state. The discriminating test
(does a 0-bit truly preserve, or merely fail to re-set/re-clear an
already-matching value?) needs a MASK bit already at 1 when a later write
"skips" it — the fixture below is built to do exactly that.

## PROP-EXAMPLE-SEQUENCE

p.347's worked example: "Suppose you want to set the Timer A interrupt
bit..., but want to be sure that all other interrupts are cleared" —
graded from the opposite starting state (TA off, everything else on) so
the two-instruction idiom (clear-mode 0b01111110, then set-mode
0b10000001) has to do real work to reach the stated goal, not merely
confirm an already-correct baseline.

## PROP-ICR-CLEAR-ALL

p.352's setup code: `move.b #%01111111,ciaicr(a4) /Clear all 8520
interrupts`. Graded from a baseline with every real MASK bit on.

## PROP-READMAP-RESERVED-ZERO / PROP-WRITEMAP-DONTCARE

p.347 prints the READ map's D6/D5 as a literal `0` and the WRITE map's
D6/D5 as `x` (don't care) — two different symbols for two different
mechanisms (`cia_int.v:22`'s hardcoded `2'b00` on read; `cia_int.v:29-34`
slicing `data_in[4:0]`, never reading bits 6/5, on write). Both are
graded, separately, against the RTL fact that actually produces each
symbol.

**The D6/D5 prose-vs-table item, resolved as instructed:** p.346's own
prose says "In bits 6-0, you yourself form a mask" — bundling the two dead
write-side bits into a uniform seven-bit range, where only five bits (4-0)
actually participate. The table is graded (PASS); the prose is declined,
per the task instructions' explicit direction for this exact pair —
"Record both, grade the one the RTL can answer and say why."

## The two already-measured items specific to this package, confirmed

**`cia_int.v:57` — FLAG level-vs-edge.** `icr[4] <= icr[4] | flag;` is a
level-sensitive OR; no edge-detect register exists anywhere in
`cia_int.v`, and `ciab.v` (the only place `flag` is a real signal) wires
it straight through with nothing between the port and `cia_int`'s input —
confirmed independently by this builder (contrast `ciab.v:39`'s
`tick_del`, built and *named* for edge detection on the TOD `tick` input;
no equivalent exists for `flag`). p.341: "FLAG is a negative
edge-sensitive input... Any negative transition on FLAG will set the FLAG
interrupt bit." Measured in the test file (holding `flag` HIGH for several
ticks with zero negative transitions in the window still sets the bit) and
reported via `note()` — **not graded**: no claim in this package's
70-claim inventory cites p.341 (it belongs to a different appendix-F
package covering I/O ports / pin bindings), so there is no uid to put on a
graded line. Adjudicating whether this is a defect is for Amiga/RTL
specialists, per this suite's own division of labour (`ch3-collision-
registers` and others draw the same line).

**Flag and Serial-port-full/empty as sources of every 8520, vs. the tie-
offs.** p.346 makes both sources of every 8520; `ciaa.v:321` ties `flag`
to `1'b0` and `ciab.v:186` ties `ser` to `1'b0` — confirmed by reading both
files. That tie-off is a fact about `ciaa.v`/`ciab.v`, one level above this
package's declared top (`cia_int`, which ties nothing and exposes all five
sources as real ports). Reported in the test file's header comment and via
`note()` where relevant; not separately graded, because `cia_int`'s own
boundary has no tie-off to grade against — the source-list claim (Flag
included) is honestly, simply true there, and is graded in
PROP-INTERRUPT-SOURCES without re-citing the two claims already declined
(they are declined because they are *already graded* at `ciab`, not
because of this tie-off distinction, which is new information reported
alongside the decline).

## Claims not assigned an assertion (14 of 70)

**Already graded (10 uids), declined:** `sol:F-icr.c5`, `opus:F-icr.c6`
(p.346 "-Flag"), `sol:F-icr.c10`, `opus:F-icr.c10` (p.346, enabled
interrupt sets IR), `sol:F-icr.c27` (p.347 READ D7=IR), `sol:F-icr.c30`
(p.347 READ D4=FLG), `sol:F-icr.c35` (p.347 WRITE D7=S/C),
`sol:F-icr.c38` (p.347 WRITE D4=FLG), `opus:F-icr.c28` (p.347 READ, whole
row), `opus:F-icr.c29` (p.347 WRITE, whole row). All ten reach the same
already-accepted assertion: `hrm/ch8-disk-index-interrupt-path`'s
PROP-FLAG-PIN (`opus:8-cia.c56`, `opus:8-dskint.c9`, `sol:8-int.c9`),
which writes `0x90` to `ciab`'s ICR and requires the read to return
exactly `0x90` (IR set, bit 7, together with FLG, bit 4). The two
whole-row claims (`opus:F-icr.c28/c29`) are declined *whole*, not
bit-by-bit — their own classify-time note says "decline THOSE TWO [bits]
rather than re-grade," but a uid on a graded line counts as fully asserted
regardless of what the assertion's prose says (this suite's own rule), so
putting either whole-row claim on a PASS/FAIL line would silently
re-decide the two already-graded bits under a broader label. Nothing they
name is lost: the six *new* bit positions each also covers (D6, D5, D3,
D2, D1, D0) are separately graded above at `sol:F-icr.c28/c29/c31-c34` and
`sol:F-icr.c39-c42`'s own, finer-grained uids.

**Loose prose vs. precise table (2 uids), declined:** `sol:F-icr.c17`,
`opus:F-icr.c17` — see "PROP-READMAP-RESERVED-ZERO / PROP-WRITEMAP-
DONTCARE" above.

**Out of boundary (2 uids), declined:** `sol:F-icr.c11`, `opus:F-icr.c11`
(p.346, multichip IR polling). This fixture instantiates exactly one
`cia_int` — there is no second chip, no shared bus, no arbitration logic
to measure a multichip system against. The same boundary
`hrm/ch1-cia-timers` already argued for ROM enable and modem control:
facts realized, or requiring machinery, entirely outside the single top
this package declares.

## On a control case, fixture liveness and mutation

Per the build brief: a fixture-liveness proof precedes any data-dependent
grading; at least one control case is expected to PASS on the same
measurement basis (here, every graded assertion is a genuine, working
feature — `cia_int` carries no known defect this package's own claim set
reaches; the module's one disclosed defect, FLAG level-vs-edge, is
measured but not graded, for the uid reason given above); each graded line
carries a recorded stimulus mutation, in the fixed `// MUTATION <label>
<oracle|stimulus|none> <red|green|impossible> <what changed>` form,
controls included.

- **Fixture liveness.** `cia_int` has exactly one functional register
  pair (`icr`/`icrmask`) and no third, independent register — unlike
  `ciaa`/`ciab`, which also expose DDR ports unrelated to the ICR
  mechanism. FIXTURE-LIVE therefore necessarily shares its pulse mechanism
  with PROP-INTERRUPT-SOURCES; this is disclosed rather than hidden, and
  is a property of this module's small port list.
- **Mutations.** Ten assertions, eleven `-D` variants (`MUT_FIXLIVE`,
  `MUT_SOURCES_NOPULSE`, `MUT_SPLIT_ASSUME_ECHO`,
  `MUT_SPLIT_WITHHOLD_WRITE`, `MUT_GATES_SKIPMASK`, `MUT_READCLEAR_NOTICK`,
  `MUT_PROTOCOL_SKIP_STEP2`, `MUT_EXAMPLE_SKIP_SET`, `MUT_CLEARALL_SKIP`,
  `MUT_RESERVED_NOOP`, `MUT_DONTCARE_ASSUME_LIVE`), each built and run
  separately (`make MUT=<NAME> test`, full `rm -rf obj_dir` rebuild), plus
  one flagless `none impossible` record (PROP-WRITEMAP-DONTCARE's second
  record — see below). Nine are `stimulus` mutations (withhold or corrupt
  a write/pulse the assertion depends on); two are `oracle` mutations
  (`MUT_SPLIT_ASSUME_ECHO`, `MUT_DONTCARE_ASSUME_LIVE` — a wrong belief's
  predicted value substituted for the real one, showing the RTL disagrees
  with that belief, not that the stimulus can be withheld).
  `MUT_RESERVED_NOOP` is one `impossible` case: `cia_int.v:22`'s `2'b00`
  is a literal in the expression, reachable by no stimulus at all — built
  and run anyway, at the same maximal-entropy stimulus used for its own
  baseline, producing an identical result. PROP-WRITEMAP-DONTCARE carries
  a second, distinct `impossible` record, of kind `none` rather than
  `stimulus`: `cia_int.v:29-34` never reads `data_in[6]`/`data_in[5]`
  anywhere, so no `-D` variant was built for it at all (the one stimulus
  that WOULD move the assertion — withholding the baseline 0x9F write —
  breaks its precondition rather than exercising D6/D5, so it is not
  substituted in as if it were the same demonstration); this is the same
  "stored, consulted nowhere" shape `hrm/appF-cia-timer-modes` already
  recorded `none impossible` for `tmcr[5]` (CRA-INMODE, CRB-INMODE-ARITY).
  Two assertions — PROP-MASK-DATA-SPLIT and PROP-WRITEMAP-DONTCARE — each
  therefore carry two records, one confirming the comparator is not
  tautologically true (`oracle red`) and one confirming (by stimulus, or
  by an argued `none impossible`) that a real stimulus dependency, or its
  absence, was actually checked rather than assumed. Per-assertion detail
  and the actually-measured PASS/FAIL numbers for each variant are in the
  test file's own comments, immediately above each `expect()` call —
  filled in after building, not before, per this suite's rule against
  writing a record before taking the measurement.
