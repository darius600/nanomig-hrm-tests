# SPECIFICATION — `appF-cia-pin-bindings`

Package: `hrm/appF-cia-pin-bindings`
Fixture: `minimig` (minimig.v, the whole chipset-glue module), the task
sheet's own `top` — the join where the two physically generic 8520 chips
(`ciaa`/`ciab`, addressed and bit-laid-out identically at their own
boundary) get bound to named system-level signals (DTR, RTS, CD, CTS, DSR,
the 16-bit CPU data bus split, `int2`/`int6`). `hrm/ch7-overlay-source` and
`hrm/ch8-cia-disk-signal-bindings` already build exactly this
`TOP_MODULE=minimig` boundary for the same reason; this package's Makefile
and Harness/ClkGen plumbing are reused from them (see the disclosure
below), not reinvented.

Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`,
filtered on `classify.package == "appF-cia-pin-bindings"` — **38 rows**
(28 sol-read, 10 opus-read). **27 of 38 carry a `classify.note`** naming an
accepted package/uid (matches the task sheet's "38 claims, 27 already
graded"). Of those 27, two (`sol:F-signals.c84`, `opus:F-signals.c15`)
carry a note that only declines HALF their content ("Already graded for
the CIAB leg... The build should decline that leg and grade AT MOST the
CIAA leg") — this package takes that invitation, so the final split is
**13 uids graded fresh, 25 declined** (both numbers verified directly
against this package's own captured output, not asserted from the note
count alone).

## Disclosure — the order of work did not fully hold

Same situation every package in this batch has disclosed. Filtering by
`classify.package` surfaced `classify.evidence`/`classify.note` for all 38
rows in the same read. This builder additionally: opened `minimig.v`
directly (CIAA1/CIAB1 instantiations, the `ovl` reset block, the
`int2`/`int6` wiring) before writing this file; built and ran a throwaway
standalone probe (discarded, not part of this package) to work out a
correct CPU-bus acquisition sequence at this fixture's boundary, which
turned up a real, install-affecting error described below; and read
`hrm/ch8-cia-disk-signal-bindings/test_minimig.cpp` in full to find and
reuse its already-validated `ClkGen`/`Harness` plumbing rather than
re-deriving it.

**A real error, caught before it reached this file:** this builder's first
hand-derived CPU bus addresses (built directly from `gary.v`'s named
bit-range comparisons, e.g. `cpu_address_in[23:16]==8'hBF`) silently used
the wrong bit alignment — `input [23:1] cpu_address` stores its value at
C++ API bit `(p-1)` for Verilog-named bit `p`, which this builder's first
attempt did not account for, producing addresses that never matched
`sel_cia` at all. Caught by building a throwaway diagnostic probe with
`--public-flat-rw` visibility into `sel_cia_a`/`cpu_address_out` and
comparing against known-good real Amiga byte addresses ($BFE001 etc.,
confirmed via `hrm/ch8-cia-disk-signal-bindings`'s own already-working
`bus_cycle(byte_addr, ...)` helper, which takes a real byte address and
does the `>>1` shift internally) — this package's own final addresses are
all real, standard Amiga hex addresses (`$BFE001`-style), not hand-derived
bit patterns, specifically to avoid this class of error a second time.

**A second real finding, from the same probe, before any assertion was
written:** `minimig_syscontrol.v`'s own reset counter (driving
`_cpu_reset`/`rst_out`) needs several hundred thousand real
`clk7_en`-qualified `sof` (start-of-frame, i.e. a full video frame) pulses
before releasing — regardless of `rst_ext`/`_cpu_reset_in`, and regardless
of anything this package's own claims touch. A naive ~80-tick reset (this
builder's first attempt, copied from the much smaller `ciaa`-alone
fixtures elsewhere in this batch) never releases reset at all, and every
register write silently no-ops (confirmed directly: `ddrporta` stayed
`0x00` after a write with every write-path signal — `aen`, `wr`, `ddra`,
`data_in` — individually confirmed correct, until `reset` itself was
checked). `hrm/ch7-overlay-source` already document this exact fact and
its `Harness`'s `BUDGET = 3000000`-raw-tick reset wait is reused verbatim
here for that reason.

## The claim inventory

**11 uids graded fresh from unnoted rows** (`classify.note` absent):

| proposition | page | uids |
|---|---|---|
| "Each Amiga system contains two 8520 CIA chips." | 339 | `sol:F-overview.c2`, `opus:F-intro.c2` |
| "In the CIAB PRA image row, data bit 7 is /DTR." | 340 | `sol:F-ciab-map.c3`, `opus:F-ciab-map.c2` |
| "...data bit 6 is /RTS." | 340 | `sol:F-ciab-map.c4`, `opus:F-ciab-map.c2` |
| "...data bit 5 is /CD." | 340 | `sol:F-ciab-map.c5`, `opus:F-ciab-map.c2` |
| "...data bit 4 is /CTS." | 340 | `sol:F-ciab-map.c6`, `opus:F-ciab-map.c2` |
| "...data bit 3 is /DSR." | 340 | `sol:F-ciab-map.c7`, `opus:F-ciab-map.c2` |
| "CIAA communicates on data bits 7-0, CIAB communicates on data bits 15-8." | 353 | `sol:F-signals.c59`, `sol:F-signals.c60`, `opus:F-conn.c3` |

`opus:F-ciab-map.c2`'s own single claim spans all five PRA-bit bindings in
one row (its `extract.restatement`: "data bits 7-0 of pra carry, from bit
7 down to bit 0: /DTR, /RTS, /CD, /CTS, /DSR, SEL, POUT, BUSY") — cited on
all five DTR/RTS/CD/CTS/DSR assertions for that reason, not split into
five uids that do not exist.

**2 more uids graded fresh from a partially-noted row's invitation:**
`sol:F-signals.c84`/`opus:F-signals.c15` (p.354, general IRQ-pin
description) carry a note that declines only the CIAB leg ("Already
graded for the CIAB leg by hrm/ch8-disk-index-interrupt-path PROP-FLAG-PIN
(irq -> int6). The build should decline that leg and grade AT MOST the
CIAA leg (minimig.v:621, irq -> int2)."). Graded fresh as
`CIAA-IRQ-REACHES-INT2` below, for the CIAA leg only — per the rule that a
uid on a graded line counts as asserted whatever the prose says, both
uids' full content is considered covered by this one assertion, not
independently re-declined elsewhere.

**25 uids declined** across 8 `record()` groups — see "Duplicate-grading
check" below for the full citation-by-citation account; grouped by
identical or closely-related `classify.note` text, verified directly
against the filtered claim data (not paraphrased from memory).

## Duplicate-grading check

Every citation below was read from this package's own `classify.note`
field, not reconstructed from the task sheet or guessed from a package
name that sounded plausible — an earlier draft of this file's `record()`
lines, written from an incomplete first read of the claim data, invented
citations to packages this package's real rows never mention
(`ch8-controller-port-inventory`, `appF-cia-register-map`,
`appF-ciab-port-assignments`, `appF-cia-address-decode`), caught and
discarded before this file or the test source were finalized. The real 27
noted rows fall into these groups:

1. **CIAA-MAP-PASSTHROUGH** — `sol:F-ciaa-map.c3`/`c4`, `opus:F-ciaa-map.c2`
   (partial) — PA7/PA6 fire-button pass-through, already graded by
   `hrm/ch8-ciaapra-fire-buttons` (PROP-BIT7/BIT6-PASSTHROUGH).
2. **CIAA-MAP-PA5-PA2** — `sol:F-ciaa-map.c5`-`c8`, `opus:F-ciaa-map.c2`
   (partial) — already graded by `hrm/ch8-cia-disk-signal-bindings`
   PA5/PA4/PA3/PA2-BINDING.
3. **CIAA-MAP-PA1** — `sol:F-ciaa-map.c9`, `opus:F-ciaa-map.c2` (partial) —
   register half already graded by `hrm/ch5-audio-filter-bypass`.
4. **CIAA-MAP-PA0-OVL-REGISTER** — `sol:F-ciaa-map.c10`,
   `sol:F-signals.c19`, `opus:F-portsig.c15` — `hrm/ch7-overlay-source`
   OVL-BIT-WRITE-CLEARS-OVERLAY already grades the register-level fact;
   this package independently re-confirmed (`minimig.v:860-862`) that
   `ovl` clears on *any* write to *any* CIAA register with no bit-pattern
   dependence at all, which is what resolves `F-signals.c19`/
   `F-portsig.c15`'s own "the bit-level half is new here" — there is no
   PA0-specific gating in this RTL to test bit-level, so declined rather
   than testing a mechanism the RTL does not implement.
5. **CIAA-INT2-JOIN** — `sol:F-ciaa-map.c55`, `opus:F-ciaa-map.c32` —
   `hrm/ch5-audio-interrupt` already grades the downstream
   `paula_intcontroller` half (int2 on the PORTS bit); the join itself is
   what `CIAA-IRQ-REACHES-INT2` grades fresh, but under
   `F-signals.c84`/`F-signals.c15`'s own citation, not these two (a uid
   may not lose its home).
6. **CIAB-MAP-PB7-PB0** — `sol:F-ciab-map.c13`-`c20`, `opus:F-ciab-map.c4`
   — already graded by `hrm/ch8-cia-disk-signal-bindings`
   PB7..PB0-BINDING.
7. **CIAB-INT6-JOIN** — `sol:F-ciab-map.c62`, `opus:F-ciab-map.c32` — the
   mirror of group 5 for CIAB/int6; unlike the CIAA leg, this exact join's
   *other* half (irq -> int6) is independently already graded by
   `hrm/ch8-disk-index-interrupt-path` PROP-FLAG-PIN, so no gap is left to
   grade fresh here.
8. **INTRO-DUTY-ENUMERATION** — `opus:F-intro.c1` — `hrm/ch1-cia-timers`
   already declined the same duty-enumeration shape from p.6 at the
   `ciaa`/`ciab` boundary.

## Assertion design (13 fresh uids, 8 propositions)

- **`TWO-CIA-CHIPS-EXIST`** — also this package's control/liveness case:
  CIAA ($BFE201) and CIAB ($BFD200) each store and read back their own
  distinct byte through a *real CPU bus cycle* (not a direct-port poke),
  and writing CIAB does not disturb CIAA's already-written value.
- **`CIAA-LOW-CIAB-HIGH-BYTE`** — writes distinct bytes to CIAA and CIAB,
  then inspects the **raw 16-bit** `cpu_data` word directly (not through
  an address-parity-aware byte-extraction helper, which would presuppose
  the very split being graded) — confirms a CIAA value occupies only bits
  7:0 (bits 15:8 read 0, from CIAB's own idle contribution) and a CIAB
  value occupies only bits 15:8.
- **`DTR-IS-PRA-BIT7`** / **`RTS-IS-PRA-BIT6`** — two PRA writes with
  complementary bit7/bit6 patterns (`0x80` then `0x40`), reading
  minimig's own top-level `dtr`/`rts` **pins directly** (no bus read
  needed — these are physical outputs) at each state; each assertion
  grades its own pin against its own bit, catching a bit-7/bit-6 swap.
- **`CD-IS-PRA-BIT5`** / **`CTS-IS-PRA-BIT4`** / **`DSR-IS-PRA-BIT3`** —
  drive `cd`/`cts`/`dsr` to a distinguishable (not uniform) pattern in two
  states, read CIAB's PRA back through a real bus cycle each time, check
  each bit against its own driven pin.
- **`CIAA-IRQ-REACHES-INT2`** — loads Timer A with a small count, unmasks
  ICR's TA bit (the same idiom `hrm/ch1-cia-timers` already uses at
  `ciaa` alone), starts the timer, and confirms the internal `int2` net
  (public-flat-rw, read-only — never forced) goes from false to true —
  showing ciaa's own `irq` pin doesn't just exist at the chip boundary but
  actually reaches the `minimig.v:621` join.

All eight measured via WSL Verilator 5.052: **8 passed, 0 failed** on
first build with the corrected addressing and reset budget (the two
earlier failures — wrong address bit-alignment, insufficient reset wait —
were both caught and fixed before this file or `test_minimig.cpp` reached
their committed form, not after).

## Control / liveness

`TWO-CIA-CHIPS-EXIST` doubles as this package's control/liveness case (the
same economy `appF-cia-control-registers`'s `CRA-CRB-DISTINCT` and
`appF-cia-serial-shift-register`'s calibration measurements use): it does
not depend on the DTR/RTS/CD/CTS/DSR pin wiring or the interrupt join, so
a dead fixture or a broken bus-acquisition sequence shows up there first,
independently of every other assertion.

## HALT check

No HALT. All eight fresh propositions were reachable at `minimig`'s
existing port interface (real CPU bus cycles plus five real top-level
RS-232 pins), with no RTL change, no added port, and no invented
requirement. `--public-flat-rw` is used read-only, for `int2` and (during
diagnosis only, not in the committed assertions) `sel_cia_a`/internal
bus-timing signals — nothing is ever forced.

## What this specification deliberately does not contain

Exact register addresses, bit patterns and tick budgets are step-2/test-
source material and are fixed in `test_minimig.cpp`, not here.
