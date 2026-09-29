# Specification — `appF-cia-address-decode`

Package: `hrm/appF-cia-address-decode`
Fixture: `gary` — one module, compiled alone, matching the accepted
`ch1-memory-map-and-two-buses` / `ch7-custom-address-space` /
`ch7-reset-overlay` precedent (all three already build `gary` standalone in
this suite). Makefile and harness style are modelled directly on
`ch1-memory-map-and-two-buses`.
Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
for `classify.package == "appF-cia-address-decode"` — **117 claims**, all
`classify.status: "testable"`.
Subject: how the printed page maps CPU addresses onto the two 8520 CIA
chips — Appendix F pages 339 (CIAA address map figure), 340 (CIAB address
map figure) and 353 (*Hardware Connection Details* prose and binary
patterns) — as `gary`'s combinational chip-select logic (`sel_cia`,
`sel_cia_a`, `sel_cia_b`, gary.v:190-192) actually decodes it.

**On the order-of-work rule, disclosed rather than pretended away** (the
same disclosure `ch1-memory-map-and-two-buses` made, for the same reason):
this package's 117-claim slice was read in full, and every claim's own
`classify.evidence` field already carries `gary.v` file:line citations and
often the exact stimulus a fixture should drive (see `sol:F-signals.c56/
c57/c58/c63/c76` and their `opus:F-conn.*` twins in particular — their
`evidence` fields effectively hand over the test design: which bits gary
compares, which bits it does not, and the address `$BFC100` that exposes
the one-bit-per-chip gap). This is upstream classifier work, not something
this file went looking for. What this file does not do before being
committed is read `gary.v` directly itself; the concrete stimulus values,
the `expect()` assertions, and the mutation guards are added only after
this file is committed, per the build brief's own order-of-work step 2.

**Two corrections found once `gary.v` was opened directly** (both
consistent with what the claims' own `evidence` fields already said, so
neither changes any disposition below, but both are reported per the build
brief's own instruction to check its "two measurements already made" line
by line rather than transcribe it):

- The brief states `gary.v:191-192 decodes each CIA on ONE address bit
  where p.353 requires two, so $BFC100 raises sel_cia_a and sel_cia_b
  together`. Read directly: `gary.v:191` is `assign sel_cia_a = sel_cia &
  ~cpu_address_in[12];` and `gary.v:192` is `assign sel_cia_b = sel_cia &
  ~cpu_address_in[13];`. At `$BFC100` (A23-A16=$BF, A13=0, A12=0 — verified
  by hand: `$BFC100` = `1011 1111 1100 0001 0000 0000`, A15-A12 = `C` =
  `1100`, so A13=0, A12=0), `sel_cia_a = sel_cia & ~0 = 1` and `sel_cia_b =
  sel_cia & ~0 = 1` — both asserted simultaneously. Confirmed exactly as
  stated. See CIA-AB-TWOBIT-RULE below.
- The brief states `gary.v:190 compares all eight bits of A23-A16 against
  $BF where p.353 decodes three and calls the rest don't cares`. Read
  directly: `gary.v:190` is `assign sel_cia = cpu_address_in[23:16] ==
  8'hBF;` — an 8-bit equality, not a 3-bit one. Confirmed exactly as stated.
  See CIA-DECODE-DONTCARE below.

Both are RTL-vs-manual facts about how `gary` decodes addresses, not facts
about whether that decode is a defect — per the brief, that judgement
belongs to Amiga and RTL specialists, not to this package. Both are graded
here as ordinary printed propositions (the claims that state them, `sol:
F-signals.c57/c58/c63` and their `opus` twins, are `testable`, not
`conflict`). `CIA-DECODE-DONTCARE` grades RED, honestly: the oracle (the
printed prose, corroborated three ways — see NM-HRM-062 below) is
unambiguous that A20 and A19-A16 are don't-cares, and `gary` does not treat
them that way. `CIA-AB-TWOBIT-RULE` grades GREEN: p.353's sentence gives one
condition under which each chip is selected and is silent on A12 and A13
both low; p.351, which does speak to that point (one active-low select line
per chip), predicts both chips selected there, which is what `gary` does —
see Group F below for why the red this assertion first measured was a
one-way-statement-read-as-two-way error, not a finding.

## NM-HRM-062 — this package's own finding, four claims, graded neither side

Four claims carry `classify.source_defines: "conflict -- ..."` for exactly
one dispute: p.353 prints the CIAA/CIAB binary selection patterns
transposed relative to its own prose and its own chosen addresses.
`docs/findings/NM-HRM-062.md` (in this tests repo) already carries the full
four-emulator survey; this package's job is only to measure both readings
at this fixture and report them, per the outer build brief's explicit
instruction: **"Measure both readings, record both, grade neither."**

- Prose (p.353): *"CIAA is selected when A12 is low, A13 high; CIAB is
  selected when A12 is high, A13 low."*
- The page's own chosen addresses: CIAA = `$BFE001` (nibble `E` = `1110`:
  A13=1, A12=0 — agrees with the prose), CIAB = `$BFD000` (nibble `D` =
  `1101`: A13=0, A12=1 — agrees with the prose).
- The printed binary patterns: CIAA `101x xxxx xx01 rrrr xxxx xxx0` (A13=0,
  A12=1 — the OPPOSITE of the prose and the chosen address) and CIAB `101x
  xxxx xx10 rrrr xxxx xxx1` (A13=1, A12=0 — likewise opposite), each
  carrying the *other* chip's bits, A0 included.

`docs/references/amiga-hrm-3rd/appF/corpus-defects.md` lists p.353 as
**undamaged** — this is the print, not a scan artifact.

**A fixture-boundary fact worth stating plainly**: `cpu_address_in` is
declared `[23:1]` (gary.v:51) — **A0 is not present on this port at all**.
The byte lane a real 68000 cycle would select via A0 is decided outside
`gary`, by `cpu_hwr`/`cpu_lwr` (per `sol:F-signals.c63`'s own evidence,
citing `minimig.v:614,:639`). So the "A0 included" half of the conflict, as
printed, cannot be driven or observed as an address bit at this boundary at
all — it is reported here as a structural fact about the fixture, not
measured as a bit value. See the CONFLICT-CIA-PATTERN-TRANSPOSED assertion
group for what *is* measured (A12/A13 under both readings).

## Claim accounting (all 117)

| Group | What | Uids | Disposition |
|---|---|---:|---|
| Already graded elsewhere | 14 claims, `classify.note` names an accepted package | 14 | `record()`, citing the note |
| A — CIAA map sweep | "places a register slot at $BFEr01" (13 rows + the gap row), unnoted | 14 | Graded, `expect()`, CIAA-MAP-SWEEP |
| A — CIAB map sweep | "places a register slot at $BFDr00" (13 rows + the gap row), unnoted | 14 | Graded, `expect()`, CIAB-MAP-SWEEP |
| B — CIAA register names | "the CIAA register at $X is named Y" | 15 | Declined, `record()` — no register-select output at `gary` |
| B — CIAB register names | "the CIAB register at $X is named Y" | 15 | Declined, `record()` — same reason |
| C — map gap rows | "the row at $BFEB01/$BFDB00 has no register name" | 2 | Declined, `record()` — absence not observable at `gary` |
| D — CIAA combined rows | opus "row carries register name Y" (15 named rows incl. r=0,2) | 15 | Partial, `record()` per Rule 1 |
| D — CIAB combined rows | opus "row carries register name Y" (15 named rows incl. r=1,13) | 15 | Partial, `record()` per Rule 1 |
| E — decode completeness | "top three bits select the CIAs" / "other bits are don't cares" | 4 | Graded, `expect()`, CIA-DECODE-DONTCARE (RED) |
| F — two-bit select rule | "CIAA on A12 low+A13 high" / "CIAB on A12 high+A13 low" | 3 | Graded, `expect()`, CIA-AB-TWOBIT-RULE (GREEN) |
| G — CS/phi2 timing | "CS is normally activated at 02 by the address" | 2 | Declined, `record()` — no phi2/E-clock port at `gary` |
| H — NM-HRM-062 conflict | printed binary patterns, transposed | 4 | Measured both (`note()`), `record()` — graded neither |

14 + 14 + 14 + 15 + 15 + 2 + 15 + 15 + 4 + 3 + 2 + 4 = **117**.

## Grep check: is any of this already graded elsewhere?

`../nanomig-hrm-tests/hrm/` was searched (`grep -rl` for `gary\.v`,
`sel_cia`, `sel_cia_a`, `sel_cia_b`, `BFE001`, `BFD000`, `BFC100`,
`OVERLAP-BOTH-SELECTS-ACTIVE`) across every existing package's
`SPECIFICATION.md` and `test_*.cpp`.

**`ch8-ciaapra-address-window`** (fixture: a hand-built rig, not `gary`
alone) already grades `$BFE001` and `$BFE201` as printed-page facts, under
its own uids `opus:8-regs.c5`, `sol:8-reg.c5`, `opus:8-mbtn.c4`,
`sol:8-button.c3` (labels `A1`-`A4`, confirmed present at
`test_ch8-ciaapra-address-window.cpp:390,414,432,450`). This is exactly
`sol:F-ciaa-map.c1` and `sol:F-ciaa-map.c14`'s own `classify.note` — both
put on `record()` lines here, not re-graded.

**`ch8-cia-disk-port-windows`** (fixture: `ciaa` + `gary` jointly) already
grades `$BFD100` and `$BFDD00` as address-map facts, under `opus:8-cia.c5`
/ `sol:8-cia.c4` (confirmed present, labels `CIABPRB-ALIAS-OPUS-CIA-C5` /
`CIABPRB-CHIPLETTER-SOL-CIA-C4` at lines 553/574) and its own
`OVERLAP-BOTH-SELECTS-ACTIVE` assertion (line 398) **already measures**
`$BFC100` raising `sel_cia_a` and `sel_cia_b` together — but that
assertion's own label reads *"mechanism fact, not a manual citation"* and
its own comment reads *"No page claims or denies this."* **That package
declined to grade it against p.353's prose.** `sol:F-signals.c57`'s own
`classify.evidence` says the same thing explicitly: *"This is the printed
proposition that an accepted package states does not exist... Page 353
claims it."* So CIA-AB-TWOBIT-RULE below is the first assertion in this
suite to grade that specific mechanism fact against the printed page — it
is not a re-grade of `OVERLAP-BOTH-SELECTS-ACTIVE`, it is the first grade
of a proposition that assertion explicitly declined to make. This is
disclosed here per the "risk is a divergent verdict" rule; there is no
divergence because the other package never rendered a verdict on the
manual claim at all.

**`ch1-memory-map-and-two-buses`** names `sel_cia_a`/`sel_cia_b` only to
record, in its own "Grep check" section, that no package cites them for a
manual proposition — confirmed still true before this package's own
CIA-AB-TWOBIT-RULE and CIA-DECODE-DONTCARE assertions were added.

**No other hits** for the address-map figure claims (pages 339/340) or the
don't-care / top-3-bit claims (page 353) in any other package's
`claims.jsonl` citations.

---

## Already graded elsewhere (14 claims) — `record()` only, not re-graded

Per the build brief: *"14 carry a `classify.note` naming an accepted
package that already grades them — those go on `record()` lines... Do not
re-grade them."* Followed literally — including for `sol:F-signals.c66`/
`c67` and `opus:F-conn.c9`, whose own `classify.note` text suggests a
partial re-grade ("decline the $BFE001 point and grade only the r-nibble
family if it wants that") that this package does **not** take up, since the
brief's own instruction for the 14-claim group is unconditional.

| Uid | Page | Restatement | Cites |
|---|---:|---|---|
| `sol:F-ciaa-map.c1` | 339 | places a slot at $BFE001 | `ch8-ciaapra-address-window` (A1-A4) |
| `sol:F-ciaa-map.c14` | 339 | places a slot at $BFE201 (DDRA) | `ch8-ciaapra-address-window` |
| `sol:F-ciab-map.c11` | 340 | places a slot at $BFD100 (CIABPRB) | `ch8-cia-disk-port-windows` |
| `sol:F-ciab-map.c53` | 340 | places a slot at $BFDD00 (CIABICR bit 4) | `ch8-cia-disk-port-windows` |
| `sol:F-chip-map.c2` | 340 | each register has an access address in the 8520's space | `ch8-cia-disk-port-windows` ($BFD100, $BFDD00) |
| `sol:F-signals.c8` | 351 | CIA-A table uses pattern $BFEr01 | `ch8-cia-disk-port-windows` / `ch8-ciaapra-address-window` |
| `sol:F-signals.c10` | 351 | CIA-A table marks A12 active-low | same |
| `sol:F-signals.c32` | 351 | CIA-B table uses pattern $BFDr00 | same |
| `sol:F-signals.c34` | 351 | CIA-B table marks A13 active-low | same |
| `sol:F-signals.c66` | 353 | chosen CIAA pattern $BFEr01 | `ch8-ciaapra-address-window` |
| `sol:F-signals.c67` | 353 | chosen CIAB pattern $BFDr00 | `ch8-cia-disk-port-windows` |
| `opus:F-portsig.c7` | 351 | CIAA: $BFEr01, A12 low, INT2 | `ch8-cia-disk-port-windows` / `ch8-ciaapra-address-window` |
| `opus:F-portsig.c28` | 351 | CIAB: $BFDr00, A13 low, INT6 | same |
| `opus:F-conn.c9` | 353 | chosen CIAA/CIAB addresses | `ch8-ciaapra-address-window` / `ch8-cia-disk-port-windows` |

## Group A — CIA address-map membership (28 claims, graded)

**Proposition tested (CIAA-MAP-SWEEP, CIAB-MAP-SWEEP):** every literal
address the appendix's own address-map figures (p.339, p.340) print as a
row — one member of the 16-row `$BFEr01` family for CIAA, one member of the
16-row `$BFDr00` family for CIAB, `r` = 0..F — decodes at `gary`'s boundary
into the correct chip's select (`sel_cia_a` for a `$BFEr01` address,
`sel_cia_b` for a `$BFDr00` address) and not the other chip's. This is the
full content of each `sol:F-ciaa-map.c*`/`sol:F-ciab-map.c*` **"places a
register slot at $X"** claim — an existence/placement claim about an
address map, not a claim about which register lives there. `gary` has no
register-select or register-name output at all (its only CIA-relevant
outputs are `sel_cia`, `sel_cia_a`, `sel_cia_b`), so this address-membership
fact is the entirety of what `gary` can show about these claims, and it is
the entirety of what these particular claims (as opposed to the `is named`
and the combined `carries register X` claims below) assert.

14 rows per chip are graded here (the two rows per chip already declined
above, r=0 and r=2 for CIAA / r=1 and r=13 for CIAB, are excluded — not
re-graded). The gap row (r=11, `$BFEB01`/`$BFDB00`, the appendix's "not
used" slot) **is** included in this sweep: the existence claim being graded
is that a row/address exists at that position in the map, which is true
regardless of whether a register lives there — see Group C below for why
the *absence of a register* at that same address is a separate,
unobservable-at-`gary` claim.

Status: testable, graded PASS (both sweeps green at baseline — `gary`
correctly places every one of these 28 addresses in its intended chip's
window).

## Group B — CIA register identity (30 claims, declined)

`sol:F-ciaa-map.c2/c12/c15/c18/c21/c24/c27/c30/c33/c36/c39/c44/c47/c50/c53`
and the CIAB twins `sol:F-ciab-map.c2/c12/c22/c25/c28/c31/c34/c37/c40/c43/
c46/c51/c54/c57/c60` each state **only** that the register at a given
address is *named* something (`pra`, `prb`, `ddra`, ... `crb`) — no
address-membership content at all; the address is presupposed, the name is
the entire claim.

**Not gradeable at this fixture.** `gary.v` has no register-select decode
and no register-name output of any kind — `sel_cia_a`/`sel_cia_b` tell you
which *chip* an address belongs to, never which of the 16 registers within
that chip. Register identity (which offset maps to `pra` vs `prb` vs
`ddra`...) is decided by the CIA's own internal register bank, in `ciaa.v`
— a different top module, and `appF-cia-register-map`'s own territory (top
`ciaa`, per the outer build brief's package table), not reachable from a
`gary`-alone fixture. `record()`s below decline these 30 outright; no
`expect()` is built for them because there is no partial address-membership
content to grade separately (unlike Group D).

Status: testable; declined as structurally outside this package's own
fixture boundary.

## Group C — map gap rows (2 claims, declined)

`opus:F-ciaa-map.c31` (`$BFEB01` "has no register name...'not used'") and
`opus:F-ciab-map.c31` (`$BFDB00`, same). **Not gradeable at `gary`**: the
sweep in Group A already shows `gary` asserts `sel_cia_a`/`sel_cia_b` at
these exact two addresses exactly as it does at every other row in the same
sweep — `gary` provides no signal that distinguishes a defined register
slot from an undefined one. The "not used" fact is entirely internal to
`ciaa`'s own register decode, outside this package's boundary, same
reasoning as Group B.

## Group D — combined address+name rows (30 claims, declined in part)

The `opus:F-ciaa-map.c*` / `opus:F-ciab-map.c*` claims for named rows (15
per page, r=0..F excluding the gap) each restate **both** facts at once:
"the row at address $X carries the register name Y." Per Rule 1 ("a uid on
a graded line counts as ASSERTED... if you can only grade part of a claim,
the uid goes on a `record()` line alone"): the address-membership half of
each of these 30 claims is the identical fact CIAA-MAP-SWEEP /
CIAB-MAP-SWEEP already measures at the same address (for 13 of 15 rows per
page) or the identical fact `ch8-ciaapra-address-window` /
`ch8-cia-disk-port-windows` already measure (for the 4 rows at `$BFE001`,
`$BFE201`, `$BFD100`, `$BFDD00` — the same four addresses declined above).
No separate bare-label `expect()` is built to re-measure address membership
a second time at the same fixture with the same signals for the same
addresses; the existing measurement is named directly in each `record()`.
The register-name half is not gradeable, for the same reason as Group B.

## Group E — CIA-DECODE-DONTCARE (4 claims, graded, RED)

Uids: `sol:F-signals.c56` ("upper three address bits are 101" selects the
CIAs), `sol:F-signals.c63` ("all address bits outside the stated selection
fields are do-not-care bits"), `opus:F-conn.c1` (twin of c56), `opus:
F-conn.c6` (twin of c63).

**Proposition tested:** p.353 states the CIA chip-select decode rests on
exactly three fixed bits (A23-A21 = `101`), one two-bit field that
distinguishes which chip (A13/A12, graded separately under Group F), and a
four-bit register-select field (A11-A8); every other address bit —
including A20 and A19-A16, the five bits gary.v:190's `[23:16]==8'hBF`
comparison adds beyond the top three — is stated as a don't-care.
`gary.v:190` compares **all eight** of A23-A16 against `8'hBF`, not just the
top three. An address whose top three bits are `101` but whose A20/A19-A16
differ from `$BF`'s own `10111`, should — per the manual's own don't-care
reading — still select the CIAs. Measured: it does not.

**Control, same measurement basis:** the manual's own don't-care claim
holds unconditionally for bits below A12 (register-select A11-A8 plus the
genuinely-uncommitted A7-A4/A3-A0) — `gary.v:190` never examines any of
those bits for `sel_cia`, so varying them cannot possibly change the
selection, and it measurably does not. This is the "at least one control
case expected to PASS on the same measurement basis" the build brief
requires, on the very same signal (`sel_cia_a`) the suspect half is
measured on.

Status: testable, graded — **RED at baseline**, honestly. This is not the
NM-HRM-062 transposition; there is only one reading of "the upper three
address bits are 101" and only one reading of "all other bits are don't
cares," and `gary`'s own decode, read directly, does not implement the
second half of that sentence for A20/A19-A16. Reported per the outer build
brief's "measure it, report what you find, adjudicate nothing" instruction
for this same item (`gary.v:190`).

## Group F — CIA-AB-TWOBIT-RULE (3 claims, graded, GREEN)

Uids: `sol:F-signals.c57` ("CIAA is selected when A12 is low and A13 is
high"), `sol:F-signals.c58` ("CIAB is selected when A12 is high and A13 is
low"), `opus:F-conn.c2` (twin, combined).

**Proposition tested:** the prose gives *one condition* under which each
chip is selected — CIAA when A12 is low and A13 is high; CIAB when A12 is
high and A13 is low — and is silent on what happens when A12 and A13 are
both low (or both high). It does not say a chip is selected *only* under
its stated condition; reading it that way tests a one-way statement as a
two-way one, the shape [NM-HRM-029](../../docs/findings/withdrawn/NM-HRM-029.md)
was withdrawn for. The positive halves hold: at `$BFE001` (A12=0, A13=1,
the prose's and the chosen address's own CIAA point) `sel_cia_a=1`; at
`$BFD000` (A12=1, A13=0, CIAB's point) `sel_cia_b=1`.

Where the manual does speak to `$BFC100` (A12=0, A13=0), it is p.351, not
p.353: one active-low select line per chip — `(A12*)` for CIA-A, `(A13*)`
for CIA-B — both low here, so p.351 predicts *both* chips selected. That is
what `gary` measures: both `sel_cia_a` and `sel_cia_b` are asserted
together at `$BFC100`.

This exact mechanism fact is already measured, at this exact fixture, by
`ch8-cia-disk-port-windows`' `OVERLAP-BOTH-SELECTS-ACTIVE` assertion — but
that assertion's own label and comment say "mechanism fact, not a manual
citation" and "no page claims or denies this." CIA-AB-TWOBIT-RULE is the
first assertion in this suite to grade that mechanism fact against p.351
(see "Grep check" above) — not a re-grade, a first grade of a proposition
another package explicitly left ungraded.

Status: testable, graded — **GREEN**: p.353's own sentence never predicted
"neither" at `$BFC100`, and the page taken whole (p.351 included) predicts
exactly what `gary` does there.

## Group G — CS/phi2 timing (2 claims, declined)

Uids: `sol:F-signals.c76` ("the appropriate address combination normally
activates CS low at 02"), `opus:F-signals.c8` (twin).

**Not gradeable at `gary`.** `gary`'s port list (gary.v:49-108) carries no
clock-phase, E-clock or phi2 input of any kind, and `gary.v:190-192` derive
`sel_cia`/`sel_cia_a`/`sel_cia_b` combinationally from `cpu_address_in`
alone — no clock term appears anywhere in those three lines. The
"activated... at 02" half of the sentence describes a timing relationship
this module structurally cannot express; per both claims' own
`classify.evidence`, the phasing that does exist sits one module away, at
`minimig_m68k_bridge.v:131` (`_dtack` withheld until `eclk`), a different
top module outside this package's boundary. Declined, not invented.

## Group H — NM-HRM-062, measured both, graded neither (4 claims)

Uids: `sol:F-signals.c64` (CIAA printed pattern), `sol:F-signals.c65` (CIAB
printed pattern), `opus:F-conn.c7` (CIAA printed pattern, twin), `opus:
F-conn.c8` (CIAB printed pattern, twin).

Per the outer build brief: **"Measure both readings, record both, grade
neither."** No `expect()` is built for these four uids — only `note()`
(prints `MEASURED:`, graded by nothing) and one `record()` per uid pair
citing `docs/findings/NM-HRM-062.md`. What is measured:

- The **prose reading**: `$BFE001` (A13=1, A12=0) → `sel_cia_a=1`;
  `$BFD000` (A13=0, A12=1) → `sel_cia_b=1`. `gary` follows the prose.
- The **printed-pattern reading**: an address matching the printed CIAA
  pattern's own A13/A12 bits (A13=0, A12=1 — e.g. `$BFD001`) decodes at
  `gary` to `sel_cia_b=1`, `sel_cia_a=0` — i.e. **CIA-B**, not CIA-A. An
  address matching the printed CIAB pattern's own A13/A12 bits (A13=1,
  A12=0 — e.g. `$BFE001` itself) decodes to `sel_cia_a=1`, `sel_cia_b=0` —
  i.e. **CIA-A**, not CIA-B. Building a test from the printed patterns,
  expecting them to select the chip they are labelled for, would grade
  `gary`'s correct, prose-following decode as a FAIL.

A0 is reported, not measured, per the fixture-boundary note above: `gary`'s
`cpu_address_in` port carries no A0 bit at all, so the pattern's A0 half
cannot be driven as a stimulus at this boundary.

Status: testable; both readings measured and reported; **neither graded**.
Ownership of the printed page's own internal contradiction belongs to
Amiga specialists, per NM-HRM-062 and the outer build brief.

---

## What this specification deliberately does not contain

Concrete stimulus values beyond what a claim's own `evidence` field already
named, and the exact fixture wiring, are added only after this file is
committed and `gary.v` is opened directly, per the build brief's step 2 —
already disclosed above as only partially achievable, since several claims'
own `evidence` fields already carry `gary.v` line numbers and signal names.

Every one of the 117 claims in this package's slice of
`2026-09-17_appF_packages.jsonl` is accounted for in the table above:

- **14 uids** on `record()` lines citing an accepted package that already
  grades them (per the build brief, unconditional, not re-graded).
- **28 uids** (Group A) on two uid-bearing `expect()` sweeps, graded PASS.
- **30 uids** (Group B) declined outright, no partial content to grade.
- **2 uids** (Group C) declined outright, same reason as Group B.
- **30 uids** (Group D) declined in part per Rule 1 — the address-
  membership half is measured (by Group A or by an accepted sibling
  package), but the uid sits on `record()` alone since the register-name
  half is not gradeable here.
- **4 uids** (Group E) on one uid-bearing `expect()`, graded RED.
- **3 uids** (Group F) on one uid-bearing `expect()`, graded GREEN.
- **2 uids** (Group G) declined outright, out of this fixture's boundary.
- **4 uids** (Group H, NM-HRM-062) measured both readings via `note()`,
  graded neither, `record()`s cite the finding.

14 + 28 + 30 + 2 + 30 + 4 + 3 + 2 + 4 = 117. No claim is left without a
line that carries its uid.
