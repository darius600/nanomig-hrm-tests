# Specification — `appF-cia-interval-timers`

Package: `hrm/appF-cia-interval-timers`
Fixture: **one** top, `ciaa`, compiled alone with its own CIA submodules
(`cia_int.v`, `cia_timera.v`, `cia_timerb.v`, `cia_timerd.v` — the latter
present only so Verilator can elaborate `ciaa`, the same shape
`hrm/ch1-cia-timers` already uses for its `ciaa` fixture). `ciab` is not
built here — every claim in this package's 106 is about the interval-timer
register map and control logic that is identical in both chips, and `ciaa`
alone carries both of its own Timer A and Timer B instances, which is
already enough to grade "linked" behaviour between two timers without a
second chip.
Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
to `classify.package == "appF-cia-interval-timers"` (106 claims, all
`classify.status: "testable"`).
Subject: printed pages 341–343 — the interval timers' counter/latch pair,
control-register functions (start/stop, one-shot/continuous, force load),
input-mode clock selection, the latch-reload conditions, and the BIT NAMES
tables for registers 4–7 (TALO/TAHI/TBLO/TBHI, read view and write view).

**Order-of-work disclosure, stated here rather than at the end, because it
governs how to read everything below:** the blind order this pipeline asks
for — write this file from the quotations alone, commit it, only then open
the RTL — does not fully hold, for the same reason `hrm/ch1-cia-timers`
already disclosed for itself, sharper here. Every one of this package's 106
claims carries a `classify.evidence` field that already cites `ciaa.v`,
`cia_timera.v` and `cia_timerb.v` by file and line (register-decoder rows,
the `tmr`/`tmlh`/`tmll`/`tmcr` signal names, the reload/underflow logic) —
that evidence was read while filtering the claims file, before this
document could be started at all. The task instructions that assigned this
package additionally pre-exposed two specific measurements: `cia_timera.v:76`
gates a reload with `(~start | oneshot)`, admitting a reload while running
in one-shot mode where p.343 allows it only while stopped; and a built
fixture's readings (one-shot START self-clears at underflow, continuous does
not; the force-load strobe reloads and reads back zero; CRA reads `0x6F` for
a written `0xFF`). Both were independently re-derived by this builder by
reading `ciaa.v`, `cia_timera.v`, `cia_timerb.v` and `cia_int.v` in full
(not re-typed from what was pre-exposed) before this file was written, and
every concrete register/bit fact below was checked against that reading.
What the pre-exposure did **not** do: choose, narrow, or adjust any expected
value. Every proposition below still rests on the printed sentence named
against it; where the RTL and the manual disagree, that disagreement is
reported as a graded result, never resolved by picking a looser reading.

## Claim accounting (all 106)

| Group | Count | Disposition |
|---|---:|---|
| BIT NAMES, read view (TALO/TAHI/TBLO/TBHI, p.343 table) | 36 | Graded, 4 composed propositions (PROP-BITS-\*-READ) |
| BIT NAMES, write view (same 4 registers, p.343 table) | 36 | Graded, 4 composed propositions (PROP-BITS-\*-WRITE) |
| Control register exists per timer (p.342) | 2 | Graded, PROP-CONTROL-REGISTER-EXISTS |
| A control bit selects the decrement clock (p.343, general sentence) | 2 | Graded, PROP-CLOCK-SELECT-GENERAL |
| Per-timer clock-source enumeration naming the CNT pin (p.343) | 4 | **Declined** — no `cnt` port exists anywhere in this design |
| Latch loaded on underflow/force-load/hi-byte-write-while-stopped (p.343) | 2 | Graded, PROP-LATCH-LOAD-STOPPED |
| Hi-byte write while running loads latch, not counter (p.343) | 2 | Graded, PROP-RUNNING-WRITE-COUNTER-GATE (confirms the pre-exposed reload-gate measurement) |
| One-shot hi-byte write starts counting regardless of start bit (p.342) | 2 | Graded, PROP-ONESHOT-THI-WRITE |
| Force load strobe, any time, running or stopped (p.342) | 2 | Graded, PROP-FORCE-LOAD |
| One-shot/continuous mode selection and behaviour (p.342) | 4 | Graded, PROP-RUN-MODE (confirms the pre-exposed self-clear measurement) |
| Timers can be linked for extended operations (p.341) | 2 | Graded, PROP-LINKING |
| Counter is 16-bit read-only, latch is 16-bit write-only, general shape (p.341) | 6 | **Declined**, `record()` — already graded by `hrm/ch1-cia-timers` |
| Latch also called prescalar, divides input clock (p.341) | 4 | **Declined**, `record()` — already graded by `hrm/ch1-cia-timers` |
| Start/stop control bit (p.342) | 2 | **Declined**, `record()` — already graded by `hrm/ch1-cia-timers` |

36+36+2+2+4+2+2+2+2+4+2+6+4+2 = 106. Every claim is accounted for below by
uid.

## Claims already graded elsewhere — 12 claims, one shared `record()` reason

All 12 carry an identical `classify.note`: `hrm/ch1-cia-timers`'s
`CIAA-TIMER-PROVIDED` / `CIAB-TIMER-PROVIDED` assertion (uids
`sol:1-guidelines.c46`, `opus:1-guidelines.c46`) already grades the
composite "load a count into the latch, start the timer, watch it count
down, see underflow raise `irq`" proposition at both `ciaa` and `ciab`
(`hrm/ch1-cia-timers/test_ciaa.cpp:267-278,296-319`). That composite already
exercises: a 16-bit counter exists and is read-only from the bus, a 16-bit
latch exists and is loaded by writes, the latch is also the prescalar value
counted down, and a control bit starts/stops the timer. This package's own
claims restate those same structural facts, more finely split, from the same
two manual sentences (p.341's counter/latch paragraph, p.341's prescalar
sentence, p.342's Start/Stop sentence) — still individually testable here,
but not re-graded, per the build brief's explicit instruction for exactly
this situation.

- `timer_counter_latch` (6): `opus:F-timers.c1`, `sol:F-timers.c1`,
  `opus:F-timers.c2`, `sol:F-timers.c2`, `sol:F-timers.c3`, `sol:F-timers.c4`
- `timer_prescalar` (4): `opus:F-timers.c3`, `opus:F-timers.c4`,
  `sol:F-timers.c5`, `sol:F-timers.c6`
- `timer_start_stop` (2): `sol:F-timers.c11`, `opus:F-timers.c9`

## PROP-BITS-\*-READ and PROP-BITS-\*-WRITE — the p.343 BIT NAMES tables

p.343 prints two tables, "BIT NAMES on READ-Register" and "BIT NAMES on
WRITE-Register", each four rows (registers 4–7: TALO, TAHI, TBLO, TBHI), each
row eight bit names (`TAL7..TAL0` etc. on the read side, `PAL7..PAL0` etc. on
the write side — the corpus's OCR prints the D1 column as `TALI`/`PALI`
where the manual means `TAL1`/`PAL1`; `corpus-defects.md` and this package's
own `opus:F-timers.c25` evidence both note the same OCR artefact, read as a
transcription defect, not as the manual naming bit 1 differently from bits
0–7 elsewhere). `sol` filed one claim per named bit (32 claims per table);
`opus` filed one claim per register naming the whole row (4 claims per
table). Both are the same 8 propositions per table read at two grain sizes,
so each register/direction is graded as one composed proposition citing all
9 uids that name it.

**Testable proposition, per register and direction:** the register's read
view (or write view) carries eight independently real, correctly-positioned
bits matching the printed name column — not a narrower register (a stuck
bit, a bit that does not exist, or a bit whose position does not match its
printed name).

| Prop | Register | Direction | `sol` uids (bits 7..0) | `opus` uid |
|---|---|---|---|---|
| PROP-BITS-TALO-READ | 4 (TALO) | read | `sol:F-timers.c22`..`c29` | `opus:F-timers.c25` |
| PROP-BITS-TAHI-READ | 5 (TAHI) | read | `sol:F-timers.c30`..`c37` | `opus:F-timers.c26` |
| PROP-BITS-TBLO-READ | 6 (TBLO) | read | `sol:F-timers.c38`..`c45` | `opus:F-timers.c27` |
| PROP-BITS-TBHI-READ | 7 (TBHI) | read | `sol:F-timers.c46`..`c53` | `opus:F-timers.c28` |
| PROP-BITS-TALO-WRITE | 4 (TALO) | write | `sol:F-timers.c54`..`c61` | `opus:F-timers.c29` |
| PROP-BITS-TAHI-WRITE | 5 (TAHI) | write | `sol:F-timers.c62`..`c69` | `opus:F-timers.c30` |
| PROP-BITS-TBLO-WRITE | 6 (TBLO) | write | `sol:F-timers.c70`..`c77` | `opus:F-timers.c31` |
| PROP-BITS-TBHI-WRITE | 7 (TBHI) | write | `sol:F-timers.c78`..`c85` | `opus:F-timers.c32` |

**Measurement basis, disclosed here because it is the same mechanism for
both directions and both grain sizes.** p.341 already establishes (and
`hrm/ch1-cia-timers` already grades) that reading a timer register returns
the counter and writing one reaches the latch, never the reverse — this
package does not re-litigate that. The write-view latch has no independent
read path of its own on this bus (confirmed by direct reading: the register
mux for registers 4–7 sources the counter, `tmr`, unconditionally on every
read — there is no register bit anywhere that exposes the raw latch
content). The only way to observe whether a written bit reached the latch
bit-for-bit is therefore the same path p.343 itself documents for moving the
latch into the counter (force load, or a hi-byte write while stopped, or
underflow) followed by a read. Both the READ and the WRITE proposition for a
given register are graded through that same round trip; what makes them
independent, gradable propositions rather than one, is which half of the
round trip each one's mutation targets — the WRITE proposition's mutation
defeats the write half (the intended pattern is sent to the wrong register,
so the latch never receives it); the READ proposition's mutation defeats the
read half (the verification read is redirected to the wrong register's
address, so a correctly-loaded counter is checked against the wrong
comparison). A passing WRITE proposition with a failing READ proposition (or
vice versa) is possible and meaningful under this design; both directions
are exercised per register with a walking-bit pattern (each of the 8 bit
positions set alone, then cleared alone against a background of the other
7), which is stronger than a single fixed byte because it cannot be
satisfied by a register that is stuck at a constant value that happens to
match one probe.

## PROP-CONTROL-REGISTER-EXISTS — p.342, "each timer has an associated control register"

`sol:F-timers.c10`, `opus:F-timers.c8`. **Testable proposition:** CRA and CRB
are two independently addressable, independently writable registers, one
"associated with" each timer — not a single shared register, and not a
register that silently ignores writes. Graded by writing a known pattern to
CRA, a *different* known pattern to CRB, and confirming each reads back its
own pattern and that writing CRB does not disturb what CRA already holds
(the "associated with each timer", i.e. not shared, half of the sentence).

## PROP-CLOCK-SELECT-GENERAL and the CNT decline — p.343, "Control bits allow selection of the clock used to decrement the timer"

`sol:F-timers.c17`, `opus:F-timers.c20` (the general sentence, naming no
specific source) are graded as **PROP-CLOCK-SELECT-GENERAL**: at least one
interval timer's decrement clock is demonstrably selectable by a control
bit, not fixed. Testable at this boundary via Timer B, the only one of the
two timers where a control bit performs any selection at all in this RTL
(confirmed by direct reading of both `cia_timera.v` and `cia_timerb.v` in
full).

`sol:F-timers.c18`, `sol:F-timers.c19`, `opus:F-timers.c21`,
`opus:F-timers.c22` each name a *specific* enumeration of sources per timer,
and every one of the four bundles the CNT pin into the same sentence as the
tested clock source ("Timer A can count φ2 clock pulses **or external
pulses applied to the CNT pin**"; Timer B's four-way list names CNT twice).
**Declined.** Checked directly: `ciaa`'s own port list, `cia_timera.v`'s
port list, and `cia_timerb.v`'s port list were all read in full above, and
none declares a `cnt` input anywhere — not "not wired at this boundary" (a
`minimig`-level fact one level up, the kind of boundary `hrm/ch1-cia-timers`
already declined some page-6 claims for) but structurally absent from the
whole design as pinned at this submodule commit. A claim that names CNT as
one of the sources under test cannot be graded in full without adding a
port, which the build brief forbids outright, and grading only the
non-CNT half of a single uid while silently ignoring the CNT half would
report the uid as ASSERTED for a narrower reason than what it actually
prints. Since a uid must be wholly asserted or wholly declined (never
split), these four are declined. This is also reported as a `note()`
alongside the decline, for visibility, since it is a real, checked RTL fact
and not merely a boundary-of-convenience call.

## PROP-LATCH-LOAD-STOPPED and PROP-RUNNING-WRITE-COUNTER-GATE — p.343, the reload-condition paragraph

p.343: *"The timer latch is loaded into the timer on any timer underflow, on
a force load, or following a write to the high byte of the pre-scalar while
the timer is stopped. If the timer is running, a write to the high byte
will load the timer latch but not the counter."*

`sol:F-timers.c20`, `opus:F-timers.c23` carry the first (three-condition)
sentence; `sol:F-timers.c21`, `opus:F-timers.c24` carry the second
(running-write) sentence. These are graded as two separate propositions,
because independent verification found they do not have the same verdict.

- **PROP-LATCH-LOAD-STOPPED** (`sol:F-timers.c20`, `opus:F-timers.c23`):
  the counter is reloaded from the latch on (a) underflow, (b) the force-load
  strobe, and (c) a hi-byte write while stopped. Confirmed true by direct
  reading — expected PASS, and this package's control case (see below).
- **PROP-RUNNING-WRITE-COUNTER-GATE** (`sol:F-timers.c21`,
  `opus:F-timers.c24`): while the timer is running, a hi-byte write must
  load the latch but leave the counter alone. **This is the pre-exposed
  reload-gate measurement, confirmed by direct reading of
  `cia_timera.v:76`** (`cia_timerb.v:74` carries the identical pattern for
  Timer B, an additional confirmation beyond what was pre-exposed):
  `thi_load <= thi & wr & (~start | oneshot);`. The `| oneshot` term means
  that in one-shot mode, a hi-byte write reloads `thi_load` — and therefore
  the counter, one cycle later, via `reload = thi_load | forceload |
  underflow` — **regardless of `start`**, i.e. even while the timer is
  running. In continuous mode (`oneshot=0`) the gate reduces to `~start`
  alone and the page's rule holds exactly. **Graded strictly as the page
  states it, with no mode carve-out the page itself does not make**: tested
  under one-shot running (the case the page's plain sentence covers and the
  RTL violates), so **this proposition is expected to read FAIL at
  baseline** — a confirmed, reported defect, not a test defect. Its
  mutation flips the same stimulus to continuous mode, which the RTL
  satisfies, turning the assertion GREEN — demonstrating the baseline FAIL
  is tied to the specific gap named above and is not an always-red or
  otherwise broken assertion.

## PROP-ONESHOT-THI-WRITE — p.342, "a write to timer-high... will transfer the timer latch to the counter and initiate counting regardless of the start bit"

`sol:F-timers.c15`, `opus:F-timers.c18`. Register 5 for Timer A, register 7
for Timer B (confirmed against `ciaa.v`'s own decoder: `rs==4'h5` feeds
Timer A's `thi` port, `rs==4'h7` feeds Timer B's — matching the printed
register numbers exactly). Distinct from PROP-RUNNING-WRITE-COUNTER-GATE:
this is the **stopped**-timer case, and it additionally requires the start
bit itself to change ("regardless of the start bit" reads most naturally as
this write can start a timer that was never told to start) — a second
effect beyond the reload PROP-LATCH-LOAD-STOPPED already grades. Confirmed
true by direct reading of `cia_timera.v:41-42`/`cia_timerb.v:40-41` (`thi_load
&& oneshot` sets the start bit). Expected PASS.

## PROP-FORCE-LOAD — p.342, "a strobe bit allows the timer latch to be loaded into the timer counter at any time, whether the timer is running or not"

`sol:F-timers.c16`, `opus:F-timers.c19`. Graded at both a stopped timer and a
running one (continuous mode, so the running case does not depend on the
one-shot gap graded above) — both must show an immediate reload on the
strobe. Confirmed true by direct reading. Expected PASS. Also reported via
`note()`: independently re-confirmed both pre-exposed CRA readback facts
(bit 4, the strobe, is never stored — `tmcr[4]` is forced to `1'b0` on every
control-register write — so it always reads back 0 regardless of what was
written; and `CRA` written `0xFF` reads back `0x6F`, since bit 7 is not part
of `tmcr` at all and bit 4 is the forced strobe bit). Neither fact is itself
one of this package's 106 claims (no uid names a specific CRA bit's
readback value), so neither becomes a graded assertion — they are measured
and reported, not adjudicated, per the build brief.

## PROP-RUN-MODE — p.342, One-shot/continuous

`sol:F-timers.c14` (the whole paragraph in one quote), `opus:F-timers.c15`
(the selector sentence alone), `opus:F-timers.c16` (one-shot alone),
`opus:F-timers.c17` (continuous alone) — four claims on the same paragraph,
graded as one composed proposition: (a) a control bit selects between the
two modes; (b) one-shot: count to zero, interrupt (underflow), reload, then
**stop**; (c) continuous: count to zero, interrupt, reload, and **repeat**.
**This confirms the pre-exposed measurement** ("one-shot START self-clears
at the underflow on both timers and continuous does not"): confirmed by
direct reading, `cia_timera.v:43-44`/`cia_timerb.v:42-43` clear the start
bit (`tmcr[0] <= 1'd0`) only `if (underflow && oneshot)` — continuous mode
never takes that branch, so `start` stays set and the next reload
(underflow-triggered, since `reload` includes `underflow`) is followed by
further counting rather than a stop. Expected PASS.

## PROP-LINKING — p.341, "The timers can be used independently or linked for extended operations"

`opus:F-timers.c5`, `sol:F-timers.c7`. Timer B's count-source mux
(`cia_timerb.v:31`, `count = tmcr[6] ? tmra_ovf : eclk`) is the linking
mechanism: with `tmcr[6]` (CRB bit 6) set, Timer B decrements only on Timer
A's underflow pulses rather than on every qualifying clock edge, which is
exactly "extended operations" — a Timer-B count that would otherwise span
at most 65536 clock edges can span up to 65536 Timer-A periods instead.
Graded by comparing Timer B's decrement rate under the two source settings,
with Timer A left free-running underneath; the "linked" setting must show
measurably fewer Timer B decrements per fixed tick budget than the
"unlinked" setting, and the linked decrements must track Timer A's own
underflow cadence (not merely differ from the eclk-sourced rate for some
other reason). Expected PASS.

## Fixture-liveness proof and control case

`hrm/ch1-cia-timers` already built and validated a `ciaa` Harness/Checks
pair against this exact pinned RTL commit; this package's fixture reuses
that bus-timing protocol verbatim (posedge-clk, `clk7_en`-gated writes need
one tick to latch; `data_out` is combinational and needs only `eval()`) —
confirmed still accurate by this package's own direct reading. Fixture
liveness: two distinct, known bytes written to `DDRA` (a register no claim
in this package depends on — port A is out of this package's subject) must
both arrive and read back correctly before any interval-timer assertion is
graded, mirroring `hrm/ch1-cia-timers`'s own `FIXTURE-LIVE-CIAA`.

**Control case, on the same measurement basis as anything under suspicion.**
PROP-LATCH-LOAD-STOPPED and PROP-FORCE-LOAD are both expected PASS, read
through the identical write→reload→read-counter mechanism
PROP-RUNNING-WRITE-COUNTER-GATE uses to reach its FAIL — a passing result on
that shared mechanism, alongside a failing one, is what makes the FAIL
mean something rather than "the harness cannot read a counter at all."

## On mutation

Every graded assertion (fixture-liveness included) carries a stimulus
mutation recorded in the C++ source in the fixed `// MUTATION <label>
<oracle|stimulus|none> <red|green|impossible> <what changed>` form, replayable
as `make MUT=<NAME> test` with a full `rm -rf obj_dir` rebuild. No mutation
touches an oracle/expected value or any file under `nanomig/`. Concrete
stimulus values (exact byte patterns, tick counts, load values) are step-2
material, fixed once the RTL reading above is complete — recorded in
`test_ciaa.cpp`'s own header, not duplicated here.

## What this specification deliberately narrows

No claim here re-grades anything `hrm/ch1-cia-timers` already grades (the 12
`record()` claims above), and no claim here re-grades `appF-cia-timer-modes`
territory: the CRB5/CRB6 INMODE **bit-field table** on p.349 (a different
page, a different package, `cia_timera` as its declared top) is not this
package's subject — this package's own input-mode claims are p.343's prose
enumeration only, and the CNT-pin absence they surface is reported here
independently, on this package's own pages, not borrowed from the "five
things already measured" list in the build brief (that list's own CRB5 item
concerns p.349, out of this package's claim set entirely — checked: every
one of this package's 106 claims cites page 341, 342, or 343 only).
