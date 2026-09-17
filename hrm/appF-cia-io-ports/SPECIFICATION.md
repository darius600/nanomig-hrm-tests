# SPECIFICATION — `appF-cia-io-ports`

Package: `hrm/appF-cia-io-ports`
Fixture: `ciaa`, compiled with its CIA submodules (`cia_int`, `cia_timera`,
`cia_timerb`, `cia_timerd`) — the top the task sheet names, the same
single-chip boundary every other appendix-F CIA package in this batch uses.

Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
on `classify.package == "appF-cia-io-ports"` — **44 rows, all page 341**, all
`status: testable`, all `observable: yes, at ciaa`. **All 44 carry a
`classify.note` naming an accepted package and uid that already grades the
identical printed proposition.** Per the build brief: "Put each on a
`record()` line citing that package and uid. Do not re-grade it." This
package is that shape in full — the task sheet's own table already says so
("44 — all of them") and this document confirms it directly from the claim
data rather than taking the count on trust (see the tally below).

## Disclosure — the order of work did not fully hold

Same situation this batch's other packages have each disclosed
(`ch1-cia-timers`, `ch1-userio-ports`, `appF-cia-control-registers`):
`classify.evidence` and `classify.note` live in the same JSON record as
`extract.quote`, in the one file that had to be filtered by
`classify.package` to find this package's 44 rows. Filtering necessarily
surfaced both the evidence and the note text for all 44 rows — in this
package's case, the note text is not incidental exposure but **the entire
content this document needs**: identifying the accepted package and uid each
claim declines to is the whole job. This builder additionally opened the
three cited sibling packages' own test sources directly (not just their
notes) to confirm each citation names a real, existing graded line with the
verdict the note describes — reported under "Duplicate-grading check" below.
`ciaa.v` itself was read (for the fixture-liveness control's own register
choice) but no assertion in this package rests on anything read there beyond
what the three sibling packages already established.

## The claim inventory — 44 uids, 3 duplicate-citation groups

Grouped by which already-accepted package/uid each `classify.note` names
(verified, not just counted — see below):

| group | citing package, PROP, uid(s) | verdict there | count | this package's uids |
|---|---|---|---:|---|
| **A** | `ch8-parallel-port-direction` PROP-1 (`opus:8-par.c1`, `sol:8-par.c1`) | **FAIL** | 22 | see `test_ciaa.cpp`'s first `record()` |
| **B** | `ch8-ciaa-port-direction` PROP-2 (`opus:8-dio.c46`, `sol:8-dio.c43`) | **FAIL** | 4 | see `test_ciaa.cpp`'s second `record()` |
| **C** | `ch8-ciaa-port-direction` PROP-1 (`opus:8-dio.c44`) | **FAIL** | 18 | see `test_ciaa.cpp`'s third `record()` |

22 + 4 + 18 = 44, matching the task sheet's count and this package's own row
count, confirmed by grouping the 44 rows' `classify.note` text directly
(identical note text = identical citation target; three distinct note texts
found, no fourth).

The four page-341 propositions behind these 44 uids (verified against
`hrm-testpipeline/docs/references/amiga-hrm-3rd/appF/p341.txt` directly —
every quotation is an exact substring of the corpus; `corpus-defects.md`
flags pp.341-352's *tabular alignment* as damaged, not this page's running
prose or its 4-row PRA/PRB/DDRA/DDRB register-name table, which reproduces
cleanly):

1. "Ports A and B each consist of an 8-bit peripheral data register (PR) and
   an 8-bit data direction register (DDR)." — register existence (`c1`,
   both readers) and, restated per-row, the PRB/DDRB table rows (`c16-23`,
   `c32-39` `sol`; `c9`, `c11` `opus`).
2. "If a bit in the DDR is set to a 1, the corresponding bit position in the
   PR becomes an output. If a DDR bit is set to a 0, the corresponding PR
   bit is defined as an input." — DDR-bit meaning (`c2`, `c3`, both
   readers), and, restated per-row, the PRA/DDRA table rows (`c8-c15`,
   `c24-c31` `sol`; `c8`, `c10` `opus`).
3. "When you READ a PR register, you read the actual current state of the
   I/O pins (PA0-PA7, PB0-PB7), regardless of whether you have set them to
   be inputs or outputs." — PR-read-reflects-pin-state (`c4`, both
   readers).
4. The REG NAME table itself (p.341, rows `0 PRA`/`1 PRB`/`2 DDRA`/`3
   DDRB`) — each row restates one of the above at a specific register
   address; graded as duplicates of whichever underlying proposition (1 or
   2) the row's own register belongs to, per port.

## Duplicate-grading check — verified against the cited sources directly

Not taken on the note text alone. Opened both cited packages' own test
sources:

- **`hrm/ch8-parallel-port-direction/test_ch8-parallel-port-direction.cpp`**
  (line ~312): `expect(..., "opus:8-par.c1 sol:8-par.c1 PROP-1: the
  general-purpose ...")`. Confirmed **FAIL** at baseline (this package's own
  header comment: "opus:8-par.c1/sol:8-par.c1 PROP-1 FAIL" — `ciaa`'s port-B
  write register is commented out in the RTL, so a CIAAPRB read always
  echoes `portb_in`, never a value the CPU itself wrote).
- **`hrm/ch8-ciaa-port-direction/test_ch8-ciaa-port-direction.cpp`** (lines
  ~344-382): two separate `expect()` lines, `opus:8-dio.c44 PROP-1` (line
  349) and `opus:8-dio.c46 sol:8-dio.c43 PROP-2` (line 369). Both **FAIL**
  at baseline — each is graded "across all eight bit positions (PA0-PA7)"
  and each label states its own result directly: "True only for PA0 and
  PA1; false for PA2-PA7" (PROP-1: DDRA-to-PRA per-bit correspondence not
  universal; PROP-2: DDR=1-makes-PR-an-output not universal, for the same
  PA2-PA7 reason plus a distinct PA6/PA7 readback gap the label itself
  spells out).

All three citation targets exist, are graded (not themselves declined), and
their verdicts match what each `classify.note` in this package's own claim
data already said. No citation in this package's 44 rows points at a
package or uid that turns out not to exist or not to be graded.

## Why none of the 44 is re-graded

The build brief is explicit that re-grading a duplicated proposition risks
"this appendix reaching a different verdict from an accepted package about
the same register bit" — the actual risk here, since the underlying finding
(port B's write path dead in RTL; port A's per-bit correspondence not
universal) is a single RTL fact reachable from either page. Building a
fresh `expect()` against the identical mechanism, at the identical `ciaa`
boundary, would not add information and could only either agree (redundant)
or disagree (a contradiction this suite specifically exists to avoid). Every
one of the 44 is declined for this reason, on a `record()` line citing its
group's accepted package/PROP/uid(s), per the build brief's instruction for
this exact shape.

## Control / liveness

Per the build brief ("the fixture proves it can deliver a known value
before anything reads one") and this batch's own precedent
(`ch1-userio-ports`, `ch1-keyboard-translation`): a record()-only package
still needs its fixture shown live, so a `record()` decline is not mistaken
for, or concealing, a dead harness. `test_ciaa.cpp` drives two distinct,
known bytes into CIAADDRA (`rs=0x2`) and confirms both arrive and read back
correctly and distinctly, before any `record()` line is trusted — the same
DDRA-based liveness check `ch1-cia-timers`'s own `ciaa` fixture already
uses, reused here as verified plumbing, not as a new claim. This check
carries no uid and grades nothing from page 341.

## HALT check

No HALT. Every one of the 44 claims is reachable by citation to an
already-accepted, verified sibling package. No RTL change, no added port,
no invented requirement was needed to reach any of the 44 dispositions.

## What this specification deliberately does not contain

The fixture-liveness control's exact byte values are step-2/test-source
material and are fixed in `test_ciaa.cpp`, not here.
