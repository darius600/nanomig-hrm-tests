# Specification — `ch8-appb-copper-access-marks`

Package: `hrm/ch8-appb-copper-access-marks`
Fixture: `agnus_copper` (module `agnus_copper.v`, compiled alone)
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-appb-copper-access-marks.jsonl` (7 claims)

Subject: Appendix B's Copper-access marks on four `paula`-chip register rows
— ADKCONR ($010), SERDATR ($018), SERDAT ($030), SERPER ($032) — each row
carrying a leading `*`, composed with the legend that defines `*` on p.287.

This slice's own build brief (`work/2026-09-16_ch8_s4_build_brief.md`) names
this as the Chapter 8 lineage layer (`agnus_copper` exists in
`MiSTer-devel/Minimig-AGA_MiSTer` too), so a red here is a statement about the
Minimig line, not only about NanoMig.

## The two marks, quoted in full (p.287)

> `*    Address not writable by the Copper.`
— `docs/references/amiga-hrm-3rd/appB3/p287.txt:22`

> `~     Address not writable by the Copper unless the "copper danger bit",
> COPCON is set true.`
— `docs/references/amiga-hrm-3rd/appB3/p287.txt:24`

`*` carries no chipset qualifier and no exception of any kind. `~` names one
exception (COPCON's danger bit, CDANG) and no others — in particular neither
mark mentions the ECS/OCS chipset distinction. The two glyphs are visually
and typographically distinct on the printed page, and the appendix places
each register under exactly one of them.

## The four rows (p.289)

> `ADKCONR      *010   R           P    Audio, disk control register read`
— `p289.txt:17`

> `SERDATR      *018   R           P    Serial port data and status read`
— `p289.txt:21`

> `SERDAT       *030   W           P    Serial port data and stop bits write`
— `p289.txt:33`

> `SERPER       *032   W           P    Serial port period and control`
— `p289.txt:34`

All four carry `*`, not `~`, and none carries the ECS mark `(E)` that other
rows on the same page do (e.g. `VPOSR *004 R A (E)`, `p289.txt:11`). Composed
with the p.287 legend, each row states: this address is not writable by the
Copper, unconditionally, with no chipset and no COPCON exception named.

For contrast, quoted for use as this package's control (not one of the 7
claims — see *Control case* below):

> `BLTCON0      ~040   W     A          Blitter control register 0`
— `p289.txt:41`

BLTCON0 carries `~`: writable by the Copper once COPCON's danger bit is set,
by the legend's own words, with no chipset qualifier either.

## Claim inventory (all 7)

| # | uid | register | page | restatement |
|---|---|---|---|---|
| 1 | `sol:8-serdat.c78` | SERDAT $030 | 289 | "The asterisk marks the SERDAT address as not writable by the Copper." |
| 2 | `sol:8-serdatr.c145` | SERDATR $018 | 289 | "The asterisk marks the SERDATR address as not writable by the Copper." |
| 3 | `sol:8-serper.c33` | SERPER $032 | 289 | "The asterisk marks the SERPER address as not writable by the Copper." |
| 4 | `opus:8-regsum.c7` | SERDATR $018 | 289 | "The row carries the mark `*` ... The conditional glyph `~` and the ECS marker `(E)` ... do not appear on this row." |
| 5 | `opus:8-regsum.c13` | SERDAT $030 | 289 | same shape as #4, SERDAT's row |
| 6 | `opus:8-regsum.c19` | SERPER $032 | 289 | same shape as #4, SERPER's row |
| 7 | `opus:8-regsum.c25` | ADKCONR $010 | 289 | same shape as #4, ADKCONR's row |

Claims 1/4 (SERDAT), 3/6 (SERPER) and 2 alone (SERDATR, since its narrower
twin is claim 4) are two independent readings of the same printed fact: the
"serdat/serdatr/serper" reading lot restated the row's composed meaning
directly ("not writable by the Copper"); the "regsum" reading lot restated
only the glyph's identity against its two neighbours (`~`, `(E)`) without
asserting what the glyph means. Both are real, distinct claims with their
own uids and both need a home below.

## Already graded elsewhere — corrected from the claims file's own notes

Two claims carry a `classify.note` in the claims file naming an accepted
package. Both notes were checked against the accepted corpus directly before
being trusted (per this project's standing rule that a `classify.note` is
where a claim was slated to go, not proof it got there), and **one of the
two package names in the claims file is wrong** — see *What turned out to be
wrong* below for the correction and how it was found.

**`sol:8-serdatr.c145`** (SERDATR's `*` mark, full "not writable" reading).
The claims file's own note names `hrm/ch8-appb-serial-register-rows`. That
package's own `SPECIFICATION.md` (lines 59–61) reads: *"The '\*' mark, out of
scope for the same reason ... No claim's `restatement` addresses what that
mark means, so nothing here tests it either."* — it does not grade this
claim. The uid pair the note actually describes (`opus:8-appB.c37` /
`sol:8-appB.c36`, "the \* mark" on SERDATR) is real and traced, in the s1
classified corpus (`2026-09-14_ch8_s1_classified_c5-appB.jsonl`), to
`"package": "hrm/agnus_copper_move_sweep"` — and that uid pair is cited,
verified directly in the built test source, on
`hrm/agnus_copper_move_sweep`'s two ecs=0 sweep assertions:

    SWEEP ecs=0 cdang=0 vs spec Reading B (HRM_CONFORMANCE, primary oracle)
    SWEEP ecs=0 cdang=1 vs spec Reading B (HRM_CONFORMANCE, primary oracle)

Both PASS in that package's own accepted run: at ecs=0 (OCS), SERDATR is
correctly blocked in both CDANG states, matching the unconditional `*`
reading. `sol:8-serdatr.c145` is put on a `record()` line citing these two
assertions instead of the claims file's own citation.

**`opus:8-regsum.c25`** (ADKCONR's `*` mark). The claims file's own note says
only "subject of slice s2" and names the uid pair
`opus:8-appB-adkconr.c2`/`sol:8-appb-adkconr.c2` without a package. That pair
was independently traced the same way: `sol:8-appb-adkconr.c2` is cited on
`hrm/agnus_copper_move_sweep`'s `SWEEP ecs=0 cdang=0` assertion (same label
as above). `opus:8-regsum.c25` is put on a `record()` line citing that
assertion.

**The other five claims' composed content is also inside `Reading B`'s
citation lists** (`opus:8-appB.c42`/`sol:8-appB.c41` for SERDAT,
`opus:8-appB.c47`/`sol:8-appB.c46` for SERPER — both s1 uids, both traced to
`agnus_copper_move_sweep` the same way) — but only at ecs=0. This package
does not re-cite those s1 uids (they are not this claims file's uids to
carry), and it does not re-grade the ecs=0 corners: see *Why this package is
scoped to ECS* below.

## Why this package is scoped to ECS

`agnus_copper_move_sweep`'s two ecs=0 sweeps grade the full 256-address
accepted set against "Reading B" (Appendix B, p.287+p.289) and PASS — the
`*` mark holds, unconditionally, at ecs=0, for every row in this package's
lot. Its two ecs=1 sweeps grade the same 256-address set again, but against
a **different** oracle — Appendix C (p.310, "the Copper can access all of
the Amiga chip registers") — and their citation lists name only chapter-2 /
Appendix-C claims, never an Appendix-B row uid. The comment above those two
assertions in that package's own source records this explicitly as a
correction: an earlier version graded the ECS rows against an invented
chapter-2 rule and put a red in the wrong place; grading them against
Appendix C fixed that.

**What that leaves genuinely open**: nobody has graded, for these four
specific `*`-marked rows, whether Appendix B's own unconditional legend
survives at ECS=1/CDANG=1 — the corner where `agnus_copper.v:225`'s decode
(`ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs`) admits the whole
$000–$1FE range regardless of the `*`/`~` distinction. Appendix C's "all
chip registers" sentence is a general permission that this package's reading
kit does not include (p.310 is not among the pages this slice was assigned:
`work/2026-09-16_ch8_s4_build_brief.md`'s page list stops at p.289/p.287)
and that, on its own wording, says nothing about whether it overrides a
register-specific unconditional exception stated elsewhere in the same
appendix. This package grades Appendix B's own statement, on its own terms,
at the corner no accepted package has put it to: ECS=1, both CDANG states.

This is why five of the seven claims are graded here at ECS only, not at
OCS: the OCS corners are already measured and accepted (cited above), and
re-asserting them under fresh uids risks exactly the outcome the build brief
warns against — this slice reaching a different verdict from an accepted
one on the same printed row. The two ECS corners are not covered by any
accepted package under any oracle that reads Appendix B.

## Related history checked before writing this up

**NM-HRM-044** (`docs/findings/NM-HRM-044.md`) is the closest precedent: the
manual permits Copper initialisation of DSKPTH ($020, also `*`-marked, same
decode mechanism) on p.279 and forbids it on p.289; the entry's own status is
"source observation," not a graded conformance defect, specifically because
the contradiction is between *two manual passages*, and "no implementation
can settle... which of two printed sentences describes the hardware." That
entry explicitly separates itself from `NM-HRM-004` ("a different claim,
register and page") and explicitly declines to grade the ECS+CDANG=1 corner
for DSKPTH, calling it `NM-HRM-004`'s territory.

**This package's claim is not the same shape as NM-HRM-044's**, and the
difference matters: DSKPTH has an Appendix-A sentence (p.279) that
*affirmatively permits* Copper writes, giving the manual a second voice to
contradict the first. Checked directly: p.265 (ADKCON/ADKCONR) and p.283
(SERDAT/SERDATR/SERPER) — this slice's own Appendix-A pages for these four
registers — say nothing at all about Copper access, in either direction.
There is no second manual passage for SERDAT/SERPER/SERDATR/ADKCONR playing
p.279's role. What stands opposite Appendix B's `*` here is Appendix C's
*general* COPCON sentence, not a register-specific permission — a weaker,
differently-shaped tension than NM-HRM-044's. **This package therefore
grades Appendix B's statement directly, rather than filing a source
observation that declines to grade**, and flags the Appendix-C tension in
prose (above) for whoever adjudicates it.

**p.289's own marks were read in slice s1**, in
`2026-09-14_ch8_s1_classified_c5-appB.jsonl` — that is the corpus this
package traced `opus:8-appB.c36/.c37/.c41/.c42/.c46/.c47` back to, and it is
where the composed `*`-mark propositions for these same four rows were
first classified as testable and routed to `agnus_copper_move_sweep`, not to
`ch8-appb-serial-register-rows`.

**`NM-HRM-004`** (`docs/findings/NM-HRM-004.md`) is a different corner of the
same RTL line entirely: it is about the `~`-marked range ($040–$07E) at
ECS=1/CDANG=**0**, where Appendix C states the Copper should reach that range
and the RTL requires CDANG regardless. This package's control register,
BLTCON0, sits in that range — see *Control case* — but this package does not
graded-assert BLTCON0 at ECS=1/CDANG=0 as a finding; it uses that exact
corner only as a **matches-print** control point (BLTCON0 correctly blocked
there under this package's own oracle, `~`'s COPCON-only rule — `NM-HRM-004`
is about the *Appendix-C* reading of that same corner, a different oracle,
already tracked elsewhere).

## Propositions

### P-SERDAT — SERDAT is not writable by the Copper [`sol:8-serdat.c78`, `opus:8-regsum.c13`]

> `SERDAT       *030   W           P    Serial port data and stop bits write`
composed with the p.287 `*` legend.

Testable form, scoped to ECS (see above): a Copper MOVE targeting $030,
executed with `ecs`=1, reaches the register when `cdang`=1 and does not when
`cdang`=0.

Expected, per the unconditional legend: **blocked in both CDANG states.**

- ECS=1, CDANG=0: **PASS** expected (matches the unconditional reading; also
  serves as this proposition's own control point).
- ECS=1, CDANG=1: **RED** expected — this is where S4-C5 predicted a red.

### P-SERPER — SERPER is not writable by the Copper [`sol:8-serper.c33`, `opus:8-regsum.c19`]

> `SERPER       *032   W           P    Serial port period and control`
composed with the p.287 `*` legend. Same testable form and same expected
shape as P-SERDAT, at $032.

### P-SERDATR — SERDATR is not writable by the Copper [`opus:8-regsum.c7`]

> `SERDATR      *018   R           P    Serial port data and status read`
composed with the p.287 `*` legend. Same testable form and same expected
shape as P-SERDAT, at $018.

(`opus:8-regsum.c7`'s own restatement is the narrower glyph-identity form —
"carries `*`, not `~`, not `(E)`" — but its own `classify.evidence` already
composes that with the p.287 legend and the same `agnus_copper.v:225` decode
used for the other six claims, and grades it `testable`/`observable: yes, at
agnus_copper` on that composed basis. Graded here as the composed
proposition, on the fresh ECS ground SERDATR's own full-reading twin
(`sol:8-serdatr.c145`) does not reach because that twin is routed to
`record()` above.)

## Control case

**BLTCON0 ($040, `~`-marked)**, at the same two corners (ECS=1, CDANG=0 and
CDANG=1): expected **blocked at CDANG=0, admitted at CDANG=1** — matching
`~`'s own conditional legend exactly, with no ECS term in that legend either.

This is the control the build assignment named directly: on the identical
fixture, at the identical (ECS, CDANG) corners used to grade P-SERDAT /
P-SERPER / P-SERDATR, a `~`-marked register's conditional behaviour is
graded against its own rule and comes out green in both corners. Without it,
a red on the `*`-marked rows would not survive the objection that this
fixture cannot tell the two marks apart — it would be equally consistent
with a fixture that reports "admitted" for every register once CDANG is set,
mark or no mark. BLTCON0 at CDANG=0 (still blocked, like the `*` rows) and
at CDANG=1 (admitted, unlike the `*` rows) rules that out: the same
measurement, on the same fixture, produces the row-appropriate answer for a
register whose printed rule is genuinely conditional.

## Fixture liveness (precondition)

**DMACON ($096)**, an address outside every protected range at any
(`ecs`,`cdang`) combination (`ir2[8:7]` = `01`, `ir2[8:6]` = `010`; neither
decode term in `agnus_copper.v:225` can ever set `illegalreg` for it):
a Copper MOVE reaches it at ECS=1 with CDANG=0 and with CDANG=1. Without this,
a "blocked" result anywhere above would be indistinguishable from a harness
that cannot make the Copper write anything at all under these corners.

## What this specification deliberately does not contain

Port names, signal names and concrete stimulus/expected values are step-3
material, added only once this file is committed and the RTL is opened for
that purpose — per the build brief's order of work. **One exception, and it
is disclosed rather than hidden**: locating the *correct* package/assertion
for the two `record()`-routed claims (`sol:8-serdatr.c145`,
`opus:8-regsum.c25`) required opening `agnus_copper.v` (to check the decode
line numbers the classify evidence already cited) and reading
`hrm/agnus_copper_move_sweep`'s and `hrm/agnus_copper_move_range`'s test
sources and the s1/s2 classified corpora, before this file was committed —
because the claims file's own note for `c145` named a package that, checked
directly, does not test the claim at all (see *Already graded elsewhere*
above). That reading informed which package to cite and how to scope P-SERDAT
/ P-SERPER / P-SERDATR away from already-graded ground; it did not supply
any oracle value in this file — every expected value above still comes from
p.287/p.289 alone. Flagged here per the build brief's own instruction rather
than papered over.

## Claims accounted for

All 7 claims are accounted for: P-SERDAT (2: `sol:8-serdat.c78`,
`opus:8-regsum.c13`), P-SERPER (2: `sol:8-serper.c33`,
`opus:8-regsum.c19`), P-SERDATR (1: `opus:8-regsum.c7`), and two on
`record()` lines (`sol:8-serdatr.c145`, `opus:8-regsum.c25`).

## Mutation plan (per assertion, replayable via `-D`, see BUILD-BRIEF)

- **LIVENESS** (green baseline): stimulus — retarget the liveness MOVE from
  DMACON to SERDAT under CDANG=0 (still ECS=1); expect red (SERDAT is blocked
  there), showing the liveness check is not vacuously green.
- **CTRL-BLTCON0** (green baseline): stimulus — invert which CDANG value is
  expected blocked-vs-admitted for BLTCON0; expect red.
- **SERDAT-CDANG0** (green baseline, the control half of P-SERDAT): stimulus
  — drive CDANG=1 instead of 0 (still ECS=1); expect red (this is P-SERDAT's
  own red half).
- **SERDAT-CDANG1** (red baseline, the finding): stimulus — drive CDANG=0
  instead of 1 (still ECS=1); expect green, proving the comparator is not
  stuck red.
- **SERPER-CDANG0** / **SERPER-CDANG1**: same shape as SERDAT's pair, at $032.
- **SERDATR-CDANG0** / **SERDATR-CDANG1**: same shape as SERDAT's pair, at
  $018.

Every mutation is a stimulus change (which CDANG value is driven, or which
register is targeted), never an oracle change — the expected outcome text is
never edited by a mutation, only which measured value it is compared
against.
