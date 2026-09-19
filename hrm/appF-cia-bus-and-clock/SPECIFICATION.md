# SPECIFICATION — `appF-cia-bus-and-clock`

Package: `hrm/appF-cia-bus-and-clock`
Fixtures: **two** — `minimig_m68k_bridge` (the task sheet's own `top`,
minimig_m68k_bridge.v alone) and `amiga_clk` (amiga_clk.v alone). Neither
`paula_uart` nor `agnus` is built as a fixture — see "Why only two
fixtures" below. `hrm/ch1-cia-timers` is this batch's own precedent for a
package building more than one fixture.

Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`,
filtered on `classify.package == "appF-cia-bus-and-clock"` — **11 rows** (6
sol-read, 5 opus-read), all `status: testable`. **4 of 11 carry a
`classify.note`** (matches the task sheet's "11 claims, 4 already graded").
The rows span four different `observable: yes, at X` values across pages
345, 352 and 353 — a genuinely mixed bag the classifier grouped by subject
("bus and clock"), not by module.

## Disclosure — the order of work did not fully hold

Same situation every package in this batch has disclosed. Filtering by
`classify.package` surfaced `classify.evidence`/`classify.note` for all 11
rows in the same read. This builder additionally opened
`minimig_m68k_bridge.v` and `amiga_clk.v` directly and in full before
writing this file (both needed for fixture design), and built a standalone
throwaway probe of `minimig_m68k_bridge`'s bus-cycle timing (not part of
this package, discarded after use) to verify a working `_as`/`cck`
acquisition sequence empirically before writing `test_minimig_m68k_
bridge.cpp` itself — reported here rather than presented as derived purely
from reading the RTL.

## The claim inventory

| bare id | reader(s) | page | fixture | already graded? |
|---|---|---|---|---|
| `F-sdr.c4` | sol, opus | 345 | `paula_uart` | **yes** — `ch8-serial-interface` |
| `F-example.c10`/`c13` | sol/opus | 352 | `agnus` | **yes** — `agnus_top_dmacon` |
| `F-signals.c71`/`c3` | sol/opus | 353 | `amiga_clk` | no |
| `F-signals.c77`/`c9` | sol/opus | 353 | `minimig_m68k_bridge` | no |
| `F-signals.c78` | sol | 353 | `minimig_m68k_bridge` | no |
| `F-signals.c79` | sol | 353 | `minimig_m68k_bridge` | no |
| `F-signals.c10` | opus | 353 | `minimig_m68k_bridge` | no |

4 already-graded + 7 fresh = 11, matching the task sheet. `opus:F-signals.
c10`'s own quote/restatement spans **both** halves of the R/W sentence
("A high on R/W indicates a read ... while a low indicates a write") in one
row, where `sol` split the same sentence into two bare ids (`c78`, `c79`).
Both decompositions are graded in full below — `opus:F-signals.c10` is
cited on **both** `RW-HIGH-IS-READ` and `RW-LOW-IS-WRITE`, since its own
claim content genuinely names both, rather than picking one arbitrarily or
inventing a second bare id that does not exist.

## Duplicate-grading check — verified against the cited sources directly

- **`F-sdr.c4`** (p.345, "the RS-232 compatible serial port is controlled
  by the Paula chip") — `hrm/ch8-serial-interface` already grades this at
  `paula_uart`'s own boundary across 97 `expect()` assertions; `hrm/ch1-
  paula-what-it-contains`'s `THREE-CHIPS-DISJOINT-OUTPUTS` independently
  grades `txd` as Paula's own output. Confirmed: no CIA module (`ciaa.v`,
  `ciab.v`) declares a `txd`/`rxd` port at all.
- **`F-example.c10`/`c13`** (p.352, the printed example's DMACON line) —
  `hrm/ch7-dmacon-register-bits` already grades DMACON's SET/CLR direction across
  all thirteen bits and the `$096` address decode, at `agnus`'s own
  boundary.

Both citation targets are accepted packages already graded, not themselves
declined — consistent with the note text and not re-verified against every
individual `expect()` line in each (a wider trawl than this package's own
two citations need).

## Why only two fixtures

`paula_uart` and `agnus` each appear in this package only through a
**pure duplicate-citation decline** — neither of this package's two
citations to them needs any fresh measurement of its own, and
`minimig_m68k_bridge` cannot even see DMACON (an Agnus/Paula register, not
a CIA one) regardless of duplication. Building either as a compiled
fixture here would add a Verilator target with zero test content. Both
`record()` lines are printed from `test_minimig_m68k_bridge.cpp` (this
package's primary/first-listed fixture) instead — `refresh_captures.py`
runs every binary a package builds and reads whatever each one prints, so
which binary a `record()` line comes from does not affect the coverage
ledger.

## Assertion design (7 fresh uids, 4 propositions)

### `amiga_clk` — `E-CLOCK-IS-TENTH` (`sol:F-signals.c71`, `opus:F-signals.c3`)

p.353: "The E clock runs at 1/10 of the CPU clock." `amiga_clk.v`'s only
input clock is `clk_28`; `clk7_en` (a plain output port, "7MHz output
clock enable") is the module's own operationalization of "CPU clock" —
confirmed by `minimig_m68k_bridge.v`'s own header timing diagram, which
draws "CPU_CLK" toggling at the same rate `clk7_en`/`clk7n_en` gate. `eclk`
is a 10-bit one-hot rotator (`amiga_clk.v:22,36,64`); `minimig.v:620`/`:645`
wire bit 8 specifically into each CIA's single-bit `eclk` port (by the
shifter's own symmetry every bit behaves identically, so this choice is
cosmetic — made only to match what the rest of the tree actually uses, not
because the RTL treats bit 8 specially). The fixture counts `clk7_en`
pulses strictly between two consecutive rising edges of `eclk[8]` and
checks the count is exactly 10 — not assumed from the module's own port
comments (`// 0.709379 MHz clock enable output`), measured directly.

### `minimig_m68k_bridge` — three R/W propositions (p.353)

"The R/W signal is normally supplied by the microprocessor and controls
the direction of data transfers of the 8520. A high on R/W indicates a
read (data transfer out of the 8520), while a low indicates a write (data
transfer into the 8520)."

`rd`/`hwr`/`lwr` are pure combinational functions of an internal `enable`
and latched `lr_w`/`l_uds`/`l_lds` (`:139-141`). `enable` needs the
module's own registered address-strobe copy and DTACK state to be
acquired first — this fixture drives `_as` and `cck` through one complete,
empirically-verified acquisition (`Harness::acquire_enable()`, 16 ticks
`_as` asserted + `cck` high, then `cck` dropped) using the simpler of the
two paths `_ta_n` can clear through (`vpa=0`, so no `eclk`/`vma`
synchronisation is needed for this measurement) before any R/W assertion
is graded:

- **`RW-SOURCE-CONTROLS-DIRECTION`** (`c77`/`c9`): with the acquired cycle
  held fixed, changing **only** `r_w` changes which of `{rd}` vs
  `{hwr,lwr}` this fixture's own pins assert — `r_w` is what determines
  direction here, not some other input.
- **`RW-HIGH-IS-READ`** (`c78`, `opus:c10`): `r_w` high gives `rd=1`,
  `hwr=0`, `lwr=0`.
- **`RW-LOW-IS-WRITE`** (`c79`, `opus:c10`): `r_w` low (with both byte
  strobes asserted) gives `rd=0`, `hwr=1`, `lwr=1`.

All measured via WSL Verilator 5.052: **6 passed, 0 failed** (4 on the
bridge fixture, 2 on the clock fixture) on first build — no RTL surprise
required a redesign here, unlike `appF-cia-control-registers` and
`appF-cia-serial-shift-register` in this same batch (both needed a
measured-then-fixed timing-accounting correction; reported there, not
repeated here since it did not happen in this package).

## Control / liveness

Each fixture carries its own independent liveness check: `amiga_clk`'s
confirms `clk7_en` actually pulses at all (so a dead reference clock reads
as "untestable," not "vacuously true") before `E-CLOCK-IS-TENTH` is
trusted; `minimig_m68k_bridge`'s uses `address_out` (a plain registered
copy of `address`, gated by nothing) — a path independent of the whole
`_as`/`cck`/DTACK acquisition the three R/W assertions depend on.

## HALT check

No HALT. All seven fresh propositions were reachable at each fixture's
existing port interface, with no RTL change, no added port, and no
invented requirement.

## What this specification deliberately does not contain

Exact tick counts, the `_as`/`cck` acquisition's specific timing, and the
`eclk` bit choice's numeric value are step-2/test-source material and are
fixed in the two `test_*.cpp` files, not here.
