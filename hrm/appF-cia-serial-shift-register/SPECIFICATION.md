# SPECIFICATION — `appF-cia-serial-shift-register`

Package: `hrm/appF-cia-serial-shift-register`
Fixture: `ciaa`, compiled with its CIA submodules (`cia_int`, `cia_timera`,
`cia_timerb`, `cia_timerd`) — the top the task sheet names.

Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
on `classify.package == "appF-cia-serial-shift-register"` — **27 rows, 17
sol-read + 10 opus-read**, all `status: testable`, pages 345-346. **15 of 27
carry a `classify.note` naming an accepted package/uid** (matches the task
sheet's "27 claims, 15 already graded"). The remaining 12 uids (6
propositions x sol/opus) are fresh.

## Disclosure — the order of work did not fully hold

Same situation every package in this batch has disclosed. Filtering by
`classify.package` surfaced `classify.evidence`/`classify.note` for all 27
rows in the same read. This builder additionally opened `ciaa.v` (the
`sdr_latch`/`ser_tx_run`/`ser_tx_cnt` block, `cia_int.v`, `cia_timera.v`)
directly before writing this file, and opened `hrm/ch8-keyboard-serial-
register`'s own test source to verify its cited PROP-1/PROP-2/WIDTH-8BIT
uids and verdicts directly rather than trusting the note text alone.

## The claim inventory

**15 already-graded (declined, `record()`), 4 citation groups:**

| group | citing package/PROP, uid(s) | count | this package's uids |
|---|---|---:|---|
| `WIDTH-8BIT` | `ch8-keyboard-serial-register`, untagged assertion WIDTH-8BIT (`opus:8-appG.c13`, `sol:8-appG.c20`, `opus:8-appG.c142`, `sol:8-appG.c170`) | 2 | `sol:F-sdr.c1`, `opus:F-sdr.c1` |
| `KEYBOARD-USES-ONE-REGISTER` | `ch8-keyboard-serial-register` PROP-1 (`opus:8-kbd.c1`, `sol:8-kbd.c1`) | 2 | `sol:F-sdr.c3`, `opus:F-sdr.c3` |
| `INPUT-MODE-EIGHT-PULSES` | `ch8-keyboard-serial-register` PROP-1 + PROP-2 (`opus:8-kbd.c1`/`sol:8-kbd.c1`, `opus:8-kbd.c12`/`sol:8-kbd.c9`) | 2 | `sol:F-sdr.c6`, `opus:F-sdr.c6` |
| `SDR-TABLE-ROWS` | `ch8-keyboard-serial-register` PROP-1 + WIDTH-8BIT | 9 | `sol:F-sdr.c24..c31`, `opus:F-sdr.c23` |

2+2+2+9 = 15, verified by grouping the 27 rows' `classify.note` text
directly (four distinct texts, no fifth). All four citation targets were
opened directly in `hrm/ch8-keyboard-serial-register/test_ch8-keyboard-
serial-register.cpp`: PROP-1 (line ~452, `opus:8-kbd.c1 sol:8-kbd.c1`),
PROP-2 (line ~475, `opus:8-kbd.c12 sol:8-kbd.c9`), and WIDTH-8BIT's four
uids — all confirmed to exist and be graded (not themselves declined).

**12 fresh, 6 propositions (all page 345, output-mode paragraphs):**

| proposition | uids |
|---|---|
| "In the output mode, Timer A is used as the baud rate generator." | `sol:F-sdr.c7`, `opus:F-sdr.c7` |
| "Data is shifted out on the SP pin at 1/2 the underflow rate of Timer A." | `sol:F-sdr.c8`, `opus:F-sdr.c8` |
| "The maximum baud rate possible is 02 divided by 4, but the maximum usable baud rate will be determined by line loading and the speed at which the receiver responds to input data." | `sol:F-sdr.c9`, `opus:F-sdr.c9` |
| "Transmission will start following a write to the serial data register." | `sol:F-sdr.c12`, `opus:F-sdr.c11` |
| "After eight CNT pulses, an interrupt is generated to indicate that more data can be sent." | `sol:F-sdr.c16`, `opus:F-sdr.c15` |
| "If the serial data register was reloaded with new information prior to this interrupt, the new data will automatically be loaded into the shift register and transmission will continue." | `sol:F-sdr.c17`, `opus:F-sdr.c16` |

Verified against `hrm-testpipeline/docs/references/amiga-hrm-3rd/appF/p345.txt`
directly — every quotation is an exact substring of the corpus.

## Structural finding this whole package rests on

`ciaa.v`'s complete port list (`ciaa.v:82-104`) has **no SP pin and no CNT
pin**, and there is **no bit-serial shifter anywhere** — the whole "serial
data register" is `sdr_latch` (`ciaa.v:162`), a flat 8-bit parallel
register written whole (from `kbd_mouse_data` on a keystrobe, or from
`data_in` on a bus write) and read whole. "Shifting" in this RTL is
entirely a *byte-timing* abstraction: `ser_tx_run`/`ser_tx_cnt`
(`ciaa.v:178-198`) count Timer A underflow pulses (`tmra_ovf`) and raise an
interrupt (`ser_tx_irq`) after enough of them, with no per-bit signal ever
observable. Every fresh assertion below is built around this measured
structure — none invents a literal SP/CNT signal or a bit-level readback
that does not exist.

**One further fact, flagged by the build task specifically and reported
here though it carries no uid in this package:** p.345's "SDR data is
shifted out MSB first. Serial input data should appear in this same
format" (bare ids `F-sdr.c18`-`c20`, both readers) is `status:
not-asserted` for both readers (`classify.package` absent — neither reader
assigned these rows a package at all) because there is no bit-serial output
path to observe an order on. It is nonetheless the premise
`hrm/ch8-keyboard-serial-register`'s own bit map rests on: Appendix G
states the keyboard's transmit ORDER (6-5-4-3-2-1-0-7); only this p.345
sentence states that the register fills MSB-first, which is what turns
that order into the specific bit positions `ciaa.v:186`'s `sdr_latch[7:0]
<= ~{kbd_mouse_data[6:0],kbd_mouse_data[7]}` lands them at — confirmed read
directly in this file's own header comment reasoning and printed as a
`note()` at test runtime. Chapter 8 graded the composite permutation
without ever reading this page. Reported, adjudicated by nobody here.

## Assertion design (12 fresh uids, 6 propositions)

`tmra_ovf`/`ser_tx_cnt`/`ser_tx_run` are internal wires, not ports — not
readable from a black-box Verilator fixture without `--public` (which
would mean touching the pinned RTL's build, not permitted). Every
measurement below instead uses only `ciaa`'s real ports: TALO/TAHI readback
to watch Timer A's own down-counter directly, and the top-level `irq` pin
(armed once via an ICR mask **write**, then only ever polled — an ICR
*read* clears its latched bits, `cia_int.v:43-50`, and would corrupt the
very state being measured).

- **`TIMER-A-IS-GENERATOR`** (`c7`): with Timer A stopped and an
  independently-running Timer B ticking far more than a transmission needs,
  completion never occurs; starting Timer A specifically is what lets it
  complete. Isolates Timer A from Timer B, not just "some timer runs."
- **`HALF-RATE-SHIFT`** (`c8`): two independent `ticks-to-completion` runs
  at different Timer A latch values fit the line `ticks(L) = C + K*(L+1)`;
  solving for `K` (Timer A underflow pulses needed) without assuming it,
  measured **K = 16** for an 8-bit byte — exactly twice the bit count,
  operationalizing "1/2 the underflow rate."
- **`MAX-BAUD-CEILING`** (`c9`): Timer A's own fastest legal (non-zero-
  latch) underflow period, measured directly by watching TALO's own
  readback pattern, is **2 clock ticks**; combined with `HALF-RATE-SHIFT`'s
  2-underflows-per-bit factor, the minimum achievable bit period is 4
  ticks — a ceiling of (Timer A clock)/4, matching the printed "Ø2 divided
  by 4" numerically. The "line loading"/"receiver response speed" clause
  names external analog factors with no RTL counterpart at this boundary
  and is not graded.
- **`SDR-WRITE-STARTS-TX`** (`c12`/`c11`): Timer A running in output mode,
  alone, never completes a transmission; writing SDR is what starts it.
- **`EIGHT-PULSES-INTERRUPT`** (`c16`/`c15`): a **third**, independent
  Timer A latch value's measured completion tick is checked against the
  calibration line fitted from the other two — not merely re-observed at
  the same two points `K` was fitted from.
- **`RELOAD-CONTINUES`** (`c17`/`c16`): rewriting SDR partway through an
  in-progress transmission does not restart the pulse count (completion
  lands exactly where the first write alone predicted) and the reloaded
  byte is what SDR holds once transmission completes.

All six measured via `WSL Verilator 5.052`: **6 passed, 1 failed** on first
build (`RELOAD-CONTINUES` off by exactly the 2 ticks its own reload's
`write_reg()` call consumed but a loop-local tick counter didn't see);
fixed by tracking `Harness::elapsed` (every posedge the DUT has actually
seen, incremented inside `tick()` itself) instead of a caller-local loop
index, and reconfirmed: **7 passed, 0 failed**. Reported here per this
suite's rule against writing a record before taking the measurement.

## Control / liveness

`FIXTURE-LIVENESS` (CIAADDRA round-trip, the same convention every `ciaa`
package in this batch uses) precedes every other assertion and the four
`record()` lines' own trustworthiness rests on it too.

## HALT check

No HALT. All 12 fresh propositions were reachable at `ciaa`'s existing
register and `irq`-pin interface, with no RTL change, no added port
(`--public` included), and no invented requirement — the "1/2 rate" and
"Ø2/4" numeric claims were operationalized from the down-counter's own
structure (smallest nonzero latch = fastest period), not assumed from the
RTL producing a particular number.

## What this specification deliberately does not contain

Exact register-select constants, latch values and tick budgets are step-2
material and are fixed in `test_ciaa.cpp`, not here.
