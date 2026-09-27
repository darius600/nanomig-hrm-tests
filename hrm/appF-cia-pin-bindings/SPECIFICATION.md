# SPECIFICATION — `appF-cia-pin-bindings`

Package: `hrm/appF-cia-pin-bindings`
Fixture: `minimig` (minimig.v, the whole chipset-glue module) — the join
where the two physically generic 8520 chips
(`ciaa`/`ciab`, addressed and bit-laid-out identically at their own
boundary) get bound to named system-level signals (DTR, RTS, CD, CTS, DSR,
the 16-bit CPU data bus split, `int2`/`int6`). `hrm/ch7-overlay-source` and
`hrm/ch8-cia-disk-signal-bindings` build exactly this
`TOP_MODULE=minimig` boundary for the same reason; this package's Makefile
and Harness/ClkGen plumbing are shared with them (see "Harness notes"
below).

Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`,
filtered on `classify.package == "appF-cia-pin-bindings"` — **38 rows**
(28 sol-read, 10 opus-read). **27 of 38 carry a `classify.note`** naming an
accepted package/uid. Of those 27, two (`sol:F-signals.c84`,
`opus:F-signals.c15`) carry a note that only declines HALF their content
(the CIAB leg is already graded) and leaves the CIAA leg to this package,
which grades it. Two more noted rows,
`sol:F-ciaa-map.c55`/`opus:F-ciaa-map.c32` ("CIAA can generate interrupt
INT2") and `sol:F-ciab-map.c62`/`opus:F-ciab-map.c32` ("CIAB can generate
interrupt INT6"), are established whole by this package's own
CIAA-IRQ-REACHES-INT2/-PORTS and CIAB-IRQ-REACHES-EXTER assertions, so
those four are graded rather than declined. The final split is **17 uids
graded fresh, 21 declined** (both numbers verified directly against this
package's own captured output, not asserted from the note count alone).

## Harness notes

**Addresses are real Amiga byte addresses.** `input [23:1] cpu_address`
stores its value at C++ API bit `(p-1)` for Verilog-named bit `p`, so an
address hand-derived from `gary.v`'s named bit-range comparisons (e.g.
`cpu_address_in[23:16]==8'hBF`) lands with the wrong bit alignment and
never matches `sel_cia` at all. Every address in this package is a real,
standard Amiga hex address (`$BFE001`-style) passed to
`bus_cycle(byte_addr, ...)`, which does the `>>1` shift internally, as
`hrm/ch8-cia-disk-signal-bindings`'s own helper does.

**Reset needs a long budget.** `minimig_syscontrol.v`'s own reset counter
(driving `_cpu_reset`/`rst_out`) needs several hundred thousand real
`clk7_en`-qualified `sof` (start-of-frame, i.e. a full video frame) pulses
before releasing — regardless of `rst_ext`/`_cpu_reset_in`, and regardless
of anything this package's own claims touch. A short (~80-tick) reset, as
the much smaller `ciaa`-alone fixtures use, never releases reset at all,
and every register write silently no-ops (`ddrporta` stays `0x00` after a
write with every write-path signal — `aen`, `wr`, `ddra`, `data_in` —
correct). `hrm/ch7-overlay-source` documents this exact fact, and its
`Harness`'s `BUDGET = 3000000`-raw-tick reset wait is used here for that
reason.

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

**2 more uids graded fresh from a partially-noted row:**
`sol:F-signals.c84`/`opus:F-signals.c15` (p.354, general IRQ-pin
description) carry a note that declines only the CIAB leg, already graded
by hrm/ch8-disk-index-interrupt-path PROP-FLAG-PIN (irq -> int6), and
leaves at most the CIAA leg (minimig.v:621, irq -> int2). Graded fresh as
`CIAA-IRQ-REACHES-INT2` below, for the CIAA leg only — per the rule that a
uid on a graded line counts as asserted whatever the prose says, both
uids' full content is considered covered by this one assertion, not
independently re-declined elsewhere.

**4 more uids graded fresh, cited on assertions built for a different
pair of ids:** `sol:F-ciaa-map.c55`/`opus:F-ciaa-map.c32` ("CIAA can
generate interrupt INT2") are established whole by `CIAA-IRQ-REACHES-INT2`
and `CIAA-IRQ-REACHES-PORTS` (ciaa's own `irq` pin actually reaching,
respectively, `int2` and INTREQ bit 3). `sol:F-ciab-map.c62`/
`opus:F-ciab-map.c32` ("CIAB can generate interrupt INT6") are established
whole by `CIAB-IRQ-REACHES-EXTER` (ciab's own `irq` pin reaching INTREQ
bit 13, which p.219 "Bit 13, EXTER, becomes a 1 when ... INT6* ..." ties
to CIAB generating INT6). All four uids are cited directly on those
assertions' own labels, not carried by proximity.

**21 uids declined** across 6 `record()` groups — see "Duplicate-grading
check" below for the full citation-by-citation account; grouped by
identical or closely-related `classify.note` text, verified directly
against the filtered claim data.

## Duplicate-grading check

Every citation below is read from this package's own `classify.note`
field. The 21 declined rows fall into these groups (of the 27 noted rows, 4 —
`sol:F-ciaa-map.c55`/`opus:F-ciaa-map.c32`, `sol:F-ciab-map.c62`/
`opus:F-ciab-map.c32` — are graded instead; see above):

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
5. **CIAB-MAP-PB7-PB0** — `sol:F-ciab-map.c13`-`c20`, `opus:F-ciab-map.c4`
   — already graded by `hrm/ch8-cia-disk-signal-bindings`
   PB7..PB0-BINDING.
6. **INTRO-DUTY-ENUMERATION** — `opus:F-intro.c1` — `hrm/ch1-cia-timers`
   already declined the same duty-enumeration shape from p.6 at the
   `ciaa`/`ciab` boundary.

## Assertion design (17 fresh uids, 10 propositions)

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
  actually reaches the `minimig.v:621` join. Also establishes, whole,
  "CIAA can generate interrupt INT2" (`sol:F-ciaa-map.c55`,
  `opus:F-ciaa-map.c32`).
- **`CIAA-IRQ-REACHES-PORTS`** — same Timer A/ICR/CRA idiom, read one
  register further downstream at INTREQ bit 3
  (`paula_intcontroller.v:112`); the CIA leg only of Figure 7-4's PORTS
  row (p.221) — the expansion-connector leg is absent from this RTL. Also
  establishes, whole, "CIAA can generate interrupt INT2" (same two ids as
  above); the bit-3 = PORTS identification is cited to p.219/p.280, not to
  Figure 7-4 or the RTL line.
- **`CIAB-IRQ-REACHES-EXTER`** — the CIAB mirror of
  `CIAA-IRQ-REACHES-PORTS`, at INTREQ bit 13 (`paula_intcontroller.v:132`)
  for Figure 7-4's EXTER row. Establishes, whole, "CIAB can generate
  interrupt INT6" (`sol:F-ciab-map.c62`, `opus:F-ciab-map.c32`); the
  bit-13 = EXTER identification is likewise cited to p.219/p.280.

Measured via WSL Verilator 5.052: the eight base propositions and the two
Extension assertions below give **10 passed, 0 failed** across this
package's ten propositions.

## Control / liveness

`TWO-CIA-CHIPS-EXIST` doubles as this package's control/liveness case (the
same economy `appF-cia-control-registers`'s `CRA-CRB-DISTINCT` and
`appF-cia-serial-shift-register`'s calibration measurements use): it does
not depend on the DTR/RTS/CD/CTS/DSR pin wiring or the interrupt join, so
a dead fixture or a broken bus-acquisition sequence shows up there first,
independently of every other assertion.

## Reachability

All eight base propositions are reachable at `minimig`'s existing port
interface (real CPU bus cycles plus five real top-level RS-232 pins), with
no RTL change, no added port, and no invented requirement.
`--public-flat-rw` is used read-only, for `int2` and the INTREQ register —
nothing is ever forced.

## Extension — interrupt-source naming (Figure 7-4, p.221)

Figure 7-4's "Description" cells name each interrupt's raising condition as
TWO sources joined by "&": `opus:7-fig7-4.c12`'s own restatement is "the
name PORTS denotes the interrupt raised by the condition 'external INT2 &
CIAA'"; `opus:7-fig7-4.c42`'s is the EXTER mirror for "external INT6 &
CIAB". One of the two named sources on each row is an expansion-connector
pin absent from this RTL entirely; the other is a CIA's own irq output.
Only the CIA leg of each row is measured here (`CIAA-IRQ-REACHES-PORTS`,
`CIAB-IRQ-REACHES-EXTER`), so neither id is carried on those assertions —
a passing label may not carry an id for a clause it does not establish,
even with a caveat next to it. Both are instead declined on their own
record() lines (`PORTS-DENOTATION-TWO-SOURCES`,
`EXTER-DENOTATION-TWO-SOURCES`); see "The claim inventory" and "Assertion
design" above for the single-source claims (`sol:F-ciaa-map.c55`/
`opus:F-ciaa-map.c32`, `sol:F-ciab-map.c62`/`opus:F-ciab-map.c32`) those
two assertions establish whole instead. The bit index each assertion
checks (INTREQ bit 3 for PORTS, bit 13 for EXTER) is cited to p.219/p.280,
not to Figure 7-4 (which prints no bit numbers) or to the RTL line alone.

## What this specification deliberately does not contain

Exact register addresses, bit patterns and tick budgets are test-source
material and are fixed in `test_minimig.cpp`, not here.
