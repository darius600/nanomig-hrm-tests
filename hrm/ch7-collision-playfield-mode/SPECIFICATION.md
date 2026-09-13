# Specification: ch7-collision-playfield-mode

Fixture: `denise_collision`
Claims file: `ch7-collision-playfield-mode.jsonl` (8 claims, chapter 7 body p.214 and
appendix A p.273)

Written from the claim quotations alone, before the RTL was opened. Per
BUILD-BRIEF, this file names propositions and test intent only; it does not
name module port names or expected bit-level values beyond what the task
sheet itself supplied (the fixture's `dblpf` mode control, and "all fifteen
CLXDAT pairs" as the sweep domain — both given directly in
AUFTRAG-ch7-collision-playfield-mode.md, not derived by reading RTL).

## Why this package exists

Three existing packages between them cover single-playfield mode only
(`ch3-collision-registers`, dblpf pinned at 0) and dual-playfield mode only
(`ch4-sprite-collision`, one dblpf=1 stimulus). None of them measures the
*comparison* between the two modes. Page 214's sentence is exactly that
comparison, and it has not been asserted anywhere until this package.
`dblpf`'s value at power-up is a harness convention, not something the manual
specifies — so the assertion below is a RELATION between the two modes,
never an absolute claim about what either mode alone must read.

## Propositions and assertions

Each assertion below corresponds to one proposition drawn from the claims
file. Where two claims (one from reader `sol`, one from reader `opus`) quote
the same sentence, they are cited together on one assertion, per the
crossread pairing already recorded in the claims file (explicit for
`opus:7-cd.c26` / `sol:7-collision.c27`; the same pattern holds for the other
three pairs by identical quote and page).

### A1 — PLAYFIELD-MODE-INVARIANCE (p.214)

Claims: `sol:7-collision.c27`, `opus:7-cd.c26`

Quote (opus's restored, complete sentence; sol's is the same clause,
shortened): "Notice that in this table, collision detection does not change
when you select either single- or dual-playfield mode."

**Proposition:** for one held stimulus, the full CLXDAT collision result —
every one of the fifteen collision-pair bits described in Appendix A — must
read identically whether the harness drives `dblpf=0` (single-playfield) or
`dblpf=1` (dual-playfield).

**Form of the assertion:** a RELATION — `CLXDAT(stimulus, dblpf=0) ==
CLXDAT(stimulus, dblpf=1)` — never an absolute value for either mode, because
the manual does not specify a reset value for the mode select; only this
harness's convention does.

**Control case:** the held stimulus must be one that produces at least one
set collision bit in both modes (a real, staged collision), not an all-zero
stimulus — otherwise a data path that is silently dead would satisfy this
relation vacuously.

**Mutation:** feed different content (not different `dblpf`) into the two
runs being compared, so they are no longer "one held stimulus," and confirm
the equality assertion goes red; then revert. This proves the comparison
actually reads content and is not a tautology.

### A2 — BITPLANE-CONTENT-DETERMINES-COLLISION (p.214)

Claims: `sol:7-collision.c28`, `opus:7-cd.c27`

Quote: "Collision detection depends only on the actual bits present in the
odd-numbered or even-numbered bitplanes."

**Proposition:** this sentence asserts two things jointly — collision *does*
track bitplane content (it "depends" on it), and *nothing else* the fixture
can vary changes it (it depends "only" on it). A1 already covers the
exclusivity half with respect to playfield mode, which is this package's
assigned boundary. A2 covers the complementary, non-redundant half:
holding `dblpf` fixed, changing which bitplane bits are actually driven must
change the collision result.

**Form of the assertion:** a RELATION in the other direction —
`CLXDAT(stimulus_A, dblpf=k) != CLXDAT(stimulus_B, dblpf=k)` for two stimuli
that differ only in bitplane content, `dblpf` held fixed.

**Control case:** shares A1's staged-collision requirement; in addition, the
two compared stimuli must be chosen so the manual gives them different
expected outcomes (e.g. a bitplane pattern that participates in a collision
vs. one that does not), not merely different in some field that the manual
does not tie to CLXDAT.

**Mutation:** make the two stimuli's bitplane content identical (the
opposite of the assertion's precondition) and confirm the inequality
assertion now correctly goes red (since with identical content the two
CLXDAT reads should be equal); then revert.

### A3 — PLAYFIELD-1-IS-ODD-BITPLANES (p.273)

Claims: `sol:7-appA-clxdat.c6`, `opus:7-appA-clxdat.c3`

Quote: "Playfield 1 is all odd-numbered enabled bitplanes."

**Proposition:** wherever CLXDAT reports a playfield-1-to-sprite collision,
that report tracks the enabled odd-numbered bitplanes' content, and only
that.

**Why this package, not only ch3:** per the claims file's own prior-coverage
note, `ch3-collision-registers` already asserts this equation, but only with
one bitplane enabled and the opposite playfield left disabled — the
opposite playfield is vacuously matched and the mode gate this package
exists to test never engages. This package must re-measure the same
equation with both playfield 1 and playfield 2 populated with real, distinct
content simultaneously, swept across `dblpf=0` and `dblpf=1`. The claims file
records that under this harder condition, playfield 1's half of the equation
does not hold at one of the two `dblpf` settings — i.e. this assertion is
expected to disagree with the RTL at that setting, which is a valid, finished
red result, not something to adjust.

**Control case:** a stimulus where the odd bitplanes alone stage a genuine
playfield-1/sprite collision (even bitplanes clear, or vice-versa as a second
case), under the both-playfields-live condition above.

**Mutation:** swap which bitplanes (odd vs. even) carry the staged collision
content and confirm the assertion — that playfield 1 tracks odd, specifically
— goes red when an even-only pattern is (wrongly) checked against the
playfield-1 expectation; then revert.

### A4 — PLAYFIELD-2-IS-EVEN-BITPLANES (p.273)

Claims: `sol:7-appA-clxdat.c7`, `opus:7-appA-clxdat.c4`

Quote: "Playfield 2 is all even-numbered enabled bitplanes."

Mirrors A3 for the even-bitplane / playfield-2 side, under the same
both-playfields-live, `dblpf`-swept condition, with the same control-case and
mutation discipline.

## Harness precondition (not a claims-file proposition)

Before A1–A4 are graded, the fixture must prove its data path is alive:
distinct, known bitplane and sprite content driven in must arrive at CLXDAT,
in order, on the cycle they were fetched on. Chapter 6's salvaged-harness
lesson (a DMA grant sampled inside the same `eval()` as the request, making
every `data_out` read back zero) is the standing example of why this is
required before any data-dependent assertion is trusted. This precondition
is graded through `expect()` like every other check, but it is a harness
self-check, not one of the 8 claims, and is not counted against them.

## Claims accounted for

All 8 claims in `ch7-collision-playfield-mode.jsonl` map onto the four
assertions above, two claims each:

| Assertion | Claims |
|---|---|
| A1 | `sol:7-collision.c27`, `opus:7-cd.c26` |
| A2 | `sol:7-collision.c28`, `opus:7-cd.c27` |
| A3 | `sol:7-appA-clxdat.c6`, `opus:7-appA-clxdat.c3` |
| A4 | `sol:7-appA-clxdat.c7`, `opus:7-appA-clxdat.c4` |

No claim in this file was left unassertable.

## Chipset scope

All 8 claims classify as `applies_to: [OCS, ECS]` — appendix C is measured
silent on collision, so nothing here is OCS-only. No chipset-conditional
assertion is required.

## HALT conditions

None encountered while writing this specification. The claims file supplied
enough (CLXDAT as a 15-pair register, playfield 1/2 defined in terms of
odd/even enabled bitplanes, `dblpf` as the named mode control already given
by the task sheet) to state every proposition testably without reading the
RTL or knowing any other port name. If, once the RTL is opened, the
`denise_collision` port list does not expose a mode input compatible with
`dblpf` as described, or does not expose CLXDAT as a 15-bit-relevant read,
that is a HALT to report, not something to work around.
